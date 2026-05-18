# State Machine Configuration

This page provides an overview of the state machine configuration options available in the PIO (Programmable Input/Output) peripheral of the Raspberry Pi Pico. The PIO allows for flexible and efficient control of GPIO pins, making it ideal for a wide range of applications.

## Assignment of Pins

```cpp
sm.reserve_out_pins(pin1, pin2, ...);
```

```cpp
sm.reserve_set_pins(pin1, pin2, ...);
```

```cpp
sm.reserve_in_pins(pin1, pin2, ...);
```

```cpp
sm.reserve_listen_pins(pin1, pin2, ...);
```

```cpp
sm.reserve_gpio_pin(pin1, pin2, ...);
```

```cpp
sm.reserve_sideset_pins(pin1, pin2, ...);
```

```cpp
sm.config_set_jmp_pin(uint pin);
```

```cpp
sm.config_set_clkdiv_int_frac(uint16_t div_int, uint8_t div_frac);
sm.config_set_clkdiv(float div);
```

```cpp
sm.config_set_sideset(uint bit_count, bool optional, bool pindirs);
```

```cpp
sm.config_set_wrap(uint wrap_target, uint wrap);
```

```cpp
sm.config_set_in_shift(bool shift_right, bool autopush = false, uint push_threshold = 32);
sm.config_set_in_shift_left(bool autopush = false, uint push_threshold = 32);
sm.config_set_in_shift_right(bool autopush = false, uint push_threshold = 32);
```

```cpp
sm.config_set_out_shift(bool shift_right, bool autopull = false, uint pull_threshold = 32);
sm.config_set_out_shift_left(bool autopull = false, uint pull_threshold = 32);
sm.config_set_out_shift_right(bool autopull = false, uint pull_threshold = 32);
```

```cpp
sm.config_set_fifo_join(enum pio_fifo_join join);
sm.config_set_fifo_join_none();
sm.config_set_fifo_join_tx();
sm.config_set_fifo_join_rx();
sm.config_set_fifo_join_txput();
sm.config_set_fifo_join_txget();
sm.config_set_fifo_join_putget();
```

```cpp
sm.config_set_out_special(bool sticky, bool has_enable_pin, uint enable_pin_index);*this; }
```

```cpp
sm.config_set_mov_status(enum pio_mov_status_type status_sel, uint status_n);
```
