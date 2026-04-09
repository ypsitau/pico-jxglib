# Wi-Fi Connection

This article covers the following network operations:

1. Scan for available Wi-Fi access points
2. Connect to Wi-Fi with a specified SSID and password
3. Set a static IP address
4. Setting up auto-start for Wi-Fi connection

## Connecting to a Wi-Fi Network

Connect to the pico-jxgLABO shell on the Pico board via USB serial, and use the `net` shell command for network operations. First, use the `wifi-scan` subcommand to find available access points:

```text
L:/>net wifi-scan
ssid:MyHome-WiFi                      rssi: -68 channel:  8 mac:xx:xx:xx:xx:xx:xx security:5
ssid:A12345678                        rssi: -75 channel: 11 mac:xx:xx:xx:xx:xx:xx security:7
ssid:LivingRoomAP                     rssi: -76 channel:  4 mac:xx:xx:xx:xx:xx:xx security:7
ssid:HUMIN-4512                       rssi: -76 channel:  4 mac:xx:xx:xx:xx:xx:xx security:7
```

Connect to a Wi-Fi router with the `wifi-connect` subcommand, specifying SSID and password:

```text
L:/>net wifi-connect {ssid:'MyHome-WiFi' password:'PASSWORD'}
Connected ssid:'MyHome-WiFi' auth:wpa2 addr:192.168.0.20 netmask:255.255.255.0 gateway:192.168.0.1
```

In this example, DHCP assigns IP address `192.168.0.20`, netmask `255.255.255.0`, and gateway `192.168.0.1`.

:::message
Some Wi-Fi routers may not work well with the Pico board. If your host PC is Windows, you can use the mobile hotspot feature as an access point. This is also safer than using a Wi-Fi router, so give it a try.
:::

To set a static IP address, use the `config` subcommand. The example below sets the IP to `192.168.0.101`:

```text
L:/>net config {addr:192.168.0.101}
Connected ssid:'MyHome-WiFi' auth:wpa2 addr:192.168.0.101 netmask:255.255.255.0 gateway:192.168.0.1
```

You can also specify netmask and gateway, but these must match the values assigned by DHCP when connecting to the access point, so usually you only specify the IP address.

You can combine `wifi-connect` and `config`:

```text
L:/>net wifi-connect {ssid:'MyHome-WiFi' password:'PASSWORD'} config {addr:192.168.0.101}
```

## Setting Up Auto-Start

Create a file named `.startup` in the root of the `L:` drive to run commands automatically when the Pico board powers on. Here’s an example to automate Wi-Fi connection and Telnet server startup:

```text title=".startup"
net wifi-connect {ssid:'SSID' password:'PASSWORD'} config {addr:XXX.XXX.XXX.XXX}
telnet-server start
led on flip:50,500,50,500,50,2000
```

When you connect the Pico board running pico-jxgLABO via USB, the `L:` drive appears as a USB drive on your PC (often as `D:` on Windows). Edit the file above, change `SSID`, `PASSWORD`, and `XXX.XXX.XXX.XXX` as needed, and copy it to the drive.

After rebooting, if all commands in the script run without error, the `led` command will blink the built-in LED to indicate successful startup. You can set different blink patterns for each board’s IP address for easy identification (e.g., one blink for 192.168.0.101, two for 192.168.0.102, etc.).
