# PIO

## Directives

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

## Instructions

All instructions can be used with the following modifiers:

```cpp
.side(bits)
```

```cpp
.delay(cycles)
```

!!! abstract ".jmp"

    ```cpp
    .jmp(addr or "label")
    ```

    ```cpp
    .jmp("!x", addr or "label")
    ```

    ```cpp
    .jmp("x--", addr or "label")
    ```

    ```cpp
    .jmp("!y", addr or "label")
    ```

    ```cpp
    .jmp("y--", addr or "label")
    ```

    ```cpp
    .jmp("x!=y", addr or "label")
    ```

    ```cpp
    .jmp("pin", addr or "label")
    ```

    ```cpp
    .jmp("!osre", addr or "label")
    ```

!!! abstract ".wait"

    ```cpp
    .wait(0 or 1, "gpio", index)
    ```

    ```cpp
    .wait(0 or 1, "pin", index)
    ```

    ```cpp
    .wait(0 or 1, "irq", index)
    ```

    ```cpp
    .wait(0 or 1, "jmppin", index)
    ```


!!! abstract ".in"

    ```cpp
    .in("pins", count)
    ```

    ```cpp
    .in("x", count)
    ```

    ```cpp
    .in("y", count)
    ```

    ```cpp
    .in("null", count)
    ```

    ```cpp
    .in("isr", count)
    ```

    ```cpp
    .in("osr", count)
    ```

!!! abstract ".out"

    ```cpp
    .out("pins", count)
    ```

    ```cpp
    .out("x", count)
    ```

    ```cpp
    .out("y", count)
    ```

    ```cpp
    .out("null", count)
    ```

    ```cpp
    .out("pindirs", count)
    ```

    ```cpp
    .out("pc", count)
    ```

    ```cpp
    .out("isr", count)
    ```

    ```cpp
    .out("osr", count)
    ```

!!! abstract ".push"

    ```cpp
    .push()
    ```

    `.push()` instruction can be used with the following modifiers:

    ```cpp
    .iffull()
    ```

    ```cpp
    .block()
    ```

    ```cpp
    .noblock()
    ```

!!! abstract ".pull"

    ```cpp
    .pull()
    ```

    `.pull()` instruction can be used with the following modifiers:

    ```cpp
    .ifempty()
    ```

    ```cpp
    .block()
    ```

    ```cpp
    .noblock()
    ```

!!! abstract ".mov"

    ```cpp
    .mov("dest", "src")
    ```

    ```cpp
    .mov("osr", "rxfifo[]", index)
    ```

    ```cpp
    .mov("osr", "rxfifo[index]")
    ```

    ```cpp
    .mov("osr", "rxfifo[y]")
    ```

    ```cpp
    .mov("rxfifo[]", index, "isr")
    ```

    ```cpp
    .mov("rxfifo[index]", "isr")
    ```

    ```cpp
    .mov("rxfifo[y]", "isr")
    ```


!!! abstract ".irq"

    ```cpp
    .irq("set", irq_n);
    ```

    ```cpp
    .irq("wait", irq_n);
    ```

    ```cpp
    .irq("clear", irq_n);
    ```

    ```cpp
    .irq(irq_n)
    ```

!!! abstract ".set"

    ```cpp
    .set("pins", value)
    ```

    ```cpp
    .set("x", value)
    ```

    ```cpp
    .set("y", value)
    ```

    ```cpp
    .set("pindirs", value)
    ```
