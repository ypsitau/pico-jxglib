#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "jxglib/Serial.h"
#include "jxglib/Shell.h"
#include "jxglib/I2C.h"
#include <math.h>

using namespace jxglib;

// MPU6050（MCU6050）のアドレスと主要レジスタ
#define MPU6050_ADDRESS 0x68        // AD0ピンがLowの場合（0x69の場合もある）
#define MPU6050_REG_WHO_AM_I 0x75
#define MPU6050_REG_PWR_MGMT_1 0x6B
#define MPU6050_REG_PWR_MGMT_2 0x6C
#define MPU6050_REG_CONFIG 0x1A
#define MPU6050_REG_GYRO_CONFIG 0x1B
#define MPU6050_REG_ACCEL_CONFIG 0x1C
#define MPU6050_REG_ACCEL_XOUT_H 0x3B
#define MPU6050_REG_TEMP_OUT_H 0x41
#define MPU6050_REG_GYRO_XOUT_H 0x43

// I2Cピン設定（Raspberry Pi Picoの場合）
#define I2C_SDA_PIN 16
#define I2C_SCL_PIN 17

// センサーデータ構造体
struct MPU6050_RawData {
    int16_t accel_x, accel_y, accel_z;
    int16_t temperature;
    int16_t gyro_x, gyro_y, gyro_z;
};

struct MPU6050_ScaledData {
    float accel_x, accel_y, accel_z;  // g単位
    float temperature;                 // 摂氏
    float gyro_x, gyro_y, gyro_z;     // 度/秒
};

// 加速度センサーの測定範囲設定
enum AccelRange {
    ACCEL_RANGE_2G = 0,   // ±2g  (16384 LSB/g)
    ACCEL_RANGE_4G = 1,   // ±4g  (8192 LSB/g)
    ACCEL_RANGE_8G = 2,   // ±8g  (4096 LSB/g)
    ACCEL_RANGE_16G = 3   // ±16g (2048 LSB/g)
};

// ジャイロスコープの測定範囲設定
enum GyroRange {
    GYRO_RANGE_250DPS = 0,  // ±250°/s  (131 LSB/°/s)
    GYRO_RANGE_500DPS = 1,  // ±500°/s  (65.5 LSB/°/s)
    GYRO_RANGE_1000DPS = 2, // ±1000°/s (32.8 LSB/°/s)
    GYRO_RANGE_2000DPS = 3  // ±2000°/s (16.4 LSB/°/s)
};

class MPU6050 {
private:
    I2C& i2c_;
    uint8_t address_;
    AccelRange accel_range_;
    GyroRange gyro_range_;
    float accel_scale_;
    float gyro_scale_;

public:
    MPU6050(I2C& i2c, uint8_t address = MPU6050_ADDRESS) 
        : i2c_(i2c), address_(address), 
          accel_range_(ACCEL_RANGE_2G), gyro_range_(GYRO_RANGE_250DPS) {
        update_scale_factors();
    }

    // スケールファクターの更新
    void update_scale_factors() {
        // 加速度センサーのスケールファクター
        switch (accel_range_) {
            case ACCEL_RANGE_2G:  accel_scale_ = 16384.0f; break;
            case ACCEL_RANGE_4G:  accel_scale_ = 8192.0f;  break;
            case ACCEL_RANGE_8G:  accel_scale_ = 4096.0f;  break;
            case ACCEL_RANGE_16G: accel_scale_ = 2048.0f;  break;
        }
        
        // ジャイロスコープのスケールファクター
        switch (gyro_range_) {
            case GYRO_RANGE_250DPS:  gyro_scale_ = 131.0f;  break;
            case GYRO_RANGE_500DPS:  gyro_scale_ = 65.5f;   break;
            case GYRO_RANGE_1000DPS: gyro_scale_ = 32.8f;   break;
            case GYRO_RANGE_2000DPS: gyro_scale_ = 16.4f;   break;
        }
    }

    // MPU6050の初期化
    bool initialize() {
        // I2Cピンの設定
        gpio_set_function(I2C_SDA_PIN, GPIO_FUNC_I2C);
        gpio_set_function(I2C_SCL_PIN, GPIO_FUNC_I2C);
        gpio_pull_up(I2C_SDA_PIN);
        gpio_pull_up(I2C_SCL_PIN);

        // I2Cの初期化（100kHz）
        i2c_.init(100000);

        printf("MPU6050 初期化開始...\n");

        // デバイスの確認
        uint8_t who_am_i;
        if (!read_register(MPU6050_REG_WHO_AM_I, &who_am_i, 1)) {
            printf("MPU6050: デバイスとの通信に失敗しました\n");
            return false;
        }

        printf("MPU6050: WHO_AM_I = 0x%02X ", who_am_i);
        if (who_am_i == 0x68) {
            printf("(正常)\n");
        } else {
            printf("(警告: 期待値は0x68)\n");
        }

        // リセット
        if (!write_register(MPU6050_REG_PWR_MGMT_1, 0x80)) {
            printf("MPU6050: リセットに失敗しました\n");
            return false;
        }
        sleep_ms(100);

        // パワーマネジメントの設定（スリープモードを解除）
        if (!write_register(MPU6050_REG_PWR_MGMT_1, 0x00)) {
            printf("MPU6050: パワーマネジメント設定に失敗しました\n");
            return false;
        }
        sleep_ms(10);

        // サンプルレート分周器の設定（1kHz / (1 + 7) = 125Hz）
        if (!write_register(0x19, 0x07)) {
            printf("MPU6050: サンプルレート設定に失敗しました\n");
            return false;
        }

        // デジタルローパスフィルタ設定（94Hz）
        if (!write_register(MPU6050_REG_CONFIG, 0x02)) {
            printf("MPU6050: フィルタ設定に失敗しました\n");
            return false;
        }

        // 加速度センサー設定
        if (!set_accel_range(accel_range_)) {
            printf("MPU6050: 加速度センサー設定に失敗しました\n");
            return false;
        }

        // ジャイロスコープ設定
        if (!set_gyro_range(gyro_range_)) {
            printf("MPU6050: ジャイロスコープ設定に失敗しました\n");
            return false;
        }

        sleep_ms(100);
        printf("MPU6050: 初期化完了\n");
        return true;
    }

    // レジスタからデータを読み取り
    bool read_register(uint8_t reg, uint8_t* data, size_t len) {
        // レジスタアドレスを送信
        int result = i2c_.write_blocking(address_, &reg, 1, true);
        if (result < 0) {
            printf("レジスタアドレス送信エラー: %d\n", result);
            return false;
        }

        // データを読み取り
        result = i2c_.read_blocking(address_, data, len, false);
        if (result < 0) {
            printf("データ読み取りエラー: %d\n", result);
            return false;
        }

        return true;
    }

    // レジスタにデータを書き込み
    bool write_register(uint8_t reg, uint8_t data) {
        uint8_t buffer[2] = {reg, data};
        int result = i2c_.write_blocking(address_, buffer, 2, false);
        if (result < 0) {
            printf("レジスタ書き込みエラー: %d\n", result);
            return false;
        }
        return true;
    }

    // 加速度センサーの測定範囲設定
    bool set_accel_range(AccelRange range) {
        accel_range_ = range;
        update_scale_factors();
        return write_register(MPU6050_REG_ACCEL_CONFIG, (uint8_t)(range << 3));
    }

    // ジャイロスコープの測定範囲設定
    bool set_gyro_range(GyroRange range) {
        gyro_range_ = range;
        update_scale_factors();
        return write_register(MPU6050_REG_GYRO_CONFIG, (uint8_t)(range << 3));
    }

    // 生データの読み取り
    bool read_raw_data(MPU6050_RawData* data) {
        uint8_t raw_data[14];
        if (!read_register(MPU6050_REG_ACCEL_XOUT_H, raw_data, 14)) {
            return false;
        }

        // データを変換（ビッグエンディアン）
        data->accel_x = (int16_t)((raw_data[0] << 8) | raw_data[1]);
        data->accel_y = (int16_t)((raw_data[2] << 8) | raw_data[3]);
        data->accel_z = (int16_t)((raw_data[4] << 8) | raw_data[5]);
        data->temperature = (int16_t)((raw_data[6] << 8) | raw_data[7]);
        data->gyro_x = (int16_t)((raw_data[8] << 8) | raw_data[9]);
        data->gyro_y = (int16_t)((raw_data[10] << 8) | raw_data[11]);
        data->gyro_z = (int16_t)((raw_data[12] << 8) | raw_data[13]);

        return true;
    }

    // スケール済みデータの読み取り
    bool read_scaled_data(MPU6050_ScaledData* data) {
        MPU6050_RawData raw;
        if (!read_raw_data(&raw)) {
            return false;
        }

        // 物理単位に変換
        data->accel_x = raw.accel_x / accel_scale_;
        data->accel_y = raw.accel_y / accel_scale_;
        data->accel_z = raw.accel_z / accel_scale_;
        data->temperature = raw.temperature / 340.0f + 36.53f;
        data->gyro_x = raw.gyro_x / gyro_scale_;
        data->gyro_y = raw.gyro_y / gyro_scale_;
        data->gyro_z = raw.gyro_z / gyro_scale_;

        return true;
    }

    // 傾き角度の計算（加速度センサーから）
    void calculate_angles(const MPU6050_ScaledData& data, float* roll, float* pitch) {
        *roll = atan2(data.accel_y, data.accel_z) * 180.0f / M_PI;
        *pitch = atan2(-data.accel_x, sqrt(data.accel_y * data.accel_y + data.accel_z * data.accel_z)) * 180.0f / M_PI;
    }

    // センサーキャリブレーション（静止状態で実行）
    bool calibrate(int samples = 1000) {
        printf("MPU6050 キャリブレーション開始（%d回サンプリング）...\n", samples);
        printf("センサーを静止状態にしてください\n");
        
        float accel_offset[3] = {0, 0, 0};
        float gyro_offset[3] = {0, 0, 0};
        
        for (int i = 0; i < samples; i++) {
            MPU6050_ScaledData data;
            if (read_scaled_data(&data)) {
                accel_offset[0] += data.accel_x;
                accel_offset[1] += data.accel_y;
                accel_offset[2] += data.accel_z - 1.0f; // 重力分を除く
                gyro_offset[0] += data.gyro_x;
                gyro_offset[1] += data.gyro_y;
                gyro_offset[2] += data.gyro_z;
                
                if (i % 100 == 0) {
                    printf("進行状況: %d%%\n", (i * 100) / samples);
                }
            }
            sleep_ms(2);
        }
        
        // 平均値を計算
        for (int i = 0; i < 3; i++) {
            accel_offset[i] /= samples;
            gyro_offset[i] /= samples;
        }
        
        printf("キャリブレーション完了\n");
        printf("加速度オフセット: X=%.4f, Y=%.4f, Z=%.4f\n", 
               accel_offset[0], accel_offset[1], accel_offset[2]);
        printf("ジャイロオフセット: X=%.4f, Y=%.4f, Z=%.4f\n", 
               gyro_offset[0], gyro_offset[1], gyro_offset[2]);
        
        return true;
    }

    // 自己診断テスト
    bool self_test() {
        printf("MPU6050 自己診断テスト開始...\n");
        
        // 複数回読み取りして安定性をチェック
        MPU6050_ScaledData data[10];
        for (int i = 0; i < 10; i++) {
            if (!read_scaled_data(&data[i])) {
                printf("データ読み取りエラー\n");
                return false;
            }
            sleep_ms(10);
        }
        
        // データの変動をチェック
        float accel_variance = 0, gyro_variance = 0;
        for (int i = 1; i < 10; i++) {
            float accel_diff = fabs(data[i].accel_x - data[i-1].accel_x) +
                              fabs(data[i].accel_y - data[i-1].accel_y) +
                              fabs(data[i].accel_z - data[i-1].accel_z);
            float gyro_diff = fabs(data[i].gyro_x - data[i-1].gyro_x) +
                             fabs(data[i].gyro_y - data[i-1].gyro_y) +
                             fabs(data[i].gyro_z - data[i-1].gyro_z);
            accel_variance += accel_diff;
            gyro_variance += gyro_diff;
        }
        
        printf("加速度変動: %.4f (閾値: 0.1g)\n", accel_variance / 9);
        printf("ジャイロ変動: %.4f (閾値: 10°/s)\n", gyro_variance / 9);
        
        bool test_passed = (accel_variance / 9 < 0.1f) && (gyro_variance / 9 < 10.0f);
        printf("自己診断テスト: %s\n", test_passed ? "合格" : "不合格");
        
        return test_passed;
    }
};

// MPU6050インスタンス
MPU6050 mpu6050(I2C0);

// MPU6050テスト関数
void test_mpu6050() {
    printf("\n=== MPU6050 (MCU6050) テスト開始 ===\n");
    
    if (!mpu6050.initialize()) {
        printf("MPU6050の初期化に失敗しました\n");
        return;
    }

    // 自己診断テスト
    mpu6050.self_test();

    printf("\n--- リアルタイムデータ表示（10秒間）---\n");
    printf("Accel(g)    | Gyro(°/s)   | Temp(°C) | Roll(°) | Pitch(°)\n");
    printf("--------------------------------------------------------\n");

    for (int i = 0; i < 50; i++) {  // 10秒間（200ms間隔）
        MPU6050_ScaledData data;
        if (mpu6050.read_scaled_data(&data)) {
            float roll, pitch;
            mpu6050.calculate_angles(data, &roll, &pitch);
            
            printf("X:%6.2f Y:%6.2f Z:%6.2f | X:%7.1f Y:%7.1f Z:%7.1f | %6.1f | %6.1f | %6.1f\n",
                   data.accel_x, data.accel_y, data.accel_z,
                   data.gyro_x, data.gyro_y, data.gyro_z,
                   data.temperature, roll, pitch);
        } else {
            printf("データ読み取りエラー\n");
        }
        sleep_ms(200);
    }

    // キャリブレーション実行
    printf("\n--- キャリブレーション ---\n");
    mpu6050.calibrate(500);
    
    printf("\n=== MPU6050 テスト終了 ===\n");
}

int main()
{
    ::stdio_init_all();
    
#if 0
    // シリアル出力の安定化を待つ
    sleep_ms(2000);
    // MPU6050テストの実行
    test_mpu6050();
#endif    
    Serial::Terminal terminal;
    Shell::AttachTerminal(terminal.Initialize());
    for (;;) Tickable::Tick();
}
