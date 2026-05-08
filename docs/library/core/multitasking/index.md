# Multitasking

pico-jxglib doesn't provide a management of task contexts, but it provides a simple cooperative multitasking mechanism. This is achieved by using the `Tickable` class, which allows you to create tasks that can be scheduled to run at specific intervals.

## Tickable

Let's see a simple program that provides a shell functionality:

```cpp title="tickable-none.cpp" linenums="1" hl_lines="15"
{% include "./sample/tickable-none/tickable-none.cpp" %}
```

`Tickable::Tick()` is called in the main loop, and it runs the scheduled tasks. In this function, `Tickable::OnTick()` for each registered `Tickable` object is called.

There seems to be no tasks in the above code, but there are actully some tasks running. If you run the above code and type `>ticks` in the shell, you can see the following output:

```text
>ticks
Tick Called Depth Max: 1
Serial::Terminal     Normal 0msec
Shell                Normal 0msec
Shell::BreakDetector Lowest 100msec
```

Various services in pico-jxglib are implemented as `Tickable` objects, allowing them to run in the background without blocking the main loop.

## Running a Single Task

```cpp title="tickable-single.cpp" linenums="1" hl_lines="8-12"
{% include "./sample/tickable-single/tickable-single.cpp" %}
```

## Running Multiple Tasks

```cpp title="tickable-multi.cpp" linenums="1" hl_lines="8-12 14-18 20-24"
{% include "./sample/tickable-multi/tickable-multi.cpp" %}
```
