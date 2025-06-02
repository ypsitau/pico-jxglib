//==============================================================================
// Tokenizer.cpp
//==============================================================================
#include <ctype.h>
#include "jxglib/Tokenizer.h"

namespace jxglib {

//------------------------------------------------------------------------------
// Tokenizer
//------------------------------------------------------------------------------
Tokenizer::Tokenizer(Mode mode) : mode_{mode}, errorMsg_{""}
{
}

bool Tokenizer::Tokenize(char* str, int bytesStr, char* tokenTbl[], int* pnToken)
{
	Stat stat = Stat::Head;
	int nTokenMax = *pnToken;
	int& nToken = *pnToken;
	nToken = 0;
	bool contFlag = true;
	for (char* p = str; contFlag; p++) {
		switch (stat) {
		case Stat::Head: {
			if (*p == '\0') {
				contFlag = false;
			} else if (isspace(*p)) {
				// nothing to do
			} else if (nToken >= nTokenMax - 1) {
				errorMsg_ = "too many tokens";
				return false;
			} else if (*p == '"') {
				tokenTbl[nToken++] = p + 1;
				stat = Stat::Quoted;
			} else if (*p == '\\') {
				tokenTbl[nToken++] = p;
				DeleteChar(p); p--;
				stat = Stat::NoQuotedEscape;
			} else {
				tokenTbl[nToken++] = p;
				stat = Stat::NoQuoted;
				if (mode_ != Mode::Shell) {
					// nothing to do
				} else if (*p == '>') {
					if (*(p + 1) == '>') {
						if (*(p + 2) == '\0') {
							p += 1;
						} else if (::isspace(*(p + 2))) {
							*(p + 2) = '\0';
							p += 2;
						} else if (InsertChar(str, bytesStr, p + 2)) {
							*(p + 2) = '\0';
							p += 2;
						} else {
							p += 1;
						}
					} else if (*(p + 1) == '\0') {
						// nothing to do
					} else if (::isspace(*(p + 1))) {
						*(p + 1) = '\0';
						p += 1;
					} else if (InsertChar(str, bytesStr, p + 1)) {
						*(p + 1) = '\0';
						p += 1;
					}
					stat = Stat::Head;
				}
			}
			break;
		}
		case Stat::Quoted: {
			if (*p == '\0') {
				errorMsg_ = "unmatched double quotation";
				return false;
			} else if (*p == '\\') {
				DeleteChar(p); p--;
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
			if (*p == '\0') {
				errorMsg_ = "unmatched double quotation";
				return false;
			} else if (*p == 'n') {
				*p = '\n';
				stat = Stat::Quoted;
			} else if (*p == 'r') {
				*p = '\r';
				stat = Stat::Quoted;
			} else if (*p == 't') {
				*p = '\t';
				stat = Stat::Quoted;
			} else {
				stat = Stat::Quoted;
			}
			break;
		}
		case Stat::NoQuoted: {
			if (*p == '\0') {
				contFlag = false;
			} else if (*p == '\\') {
				DeleteChar(p); p--;
				stat = Stat::NoQuotedEscape;
			} else if (isspace(*p)) {
				*p = '\0';
				stat = Stat::Head;
			} else if (mode_ == Mode::Shell) {
				if (*p == '>' && InsertChar(str, bytesStr, p)) {
					*p = '\0';
					stat = Stat::Head;
				}
			} else {
				// nothing to do
			}
			break;
		}
		case Stat::NoQuotedEscape: {
			if (*p == '\0') {
				contFlag = false;
			} else if (*p == 'n') {
				*p = '\n';
				stat = Stat::NoQuoted;
			} else if (*p == 'r') {
				*p = '\r';
				stat = Stat::NoQuoted;
			} else if (*p == 't') {
				*p = '\t';
				stat = Stat::NoQuoted;
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
			if (*p == '\0') {
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
			if (*p == '\0') {
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
			if (*p == '\0') {
				return tokenLast; // error:
			} else {
				stat = Stat::Quoted;
			}
			break;
		}
		case Stat::NoQuoted: {
			if (*p == '\0') {
				contFlag = false;
			} else if (*p == '\\') {
				stat = Stat::NoQuotedEscape;
			} else if (isspace(*p)) {
				tokenLast = p + 1;
				stat = Stat::Head;
			} else if (mode_ == Mode::Shell) {
				if (*p == '>') {
					if (*(p + 1) == '>') p++;
					tokenLast = p + 1;
					stat = Stat::Head;
				}
			} else {
				// nothing to do
			}
			break;
		}
		case Stat::NoQuotedEscape: {
			if (*p == '\0') {
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

void Tokenizer::DeleteChar(char* p)
{
	if (*p) ::memmove(p, p + 1, ::strlen(p + 1) + 1);
}

bool Tokenizer::InsertChar(char* str, int bytesStr, char* p)
{
	if (::strlen(str) + 1 >= bytesStr) return false;
	::memmove(p + 1, p, ::strlen(p) + 1);
	return true;
}

}
