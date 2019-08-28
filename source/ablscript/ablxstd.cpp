//===========================================================================//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//
//***************************************************************************
//
//								ABLXSTD.CPP
//
//***************************************************************************

#include "stdinc.h"

#include "abl.h"

namespace mclib::abl {

//***************************************************************************

inline int32_t double2long(double _in)
{
	_in += 6755399441055744.0;
	return (*(int32_t*)&_in);
}

//----------
// EXTERNALS

extern int32_t level;
extern int32_t execLineNumber;
extern int32_t FileNumber;
extern const std::wstring_view& codeSegmentPtr;
extern TokenCodeType codeToken;
extern StackItem* stack;
extern const std::unique_ptr<StackItem>& tos;
extern const std::unique_ptr<StackItem>& stackFrameBasePtr;
extern const std::unique_ptr<SymTableNode>& CurRoutineIdPtr;
extern int32_t CurModuleHandle;
extern const std::unique_ptr<Type>& IntegerTypePtr;
extern const std::unique_ptr<Type>& RealTypePtr;
extern const std::unique_ptr<Type>& BooleanTypePtr;
extern const std::unique_ptr<Type>& CharTypePtr;
extern const std::unique_ptr<ABLModule>& CurModule;
extern const std::unique_ptr<ABLModule>& CurFSM;
extern int32_t MaxLoopIterations;
extern const std::unique_ptr<Debugger>& debugger;
extern bool NewStateSet;

//--------
// GLOBALS

StackItem returnValue;
bool eofFlag = false;
bool ExitWithReturn = false;
bool ExitFromTacOrder = false;
bool SkipOrder = false;
TokenCodeType ExitRoutineCodeSegment[2] = {TKN_END_FUNCTION, TKN_SEMICOLON};
TokenCodeType ExitOrderCodeSegment[2] = {TKN_END_ORDER, TKN_SEMICOLON};
TokenCodeType ExitStateCodeSegment[2] = {TKN_END_STATE, TKN_SEMICOLON};

//***************************************************************************
// USEFUL ABL HELP ROUTINES
//***************************************************************************

wchar_t
ABLi_popChar(void)
{
	getCodeToken();
	execExpression();
	wchar_t val = (wchar_t)tos->integer;
	pop();
	return (val);
}

//---------------------------------------------------------------------------

int32_t
ABLi_popInteger(void)
{
	getCodeToken();
	execExpression();
	int32_t val = tos->integer;
	pop();
	return (val);
}

//---------------------------------------------------------------------------

float
ABLi_popReal(void)
{
	getCodeToken();
	execExpression();
	float val = tos->real;
	pop();
	return (val);
}

//---------------------------------------------------------------------------

float
ABLi_popIntegerReal(void)
{
	getCodeToken();
	const std::unique_ptr<Type>& paramTypePtr = execExpression();
	float val = 0.0;
	if (paramTypePtr == IntegerTypePtr)
		val = (float)tos->integer;
	else
		val = tos->real;
	pop();
	return (val);
}

//---------------------------------------------------------------------------

bool
ABLi_popBoolean(void)
{
	getCodeToken();
	execExpression();
	int32_t val = tos->integer;
	pop();
	return (val == 1);
}

//---------------------------------------------------------------------------

const std::wstring_view&
ABLi_popCharPtr(void)
{
	//--------------------------
	// Get destination string...
	getCodeToken();
	execExpression();
	const std::wstring_view& charPtr = (const std::wstring_view&)tos->address;
	pop();
	return (charPtr);
}

//---------------------------------------------------------------------------

int32_t*
ABLi_popIntegerPtr(void)
{
	getCodeToken();
	const std::unique_ptr<SymTableNode>& idPtr = getCodeSymTableNodePtr();
	execVariable(idPtr, USE_REFPARAM);
	int32_t* integerPtr = (int32_t*)(&((const std::unique_ptr<StackItem>&)tos->address)->integer);
	pop();
	return (integerPtr);
}

//---------------------------------------------------------------------------

float*
ABLi_popRealPtr(void)
{
	getCodeToken();
	const std::unique_ptr<SymTableNode>& idPtr = getCodeSymTableNodePtr();
	execVariable(idPtr, USE_REFPARAM);
	float* realPtr = (float*)(&((const std::unique_ptr<StackItem>&)tos->address)->real);
	pop();
	return (realPtr);
}

//---------------------------------------------------------------------------

const std::wstring_view&
ABLi_popBooleanPtr(void)
{
	//--------------------------
	// Get destination string...
	getCodeToken();
	execExpression();
	const std::wstring_view& charPtr = (const std::wstring_view&)tos->address;
	pop();
	return (charPtr);
}

//---------------------------------------------------------------------------

int32_t
ABLi_popAnything(ABLStackItem* value)
{
	getCodeToken();
	const std::unique_ptr<Type>& paramTypePtr = execExpression();
	int32_t type = -1;
	if (paramTypePtr == IntegerTypePtr)
	{
		value->type = type = ABL_STACKITEM_INTEGER;
		value->data.integer = tos->integer;
	}
	else if (paramTypePtr == BooleanTypePtr)
	{
		value->type = type = ABL_STACKITEM_BOOLEAN;
		value->data.boolean = (tos->integer ? true : false);
	}
	else if (paramTypePtr == CharTypePtr)
	{
		value->type = type = ABL_STACKITEM_CHAR;
		value->data.character = tos->byte;
	}
	else if (paramTypePtr == RealTypePtr)
	{
		value->type = type = ABL_STACKITEM_REAL;
		value->data.real = tos->real;
	}
	else if (paramTypePtr->form == FRM_ARRAY)
	{
		if (paramTypePtr->info.array.elementTypePtr == CharTypePtr)
		{
			value->type = type = ABL_STACKITEM_CHAR_PTR;
			value->data.characterPtr = (const std::wstring_view&)tos->address;
		}
		else if (paramTypePtr->info.array.elementTypePtr == IntegerTypePtr)
		{
			value->type = type = ABL_STACKITEM_INTEGER_PTR;
			value->data.integerPtr = (int32_t*)tos->address;
		}
		else if (paramTypePtr->info.array.elementTypePtr == RealTypePtr)
		{
			value->type = type = ABL_STACKITEM_REAL_PTR;
			value->data.realPtr = (float*)tos->address;
		}
		else if (paramTypePtr->info.array.elementTypePtr == BooleanTypePtr)
		{
			value->type = type = ABL_STACKITEM_BOOLEAN_PTR;
			value->data.booleanPtr = (bool*)tos->address;
		}
	}
	pop();
	return (type);
}

//---------------------------------------------------------------------------

void
ABLi_pushBoolean(bool value)
{
	const std::unique_ptr<StackItem>& valuePtr = ++tos;
	if (valuePtr >= &stack[MAXSIZE_STACK])
		runtimeError(ABL_ERR_RUNTIME_STACK_OVERFLOW);
	valuePtr->integer = value ? 1 : 0;
}

//---------------------------------------------------------------------------

void
ABLi_pushInteger(int32_t value)
{
	const std::unique_ptr<StackItem>& valuePtr = ++tos;
	if (valuePtr >= &stack[MAXSIZE_STACK])
		runtimeError(ABL_ERR_RUNTIME_STACK_OVERFLOW);
	valuePtr->integer = value;
}

//---------------------------------------------------------------------------

void
ABLi_pushReal(float value)
{
	const std::unique_ptr<StackItem>& valuePtr = ++tos;
	if (valuePtr >= &stack[MAXSIZE_STACK])
		runtimeError(ABL_ERR_RUNTIME_STACK_OVERFLOW);
	valuePtr->real = value;
}

//---------------------------------------------------------------------------

void
ABLi_pushChar(wchar_t value)
{
	const std::unique_ptr<StackItem>& valuePtr = ++tos;
	if (valuePtr >= &stack[MAXSIZE_STACK])
		runtimeError(ABL_ERR_RUNTIME_STACK_OVERFLOW);
	valuePtr->integer = value;
}

//---------------------------------------------------------------------------

int32_t
ABLi_peekInteger(void)
{
	getCodeToken();
	execExpression();
	return (tos->integer);
}

//---------------------------------------------------------------------------

float
ABLi_peekReal(void)
{
	getCodeToken();
	execExpression();
	return (tos->real);
}

//---------------------------------------------------------------------------

bool
ABLi_peekBoolean(void)
{
	getCodeToken();
	execExpression();
	return (tos->integer == 1);
}

//---------------------------------------------------------------------------

const std::wstring_view&
ABLi_peekCharPtr(void)
{
	getCodeToken();
	execExpression();
	return ((const std::wstring_view&)tos->address);
}

//---------------------------------------------------------------------------

int32_t*
ABLi_peekIntegerPtr(void)
{
	getCodeToken();
	const std::unique_ptr<SymTableNode>& idPtr = getCodeSymTableNodePtr();
	execVariable(idPtr, USE_REFPARAM);
	return ((int32_t*)(&((const std::unique_ptr<StackItem>&)tos->address)->integer));
}

//---------------------------------------------------------------------------

float*
ABLi_peekRealPtr(void)
{
	getCodeToken();
	const std::unique_ptr<SymTableNode>& idPtr = getCodeSymTableNodePtr();
	execVariable(idPtr, USE_REFPARAM);
	return ((float*)(&((const std::unique_ptr<StackItem>&)tos->address)->real));
}

//---------------------------------------------------------------------------

void
ABLi_pokeChar(int32_t val)
{
	tos->integer = val;
}

//---------------------------------------------------------------------------

void
ABLi_pokeInteger(int32_t val)
{
	tos->integer = val;
}

//---------------------------------------------------------------------------

void
ABLi_pokeReal(float val)
{
	tos->real = val;
}

//---------------------------------------------------------------------------

void
ABLi_pokeBoolean(bool val)
{
	tos->integer = val ? 1 : 0;
}

//***************************************************************************

void
execOrderReturn(int32_t returnVal)
{
	//-----------------------------
	// Assignment to function id...
	const std::unique_ptr<StackFrameHeader>& headerPtr = (const std::unique_ptr<StackFrameHeader>&)stackFrameBasePtr;
	int32_t delta = level - CurRoutineIdPtr->level - 1;
	while (delta-- > 0)
		headerPtr = (const std::unique_ptr<StackFrameHeader>&)headerPtr->staticLink.address;
	if (CurRoutineIdPtr->defn.info.routine.flags & ROUTINE_FLAG_STATE)
	{
		//----------------------------------
		// Return in a state function, so...
		if (debugger)
			debugger->traceDataStore(CurRoutineIdPtr, CurRoutineIdPtr->ptype,
				(const std::unique_ptr<StackItem>&)headerPtr, CurRoutineIdPtr->ptype);
		ExitWithReturn = true;
		ExitFromTacOrder = true;
		if (returnVal == 0)
		{
			//----------------------------------------------------------
			// Use the "eject" code only if called for a failed Order...
			codeSegmentPtr = (const std::wstring_view&)ExitStateCodeSegment;
			getCodeToken();
		}
	}
	else
	{
		//-------------------------------------------------------------------------
		// All Order functions (TacticalOrder/GeneralOrder/ActionOrder) must
		// return an integer error code, so we assume the return type is
		// IntegerTypePtr...
		const std::unique_ptr<StackItem>& targetPtr = (const std::unique_ptr<StackItem>&)headerPtr;
		targetPtr->integer = returnVal;
		//----------------------------------------------------------------------
		// Preserve the return value, in case we need it for the calling user...
		memcpy(&returnValue, targetPtr, sizeof(StackItem));
		if (debugger)
			debugger->traceDataStore(CurRoutineIdPtr, CurRoutineIdPtr->ptype,
				(const std::unique_ptr<StackItem>&)headerPtr, CurRoutineIdPtr->ptype);
		ExitWithReturn = true;
		ExitFromTacOrder = true;
		if (returnVal == 0)
		{
			//----------------------------------------------------------
			// Use the "eject" code only if called for a failed Order...
			codeSegmentPtr = (const std::wstring_view&)ExitOrderCodeSegment;
			getCodeToken();
		}
	}
}

//***************************************************************************

void
execStdReturn(void)
{
	memset(&returnValue, 0, sizeof(StackItem));
	if (CurRoutineIdPtr->ptype)
	{
		//-----------------------------
		// Assignment to function id...
		const std::unique_ptr<StackFrameHeader>& headerPtr = (const std::unique_ptr<StackFrameHeader>&)stackFrameBasePtr;
		int32_t delta = level - CurRoutineIdPtr->level - 1;
		while (delta-- > 0)
			headerPtr = (const std::unique_ptr<StackFrameHeader>&)headerPtr->staticLink.address;
		const std::unique_ptr<StackItem>& targetPtr = (const std::unique_ptr<StackItem>&)headerPtr;
		const std::unique_ptr<Type>& targetTypePtr = (const std::unique_ptr<Type>&)(CurRoutineIdPtr->ptype);
		getCodeToken();
		//---------------------------------------------------------------
		// Routine execExpression() leaves the expression value on top of
		// stack...
		getCodeToken();
		const std::unique_ptr<Type>& expressionTypePtr = execExpression();
		//--------------------------
		// Now, do the assignment...
		if ((targetTypePtr == RealTypePtr) && (expressionTypePtr == IntegerTypePtr))
		{
			//-------------------------
			// integer assigned to real
			targetPtr->real = (float)(tos->integer);
		}
		else if (targetTypePtr->form == FRM_ARRAY)
		{
			//-------------------------
			// Copy the array/record...
			const std::wstring_view& dest = (const std::wstring_view&)targetPtr;
			const std::wstring_view& src = tos->address;
			int32_t size = targetTypePtr->size;
			memcpy(dest, src, size);
		}
		else if ((targetTypePtr == IntegerTypePtr) || (targetTypePtr->form == FRM_ENUM))
		{
			//------------------------------------------------------
			// Range check assignment to integer or enum subrange...
			targetPtr->integer = tos->integer;
		}
		else
		{
			//-----------------------
			// Assign real to real...
			targetPtr->real = tos->real;
		}
		//-----------------------------
		// Grab the expression value...
		pop();
		//----------------------------------------------------------------------
		// Preserve the return value, in case we need it for the calling user...
		memcpy(&returnValue, targetPtr, sizeof(StackItem));
		if (debugger)
			debugger->traceDataStore(
				CurRoutineIdPtr, CurRoutineIdPtr->ptype, targetPtr, targetTypePtr);
	}
	//-----------------------
	// Grab the semi-colon...
	getCodeToken();
	if (CurRoutineIdPtr->defn.info.routine.flags & ROUTINE_FLAG_ORDER)
		codeSegmentPtr = (const std::wstring_view&)ExitOrderCodeSegment;
	else if (CurRoutineIdPtr->defn.info.routine.flags & ROUTINE_FLAG_STATE)
		codeSegmentPtr = (const std::wstring_view&)ExitStateCodeSegment;
	else
		codeSegmentPtr = (const std::wstring_view&)ExitRoutineCodeSegment;
	ExitWithReturn = true;
	getCodeToken();
}

//***************************************************************************

void
execStdPrint(void)
{
	//---------------------------
	// Grab the opening LPAREN...
	getCodeToken();
	//----------------------------
	// Get parameter expression...
	getCodeToken();
	const std::unique_ptr<Type>& paramTypePtr = execExpression();
	wchar_t buffer[20];
	const std::wstring_view& s = buffer;
	if (paramTypePtr == IntegerTypePtr)
		sprintf(buffer, "%d", tos->integer);
	else if (paramTypePtr == BooleanTypePtr)
		sprintf(buffer, "%s", tos->integer ? "true" : "false");
	else if (paramTypePtr == CharTypePtr)
		sprintf(buffer, "%c", tos->byte);
	else if (paramTypePtr == RealTypePtr)
		sprintf(buffer, "%.4f", tos->real);
	else if ((paramTypePtr->form == FRM_ARRAY) && (paramTypePtr->info.array.elementTypePtr == CharTypePtr))
		s = (const std::wstring_view&)tos->address;
	pop();
	if (debugger)
	{
		wchar_t message[512];
		sprintf(message, "PRINT:  \"%s\"", s);
		debugger->print(message);
		sprintf(message, "   MODULE %s", CurModule->getName());
		debugger->print(message);
		sprintf(message, "   FILE %s", CurModule->getSourceFile(FileNumber));
		debugger->print(message);
		sprintf(message, "   LINE %d", execLineNumber);
		debugger->print(message);
	}
	/*	else if (TACMAP) {
			aChatWindow* chatWin = TACMAP->getChatWindow();
			if (chatWin)
				chatWin->processChatString(0, s, -1);
			else {
	#ifdef _DEBUG
				OutputDebugString(s);
	#endif
			}
			}
	*/
	else
	{
#ifdef _DEBUG
		ABLDebugPrintCallback(s);
#endif
	}
	//-----------------------
	// Grab closing RPAREN...
	getCodeToken();
}

//***************************************************************************

const std::unique_ptr<Type>&
execStdConcat(void)
{
	//-------------------
	// Grab the LPAREN...
	getCodeToken();
	//--------------------------
	// Get destination string...
	getCodeToken();
	execExpression();
	const std::wstring_view& dest = (const std::wstring_view&)tos->address;
	pop();
	//----------------------
	// Get item to append...
	getCodeToken();
	const std::unique_ptr<Type>& paramTypePtr = execExpression();
	wchar_t buffer[20];
	if (paramTypePtr == IntegerTypePtr)
	{
		sprintf(buffer, "%d", tos->integer);
		strcat(dest, buffer);
	}
	else if (paramTypePtr == CharTypePtr)
	{
		sprintf(buffer, "%c", tos->byte);
		strcat(dest, buffer);
	}
	else if (paramTypePtr == RealTypePtr)
	{
		sprintf(buffer, "%.2f", tos->real);
		strcat(dest, buffer);
	}
	else if (paramTypePtr == BooleanTypePtr)
	{
		sprintf(buffer, "%s", tos->integer ? "true" : "false");
		strcat(dest, buffer);
	}
	else if ((paramTypePtr->form == FRM_ARRAY) && (paramTypePtr->info.array.elementTypePtr == CharTypePtr))
		strcat(dest, (const std::wstring_view&)tos->address);
	tos->integer = 0;
	getCodeToken();
	return (IntegerTypePtr);
}

//***************************************************************************

void
execStdAbs(void)
{
	float val = ABLi_popIntegerReal();
	if (val < 0.0)
		val = -val;
	ABLi_pushReal(val);
}

//*****************************************************************************

void
execStdRound(void)
{
	float val = ABLi_popReal();
	if (val > 0.0)
		ABLi_pushInteger((int32_t)(val + 0.5));
	else
		ABLi_pushInteger((int32_t)(val - 0.5));
}

//***************************************************************************

void
execStdSqrt(void)
{
	float val = ABLi_popIntegerReal();
	if (val < 0.0)
		runtimeError(ABL_ERR_RUNTIME_INVALID_FUNCTION_ARGUMENT);
	else
		ABLi_pushReal((float)sqrt(val));
}

//***************************************************************************

void
execStdTrunc(void)
{
	float val = ABLi_popReal();
	ABLi_pushInteger((int32_t)val);
}

//***************************************************************************

void
execStdFileOpen(void)
{
	const std::wstring_view& fileName = ABLi_popCharPtr();
	int32_t fileHandle = -1;
	UserFile* userFile = UserFile::getNewFile();
	if (userFile)
	{
		int32_t err = userFile->open(fileName);
		if (!err)
			fileHandle = userFile->handle;
	}
	ABLi_pushInteger(fileHandle);
}

//---------------------------------------------------------------------------

void
execStdFileWrite(void)
{
	int32_t fileHandle = ABLi_popInteger();
	const std::wstring_view& string = ABLi_popCharPtr();
	UserFile* userFile = UserFile::files[fileHandle];
	if (userFile->inUse)
		userFile->write(string);
}

//---------------------------------------------------------------------------

void
execStdFileClose(void)
{
	int32_t fileHandle = ABLi_popInteger();
	UserFile* userFile = UserFile::files[fileHandle];
	if (userFile->inUse)
		userFile->close();
}

//***************************************************************************

void
execStdGetModule(void)
{
	//----------------------------------------------------------
	// Return the handle of the current module being executed...
	const std::wstring_view& curBuffer = ABLi_popCharPtr();
	const std::wstring_view& fsmBuffer = ABLi_popCharPtr();
	strcpy(curBuffer, CurModule->getFileName());
	strcpy(fsmBuffer, CurFSM ? CurFSM->getFileName() : "none");
	ABLi_pushInteger(CurModuleHandle);
}

//***************************************************************************

void
execStdSetMaxLoops(void)
{
	//----------------------------------------------------------------------
	//
	//	SET MAX LOOPS function:
	//
	//		Sets the max number of loops that may occur (in a for, while or
	//		repeat loop) before an infinite loop run-time error occurs.
	//
	//		PARAMS:	integer
	//
	//		RETURN: none
	//
	//----------------------------------------------------------------------
	MaxLoopIterations = ABLi_popInteger() + 1;
}

//---------------------------------------------------------------------------

void
execStdFatal(void)
{
	//----------------------------------------------------------------------
	//
	//	FATAL function:
	//
	//		If the debugger is active, this immediately jumps into debug mode.
	//		Otherwise, it causes a fatal and exits the game (displaying the
	//		string passed in).
	//
	//		PARAMS:	integer							fatal code to display
	//
	//				wchar_t[]							message
	//
	//		RETURN: none
	//
	//----------------------------------------------------------------------
	int32_t code = ABLi_popInteger();
	const std::wstring_view& s = ABLi_popCharPtr();
	wchar_t message[512];
	if (debugger)
	{
		sprintf(message, "FATAL:  [%d] \"%s\"", code, s);
		debugger->print(message);
		sprintf(message, "   MODULE (%d) %s", CurModule->getId(), CurModule->getName());
		debugger->print(message);
		sprintf(message, "   FILE %s", CurModule->getSourceFile(FileNumber));
		debugger->print(message);
		sprintf(message, "   LINE %d", execLineNumber);
		debugger->print(message);
		debugger->debugMode();
	}
	else
	{
		sprintf(message, "ABL FATAL: [%d] %s", code, s);
		ABL_Fatal(0, s);
	}
}

//---------------------------------------------------------------------------

void
execStdAssert(void)
{
	//----------------------------------------------------------------------
	//
	//	ASSERT function:
	//
	//		If the debugger is active, this immediately jumps into debug mode
	//		if expression is FALSE. Otherwise, the _ASSERT statement is ignored
	//		unless the #debug directive has been issued in the module. If
	//		so, a fatal occurs and exits the game (displaying the
	//		string passed in).
	//
	//		PARAMS:	boolean							expression
	//
	//				integer							_ASSERT code to display
	//
	//				wchar_t[]							message
	//
	//		RETURN: none
	//
	//----------------------------------------------------------------------
	int32_t expression = ABLi_popInteger();
	int32_t code = ABLi_popInteger();
	const std::wstring_view& s = ABLi_popCharPtr();
	if (!expression)
	{
		wchar_t message[512];
		if (debugger)
		{
			sprintf(message, "ASSERT:  [%d] \"%s\"", code, s);
			debugger->print(message);
			sprintf(message, "   MODULE (%d) %s", CurModule->getId(), CurModule->getName());
			debugger->print(message);
			sprintf(message, "   FILE %s", CurModule->getSourceFile(FileNumber));
			debugger->print(message);
			sprintf(message, "   LINE %d", execLineNumber);
			debugger->print(message);
			debugger->debugMode();
		}
		else
		{
			sprintf(message, "ABL ASSERT: [%d] %s", code, s);
			ABL_Fatal(0, message);
		}
	}
}

//-----------------------------------------------------------------------------

void
execStdRandom(void)
{
	int32_t n = ABLi_peekInteger();
	//---------------------------------------------------------------------
	// This is, like, a really bad number generator. But, you get the idea.
	// Once we know which pseudo-random number algorithm we want to use, we
	// should plug it in here. The range for the random number (r), given a
	// param of (n), should be: 0 <= r <= (n-1).
	ABLi_pokeInteger(ABLRandomCallback(n));
}

//-----------------------------------------------------------------------------

void
execStdSeedRandom(void)
{
	int32_t seed = ABLi_popInteger();
	if (seed == -1)
		ABLSeedRandomCallback(time(nullptr));
	else
		ABLSeedRandomCallback(seed);
}

//-----------------------------------------------------------------------------

void
execStdResetOrders(void)
{
	int32_t scope = ABLi_popInteger();
	if (scope == 0)
		CurModule->resetOrderCallFlags();
	else if (scope == 1)
	{
		int32_t startIndex = CurRoutineIdPtr->defn.info.routine.orderCallIndex;
		int32_t endIndex = startIndex + CurRoutineIdPtr->defn.info.routine.numOrderCalls;
		for (size_t i = startIndex; i < endIndex; i++)
		{
			uint8_t orderDWord = (uint8_t)(i / 32);
			uint8_t orderBitMask = (uint8_t)(i % 32);
			CurModule->clearOrderCallFlag(orderDWord, orderBitMask);
		}
	}
}

//---------------------------------------------------------------------------

void
execStdGetStateHandle(void)
{
	const std::wstring_view& name = ABLi_popCharPtr();
	int32_t stateHandle = CurFSM->findStateHandle(_strlwr(name));
	ABLi_pushInteger(stateHandle);
}

//---------------------------------------------------------------------------

void
execStdGetCurrentStateHandle(void)
{
	int32_t stateHandle = CurFSM->getStateHandle();
	ABLi_pushInteger(stateHandle);
}

//---------------------------------------------------------------------------

extern const std::unique_ptr<ModuleEntry>& ModuleRegistry;

extern wchar_t SetStateDebugStr[256];

void
execStdSetState(void)
{
	uint32_t stateHandle = ABLi_popInteger();
	if (stateHandle > 0)
	{
		const std::unique_ptr<SymTableNode>& stateFunction =
			ModuleRegistry[CurFSM->getHandle()].stateHandles[stateHandle].state;
		CurFSM->setPrevState(CurFSM->getState());
		CurFSM->setState(stateFunction);
		sprintf(SetStateDebugStr, "%s:%s, line %d", CurFSM->getFileName(), stateFunction->name,
			execLineNumber);
		NewStateSet = true;
	}
}

//---------------------------------------------------------------------------

void
execStdGetFunctionHandle(void)
{
	const std::wstring_view& name = ABLi_popCharPtr();
	const std::unique_ptr<SymTableNode>& function = CurModule->findFunction(name, false);
	if (function)
		ABLi_pushInteger((uint32_t)function);
	else
		ABLi_pushInteger(0);
}

//---------------------------------------------------------------------------

void
execStdSetFlag(void)
{
	uint32_t bits = (uint32_t)ABLi_popInteger();
	uint32_t flag = (uint32_t)ABLi_popInteger();
	bool set = ABLi_popBoolean();
	bits &= (flag ^ 0xFFFFFFFF);
	if (set)
		bits |= flag;
	ABLi_pushInteger(bits);
}

//---------------------------------------------------------------------------

void
execStdGetFlag(void)
{
	uint32_t bits = (uint32_t)ABLi_popInteger();
	uint32_t flag = (uint32_t)ABLi_popInteger();
	bool set = ((bits & flag) != 0);
	ABLi_pushInteger(set);
}

//---------------------------------------------------------------------------

void
execStdCallFunction(void)
{
	uint32_t address = ABLi_popInteger();
	if (address)
	{
		// GLENN: Not functional, yet...
	}
}

//***************************************************************************

void
initStandardRoutines(void)
{
	//-------------------------------------------------------------
	// Fatal and Assert will have hardcoded keys so we can look for
	// 'em in the rest of the ABL code (example: ignore asserts if
	// the assert_off option has been set).
	enterStandardRoutine("fatal", RTN_FATAL, false, "iC", nullptr, execStdFatal);
	enterStandardRoutine("_ASSERT", RTN_ASSERT, false, "biC", nullptr, execStdAssert);
	enterStandardRoutine(
		"getstatehandle", RTN_GET_STATE_HANDLE, false, "C", "i", execStdGetStateHandle);
	enterStandardRoutine(
		"getcurrentstatehandle", -1, false, nullptr, "i", execStdGetCurrentStateHandle);
	enterStandardRoutine("abs", -1, false, "*", "r", execStdAbs);
	enterStandardRoutine("sqrt", -1, false, "*", "r", execStdSqrt);
	enterStandardRoutine("round", -1, false, "r", "i", execStdRound);
	enterStandardRoutine("trunc", -1, false, "r", "i", execStdTrunc);
	enterStandardRoutine("random", -1, false, "i", "i", execStdRandom);
	enterStandardRoutine("seedrandom", -1, false, "i", nullptr, execStdSeedRandom);
	enterStandardRoutine("setmaxloops", -1, false, "i", nullptr, execStdSetMaxLoops);
	enterStandardRoutine("fileopen", -1, false, "C", "i", execStdFileOpen);
	enterStandardRoutine("filewrite", -1, false, "iC", nullptr, execStdFileWrite);
	enterStandardRoutine("fileclose", -1, false, "i", nullptr, execStdFileClose);
	enterStandardRoutine("getmodule", -1, false, "CC", "i", execStdGetModule);
	enterStandardRoutine("resetorders", -1, false, "i", nullptr, execStdResetOrders);
	enterStandardRoutine("setstate", -1, false, "i", nullptr, execStdSetState);
	enterStandardRoutine("getfunctionhandle", -1, false, "C", "i", execStdGetFunctionHandle);
	enterStandardRoutine("callfunction", -1, false, "i", nullptr, execStdCallFunction);
	enterStandardRoutine("setflag", -1, false, "iib", "i", execStdSetFlag);
	enterStandardRoutine("getflag", -1, false, "ii", "b", execStdGetFlag);
}

//-----------------------------------------------------------------------------

const std::unique_ptr<Type>&
execStandardRoutineCall(const std::unique_ptr<SymTableNode>& routineIdPtr, bool skipOrder)
{
	int32_t key = routineIdPtr->defn.info.routine.key;
	switch (key)
	{
	case RTN_RETURN:
		execStdReturn();
		return (nullptr);
	case RTN_PRINT:
		execStdPrint();
		return (nullptr);
	case RTN_CONCAT:
		return (execStdConcat());
	default:
	{
		if (key >= NumStandardFunctions)
		{
			wchar_t err[255];
			sprintf(err, " ABL: Undefined ABL RoutineKey in %s:%d", CurModule->getName(),
				execLineNumber);
			ABL_Fatal(0, err);
		}
		if (FunctionInfoTable[key].numParams > 0)
			getCodeToken();
		SkipOrder = skipOrder;
		if (FunctionCallbackTable[key])
			(*FunctionCallbackTable[key])();
		else
		{
			wchar_t err[255];
			sprintf(err, " ABL: Undefined ABL RoutineKey %d in %s:%d", key, CurModule->getName(),
				execLineNumber);
			ABL_Fatal(key, err);
		}
		getCodeToken();
		switch (FunctionInfoTable[key].returnType)
		{
		case RETURN_TYPE_NONE:
			return (nullptr);
		case RETURN_TYPE_INTEGER:
			return (IntegerTypePtr);
		case RETURN_TYPE_REAL:
			return (RealTypePtr);
		case RETURN_TYPE_BOOLEAN:
			return (BooleanTypePtr);
		}
	}
	}
	return (nullptr);
}

//***************************************************************************

} // namespace mclib::abl
