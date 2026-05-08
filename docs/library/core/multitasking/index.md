# Multitasking

pico-jxglib doesn't provide a management of task contexts, but it provides a simple cooperative multitasking mechanism. This is achieved by using the `Tickable` class, which allows you to create tasks that can be scheduled to run at specific intervals.

Let's see a simple program that provides a shell functionality:

```cpp title="tickable-simple.cpp" linenums="1" hl_lines="8-12"
{% include "./sample/tickable-simple/tickable-simple.cpp" %}
```

`Tickable::Tick()` is called in the main loop, and it runs the scheduled tasks. In this function, `Tickable::OnTick()` for each registered `Tickable` object is called.

The macro `TickableEntry` is used to create a `Tickable` object and register it to the scheduler.

