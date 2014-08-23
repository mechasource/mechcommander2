//===========================================================================//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//
//***************************************************************************
//
//								SCANNER.H
//
//***************************************************************************

#ifndef ABLSCAN_H
#define	ABLSCAN_H

#ifndef ABLGEN_H
#include "ablgen.h"
#endif

#ifndef ABLERR_H
#include "ablerr.h"
#endif

//***************************************************************************

//#define	MIN_INTEGER				-2147483648
//#define	MAX_INTEGER				2147483647

typedef enum __ablscan_const {
	TAB_SIZE					= 4,
	MAX_DIGIT_COUNT				= 20,
	MAX_EXPONENT				= 20,
};

//***************************************************************************

//------------
// TOKEN CODES

typedef enum TokenCodeType {
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
} TokenCodeType;

typedef enum CharCodeType {
	CHR_LETTER,
	CHR_DIGIT,
	CHR_DQUOTE,
	CHR_SPECIAL,
	CHR_EOF
} CharCodeType;

typedef struct ReservedWord {
	PSTR			string;
	TokenCodeType	tokenCode;
} ReservedWord;

//***************************************************************************

//------------------
// LITERAL structure

typedef enum LiteralType {
	LIT_INTEGER,
	LIT_REAL,
	LIT_STRING
} LiteralType;

typedef struct Literal {
	LiteralType		type;
	struct {
		int32_t		integer;
		float		real;
		char		string[MAXLEN_TOKENSTRING];
	} value;
} Literal;

//---------------------------------------------------------------------------

typedef struct CaseItem {
	int32_t					labelValue;
	PSTR				branchLocation;
	struct CaseItem*	next;
} CaseItem;
typedef CaseItem* CaseItemPtr;

//***************************************************************************

class ABLFile {

    public:

		PSTR		fileName;
		PVOID		file;

		static int32_t (__stdcall *createCB)(PVOID* file, PSTR fName);
		static int32_t (__stdcall *openCB)(PVOID* file, PSTR fName);
		static int32_t (__stdcall *closeCB)(PVOID* file);
		static bool (__stdcall *eofCB)(PVOID file);
		static int32_t (__stdcall *readCB)(PVOID file, puint8_t buffer, int32_t length);
		static int32_t (__stdcall *readLongCB)(PVOID file);
		static int32_t (__stdcall *readStringCB)(PVOID file, puint8_t buffer);
		static int32_t (__stdcall *readLineExCB)(PVOID file, puint8_t buffer, int32_t maxLength);
		static int32_t (__stdcall *writeCB)(PVOID file, puint8_t buffer, int32_t length);
		static int32_t (__stdcall *writeByteCB)(PVOID file, uint8_t byte);
		static int32_t (__stdcall *writeLongCB)(PVOID file, int32_t value);
		static int32_t (__stdcall *writeStringCB)(PVOID file, PSTR buffer);

	public:

		PVOID operator new (size_t ourSize);
		void operator delete (PVOID us);

		void init(void);
	
		ABLFile(void)
		{
			init();
		}

		void destroy (void);

		~ABLFile (void)
		{
			destroy();
		}
		
		int32_t set (PVOID fPtr)
		{
			file = fPtr;
		}

		PVOID get (void)
		{
			return(file);
		}

		int32_t create (PSTR fileName);
		int32_t open (PSTR fileName);
		int32_t close (void);
		bool eof (void);
		int32_t read (puint8_t buffer, int32_t length);
		int32_t readLong (void);
		int32_t readString (puint8_t buffer);
		int32_t readLineEx (puint8_t buffer, int32_t maxLength);
		int32_t write (puint8_t buffer, int32_t length);
		int32_t writeByte (uint8_t val);
		int32_t writeLong (int32_t val);
		int32_t writeString (PSTR buffer);
};

//***************************************************************************

//----------
// FUNCTIONS

inline CharCodeType __stdcall calcCharCode(int32_t ch);
int32_t __stdcall isReservedWord(void);
void __stdcall initScanner(PSTR fileName);
void __stdcall quitScanner(void);
void __stdcall skipComment(void);
void __stdcall skipBlanks(void);
void __stdcall getChar(void);
void __stdcall crunchToken(void);
void __stdcall downShiftWord(void);
void __stdcall getToken(void);
void __stdcall getWord (void);
void __stdcall accumulateValue(float* valuePtr, SyntaxErrorType errCode);
void __stdcall getNumber(void);
void __stdcall getString(void);
void __stdcall getSpecial(void);
bool __stdcall tokenIn (TokenCodeType* tokenList);
void __stdcall synchronize(TokenCodeType* tokenList1, TokenCodeType* tokenList2, TokenCodeType* tokenList3);
bool __stdcall getSourceLine(void);
void __stdcall printLine (PSTR line);
void __stdcall initPageHeader(PSTR fileName);
void __stdcall printPageHeader(void);

//----------
// VARIABLES

extern char		wordString[MAXLEN_TOKENSTRING];
extern PVOID	(__stdcall *ABLSystemMallocCallback) (size_t memSize);
extern PVOID	(__stdcall *ABLStackMallocCallback) (size_t memSize);
extern PVOID	(__stdcall *ABLCodeMallocCallback) (size_t memSize);
extern PVOID	(__stdcall *ABLSymbolMallocCallback) (size_t memSize);
extern void		(__stdcall *ABLSystemFreeCallback) (PVOID memBlock);
extern void		(__stdcall *ABLStackFreeCallback) (PVOID memBlock);
extern void		(__stdcall *ABLCodeFreeCallback) (PVOID memBlock);
extern void		(__stdcall *ABLSymbolFreeCallback) (PVOID memBlock);
extern void		(__stdcall *ABLDebugPrintCallback) (PSTR s);
extern int32_t		(__stdcall *ABLRandomCallback) (int32_t range);
extern void		(__stdcall *ABLSeedRandomCallback) (size_t range);
extern size_t	(__stdcall *ABLGetTimeCallback) (void);
extern void		(__stdcall *ABLFatalCallback) (int32_t code, PSTR s);

//***************************************************************************
	
#endif


