//===========================================================================//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//
//***************************************************************************
//
//								ABLEXPR.CPP
//
//***************************************************************************
#include "stdinc.h"

//#include "ablgen.h"
//#include "ablerr.h"
//#include "ablscan.h"
//#include "ablsymt.h"
//#include "ablparse.h"
//#include "ablexec.h"
//#include "ablenv.h"

namespace mclib::abl
{

//***************************************************************************

extern TokenCodeType curToken;
extern wchar_t tokenString[];
extern wchar_t wordString[];
extern Literal curLiteral;

extern const std::unique_ptr<SymTableNode>& SymTableDisplay[];
extern int32_t level;

extern const std::unique_ptr<Type>&IntegerTypePtr, CharTypePtr, RealTypePtr, BooleanTypePtr;
extern Type DummyType;

extern TokenCodeType statementEndList[];

extern bool EnterStateSymbol;
extern const std::unique_ptr<ABLModule>& CurFSM;
const std::unique_ptr<SymTableNode>&
forwardState(std::wstring_view stateName);
extern const std::unique_ptr<SymTableNode>& CurModuleIdPtr;

//***************************************************************************

TokenCodeType relationalOperatorList[] = {
	TKN_LT, TKN_LE, TKN_EQUALEQUAL, TKN_NE, TKN_GE, TKN_GT, TKN_NONE};

TokenCodeType addOperatorList[] = {TKN_PLUS, TKN_MINUS, TKN_OR, TKN_NONE};

TokenCodeType multiplyOperatorList[] = {TKN_STAR, TKN_FSLASH,
	TKN_DIV, // we'll probably want to make this covered with FSLASH
	TKN_MOD, TKN_AND, TKN_NONE};

//***************************************************************************
// MISC
//***************************************************************************

inline bool
integerOperands(const std::unique_ptr<Type>& type1, const std::unique_ptr<Type>& type2)
{
	return ((type1 == IntegerTypePtr) && (type2 == IntegerTypePtr));
}

//***************************************************************************

inline bool
realOperands(const std::unique_ptr<Type>& type1, const std::unique_ptr<Type>& type2)
{
	if (type1 == RealTypePtr)
		return ((type2 == RealTypePtr) || (type2 == IntegerTypePtr));
	else if (type2 == RealTypePtr)
		return (type1 == IntegerTypePtr);
	else
		return (false);
}

//***************************************************************************

inline bool
booleanOperands(const std::unique_ptr<Type>& type1, const std::unique_ptr<Type>& type2)
{
	return ((type1 == BooleanTypePtr) && (type2 == BooleanTypePtr));
}

//***************************************************************************

void checkRelationalOpTypes(const std::unique_ptr<Type>& type1, const std::unique_ptr<Type>& type2)
{
	if (type1 && type2)
	{
		if ((type1 == type2) && ((type1->form == FRM_SCALAR) || (type1->form == FRM_ENUM)))
			return;
		if (((type1 == IntegerTypePtr) && (type2 == RealTypePtr)) || ((type2 == IntegerTypePtr) && (type1 == RealTypePtr)))
			return;
		if ((type1->form == FRM_ARRAY) && (type2->form == FRM_ARRAY) && (type1->info.array.elementTypePtr == CharTypePtr) && (type2->info.array.elementTypePtr == CharTypePtr) && (type1->info.array.elementCount == type2->info.array.elementCount))
			return;
	}
	syntaxError(ABL_ERR_SYNTAX_INCOMPATIBLE_TYPES);
}

//***************************************************************************

int32_t
isAssignTypeCompatible(const std::unique_ptr<Type>& type1, const std::unique_ptr<Type>& type2)
{
	if (type1 == type2)
		return (1);
	if ((type1 == RealTypePtr) && (type2 == IntegerTypePtr))
		return (1);
	if ((type1->form == FRM_ARRAY) && (type2->form == FRM_ARRAY) && (type1->info.array.elementTypePtr == CharTypePtr) && (type2->info.array.elementTypePtr == CharTypePtr) && (type1->info.array.elementCount >= type2->info.array.elementCount))
		return (1);
	return (0);
}

//***************************************************************************
// EXPRESSION routines
//***************************************************************************

const std::unique_ptr<Type>&
variable(const std::unique_ptr<SymTableNode>& variableIdPtr)
{
	const std::unique_ptr<Type>& ptype = (const std::unique_ptr<Type>&)(variableIdPtr->ptype);
	DefinitionType defnKey = variableIdPtr->defn.key;
	crunchSymTableNodePtr(variableIdPtr);
	switch (defnKey)
	{
	case DFN_VAR:
	case DFN_VALPARAM:
	case DFN_REFPARAM:
	case DFN_FUNCTION:
	case DFN_UNDEFINED:
		break;
	default:
		// ptype = &DummyType;
		// syntaxError(ABL_ERR_SYNTAX_INVALID_IDENTIFIER_USAGE);
		NODEFAULT;
	}
	getToken();
	//---------------------------------------------------------------------
	// There should not be a parameter list. However, if there is, parse it
	// for error recovery...
	if (curToken == TKN_LPAREN)
	{
		syntaxError(ABL_ERR_SYNTAX_UNEXPECTED_TOKEN);
		actualParamList(variableIdPtr, 0);
		return (ptype);
	}
	//-----------
	// Subscripts
	while (curToken == TKN_LBRACKET)
	{
		if (curToken == TKN_LBRACKET)
			ptype = arraySubscriptList(ptype);
	}
	return (ptype);
}

//***************************************************************************

const std::unique_ptr<Type>&
arraySubscriptList(const std::unique_ptr<Type>& ptype)
{
	const std::unique_ptr<Type>& indexTypePtr = nullptr;
	const std::unique_ptr<Type>& elementTypePtr = nullptr;
	const std::unique_ptr<Type>& subscriptTypePtr = nullptr;
	do
	{
		if (ptype->form == FRM_ARRAY)
		{
			indexTypePtr = ptype->info.array.indexTypePtr;
			elementTypePtr = ptype->info.array.elementTypePtr;
			getToken();
			subscriptTypePtr = expression();
			//-------------------------------------------------------------
			// If the subscript expression isn't assignment type compatible
			// with its corresponding subscript type, we're screwed...
			if (!isAssignTypeCompatible(indexTypePtr, subscriptTypePtr))
				syntaxError(ABL_ERR_SYNTAX_INCOMPATIBLE_TYPES);
			ptype = elementTypePtr;
		}
		else
		{
			syntaxError(ABL_ERR_SYNTAX_TOO_MANY_SUBSCRIPTS);
			while ((curToken != TKN_RBRACKET) && !tokenIn(statementEndList))
				getToken();
		}
	} while (curToken == TKN_COMMA);
	ifTokenGetElseError(TKN_RBRACKET, ABL_ERR_SYNTAX_MISSING_RBRACKET);
	return (ptype);
}

//***************************************************************************

const std::unique_ptr<Type>&
factor(void)
{
	const std::unique_ptr<Type>& thisType = nullptr;
	switch (curToken)
	{
	case TKN_IDENTIFIER:
	{
		const std::unique_ptr<SymTableNode>& IdPtr = nullptr;
		searchAndFindAllSymTables(IdPtr);
		switch (IdPtr->defn.key)
		{
		case DFN_FUNCTION:
			crunchSymTableNodePtr(IdPtr);
			getToken();
			thisType = routineCall(IdPtr, 1);
			break;
		case DFN_CONST:
			crunchSymTableNodePtr(IdPtr);
			getToken();
			thisType = (const std::unique_ptr<Type>&)(IdPtr->ptype);
			break;
		default:
			thisType = (const std::unique_ptr<Type>&)variable(IdPtr);
			break;
		}
	}
	break;
	case TKN_NUMBER:
	{
		const std::unique_ptr<SymTableNode>& thisNode = searchSymTable(tokenString, SymTableDisplay[1]);
		if (!thisNode)
			thisNode = enterSymTable(tokenString, &SymTableDisplay[1]);
		if (curLiteral.type == LIT_INTEGER)
		{
			thisNode->ptype = IntegerTypePtr;
			thisType = (const std::unique_ptr<Type>&)(thisNode->ptype);
			thisNode->defn.info.constant.value.integer = curLiteral.value.integer;
		}
		else
		{
			thisNode->ptype = RealTypePtr;
			thisType = (const std::unique_ptr<Type>&)(thisNode->ptype);
			thisNode->defn.info.constant.value.real = curLiteral.value.real;
		}
		crunchSymTableNodePtr(thisNode);
		getToken();
	}
	break;
	case TKN_STRING:
	{
		int32_t length = strlen(curLiteral.value.string);
		if (EnterStateSymbol)
		{
			const std::unique_ptr<SymTableNode>& stateSymbol =
				searchSymTableForState(curLiteral.value.string, SymTableDisplay[1]);
			if (!stateSymbol)
				forwardState(curLiteral.value.string);
		}
		const std::unique_ptr<SymTableNode>& thisNode = searchSymTableForString(tokenString, SymTableDisplay[1]);
		if (!thisNode) // {
			thisNode = enterSymTable(tokenString, &SymTableDisplay[1]);
		if (length == 1)
		{
			thisNode->defn.info.constant.value.character = curLiteral.value.string[0];
			thisType = CharTypePtr;
		}
		else
		{
			thisNode->ptype = thisType = makeStringType(length);
			thisNode->info = (std::wstring_view)ABLSymbolMallocCallback(length + 1);
			if (!thisNode->info)
				ABL_Fatal(0, " ABL: Unable to AblSymTableHeap->malloc string literal ");
			strcpy(thisNode->info, curLiteral.value.string);
		}
		//}
		crunchSymTableNodePtr(thisNode);
		getToken();
	}
	break;
	case TKN_NOT:
		getToken();
		thisType = factor();
		break;
	case TKN_LPAREN:
		getToken();
		thisType = expression();
		ifTokenGetElseError(TKN_RPAREN, ABL_ERR_SYNTAX_MISSING_RPAREN);
		break;
	default:
		// syntaxError(ABL_ERR_SYNTAX_INVALID_EXPRESSION);
		// thisType = &DummyType;
		// break;
		NODEFAULT;
	}
	return (thisType);
}

//***************************************************************************

const std::unique_ptr<Type>&
term(void)
{
	//-------------------------
	// Grab the first factor...
	const std::unique_ptr<Type>& resultType = factor();
	//------------------------------------------------------------------
	// Now, continue grabbing factors separated by multiply operators...
	while (tokenIn(multiplyOperatorList))
	{
		TokenCodeType op = curToken;
		getToken();
		const std::unique_ptr<Type>& secondType = factor();
		switch (op)
		{
		case TKN_STAR:
			if (integerOperands(resultType, secondType))
			{
				//---------------------------------------------------
				// Both operands are integer, so result is integer...
				resultType = IntegerTypePtr;
			}
			else if (realOperands(resultType, secondType))
			{
				//----------------------------------------------------
				// Both real operands, or mixed (real and integer)...
				resultType = RealTypePtr;
			}
			else
			{
				syntaxError(ABL_ERR_SYNTAX_INCOMPATIBLE_TYPES);
				resultType = &DummyType;
			}
			break;
		case TKN_FSLASH:
			if (integerOperands(resultType, secondType))
			{
				//---------------------------------------------------
				// Both operands are integer, so result is integer...
				resultType = IntegerTypePtr;
			}
			else if (realOperands(resultType, secondType))
			{
				//----------------------------------------------------
				// Both real operands, or mixed (real and integer)...
				resultType = RealTypePtr;
			}
			else
			{
				syntaxError(ABL_ERR_SYNTAX_INCOMPATIBLE_TYPES);
				resultType = &DummyType;
			}
			break;
		case TKN_DIV:
		case TKN_MOD:
			//----------------------------------------------------------
			// Both operands should be integer, and result is integer...
			if (!integerOperands(resultType, secondType))
				syntaxError(ABL_ERR_SYNTAX_INCOMPATIBLE_TYPES);
			resultType = IntegerTypePtr;
			break;
		case TKN_AND:
			if (!booleanOperands(resultType, secondType))
				syntaxError(ABL_ERR_SYNTAX_INCOMPATIBLE_TYPES);
			resultType = BooleanTypePtr;
			break;
		}
	}
	return (resultType);
}

//***************************************************************************

const std::unique_ptr<Type>&
simpleExpression(void)
{
	bool usedUnaryOp = false;
	TokenCodeType unaryOp = TKN_PLUS;
	if ((curToken == TKN_PLUS) || (curToken == TKN_MINUS))
	{
		unaryOp = curToken;
		usedUnaryOp = true;
		getToken();
	}
	//------------------------------------------------
	// Grab the first term in the simple expression...
	const std::unique_ptr<Type>& resultType = term();
	if (usedUnaryOp && (resultType != IntegerTypePtr) && (resultType != RealTypePtr))
		syntaxError(ABL_ERR_SYNTAX_INCOMPATIBLE_TYPES);
	//---------------------------------------------------
	// Continue to process all terms in the expression...
	while (tokenIn(addOperatorList))
	{
		TokenCodeType op = curToken;
		getToken();
		const std::unique_ptr<Type>& secondType = term();
		switch (op)
		{
		case TKN_PLUS:
		case TKN_MINUS:
			if (integerOperands(resultType, secondType))
			{
				//---------------------------------------------------
				// Both operands are integer, so result is integer...
				resultType = IntegerTypePtr;
			}
			else if (realOperands(resultType, secondType))
			{
				//----------------------------------------------------
				// Both real operands, or mixed (real and integer)...
				resultType = RealTypePtr;
			}
			else
			{
				syntaxError(ABL_ERR_SYNTAX_INCOMPATIBLE_TYPES);
				resultType = &DummyType;
			}
			break;
		case TKN_OR:
			if (!booleanOperands(resultType, secondType))
				syntaxError(ABL_ERR_SYNTAX_INCOMPATIBLE_TYPES);
			resultType = BooleanTypePtr;
			break;
		}
	}
	return (resultType);
}

//***************************************************************************

const std::unique_ptr<Type>&
expression(void)
{
	//------------------------------------
	// Grab the first simple expression...
	const std::unique_ptr<Type>& resultType = simpleExpression();
	if (tokenIn(relationalOperatorList))
	{
		//---------------------------------------
		// Snatch the second simple expression...
		getToken();
		const std::unique_ptr<Type>& secondType = simpleExpression();
		checkRelationalOpTypes(resultType, secondType);
		resultType = BooleanTypePtr;
	}
	return (resultType);
}

//***************************************************************************

} // namespace mclib::abl
