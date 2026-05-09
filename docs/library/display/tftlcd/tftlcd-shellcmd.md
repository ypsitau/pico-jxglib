# Create Display in Program

## Building and Flashing the Program

Let's connect a TFT LCD and perform drawing operations.

Here, we will create a sample program that implements a custom command named `argtest`. It displays the contents of the arguments passed to it.

Create a new Pico SDK project named `tftlcd-shellcmd`.

{% include-markdown "include/create-open-project.md" %}

Clone the pico-jxglib repository from GitHub so the direcory structure looks like this:

```text
├── pico-jxglib/
└── tftlcd-shellcmd/
    ├── CMakeLists.txt
    ├── tftlcd-shellcmd.cpp
    └── ...
```

{% include-markdown "include/clone-repository.md" %}

The breadboard wiring image is as follows:

![circuit-st7789.png](images/circuit-st7789.png)

Add the following lines to the end of `CMakeLists.txt`:

```cmake title="CMakeLists.txt" linenums="1"
{% include "./sample/tftlcd-shellcmd/CMakeLists.txt" start="# mkdocs-start" end="# mkdocs-end" %}
```

Edit the source file as follows:

```cpp title="tftlcd-shellcmd.cpp" linenums="1"
{% include "./sample/tftlcd-shellcmd/tftlcd-shellcmd.cpp" %}
```
