# fx2lafw Compatible Logic Analyzer for Raspberry Pi Pico

## 概要

PulseViewとの USB通信参考用に作成した、fx2lafw互換の最小限のロジックアナライザ実装です。

## 特徴

- fx2lafw互換のUSBインターフェース
- PulseViewから直接認識・使用可能
- 8bit/16bit サンプリングモード対応
- テストデータ生成機能付き

## USB 仕様

- **VID:PID**: `1D50:608C` (fx2lafw互換)
- **USB Class**: Vendor Specific
- **Endpoints**: Control + Vendor Bulk IN

## サポートコマンド

### CMD_GET_FW_VERSION (0xB0)
ファームウェアバージョンを取得
- 応答: `struct version_info { uint8_t major; uint8_t minor; }`

### CMD_START (0xB1)
データ取得開始
- パラメータ: `struct cmd_start_acquisition { uint8_t flags; uint8_t sample_delay_h; uint8_t sample_delay_l; }`
- flags bit[5]: 0=8bit, 1=16bit
- flags bit[6]: 0=30MHz, 1=48MHz

### CMD_GET_REVID_VERSION (0xB2)
リビジョンID取得
- 応答: `uint16_t revid`

## ビルド方法

```bash
cd build
cmake .. -G "Ninja" -DCMAKE_BUILD_TYPE=Release
ninja
```

## PulseViewでの使用方法

1. Picoを接続
2. PulseViewを起動
3. "Driver" で "fx2lafw" を選択
4. デバイスが自動検出されることを確認
5. サンプリング設定を行い、Startボタンを押す

## GPIO接続

- GPIO 0-7: ロジアナ入力チャンネル (CH0-CH7)
- 16bitモードの場合: GPIO 0-15 (CH0-CH15)

## テストデータ

実装にはテストデータ生成機能が含まれており、実際のGPIO信号の代わりにスクエア波パターンを生成します。

## USB通信の流れ

1. PulseViewがデバイスを検出
2. CMD_GET_FW_VERSION でバージョン確認
3. CMD_GET_REVID_VERSION でリビジョン確認
4. CMD_START でサンプリング開始
5. Vendor Bulk INエンドポイントからデータ受信

## デバッグ

UART経由でデバッグ情報が出力されます：
- USB接続状態
- 受信したコマンド詳細
- サンプリング状態
- データ送信状況

## 制限事項

- 高速サンプリングなし（PIO未使用）
- テストデータのみ（実際のGPIOサンプリングは基本的な実装）
- バッファサイズ制限あり

この実装は主にPulseViewとのUSB通信プロトコルの参考用です。
