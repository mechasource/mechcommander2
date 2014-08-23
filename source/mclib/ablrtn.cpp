//===========================================================================//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//
//***************************************************************************
//
//								ABLRTN.CPP
//
//***************************************************************************
#include "stdafx.h"

//#include <stdio.h>
//#include <stdlib.h>
//#include <string.h>

#ifndef ABLGEN_H
#include "ablgen.h"
#endif

#ifndef ABLERR_H
#include "ablerr.h"
#endif

#ifndef ABLSCAN_H
#include "ablscan.h"
#endif

#ifndef ABLSYMT_H
#include "ablsymt.h"
#endif

#ifndef ABLPARSE_H
#include "ablparse.h"
#endif

#ifndef ABLEXEC_H
#include "ablexec.h"
#endif


#ifndef ABLENV_H
#include "ablenv.h"
#endif

#ifndef ABL_H
#include "abl.h"
#endif

#ifndef ABLDBUG_H
#include "abldbug.h"
#endif

//***************************************************************************

extern int32_t				MaxBreaks;
extern int32_t				MaxWatches;
extern DebuggerPtr		debugger;
extern int32_t				NumModules;
extern ModuleEntryPtr	ModuleRegistry;
extern ABLModulePtr*	ModuleInstanceRegistry;
extern int32_t				MaxModules;
extern ABLModulePtr*	LibraryInstanceRegistry;
extern int32_t				MaxLibraries;
extern int32_t				NumModulesRegistered;
extern int32_t				NumModuleInstances;
extern int32_t				MaxWatchesPerModule;
extern int32_t				MaxBreakPointsPerModule;
extern int32_t				MaxCodeBufferSize;
extern ABLModulePtr		CurModule;
extern ABLModulePtr		CurLibrary;
extern PSTR			codeBuffer;
extern PSTR			codeBufferPtr;
extern PSTR			codeSegmentPtr;
extern PSTR			codeSegmentLimit;
extern PSTR			statementStartPtr;
extern StackItem*		stack;
extern StackItemPtr		tos;
extern StackItemPtr		stackFrameBasePtr;
extern StackItemPtr		StaticDataPtr;
extern int32_t*			StaticVariablesSizes;
extern int32_t*			EternalVariablesSizes;
extern int32_t				MaxEternalVariables;
extern int32_t				eternalOffset;
extern int32_t				MaxStaticVariables;
extern int32_t				NumStaticVariables;
extern int32_t				NumOrderCalls;
extern StateHandleInfo	StateHandleList[MAX_STATE_HANDLES_PER_MODULE];
extern int32_t				NumStateHandles;
extern int32_t				CurModuleHandle;
extern bool				CallModuleInit;
extern bool				AutoReturnFromOrders;
extern int32_t				MaxLoopIterations;
extern bool				AssertEnabled;
extern bool				IncludeDebugInfo;
extern bool				ProfileABL;
extern bool				Crunch;
extern int32_t				level;
extern int32_t				lineNumber;
extern int32_t				FileNumber;
extern ABLFile*		sourceFile;
extern bool				printFlag;
extern bool				blockFlag;
extern BlockType		blockType;
extern SymTableNodePtr	CurModuleIdPtr;
extern SymTableNodePtr	CurRoutineIdPtr;
extern bool				DumbGetCharOn;
extern int32_t				NumOpenFiles;
extern int32_t				NumSourceFiles;
extern int32_t				NumLibrariesUsed;
extern ABLModulePtr		LibrariesUsed[MAX_LIBRARIES_USED];
extern int32_t				bufferOffset;
extern PSTR			bufferp;
extern PSTR			tokenp;
extern int32_t				digitCount;
extern bool				countError;
extern int32_t				pageNumber;
extern int32_t				lineCount;

extern int32_t				CurAlarm;

extern bool				eofFlag;
extern bool				ExitWithReturn;
extern bool				ExitFromTacOrder;

extern int32_t				dummyCount;

extern int32_t				lineNumber;
extern int32_t				FileNumber;
extern int32_t				errorCount;
extern int32_t				execStatementCount;
extern int32_t				NumSourceFiles;
extern char				SourceFiles[MAX_SOURCE_FILES][MAXLEN_FILENAME];
 
extern TokenCodeType	curToken;
extern char				wordString[];
extern SymTableNodePtr	symTableDisplay[];
extern int32_t				level;
extern bool				blockFlag;
extern BlockType		blockType;
extern bool				printFlag;
extern SymTableNodePtr	CurRoutineIdPtr;

extern Type				DummyType;
extern StackItem*		stack;
//extern StackItem*		eternalStack;
extern StackItemPtr		tos;
extern StackItemPtr		stackFrameBasePtr;
extern int32_t				eternalOffset;

extern TokenCodeType	statementStartList[];
extern TokenCodeType	statementEndList[];
extern TokenCodeType	declarationStartList[];

extern char				tokenString[MAXLEN_TOKENSTRING];

extern CharCodeType		charTable[256];

extern char				sourceBuffer[MAXLEN_SOURCELINE];
extern int32_t				bufferOffset;
extern PSTR			bufferp;
extern PSTR			tokenp;

extern int32_t				digitCount;
extern bool				countError;

extern bool				eofFlag;
extern int32_t				pageNumber;

extern SymTableNodePtr	SymTableDisplay[MAX_NESTING_LEVEL];
extern TypePtr			IntegerTypePtr;
extern TypePtr			RealTypePtr;
extern TypePtr			BooleanTypePtr;

extern StackItemPtr		StaticDataPtr;
extern StackItem		returnValue;

extern ModuleEntryPtr	ModuleRegistry;
extern int32_t				MaxModules;
extern int32_t				NumModulesRegistered;
extern int32_t				MaxStaticVariables;
extern int32_t				NumStaticVariables;
extern int32_t				NumSourceFiles;

extern bool				IncludeDebugInfo;
extern bool				AssertEnabled;
extern bool				PrintEnabled;
extern bool				StringFunctionsEnabled;
extern bool				DebugCodeEnabled;
extern DebuggerPtr		debugger;

bool					ABLenabled = false;
char					buffer[MAXLEN_PRINTLINE];

extern int32_t				CallStackLevel;
extern bool				SkipOrder;

extern ABLModulePtr		CurFSM;
extern bool				NewStateSet;

extern void transState (SymTableNodePtr newState);

int32_t					numLibrariesLoaded = 0;
int32_t					NumExecutions = 0;

PVOID (*ABLSystemMallocCallback) (uint32_t memSize) = NULL;
PVOID (*ABLStackMallocCallback) (uint32_t memSize) = NULL;
PVOID (*ABLCodeMallocCallback) (uint32_t memSize) = NULL;
PVOID (*ABLSymbolMallocCallback) (uint32_t memSize) = NULL;
void (*ABLSystemFreeCallback) (PVOID memBlock) = NULL;
void (*ABLStackFreeCallback) (PVOID memBlock) = NULL;
void (*ABLCodeFreeCallback) (PVOID memBlock) = NULL;
void (*ABLSymbolFreeCallback) (PVOID memBlock) = NULL;
void (*ABLDebugPrintCallback) (PSTR s) = NULL;
int32_t (*ABLRandomCallback) (int32_t range) = NULL;
void (*ABLSeedRandomCallback) (uint32_t range) = NULL;
uint32_t (*ABLGetTimeCallback) (void) = NULL;
void (*ABLFatalCallback) (int32_t code, PSTR s) = NULL;
void (*ABLEndlessStateCallback) (UserFile* log) = NULL;

//***************************************************************************

TokenCodeType followHeaderList[] = {
	TKN_SEMICOLON,
	TKN_EOF,
	TKN_NONE
};

TokenCodeType followModuleIdList[] = {
	TKN_LPAREN,
	TKN_COLON,
	TKN_SEMICOLON,
	TKN_EOF,
	TKN_NONE
};

TokenCodeType followFunctionIdList[] = {
	TKN_LPAREN,
	TKN_COLON,
	TKN_SEMICOLON,
	TKN_EOF,
	TKN_NONE
};

TokenCodeType followParamsList[] = {
	TKN_RPAREN,
	TKN_COMMA,
	TKN_EOF,
	TKN_NONE
};

TokenCodeType followParamList[] = {
	TKN_COMMA,
	TKN_RPAREN,
	TKN_NONE
};

TokenCodeType followModuleDeclsList[] = {
	TKN_SEMICOLON,
	TKN_CODE,
	TKN_EOF,
	TKN_NONE
};

TokenCodeType followRoutineDeclsList[] = {
	TKN_SEMICOLON,
	TKN_CODE,
	TKN_EOF,
	TKN_NONE
};

TokenCodeType followDeclsList[] = {
	TKN_SEMICOLON,
	TKN_EOF,
	TKN_NONE
};

void ABL_CloseProfileLog (void);
void ABL_OpenProfileLog (void);
void ABL_AddToProfileLog (PSTR profileString);

//***************************************************************************
// ABL library interface routines
//***************************************************************************

int32_t DefaultRandom (int32_t /* range */) {

	return(0);
}

//---------------------------------------------------------------------------

void DefaultSeedRandom (uint32_t /* seed */) {
}

//---------------------------------------------------------------------------

void DefaultDebugPrintCallback (PSTR /* s */) {
}

//---------------------------------------------------------------------------

uint32_t DefaultGetTimeCallback (void) {

	return(0);
}

//---------------------------------------------------------------------------

void ABLi_setRandomCallbacks (void (*seedRandomCallback) (uint32_t seed),
							  int32_t (*randomCallback) (int32_t range)) {

	ABLSeedRandomCallback = seedRandomCallback;
	ABLRandomCallback = randomCallback;
}

//---------------------------------------------------------------------------

void ABLi_setDebugPrintCallback (void (*debugPrintCallback) (PSTR s)) {

	ABLDebugPrintCallback = debugPrintCallback;
}

//---------------------------------------------------------------------------

void ABLi_setGetTimeCallback (uint32_t (*getTimeCallback) (void)) {

	ABLGetTimeCallback = getTimeCallback;
}

//---------------------------------------------------------------------------

void ABLi_setEndlessStateCallback (void (*endlessStateCallback) (UserFile* log)) {

	ABLEndlessStateCallback = endlessStateCallback;
}

//---------------------------------------------------------------------------

void ABLi_init (uint32_t runtimeStackSize,
				uint32_t maxCodeBufferSize,
				uint32_t maxRegisteredModules,
				uint32_t maxStaticVariables,
				PVOID (*systemMallocCallback) (uint32_t memSize),
				PVOID (*stackMallocCallback) (uint32_t memSize),
				PVOID (*codeMallocCallback) (uint32_t memSize),
				PVOID (*symbolMallocCallback) (uint32_t memSize),
				void (*systemFreeCallback) (PVOID memBlock),
				void (*stackFreeCallback) (PVOID memBlock),
				void (*codeFreeCallback) (PVOID memBlock),
				void (*symbolFreeCallback) (PVOID memBlock),
				int32_t (*fileCreateCB) (PVOID* file, PSTR fName),
				int32_t (*fileOpenCB) (PVOID* file, PSTR fName),
				int32_t (*fileCloseCB) (PVOID* file),
				bool (*fileEofCB) (PVOID file),
				int32_t (*fileReadCB) (PVOID file, puint8_t buffer, int32_t length),
				int32_t (*fileReadLongCB) (PVOID file),
				int32_t (*fileReadStringCB) (PVOID file, puint8_t buffer),
				int32_t (*fileReadLineExCB) (PVOID file, puint8_t buffer, int32_t maxLength),
				int32_t (*fileWriteCB) (PVOID file, puint8_t buffer, int32_t length),
				int32_t (*fileWriteByteCB) (PVOID file, uint8_t byte),
				int32_t (*fileWriteLongCB) (PVOID file, int32_t value),
				int32_t (*fileWriteStringCB) (PVOID file, PSTR buffer),
				void (*debuggerPrintCallback) (PSTR s),
				void (*fatalCallback) (int32_t code, PSTR s),
				bool debugInfo,
				bool debug,
				bool profile) {

// HACK, for testing...
#ifdef _DEBUG
profile = true;
#endif
//
	//-------------------------------------------------------------------
	// Let's make sure we have not created too many built-in ABL routines
	// for the lovely user...
#if (NUM_ABL_ROUTINES > 254)
#error MAJOR ABL ERROR: Too Many ABL Routines
#endif

	ABLSystemMallocCallback = systemMallocCallback;
	ABLStackMallocCallback = stackMallocCallback;
	ABLCodeMallocCallback = codeMallocCallback;
	ABLSymbolMallocCallback = symbolMallocCallback;
	ABLSystemFreeCallback = systemFreeCallback;
	ABLStackFreeCallback = stackFreeCallback;
	ABLCodeFreeCallback = codeFreeCallback;
	ABLSymbolFreeCallback = symbolFreeCallback;
	ABLDebugPrintCallback = DefaultDebugPrintCallback;
	ABLRandomCallback = DefaultRandom;
	ABLSeedRandomCallback = DefaultSeedRandom;
	ABLGetTimeCallback = DefaultGetTimeCallback;
	ABLFatalCallback = fatalCallback;

	ABLFile::createCB = fileCreateCB;
	ABLFile::openCB = fileOpenCB;
	ABLFile::closeCB = fileCloseCB;
	ABLFile::eofCB = fileEofCB;
	ABLFile::readCB = fileReadCB;
	ABLFile::readLongCB = fileReadLongCB;
	ABLFile::readStringCB = fileReadStringCB;
	ABLFile::readLineExCB = fileReadLineExCB;
	ABLFile::writeCB = fileWriteCB;
	ABLFile::writeByteCB = fileWriteByteCB;
	ABLFile::writeLongCB = fileWriteLongCB;
	ABLFile::writeStringCB = fileWriteStringCB;

	NumStandardFunctions = NUM_ABL_ROUTINES;

	ABLenabled = true;
	MaxBreaks = 50;
	MaxWatches = 50;
	debugger = NULL;
	NumModules = 0;
	ModuleRegistry = NULL;
	ModuleInstanceRegistry = NULL;
	LibraryInstanceRegistry = NULL;
	MaxModules = 0;
	MaxLibraries = 0;
	NumModulesRegistered = 0;
	NumModuleInstances = 0;
	MaxWatchesPerModule = 20;
	MaxBreakPointsPerModule = 20;
	CurModule = NULL;
	errorCount = 0;
	codeBuffer = NULL;
	codeBufferPtr = NULL;
	codeSegmentPtr = NULL;
	codeSegmentLimit = NULL;
	statementStartPtr = NULL;
	execStatementCount = 0;
	stack = NULL;
	tos = NULL;
	stackFrameBasePtr = NULL;
	StaticDataPtr = NULL;
	StaticVariablesSizes = NULL;
	EternalVariablesSizes = NULL;
	MaxEternalVariables = 5000;
	eternalOffset = 0;
	MaxStaticVariables = 0;
	NumStaticVariables = 0;
	NumOrderCalls = 0;
	NumStateHandles = 1;
	CurModuleHandle = 0;
	CallModuleInit = false;
	AutoReturnFromOrders = false;
	MaxLoopIterations = 100001;
	AssertEnabled = false;
	PrintEnabled = false;
	StringFunctionsEnabled = true;
	IncludeDebugInfo = true;
	ProfileABL = profile;
	Crunch = true;
	level = 0;
	lineNumber = 0;
	FileNumber = 0;
	sourceFile = NULL;
	printFlag = true;
	blockFlag = false;
	blockType = BLOCK_MODULE;
	CurModuleIdPtr = NULL;
	CurRoutineIdPtr = NULL;
	DumbGetCharOn = false;
	NumOpenFiles = 0;
	NumSourceFiles = 0;
	bufferOffset = 0;
	bufferp = sourceBuffer;
	tokenp = tokenString;
	digitCount = 0;
	countError = false;
	pageNumber = 0;
	lineCount = MAX_LINES_PER_PAGE;

	eofFlag = false;
	ExitWithReturn = false;
	ExitFromTacOrder = false;
	dummyCount = 0;
	numLibrariesLoaded = 0;
	
	//----------------------------------
	// Initialize the character table...
	int32_t curCh;
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

	//--------------------------------------------------------------------
	// Allocate the code buffer used during pre-processing/interpreting...
	MaxCodeBufferSize = maxCodeBufferSize;
	codeBuffer = (PSTR)ABLCodeMallocCallback(maxCodeBufferSize);
	if (!codeBuffer)
		ABL_Fatal(0, " ABL: Unable to AblCodeHeap->malloc preprocess code buffer ");
	
	//----------------------------------------------------------------------
	// Alloc the static variable data block. Ultimately, we may want to just
	// implement the static data construction with a linked list, rather
	// than a set max size...
	MaxStaticVariables = maxStaticVariables;
	NumStaticVariables = 0;
	NumOrderCalls = 0;
	NumStateHandles = 1;
	StaticDataPtr = NULL;
	StaticVariablesSizes = NULL;
	if (MaxStaticVariables > 0) {
		StaticVariablesSizes = (int32_t*)ABLStackMallocCallback(sizeof(int32_t) * MaxStaticVariables);
		if (!StaticVariablesSizes)
			ABL_Fatal(0, " ABL: Unable to AblStackHeap->malloc StaticVariablesSizes ");
	}

	//------------------------------
	// Allocate the runtime stack...
	stack = (StackItemPtr)ABLStackMallocCallback(sizeof(StackItem) * (runtimeStackSize / sizeof(StackItem)));
	if (!stack)
		ABL_Fatal(0, " ABL: Unable to AblStackHeap->malloc stack ");

	//-----------------------------------
	// Allocate Eternal Vars Size List...
	if (MaxEternalVariables > 0) {
		EternalVariablesSizes = (int32_t*)ABLStackMallocCallback(sizeof(int32_t) * MaxEternalVariables);
		if (!EternalVariablesSizes)
			ABL_Fatal(0, " ABL: Unable to AblStackHeap->malloc EternalVariablesSizes ");
	}

	//-------------------------
	// Prep the symbol table...
	initSymTable();

	initModuleRegistry(maxRegisteredModules);

	initLibraryRegistry(10);

	//--------------------------------
	// Init the debugger, if needed...
	IncludeDebugInfo = debugInfo;
	if (debug) {
		IncludeDebugInfo = true;
		debugger = new Debugger;
		if (!debugger)
			ABL_Fatal(0, " Unable to initialize ABL Debugger. ");
		debugger->init(debuggerPrintCallback, NULL);
	}

	UserFile::setup();

	if (ProfileABL)
		ABL_OpenProfileLog();

#ifdef TEST_ABLFILE
	ABLFile* aFile = new ABLFile;
	aFile->create("ablFile.txt");
	aFile->writeString("Testing writeString");
	aFile->writeByte(NULL);
	aFile->writeLong(100);
	aFile->close();
	delete aFile;
	aFile = NULL;

	uint8_t s[100];
	ABLFile* bFile = new ABLFile;
	bFile->open("ablFile.txt");
	bFile->readString(s);
	int32_t val = bFile->readLong();
	bFile->close();
	delete bFile;
	bFile = NULL;
#endif
}

//***************************************************************************

int32_t ABLi_preProcess (PSTR sourceFileName, int32_t* numErrors, int32_t* numLinesProcessed, int32_t* numFilesProcessed, bool printLines) {

	//--------------------------------------------------------------------------------
	// First, check if this module has already been registered into the environment...
	int32_t i;
	for (i = 0; i < NumModulesRegistered; i++)
		if (strcmp(strlwr(sourceFileName), ModuleRegistry[i].fileName) == 0)
			return(i);

	//---------------------------------
	// Init some important variables...
	level = 0;
	lineNumber = 0;
	FileNumber = 0;
	AssertEnabled = (debugger != NULL);
	PrintEnabled = (debugger != NULL);
	StringFunctionsEnabled = true;
	DebugCodeEnabled = (debugger != NULL);
	NumSourceFiles = 0;
	NumLibrariesUsed = 0;
	sourceFile = NULL;
	printFlag = printLines;
	blockFlag = false;
	blockType = BLOCK_MODULE;
	bufferOffset = 0;
	bufferp = sourceBuffer;
	tokenp = tokenString;
	digitCount = 0;
	countError = false;
	pageNumber = 0;
	errorCount = 0;
	execStatementCount = 0;
	eofFlag = false;
	NumStaticVariables = 0;
	NumOrderCalls = 0;
	NumStateHandles = 1;
	for (i = 0; i < MAX_STATE_HANDLES_PER_MODULE; i++) {
		StateHandleList[i].name[0] = NULL;
		StateHandleList[i].state = NULL;
	}

	if (numErrors)
		*numErrors = 0;

	if (numLinesProcessed)
		*numLinesProcessed = 0;

	//---------------------------------------
	// Now, let's open the ABL source file...	
	int32_t openErr = ABL_NO_ERR;
	if ((openErr = openSourceFile(sourceFileName)) != ABL_NO_ERR)
		return(openErr);

	//------------------------
	// Set up the code buffer.
	codeBufferPtr = codeBuffer;

	//------------------
	// Get it rolling...
	getToken();

	SymTableNodePtr moduleIdPtr = moduleHeader();
	CurModuleIdPtr = moduleIdPtr;
	CurRoutineIdPtr = moduleIdPtr;

	//---------------------
	// Error synchronize...
	synchronize(followHeaderList, declarationStartList, statementStartList);
	if (curToken == TKN_SEMICOLON)
		getToken();
	else if (tokenIn(declarationStartList) || tokenIn(statementStartList))
		syntaxError(ABL_ERR_SYNTAX_MISSING_SEMICOLON);

	//-------------------------------------------------------------
	// Parse the module's block. Use to call block(), now we do the
	// stuff right here...
	//block(moduleIdPtr);
	declarations(moduleIdPtr, true);
	if ((moduleIdPtr->defn.info.routine.flags & ROUTINE_FLAG_FSM) == 0) {
		synchronize(followModuleDeclsList, NULL, NULL);
		if (curToken != TKN_CODE)
			syntaxError(ABL_ERR_SYNTAX_MISSING_CODE);
		crunchToken();
		blockType = BLOCK_MODULE;
		blockFlag = true;

		getToken();

		TokenCodeType endToken = TKN_END_MODULE;
		if (CurLibrary)
			endToken = TKN_END_LIBRARY;
		if (curToken != endToken)
			do {
				statement();
				while (curToken == TKN_SEMICOLON)
					getToken();
				if (curToken == endToken)
					break;
		
				//------------------------------------
				// Synchronize after possible error...
				synchronize(statementStartList, NULL, NULL);
			} while (tokenIn(statementStartList));
	}

	if (CurLibrary)
		ifTokenGetElseError(TKN_END_LIBRARY, ABL_ERR_SYNTAX_MISSING_END_LIBRARY);
	else if (moduleIdPtr->defn.info.routine.flags & ROUTINE_FLAG_FSM)
		ifTokenGetElseError(TKN_END_FSM, ABL_ERR_SYNTAX_MISSING_END_FSM);
	else
		ifTokenGetElseError(TKN_END_MODULE, ABL_ERR_SYNTAX_MISSING_END_MODULE);

	blockFlag = false;

	moduleIdPtr->defn.info.routine.localSymTable = exitScope();
	moduleIdPtr->defn.info.routine.codeSegment = createCodeSegment(moduleIdPtr->defn.info.routine.codeSegmentSize);

	analyzeBlock(moduleIdPtr->defn.info.routine.codeSegment);

	ifTokenGetElseError(TKN_PERIOD, ABL_ERR_SYNTAX_MISSING_PERIOD);

	while (curToken != TKN_EOF) {
		syntaxError(TKN_UNEXPECTED_TOKEN);
		getToken();
	}

	//------------------------------------------
	// Done with the source file, so close it...
	closeSourceFile();

	//extractSymTable(&SymTableDisplay[0], moduleIdPtr);

	//--------------------------------------------------
	// Register the new module in the ABL environment...
	ModuleRegistry[NumModulesRegistered].fileName = (PSTR)ABLStackMallocCallback(strlen(sourceFileName) + 1);
	if (!ModuleRegistry[NumModulesRegistered].fileName)
		ABL_Fatal(0, " ABL: Unable to AblStackHeap->malloc module filename ");
	strcpy(ModuleRegistry[NumModulesRegistered].fileName, strlwr(sourceFileName));
	ModuleRegistry[NumModulesRegistered].moduleIdPtr = moduleIdPtr;

	ModuleRegistry[NumModulesRegistered].numSourceFiles = NumSourceFiles;
	ModuleRegistry[NumModulesRegistered].sourceFiles = (PSTR*)ABLStackMallocCallback(NumSourceFiles * sizeof(PSTR));
	if (!ModuleRegistry[NumModulesRegistered].sourceFiles)
		ABL_Fatal(0, " ABL: Unable to AblStackHeap->malloc sourceFiles ");
	for (i = 0; i < NumSourceFiles; i++) {
		ModuleRegistry[NumModulesRegistered].sourceFiles[i] = (PSTR)ABLStackMallocCallback(strlen(SourceFiles[i]) + 1);
		strcpy(ModuleRegistry[NumModulesRegistered].sourceFiles[i], SourceFiles[i]);
	}

	if (NumLibrariesUsed > 0) {
		ModuleRegistry[NumModulesRegistered].numLibrariesUsed = NumLibrariesUsed;
		ModuleRegistry[NumModulesRegistered].librariesUsed = (ABLModulePtr*)ABLStackMallocCallback(NumLibrariesUsed * sizeof(SymTableNodePtr));
		if (!ModuleRegistry[NumModulesRegistered].librariesUsed)
			ABL_Fatal(0, " ABL: Unable to AblStackHeap->malloc librariesUsed ");
		for (i = 0; i < NumLibrariesUsed; i++)
			ModuleRegistry[NumModulesRegistered].librariesUsed[i] = LibrariesUsed[i];
	}

	ModuleRegistry[NumModulesRegistered].numStaticVars = NumStaticVariables;
	ModuleRegistry[NumModulesRegistered].sizeStaticVars	= NULL;
	ModuleRegistry[NumModulesRegistered].totalSizeStaticVars = 0;
	if (NumStaticVariables) {
		ModuleRegistry[NumModulesRegistered].sizeStaticVars = (int32_t*)ABLStackMallocCallback(sizeof(int32_t) * NumStaticVariables);
		if (!ModuleRegistry[NumModulesRegistered].sizeStaticVars)
			ABL_Fatal(0, " ABL: Unable to AblStackHeap->malloc module sizeStaticVars ");
		memcpy(ModuleRegistry[NumModulesRegistered].sizeStaticVars, StaticVariablesSizes, sizeof(int32_t) * NumStaticVariables);
		ModuleRegistry[NumModulesRegistered].totalSizeStaticVars = sizeof(int32_t) * NumStaticVariables;
		for (int32_t i = 0; i < ModuleRegistry[NumModulesRegistered].numStaticVars; i++)
			ModuleRegistry[NumModulesRegistered].totalSizeStaticVars += ModuleRegistry[NumModulesRegistered].sizeStaticVars[i];
	}
	ModuleRegistry[NumModulesRegistered].numOrderCalls = NumOrderCalls;
	ModuleRegistry[NumModulesRegistered].numInstances = 0;



	ModuleRegistry[NumModulesRegistered].numStateHandles = NumStateHandles;
	if (NumStateHandles > 1) {
		ModuleRegistry[NumModulesRegistered].stateHandles = (StateHandleInfoPtr)ABLStackMallocCallback(sizeof(StateHandleInfo) * NumStateHandles);
		memcpy(ModuleRegistry[NumModulesRegistered].stateHandles, StateHandleList, sizeof(StateHandleInfo) * NumStateHandles);
	}
	
	NumModulesRegistered++;

	//---------------------------------------------------------------
	// Now, exit with the number of source lines processed, if any...
	if (numLinesProcessed)
		*numLinesProcessed = lineNumber;

	if (numFilesProcessed)
		*numFilesProcessed = FileNumber;

	if (numErrors)
		*numErrors = errorCount;

	return(NumModulesRegistered - 1);
}

//***************************************************************************

int32_t ABLi_execute (SymTableNodePtr  moduleIdPtr , SymTableNodePtr /* functionIdPtr */, 
				   ABLParamPtr paramList, StackItemPtr returnVal) 
{

	//insertSymTable(&SymTableDisplay[0], moduleIdPtr);

	//--------------------------
	// Execute the ABL module...

	//----------------------------------------------------------------------------------
	// NOTE: Ultimately, we want the ABL virtual machine to be set up once, and then
	// ABL modules/routines can be called multiple times from within a program. For now,
	// to test, let's just assume every time we run a module, it is a self-contained
	// ABL program.

	//---------------------------------
	// Init some important variables...
	//lineNumber = 0;
	//printFlag = true;
	//blockFlag = false;
	//blockType = BLOCK_MODULE;
	CurModuleIdPtr = NULL;
	CurRoutineIdPtr = NULL;
	//bufferOffset = 0;
	//bufferp = sourceBuffer;
	//tokenp = tokenString;
	//digitCount = 0;
	//countError = false;
	errorCount = 0;
	execStatementCount = 0;

	NumExecutions++;

	//------------------
	// Init the stack...
	stackFrameBasePtr = tos = (stack + eternalOffset);

	//---------------------------------------
	// Initialize the module's stack frame...
	level = 1;
	CallStackLevel = 0;
	stackFrameBasePtr = tos + 1;
	
	//-------------------------
	// Function return value...
	pushInteger(0);
	//---------------
	// Static Link...
	pushAddress(NULL);
	//----------------
	// Dynamic Link...
	pushAddress(NULL);
	//------------------
	// Return Address...
	pushAddress(NULL);

	//initDebugger();

	//----------
	// Run it...

	if (paramList) {
		//------------------------------------------------------------------------------
		// NOTE: Currently, parameter passing of arrays is not functioning. This MUST be
		// done...
		int32_t curParam = 0;
		for (SymTableNodePtr formalIdPtr = (SymTableNodePtr)(moduleIdPtr->defn.info.routine.params);
			 formalIdPtr != NULL;
			 formalIdPtr = formalIdPtr->next) {

			TypePtr formalTypePtr = (TypePtr)(formalIdPtr->typePtr);

			if (formalIdPtr->defn.key == DFN_VALPARAM) {

				if (formalTypePtr == RealTypePtr) {
					if (paramList[curParam].type == ABL_PARAM_INTEGER) {
						//---------------------------------------------
						// Real formal parameter, but integer actual...
						pushReal((float)(paramList[curParam].integer));
						}
					else if (paramList[curParam].type == ABL_PARAM_REAL)
						pushReal(paramList[curParam].real);
					}
				else if (formalTypePtr == IntegerTypePtr) {
					if (paramList[curParam].type== ABL_PARAM_INTEGER)
						pushInteger(paramList[curParam].integer);
					else
						return(0);
				}

				//----------------------------------------------------------
				// Formal parameter is an array or record, so make a copy...
				if ((formalTypePtr->form == FRM_ARRAY)/* || (formalTypePtr->form == FRM_RECORD)*/) {
					//------------------------------------------------------------------------------
					// The following is a little inefficient, but is kept this way to keep it clear.
					// Once it's verified to work, optimize...
					int32_t size = formalTypePtr->size;
					PSTR dest = (PSTR)ABLStackMallocCallback((size_t)size);
					if (!dest)
						ABL_Fatal(0, " ABL: Unable to AblStackHeap->malloc module formal array param ");
					PSTR src = tos->address;
					PSTR savePtr = dest;
					memcpy(dest, src, size);
					tos->address = savePtr;
				}
				}
			else {
				//-------------------------------
				// pass by reference parameter...
				if (formalTypePtr == RealTypePtr)
					pushAddress((Address)&(paramList[curParam].real));
				else if (formalTypePtr == IntegerTypePtr)
					pushAddress((Address)&(paramList[curParam].integer));
				else
					return(0);
				//SymTableNodePtr idPtr = getCodeSymTableNodePtr();
				//execVariable(idPtr, USE_REFPARAM);
			}
			curParam++;
		}
	}

	execute(moduleIdPtr);

	if (returnVal)
		memcpy(returnVal, &returnValue, sizeof(StackItem));

	//extractSymTable(&SymTableDisplay[0], moduleIdPtr);

	//-----------
	// Summary...
	return(execStatementCount);
}

//***************************************************************************

int32_t ABLi_deleteModule (SymTableNodePtr /* moduleIdPtr */) {

	return(ABL_NO_ERR);
}

//***************************************************************************

void ABLi_close (void) {

	if (!codeBuffer)
		return;

	UserFile::cleanup();

	destroyModuleRegistry();

	destroyLibraryRegistry();

	if (StaticVariablesSizes) {
		ABLStackFreeCallback(StaticVariablesSizes);
		StaticVariablesSizes = NULL;
	}

	if (EternalVariablesSizes) {
		ABLStackFreeCallback(EternalVariablesSizes);
		EternalVariablesSizes = NULL;
	}
	
	if (codeBuffer) {
		ABLCodeFreeCallback(codeBuffer);
		codeBuffer = NULL;
	}

	if (stack) {
		ABLStackFreeCallback(stack);
		stack = NULL;
	}

	if (debugger) {
		delete debugger;
		debugger = NULL;
	}

	ABL_CloseProfileLog();

	ABLenabled = false;
}

//***************************************************************************

ABLModulePtr ABLi_loadLibrary (PSTR sourceFileName, int32_t* numErrors, int32_t* numLinesProcessed, int32_t* numFilesProcessed, bool printLines, bool createInstance) {

	//--------------------------------------------------------------------
	// Create an instance of it so it may be used from other modules. Note
	// that we need this when preprocessing since all identifiers in the
	// library should point to this module...
	ABLModulePtr library = new ABLModule;
	if (!library)
		ABL_Fatal(0, "ABL: no RAM for library");

	CurLibrary = library;

	//-------------------------------------------------------------
	// Preprocess the library. Note that a library should be loaded
	// just once.
	int32_t libraryHandle = ABLi_preProcess(sourceFileName, numErrors, numLinesProcessed, numFilesProcessed, printLines);
	if (libraryHandle < (NumModulesRegistered - 1)) {
		//------------------
		// Already loaded...
		delete library;
		library = NULL;
		CurLibrary = NULL;
		return(NULL);
	}

	CurLibrary = NULL;

	LibraryInstanceRegistry[numLibrariesLoaded] = library;
	numLibrariesLoaded++;

	if (!createInstance) {
		CurLibrary = NULL;
		return(library);
	}

	int32_t err = library->init(libraryHandle);
	ABL_Assert(err == ABL_NO_ERR, err, " Error Loading ABL Library ");

	library->setName(sourceFileName);

	return(library);
}

//***************************************************************************

ABLParamPtr ABLi_createParamList (int32_t numParameters) {

	if (numParameters) {
		ABLParamPtr paramList = (ABLParamPtr)ABLStackMallocCallback(sizeof(ABLParam) * (numParameters + 1));
		if (!paramList)
			ABL_Fatal(0, " ABL: Unable to AblStackHeap->malloc paramList ");
		memset(paramList, 0, sizeof(ABLParam) * (numParameters + 1)); 
		return(paramList);
	}
	return(NULL);
}

//***************************************************************************

void ABLi_setIntegerParam (ABLParamPtr paramList, int32_t index, int32_t value) {

	if (paramList) {
		paramList[index].type = ABL_PARAM_INTEGER;
		paramList[index].integer = value;
	}
}

//***************************************************************************

void ABLi_setRealParam (ABLParamPtr paramList, int32_t index, float value) {

	if (paramList) {
		paramList[index].type = ABL_PARAM_REAL;
		paramList[index].real = value;
	}
}

//***************************************************************************

void ABLi_deleteParamList (ABLParamPtr paramList) {

	if (paramList)
		ABLStackFreeCallback(paramList);
}

//***************************************************************************

ABLModulePtr ABLi_getModule (int32_t id) {

	if ((id >= 0) && (id < NumModules))
		return(ModuleInstanceRegistry[id]);
	return(NULL);
}

//***************************************************************************

bool ABLi_enabled (void) {

	return(ABLenabled);
}

//***************************************************************************

void ABLi_addFunction (PSTR name,
					   bool isOrder,
					   PSTR paramList,
					   PSTR returnType,
					   void (*codeCallback)(void)) {

	enterStandardRoutine(name, -1, isOrder, paramList, returnType, codeCallback);
}

//***************************************************************************

int32_t ABLi_registerInteger (PSTR name, int32_t* address, int32_t numElements) {

	if (strlen(name) >= MAXLEN_TOKENSTRING)
		ABL_Fatal(0, " ABLi_registerInteger: variable name too int32_t ");

	level = 0;
	strcpy(wordString, name);
	SymTableNodePtr idPtr = NULL;
	searchAndEnterThisTable (idPtr, SymTableDisplay[0]);

	if (numElements == 1) {
		idPtr->library = NULL;
		idPtr->defn.key = DFN_VAR;
		idPtr->labelIndex = 0;
		idPtr->typePtr = setType(IntegerTypePtr);
		idPtr->defn.info.data.varType = VAR_TYPE_REGISTERED;
		idPtr->defn.info.data.registeredData = address;
	}
	return(ABL_NO_ERR);
}

//***************************************************************************

int32_t ABLi_registerReal (PSTR name, float* address, int32_t /* numElements */) {

	if (strlen(name) >= MAXLEN_TOKENSTRING)
		ABL_Fatal(0, " ABLi_registerInteger: variable name too int32_t ");

	level = 0;
	strcpy(wordString, name);
	SymTableNodePtr idPtr = NULL;
	searchAndEnterThisTable (idPtr, SymTableDisplay[0]);

	idPtr->library = NULL;
	idPtr->defn.key = DFN_VAR;
	idPtr->labelIndex = 0;
	idPtr->typePtr = setType(RealTypePtr);
	idPtr->defn.info.data.varType = VAR_TYPE_REGISTERED;
	idPtr->defn.info.data.registeredData = address;
	return(ABL_NO_ERR);
}

//***************************************************************************

bool ABLi_getSkipOrder (void) {

	return(SkipOrder);
}

//***************************************************************************

void ABLi_resetOrders (void) {

	CurModule->resetOrderCallFlags();
}

//***************************************************************************

int32_t ABLi_getCurrentState (void) {

	if (CurFSM)
		return(CurFSM->getStateHandle());
	return(NULL);
}

//***************************************************************************

void ABLi_transState (int32_t newStateHandle) {

	if (CurFSM && (newStateHandle > 0) && (newStateHandle < ModuleRegistry[CurFSM->getHandle()].numStateHandles))
		transState(ModuleRegistry[CurFSM->getHandle()].stateHandles[newStateHandle].state);
}

//***************************************************************************
// MODULE routines
//***************************************************************************

SymTableNodePtr moduleHeader (void) {

	SymTableNodePtr moduleIdPtr = NULL;
	bool isFSM = false;
	if (CurLibrary)
		ifTokenGetElseError(TKN_LIBRARY, ABL_ERR_SYNTAX_MISSING_LIBRARY);
	else if (curToken == TKN_FSM) {
		isFSM = true;
		getToken();
		}
	else
		ifTokenGetElseError(TKN_MODULE, ABL_ERR_SYNTAX_MISSING_MODULE);

	//------------------------------------------------------------
	// NOTE: If a module is a library, its library pointer will be
	// non-NULL.
	
	if (curToken == TKN_IDENTIFIER) {
		searchAndEnterLocalSymTable(moduleIdPtr);
		moduleIdPtr->defn.key = DFN_MODULE;
		moduleIdPtr->defn.info.routine.key = RTN_DECLARED;
		moduleIdPtr->defn.info.routine.flags = (isFSM ? ROUTINE_FLAG_FSM : 0);
		moduleIdPtr->defn.info.routine.orderCallIndex = 0;
		moduleIdPtr->defn.info.routine.numOrderCalls = 0;
		moduleIdPtr->defn.info.routine.paramCount = 0;
		moduleIdPtr->defn.info.routine.totalParamSize = 0;
		moduleIdPtr->defn.info.routine.totalLocalSize = 0;
		moduleIdPtr->defn.info.routine.params = NULL;
		moduleIdPtr->defn.info.routine.locals = NULL;
		moduleIdPtr->defn.info.routine.localSymTable = NULL;
		moduleIdPtr->defn.info.routine.codeSegment = NULL;
		moduleIdPtr->library = CurLibrary;
		moduleIdPtr->typePtr = &DummyType;
		moduleIdPtr->labelIndex = 0;
		getToken();
		}
	else
		syntaxError(ABL_ERR_SYNTAX_MISSING_IDENTIFIER);

	//--------------------------------------------
	// For now, modules do not have param lists...
	synchronize(followModuleIdList, declarationStartList, statementStartList);
	enterScope(NULL);

	if (curToken == TKN_LPAREN) {
		int32_t paramCount;
		int32_t totalParamSize;
		SymTableNodePtr paramListPtr = formalParamList(&paramCount, &totalParamSize);
		//if (forwardFlag)
		//	syntaxError(ABL_ERR_SYNTAX_ALREADY_FORWARDED);
		//else {
			moduleIdPtr->defn.info.routine.paramCount = (uint8_t)paramCount;
			moduleIdPtr->defn.info.routine.totalParamSize = (uint8_t)totalParamSize;
			moduleIdPtr->defn.info.routine.params = paramListPtr;
		//}
	}
	//else if (!forwardFlag) {
	//	functionIdPtr->defn.info.routine.paramCount = 0;
	//	functionIdPtr->defn.info.routine.totalParamSize = 0;
	//	functionIdPtr->defn.info.routine.params = NULL;
	//}

	//-----------------------------
	// Now, check if return type...
	moduleIdPtr->typePtr = NULL;
	SymTableNodePtr typeIdPtr = NULL;
	if (curToken == TKN_COLON) {
		getToken();
		if (curToken == TKN_IDENTIFIER) {
			searchAndFindAllSymTables(typeIdPtr);
			
			if (typeIdPtr->defn.key != DFN_TYPE)
				syntaxError(ABL_ERR_SYNTAX_INVALID_TYPE);

			//if (!forwardFlag)
				moduleIdPtr->typePtr = typeIdPtr->typePtr;

			getToken();
			}
		else {
			syntaxError(ABL_ERR_SYNTAX_MISSING_IDENTIFIER);
			moduleIdPtr->typePtr = &DummyType;
		}

		//if (forwardFlag)
		//	syntaxError(ABL_ERR_SYNTAX_ALREADY_FORWARDED);
	}

	return(moduleIdPtr);
}

//***************************************************************************
// ROUTINE/FUNCTION routines
//***************************************************************************

void routine (void) {

	SymTableNodePtr routineIdPtr = NULL;
	
	//------------------------------------------------------------------------
	// Do we want to have "procedures", or do functions handle both cases (with
	// and without return values)? For now, functions handle both...
	routineIdPtr = functionHeader();

	//------------------------------------------------------------------
	// We need to save a pointer to the current routine we're parsing...
	SymTableNodePtr prevRoutineIdPtr = CurRoutineIdPtr;
	CurRoutineIdPtr = routineIdPtr;

	//---------------------
	// Error synchronize...
	synchronize(followHeaderList, declarationStartList, statementStartList);

	if (curToken == TKN_SEMICOLON)
		getToken();
	else if (tokenIn(declarationStartList) || tokenIn(statementStartList))
		syntaxError(ABL_ERR_SYNTAX_MISSING_SEMICOLON);

	if (strcmp(wordString, "forward") != 0) {
		routineIdPtr->defn.info.routine.key = RTN_DECLARED;
		//analyzeRoutineHeader(routineIdPtr);

		routineIdPtr->defn.info.routine.locals = NULL;

		declarations(routineIdPtr, false);
		synchronize(followRoutineDeclsList, NULL, NULL);
		if (curToken != TKN_CODE)
			syntaxError(ABL_ERR_SYNTAX_MISSING_CODE);
		crunchToken();

		blockType = BLOCK_ROUTINE;
		blockFlag = true;

		routineIdPtr->defn.info.routine.orderCallIndex = (uint16_t)NumOrderCalls;

		//compoundStatement();
		getToken();

		TokenCodeType endToken = TKN_END_FUNCTION;
		if (routineIdPtr->defn.info.routine.flags & ROUTINE_FLAG_STATE)
			endToken = TKN_END_STATE;
		else if (routineIdPtr->defn.info.routine.flags & ROUTINE_FLAG_ORDER)
			endToken = TKN_END_ORDER;
		if (curToken != endToken)
			do {
				statement();
				while (curToken == TKN_SEMICOLON)
					getToken();
				if (curToken == endToken)
					break;
		
				//------------------------------------
				// Synchronize after possible error...
				synchronize(statementStartList, NULL, NULL);
			} while (tokenIn(statementStartList));
		ifTokenGetElseError(endToken, ABL_ERR_SYNTAX_MISSING_END_FUNCTION);
		blockFlag = false;


		routineIdPtr->defn.info.routine.numOrderCalls = (uint16_t)NumOrderCalls - routineIdPtr->defn.info.routine.orderCallIndex;
		routineIdPtr->defn.info.routine.codeSegment = createCodeSegment(routineIdPtr->defn.info.routine.codeSegmentSize);
		analyzeBlock(routineIdPtr->defn.info.routine.codeSegment);
		}
	else {
		getToken();
		routineIdPtr->defn.info.routine.key = RTN_FORWARD;
		analyzeRoutineHeader(routineIdPtr);
	}

	if (routineIdPtr->defn.info.routine.flags & ROUTINE_FLAG_STATE) {
		//-----------------------------------
		// Add it to the state handle list...
		if (NumStateHandles == MAX_STATE_HANDLES_PER_MODULE)
			ABL_Fatal(0, "ABL: too many states in fsm [19 max]");
		strcpy(StateHandleList[NumStateHandles].name, routineIdPtr->name);
		StateHandleList[NumStateHandles].state = routineIdPtr;
		NumStateHandles++;
	}

	routineIdPtr->defn.info.routine.localSymTable = exitScope();

	CurRoutineIdPtr = prevRoutineIdPtr;
}

//***************************************************************************

SymTableNodePtr forwardState (PSTR stateName) {

	SymTableNodePtr stateSymbol = searchSymTableForState(stateName, SymTableDisplay[1]);
	if (stateSymbol)
		ABL_Fatal(0, " ABL.forwardState: State symbol should not exist ");
	stateSymbol = enterSymTable(stateName, &SymTableDisplay[1]);
	stateSymbol->defn.key = DFN_FUNCTION;
	stateSymbol->defn.info.routine.key = RTN_FORWARD;
	stateSymbol->defn.info.routine.paramCount = 0;
	stateSymbol->defn.info.routine.flags = ROUTINE_FLAG_STATE;
	stateSymbol->defn.info.routine.orderCallIndex = 0;
	stateSymbol->defn.info.routine.numOrderCalls = 0;
	stateSymbol->defn.info.routine.totalParamSize = 0;
	stateSymbol->defn.info.routine.totalLocalSize = 0;
	stateSymbol->defn.info.routine.params = NULL;
	stateSymbol->defn.info.routine.locals = NULL;
	stateSymbol->defn.info.routine.localSymTable = NULL;
	stateSymbol->defn.info.routine.codeSegment = NULL;
	stateSymbol->library = CurLibrary;
	stateSymbol->typePtr = NULL;
	stateSymbol->labelIndex = 0;
	//-------------------
	// Force the level...
	stateSymbol->level = 1;
	return(stateSymbol);
}

//***************************************************************************

SymTableNodePtr functionHeader (void) {

	bool isState = false;
	bool isOrder = false;
	if (curToken == TKN_STATE)
		isState = true;
	else if (curToken == TKN_ORDER)
		isOrder = true;
	getToken();
	SymTableNodePtr functionIdPtr = NULL;
	SymTableNodePtr typeIdPtr = NULL;

	bool forwardFlag = false;

	if (curToken == TKN_IDENTIFIER) {
		functionIdPtr = searchSymTableForFunction(wordString, SymTableDisplay[level]);
		if (!functionIdPtr) {
			enterLocalSymTable(functionIdPtr);
			functionIdPtr->defn.key = DFN_FUNCTION;
			functionIdPtr->defn.info.routine.key = RTN_DECLARED;
			functionIdPtr->defn.info.routine.paramCount = 0;
			functionIdPtr->defn.info.routine.flags = 0;
			functionIdPtr->defn.info.routine.orderCallIndex = 0;
			functionIdPtr->defn.info.routine.numOrderCalls = 0;
			functionIdPtr->defn.info.routine.totalParamSize = 0;
			functionIdPtr->defn.info.routine.totalLocalSize = 0;
			functionIdPtr->defn.info.routine.params = NULL;
			functionIdPtr->defn.info.routine.locals = NULL;
			functionIdPtr->defn.info.routine.localSymTable = NULL;
			functionIdPtr->defn.info.routine.codeSegment = NULL;
			if (isOrder)
				functionIdPtr->defn.info.routine.flags |= ROUTINE_FLAG_ORDER;
			if (isState)
				functionIdPtr->defn.info.routine.flags |= ROUTINE_FLAG_STATE;
			functionIdPtr->library = CurLibrary;
			functionIdPtr->typePtr = &DummyType;
			functionIdPtr->labelIndex = 0;
			}
		else if ((functionIdPtr->defn.key == DFN_FUNCTION) &&
				 (functionIdPtr->defn.info.routine.key == RTN_FORWARD))
			forwardFlag = true;
		else
			syntaxError(ABL_ERR_SYNTAX_REDEFINED_IDENTIFIER);
		getToken();
	}

	//---------------------
	// Error synchronize...
	synchronize(followFunctionIdList, declarationStartList, statementStartList);

	enterScope(NULL);

	if (curToken == TKN_LPAREN) {
		int32_t paramCount;
		int32_t totalParamSize;
		SymTableNodePtr paramListPtr = formalParamList(&paramCount, &totalParamSize);
		if (forwardFlag)
			syntaxError(ABL_ERR_SYNTAX_ALREADY_FORWARDED);
		else {
			functionIdPtr->defn.info.routine.paramCount = (uint8_t)paramCount;
			functionIdPtr->defn.info.routine.totalParamSize = (uint8_t)totalParamSize;
			functionIdPtr->defn.info.routine.params = paramListPtr;
		}
		}
	else if (!forwardFlag) {
		functionIdPtr->defn.info.routine.paramCount = 0;
		functionIdPtr->defn.info.routine.totalParamSize = 0;
		functionIdPtr->defn.info.routine.params = NULL;
	}

	//--------------------------------------------
	// Now, check if return type and/or forward...
	functionIdPtr->typePtr = NULL;
	bool hasIntegerReturnValue = false;
	if (curToken == TKN_COLON) {
		getToken();
		if (curToken == TKN_IDENTIFIER) {
			searchAndFindAllSymTables(typeIdPtr);
			
			if (typeIdPtr->defn.key != DFN_TYPE)
				syntaxError(ABL_ERR_SYNTAX_INVALID_TYPE);

			hasIntegerReturnValue = (typeIdPtr->typePtr == IntegerTypePtr);

			if (!forwardFlag)
				functionIdPtr->typePtr = typeIdPtr->typePtr;

			getToken();
			}
		else {
			syntaxError(ABL_ERR_SYNTAX_MISSING_IDENTIFIER);
			functionIdPtr->typePtr = &DummyType;
		}

		if (forwardFlag)
			syntaxError(ABL_ERR_SYNTAX_ALREADY_FORWARDED);
	}

	if (isOrder && !hasIntegerReturnValue)
		syntaxError(ABL_ERR_SYNTAX_ORDER_RETURNTYPE);

	return(functionIdPtr);
}

//***************************************************************************

SymTableNodePtr formalParamList (int32_t* count, int32_t* totalSize) {

	SymTableNodePtr paramIdPtr;
	SymTableNodePtr firstIdPtr;
	SymTableNodePtr lastIdPtr = NULL;
	SymTableNodePtr paramListPtr = NULL;
	SymTableNodePtr typeIdPtr;
	TypePtr paramTypePtr;
	DefinitionType paramDefn;
	int32_t paramCount = 0;
	int32_t paramOffset = STACK_FRAME_HEADER_SIZE;

	getToken();

	while ((curToken == TKN_IDENTIFIER) || (curToken == TKN_REF)) {
		firstIdPtr = NULL;

		if (curToken == TKN_REF) {
			paramDefn = DFN_REFPARAM;
			getToken();
			}
		else
			paramDefn = DFN_VALPARAM;

		//-----------------------------------
		// Grab the type of this parameter...
		if (curToken == TKN_IDENTIFIER) {
			searchAndFindAllSymTables(typeIdPtr);
			if (typeIdPtr->defn.key != DFN_TYPE)
				syntaxError(ABL_ERR_SYNTAX_INVALID_TYPE);
			paramTypePtr = (TypePtr)(typeIdPtr->typePtr);
			getToken();
			}
		else {
			syntaxError(ABL_ERR_SYNTAX_MISSING_IDENTIFIER);
			paramTypePtr = &DummyType;
		}

		//------------------------------------------------------------
		// Is this an array parameter? If so, parse the dimensions and
		// whether it's an open array...
		if (curToken == TKN_LBRACKET) {
		}
		
		//-----------------------------------------------
		// Now grab the variable id for this parameter...
		if (curToken == TKN_IDENTIFIER) {
			searchAndEnterLocalSymTable(paramIdPtr);
			paramIdPtr->defn.key = paramDefn;
			paramIdPtr->labelIndex = 0;

			paramIdPtr->typePtr = paramTypePtr;
			paramIdPtr->defn.info.data.offset = paramOffset++;
			
			paramCount++;

			if (paramListPtr == NULL)
				paramListPtr = paramIdPtr;
			if (lastIdPtr != NULL)
				lastIdPtr->next = paramIdPtr;
			lastIdPtr = paramIdPtr;
			//if (firstIdPtr == NULL)
			//	firstIdPtr = lastIdPtr = paramIdPtr;
			//else {
			//	lastIdPtr->next = paramIdPtr;
			//	lastIdPtr = paramIdPtr;
			//}

			getToken();
			//ifTokenGet(TKN_COMMA);
			}
		else {
			syntaxError(ABL_ERR_SYNTAX_MISSING_IDENTIFIER);
			//--------------------------------
			// Do we need to synchronize here?
		}


		//------------------------------------------------------------------
		// Assign the offset and the type to all param ids in the sublist...
		//for (paramIdPtr = firstIdPtr; paramIdPtr != NULL; paramIdPtr = paramIdPtr->next) {
		//	paramIdPtr->typePtr = paramTypePtr;
		//	paramIdPtr->defn.info.data.offset = paramOffset++;
		//}

		//-----------------------------------------------
		// Link this list to the list of all param ids...
		//if (prevLastIdPtr != NULL)
		//	prevLastIdPtr->next = firstIdPtr;
		//prevLastIdPtr = lastIdPtr;

		//-------------------------------
		// Error synch... should be ; or )
		synchronize(followParamsList, NULL, NULL);
		ifTokenGet(TKN_COMMA);
	}

	ifTokenGetElseError(TKN_RPAREN, ABL_ERR_SYNTAX_MISSING_RPAREN);
	*count = paramCount;
	*totalSize = paramOffset - STACK_FRAME_HEADER_SIZE;

	if (paramCount > 255)
		syntaxError(ABL_ERR_SYNTAX_TOO_MANY_FORMAL_PARAMETERS);

	return(paramListPtr);
}

//***************************************************************************

TypePtr routineCall (SymTableNodePtr routineIdPtr, int32_t paramCheckFlag) {

	SymTableNodePtr thisRoutineIdPtr = CurRoutineIdPtr;
	TypePtr resultType = NULL;

	if ((routineIdPtr->defn.info.routine.key == RTN_DECLARED) ||
		(routineIdPtr->defn.info.routine.key == RTN_FORWARD) ||
		!paramCheckFlag)
		resultType = declaredRoutineCall(routineIdPtr, paramCheckFlag);
	else
		resultType = standardRoutineCall(routineIdPtr);

	CurRoutineIdPtr = thisRoutineIdPtr;
	return(resultType);
}

//***************************************************************************

TypePtr declaredRoutineCall (SymTableNodePtr routineIdPtr, int32_t paramCheckFlag) {

	actualParamList(routineIdPtr, paramCheckFlag);
	return(/*routineIdPtr->defn.key == DFN_PROCEDURE ? NULL :*/ (TypePtr)(routineIdPtr->typePtr));
}

//***************************************************************************

void actualParamList (SymTableNodePtr routineIdPtr, int32_t paramCheckFlag) {

	SymTableNodePtr formalParamIdPtr = NULL;
	DefinitionType formalParamDefn = (DefinitionType)0;
	TypePtr formalParamTypePtr = NULL;
	TypePtr actualParamTypePtr = NULL;

	if (paramCheckFlag)
		formalParamIdPtr = (SymTableNodePtr)(routineIdPtr->defn.info.routine.params);

	if (curToken == TKN_LPAREN) {
		//-----------------------------------------------
		// Loop to process actual parameter expression...
		do {
			//----------------------------------------------------
			// Obtain info about the corresponding formal param...
			if (paramCheckFlag && (formalParamIdPtr != NULL)) {
				formalParamDefn = formalParamIdPtr->defn.key;
				formalParamTypePtr = (TypePtr)(formalParamIdPtr->typePtr);
			}

			getToken();

			if ((formalParamIdPtr == NULL) || (formalParamDefn == DFN_VALPARAM) || !paramCheckFlag) {
				//--------------
				// VAL Params...
				actualParamTypePtr = expression();
				if (paramCheckFlag && (formalParamIdPtr != NULL) && !isAssignTypeCompatible(formalParamTypePtr, actualParamTypePtr))
					syntaxError(ABL_ERR_SYNTAX_INCOMPATIBLE_TYPES);
				}
			else {
				//--------------
				// VAR params...
				if (curToken == TKN_IDENTIFIER) {
					SymTableNodePtr idPtr;
					searchAndFindAllSymTables(idPtr);
					actualParamTypePtr = variable(idPtr);

					if (formalParamTypePtr != actualParamTypePtr)
						syntaxError(ABL_ERR_SYNTAX_INCOMPATIBLE_TYPES);
					}
				else {
					//------------------------------------------------------------------
					// Not a variable--parse the expression anyway for error recovery...
					actualParamTypePtr = expression();
					syntaxError(ABL_ERR_SYNTAX_INVALID_REF_PARAM);
				}
			}

			//---------------------------------------
			// More actual params than formal params?
			if (paramCheckFlag) {
				if (formalParamIdPtr == NULL)
					syntaxError(ABL_ERR_SYNTAX_WRONG_NUMBER_OF_PARAMS);
				else
					formalParamIdPtr = formalParamIdPtr->next;
			}

			//---------------
			// Error synch...
			synchronize(followParamList, statementEndList, NULL);
		} while (curToken == TKN_COMMA);

		ifTokenGetElseError(TKN_RPAREN, ABL_ERR_SYNTAX_MISSING_RPAREN);
	}

	//----------------------------------------
	// Fewer actual params than formal params?
	if (paramCheckFlag && (formalParamIdPtr != NULL))
		syntaxError(ABL_ERR_SYNTAX_WRONG_NUMBER_OF_PARAMS);
}

//***************************************************************************

extern char	SetStateDebugStr[256];

void transState (SymTableNodePtr newState) {

	if (CurFSM) {
		CurFSM->setPrevState(CurFSM->getState());
		CurFSM->setState(newState);
		sprintf(SetStateDebugStr, "%s:%s, line %d", CurModule->getFileName(), newState->name, execLineNumber);
		NewStateSet = true;
	}
}

//***************************************************************************

