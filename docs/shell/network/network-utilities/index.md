# Network Utilities

First, the basic `ping` command. Try pinging the gateway:

```text
L:/>ping 192.168.0.1
Reply from 192.168.0.1: time=2ms
Reply from 192.168.0.1: time=1ms
Reply from 192.168.0.1: time=2ms
```

Use `nslookup` to query a DNS server:

```text
L:/>nslookup raspberrypi.com
104.21.88.234
```

Use `ntp` to connect to an NTP server and get the current time:

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
