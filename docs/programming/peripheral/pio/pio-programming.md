# Programming PIO

Let's see how to program the PIO peripheral by creating a simple program that blinks an LED connected to a GPIO pin.

## Building and Flashing the Program

Create a new Pico SDK project named `pio-blink`.

{% include-markdown "include/create-open-project.md" %}

Clone the pico-jxglib repository from GitHub so the direcory structure looks like this:

```text
├── pico-jxglib/
└── pio-blink/
    ├── CMakeLists.txt
    ├── pio-blink.cpp
    └── ...
```

{% include-markdown "include/clone-repository.md" %}

Add the following lines to the end of `CMakeLists.txt`:

```cmake title="CMakeLists.txt" linenums="1"
{% include "./sample/pio-blink/CMakeLists.txt" start="# mkdocs-start" end="# mkdocs-end" %}
```

Enable UART or USB stdio as described below.

{% include-markdown "include/enable-stdio.md" %}

Edit `pio-blink.cpp` as follows:

```cpp title="pio-blink.cpp" linenums="1"
{% include "./sample/pio-blink/pio-blink.cpp" %}
```

Build and flash the program to the board.

{% include-markdown "include/build-and-flash.md" %}

## Running the Program

Connect an LED to GPIO pin 15 of the Pico board. The program will make the LED blink with a frequency of 2 Hz.

## About the Program

```cpp linenums="10"
PIO::Program program;
program
.program("blink")
    .pull()
    .out("y", 32)
.wrap_target()
    .mov("x", "y")
    .set("pins", 1)         // Turn LED on
.L("lp1")
    .jmp("x--", "lp1")      // Delay for (x + 1) cycles, x is a 32 bit number
    .mov("x", "y")
    .set("pins", 0)         // Turn LED off
.L("lp2")
    .jmp("x--", "lp2")      // Delay for the same number of cycles again
.wrap()                     // Blink forever!
.end();
```

Creates a PIO program named "blink".

```cpp linenums="26"
PIO::StateMachine sm;
sm.set_program(program);
```

Creates a state machine and sets the PIO program to it.

```cpp linenums="28"
sm.config_pin_set(GPIO15);
```

Reserves GPIO pin 15 as a set pin. This is required because the PIO program uses the `set` instruction to set the value of GPIO pins, and the state machine needs to know which pins are used for this purpose.

```cpp linenums="29"
sm.init();
```

Initializes the state machine. This is required before enabling the state machine.

```cpp linenums="30"
sm.set_enabled();
```

Runs the state machine.

```cpp linenums="32"
sm.put(static_cast<uint>((static_cast<float>(::clock_get_hz(clk_sys)) / (freq * 2))) - 3);
```

Puts a value into the state machine's TX FIFO.
