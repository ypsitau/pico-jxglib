# Startup File for Logic Analyzer

If your firmware is configured to have a file storage on the flash memory of the Pico board just like pico-jxgLABO does, you can create a `.startup` file in the root directory of the primary drive (e.g., `L:`) to automatically execute commands at startup. This is useful for configuring the logic analyzer settings without needing to manually enter commands each time.

For example, to set GPIO pins 2 to 22 and 26 to 28 as external signal measurement targets at startup, create a `.startup` file with the following content:

```text title=".startup"
la -p 2-22,26-28 --target:external
```
