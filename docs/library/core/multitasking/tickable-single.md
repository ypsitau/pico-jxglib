# Running a Single Task

Create a new Pico SDK project named `tickable-single`.

{% include-markdown "include/create-open-project.md" %}

Clone the pico-jxglib repository from GitHub so the direcory structure looks like this:

```text
├── pico-jxglib/
└── tickable-single/
    ├── CMakeLists.txt
    ├── tickable-single.cpp
    └── ...
```

{% include-markdown "include/clone-repository.md" %}

Add the following lines to the end of `CMakeLists.txt`:

```cmake title="CMakeLists.txt" linenums="1"
{% include "./sample/tickable-single/CMakeLists.txt" start="# mkdocs-start" end="# mkdocs-end" %}
```

Enable UART or USB stdio as described below.

{% include-markdown "include/enable-stdio.md" %}

Edit `tickable-single.cpp` as follows:

```cpp title="tickable-single.cpp" linenums="1" hl_lines="8-12"
{% include "./sample/tickable-single/tickable-single.cpp" %}
```
