//==============================================================================
// Tokenizer.cpp
//==============================================================================
#include <ctype.h>
#include "jxglib/Tokenizer.h"

namespace jxglib {

//------------------------------------------------------------------------------
// Tokenizer
//------------------------------------------------------------------------------
const Tokenizer Tokenizer::Default;

Tokenizer::Tokenizer(const char** specialTokens, int nSpecialTokens) : specialTokens_{specialTokens}, nSpecialTokens_{nSpecialTokens}
{
}

bool Tokenizer::Tokenize(char* str, int bytesStr, char* tokenTbl[], int* pnToken, const char** pErrorMsg) const
{
	Stat stat = Stat::Head;
	int nTokenMax = *pnToken;
	int& nToken = *pnToken;
	nToken = 0;
	bool contFlag = true;
	int nCharsFwd = 1;
	char chQuoteInNoQuoted = '\0';
	for (char* p = str; contFlag; p += nCharsFwd) {
		nCharsFwd = 1;
		switch (stat) {
		case Stat::Head: {
			int nCharsToken;
			if (*p == '\0') {
				contFlag = false;
			} else if (isspace(*p)) {
				// nothing to do
			} else if (nToken >= nTokenMax - 1) {
				*pErrorMsg = "too many tokens";
				return false;
			} else if (*p == '"') {
				tokenTbl[nToken++] = p + 1;
				stat = Stat::Quoted;
			} else if (*p == '\\') {
				tokenTbl[nToken++] = p;
				DeleteChar(p); p--;
				stat = Stat::NoQuotedEscape;
			} else if ((nCharsToken = FindSpecialToken(p)) > 0) {
				tokenTbl[nToken++] = p;
				nCharsFwd = nCharsToken;
				stat = Stat::AfterSpecial;
			} else if (*p == '\'') {
				tokenTbl[nToken++] = p;
				chQuoteInNoQuoted = *p;
				stat = Stat::QuotedInNoQuoted;
			} else {
				tokenTbl[nToken++] = p;
				stat = Stat::NoQuoted;
			}
			break;
		}
		case Stat::Quoted: {
			if (*p == '\0') {
				*pErrorMsg = "unmatched double quotation";
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
				*pErrorMsg = "unmatched double quotation";
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
			int nCharsToken;
			if (*p == '\0') {
				contFlag = false;
			} else if (*p == '\\') {
				DeleteChar(p); p--;
				stat = Stat::NoQuotedEscape;
			} else if (*p == '"' || *p == '\'') {
				chQuoteInNoQuoted = *p;
				stat = Stat::QuotedInNoQuoted;
			} else if (isspace(*p)) {
				*p = '\0';
				stat = Stat::Head;
			} else if ((nCharsToken = FindSpecialToken(p)) > 0) {
				if (InsertChar(str, bytesStr, p)) *p++ = '\0';
				if (nToken >= nTokenMax - 1) {
					*pErrorMsg = "too many tokens";
					return false;
				}
				tokenTbl[nToken++] = p;
				nCharsFwd = nCharsToken;
				stat = Stat::AfterSpecial;
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
		case Stat::QuotedInNoQuoted: {
			if (*p == '\0') {
				contFlag = false;
			} else if (*p == '\\') {
				stat = Stat::QuotedInNoQuotedEscape;
			} else if (*p == chQuoteInNoQuoted) {
				stat = Stat::NoQuoted;
			}
			break;
		}
		case Stat::QuotedInNoQuotedEscape: {
			if (*p == '\0') {
				contFlag = false;
			} else {
				stat = Stat::QuotedInNoQuoted;
			}
			break;
		}
		case Stat::AfterSpecial: {
			if (*p == '\0') {
				contFlag = false;
			} else if (::isspace(*p) || InsertChar(str, bytesStr, p)) {
				*p = '\0';
				stat = Stat::Head;
			}
			break;
		}
		default: break;
		}
	}
	tokenTbl[nToken] = nullptr;
	return true;
}

const char* Tokenizer::FindLastToken(const char* str) const
{
	Stat stat = Stat::Head;
	bool contFlag = true;
	const char* tokenLast = str;
	int nCharsFwd = 1;
	char chQuoteInNoQuoted = '\0';
	for (const char* p = str; contFlag; p += nCharsFwd) {
		nCharsFwd = 1;
		switch (stat) {
		case Stat::Head: {
			int nCharsToken;
			if (*p == '\0') {
				contFlag = false;
			} else if (isspace(*p)) {
				tokenLast = p + 1;
			} else if (*p == '"') {
				stat = Stat::Quoted;
			} else if (*p == '\\') {
				stat = Stat::NoQuotedEscape;
			} else if ((nCharsToken = FindSpecialToken(p)) > 0) {
				tokenLast = p + nCharsToken;
				nCharsFwd = nCharsToken;
			} else if (*p == '\'') {
				chQuoteInNoQuoted = *p;
				stat = Stat::QuotedInNoQuoted;
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
			int nCharsToken;
			if (*p == '\0') {
				contFlag = false;
			} else if (*p == '\\') {
				stat = Stat::NoQuotedEscape;
			} else if (*p == '"' || *p == '\'') {
				chQuoteInNoQuoted = *p;
				stat = Stat::QuotedInNoQuoted;
			} else if (isspace(*p)) {
				tokenLast = p + 1;
				stat = Stat::Head;
			} else if ((nCharsToken = FindSpecialToken(p)) > 0) {
				tokenLast = p + nCharsToken;
				nCharsFwd = nCharsToken;
				stat = Stat::Head;
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
		case Stat::QuotedInNoQuoted: {
			if (*p == '\0') {
				contFlag = false;
			} else if (*p == '\\') {
				stat = Stat::QuotedInNoQuotedEscape;
			} else if (*p == chQuoteInNoQuoted) {
				stat = Stat::NoQuoted;
			}
			break;
		}
		case Stat::QuotedInNoQuotedEscape: {
			if (*p == '\0') {
				contFlag = false;
			} else {
				stat = Stat::QuotedInNoQuoted;
			}
			break;
		}
		case Stat::AfterSpecial: {	// this case never occurs in FindLastToken
			if (*p == '\0') {
				contFlag = false;
			} else {
				stat = Stat::Head;
			}
			break;
		}
		default: break;
		}
	}
	return tokenLast;
}

int Tokenizer::FindSpecialToken(const char* p) const
{
	if (!specialTokens_) return 0;
	for (int i = 0; i < nSpecialTokens_; ++i) {
		const char* specialToken = specialTokens_[i];
		int len = ::strlen(specialToken);
		if (::strncmp(p, specialToken, len) == 0) return len;
	}
	return 0;
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

void Tokenizer::RemoveSurroundingQuotes(char* token)
{
	int len = ::strlen(token);
	if (len < 2) return;
	char chFirst = token[0];
	if ((chFirst == '"' || chFirst == '\'') && token[len - 1] == chFirst) {
		token[len - 1] = '\0';
		::memmove(token, token + 1, len - 2 + 1);
	}
}

}
