//===========================================================================//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//
//***************************************************************************
//
//								ABLSCAN.CPP
//
//***************************************************************************
#include "stdafx.h"

//#include <stdio.h>
//#include <stdlib.h>
//#include <math.h>
//#include <string.h>
//#include <time.h>

#ifndef ABLGEN_H
#include "ablgen.h"
#endif

#ifndef ABLERR_H
#include "ablerr.h"
#endif

#ifndef ABLSCAN_H
#include "ablscan.h"
#endif

#ifndef ABLPARSE_H
#include "ablparse.h"
#endif

#ifndef ABLENV_H
#include "ablenv.h"
#endif

//***************************************************************************
//#pragma warning(disable:4244)

//---------------------
// RESERVED uint16_t tables

#define	MINLEN_RESERVED_WORD	2
#define	MAXLEN_RESERVED_WORD	11

ReservedWord reservedWord2[] = {
	{"if", TKN_IF},
	{"or", TKN_OR},
	{"do", TKN_DO},
	{"to", TKN_TO},
	{NULL, TKN_NONE}
};

ReservedWord reservedWord3[] = {
	{"and", TKN_AND},
	{"for", TKN_FOR},
	{"mod", TKN_MOD},
	{"not", TKN_NOT},
	{"var", TKN_VAR},
	{"fsm", TKN_FSM},
	{NULL, TKN_NONE}
};

ReservedWord reservedWord4[] = {
	{"else", TKN_ELSE},
	{"then", TKN_THEN},
	{"case", TKN_CASE},
	{"code", TKN_CODE},
	{"type", TKN_TYPE},
	{NULL, TKN_NONE}
};

ReservedWord reservedWord5[] = {
	{"const", TKN_CONST},
	{"until", TKN_UNTIL},
	{"while", TKN_WHILE},
	{"endif", TKN_END_IF},
	{"order", TKN_ORDER},
	{"state", TKN_STATE},
	{"trans", TKN_TRANS},
	{NULL, TKN_NONE}
};

ReservedWord reservedWord6[] = {
	{"module", TKN_MODULE},
	{"repeat", TKN_REPEAT},
	{"endfor", TKN_END_FOR},
	{"switch", TKN_SWITCH},
	{"static", TKN_STATIC},
	{"endfsm", TKN_END_FSM},
	{NULL, TKN_NONE}
};

ReservedWord reservedWord7[] = {
	{"endcase", TKN_END_CASE},
	{"eternal", TKN_ETERNAL},
	{"library", TKN_LIBRARY},
	{NULL, TKN_NONE}
};

ReservedWord reservedWord8[] = {
	{"function", TKN_FUNCTION},
	{"endwhile", TKN_END_WHILE},
	{"endorder", TKN_END_ORDER},
	{"endstate", TKN_END_STATE},
	{NULL, TKN_NONE}
};

ReservedWord reservedWord9[] = {
	{"endswitch", TKN_END_SWITCH},
	{"endmodule", TKN_END_MODULE},
	{"transback", TKN_TRANS_BACK},
	{NULL, TKN_NONE}					//This was missing.  Thank You Symantec!!!!		1/24/97 -fs
};

ReservedWord reservedWord10[] = {
	{"endlibrary", TKN_END_LIBRARY},
	{NULL, TKN_NONE}
};

ReservedWord reservedWord11[] = {
	{"endfunction", TKN_END_FUNCTION},
	{NULL, TKN_NONE}
};

ReservedWord* reservedWordTable[] = {
	NULL,
	NULL,
	reservedWord2,
	reservedWord3,
	reservedWord4,
	reservedWord5,
	reservedWord6,
	reservedWord7,
	reservedWord8,
	reservedWord9,
	reservedWord10,
	reservedWord11
};

//------------
// TOKEN lists

TokenCodeType statementStartList[] = {
	TKN_FOR,
	TKN_IF,
	TKN_REPEAT,
	TKN_WHILE,
	TKN_SWITCH,
	TKN_TRANS,
	TKN_TRANS_BACK,
	TKN_IDENTIFIER,
	TKN_NONE
};

TokenCodeType statementEndList[] = {
	TKN_SEMICOLON,
	TKN_END_IF,
	TKN_END_WHILE,
	TKN_END_FOR,
	TKN_END_SWITCH,
	TKN_END_FUNCTION,
	TKN_END_ORDER,
	TKN_END_STATE,
	TKN_ELSE,
	TKN_ELSIF,
	TKN_UNTIL,
	TKN_EOF,
	TKN_NONE
};

TokenCodeType declarationStartList[] = {
	TKN_CONST,
	TKN_VAR,
	TKN_FUNCTION,
	TKN_ORDER,
	TKN_STATE,
	TKN_NONE
};

PSTR TokenStrings[NUM_TOKENS] = {
	"{BAD TOKEN}",
	"{IDENTIFIER}",
	"{NUMBER}",
	"{TYPE}",
	"{STRING}",
	"*",
	"(",
	")",
	"-",
	"+",
	"=",
	"[",
	"]",
	":",
	";",
	"<",
	">",
	",",
	".",
	"/",
	"==",
	"<=",
	">=",
	"<>",
	"{EOF}",
	"{ERROR}",
	"code",
	"order",
	"state",
	"and",
	"switch",
	"case",
	"const",
	"div",
	"do",
	"of",
	"else",
	"endif",
	"endwhile",
	"endfor",
	"endfunction",
	"endorder",
	"endstate",
	"endmodule",
	"endfsm",
	"endlibrary",
	"endvar",
	"endcode",
	"endcase",
	"endswitch",
	"for",
	"function",
	"if",
	"mod",
	"not",
	"or",
	"repeat",
	"then",
	"to",
	"until",
	"var",
	"@",
	"while",
	"elsif",
	"return",
	"module",
	"fsm",
	"trans",
	"transback",
	"library",
	"eternal",
	"static",
	"#",
	"{UNEXPECTED TOKEN}",
	"{STATEMENT MARKER}",
	"{ADDRESS MARKER}"
};

//--------
// GLOBALS

char			curChar;
TokenCodeType	curToken;
Literal			curLiteral;
int32_t			level = 0;
int32_t			lineNumber = 0;
int32_t			FileNumber = 0;
ABLFile*		sourceFile = NULL;
bool			printFlag = true;
bool			blockFlag = false;
BlockType		blockType = BLOCK_MODULE;
SymTableNodePtr	CurModuleIdPtr = NULL;
SymTableNodePtr	CurRoutineIdPtr = NULL;
bool			DumbGetCharOn = false;

int32_t			NumOpenFiles = 0;
int32_t			NumSourceFiles = 0;
char			SourceFiles[MAX_SOURCE_FILES][MAXLEN_FILENAME];
SourceFile		openFiles[MAX_INCLUDE_DEPTH];
char			sourceBuffer[MAXLEN_SOURCELINE];
char			tokenString[MAXLEN_TOKENSTRING];
char			wordString[MAXLEN_TOKENSTRING];
int32_t			bufferOffset = 0;
PSTR			bufferp = sourceBuffer;
PSTR			tokenp = tokenString;

int32_t			digitCount = 0;
bool			countError = false;

int32_t			pageNumber = 0;
int32_t			lineCount = MAX_LINES_PER_PAGE;

char			sourceName[MAXLEN_FILENAME];
char			date[LEN_DATESTRING];

CharCodeType	charTable[256];

extern bool		AssertEnabled;
extern bool		PrintEnabled;
extern bool		StringFunctionsEnabled;
extern bool		DebugCodeEnabled;

extern ABLModulePtr	CurLibrary;

int32_t (*ABLFile::createCB) (PVOID* file, PSTR fName) = NULL;
int32_t (*ABLFile::openCB) (PVOID* file, PSTR fName) = NULL;
int32_t (*ABLFile::closeCB) (PVOID* file) = NULL;
bool (*ABLFile::eofCB) (PVOID file) = NULL;
int32_t (*ABLFile::readCB) (PVOID file, puint8_t buffer, int32_t length) = NULL;
int32_t (*ABLFile::readLongCB) (PVOID file) = NULL;
int32_t (*ABLFile::readStringCB) (PVOID file, puint8_t buffer) = NULL;
int32_t (*ABLFile::readLineExCB) (PVOID file, puint8_t buffer, int32_t maxLength) = NULL;
int32_t (*ABLFile::writeCB) (PVOID file, puint8_t buffer, int32_t length) = NULL;
int32_t (*ABLFile::writeByteCB) (PVOID file, uint8_t byte) = NULL;
int32_t (*ABLFile::writeLongCB) (PVOID file, int32_t value) = NULL;
int32_t (*ABLFile::writeStringCB) (PVOID file, PSTR buffer) = NULL;

//***************************************************************************
// ABL FILE routines
//***************************************************************************

PVOID ABLFile::operator new (size_t ourSize) {

	PVOID result = ABLSystemMallocCallback(ourSize);
	return(result);
}

//---------------------------------------------------------------------------

void ABLFile::operator delete (PVOID us) {

	ABLSystemFreeCallback(us);
}	

//---------------------------------------------------------------------------

void ABLFile::init (void) {

	fileName = 0;
	file = NULL;
}

//-----------------------------------------------------------------------------
	
void ABLFile::destroy (void) {

	if (fileName) {
		ABLSystemFreeCallback(fileName);
		fileName = NULL;
	}
	file = NULL;
}

//---------------------------------------------------------------------------

int32_t ABLFile::create (PSTR fName) {

	if (fName) {
		fileName = (PSTR)ABLSystemMallocCallback(strlen(fName)+1);
		strcpy(fileName,fName);
		return(createCB(&file, fileName));
	}
	return(-1);
}

//---------------------------------------------------------------------------

int32_t ABLFile::open (PSTR fName) {

	if (fName) {
		fileName = (PSTR)ABLSystemMallocCallback(strlen(fName)+1);
		strcpy(fileName,fName);
		return(openCB(&file, fileName));
	}
	return(-1);
}

//---------------------------------------------------------------------------

int32_t ABLFile::close (void) {

	if (file)
		return(closeCB(&file));
	return(-1);
}

//-----------------------------------------------------------------------------

bool ABLFile::eof (void) {

	if (file)
		return(eofCB(file));
	return(true);
}

//-----------------------------------------------------------------------------

int32_t ABLFile::read (puint8_t buffer, int32_t length) {

	if (file)
		return(readCB(file, buffer, length));
	return(0);
}

//-----------------------------------------------------------------------------

int32_t ABLFile::readLong (void) {

	if (file)
		return(readLongCB(file));
	return(-1);
}

//-----------------------------------------------------------------------------

int32_t ABLFile::readString (puint8_t buffer) {

	if (file)
		return(readStringCB(file, buffer));
	return(0);
}

//-----------------------------------------------------------------------------

int32_t ABLFile::readLineEx (puint8_t buffer, int32_t maxLength) {

	if (file)
		return(readLineExCB(file, buffer, maxLength));
	return(0);
}

//-----------------------------------------------------------------------------

int32_t ABLFile::write (puint8_t buffer, int32_t length) {

	if (file)
		return(writeCB(file, buffer, length));
	return(0);
}

//-----------------------------------------------------------------------------

int32_t ABLFile::writeByte (uint8_t val) {

	if (file)
		return(writeByteCB(file, val));
	return(0);
}

//-----------------------------------------------------------------------------

int32_t ABLFile::writeLong (int32_t val) {

	if (file)
		return(writeLongCB(file, val));
	return(0);
}

//-----------------------------------------------------------------------------

int32_t ABLFile::writeString (PSTR buffer) {

	if (file)
		return(writeStringCB(file, buffer));
	return(0);
}

//***************************************************************************
// MISC routines
//***************************************************************************

inline CharCodeType calcCharCode (int32_t ch) {

	return(charTable[ch]);
}

//***************************************************************************

int32_t isReservedWord (void) 
{
	int32_t wordLength = strlen(wordString);
	if ((wordLength >= MINLEN_RESERVED_WORD) && (wordLength <= MAXLEN_RESERVED_WORD) && reservedWordTable[wordLength])
	{
		for (ReservedWord* rwPtr = reservedWordTable[wordLength]; rwPtr->string != NULL; rwPtr++)
		{
			if (strcmp(wordString, rwPtr->string) == 0) 
			{
				curToken = rwPtr->tokenCode;
				return(1);
			}
		}
	}
	return(0);
}

//***************************************************************************
// INITIALIZATION routines
//***************************************************************************

void getChar();

void initScanner (PSTR fileName) {

	int32_t curCh;
	//----------------------------------
	// Initialize the character table...
	for (curCh = 0; curCh < 256; curCh++)
		charTable[curCh] = CHR_SPECIAL;
	for (curCh = '0'; curCh <= '9'; curCh++)
		charTable[curCh] = CHR_DIGIT;
	for (curCh = 'A'; curCh <= 'Z'; curCh++)
		charTable[curCh] = CHR_LETTER;
	for (curCh = 'a'; curCh <= 'z'; curCh++)
		charTable[curCh] = CHR_LETTER;
	charTable['\"'] = CHR_DQUOTE;
	charTable[CHAR_EOF] = CHR_EOF;

	//---------------------------------------
	// Now, let's open the ABL source file...
	if (fileName) 
	{
		sourceFile = new ABLFile;
		if (sourceFile->open(fileName) != ABL_NO_ERR) 
		{
			syntaxError(ABL_ERR_SYNTAX_SOURCE_FILE_OPEN);
			curToken = TKN_ERROR;
		}

		sourceBuffer[0] = NULL;
		bufferp = sourceBuffer;
		getChar();
	}
}

//***************************************************************************

void quitScanner (void) {

	sourceFile->close();

	delete sourceFile;
	sourceFile = NULL;
}

//***************************************************************************
// CHARACTER routines
//***************************************************************************

void skipLineComment (void) {

	//-------------------------------------------------------------------
	// Just blow off the rest of the current source line we've read in...
	*bufferp = NULL;
	curChar = ' ';
}

//***************************************************************************

void skipBlockComment (void) {

	//----------------------------------------------------
	// Snag the asterisk, then continue until we encounter
	// the closing "*/"...
	DumbGetCharOn = true;
	getChar();
	getChar();
	do {
		while ((curChar != '*') && (curChar != CHR_EOF))
			getChar();
		if (curChar == CHR_EOF) {
			syntaxError(ABL_ERR_SYNTAX_UNEXPECTED_EOF);
			curToken = TKN_ERROR;
		}
		getChar();
	} while (curChar != '/');
	curChar = ' ';
	DumbGetCharOn = false;
}

//***************************************************************************

void skipBlanks (void) {

	while (curChar == ' ')
		getChar();
}

//***************************************************************************

void languageDirective (void) {

	DumbGetCharOn = true;
	getChar();

	char directive[32];
	int32_t directiveLength = 0;
	while ((curChar != ' ') && (curChar != '\n')  && (curChar != '\r') && (directiveLength < 31)) {
		directive[directiveLength++] = curChar;
		getChar();
	}
	directive[directiveLength] = NULL;
	_strlwr(directive);

	if (strcmp(directive, "include") == 0) {
		//---------------------
		// INCLUDE directive...
		getChar();
		if (curChar == '"') {
			getChar();
			char fileName[128];
			int32_t fileNameLength = 0;
			while ((curChar != '"') && (fileNameLength < 127)) {
				fileName[fileNameLength++] = curChar;
				getChar();
			}
			fileName[fileNameLength] = NULL;
			//-----------------------------------------------------------------------
			// The language directive should be on a line by itself, since everything
			// after the directive on this line is ignored...
			DumbGetCharOn = false;
			//----------------------------------------------------
			// Now that we have the file name, let's include it...
			int32_t openErr = ABL_NO_ERR;
			if ((openErr = openSourceFile(fileName)) != ABL_NO_ERR) {
				syntaxError(ABL_ERR_SYNTAX_SOURCE_FILE_OPEN);
				curToken = TKN_ERROR;
			}
			}
		else {
			syntaxError(ABL_ERR_SYNTAX_BAD_LANGUAGE_DIRECTIVE_USAGE);
			curToken = TKN_ERROR;
		}
		}
	else if (strcmp(directive, "include_") == 0) {
		//---------------------
		// INCLUDE directive...
		getChar();
		if (curChar == '"') {
			getChar();
			char fileName[128];
			int32_t fileNameLength = 0;
			while ((curChar != '"') && (fileNameLength < 127)) {
				fileName[fileNameLength++] = curChar;
				getChar();
			}
			fileName[fileNameLength] = NULL;
			//-----------------------------------------------------------------------
			// The language directive should be on a line by itself, since everything
			// after the directive on this line is ignored...
			DumbGetCharOn = false;
			//----------------------------------------------------
			// Now that we have the file name, let's include it...
			int32_t openErr = ABL_NO_ERR;

			//---------------------------------------
			// What's the current module's directory?
			char fullPath[255];
			int32_t curChar = strlen(SourceFiles[0]);
			while ((curChar > -1) && (SourceFiles[0][curChar] != '\\'))
				curChar--;
			if (curChar == -1)
				strcpy(fullPath, fileName);
			else {
				strcpy(fullPath, SourceFiles[0]);
				fullPath[curChar + 1] = NULL;
				strcat(fullPath, fileName);
			}
			if ((openErr = openSourceFile(fullPath)) != ABL_NO_ERR) {
				syntaxError(ABL_ERR_SYNTAX_SOURCE_FILE_OPEN);
				curToken = TKN_ERROR;
			}
			}
		else {
			syntaxError(ABL_ERR_SYNTAX_BAD_LANGUAGE_DIRECTIVE_USAGE);
			curToken = TKN_ERROR;
		}
		}
	else if (strcmp(directive, "assert_on") == 0) {
		DumbGetCharOn = false;
		curChar = ' ';
		AssertEnabled = true;
		}
	else if (strcmp(directive, "assert_off") == 0) {
		DumbGetCharOn = false;
		curChar = ' ';
		AssertEnabled = false;
		}
	else if (strcmp(directive, "print_on") == 0) {
		DumbGetCharOn = false;
		curChar = ' ';
		PrintEnabled = true;
		}
	else if (strcmp(directive, "print_off") == 0) {
		DumbGetCharOn = false;
		curChar = ' ';
		PrintEnabled = false;
		}
	else if (strcmp(directive, "stringfuncs_on") == 0) {
		DumbGetCharOn = false;
		curChar = ' ';
		StringFunctionsEnabled = true;
		}
	else if (strcmp(directive, "stringfuncs_off") == 0) {
		DumbGetCharOn = false;
		curChar = ' ';
		StringFunctionsEnabled = false;
		}
	else if (strcmp(directive, "debug_on") == 0) {
		DumbGetCharOn = false;
		curChar = ' ';
		DebugCodeEnabled = true;
		}
	else if (strcmp(directive, "debug_off") == 0) {
		DumbGetCharOn = false;
		curChar = ' ';
		DebugCodeEnabled = false;
		}
	else if (strcmp(directive, "debug_start") == 0) {
		bool stillCutting = true;
		if (!DebugCodeEnabled) {
			int32_t nestedLevel = 1;
			getChar();
//			getChar();
			do {
				while ((curChar != '#') && (curChar != CHR_EOF))
					getChar();
				if (curChar == '#') {
					char directive2[32];
					int32_t directiveLength = 0;
					while ((curChar != ' ') && (curChar != '\n')  && (curChar != '\r') && (directiveLength < 31)) {
						directive2[directiveLength++] = curChar;
						getChar();
					}
					directive2[directiveLength] = NULL;
					_strlwr(directive2);
					if (strcmp(directive2, "#debug_start") == 0)
						nestedLevel++;
					else if (strcmp(directive2, "#debug_end") == 0) {
						nestedLevel--;
						if (nestedLevel == 0)
							stillCutting = false;
					}
					}
				else if (curChar == CHR_EOF) {
					syntaxError(ABL_ERR_SYNTAX_UNEXPECTED_EOF);
					curToken = TKN_ERROR;
					}
				else
					getChar();
			} while (stillCutting);
		}
		curChar = ' ';
		DumbGetCharOn = false;
		}
	else if (strcmp(directive, "debug_end") == 0) {
		curChar = ' ';
		DumbGetCharOn = false;
		}
	else {
		syntaxError(ABL_ERR_SYNTAX_UNKNOWN_LANGUAGE_DIRECTIVE);
		curToken = TKN_ERROR;
	}
}

//***************************************************************************

void getChar (void) {

	if (*bufferp == NULL) {
		if (!getSourceLine()) {
			if (NumOpenFiles > 1) {
				closeSourceFile();
				return;
				}
			else {
				curChar = CHAR_EOF;
				return;
			}
		}
		bufferp = sourceBuffer;
		bufferOffset = 0;
	}

	curChar = *bufferp;
	bufferp++;

	if (DumbGetCharOn) {
		bufferOffset++;
		return;
	}

	//-------------------
	// Special Characters
	switch (curChar) {
		case '\t':
			// TAB
			bufferOffset += TAB_SIZE - bufferOffset % TAB_SIZE;
			curChar = ' ';
			break;
		case '\n':
		case '\r':
			// NEWLINE
			bufferOffset++;
			curChar = ' ';
			break;
		case '/':
			if (*bufferp == '/')
				skipLineComment();
			else if (*bufferp == '*')
				skipBlockComment();
			else
				bufferOffset++;
			break;
		case '#':
			languageDirective();
			break;
		default:
			bufferOffset++;
	}
}

//***************************************************************************
// TOKEN routines
//***************************************************************************

void downShiftWord (void) {

	int32_t offset = 'a' - 'A';
	PSTR wp = wordString;
	PSTR tp = tokenString;

	int32_t checkLengthWord = strlen(wordString);
	int32_t checkLengthToken = strlen(tokenString);

	if ((checkLengthWord >= MAXLEN_TOKENSTRING) || (checkLengthToken >= MAXLEN_TOKENSTRING))
		ABL_Fatal(-1," Boy did Glenn screw the pooch here!! ");

	do {
		*wp = ((*tp >= 'A') && (*tp <= 'Z')) ? (*tp + (char)offset) : *tp;
		wp++;
		tp++;
	} while (*tp != NULL);
	*wp = NULL;
}

//***************************************************************************

void getToken(void) {

	skipBlanks();
	tokenp = tokenString;

	switch (calcCharCode(curChar)) {
		case CHR_LETTER:
			getWord();
			break;
		case CHR_DIGIT:
			getNumber();
			break;
		case CHR_DQUOTE:
			getString();
			break;
		case CHR_EOF:
			curToken = TKN_EOF;
			break;
		default:
			getSpecial();
			break;
	}

	if (blockFlag)
		crunchToken();
}

//***************************************************************************

void getWord (void) {

	while ((calcCharCode(curChar) == CHR_LETTER) || (calcCharCode(curChar) == CHR_DIGIT) || (curChar == '_')) {
		*tokenp = curChar;
		tokenp++;
		getChar();
	}

	*tokenp = NULL;
	downShiftWord();

	if (curChar == '.') {
		TokenCodeType endToken = TKN_END_MODULE;
		if (CurLibrary)
			endToken = TKN_END_LIBRARY;
		else if (CurModuleIdPtr->defn.info.routine.flags & ROUTINE_FLAG_FSM)
			endToken = TKN_END_FSM;
		//-------------------------------------------------------------------
		// If we have an "endmodule" string, stop. Otherwise, keep reading...
		if (strcmp(wordString, TokenStrings[endToken])) {
			*tokenp = curChar;
			tokenp++;
			getChar();
			while ((calcCharCode(curChar) == CHR_LETTER) || (calcCharCode(curChar) == CHR_DIGIT) || (curChar == '_')) {
				*tokenp = curChar;
				tokenp++;
				getChar();
			}
			*tokenp = NULL;
			downShiftWord();
		}
	}


	if (!isReservedWord())
		curToken = TKN_IDENTIFIER;
}

//***************************************************************************

void accumulateValue (float* valuePtr, SyntaxErrorType errCode) {

	float value = *valuePtr;

	//--------------------------------------------
	// Make sure the first character is a digit...
	if (calcCharCode(curChar) != CHR_DIGIT) {
		syntaxError(errCode);
		curToken = TKN_ERROR;
		return;
	}

	do {
		*tokenp = curChar;
		tokenp++;
		if (++digitCount <= MAX_DIGIT_COUNT)
			value = 10 * value + (curChar - '0');
		else
			countError = true;

		getChar();
	} while (calcCharCode(curChar) == CHR_DIGIT);

	*valuePtr = value;
}

//***************************************************************************

void getNumber (void) {

	int32_t wholeCount = 0;
	int32_t decimalOffset = 0;
	char exponentSign = '+';
	int32_t exponent = 0;
	float numberValue = (float)0.0;
	float exponentValue = (float)0.0;
	digitCount = 0;
	countError = false;
	curToken = TKN_NONE;

	//------------------------------------
	// Assume it's an integer, at first...
	curLiteral.type = LIT_INTEGER;

	accumulateValue(&numberValue, ABL_ERR_SYNTAX_INVALID_NUMBER);
	if (curToken == TKN_ERROR)
		return;
	wholeCount = digitCount;

	if (curChar == '.') {
		getChar();
		curLiteral.type = LIT_REAL;
		*tokenp = '.';
		tokenp++;

		accumulateValue(&numberValue, ABL_ERR_SYNTAX_INVALID_FRACTION);
		if (curToken == TKN_ERROR)
			return;
		decimalOffset = wholeCount - digitCount;
	}

	if ((curChar == 'E') || (curChar == 'e')) {
		curLiteral.type = LIT_REAL;
		*tokenp = curChar;
		tokenp++;

		//-------------------
		// Any exponent sign?
		if ((curChar == '+') || (curChar == '-')) {
			*tokenp = exponentSign = curChar;
			tokenp++;
			getChar();
		}

		accumulateValue(&exponentValue, ABL_ERR_SYNTAX_INVALID_EXPONENT);
		if (curToken == TKN_ERROR)
			return;
		if (exponentSign == '-')
			exponentValue = -exponentValue;
	}

	//-----------------
	// Too many digits?
	if (countError) {
		syntaxError(ABL_ERR_SYNTAX_TOO_MANY_DIGITS);
		curToken = TKN_ERROR;
		return;
	}

	//------------------------------------------------------------
	// Adjust number's value using decimal offset and the exponent
	exponent = exponentValue + decimalOffset;
	if ((exponent + wholeCount < -MAX_EXPONENT) || (exponent + wholeCount > MAX_EXPONENT)) {
		syntaxError(ABL_ERR_SYNTAX_REAL_OUT_OF_RANGE);
		curToken = TKN_ERROR;
		return;
	}

	if (exponent != 0)
		numberValue *= (float)pow(10.0, (double)exponent);

	//---------------------------
	// Set the literal's value...
	if (curLiteral.type == LIT_INTEGER) 
	{
		// This line can never be true, right?  
		// If I cast to int32_t it can't be outside that range!!
//		if (((int32_t)numberValue < -MAX_INTEGER) || ((int32_t)numberValue > MAX_INTEGER)) 
//		{
//			syntaxError(ABL_ERR_SYNTAX_INTEGER_OUT_OF_RANGE);
//			curToken = TKN_ERROR;
//			return;
//		}
		curLiteral.value.integer = (int32_t)numberValue;
		}
	else
		curLiteral.value.real = numberValue;

	*tokenp = NULL;
	curToken = TKN_NUMBER;
}

//***************************************************************************

//---------------------------------------------------
// NOTE: Currently, ABL does not have STRING types...

void getString (void) {

	PSTR stringPtr = curLiteral.value.string;
	*tokenp = '\"';
	getChar();

	while (curChar != CHAR_EOF) {
		if (curChar == '\"')
			break;
		*tokenp = curChar;
		tokenp++;
		*stringPtr = curChar;
		stringPtr++;
		getChar();
	}
	curChar = ' ';

	*tokenp = NULL;
	*stringPtr = NULL;
	curToken = TKN_STRING;
	curLiteral.type = LIT_STRING;
}

//***************************************************************************

void getSpecial (void) {

	*tokenp = curChar;
	tokenp++;

	switch (curChar) {
//		case '^':
//			curToken = TKN_UPARROW;
//			getChar();
//			break;
		case '*':
			curToken = TKN_STAR;
			getChar();
			break;
		case '(':
			curToken = TKN_LPAREN;
			getChar();
			break;
		case ')':
			curToken = TKN_RPAREN;
			getChar();
			break;
		case '-':
			curToken = TKN_MINUS;
			getChar();
			break;
		case '+':
			curToken = TKN_PLUS;
			getChar();
			break;
		case '[':
			curToken = TKN_LBRACKET;
			getChar();
			break;
		case ']':
			curToken = TKN_RBRACKET;
			getChar();
			break;
		case ';':
			curToken = TKN_SEMICOLON;
			getChar();
			break;
		case '@':
			curToken = TKN_REF;
			getChar();
			break;
		case '#':
			curToken = TKN_POUND;
			getChar();
			break;
// NOTE: For now, let's use the BEGIN and END keywords instead of braces.
// If popular demand is to get rid of the begin/end keywords and switch
// to braces, we can do so...
//		case '{':
//			curToken = TKN_BEGIN;
//			getChar();
//			break;
//		case '}':
//			curToken = TKN_END;
//			getChar();
//			break;
		case ',':
			curToken = TKN_COMMA;
			getChar();
			break;
		case '/':
			//-----------------------------------------------
			// In theory :), we shouldn't have to worry about
			// comments here, as getChar syphons them out...
			curToken = TKN_FSLASH;
			getChar();
			break;
		case '=':
			getChar();
			if (curChar == '=') {
				*tokenp = '=';
				tokenp++;
				curToken = TKN_EQUALEQUAL;
				getChar();
				}
			else
				curToken = TKN_EQUAL;
			break;
		case '<':
			getChar();
			if (curChar == '=') {
				*tokenp = '=';
				tokenp++;
				curToken = TKN_LE;
				getChar();
				}
			else if (curChar == '>') {
				*tokenp = '>';
				tokenp++;
				curToken = TKN_NE;
				getChar();
				}
			else
				curToken = TKN_LT;
			break;
		case '>':
			getChar();
			if (curChar == '=') {
				*tokenp = '=';
				tokenp++;
				curToken = TKN_GE;
				getChar();
				}
			else
				curToken = TKN_GT;
			break;
		case '.':
			curToken = TKN_PERIOD;
			getChar();
			break;
		case ':':
			curToken = TKN_COLON;
			getChar();
			break;
		default:
			curToken = TKN_ERROR;
			getChar();
			break;
	}

	*tokenp = NULL;
}

//***************************************************************************
// TOKEN TESTER routines
//***************************************************************************

bool tokenIn (TokenCodeType* tokenList) {

	if (!tokenList)
		return(false);

	for (TokenCodeType* tp = &tokenList[0]; *tp; tp++)
		if (curToken == *tp)
			return(true);
	return(false);
}

//***************************************************************************

void synchronize (TokenCodeType* tokenList1,
				  TokenCodeType* tokenList2,
				  TokenCodeType* tokenList3) {

	bool badLists = (!tokenIn(tokenList1) && !tokenIn(tokenList2) && !tokenIn(tokenList3));

	if (badLists) {
		syntaxError((curToken == TKN_EOF) ? ABL_ERR_SYNTAX_UNEXPECTED_EOF : ABL_ERR_SYNTAX_UNEXPECTED_TOKEN);

		//----------------------------------------------
		// Now, we need to re-sync by skipping tokens...
		while (!tokenIn(tokenList1) &&
			   !tokenIn(tokenList2) &&
			   !tokenIn(tokenList3) &&
			   (curToken != TKN_EOF))
			getToken();
	}
}

//***************************************************************************
// SOURCE FILE routines
//***************************************************************************

bool getSourceLine (void) {

	if (!sourceFile->eof())
	{
		int32_t numChars = sourceFile->readLineEx((puint8_t)sourceBuffer, MAXLEN_SOURCELINE);
		if (numChars == 0)
			return(false);
		lineNumber++;
		if (printFlag) 
		{
			char printBuffer[MAXLEN_SOURCELINE + 9];
			sprintf(printBuffer, "%4d %d: %s", lineNumber, level, sourceBuffer);
			printLine(printBuffer);
		}
		return(true);
	}
	else
		return(false);
}

//---------------------------------------------------------------------------

int32_t openSourceFile (PSTR sourceFileName) {

	//---------------------------------------
	// Now, let's open the ABL source file...
	if (!sourceFileName)
		return(-1);

	if (NumOpenFiles == MAX_INCLUDE_DEPTH)
		return(-2);

	if (NumSourceFiles == MAX_SOURCE_FILES)
		return(-3);

	ABLFile* sFile = NULL;
	sFile = new ABLFile;

	if (sFile->open(sourceFileName) != ABL_NO_ERR)
		return(-3);

	strcpy(SourceFiles[NumSourceFiles], sourceFileName);
	FileNumber = NumSourceFiles++;

	sourceFile = sFile;
	strcpy(openFiles[NumOpenFiles].fileName, sourceFileName);
	openFiles[NumOpenFiles].filePtr = sFile;
	openFiles[NumOpenFiles].fileNumber = (uint8_t)FileNumber;
	openFiles[NumOpenFiles].lineNumber = 0;
	if (NumOpenFiles > 0)
		openFiles[NumOpenFiles - 1].lineNumber = lineNumber;
	NumOpenFiles++;

	lineNumber = 0;
	sourceBuffer[0] = NULL;
	bufferp = sourceBuffer;
	getChar();

	return(ABL_NO_ERR);
}

//---------------------------------------------------------------------------

int32_t closeSourceFile (void) {

	if (NumOpenFiles == 0)
		return(-1);

	sourceFile->close();

	delete sourceFile;
	sourceFile = NULL;

	openFiles[NumOpenFiles].filePtr = NULL;
	NumOpenFiles--;

	if (NumOpenFiles > 0) {
		sourceFile = openFiles[NumOpenFiles - 1].filePtr;
		FileNumber = openFiles[NumOpenFiles - 1].fileNumber;
		lineNumber = openFiles[NumOpenFiles - 1].lineNumber;
	}

	return(ABL_NO_ERR);
}

//***************************************************************************
// PRINTOUT routines
//***************************************************************************

void printLine (PSTR line) {

	if (++lineCount > MAX_LINES_PER_PAGE) {
		printPageHeader();
		lineCount = 1;
	}

	PSTR saveChPtr = NULL;
	char saveCh = ' ';
	if (strlen(line) > MAXLEN_PRINTLINE) {
		saveCh = line[MAXLEN_PRINTLINE];
		saveChPtr = &line[MAXLEN_PRINTLINE];
		*saveChPtr = NULL;
	}
	printf("%s", line);
	if (saveChPtr)
		*saveChPtr = saveCh;
}

//***************************************************************************

void initPageHeader (PSTR fileName) {

	//--------------------------
	// Save the source file name
	strncpy(sourceName, fileName, MAXLEN_FILENAME - 1);

	//-------------------------------
	// Save the current time and date
	time_t timer;
	time(&timer);
	strcpy(date, asctime(localtime(&timer)));
}

//***************************************************************************

void printPageHeader (void) {

	//putchar(CHAR_FORMFEED);
	printf("Page %d   %s   %s\n\n", ++pageNumber, sourceName, date);
}

//***************************************************************************

