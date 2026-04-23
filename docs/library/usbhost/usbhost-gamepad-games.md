## Enjoying Games with a Gamepad

Now that you've connected a gamepad, you probably want to enjoy some games—the main purpose of the device! I found some high-quality games that run on the Pico board in [KenKen's](http://www.ze.em-net.ne.jp/~kenken/index.html) [LCD Game Development with Raspberry Pi Pico](http://www.ze.em-net.ne.jp/~kenken/picogames/index.html), and made it possible to play these games using a gamepad.

The button and joystick assignments for the gamepad are as follows. As mentioned earlier, the actual mapping of each button and analog joystick may differ depending on the model, but this general assignment should work for most cases (excluding the right analog joystick).

- Left and right analog joysticks ... Up, Down, Left, Right
- D-pad ... Up, Down, Left, Right
- A, B, X, Y buttons ... Fire
- Other buttons ... Start

For wiring, please refer to the [diagram above].

### How to Build

1. Clone the repositories.

    ```bash
    git clone https://github.com/ypsitau/picopacman
    git clone https://github.com/ypsitau/picotetris
    git clone https://github.com/ypsitau/picoinvader
    git clone https://github.com/ypsitau/picohakomusu
    git clone https://github.com/ypsitau/picopegsol
    git clone https://github.com/ypsitau/pico-jxglib
    cd pico-jxglib
    git submodule update --init
    ```

1. Move to the directory of the game you want to build and run `code .` to launch VSCode. When the message box `Do you want to import this project as Raspberry Pi Pico project?` appears, click `[Yes]`.

1. The `Import Pico Project` pane will open. Review the contents and click `[Import]`.

1. From the command palette, run `>Raspberry Pi Pico: Switch Board` and select the board you want to use.

1. From the command palette, run `>CMake: Build` or press `[F7]`. When the `Select a kit for project` combo box appears, select `Pico Using compilers:...` to build the project.

By default, the ILI9341 is set as the TFT LCD. If you want to use the ST7789, uncomment the `//#define USE_ST7789` line in `ili9341_spi.c`.

### Pac-Man

UF2 files

- [Pico board + ILI9341](https://github.com/ypsitau/picopacman/raw/refs/heads/main/bin/lcd-pacman-pico-ili9341.uf2)
- [Pico board + ST7789](https://github.com/ypsitau/picopacman/raw/refs/heads/main/bin/lcd-pacman-pico-st7789.uf2)
- [Pico2 board + ILI9341](https://github.com/ypsitau/picopacman/raw/refs/heads/main/bin/lcd-pacman-pico2-ili9341.uf2)
- [Pico2 board + ST7789](https://github.com/ypsitau/picopacman/raw/refs/heads/main/bin/lcd-pacman-pico2-st7789.uf2)

![picopacman.jpg](https://raw.githubusercontent.com/ypsitau/zenn/main/images/2025-04-23-usbhost-gamepad/picopacman.jpg)

### Tetris

UF2 files

- [Pico board + ILI9341](https://github.com/ypsitau/picotetris/raw/refs/heads/main/bin/tetrispico-pico-ili9341.uf2)
- [Pico board + ST7789](https://github.com/ypsitau/picotetris/raw/refs/heads/main/bin/tetrispico-pico-st7789.uf2)
- [Pico2 board + ILI9341](https://github.com/ypsitau/picotetris/raw/refs/heads/main/bin/tetrispico-pico2-ili9341.uf2)
- [Pico2 board + ST7789](https://github.com/ypsitau/picotetris/raw/refs/heads/main/bin/tetrispico-pico2-st7789.uf2)

![picopacman.jpg](https://raw.githubusercontent.com/ypsitau/zenn/main/images/2025-04-23-usbhost-gamepad/picotetris.jpg)

### Invader Game

UF2 files

- [Pico board + ILI9341](https://github.com/ypsitau/picoinvader/raw/refs/heads/main/bin/invaderpico-pico-ili9341.uf2)
- [Pico board + ST7789](https://github.com/ypsitau/picoinvader/raw/refs/heads/main/bin/invaderpico-pico-st7789.uf2)
- [Pico2 board + ILI9341](https://github.com/ypsitau/picoinvader/raw/refs/heads/main/bin/invaderpico-pico2-ili9341.uf2)
- [Pico2 board + ST7789](https://github.com/ypsitau/picoinvader/raw/refs/heads/main/bin/invaderpico-pico2-st7789.uf2)

![picopacman.jpg](https://raw.githubusercontent.com/ypsitau/zenn/main/images/2025-04-23-usbhost-gamepad/picoinvader.jpg)

### Hakoniwa Puzzle

![picopacman.jpg](https://raw.githubusercontent.com/ypsitau/zenn/main/images/2025-04-23-usbhost-gamepad/picohakomusu.jpg)

### Peg Solitaire

![picopacman.jpg](https://raw.githubusercontent.com/ypsitau/zenn/main/images/2025-04-23-usbhost-gamepad/picopegsol.jpg)
