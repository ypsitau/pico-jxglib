# Display - WS2812

In this guide, we will use the full-color serial LED WS2812 to create illumination strips and LED displays. With pico-jxgLABO, you can control WS2812 LEDs using simple commands—**no programming required**. You can turn on LEDs or display animations just by entering commands.

## Demo Video

<div class="video-container">
  <iframe 
    src="https://www.youtube.com/embed/_Ihu_z_rbcE?rel=0&modestbranding=1" 
    title="pico-jxgLABO WS2812 Demo"
    frameborder="0" 
    allow="accelerometer; autoplay; clipboard-write; encrypted-media; gyroscope; picture-in-picture; web-share" 
    allowfullscreen>
  </iframe>
</div>

## About the Full-Color Serial LED WS2812

The WS2812 is a full-color serial LED that integrates RGB LEDs and a control circuit into a single package. Each LED chip supports 24-bit color (8 bits per channel), producing vivid colors. Since it is controlled via serial communication, you can connect multiple LEDs in series and control them all with a single data line. This makes it easy to build displays and illuminations with many LEDs.

Some third-party Raspberry Pi Pico compatible boards come with built-in WS2812 LEDs, such as the Speed Studio XIAO RP2040 and Waveshare RP2040-Zero.

![speedstudio-xiao-rp2040-and-waveshare-rp2040-zero](https://raw.githubusercontent.com/ypsitau/zenn/main/images/2025-11-11-labo-led/speedstudio-xiao-rp2040-and-waveshare-rp2040-zero.jpg)
*Speed Studio XIAO RP2040 (left) and Waveshare RP2040-Zero (right)*

LED strips with WS2812 LEDs arranged in a line are also commercially available. Below is an example of a strip with 60 WS2812 LEDs.

![ws2812-strip](https://raw.githubusercontent.com/ypsitau/zenn/main/images/2025-11-11-labo-led/ws2812-strip.jpg)

There are also WS2812 modules arranged in a matrix, which allow you to easily create small displays. Below is a 16x16 matrix module.

![ws2812-matrix](https://raw.githubusercontent.com/ypsitau/zenn/main/images/2025-11-11-labo-led/ws2812-matrix.jpg)
The shape may vary, but the control method is always the same. Connect 5V power to VCC and GND, and connect the control signal to the DIN (Data In) pin. By sending signals using pulse width modulation, you transmit 1s and 0s, and by sending 24-bit RGB data in sequence, you set the color of each LED.

![ws2812-pinout](https://raw.githubusercontent.com/ypsitau/zenn/main/images/2025-11-11-labo-led/ws2812-pinout.jpg)

To connect multiple LEDs, connect the DOUT (Data Out) pin of one LED to the DIN pin of the next. Once a WS2812 stores its own RGB data, it passes the remaining data to the next LED via DOUT.

## Connecting the Full-Color Serial LED


Connect the VCC and GND of the WS2812 to a 5V power supply, and connect the DIN pin to a GPIO pin on the Pico board.

If you have only a small number of LEDs (up to a few dozen), you can use the Pico board's 5V output (VSYS) to power the WS2812. Below is an example where the DIN pin is connected to GPIO2.

![ws2812-circuit](https://raw.githubusercontent.com/ypsitau/zenn/main/images/2025-11-11-labo-led/ws2812-circuit.png)

When lit at full white, each WS2812 consumes about 12mA. A strip with 60 LEDs will require about 720mA, and a 16x16 matrix module (256 LEDs) will require up to about 3A. The recommended maximum output current for the Pico board's VSYS is 300mA, which is not enough to drive many LEDs.

If you need to connect many LEDs, use an external 5V power supply as shown below. The GND of the Pico board and the external power supply must be connected together.

![ws2812-circuit-extpower](https://raw.githubusercontent.com/ypsitau/zenn/main/images/2025-11-11-labo-led/ws2812-circuit-extpower.png)

## How to Use the Commands

pico-jxgLABO provides the `ws2812` command for direct control of WS2812 LEDs, and the `display-ws2812` command to use them as a display. Here are the basic commands.

### Direct Control of WS2812 LEDs

You can control WS2812 LEDs directly with the `ws2812` command.

First, use the `setup` subcommand to set the connection information for the WS2812. Specify the data input pin with the `din` parameter.

```text
L:/>ws2812 setup {din:PIN}
```

WS2812 LEDs are very bright, so you can adjust the brightness with the `brightness` subcommand. The brightness can be set from 0.0 (off) to 1.0 (maximum). The following sets the brightness to 0.1 (10% of maximum).

```text
L:/>ws2812 brightness:.1
```

Here are some hardware-specific examples:

- For the Speed Studio XIAO RP2040 board, the built-in WS2812 DIN pin is connected to GPIO12. The VCC is connected to GPIO11, so set this pin as a digital output and output 1.

  ```text
  L:/>ws2812 setup {din:12}
  L:/>gpio11 func:sio dir:out put:1
  ```


- For the Waveshare RP2040-Zero board, the built-in WS2812 DIN pin is connected to GPIO16. Set it as follows:

  ```text
  L:/>ws2812 setup {din:16}
  ```

- For external LED strips or matrix modules, the setup is the same. For example, if you connect these devices to GPIO2:

  ```text
  L:/>ws2812 setup {din:2}
  ```

Below, we explain how to operate using a circuit with an LED strip connected to GPIO2.

Use the `put` subcommand to set the color of the LED. For example, to light up red:

```text
L:/>ws2812 put:red
```

![ws2812-red](https://raw.githubusercontent.com/ypsitau/zenn/main/images/2025-11-11-labo-led/ws2812-red.jpg)

You can also specify the color using a hexadecimal color code:

```text
L:/>ws2812 put:#ff0000
```

If you run the `put` subcommand multiple times, you can set the colors of multiple LEDs connected in series:

```text
L:/>ws2812 put:red put:green put:blue
```

![ws2812-rgb](https://raw.githubusercontent.com/ypsitau/zenn/main/images/2025-11-11-labo-led/ws2812-rgb.jpg)

The colors are set in order from the LED closest to the Pico board.

### Using WS2812 LEDs as a Display


#### How to Use the `display-ws2812` Command

The `display-ws2812` command allows you to use WS2812 LEDs as a display. You can display flowing patterns on an LED strip, or show images, text, and animations on a 2D matrix module.

Use the `setup` subcommand of `display-ws2812` to set the connection information and display shape. Below is an example for a strip of 60 WS2812 LEDs connected to GPIO2:

```text
L:/>display-ws2812 setup {din:2 straight:60}
```

The `din` subcommand specifies the GPIO pin connected to the DIN pin. `straight:n` means n LEDs are arranged in a straight line.

You can check the display information with the `ls-display` command:

```text
L:/>ls-display
display 0: WS2812 60x1 DIN:2 Layout:straight
```

You can see that it is recognized as a 60 x 1 pixel display.

WS2812 LEDs are very bright, so you can adjust the brightness with the `brightness` subcommand. The brightness can be set from 0.0 (off) to 1.0 (maximum). The following sets the brightness to 0.1 (10% of maximum).

```text
L:/>display-ws2812 brightness:.1
```

If you use a 2D matrix module, specify the layout as `LAYOUT-START-DIR:WIDTH,HEIGHT` instead of `straight`. Each field is specified as follows:

- `LAYOUT`: Specify `straight` or `zigzag` depending on the wiring layout.  
  ![ws2812-matrix-layout](https://raw.githubusercontent.com/ypsitau/zenn/main/images/2025-11-11-labo-led/ws2812-matrix-layout.png)
- `START`: Specify the position of the first LED as seen from the Pico board: `nw` (north-west: top left), `ne` (north-east: top right), `sw` (south-west: bottom left), or `se` (south-east: bottom right).  
  ![ws2812-matrix-start](https://raw.githubusercontent.com/ypsitau/zenn/main/images/2025-11-11-labo-led/ws2812-matrix-start.png)
- `DIR`: Specify the direction of each row as `vert` (vertical) or `horz` (horizontal).  
  ![ws2812-matrix-dir](https://raw.githubusercontent.com/ypsitau/zenn/main/images/2025-11-11-labo-led/ws2812-matrix-dir.png)
- `WIDTH` and `HEIGHT`: Specify the width and height of the matrix.

For example, for a 16x16 matrix module wired in a zigzag pattern, starting from the top left and connected vertically, specify `zigzag-nw-vert:16,16`.

```text
L:/>display-ws2812 setup {din:2 zigzag-nw-vert:16,16}
L:/>ls-display
display 0: WS2812 16x16 DIN:2 Layout:zigzag-nw-vert
```

#### `draw` コマンドについて

ディスプレイに画像を表示するには `draw` コマンドを使います。`draw` コマンドは描画機能をいくつか持っていますが、ここでは画像ファイルを読み込んで表示する方法を紹介します。

以下に `draw` コマンドの動作イメージを示します。

![draw-command-image](https://raw.githubusercontent.com/ypsitau/zenn/main/images/2025-11-11-labo-led/draw-command-image.png)

`draw` コマンドの内部にはイメージバッファがあり、`image-load` サブコマンドで画像ファイルを読み込むと、このバッファに画像データが格納されます。`image` サブコマンドでイメージバッファの内容をディスプレイに表示します。

`image` サブコマンドは `offset` と `size` サブコマンドでイメージバッファ内の表示範囲を指定します。

![draw-command-image-params](https://raw.githubusercontent.com/ypsitau/zenn/main/images/2025-11-11-labo-led/draw-command-image-params.png)

画像描画に関する `draw` サブコマンドには他に以下のものがあります。

- `offset-shift`: 表示範囲の位置を指定量だけずらすことができます
- `repeat-x`, `repeat-y`: 表示範囲をディスプレイの幅および高さいっぱいに繰り返し表示します。

#### 赤いドットが流れるイルミネーションを表示する

実際に画像を表示してみましょう。ここでは、60 個の WS2812 が連なった LED ストリップを使い、赤いドットが流れるアニメーションを表示します。

1. ディスプレイのセットアップを行います。LED の DIN 端子を GPIO2 に接続します。

   ```text
   L:/>display-ws2812 setup {din:2 straight:60}
   ```

2. 画像ファイル [red-dot.png](https://raw.githubusercontent.com/ypsitau/zenn/main/resource/2025-11-11-labo-led/red-dot.png) をダウンロードします。拡大すると以下のようなピクセルイメージになっています。

   ![red-dot-image](https://raw.githubusercontent.com/ypsitau/zenn/main/images/2025-11-11-labo-led/red-dot-image.png)
   *`red-dot.png` のイメージ*

   このファイルを Pico ボードの内部ストレージ `L:` ドライブに保存します。USB ケーブルで Pico ボードをホスト PC に接続していると、Pico ボード内部の `L:` ドライブは USB メモリとしてホスト PC に認識されるので、そのドライブにコピーします。

3. `draw` コマンドの `image-load` サブコマンドで画像ファイルをイメージバッファに読み込みます。

   ```text
   L:/>draw image-load:red-dot.png
   ```

4. `image` サブコマンドを実行すると、イメージバッファの内容をディスプレイに表示します。このとき、`size` サブコマンドでイメージ内の表示領域が 8 x 1 になるように指定します。

   ```text
   L:/>draw image {size:8,1}
   ```

   赤い LED が一つだけ点灯します。

5. `repeat-x` サブコマンドを使うと、イメージをディスプレイの幅いっぱいに繰り返して表示します。8 個ごとに赤い LED が点灯することを確認してみましょう。

   ```text
   L:/>draw image {size:8,1 repeat-x}
   ```

6. `offset-shift` サブコマンドを使うと、イメージ内の表示領域の位置を指定量だけずらすことができます。以下は、表示領域を右に 1 ピクセルずらす例です。

   ```text
   L:/>draw image {offset-shift:1,0}
   ```

   この操作によって、表示内容が以下のように変化し、ドットが動いているように見えます。

   ![red-dot-shifted](https://raw.githubusercontent.com/ypsitau/zenn/main/images/2025-11-11-labo-led/red-dot-shifted.png)

   何度か実行して、赤い LED が移動している様子を確認してみてください。`offset-shift` は、表示領域が右端まで到達すると左端に戻る動作をします。
 
7. `repeat` サブコマンドは、ブレース `{}` 内のサブコマンドを繰り返し実行します。`Ctrl-C` キーを押すと繰り返しを停止できます。

   ```text
   L:/>draw repeat { image {offset-shift:1,0} }
   ```
   
   表示速度が速すぎてすべての LED が点灯しているように見えますね。`sleep` サブコマンドで指定したミリ秒だけ処理を一時停止できるので、これを使って表示速度を調整しましょう。以下は 100ms で表示位置をずらす例です。

   ```text
   L:/>draw repeat { image {offset-shift:1,0} sleep:100 }
   ```

   イルミネーションストリップの完成です!

実行するコマンドを以下にまとめます。

```text
L:/>display-ws2812 setup {din:2 straight:60} brightness:.1
L:/>draw image-load:red-dot.png
L:/>draw image {size:8,1 repeat-x}
L:/>draw repeat { image {offset-shift:1,0} sleep:100 }
```

#### 様々なパターンのイルミネーションを表示する

イメージファイルの内容や表示範囲の移動量を変えると、様々なパターンのイルミネーションを表示できます。

- **色相グラデーション**

  [hue-32-horz.png](https://raw.githubusercontent.com/ypsitau/zenn/main/resource/2025-11-11-labo-led/hue-32-horz.png) は、32 色の色相グラデーションパターンが水平方向に 2 列並んだ画像です。

  ![hue-32-horz-image](https://raw.githubusercontent.com/ypsitau/zenn/main/images/2025-11-11-labo-led/hue-32-horz-image.png)
   *`hue-32-horz.png` のイメージ*

  このファイルを Pico ボードの内部ストレージ `L:` ドライブに保存して、以下のコマンドを実行します。

  ```text title="イルミネーションバー: 色相グラデーション"
  L:/>display-ws2812 setup {din:2 straight:60} brightness:.1
  L:/>draw image-load:hue-32-horz.png
  L:/>draw image {size:32,1 repeat-x}
  L:/>draw repeat { image {offset-shift:1,0} sleep:100 }
  ```

  `offset-shift:1,0` を実行すると、以下のように表示範囲が右にずれて、色相が流れるように見えます。

  ![hue-32-horz-shifted](https://raw.githubusercontent.com/ypsitau/zenn/main/images/2025-11-11-labo-led/hue-32-horz-shifted.png)

- **色を変えながら流れるドット**

  [hue-256-vert.png](https://raw.githubusercontent.com/ypsitau/zenn/main/resource/2025-11-11-labo-led/hue-256-vert.png) は、256 色の色相グラデーションが垂直方向に並んだ画像です。

  ![hue-256-vert-image](https://raw.githubusercontent.com/ypsitau/zenn/main/images/2025-11-11-labo-led/hue-256-vert-image.png)
   *`hue-256-vert.png` のイメージ*

  このファイルを Pico ボードの内部ストレージ `L:` ドライブに保存して、以下のコマンドを実行します。

  ```text title="イルミネーションバー: 色を変えながら流れるドット"
  L:/>display-ws2812 setup {din:2 straight:60} brightness:.1
  L:/>draw image-load:hue-256-vert.png
  L:/>draw image {size:8,1 repeat-x}
  L:/>draw repeat { image {offset-shift:1,1} sleep:100 }
  ```

   `offset-shift:1,1` を実行すると、以下のように表示範囲が右下にずれていくので、同時に色相も変化しながらドットが流れるように見えます。

  ![hue-256-vert-shifted](https://raw.githubusercontent.com/ypsitau/zenn/main/images/2025-11-11-labo-led/hue-256-vert-shifted.png)

  `offset-shift` で縦方向の移動量を変えて `offset-shift:1,2` のように実行すると、色相の変化速度を速くできます。

#### WS2812 マトリクスモジュールに画像を表示する

マトリクス状に配置された WS2812 モジュールを使うと、小型のディスプレイを簡単に作成できます。以下に 16x16 のマトリクスモジュールを Pico ボードに接続した例を示します。

![ws2812-matrix-connect](https://raw.githubusercontent.com/ypsitau/zenn/main/images/2025-11-11-labo-led/ws2812-matrix-connect.jpg)

マトリクスモジュールの DIN ピンを GPIO2 に接続しています。

- **文字が流れるディスプレイ**

  [alphabet-white-16.png](https://raw.githubusercontent.com/ypsitau/zenn/main/resource/2025-11-11-labo-led/alphabet-white-16.png) は、縦 16 ドットで横にアルファベット大文字 A から Z までが白色で並んだ画像ファイルです。

   ![alphabet-white-16-image](https://raw.githubusercontent.com/ypsitau/zenn/main/images/2025-11-11-labo-led/alphabet-white-16-image.png)
   *`alphabet-white-16.png` のイメージ*

  このファイルを Pico ボードの内部ストレージ `L:` ドライブに保存して、以下のコマンドを実行します。

   ```text title="文字が流れるディスプレイ"
   L:/>display-ws2812 setup {din:2 zigzag-nw-vert:16,16} brightness:.1
   L:/>draw image-load:alphabet-white-16.png
   L:/>draw image {size:16,16}
   L:/>draw repeat { image {offset-shift:1,0} sleep:100 }
   ```

  `offset-shift:1,0' を実行すると、以下のように表示範囲が右にずれて、文字が流れるように見えます。

   ![alphabet-white-16-shifted](https://raw.githubusercontent.com/ypsitau/zenn/main/images/2025-11-11-labo-led/alphabet-white-16-shifted.png)

- **アニメーション表示**

  [rect-inflate-16.png](https://raw.githubusercontent.com/ypsitau/zenn/main/resource/2025-11-11-labo-led/rect-inflate-16.png) は、16x16 ドットの中で四角形が徐々に大きくなるイメージを横に並べた画像ファイルです。

   ![rect-inflate-16-image](https://raw.githubusercontent.com/ypsitau/zenn/main/images/2025-11-11-labo-led/rect-inflate-16-image.png)
   *`rect-inflate-16.png` のイメージ*

  このファイルを Pico ボードの内部ストレージ `L:` ドライブに保存して、以下のコマンドを実行します。

   ```text title="アニメーション表示"
   L:/>display-ws2812 setup {din:2 zigzag-nw-vert:16,16} brightness:.1
   L:/>draw image-load:rect-inflate-16.png
   L:/>draw image {size:16,16}
   L:/>draw repeat { image {offset-shift:16,0} sleep:100 }
   ```

   `offset-shift:16,0` を実行すると、以下のように表示範囲が 1 コマ分 (16 ドット) ずつ右にずれて、アニメーション効果が得られます。

   ![rect-inflate-16-shifted](https://raw.githubusercontent.com/ypsitau/zenn/main/images/2025-11-11-labo-led/rect-inflate-16-shifted.png)

   コマンド操作はまったく同じで、画像ファイルの中身を変えるだけで様々なアニメーションを表示できます。

## 電源投入で自動的に画像を表示する

電源投入時に自動的に画像を表示するには、Pico ボードの内部ストレージ `L:` ドライブに `.startup` という名前のスクリプトファイルを作成し、実行したいコマンドを記述します。

以下は、赤いドットが流れるイルミネーションバーを表示するための `.startup` ファイルの例です。

```text title=".startup"
display-ws2812 setup {din:2 straight:60} brightness:.1
draw image-load:red-dot.png
draw image {size:8,1 repeat-x}
draw repeat { image {offset-shift:1,0} sleep:100 }
```
