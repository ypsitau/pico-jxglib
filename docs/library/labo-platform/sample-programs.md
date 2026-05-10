# Sample Programs

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
    {% include "./sample/your-project-LABOPlatform/CMakeLists.txt" start="# mkdocs-start" end="# mkdocs-end" %}
    ```

    Edit `your-project.cpp` as follows:

    ```cpp title="your-project.cpp" linenums="1"
    {% include "./sample/your-project-LABOPlatform/your-project.cpp" %}
    ```
