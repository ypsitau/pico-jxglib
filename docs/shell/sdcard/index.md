今回は、pico-jxgLABO を使って Pico ボードに SD カードを接続します。

pico-jxgLABO では、Pico ボードのフラッシュメモリの一部を `L:` ドライブとしてマウントし、ファイルの読み書きができるようにしています。このドライブの容量は、Pico2 ボードの場合で約 2.5 MByte、Pico ボードですと 0.5 MByte (500 KByte) 程度です。テキストデータの保存程度ならば実用的に使えますが、画像データや音声データなどの大容量データを扱うには不十分です。

SD カードを接続すれば、GByte 単位の大容量ストレージが利用できるようになり、Pico ボードの活用範囲が大きく広がります。SD カードは安価で入手しやすく、Pico ボードの SPI インターフェースに接続して使うことができます。

## SD カードリーダモジュール

SD カードリーダモジュールは、基本的に SD カードスロットといくつかの抵抗や電圧レギュレータが載っているだけのシンプルなものですが、供給電圧や信号のレベル、プルアップ抵抗の有無などがモジュールによって異なるので注意が必要です。

以下に、手元にあった SD カードリーダモジュール (主に Amazon で入手) の外観を示し、供給電圧やプルアップ抵抗の有無、信号レベルについてまとめました。

|外観|注釈|
|----|----|
|![sdcard-adapter](https://raw.githubusercontent.com/ypsitau/zenn/main/images/2025-06-06-fs-media/sdcard-adapter.jpg)|標準 SD カードのモジュールです。供給電圧は **5V** と **3.3V** の両方の端子が用意されていて、5V の場合は電圧レギュレータで 3.3V に降圧して SD カードに供給されます。すべての信号線に 10kΩ のプルアップ抵抗がついているので、外部のプルアップ抵抗は**必要ありません**。信号レベルは **3.3V** です。|
|![ILI9341-back](https://raw.githubusercontent.com/ypsitau/zenn/main/images/2025-06-06-fs-media/ILI9341-back.jpg)|TFT LCD ILI9341 についている標準 SD カードのスロットです。電源は TFT LCD 用のコネクタから **3.3V** を供給します。外部のプルアップ抵抗が**必要です**[^pullup]。信号レベルは **3.3V** です。|
|![u-sdcard-adapter-1](https://raw.githubusercontent.com/ypsitau/zenn/main/images/2025-06-06-fs-media/u-sdcard-adapter-1.jpg)|microSD カードのモジュールです。供給電圧は **5V** で、電圧レギュレータで 3.3V に降圧して SD カードに供給されます。外部のプルアップ抵抗は**必要ありません**。信号線にはバッファ (74HC125) が入っており、**3.3V**、**5V** の両方の信号レベルに接続できます。|
|![u-sdcard-adapter-2](https://raw.githubusercontent.com/ypsitau/zenn/main/images/2025-06-06-fs-media/u-sdcard-adapter-2.jpg)|microSD カードのモジュールです。供給電圧は **3.3V** です。すべての信号線に 10kΩ のプルアップ抵抗がついているので、外部のプルアップ抵抗は**必要ありません**。信号レベルは **3.3V** です。|

供給電圧の見分け方ですが、SD カードリーダモジュールの基板上に以下に示すような電圧レギュレータが載っている場合は 5V 供給、載っていない場合は 3.3V 供給と考えてよいでしょう。

![sdcard-adapter-voltage](https://raw.githubusercontent.com/ypsitau/zenn/main/images/2025-10-22-labo-sdcard-rtc/sdcard-adapter-voltage.jpg)

[^pullup]: 手元の SD カードで試したところ、プルアップ抵抗がなくても動作しましたが、SD カードの種類によってはプルアップ抵抗が必要な場合があります。動作しない場合は、プルアップ抵抗の有無を確認してください。

## デバイスの接続と動作確認

SD カードリーダモジュールは SPI インターフェースに接続します。Pico ボードの SPI0 または SPI1 を使うことができ、ピンレイアウトもコマンドで自由に設定できます。ここでは SPI0 を使い、以下のように接続します。

|SD カードリーダモジュール|Pico ピン番号 |GPIO |ファンクション|
|-----------------------|-------------|-----|-------------|
|VCC                    |39 or 36     |     |VSYS or 3V3 |
|GND                    |3            |     |GND          |
|SCK                    |4            |GPIO2|SPI0 SCK     |
|MOSI                   |5            |GPIO3|SPI0 TX (MOSI)|
|MISO                   |6            |GPIO4|SPI0 RX (MISO)|
|CS                     |7            |GPIO5|SIO          |

:::message alert
SD Card カードリーダモジュールの VCC の接続先は供給電圧によって異なりますので、注意してください。
- 供給電圧 5V の場合は Pico ボードの VSYS (ピン番号 39) に接続します。3V3 に接続するとモジュール基板の電圧レギュレータの電圧降下によって適切な電圧が供給されず、動作しません
- 供給電圧 3.3V の場合は Pico ボードの 3V3 (ピン番号 36) に接続します。VSYS に接続すると**SD カードが壊れる可能性があります**
:::

配線図を以下に示します。GND は Pico ボード上で複数出ているので、どこに接続しても構いません。

![circuit-sdcard](https://raw.githubusercontent.com/ypsitau/zenn/main/images/2025-10-22-labo-sdcard-rtc/circuit-sdcard.png)

以下のコマンドを実行して、SPI0 の GPIO 割り当てを GPIO2 (SPI0 SCK), GPIO3 (SPI0 TX), GPIO4 (SPI0 RX) に設定します。適切なファンクション割り当てが自動的に行われるので、記述の順序は気にしなくて大丈夫です。

```text
L:/>spi0 -p 2,3,4
```

以下のコマンドを実行して、SD カードを接続する SPI インターフェースを SPI0、CS ピンを GPIO5 に設定します。またドライブ名として `M` を指定します。

```text
L:/>sdcard setup {spi:0 cs:5 drive:'M'}
```

これで SD カードのセットアップは完了です。

`ls-drive` コマンドで利用可能なドライブの一覧を表示してみましょう。

```text
L:/>ls-drive
 Drive  Format           Total
*L:     FAT12          2621440
 M:     unmounted            0
```

まだスロットに SD カードが挿入されていないので unmounted になっています。SD カードを入れてから、再度 `ls-drive` コマンドを実行してみましょう。ここでは FAT フォーマットされた 32 GByte の SD カードを挿入しました。

```text
L:/>ls-drive
 Drive  Format        Total
*L:     FAT12       2621440
 M:     FAT32   30945574912
```

いろいろなファイル操作を試してみてください。[こちら](https://zenn.dev/ypsitau/articles/2025-06-09-fs-shell#%E3%82%B7%E3%82%A7%E3%83%AB%E3%81%AB%E3%82%88%E3%82%8B%E3%83%95%E3%82%A1%E3%82%A4%E3%83%AB%E6%93%8D%E4%BD%9C) に pico-jxgLABO[^pico-jxglib] が提供するファイル操作コマンドの使い方をまとめてあります。

[^pico-jxglib]: pico-jxgLABO の機能は、ライブラリ [pico-jxglib](https://zenn.dev/ypsitau/articles/2025-01-24-jxglib-intro) に基づいています。

```text
L:/>M:
M:/>dir
...
```
