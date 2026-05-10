# Multitasking

pico-jxglib doesn't provide a mechanism for switching task contexts, but it provides a simple cooperative multitasking mechanism. This is achieved by using the `Tickable` class, which allows you to create tasks that can be scheduled to run at specific intervals.
