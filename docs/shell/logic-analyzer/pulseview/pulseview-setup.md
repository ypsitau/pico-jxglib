# Connect to Pico Board

Follow these steps to connect pico-jxgLABO and PulseView:

1. Connect the Pico board with pico-jxgLABO flashed to your PC using a USB cable.
3. In Tera Term, run the pico-jxgLABO logic analyzer command `la` and specify the GPIO pins to measure. In the example below, GPIO2, GPIO3, and GPIO4 are selected:
     ```text
     L:/>la -p 2,3,4
     disabled ---- 12.5MHz (samplers:1) pins:2-4 events:0/0 (heap-ratio:0.7)
     ```
4. Start PulseView. One of the following main screens will appear:

   ![pulseview-main](images/pulseview-main.png)

   ![pulseview-main-demo](images/pulseview-main-demo.png)

   Click the area labeled `<No Device>` or `Demo device` to open the "Connect to Device" dialog.
5. In `Step 1: Choose the driver`, select `RaspberryPI PICO (raspberrypi-pico)` from the dropdown list.

   ![pulseview-connect-1](images/pulseview-connect-1.png)
6. In `Step 2: Choose the interface`, select `Serial Port` and specify the application serial port noted in step 2. Leave the baud rate blank.

   ![pulseview-connect-2](images/pulseview-connect-2.png)
7. In `Step 3: Scan for devices`, click the `Scan for devices using driver above` button. In the list for `Step 4: Select the device`, confirm that `RaspberryPi PICO with 3 channels` appears and click `OK`.

   ![pulseview-connect-3](images/pulseview-connect-3.png)
8. The main screen will look like this:

   ![pulseview-main-connected](images/pulseview-main-connected.png)
