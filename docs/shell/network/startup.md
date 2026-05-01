# Startup File for Network

If your firmware is configured to have a file storage on the flash memory of the Pico board just like pico-jxgLABO does, you can create a `.startup` file in the root directory of the primary drive (e.g., `L:`) to automatically execute commands at startup. This is useful for configuring the network settings without needing to manually enter commands each time.

Here’s an example to establish Wi-Fi connection and start a Telnet server at startup:

```text title=".startup" linenums="1"
net wifi-connect {ssid:'SSID' password:'PASSWORD'} config {addr:XXX.XXX.XXX.XXX}
telnet-server start
led on flip:50,500,50,500,50,2000
```

Edit the file above, change `SSID`, `PASSWORD`, and `XXX.XXX.XXX.XXX` as needed, and copy it to the drive.

After rebooting, if all commands in the script run without error, the `led` command will blink the built-in LED to indicate successful startup. You can set different blink patterns for each board’s IP address for easy identification (e.g., one blink for 192.168.0.101, two for 192.168.0.102, etc.).
