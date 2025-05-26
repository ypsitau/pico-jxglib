//==============================================================================
// Tokenizer.cpp
//==============================================================================
#include <ctype.h>
#include "jxglib/Tokenizer.h"

namespace jxglib {

//------------------------------------------------------------------------------
// Tokenizer
//------------------------------------------------------------------------------
Tokenizer::Tokenizer()
{
}

bool Tokenizer::Tokenize(char* str, int *pnToken, char* tokenTbl[], const char** pErrMsg)
{
	Stat stat = Stat::Head;
	int nTokenMax = *pnToken;
	int& nToken = *pnToken;
	nToken = 0;
	bool contFlag = true;
	for (char* p = str; contFlag; p++) {
		switch (stat) {
		case Stat::Head: {
			if (!*p) {
				contFlag = false;
			} else if (isspace(*p)) {
				// nothing to do
			} else if (nToken >= nTokenMax - 1) {
				*pErrMsg = "too many tokens";
				return false;
			} else if (*p == '"') {
				tokenTbl[nToken++] = p + 1;
				stat = Stat::Quoted;
			} else if (*p == '\\') {
				tokenTbl[nToken++] = p;
				DeleteChar(p);
				stat = Stat::NoQuotedEscape;
			} else {
				tokenTbl[nToken++] = p;
				stat = Stat::NoQuoted;
			}
			break;
		}
		case Stat::Quoted: {
			if (!*p) {
				*pErrMsg = "unmatched double quotation";
				return false;
			} else if (*p == '\\') {
				DeleteChar(p);
				stat = Stat::QuotedEscape;
			} else if (*p == '"') {
				*p = '\0';
				stat = Stat::Head;
			} else {
				// nothing to do
			}
			break;
		}
		case Stat::QuotedEscape: {
			if (!*p) {
				*pErrMsg = "unmatched double quotation";
				return false;
			} else {
				stat = Stat::Quoted;
			}
			break;
		}
		case Stat::NoQuoted: {
			if (!*p) {
				contFlag = false;
			} else if (*p == '\\') {
				DeleteChar(p);
				stat = Stat::NoQuotedEscape;
			} else if (isspace(*p)) {
				*p = '\0';
				stat = Stat::Head;
			} else {
				// nothing to do
			}
			break;
		}
		case Stat::NoQuotedEscape: {
			if (!*p) {
				contFlag = false;
			} else {
				stat = Stat::NoQuoted;
			}
			break;
		}
		}
	}
	tokenTbl[nToken] = nullptr;
	return true;
}

const char* Tokenizer::FindLastToken(const char* str)
{
	Stat stat = Stat::Head;
	bool contFlag = true;
	const char* tokenLast = str;
	for (const char* p = str; contFlag; p++) {
		switch (stat) {
		case Stat::Head: {
			if (!*p) {
				contFlag = false;
			} else if (isspace(*p)) {
				tokenLast = p + 1;
			} else if (*p == '"') {
				stat = Stat::Quoted;
			} else if (*p == '\\') {
				stat = Stat::NoQuotedEscape;
			} else {
				stat = Stat::NoQuoted;
			}
			break;
		}
		case Stat::Quoted: {
			if (!*p) {
				return tokenLast;	// error: unmatched double quotation
			} else if (*p == '\\') {
				stat = Stat::QuotedEscape;
			} else if (*p == '"') {
				stat = Stat::Head;
			} else {
				// nothing to do
			}
			break;
		}
		case Stat::QuotedEscape: {
			if (!*p) {
				return tokenLast; // error:
			} else {
				stat = Stat::Quoted;
			}
			break;
		}
		case Stat::NoQuoted: {
			if (!*p) {
				contFlag = false;
			} else if (*p == '\\') {
				stat = Stat::NoQuotedEscape;
			} else if (isspace(*p)) {
				tokenLast = p + 1;
				stat = Stat::Head;
			} else {
				// nothing to do
			}
			break;
		}
		case Stat::NoQuotedEscape: {
			if (!*p) {
				contFlag = false;
			} else {
				stat = Stat::NoQuoted;
			}
			break;
		}
		}
	}
	return tokenLast;
}

}
