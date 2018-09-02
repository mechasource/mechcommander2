//===========================================================================//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//
//***************************************************************************
//
//								ABLEXEC.CPP
//
//***************************************************************************
#include "stdinc.h"

//#include <stdio.h>
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

#ifndef ABLDBUG_H
#include "abldbug.h"
#endif

//***************************************************************************

//----------
// EXTERNALS

extern int32_t level;
extern int32_t CallStackLevel;
extern int32_t execLineNumber;
extern int32_t execStatementCount;
extern PSTR codeSegmentPtr;
extern PSTR statementStartPtr;
extern TokenCodeType codeToken;
extern int32_t NumExecutions;

extern StackItem* stack;
extern StackItemPtr tos;
extern StackItemPtr stackFrameBasePtr;
extern SymTableNodePtr CurRoutineIdPtr;

extern TypePtr IntegerTypePtr;
extern TypePtr CharTypePtr;
extern TypePtr RealTypePtr;
extern TypePtr BooleanTypePtr;

extern bool ExitWithReturn;
extern bool ExitFromTacOrder;
extern bool AutoReturnFromOrders;

extern int32_t MaxLoopIterations;

extern DebuggerPtr debugger;
extern ABLModulePtr CurModule;
extern ABLModulePtr CurFSM;
extern SymTableNodePtr CurModuleIdPtr;
extern int32_t CurModuleHandle;
extern bool CallModuleInit;
extern StackItemPtr StaticDataPtr;
uint32_t* OrderCompletionFlags = nullptr;
extern ModuleEntryPtr ModuleRegistry;
extern ABLModulePtr* ModuleInstanceRegistry;
extern ABLModulePtr CurModule;
extern ABLModulePtr CurLibrary;
extern int32_t ProfileLogFunctionTimeLimit;
extern ABLFile* ProfileLog;
extern bool NewStateSet;

int32_t dummyCount = 0;

void execOrderReturn(int32_t returnVal);
void ABL_AddToProfileLog(PSTR profileString);
void transState(SymTableNodePtr newState);

//***************************************************************************
//
//***************************************************************************

void execStatement(void)
{
	if (codeToken == TKN_STATEMENT_MARKER)
	{
		execLineNumber = getCodeStatementMarker();
		execStatementCount++;
		statementStartPtr = codeSegmentPtr;
		if (debugger)
			debugger->traceStatementExecution();
		getCodeToken();
	}
	switch (codeToken)
	{
	case TKN_IDENTIFIER:
	{
		SymTableNodePtr idPtr = getCodeSymTableNodePtr();
		ABL_Assert(idPtr != nullptr, 0, " oops ");
		if (idPtr->defn.key == DFN_FUNCTION)
		{
			bool skipOrder		 = false;
			uint8_t orderDWord   = 0;
			uint8_t orderBitMask = 0;
			if ((idPtr->defn.info.routine.flags & ROUTINE_FLAG_ORDER) &&
				CurModule->getOrderCallFlags())
			{
				orderDWord   = getCodeByte();
				orderBitMask = getCodeByte();
				skipOrder =
					!CurModule->isLibrary() &&
					CurModule->getOrderCallFlag(orderDWord, orderBitMask);
			}
			TypePtr returnType = execRoutineCall(idPtr, skipOrder);
			if (idPtr->defn.info.routine.flags & ROUTINE_FLAG_ORDER)
			{
				if (AutoReturnFromOrders)
				{
					//-----------------------------------------------------------------
					// We called an Order function, and we're in an Orders/State
					// block, so do we continue the flow of orders or stop here?
					int32_t returnVal = tos->integer;
					pop();
					if (returnVal == 0)
						execOrderReturn(returnVal);
					else if (CurModule->getOrderCallFlags())
					{
						CurModule->setOrderCallFlag(orderDWord, orderBitMask);
					}
				}
			}
			else if (returnType)
			{
				//------------------------------------------
				// In case this routine returns a value, pop
				// the return value off the stack...
				pop();
			}
		}
		else
			execAssignmentStatement(idPtr);
	}
	break;
	case TKN_CODE:
	{
		bool wasAutoReturnFromOrders = AutoReturnFromOrders;
		AutoReturnFromOrders =
			((CurRoutineIdPtr->defn.info.routine.flags &
				 (ROUTINE_FLAG_ORDER + ROUTINE_FLAG_STATE)) != 0);
		getCodeToken();
		TokenCodeType endToken = TKN_END_FUNCTION;
		if (CurRoutineIdPtr->defn.info.routine.flags & ROUTINE_FLAG_ORDER)
			endToken = TKN_END_ORDER;
		else if (CurRoutineIdPtr->defn.info.routine.flags & ROUTINE_FLAG_STATE)
			endToken = TKN_END_STATE;
		TokenCodeType endTokenFinal = TKN_END_MODULE;
		if (CurLibrary)
			endTokenFinal = TKN_END_LIBRARY;
		else if (CurRoutineIdPtr->defn.info.routine.flags & ROUTINE_FLAG_FSM)
			endTokenFinal = TKN_END_FSM;
		while ((codeToken != endToken) && (codeToken != endTokenFinal) &&
			   !NewStateSet)
			execStatement();
		if (NewStateSet)
			return;
		getCodeToken();
		AutoReturnFromOrders = wasAutoReturnFromOrders;
	}
	break;
	case TKN_FOR:
		execForStatement();
		break;
	case TKN_IF:
		execIfStatement();
		break;
	case TKN_REPEAT:
		execRepeatStatement();
		break;
	case TKN_WHILE:
		execWhileStatement();
		break;
	case TKN_SWITCH:
		execSwitchStatement();
		break;
	case TKN_TRANS:
		execTransStatement();
		break;
	case TKN_TRANS_BACK:
		execTransBackStatement();
		break;
	case TKN_SEMICOLON:
	case TKN_ELSE:
	case TKN_UNTIL:
		break;
	default:
		// runtimeError(ABL_ERR_RUNTIME_UNIMPLEMENTED_FEATURE);
		NODEFAULT;
	}
	while (codeToken == TKN_SEMICOLON)
		getCodeToken();
}

//***************************************************************************

void execAssignmentStatement(SymTableNodePtr idPtr)
{
	StackItemPtr targetPtr;
	TypePtr targetTypePtr;
	TypePtr expressionTypePtr;
	//--------------------------
	// Assignment to variable...
	targetTypePtr = execVariable(idPtr, USE_TARGET);
	targetPtr	 = (StackItemPtr)tos->address;
	//------------------------------
	// Pop off the target address...
	pop();
	//------------------------
	// Pop the size, if nec...
	// if (targetTypePtr->form == FRM_ARRAY)
	//	pop();
	//---------------------------------------------------------------
	// Routine execExpression() leaves the expression value on top of
	// stack...
	getCodeToken();
	expressionTypePtr = execExpression();
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
		PSTR dest	= (PSTR)targetPtr;
		PSTR src	 = tos->address;
		int32_t size = targetTypePtr->size;
		memcpy(dest, src, size);
	}
	else if ((targetTypePtr == IntegerTypePtr) ||
			 (targetTypePtr->form == FRM_ENUM))
	{
		//------------------------------------------------------
		// Range check assignment to integer or enum subrange...
		targetPtr->integer = tos->integer;
	}
	else if (targetTypePtr == CharTypePtr)
		targetPtr->byte = tos->byte;
	else
	{
		//-----------------------
		// Assign real to real...
		targetPtr->real = tos->real;
	}
	//-----------------------------
	// Grab the expression value...
	pop();
	if (debugger)
		debugger->traceDataStore(
			idPtr, idPtr->typePtr, targetPtr, targetTypePtr);
}

//***************************************************************************

TypePtr execRoutineCall(SymTableNodePtr routineIdPtr, bool skipOrder)
{
	if (routineIdPtr->defn.info.routine.key == RTN_DECLARED)
		return (execDeclaredRoutineCall(routineIdPtr, skipOrder));
	else
		return (execStandardRoutineCall(routineIdPtr, skipOrder));
}

//***************************************************************************

TypePtr execDeclaredRoutineCall(SymTableNodePtr routineIdPtr, bool skipOrder)
{
	if (skipOrder)
	{
		StackItemPtr curStackFrameBase = tos;
		//----------------------------------------
		// Push parameter values onto the stack...
		getCodeToken();
		if (codeToken == TKN_LPAREN)
		{
			execActualParams(routineIdPtr);
			getCodeToken();
		}
		getCodeToken();
		tos = curStackFrameBase;
		pushInteger(1);
		return ((TypePtr)(routineIdPtr->typePtr));
	}
	int32_t oldLevel = level;					// level of caller
	int32_t newLevel = routineIdPtr->level + 1; // level of callee
	CallStackLevel++;
	//-------------------------------------------
	// First, set up the stack frame of callee...
	StackItemPtr newStackFrameBasePtr = tos + 1;
	bool isLibraryCall				  = (routineIdPtr->library &&
							 (routineIdPtr->library != CurRoutineIdPtr->library));
	if (isLibraryCall)
		pushStackFrameHeader(-1, -1);
	else
		pushStackFrameHeader(oldLevel, newLevel);
	//----------------------------------------
	// Push parameter values onto the stack...
	getCodeToken();
	if (codeToken == TKN_LPAREN)
	{
		execActualParams(routineIdPtr);
		getCodeToken();
	}
	//-------------------------------------------------
	// Set the return address in the new stack frame...
	level							 = newLevel;
	stackFrameBasePtr				 = newStackFrameBasePtr;
	StackFrameHeaderPtr headerPtr	= (StackFrameHeaderPtr)stackFrameBasePtr;
	headerPtr->returnAddress.address = codeSegmentPtr - 1;
	//---------------------------------------------------------
	// If we're calling a library function, we need to set some
	// module-specific info...
	ABLModulePtr PrevModule = nullptr;
	if (isLibraryCall)
	{
		PrevModule		= CurModule;
		CurModule		= routineIdPtr->library;
		CurModuleHandle = CurModule->getHandle();
		if (debugger)
			debugger->setModule(CurModule);
		StaticDataPtr  = CurModule->getStaticData();
		CallModuleInit = !CurModule->getInitCalled();
		CurModule->setInitCalled(true);
		//	routineEntry(ModuleRegistry[CurModule->getHandle()].moduleIdPtr);
	}
	if (ProfileLog)
	{
		int32_t functionStartTime = ABLGetTimeCallback();
		execute(routineIdPtr);
		int32_t functionExecTime = ABLGetTimeCallback() - functionStartTime;
		if (functionExecTime > ProfileLogFunctionTimeLimit)
		{
			char s[512];
			sprintf_s(s, _countof(s), "[%08d] ", NumExecutions);
			for (size_t i = 0; i < CallStackLevel; i++)
				strcat(s, " ");
			char s1[512];
			sprintf_s(s1, _countof(s1), "%s (%d)\n", routineIdPtr->name,
				functionExecTime);
			strcat(s, s1);
			ABL_AddToProfileLog(s);
		}
	}
	else
		execute(routineIdPtr);
	//----------------------------------------------------------------
	// If we're calling a library function, reset some module-specific
	// info...
	if (isLibraryCall)
	{
		//	routineExit(ModuleRegistry[CurModule->getHandle()].moduleIdPtr);
		CurModule		= PrevModule;
		CurModuleHandle = CurModule->getHandle();
		if (debugger)
			debugger->setModule(CurModule);
		StaticDataPtr = CurModule->getStaticData();
	}
	//-------------------------------------------------------
	// Return from the callee, and grab the first token after
	// the return...
	level = oldLevel;
	getCodeToken();
	CallStackLevel--;
	return ((TypePtr)(routineIdPtr->typePtr));
}

//***************************************************************************

void setOpenArray(TypePtr arrayTypePtr, int32_t size)
{
	int32_t numElements = size / arrayTypePtr->size;
	arrayTypePtr->size  = size;
	while (arrayTypePtr->info.array.elementTypePtr->form == FRM_ARRAY)
		arrayTypePtr = arrayTypePtr->info.array.elementTypePtr;
	arrayTypePtr->info.array.elementCount = numElements;
}

//***************************************************************************

void execActualParams(SymTableNodePtr routineIdPtr)
{
	//--------------------------
	// Execute the parameters...
	for (SymTableNodePtr formalIdPtr =
			 (SymTableNodePtr)(routineIdPtr->defn.info.routine.params);
		 formalIdPtr != nullptr; formalIdPtr = formalIdPtr->next)
	{
		TypePtr formalTypePtr = (TypePtr)(formalIdPtr->typePtr);
		getCodeToken();
		if (formalIdPtr->defn.key == DFN_VALPARAM)
		{
			//-------------------
			// pass by value parameter...
			TypePtr actualTypePtr = execExpression();
			if ((formalTypePtr == RealTypePtr) &&
				(actualTypePtr == IntegerTypePtr))
			{
				//---------------------------------------------
				// Real formal parameter, but integer actual...
				tos->real = (float)(tos->integer);
			}
			//----------------------------------------------------------
			// Formal parameter is an array or record, so make a copy...
			if ((formalTypePtr->form ==
					FRM_ARRAY) /* || (formalTypePtr->form == FRM_RECORD)*/)
			{
				//------------------------------------------------------------------------------
				// The following is a little inefficient, but is kept this way
				// to keep it clear. Once it's verified to work, optimize...
				int32_t size = formalTypePtr->size;
				PSTR src	 = tos->address;
				PSTR dest	= (PSTR)ABLStackMallocCallback((size_t)size);
				if (!dest)
				{
					char err[255];
					sprintf(err,
						" ABL: Unable to AblStackHeap->malloc actual array "
						"param in module %s)",
						CurModule->getName());
					ABL_Fatal(0, err);
				}
				PSTR savePtr = dest;
				memcpy(dest, src, size);
				tos->address = savePtr;
			}
		}
		else
		{
			//-------------------------------
			// pass by reference parameter...
			SymTableNodePtr idPtr = getCodeSymTableNodePtr();
			execVariable(idPtr, USE_REFPARAM);
		}
	}
}

//***************************************************************************

void execSwitchStatement(void)
{
	getCodeToken();
	PSTR branchTableLocation = getCodeAddressMarker();
	getCodeToken();
	TypePtr switchExpressionTypePtr = execExpression();
	int32_t switchExpressionValue;
	if ((switchExpressionTypePtr == IntegerTypePtr) ||
		(switchExpressionTypePtr->form == FRM_ENUM))
		switchExpressionValue = tos->integer;
	else
		switchExpressionValue = tos->byte;
	pop();
	//---------------------------------------------------------
	// Now, search the branch table for the expression value...
	codeSegmentPtr = branchTableLocation;
	getCodeToken();
	int32_t caseLabelCount  = getCodeInteger();
	bool done				= false;
	PSTR caseBranchLocation = nullptr;
	while (!done && caseLabelCount--)
	{
		int32_t caseLabelValue = getCodeInteger();
		caseBranchLocation	 = getCodeAddress();
		done				   = (caseLabelValue == switchExpressionValue);
	}
	//-----------------------------------------------
	// If found, go to the aprropriate branch code...
	if (caseLabelCount >= 0)
	{
		codeSegmentPtr = caseBranchLocation;
		getCodeToken();
		if (codeToken != TKN_END_CASE)
			do
			{
				execStatement();
				if (ExitWithReturn)
					return;
			} while (codeToken != TKN_END_CASE);
		//----------------------------------
		// Grab the end case and semi-colon...
		getCodeToken();
		getCodeToken();
		codeSegmentPtr = getCodeAddressMarker();
		getCodeToken();
	}
	else
	{
		//-----------------------------------------------------------------
		// Since the branch table is located at the end of the case blocks,
		// the code directly after the switch statement follows our
		// current code location, already. Just grab the endswitch
		// and semi-colon...
		getCodeToken();
		getCodeToken();
	}
}

//***************************************************************************

void execForStatement(void)
{
	getCodeToken();
	//---------------------------------------
	// Grab address of the end of the loop...
	PSTR loopEndLocation = getCodeAddressMarker();
	//--------------------------------------------------------
	// Get the address of the control variable's stack item...
	getCodeToken();
	SymTableNodePtr controlIdPtr = getCodeSymTableNodePtr();
	TypePtr controlTypePtr		 = execVariable(controlIdPtr, USE_TARGET);
	StackItemPtr targetPtr		 = (StackItemPtr)tos->address;
	//------------------------------------
	// Control variable address...
	pop();
	//-------------------------------
	// Eval the initial expression...
	getCodeToken();
	execExpression();
	int32_t initialValue;
	if (controlTypePtr == IntegerTypePtr)
		initialValue = tos->integer;
	else
		initialValue = tos->byte;
	//---------------------
	// The initial value...
	pop();
	int32_t deltaValue;
	if (codeToken == TKN_TO)
		deltaValue = 1;
	else
		deltaValue = -1;
	//----------------------------------
	// Now, eval the final expression...
	getCodeToken();
	execExpression();
	int32_t finalValue;
	if (controlTypePtr == IntegerTypePtr)
		finalValue = tos->integer;
	else
		finalValue = tos->byte;
	//-------------------
	// The final value...
	pop();
	//----------------------------
	// Address of start of loop...
	PSTR loopStartLocation = codeSegmentPtr;
	int32_t controlValue   = initialValue;
	//-----------------------------
	// Now, execute the FOR loop...
	int32_t iterations = 0;
	if (deltaValue == 1)
		while (controlValue <= finalValue)
		{
			if (controlTypePtr == IntegerTypePtr)
				targetPtr->integer = controlValue;
			else
				targetPtr->byte = (uint8_t)controlValue;
			getCodeToken();
			if (codeToken != TKN_END_FOR)
				do
				{
					execStatement();
					if (ExitWithReturn)
						return;
				} while (codeToken != TKN_END_FOR);
			//---------------------------
			// Check for infinite loop...
			if (++iterations == MaxLoopIterations)
				runtimeError(ABL_ERR_RUNTIME_INFINITE_LOOP);
			controlValue++;
			codeSegmentPtr = loopStartLocation;
		}
	else
		while (controlValue >= finalValue)
		{
			if (controlTypePtr == IntegerTypePtr)
				targetPtr->integer = controlValue;
			else
				targetPtr->byte = (uint8_t)controlValue;
			getCodeToken();
			if (codeToken != TKN_END_FOR)
				do
				{
					execStatement();
					if (ExitWithReturn)
						return;
				} while (codeToken != TKN_END_FOR);
			//---------------------------
			// Check for infinite loop...
			if (++iterations == MaxLoopIterations)
				runtimeError(ABL_ERR_RUNTIME_INFINITE_LOOP);
			controlValue--;
			codeSegmentPtr = loopStartLocation;
		}
	codeSegmentPtr = loopEndLocation;
	getCodeToken();
}

//***************************************************************************

void execTransStatement(void)
{
	getCodeToken();
	getCodeToken();
	SymTableNodePtr idPtr = getCodeSymTableNodePtr();
	transState(idPtr);
	getCodeToken();
}

//***************************************************************************

void execTransBackStatement(void)
{
	SymTableNodePtr prevState = CurModule->getPrevState();
	if (!prevState)
		runtimeError(ABL_ERR_RUNTIME_NULL_PREVSTATE);
	transState(prevState);
	getCodeToken();
}

//***************************************************************************

void execIfStatement(void)
{
	getCodeToken();
	PSTR falseLocation = getCodeAddressMarker();
	//-------------------------------
	// Eval the boolean expression. Note that, unlike C/C++, the expression
	// must be true(1) or false(0). In C/C++, an expression is true if it's
	// non-zero. Not the case in ABL using this current implementation. Do we
	// want to change this?
	getCodeToken();
	execExpression();
	bool test = (tos->integer == 1);
	pop();
	if (test)
	{
		//---------------------------
		// execute the TRUE branch...
		getCodeToken();
		if ((codeToken != TKN_END_IF) && (codeToken != TKN_ELSE))
			do
			{
				execStatement();
				if (ExitWithReturn)
					return;
			} while ((codeToken != TKN_END_IF) && (codeToken != TKN_ELSE));
		if (codeToken == TKN_ELSE)
		{
			getCodeToken();
			codeSegmentPtr = getCodeAddressMarker();
			getCodeToken();
		}
	}
	else
	{
		//----------------------------
		// Execute the FALSE branch...
		codeSegmentPtr = falseLocation;
		getCodeToken();
		if (codeToken == TKN_ELSE)
		{
			getCodeToken();
			getCodeAddressMarker();
			getCodeToken();
			if (codeToken != TKN_END_IF)
				do
				{
					execStatement();
					if (ExitWithReturn)
						return;
				} while (codeToken != TKN_END_IF);
		}
	}
	getCodeToken();
}

//***************************************************************************

void execRepeatStatement(void)
{
	PSTR loopStartLocation = codeSegmentPtr;
	int32_t iterations	 = 0;
	do
	{
		getCodeToken();
		if (codeToken != TKN_UNTIL)
			do
			{
				execStatement();
				if (ExitWithReturn)
					return;
			} while (codeToken != TKN_UNTIL);
		//---------------------------
		// Check for infinite loop...
		iterations++;
		if (iterations == MaxLoopIterations)
			runtimeError(ABL_ERR_RUNTIME_INFINITE_LOOP);
		//-------------------------------
		// Eval the boolean expression...
		getCodeToken();
		execExpression();
		if (tos->integer == 0)
			codeSegmentPtr = loopStartLocation;
		//--------------------------
		// Grab the boolean value...
		pop();
	} while (codeSegmentPtr == loopStartLocation);
}

//***************************************************************************

void execWhileStatement(void)
{
	getCodeToken();
	PSTR loopEndLocation = getCodeAddressMarker();
	PSTR testLocation	= codeSegmentPtr;
	bool loopDone		 = false;
	int32_t iterations   = 0;
	do
	{
		//-------------------------------
		// Eval the boolean expression...
		getCodeToken();
		execExpression();
		if (tos->integer == 0)
		{
			codeSegmentPtr = loopEndLocation;
			loopDone	   = true;
		}
		//-------------------------
		// Get the boolean value...
		pop();
		//----------------------------------
		// If TRUE, execute the statement...
		if (!loopDone)
		{
			getCodeToken();
			if (codeToken != TKN_END_WHILE)
				do
				{
					execStatement();
					if (ExitWithReturn)
						return;
				} while (codeToken != TKN_END_WHILE);
			codeSegmentPtr = testLocation;
			//---------------------------
			// Check for infinite loop...
			iterations++;
			if (iterations == MaxLoopIterations)
				runtimeError(ABL_ERR_RUNTIME_INFINITE_LOOP);
		}
	} while (!loopDone);
	getCodeToken();
}

//***************************************************************************
