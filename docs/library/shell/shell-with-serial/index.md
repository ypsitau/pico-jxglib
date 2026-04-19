# Shell with Stdio

## Building and Flashing the Program

Create a new Pico SDK project named `shell-with-stdio`.

{% include-markdown "include/new-project.md" %}

Clone the pico-jxglib repository from GitHub so the direcory structure looks like this:

```text
├── pico-jxglib/
└── shell-with-stdio/
    ├── CMakeLists.txt
    ├── shell-with-stdio.cpp
    └── ...
```

{% include-markdown "include/clone-repository.md" %}

Add the following lines to the end of `CMakeLists.txt`:

```cmake title="CMakeLists.txt"
{% include "sample/shell-with-stdio/CMakeLists.txt" start="# mkdocs-start" end="# mkdocs-end" %}
```

Enable UART or USB stdio as described below.

{% include-markdown "include/enable-stdio.md" %}

Edit `shell-with-stdio.cpp` as follows:

```cpp title="shell-with-stdio.cpp"
{% include "sample/shell-with-stdio/shell-with-stdio.cpp" %}
```

{% include-markdown "include/build-and-flash.md" %}

## Running the Program

Open a terminal emulator to connect it.

{% include-markdown "include/setup-terminal-for-stdio.md" %}

A prompt will appear in the terminal like this:

```text
>
```
