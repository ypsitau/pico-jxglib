# Tickable and Main Loop

Let's create a simple program to see how `Tickable` works.

Create a new Pico SDK project named `tickable-tick`.

{% include-markdown "include/create-open-project.md" %}

Clone the pico-jxglib repository from GitHub so the direcory structure looks like this:

```text
├── pico-jxglib/
└── tickable-tick/
    ├── CMakeLists.txt
    ├── tickable-tick.cpp
    └── ...
```

{% include-markdown "include/clone-repository.md" %}

Add the following lines to the end of `CMakeLists.txt`:

```cmake title="CMakeLists.txt" linenums="1"
{% include "./sample/tickable-tick/CMakeLists.txt" start="# mkdocs-start" end="# mkdocs-end" %}
```

Enable UART or USB stdio as described below.

{% include-markdown "include/enable-stdio.md" %}

Edit `tickable-tick.cpp` as follows:

```cpp title="tickable-none.cpp" linenums="1" hl_lines="15"
{% include "./sample/tickable-tick/tickable-tick.cpp" %}
```

`Tickable::Tick()` is called in the main loop. The implementation of `Tickable::Tick()` is just simple as follows:

```cpp
for (Tickable* pTickable = pTickableTop_; pTickable; pTickable = pTickable->GetNext()) {
    if (pTickable->IsExpired(msecCurSaved_) && !pTickable->IsTickCalled()) {
        pTickable->SetTickCalled();
        pTickable->OnTick();
        pTickable->ClearTickCalled();
    }
}
```

For each `Tickable` object, `OnTick()` is called when the current time has passed the scheduled time for it. `SetTickCalled()` and `ClearTickCalled()` are used to set and clear a flag referred by `IsTickCalled()`, which prevents multiple calls to `OnTick()`.
