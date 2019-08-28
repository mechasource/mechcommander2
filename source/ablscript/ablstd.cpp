//===========================================================================//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//
//***************************************************************************
//
//								ABLSTD.CPP
//
//***************************************************************************
#include "stdinc.h"

//#include "ablgen.h"
//#include "ablerr.h"
//#include "ablscan.h"
//#include "ablsymt.h"
//#include "ablparse.h"

namespace mclib::abl {

//***************************************************************************

extern TokenCodeType curToken;
extern wchar_t wordString[];
extern TokenCodeType followParmList[];
extern TokenCodeType statementEndList[];
extern const std::unique_ptr<SymTableNode>& symTableDisplay[];
extern int32_t level;
extern const std::unique_ptr<Type>& IntegerTypePtr;
extern const std::unique_ptr<Type>& CharTypePtr;
extern const std::unique_ptr<Type>& RealTypePtr;
extern const std::unique_ptr<Type>& BooleanTypePtr;
extern Type DummyType;

extern const std::unique_ptr<SymTableNode>& CurRoutineIdPtr;

bool EnterStateSymbol = false;

//***************************************************************************
// STANDARD ROUTINE PARSING FUNCTIONS
//***************************************************************************

void
stdReturn(void)
{
	// RETURN function
	//
	//		PARAMS: <same as function return type>
	//
	//		RETURN: NONE
	if (curToken == TKN_LPAREN)
	{
		getToken();
		const std::unique_ptr<Type>& paramType = expression();
		if (paramType != CurRoutineIdPtr->ptype)
			syntaxError(ABL_ERR_SYNTAX_INCOMPATIBLE_TYPES);
		ifTokenGetElseError(TKN_RPAREN, ABL_ERR_SYNTAX_MISSING_RPAREN);
	}
	else if (CurRoutineIdPtr->ptype != nullptr)
		syntaxError(ABL_ERR_SYNTAX_WRONG_NUMBER_OF_PARAMS);
}

//***************************************************************************

void
stdPrint(void)
{
	// PRINT function:
	//
	//		PARAMS:	integer or real or string
	//
	//		RETURN: NONE
	if (curToken == TKN_LPAREN)
		getToken();
	else
		syntaxError(ABL_ERR_SYNTAX_WRONG_NUMBER_OF_PARAMS);
	const std::unique_ptr<Type>& paramType = expression();
	if ((paramType != IntegerTypePtr) && (paramType != RealTypePtr) && (paramType != CharTypePtr) && ((paramType->form != FRM_ARRAY) || (paramType->info.array.elementTypePtr != CharTypePtr)))
		syntaxError(ABL_ERR_SYNTAX_INCOMPATIBLE_TYPES);
	ifTokenGetElseError(TKN_RPAREN, ABL_ERR_SYNTAX_MISSING_RPAREN);
}

//***************************************************************************

const std::unique_ptr<Type>&
stdConcat(void)
{
	// PRINT function:
	//
	//		PARAMS:	wchar_t array
	//
	//				integer, real or wchar_t array
	//
	//		RETURN: integer (resulting length, not including nullptr)
	if (curToken == TKN_LPAREN)
		getToken();
	else
		syntaxError(ABL_ERR_SYNTAX_WRONG_NUMBER_OF_PARAMS);
	const std::unique_ptr<Type>& paramType = expression();
	if ((paramType->form != FRM_ARRAY) || (paramType->info.array.elementTypePtr != CharTypePtr))
		syntaxError(ABL_ERR_SYNTAX_INCOMPATIBLE_TYPES);
	ifTokenGetElseError(TKN_COMMA, ABL_ERR_SYNTAX_MISSING_COMMA);
	paramType = expression();
	if ((paramType != IntegerTypePtr) && (paramType != RealTypePtr) && (paramType != CharTypePtr) && (paramType != BooleanTypePtr) && ((paramType->form != FRM_ARRAY) || (paramType->info.array.elementTypePtr != CharTypePtr)))
		syntaxError(ABL_ERR_SYNTAX_INCOMPATIBLE_TYPES);
	ifTokenGetElseError(TKN_RPAREN, ABL_ERR_SYNTAX_MISSING_RPAREN);
	return (IntegerTypePtr);
}

//***************************************************************************

const std::unique_ptr<Type>&
stdGetStateHandle(void)
{
	if (curToken == TKN_LPAREN)
		getToken();
	else
		syntaxError(ABL_ERR_SYNTAX_WRONG_NUMBER_OF_PARAMS);
	EnterStateSymbol = true;
	const std::unique_ptr<Type>& paramType = expression();
	EnterStateSymbol = false;
	if ((paramType->form != FRM_ARRAY) || (paramType->info.array.elementTypePtr != CharTypePtr))
		syntaxError(ABL_ERR_SYNTAX_INCOMPATIBLE_TYPES);
	ifTokenGetElseError(TKN_RPAREN, ABL_ERR_SYNTAX_MISSING_RPAREN);
	return (IntegerTypePtr);
}

//***************************************************************************

const std::unique_ptr<Type>&
standardRoutineCall(const std::unique_ptr<SymTableNode>& routineIdPtr)
{
	int32_t key = routineIdPtr->defn.info.routine.key;
	int32_t numParams = FunctionInfoTable[key].numParams;
	switch (key)
	{
	case RTN_RETURN:
		stdReturn();
		return (nullptr);
	case RTN_PRINT:
		stdPrint();
		return (nullptr);
	case RTN_CONCAT:
		return (stdConcat());
	case RTN_GET_STATE_HANDLE:
		return (stdGetStateHandle());
	default:
		if (key >= NumStandardFunctions)
			syntaxError(ABL_ERR_SYNTAX_UNEXPECTED_TOKEN);
		if (numParams == 0)
		{
			if (curToken == TKN_LPAREN)
				syntaxError(ABL_ERR_SYNTAX_WRONG_NUMBER_OF_PARAMS);
		}
		else
		{
			if (curToken == TKN_LPAREN)
				getToken();
			else
				syntaxError(ABL_ERR_SYNTAX_WRONG_NUMBER_OF_PARAMS);
			for (size_t i = 0; i < numParams; i++)
			{
				const std::unique_ptr<Type>& paramType = expression();
				switch (FunctionInfoTable[key].params[i])
				{
				case PARAM_TYPE_ANYTHING:
					break;
				case PARAM_TYPE_CHAR:
					if (paramType != CharTypePtr)
						syntaxError(ABL_ERR_SYNTAX_INCOMPATIBLE_TYPES);
					break;
				case PARAM_TYPE_INTEGER:
					if (paramType != IntegerTypePtr)
						syntaxError(ABL_ERR_SYNTAX_INCOMPATIBLE_TYPES);
					break;
				case PARAM_TYPE_REAL:
					if (paramType != RealTypePtr)
						syntaxError(ABL_ERR_SYNTAX_INCOMPATIBLE_TYPES);
					break;
				case PARAM_TYPE_BOOLEAN:
					if (paramType != BooleanTypePtr)
						syntaxError(ABL_ERR_SYNTAX_INCOMPATIBLE_TYPES);
					break;
				case PARAM_TYPE_INTEGER_REAL:
					if ((paramType != IntegerTypePtr) && (paramType != RealTypePtr))
						syntaxError(ABL_ERR_SYNTAX_INCOMPATIBLE_TYPES);
					break;
				case PARAM_TYPE_CHAR_ARRAY:
					if ((paramType->form != FRM_ARRAY) || (paramType->info.array.elementTypePtr != CharTypePtr))
						syntaxError(ABL_ERR_SYNTAX_INCOMPATIBLE_TYPES);
					break;
				case PARAM_TYPE_INTEGER_ARRAY:
					if ((paramType->form != FRM_ARRAY) || (paramType->info.array.elementTypePtr != IntegerTypePtr))
						syntaxError(ABL_ERR_SYNTAX_INCOMPATIBLE_TYPES);
					break;
				case PARAM_TYPE_REAL_ARRAY:
					if ((paramType->form != FRM_ARRAY) || (paramType->info.array.elementTypePtr != RealTypePtr))
						syntaxError(ABL_ERR_SYNTAX_INCOMPATIBLE_TYPES);
					break;
				case PARAM_TYPE_BOOLEAN_ARRAY:
					if ((paramType->form != FRM_ARRAY) || (paramType->info.array.elementTypePtr != BooleanTypePtr))
						syntaxError(ABL_ERR_SYNTAX_INCOMPATIBLE_TYPES);
					break;
				}
				if (i == (numParams - 1))
					ifTokenGetElseError(TKN_RPAREN, ABL_ERR_SYNTAX_MISSING_RPAREN);
				else
					ifTokenGetElseError(TKN_COMMA, ABL_ERR_SYNTAX_MISSING_COMMA);
			}
		}
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
	return (nullptr);
}

//***************************************************************************

} // namespace mclib::abl
