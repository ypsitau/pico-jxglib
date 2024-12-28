#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/pwm.h"
#include "jxglib/Common.h"

using namespace jxglib;

int main()
{
    ::stdio_init_all();
    GPIO0.set_function_PWM0_A().pwm_set_level(1).pwm_set_wrap_slice(3).pwm_set_enabled_slice(true);
    GPIO1.set_function_PWM0_B().pwm_set_level(3);
    GPIO2.set_function_PWM1_A();
    GPIO3.set_function_PWM1_B();
    GPIO4.set_function_PWM2_A();
    GPIO5.set_function_PWM2_B();
    GPIO6.set_function_PWM3_A();
    GPIO7.set_function_PWM3_B();
    GPIO8.set_function_PWM4_A();
    GPIO9.set_function_PWM4_B();
    GPIO10.set_function_PWM5_A();
    GPIO11.set_function_PWM5_B();
    GPIO12.set_function_PWM6_A();
    GPIO13.set_function_PWM6_B();
    GPIO14.set_function_PWM7_A();
    GPIO15.set_function_PWM7_B();
    GPIO16.set_function_PWM0_A();
    GPIO17.set_function_PWM0_B();
    GPIO18.set_function_PWM1_A();
    GPIO19.set_function_PWM1_B();
    GPIO20.set_function_PWM2_A();
    GPIO21.set_function_PWM2_B();
    GPIO22.set_function_PWM3_A();
    GPIO23.set_function_PWM3_B();
    GPIO24.set_function_PWM4_A();
    GPIO25.set_function_PWM4_B();
    GPIO26.set_function_PWM5_A();
    GPIO27.set_function_PWM5_B();
    GPIO28.set_function_PWM6_A();
    GPIO29.set_function_PWM6_B();
}
