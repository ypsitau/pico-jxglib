# Library APIs

## Development Environment

The library APIs of pico-jxglib are developed with Pico SDK. Please see [Getting Started with Pico SDK](../development/pico-sdk/) for setting up the development environment. It also gives you a quick introduction to Visual Studio Code (VSCode), which is used throughout the documentation here.

Does it look complicated? Don't worry! Installing Pico SDK is much easier than you think. Just follow the instructions in the link above, and you'll be ready to use pico-jxglib in no time!

## How to Add pico-jxglib to Your Project

First, you need to create a new Pico project. From the VSCode command palette, run `>Raspberry Pi Pico: New Pico Project` and create a project with the following settings:

- **Name** ... Enter the project name. Here, we use `your-project`.
- **Board type** ... Select your board type.
- **Location** ... Select the parent directory where the project directory will be created.
- **Code generation options** ... **Check `Generate C++ code`**

The created project directory will look like this:

```text
└── your-project/
    ├── CMakeLists.txt
    ├── your-project.cpp
    └── ...
```

You can get the latest pico-jxglib from GitHub as follows:

```bash
$ git clone https://github.com/ypsitau/pico-jxglib.git
$ cd pico-jxglib
$ git submodule update --init --recursive
```

!!! note
    pico-jxglib is updated almost daily. If you've already cloned it, run the following command in the `pico-jxglib` directory to get the latest version:

    ```bash
    git pull
    ```

But, wait. Where should we put the `pico-jxglib` directory? There are two ways of arranging the directories of your project and pico-jxglib: 

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
    
    ```cmake title="CMakeLists.txt" linenums="1"
    target_link_libraries(your-project jxglib_AAAAAA jxglib_BBBBBB jxglib_CCCCCC)
    add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/pico-jxglib)
    ```

    This style is useful when you want to include pico-jxglib in your project. It specifically works well with Git's submodule feature, allowing you to manage pico-jxglib as part of your project repository.

***Outside Style***
    
:    Add this command to your `CMakeLists.txt`:
    
    ```cmake title="CMakeLists.txt" linenums="1"
    target_link_libraries(your-project jxglib_AAAAAA jxglib_BBBBBB jxglib_CCCCCC)
    add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/../pico-jxglib pico-jxglib)
    ```
    
    Second argument `pico-jxglib` is required in `add_subdirectory()` because the added directory contains a reference to a parent directory. The string in the second argument is used as the output directory name for generated files. As long as it doesn't conflict with other directories in `build`, you can use any name.

    This style is useful when you want to share pico-jxglib across multiple projects.

## Example Projects

There are two example projects that demonstrate how to add pico-jxglib to your project and use its APIs:

- ***Blinky Project*** ... The simplest project that blinks an LED connected to GPIO15.
- ***LABOPlatform Project*** ... A more complicated project that has the same functionality as pico-jxgLABO, including the interactive shell and built-in logic analyzer.

Modify the `CMakeLists.txt` and `your-project.cpp` files as described in the section above.

=== "Blinky Project"

    Let's create the simplest project that blinks an LED connected to GPIO15.

    Add the following lines to the end of `CMakeLists.txt`:

    ```cmake title="CMakeLists.txt" linenums="1"
    target_link_libraries(your-project jxglib_Common)
    add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/../pico-jxglib pico-jxglib)
    ```

    Edit the source file as follows:

    ```cpp title="your-project.cpp" linenums="1"
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

=== "LABOPlatform Project"

    Let's create a more complicated project that has the same functionality as pico-jxgLABO, including the interactive shell and built-in logic analyzer.

    Add the following lines to the end of `CMakeLists.txt`:

    ```cmake title="CMakeLists.txt" linenums="1"
    target_link_libraries(your-project jxglib_LABOPlatform_FullCmd)
    add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/../pico-jxglib pico-jxglib)
    jxglib_configure_LABOPlatform(your-project)
    ```

    Edit the source file as follows:

    ```cpp title="your-project.cpp" linenums="1"
    #include "pico/stdlib.h"
    #include "jxglib/LABOPlatform.h"

    using namespace jxglib;

    int main(void)
    {
        ::stdio_init_all();
        LABOPlatform::Instance.Initialize();
        for (;;) {
            // Your code here
            Tickable::Tick();
        }
    }
    ```
