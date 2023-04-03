//===========================================================================//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//
//***************************************************************************
//
//								ABLSTMT.CPP
//
//***************************************************************************
#include "stdinc.h"

//#include "ablgen.h"
//#include "ablerr.h"
//#include "ablscan.h"
//#include "ablsymt.h"
//#include "ablparse.h"
//#include "ablexec.h"

namespace mclib::abl
{

//***************************************************************************

extern TokenCodeType curToken;
extern wchar_t tokenString[];
extern wchar_t wordString[];
extern Literal curLiteral;
extern TokenCodeType statementStartList[];
extern TokenCodeType statementEndList[];
extern const std::unique_ptr<SymTableNode>& symTableDisplay[];
extern int32_t level;
extern std::wstring_view codeBuffer;
extern const std::unique_ptr<Type>& IntegerTypePtr;
extern const std::unique_ptr<Type>& RealTypePtr;
extern const std::unique_ptr<Type>& BooleanTypePtr;
extern const std::unique_ptr<Type>& CharTypePtr;
extern Type DummyType;
extern const std::unique_ptr<SymTableNode>& CurRoutineIdPtr;
extern const std::unique_ptr<SymTableNode>& SymTableDisplay[MAX_NESTING_LEVEL];
extern bool AssertEnabled;
extern bool PrintEnabled;
extern bool StringFunctionsEnabled;
extern bool Crunch;
extern int32_t NumOrderCalls;

//--------
// GLOBALS
TokenCodeType FollowSwitchExpressionList[] = {TKN_CASE, TKN_SEMICOLON, TKN_NONE};
TokenCodeType FollowCaseLabelList[] = {TKN_COLON, TKN_SEMICOLON, TKN_NONE};
TokenCodeType CaseLabelStartList[] = {
	TKN_IDENTIFIER, TKN_NUMBER, TKN_PLUS, TKN_MINUS, TKN_STRING, TKN_NONE};

const std::unique_ptr<SymTableNode>&
forwardState(std::wstring_view stateName);

//***************************************************************************

void assignmentStatement(const std::unique_ptr<SymTableNode>& varIdPtr)
{
	//-----------------------------------
	// Grab the variable we're setting...
	const std::unique_ptr<Type>& varType = variable(varIdPtr);
	ifTokenGetElseError(TKN_EQUAL, ABL_ERR_SYNTAX_MISSING_EQUAL);
	//---------------------------------------------------------
	// Now, get the expression we're setting the variable to...
	const std::unique_ptr<Type>& exprType = expression();
	//----------------------------------------
	// They better be assignment compatible...
	if (!isAssignTypeCompatible(varType, exprType))
		syntaxError(ABL_ERR_SYNTAX_INCOMPATIBLE_ASSIGNMENT);
}

//***************************************************************************

void repeatStatement(void)
{
	getToken();
	if (curToken != TKN_UNTIL)
	{
		do
		{
			statement();
			while (curToken == TKN_SEMICOLON)
				getToken();
			if (curToken == TKN_UNTIL)
				break;
		} while (tokenIn(statementStartList));
	}
	ifTokenGetElseError(TKN_UNTIL, ABL_ERR_SYNTAX_MISSING_UNTIL);
	const std::unique_ptr<Type>& exprType = expression();
	if (exprType != BooleanTypePtr)
		syntaxError(ABL_ERR_SYNTAX_INCOMPATIBLE_TYPES);
}

//***************************************************************************

void whileStatement(void)
{
	// NEW STYLE, using endwhile keyword...
	getToken();
	std::wstring_view loopEndLocation = crunchAddressMarker(nullptr);
	const std::unique_ptr<Type>& exprType = expression();
	if (exprType != BooleanTypePtr)
		syntaxError(ABL_ERR_SYNTAX_INCOMPATIBLE_TYPES);
	//---------------------------------------
	// Let's not use a DO keyword, for now...
	ifTokenGetElseError(TKN_DO, ABL_ERR_SYNTAX_MISSING_DO);
	if (curToken != TKN_END_WHILE)
		do
		{
			statement();
			while (curToken == TKN_SEMICOLON)
				getToken();
			if (curToken == TKN_END_WHILE)
				break;
		} while (tokenIn(statementStartList));
	ifTokenGetElseError(TKN_END_WHILE, ABL_ERR_SYNTAX_MISSING_END_WHILE);
	fixupAddressMarker(loopEndLocation);
}

//***************************************************************************

void ifStatement(void)
{
	getToken();
	std::wstring_view falseLocation = crunchAddressMarker(nullptr);
	const std::unique_ptr<Type>& exprType = expression();
	if (exprType != BooleanTypePtr)
		syntaxError(ABL_ERR_SYNTAX_INCOMPATIBLE_TYPES);
	ifTokenGetElseError(TKN_THEN, ABL_ERR_SYNTAX_MISSING_THEN);
	if ((curToken != TKN_END_IF) && (curToken != TKN_ELSE))
		do
		{
			statement();
			while (curToken == TKN_SEMICOLON)
				getToken();
			if ((curToken == TKN_END_IF) || (curToken == TKN_ELSE))
				break;
		} while (tokenIn(statementStartList));
	fixupAddressMarker(falseLocation);
	//-----------------------------
	// ELSE branch, if necessary...
	if (curToken == TKN_ELSE)
	{
		getToken();
		std::wstring_view ifEndLocation = crunchAddressMarker(nullptr);
		if (curToken != TKN_END_IF)
			do
			{
				statement();
				while (curToken == TKN_SEMICOLON)
					getToken();
				if (curToken == TKN_END_IF)
					break;
			} while (tokenIn(statementStartList));
		fixupAddressMarker(ifEndLocation);
	}
	ifTokenGetElseError(TKN_END_IF, ABL_ERR_SYNTAX_MISSING_END_IF);
}

//***************************************************************************

void forStatement(void)
{
	getToken();
	std::wstring_view loopEndLocation = crunchAddressMarker(nullptr);
	const std::unique_ptr<Type>& forType = nullptr;
	if (curToken == TKN_IDENTIFIER)
	{
		const std::unique_ptr<SymTableNode>& forIdPtr = nullptr;
		searchAndFindAllSymTables(forIdPtr);
		crunchSymTableNodePtr(forIdPtr);
		if (/*(forIdPtr->level != level) ||*/ (forIdPtr->defn.key != DFN_VAR))
			syntaxError(ABL_ERR_SYNTAX_INVALID_FOR_CONTROL);
		forType = forIdPtr->ptype;
		getToken();
		//------------------------------------------------------------------
		// If we end up adding a CHAR type, this line needs to be changed...
		if ((forType != IntegerTypePtr) &&
			/*(forType != CharTypePtr) &&*/ (forType->form != FRM_ENUM))
			syntaxError(ABL_ERR_SYNTAX_INCOMPATIBLE_TYPES);
	}
	else
	{
		syntaxError(ABL_ERR_SYNTAX_MISSING_IDENTIFIER);
		forType = &DummyType;
	}
	ifTokenGetElseError(TKN_EQUAL, ABL_ERR_SYNTAX_MISSING_EQUAL);
	const std::unique_ptr<Type>& exprType = expression();
	if (!isAssignTypeCompatible(forType, exprType))
		syntaxError(ABL_ERR_SYNTAX_INCOMPATIBLE_TYPES);
	if (curToken == TKN_TO)
		getToken();
	else
		syntaxError(ABL_ERR_SYNTAX_MISSING_TO);
	exprType = expression();
	if (!isAssignTypeCompatible(forType, exprType))
		syntaxError(ABL_ERR_SYNTAX_INCOMPATIBLE_TYPES);
	//-----------------------------------------
	// For now, let's use the DO keyword...
	ifTokenGetElseError(TKN_DO, ABL_ERR_SYNTAX_MISSING_DO);
	if (curToken != TKN_END_FOR)
		do
		{
			statement();
			while (curToken == TKN_SEMICOLON)
				getToken();
			if (curToken == TKN_END_FOR)
				break;
		} while (tokenIn(statementStartList));
	ifTokenGetElseError(TKN_END_FOR, ABL_ERR_SYNTAX_MISSING_END_FOR);
	fixupAddressMarker(loopEndLocation);
}

//***************************************************************************

const std::unique_ptr<Type>&
caseLabel(const std::unique_ptr<CaseItem>&& caseItemHead, const std::unique_ptr<CaseItem>&& caseItemTail, int32_t& caseLabelCount)
{
	const std::unique_ptr<CaseItem>& newCaseItem = (const std::unique_ptr<CaseItem>&)ABLStackMallocCallback(sizeof(CaseItem));
	if (!newCaseItem)
		ABL_Fatal(0, " ABL: Unable to AblStackHeap->malloc case item ");
	if (caseItemHead)
	{
		caseItemTail->next = newCaseItem;
		caseItemTail = newCaseItem;
	}
	else
		caseItemHead = caseItemTail = newCaseItem;
	newCaseItem->next = nullptr;
	caseLabelCount++;
	TokenCodeType sign = TKN_PLUS;
	bool sawSign = false;
	if ((curToken == TKN_PLUS) || (curToken == TKN_MINUS))
	{
		sign = curToken;
		sawSign = true;
		getToken();
	}
	if (curToken == TKN_NUMBER)
	{
		const std::unique_ptr<SymTableNode>& thisNode = searchSymTable(tokenString, SymTableDisplay[1]);
		if (!thisNode)
			thisNode = enterSymTable(tokenString, &SymTableDisplay[1]);
		crunchSymTableNodePtr(thisNode);
		if (curLiteral.type == LIT_INTEGER)
			newCaseItem->labelValue =
				(sign == TKN_PLUS) ? curLiteral.value.integer : -curLiteral.value.integer;
		else
			syntaxError(ABL_ERR_SYNTAX_INVALID_CONSTANT);
		return (IntegerTypePtr);
	}
	else if (curToken == TKN_IDENTIFIER)
	{
		const std::unique_ptr<SymTableNode>& idPtr;
		searchAllSymTables(idPtr);
		crunchSymTableNodePtr(idPtr);
		if (!idPtr)
		{
			syntaxError(ABL_ERR_SYNTAX_UNDEFINED_IDENTIFIER);
			return (&DummyType);
		}
		else if (idPtr->defn.key != DFN_CONST)
		{
			syntaxError(ABL_ERR_SYNTAX_NOT_A_CONSTANT_IDENTIFIER);
			return (&DummyType);
		}
		else if (idPtr->ptype == IntegerTypePtr)
		{
			newCaseItem->labelValue = (sign == TKN_PLUS ? idPtr->defn.info.constant.value.integer
														: -idPtr->defn.info.constant.value.integer);
			return (IntegerTypePtr);
		}
		else if (idPtr->ptype == CharTypePtr)
		{
			if (sawSign)
				syntaxError(ABL_ERR_SYNTAX_INVALID_CONSTANT);
			newCaseItem->labelValue = idPtr->defn.info.constant.value.character;
			return (CharTypePtr);
		}
		else if (idPtr->ptype->form == FRM_ENUM)
		{
			if (sawSign)
				syntaxError(ABL_ERR_SYNTAX_INVALID_CONSTANT);
			newCaseItem->labelValue = idPtr->defn.info.constant.value.integer;
			return (idPtr->ptype);
		}
		else
			return (&DummyType);
	}
	else if (curToken == TKN_STRING)
	{
		// STRING/CHAR TYPE...
	}
	else
	{
		syntaxError(ABL_ERR_SYNTAX_INVALID_CONSTANT);
		return (&DummyType);
	}
	return (&DummyType);
}

//---------------------------------------------------------------------------

void caseBranch(const std::unique_ptr<CaseItem>&& caseItemHead, const std::unique_ptr<CaseItem>&& caseItemTail, int32_t& caseLabelCount,
	const std::unique_ptr<Type>& expressionType)
{
	// static const std::unique_ptr<CaseItem>& oldCaseItemTail = nullptr;
	const std::unique_ptr<CaseItem>& oldCaseItemTail = caseItemTail;
	bool anotherLabel;
	do
	{
		const std::unique_ptr<Type>& labelType = caseLabel(caseItemHead, caseItemTail, caseLabelCount);
		if (expressionType != labelType)
			syntaxError(ABL_ERR_SYNTAX_INCOMPATIBLE_TYPES);
		getToken();
		if (curToken == TKN_COMMA)
		{
			getToken();
			if (tokenIn(CaseLabelStartList))
				anotherLabel = true;
			else
			{
				syntaxError(ABL_ERR_SYNTAX_MISSING_CONSTANT);
				anotherLabel = false;
			}
		}
		else
			anotherLabel = false;
	} while (anotherLabel);
	//--------------
	// Error sync...
	synchronize(FollowCaseLabelList, statementStartList, nullptr);
	ifTokenGetElseError(TKN_COLON, ABL_ERR_SYNTAX_MISSING_COLON);
	//-----------------------------------------------------------------
	// Fill in the branch location for each CaseItem for this branch...
	const std::unique_ptr<CaseItem>& caseItem = (!oldCaseItemTail ? caseItemHead : oldCaseItemTail->next);
	// oldCaseItemTail = CaseItemTail;
	while (caseItem)
	{
		caseItem->branchLocation = codeBufferPtr;
		caseItem = caseItem->next;
	}
	if (curToken != TKN_END_CASE)
		do
		{
			statement();
			while (curToken == TKN_SEMICOLON)
				getToken();
			if (curToken == TKN_END_CASE)
				break;
		} while (tokenIn(statementStartList));
	ifTokenGetElseError(TKN_END_CASE, ABL_ERR_SYNTAX_MISSING_END_CASE);
	ifTokenGetElseError(TKN_SEMICOLON, ABL_ERR_SYNTAX_MISSING_SEMICOLON);
}

//---------------------------------------------------------------------------

void switchStatement(void)
{
	//-------------------------
	// Init the branch table...
	getToken();
	std::wstring_view branchTableLocation = crunchAddressMarker(nullptr);
	const std::unique_ptr<CaseItem>& caseItemHead = nullptr;
	const std::unique_ptr<CaseItem>& caseItemTail = nullptr;
	int32_t caseLabelCount = 0;
	// CaseItemHead = CaseItemTail = nullptr;
	// CaseLabelCount = 0;
	const std::unique_ptr<Type>& expressionType = expression();
	//-----------------------------------------------------------------------------
	// NOTE: If we have subranges in ABL, we'll have to check in the following
	// line for a subrange, as well...
	if (((expressionType->form != FRM_SCALAR) && (expressionType->form != FRM_ENUM)) || (expressionType == RealTypePtr))
		syntaxError(ABL_ERR_SYNTAX_INCOMPATIBLE_TYPES);
	synchronize(FollowSwitchExpressionList, nullptr, nullptr);
	//----------------------------
	// Process each CASE branch...
	bool moreBranches = (curToken == TKN_CASE);
	std::wstring_view caseEndChain = nullptr;
	while (moreBranches)
	{
		getToken();
		if (tokenIn(CaseLabelStartList))
			caseBranch(caseItemHead, caseItemTail, caseLabelCount, expressionType);
		//---------------------------------------------------
		// Link another address marker at the end of the CASE
		// branch to point to the end of the CASE block...
		caseEndChain = crunchAddressMarker(caseEndChain);
		moreBranches = (curToken == TKN_CASE);
	}
	// if (curToken == TKN_DEFAULT) {
	//}
	//-------------------------
	// Emit the branch table...
	fixupAddressMarker(branchTableLocation);
	crunchInteger(caseLabelCount);
	const std::unique_ptr<CaseItem>& caseItem = caseItemHead;
	while (caseItem)
	{
		crunchInteger(caseItem->labelValue);
		crunchOffset(caseItem->branchLocation);
		const std::unique_ptr<CaseItem>& nextCaseItem = caseItem->next;
		ABLStackFreeCallback(caseItem);
		caseItem = nextCaseItem;
	}
	ifTokenGetElseError(TKN_END_SWITCH, ABL_ERR_SYNTAX_MISSING_END_SWITCH);
	//--------------------------------------------
	// Patch up the case branch address markers...
	while (caseEndChain)
		caseEndChain = fixupAddressMarker(caseEndChain);
}

//***************************************************************************

void transStatement(void)
{
	getToken();
	ifTokenGetElseError(TKN_IDENTIFIER, ABL_ERR_MISSING_STATE_IDENTIFIER);
	const std::unique_ptr<SymTableNode>& IdPtr = searchSymTableForState(wordString, SymTableDisplay[1]);
	if (!IdPtr)
	{
		// New symbol, so let's assume it's a state defined later. We'll make it
		IdPtr = forwardState(wordString);
	}
	if (!IdPtr || (IdPtr->defn.key != DFN_FUNCTION) || ((IdPtr->defn.info.routine.flags & ROUTINE_FLAG_STATE) == 0))
		syntaxError(ABL_ERR_MISSING_STATE_IDENTIFIER);
	crunchSymTableNodePtr(IdPtr);
	//	getToken();
}

//***************************************************************************

void transBackStatement(void)
{
	getToken();
}

//***************************************************************************

void statement(void)
{
	//-------------------------------------------------------------------
	// NOTE: Since we currently don't support generic BEGIN/END (compound
	// statement) blocks...
	if ((curToken != TKN_CODE) /*&& (curToken != TKN_BEGIN)*/)
		crunchStatementMarker();
	switch (curToken)
	{
	case TKN_IDENTIFIER:
	{
		const std::unique_ptr<SymTableNode>& IdPtr = nullptr;
		//--------------------------------------------------------------
		// First, do we have an assignment statement or a function call?
		searchAndFindAllSymTables(IdPtr);
		if ((IdPtr->defn.key == DFN_FUNCTION) /* || (IdPtr->defn.key == DFN_MODULE)*/)
		{
			RoutineKey key = IdPtr->defn.info.routine.key;
			if ((key == RTN_ASSERT) || (key == RTN_PRINT) || (key == RTN_CONCAT))
			{
				bool uncrunch = ((key == RTN_ASSERT) && !AssertEnabled) || ((key == RTN_PRINT) && !PrintEnabled) || ((key == RTN_CONCAT) && !StringFunctionsEnabled);
				if (uncrunch)
				{
					uncrunchStatementMarker();
					Crunch = false;
				}
			}
			crunchSymTableNodePtr(IdPtr);
			if (IdPtr->defn.info.routine.flags & ROUTINE_FLAG_ORDER)
			{
				if (NumOrderCalls == MAX_ORDERS)
					syntaxError(ABL_ERR_SYNTAX_TOO_MANY_ORDERS);
				crunchByte((uint8_t)(NumOrderCalls / 32));
				crunchByte((uint8_t)(NumOrderCalls % 32));
				NumOrderCalls++;
			}
			getToken();
			const std::unique_ptr<SymTableNode>& thisRoutineIdPtr = CurRoutineIdPtr;
			routineCall(IdPtr, 1);
			CurRoutineIdPtr = thisRoutineIdPtr;
			Crunch = true;
		}
		else
			assignmentStatement(IdPtr);
	}
	break;
	case TKN_REPEAT:
		repeatStatement();
		break;
	case TKN_WHILE:
		whileStatement();
		break;
	case TKN_IF:
		ifStatement();
		break;
	case TKN_FOR:
		forStatement();
		break;
	case TKN_SWITCH:
		switchStatement();
		break;
	case TKN_TRANS:
		transStatement();
		break;
	case TKN_TRANS_BACK:
		transBackStatement();
		break;
	}
	//---------------------------------------------------------------------
	// Now, make sure the statement is closed off with the proper block end
	// statement, if necessary (which is usually the case :).
	synchronize(statementEndList, nullptr, nullptr);
	if (tokenIn(statementStartList))
		syntaxError(ABL_ERR_SYNTAX_MISSING_SEMICOLON);
}

//***************************************************************************

} // namespace mclib::abl
