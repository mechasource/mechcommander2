//===========================================================================//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//
//***************************************************************************
//
//								ABLDECL.CPP
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
extern wchar_t wordString[];
extern Literal curLiteral;

extern const std::unique_ptr<SymTableNode>& SymTableDisplay[];
extern int32_t level;

extern const std::unique_ptr<Type>& IntegerTypePtr;
extern const std::unique_ptr<Type>& CharTypePtr;
extern const std::unique_ptr<Type>& RealTypePtr;
extern const std::unique_ptr<Type>& BooleanTypePtr;

extern TokenCodeType declarationStartList[];
extern TokenCodeType statementStartList[];

extern int32_t eternalOffset;
extern int32_t NumStaticVariables;
extern int32_t MaxStaticVariables;
extern int32_t* StaticVariablesSizes;
extern int32_t* EternalVariablesSizes;
extern const std::unique_ptr<ABLModule>& CurLibrary;

//***************************************************************************

TokenCodeType followRoutineList[] = {TKN_SEMICOLON, TKN_EOF, TKN_NONE};

TokenCodeType followDeclarationList[] = {TKN_SEMICOLON, TKN_IDENTIFIER, TKN_EOF, TKN_NONE};

TokenCodeType followVariablesList[] = {TKN_SEMICOLON, TKN_IDENTIFIER, TKN_EOF, TKN_NONE};

TokenCodeType followVarBlockList[] = {
	TKN_FUNCTION, TKN_ORDER, TKN_STATE, TKN_CODE, TKN_EOF, TKN_NONE};

TokenCodeType followDimensionList[] = {TKN_COMMA, TKN_RBRACKET, TKN_EOF, TKN_NONE};

TokenCodeType indexTypeStartList[] = {TKN_IDENTIFIER, TKN_NUMBER, TKN_NONE};

TokenCodeType followIndexesList[] = {TKN_OF, TKN_IDENTIFIER, TKN_LPAREN, TKN_PLUS, TKN_MINUS,
	TKN_NUMBER, TKN_SEMICOLON, TKN_EOF, TKN_NONE};

//***************************************************************************
// MISC. routines
//***************************************************************************

void ifTokenGet(TokenCodeType tokenCode)
{
	if (curToken == tokenCode)
		getToken();
}

//***************************************************************************

void ifTokenGetElseError(TokenCodeType tokenCode, SyntaxErrorType errorCode)
{
	if (curToken == tokenCode)
		getToken();
	else
		syntaxError(errorCode);
}

//***************************************************************************
// DECLARATIONS routines
//***************************************************************************

void declarations(const std::unique_ptr<SymTableNode>& routineIdPtr, bool allowFunctions)
{
	if (curToken == TKN_CONST)
	{
		getToken();
		constDefinitions();
	}
	if (curToken == TKN_TYPE)
	{
		getToken();
		typeDefinitions();
	}
	if (curToken == TKN_VAR)
	{
		getToken();
		varDeclarations(routineIdPtr);
	}
	//---------------------------------------------------
	// Loop to process all of the function definitions...
	if (allowFunctions)
		while ((curToken == TKN_FUNCTION) || (curToken == TKN_ORDER) || (curToken == TKN_STATE))
		{
			routine();
			//---------------------
			// Error synchronize...
			synchronize(followRoutineList, declarationStartList, statementStartList);
			if (curToken == TKN_SEMICOLON)
				getToken();
			else if (tokenIn(declarationStartList) || tokenIn(statementStartList))
				syntaxError(ABL_ERR_SYNTAX_MISSING_SEMICOLON);
		}
	else if ((curToken == TKN_FUNCTION) || (curToken == TKN_ORDER) || (curToken == TKN_STATE))
		syntaxError(ABL_ERR_SYNTAX_NO_FUNCTION_NESTING);
}

//***************************************************************************
// CONST routines
//***************************************************************************

void constDefinitions(void)
{
	//-------------------------------------------------------
	// Loop to process definitions separated by semicolons...
	while (curToken == TKN_IDENTIFIER)
	{
		const std::unique_ptr<SymTableNode>& constantIdPtr;
		searchAndEnterLocalSymTable(constantIdPtr);
		constantIdPtr->defn.key = DFN_CONST;
		constantIdPtr->library = CurLibrary;
		getToken();
		ifTokenGetElseError(TKN_EQUAL, ABL_ERR_SYNTAX_MISSING_EQUAL);
		doConst(constantIdPtr);
		analyzeConstDefn(constantIdPtr);
		//---------------------------------
		// Error synchronize: should be a ;
		synchronize(followDeclarationList, declarationStartList, statementStartList);
		if (curToken == TKN_SEMICOLON)
			getToken();
		else if (tokenIn(declarationStartList) || tokenIn(statementStartList))
			syntaxError(ABL_ERR_SYNTAX_MISSING_SEMICOLON);
	}
}

//***************************************************************************

const std::unique_ptr<Type>&
makeStringType(int32_t length)
{
	const std::unique_ptr<Type>& stringTypePtr = createType();
	if (!stringTypePtr)
		ABL_Fatal(0, " ABL: Unable to AblStackHeap->malloc stringType ");
	stringTypePtr->form = FRM_ARRAY;
	stringTypePtr->size = length;
	stringTypePtr->typeIdPtr = nullptr;
	stringTypePtr->info.array.indexTypePtr = IntegerTypePtr;
	stringTypePtr->info.array.elementTypePtr = CharTypePtr;
	stringTypePtr->info.array.elementCount = length + 1;
	return (stringTypePtr);
}

//***************************************************************************

void doConst(const std::unique_ptr<SymTableNode>& constantIdPtr)
{
	TokenCodeType sign = TKN_PLUS;
	bool sawSign = false;
	if ((curToken == TKN_PLUS) || (curToken == TKN_MINUS))
	{
		sign = curToken;
		sawSign = true;
		getToken();
	}
	//----------------------------------
	// Numeric constant: real or integer
	if (curToken == TKN_NUMBER)
	{
		if (curLiteral.type == LIT_INTEGER)
		{
			if (sign == TKN_PLUS)
				constantIdPtr->defn.info.constant.value.integer = curLiteral.value.integer;
			else
				constantIdPtr->defn.info.constant.value.integer = -(curLiteral.value.integer);
			constantIdPtr->ptype = setType(IntegerTypePtr);
		}
		else
		{
			if (sign == TKN_PLUS)
				constantIdPtr->defn.info.constant.value.real = curLiteral.value.real;
			else
				constantIdPtr->defn.info.constant.value.real = -(curLiteral.value.real);
			constantIdPtr->ptype = setType(RealTypePtr);
		}
	}
	else if (curToken == TKN_IDENTIFIER)
	{
		const std::unique_ptr<SymTableNode>& idPtr = nullptr;
		searchAllSymTables(idPtr);
		if (!idPtr)
			syntaxError(ABL_ERR_SYNTAX_UNDEFINED_IDENTIFIER);
		else if (idPtr->defn.key != DFN_CONST)
			syntaxError(ABL_ERR_SYNTAX_NOT_A_CONSTANT_IDENTIFIER);
		else if (idPtr->ptype == IntegerTypePtr)
		{
			if (sign == TKN_PLUS)
				constantIdPtr->defn.info.constant.value.integer =
					idPtr->defn.info.constant.value.integer;
			else
				constantIdPtr->defn.info.constant.value.integer =
					-(idPtr->defn.info.constant.value.integer);
			constantIdPtr->ptype = setType(IntegerTypePtr);
		}
		else if (idPtr->ptype == CharTypePtr)
		{
			if (sawSign)
				syntaxError(ABL_ERR_SYNTAX_INVALID_CONSTANT);
			constantIdPtr->defn.info.constant.value.character =
				idPtr->defn.info.constant.value.character;
			constantIdPtr->ptype = setType(CharTypePtr);
		}
		else if (idPtr->ptype == RealTypePtr)
		{
			if (sign == TKN_PLUS)
				constantIdPtr->defn.info.constant.value.real = idPtr->defn.info.constant.value.real;
			else
				constantIdPtr->defn.info.constant.value.real =
					-(idPtr->defn.info.constant.value.real);
			constantIdPtr->ptype = setType(RealTypePtr);
		}
		else if (((Type*)(idPtr->ptype))->form == FRM_ENUM)
		{
			if (sawSign)
				syntaxError(ABL_ERR_SYNTAX_INVALID_CONSTANT);
			constantIdPtr->defn.info.constant.value.integer =
				idPtr->defn.info.constant.value.integer;
			constantIdPtr->ptype = setType(idPtr->ptype);
		}
		else if (((const std::unique_ptr<Type>&)(idPtr->ptype))->form == FRM_ARRAY)
		{
			if (sawSign)
				syntaxError(ABL_ERR_SYNTAX_INVALID_CONSTANT);
			constantIdPtr->defn.info.constant.value.stringPtr =
				idPtr->defn.info.constant.value.stringPtr;
			constantIdPtr->ptype = setType(idPtr->ptype);
		}
	}
	else if (curToken == TKN_STRING)
	{
		if (sawSign)
			syntaxError(ABL_ERR_SYNTAX_INVALID_CONSTANT);
		if (strlen(curLiteral.value.string) == 1)
		{
			constantIdPtr->defn.info.constant.value.character = curLiteral.value.string[0];
			constantIdPtr->ptype = setType(CharTypePtr);
		}
		else
		{
			int32_t length = strlen(curLiteral.value.string);
			constantIdPtr->defn.info.constant.value.stringPtr =
				(std::wstring_view)ABLSymbolMallocCallback(length + 1);
			if (!constantIdPtr->defn.info.constant.value.stringPtr)
				ABL_Fatal(0,
					" ABL: Unable to AblSymbolHeap->malloc array "
					"string constant ");
			strcpy(constantIdPtr->defn.info.constant.value.stringPtr, curLiteral.value.string);
			constantIdPtr->ptype = makeStringType(length);
		}
	}
	else
	{
		constantIdPtr->ptype = nullptr;
		syntaxError(ABL_ERR_SYNTAX_INVALID_CONSTANT);
	}
	getToken();
}

//***************************************************************************
// TYPE routines
//***************************************************************************

//---------------------------------------------------------------------------
// Need to implement type routines if we allow user-defined types, and/or the
// PASCAL style array types (otherwise, arrays should be implemented in the
// var routines...

void typeDefinitions(void)
{
	while (curToken == TKN_IDENTIFIER)
	{
		const std::unique_ptr<SymTableNode>& typeIdPtr;
		searchAndEnterLocalSymTable(typeIdPtr);
		typeIdPtr->defn.key = DFN_TYPE;
		typeIdPtr->library = CurLibrary;
		getToken();
		ifTokenGetElseError(TKN_EQUAL, ABL_ERR_SYNTAX_MISSING_EQUAL);
		//----------------------------------
		// Process the type specification...
		typeIdPtr->ptype = doType();
		if (typeIdPtr->ptype->typeIdPtr == nullptr)
			typeIdPtr->ptype->typeIdPtr = typeIdPtr;
		analyzeTypeDefn(typeIdPtr);
		//---------------
		// Error synch...
		synchronize(followDeclarationList, declarationStartList, statementStartList);
		if (curToken == TKN_SEMICOLON)
			getToken();
		else if (tokenIn(declarationStartList) || tokenIn(statementStartList))
			syntaxError(ABL_ERR_SYNTAX_MISSING_SEMICOLON);
	}
}

//***************************************************************************

const std::unique_ptr<Type>&
doType(void)
{
	switch (curToken)
	{
	case TKN_IDENTIFIER:
	{
		const std::unique_ptr<SymTableNode>& idPtr;
		searchAllSymTables(idPtr);
		if (!idPtr)
		{
			syntaxError(ABL_ERR_SYNTAX_UNDEFINED_IDENTIFIER);
			return (nullptr);
		}
		else if (idPtr->defn.key == DFN_TYPE)
		{
			//----------------------------------------------------------
			// NOTE: Array types should be parsed in this case if a left
			// bracket follows the type identifier.
			const std::unique_ptr<Type>& elementType = setType(identifierType(idPtr));
			if (curToken == TKN_LBRACKET)
			{
				//--------------
				// Array type...
				const std::unique_ptr<Type>& ptype = createType();
				if (!ptype)
					ABL_Fatal(0, " ABL: Unable to AblStackHeap->malloc array type ");
				const std::unique_ptr<Type>& elementTypePtr = ptype;
				do
				{
					getToken();
					if (tokenIn(indexTypeStartList))
					{
						elementTypePtr->form = FRM_ARRAY;
						elementTypePtr->size = 0;
						elementTypePtr->typeIdPtr = nullptr;
						//----------------------------------------------
						// All array indices must be integer, for now...
						elementTypePtr->info.array.indexTypePtr = setType(IntegerTypePtr);
						//------------------------
						// Read the index count...
						switch (curToken)
						{
						case TKN_NUMBER:
							if (curLiteral.type == LIT_INTEGER)
								elementTypePtr->info.array.elementCount = curLiteral.value.integer;
							else
							{
								elementTypePtr->form = FRM_NONE;
								elementTypePtr->size = 0;
								elementTypePtr->typeIdPtr = nullptr;
								elementTypePtr->info.array.indexTypePtr = nullptr;
								syntaxError(ABL_ERR_SYNTAX_INVALID_INDEX_TYPE);
							}
							getToken();
							break;
						case TKN_IDENTIFIER:
						{
							const std::unique_ptr<SymTableNode>& idPtr;
							searchAllSymTables(idPtr);
							if (idPtr == nullptr)
								syntaxError(ABL_ERR_SYNTAX_UNDEFINED_IDENTIFIER);
							else if (idPtr->defn.key == DFN_CONST)
							{
								if (idPtr->ptype == IntegerTypePtr)
									elementTypePtr->info.array.elementCount =
										idPtr->defn.info.constant.value.integer;
								else
								{
									elementTypePtr->form = FRM_NONE;
									elementTypePtr->size = 0;
									elementTypePtr->typeIdPtr = nullptr;
									elementTypePtr->info.array.indexTypePtr = nullptr;
									syntaxError(ABL_ERR_SYNTAX_INVALID_INDEX_TYPE);
								}
							}
							else
							{
								elementTypePtr->form = FRM_NONE;
								elementTypePtr->size = 0;
								elementTypePtr->typeIdPtr = nullptr;
								elementTypePtr->info.array.indexTypePtr = nullptr;
								syntaxError(ABL_ERR_SYNTAX_INVALID_INDEX_TYPE);
							}
							getToken();
						}
						break;
						default:
							elementTypePtr->form = FRM_NONE;
							elementTypePtr->size = 0;
							elementTypePtr->typeIdPtr = nullptr;
							elementTypePtr->info.array.indexTypePtr = nullptr;
							syntaxError(ABL_ERR_SYNTAX_INVALID_INDEX_TYPE);
							getToken();
						}
					}
					else
					{
						elementTypePtr->form = FRM_NONE;
						elementTypePtr->size = 0;
						elementTypePtr->typeIdPtr = nullptr;
						elementTypePtr->info.array.indexTypePtr = nullptr;
						syntaxError(ABL_ERR_SYNTAX_INVALID_INDEX_TYPE);
						getToken();
					}
					synchronize(followDimensionList, nullptr, nullptr);
					//--------------------------------
					// Create an array element type...
					if (curToken == TKN_COMMA)
					{
						elementTypePtr = elementTypePtr->info.array.elementTypePtr = createType();
						if (!elementTypePtr)
							ABL_Fatal(0,
								" ABL: Unable to AblStackHeap->malloc "
								"array element Type ");
					}
				} while (curToken == TKN_COMMA);
				ifTokenGetElseError(TKN_RBRACKET, ABL_ERR_SYNTAX_MISSING_RBRACKET);
				elementTypePtr->info.array.elementTypePtr = elementType;
				ptype->size = arraySize(ptype);
				elementType = ptype;
			}
			return (elementType);
		}
		else
		{
			syntaxError(ABL_ERR_SYNTAX_NOT_A_TYPE_IDENTIFIER);
			return (nullptr);
		}
	}
	break;
	case TKN_LPAREN:
		return (enumerationType());
	default:
		syntaxError(ABL_ERR_SYNTAX_INVALID_TYPE);
		return (nullptr);
	}
}

//***************************************************************************

const std::unique_ptr<Type>&
identifierType(const std::unique_ptr<SymTableNode>& idPtr)
{
	const std::unique_ptr<Type>& ptype = (const std::unique_ptr<Type>&)idPtr->ptype;
	getToken();
	return (ptype);
}

//***************************************************************************

const std::unique_ptr<Type>&
enumerationType(void)
{
	const std::unique_ptr<SymTableNode>& constantIdPtr = nullptr;
	const std::unique_ptr<SymTableNode>& lastIdPtr = nullptr;
	const std::unique_ptr<Type>& ptype = createType();
	if (!ptype)
		ABL_Fatal(0, " ABL: Unable to AblStackHeap->malloc enumeration type ");
	int32_t constantValue = -1;
	ptype->form = FRM_ENUM;
	ptype->size = sizeof(int32_t);
	ptype->typeIdPtr = nullptr;
	getToken();
	//------------------------------------------------------------
	// Process list of identifiers in this new enumeration type...
	while (curToken == TKN_IDENTIFIER)
	{
		searchAndEnterLocalSymTable(constantIdPtr);
		constantIdPtr->defn.key = DFN_CONST;
		constantIdPtr->defn.info.constant.value.integer = ++constantValue;
		constantIdPtr->ptype = ptype;
		constantIdPtr->library = CurLibrary;
		if (lastIdPtr == nullptr)
			ptype->info.enumeration.constIdPtr = lastIdPtr = constantIdPtr;
		else
		{
			lastIdPtr->next = constantIdPtr;
			lastIdPtr = constantIdPtr;
		}
		getToken();
		ifTokenGet(TKN_COMMA);
	}
	ifTokenGetElseError(TKN_RPAREN, ABL_ERR_SYNTAX_MISSING_RPAREN);
	ptype->info.enumeration.max = constantValue;
	return (ptype);
}

//***************************************************************************

int32_t
arraySize(const std::unique_ptr<Type>& ptype)
{
	if (ptype->info.array.elementTypePtr->size == 0)
		ptype->info.array.elementTypePtr->size = arraySize(ptype->info.array.elementTypePtr);
	if (ptype->info.array.elementCount == -1)
	{
		//--------------------------------------------------------------
		// Open array, so just return the size of its element. Remember,
		// open arrays must be open at the end...
		ptype->size = ptype->info.array.elementTypePtr->size;
	}
	else
		ptype->size = ptype->info.array.elementCount * ptype->info.array.elementTypePtr->size;
	return (ptype->size);
}

//***************************************************************************
// VAR routines
//***************************************************************************

void varDeclarations(const std::unique_ptr<SymTableNode>& routineIdPtr)
{
	varOrFieldDeclarations(
		routineIdPtr, STACK_FRAME_HEADER_SIZE + routineIdPtr->defn.info.routine.paramCount);
}

//***************************************************************************

void varOrFieldDeclarations(const std::unique_ptr<SymTableNode>& routineIdPtr, int32_t offset)
{
	bool varFlag = (routineIdPtr != nullptr);
	const std::unique_ptr<SymTableNode>& idPtr = nullptr;
	const std::unique_ptr<SymTableNode>& firstIdPtr = nullptr;
	const std::unique_ptr<SymTableNode>& lastIdPtr = nullptr;
	const std::unique_ptr<SymTableNode>& prevLastIdPtr = nullptr;
	int32_t totalSize = 0;
	while ((curToken == TKN_IDENTIFIER) || (curToken == TKN_ETERNAL) || (curToken == TKN_STATIC))
	{
		VariableType varType = VAR_TYPE_NORMAL;
		if ((curToken == TKN_ETERNAL) || (curToken == TKN_STATIC))
		{
			if (curToken == TKN_ETERNAL)
				varType = VAR_TYPE_ETERNAL;
			else
				varType = VAR_TYPE_STATIC;
			getToken();
			if (curToken != TKN_IDENTIFIER)
				syntaxError(ABL_ERR_SYNTAX_MISSING_IDENTIFIER);
		}
		firstIdPtr = nullptr;
		//------------------------------
		// Process the variable type...
		const std::unique_ptr<Type>& ptype = doType();
		//------------------------------------------------------------------
		// Since we haven't really assigned it here, decrement its
		// numInstances. Every variable in this list will set it properly...
		ptype->numInstances--;
		int32_t size = ptype->size;
		//-------------------------------------------------------
		// Now that we've read the type, read in the variable (or
		// possibly list of variables) declared of this type.
		// Loop to process every variable (and field, if records
		// are being implemented:) in sublist...
		while (curToken == TKN_IDENTIFIER)
		{
			if (varFlag)
			{
				//---------------------------------------------
				// We're working with a variable declaration...
				if (varType == VAR_TYPE_ETERNAL)
				{
					int32_t curLevel = level;
					level = 0;
					searchAndEnterThisTable(idPtr, SymTableDisplay[0]);
					level = curLevel;
				}
				else
					searchAndEnterLocalSymTable(idPtr);
				idPtr->library = CurLibrary;
				idPtr->defn.key = DFN_VAR;
			}
			else
				syntaxError(ABL_ERR_SYNTAX_NO_RECORD_TYPES);
			idPtr->labelIndex = 0;
			//------------------------------------------
			// Now, link Id's together into a sublist...
			if (!firstIdPtr)
			{
				firstIdPtr = lastIdPtr = idPtr;
				if (varFlag && (varType != VAR_TYPE_ETERNAL) && (routineIdPtr->defn.info.routine.locals == nullptr))
					routineIdPtr->defn.info.routine.locals = idPtr;
			}
			else
			{
				lastIdPtr->next = idPtr;
				lastIdPtr = idPtr;
			}
			getToken();
			ifTokenGet(TKN_COMMA);
		}
		//--------------------------------------------------------------------------
		// Assign the offset and the type to all variable or field Ids in
		// sublist...
		for (idPtr = firstIdPtr; idPtr != nullptr; idPtr = idPtr->next)
		{
			idPtr->ptype = setType(ptype);
			if (varFlag)
			{
				idPtr->defn.info.data.varType = varType;
				switch (varType)
				{
				case VAR_TYPE_NORMAL:
					totalSize += size;
					idPtr->defn.info.data.offset = offset++;
					break;
				case VAR_TYPE_ETERNAL:
				{
					idPtr->defn.info.data.offset = eternalOffset;
					//-----------------------------------
					// Initialize the variable to zero...
					const std::unique_ptr<StackItem>& dataPtr = (const std::unique_ptr<StackItem>&)stack + eternalOffset;
					if (ptype->form == FRM_ARRAY)
					{
						dataPtr->address = (Address)ABLStackMallocCallback((size_t)size);
						if (!dataPtr->address)
							ABL_Fatal(0,
								" ABL: Unable to AblStackHeap->malloc "
								"eternal array ");
						memset(dataPtr->address, 0, size);
						EternalVariablesSizes[eternalOffset] = size;
					}
					else
					{
						dataPtr->integer = 0;
						EternalVariablesSizes[eternalOffset] = 0;
					}
					eternalOffset++;
				}
				break;
				case VAR_TYPE_STATIC:
				{
					if (NumStaticVariables == MaxStaticVariables)
						syntaxError(ABL_ERR_SYNTAX_TOO_MANY_STATIC_VARS);
					idPtr->defn.info.data.offset = NumStaticVariables;
					if (ptype->form == FRM_ARRAY)
						StaticVariablesSizes[NumStaticVariables] = size;
					else
						StaticVariablesSizes[NumStaticVariables] = 0;
					NumStaticVariables++;
				}
				break;
				}
				analyzeVarDecl(idPtr);
			}
			else
			{
				//----------------
				// record field...
				idPtr->defn.info.data.varType = VAR_TYPE_NORMAL;
				idPtr->defn.info.data.offset = offset;
				offset += size;
			}
		}
		//--------------------------------------------------
		// Now, link this sublist to the previous sublist...
		if (varType != VAR_TYPE_ETERNAL)
		{
			if (prevLastIdPtr != nullptr)
				prevLastIdPtr->next = firstIdPtr;
			prevLastIdPtr = lastIdPtr;
		}
		//---------------------
		// Error synchronize...
		if (varFlag)
			synchronize(followVariablesList, declarationStartList, statementStartList);
		if (curToken == TKN_SEMICOLON)
			getToken();
		else if (varFlag && (tokenIn(declarationStartList) || tokenIn(statementStartList)))
			syntaxError(ABL_ERR_SYNTAX_MISSING_SEMICOLON);
	}
	synchronize(followVarBlockList, nullptr, nullptr);
	if (varFlag)
	{
		//----------------------------------------------------------------
		// If the following error occurs too frequently, simply make the
		// totalLocalSize field an uint32_t instead, and dramatically
		// increase the totalSize limit here...
		if (totalSize > 32000)
			syntaxError(ABL_ERR_SYNTAX_TOO_MANY_LOCAL_VARIABLES);
		routineIdPtr->defn.info.routine.totalLocalSize = (uint16_t)totalSize;
	}
}

//***************************************************************************

} // namespace mclib::abl
