# Terminal

When it comes to displays, people tend to focus on graphics and GUIs, but in real applications, there are many situations where you want to display various text information. If you only have "display text at a specified coordinate on the screen," you can only output a limited amount of information. Terminal functionality, where you just send strings and the drawing position is updated automatically, and the screen scrolls when it overflows, is very convenient.

**pico-jxglib**'s Terminal provides the following natural and expected features:

1. Draw characters and strings while updating the drawing position
1. Automatically wrap to the next line when reaching the right edge
1. Scroll when reaching the bottom of the screen
1. Record output in a round line buffer and allow rollback
1. Read back the contents of the round line buffer

Especially, the last feature—reading back the round line buffer—can be combined with storage and communication features (to be implemented in **pico-jxglib**) to make an effective data logger.

Before we get to actual projects using Terminal, let's explain OLED devices, which are popular display devices for single-board microcontrollers alongside TFT LCDs.

```mermaid
classDiagram
  {% include "include/shell-class.mmd" start="%% mkdocs-start:terminal" end="%% mkdocs-end:terminal" %}
  {% include "include/shell-class.mmd" start="%% mkdocs-detail-start:terminal" end="%% mkdocs-detail-end:terminal" %}
  classDef Class_Input fill:#ffc
  classDef Class_Output fill:#f8c
  classDef Class_Both fill:#fc9
```
