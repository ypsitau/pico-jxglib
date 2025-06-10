#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "jxglib/Common.h"

using namespace jxglib;

#define DS3231_ADDR 0x68

// BCD to decimal変換
uint8_t bcd2dec(uint8_t val) {
    return ((val >> 4) * 10) + (val & 0x0F);
}

// decimal to BCD変換
uint8_t dec2bcd(uint8_t val) {
    return ((val / 10) << 4) | (val % 10);
}

// DS3231に時刻を設定する関数
void set_ds3231_time(i2c_inst_t *i2c, uint16_t year, uint8_t mon, uint8_t day, uint8_t hour, uint8_t min, uint8_t sec) {
    uint8_t data[8];
    data[0] = 0x00; // 書き込み開始レジスタ
    data[1] = dec2bcd(sec);
    data[2] = dec2bcd(min);
    data[3] = dec2bcd(hour);
    data[4] = 0; // 曜日（未設定: 0）
    data[5] = dec2bcd(day);
    data[6] = dec2bcd(mon & 0x1F); // 世紀ビットは未設定
    data[7] = dec2bcd(year % 100);

    i2c_write_blocking(i2c, DS3231_ADDR, data, 8, false);
}

void read_ds3231_time(i2c_inst_t *i2c, uint8_t *data) {
    uint8_t reg = 0x00; // 時刻情報の先頭レジスタ
    i2c_write_blocking(i2c, DS3231_ADDR, &reg, 1, true);
    i2c_read_blocking(i2c, DS3231_ADDR, data, 7, false);
}

int main()
{
    stdio_init_all();
    ::i2c_init(i2c1, 400'000);
    GPIO2.set_function_I2C1_SDA().pull_up();
    GPIO3.set_function_I2C1_SCL().pull_up();

    uint8_t time_data[7];

#if 1
    while (true) {
        read_ds3231_time(i2c1, time_data);

        uint8_t sec  = bcd2dec(time_data[0]);
        uint8_t min  = bcd2dec(time_data[1]);
        uint8_t hour = bcd2dec(time_data[2]);
        uint8_t day  = bcd2dec(time_data[4]);
        uint8_t mon  = bcd2dec(time_data[5] & 0x1F); // 上位ビットは世紀ビット
        uint16_t year = 2000 + bcd2dec(time_data[6]);

        printf("%04d-%02d-%02d %02d:%02d:%02d\n", year, mon, day, hour, min, sec);
        sleep_ms(1000);
    }
#endif
	//set_ds3231_time(i2c1, 2023, 10, 31, 12, 34, 56);
}
