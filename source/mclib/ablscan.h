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

#define	TAB_SIZE				4

//#define	MIN_INTEGER				-2147483648
#define	MAX_INTEGER				2147483647
#define	MAX_DIGIT_COUNT			20
#define	MAX_EXPONENT			20

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
	char*			string;
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
		long		integer;
		float		real;
		char		string[MAXLEN_TOKENSTRING];
	} value;
} Literal;

//---------------------------------------------------------------------------

typedef struct CaseItem {
	int					labelValue;
	char*				branchLocation;
	struct CaseItem*	next;
} CaseItem;
typedef CaseItem* CaseItemPtr;

//***************************************************************************

class ABLFile {

    public:

		char*		fileName;
		void*		file;

		static long (*createCB) (void** file, char* fName);
		static long (*openCB) (void** file, char* fName);
		static long (*closeCB) (void** file);
		static bool (*eofCB) (void* file);
		static long (*readCB) (void* file, unsigned char* buffer, long length);
		static long (*readLongCB) (void* file);
		static long (*readStringCB) (void* file, unsigned char* buffer);
		static long (*readLineExCB) (void* file, unsigned char* buffer, long maxLength);
		static long (*writeCB) (void* file, unsigned char* buffer, long length);
		static long (*writeByteCB) (void* file, unsigned char byte);
		static long (*writeLongCB) (void* file, long value);
		static long (*writeStringCB) (void* file, char* buffer);

	public:

		void* operator new (size_t ourSize);

		void operator delete (void *us);

		void init (void);
	
		ABLFile (void) {
			init();
		}

		void destroy (void);

		~ABLFile (void) {
			destroy();
		}
		
		long set (void* fPtr) {
			file = fPtr;
		}

		void* get (void) {
			return(file);
		}

		long create (char* fileName);

		long open (char* fileName);

		long close (void);

		bool eof (void);

		long read (unsigned char* buffer, long length);

		long readLong (void);

		long readString (unsigned char* buffer);

		long readLineEx (unsigned char* buffer, long maxLength);

		long write (unsigned char* buffer, long length);

		long writeByte (unsigned char val);

		long writeLong (long val);

		long writeString (char* buffer);
};

//***************************************************************************

//----------
// FUNCTIONS

inline CharCodeType __stdcall calcCharCode(long ch);
long __stdcall isReservedWord(void);
void __stdcall initScanner(char* fileName);
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
void __stdcall printLine (char* line);
void __stdcall initPageHeader(char* fileName);
void __stdcall printPageHeader(void);

//----------
// VARIABLES

extern char		wordString[MAXLEN_TOKENSTRING];
extern void*	(__stdcall *ABLSystemMallocCallback) (size_t memSize);
extern void*	(__stdcall *ABLStackMallocCallback) (size_t memSize);
extern void*	(__stdcall *ABLCodeMallocCallback) (size_t memSize);
extern void*	(__stdcall *ABLSymbolMallocCallback) (size_t memSize);
extern void		(__stdcall *ABLSystemFreeCallback) (void* memBlock);
extern void		(__stdcall *ABLStackFreeCallback) (void* memBlock);
extern void		(__stdcall *ABLCodeFreeCallback) (void* memBlock);
extern void		(__stdcall *ABLSymbolFreeCallback) (void* memBlock);
extern void		(__stdcall *ABLDebugPrintCallback) (char* s);
extern long		(__stdcall *ABLRandomCallback) (long range);
extern void		(__stdcall *ABLSeedRandomCallback) (size_t range);
extern size_t	(__stdcall *ABLGetTimeCallback) (void);
extern void		(__stdcall *ABLFatalCallback) (long code, char* s);

//***************************************************************************
	
#endif


