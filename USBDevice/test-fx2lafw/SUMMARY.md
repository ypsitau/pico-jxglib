# fx2lafw互換ロジックアナライザ - PulseView USB通信リファレンス

## 完成したもの

✅ **ビルド成功** - `fx2lafw_simple.uf2` ファイル生成完了  
✅ **fx2lafw互換USBインターフェース**  
✅ **PulseViewとの通信プロトコル実装**  
✅ **最小限のサンプリング機能**

## ファイル構成

```
test-fx2lafw/
├── fx2lafw_simple.uf2        # Picoに書き込むファイル
├── test-fx2lafw.c            # メイン実装 (USB通信部分)
├── usb_descriptors.c         # USB記述子 (fx2lafw互換)
├── tusb_config.h             # TinyUSB設定
├── CMakeLists.txt            # ビルド設定
└── README.md                 # 使用方法
```

## 実装されたUSB通信コード

### 1. USB記述子 (usb_descriptors.c)
- **VID:PID**: `1D50:608C` (fx2lafw互換)
- **デバイス名**: "fx2lafw" 
- **製造元**: "sigrok"

### 2. USBコントロール転送処理 (test-fx2lafw.c)

```c
bool tud_vendor_control_xfer_cb(uint8_t rhport, uint8_t stage, tusb_control_request_t const* request)
```

#### サポートコマンド:
- `CMD_GET_FW_VERSION (0xB0)` - ファームウェアバージョン取得
- `CMD_START (0xB1)` - サンプリング開始
- `CMD_GET_REVID_VERSION (0xB2)` - リビジョンID取得

### 3. データ転送処理

```c
void vendor_task(void) {
    uint32_t bytes_sent = tud_vendor_write(sample_buffer, bytes_to_send);
    tud_vendor_flush();
}
```

## PulseViewとの通信シーケンス

1. **デバイス検出**: PulseViewがUSBデバイス一覧をスキャン
2. **バージョン確認**: `CMD_GET_FW_VERSION` でfx2lafwファームウェア確認
3. **リビジョン取得**: `CMD_GET_REVID_VERSION` でハードウェアバージョン確認
4. **サンプリング開始**: `CMD_START` + パラメータでデータ取得開始
5. **データ受信**: Vendor Bulk INエンドポイントからサンプルデータ受信

## 使用方法

1. **書き込み**:
   ```bash
   # BOOTSELボタンを押しながらPicoを接続
   cp fx2lafw_simple.uf2 /path/to/RPI-RP2/
   ```

2. **PulseView接続**:
   - PulseViewを起動
   - Driver: "fx2lafw"を選択
   - デバイスが自動認識される
   - サンプリング設定後、Startボタンでテストデータ取得
   - **注意**: GPIO 2-9 がサンプリングチャンネル (GPIO 0,1 はUART用)

3. **デバッグ**:
   - UART (GPIO 0/1) でデバッグ情報出力
   - USB通信の詳細がリアルタイムで確認可能

## 主要なUSB通信実装ポイント

1. **TinyUSB Vendor Class使用**
2. **Control Endpoint (EP0)でコマンド受信**
3. **Bulk IN Endpointでデータ送信**
4. **fx2lafw互換のコマンド構造体**
5. **Non-blocking USBタスク処理**

このコードを参考にして、PulseViewとのUSB通信を理解することができます。
