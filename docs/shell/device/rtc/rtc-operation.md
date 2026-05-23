# Command Operation

## Operation Check

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

## Time Synchronization with NTP Server

If your Pico board is connected to Wi-Fi ([see here](../../network/index.md)), you can use the `ntp` command with the `-r` option to get the correct time from an NTP server and set it on the RTC module:

```text
L:/>ntp -r
2025-10-18 16:27:31
RTC updated
L:/>rtc
2025-10-18 16:27:33.000
```

## File Operations

Let's create a file and check that the timestamp is recorded correctly:

```text
L:/>touch testfile.txt
L:/>dir
-a--- 2025-10-18 16:28:02      0 testfile.txt
```

## Command Reference

### rtc

```text title="Help of the Command"
Usage: rtc [OPTION]... [DATE] [TIME]
Options:
 -h --help    prints this help
 -v --verbose verbose output

Set or get the RTC time. If no argument is given, it prints the current time.
```

### rtc-ds3231

```text title="Help of the Command"
Usage: rtc-ds3231 [OPTION]...
Options:
 -h --help prints this help
Subcommands:
  setup  Set up a DS3231 device with the given parameters: {i2c:BUS}
```
