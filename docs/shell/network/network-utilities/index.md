# Network Utilities

## ping

```text title="Help of the Command"
Usage: ping URL
```

The command checks if the specified host is reachable by sending ICMP echo requests. For example, to check if the gateway is reachable:

```text
L:/>ping 192.168.0.1
Reply from 192.168.0.1: time=2ms
Reply from 192.168.0.1: time=1ms
Reply from 192.168.0.1: time=2ms
```

## nslookup

```text title="Help of the Command"
Usage: nslookup URL
Options:
 -h --help prints this help
```

The command queries a DNS server to resolve a hostname to an IP address. For example, to resolve `raspberrypi.com`:

```text
L:/>nslookup raspberrypi.com
104.21.88.234
```

## ntp

```text title="Help of the Command"
Usage: ntp [OPTION]... [SERVER]
Options:
 -h --help print this help
 -u --utc  print UTC time
 -r --rtc  set RTC if available
```

The command connects to an NTP server to get the current time. If no server is specified, it uses a default NTP server. For example:

```text
L:/>ntp
2025-10-06 12:34:56Z
```

By default, UTC time is shown. Set the shell variable `TZ` to display local time. For example, to set Japan Standard Time (JST):

```text
L:/>set TZ=JST-9
```

Run `ntp` again to see the time in JST:

```text
L:/>ntp
2025-10-06 21:34:56
```

When the `-r` (or `--rtc`) option is specified, the command also sets the connected RTC (Real-Time Clock) to the obtained.

```text
L:/>ntp -r
2025-10-06 21:38:12
RTC updated
```

See [this page](../../device/rtc/index.md) for details on RTC.
