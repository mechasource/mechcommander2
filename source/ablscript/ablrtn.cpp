//===========================================================================//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//
//***************************************************************************
//
//								ABLRTN.CPP
//
//***************************************************************************
#include "stdinc.h"

//#include "ablgen.h"
//#include "ablerr.h"
//#include "ablscan.h"
//#include "ablsymt.h"
//#include "ablparse.h"
//#include "ablexec.h"
//#include "ablenv.h"
//#include "abl.h"
//#include "abldbug.h"

namespace mclib::abl
{

//***************************************************************************

extern int32_t MaxBreaks;
extern int32_t MaxWatches;
extern const std::unique_ptr<Debugger>& debugger;
extern int32_t NumModules;
extern const std::unique_ptr<ModuleEntry>& ModuleRegistry;
extern const std::unique_ptr<ABLModule>&* ModuleInstanceRegistry;
extern int32_t MaxModules;
extern const std::unique_ptr<ABLModule>&* LibraryInstanceRegistry;
extern int32_t MaxLibraries;
extern int32_t NumModulesRegistered;
extern int32_t NumModuleInstances;
extern int32_t MaxWatchesPerModule;
extern int32_t MaxBreakPointsPerModule;
extern int32_t MaxCodeBufferSize;
extern const std::unique_ptr<ABLModule>& CurModule;
extern const std::unique_ptr<ABLModule>& CurLibrary;
extern std::wstring_view codeBuffer;
extern std::wstring_view codeBufferPtr;
extern std::wstring_view codeSegmentPtr;
extern std::wstring_view codeSegmentLimit;
extern std::wstring_view statementStartPtr;
extern StackItem* stack;
extern const std::unique_ptr<StackItem>& tos;
extern const std::unique_ptr<StackItem>& stackFrameBasePtr;
extern const std::unique_ptr<StackItem>& StaticDataPtr;
extern int32_t* StaticVariablesSizes;
extern int32_t* EternalVariablesSizes;
extern int32_t MaxEternalVariables;
extern int32_t eternalOffset;
extern int32_t MaxStaticVariables;
extern int32_t NumStaticVariables;
extern int32_t NumOrderCalls;
extern StateHandleInfo StateHandleList[MAX_STATE_HANDLES_PER_MODULE];
extern int32_t NumStateHandles;
extern int32_t CurModuleHandle;
extern bool CallModuleInit;
extern bool AutoReturnFromOrders;
extern int32_t MaxLoopIterations;
extern bool AssertEnabled;
extern bool IncludeDebugInfo;
extern bool ProfileABL;
extern bool Crunch;
extern int32_t level;
extern int32_t lineNumber;
extern int32_t FileNumber;
extern ABLFile* sourceFile;
extern bool printFlag;
extern bool blockFlag;
extern BlockType blockType;
extern const std::unique_ptr<SymTableNode>& CurModuleIdPtr;
extern const std::unique_ptr<SymTableNode>& CurRoutineIdPtr;
extern bool DumbGetCharOn;
extern int32_t NumOpenFiles;
extern int32_t NumSourceFiles;
extern int32_t NumLibrariesUsed;
extern const std::unique_ptr<ABLModule>& LibrariesUsed[MAX_LIBRARIES_USED];
extern int32_t bufferOffset;
extern std::wstring_view bufferp;
extern std::wstring_view tokenp;
extern int32_t digitCount;
extern bool countError;
extern int32_t pageNumber;
extern int32_t lineCount;

extern int32_t CurAlarm;

extern bool eofFlag;
extern bool ExitWithReturn;
extern bool ExitFromTacOrder;

extern int32_t dummyCount;

extern int32_t lineNumber;
extern int32_t FileNumber;
extern int32_t errorCount;
extern int32_t execStatementCount;
extern int32_t NumSourceFiles;
extern wchar_t SourceFiles[MAX_SOURCE_FILES][MAXLEN_FILENAME];

extern TokenCodeType curToken;
extern wchar_t wordString[];
extern const std::unique_ptr<SymTableNode>& symTableDisplay[];
extern int32_t level;
extern bool blockFlag;
extern BlockType blockType;
extern bool printFlag;
extern const std::unique_ptr<SymTableNode>& CurRoutineIdPtr;

extern Type DummyType;
extern StackItem* stack;
// extern StackItem*		eternalStack;
extern const std::unique_ptr<StackItem>& tos;
extern const std::unique_ptr<StackItem>& stackFrameBasePtr;
extern int32_t eternalOffset;

extern TokenCodeType statementStartList[];
extern TokenCodeType statementEndList[];
extern TokenCodeType declarationStartList[];

extern wchar_t tokenString[MAXLEN_TOKENSTRING];

extern CharCodeType charTable[256];

extern wchar_t sourceBuffer[MAXLEN_SOURCELINE];
extern int32_t bufferOffset;
extern std::wstring_view bufferp;
extern std::wstring_view tokenp;

extern int32_t digitCount;
extern bool countError;

extern bool eofFlag;
extern int32_t pageNumber;

extern const std::unique_ptr<SymTableNode>& SymTableDisplay[MAX_NESTING_LEVEL];
extern const std::unique_ptr<Type>& IntegerTypePtr;
extern const std::unique_ptr<Type>& RealTypePtr;
extern const std::unique_ptr<Type>& BooleanTypePtr;

extern const std::unique_ptr<StackItem>& StaticDataPtr;
extern StackItem returnValue;

extern const std::unique_ptr<ModuleEntry>& ModuleRegistry;
extern int32_t MaxModules;
extern int32_t NumModulesRegistered;
extern int32_t MaxStaticVariables;
extern int32_t NumStaticVariables;
extern int32_t NumSourceFiles;

extern bool IncludeDebugInfo;
extern bool AssertEnabled;
extern bool PrintEnabled;
extern bool StringFunctionsEnabled;
extern bool DebugCodeEnabled;
extern const std::unique_ptr<Debugger>& debugger;

bool ABLenabled = false;
wchar_t buffer[MAXLEN_PRINTLINE];

extern int32_t CallStackLevel;
extern bool SkipOrder;

extern const std::unique_ptr<ABLModule>& CurFSM;
extern bool NewStateSet;

extern void
transState(const std::unique_ptr<SymTableNode>& newState);

int32_t numLibrariesLoaded = 0;
int32_t NumExecutions = 0;

PVOID(*ABLSystemMallocCallback)
(uint32_t memSize) = nullptr;
PVOID(*ABLStackMallocCallback)
(uint32_t memSize) = nullptr;
PVOID(*ABLCodeMallocCallback)
(uint32_t memSize) = nullptr;
PVOID(*ABLSymbolMallocCallback)
(uint32_t memSize) = nullptr;
void (*ABLSystemFreeCallback)(PVOID memBlock) = nullptr;
void (*ABLStackFreeCallback)(PVOID memBlock) = nullptr;
void (*ABLCodeFreeCallback)(PVOID memBlock) = nullptr;
void (*ABLSymbolFreeCallback)(PVOID memBlock) = nullptr;
void (*ABLDebugPrintCallback)(std::wstring_view s) = nullptr;
int32_t (*ABLRandomCallback)(int32_t range) = nullptr;
void (*ABLSeedRandomCallback)(uint32_t range) = nullptr;
uint32_t (*ABLGetTimeCallback)(void) = nullptr;
void (*ABLFatalCallback)(int32_t code, std::wstring_view s) = nullptr;
void (*ABLEndlessStateCallback)(UserFile* log) = nullptr;

//***************************************************************************

TokenCodeType followHeaderList[] = {TKN_SEMICOLON, TKN_EOF, TKN_NONE};

TokenCodeType followModuleIdList[] = {TKN_LPAREN, TKN_COLON, TKN_SEMICOLON, TKN_EOF, TKN_NONE};

TokenCodeType followFunctionIdList[] = {TKN_LPAREN, TKN_COLON, TKN_SEMICOLON, TKN_EOF, TKN_NONE};

TokenCodeType followParamsList[] = {TKN_RPAREN, TKN_COMMA, TKN_EOF, TKN_NONE};

TokenCodeType followParamList[] = {TKN_COMMA, TKN_RPAREN, TKN_NONE};

TokenCodeType followModuleDeclsList[] = {TKN_SEMICOLON, TKN_CODE, TKN_EOF, TKN_NONE};

TokenCodeType followRoutineDeclsList[] = {TKN_SEMICOLON, TKN_CODE, TKN_EOF, TKN_NONE};

TokenCodeType followDeclsList[] = {TKN_SEMICOLON, TKN_EOF, TKN_NONE};

void ABL_CloseProfileLog();
void ABL_OpenProfileLog();
void ABL_AddToProfileLog(std::wstring_view profileString);

//***************************************************************************
// ABL library interface routines
//***************************************************************************

int32_t DefaultRandom(int32_t /* range */)
{
	return (0);
}

//---------------------------------------------------------------------------

void DefaultSeedRandom(uint32_t /* seed */) { }

//---------------------------------------------------------------------------

void DefaultDebugPrintCallback(std::wstring_view /* s */) { }

//---------------------------------------------------------------------------

uint32_t
DefaultGetTimeCallback(void)
{
	return (0);
}

//---------------------------------------------------------------------------

void ABLi_setRandomCallbacks(
	void (*seedRandomCallback)(uint32_t seed), int32_t (*randomCallback)(int32_t range))
{
	ABLSeedRandomCallback = seedRandomCallback;
	ABLRandomCallback = randomCallback;
}

//---------------------------------------------------------------------------

void ABLi_setDebugPrintCallback(void (*debugPrintCallback)(std::wstring_view s))
{
	ABLDebugPrintCallback = debugPrintCallback;
}

//---------------------------------------------------------------------------

void ABLi_setGetTimeCallback(uint32_t (*getTimeCallback)(void))
{
	ABLGetTimeCallback = getTimeCallback;
}

//---------------------------------------------------------------------------

void ABLi_setEndlessStateCallback(void (*endlessStateCallback)(UserFile* log))
{
	ABLEndlessStateCallback = endlessStateCallback;
}

//---------------------------------------------------------------------------

void ABLi_init(uint32_t runtimeStackSize, uint32_t maxCodeBufferSize, uint32_t maxRegisteredModules,
	uint32_t maxStaticVariables, PVOID (*systemMallocCallback)(uint32_t memSize),
	PVOID (*stackMallocCallback)(uint32_t memSize), PVOID (*codeMallocCallback)(uint32_t memSize),
	PVOID (*symbolMallocCallback)(uint32_t memSize), void (*systemFreeCallback)(PVOID memBlock),
	void (*stackFreeCallback)(PVOID memBlock), void (*codeFreeCallback)(PVOID memBlock),
	void (*symbolFreeCallback)(PVOID memBlock), int32_t (*fileCreateCB)(PVOID* file, std::wstring_view filename),
	int32_t (*fileOpenCB)(PVOID* file, std::wstring_view filename), int32_t (*fileCloseCB)(PVOID* file),
	bool (*fileEofCB)(PVOID file),
	int32_t (*fileReadCB)(PVOID file, uint8_t* buffer, int32_t length),
	int32_t (*fileReadLongCB)(PVOID file), int32_t (*fileReadStringCB)(PVOID file, uint8_t* buffer),
	int32_t (*fileReadLineExCB)(PVOID file, uint8_t* buffer, int32_t maxLength),
	int32_t (*fileWriteCB)(PVOID file, uint8_t* buffer, int32_t length),
	int32_t (*fileWriteByteCB)(PVOID file, uint8_t byte),
	int32_t (*fileWriteLongCB)(PVOID file, int32_t value),
	int32_t (*fileWriteStringCB)(PVOID file, std::wstring_view buffer), void (*debuggerPrintCallback)(std::wstring_view s),
	void (*fatalCallback)(int32_t code, std::wstring_view s), bool debugInfo, bool debug, bool profile)
{
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
	debugger = nullptr;
	NumModules = 0;
	ModuleRegistry = nullptr;
	ModuleInstanceRegistry = nullptr;
	LibraryInstanceRegistry = nullptr;
	MaxModules = 0;
	MaxLibraries = 0;
	NumModulesRegistered = 0;
	NumModuleInstances = 0;
	MaxWatchesPerModule = 20;
	MaxBreakPointsPerModule = 20;
	CurModule = nullptr;
	errorCount = 0;
	codeBuffer = nullptr;
	codeBufferPtr = nullptr;
	codeSegmentPtr = nullptr;
	codeSegmentLimit = nullptr;
	statementStartPtr = nullptr;
	execStatementCount = 0;
	stack = nullptr;
	tos = nullptr;
	stackFrameBasePtr = nullptr;
	StaticDataPtr = nullptr;
	StaticVariablesSizes = nullptr;
	EternalVariablesSizes = nullptr;
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
	sourceFile = nullptr;
	printFlag = true;
	blockFlag = false;
	blockType = BLOCK_MODULE;
	CurModuleIdPtr = nullptr;
	CurRoutineIdPtr = nullptr;
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
	codeBuffer = (std::wstring_view)ABLCodeMallocCallback(maxCodeBufferSize);
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
	StaticDataPtr = nullptr;
	StaticVariablesSizes = nullptr;
	if (MaxStaticVariables > 0)
	{
		StaticVariablesSizes =
			(int32_t*)ABLStackMallocCallback(sizeof(int32_t) * MaxStaticVariables);
		if (!StaticVariablesSizes)
			ABL_Fatal(0, " ABL: Unable to AblStackHeap->malloc StaticVariablesSizes ");
	}
	//------------------------------
	// Allocate the runtime stack...
	stack = (const std::unique_ptr<StackItem>&)ABLStackMallocCallback(
		sizeof(StackItem) * (runtimeStackSize / sizeof(StackItem)));
	if (!stack)
		ABL_Fatal(0, " ABL: Unable to AblStackHeap->malloc stack ");
	//-----------------------------------
	// Allocate Eternal Vars Size List...
	if (MaxEternalVariables > 0)
	{
		EternalVariablesSizes =
			(int32_t*)ABLStackMallocCallback(sizeof(int32_t) * MaxEternalVariables);
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
	if (debug)
	{
		IncludeDebugInfo = true;
		debugger = new Debugger;
		if (!debugger)
			ABL_Fatal(0, " Unable to initialize ABL Debugger. ");
		debugger->init(debuggerPrintCallback, nullptr);
	}
	UserFile::setup();
	if (ProfileABL)
		ABL_OpenProfileLog();
#ifdef TEST_ABLFILE
	ABLFile* aFile = new ABLFile;
	aFile->create("ablFile.txt");
	aFile->writeString("Testing writeString");
	aFile->writeByte(nullptr);
	aFile->writeLong(100);
	aFile->close();
	delete aFile;
	aFile = nullptr;
	uint8_t s[100];
	ABLFile* bFile = new ABLFile;
	bFile->open("ablFile.txt");
	bFile->readString(s);
	int32_t val = bFile->readLong();
	bFile->close();
	delete bFile;
	bFile = nullptr;
#endif
}

//***************************************************************************

int32_t
ABLi_preProcess(std::wstring_view sourceFileName, int32_t* numErrors, int32_t* numLinesProcessed,
	int32_t* numFilesProcessed, bool printLines)
{
	//--------------------------------------------------------------------------------
	// First, check if this module has already been registered into the
	// environment...
	int32_t i;
	for (i = 0; i < NumModulesRegistered; i++)
		if (strcmp(strlwr(sourceFileName), ModuleRegistry[i].fileName) == 0)
			return (i);
	//---------------------------------
	// Init some important variables...
	level = 0;
	lineNumber = 0;
	FileNumber = 0;
	AssertEnabled = (debugger != nullptr);
	PrintEnabled = (debugger != nullptr);
	StringFunctionsEnabled = true;
	DebugCodeEnabled = (debugger != nullptr);
	NumSourceFiles = 0;
	NumLibrariesUsed = 0;
	sourceFile = nullptr;
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
	for (i = 0; i < MAX_STATE_HANDLES_PER_MODULE; i++)
	{
		StateHandleList[i].name[0] = nullptr;
		StateHandleList[i].state = nullptr;
	}
	if (numErrors)
		*numErrors = 0;
	if (numLinesProcessed)
		*numLinesProcessed = 0;
	//---------------------------------------
	// Now, let's open the ABL source file...
	int32_t openErr = ABL_NO_ERR;
	if ((openErr = openSourceFile(sourceFileName)) != ABL_NO_ERR)
		return (openErr);
	//------------------------
	// Set up the code buffer.
	codeBufferPtr = codeBuffer;
	//------------------
	// Get it rolling...
	getToken();
	const std::unique_ptr<SymTableNode>& moduleIdPtr = moduleHeader();
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
	// block(moduleIdPtr);
	declarations(moduleIdPtr, true);
	if ((moduleIdPtr->defn.info.routine.flags & ROUTINE_FLAG_FSM) == 0)
	{
		synchronize(followModuleDeclsList, nullptr, nullptr);
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
			do
			{
				statement();
				while (curToken == TKN_SEMICOLON)
					getToken();
				if (curToken == endToken)
					break;
				//------------------------------------
				// Synchronize after possible error...
				synchronize(statementStartList, nullptr, nullptr);
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
	moduleIdPtr->defn.info.routine.codeSegment =
		createCodeSegment(moduleIdPtr->defn.info.routine.codeSegmentSize);
	analyzeBlock(moduleIdPtr->defn.info.routine.codeSegment);
	ifTokenGetElseError(TKN_PERIOD, ABL_ERR_SYNTAX_MISSING_PERIOD);
	while (curToken != TKN_EOF)
	{
		syntaxError(TKN_UNEXPECTED_TOKEN);
		getToken();
	}
	//------------------------------------------
	// Done with the source file, so close it...
	closeSourceFile();
	// extractSymTable(&SymTableDisplay[0], moduleIdPtr);
	//--------------------------------------------------
	// Register the new module in the ABL environment...
	ModuleRegistry[NumModulesRegistered].fileName =
		(std::wstring_view)ABLStackMallocCallback(strlen(sourceFileName) + 1);
	if (!ModuleRegistry[NumModulesRegistered].fileName)
		ABL_Fatal(0, " ABL: Unable to AblStackHeap->malloc module filename ");
	strcpy(ModuleRegistry[NumModulesRegistered].fileName, strlwr(sourceFileName));
	ModuleRegistry[NumModulesRegistered].moduleIdPtr = moduleIdPtr;
	ModuleRegistry[NumModulesRegistered].numSourceFiles = NumSourceFiles;
	ModuleRegistry[NumModulesRegistered].sourceFiles =
		(std::wstring_view*)ABLStackMallocCallback(NumSourceFiles * sizeof(std::wstring_view));
	if (!ModuleRegistry[NumModulesRegistered].sourceFiles)
		ABL_Fatal(0, " ABL: Unable to AblStackHeap->malloc sourceFiles ");
	for (i = 0; i < NumSourceFiles; i++)
	{
		ModuleRegistry[NumModulesRegistered].sourceFiles[i] =
			(std::wstring_view)ABLStackMallocCallback(strlen(SourceFiles[i]) + 1);
		strcpy(ModuleRegistry[NumModulesRegistered].sourceFiles[i], SourceFiles[i]);
	}
	if (NumLibrariesUsed > 0)
	{
		ModuleRegistry[NumModulesRegistered].numLibrariesUsed = NumLibrariesUsed;
		ModuleRegistry[NumModulesRegistered].librariesUsed =
			(const std::unique_ptr<ABLModule>&*)ABLStackMallocCallback(NumLibrariesUsed * sizeof(const std::unique_ptr<SymTableNode>&));
		if (!ModuleRegistry[NumModulesRegistered].librariesUsed)
			ABL_Fatal(0, " ABL: Unable to AblStackHeap->malloc librariesUsed ");
		for (i = 0; i < NumLibrariesUsed; i++)
			ModuleRegistry[NumModulesRegistered].librariesUsed[i] = LibrariesUsed[i];
	}
	ModuleRegistry[NumModulesRegistered].numStaticVars = NumStaticVariables;
	ModuleRegistry[NumModulesRegistered].sizeStaticVars = nullptr;
	ModuleRegistry[NumModulesRegistered].totalSizeStaticVars = 0;
	if (NumStaticVariables)
	{
		ModuleRegistry[NumModulesRegistered].sizeStaticVars =
			(int32_t*)ABLStackMallocCallback(sizeof(int32_t) * NumStaticVariables);
		if (!ModuleRegistry[NumModulesRegistered].sizeStaticVars)
			ABL_Fatal(0, " ABL: Unable to AblStackHeap->malloc module sizeStaticVars ");
		memcpy(ModuleRegistry[NumModulesRegistered].sizeStaticVars, StaticVariablesSizes,
			sizeof(int32_t) * NumStaticVariables);
		ModuleRegistry[NumModulesRegistered].totalSizeStaticVars =
			sizeof(int32_t) * NumStaticVariables;
		for (size_t i = 0; i < ModuleRegistry[NumModulesRegistered].numStaticVars; i++)
			ModuleRegistry[NumModulesRegistered].totalSizeStaticVars +=
				ModuleRegistry[NumModulesRegistered].sizeStaticVars[i];
	}
	ModuleRegistry[NumModulesRegistered].numOrderCalls = NumOrderCalls;
	ModuleRegistry[NumModulesRegistered].numInstances = 0;
	ModuleRegistry[NumModulesRegistered].numStateHandles = NumStateHandles;
	if (NumStateHandles > 1)
	{
		ModuleRegistry[NumModulesRegistered].stateHandles =
			(const std::unique_ptr<StateHandleInfo>&)ABLStackMallocCallback(sizeof(StateHandleInfo) * NumStateHandles);
		memcpy(ModuleRegistry[NumModulesRegistered].stateHandles, StateHandleList,
			sizeof(StateHandleInfo) * NumStateHandles);
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
	return (NumModulesRegistered - 1);
}

//***************************************************************************

int32_t
ABLi_execute(const std::unique_ptr<SymTableNode>& moduleIdPtr, const std::unique_ptr<SymTableNode>& /* functionIdPtr */,
	const std::unique_ptr<ABLParam>& paramList, const std::unique_ptr<StackItem>& returnVal)
{
	// insertSymTable(&SymTableDisplay[0], moduleIdPtr);
	//--------------------------
	// Execute the ABL module...
	//----------------------------------------------------------------------------------
	// NOTE: Ultimately, we want the ABL virtual machine to be set up once, and
	// then ABL modules/routines can be called multiple times from within a
	// program. For now, to test, let's just assume every time we run a module,
	// it is a self-contained ABL program.
	//---------------------------------
	// Init some important variables...
	// lineNumber = 0;
	// printFlag = true;
	// blockFlag = false;
	// blockType = BLOCK_MODULE;
	CurModuleIdPtr = nullptr;
	CurRoutineIdPtr = nullptr;
	// bufferOffset = 0;
	// bufferp = sourceBuffer;
	// tokenp = tokenString;
	// digitCount = 0;
	// countError = false;
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
	pushAddress(nullptr);
	//----------------
	// Dynamic Link...
	pushAddress(nullptr);
	//------------------
	// Return Address...
	pushAddress(nullptr);
	// initDebugger();
	//----------
	// Run it...
	if (paramList)
	{
		//------------------------------------------------------------------------------
		// NOTE: Currently, parameter passing of arrays is not functioning. This
		// MUST be done...
		int32_t curParam = 0;
		for (const std::unique_ptr<SymTableNode>& formalIdPtr = (const std::unique_ptr<SymTableNode>&)(moduleIdPtr->defn.info.routine.params);
			 formalIdPtr != nullptr; formalIdPtr = formalIdPtr->next)
		{
			const std::unique_ptr<Type>& formalTypePtr = (const std::unique_ptr<Type>&)(formalIdPtr->ptype);
			if (formalIdPtr->defn.key == DFN_VALPARAM)
			{
				if (formalTypePtr == RealTypePtr)
				{
					if (paramList[curParam].type == ABL_PARAM_INTEGER)
					{
						//---------------------------------------------
						// Real formal parameter, but integer actual...
						pushReal((float)(paramList[curParam].integer));
					}
					else if (paramList[curParam].type == ABL_PARAM_REAL)
						pushReal(paramList[curParam].real);
				}
				else if (formalTypePtr == IntegerTypePtr)
				{
					if (paramList[curParam].type == ABL_PARAM_INTEGER)
						pushInteger(paramList[curParam].integer);
					else
						return (0);
				}
				//----------------------------------------------------------
				// Formal parameter is an array or record, so make a copy...
				if ((formalTypePtr->form == FRM_ARRAY) /* || (formalTypePtr->form == FRM_RECORD)*/)
				{
					//------------------------------------------------------------------------------
					// The following is a little inefficient, but is kept this
					// way to keep it clear. Once it's verified to work,
					// optimize...
					int32_t size = formalTypePtr->size;
					std::wstring_view dest = (std::wstring_view)ABLStackMallocCallback((size_t)size);
					if (!dest)
						ABL_Fatal(0,
							" ABL: Unable to AblStackHeap->malloc "
							"module formal array param ");
					std::wstring_view src = tos->address;
					std::wstring_view savePtr = dest;
					memcpy(dest, src, size);
					tos->address = savePtr;
				}
			}
			else
			{
				//-------------------------------
				// pass by reference parameter...
				if (formalTypePtr == RealTypePtr)
					pushAddress((Address) & (paramList[curParam].real));
				else if (formalTypePtr == IntegerTypePtr)
					pushAddress((Address) & (paramList[curParam].integer));
				else
					return (0);
				// const std::unique_ptr<SymTableNode>& idPtr = getCodeSymTableNodePtr();
				// execVariable(idPtr, USE_REFPARAM);
			}
			curParam++;
		}
	}
	execute(moduleIdPtr);
	if (returnVal)
		memcpy(returnVal, &returnValue, sizeof(StackItem));
	// extractSymTable(&SymTableDisplay[0], moduleIdPtr);
	//-----------
	// Summary...
	return (execStatementCount);
}

//***************************************************************************

int32_t ABLi_deleteModule(const std::unique_ptr<SymTableNode>& /* moduleIdPtr */)
{
	return (ABL_NO_ERR);
}

//***************************************************************************

void ABLi_close(void)
{
	if (!codeBuffer)
		return;
	UserFile::cleanup();
	destroyModuleRegistry();
	destroyLibraryRegistry();
	if (StaticVariablesSizes)
	{
		ABLStackFreeCallback(StaticVariablesSizes);
		StaticVariablesSizes = nullptr;
	}
	if (EternalVariablesSizes)
	{
		ABLStackFreeCallback(EternalVariablesSizes);
		EternalVariablesSizes = nullptr;
	}
	if (codeBuffer)
	{
		ABLCodeFreeCallback(codeBuffer);
		codeBuffer = nullptr;
	}
	if (stack)
	{
		ABLStackFreeCallback(stack);
		stack = nullptr;
	}
	if (debugger)
	{
		delete debugger;
		debugger = nullptr;
	}
	ABL_CloseProfileLog();
	ABLenabled = false;
}

//***************************************************************************

const std::unique_ptr<ABLModule>&
ABLi_loadLibrary(std::wstring_view sourceFileName, int32_t* numErrors, int32_t* numLinesProcessed,
	int32_t* numFilesProcessed, bool printLines, bool createInstance)
{
	//--------------------------------------------------------------------
	// Create an instance of it so it may be used from other modules. Note
	// that we need this when preprocessing since all identifiers in the
	// library should point to this module...
	const std::unique_ptr<ABLModule>& library = new ABLModule;
	if (!library)
		ABL_Fatal(0, "ABL: no RAM for library");
	CurLibrary = library;
	//-------------------------------------------------------------
	// Preprocess the library. Note that a library should be loaded
	// just once.
	int32_t libraryHandle = ABLi_preProcess(
		sourceFileName, numErrors, numLinesProcessed, numFilesProcessed, printLines);
	if (libraryHandle < (NumModulesRegistered - 1))
	{
		//------------------
		// Already loaded...
		delete library;
		library = nullptr;
		CurLibrary = nullptr;
		return (nullptr);
	}
	CurLibrary = nullptr;
	LibraryInstanceRegistry[numLibrariesLoaded] = library;
	numLibrariesLoaded++;
	if (!createInstance)
	{
		CurLibrary = nullptr;
		return (library);
	}
	int32_t err = library->init(libraryHandle);
	ABL_Assert(err == ABL_NO_ERR, err, " Error Loading ABL Library ");
	library->setName(sourceFileName);
	return (library);
}

//***************************************************************************

const std::unique_ptr<ABLParam>&
ABLi_createParamList(int32_t numParameters)
{
	if (numParameters)
	{
		const std::unique_ptr<ABLParam>& paramList =
			(const std::unique_ptr<ABLParam>&)ABLStackMallocCallback(sizeof(ABLParam) * (numParameters + 1));
		if (!paramList)
			ABL_Fatal(0, " ABL: Unable to AblStackHeap->malloc paramList ");
		memset(paramList, 0, sizeof(ABLParam) * (numParameters + 1));
		return (paramList);
	}
	return (nullptr);
}

//***************************************************************************

void ABLi_setIntegerParam(const std::unique_ptr<ABLParam>& paramList, int32_t index, int32_t value)
{
	if (paramList)
	{
		paramList[index].type = ABL_PARAM_INTEGER;
		paramList[index].integer = value;
	}
}

//***************************************************************************

void ABLi_setRealParam(const std::unique_ptr<ABLParam>& paramList, int32_t index, float value)
{
	if (paramList)
	{
		paramList[index].type = ABL_PARAM_REAL;
		paramList[index].real = value;
	}
}

//***************************************************************************

void ABLi_deleteParamList(const std::unique_ptr<ABLParam>& paramList)
{
	if (paramList)
		ABLStackFreeCallback(paramList);
}

//***************************************************************************

const std::unique_ptr<ABLModule>&
ABLi_getModule(int32_t id)
{
	if ((id >= 0) && (id < NumModules))
		return (ModuleInstanceRegistry[id]);
	return (nullptr);
}

//***************************************************************************

bool ABLi_enabled(void)
{
	return (ABLenabled);
}

//***************************************************************************

void ABLi_addFunction(
	std::wstring_view name, bool isOrder, std::wstring_view paramList, std::wstring_view returnType, void (*codeCallback)(void))
{
	enterStandardRoutine(name, -1, isOrder, paramList, returnType, codeCallback);
}

//***************************************************************************

int32_t
ABLi_registerInteger(std::wstring_view name, int32_t* address, int32_t numElements)
{
	if (strlen(name) >= MAXLEN_TOKENSTRING)
		ABL_Fatal(0, " ABLi_registerInteger: variable name too int32_t ");
	level = 0;
	strcpy(wordString, name);
	const std::unique_ptr<SymTableNode>& idPtr = nullptr;
	searchAndEnterThisTable(idPtr, SymTableDisplay[0]);
	if (numElements == 1)
	{
		idPtr->library = nullptr;
		idPtr->defn.key = DFN_VAR;
		idPtr->labelIndex = 0;
		idPtr->ptype = setType(IntegerTypePtr);
		idPtr->defn.info.data.varType = VAR_TYPE_REGISTERED;
		idPtr->defn.info.data.registeredData = address;
	}
	return (ABL_NO_ERR);
}

//***************************************************************************

int32_t
ABLi_registerReal(std::wstring_view name, float* address, int32_t /* numElements */)
{
	if (strlen(name) >= MAXLEN_TOKENSTRING)
		ABL_Fatal(0, " ABLi_registerInteger: variable name too int32_t ");
	level = 0;
	strcpy(wordString, name);
	const std::unique_ptr<SymTableNode>& idPtr = nullptr;
	searchAndEnterThisTable(idPtr, SymTableDisplay[0]);
	idPtr->library = nullptr;
	idPtr->defn.key = DFN_VAR;
	idPtr->labelIndex = 0;
	idPtr->ptype = setType(RealTypePtr);
	idPtr->defn.info.data.varType = VAR_TYPE_REGISTERED;
	idPtr->defn.info.data.registeredData = address;
	return (ABL_NO_ERR);
}

//***************************************************************************

bool ABLi_getSkipOrder(void)
{
	return (SkipOrder);
}

//***************************************************************************

void ABLi_resetOrders(void)
{
	CurModule->resetOrderCallFlags();
}

//***************************************************************************

int32_t
ABLi_getCurrentState(void)
{
	if (CurFSM)
		return (CurFSM->getStateHandle());
	return (nullptr);
}

//***************************************************************************

void ABLi_transState(int32_t newStateHandle)
{
	if (CurFSM && (newStateHandle > 0) && (newStateHandle < ModuleRegistry[CurFSM->getHandle()].numStateHandles))
		transState(ModuleRegistry[CurFSM->getHandle()].stateHandles[newStateHandle].state);
}

//***************************************************************************
// MODULE routines
//***************************************************************************

const std::unique_ptr<SymTableNode>&
moduleHeader(void)
{
	const std::unique_ptr<SymTableNode>& moduleIdPtr = nullptr;
	bool isFSM = false;
	if (CurLibrary)
		ifTokenGetElseError(TKN_LIBRARY, ABL_ERR_SYNTAX_MISSING_LIBRARY);
	else if (curToken == TKN_FSM)
	{
		isFSM = true;
		getToken();
	}
	else
		ifTokenGetElseError(TKN_MODULE, ABL_ERR_SYNTAX_MISSING_MODULE);
	//------------------------------------------------------------
	// NOTE: If a module is a library, its library pointer will be
	// non-nullptr.
	if (curToken == TKN_IDENTIFIER)
	{
		searchAndEnterLocalSymTable(moduleIdPtr);
		moduleIdPtr->defn.key = DFN_MODULE;
		moduleIdPtr->defn.info.routine.key = RTN_DECLARED;
		moduleIdPtr->defn.info.routine.flags = (isFSM ? ROUTINE_FLAG_FSM : 0);
		moduleIdPtr->defn.info.routine.orderCallIndex = 0;
		moduleIdPtr->defn.info.routine.numOrderCalls = 0;
		moduleIdPtr->defn.info.routine.paramCount = 0;
		moduleIdPtr->defn.info.routine.totalParamSize = 0;
		moduleIdPtr->defn.info.routine.totalLocalSize = 0;
		moduleIdPtr->defn.info.routine.params = nullptr;
		moduleIdPtr->defn.info.routine.locals = nullptr;
		moduleIdPtr->defn.info.routine.localSymTable = nullptr;
		moduleIdPtr->defn.info.routine.codeSegment = nullptr;
		moduleIdPtr->library = CurLibrary;
		moduleIdPtr->ptype = &DummyType;
		moduleIdPtr->labelIndex = 0;
		getToken();
	}
	else
		syntaxError(ABL_ERR_SYNTAX_MISSING_IDENTIFIER);
	//--------------------------------------------
	// For now, modules do not have param lists...
	synchronize(followModuleIdList, declarationStartList, statementStartList);
	enterScope(nullptr);
	if (curToken == TKN_LPAREN)
	{
		int32_t paramCount;
		int32_t totalParamSize;
		const std::unique_ptr<SymTableNode>& paramListPtr = formalParamList(&paramCount, &totalParamSize);
		// if (forwardFlag)
		//	syntaxError(ABL_ERR_SYNTAX_ALREADY_FORWARDED);
		// else {
		moduleIdPtr->defn.info.routine.paramCount = (uint8_t)paramCount;
		moduleIdPtr->defn.info.routine.totalParamSize = (uint8_t)totalParamSize;
		moduleIdPtr->defn.info.routine.params = paramListPtr;
		//}
	}
	// else if (!forwardFlag) {
	//	functionIdPtr->defn.info.routine.paramCount = 0;
	//	functionIdPtr->defn.info.routine.totalParamSize = 0;
	//	functionIdPtr->defn.info.routine.params = nullptr;
	//}
	//-----------------------------
	// Now, check if return type...
	moduleIdPtr->ptype = nullptr;
	const std::unique_ptr<SymTableNode>& typeIdPtr = nullptr;
	if (curToken == TKN_COLON)
	{
		getToken();
		if (curToken == TKN_IDENTIFIER)
		{
			searchAndFindAllSymTables(typeIdPtr);
			if (typeIdPtr->defn.key != DFN_TYPE)
				syntaxError(ABL_ERR_SYNTAX_INVALID_TYPE);
			// if (!forwardFlag)
			moduleIdPtr->ptype = typeIdPtr->ptype;
			getToken();
		}
		else
		{
			syntaxError(ABL_ERR_SYNTAX_MISSING_IDENTIFIER);
			moduleIdPtr->ptype = &DummyType;
		}
		// if (forwardFlag)
		//	syntaxError(ABL_ERR_SYNTAX_ALREADY_FORWARDED);
	}
	return (moduleIdPtr);
}

//***************************************************************************
// ROUTINE/FUNCTION routines
//***************************************************************************

void routine(void)
{
	const std::unique_ptr<SymTableNode>& routineIdPtr = nullptr;
	//------------------------------------------------------------------------
	// Do we want to have "procedures", or do functions handle both cases (with
	// and without return values)? For now, functions handle both...
	routineIdPtr = functionHeader();
	//------------------------------------------------------------------
	// We need to save a pointer to the current routine we're parsing...
	const std::unique_ptr<SymTableNode>& prevRoutineIdPtr = CurRoutineIdPtr;
	CurRoutineIdPtr = routineIdPtr;
	//---------------------
	// Error synchronize...
	synchronize(followHeaderList, declarationStartList, statementStartList);
	if (curToken == TKN_SEMICOLON)
		getToken();
	else if (tokenIn(declarationStartList) || tokenIn(statementStartList))
		syntaxError(ABL_ERR_SYNTAX_MISSING_SEMICOLON);
	if (strcmp(wordString, "forward") != 0)
	{
		routineIdPtr->defn.info.routine.key = RTN_DECLARED;
		// analyzeRoutineHeader(routineIdPtr);
		routineIdPtr->defn.info.routine.locals = nullptr;
		declarations(routineIdPtr, false);
		synchronize(followRoutineDeclsList, nullptr, nullptr);
		if (curToken != TKN_CODE)
			syntaxError(ABL_ERR_SYNTAX_MISSING_CODE);
		crunchToken();
		blockType = BLOCK_ROUTINE;
		blockFlag = true;
		routineIdPtr->defn.info.routine.orderCallIndex = (uint16_t)NumOrderCalls;
		// compoundStatement();
		getToken();
		TokenCodeType endToken = TKN_END_FUNCTION;
		if (routineIdPtr->defn.info.routine.flags & ROUTINE_FLAG_STATE)
			endToken = TKN_END_STATE;
		else if (routineIdPtr->defn.info.routine.flags & ROUTINE_FLAG_ORDER)
			endToken = TKN_END_ORDER;
		if (curToken != endToken)
			do
			{
				statement();
				while (curToken == TKN_SEMICOLON)
					getToken();
				if (curToken == endToken)
					break;
				//------------------------------------
				// Synchronize after possible error...
				synchronize(statementStartList, nullptr, nullptr);
			} while (tokenIn(statementStartList));
		ifTokenGetElseError(endToken, ABL_ERR_SYNTAX_MISSING_END_FUNCTION);
		blockFlag = false;
		routineIdPtr->defn.info.routine.numOrderCalls =
			(uint16_t)NumOrderCalls - routineIdPtr->defn.info.routine.orderCallIndex;
		routineIdPtr->defn.info.routine.codeSegment =
			createCodeSegment(routineIdPtr->defn.info.routine.codeSegmentSize);
		analyzeBlock(routineIdPtr->defn.info.routine.codeSegment);
	}
	else
	{
		getToken();
		routineIdPtr->defn.info.routine.key = RTN_FORWARD;
		analyzeRoutineHeader(routineIdPtr);
	}
	if (routineIdPtr->defn.info.routine.flags & ROUTINE_FLAG_STATE)
	{
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

const std::unique_ptr<SymTableNode>&
forwardState(std::wstring_view stateName)
{
	const std::unique_ptr<SymTableNode>& stateSymbol = searchSymTableForState(stateName, SymTableDisplay[1]);
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
	stateSymbol->defn.info.routine.params = nullptr;
	stateSymbol->defn.info.routine.locals = nullptr;
	stateSymbol->defn.info.routine.localSymTable = nullptr;
	stateSymbol->defn.info.routine.codeSegment = nullptr;
	stateSymbol->library = CurLibrary;
	stateSymbol->ptype = nullptr;
	stateSymbol->labelIndex = 0;
	//-------------------
	// Force the level...
	stateSymbol->level = 1;
	return (stateSymbol);
}

//***************************************************************************

const std::unique_ptr<SymTableNode>&
functionHeader(void)
{
	bool isState = false;
	bool isOrder = false;
	if (curToken == TKN_STATE)
		isState = true;
	else if (curToken == TKN_ORDER)
		isOrder = true;
	getToken();
	const std::unique_ptr<SymTableNode>& functionIdPtr = nullptr;
	const std::unique_ptr<SymTableNode>& typeIdPtr = nullptr;
	bool forwardFlag = false;
	if (curToken == TKN_IDENTIFIER)
	{
		functionIdPtr = searchSymTableForFunction(wordString, SymTableDisplay[level]);
		if (!functionIdPtr)
		{
			enterLocalSymTable(functionIdPtr);
			functionIdPtr->defn.key = DFN_FUNCTION;
			functionIdPtr->defn.info.routine.key = RTN_DECLARED;
			functionIdPtr->defn.info.routine.paramCount = 0;
			functionIdPtr->defn.info.routine.flags = 0;
			functionIdPtr->defn.info.routine.orderCallIndex = 0;
			functionIdPtr->defn.info.routine.numOrderCalls = 0;
			functionIdPtr->defn.info.routine.totalParamSize = 0;
			functionIdPtr->defn.info.routine.totalLocalSize = 0;
			functionIdPtr->defn.info.routine.params = nullptr;
			functionIdPtr->defn.info.routine.locals = nullptr;
			functionIdPtr->defn.info.routine.localSymTable = nullptr;
			functionIdPtr->defn.info.routine.codeSegment = nullptr;
			if (isOrder)
				functionIdPtr->defn.info.routine.flags |= ROUTINE_FLAG_ORDER;
			if (isState)
				functionIdPtr->defn.info.routine.flags |= ROUTINE_FLAG_STATE;
			functionIdPtr->library = CurLibrary;
			functionIdPtr->ptype = &DummyType;
			functionIdPtr->labelIndex = 0;
		}
		else if ((functionIdPtr->defn.key == DFN_FUNCTION) && (functionIdPtr->defn.info.routine.key == RTN_FORWARD))
			forwardFlag = true;
		else
			syntaxError(ABL_ERR_SYNTAX_REDEFINED_IDENTIFIER);
		getToken();
	}
	//---------------------
	// Error synchronize...
	synchronize(followFunctionIdList, declarationStartList, statementStartList);
	enterScope(nullptr);
	if (curToken == TKN_LPAREN)
	{
		int32_t paramCount;
		int32_t totalParamSize;
		const std::unique_ptr<SymTableNode>& paramListPtr = formalParamList(&paramCount, &totalParamSize);
		if (forwardFlag)
			syntaxError(ABL_ERR_SYNTAX_ALREADY_FORWARDED);
		else
		{
			functionIdPtr->defn.info.routine.paramCount = (uint8_t)paramCount;
			functionIdPtr->defn.info.routine.totalParamSize = (uint8_t)totalParamSize;
			functionIdPtr->defn.info.routine.params = paramListPtr;
		}
	}
	else if (!forwardFlag)
	{
		functionIdPtr->defn.info.routine.paramCount = 0;
		functionIdPtr->defn.info.routine.totalParamSize = 0;
		functionIdPtr->defn.info.routine.params = nullptr;
	}
	//--------------------------------------------
	// Now, check if return type and/or forward...
	functionIdPtr->ptype = nullptr;
	bool hasIntegerReturnValue = false;
	if (curToken == TKN_COLON)
	{
		getToken();
		if (curToken == TKN_IDENTIFIER)
		{
			searchAndFindAllSymTables(typeIdPtr);
			if (typeIdPtr->defn.key != DFN_TYPE)
				syntaxError(ABL_ERR_SYNTAX_INVALID_TYPE);
			hasIntegerReturnValue = (typeIdPtr->ptype == IntegerTypePtr);
			if (!forwardFlag)
				functionIdPtr->ptype = typeIdPtr->ptype;
			getToken();
		}
		else
		{
			syntaxError(ABL_ERR_SYNTAX_MISSING_IDENTIFIER);
			functionIdPtr->ptype = &DummyType;
		}
		if (forwardFlag)
			syntaxError(ABL_ERR_SYNTAX_ALREADY_FORWARDED);
	}
	if (isOrder && !hasIntegerReturnValue)
		syntaxError(ABL_ERR_SYNTAX_ORDER_RETURNTYPE);
	return (functionIdPtr);
}

//***************************************************************************

const std::unique_ptr<SymTableNode>&
formalParamList(int32_t* count, int32_t* totalSize)
{
	const std::unique_ptr<SymTableNode>& paramIdPtr;
	const std::unique_ptr<SymTableNode>& firstIdPtr;
	const std::unique_ptr<SymTableNode>& lastIdPtr = nullptr;
	const std::unique_ptr<SymTableNode>& paramListPtr = nullptr;
	const std::unique_ptr<SymTableNode>& typeIdPtr;
	const std::unique_ptr<Type>& paramTypePtr;
	DefinitionType paramDefn;
	int32_t paramCount = 0;
	int32_t paramOffset = STACK_FRAME_HEADER_SIZE;
	getToken();
	while ((curToken == TKN_IDENTIFIER) || (curToken == TKN_REF))
	{
		firstIdPtr = nullptr;
		if (curToken == TKN_REF)
		{
			paramDefn = DFN_REFPARAM;
			getToken();
		}
		else
			paramDefn = DFN_VALPARAM;
		//-----------------------------------
		// Grab the type of this parameter...
		if (curToken == TKN_IDENTIFIER)
		{
			searchAndFindAllSymTables(typeIdPtr);
			if (typeIdPtr->defn.key != DFN_TYPE)
				syntaxError(ABL_ERR_SYNTAX_INVALID_TYPE);
			paramTypePtr = (const std::unique_ptr<Type>&)(typeIdPtr->ptype);
			getToken();
		}
		else
		{
			syntaxError(ABL_ERR_SYNTAX_MISSING_IDENTIFIER);
			paramTypePtr = &DummyType;
		}
		//------------------------------------------------------------
		// Is this an array parameter? If so, parse the dimensions and
		// whether it's an open array...
		if (curToken == TKN_LBRACKET)
		{
		}
		//-----------------------------------------------
		// Now grab the variable id for this parameter...
		if (curToken == TKN_IDENTIFIER)
		{
			searchAndEnterLocalSymTable(paramIdPtr);
			paramIdPtr->defn.key = paramDefn;
			paramIdPtr->labelIndex = 0;
			paramIdPtr->ptype = paramTypePtr;
			paramIdPtr->defn.info.data.offset = paramOffset++;
			paramCount++;
			if (paramListPtr == nullptr)
				paramListPtr = paramIdPtr;
			if (lastIdPtr != nullptr)
				lastIdPtr->next = paramIdPtr;
			lastIdPtr = paramIdPtr;
			// if (firstIdPtr == nullptr)
			//	firstIdPtr = lastIdPtr = paramIdPtr;
			// else {
			//	lastIdPtr->next = paramIdPtr;
			//	lastIdPtr = paramIdPtr;
			//}
			getToken();
			// ifTokenGet(TKN_COMMA);
		}
		else
		{
			syntaxError(ABL_ERR_SYNTAX_MISSING_IDENTIFIER);
			//--------------------------------
			// Do we need to synchronize here?
		}
		//------------------------------------------------------------------
		// Assign the offset and the type to all param ids in the sublist...
		// for (paramIdPtr = firstIdPtr; paramIdPtr != nullptr; paramIdPtr =
		// paramIdPtr->next) { 	paramIdPtr->ptype = paramTypePtr;
		//	paramIdPtr->defn.info.data.offset = paramOffset++;
		//}
		//-----------------------------------------------
		// Link this list to the list of all param ids...
		// if (prevLastIdPtr != nullptr)
		//	prevLastIdPtr->next = firstIdPtr;
		// prevLastIdPtr = lastIdPtr;
		//-------------------------------
		// Error synch... should be ; or )
		synchronize(followParamsList, nullptr, nullptr);
		ifTokenGet(TKN_COMMA);
	}
	ifTokenGetElseError(TKN_RPAREN, ABL_ERR_SYNTAX_MISSING_RPAREN);
	*count = paramCount;
	*totalSize = paramOffset - STACK_FRAME_HEADER_SIZE;
	if (paramCount > 255)
		syntaxError(ABL_ERR_SYNTAX_TOO_MANY_FORMAL_PARAMETERS);
	return (paramListPtr);
}

//***************************************************************************

const std::unique_ptr<Type>&
routineCall(const std::unique_ptr<SymTableNode>& routineIdPtr, int32_t paramCheckFlag)
{
	const std::unique_ptr<SymTableNode>& thisRoutineIdPtr = CurRoutineIdPtr;
	const std::unique_ptr<Type>& resultType = nullptr;
	if ((routineIdPtr->defn.info.routine.key == RTN_DECLARED) || (routineIdPtr->defn.info.routine.key == RTN_FORWARD) || !paramCheckFlag)
		resultType = declaredRoutineCall(routineIdPtr, paramCheckFlag);
	else
		resultType = standardRoutineCall(routineIdPtr);
	CurRoutineIdPtr = thisRoutineIdPtr;
	return (resultType);
}

//***************************************************************************

const std::unique_ptr<Type>&
declaredRoutineCall(const std::unique_ptr<SymTableNode>& routineIdPtr, int32_t paramCheckFlag)
{
	actualParamList(routineIdPtr, paramCheckFlag);
	return (
		/*routineIdPtr->defn.key == DFN_PROCEDURE ? nullptr :*/ (const std::unique_ptr<Type>&)(routineIdPtr->ptype));
}

//***************************************************************************

void actualParamList(const std::unique_ptr<SymTableNode>& routineIdPtr, int32_t paramCheckFlag)
{
	const std::unique_ptr<SymTableNode>& formalParamIdPtr = nullptr;
	DefinitionType formalParamDefn = (DefinitionType)0;
	const std::unique_ptr<Type>& formalParamTypePtr = nullptr;
	const std::unique_ptr<Type>& actualParamTypePtr = nullptr;
	if (paramCheckFlag)
		formalParamIdPtr = (const std::unique_ptr<SymTableNode>&)(routineIdPtr->defn.info.routine.params);
	if (curToken == TKN_LPAREN)
	{
		//-----------------------------------------------
		// Loop to process actual parameter expression...
		do
		{
			//----------------------------------------------------
			// Obtain info about the corresponding formal param...
			if (paramCheckFlag && (formalParamIdPtr != nullptr))
			{
				formalParamDefn = formalParamIdPtr->defn.key;
				formalParamTypePtr = (const std::unique_ptr<Type>&)(formalParamIdPtr->ptype);
			}
			getToken();
			if ((formalParamIdPtr == nullptr) || (formalParamDefn == DFN_VALPARAM) || !paramCheckFlag)
			{
				//--------------
				// VAL Params...
				actualParamTypePtr = expression();
				if (paramCheckFlag && (formalParamIdPtr != nullptr) && !isAssignTypeCompatible(formalParamTypePtr, actualParamTypePtr))
					syntaxError(ABL_ERR_SYNTAX_INCOMPATIBLE_TYPES);
			}
			else
			{
				//--------------
				// VAR params...
				if (curToken == TKN_IDENTIFIER)
				{
					const std::unique_ptr<SymTableNode>& idPtr;
					searchAndFindAllSymTables(idPtr);
					actualParamTypePtr = variable(idPtr);
					if (formalParamTypePtr != actualParamTypePtr)
						syntaxError(ABL_ERR_SYNTAX_INCOMPATIBLE_TYPES);
				}
				else
				{
					//------------------------------------------------------------------
					// Not a variable--parse the expression anyway for error
					// recovery...
					actualParamTypePtr = expression();
					syntaxError(ABL_ERR_SYNTAX_INVALID_REF_PARAM);
				}
			}
			//---------------------------------------
			// More actual params than formal params?
			if (paramCheckFlag)
			{
				if (formalParamIdPtr == nullptr)
					syntaxError(ABL_ERR_SYNTAX_WRONG_NUMBER_OF_PARAMS);
				else
					formalParamIdPtr = formalParamIdPtr->next;
			}
			//---------------
			// Error synch...
			synchronize(followParamList, statementEndList, nullptr);
		} while (curToken == TKN_COMMA);
		ifTokenGetElseError(TKN_RPAREN, ABL_ERR_SYNTAX_MISSING_RPAREN);
	}
	//----------------------------------------
	// Fewer actual params than formal params?
	if (paramCheckFlag && (formalParamIdPtr != nullptr))
		syntaxError(ABL_ERR_SYNTAX_WRONG_NUMBER_OF_PARAMS);
}

//***************************************************************************

extern wchar_t SetStateDebugStr[256];

void transState(const std::unique_ptr<SymTableNode>& newState)
{
	if (CurFSM)
	{
		CurFSM->setPrevState(CurFSM->getState());
		CurFSM->setState(newState);
		sprintf(SetStateDebugStr, "%s:%s, line %d", CurModule->getFileName(), newState->name,
			execLineNumber);
		NewStateSet = true;
	}
}

//***************************************************************************

} // namespace mclib::abl
