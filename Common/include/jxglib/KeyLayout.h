//==============================================================================
// jxglib/KeyLayout.h
//==============================================================================
#ifndef PICO_JXGLIB_KEYLAYOUT_H
#define PICO_JXGLIB_KEYLAYOUT_H
#include "pico/stdlib.h"
#include "jxglib/KeyCode.h"
#include "jxglib/KeyData.h"

namespace jxglib {

//------------------------------------------------------------------------------
// KeyLayout
//------------------------------------------------------------------------------
class KeyLayout {
public:
    struct CharEntry {
        char charCode;
        char charCodeShift;
    };
private:
    bool nonUSFlag_;
public:
    KeyLayout(bool nonUSFlag) : nonUSFlag_{nonUSFlag} {}
public:
    bool IsNonUS() const { return nonUSFlag_; }
    KeyData CreateKeyData(uint8_t keyCode, uint8_t modifier) const;
public:
    virtual const CharEntry* GetCharEntryTbl() const = 0;
    virtual uint8_t ConvKeyCodeToCharCode(uint8_t keyCode, uint8_t modifier) const;
};

//------------------------------------------------------------------------------
// KeyLayout_101
//------------------------------------------------------------------------------
class KeyLayout_101 : public KeyLayout {
public:
    static const KeyLayout_101 Instance;
public:
    KeyLayout_101() : KeyLayout(false) {}
    virtual const CharEntry* GetCharEntryTbl() const;
};
    
//------------------------------------------------------------------------------
// KeyLayout_106
//------------------------------------------------------------------------------
class KeyLayout_106 : public KeyLayout {
public:
    static const KeyLayout_106 Instance;
public:
    KeyLayout_106() : KeyLayout(true) {}
    virtual const CharEntry* GetCharEntryTbl() const;
};
        
}

#endif
