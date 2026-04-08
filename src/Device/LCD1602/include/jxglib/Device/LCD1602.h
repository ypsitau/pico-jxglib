//==============================================================================
// jxglib/Device/LCD1602.h
//==============================================================================
#ifndef PICO_JXGLIB_DEVICE_LCD1602_H
#define PICO_JXGLIB_DEVICE_LCD1602_H
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "jxglib/Common.h"
#include "jxglib/Printable.h"

namespace jxglib::Device {

//------------------------------------------------------------------------------
// LCD1602
//------------------------------------------------------------------------------
struct LCD1602 : public Printable {
public:
	class Raw {
	private:
		i2c_inst_t* i2c_;
		uint8_t addr_;
	public:
		Raw(i2c_inst_t* i2c, uint8_t addr) : i2c_{i2c}, addr_{addr} {}
	public:
		uint8_t GetAddr() const { return addr_; }
	public:
		void SendRawByte(uint8_t value) const {
			::i2c_write_blocking(i2c_, addr_, &value, 1, false);
		}
		void SendNibble(uint8_t value) {
			const uint64_t usecDelay = 600;
			SendRawByte(value | (0 << 2));
			::sleep_us(usecDelay);
			SendRawByte(value | (1 << 2));
			::sleep_us(usecDelay);
			SendRawByte(value | (0 << 2));
			::sleep_us(usecDelay);
		}
		void SendByte(uint8_t value, uint8_t mode) {
			// mode bit3: Backlight
			//      bit2: Clock
			//      bit1: R/W (1: Read, 0: Write)
			//      bit0: RS (1: Data, 0: Instruction)
			mode |= (1 << 3);	// Backlight
			SendNibble((value & 0xf0) | mode);
			SendNibble((value << 4) | mode);
		}
		void SendInstruction(uint8_t inst) { SendByte(inst, 0); }
	public:
		void ClearDisplay() {
			SendInstruction(0x01);
		}
		void ReturnHome(uint8_t dummy = 0) {
			SendInstruction(0x02 | dummy);
		}
		void EntryModeSet(uint8_t shiftDir, uint8_t shiftEnable) {
			SendInstruction(0x04 | (shiftDir << 1) | (shiftEnable << 0));
		}
		void DisplayOnOffControl(uint8_t displayEnable, uint8_t cursorEnable, uint8_t blinkEnable) {
			SendInstruction(0x08 | (displayEnable << 2) | (cursorEnable << 1) | (blinkEnable << 0));
		}
		void CursorOrDisplayShift(uint8_t shiftDir, uint8_t displayShiftEnable) {
			SendInstruction(0x10 | (shiftDir << 3) | (displayShiftEnable << 2));
		}
		void FunctionSet(uint8_t dataLength, uint8_t twoLineEnable, uint8_t largeFont) {
			SendInstruction(0x20 | (dataLength << 4) | (twoLineEnable << 3) | (largeFont << 2));
		}
		void SetCharacterGeneratorRAMAddress(uint8_t addr) {
			SendInstruction(0x40 | addr);
		}
		void SetDisplayDataRAMAddress(uint8_t addr) {
			SendInstruction(0x80 | addr);
		}
		void WriteData(uint8_t data) {
			SendByte(data, 1);
		}
	};
public:
	static const uint8_t DefaultAddr = 0x27;
	static const int nCols = 16;
	static const int nRows = 2;
public:
	Raw raw;
private:
	int x_, y_;
	char chPrev_;
	uint8_t vram_[nCols * nRows];
public:
	LCD1602(i2c_inst_t* i2c, uint8_t addr = DefaultAddr) : raw(i2c, addr), x_{0}, y_{0}, chPrev_{'\0'} {}
public:
	void Initialize();
public:
	// Virtual functions of Printable
	Printable& ClearScreen() override;
	bool Flush() override;
	Printable& Locate(int x, int y) override;
	Printable& Print(const char* str) override;
	Printable& PutCharRaw(char ch) override;
public:
	static uint8_t CalcAddr(int x, int y) { return static_cast<uint8_t>((y << 6) | x); }
};

}

#endif
