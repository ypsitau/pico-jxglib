# RTC (Real Time Clock)

In this guide, we will connect an RTC (Real Time Clock) module to the Pico board using pico-jxgLABO.

By connecting an RTC module, you can accurately obtain the current time and record timestamps when creating or updating files. The RTC module is used via the I2C interface.

## About the DS3231 RTC Module

An RTC module is hardware that keeps track of real-time date and time. The original Pico (RP2040) has a built-in RTC, but it loses the date and time when power is lost, so it is not very practical. In any case, the successor Pico2 (RP2350) removed this module, so it is not a candidate for practical use.

The practical way to use RTC is to connect an RTC module with a backup battery to the Pico board. This allows the date and time to be retained regardless of the Pico board's power state.

The DS3231 is a commonly used RTC module for electronics projects, as it can be connected via I2C. There is also a cheaper DS1307, but it can drift by several seconds per day, so the DS3231 is recommended for its higher accuracy.

The DS3231 module I purchased from Amazon looks like this:

![rtc-ds3231](https://raw.githubusercontent.com/ypsitau/zenn/main/images/2025-06-22-rtc/rtc-ds3231.jpg)

It comes with a backup battery pre-installed and has a compact shape. The signal names printed on the board can be confusing, but they correspond as follows:

|Board Label|Signal Name|
|------|-----|
|`+`   |VCC  |
|`D`   |SDA  |
|`C`   |SCL  |
|`-`   |GND  |

## Device Connection and Operation Check

The DS3231 RTC module is connected via the I2C interface. You can use either I2C0 or I2C1 on the Pico board, and the pin layout can be freely set with commands. Here, we use I2C0 and connect as follows:

|DS3231       |Pico Pin No.|GPIO  |Function   |
|-------------|------------|------|-----------|
|VCC          |36          |      |3V3        |
|GND          |8           |      |GND        |
|SDA          |11          |GPIO8 |I2C0 SDA   |
|SCL          |12          |GPIO9 |I2C0 SCL   |

The wiring diagram is shown below. There are multiple GND pins on the Pico board, so you can connect to any of them.


![circuit-rtc](https://raw.githubusercontent.com/ypsitau/zenn/main/images/2025-10-22-labo-sdcard-rtc/circuit-rtc.png)

Run the following command to set the GPIO assignment for I2C0 to GPIO8 (I2C0 SDA) and GPIO9 (I2C0 SCL). The appropriate function assignment is done automatically, so the order does not matter.

```text
L:/>i2c0 -p 8,9
```

Scan the devices connected to the I2C bus to confirm that the RTC module is connected correctly. The DS3231 uses I2C address `0x68`.

```text
L:/>i2c0 scan
Bus Scan on I2C0
   0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F
00 -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
10 -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
20 -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
30 -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
40 -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
50 -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
60 -- -- -- -- -- -- -- -- 68 -- -- -- -- -- -- --
70 -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
```

Run the following command to set up the DS3231 RTC module:

```text
L:/>rtc-ds3231 setup {i2c:0}
```

This completes the setup of the DS3231 RTC module.

Run the `rtc` command to get the current time from the RTC module:

```text
L:/>rtc
2000-01-01 00:00:00.000
```

You can set the date and time on the RTC module by specifying the date and time as an argument to the `rtc` command:


```text
L:/>rtc 2025-10-18 16:20:52
2025-10-18 16:20:52.000
```

If your Pico board is connected to Wi-Fi ([see here](https://zenn.dev/ypsitau/articles/2025-10-06-labo-wifi)), you can use the `ntp` command with the `-r` option to get the correct time from an NTP server and set it on the RTC module:

```text
L:/>ntp -r
2025-10-18 16:27:31
RTC updated
L:/>rtc
2025-10-18 16:27:33.000
```

Let's create a file and check that the timestamp is recorded correctly:

```text
L:/>touch testfile.txt
L:/>dir
-a--- 2025-10-18 16:28:02      0 testfile.txt
```
