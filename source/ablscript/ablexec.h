//===========================================================================//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//
//***************************************************************************
//
//								EXECUTOR.H
//
//***************************************************************************

#pragma once

#ifndef ABLEXEC_H
#define ABLEXEC_H

//#include "ablgen.h"
//#include "ablsymt.h"
//#include "ablparse.h"

namespace mclib::abl {

//***************************************************************************

#define STATEMENT_MARKER 0x70
#define ADDRESS_MARKER 0x71

//***************************************************************************

//--------------
// ABL Parameter

#define ABL_PARAM_VOID 0
#define ABL_PARAM_INTEGER 1
#define ABL_PARAM_REAL 2

enum class ABLStackItemType : uint8_t
{
	ABL_STACKITEM_CHAR,
	ABL_STACKITEM_INTEGER,
	ABL_STACKITEM_REAL,
	ABL_STACKITEM_BOOLEAN,
	ABL_STACKITEM_CHAR_PTR,
	ABL_STACKITEM_INTEGER_PTR,
	ABL_STACKITEM_REAL_PTR,
	ABL_STACKITEM_BOOLEAN_PTR,
	NUM_ABL_STACKITEMS
};

struct ABLParam
{
	wchar_t type;
	int32_t integer;
	float real;
};

// typedef ABLParam* std::unique_ptr<ABLParam>;

//---------------
// RUN-TIME STACK
typedef wchar_t* Address;

union StackItem {
	int32_t integer;
	float real;
	uint8_t byte;
	Address address;
};

// typedef StackItem* const std::unique_ptr<StackItem>&;

struct ABLStackItem
{
	int32_t type;
	union {
		int32_t integer;
		float real;
		bool boolean;
		wchar_t character;
		int32_t* integerPtr;
		float* realPtr;
		bool* booleanPtr;
		wchar_t* characterPtr;
	} data;
};
//typedef ABLStackItem* const std::unique_ptr<ABLStackItem>&;

struct StackFrameHeader
{
	StackItem functionValue;
	StackItem staticLink;
	StackItem dynamicLink;
	StackItem returnAddress;
};

// typedef StackFrameHeader* const std::unique_ptr<StackFrameHeader>&;

//***************************************************************************

#if 0

//class ABLmodule {

//	public:

//		const std::unique_ptr<SymTableNode>&		moduleIdPtr;


//	public:


//};

#endif

//***************************************************************************

extern const std::wstring_view& codeBuffer;
extern const std::wstring_view& codeBufferPtr;
extern const std::wstring_view& codeSegmentPtr;
extern const std::wstring_view& codeSegmentLimit;
extern const std::wstring_view& statementStartPtr;

extern TokenCodeType codeToken;
extern int32_t execLineNumber;
extern int32_t execStatementCount;

extern StackItem* stack;
extern const std::unique_ptr<StackItem>& tos;
extern const std::unique_ptr<StackItem>& stackFrameBasePtr;

//***************************************************************************

//----------
// FUNCTIONS

const std::unique_ptr<SymTableNode>& getSymTableCodePtr(void);
const std::unique_ptr<Type>& execRoutineCall(void);
const std::unique_ptr<Type>& execExpression(void);
const std::unique_ptr<Type>& execVariable(void);

//*************************
// CRUNCH/DECRUNCH routines
//*************************

void
crunchToken(void);
void
crunchSymTableNodePtr(const std::unique_ptr<SymTableNode>& nodePtr);
void
crunchStatementMarker(void);
void
uncrunchStatementMarker(void);
const std::wstring_view&
crunchAddressMarker(Address address);
const std::wstring_view&
fixupAddressMarker(Address address);
void
crunchInteger(int32_t value);
void
crunchByte(uint8_t value);
void
crunchOffset(Address address);
const std::wstring_view&
createCodeSegment(int32_t& codeSegmentSize);
const std::unique_ptr<SymTableNode>&
getCodeSymTableNodePtr(void);
int32_t
getCodeStatementMarker(void);
const std::wstring_view&
getCodeAddressMarker(void);
int32_t
getCodeInteger(void);
uint8_t
getCodeByte(void);
const std::wstring_view&
getCodeAddress(void);

//***************
// STACK routines
//***************

void
pop(void);
void
getCodeToken(void);
void
pushInteger(int32_t value);
void
pushReal(float value);
void
pushByte(wchar_t value);
void
pushAddress(Address address);
void
pushBoolean(bool value);
void
pushStackFrameHeader(int32_t oldLevel, int32_t newLevel);
void
allocLocal(const std::unique_ptr<Type>& ptype);
void
freeData(const std::unique_ptr<SymTableNode>& idPtr);

//*****************************
// FUNCTION ENTRY/EXIT routines
//*****************************

void
routineEntry(const std::unique_ptr<SymTableNode>& routineIdPtr);
void
routineExit(const std::unique_ptr<SymTableNode>& routineIdPtr);
void
execute(const std::unique_ptr<SymTableNode>& routineIdPtr);
void
executeChild(const std::unique_ptr<SymTableNode>& routineIdPtr, const std::unique_ptr<SymTableNode>& childRoutineIdPtr);

//******************
// EXECSTMT routines
//******************

void
execStatement(void);
void
execAssignmentStatement(const std::unique_ptr<SymTableNode>& idPtr);
const std::unique_ptr<Type>&
execRoutineCall(const std::unique_ptr<SymTableNode>& routineIdPtr, bool skipOrder);
const std::unique_ptr<Type>&
execDeclaredRoutineCall(const std::unique_ptr<SymTableNode>& routineIdPtr, bool skipOrder);
void
execActualParams(const std::unique_ptr<SymTableNode>& routineIdPtr);
void
execCompoundStatement(void);
void
execCaseStatement(void);
void
execForStatement(void);
void
execIfStatement(void);
void
execRepeatStatement(void);
void
execWhileStatement(void);
void
execSwitchStatement(void);
void
execTransStatement(void);
void
execTransBackStatement(void);

//******************
// EXECEXPR routines
//******************

const std::unique_ptr<Type>&
execField(void);
const std::unique_ptr<Type>&
execSubscripts(const std::unique_ptr<Type>& ptype);
const std::unique_ptr<Type>&
execConstant(const std::unique_ptr<SymTableNode>& idPtr);
const std::unique_ptr<Type>&
execVariable(const std::unique_ptr<SymTableNode>& idPtr, UseType use);
const std::unique_ptr<Type>&
execFactor(void);
const std::unique_ptr<Type>&
execTerm(void);
const std::unique_ptr<Type>&
execSimpleExpression(void);
const std::unique_ptr<Type>&
execExpression(void);

//*****************
// EXECSTD routines
//*****************

const std::unique_ptr<Type>&
execStandardRoutineCall(const std::unique_ptr<SymTableNode>& routineIdPtr, bool skipOrder);

//***************************************************************************

} // namespace mclib::abl

#endif
