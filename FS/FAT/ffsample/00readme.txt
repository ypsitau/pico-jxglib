FatFs Module Sample Projects                             (C)ChaN, 2024


  This archive contains sample projects for function/compatibility test
  of FatFs module with platform dependent low level disk I/O modules.


DIRECTORIES

  Directory     Platform
  ------------- -------------------------------------------------
  generic       Generic MCUs
  lpc17xx       NXP LPC17xx (LPC1768)
  lpc23xx       NXP LPC23xx (LPC2388)
  stm32         ST STM32 (STM32F100RBT6)
  avr           Microchip AVR (ATmega1284)
  pic24         Microchip PIC24 (PIC24FJ64GA002)
  rx220         Renesas RX220 (R5F52206BDFM)
  rx62n         Renesas RX62N (R5F562N8BDFB)
  sh2           Renesas SH-2A (R5S72620W14FPU)
  h8            Renesas H8/300H (HD64F3694)
  v850          Renesas V850ES (UPD70F3340)
  win32         Windows OS (VC2008)

  Each sample project contains support for following media.

           MMC/SD  MMC/SD/eMMC   CFC     NAND-FTL   Virtual 
            (SPI)   (Native)   (8-bit)  (SLC/Small)  Drive  
  Generic     o                                             
  LPC17xx     o                                        o    
  LPC23xx              o                   o                
  STM32       o                                             
  AVR         o                  o                          
  PIC24       o                                             
  H8/300H     o                                             
  RX220       o                                             
  RX62N       o                                             
  SH-2A       o                                             
  V850ES      o                                             

  The disk I/O modules will able to be used as reference design for any other
  file system modules as well. You will able to find various implementations
  on the web other than these samples.



AGREEMENTS

  These sample projects for FatFs module are free software and there is NO
  WARRANTY. You can use, modify and redistribute it for personal, non-profit
  or commercial product without any restriction UNDER YOUR RESPONSIBILITY.



REVISION HISTORY

  Apr 29, 2006  First release.
  Aug 19, 2006  MMC module: Fixed a bug that disk_initialize() never time-out when card does not go ready.
  Oct 12, 2006  CF module: Fixed a bug that disk_initialize() can fail at 3.3V.
  Oct 22, 2006  Added a sample project for V850ES.
  Feb 04, 2007  All modules: Modified for FatFs module R0.04.
                MMC module: Fixed a bug that disk_ioctl() returns incorrect disk size.
  Apr 03, 2007  All modules: Modified for FatFs module R0.04a.
                MMC module: Supported high capacity SD memory cards.
  May 05, 2007  MMC modules: Fixed a bug that GET_SECTOR_COUNT via disk_ioctl() fails on MMC.
  Aug 26, 2007  Added some ioctl sub-functions.
  Oct 13, 2007  MMC modules: Fixed send_cmd() sends incorrect command packet.
  Dec 12, 2007  Added a sample project for Microchip PIC.
  Feb 03, 2008  All modules: Modified for FatFs module R0.05a.
  Apr 01, 2008  Modified main() for FatFs module R0.06.
  Oct 18, 2008  Added a sample project for NXP LPC2300.
  Apr 01, 2009  Modified for FatFs module R0.07.
  Apr 18, 2009  Modified for FatFs module R0.07a.
  Jun 25, 2009  Modified for FatFs module R0.07c.
  Jul 13, 2009  Added sample project for Unicoede API.
  May 15, 2010  Modified for FatFs module R0.08.
                Added a sample project for Renesas SH2A.
  Aug 16, 2010  Modified for FatFs module R0.08a.
                LPC2300: Added NAND-FTL driver.
  Oct 14, 2010  Added a sample project for generic uC.
  Nov 25, 2010  AVR: Fixed socket controls of MMC drivers. Added bitbanging MMC driver.
  Jan 15, 2011  Modified for FatFs module R0.08b.
  Feb 06, 2011  Added a sample project for Renesas RX62N.
  Jun 11, 2011  LPC2300: Updated project.
  Jul 31, 2011  Added LPC1700 project.
  Dec 22, 2011  LPC2300: Added JPEG decompressor.
  Sep 06, 2011  Modified for FatFs module R0.09.
  Feb 06, 2012  LPC2300: Updated NAND-FTL driver.
  May 09, 2012  Added FM3 project.
  Jun 08, 2012  AVR: Updated ATA/CFC drivers.
                LPC2300: Updated NAND-FTL driver.
  Aug 29, 2012  MMC modules: Fixed misrecognization of card size grater than 32GB.
  Jan 24, 2013  Modified for FatFs module R0.09b.
  Jun 22, 2013  AVR/PIC24: Updated MMC drivers.
  Jul 28, 2013  Added a foolproof project for ATMEL AVR.
  Oct 02, 2013  Modified for FatFs module R0.10.
  Nov 16, 2013  MMC modules: Fixed transmission of a CMD12 can fail.
  Dec 14, 2013  Added STM32 project.
  Jan 15, 2014  Updated for FatFs module R0.10a.
  May 19, 2014  Updated for FatFs module R0.10b.
  Jun 19, 2014  Added 'dl' command. (Win32 project)
  Nov 09, 2014  Updated for FatFs module R0.10c.
  Nov 26, 2014  Updated ff.c with patch2 applied.
  May 13, 2015  Updated for FatFs module R0.11.
  Sep 05, 2015  Updated for FatFs module R0.11a.
  Apr 12, 2016  Updated for FatFs module R0.12.
  Jul 10, 2016  Updated for FatFs module R0.12a.
  Sep 04, 2016  Updated for FatFs module R0.12b.
  Nov 15, 2016  Added RX220 project.
  Mar 04, 2017  Updated some projects. Updated to FatFs R0.12b.
  May 21, 2017  Updated to FatFs R0.13.
  Oct 14, 2017  Updated to FatFs R0.13a.
  Apr 07, 2018  Updated to FatFs R0.13b.
  Oct 14, 2019  Updated to FatFs R0.14.
  Dec 03, 2020  Updated to FatFs R0.14a.
  Apr 17, 2021  Updated to FatFs R0.14b.
  Nov 06, 2022  Updated to FatFs R0.15.
  Nov 22, 2024  Updated to FatFs R0.15a.

