# Set Program to State Machine

## State Machine and Program

## Functions to Set Program

```cpp
StateMachine& set_program(const Program& program);
```

```cpp
StateMachine& set_program(const Program& program, pio_hw_t* pio);
```

```cpp
StateMachine& set_program(const Program& program, pio_hw_t* pio, uint sm);
```

## Functions to Share Program with Another State Machine

```cpp
StateMachine& share_program(StateMachine& smToShareProgram);
```

```cpp
StateMachine& share_program(StateMachine& smToShareProgram, pio_hw_t* pio);
```

```cpp
StateMachine& share_program(StateMachine& smToShareProgram, pio_hw_t* pio, uint sm);
```
