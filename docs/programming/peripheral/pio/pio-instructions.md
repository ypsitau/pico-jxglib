# Instructions

Instructions are the basic building blocks of a PIO program. Each instruction corresponds to a single machine code that can be executed by the PIO state machine.

All instructions can be used with the following modifiers:

!!! abstract ".side"

    ```cpp
    .side(bits)
    ```

    Sets the value of side-set pins that start from the GPIO base pin specified by `StateMachine::set_sideset_pins()` or `StateMachine::config_set_sideset_pins()`. This modifier is only valid when the `.side_set()` directive is used. If the `.side_set()` directive comes with the `.pindirs()` modifier, the value specified by this modifier sets the direction of the pin, not the value on the pin.

!!! abstract ".delay"

    ```cpp
    .delay(cycles)
    ```

    An instruction is executed in a single cycle. When the `.delay()` modifier is used, the specified `cycles` are added to the execution time of the instruction.

Belos is a list of instructions available in the method-chain PIO assembler.

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

    Jump to the specified address or label if the output shift register (`osr`) is not empty.

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

    The `count` bits from the GPIO pins specified by `StateMachine::set_in_pins()` or `StateMachine::config_set_in_pins()` are shifted into the `isr`.

    ```cpp
    .in("x", count)
    ```

    The least significant `count` bits of the `x` register are shifted into the `isr`.

    ```cpp
    .in("y", count)
    ```

    The least significant `count` bits of the `y` register are shifted into the `isr`.

    ```cpp
    .in("null", count)
    ```

    The `count` bits of zeros are shifted into the `isr`.

    ```cpp
    .in("isr", count)
    ```

    The least significant `count` bits of the `isr` are shifted into the `isr`.

    ```cpp
    .in("osr", count)
    ```

    The least significant `count` bits of the `osr` are shifted into the `isr`.


!!! abstract ".out"

    ```cpp
    .out("pins", count)
    ```

    The `count` bits from the `osr` are shifted out to the GPIO pins specified by `StateMachine::set_out_pins()` or `StateMachine::config_set_out_pins()`.

    ```cpp
    .out("x", count)
    ```

    The `count` bits from the `osr` are shifted into the least significant bits of the `x` register. The most significant bits of the `x` register are filled with zeros.

    ```cpp
    .out("y", count)
    ```

    The `count` bits from the `osr` are shifted into the least significant bits of the `y` register. The most significant bits of the `y` register are filled with zeros.

    ```cpp
    .out("null", count)
    ```

    The value of the `osr` is shifted by `count` bits.

    ```cpp
    .out("pindirs", count)
    ```

    The `count` bits from the `osr` are shifted out to the pin direction control of the GPIO pins specified by `StateMachine::set_out_pins()` or `StateMachine::config_set_out_pins()`.

    ```cpp
    .out("pc", count)
    ```

    The `count` bits from the `osr` are shifted into the least significant bits of the PC register. The most significant bits of the PC register are filled with zeros.

    ```cpp
    .out("isr", count)
    ```

    The `count` bits from the `osr` are shifted into the least significant bits of the `isr`. The most significant bits of the `isr` are filled with zeros.

    ```cpp
    .out("osr", count)
    ```

    The `count` bits from the `osr` are shifted into the least significant bits of the `osr`. The most significant bits of the `osr` are filled with zeros.

!!! abstract ".push"

    ```cpp
    .push()
    ```

    `.push()` instruction can be used with the following modifiers:

    - `.iffull()`: Pushes the `isr` value to the RX FIFO only when `isr`'s shift amount reaches the specified value.
    - `.block()`: Blocks the execution until the RX FIFO is ready to accept data.
    - `.noblock()`: Does not block the execution, even if the RX FIFO is not ready.

!!! abstract ".pull"

    ```cpp
    .pull()
    ```

    `.pull()` instruction can be used with the following modifiers:

    - `.ifempty()`: Pulls data from the TX FIFO to the `osr` only when the `osr`'s shift amount is zero.
    - `.block()`: Blocks the execution until there is data in the TX FIFO.
    - `.noblock()`: Does not block the execution, and sets the `osr` with the value of the X when there is no data in the TX FIFO.

!!! abstract ".mov"

    ```cpp
    .mov("x", "src")
    ```

    Move data from `src` to the `x` register.

    ```cpp
    .mov("y", "src")
    ```

    Move data from `src` to the `y` register.

    ```cpp
    .mov("exec", "src")
    ```
   

    ```cpp
    .mov("pc", "src")
    ```

    ```cpp
    .mov("isr", "src")
    ```

    ```cpp
    .mov("osr", "src")
    ```

    The `src` can be `pins`, `x`, `y`, `null`, `status`, `isr`, or `osr`. It can also be applied with the following operators:

    - `!src`: Bitwise complement of the value of `src` is moved to `dst`.
    - `~src`: Same as `!src`.
    - `::src`: Bit revese of the value of `src` is moved to `dst`. For example, if the value of `src` is `0b00010011'00000000'10101010'00111100`, the value moved to `dst` will be `0b00111100'01010101'00000000'11001000`.

    If the PIO version is 2.0 or above, the following variants of the `.mov` instruction are also available for moving data between the shift registers and the FIFOs.

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

    Write the specified value to the destination. The size of value is 5 bits, so the valid range of value is from 0 to 31.
