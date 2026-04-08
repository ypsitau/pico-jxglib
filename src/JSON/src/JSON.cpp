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
JSON::JSON() : stat_{Stat::Value}, strictFlag_{false}, context_{Context::ArrayValue},
	iLine_{0}, iBuff_{0}, iGroupStack_{0}, nCodeDigits_{0}, codeAccum_{0}, errorMsg_{""}
{
}

void JSON::Reset()
{
	stat_ = Stat::Value;
	context_ = Context::ArrayValue;
	iLine_ = 0;
	iBuff_ = 0;
	iGroupStack_ = 0;
	nCodeDigits_ = 0;
	codeAccum_ = 0;
	errorMsg_ = "";
}

bool JSON::Parse(const char* str)
{
	for (const char* p = str; *p; p++) {
		if (!FeedChar(*p)) return false;
	}
	return true;
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
	const char* errorMsg_ObjectName = "Object name must be a string";

	bool contFlag = true;
	while (contFlag) {
		contFlag = false;
		switch (stat_) {
		case Stat::Value: {
			if (IsWhitespace(ch)) {
				// skip
			} else if (ch == '"') {
				ClearBuff();
				stat_ = Stat::String;
			} else if (ch == '-' || IsDigit(ch)) {
				if (context_ == Context::ObjectName) {
					errorMsg_ = errorMsg_ObjectName;
					return false;
				}
				ClearBuff();
				contFlag = true;
				stat_ = Stat::Number;
			} else if (ch == '{') {
				if (context_ == Context::ObjectName) {
					errorMsg_ = errorMsg_ObjectName;
					return false;
				}
				if (iGroupStack_ >= count_of(groupStack_)) {
					errorMsg_ = "object/array stack overflow";
					return false;
				}
				groupStack_[iGroupStack_++] = '{';
				ReportObjectStart();
				context_ = Context::ObjectName;
			} else if (ch == '}') {
				if (iGroupStack_ > 0 && groupStack_[iGroupStack_ - 1] == '{') {
					iGroupStack_--;
					ReportObjectEnd();
					context_ = (iGroupStack_ > 0 && groupStack_[iGroupStack_ - 1] == '{')?
														Context::ObjectValue : Context::ArrayValue;
					stat_ = Stat::SeekNextValue;
				} else {
					errorMsg_ = "Unmatched closing brace";
					return false;
				}
			} else if (ch == '[') {
				if (context_ == Context::ObjectName) {
					errorMsg_ = errorMsg_ObjectName;
					return false;
				}
				if (iGroupStack_ >= count_of(groupStack_)) {
					errorMsg_ = "object/array stack overflow";
					return false;
				}
				groupStack_[iGroupStack_++] = '[';
				ReportArrayStart();
				context_ = Context::ArrayValue;
			} else if (ch == ']') {
				if (iGroupStack_ > 0 && groupStack_[iGroupStack_ - 1] == '[') {
					iGroupStack_--;
					ReportArrayEnd();
					context_ = (iGroupStack_ > 0 && groupStack_[iGroupStack_ - 1] == '{')?
														Context::ObjectValue : Context::ArrayValue;
					stat_ = Stat::SeekNextValue;
				} else {
					errorMsg_ = "Unmatched closing bracket";
					return false;
				}
			} else if (IsSymbolCharFirst(ch)) {
				if (context_ == Context::ObjectName) {
					errorMsg_ = errorMsg_ObjectName;
					return false;
				}
				ClearBuff();
				if (!AddBuff(ch)) return false;
				stat_ = Stat::Symbol;
			}
			break;
		}
		case Stat::SeekNextValue: {
			if (IsWhitespace(ch)) {
				// skip
			} else if (ch == ',') {
				context_ = (context_ == Context::ObjectValue)? Context::ObjectName : Context::ArrayValue;
				stat_ = Stat::Value;
			} else if (ch == '}') {
				contFlag = true;
				stat_ = Stat::Value;
			} else if (ch == ']') {
				contFlag = true;
				stat_ = Stat::Value;
			} else {
				errorMsg_ = "Unexpected character";
				return false;
			}
			break;
		}
		case Stat::SeekObjectValue: {
			if (IsWhitespace(ch)) {
				// skip
			} else if (ch == ':') {
				context_ = Context::ObjectValue;
				stat_ = Stat::Value;
			} else {
				errorMsg_ = "Colon is expected";
				return false;
			}
			break;
		}
		case Stat::String: {
			if (ch == '\\') {
				stat_ = Stat::String_Escape;
			} else if (ch == '"') {
				if (context_ == Context::ObjectName) {
					::strcpy(objectName_, TerminateBuff());
					stat_ = Stat::SeekObjectValue;
				} else {
					ReportString();
					stat_ = Stat::SeekNextValue;
				}
			} else {
				if (!AddBuff(ch)) return false;
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
				stat_ = Stat::Number_ExponentSign;
			} else {
				ReportNumber();
				contFlag = true;
				stat_ = Stat::SeekNextValue;
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
				stat_ = Stat::Number_ExponentSign;
			} else {
				ReportNumber();
				contFlag = true;
				stat_ = Stat::SeekNextValue;
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
				ReportNumber();
				contFlag = true;
				stat_ = Stat::SeekNextValue;
			}
			break;
		}
		case Stat::Symbol: {
			if (IsSymbolChar(ch)) {
				if (!AddBuff(ch)) return false;
			} else {
				ReportSymbol();
				contFlag = true;
				stat_ = Stat::SeekNextValue;
			}
			break;
		}
		}
	}
	if (ch == '\n') iLine_++;
	return true;
}

void JSON::ReportString()
{
	if (context_ == Context::ObjectValue) {
		OnStringNamed(objectName_, TerminateBuff());
	} else {
		OnString(TerminateBuff());
	}
}

void JSON::ReportNumber()
{
	if (context_ == Context::ObjectValue) {
		OnNumberNamed(objectName_, ::strtod(TerminateBuff(), nullptr));
	} else {
		OnNumber(::strtod(TerminateBuff(), nullptr));
	}
}

void JSON::ReportSymbol()
{
	if (context_ == Context::ObjectValue) {
		OnSymbolNamed(objectName_, TerminateBuff());
	} else {
		OnSymbol(TerminateBuff());
	}
}

void JSON::ReportObjectStart()
{
	if (context_ == Context::ObjectValue) {
		OnObjectStartNamed(objectName_);
	} else {
		OnObjectStart();
	}
}

void JSON::ReportObjectEnd()
{
	OnObjectEnd();
}

void JSON::ReportArrayStart()
{
	if (context_ == Context::ObjectValue) {
		OnArrayStartNamed(objectName_);
	} else {
		OnArrayStart();
	}
}

void JSON::ReportArrayEnd()
{
	OnArrayEnd();
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

//------------------------------------------------------------------------------
// JSON_Debug
//------------------------------------------------------------------------------
void JSON_Debug::OnString(const char* str)
{
	::printf("%*s\"%s\"\n", indentLevel_ * 2, "", str);
}

void JSON_Debug::OnStringNamed(const char* objectName, const char* str)
{
	::printf("%*s\"%s\" : \"%s\"\n", indentLevel_ * 2, "", objectName, str);
}

void JSON_Debug::OnNumber(double num)
{
	::printf("%*s%f\n", indentLevel_ * 2, "", num);
}

void JSON_Debug::OnNumberNamed(const char* objectName, double num)
{
	::printf("%*s\"%s\" : %f\n", indentLevel_ * 2, "", objectName, num);
}

void JSON_Debug::OnSymbol(const char* symbol)
{
	::printf("%*s`%s`\n", indentLevel_ * 2, "", symbol);
}

void JSON_Debug::OnSymbolNamed(const char* objectName, const char* symbol)
{
	::printf("%*s\"%s\" : `%s`\n", indentLevel_ * 2, "", objectName, symbol);
}

void JSON_Debug::OnObjectStart()
{
	::printf("%*s{\n", indentLevel_ * 2, "");
	indentLevel_++;
}

void JSON_Debug::OnObjectStartNamed(const char* objectName)
{
	::printf("%*s\"%s\" : {\n", indentLevel_ * 2, "", objectName);
	indentLevel_++;
}

void JSON_Debug::OnObjectEnd()
{
	indentLevel_--;
	::printf("%*s}\n", indentLevel_ * 2, "");
}

void JSON_Debug::OnArrayStart()
{
	::printf("%*s[\n", indentLevel_ * 2, "");
	indentLevel_++;
}

void JSON_Debug::OnArrayStartNamed(const char* objectName)
{
	::printf("%*s\"%s\" : [\n", indentLevel_ * 2, "", objectName);
	indentLevel_++;
}

void JSON_Debug::OnArrayEnd()
{
	indentLevel_--;
	::printf("%*s]\n", indentLevel_ * 2, "");
}

}
