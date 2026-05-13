#include <stdio.h>
#include "pico/stdlib.h"
#include "jxglib/GPIO.h"

using namespace jxglib;

int main()
{
// mkdocs-start:[GPIO0]
GPIO0.set_function_SPI0_RX();
GPIO0.set_function_UART0_TX();
GPIO0.set_function_I2C0_SDA();
GPIO0.set_function_PWM0_A();
GPIO0.set_function_USB_OVCUR_DET();
// mkdocs-end:[GPIO0]
// mkdocs-start:[GPIO1]
GPIO1.set_function_SPI0_CSn();
GPIO1.set_function_UART0_RX();
GPIO1.set_function_I2C0_SCL();
GPIO1.set_function_PWM0_B();
GPIO1.set_function_USB_VBUS_DET();
// mkdocs-end:[GPIO1]
// mkdocs-start:[GPIO2]
GPIO2.set_function_SPI0_SCK();
GPIO2.set_function_UART0_CTS();
GPIO2.set_function_I2C1_SDA();
GPIO2.set_function_PWM1_A();
GPIO2.set_function_USB_VBUS_DET();
// mkdocs-end:[GPIO2]
// mkdocs-start:[GPIO3]
GPIO3.set_function_SPI0_TX();
GPIO3.set_function_UART0_RTS();
GPIO3.set_function_I2C1_SCL();
GPIO3.set_function_PWM1_B();
GPIO3.set_function_USB_OVCUR_DET();
// mkdocs-end:[GPIO3]
// mkdocs-start:[GPIO4]
GPIO4.set_function_SPI0_RX();
GPIO4.set_function_UART1_TX();
GPIO4.set_function_I2C0_SDA();
GPIO4.set_function_PWM2_A();
GPIO4.set_function_USB_VBUS_DET();
// mkdocs-end:[GPIO4]
// mkdocs-start:[GPIO5]
GPIO5.set_function_SPI0_CSn();
GPIO5.set_function_UART1_RX();
GPIO5.set_function_I2C0_SCL();
GPIO5.set_function_PWM2_B();
GPIO5.set_function_USB_VBUS_EN();
// mkdocs-end:[GPIO5]
// mkdocs-start:[GPIO6]
GPIO6.set_function_SPI0_SCK();
GPIO6.set_function_UART1_CTS();
GPIO6.set_function_I2C1_SDA();
GPIO6.set_function_PWM3_A();
GPIO6.set_function_USB_OVCUR_DET();
// mkdocs-end:[GPIO6]
// mkdocs-start:[GPIO7]
GPIO7.set_function_SPI0_TX();
GPIO7.set_function_UART1_RTS();
GPIO7.set_function_I2C1_SCL();
GPIO7.set_function_PWM3_B();
GPIO7.set_function_USB_VBUS_DET();
// mkdocs-end:[GPIO7]
// mkdocs-start:[GPIO8]
GPIO8.set_function_SPI1_RX();
GPIO8.set_function_UART1_TX();
GPIO8.set_function_I2C0_SDA();
GPIO8.set_function_PWM4_A();
GPIO8.set_function_USB_VBUS_EN();
// mkdocs-end:[GPIO8]
// mkdocs-start:[GPIO9]
GPIO9.set_function_SPI1_CSn();
GPIO9.set_function_UART1_RX();
GPIO9.set_function_I2C0_SCL();
GPIO9.set_function_PWM4_B();
GPIO9.set_function_USB_OVCUR_DET();
// mkdocs-end:[GPIO9]
// mkdocs-start:[GPIO10]
GPIO10.set_function_SPI1_SCK();
GPIO10.set_function_UART1_CTS();
GPIO10.set_function_I2C1_SDA();
GPIO10.set_function_PWM5_A();
GPIO10.set_function_USB_VBUS_DET();
// mkdocs-end:[GPIO10]
// mkdocs-start:[GPIO11]
GPIO11.set_function_SPI1_TX();
GPIO11.set_function_UART1_RTS();
GPIO11.set_function_I2C1_SCL();
GPIO11.set_function_PWM5_B();
GPIO11.set_function_USB_VBUS_EN();
// mkdocs-end:[GPIO11]
// mkdocs-start:[GPIO12]
GPIO12.set_function_SPI1_RX();
GPIO12.set_function_UART0_TX();
GPIO12.set_function_I2C0_SDA();
GPIO12.set_function_PWM6_A();
GPIO12.set_function_USB_OVCUR_DET();
// mkdocs-end:[GPIO12]
// mkdocs-start:[GPIO13]
GPIO13.set_function_SPI1_CSn();
GPIO13.set_function_UART0_RX();
GPIO13.set_function_I2C0_SCL();
GPIO13.set_function_PWM6_B();
GPIO13.set_function_USB_VBUS_DET();
// mkdocs-end:[GPIO13]
// mkdocs-start:[GPIO14]
GPIO14.set_function_SPI1_SCK();
GPIO14.set_function_UART0_CTS();
GPIO14.set_function_I2C1_SDA();
GPIO14.set_function_PWM7_A();
GPIO14.set_function_USB_VBUS_EN();
// mkdocs-end:[GPIO14]
// mkdocs-start:[GPIO15]
GPIO15.set_function_SPI1_TX();
GPIO15.set_function_UART0_RTS();
GPIO15.set_function_I2C1_SCL();
GPIO15.set_function_PWM7_B();
GPIO15.set_function_USB_OVCUR_DET();
// mkdocs-end:[GPIO15]
// mkdocs-start:[GPIO16]
GPIO16.set_function_SPI0_RX();
GPIO16.set_function_UART0_TX();
GPIO16.set_function_I2C0_SDA();
GPIO16.set_function_PWM0_A();
GPIO16.set_function_USB_VBUS_DET();
// mkdocs-end:[GPIO16]
// mkdocs-start:[GPIO17]
GPIO17.set_function_SPI0_CSn();
GPIO17.set_function_UART0_RX();
GPIO17.set_function_I2C0_SCL();
GPIO17.set_function_PWM0_B();
GPIO17.set_function_USB_VBUS_EN();
// mkdocs-end:[GPIO17]
// mkdocs-start:[GPIO18]
GPIO18.set_function_SPI0_SCK();
GPIO18.set_function_UART0_CTS();
GPIO18.set_function_I2C1_SDA();
GPIO18.set_function_PWM1_A();
GPIO18.set_function_USB_OVCUR_DET();
// mkdocs-end:[GPIO18]
// mkdocs-start:[GPIO19]
GPIO19.set_function_SPI0_TX();
GPIO19.set_function_UART0_RTS();
GPIO19.set_function_I2C0_SCL();
GPIO19.set_function_PWM1_B();
GPIO19.set_function_USB_VBUS_DET();
// mkdocs-end:[GPIO19]
// mkdocs-start:[GPIO20]
GPIO20.set_function_SPI0_RX();
GPIO20.set_function_UART1_TX();
GPIO20.set_function_I2C0_SDA();
GPIO20.set_function_PWM2_A();
GPIO20.set_function_CLOCK_GPIN0();
GPIO20.set_function_USB_VBUS_DET();
// mkdocs-end:[GPIO20]
// mkdocs-start:[GPIO21]
GPIO21.set_function_SPI0_CSn();
GPIO21.set_function_UART1_RX();
GPIO21.set_function_I2C0_SCL();
GPIO21.set_function_PWM2_B();
GPIO21.set_function_CLOCK_GPOUT0();
GPIO21.set_function_USB_OVCUR_DET();
// mkdocs-end:[GPIO21]
// mkdocs-start:[GPIO22]
GPIO22.set_function_SPI0_SCK();
GPIO22.set_function_UART1_CTS();
GPIO22.set_function_I2C1_SDA();
GPIO22.set_function_PWM3_A();
GPIO22.set_function_CLOCK_GPIN1();
GPIO22.set_function_USB_VBUS_DET();
// mkdocs-end:[GPIO22]
// mkdocs-start:[GPIO23]
GPIO23.set_function_SPI0_TX();
GPIO23.set_function_UART1_RTS();
GPIO23.set_function_I2C1_SCL();
GPIO23.set_function_PWM3_B();
GPIO23.set_function_CLOCK_GPOUT1();
GPIO23.set_function_USB_VBUS_EN();
// mkdocs-end:[GPIO23]
// mkdocs-start:[GPIO24]
GPIO24.set_function_SPI1_RX();
GPIO24.set_function_UART1_TX();
GPIO24.set_function_I2C1_SDA();
GPIO24.set_function_PWM4_A();
GPIO24.set_function_CLOCK_GPOUT2();
GPIO24.set_function_USB_OVCUR_DET();
// mkdocs-end:[GPIO24]
// mkdocs-start:[GPIO25]
GPIO25.set_function_SPI1_CSn();
GPIO25.set_function_UART1_RX();
GPIO25.set_function_I2C0_SCL();
GPIO25.set_function_PWM4_B();
GPIO25.set_function_CLOCK_GPOUT3();
GPIO25.set_function_USB_VBUS_DET();
// mkdocs-end:[GPIO25]
// mkdocs-start:[GPIO26]
GPIO26.set_function_SPI1_SCK();
GPIO26.set_function_UART0_CTS();
GPIO26.set_function_I2C1_SDA();
GPIO26.set_function_PWM5_A();
GPIO26.set_function_USB_VBUS_EN();
// mkdocs-end:[GPIO26]
// mkdocs-start:[GPIO27]
GPIO27.set_function_SPI1_TX();
GPIO27.set_function_UART1_RTS();
GPIO27.set_function_I2C1_SCL();
GPIO27.set_function_PWM5_B();
GPIO27.set_function_USB_OVCUR_DET();
// mkdocs-end:[GPIO27]
// mkdocs-start:[GPIO28]
GPIO28.set_function_SPI1_RX();
GPIO28.set_function_UART0_TX();
GPIO28.set_function_I2C0_SDA();
GPIO28.set_function_PWM6_A();
GPIO28.set_function_USB_VBUS_DET();
// mkdocs-end:[GPIO28]
// mkdocs-start:[GPIO29]
GPIO29.set_function_SPI1_CSn();
GPIO29.set_function_UART0_RX();
GPIO29.set_function_I2C0_SCL();
GPIO29.set_function_PWM6_B();
GPIO29.set_function_USB_VBUS_EN();
// mkdocs-end:[GPIO29]
}
