#if 1
//#include "user_define.h"
#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/spi.h"
#include "ff.h"			/* Obtains integer types */
#include "diskio.h"		/* Declarations of disk functions */

#define SPI_CH spi0
#define P_SPI_SCK 2
#define P_SPI_TX 3
#define P_SPI_RX 4
#define P_SPI_CS 5

/* Definitions for MMC/SDC command */
#define CMD0    (0x40+0)    /* GO_IDLE_STATE */
#define CMD1    (0x40+1)    /* SEND_OP_COND (MMC) */
#define CMD16    (0x40+16)    /* SET_BLOCKLEN */
#define CMD17    (0x40+17)    /* READ_SINGLE_BLOCK */

#define CS_HIGH() {gpio_put(P_SPI_CS, 1);}
#define CS_LOW() {gpio_put(P_SPI_CS, 0);}

static BYTE rcv_spi( void );        /* Send a 0xFF to the SDC/MMC and get the received byte */
static void xmit_spi( BYTE d );    /* Send a byte to the SDC/MMC */
static BYTE send_cmd( BYTE cmd, DWORD arg );

/*-----------------------------------------------------------------------*/
/* Initialize Disk Drive                                                 */
/*-----------------------------------------------------------------------*/

DSTATUS disk_initialize( BYTE drv)
{
    DSTATUS stat = STA_NOINIT;
    WORD i;

    /* マイコンリソースの初期設定 */
    {
        spi_init( SPI_CH, 100 * 1000 );
        spi_set_format( SPI_CH, 8, SPI_CPOL_0, SPI_CPHA_0, SPI_MSB_FIRST );
        gpio_set_function( P_SPI_RX, GPIO_FUNC_SPI );
        gpio_set_function( P_SPI_SCK, GPIO_FUNC_SPI );
        gpio_set_function( P_SPI_TX, GPIO_FUNC_SPI );
		gpio_pull_up(P_SPI_TX);
        gpio_init( P_SPI_CS );
        gpio_set_dir( P_SPI_CS, GPIO_OUT );
    }

    CS_HIGH();
	gpio_set_function(P_SPI_RX, GPIO_FUNC_SIO);
	gpio_set_dir(P_SPI_RX, GPIO_OUT);
	gpio_put(P_SPI_RX, 1);
    /* ダミークロックを80発 */
    {
        BYTE i;
        for ( i= 0; i < 10; i++ )
        {
            rcv_spi();
        }
    }
	gpio_put(P_SPI_RX, 0);
	gpio_set_function( P_SPI_RX, GPIO_FUNC_SPI );

	spi_init( SPI_CH, 10 * 1000 * 1000 );
    {
        BYTE cmd_res;

		/* CMD0を0x01が返ってくるまでトライ */
        for ( i = 0; i < 10000; i++ )
        {
            cmd_res = send_cmd( CMD0, 0 );
            printf("A:%02x\n", cmd_res);
			if ( cmd_res == 0x01 )
            {
                break;
            }
        }

        if ( cmd_res == 0x01 )/* Enter Idle state */
        {
            /* CMD1を0x00が返ってくるまでトライ */
            for ( i = 0; i < 10000; i++ )
            {
                cmd_res = send_cmd( CMD1, 0 );
				printf("B:%02x\n", cmd_res);
                if ( cmd_res == 0x00 )
                {
                    /* 初期化完了 */
                    stat = 0;
                    break;
                }
                sleep_us( 100 );
            }

            /* CMD16でブロックサイズを512バイトに設定 */
            send_cmd( CMD16, 512 );
        }
    }

    CS_HIGH();
    rcv_spi();

    return ( stat );
}



/*-----------------------------------------------------------------------*/
/* Read Partial Sector                                                   */
/*-----------------------------------------------------------------------*/

DRESULT disk_readp (
    BYTE* buff,        /* Pointer to the destination object */
    DWORD sector,    /* Sector number (LBA) */
    UINT offset,    /* Offset in the sector */
    UINT count        /* Byte count (bit15:destination) */
)
{
    DRESULT res;
    BYTE cmd_res;

    res = RES_ERROR;

    cmd_res = send_cmd( CMD17, ( sector * 512 ) );
    if ( cmd_res == 0x00 )
    {
        WORD i;
        /* なんらかの応答が来るまでループ */
        for ( i = 0; i < 40000; i++ )
        {
            cmd_res = rcv_spi();
            if ( cmd_res != 0xFF )
            {
                break;
            }
        }

        /* 応答がデータパケットの先頭0xFE時の処理 */
        if ( cmd_res == 0xFE )
        {
            /* offsetまで読み飛ばす */
            for ( i = 0; i < offset; i++ )
            {
                rcv_spi();
            }

            /* データを読み込む */
            if ( buff != 0 )
            {
                for ( i = 0; i < count; i++ )
                {
                    buff[ i ] = rcv_spi();
                }
            }
            else
            {
                for ( i = 0; i < count; i++ )
                {
                    rcv_spi();
                }
            }

            /* 残りは読み飛ばす */
            for ( i = 0; i < ( 514 - ( offset + count ) ); i++ )
            {
                rcv_spi();
            }

            res = RES_OK;
        }
    }

    CS_HIGH();
    rcv_spi();

    return ( res );
}



/*-----------------------------------------------------------------------*/
/* Write Partial Sector                                                  */
/*-----------------------------------------------------------------------*/

DRESULT disk_writep (
    const BYTE* buff,        /* Pointer to the data to be written, NULL:Initiate/Finalize write operation */
    DWORD sc        /* Sector number (LBA) or Number of bytes to send */
)
{
    DRESULT res;


    if (!buff) {
        if (sc) {

            // Initiate write process

        } else {

            // Finalize write process

        }
    } else {

        // Send data to the disk

    }

    return res;
}


static void xmit_spi( BYTE d )    /* Send a byte to the SDC/MMC */
{
    BYTE src[ 1 ];

    src[ 0 ] = d;
    spi_write_blocking( SPI_CH, src, 1 );
}

static BYTE rcv_spi( void )        /* Send a 0xFF to the SDC/MMC and get the received byte */
{
    BYTE dst[ 1 ];

    spi_read_blocking( SPI_CH, 0xFF, dst, 1 );

    return ( dst[ 0 ] );
}


static BYTE send_cmd(
    BYTE cmd,        /* 1st byte (Start + Index) */
    DWORD arg        /* Argument (32 bits) */
)
{
    BYTE n, res;

    /* Select the card */
    CS_HIGH();
    rcv_spi();
    CS_LOW();
    rcv_spi();

    /* Send a command packet */
    xmit_spi( cmd );                        /* Start + Command index */
    xmit_spi( (BYTE)( arg >> 24 ) );        /* Argument[31..24] */
    xmit_spi( (BYTE)( arg >> 16 ) );        /* Argument[23..16] */
    xmit_spi( (BYTE)( arg >>  8 ) );            /* Argument[15..8] */
    xmit_spi( (BYTE)arg );                /* Argument[7..0] */
    n = 0x01;                            /* Dummy CRC + Stop */
    if ( cmd == CMD0 )
    {
        n = 0x95;            /* Valid CRC for CMD0(0) */
    }
    xmit_spi( n );

    /* Receive a command response */
    n = 10;                                /* Wait for a valid response in timeout of 10 attempts */
    for ( n = 0; n < 10; n++ )
    {
        res = rcv_spi();
        if ( ( res & 0x80 ) == 0x00 )
        {
            break;
        }
    }

    return ( res );            /* Return with the response value */
}
#endif

#if 0

/*------------------------------------------------------------------------*/
/* STM32F100: MMCv3/SDv1/SDv2 (SPI mode) control module                   */
/*------------------------------------------------------------------------*/
/*
/  Copyright (C) 2018, ChaN, all right reserved.
/
/ * This software is a free software and there is NO WARRANTY.
/ * No restriction on use. You can use, modify and redistribute it for
/   personal, non-profit or commercial products UNDER YOUR RESPONSIBILITY.
/ * Redistributions of source code must retain the above copyright notice.
/
/-------------------------------------------------------------------------*/
#include "pico/stdlib.h"
#include "hardware/spi.h"

/*
   GPIO 16 (pin 21) MISO/spi0_rx-> SDO
   GPIO 17 (pin 22) Chip select -> !CS
   GPIO 18 (pin 24) SCK/spi0_sclk -> SCK
   GPIO 19 (pin 25) MOSI/spi0_tx -> SDI
*/
#define DEF_SPI_SCK_PIN	2
#define DEF_SPI_TX_PIN	3
#define DEF_SPI_RX_PIN	4
#define DEF_SPI_CSN_PIN	5

#define SPIDEV		spi0

#define FCLK_FAST()	{ spi_init( SPIDEV, 5 * 1000 * 1000 ); }
#define FCLK_SLOW()	{ spi_init( SPIDEV, 100 * 1000 ); }

#define CS_HIGH()	{ gpio_put( DEF_SPI_CSN_PIN, 1 ); /* HIGH */ }
#define CS_LOW()	{ gpio_put(DEF_SPI_CSN_PIN , 0 ); /* LOW */ }

#define	MMC_CD		1 /* Card detect (yes:true, no:false, default:true) */
#define	MMC_WP		0 /* Write protected (yes:true, no:false, default:false) */


volatile  int  conter1;

void  Delay( int ms )
{
  //for( conter1=0; conter1<125*ms ; conter1++ ) ;
  sleep_ms(ms);
}


/*--------------------------------------------------------------------------

   Module Private Functions

---------------------------------------------------------------------------*/
#include "ff.h"			/* Obtains integer types */
#include "diskio.h"		/* Declarations of disk functions */


/* MMC card type flags (MMC_GET_TYPE) */
#define CT_MMC3		0x01		/* MMC ver 3 */
#define CT_MMC4		0x02		/* MMC ver 4+ */
#define CT_MMC		0x03		/* MMC */
#define CT_SDC1		0x02		/* SDC ver 1 */
#define CT_SDC2		0x04		/* SDC ver 2+ */
#define CT_SDC		0x0C		/* SDC */
#define CT_BLOCK	0x10		/* Block addressing */


/* MMC/SD command */
#define CMD0	(0)			/* GO_IDLE_STATE */
#define CMD1	(1)			/* SEND_OP_COND (MMC) */
#define	ACMD41	(0x80+41)	/* SEND_OP_COND (SDC) */
#define CMD8	(8)			/* SEND_IF_COND */
#define CMD9	(9)			/* SEND_CSD */
#define CMD10	(10)		/* SEND_CID */
#define CMD12	(12)		/* STOP_TRANSMISSION */
#define ACMD13	(0x80+13)	/* SD_STATUS (SDC) */
#define CMD16	(16)		/* SET_BLOCKLEN */
#define CMD17	(17)		/* READ_SINGLE_BLOCK */
#define CMD18	(18)		/* READ_MULTIPLE_BLOCK */
#define CMD23	(23)		/* SET_BLOCK_COUNT (MMC) */
#define	ACMD23	(0x80+23)	/* SET_WR_BLK_ERASE_COUNT (SDC) */
#define CMD24	(24)		/* WRITE_BLOCK */
#define CMD25	(25)		/* WRITE_MULTIPLE_BLOCK */
#define CMD32	(32)		/* ERASE_ER_BLK_START */
#define CMD33	(33)		/* ERASE_ER_BLK_END */
#define CMD38	(38)		/* ERASE */
#define CMD55	(55)		/* APP_CMD */
#define CMD58	(58)		/* READ_OCR */


static volatile DSTATUS Stat = STA_NOINIT;	/* Physical drive status */
static volatile UINT Timer1, Timer2;		/* 1kHz decrement timer stopped at zero (disk_timerproc()) */

static BYTE CardType;	/* Card type flags */



/*-----------------------------------------------------------------------*/
/* SPI controls (Platform dependent)                                     */
/*-----------------------------------------------------------------------*/

/* Initialize MMC interface */
static void init_spi (void)
{
	//spi_init( SPIDEV, 2 * 1000 * 1000 );
	//spi_init( SPIDEV, 5 * 1000 * 1000 );
	spi_cpol_t cpol = SPI_CPOL_0;
	spi_cpha_t cpha = SPI_CPHA_0;
	spi_set_format(SPIDEV, 8, cpol, cpha, SPI_MSB_FIRST); 
	gpio_set_function( DEF_SPI_TX_PIN, GPIO_FUNC_SPI );
	gpio_set_function( DEF_SPI_RX_PIN, GPIO_FUNC_SPI );
	gpio_set_function( DEF_SPI_SCK_PIN, GPIO_FUNC_SPI );
	//gpio_pull_up(DEF_SPI_RX_PIN);
	/* CS# */
	gpio_init( DEF_SPI_CSN_PIN );
	gpio_set_dir( DEF_SPI_CSN_PIN, GPIO_OUT);

	CS_HIGH();            /* Set CS# high */

#if 0
	for (Timer1 = 10; Timer1; ) ;	/* 10ms */
#else
	Delay( 10 );
#endif
}


/* Exchange a byte */
static BYTE xchg_spi (
	BYTE dat	/* Data to send */
)
{
	uint8_t  src, dst;

	src = dat;
	spi_write_read_blocking( SPIDEV, &src, &dst, 1 );
	return  (BYTE)dst;
}


/* Receive multiple byte */
static void rcvr_spi_multi (
	BYTE *buff,		/* Pointer to data buffer */
	UINT btr		/* Number of bytes to receive (even number) */
)
{

	spi_read_blocking( SPIDEV, 0xff, buff, btr );
}


/* Send multiple byte */
static void xmit_spi_multi (
	const BYTE *buff,	/* Pointer to the data */
	UINT btx			/* Number of bytes to send (even number) */
)
{
	spi_write_blocking( SPIDEV, buff, btx );
}


/*-----------------------------------------------------------------------*/
/* Wait for card ready                                                   */
/*-----------------------------------------------------------------------*/

static int wait_ready (	/* 1:Ready, 0:Timeout */
	UINT wt			/* Timeout [ms] */
)
{
	BYTE d;


	Timer2 = wt;
	do {
		d = xchg_spi(0xFF);
		/* This loop takes a time. Insert rot_rdq() here for multitask envilonment. */
	} while (d != 0xFF && Timer2);	/* Wait for card goes ready or timeout */

	return (d == 0xFF) ? 1 : 0;
}



/*-----------------------------------------------------------------------*/
/* Deselect card and release SPI                                         */
/*-----------------------------------------------------------------------*/

static void deselect (void)
{
	CS_HIGH();		/* Set CS# high */
	xchg_spi(0xFF);	/* Dummy clock (force DO hi-z for multiple slave SPI) */

}



/*-----------------------------------------------------------------------*/
/* Select card and wait for ready                                        */
/*-----------------------------------------------------------------------*/

static int select (void)	/* 1:OK, 0:Timeout */
{
	CS_LOW();		/* Set CS# low */

    for (int i= 0; i < 10; i++ )
		xchg_spi(0xFF);	/* Dummy clock (force DO enabled) */
	if (wait_ready(500)) return 1;	/* Wait for card ready */

	deselect();
	return 0;	/* Timeout */
}



/*-----------------------------------------------------------------------*/
/* Receive a data packet from the MMC                                    */
/*-----------------------------------------------------------------------*/

static int rcvr_datablock (	/* 1:OK, 0:Error */
	BYTE *buff,				/* Data buffer */
	UINT btr				/* Data block length (byte) */
)
{
	BYTE token;


	Timer1 = 200;
	do {							/* Wait for DataStart token in timeout of 200ms */
		token = xchg_spi(0xFF);
		/* This loop will take a time. Insert rot_rdq() here for multitask envilonment. */
	} while ((token == 0xFF) && Timer1);
	if(token != 0xFE) return 0;		/* Function fails if invalid DataStart token or timeout */

	rcvr_spi_multi(buff, btr);		/* Store trailing data to the buffer */
	xchg_spi(0xFF); xchg_spi(0xFF);			/* Discard CRC */

	return 1;						/* Function succeeded */
}



/*-----------------------------------------------------------------------*/
/* Send a data packet to the MMC                                         */
/*-----------------------------------------------------------------------*/

#if FF_FS_READONLY == 0
static int xmit_datablock (	/* 1:OK, 0:Failed */
	const BYTE *buff,		/* Ponter to 512 byte data to be sent */
	BYTE token				/* Token */
)
{
	BYTE resp;


	if (!wait_ready(500)) return 0;		/* Wait for card ready */

	xchg_spi(token);					/* Send token */
	if (token != 0xFD) {				/* Send data if token is other than StopTran */
		xmit_spi_multi(buff, 512);		/* Data */
		xchg_spi(0xFF); xchg_spi(0xFF);	/* Dummy CRC */

		resp = xchg_spi(0xFF);				/* Receive data resp */
		if ((resp & 0x1F) != 0x05) return 0;	/* Function fails if the data packet was not accepted */
	}
	return 1;
}
#endif


/*-----------------------------------------------------------------------*/
/* Send a command packet to the MMC                                      */
/*-----------------------------------------------------------------------*/

static BYTE send_cmd (	/* Return value: R1 resp (bit7==1:Failed to send) */
	BYTE cmd,			/* Command index */
	DWORD arg			/* Argument */
)
{
	BYTE n, res;


	if (cmd & 0x80) {	/* Send a CMD55 prior to ACMD<n> */
		cmd &= 0x7F;
		res = send_cmd(CMD55, 0);
		if (res > 1) return res;
	}

	/* Select the card and wait for ready except to stop multiple block read */
	if (cmd != CMD12) {
		deselect();
		if (!select()) return 0xFF;
	}

	/* Send command packet */
	xchg_spi(0x40 | cmd);				/* Start + command index */
	xchg_spi((BYTE)(arg >> 24));		/* Argument[31..24] */
	xchg_spi((BYTE)(arg >> 16));		/* Argument[23..16] */
	xchg_spi((BYTE)(arg >> 8));			/* Argument[15..8] */
	xchg_spi((BYTE)arg);				/* Argument[7..0] */
	n = 0x01;							/* Dummy CRC + Stop */
	if (cmd == CMD0) n = 0x95;			/* Valid CRC for CMD0(0) */
	if (cmd == CMD8) n = 0x87;			/* Valid CRC for CMD8(0x1AA) */
	xchg_spi(n);

	/* Receive command resp */
	if (cmd == CMD12) xchg_spi(0xFF);	/* Diacard following one byte when CMD12 */
	n = 10;								/* Wait for response (10 bytes max) */
	do {
		res = xchg_spi(0xFF);
	} while ((res & 0x80) && --n);

	return res;							/* Return received response */
}



/*--------------------------------------------------------------------------

   Public Functions

---------------------------------------------------------------------------*/


/*-----------------------------------------------------------------------*/
/* Initialize disk drive                                                 */
/*-----------------------------------------------------------------------*/

DSTATUS disk_initialize (
	BYTE drv		/* Physical drive number (0) */
)
{
	BYTE n, cmd, ty, ocr[4];


	if (drv) return STA_NOINIT;			/* Supports only drive 0 */
	init_spi();							/* Initialize SPI */

	if (Stat & STA_NODISK) return Stat;	/* Is card existing in the soket? */

	FCLK_SLOW();
	for (n = 10; n; n--) xchg_spi(0xFF);	/* Send 80 dummy clocks */

	ty = 0;
	if (send_cmd(CMD0, 0) == 1) {			/* Put the card SPI/Idle state */
		Timer1 = 1000;						/* Initialization timeout = 1 sec */
		if (send_cmd(CMD8, 0x1AA) == 1) {	/* SDv2? */
			for (n = 0; n < 4; n++) ocr[n] = xchg_spi(0xFF);	/* Get 32 bit return value of R7 resp */
			if (ocr[2] == 0x01 && ocr[3] == 0xAA) {				/* Is the card supports vcc of 2.7-3.6V? */
				while (Timer1 && send_cmd(ACMD41, 1UL << 30)) ;	/* Wait for end of initialization with ACMD41(HCS) */
				if (Timer1 && send_cmd(CMD58, 0) == 0) {		/* Check CCS bit in the OCR */
					for (n = 0; n < 4; n++) ocr[n] = xchg_spi(0xFF);
					ty = (ocr[0] & 0x40) ? CT_SDC2 | CT_BLOCK : CT_SDC2;	/* Card id SDv2 */
				}
			}
		} else {	/* Not SDv2 card */
			if (send_cmd(ACMD41, 0) <= 1) 	{	/* SDv1 or MMC? */
				ty = CT_SDC1; cmd = ACMD41;	/* SDv1 (ACMD41(0)) */
			} else {
				ty = CT_MMC3; cmd = CMD1;	/* MMCv3 (CMD1(0)) */
			}
			while (Timer1 && send_cmd(cmd, 0)) ;		/* Wait for end of initialization */
			if (!Timer1 || send_cmd(CMD16, 512) != 0)	/* Set block length: 512 */
				ty = 0;
		}
	}
	CardType = ty;	/* Card type */
	deselect();

	if (ty) {			/* OK */
		FCLK_FAST();			/* Set fast clock */
		Stat &= ~STA_NOINIT;	/* Clear STA_NOINIT flag */
	} else {			/* Failed */
		Stat = STA_NOINIT;
	}

	return Stat;
}



/*-----------------------------------------------------------------------*/
/* Get disk status                                                       */
/*-----------------------------------------------------------------------*/

DSTATUS disk_status (
	BYTE drv		/* Physical drive number (0) */
)
{
	if (drv) return STA_NOINIT;		/* Supports only drive 0 */

	return Stat;	/* Return disk status */
}



/*-----------------------------------------------------------------------*/
/* Read sector(s)                                                        */
/*-----------------------------------------------------------------------*/

DRESULT disk_read (
	BYTE drv,		/* Physical drive number (0) */
	BYTE *buff,		/* Pointer to the data buffer to store read data */
	LBA_t sector,	/* Start sector number (LBA) */
	UINT count		/* Number of sectors to read (1..128) */
)
{
	DWORD sect = (DWORD)sector;


	if (drv || !count) return RES_PARERR;		/* Check parameter */
	if (Stat & STA_NOINIT) return RES_NOTRDY;	/* Check if drive is ready */

	if (!(CardType & CT_BLOCK)) sect *= 512;	/* LBA ot BA conversion (byte addressing cards) */

	if (count == 1) {	/* Single sector read */
		if ((send_cmd(CMD17, sect) == 0)	/* READ_SINGLE_BLOCK */
			&& rcvr_datablock(buff, 512)) {
			count = 0;
		}
	}
	else {				/* Multiple sector read */
		if (send_cmd(CMD18, sect) == 0) {	/* READ_MULTIPLE_BLOCK */
			do {
				if (!rcvr_datablock(buff, 512)) break;
				buff += 512;
			} while (--count);
			send_cmd(CMD12, 0);				/* STOP_TRANSMISSION */
		}
	}
	deselect();

	return count ? RES_ERROR : RES_OK;	/* Return result */
}


/*-----------------------------------------------------------------------*/
/* Write sector(s)                                                       */
/*-----------------------------------------------------------------------*/

#if FF_FS_READONLY == 0
DRESULT disk_write (
	BYTE drv,			/* Physical drive number (0) */
	const BYTE *buff,	/* Ponter to the data to write */
	LBA_t sector,		/* Start sector number (LBA) */
	UINT count			/* Number of sectors to write (1..128) */
)
{
	DWORD sect = (DWORD)sector;


	if (drv || !count) return RES_PARERR;		/* Check parameter */
	if (Stat & STA_NOINIT) return RES_NOTRDY;	/* Check drive status */
	if (Stat & STA_PROTECT) return RES_WRPRT;	/* Check write protect */

	if (!(CardType & CT_BLOCK)) sect *= 512;	/* LBA ==> BA conversion (byte addressing cards) */

	if (count == 1) {	/* Single sector write */
		if ((send_cmd(CMD24, sect) == 0)	/* WRITE_BLOCK */
			&& xmit_datablock(buff, 0xFE)) {
			count = 0;
		}
	}
	else {				/* Multiple sector write */
		if (CardType & CT_SDC) send_cmd(ACMD23, count);	/* Predefine number of sectors */
		if (send_cmd(CMD25, sect) == 0) {	/* WRITE_MULTIPLE_BLOCK */
			do {
				if (!xmit_datablock(buff, 0xFC)) break;
				buff += 512;
			} while (--count);
			if (!xmit_datablock(0, 0xFD)) count = 1;	/* STOP_TRAN token */
		}
	}
	deselect();

	return count ? RES_ERROR : RES_OK;	/* Return result */
}
#endif

/*-----------------------------------------------------------------------*/
/* Miscellaneous drive controls other than data read/write               */
/*-----------------------------------------------------------------------*/

DRESULT disk_ioctl (
	BYTE drv,		/* Physical drive number (0) */
	BYTE cmd,		/* Control command code */
	void *buff		/* Pointer to the conrtol data */
)
{
	DRESULT res;
	BYTE n, csd[16];
	DWORD st, ed, csize;
	LBA_t *dp;


	if (drv) return RES_PARERR;					/* Check parameter */
	if (Stat & STA_NOINIT) return RES_NOTRDY;	/* Check if drive is ready */

	res = RES_ERROR;

	switch (cmd) {
	case CTRL_SYNC :		/* Wait for end of internal write process of the drive */
		if (select()) res = RES_OK;
		break;

	case GET_SECTOR_COUNT :	/* Get drive capacity in unit of sector (DWORD) */
		if ((send_cmd(CMD9, 0) == 0) && rcvr_datablock(csd, 16)) {
			if ((csd[0] >> 6) == 1) {	/* SDC CSD ver 2 */
				csize = csd[9] + ((WORD)csd[8] << 8) + ((DWORD)(csd[7] & 63) << 16) + 1;
				*(LBA_t*)buff = csize << 10;
			} else {					/* SDC CSD ver 1 or MMC */
				n = (csd[5] & 15) + ((csd[10] & 128) >> 7) + ((csd[9] & 3) << 1) + 2;
				csize = (csd[8] >> 6) + ((WORD)csd[7] << 2) + ((WORD)(csd[6] & 3) << 10) + 1;
				*(LBA_t*)buff = csize << (n - 9);
			}
			res = RES_OK;
		}
		break;

	case GET_BLOCK_SIZE :	/* Get erase block size in unit of sector (DWORD) */
		if (CardType & CT_SDC2) {	/* SDC ver 2+ */
			if (send_cmd(ACMD13, 0) == 0) {	/* Read SD status */
				xchg_spi(0xFF);
				if (rcvr_datablock(csd, 16)) {				/* Read partial block */
					for (n = 64 - 16; n; n--) xchg_spi(0xFF);	/* Purge trailing data */
					*(DWORD*)buff = 16UL << (csd[10] >> 4);
					res = RES_OK;
				}
			}
		} else {					/* SDC ver 1 or MMC */
			if ((send_cmd(CMD9, 0) == 0) && rcvr_datablock(csd, 16)) {	/* Read CSD */
				if (CardType & CT_SDC1) {	/* SDC ver 1.XX */
					*(DWORD*)buff = (((csd[10] & 63) << 1) + ((WORD)(csd[11] & 128) >> 7) + 1) << ((csd[13] >> 6) - 1);
				} else {					/* MMC */
					*(DWORD*)buff = ((WORD)((csd[10] & 124) >> 2) + 1) * (((csd[11] & 3) << 3) + ((csd[11] & 224) >> 5) + 1);
				}
				res = RES_OK;
			}
		}
		break;

	case CTRL_TRIM :	/* Erase a block of sectors (used when _USE_ERASE == 1) */
		if (!(CardType & CT_SDC)) break;				/* Check if the card is SDC */
		if (disk_ioctl(drv, MMC_GET_CSD, csd)) break;	/* Get CSD */
		if (!(csd[10] & 0x40)) break;					/* Check if ERASE_BLK_EN = 1 */
		dp = buff; st = (DWORD)dp[0]; ed = (DWORD)dp[1];	/* Load sector block */
		if (!(CardType & CT_BLOCK)) {
			st *= 512; ed *= 512;
		}
		if (send_cmd(CMD32, st) == 0 && send_cmd(CMD33, ed) == 0 && send_cmd(CMD38, 0) == 0 && wait_ready(30000)) {	/* Erase sector block */
			res = RES_OK;	/* FatFs does not check result of this command */
		}
		break;

	/* Following commands are never used by FatFs module */

	case MMC_GET_TYPE:		/* Get MMC/SDC type (BYTE) */
		*(BYTE*)buff = CardType;
		res = RES_OK;
		break;

	case MMC_GET_CSD:		/* Read CSD (16 bytes) */
		if (send_cmd(CMD9, 0) == 0 && rcvr_datablock((BYTE*)buff, 16)) {	/* READ_CSD */
			res = RES_OK;
		}
		break;

	case MMC_GET_CID:		/* Read CID (16 bytes) */
		if (send_cmd(CMD10, 0) == 0 && rcvr_datablock((BYTE*)buff, 16)) {	/* READ_CID */
			res = RES_OK;
		}
		break;

	case MMC_GET_OCR:		/* Read OCR (4 bytes) */
		if (send_cmd(CMD58, 0) == 0) {	/* READ_OCR */
			for (n = 0; n < 4; n++) *(((BYTE*)buff) + n) = xchg_spi(0xFF);
			res = RES_OK;
		}
		break;

	case MMC_GET_SDSTAT:	/* Read SD status (64 bytes) */
		if (send_cmd(ACMD13, 0) == 0) {	/* SD_STATUS */
			xchg_spi(0xFF);
			if (rcvr_datablock((BYTE*)buff, 64)) res = RES_OK;
		}
		break;

	default:
		res = RES_PARERR;
	}

	deselect();

	return res;
}



/*-----------------------------------------------------------------------*/
/* Device timer function                                                 */
/*-----------------------------------------------------------------------*/
/* This function must be called from timer interrupt routine in period
/  of 1 ms to generate card control timing.
*/

void disk_timerproc (void)
{
	WORD n;
	BYTE s;


	n = Timer1;						/* 1kHz decrement timer stopped at 0 */
	if (n) Timer1 = --n;
	n = Timer2;
	if (n) Timer2 = --n;

	s = Stat;
	if (MMC_WP) {	/* Write protected */
		s |= STA_PROTECT;
	} else {		/* Write enabled */
		s &= ~STA_PROTECT;
	}
	if (MMC_CD) {	/* Card is in socket */
		s &= ~STA_NODISK;
	} else {		/* Socket empty */
		s |= (STA_NODISK | STA_NOINIT);
	}
	Stat = s;
}
#endif
