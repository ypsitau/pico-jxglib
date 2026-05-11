# Instructions

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

    Jump to the specified address or label.

    ```cpp
    .jmp("!x", addr or "label")
    ```

    Jump to the specified address or label if the value of register `x` is zero.


    ```cpp
    .jmp("x--", addr or "label")
    ```

    Jump to the specified address or label if the value of register `x` is not zero, and then decrement the value of register `x`.

    ```cpp
    .jmp("!y", addr or "label")
    ```

    Jump to the specified address or label if the value of register `y` is zero.

    ```cpp
    .jmp("y--", addr or "label")
    ```

    Jump to the specified address or label if the value of register `y` is not zero, and then decrement the value of register `y`.

    ```cpp
    .jmp("x!=y", addr or "label")
    ```

    Jump to the specified address or label if the value of register `x` is not equal to the value of register `y`.

    ```cpp
    .jmp("pin", addr or "label")
    ```

    Jump to the specified address or label if the GPIO pin specified by `StateMachine::set_jmp_pin()` is high.

    ```cpp
    .jmp("!osre", addr or "label")
    ```

    Jump to the specified address or label if the output shift register (OSR) is not empty.

!!! abstract ".wait"

    ```cpp
    .wait(0 or 1, "gpio", index)
    ```

    Wait until the GPIO pin specified by `index` is low (0) or high (1). The `index` is the absolute GPIO pin number.

    ```cpp
    .wait(0 or 1, "pin", index)
    ```

    Wait until the GPIO pin specified by `index` is low (0) or high (1). The `index` is the relative index from the GPIO base pin specified by `StateMachine::set_in_pins()`.

    ```cpp
    .wait(0 or 1, "irq", index)
    ```

    Wait until the IRQ specified by `index` is low (0) or high (1).

    ```cpp
    .wait(0 or 1, "jmppin")
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
