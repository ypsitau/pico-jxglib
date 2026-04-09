# Library APIs

## How to Add pico-jxglib to Your Project

You can get the latest pico-jxglib from GitHub as follows:

```bash
$ git clone https://github.com/ypsitau/pico-jxglib.git
$ cd pico-jxglib
$ git submodule update --init --recursive
```

You add this directory to your project with the `add_subdirectory()` command in `CMakeLists.txt`. There are two ways of arranging the directories of your project and pico-jxglib: 

<div class="grid" markdown>
```text hl_lines="2" title="Inside Style"
└── your-project/
    ├── pico-jxglib/
    ├── CMakeLists.txt
    ├── your-project.cpp
    └── ...
```

```text hl_lines="1" title="Outside Style"
├── pico-jxglib/
└── your-project/
    ├── CMakeLists.txt
    ├── your-project.cpp
    └── ...
```
</div>

So you can choose either of the following styles to add pico-jxglib to your project:

***Inside Style***
   
:    Add this command to your `CMakeLists.txt`:
    
    ```cmake title="CMakeLists.txt"
    add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/pico-jxglib)
    ```

    This style is useful when you want to include pico-jxglib in your project. It specifically works well with Git's submodule feature, allowing you to manage pico-jxglib as part of your project repository.

***Outside Style***
    
:    Add this command to your `CMakeLists.txt`:
    
    ```cmake title="CMakeLists.txt"

    add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/../pico-jxglib pico-jxglib)
    ```
    
    Second argument `pico-jxglib` is required in `add_subdirectory()` because the added directory contains a reference to a parent directory. The string in the second argument is used as the output directory name for generated files. As long as it doesn't conflict with other directories in `build`, you can use any name.

    This style is useful when you want to share pico-jxglib across multiple projects.

## Example Project

Let's make a simple blink program using pico-jxglib.

From the VSCode command palette, run `>Raspberry Pi Pico: New Pico Project` and create a project with the following settings:

- **Name** ... Enter the project name. For this example, use `blink`.
- **Board type** ... Select your board type.
- **Location** ... Select the parent directory where the project directory will be created.
- **Code generation options** ... **Check `Generate C++ code`**

Assume your project directory and pico-jxglib are arranged in ***Outside Style*** as follows:

```text hl_lines="1"
├── pico-jxglib/
└── blink/
    ├── CMakeLists.txt
    ├── blink.cpp
    └── ...
```

Add the following lines to the end of `CMakeLists.txt`:

```cmake title="CMakeLists.txt"
target_link_libraries(blink jxglib_Common)
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/../pico-jxglib pico-jxglib)
```

The order of `target_link_libraries()` and `add_subdirectory()` doesn't matter.

Edit the source file as follows:

```cpp title="blink.cpp"
#include "pico/stdlib.h"
#include "jxglib/Common.h"

using namespace jxglib;

int main()
{
    GPIO15.init().set_dir_OUT();
    while (true) {
        GPIO15.put(true);
        ::sleep_ms(500);
        GPIO15.put(false);
        ::sleep_ms(500);
    }
}
```

`using namespace jxglib;` is a "magic spell" to use pico-jxglib functions and classes in your code[^namespace].

[^namespace]: All pico-jxglib functions, classes, and global variables are defined in the `jxglib` namespace to avoid conflicts with other libraries. For example, `GPIO15` in this code is actually `jxglib::GPIO15`, but using `using namespace` lets you omit the prefix.

For how to build and write to the board, see ["Getting Started with Pico SDK"](../development/pico-sdk/index.md).
