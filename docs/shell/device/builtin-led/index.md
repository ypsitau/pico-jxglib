# Built-in LED

The built-in LED on the Pico board can be controlled with the `led` command. This is a simple way to confirm that you can control the board, and it’s also useful for indicating the board’s status. Here’s how to use it:

To turn the LED on and off:

```text
L:/>led on
L:/>led off
```

Use the `flip` subcommand to toggle the LED at specified intervals. The following example blinks the LED every 100ms:

```text
L:/>led on flip:100
```

The `flip` subcommand blinks the LED eternally in the background, so you can run other commands while the LED blinks. To stop the blinking, use the `led off` command:

```text
L:/>led off
```

The following example blinks the LED for 50ms, off for 500ms, three times, then off for 2000ms, and repeats:

```text
L:/>led on flip:50,500,50,500,50,2000
```

To stop flipping after a certain number of times, add `*` at the end. The following blinks three times, then turns off:

```text
L:/>led on flip:50,500,50,500,50,*
```
