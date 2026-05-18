# Directives

Directives are used to control the assembly process of a PIO program. They do not correspond to any machine code, but they can affect the generated machine code in various ways, such as defining labels, setting the origin of the program, or specifying the PIO version.

Below is a list of directives available in the method-chain PIO assember.

!!! abstract ".program"

    ```cpp
    .program("name")
    ```

    Defines a PIO program with the given name.

!!! abstract ".end"

    ```cpp
    .end()
    ```

    Marks the end of the PIO program. This directive does finalization work such as resolving labels and calculating the size of the program.

!!! abstract ".L"

    ```cpp
    .L("label")
    ```

    Defines a local label that can be used for JMP instructions.

!!! abstract ".pub"

    ```cpp
    uint relAddr;
    .pub(&relAddr)
    ```

    Retrives the relative address of the current instruction and stores it in `relAddr`. This can be used to implement position-independent code.

!!! abstract ".entry"

    ```cpp
    .entry()
    ```

    Marks the current relative address as the entry point of the program.

!!! abstract ".origin"

    ```cpp
    .origin(offset)
    ```

!!! abstract ".pio_version"

    ```cpp
    .pio_version(version)
    ```

    Specifies the PIO version to target. This infomration is used to check if the instructions used in the program are compatible with the specified PIO version.

!!! abstract ".side_set"

    ```cpp
    .side_set(bit_count)
    ```

    `.side_set()` directive can be used with the following modifiers:

    - `.opt()`: Makes `.side()` modifier optional. If not specified, all the instruction must have the `.side()` modifier.
    - `.pindirs()`: The value specified by `.side()` modifier sets the direction of the pin, not the value on the pin.

!!! abstract ".wrap_target"

    ```cpp
    .wrap_target()
    ```

    Marks the beginning of a wrap block. A wrap block is a section of code that will be repeated indefinitely. The end of the wrap block is marked by the `.wrap()` directive.

!!! abstract ".wrap"

    ```cpp
    .wrap()
    ```

    Marks the end of a wrap block. The code between `.wrap_target()` and `.wrap()` will be repeated indefinitely.
