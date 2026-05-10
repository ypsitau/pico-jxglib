# List of Tickables

When your firmware links shell command functionality, you can use the `ticks` command to list all `Tickable` objects. The output of the `ticks` command looks like this:

```text
>ticks
Tick Called Depth Max: 1
Serial::Terminal     Normal 0msec
Shell                Normal 0msec
Shell::BreakDetector Lowest 100msec
```

Various services in pico-jxglib are implemented as `Tickable` objects, allowing them to run in the background without blocking the main loop.
