# Wiring

```text
L:>spi1 -p 14,15 --baudrate:50000000
L:>display-st7789 setup {spi:1 rst:6 dc:7 cs:8 bl:9}
```

```text
L:>display
display#0: ST7789 [TypeA] 240x320 spi1 RST:6 DC:7 CS:8 BL:9 CPOL:0 CPHA:0
```

```text
L:>spi1 -p 14,15 --baudrate:50000000
L:>display-st7789 setup {spi:1 rst:6 dc:7 cs:8 bl:9}
L:>display-st7789 setup {spi:1 rst:10 dc:11 cs:12 bl:13}
```

```text
L:>display
display#0: ST7789 [TypeA] 240x320 spi1 RST:6 DC:7 CS:8 BL:9 CPOL:0 CPHA:0
Display#1: ST7789 [TypeA] 240x320 spi1 RST:10 DC:11 CS:12 BL:13 CPOL:0 CPHA:0
```
