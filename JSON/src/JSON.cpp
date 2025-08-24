//==============================================================================
// JSON.cpp
// https://www.json.org/json-en.html
//==============================================================================
#include <stdlib.h>
#include "jxglib/JSON.h"

namespace jxglib {

//------------------------------------------------------------------------------
// JSON
//------------------------------------------------------------------------------
JSON::JSON() : stat_{Stat::Value}, strictFlag_{false},
	iLine_{0}, iBuff_{0}, iGroupStack_{0}, nCodeDigits_{0}, codeAccum_{0},
	errorMsg_{""}, pHandler_{nullptr}
{
}

void JSON::Reset()
{
	stat_ = Stat::Value;
	iLine_ = 0;
	iBuff_ = 0;
	iGroupStack_ = 0;
	nCodeDigits_ = 0;
	codeAccum_ = 0;
	errorMsg_ = "";
}

bool JSON::Parse(Stream& stream)
{
	while (int ch = stream.ReadChar()) {
		if (ch < 0) break;
		if (!FeedChar((char)ch)) return false;
	}
	return true;
}

bool JSON::FeedChar(char ch)
{
	bool contFlag = true;
	while (contFlag) {
		contFlag = false;
		switch (stat_) {
		case Stat::Value: {
			if (IsWhitespace(ch)) {
				// skip
			} else if (ch == '"') {
				stat_ = Stat::String;
			} else if (ch == '-' || IsDigit(ch)) {
				ClearBuff();
				contFlag = true;
				stat_ = Stat::Number;
			} else if (ch == '{') {
				if (iGroupStack_ >= count_of(groupStack_)) {
					errorMsg_ = "object/array stack overflow";
					return false;
				}
				groupStack_[iGroupStack_++] = '{';
				if (pHandler_) pHandler_->OnObjectStart();
			} else if (ch == '}') {
				if (iGroupStack_ > 0 && groupStack_[iGroupStack_ - 1] == '{') {
					iGroupStack_--;
					if (pHandler_) pHandler_->OnObjectEnd();
				} else {
					errorMsg_ = "Unmatched closing brace";
					return false;
				}
			} else if (ch == '[') {
				if (iGroupStack_ >= count_of(groupStack_)) {
					errorMsg_ = "object/array stack overflow";
					return false;
				}
				groupStack_[iGroupStack_++] = '[';
				if (pHandler_) pHandler_->OnArrayStart();
			} else if (ch == ']') {
				if (iGroupStack_ > 0 && groupStack_[iGroupStack_ - 1] == '[') {
					iGroupStack_--;
					if (pHandler_) pHandler_->OnArrayEnd();
				} else {
					errorMsg_ = "Unmatched closing bracket";
					return false;
				}
			} else if (IsSymbolCharFirst(ch)) {
				ClearBuff();
				if (!AddBuff(ch)) return false;
				stat_ = Stat::Symbol;
			}
			break;
		}
		case Stat::String: {
			if (ch == '\\') {
				stat_ = Stat::String_Escape;
			} else if (ch == '"') {
				if (pHandler_) pHandler_->OnString(TerminateBuff());				
				stat_ = Stat::Value;
			}
			break;
		}
		case Stat::String_Escape: {
			if (ch == '"') {
				if (!AddBuff(ch)) return false;
				stat_ = Stat::String;
			} else if (ch == '\\') {
				if (!AddBuff(ch)) return false;
				stat_ = Stat::String;
			} else if (ch == '/') {
				if (!AddBuff(ch)) return false;
				stat_ = Stat::String;
			} else if (ch == 'b') {
				if (!AddBuff('\b')) return false;
				stat_ = Stat::String;
			} else if (ch == 'f') {
				if (!AddBuff('\f')) return false;
				stat_ = Stat::String;
			} else if (ch == 'n') {
				if (!AddBuff('\n')) return false;
				stat_ = Stat::String;
			} else if (ch == 'r') {
				if (!AddBuff('\r')) return false;
				stat_ = Stat::String;
			} else if (ch == 't') {
				if (!AddBuff('\t')) return false;
				stat_ = Stat::String;
			} else if (ch == 'u') {
				nCodeDigits_ = 0;
				codeAccum_ = 0;
				stat_ = Stat::String_Unicode;
			} else if (strictFlag_) {
				errorMsg_ = "Invalid character in string escape sequence";
				return false;
			} else {
				if (!AddBuff(ch)) return false;
				stat_ = Stat::String;
			}
			break;
		}
		case Stat::String_Unicode: {
			if (IsDigit(ch)) {
				codeAccum_ = (codeAccum_ << 4) | (ch - '0');
				nCodeDigits_++;
			} else if (ch >= 'a' && ch <= 'f') {
				codeAccum_ = (codeAccum_ << 4) | (ch - 'a' + 10);
				nCodeDigits_++;
			} else if (ch >= 'A' && ch <= 'F') {
				codeAccum_ = (codeAccum_ << 4) | (ch - 'A' + 10);
				nCodeDigits_++;
			} else if (strictFlag_) {
				errorMsg_ = "Invalid character in string unicode escape sequence";
				return false;
			} else {
				contFlag = true;
				stat_ = Stat::String;
			}
			if (nCodeDigits_ != 4) {
				// nothing to do
			} else if (codeAccum_ <= 0x7f) {
				if (!AddBuff((char)codeAccum_)) return false;
				stat_ = Stat::String;
			} else if (codeAccum_ <= 0x7ff) {
				if (!AddBuff((char)((codeAccum_ >> 6) | 0xc0))) return false;
				if (!AddBuff((char)((codeAccum_ & 0x3f) | 0x80))) return false;
				stat_ = Stat::String;
			} else if (codeAccum_ <= 0xffff) {
				if (!AddBuff((char)((codeAccum_ >> 12) | 0xe0))) return false;
				if (!AddBuff((char)((codeAccum_ >> 6) & 0x3f) | 0x80)) return false;
				if (!AddBuff((char)((codeAccum_ & 0x3f) | 0x80))) return false;
				stat_ = Stat::String;
			} else if (codeAccum_ <= 0x10ffff) {
				if (!AddBuff((char)((codeAccum_ >> 18) | 0xf0))) return false;
				if (!AddBuff((char)((codeAccum_ >> 12) & 0x3f) | 0x80)) return false;
				if (!AddBuff((char)((codeAccum_ >> 6) & 0x3f) | 0x80)) return false;
				if (!AddBuff((char)((codeAccum_ & 0x3f) | 0x80))) return false;
				stat_ = Stat::String;
			} else if (strictFlag_) {
				errorMsg_ = "Invalid code point in string unicode escape sequence";
				return false;
			} else {
				// ignore
				stat_ = Stat::String;
			}
			break;
		}
		case Stat::Number: {
			if (ch == '-') {
				if (!AddBuff(ch)) return false;
				stat_ = Stat::Number_IntegerTop;
			} else {
				contFlag = true;
				stat_ = Stat::Number_IntegerTop;
			}
			break;
		}
		case Stat::Number_IntegerTop: {
			if (ch == '0') {
				if (!AddBuff(ch)) return false;
				stat_ = Stat::Number_AfterInteger;
			} else if (IsDigit1_9(ch)) {
				if (!AddBuff(ch)) return false;
				stat_ = Stat::Number_Integer;
			} else {
				errorMsg_ = "Invalid character in number";
				return false;
			}
			break;
		}
		case Stat::Number_Integer: {
			if (IsDigit(ch)) {
				if (!AddBuff(ch)) return false;
			} else {
				contFlag = true;
				stat_ = Stat::Number_AfterInteger;
			}
			break;
		}
		case Stat::Number_AfterInteger: {
			if (ch == '.') {
				if (!AddBuff(ch)) return false;
				stat_ = Stat::Number_Fraction;
			} else if (ch == 'e' || ch == 'E') {
				if (!AddBuff(ch)) return false;
				stat_ = Stat::Number_Exponent;
			} else {
				if (pHandler_) pHandler_->OnNumber(::strtod(TerminateBuff(), nullptr));
				contFlag = true;
				stat_ = Stat::Value;
			}
			break;
		}
		case Stat::Number_Fraction: {
			if (IsDigit(ch)) {
				if (!AddBuff(ch)) return false;
			} else {
				contFlag = true;
				stat_ = Stat::Number_AfterFraction;
			}
			break;
		}
		case Stat::Number_AfterFraction: {
			if (ch == 'e' || ch == 'E') {
				if (!AddBuff(ch)) return false;
				stat_ = Stat::Number_Exponent;
			} else {
				if (pHandler_) pHandler_->OnNumber(::strtod(TerminateBuff(), nullptr));
				contFlag = true;
				stat_ = Stat::Value;
			}
			break;
		}
		case Stat::Number_ExponentSign: {
			if (ch == '+' || ch == '-') {
				if (!AddBuff(ch)) return false;
				stat_ = Stat::Number_ExponentTop;
			} else {
				contFlag = true;
				stat_ = Stat::Number_ExponentTop;
			}
			break;
		}
		case Stat::Number_ExponentTop: {
			if (IsDigit(ch)) {
				if (!AddBuff(ch)) return false;
				stat_ = Stat::Number_Exponent;
			} else {
				errorMsg_ = "Invalid character in number exponent";
				return false;
			}
			break;
		}
		case Stat::Number_Exponent: {
			if (IsDigit(ch)) {
				if (!AddBuff(ch)) return false;
			} else {
				if (pHandler_) pHandler_->OnNumber(::strtod(TerminateBuff(), nullptr));
				contFlag = true;
				stat_ = Stat::Value;
			}
			break;
		}
		case Stat::Symbol: {
			if (IsSymbolChar(ch)) {
				if (!AddBuff(ch)) return false;
			} else {
				if (pHandler_) pHandler_->OnSymbol(TerminateBuff());
				contFlag = true;
				stat_ = Stat::Value;
			}
			break;
		}
		}
	}
	if (ch == '\n') iLine_++;
	return true;
}

bool JSON::AddBuff(char ch)
{
	if (iBuff_ >= sizeof(buff_) - 1) {
		errorMsg_ = "Buffer overflow";
		return false;
	}
	buff_[iBuff_++] = ch;
	return true;
}

}
