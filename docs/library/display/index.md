# Display

```mermaid
classDiagram
  {% include "include/shell-class.mmd" start="%% mkdocs-start:display" end="%% mkdocs-end:display" %}
  {% include "include/shell-class.mmd" start="%% mkdocs-detail-start:display" end="%% mkdocs-detail-end:display" %}
```

Display classes in **pico-jxglib** are designed to provide primitive drawing functions such as drawing images, characters, and rectangles. They leave advanced drawing functions such as drawing circles and lines to specialized libraries. For example, if you need a Windows-like GUI with buttons or list boxes, there is an excellent library called [LVGL](https://lvgl.io/). **pico-jxglib** provides adapters to bridge to such libraries.
