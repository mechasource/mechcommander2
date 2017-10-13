//===========================================================================//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//
//***************************************************************************
//
//								SYMTABLE.H
//
//***************************************************************************

#pragma once

#ifndef ABLSYMT_H
#define ABLSYMT_H

//#include "ablgen.h"
//#include "ablscan.h"
//#include "dablenv.h"

//***************************************************************************

//---------------------
// DEFINITION structure

typedef union {
	int32_t integer;
	char character;
	float real;
	PSTR stringPtr;
} Value;

typedef enum
{
	VAR_TYPE_NORMAL,
	VAR_TYPE_STATIC,
	VAR_TYPE_ETERNAL,
	VAR_TYPE_REGISTERED
} VariableType;

typedef enum
{
	DFN_UNDEFINED,
	DFN_CONST,
	DFN_TYPE,
	DFN_VAR,
	DFN_VALPARAM,
	DFN_REFPARAM,
	DFN_MODULE,
	DFN_PROCEDURE,
	DFN_FUNCTION
} DefinitionType;

typedef enum
{
	RTN_DECLARED,
	RTN_FORWARD,
	RTN_RETURN,
	RTN_PRINT,
	RTN_CONCAT,
	RTN_GET_STATE_HANDLE,
	RTN_ASSERT,
	RTN_FATAL,
	NUM_ABL_ROUTINES
} RoutineKey;
// NOTE: NUMBER OF ROUTINEKEYS IS LIMITED TO 256!

typedef struct _SymTableNode* SymTableNodePtr;
typedef struct _Type* TypePtr;

typedef struct
{
	Value value;
} Constant;

typedef struct
{
	char name[128];
	SymTableNodePtr state;
} StateHandleInfo;

typedef StateHandleInfo* StateHandleInfoPtr;

#define ROUTINE_FLAG_ORDER 1
#define ROUTINE_FLAG_FSM 2
#define ROUTINE_FLAG_STATE 4

typedef struct _Routine
{
	RoutineKey key;
	uint8_t flags;
	uint16_t orderCallIndex;
	uint16_t numOrderCalls;
	uint8_t paramCount;
	uint8_t totalParamSize;
	uint16_t totalLocalSize;
	SymTableNodePtr params;
	SymTableNodePtr locals;
	SymTableNodePtr localSymTable;
	PSTR codeSegment;
	int32_t codeSegmentSize;
} Routine;

typedef struct
{
	VariableType varType;
	int32_t offset;
	PVOID registeredData;
	// SymTableNodePtr			recordIdPtr;		// Currently not implementing record
	// structures...
} Data;

typedef union {
	Constant constant;
	Routine routine;
	Data data;
} DefinitionInfo;

typedef struct
{
	DefinitionType key;
	DefinitionInfo info;
} Definition;

//***************************************************************************

//------------------
// SYMBOL TABLE node

typedef struct _SymTableNode
{
	SymTableNodePtr left;
	SymTableNodePtr parent;
	SymTableNodePtr right;
	SymTableNodePtr next;
	PSTR name;
	PSTR info;
	Definition defn;
	TypePtr typePtr;
	ABLModulePtr library;
	uint8_t level;
	int32_t labelIndex; // really for compiling only...
} SymTableNode;

typedef enum
{
	PARAM_TYPE_ANYTHING,
	PARAM_TYPE_CHAR,
	PARAM_TYPE_INTEGER,
	PARAM_TYPE_REAL,
	PARAM_TYPE_BOOLEAN,
	PARAM_TYPE_INTEGER_REAL,
	PARAM_TYPE_CHAR_ARRAY,
	PARAM_TYPE_INTEGER_ARRAY,
	PARAM_TYPE_REAL_ARRAY,
	PARAM_TYPE_BOOLEAN_ARRAY,
	NUM_PARAM_TYPES
} FunctionParamType;

typedef enum
{
	RETURN_TYPE_NONE,
	RETURN_TYPE_INTEGER,
	RETURN_TYPE_REAL,
	RETURN_TYPE_BOOLEAN,
	NUM_RETURN_TYPES
} FunctionReturnType;

#define MAX_STANDARD_FUNCTIONS 256
#define MAX_FUNCTION_PARAMS 20

typedef struct
{
	SymTableNodePtr symbol;
	int32_t numParams;
	FunctionParamType params[MAX_FUNCTION_PARAMS];
	FunctionReturnType returnType;
} StandardFunctionInfo;

//***************************************************************************

//---------------
// TYPE structure

typedef enum
{
	FRM_NONE,
	FRM_SCALAR,
	FRM_ENUM,
	FRM_ARRAY
	// FRM_RECORD
} FormType;

//---------------------------------------------------------------------
// Currently, we are only supporting the basic types: arrays and simple
// variables.

typedef struct _Type
{
	int32_t numInstances;
	FormType form;
	int32_t size;
	SymTableNodePtr typeIdPtr;
	union {
		struct
		{
			SymTableNodePtr constIdPtr;
			int32_t max;
		} enumeration;
		struct
		{
			TypePtr indexTypePtr;   // should be Integer
			TypePtr elementTypePtr; // should be Real, Integer, Char or array
			int32_t elementCount;
		} array;
		// Not currently implementing record structures...
		// struct {
		//	SymTableNodePtr		fieldSymTable;
		//} record;
	} info;
} Type;

//***************************************************************************

void searchLocalSymTable(SymTableNodePtr& IdPtr);
SymTableNodePtr searchLocalSymTableForFunction(PSTR name);
void searchThisSymTable(SymTableNodePtr& IdPtr, SymTableNodePtr thisTable);
void searchAllSymTables(SymTableNodePtr& IdPtr);
void enterLocalSymTable(SymTableNodePtr& IdPtr);
void enterNameLocalSymTable(SymTableNodePtr& IdPtr, PSTR name);
void searchAndFindAllSymTables(SymTableNodePtr& IdPtr);
void searchAndEnterLocalSymTable(SymTableNodePtr& IdPtr);
/*inline*/ void searchAndEnterThisTable(
	SymTableNodePtr& IdPtr, SymTableNodePtr thisTable);
inline SymTableNodePtr symTableSuccessor(SymTableNodePtr nodeX);

SymTableNodePtr searchSymTable(PSTR name, SymTableNodePtr nodePtr);
SymTableNodePtr searchSymTableForFunction(PSTR name, SymTableNodePtr nodePtr);
SymTableNodePtr searchSymTableForState(PSTR name, SymTableNodePtr nodePtr);
SymTableNodePtr searchSymTableForString(PSTR name, SymTableNodePtr nodePtr);
SymTableNodePtr searchSymTableDisplay(PSTR name);
SymTableNodePtr enterSymTable(PSTR name, SymTableNodePtr* ptrToNodePtr);
SymTableNodePtr insertSymTable(
	SymTableNodePtr* tableRoot, SymTableNodePtr newNode);
SymTableNodePtr extractSymTable(
	SymTableNodePtr* tableRoot, SymTableNodePtr nodeKill);
void enterStandardRoutine(PSTR name, int32_t routineKey, bool isOrder,
	PSTR paramList, PSTR returnType, void (*callback)(void));
void enterScope(SymTableNodePtr symTableRoot);
SymTableNodePtr exitScope(void);
void initSymTable(void);

TypePtr createType(void);
TypePtr setType(TypePtr type);
void clearType(TypePtr& type);

//***************************************************************************

extern StandardFunctionInfo FunctionInfoTable[MAX_STANDARD_FUNCTIONS];
// extern PVOID					FunctionCallbackTable[MAX_STANDARD_FUNCTIONS];
extern void (*FunctionCallbackTable[MAX_STANDARD_FUNCTIONS])(void);
extern int32_t NumStandardFunctions;

#endif
