# Method-Chain Assembler

`PIO::Program` is a class that represents a PIO program. You can create an instance of `PIO::Program` and add instructions to it using instance functions that have similar names to pioasm's instructions and directives, such as `.mov()` and `.wrap_target()`. Now, let's call this feature **Method-Chain Assembler**.

Below is a skeleton code of a PIO program coded with the Method-Chain Assembler:

```cpp
PIO::Program program;
program
.program("program_name")
.end();
```

The method chain starts with the `.program()` function, which specifies the name of the program. You can add instructions and directives in the middle of the chain, and end it with the `.end()` function.

The PIO program coded with the Method-Chain Assembler has the similar appearance with the one coded in pioasm. See the differences between them in the following codes:

=== "Method-Chain Assembler"
    ```cpp linenums="1"
    {% include "./sample/pio-examples/pio-examples.cpp" start="// mkdocs-start:[blink]" end="// mkdocs-end:[blink]" %}
    ```

=== "pioasm"
    ```asm linenums="1"
    {% include "./sample/pio-examples/pio-examples.pio" start="// mkdocs-start:[blink]" end="// mkdocs-end:[blink]" %}
    ```

## Example Programs

Pico SDK provides a collection of PIO examples in its official repository. You can find them at the following link:

[https://github.com/raspberrypi/pico-examples/tree/master/pio](https://github.com/raspberrypi/pico-examples/tree/master/pio)

This section shows the same PIO programs as those examples, but coded in C++ using pico-jxglib's Method-Chain Assembler. You can compare the C++ code with the original pioasm code to see how they correspond to each other.

### Example: addition

=== "Method-Chain Assembler"
    ```cpp linenums="1"
    {% include "./sample/pio-examples/pio-examples.cpp" start="// mkdocs-start:[addition]" end="// mkdocs-end:[addition]" %}
    ```

=== "pioasm"
    ```asm linenums="1"
    {% include "./sample/pio-examples/pio-examples.pio" start="// mkdocs-start:[addition]" end="// mkdocs-end:[addition]" %}
    ```

### Example: apa102_mini

=== "Method-Chain Assembler"
    ```cpp linenums="1"
    {% include "./sample/pio-examples/pio-examples.cpp" start="// mkdocs-start:[apa102_mini]" end="// mkdocs-end:[apa102_mini]" %}
    ```

=== "pioasm"
    ```asm linenums="1"
    {% include "./sample/pio-examples/pio-examples.pio" start="// mkdocs-start:[apa102_mini]" end="// mkdocs-end:[apa102_mini]" %}
    ```

### Example: blink

=== "Method-Chain Assembler"
    ```cpp linenums="1"
    {% include "./sample/pio-examples/pio-examples.cpp" start="// mkdocs-start:[blink]" end="// mkdocs-end:[blink]" %}
    ```

=== "pioasm"
    ```asm linenums="1"
    {% include "./sample/pio-examples/pio-examples.pio" start="// mkdocs-start:[blink]" end="// mkdocs-end:[blink]" %}
    ```

### Example: clocked_input

=== "Method-Chain Assembler"
    ```cpp linenums="1"
    {% include "./sample/pio-examples/pio-examples.cpp" start="// mkdocs-start:[clocked_input]" end="// mkdocs-end:[clocked_input]" %}
    ```

=== "pioasm"
    ```asm linenums="1"
    {% include "./sample/pio-examples/pio-examples.pio" start="// mkdocs-start:[clocked_input]" end="// mkdocs-end:[clocked_input]" %}
    ```

### Example: differential_manchester_tx

=== "Method-Chain Assembler"
    ```cpp linenums="1"
    {% include "./sample/pio-examples/pio-examples.cpp" start="// mkdocs-start:[differential_manchester_tx]" end="// mkdocs-end:[differential_manchester_tx]" %}
    ```

=== "pioasm"
    ```asm linenums="1"
    {% include "./sample/pio-examples/pio-examples.pio" start="// mkdocs-start:[differential_manchester_tx]" end="// mkdocs-end:[differential_manchester_tx]" %}
    ```

### Example: i2c

=== "Method-Chain Assembler"
    ```cpp linenums="1"
    {% include "./sample/pio-examples/pio-examples.cpp" start="// mkdocs-start:[i2c]" end="// mkdocs-end:[i2c]" %}
    ```

=== "pioasm"
    ```asm linenums="1"
    {% include "./sample/pio-examples/pio-examples.pio" start="// mkdocs-start:[i2c]" end="// mkdocs-end:[i2c]" %}
    ```

### Example: hub75_data_rgb888

=== "Method-Chain Assembler"
    ```cpp linenums="1"
    {% include "./sample/pio-examples/pio-examples.cpp" start="// mkdocs-start:[hub75_data_rgb888]" end="// mkdocs-end:[hub75_data_rgb888]" %}
    ```

=== "pioasm"
    ```asm linenums="1"
    {% include "./sample/pio-examples/pio-examples.pio" start="// mkdocs-start:[hub75_data_rgb888]" end="// mkdocs-end:[hub75_data_rgb888]" %}
    ```

### Example: hub75_row

=== "Method-Chain Assembler"
    ```cpp linenums="1"
    {% include "./sample/pio-examples/pio-examples.cpp" start="// mkdocs-start:[hub75_row]" end="// mkdocs-end:[hub75_row]" %}
    ```

=== "pioasm"
    ```asm linenums="1"
    {% include "./sample/pio-examples/pio-examples.pio" start="// mkdocs-start:[hub75_row]" end="// mkdocs-end:[hub75_row]" %}
    ```

### Example: manchester_tx

=== "Method-Chain Assembler"
    ```cpp linenums="1"
    {% include "./sample/pio-examples/pio-examples.cpp" start="// mkdocs-start:[manchester_tx]" end="// mkdocs-end:[manchester_tx]" %}
    ```

=== "pioasm"
    ```asm linenums="1"
    {% include "./sample/pio-examples/pio-examples.pio" start="// mkdocs-start:[manchester_tx]" end="// mkdocs-end:[manchester_tx]" %}
    ```

### Example: nec_carrier_burst

=== "Method-Chain Assembler"
    ```cpp linenums="1"
    {% include "./sample/pio-examples/pio-examples.cpp" start="// mkdocs-start:[nec_carrier_burst]" end="// mkdocs-end:[nec_carrier_burst]" %}
    ```

=== "pioasm"
    ```asm linenums="1"
    {% include "./sample/pio-examples/pio-examples.pio" start="// mkdocs-start:[nec_carrier_burst]" end="// mkdocs-end:[nec_carrier_burst]" %}
    ```

### Example: nec_carrier_control

=== "Method-Chain Assembler"
    ```cpp linenums="1"
    {% include "./sample/pio-examples/pio-examples.cpp" start="// mkdocs-start:[nec_carrier_control]" end="// mkdocs-end:[nec_carrier_control]" %}
    ```

=== "pioasm"
    ```asm linenums="1"
    {% include "./sample/pio-examples/pio-examples.pio" start="// mkdocs-start:[nec_carrier_control]" end="// mkdocs-end:[nec_carrier_control]" %}
    ```

### Example: nec_receive

=== "Method-Chain Assembler"
    ```cpp linenums="1"
    {% include "./sample/pio-examples/pio-examples.cpp" start="// mkdocs-start:[nec_receive]" end="// mkdocs-end:[nec_receive]" %}
    ```

=== "pioasm"
    ```asm linenums="1"
    {% include "./sample/pio-examples/pio-examples.pio" start="// mkdocs-start:[nec_receive]" end="// mkdocs-end:[nec_receive]" %}
    ```

### Example: onewire

=== "Method-Chain Assembler"
    ```cpp linenums="1"
    {% include "./sample/pio-examples/pio-examples.cpp" start="// mkdocs-start:[onewire]" end="// mkdocs-end:[onewire]" %}
    ```

=== "pioasm"
    ```asm linenums="1"
    {% include "./sample/pio-examples/pio-examples.pio" start="// mkdocs-start:[onewire]" end="// mkdocs-end:[onewire]" %}
    ```

### Example: pwm

=== "Method-Chain Assembler"
    ```cpp linenums="1"
    {% include "./sample/pio-examples/pio-examples.cpp" start="// mkdocs-start:[pwm]" end="// mkdocs-end:[pwm]" %}
    ```

=== "pioasm"
    ```asm linenums="1"
    {% include "./sample/pio-examples/pio-examples.pio" start="// mkdocs-start:[pwm]" end="// mkdocs-end:[pwm]" %}
    ```

### Example: quadrature_encoder

=== "Method-Chain Assembler"
    ```cpp linenums="1"
    {% include "./sample/pio-examples/pio-examples.cpp" start="// mkdocs-start:[quadrature_encoder]" end="// mkdocs-end:[quadrature_encoder]" %}
    ```

=== "pioasm"
    ```asm linenums="1"
    {% include "./sample/pio-examples/pio-examples.pio" start="// mkdocs-start:[quadrature_encoder]" end="// mkdocs-end:[quadrature_encoder]" %}
    ```

### Example: quadrature_encoder_substep

=== "Method-Chain Assembler"
    ```cpp linenums="1"
    {% include "./sample/pio-examples/pio-examples.cpp" start="// mkdocs-start:[quadrature_encoder_substep]" end="// mkdocs-end:[quadrature_encoder_substep]" %}
    ```

=== "pioasm"
    ```asm linenums="1"
    {% include "./sample/pio-examples/pio-examples.pio" start="// mkdocs-start:[quadrature_encoder_substep]" end="// mkdocs-end:[quadrature_encoder_substep]" %}
    ```

### Example: spi_cpha0

=== "Method-Chain Assembler"
    ```cpp linenums="1"
    {% include "./sample/pio-examples/pio-examples.cpp" start="// mkdocs-start:[spi_cpha0]" end="// mkdocs-end:[spi_cpha0]" %}
    ```

=== "pioasm"
    ```asm linenums="1"
    {% include "./sample/pio-examples/pio-examples.pio" start="// mkdocs-start:[spi_cpha0]" end="// mkdocs-end:[spi_cpha0]" %}
    ```

### Example: spi_cpha1

=== "Method-Chain Assembler"
    ```cpp linenums="1"
    {% include "./sample/pio-examples/pio-examples.cpp" start="// mkdocs-start:[spi_cpha1]" end="// mkdocs-end:[spi_cpha1]" %}
    ```

=== "pioasm"
    ```asm linenums="1"
    {% include "./sample/pio-examples/pio-examples.pio" start="// mkdocs-start:[spi_cpha1]" end="// mkdocs-end:[spi_cpha1]" %}
    ```

### Example: squarewave

=== "Method-Chain Assembler"
    ```cpp linenums="1"
    {% include "./sample/pio-examples/pio-examples.cpp" start="// mkdocs-start:[squarewave]" end="// mkdocs-end:[squarewave]" %}
    ```

=== "pioasm"
    ```asm linenums="1"
    {% include "./sample/pio-examples/pio-examples.pio" start="// mkdocs-start:[squarewave]" end="// mkdocs-end:[squarewave]" %}
    ```

### Example: squarewave_fast

=== "Method-Chain Assembler"
    ```cpp linenums="1"
    {% include "./sample/pio-examples/pio-examples.cpp" start="// mkdocs-start:[squarewave_fast]" end="// mkdocs-end:[squarewave_fast]" %}
    ```

=== "pioasm"
    ```asm linenums="1"
    {% include "./sample/pio-examples/pio-examples.pio" start="// mkdocs-start:[squarewave_fast]" end="// mkdocs-end:[squarewave_fast]" %}
    ```

### Example: squarewave_wrap

=== "Method-Chain Assembler"
    ```cpp linenums="1"
    {% include "./sample/pio-examples/pio-examples.cpp" start="// mkdocs-start:[squarewave_wrap]" end="// mkdocs-end:[squarewave_wrap]" %}
    ```

=== "pioasm"
    ```asm linenums="1"
    {% include "./sample/pio-examples/pio-examples.pio" start="// mkdocs-start:[squarewave_wrap]" end="// mkdocs-end:[squarewave_wrap]" %}
    ```

### Example: st7789_lcd

=== "Method-Chain Assembler"
    ```cpp linenums="1"
    {% include "./sample/pio-examples/pio-examples.cpp" start="// mkdocs-start:[st7789_lcd]" end="// mkdocs-end:[st7789_lcd]" %}
    ```

=== "pioasm"
    ```asm linenums="1"
    {% include "./sample/pio-examples/pio-examples.pio" start="// mkdocs-start:[st7789_lcd]" end="// mkdocs-end:[st7789_lcd]" %}
    ```

### Example: uart_rx

=== "Method-Chain Assembler"
    ```cpp linenums="1"
    {% include "./sample/pio-examples/pio-examples.cpp" start="// mkdocs-start:[uart_rx]" end="// mkdocs-end:[uart_rx]" %}
    ```

=== "pioasm"
    ```asm linenums="1"
    {% include "./sample/pio-examples/pio-examples.pio" start="// mkdocs-start:[uart_rx]" end="// mkdocs-end:[uart_rx]" %}
    ```

### Example: uart_tx

=== "Method-Chain Assembler"
    ```cpp linenums="1"
    {% include "./sample/pio-examples/pio-examples.cpp" start="// mkdocs-start:[uart_tx]" end="// mkdocs-end:[uart_tx]" %}
    ```

=== "pioasm"
    ```asm linenums="1"
    {% include "./sample/pio-examples/pio-examples.pio" start="// mkdocs-start:[uart_tx]" end="// mkdocs-end:[uart_tx]" %}
    ```

### Example: ws2812

=== "Method-Chain Assembler"
    ```cpp linenums="1"
    {% include "./sample/pio-examples/pio-examples.cpp" start="// mkdocs-start:[ws2812]" end="// mkdocs-end:[ws2812]" %}
    ```

=== "pioasm"
    ```asm linenums="1"
    {% include "./sample/pio-examples/pio-examples.pio" start="// mkdocs-start:[ws2812]" end="// mkdocs-end:[ws2812]" %}
    ```
