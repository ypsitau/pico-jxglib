//==============================================================================
// GPIO.cpp
//==============================================================================
#include "jxglib/Common.h"

namespace jxglib {

//------------------------------------------------------------------------------
// GPIO
//------------------------------------------------------------------------------
const GPIO GPIO::None(-1);
const GPIO0_T GPIO0;
const GPIO1_T GPIO1;
const GPIO2_T GPIO2;
const GPIO3_T GPIO3;
const GPIO4_T GPIO4;
const GPIO5_T GPIO5;
const GPIO6_T GPIO6;
const GPIO7_T GPIO7;
const GPIO8_T GPIO8;
const GPIO9_T GPIO9;
const GPIO10_T GPIO10;
const GPIO11_T GPIO11;
const GPIO12_T GPIO12;
const GPIO13_T GPIO13;
const GPIO14_T GPIO14;
const GPIO15_T GPIO15;
const GPIO16_T GPIO16;
const GPIO17_T GPIO17;
const GPIO18_T GPIO18;
const GPIO19_T GPIO19;
const GPIO20_T GPIO20;
const GPIO21_T GPIO21;
const GPIO22_T GPIO22;
const GPIO23_T GPIO23;
const GPIO24_T GPIO24;
const GPIO25_T GPIO25;
const GPIO26_T GPIO26;
const GPIO27_T GPIO27;
const GPIO28_T GPIO28;
const GPIO29_T GPIO29;

//------------------------------------------------------------------------------
// GPIO::Key
//------------------------------------------------------------------------------
GPIO::Key::Key(const GPIO& gpio, uint8_t keyCode) : gpio_{gpio}, keyCode_{keyCode}
{
}

//------------------------------------------------------------------------------
// GPIO::Keyboard
//------------------------------------------------------------------------------
GPIO::Keyboard::Keyboard()
{
}

int GPIO::Keyboard::SenseKeyCode(uint8_t keyCodeTbl[], int nKeysMax)
{
    return 0;
}

}
