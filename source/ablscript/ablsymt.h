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

namespace mclib::abl {

//***************************************************************************

//---------------------
// DEFINITION structure

union value {
	int32_t integer;
	wchar_t character;
	float real;
	std::wstring_view stringPtr;
};

enum class VariableType : uint8_t
{
	VAR_TYPE_NORMAL,
	VAR_TYPE_STATIC,
	VAR_TYPE_ETERNAL,
	VAR_TYPE_REGISTERED
};

enum class DefinitionType : uint8_t
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
};

enum class RoutineKey : uint8_t
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
};
// NOTE: NUMBER OF ROUTINEKEYS IS LIMITED TO 256!

//typedef struct _SymTableNode* SymTableNodePtr;
//typedef struct _Type* TypePtr;

struct Constant
{
	value value;
};

struct StateHandleInfo
{
	wchar_t name[128];
	const std::unique_ptr<SymTableNode>& state;
};

// typedef StateHandleInfo* StateHandleInfo;

#define ROUTINE_FLAG_ORDER 1
#define ROUTINE_FLAG_FSM 2
#define ROUTINE_FLAG_STATE 4

struct Routine
{
	RoutineKey key;
	uint8_t flags;
	uint16_t orderCallIndex;
	uint16_t numOrderCalls;
	uint8_t paramCount;
	uint8_t totalParamSize;
	uint16_t totalLocalSize;
	const std::unique_ptr<SymTableNode>& params;
	const std::unique_ptr<SymTableNode>& locals;
	const std::unique_ptr<SymTableNode>& localSymTable;
	const std::wstring_view& codeSegment;
	int32_t codeSegmentSize;
};

struct Data
{
	VariableType varType;
	int32_t offset;
	PVOID registeredData;
	// const std::unique_ptr<SymTableNode>&			recordIdPtr;		// Currently not
	// implementing record structures...
};

union DefinitionInfo {
	Constant constant;
	Routine routine;
	Data data;
};

struct Definition
{
	DefinitionType key;
	DefinitionInfo info;
};

//***************************************************************************

//------------------
// SYMBOL TABLE node

struct SymTableNode
{
	std::unique_ptr<SymTableNode> left;
	std::unique_ptr<SymTableNode> parent;
	std::unique_ptr<SymTableNode> right;
	std::unique_ptr<SymTableNode> next;
	std::unique_ptr<Type> ptype;
	std::unique_ptr<ABLModule> library;
	std::wstring_view name;
	std::wstring_view info;
	Definition defn;
	uint8_t level;
	int32_t labelIndex; // really for compiling only...
};

enum class FunctionParamType : uint8_t
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
};

enum class FunctionReturnType : uint8_t
{
	RETURN_TYPE_NONE,
	RETURN_TYPE_INTEGER,
	RETURN_TYPE_REAL,
	RETURN_TYPE_BOOLEAN,
	NUM_RETURN_TYPES
};

#define MAX_STANDARD_FUNCTIONS 256
#define MAX_FUNCTION_PARAMS 20

typedef struct
{
	const std::unique_ptr<SymTableNode>& symbol;
	int32_t numParams;
	FunctionParamType params[MAX_FUNCTION_PARAMS];
	FunctionReturnType returnType;
} StandardFunctionInfo;

//***************************************************************************

//---------------
// TYPE structure

enum class FormType : uint8_t
{
	FRM_NONE,
	FRM_SCALAR,
	FRM_ENUM,
	FRM_ARRAY
	// FRM_RECORD
};

//---------------------------------------------------------------------
// Currently, we are only supporting the basic types: arrays and simple
// variables.

struct Type
{
	int32_t numInstances;
	FormType form;
	int32_t size;
	const std::unique_ptr<SymTableNode>& typeIdPtr;
	union {
		struct
		{
			const std::unique_ptr<SymTableNode>& constIdPtr;
			int32_t max;
		} enumeration;
		struct
		{
			const std::unique_ptr<Type>& indexTypePtr; // should be Integer
			const std::unique_ptr<Type>& elementTypePtr; // should be Real, Integer, Char or array
			int32_t elementCount;
		} array;
		// Not currently implementing record structures...
		// struct {
		//	const std::unique_ptr<SymTableNode>&		fieldSymTable;
		//} record;
	} info;
};

//***************************************************************************

void
searchLocalSymTable(const std::unique_ptr<SymTableNode>&& IdPtr);
const std::unique_ptr<SymTableNode>&
searchLocalSymTableForFunction(const std::wstring_view& name);
void
searchThisSymTable(const std::unique_ptr<SymTableNode>&& IdPtr, const std::unique_ptr<SymTableNode>& thisTable);
void
searchAllSymTables(const std::unique_ptr<SymTableNode>&& IdPtr);
void
enterLocalSymTable(const std::unique_ptr<SymTableNode>&& IdPtr);
void
enterNameLocalSymTable(const std::unique_ptr<SymTableNode>&& IdPtr, const std::wstring_view& name);
void
searchAndFindAllSymTables(const std::unique_ptr<SymTableNode>&& IdPtr);
void
searchAndEnterLocalSymTable(const std::unique_ptr<SymTableNode>&& IdPtr);
/*inline*/ void
searchAndEnterThisTable(const std::unique_ptr<SymTableNode>&& IdPtr, const std::unique_ptr<SymTableNode>& thisTable);
inline const std::unique_ptr<SymTableNode>&
symTableSuccessor(const std::unique_ptr<SymTableNode>& nodeX);

const std::unique_ptr<SymTableNode>&
searchSymTable(const std::wstring_view& name, const std::unique_ptr<SymTableNode>& nodePtr);
const std::unique_ptr<SymTableNode>&
searchSymTableForFunction(const std::wstring_view& name, const std::unique_ptr<SymTableNode>& nodePtr);
const std::unique_ptr<SymTableNode>&
searchSymTableForState(const std::wstring_view& name, const std::unique_ptr<SymTableNode>& nodePtr);
const std::unique_ptr<SymTableNode>&
searchSymTableForString(const std::wstring_view& name, const std::unique_ptr<SymTableNode>& nodePtr);
const std::unique_ptr<SymTableNode>&
searchSymTableDisplay(const std::wstring_view& name);
const std::unique_ptr<SymTableNode>&
enterSymTable(const std::wstring_view& name, const std::unique_ptr<SymTableNode>&* ptrToNodePtr);
const std::unique_ptr<SymTableNode>&
insertSymTable(const std::unique_ptr<SymTableNode>&* tableRoot, const std::unique_ptr<SymTableNode>& newNode);
const std::unique_ptr<SymTableNode>&
extractSymTable(const std::unique_ptr<SymTableNode>&* tableRoot, const std::unique_ptr<SymTableNode>& nodeKill);
void
enterStandardRoutine(const std::wstring_view& name, int32_t routineKey, bool isOrder, const std::wstring_view& paramList,
	const std::wstring_view& returnType, void (*callback)(void));
void
enterScope(const std::unique_ptr<SymTableNode>& symTableRoot);
const std::unique_ptr<SymTableNode>&
exitScope(void);
void
initSymTable(void);

const std::unique_ptr<Type>&
createType(void);
const std::unique_ptr<Type>&
setType(const std::unique_ptr<Type>& type);
void
clearType(const std::unique_ptr<Type>&& type);

//***************************************************************************

extern StandardFunctionInfo FunctionInfoTable[MAX_STANDARD_FUNCTIONS];
// extern PVOID					FunctionCallbackTable[MAX_STANDARD_FUNCTIONS];
extern void (*FunctionCallbackTable[MAX_STANDARD_FUNCTIONS])(void);
extern int32_t NumStandardFunctions;

} // namespace mclib::abl

#endif
