# Directives

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

    ```cpp
    .opt()
    ```

    ```cpp
    .pindirs()
    ```

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
