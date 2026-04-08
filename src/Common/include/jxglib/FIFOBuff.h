//==============================================================================
// jxglib/FIFOBuff.h
//==============================================================================
#ifndef PICO_JXGLIB_FIFOBUFF_H
#define PICO_JXGLIB_FIFOBUFF_H
#include <stdio.h>
#include "pico/stdlib.h"

namespace jxglib {

//------------------------------------------------------------------------------
// FIFOBuff
//------------------------------------------------------------------------------
template<typename T_Elem, int buffSize, typename T_Size = uint8_t> class FIFOBuff {
public:
	static constexpr T_Size sizeMinusOne = static_cast<T_Size>(buffSize - 1);
private:
	volatile T_Size posRead_;
	volatile T_Size posRewind_;
	volatile T_Size posWrite_;
	T_Elem buff_[buffSize];
public:
	FIFOBuff() : posRead_{0}, posRewind_{0}, posWrite_{0} {}
public:
	void Clear() { posRead_ = posRewind_ = posWrite_ = 0; }
	bool IsEmpty() const { return posRead_ == posWrite_; }
	bool HasData() const { return posRead_ != posWrite_; }
	T_Size GetReadPos() const { return posRead_; }
	T_Size GetWritePos() const { return posWrite_; }
	void SetRewindPosToWritePos() { posRewind_ = posWrite_; }
	void RewindReadPos() { posRead_ = posRewind_; }
	bool IsFull() const {
		T_Size posWriteNext = (posWrite_ == sizeMinusOne)? 0 : posWrite_ + 1;
		return posWriteNext == posRead_;
	}
	bool HasRoom() const { return !IsFull(); }
	T_Size GetLength() const {
		return (posRead_ <= posWrite_)? posWrite_ - posRead_ : buffSize - posRead_ + posWrite_;
	}
	int GetRoomLength() const { return sizeMinusOne - GetLength(); }
	bool WriteData(T_Elem data) {
		T_Size posWriteNext = (posWrite_ == sizeMinusOne)? 0 : posWrite_ + 1;
		if (posWriteNext == posRead_) return false;
		buff_[posWrite_] = data;
		posWrite_ = posWriteNext;
		return true;
	}
	T_Elem ReadData() {
		T_Elem data = buff_[posRead_];
		posRead_ = (posRead_ == sizeMinusOne)? 0 : posRead_ + 1;
		return data;
	}
	T_Size WriteBuff(const T_Elem* buff, T_Size len) {
		const T_Elem* p = buff;
		int lenWritten = 0;
		for ( ; lenWritten < len && WriteData(*p); lenWritten++, p++) ;
		return lenWritten;
	}
	T_Size ReadBuff(T_Elem* buff, T_Size len) {
		T_Elem* p = buff;
		int lenRead = 0;
		for ( ; lenRead < len && HasData(); lenRead++, p++) *p = ReadData();
		return lenRead;
	}
};

}

#endif
