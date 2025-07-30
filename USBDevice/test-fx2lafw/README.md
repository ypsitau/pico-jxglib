# fx2lafw Logic Analyzer for Raspberry Pi Pico

このプロジェクトは、Raspberry Pi PicoをfX2LaFw互換のロジックアナライザとして動作させるTinyUSBベースの実装です。

## 機能

- PulseView/Sigrok対応のfx2lafwプロトコル実装
- 8チャンネルのデジタル入力（GPIO 0-7）
- 可変サンプリングレート（20kHz～24MHz）
- USB Vendor Classインターフェース

## ハードウェア接続

### 入力チャンネル

- CH0: GPIO 2
- CH1: GPIO 3  
- CH2: GPIO 4
- CH3: GPIO 5
- CH4: GPIO 6
- CH5: GPIO 7
- CH6: GPIO 8
- CH7: GPIO 9

各入力ピンは内部プルアップ抵抗が有効になっています。

### UART出力 (デバッグ用)

- TX: GPIO 0
- RX: GPIO 1
- ボーレート: 115200

## ビルド方法

```bash
cd build
ninja
```

## フラッシュ方法

1. PicoのBOOTSELボタンを押しながらUSBケーブルを接続
2. `test-fx2lafw.uf2`ファイルをPicoのドライブにコピー

## PulseViewでの使用方法

1. Picoをフラッシュ後、USBケーブルで接続
2. PulseViewを起動
3. デバイスを選択：
   - Driver: fx2lafw
   - Interface: fx2lafw (vendor=1D50 product=608C)
4. サンプリング設定を行い、測定開始

## サポートされているサンプリングレート

- 20 kHz
- 25 kHz  
- 50 kHz
- 100 kHz
- 200 kHz
- 250 kHz
- 500 kHz
- 1 MHz
- 2 MHz
- 3 MHz
- 4 MHz
- 6 MHz
- 8 MHz
- 12 MHz
- 16 MHz
- 20 MHz
- 24 MHz

## トラブルシューティング

### PulseViewがデバイスを認識しない場合

1. USBケーブルの接続を確認
2. デバイスマネージャーで「fx2lafw」デバイスが表示されているか確認
3. PulseViewを管理者権限で実行
4. libusb-1.0.dllが正しくインストールされているか確認

### Windows用ドライバ

Windows環境では、libusb-win32またはWinUSBドライバが必要です。
Zadigツールを使用してドライバをインストールできます。

## 制限事項

- 現在のバージョンではバルク転送によるデータ送信は簡易実装
- 最大サンプリングレートは実際のPicoの性能に依存
- プリトリガー機能は未実装

## ライセンス

このプロジェクトはオープンソースです。
