# State Machine Configuration

You can configure the behavior of a PIO state machine using the functions described in this page. These functions allow you to set up the pin assignments, clock divider, sideset configuration, wrap configuration, shift register behavior, FIFO join configuration, and other special configurations for the state machine. Properly configuring the state machine is essential for ensuring that it operates correctly and efficiently for your specific application.

These configurations are reflected when `sm.init()` is called.

## Assignment of Pins

```cpp
sm.config_pinseq_out(base, count);
sm.config_pin_out(pin1, pin2, ...);
```

```cpp
sm.config_pinseq_set(base, count);
sm.config_pin_set(pin1, pin2, ...);
```

```cpp
sm.config_pinseq_in(base, count);
sm.config_pin_in(pin1, pin2, ...);
```

```cpp
sm.config_pinseq_listen(base, count);
sm.config_pin_listen(pin1, pin2, ...);
```

```cpp
sm.config_pinseq_gpio(base, count);
sm.config_pin_gpio(pin1, pin2, ...);
```

```cpp
sm.config_pinseq_sideset(base, count);
sm.config_pin_sideset(pin1, pin2, ...);
```

```cpp
sm.config_set_jmp_pin(uint pin);
```

## Clock Divider

```cpp
sm.config_set_clkdiv_int_frac(uint16_t div_int, uint8_t div_frac);
sm.config_set_clkdiv(float div);
```

## Sideset Configuration

```cpp
sm.config_set_sideset(uint bit_count, bool optional, bool pindirs);
```

This function has the same effect as the `.sideset` directive.

## Wrap Configuration

```cpp
sm.config_set_wrap(uint wrap_target, uint wrap);
```

This function has the same effect as the `.wrap_target` and `.wrap` directives.

## ISR (Input Shift Register) Configuration

```cpp
sm.config_set_in_shift(bool shift_right, bool autopush = false, uint push_threshold = 32);
```

```cpp
sm.config_set_in_shift_left(bool autopush = false, uint push_threshold = 32);
```

A convenient shorthand for `sm.config_set_in_shift(false, autopush, push_threshold)`.

```cpp
sm.config_set_in_shift_right(bool autopush = false, uint push_threshold = 32);
```

A convenient shorthand for `sm.config_set_in_shift(true, autopush, push_threshold)`.

## OSR (Output Shift Register) Configuration

```cpp
sm.config_set_out_shift(bool shift_right, bool autopull = false, uint pull_threshold = 32);
```

```cpp
sm.config_set_out_shift_left(bool autopull = false, uint pull_threshold = 32);
```

A convenient shorthand for `sm.config_set_out_shift(false, autopull, pull_threshold)`.

```cpp
sm.config_set_out_shift_right(bool autopull = false, uint pull_threshold = 32);
```

A convenient shorthand for `sm.config_set_out_shift(true, autopull, pull_threshold)`.

## FIFO Join Configuration

```cpp
sm.config_set_fifo_join(enum pio_fifo_join join);
```

```cpp
sm.config_set_fifo_join_none();
```

A convenient shorthand for `sm.config_set_fifo_join(PIO_FIFO_JOIN_NONE)`.

```cpp
sm.config_set_fifo_join_tx();
```

A convenient shorthand for `sm.config_set_fifo_join(PIO_FIFO_JOIN_TX)`.

```cpp
sm.config_set_fifo_join_rx();
```

A convenient shorthand for `sm.config_set_fifo_join(PIO_FIFO_JOIN_RX)`.

```cpp
sm.config_set_fifo_join_txput();
```

A convenient shorthand for `sm.config_set_fifo_join(PIO_FIFO_JOIN_TXPUT)`.

```cpp
sm.config_set_fifo_join_txget();
```

A convenient shorthand for `sm.config_set_fifo_join(PIO_FIFO_JOIN_TXGET)`.

```cpp
sm.config_set_fifo_join_putget();
```

A convenient shorthand for `sm.config_set_fifo_join(PIO_FIFO_JOIN_PUTGET)`.

## Special Configuration

```cpp
sm.config_set_out_special(bool sticky, bool has_enable_pin, uint enable_pin_index);*this; }
```

```cpp
sm.config_set_mov_status(enum pio_mov_status_type status_sel, uint status_n);
```
