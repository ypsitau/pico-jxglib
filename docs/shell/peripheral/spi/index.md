## Command Reference

### spi

```text title="Help of the Command" linenums="1"
Usage: spi BUS [OPTION]... [COMMAND]...
  BUS: SPI bus number (0 or 1)
Options:
 -h --help                prints this help
    --help-pin            prints help for pin assignment
    --dumb                no SPI operations, just remember the pins and settings
 -v --verbose             verbose output for debugging
 -p --pin=SCK,MOSI[,MISO] sets GPIO pins for SPI (function auto-detected)
 -c --pin-cs=CS_PIN       sets CS pin (any GPIO)
 -B --baudrate=BPS        sets SPI baudrate (default: 1000000)
 -m --mode=MODE           sets SPI mode (0-3, default: 0)
    --cpol=CPOL           sets clock polarity (0 or 1, default: 0)
    --cpha=CPHA           sets clock phase (0 or 1, default: 0)
    --order=ORDER         sets bit order (msb or lsb, default: msb)
    --dummy=BYTE          sets dummy byte for read operations (default: 0x00)
Sub Commands:
 repeat[:N] {CMD...}  repeat the commands N times (default: infinite)
 sleep:MSEC           sleep for specified milliseconds
 write:DATA           write DATA to SPI
 read:N               read N bytes from SPI (requires MISO)
 transfer:DATA        transfer DATA (write and read simultaneously, requires MISO)
 cs:VALUE             set CS pin value (0, 1, lo, hi)
```
