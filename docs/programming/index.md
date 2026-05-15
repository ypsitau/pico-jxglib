# Programming with pico-jxglib

pico-jxglib is a set of C/C++ libraries that works along with the Pico SDK.

Below is a quick introduction to set up the Pico SDK:

{% include-markdown "include/quick-setup-picosdk.md" %}

## Integration with Pico SDK

Making pico-jxglib available in your project is simple: just clone the pico-jxglib repository from GitHub and add a few lines to your `CMakeLists.txt`.

If you don't have a Pico SDK project yet, create a new one as described below:

{% include-markdown "include/create-open-project.md" %}

When you create a new project with the name `your-project`, the project directory will look like this:

```text
└── your-project/
    ├── CMakeLists.txt
    ├── your-project.cpp
    └── ...
```

Then, clone the pico-jxglib repository from GitHub as follows:

{% include-markdown "include/clone-repository.md" %}

But, wait. Where should we put the `pico-jxglib` directory? There are two ways of arranging the directories of your project and pico-jxglib:

!!! abstract "Inside Style"
    `pico-jxglib` is placed inside `your-project` directory like this:
    ```text hl_lines="2"
    └── your-project/
        ├── pico-jxglib/
        ├── CMakeLists.txt
        ├── your-project.cpp
        └── ...
    ```

    In this style, add the following command to your `CMakeLists.txt`:

    ```cmake
    add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/pico-jxglib)
    ```

    This style works well with Git's submodule feature, allowing you to manage pico-jxglib as part of your project repository.

!!! abstract "Outside Style"
    `pico-jxglib` is placed in the same directory as `your-project` like this:
    ```text hl_lines="1"
    ├── pico-jxglib/
    └── your-project/
        ├── CMakeLists.txt
        ├── your-project.cpp
        └── ...
    ```

    In this style, add the following command to your `CMakeLists.txt`:

    ```cmake
    add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/../pico-jxglib pico-jxglib)
    ```

    Second argument `pico-jxglib` is required in `add_subdirectory()` because the added directory contains a reference to a parent directory. The string in the second argument is used as the output directory name for generated files. As long as it doesn't conflict with other directories in `build`, you can use any name.

    This style is useful when you want to share pico-jxglib across multiple projects like follows:
    ```text hl_lines="1"
    ├── pico-jxglib/
    ├── your-project/
    │   ├── CMakeLists.txt
    │   ├── your-project.cpp
    │   └── ...
    ├── your-another-project/
    │   ├── CMakeLists.txt
    │   ├── your-another-project.cpp
    │   └── ...
    └── your-yet-another-project/
        ├── CMakeLists.txt
        ├── your-yet-another-project.cpp
        └── ...
    ```

## Building Programs with pico-jxglib

Using the project created above, let's create actual programs that use pico-jxglib. There are two sample programs:

- ***Blinky Program*** ... The simplest program that blinks an LED connected to a GPIO.
- ***LABOPlatform Program*** ... A more complicated program that has the same functionality as pico-jxgLABO, including the interactive shell and built-in logic analyzer.

Modify the `CMakeLists.txt` and `your-project.cpp` files as described below. It is assumed that `pico-jxglib` is placed in the same directory as `your-project` (outside style).

=== "Blinky Program"

    Add the following lines to the end of `CMakeLists.txt`:

    ```cmake title="CMakeLists.txt" linenums="1"
    {% include "./sample/your-project-blinky/CMakeLists.txt" start="# mkdocs-start" end="# mkdocs-end" %}
    ```

    Edit `your-project.cpp` as follows:

    ```cpp title="your-project.cpp" linenums="1"
    {% include "./sample/your-project-blinky/your-project.cpp" %}
    ```

=== "LABOPlatform Program"

    Add the following lines to the end of `CMakeLists.txt`:

    ```cmake title="CMakeLists.txt" linenums="1"
    {% include "./labo-platform/sample/your-project-LABOPlatform/CMakeLists.txt" start="# mkdocs-start" end="# mkdocs-end" %}
    ```

    Edit `your-project.cpp` as follows:

    ```cpp title="your-project.cpp" linenums="1"
    {% include "./labo-platform/sample/your-project-LABOPlatform/your-project.cpp" %}
    ```
