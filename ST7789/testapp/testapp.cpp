#include <stdio.h>
#include "jxglib/ST7789.h"

using namespace jxglib;

int main()
{
	uint16_t* buff = reinterpret_cast<uint16_t*>(::malloc(240 * 240 * 2));
	ST7789 tft(spi0, 240, 240, 20, 21, 22);
	::stdio_init_all();
	::spi_init(spi0, 125 * 1000 * 1000);
	::gpio_set_function(PICO_DEFAULT_SPI_TX_PIN, GPIO_FUNC_SPI);
	::gpio_set_function(PICO_DEFAULT_SPI_SCK_PIN, GPIO_FUNC_SPI);
	tft.Initialize();
	tft.raw.ColumnAddressSet(0, 240);
	tft.raw.RowAddressSet(0, 240);
	for (;;) {
		::memset(buff, 0x00, 240 * 240 * 2);		
		tft.raw.MemoryWriteBy16Bit(buff, 240 * 240);
		::sleep_ms(10);
		::memset(buff, 0xff, 240 * 240 * 2);		
		tft.raw.MemoryWriteBy16Bit(buff, 240 * 240);
		::sleep_ms(10);
	}
}

#if 0
struct st7789_config {
	spi_inst_t* spi;
	uint gpio_dc;
	uint gpio_rst;
	uint gpio_bl;
	int gpio_cs;
};

static struct st7789_config st7789_cfg;
static uint16_t st7789_width;
static uint16_t st7789_height;
static bool st7789_data_mode = false;

static void st7789_SendCmd(uint8_t cmd, const uint8_t* data, size_t len)
{
	if (st7789_cfg.gpio_cs > -1) {
		spi_set_format(st7789_cfg.spi, 8, SPI_CPOL_0, SPI_CPHA_0, SPI_MSB_FIRST);
	} else {
		spi_set_format(st7789_cfg.spi, 8, SPI_CPOL_1, SPI_CPHA_1, SPI_MSB_FIRST);
	}
	st7789_data_mode = false;

	sleep_us(1);
	if (st7789_cfg.gpio_cs > -1) {
		gpio_put(st7789_cfg.gpio_cs, 0);
	}
	gpio_put(st7789_cfg.gpio_dc, 0);
	sleep_us(1);
	
	spi_write_blocking(st7789_cfg.spi, &cmd, sizeof(cmd));
	
	if (len) {
		sleep_us(1);
		gpio_put(st7789_cfg.gpio_dc, 1);
		sleep_us(1);
		
		spi_write_blocking(st7789_cfg.spi, data, len);
	}

	sleep_us(1);
	if (st7789_cfg.gpio_cs > -1) {
		gpio_put(st7789_cfg.gpio_cs, 1);
	}
	gpio_put(st7789_cfg.gpio_dc, 1);
	sleep_us(1);
}

void st7789_ColumnAddressSet(uint16_t xs, uint16_t xe)
{
	uint8_t data[] = {
		static_cast<uint8_t>(xs >> 8),
		static_cast<uint8_t>(xs & 0xff),
		static_cast<uint8_t>(xe >> 8),
		static_cast<uint8_t>(xe & 0xff),
	};

	// CASET (2Ah): Column Address Set
	st7789_SendCmd(0x2a, data, sizeof(data));
}

void st7789_RowAddressSet(uint16_t ys, uint16_t ye)
{
	uint8_t data[] = {
		static_cast<uint8_t>(ys >> 8),
		static_cast<uint8_t>(ys & 0xff),
		static_cast<uint8_t>(ye >> 8),
		static_cast<uint8_t>(ye & 0xff),
	};

	// RASET (2Bh): Row Address Set
	st7789_SendCmd(0x2b, data, sizeof(data));
}

void st7789_Initialize(const struct st7789_config* config, uint16_t width, uint16_t height)
{
	memcpy(&st7789_cfg, config, sizeof(st7789_cfg));
	st7789_width = width;
	st7789_height = height;

	if (st7789_cfg.gpio_cs > -1) {
		spi_set_format(st7789_cfg.spi, 8, SPI_CPOL_0, SPI_CPHA_0, SPI_MSB_FIRST);
	} else {
		spi_set_format(st7789_cfg.spi, 8, SPI_CPOL_1, SPI_CPHA_1, SPI_MSB_FIRST);
	}


	if (st7789_cfg.gpio_cs > -1) {
		gpio_init(st7789_cfg.gpio_cs);
	}
	gpio_init(st7789_cfg.gpio_dc);
	gpio_init(st7789_cfg.gpio_rst);
	gpio_init(st7789_cfg.gpio_bl);

	if (st7789_cfg.gpio_cs > -1) {
		gpio_set_dir(st7789_cfg.gpio_cs, GPIO_OUT);
	}
	gpio_set_dir(st7789_cfg.gpio_dc, GPIO_OUT);
	gpio_set_dir(st7789_cfg.gpio_rst, GPIO_OUT);
	gpio_set_dir(st7789_cfg.gpio_bl, GPIO_OUT);

	if (st7789_cfg.gpio_cs > -1) {
		gpio_put(st7789_cfg.gpio_cs, 1);
	}
	gpio_put(st7789_cfg.gpio_dc, 1);
	gpio_put(st7789_cfg.gpio_rst, 1);
	sleep_ms(100);
	
	// SWRESET (01h): Software Reset
	st7789_SendCmd(0x01, NULL, 0);
	sleep_ms(150);

	// SLPOUT (11h): Sleep Out
	st7789_SendCmd(0x11, NULL, 0);
	sleep_ms(50);

	// COLMOD (3Ah): Interface Pixel Format
	// - RGB interface color format     = 65K of RGB interface
	// - Control interface color format = 16bit/pixel
	uint8_t tmp1[] = { 0x55 };
	//st7789_SendCmd(0x3a, (uint8_t[]){ 0x55 }, 1);
	st7789_SendCmd(0x3a, tmp1, 1);
	sleep_ms(10);

	// MADCTL (36h): Memory Data Access Control
	// - Page Address Order            = Top to Bottom
	// - Column Address Order          = Left to Right
	// - Page/Column Order             = Normal Mode
	// - Line Address Order            = LCD Refresh Top to Bottom
	// - RGB/BGR Order                 = RGB
	// - Display Data Latch Data Order = LCD Refresh Left to Right
	uint8_t tmp2[] = { 0x00 };
	//st7789_SendCmd(0x36, (uint8_t[]){ 0x00 }, 1);
	st7789_SendCmd(0x36, tmp2, 1);
   
	st7789_ColumnAddressSet(0, width);
	st7789_RowAddressSet(0, height);

	// INVON (21h): Display Inversion On
	st7789_SendCmd(0x21, NULL, 0);
	sleep_ms(10);

	// NORON (13h): Normal Display Mode On
	st7789_SendCmd(0x13, NULL, 0);
	sleep_ms(10);

	// DISPON (29h): Display On
	st7789_SendCmd(0x29, NULL, 0);
	sleep_ms(10);

	gpio_put(st7789_cfg.gpio_bl, 1);
}

void st7789_PutPixel(uint16_t pixel)
{
	if (!st7789_data_mode) {
		sleep_us(1);
		if (st7789_cfg.gpio_cs > -1) {
			gpio_put(st7789_cfg.gpio_cs, 0);
		}
		gpio_put(st7789_cfg.gpio_dc, 0);
		sleep_us(1);

		// RAMWR (2Ch): Memory Write
		uint8_t cmd = 0x2c;
		spi_write_blocking(st7789_cfg.spi, &cmd, sizeof(cmd));

		sleep_us(1);
		if (st7789_cfg.gpio_cs > -1) {
			gpio_put(st7789_cfg.gpio_cs, 0);
		}
		gpio_put(st7789_cfg.gpio_dc, 1);
		sleep_us(1);

		if (st7789_cfg.gpio_cs > -1) {
			spi_set_format(st7789_cfg.spi, 16, SPI_CPOL_0, SPI_CPHA_0, SPI_MSB_FIRST);
		} else {
			spi_set_format(st7789_cfg.spi, 16, SPI_CPOL_1, SPI_CPHA_1, SPI_MSB_FIRST);
		}

		st7789_data_mode = true;
	}

	spi_write16_blocking(st7789_cfg.spi, &pixel, 1);
}

void st7789_SetCursor(uint16_t x, uint16_t y)
{
	st7789_ColumnAddressSet(x, st7789_width);
	st7789_RowAddressSet(y, st7789_height);
}

void st7789_ScrollVertical(uint16_t row)
{
	uint8_t data[] = {
		static_cast<uint8_t>((row >> 8) & 0xff),
		static_cast<uint8_t>(row & 0x00ff)
	};

	// VSCSAD (37h): Vertical Scroll Start Address of RAM 
	st7789_SendCmd(0x37, data, sizeof(data));
}

void st7789_Fill(uint16_t pixel)
{
	int num_pixels = st7789_width * st7789_height;

	st7789_SetCursor(0, 0);

	for (int i = 0; i < num_pixels; i++) {
		st7789_PutPixel(pixel);
	}
}

int main()
{
	static const struct st7789_config lcd_config = {
		.spi      = spi0,
		.gpio_dc  = 20,
		.gpio_rst = 21,
		.gpio_bl  = 22,
		.gpio_cs  = -1, // PICO_DEFAULT_SPI_CSN_PIN,
	};
	stdio_init_all();
	::spi_init(spi0, 125 * 1000 * 1000);
	::gpio_set_function(PICO_DEFAULT_SPI_TX_PIN, GPIO_FUNC_SPI);
	::gpio_set_function(PICO_DEFAULT_SPI_SCK_PIN, GPIO_FUNC_SPI);
	st7789_Initialize(&lcd_config, 240, 240);

	st7789_Fill(0x0000);

	while (1) {
		st7789_Fill(0x0000);
		::sleep_ms(1000);
		st7789_Fill(0xffff);
		::sleep_ms(1000);
	}
#if 0
	while (1) {
		// create a random x, y, and color value
		int rand_x = rand() % 240;
		int rand_y = rand() % 240;
		uint16_t rand_color = rand() % 0xffff;
		
		// move the cursor to the random x and y position
		st7789_SetCursor(rand_x, rand_y);

		// put the random color as the pixel value
		st7789_PutPixel(rand_color);
	}
#endif

}
#endif
