# Directives

Directives are used to control the assembly process of a PIO program. They do not correspond to any machine code, but they can affect the generated machine code in various ways, such as defining labels, setting the origin of the program, or specifying the PIO version.

Below is a list of directives available in the method-chain PIO assember.

!!! abstract ".program"

    ```cpp
    .program("name")
    ```

!!! abstract ".end"

    ```cpp
    .end()
    ```

    ```cpp
    .end(&relAddr)
    ```

!!! abstract ".L"

    ```cpp
    .L("label")
    ```

!!! abstract ".pub"

    ```cpp
    uint relAddr;
    .pub(&relAddr)
    ```

!!! abstract ".entry"

    ```cpp
    .entry()
    ```

!!! abstract ".origin"

    ```cpp
    .origin(offset)
    ```

!!! abstract ".pio_version"

    ```cpp
    .pio_version(version)
    ```

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

    ```cpp
    .wrap_target(&relAddr)
    ```

!!! abstract ".wrap"

    ```cpp
    .wrap()
    ```

    ```cpp
    .wrap(&relAddr)
    ```
