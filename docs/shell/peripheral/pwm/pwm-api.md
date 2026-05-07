## Relation to C/C++ API

The corresponding Pico SDK APIs for each `pwm` subcommand:

| Subcommand     | Pico SDK API |
|----------------|---------------------------------------------|
| `func`         | `gpio_set_function(uint gpio, gpio_function_t fn)` |
| `enable`       | `pwm_set_enabled(uint slice_num, bool enabled)` |
| `disable`      | `pwm_set_enabled(uint slice_num, bool enabled)` |
| `freq`         | (none; calculated from `clkdiv` and `wrap`) |
| `clkdiv`       | `pwm_set_clkdiv(uint slice_num, float divider)` |
| `wrap`         | `pwm_set_wrap(uint slice_num, uint16_t wrap)` |
| `duty`         | (none; calculated from `level` and `wrap`) |
| `level`        | `pwm_set_chan_level(uint slice_num, uint chan, uint16_t level)` |
| `counter`      | `pwm_set_counter(uint slice_num, uint16_t c)` |
| `phase-correct`| `pwm_set_phase_correct(uint slice_num, bool phase_correct)` |

pico-jxglib APIs corresponding to `pwm` subcommands:

| Subcommand      | pico-jxglib API |
|-----------------|-------|
| `func`          | `GPIO::set_function(gpio_function_t fn)`|
| `enable`        | `PWM::set_enabled(bool enabled)`|
| `disable`       | `PWM::set_enabled(bool enabled)`|
| `freq`          | `PWM::set_freq(uint32_t freq)` |
| `clkdiv`        | `PWM::set_clkdiv(float divider)` |
| `wrap`          | `PWM::set_wrap(uint16_t wrap)` |
| `duty`          | `PWM::set_chan_duty(float duty)` |
| `level`         | `PWM::set_chan_level(uint16_t level)` |
| `counter`       | `PWM::set_counter(uint16_t c)` |
| `phase-correct` | `PWM::set_phase_correct(bool phase_correct)` |
