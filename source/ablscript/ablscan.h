//===========================================================================//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//
//***************************************************************************
//
// SCANNER.H
//
//***************************************************************************

#pragma once

#ifndef ABLSCAN_H
#define ABLSCAN_H

//#include "ablgen.h"
//#include "ablerr.h"

namespace mclib::abl
{

//***************************************************************************

//#define MIN_INTEGER -2147483648
//#define MAX_INTEGER 2147483647

enum __ablscan_const : uint32_t
{
	TAB_SIZE = 4,
	MAX_DIGIT_COUNT = 20,
	MAX_EXPONENT = 20,
};

//***************************************************************************

//------------
// TOKEN CODES

enum class TokenCodeType : uint8_t
{
	TKN_NONE,
	TKN_IDENTIFIER,
	TKN_NUMBER,
	TKN_TYPE,
	TKN_STRING,
	TKN_STAR,
	TKN_LPAREN,
	TKN_RPAREN,
	TKN_MINUS,
	TKN_PLUS,
	TKN_EQUAL,
	TKN_LBRACKET,
	TKN_RBRACKET,
	TKN_COLON,
	TKN_SEMICOLON,
	TKN_LT,
	TKN_GT,
	TKN_COMMA,
	TKN_PERIOD,
	TKN_FSLASH,
	TKN_EQUALEQUAL,
	TKN_LE,
	TKN_GE,
	TKN_NE,
	TKN_EOF,
	TKN_ERROR,
	TKN_CODE,
	TKN_ORDER,
	TKN_STATE,
	TKN_AND,
	TKN_SWITCH,
	TKN_CASE,
	TKN_CONST,
	TKN_DIV,
	TKN_DO,
	TKN_OF,
	TKN_ELSE,
	TKN_END_IF,
	TKN_END_WHILE,
	TKN_END_FOR,
	TKN_END_FUNCTION,
	TKN_END_ORDER,
	TKN_END_STATE,
	TKN_END_MODULE,
	TKN_END_FSM,
	TKN_END_LIBRARY,
	TKN_END_VAR,
	TKN_END_CODE,
	TKN_END_CASE,
	TKN_END_SWITCH,
	TKN_FOR,
	TKN_FUNCTION,
	TKN_IF,
	TKN_MOD,
	TKN_NOT,
	TKN_OR,
	TKN_REPEAT,
	TKN_THEN,
	TKN_TO,
	TKN_UNTIL,
	TKN_VAR,
	TKN_REF,
	TKN_WHILE,
	TKN_ELSIF,
	TKN_RETURN,
	TKN_MODULE,
	TKN_FSM,
	TKN_TRANS,
	TKN_TRANS_BACK,
	TKN_LIBRARY,
	TKN_ETERNAL,
	TKN_STATIC,
	TKN_POUND,
	TKN_UNEXPECTED_TOKEN,
	TKN_STATEMENT_MARKER,
	TKN_ADDRESS_MARKER,
	NUM_TOKENS
};

enum class CharCodeType : uint8_t
{
	CHR_LETTER,
	CHR_DIGIT,
	CHR_DQUOTE,
	CHR_SPECIAL,
	CHR_EOF
};

typedef struct ReservedWord
{
	std::wstring_view string;
	TokenCodeType tokenCode;
} ReservedWord;

//***************************************************************************

//------------------
// LITERAL structure

enum class LiteralType : uint8_t
{
	LIT_INTEGER,
	LIT_REAL,
	LIT_STRING
};

struct Literal
{
	LiteralType type;
	struct
	{
		int32_t integer;
		float real;
		wchar_t string[MAXLEN_TOKENSTRING];
	} value;
};

//---------------------------------------------------------------------------

struct CaseItem
{
	int32_t labelValue;
	std::wstring_view branchLocation;
	// struct CaseItem* next;
};
// typedef CaseItem* const std::unique_ptr<CaseItem>&;

//***************************************************************************

class ABLFile
{

public:
	std::wstring_view fileName;
	PVOID file;

	static int32_t(__stdcall* createCB)(PVOID* file, std::wstring_view filename);
	static int32_t(__stdcall* openCB)(PVOID* file, std::wstring_view filename);
	static int32_t(__stdcall* closeCB)(PVOID* file);
	static bool(__stdcall* eofCB)(PVOID file);
	static int32_t(__stdcall* readCB)(PVOID file, uint8_t* buffer, int32_t length);
	static int32_t(__stdcall* readLongCB)(PVOID file);
	static int32_t(__stdcall* readStringCB)(PVOID file, uint8_t* buffer);
	static int32_t(__stdcall* readLineExCB)(PVOID file, uint8_t* buffer, int32_t maxLength);
	static int32_t(__stdcall* writeCB)(PVOID file, uint8_t* buffer, int32_t length);
	static int32_t(__stdcall* writeByteCB)(PVOID file, uint8_t byte);
	static int32_t(__stdcall* writeLongCB)(PVOID file, int32_t value);
	static int32_t(__stdcall* writeStringCB)(PVOID file, std::wstring_view buffer);

public:
	PVOID operator new(size_t ourSize);
	void operator delete(PVOID us);

	void init(void);

	ABLFile(void)
	{
		init(void);
	}

	void destroy(void);

	~ABLFile(void)
	{
		destroy(void);
	}

	int32_t set(PVOID fPtr)
	{
		file = fPtr;
	}

	PVOID get(void)
	{
		return (file);
	}

	int32_t create(std::wstring_view fileName);
	int32_t open(std::wstring_view fileName);
	int32_t close(void);
	bool eof(void);
	int32_t read(uint8_t* buffer, int32_t length);
	int32_t readLong(void);
	int32_t readString(uint8_t* buffer);
	int32_t readLineEx(uint8_t* buffer, int32_t maxLength);
	int32_t write(uint8_t* buffer, int32_t length);
	int32_t writeByte(uint8_t val);
	int32_t writeLong(int32_t val);
	int32_t writeString(std::wstring_view buffer);
};

//***************************************************************************

//----------
// FUNCTIONS

inline CharCodeType __stdcall calcCharCode(int32_t ch);
int32_t __stdcall isReservedWord(void);
void __stdcall initScanner(std::wstring_view fileName);
void __stdcall quitScanner(void);
void __stdcall skipComment(void);
void __stdcall skipBlanks(void);
void __stdcall getChar(void);
void __stdcall crunchToken(void);
void __stdcall downShiftWord(void);
void __stdcall getToken(void);
void __stdcall getWord(void);
void __stdcall accumulateValue(float* valuePtr, SyntaxErrorType errCode);
void __stdcall getNumber(void);
void __stdcall getString(void);
void __stdcall getSpecial(void);
bool __stdcall tokenIn(TokenCodeType* tokenList);
void __stdcall synchronize(
	TokenCodeType* tokenList1, TokenCodeType* tokenList2, TokenCodeType* tokenList3);
bool __stdcall getSourceLine(void);
void __stdcall printLine(std::wstring_view line);
void __stdcall initPageHeader(std::wstring_view fileName);
void __stdcall printPageHeader(void);

//----------
// VARIABLES

extern wchar_t wordString[MAXLEN_TOKENSTRING];
extern PVOID(__stdcall* ABLSystemMallocCallback)(size_t memSize);
extern PVOID(__stdcall* ABLStackMallocCallback)(size_t memSize);
extern PVOID(__stdcall* ABLCodeMallocCallback)(size_t memSize);
extern PVOID(__stdcall* ABLSymbolMallocCallback)(size_t memSize);
extern void(__stdcall* ABLSystemFreeCallback)(PVOID memBlock);
extern void(__stdcall* ABLStackFreeCallback)(PVOID memBlock);
extern void(__stdcall* ABLCodeFreeCallback)(PVOID memBlock);
extern void(__stdcall* ABLSymbolFreeCallback)(PVOID memBlock);
extern void(__stdcall* ABLDebugPrintCallback)(std::wstring_view s);
extern int32_t(__stdcall* ABLRandomCallback)(int32_t range);
extern void(__stdcall* ABLSeedRandomCallback)(size_t range);
extern size_t(__stdcall* ABLGetTimeCallback)(void);
extern void(__stdcall* ABLFatalCallback)(int32_t code, std::wstring_view s);

//***************************************************************************

} // namespace mclib::abl

#endif
