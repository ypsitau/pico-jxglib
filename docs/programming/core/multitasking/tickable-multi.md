## Running Multiple Tasks

Create a new Pico SDK project named `tickable-multi`.

{% include-markdown "include/create-open-project.md" %}

Clone the pico-jxglib repository from GitHub so the direcory structure looks like this:

```text
├── pico-jxglib/
└── tickable-multi/
    ├── CMakeLists.txt
    ├── tickable-multi.cpp
    └── ...
```

{% include-markdown "include/clone-repository.md" %}

Add the following lines to the end of `CMakeLists.txt`:

```cmake title="CMakeLists.txt" linenums="1"
{% include "./sample/tickable-multi/CMakeLists.txt" start="# mkdocs-start" end="# mkdocs-end" %}
```

Enable UART or USB stdio as described below.

{% include-markdown "include/enable-stdio.md" %}

Edit `tickable-multi.cpp` as follows:


```cpp title="tickable-multi.cpp" linenums="1" hl_lines="8-12 14-18 20-24"
{% include "./sample/tickable-multi/tickable-multi.cpp" %}
```
