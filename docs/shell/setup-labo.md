# Setting Up pico-jxgLABO

The shell is implemented in the `jxglib_Shell` library. Linking this library or other libraries that depend on it, such as `jxglib_LABOPlatform`, will make the shell available in your firmware.

<div class="grid" markdown>

Here, we will use pico-jxgLABO, a ready-to-flash UF2 Binary that links `jxglib_LABOPlatform`, as an example to demonstrate how to use the shell and its commands. It uses the USB serial interface for the shell, so you can easily try it with a single USB cable.

![shell-operation](images/shell-operation.png){:width="300px"}

</div>

Below is a list of pico-jxgLABO UF2 files:

{% include-markdown "include/uf2-list.md" %}

These UF2 files are pre-compiled with the latest version of pico-jxglib and can be flashed to your Pico board using the standard UF2 flashing method.

{% include-markdown "include/flash-uf2.md" %}

Go to [this page](../programming/shell/index.md) if you are interested in the details of how to build your own firmware with the shell.

After flashing the pico-jxgLABO, establish a serial communication with a terminal program such as Tera Term on Windows.

{% include-markdown "include/setup-terminal-for-laboplatform.md" %}
