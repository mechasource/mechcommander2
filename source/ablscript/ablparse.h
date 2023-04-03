//===========================================================================//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//
//***************************************************************************
//
// PARSER.H
//
//***************************************************************************

#pragma once

#ifndef ABLPARSE_H
#define ABLPARSE_H

//#include "ablgen.h"
//#include "ablsymt.h"
//#include "ablerr.h"

namespace mclib::abl
{

//***************************************************************************

enum class UseType : uint32_t
{
	USE_EXPR,
	USE_TARGET,
	USE_REFPARAM
} UseType;

enum class BlockType : uint32_t
{
	BLOCK_MODULE,
	BLOCK_ROUTINE
} BlockType;

//***************************************************************************

//----------
// FUNCTIONS
const std::unique_ptr<Type>&
expression(void);
const std::unique_ptr<Type>&
variable(const std::unique_ptr<SymTableNode>& variableIdPtr);
const std::unique_ptr<Type>&
arraySubscriptList(const std::unique_ptr<Type>& ptype);
// const std::unique_ptr<Type>& routineCall (const std::unique_ptr<SymTableNode>& routineIdPtr, BOOL parmCheckFlag);
void checkRelationalOpTypes(const std::unique_ptr<Type>& type1, const std::unique_ptr<Type>& type2);
int32_t
isAssignTypeCompatible(const std::unique_ptr<Type>& type1, const std::unique_ptr<Type>& type2);
void ifTokenGet(TokenCodeType tokenCode);
void ifTokenGetElseError(TokenCodeType tokenCode, SyntaxErrorType errCode);

// DECL routines
void declarations(const std::unique_ptr<SymTableNode>& routineIdPtr, bool allowFunctions);
void constDefinitions(void);
void doConst(const std::unique_ptr<SymTableNode>& constantIdPtr);
void varDeclarations(const std::unique_ptr<SymTableNode>& routineIdPtr);
void varOrFieldDeclarations(const std::unique_ptr<SymTableNode>& routineIdPtr, int32_t offset);
void typeDefinitions(void);
const std::unique_ptr<Type>&
doType(void);
const std::unique_ptr<Type>&
identifierType(const std::unique_ptr<SymTableNode>& idPtr);
const std::unique_ptr<Type>&
enumerationType(void);
const std::unique_ptr<Type>&
subrangeType(void);
const std::unique_ptr<Type>&
arrayType(void);
int32_t
arraySize(const std::unique_ptr<Type>& ptype);
const std::unique_ptr<Type>&
makeStringType(int32_t length);

// ROUTINE functions
void module(void);
const std::unique_ptr<SymTableNode>&
moduleHeader(void);
void routine(void);
const std::unique_ptr<SymTableNode>&
functionHeader(void);
const std::unique_ptr<SymTableNode>&
formalParamList(int32_t* count, int32_t* totalSize);
const std::unique_ptr<Type>&
routineCall(const std::unique_ptr<SymTableNode>& routineIdPtr, int32_t paramCheckFlag);
const std::unique_ptr<Type>&
declaredRoutineCall(const std::unique_ptr<SymTableNode>& routineIdPtr, int32_t paramCheckFlag);
void actualParamList(const std::unique_ptr<SymTableNode>& routineIdPtr, int32_t paramCheckFlag);
void block(const std::unique_ptr<SymTableNode>& routineIdPtr);

// STATEMNT routines
void compoundStatement(void);
void assignmentStatement(const std::unique_ptr<SymTableNode>& varIdPtr);
void repeatStatement(void);
void whileStatement(void);
void ifStatement(void);
void forStatement(void);
void switchStatement(void);
void transStatement(void);
void statement(void);

// STANDARD routines
void stdPrint(void);
const std::unique_ptr<Type>&
stdAbs(void);
const std::unique_ptr<Type>&
stdRound(void);
const std::unique_ptr<Type>&
stdTrunc(void);
const std::unique_ptr<Type>&
stdSqrt(void);
const std::unique_ptr<Type>&
stdRandom(void);
const std::unique_ptr<Type>&
standardRoutineCall(const std::unique_ptr<SymTableNode>& routineIdPtr);

// FILE routines
int32_t
openSourceFile(std::wstring_view sourceFileName);
int32_t
closeSourceFile(void);

#if !ANALYZE_ON
#define analyzeConstDefn(idPtr)
#define analyzeVarDecl(idPtr)
#define analyzeTypeDefn(idPtr)
#define analyzeRoutineHeader(idPtr)
#define analyzeBlock(idPtr)
#endif

//***************************************************************************

} // namespace mclib::abl

#endif
