# C/C++ API

The corresponding Pico SDK APIs for each `gpio` subcommand:

| Subcommand     | Pico SDK API |
|----------------|-------------------------------|
| `func`         | `gpio_set_function(uint gpio, gpio_function_t fn)` |
| `dir`          | `gpio_set_dir(uint gpio, bool out)` |
| `put`          | `gpio_put(uint gpio, bool value)` |
| `get`          | `bool gpio_get(uint gpio)` |
| `pull`         | `gpio_set_pulls(uint gpio, bool up, bool down)` |
| `drive`        | `gpio_set_drive_strength(uint gpio, enum gpio_drive_strength drive)` |
| `slew`         | `gpio_set_slew_rate(uint gpio, enum gpio_slew_rate slew)` |
| `hyst`         | `gpio_set_input_hysteresis_enabled(uint gpio, bool enabled)` |

The corresponding [pico-jxglib](https://zenn.dev/ypsitau/articles/2025-01-24-jxglib-intro) APIs:

| Subcommand     | pico-jxglib API |
|----------------|-------------------------------|
| `func`         | `GPIO::set_function(gpio_function_t fn)` |
| `dir`          | `GPIO::set_dir(bool out)` |
| `put`          | `GPIO::put(bool value)` |
| `get`          | `bool GPIO::get()` |
| `pull`         | `GPIO::set_pulls(bool up, bool down)` |
| `drive`        | `GPIO::set_drive_strength(enum gpio_drive_strength drive)` |
| `slew`         | `GPIO::set_slew_rate(enum gpio_slew_rate slew)` |
| `hyst`         | `GPIO::set_input_hysteresis_enabled(bool enabled)` |
