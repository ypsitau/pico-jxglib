# Custom Commands

To create a shell command, use the `ShellCmd` macro. The macro format is as follows:

```cpp
ShellCmd(name, "comment")
{
    // any code
    return Result::Success;
}
```

The following variables are passed to the command program:

- `Printable& tout` ... `Printable` instance. Used to display results
- `Printable& terr` ... `Printable` instance. Used to display results
- `int argc` ... Number of arguments
- `char** argv` ... Argument strings. `argv[0]` contains the command name itself

Return `Result::Success` if there is no error, or `Result::Error` if an error occurs.

You do **not** need to register commands. When you create a command with the `ShellCmd` macro, it is automatically registered with the shell. With this mechanism, you can add commands simply by linking the source file that implements the command to the main program.

Below is a sample program that displays the contents of the arguments passed. If you add this code to `shell-test.cpp` above, or write it in a separate source file and add that file to `add_executable()`, you can use it as a command.

```cpp
ShellCmd(argtest, "tests command line arguments")
{
    for (int i = 0; i < argc; i++) {
        tout.Printf("argv[%d] \"%s\"\n", i, argv[i]);
    }
    return Result::Success;
}
```
