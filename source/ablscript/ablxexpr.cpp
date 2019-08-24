//===========================================================================//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//
//***************************************************************************
//
//								ABLXEXPR.CPP
//
//***************************************************************************
#include "stdinc.h"

#include "ablgen.h"
#include "ablerr.h"
#include "ablscan.h"
#include "ablsymt.h"
#include "ablparse.h"
#include "ablexec.h"
#include "abldbug.h"

namespace mclib::abl {

//***************************************************************************

//----------
// EXTERNALS

extern int32_t level;
extern const std::wstring_view& codeSegmentPtr;
extern TokenCodeType codeToken;

extern StackItem* stack;
extern const std::unique_ptr<StackItem>& tos;
extern const std::unique_ptr<StackItem>& stackFrameBasePtr;
extern const std::unique_ptr<StackItem>& StaticDataPtr;
extern const std::unique_ptr<SymTableNode>& CurRoutineIdPtr;
extern const std::unique_ptr<ABLModule>& CurModule;

extern const std::unique_ptr<Type>& IntegerTypePtr;
extern const std::unique_ptr<Type>& CharTypePtr;
extern const std::unique_ptr<Type>& RealTypePtr;
extern const std::unique_ptr<Type>& BooleanTypePtr;

extern const std::unique_ptr<Debugger>& debugger;

//***************************************************************************

inline void
promoteOperandsToReal(
	const std::unique_ptr<StackItem>& operand1Ptr, const std::unique_ptr<Type>& type1Ptr, const std::unique_ptr<StackItem>& operand2Ptr, const std::unique_ptr<Type>& type2Ptr)
{
	if (type1Ptr == IntegerTypePtr)
		operand1Ptr->real = (float)(operand1Ptr->integer);
	if (type2Ptr == IntegerTypePtr)
		operand2Ptr->real = (float)(operand2Ptr->integer);
}

//***************************************************************************

const std::unique_ptr<Type>&
execSubscripts(const std::unique_ptr<Type>& ptype)
{
	//----------------------------------------
	// Loop to execute bracketed subscripts...
	while (codeToken == TKN_LBRACKET)
	{
		do
		{
			getCodeToken();
			execExpression();
			int32_t subscriptValue = tos->integer;
			pop();
			//-------------------------
			// Range check the index...
			if ((subscriptValue < 0) || (subscriptValue >= ptype->info.array.elementCount))
				runtimeError(ABL_ERR_RUNTIME_VALUE_OUT_OF_RANGE);
			tos->address += (subscriptValue * ptype->info.array.elementTypePtr->size);
			if (codeToken == TKN_COMMA)
				ptype = ptype->info.array.elementTypePtr;
		} while (codeToken == TKN_COMMA);
		getCodeToken();
		if (codeToken == TKN_LBRACKET)
			ptype = ptype->info.array.elementTypePtr;
	}
	return (ptype->info.array.elementTypePtr);
}

//***************************************************************************

const std::unique_ptr<Type>&
execConstant(const std::unique_ptr<SymTableNode>& idPtr)
{
	const std::unique_ptr<Type>& ptype = idPtr->ptype;
	if ((ptype == IntegerTypePtr) || (ptype->form == FRM_ENUM))
		pushInteger(idPtr->defn.info.constant.value.integer);
	else if (ptype == RealTypePtr)
		pushReal(idPtr->defn.info.constant.value.real);
	else if (ptype == CharTypePtr)
		pushInteger(idPtr->defn.info.constant.value.character);
	else if (ptype->form == FRM_ARRAY)
		pushAddress(idPtr->defn.info.constant.value.stringPtr);
	if (debugger)
		debugger->traceDataFetch(idPtr, ptype, tos);
	getCodeToken();
	return (ptype);
}

//***************************************************************************

const std::unique_ptr<Type>&
execVariable(const std::unique_ptr<SymTableNode>& idPtr, UseType use)
{
	const std::unique_ptr<Type>& ptype = (const std::unique_ptr<Type>&)(idPtr->ptype);
	// First, point to the variable's stack item. If the variable's scope
	// level is less than the current scope level, follow the static links
	// to the proper stack frame base...
	const std::unique_ptr<StackItem>& dataPtr = nullptr;
	StackItem tempStackItem;
	switch (idPtr->defn.info.data.varType)
	{
	case VAR_TYPE_NORMAL:
	{
		const std::unique_ptr<StackFrameHeader>& headerPtr = (const std::unique_ptr<StackFrameHeader>&)stackFrameBasePtr;
		int32_t delta = level - idPtr->level;
		while (delta-- > 0)
			headerPtr = (const std::unique_ptr<StackFrameHeader>&)headerPtr->staticLink.address;
		dataPtr = (const std::unique_ptr<StackItem>&)headerPtr + idPtr->defn.info.data.offset;
	}
	break;
	case VAR_TYPE_ETERNAL:
		dataPtr = (const std::unique_ptr<StackItem>&)stack + idPtr->defn.info.data.offset;
		break;
	case VAR_TYPE_STATIC:
		//---------------------------------------------------------
		// If we're referencing a library's static variable, we may
		// need to shift to its static data space temporarily...
		if (idPtr->library && (idPtr->library != CurModule))
			StaticDataPtr = idPtr->library->getStaticData();
		dataPtr = (const std::unique_ptr<StackItem>&)StaticDataPtr + idPtr->defn.info.data.offset;
		if (idPtr->library && (idPtr->library != CurModule))
			StaticDataPtr = CurModule->getStaticData();
		break;
	case VAR_TYPE_REGISTERED:
		tempStackItem.address = (const std::wstring_view&)idPtr->defn.info.data.registeredData;
		dataPtr = &tempStackItem;
		break;
	}
	//---------------------------------------------------------------
	// If it's a scalar or enumeration reference parameter, that item
	// points to the actual item...
	if (idPtr->defn.key == DFN_REFPARAM)
		if (ptype->form != FRM_ARRAY) /* && (ptype->form != FRM_RECORD)*/
			dataPtr = (const std::unique_ptr<StackItem>&)dataPtr->address;
	ABL_Assert(dataPtr != nullptr, 0, " ABL.execVariable(): dataPtr is nullptr ");
	//-----------------------------------------------------
	// Now, push the address of the variable's data area...
	if ((ptype->form == FRM_ARRAY) /*|| (ptype->form == FRM_RECORD)*/)
	{
		// pushInteger(ptype->size);
		pushAddress((Address)dataPtr->address);
	}
	else if (idPtr->defn.info.data.varType == VAR_TYPE_REGISTERED)
		pushAddress((Address)dataPtr->address);
	else
		pushAddress((Address)dataPtr);
	//-----------------------------------------------------------------------------------
	// If there is a subscript (or field identifier, if records are being used
	// in ABL) then modify the address to point to the proper element of the
	// array (or record)...
	getCodeToken();
	while ((codeToken == TKN_LBRACKET) /*|| (codeTOken == TKN_PERIOD)*/)
	{
		// if (codeToken == TKN_LBRACKET)
		ptype = execSubscripts(ptype);
		// else if (codeToken == TKN_PERIOD)
		//	ptype = execField(ptype);
	}
	//------------------------------------------------------------
	// Leave the modified address on the top of the stack if:
	//		a) it's an assignment target;
	//		b) it reresents a parameter passed by reference;
	//		c) it's the address of an array or record;
	// Otherwise, replace the address with the value it points to.
	if ((use != USE_TARGET) && (use != USE_REFPARAM) && (ptype->form != FRM_ARRAY))
	{
		if ((ptype == IntegerTypePtr) || (ptype->form == FRM_ENUM))
		{
			tos->integer = *((int32_t*)tos->address);
		}
		else if (ptype == CharTypePtr)
			tos->byte = *((const std::wstring_view&)tos->address);
		else
			tos->real = *((float*)tos->address);
	}
	if (debugger)
	{
		if ((use != USE_TARGET) && (use != USE_REFPARAM))
		{
			if (ptype->form == FRM_ARRAY)
				debugger->traceDataFetch(idPtr, ptype, (const std::unique_ptr<StackItem>&)tos->address);
			else
				debugger->traceDataFetch(idPtr, ptype, tos);
		}
	}
	return (ptype);
}

//***************************************************************************

const std::unique_ptr<Type>&
execFactor(void)
{
	const std::unique_ptr<Type>& resultTypePtr = nullptr;
	switch (codeToken)
	{
	case TKN_IDENTIFIER:
	{
		const std::unique_ptr<SymTableNode>& idPtr = getCodeSymTableNodePtr();
		if (idPtr->defn.key == DFN_FUNCTION)
		{
			const std::unique_ptr<SymTableNode>& thisRoutineIdPtr = CurRoutineIdPtr;
			resultTypePtr = execRoutineCall(idPtr, false);
			CurRoutineIdPtr = thisRoutineIdPtr;
		}
		else if (idPtr->defn.key == DFN_CONST)
			resultTypePtr = execConstant(idPtr);
		else
			resultTypePtr = execVariable(idPtr, USE_EXPR);
	}
	break;
	case TKN_NUMBER:
	{
		const std::unique_ptr<SymTableNode>& numberPtr = getCodeSymTableNodePtr();
		if (numberPtr->ptype == IntegerTypePtr)
		{
			pushInteger(numberPtr->defn.info.constant.value.integer);
			resultTypePtr = IntegerTypePtr;
		}
		else
		{
			pushReal(numberPtr->defn.info.constant.value.real);
			resultTypePtr = RealTypePtr;
		}
		getCodeToken();
	}
	break;
	case TKN_STRING:
	{
		const std::unique_ptr<SymTableNode>& nodePtr = getCodeSymTableNodePtr();
		int32_t length = strlen(nodePtr->name);
		if (length > 1)
		{
			//-----------------------------------------------------------------------
			// Remember, the double quotes are on the back and front of the
			// string...
			pushAddress(nodePtr->info);
			resultTypePtr = nodePtr->ptype;
		}
		else
		{
			//----------------------------------------------
			// Just push the one character in this string...
			pushByte(nodePtr->name[0]);
			resultTypePtr = CharTypePtr;
		}
		getCodeToken();
	}
	break;
	case TKN_NOT:
		getCodeToken();
		resultTypePtr = execFactor();
		//--------------------------------------
		// Following flips 1 to 0, and 0 to 1...
		tos->integer = 1 - tos->integer;
		break;
	case TKN_LPAREN:
		getCodeToken();
		resultTypePtr = execExpression();
		getCodeToken();
		break;
	}
	return (resultTypePtr);
}

//***************************************************************************

const std::unique_ptr<Type>&
execTerm(void)
{
	const std::unique_ptr<StackItem>& operand1Ptr;
	const std::unique_ptr<StackItem>& operand2Ptr;
	const std::unique_ptr<Type>& type2Ptr;
	TokenCodeType op;
	const std::unique_ptr<Type>& resultTypePtr = execFactor();
	//----------------------------------------------
	// Process the factors separated by operators...
	while ((codeToken == TKN_STAR) || (codeToken == TKN_FSLASH) || (codeToken == TKN_DIV) || (codeToken == TKN_MOD) || (codeToken == TKN_AND))
	{
		op = codeToken;
		getCodeToken();
		type2Ptr = execFactor();
		operand1Ptr = tos - 1;
		operand2Ptr = tos;
		if (op == TKN_AND)
		{
			operand1Ptr->integer = operand1Ptr->integer && operand2Ptr->integer;
			resultTypePtr = BooleanTypePtr;
		}
		else
			switch (op)
			{
			case TKN_STAR:
				if ((resultTypePtr == IntegerTypePtr) && (type2Ptr == IntegerTypePtr))
				{
					//-----------------------------
					// Both operands are integer...
					operand1Ptr->integer = operand1Ptr->integer * operand2Ptr->integer;
					resultTypePtr = IntegerTypePtr;
				}
				else
				{
					//----------------------------------------------------------------
					// Both operands are real, or one is real and the other
					// integer...
					promoteOperandsToReal(operand1Ptr, resultTypePtr, operand2Ptr, type2Ptr);
					operand1Ptr->real = operand1Ptr->real * operand2Ptr->real;
					resultTypePtr = RealTypePtr;
				}
				break;
			case TKN_FSLASH:
				//--------------------------------------------------------------------
				// Both operands are real, or one is real and the other an
				// integer. We probably want this same token to be used for
				// integers, as opposed to using the DIV token...
				if ((resultTypePtr == IntegerTypePtr) && (type2Ptr == IntegerTypePtr))
				{
					//-----------------------------
					// Both operands are integer...
					if (operand2Ptr->integer == 0)
					{
#ifdef _DEBUG
						runtimeError(ABL_ERR_RUNTIME_DIVISION_BY_ZERO);
#else
						// HACK!!!!!!!!!!!!
						operand1Ptr->integer = 0;
#endif
					}
					else
						operand1Ptr->integer = operand1Ptr->integer / operand2Ptr->integer;
					resultTypePtr = IntegerTypePtr;
				}
				else
				{
					//----------------------------------------------------------------
					// Both operands are real, or one is real and the other
					// integer...
					promoteOperandsToReal(operand1Ptr, resultTypePtr, operand2Ptr, type2Ptr);
					if (operand2Ptr->real == 0.0)
#ifdef _DEBUG
						runtimeError(ABL_ERR_RUNTIME_DIVISION_BY_ZERO);
#else
						// HACK!!!!!!!!!!!!
						operand1Ptr->real = 0.0;
#endif
					else
						operand1Ptr->real = operand1Ptr->real / operand2Ptr->real;
					resultTypePtr = RealTypePtr;
				}
				break;
			case TKN_DIV:
			case TKN_MOD:
				//-----------------------------
				// Both operands are integer...
				if (operand2Ptr->integer == 0)
#ifdef _DEBUG
					runtimeError(ABL_ERR_RUNTIME_DIVISION_BY_ZERO);
#else
					// HACK!!!!!!!!!!!!
					operand1Ptr->integer = 0;
#endif
				else
				{
					if (op == TKN_DIV)
						operand1Ptr->integer = operand1Ptr->integer / operand2Ptr->integer;
					else
						operand1Ptr->integer = operand1Ptr->integer % operand2Ptr->integer;
				}
				resultTypePtr = IntegerTypePtr;
				break;
			}
		//------------------------------
		// Pop off the second operand...
		pop();
	}
	return (resultTypePtr);
}

//***************************************************************************

const std::unique_ptr<Type>&
execSimpleExpression(void)
{
	TokenCodeType unaryOp = TKN_PLUS;
	//------------------------------------------------------
	// If there's a + or - before the expression, save it...
	if ((codeToken == TKN_PLUS) || (codeToken == TKN_MINUS))
	{
		unaryOp = codeToken;
		getCodeToken();
	}
	const std::unique_ptr<Type>& resultTypePtr = execTerm();
	//-------------------------------------------------------
	// If there was a unary -, negate the top of the stack...
	if (unaryOp == TKN_MINUS)
	{
		if (resultTypePtr == IntegerTypePtr)
			tos->integer = -(tos->integer);
		else
			tos->real = -(tos->real);
	}
	while ((codeToken == TKN_PLUS) || (codeToken == TKN_MINUS) || (codeToken == TKN_OR))
	{
		TokenCodeType op = codeToken;
		getCodeToken();
		const std::unique_ptr<Type>& type2Ptr = execTerm();
		const std::unique_ptr<StackItem>& operand1Ptr = tos - 1;
		const std::unique_ptr<StackItem>& operand2Ptr = tos;
		if (op == TKN_OR)
		{
			operand1Ptr->integer = operand1Ptr->integer || operand2Ptr->integer;
			resultTypePtr = BooleanTypePtr;
		}
		else if ((resultTypePtr == IntegerTypePtr) && (type2Ptr == IntegerTypePtr))
		{
			if (op == TKN_PLUS)
				operand1Ptr->integer = operand1Ptr->integer + operand2Ptr->integer;
			else
				operand1Ptr->integer = operand1Ptr->integer - operand2Ptr->integer;
			resultTypePtr = IntegerTypePtr;
		}
		else
		{
			//-------------------------------------------------------------------
			// Both operands are real, or one is real and the other is
			// integer...
			promoteOperandsToReal(operand1Ptr, resultTypePtr, operand2Ptr, type2Ptr);
			if (op == TKN_PLUS)
				operand1Ptr->real = operand1Ptr->real + operand2Ptr->real;
			else
				operand1Ptr->real = operand1Ptr->real - operand2Ptr->real;
			resultTypePtr = RealTypePtr;
		}
		//--------------------------
		// Pop the second operand...
		pop();
	}
	return (resultTypePtr);
}

//***************************************************************************

const std::unique_ptr<Type>&
execExpression(void)
{
	const std::unique_ptr<StackItem>& operand1Ptr;
	const std::unique_ptr<StackItem>& operand2Ptr;
	TokenCodeType op;
	bool result = false;
	//-----------------------------------
	// Get the first simple expression...
	const std::unique_ptr<Type>& resultTypePtr = execSimpleExpression();
	const std::unique_ptr<Type>& type2Ptr = nullptr;
	//-----------------------------------------------------------
	// If there is a relational operator, save it and process the
	// second simple expression...
	if ((codeToken == TKN_EQUALEQUAL) || (codeToken == TKN_LT) || (codeToken == TKN_GT) || (codeToken == TKN_NE) || (codeToken == TKN_LE) || (codeToken == TKN_GE))
	{
		op = codeToken;
		getCodeToken();
		//---------------------------------
		// Get the 2nd simple expression...
		type2Ptr = execSimpleExpression();
		operand1Ptr = tos - 1;
		operand2Ptr = tos;
		//-----------------------------------------------------
		// Both operands are integer, boolean or enumeration...
		if (((resultTypePtr == IntegerTypePtr) && (type2Ptr == IntegerTypePtr)) || (resultTypePtr->form == FRM_ENUM))
		{
			switch (op)
			{
			case TKN_EQUALEQUAL:
				result = operand1Ptr->integer == operand2Ptr->integer;
				break;
			case TKN_LT:
				result = operand1Ptr->integer < operand2Ptr->integer;
				break;
			case TKN_GT:
				result = operand1Ptr->integer > operand2Ptr->integer;
				break;
			case TKN_NE:
				result = operand1Ptr->integer != operand2Ptr->integer;
				break;
			case TKN_LE:
				result = operand1Ptr->integer <= operand2Ptr->integer;
				break;
			case TKN_GE:
				result = operand1Ptr->integer >= operand2Ptr->integer;
				break;
			}
		}
		else if (resultTypePtr == CharTypePtr)
		{
			switch (op)
			{
			case TKN_EQUALEQUAL:
				result = operand1Ptr->byte == operand2Ptr->byte;
				break;
			case TKN_LT:
				result = operand1Ptr->byte < operand2Ptr->byte;
				break;
			case TKN_GT:
				result = operand1Ptr->byte > operand2Ptr->byte;
				break;
			case TKN_NE:
				result = operand1Ptr->byte != operand2Ptr->byte;
				break;
			case TKN_LE:
				result = operand1Ptr->byte <= operand2Ptr->byte;
				break;
			case TKN_GE:
				result = operand1Ptr->byte >= operand2Ptr->byte;
				break;
			}
		}
		else if ((resultTypePtr->form == FRM_ARRAY) && (resultTypePtr->info.array.elementTypePtr == CharTypePtr))
		{
			//----------------------------------------
			// Strings. For now, always return true...
			result = true;
		}
		else if ((resultTypePtr == RealTypePtr) || (type2Ptr == RealTypePtr))
		{
			promoteOperandsToReal(operand1Ptr, resultTypePtr, operand2Ptr, type2Ptr);
			switch (op)
			{
			case TKN_EQUALEQUAL:
				result = operand1Ptr->real == operand2Ptr->real;
				break;
			case TKN_LT:
				result = operand1Ptr->real < operand2Ptr->real;
				break;
			case TKN_GT:
				result = operand1Ptr->real > operand2Ptr->real;
				break;
			case TKN_NE:
				result = operand1Ptr->real != operand2Ptr->real;
				break;
			case TKN_LE:
				result = operand1Ptr->real <= operand2Ptr->real;
				break;
			case TKN_GE:
				result = operand1Ptr->real >= operand2Ptr->real;
				break;
			}
		}
		//---------------------------------------------------------
		// Replace the two operands with the result on the stack...
		if (result)
			operand1Ptr->integer = 1;
		else
			operand1Ptr->integer = 0;
		pop();
		resultTypePtr = BooleanTypePtr;
	}
	return (resultTypePtr);
}

//***************************************************************************

} // namespace mclib::abl
