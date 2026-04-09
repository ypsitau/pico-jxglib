# Wi-Fi Connection

This article explains how to connect a Pico board to a Wi-Fi network using pico-jxgLABO.

Traditionally, connecting a microcontroller to a network requires writing and running a program, often hardcoding the SSID and password for Wi-Fi, which makes distribution and maintenance difficult. In the pico-jxgLABO environment, shell commands are provided for network connection, allowing users to connect to networks interactively via commands.

This article covers the following network operations:

- Network connection
  1. Scan for available Wi-Fi access points
  2. Connect to Wi-Fi with a specified SSID and password
  3. Set a static IP address
- Try network commands (ping, nslookup, ntp) on the Pico board
- Run a Telnet server on the Pico board for remote command operation

## Raspberry Pi Pico and Wi-Fi Functionality

### Pico W and Pico 2 W

Pico W and Pico 2 W are Pico boards equipped with a CYW43439 Wi-Fi chip (CYW43 chip).

![picow-and-pico2w](images/picow-and-pico2w.jpg)
*Pico W and Pico 2 W*

Pico W costs about 1,200 yen, Pico 2 W about 1,400 yen—roughly 400 yen more than the non-Wi-Fi models due to the CYW43 chip.

Pico W/Pico 2 W use reserved GPIOs 23, 24, 25, and 29 to control the CYW43 chip. GPIO25, used for the built-in LED on Pico/Pico 2, is now used for CYW43 control, so the built-in LED is connected to the CYW43 chip’s GPIO. To control the LED on Pico W/Pico 2 W, you must communicate with the CYW43 chip, which is tricky. The Pico SDK provides `cyw43_arch_gpio_get()` and `cyw43_arch_gpio_put()` APIs for this purpose.

Controlling the CYW43 chip with limited GPIOs is challenging. For more details, see [this article](https://zenn.dev/nonnoise/articles/0d5b97cb517e31).

PIO (Programmable I/O) is also used for CYW43 control. Pico W has 2 PIO blocks, Pico 2 W has 3, but one block is used for CYW43, so be careful when designing systems that use PIO[^pio-usage].

[^pio-usage]: pico-jxgLABO also uses PIO for logic analyzer functionality.

### Use Cases for Pico’s Network Functionality

Pico W and Pico 2 W’s Wi-Fi is often used for IoT (Internet of Things) applications, such as sending sensor data to the cloud or controlling devices from the cloud. HTTP allows web browser operation, and MQTT enables lightweight messaging.

In this article, we’ll run a Telnet server on the Pico board so you can execute pico-jxgLABO shell commands remotely. With terminal software like Tera Term, you can operate the Pico board without a USB cable.

In the photo below, multiple Pico W and Pico 2 W boards are connected to a Wi-Fi network, each with a fixed IP address.

![wifi_experiment](images/wifi-experiment.jpg)

You can operate each board without physical cable connections, allowing more flexible operation!

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

## Trying Out Network Commands

### Running Network Commands

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

### Running a Telnet Server on the Pico Board for Remote Commands {#telnet-server}

Start a Telnet server on the Pico board with the `telnet-server` command:

```text
L:/>telnet-server start
Telnet server started on port 23
```

Try connecting to the Pico board with Tera Term. From the menu, select `File` - `New Connection...` to open the connection dialog:

![teraterm-connect](images/teraterm-connect.png)

Select `TCP/IP`, enter the Pico board’s IP address as the host, select `Telnet` as the service, and click OK. A new Tera Term window will open and connect to the board. You’ll be prompted for a password; just press Enter if you haven’t set one yet.

```text
password:
L:/>
```

Now you can run all pico-jxgLABO commands remotely! You can even use [logic analyzer features](https://zenn.dev/ypsit/articles/2025-09-08-labo-la) remotely. For more, see the article below:

▶️ [Pico Board as a Lab! Try Logic Analyzer Features with pico-jxgLABO](https://zenn.dev/ypsit/articles/2025-08-01-labo-intro)

Note: When connected via Telnet, USB serial command input is disabled. Disconnecting Telnet restores USB serial communication.



### Setting a Password

It’s not secure to run a Telnet server on the network without a password, so set one. (Note: Telnet transmits in plain text, so it’s not fully secure, but at least prevents unauthorized logins.)

Set a password with the `password` command:

```text
L:/>password
New password:
Reenter password:
password changed
```

This creates a `.password` file in the root of the `L:` drive, storing a hashed password.

Password entry is only required for Telnet connections, not USB serial.

### Setting Up Auto-Start

Create a file named `.startup` in the root of the `L:` drive to run commands automatically when the Pico board powers on. Here’s an example to automate Wi-Fi connection and Telnet server startup:

```text:.startup
net wifi-connect {ssid:'SSID' password:'PASSWORD'} config {addr:XXX.XXX.XXX.XXX}
telnet-server start
led on flip:50,500,50,500,50,2000
```

When you connect the Pico board running pico-jxgLABO via USB, the `L:` drive appears as a USB drive on your PC (often as `D:` on Windows). Edit the file above, change `SSID`, `PASSWORD`, and `XXX.XXX.XXX.XXX` as needed, and copy it to the drive.

After rebooting, if all commands in the script run without error, the `led` command will blink the built-in LED to indicate successful startup. You can set different blink patterns for each board’s IP address for easy identification (e.g., one blink for 192.168.0.101, two for 192.168.0.102, etc.).
