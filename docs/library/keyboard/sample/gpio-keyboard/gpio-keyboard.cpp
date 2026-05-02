#include <stdio.h>
#include "pico/stdlib.h"
#include "jxglib/Common.h"

using namespace jxglib;

int main()
{
    ::stdio_init_all();
    GPIO::Key keyTbl[] = {
        { GPIO10.pull_up(), GPIO::LogicNeg, VK_LEFT     },
        { GPIO11.pull_up(), GPIO::LogicNeg, VK_UP       },
        { GPIO12.pull_up(), GPIO::LogicNeg, VK_DOWN     },
        { GPIO13.pull_up(), GPIO::LogicNeg, VK_RIGHT    },
        { GPIO14.pull_up(), GPIO::LogicNeg, VK_A        },
        { GPIO15.pull_up(), GPIO::LogicNeg, VK_B        },
    };
    GPIO::Keyboard keyboard;
    keyboard.Initialize(keyTbl, count_of(keyTbl));
    for (;;) {
        KeyData keyData;
        if (keyboard.GetKeyDataNB(&keyData)) {
            char ch = keyData.GetChar();
            if (ch) {
                ::printf("Char:    '%c'\n", ch);
            } else {
                ::printf("KeyCode: 0x%02x (modifier: %03b)\n", keyData.GetKeyCode(), keyData.GetModifier());
            }
        }
        Tickable::Sleep(100);
    }
}
