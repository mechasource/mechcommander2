//===========================================================================//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//
//***************************************************************************
//
//								ABLDBUG.CPP
//
//***************************************************************************
#include "stdinc.h"

//#include "ablgen.h"
//#include "ablerr.h"
//#include "ablscan.h"
//#include "ablsymt.h"
//#include "ablexec.h"
#include "abldbug.h"

namespace mclib::abl {

//***************************************************************************

//----------
// EXTERNALS

extern int32_t level;
extern int32_t execLineNumber;
// extern int32_t				execStatementCount;
extern TokenCodeType codeToken;

extern const std::wstring_view& codeBuffer;
extern const std::wstring_view& codeBufferPtr;
extern const std::wstring_view& codeSegmentPtr;
extern const std::wstring_view& statementStartPtr;

extern const std::unique_ptr<StackItem>& tos;
// extern const std::unique_ptr<StackItem>&		stackFrameBasePtr;
extern const std::unique_ptr<SymTableNode>& CurRoutineIdPtr;
extern const std::unique_ptr<SymTableNode>& symTableDisplay[];

extern int32_t errorCount;
extern wchar_t curChar;
extern TokenCodeType curToken;
extern int32_t lineNumber;
extern Literal curLiteral;
extern int32_t bufferOffset;
extern wchar_t sourceBuffer[MAXLEN_SOURCELINE];
// extern int32_t				bufferOffset;
extern const std::wstring_view& bufferp;
// extern const std::wstring_view&			tokenp;
extern wchar_t wordString[MAXLEN_TOKENSTRING];

extern const std::unique_ptr<Type>& IntegerTypePtr;
extern const std::unique_ptr<Type>& CharTypePtr;
extern const std::unique_ptr<Type>& RealTypePtr;
extern const std::unique_ptr<Type>& BooleanTypePtr;
extern Type DummyType;

extern const std::unique_ptr<ModuleEntry>& ModuleRegistry;
extern const std::unique_ptr<ABLModule>&* ModuleInstanceRegistry;
extern int32_t MaxModules;
extern int32_t NumModulesRegistered;
extern int32_t NumModuleInstances;
extern int32_t MaxWatchesPerModule;
extern int32_t MaxBreakPointsPerModule;
extern const std::wstring_view& TokenStrings[NUM_TOKENS];

// extern StackItem*		stack;
// extern const std::unique_ptr<StackItem>&		stackFrameBasePtr;
extern const std::unique_ptr<StackItem>& StaticDataPtr;
// extern const std::unique_ptr<SymTableNode>&	CurRoutineIdPtr;

// extern int32_t				MaxLoopIterations;

void
CheckMouse();

//---------------------------------------------------------------------------

int32_t MaxBreaks = 50;
int32_t MaxWatches = 50;
const std::unique_ptr<Debugger>& debugger = nullptr;

wchar_t Debugger::message[512];

extern bool takeScreenShot;

#ifdef USE_IFACE

extern aTextObject* ABLDebuggerIn;
extern ScrollingTextWindow* ABLDebuggerOut;

#endif

//***************************************************************************
// WATCH MANAGER class
//***************************************************************************

PVOID
WatchManager::operator new(size_t mySize)
{
	void* result = ABLStackMallocCallback(mySize);
	return (result);
}

//---------------------------------------------------------------------------

void
WatchManager::operator delete(PVOID us)
{
	ABLStackFreeCallback(us);
}

//---------------------------------------------------------------------------

int32_t
WatchManager::init(int32_t max)
{
	maxWatches = max;
	numWatches = 0;
	watches = (WatchPtr)ABLStackMallocCallback(max * sizeof(Watch));
	if (!watches)
		return (-1);
	return (ABL_NO_ERR);
}

//---------------------------------------------------------------------------

void
WatchManager::destroy(void)
{
	if (watches)
	{
		ABLStackFreeCallback(watches);
		watches = nullptr;
	}
	maxWatches = 0;
	numWatches = 0;
}

//---------------------------------------------------------------------------

WatchPtr
WatchManager::add(const std::unique_ptr<SymTableNode>& idPtr)
{
	//------------------------------------------
	// This routine assumes idPtr is non-nullptr...
	switch (idPtr->defn.key)
	{
	case DFN_CONST:
	case DFN_VAR:
	case DFN_VALPARAM:
	case DFN_REFPARAM:
		// case DFN_FIELD:
		if (idPtr->info)
			return ((WatchPtr)idPtr->info);
		else if (numWatches < maxWatches)
		{
			idPtr->info = (const std::wstring_view&)&watches[numWatches];
			watches[numWatches].idPtr = idPtr;
			watches[numWatches].store = false;
			watches[numWatches].breakOnStore = false;
			watches[numWatches].fetch = false;
			watches[numWatches].breakOnFetch = false;
			return (&watches[numWatches++]);
		}
		break;
	}
	return (nullptr);
}

//---------------------------------------------------------------------------

int32_t
WatchManager::remove(const std::unique_ptr<SymTableNode>& idPtr)
{
	if (!idPtr)
		return (1);
	if (!idPtr->info)
		return (2);
	//------------------------------------------
	// Find this id's watch in the watch list...
	int32_t removeIndex;
	for (removeIndex = 0; removeIndex < numWatches; removeIndex++)
		if (&watches[removeIndex] == (WatchPtr)idPtr->info)
			break;
	//--------------------------------------------------------
	// The id no longer points to a watch in the watch list...
	idPtr->info = nullptr;
	numWatches--;
	//-------------------------------------
	// Fill in the gap in the watch list...
	for (size_t i = removeIndex; removeIndex < numWatches; removeIndex++)
	{
		watches[i].idPtr = watches[i + 1].idPtr;
		watches[i].store = watches[i + 1].store;
		watches[i].breakOnStore = watches[i + 1].breakOnStore;
		watches[i].fetch = watches[i + 1].fetch;
		watches[i].breakOnFetch = watches[i + 1].breakOnFetch;
		watches[i].idPtr->info = (const std::wstring_view&)&watches[i];
	}
	return (ABL_NO_ERR);
}

//---------------------------------------------------------------------------

int32_t
WatchManager::removeAll(void)
{
	for (size_t i = 0; i < numWatches; i++)
	{
		watches[i].idPtr->info = nullptr;
		watches[i].idPtr = nullptr;
		watches[i].store = false;
		watches[i].breakOnStore = false;
		watches[i].fetch = false;
		watches[i].breakOnFetch = false;
	}
	int32_t numRemoved = numWatches;
	numWatches = 0;
	return (numRemoved);
}

//---------------------------------------------------------------------------

int32_t
WatchManager::setStore(const std::unique_ptr<SymTableNode>& idPtr, bool state, bool breakToDebug)
{
	if (!idPtr)
		return (1);
	WatchPtr watch = (WatchPtr)idPtr->info;
	if (state)
	{
		if (!watch)
			watch = add(idPtr);
		if (!watch)
			return (2);
		watch->store = true;
		watch->breakOnStore = breakToDebug;
	}
	else
	{
		if (watch)
		{
			if (watch->fetch)
			{
				watch->store = false;
				watch->breakOnStore = false;
			}
			else
				remove(idPtr);
		}
	}
	return (ABL_NO_ERR);
}

//---------------------------------------------------------------------------

int32_t
WatchManager::setFetch(const std::unique_ptr<SymTableNode>& idPtr, bool state, bool breakToDebug)
{
	if (!idPtr)
		return (1);
	WatchPtr watch = (WatchPtr)idPtr->info;
	if (state)
	{
		if (!watch)
			watch = add(idPtr);
		if (!watch)
			return (2);
		watch->fetch = true;
		watch->breakOnFetch = breakToDebug;
	}
	else
	{
		if (watch)
		{
			if (watch->store)
			{
				watch->fetch = false;
				watch->breakOnFetch = false;
			}
			else
				remove(idPtr);
		}
	}
	return (ABL_NO_ERR);
}

//---------------------------------------------------------------------------

bool
WatchManager::getStore(const std::unique_ptr<SymTableNode>& idPtr)
{
	if (!idPtr->info)
		return (false);
	return (((WatchPtr)idPtr->info)->store);
}

//---------------------------------------------------------------------------

bool
WatchManager::getFetch(const std::unique_ptr<SymTableNode>& idPtr)
{
	if (!idPtr->info)
		return (false);
	return (((WatchPtr)idPtr->info)->fetch);
}

//---------------------------------------------------------------------------

void
WatchManager::print(void)
{
	for (size_t i = 0; i < numWatches; i++)
	{
		// const std::unique_ptr<SymTableNode>& idPtr = watches[i].idPtr;
		// Display info here...
	}
}

//***************************************************************************
// BREAK POINT MANAGER class
//***************************************************************************

PVOID
BreakPointManager::operator new(size_t mySize)
{
	void* result = ABLStackMallocCallback(mySize);
	return (result);
}

//---------------------------------------------------------------------------

void
BreakPointManager::operator delete(PVOID us)
{
	ABLStackFreeCallback(us);
}

//---------------------------------------------------------------------------

int32_t
BreakPointManager::init(int32_t max)
{
	maxBreakPoints = max;
	numBreakPoints = 0;
	breakPoints = (int32_t*)ABLStackMallocCallback(max * sizeof(int32_t));
	if (!breakPoints)
		return (-1);
	return (ABL_NO_ERR);
}

//---------------------------------------------------------------------------

void
BreakPointManager::destroy(void)
{
	if (breakPoints)
	{
		ABLStackFreeCallback(breakPoints);
		breakPoints = nullptr;
	}
	maxBreakPoints = 0;
	numBreakPoints = 0;
}

//---------------------------------------------------------------------------

int32_t
BreakPointManager::add(int32_t lineNumber)
{
	//----------------------------------------------------------------
	// This does not check to make sure the line number is within
	// the source code's range--if it isn't, the break is simply never
	// hit...
	if (numBreakPoints == maxBreakPoints)
		return (1);
	else if (lineNumber <= 0)
		return (2);
	else
	{
		//------------------------------------------------
		// Insert it into the list (in ascending order)...
		int32_t insertPoint = 0;
		for (insertPoint = 0; insertPoint < numBreakPoints; insertPoint++)
		{
			if (lineNumber == breakPoints[insertPoint])
				return (ABL_NO_ERR);
			else if (lineNumber < breakPoints[insertPoint])
				break;
		}
		for (size_t i = insertPoint; i < numBreakPoints; i++)
			breakPoints[i + 1] = breakPoints[i];
		breakPoints[insertPoint] = lineNumber;
		numBreakPoints++;
	}
	return (ABL_NO_ERR);
}

//---------------------------------------------------------------------------

int32_t
BreakPointManager::remove(int32_t lineNumber)
{
	int32_t index;
	for (index = 0; index < numBreakPoints; index++)
		if (lineNumber == breakPoints[index])
			break;
	numBreakPoints--;
	//------------------
	// Shift 'em down...
	for (size_t i = index; i < numBreakPoints; i++)
		breakPoints[i] = breakPoints[i + 1];
	return (ABL_NO_ERR);
}

//---------------------------------------------------------------------------

int32_t
BreakPointManager::removeAll(void)
{
	int32_t numRemoved = numBreakPoints;
	numBreakPoints = 0;
	return (numRemoved);
}

//---------------------------------------------------------------------------

bool
BreakPointManager::isBreakPoint(int32_t lineNumber)
{
	if (numBreakPoints > 0)
	{
		for (size_t i = 0; i < numBreakPoints; i++)
			if (lineNumber == breakPoints[i])
				return (true);
	}
	return (false);
}

//---------------------------------------------------------------------------

void
BreakPointManager::print(void)
{
	//--------------------------------------------------------------
	// If no line number, do default action--list all breakpoints...
	for (size_t i = 0; i < numBreakPoints; i++)
	{
		// print info...
	}
}

//***************************************************************************
// DEBUGGER class
//***************************************************************************

PVOID
Debugger::operator new(size_t mySize)
{
	void* result = ABLStackMallocCallback(mySize);
	return (result);
}

//---------------------------------------------------------------------------

void
Debugger::operator delete(PVOID us)
{
	ABLStackFreeCallback(us);
}

//---------------------------------------------------------------------------

int32_t
Debugger::init(void (*callback)(const std::wstring_view& s), const std::unique_ptr<ABLModule>& _module)
{
	printCallback = callback;
	module = _module;
	if (module)
	{
		watchManager = module->getWatchManager();
		breakPointManager = module->getBreakPointManager();
	}
	return (ABL_NO_ERR);
}

//---------------------------------------------------------------------------

void
Debugger::destroy(void)
{
}

//---------------------------------------------------------------------------

int32_t
Debugger::print(const std::wstring_view& s)
{
	if (printCallback)
		(*printCallback)(s);
	return (ABL_NO_ERR);
}

//---------------------------------------------------------------------------

void
Debugger::setModule(const std::unique_ptr<ABLModule>& _module)
{
	module = _module;
	breakPointManager = module->getBreakPointManager();
	watchManager = module->getWatchManager();
	step = module->getStep();
	trace = traceEntry = traceExit = module->getTrace();
}

//---------------------------------------------------------------------------

int32_t
Debugger::setWatch(int32_t states)
{
	getToken();
	switch (curToken)
	{
	case TKN_SEMICOLON:
		print("Variables currently watched:\n");
		//-------------------------------------------------------------
		// No variable, so default watch action--display all watches...
		watchManager->print();
		break;
	case TKN_IDENTIFIER:
		const std::unique_ptr<SymTableNode>& idPtr = nullptr;
		searchAndFindAllSymTables(idPtr);
		getToken();
		//-----------------
		// STORE setting...
		bool breakToDebug = ((states & WATCH_BREAK) != 0);
		if (states & WATCH_STORE_OFF)
			watchManager->setStore(idPtr, false, breakToDebug);
		else if (states & WATCH_STORE_ON)
			watchManager->setStore(idPtr, true, breakToDebug);
		//-----------------
		// FETCH setting...
		if (states & WATCH_FETCH_OFF)
			watchManager->setFetch(idPtr, false, breakToDebug);
		else if (states & WATCH_FETCH_ON)
			watchManager->setFetch(idPtr, true, breakToDebug);
		break;
	}
	return (ABL_NO_ERR);
}

//---------------------------------------------------------------------------

int32_t
Debugger::addBreakPoint(void)
{
	getToken();
	switch (curToken)
	{
	case TKN_SEMICOLON:
		print("Variables currently watched:\n");
		breakPointManager->print();
		break;
	case TKN_NUMBER:
		if (curLiteral.type == LIT_INTEGER)
			breakPointManager->add(curLiteral.value.integer);
		getToken();
		break;
	}
	return (ABL_NO_ERR);
}

//---------------------------------------------------------------------------

int32_t
Debugger::removeBreakPoint(void)
{
	getToken();
	switch (curToken)
	{
	case TKN_SEMICOLON:
		breakPointManager->removeAll();
		break;
	case TKN_NUMBER:
		if (curLiteral.type == LIT_INTEGER)
			breakPointManager->remove(curLiteral.value.integer);
		getToken();
		break;
	}
	return (ABL_NO_ERR);
}

//---------------------------------------------------------------------------

void
Debugger::sprintStatement(const std::wstring_view& dest)
{
	//---------------------------------------------------------------------
	// First, rebuild the the current statement from the module code. Then,
	// spit it out as we do so...
	bool done = false;
	const std::wstring_view& cp = statementStartPtr;
	do
	{
		TokenCodeType token = (TokenCodeType)*cp;
		cp++;
		switch (token)
		{
		case TKN_STATEMENT_MARKER:
			done = true;
			break;
		case TKN_SEMICOLON:
		case TKN_END_IF:
		case TKN_END_WHILE:
		case TKN_END_FOR:
		case TKN_END_FUNCTION:
		case TKN_END_MODULE:
		case TKN_END_FSM:
		case TKN_END_LIBRARY:
		case TKN_END_CASE:
		case TKN_END_SWITCH:
		case TKN_END_ORDER:
		case TKN_END_STATE:
		case TKN_THEN:
			// case TKN_UNTIL:
			done = true;
		//--------------------------
		// Do NOT want break here...
		default:
			// done = false;
			switch (token)
			{
			case TKN_ADDRESS_MARKER:
				cp += sizeof(Address);
				break;
			case TKN_IDENTIFIER:
			case TKN_NUMBER:
			case TKN_STRING:
			{
				const std::unique_ptr<SymTableNode>& symbol = *((const std::unique_ptr<SymTableNode>&*)cp);
				strcat(dest, " ");
				strcat(dest, symbol->name);
				cp += sizeof(const std::unique_ptr<SymTableNode>&);
			}
			break;
			default:
				strcat(dest, " ");
				strcat(dest, TokenStrings[token]);
				break;
			}
			break;
		}
	} while (!done);
}

//---------------------------------------------------------------------------

void
Debugger::sprintLineNumber(const std::wstring_view& dest)
{
	//--------------------------
	// PRINT LINE NUMBER HERE...
	sprintf(dest, "LINE#");
}

//---------------------------------------------------------------------------

void
Debugger::sprintDataValue(const std::wstring_view& dest, const std::unique_ptr<StackItem>& data, const std::unique_ptr<Type>& dataType)
{
	if ((dataType->form == FRM_ENUM) && (dataType != BooleanTypePtr))
		dataType = IntegerTypePtr;
	if (dataType == IntegerTypePtr)
		sprintf(dest, "%d", data->integer);
	else if (dataType == RealTypePtr)
		sprintf(dest, "%0.6f", data->real);
	else if (dataType == BooleanTypePtr)
		sprintf(dest, "%s", (data->integer == 1) ? "true" : "false");
	else if (dataType == CharTypePtr)
		sprintf(dest, "%c", data->byte);
	else if (dataType->form == FRM_ARRAY)
	{
		if (dataType->info.array.elementTypePtr == CharTypePtr)
		{
			// PRINT CHAR ARRAY HERE...
			sprintf(dest, "CHAR ARRAY");
		}
		else
		{
			// PRINT ARRAY HERE...
			sprintf(dest, "ARRAY");
		}
	}
}

//---------------------------------------------------------------------------

int32_t
Debugger::sprintSimpleValue(const std::wstring_view& dest, const std::unique_ptr<SymTableNode>& symbol)
{
	//--------------------------------------------------------------------
	// This code is adapted from execVariable(). If that function changes,
	// this better too!
	const std::unique_ptr<Type>& ptype = (const std::unique_ptr<Type>&)(symbol->ptype);
	if (symbol->defn.key == DFN_CONST)
	{
		if (ptype == IntegerTypePtr)
			sprintf(dest, "%d", symbol->defn.info.constant.value.integer);
		else if (ptype == CharTypePtr)
			sprintf(dest, "%c", symbol->defn.info.constant.value.character);
		else
			sprintf(dest, "%.4f", symbol->defn.info.constant.value.real);
	}
	else
	{
		//--------------------------------------------------------------------
		// First, point to the variable's stack item. If the variable's scope
		// level is less than the current scope level, follow the static links
		// to the proper stack frame base...
		const std::unique_ptr<StackItem>& dataPtr = nullptr;
		switch (symbol->defn.info.data.varType)
		{
		case VAR_TYPE_NORMAL:
		{
			const std::unique_ptr<StackFrameHeader>& headerPtr = (const std::unique_ptr<StackFrameHeader>&)stackFrameBasePtr;
			int32_t delta = level - symbol->level;
			while (delta-- > 0)
				headerPtr = (const std::unique_ptr<StackFrameHeader>&)headerPtr->staticLink.address;
			dataPtr = (const std::unique_ptr<StackItem>&)headerPtr + symbol->defn.info.data.offset;
		}
		break;
		case VAR_TYPE_ETERNAL:
			dataPtr = (const std::unique_ptr<StackItem>&)stack + symbol->defn.info.data.offset;
			break;
		case VAR_TYPE_STATIC:
			dataPtr = (const std::unique_ptr<StackItem>&)StaticDataPtr + symbol->defn.info.data.offset;
			break;
		}
		//---------------------------------------------------------------
		// If it's a scalar or enumeration reference parameter, that item
		// points to the actual item...
		if ((symbol->defn.key == DFN_REFPARAM) && (ptype->form != FRM_ARRAY) /* && (ptype->form != FRM_RECORD)*/)
			dataPtr = (const std::unique_ptr<StackItem>&)dataPtr->address;
		if ((ptype->form != FRM_ARRAY) /*&& (ptype->form != FRM_RECORD)*/)
		{
			ABL_Assert(dataPtr != nullptr, 0, " Debugger.sprintSimpleValue(): dataPtr is nullptr ");
			if ((ptype == IntegerTypePtr) || (ptype->form == FRM_ENUM))
				sprintf(dest, "%d", *((int32_t*)dataPtr));
			else if (ptype == CharTypePtr)
				sprintf(dest, "\"%c\"", *((const std::wstring_view&)dataPtr));
			else
				sprintf(dest, "%.4f", *((float*)dataPtr));
		}
		else
			sprintf(dest, "ARRAY");
	}
	return (ABL_NO_ERR);
}

//---------------------------------------------------------------------------

int32_t
Debugger::sprintArrayValue(const std::wstring_view& dest, const std::unique_ptr<SymTableNode>& symbol, const std::wstring_view& subscriptString)
{
	//--------------------------------------------------------------------
	// This code is adapted from execVariable(). If that function changes,
	// this better too!
	//--------------------------------------------------------------------
	// First, point to the variable's stack item. If the variable's scope
	// level is less than the current scope level, follow the static links
	// to the proper stack frame base...
	if (symbol->defn.key == DFN_CONST)
		sprintf(dest, "\"%s\"", symbol->defn.info.constant.value.stringPtr);
	else
	{
		const std::unique_ptr<StackItem>& dataPtr = nullptr;
		switch (symbol->defn.info.data.varType)
		{
		case VAR_TYPE_NORMAL:
		{
			const std::unique_ptr<StackFrameHeader>& headerPtr = (const std::unique_ptr<StackFrameHeader>&)stackFrameBasePtr;
			int32_t delta = level - symbol->level;
			while (delta-- > 0)
				headerPtr = (const std::unique_ptr<StackFrameHeader>&)headerPtr->staticLink.address;
			dataPtr = (const std::unique_ptr<StackItem>&)headerPtr + symbol->defn.info.data.offset;
		}
		break;
		case VAR_TYPE_ETERNAL:
			dataPtr = (const std::unique_ptr<StackItem>&)stack + symbol->defn.info.data.offset;
			break;
		case VAR_TYPE_STATIC:
			dataPtr = (const std::unique_ptr<StackItem>&)StaticDataPtr + symbol->defn.info.data.offset;
			break;
		}
		const std::unique_ptr<Type>& ptype = (const std::unique_ptr<Type>&)(symbol->ptype);
		ABL_Assert(dataPtr != nullptr, 0, " Debugger.sprintArrayValue(): dataPtr is nullptr ");
		Address elementAddress = (Address)dataPtr->address;
		if (subscriptString)
		{
			const std::wstring_view& cp = subscriptString;
			//-----------------------------
			// Get past the open bracket...
			cp++;
			const std::wstring_view& token = strtok(&subscriptString[1], ",]");
			while (token)
			{
				//----------------
				// Read integer...
				int32_t index = atoi(token);
				//-------------------------
				// Range check the index...
				if ((index < 0) || (index >= ptype->info.array.elementCount))
					return (1);
				elementAddress += (index * ptype->info.array.elementTypePtr->size);
				ptype = ptype->info.array.elementTypePtr;
				token = strtok(nullptr, ",]");
			}
		}
		if ((ptype->form != FRM_ARRAY))
		{
			if ((ptype == IntegerTypePtr) || (ptype->form == FRM_ENUM))
				sprintf(dest, "%d", *((int32_t*)elementAddress));
			else if (ptype == CharTypePtr)
				sprintf(dest, "\"%c\"", *((const std::wstring_view&)elementAddress));
			else
				sprintf(dest, "%.4f", *((float*)elementAddress));
		}
		else if (ptype->info.array.elementTypePtr == CharTypePtr)
			sprintf(dest, "\"%s\"", (const std::wstring_view&)elementAddress);
		else
			sprintf(dest, "Could you be more specific?");
	}
	return (ABL_NO_ERR);
}

//---------------------------------------------------------------------------

int32_t
Debugger::sprintValue(const std::wstring_view& dest, const std::wstring_view& exprString)
{
	const std::wstring_view& subscript = strchr(exprString, '[');
	if (!subscript)
	{
		//------------------------------------------
		// Probably a simple variable or constant...
		const std::unique_ptr<SymTableNode>& symbol = debugModule->findSymbol(exprString, CurRoutineIdPtr);
		if (!symbol)
			return (1);
		if (symbol->ptype->form != FRM_ARRAY)
		{
			sprintSimpleValue(dest, symbol);
			return (ABL_NO_ERR);
		}
	}
	//-----------------
	// Must be array...
	if (subscript)
	{
		//--------------------------------
		// Looking for specific element...
		wchar_t subscriptString[255];
		strcpy(subscriptString, subscript);
		*subscript = nullptr;
		const std::unique_ptr<SymTableNode>& symbol = debugModule->findSymbol(exprString, CurRoutineIdPtr);
		if (!symbol)
			return (1);
		sprintArrayValue(dest, symbol, subscriptString);
	}
	else
	{
		//-------------------------
		// Print the whole thing...
		const std::unique_ptr<SymTableNode>& symbol = debugModule->findSymbol(exprString, CurRoutineIdPtr);
		if (!symbol)
			return (1);
		sprintArrayValue(dest, symbol, nullptr);
	}
	//-----------------------------------------------
	// We have the symbol, so parse the subscripts...
	return (ABL_NO_ERR);
}

//---------------------------------------------------------------------------

int32_t
Debugger::traceStatementExecution(void)
{
	bool halt = step;
	//--------------------------------------
	// Do we have a breakpoint on this line?
	if (breakPointManager)
	{
		if (breakPointManager->isBreakPoint(execLineNumber))
		{
			sprintf(message, "HIT BP: (%d) %s [%d]", module->getId(), module->getName(),
				execLineNumber);
			print(message);
			halt = true;
		}
	}
	//	if (trace) {
	//		message[0] = nullptr;
	//		sprintLineNumber(message);
	//		print(message);
	//	}
	//------------------------------------------------------------------
	// If we've hit a breakpoint or are currently halted due to
	// some other debugging thingy, wait for another debugger command...
	if (halt)
		debugMode();
	return (ABL_NO_ERR);
}

//---------------------------------------------------------------------------

int32_t
Debugger::traceRoutineEntry(const std::unique_ptr<SymTableNode>& idPtr)
{
	if (traceEntry)
	{
		sprintf(message, "ENTER (%d) %s:%s", module->getId(), module->getName(), idPtr->name);
		print(message);
	}
	return (ABL_NO_ERR);
}

//---------------------------------------------------------------------------

int32_t
Debugger::traceRoutineExit(const std::unique_ptr<SymTableNode>& idPtr)
{
	if (traceExit)
	{
		sprintf(message, "EXIT (%d) %s:%s", module->getId(), module->getName(), idPtr->name);
		print(message);
	}
	return (ABL_NO_ERR);
}

//---------------------------------------------------------------------------

int32_t
Debugger::traceDataStore(
	const std::unique_ptr<SymTableNode>& id, const std::unique_ptr<Type>& idType, const std::unique_ptr<StackItem>& target, const std::unique_ptr<Type>& targetType)
{
	// const std::unique_ptr<SymTableNode>& idPtr = debugModule->findSymbol(strParam1);
	// if (!idPtr) {
	//			print("Unknown identifier in current scope.\n");
	//			return;
	//		}
	//		wchar_t message[255];
	//		sprintSymbolValue(message, idPtr);
	//		print(message);
	//		}
	if (id->info && ((WatchPtr)id->info)->store)
	{
		wchar_t valString[255];
		sprintDataValue(valString, target, targetType);
		if (idType->form == FRM_ARRAY)
			sprintf(message, "STORE: (%d) %s [%d] -> %s[#] = %s\n", module->getId(),
				module->getName(), execLineNumber, id->name, valString);
		// else if (idType->form == FRM_RECORD)
		//	sprintf(message, "STORE AT LINE %d - %s.# = %s\n", execLineNumber,
		// id->name, valString);
		else
			sprintf(message, "STORE: (%d) %s [%d] -> %s = %s\n", module->getId(), module->getName(),
				execLineNumber, id->name, valString);
		print(message);
		if (((WatchPtr)id->info)->breakOnStore)
			debugMode();
	}
	return (ABL_NO_ERR);
}

//---------------------------------------------------------------------------

int32_t
Debugger::traceDataFetch(const std::unique_ptr<SymTableNode>& id, const std::unique_ptr<Type>& idType, const std::unique_ptr<StackItem>& data)
{
	const std::unique_ptr<Type>& idTypePtr = id->ptype;
	if (id->info && ((WatchPtr)id->info)->fetch)
	{
		wchar_t valString[255];
		sprintDataValue(valString, data, idType);
		if (idTypePtr->form == FRM_ARRAY)
			sprintf(message, "FETCH: (%d) %s [%d] - %s[#] = %s\n", module->getId(),
				module->getName(), execLineNumber, id->name, valString);
		// else if (idTypePtr->form == FRM_RECORD)
		//	sprintf(message, "STORE AT LINE %d - %s.# = %s\n", id->name,
		// valString);
		else
			sprintf(message, "FETCH: (%d) %s [%d] - %s = %s\n", module->getId(), module->getName(),
				execLineNumber, id->name, valString);
		print(message);
		if (((WatchPtr)id->info)->breakOnFetch)
			debugMode();
	}
	return (ABL_NO_ERR);
}

//---------------------------------------------------------------------------

void
Debugger::showValue(void)
{
	getToken();
	if (curToken == TKN_SEMICOLON)
		print("Bad Expression.\n");
	else
	{
		//------------------------------------------------------------
		// NOTE: Need a SOFT FATAL for parsing expressions here so the
		// debugger's errors don't Fatal out of the game!
		//------------------------------------------------------------
		//---------------------------------------------------------------
		// It's important that the expression parser return an error code
		// rather than fatal!
		const std::unique_ptr<Type>& ptype = expression();
		if (errorCount > 0)
			return;
		const std::wstring_view& savedCodeSegmentPtr = codeSegmentPtr;
		TokenCodeType savedCodeToken = codeToken;
		execExpression();
		if (ptype->form == FRM_ARRAY)
		{
			print("SHOW ARRAY\n");
		}
		else
		{
			wchar_t message[255];
			sprintDataValue(message, tos, ptype);
			strcat(message, "\n");
			print(message);
		}
		pop();
		codeSegmentPtr = savedCodeSegmentPtr;
		codeToken = savedCodeToken;
	}
}

//---------------------------------------------------------------------------

void
Debugger::assignVariable(void)
{
	getToken();
#if 0
	if(curToken == TKN_SEMICOLON)
		print("Need a variable.\n");
	else if(curToken == TKN_IDENTIFIER)
	{
		//----------------------------------
		// Parse the assignment statement...
		const std::unique_ptr<SymTableNode>& idPtr = nullptr;
		searchAndFindAllSymTables(idPtr);
		assigmentStatement(idPtr);
		if(errorCount > 0)
			return;
		//-------------------
		// Now, execute it...
		const std::wstring_view& savedCodeSegmentPtr = codeSegmentPtr;
		int32_t savedCodeToken = codeToken;
		codeSegmentPtr = codeBuffer + 1;
		getCodeToken();
		idPtr = getSymTableCodePtr();
		execAssignmentStatement(idPtr);
		//----------------------------
		// Restore the code segment...
		codeSegmentPtr = savedCodeSegmentPtr;
		codeToken = savedCodeToken;
	}
#endif
}

//---------------------------------------------------------------------------

void
Debugger::displayModuleInstanceRegistry(void)
{
	wchar_t buffer1[200], buffer2[40];
	for (size_t i = 0; i < ((NumModuleInstances + 1) / 2); i++)
	{
		sprintf(buffer1, "(%02d) %-20s ", ModuleInstanceRegistry[i * 2]->getId(),
			ModuleInstanceRegistry[i * 2]->getName());
		if ((i * 2 + 1) < NumModuleInstances)
		{
			sprintf(buffer2, "(%02d) %-20s ", ModuleInstanceRegistry[i * 2 + 1]->getId(),
				ModuleInstanceRegistry[i * 2 + 1]->getName());
			strcat(buffer1, buffer2);
		}
		print(buffer1);
	}
}

//---------------------------------------------------------------------------

void
Debugger::processCommand(
	int32_t commandId, const std::wstring_view& strParam1, int32_t numParam1, const std::unique_ptr<ABLModule>& moduleParam1)
{
	switch (commandId)
	{
	case DEBUG_COMMAND_SET_MODULE:
		if (moduleParam1)
		{
			debugModule = moduleParam1;
			print(" ");
			sprintf(message, "SET MODULE: %s", debugModule->getName());
			print(message);
		}
		else
		{
			print(" ");
			displayModuleInstanceRegistry();
			sprintf(message, "CURRENT MODULE: %s", debugModule->getName());
			print(message);
		}
		break;
	case DEBUG_COMMAND_TRACE:
		if (numParam1)
		{
			debugModule->setTrace(true);
			debugModule->setStep(false);
			if (module == debugModule)
			{
				trace = true;
				traceEntry = true;
				traceExit = true;
				step = false;
			}
		}
		else
		{
			debugModule->setTrace(false);
			if (module == debugModule)
			{
				trace = false;
				traceEntry = false;
				traceExit = false;
			}
		}
		break;
	case DEBUG_COMMAND_STEP:
		if (numParam1)
		{
			debugModule->setStep(true);
			debugModule->setTrace(false);
			if (module == debugModule)
			{
				step = true;
				trace = false;
				traceEntry = false;
				traceExit = false;
			}
		}
		else
		{
			debugModule->setStep(false);
			if (module == debugModule)
				step = false;
		}
		break;
	case DEBUG_COMMAND_BREAKPOINT_SET:
		print(" ");
		debugModule->getBreakPointManager()->add(numParam1);
		sprintf(message, "SET BP: %s (%d)", debugModule->getName(), numParam1);
		print(message);
		break;
	case DEBUG_COMMAND_BREAKPOINT_REMOVE:
		print(" ");
		debugModule->getBreakPointManager()->remove(numParam1);
		sprintf(message, "REMOVE BP: %s (%d)", debugModule->getName(), numParam1);
		print(message);
		break;
	case DEBUG_COMMAND_WATCH_SET:
	{
		print(" ");
		const std::unique_ptr<SymTableNode>& idPtr = debugModule->findSymbol(strParam1);
		if (!idPtr)
		{
			print("Unknown identifier in current scope.\n");
			return;
		}
		bool breakToDebug = ((numParam1 & WATCH_BREAK) != 0);
		if (numParam1 & WATCH_STORE_ON)
		{
			int32_t result = debugModule->getWatchManager()->setStore(idPtr, true, breakToDebug);
			if (result == 2)
			{
				print("Reached max watch limit--unable to set watch.\n");
				return;
			}
		}
		if (numParam1 & WATCH_FETCH_ON)
		{
			int32_t result = debugModule->getWatchManager()->setFetch(idPtr, true, breakToDebug);
			if (result == 2)
			{
				print("Reached max watch limit--unable to set watch.\n");
				return;
			}
		}
		if (numParam1 & WATCH_STORE_OFF)
			debugModule->getWatchManager()->setStore(idPtr, false, breakToDebug);
		if (numParam1 & WATCH_FETCH_OFF)
			debugModule->getWatchManager()->setFetch(idPtr, false, breakToDebug);
		bool store = debugModule->getWatchManager()->getStore(idPtr);
		bool fetch = debugModule->getWatchManager()->getFetch(idPtr);
		if (store || fetch)
		{
			sprintf(message, "SET WATCH: %s.%s (", debugModule->getName(), strParam1);
			if (store)
				strcat(message, "s");
			if (fetch)
				strcat(message, "f");
			strcat(message, ")");
		}
		else
			sprintf(message, "REMOVE WATCH: %s.%s", debugModule->getName(), strParam1);
		print(message);
	}
	break;
	case DEBUG_COMMAND_WATCH_REMOVE_ALL:
		debugModule->getWatchManager()->removeAll();
		break;
	case DEBUG_COMMAND_PRINT:
	{
		print(" ");
		int32_t err = sprintValue(message, strParam1);
		switch (err)
		{
		case ABL_NO_ERR:
			print(message);
			break;
		case 1:
			print("Unknown identifier in current scope.");
			break;
		}
	}
	break;
	case DEBUG_COMMAND_CONTINUE:
		debugCommand = false;
		break;
	case DEBUG_COMMAND_HELP:
		print(" ");
		print("b{+|-} <line#>         set/remove breakpt");
		print("m [0 thru warrior #]   set current module (or list them)");
		print("w[f|s]{+|-}{.} <variable> set/remove variable watch (fetch & "
			  "store)");
		print("p <variable>           display current value of variable");
		print("s{+|-}                 start/stop step mode");
		print("t{+|-}                 start/stop trace mode");
		print("??                     current module info");
		print("?                      help");
		break;
	case DEBUG_COMMAND_INFO:
	{
		print(" ");
		sprintf(message, "CURRENT MODULE: %s", debugModule->getName());
		print(message);
		ModuleInfo moduleInfo;
		debugModule->getInfo(&moduleInfo);
		sprintf(message, "%d static vars, %d bytes, %d largest", moduleInfo.numStaticVars,
			moduleInfo.totalSizeStaticVars, moduleInfo.largestStaticVar);
		print(message);
	}
	break;
	}
}

//---------------------------------------------------------------------------

void
Debugger::debugMode(void)
{
	//--------------------------------------------------------------------
	// Some assumptions: this will never be called during a smacker movie.
	//-------------------------------------------------------------------------------
	// When we enter debug mode, we grab control of all message handling from
	// windows until we exit ABL debug mode. This way, the ABL system is
	// preserved for debugging...
	debugModule = module;
	message[0] = nullptr;
	sprintStatement(message);
	print(message);
#ifdef USE_IFACE
	debugCommand = true;
	while (debugCommand)
	{
		//----------------------------------------------------------
		// Since MSWindows is locked out, we must Unlock the screen,
		// peek for MSWindows messages, then relock it.
		QueryPerformanceCounter((LARGE_INTEGER*)&startTime);
		bool result;
		do
		{
			MSG msg;
			result = PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE);
			if (result)
			{
				if (msg.message == WM_QUIT)
				{
					debugCommand = false;
					halt = false;
					trace = false;
					step = false;
					traceEntry = false;
					traceExit = false;
					break;
				}
				else
				{
					TranslateMessage(&msg);
					DispatchMessage(&msg);
				}
			}
		} while (result);
		// make sure we lock the screen for a valid ptr before we go to the user
		// routines.
		if (applicationActive)
		{
			UpdateDisplay(takeScreenShot);
			takeScreenShot = false;
			QueryPerformanceCounter((LARGE_INTEGER*)&stopTime);
			if (MPlayer)
			{
				// WaitForSingleObject(ReceiveMutex, INFINITE);
				// WaitForSingleObject(FreeListMutex, INFINITE);
				MPlayer->processReceiveList();
				// ReleaseMutex(FreeListMutex);
				// ReleaseMutex(ReceiveMutex);
			}
			// Insert our message loop here....
			// and jump to the users idle
			//			.
			//			.
			//			.
			//
			//
			//
			// finished with our user loop...
			CheckMouse();
		}
		prevStart = startTime;
		L_INTEGER totalTime = stopTime - startTime;
		frameRate = (float)countsPerSecond / (float)totalTime;
	}
#endif
}

//---------------------------------------------------------------------------

const std::unique_ptr<Debugger>&
ABLi_getDebugger(void)
{
	return (debugger);
}

//***************************************************************************

} // namespace mclib::abl

