#include <stdio.h>
#include "pico/stdlib.h"
#include "jxglib/GPIO.h"
#include "jxglib/RTC/DS3231.h"

using namespace jxglib;

int main()
{
    ::stdio_init_all();
    ::i2c_init(i2c0, 400'000);
    GPIO16.set_function_I2C0_SDA().pull_up();
    GPIO17.set_function_I2C0_SCL().pull_up();
    RTC::DS3231 rtc(i2c0);    
    for (;;) {
        DateTime dt;
        RTC::Get(&dt);
        ::printf("%04d-%02d-%02d %02d:%02d:%02d\n", dt.year, dt.month, dt.day, dt.hour, dt.min, dt.sec);
        ::sleep_ms(1000);
    }
}
