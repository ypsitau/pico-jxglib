# Shell

{% include-markdown "shell/index.md" start="<!-- mkdocs-start:abstract -->" end="<!-- mkdocs-end:abstract -->" %}

## How Shell Works

The following code is the minimal code to run the shell:

```cpp title="shell-with-stdio.cpp" linenums="1"
{% include "./sample/shell-with-stdio/shell-with-stdio.cpp" %}
```

It runs the shell with `stdio` as the serial interface. Even though there is no code to register any commands, you can still use various commands such as file system commands, logic analyzer commands, and so on. Where do they come from?

Hard coded in the shell itself? No. The shell is designed to be flexible and extensible, and the commands are implemented as extensions. You can add commands by linking libraries in `CMakeLists.txt` that implement the commands.

For example, the file system commands are implemented in the `jxglib_ShellCmd_FS` library. By linking this library, you can use the file system commands in the shell.

```cpp title="CMakeLists.txt" linenums="1"
target_link_libraries(shell-with-stdio PRIVATE
    jxglib_ShellCmd_FS)
```

pico-jxglib provides many libraries that implement various commands as listed [here](shell-commands.md). You can link any number of command libraries as long as there is enough room in the flash memory and RAM.

```cpp title="CMakeLists.txt" linenums="1"
target_link_libraries(shell-with-stdio PRIVATE
    jxglib_ShellCmd_FS
    jxglib_ShellCmd_LogicAnalyzer
    jxglib_ShellCmd_NetUtil)
```

In these libraries, commands are implemeted by `ShellCmd` macro, which automatically registers the commands when the library is linked. This design allows for easy embedding and removal of commands without the need for explicit registration. [:octicons-arrow-right-24: Learn More](custom-commands.md)
