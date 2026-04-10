# Library APIs

## How to Add pico-jxglib to Your Project

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

You add this directory to your project with the `add_subdirectory()` command in `CMakeLists.txt`. There are two ways of arranging the directories of your project and pico-jxglib: 

<div class="grid" markdown>
```text hl_lines="2" title="Inside Style"
└── my-project/
    ├── pico-jxglib/
    ├── CMakeLists.txt
    ├── my-project.cpp
    └── ...
```

```text hl_lines="1" title="Outside Style"
├── pico-jxglib/
└── my-project/
    ├── CMakeLists.txt
    ├── my-project.cpp
    └── ...
```
</div>

So you can choose either of the following styles to add pico-jxglib to your project:

***Inside Style***
   
:    Add this command to your `CMakeLists.txt`:
    
    ```cmake title="CMakeLists.txt"
    target_link_libraries(my-project jxglib_AAAAAA jxglib_BBBBBB jxglib_CCCCCC)
    add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/pico-jxglib)
    ```

    This style is useful when you want to include pico-jxglib in your project. It specifically works well with Git's submodule feature, allowing you to manage pico-jxglib as part of your project repository.

***Outside Style***
    
:    Add this command to your `CMakeLists.txt`:
    
    ```cmake title="CMakeLists.txt"
    target_link_libraries(my-project jxglib_AAAAAA jxglib_BBBBBB jxglib_CCCCCC)
    add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/../pico-jxglib pico-jxglib)
    ```
    
    Second argument `pico-jxglib` is required in `add_subdirectory()` because the added directory contains a reference to a parent directory. The string in the second argument is used as the output directory name for generated files. As long as it doesn't conflict with other directories in `build`, you can use any name.

    This style is useful when you want to share pico-jxglib across multiple projects.

## Example Projects

There are two example projects that demonstrate how to add pico-jxglib to your project and use its APIs:

- ***Blinky Project*** ... The simplest project that blinks an LED connected to GPIO15.
- ***LABOPlatform Project*** ... A more complicated project that has the same functionality as pico-jxgLABO, including the interactive shell and built-in logic analyzer.

=== "Blinky Project"

    Let's create the simplest project that blinks an LED connected to GPIO15.

    From the VSCode command palette, run `>Raspberry Pi Pico: New Pico Project` and create a project with the following settings:

    - **Name** ... Enter the project name. For this example, use `my-project`.
    - **Board type** ... Select your board type.
    - **Location** ... Select the parent directory where the project directory will be created.
    - **Code generation options** ... **Check `Generate C++ code`**

    Assume your project directory and pico-jxglib are arranged in ***Outside Style*** as follows:

    ```text hl_lines="1"
    ├── pico-jxglib/
    └── my-project/
        ├── CMakeLists.txt
        ├── my-project.cpp
        └── ...
    ```

    Add the following lines to the end of `CMakeLists.txt`:

    ```cmake title="CMakeLists.txt"
    target_link_libraries(my-project jxglib_Common)
    add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/../pico-jxglib pico-jxglib)
    ```

    Edit the source file as follows:

    ```cpp title="my-project.cpp"
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

    From the VSCode command palette, run `>Raspberry Pi Pico: New Pico Project` and create a project with the following settings:

    - **Name** ... Enter the project name. For this example, use `my-project`.
    - **Board type** ... Select your board type.
    - **Location** ... Select the parent directory where the project directory will be created.
    - **Code generation options** ... **Check `Generate C++ code`**

    Assume your project directory and pico-jxglib are arranged in ***Outside Style*** as follows:

    ```text hl_lines="1"
    ├── pico-jxglib/
    └── my-project/
        ├── CMakeLists.txt
        ├── my-project.cpp
        └── ...
    ```

    Add the following lines to the end of `CMakeLists.txt`:

    ```cmake title="CMakeLists.txt"
    target_link_libraries(my-project jxglib_LABOPlatform_FullCmd)
    add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/../pico-jxglib pico-jxglib)
    jxglib_configure_LABOPlatform(my-project)
    ```

    Edit the source file as follows:

    ```cpp title="my-project.cpp"
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
