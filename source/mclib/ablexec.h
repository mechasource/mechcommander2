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

//***************************************************************************

#define STATEMENT_MARKER 0x70
#define ADDRESS_MARKER 0x71

//***************************************************************************

//--------------
// ABL Parameter

#define ABL_PARAM_VOID 0
#define ABL_PARAM_INTEGER 1
#define ABL_PARAM_REAL 2

typedef enum ABLStackItemType : uint32_t
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
} ABLStackItemType;

typedef struct
{
	char type;
	int32_t integer;
	float real;
} ABLParam;

typedef ABLParam* ABLParamPtr;

//---------------
// RUN-TIME STACK

typedef union {
	int32_t integer;
	float real;
	uint8_t byte;
	Address address;
} StackItem;

typedef StackItem* StackItemPtr;

typedef struct
{
	int32_t type;
	union {
		int32_t integer;
		float real;
		bool boolean;
		char character;
		int32_t* integerPtr;
		float* realPtr;
		bool* booleanPtr;
		PSTR characterPtr;
	} data;
} ABLStackItem;

typedef ABLStackItem* ABLStackItemPtr;

typedef struct
{
	StackItem functionValue;
	StackItem staticLink;
	StackItem dynamicLink;
	StackItem returnAddress;
} StackFrameHeader;

typedef StackFrameHeader* StackFrameHeaderPtr;

//***************************************************************************

#if 0

//class ABLmodule {

//	public:

//		SymTableNodePtr		moduleIdPtr;


//	public:


//};

#endif

//***************************************************************************

extern PSTR codeBuffer;
extern PSTR codeBufferPtr;
extern PSTR codeSegmentPtr;
extern PSTR codeSegmentLimit;
extern PSTR statementStartPtr;

extern TokenCodeType codeToken;
extern int32_t execLineNumber;
extern int32_t execStatementCount;

extern StackItem* stack;
extern StackItemPtr tos;
extern StackItemPtr stackFrameBasePtr;

//***************************************************************************

//----------
// FUNCTIONS

SymTableNodePtr getSymTableCodePtr(void);
TypePtr execRoutineCall(void);
TypePtr execExpression(void);
TypePtr execVariable(void);

//*************************
// CRUNCH/DECRUNCH routines
//*************************

void crunchToken(void);
void crunchSymTableNodePtr(SymTableNodePtr nodePtr);
void crunchStatementMarker(void);
void uncrunchStatementMarker(void);
PSTR crunchAddressMarker(Address address);
PSTR fixupAddressMarker(Address address);
void crunchInteger(int32_t value);
void crunchByte(uint8_t value);
void crunchOffset(Address address);
PSTR createCodeSegment(int32_t& codeSegmentSize);
SymTableNodePtr getCodeSymTableNodePtr(void);
int32_t getCodeStatementMarker(void);
PSTR getCodeAddressMarker(void);
int32_t getCodeInteger(void);
uint8_t getCodeByte(void);
PSTR getCodeAddress(void);

//***************
// STACK routines
//***************

void pop(void);
void getCodeToken(void);
void pushInteger(int32_t value);
void pushReal(float value);
void pushByte(char value);
void pushAddress(Address address);
void pushBoolean(bool value);
void pushStackFrameHeader(int32_t oldLevel, int32_t newLevel);
void allocLocal(TypePtr typePtr);
void freeData(SymTableNodePtr idPtr);

//*****************************
// FUNCTION ENTRY/EXIT routines
//*****************************

void routineEntry(SymTableNodePtr routineIdPtr);
void routineExit(SymTableNodePtr routineIdPtr);
void execute(SymTableNodePtr routineIdPtr);
void executeChild(SymTableNodePtr routineIdPtr, SymTableNodePtr childRoutineIdPtr);

//******************
// EXECSTMT routines
//******************

void execStatement(void);
void execAssignmentStatement(SymTableNodePtr idPtr);
TypePtr execRoutineCall(SymTableNodePtr routineIdPtr, bool skipOrder);
TypePtr execDeclaredRoutineCall(SymTableNodePtr routineIdPtr, bool skipOrder);
void execActualParams(SymTableNodePtr routineIdPtr);
void execCompoundStatement(void);
void execCaseStatement(void);
void execForStatement(void);
void execIfStatement(void);
void execRepeatStatement(void);
void execWhileStatement(void);
void execSwitchStatement(void);
void execTransStatement(void);
void execTransBackStatement(void);

//******************
// EXECEXPR routines
//******************

TypePtr execField(void);
TypePtr execSubscripts(TypePtr typePtr);
TypePtr execConstant(SymTableNodePtr idPtr);
TypePtr execVariable(SymTableNodePtr idPtr, UseType use);
TypePtr execFactor(void);
TypePtr execTerm(void);
TypePtr execSimpleExpression(void);
TypePtr execExpression(void);

//*****************
// EXECSTD routines
//*****************

TypePtr execStandardRoutineCall(SymTableNodePtr routineIdPtr, bool skipOrder);

//***************************************************************************

#endif
