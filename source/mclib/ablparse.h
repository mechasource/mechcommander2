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

//***************************************************************************

typedef enum UseType : uint32_t
{
	USE_EXPR,
	USE_TARGET,
	USE_REFPARAM
} UseType;

typedef enum BlockType : uint32_t
{
	BLOCK_MODULE,
	BLOCK_ROUTINE
} BlockType;

//***************************************************************************

//----------
// FUNCTIONS
TypePtr
expression(void);
TypePtr
variable(SymTableNodePtr variableIdPtr);
TypePtr
arraySubscriptList(TypePtr typePtr);
// TypePtr routineCall (SymTableNodePtr routineIdPtr, BOOL parmCheckFlag);
void
checkRelationalOpTypes(TypePtr type1, TypePtr type2);
int32_t
isAssignTypeCompatible(TypePtr type1, TypePtr type2);
void
ifTokenGet(TokenCodeType tokenCode);
void
ifTokenGetElseError(TokenCodeType tokenCode, SyntaxErrorType errCode);

// DECL routines
void
declarations(SymTableNodePtr routineIdPtr, bool allowFunctions);
void
constDefinitions(void);
void
doConst(SymTableNodePtr constantIdPtr);
void
varDeclarations(SymTableNodePtr routineIdPtr);
void
varOrFieldDeclarations(SymTableNodePtr routineIdPtr, int32_t offset);
void
typeDefinitions(void);
TypePtr
doType(void);
TypePtr
identifierType(SymTableNodePtr idPtr);
TypePtr
enumerationType(void);
TypePtr
subrangeType(void);
TypePtr
arrayType(void);
int32_t
arraySize(TypePtr typePtr);
TypePtr
makeStringType(int32_t length);

// ROUTINE functions
void
module(void);
SymTableNodePtr
moduleHeader(void);
void
routine(void);
SymTableNodePtr
functionHeader(void);
SymTableNodePtr
formalParamList(int32_t* count, int32_t* totalSize);
TypePtr
routineCall(SymTableNodePtr routineIdPtr, int32_t paramCheckFlag);
TypePtr
declaredRoutineCall(SymTableNodePtr routineIdPtr, int32_t paramCheckFlag);
void
actualParamList(SymTableNodePtr routineIdPtr, int32_t paramCheckFlag);
void
block(SymTableNodePtr routineIdPtr);

// STATEMNT routines
void
compoundStatement(void);
void
assignmentStatement(SymTableNodePtr varIdPtr);
void
repeatStatement(void);
void
whileStatement(void);
void
ifStatement(void);
void
forStatement(void);
void
switchStatement(void);
void
transStatement(void);
void
statement(void);

// STANDARD routines
void
stdPrint(void);
TypePtr
stdAbs(void);
TypePtr
stdRound(void);
TypePtr
stdTrunc(void);
TypePtr
stdSqrt(void);
TypePtr
stdRandom(void);
TypePtr
standardRoutineCall(SymTableNodePtr routineIdPtr);

// FILE routines
int32_t
openSourceFile(PSTR sourceFileName);
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

#endif
