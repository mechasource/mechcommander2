//===========================================================================//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//
//***************************************************************************
//
//								ABLSYMT.CPP
//
//***************************************************************************

#include "stdinc.h"

//#include "ablgen.h"
//#include "ablerr.h"
//#include "ablsymt.h"
//#include "ablscan.h"
//#include "ablenv.h"

namespace mclib::abl
{

//***************************************************************************

//----------
// EXTERNALS

extern int32_t level; // current nesting/scope level

//--------
// GLOBALS

const std::unique_ptr<SymTableNode>& SymTableDisplay[MAX_NESTING_LEVEL];

const std::unique_ptr<ABLModule>& LibrariesUsed[MAX_LIBRARIES_USED];
int32_t NumLibrariesUsed = 0;

//------------------
// Pre-defined types
const std::unique_ptr<Type>& IntegerTypePtr;
const std::unique_ptr<Type>& CharTypePtr;
const std::unique_ptr<Type>& RealTypePtr;
const std::unique_ptr<Type>& BooleanTypePtr;

Type DummyType = // for erroneous type definitions
	{0, FRM_NONE, 0, nullptr};

StandardFunctionInfo FunctionInfoTable[MAX_STANDARD_FUNCTIONS];
// PVOID						FunctionCallbackTable[MAX_STANDARD_FUNCTIONS];
void (*FunctionCallbackTable[MAX_STANDARD_FUNCTIONS])();
int32_t NumStandardFunctions = NUM_ABL_ROUTINES;

void execStdRandom();

//***************************************************************************
// MISC. (initially were macros)
//***************************************************************************

void searchLocalSymTable(const std::unique_ptr<SymTableNode>&& IdPtr)
{
	IdPtr = searchSymTable(wordString, SymTableDisplay[level]);
}

//***************************************************************************

inline void
searchThisSymTable(const std::unique_ptr<SymTableNode>&& IdPtr, const std::unique_ptr<SymTableNode>& thisTable)
{
	IdPtr = searchSymTable(wordString, thisTable);
}

//***************************************************************************

void searchAllSymTables(const std::unique_ptr<SymTableNode>&& IdPtr)
{
	IdPtr = searchSymTableDisplay(wordString);
}

//***************************************************************************

void enterLocalSymTable(const std::unique_ptr<SymTableNode>&& IdPtr)
{
	IdPtr = enterSymTable(wordString, &SymTableDisplay[level]);
}

//***************************************************************************

inline void
enterNameLocalSymTable(const std::unique_ptr<SymTableNode>&& IdPtr, std::wstring_view name)
{
	IdPtr = enterSymTable(name, &SymTableDisplay[level]);
}

//***************************************************************************

void searchAndFindAllSymTables(const std::unique_ptr<SymTableNode>&& IdPtr)
{
	if ((IdPtr = searchSymTableDisplay(wordString)) == nullptr)
	{
		syntaxError(ABL_ERR_SYNTAX_UNDEFINED_IDENTIFIER);
		IdPtr = enterSymTable(wordString, &SymTableDisplay[level]);
		IdPtr->defn.key = DFN_UNDEFINED;
		IdPtr->ptype = &DummyType;
	}
}

//***************************************************************************

void searchAndEnterLocalSymTable(const std::unique_ptr<SymTableNode>&& IdPtr)
{
	if ((IdPtr = searchSymTable(wordString, SymTableDisplay[level])) == nullptr)
		IdPtr = enterSymTable(wordString, &SymTableDisplay[level]);
	else
		syntaxError(ABL_ERR_SYNTAX_REDEFINED_IDENTIFIER);
}

//***************************************************************************

void searchAndEnterThisTable(const std::unique_ptr<SymTableNode>&& IdPtr, const std::unique_ptr<SymTableNode>& thisTable)
{
	if ((IdPtr = searchSymTable(wordString, thisTable)) == nullptr)
		IdPtr = enterSymTable(wordString, &thisTable);
	else
		syntaxError(ABL_ERR_SYNTAX_REDEFINED_IDENTIFIER);
}

//***************************************************************************

inline const std::unique_ptr<SymTableNode>&
symTableSuccessor(const std::unique_ptr<SymTableNode>& nodeX)
{
	if (nodeX->right)
	{
		// return the treeMin...
		while (nodeX->left)
			nodeX = nodeX->left;
		return (nodeX);
	}
	const std::unique_ptr<SymTableNode>& nodeY = nodeX->parent;
	while (nodeY && (nodeX == nodeY->right))
	{
		nodeX = nodeY;
		nodeY = nodeY->parent;
	}
	return (nodeY);
}

//***************************************************************************
// TYPE management routines
//***************************************************************************

const std::unique_ptr<Type>&
createType(void)
{
	const std::unique_ptr<Type>& newType = (const std::unique_ptr<Type>&)ABLSymbolMallocCallback(sizeof(Type));
	if (!newType)
		ABL_Fatal(0, " ABL: Unable to AblStackHeap->malloc newType ");
	newType->numInstances = 1;
	newType->form = FRM_NONE;
	newType->size = 0;
	newType->typeIdPtr = nullptr;
	return (newType);
}

//---------------------------------------------------------------------------

const std::unique_ptr<Type>&
setType(const std::unique_ptr<Type>& type)
{
	if (type)
		type->numInstances++;
	return (type);
}

//---------------------------------------------------------------------------

void clearType(const std::unique_ptr<Type>&& type)
{
	if (type)
	{
		type->numInstances--;
		if (type->numInstances == 0)
		{
			ABLSymbolFreeCallback(type);
			type = nullptr;
		}
	}
}

//***************************************************************************
// SYMBOL TABLE routines
//***************************************************************************

void recordLibraryUsed(const std::unique_ptr<SymTableNode>& memberNodePtr)
{
	//------------------------------------------------------------------
	// If the library already on our list, then don't bother. Otherwise,
	// add it to our list...
	const std::unique_ptr<ABLModule>& library = memberNodePtr->library;
	for (size_t i = 0; i < NumLibrariesUsed; i++)
		if (LibrariesUsed[i] == library)
			return;
	//---------------------------------------------------
	// New library, so record it if we still have room...
	if (NumLibrariesUsed > MAX_LIBRARIES_USED)
		ABL_Fatal(0, " ABL: Too many libraries referenced from module ");
	LibrariesUsed[NumLibrariesUsed++] = library;
}

//***************************************************************************

const std::unique_ptr<SymTableNode>&
searchSymTable(std::wstring_view name, const std::unique_ptr<SymTableNode>& nodePtr)
{
	while (nodePtr)
	{
		int32_t compareResult = strcmp(name, nodePtr->name);
		if (compareResult == 0)
			return (nodePtr);
		if (compareResult < 0)
			nodePtr = nodePtr->left;
		else
			nodePtr = nodePtr->right;
	}
	return (nullptr);
}

//***************************************************************************

const std::unique_ptr<SymTableNode>&
searchSymTableForFunction(std::wstring_view name, const std::unique_ptr<SymTableNode>& nodePtr)
{
	while (nodePtr)
	{
		int32_t compareResult = strcmp(name, nodePtr->name);
		if (compareResult == 0)
		{
			if (nodePtr->ptype == nullptr)
				if (nodePtr->defn.key == DFN_FUNCTION)
					return (nodePtr);
			nodePtr = nodePtr->right;
		}
		else if (compareResult < 0)
			nodePtr = nodePtr->left;
		else
			nodePtr = nodePtr->right;
	}
	return (nullptr);
}

//***************************************************************************

const std::unique_ptr<SymTableNode>&
searchSymTableForState(std::wstring_view name, const std::unique_ptr<SymTableNode>& nodePtr)
{
	while (nodePtr)
	{
		int32_t compareResult = strcmp(name, nodePtr->name);
		if (compareResult == 0)
		{
			if (nodePtr->ptype == nullptr)
				if (nodePtr->defn.key == DFN_FUNCTION)
					if (nodePtr->defn.info.routine.flags & ROUTINE_FLAG_STATE)
						return (nodePtr);
			nodePtr = nodePtr->right;
		}
		else if (compareResult < 0)
			nodePtr = nodePtr->left;
		else
			nodePtr = nodePtr->right;
	}
	return (nullptr);
}

//***************************************************************************

const std::unique_ptr<SymTableNode>&
searchSymTableForString(std::wstring_view name, const std::unique_ptr<SymTableNode>& nodePtr)
{
	while (nodePtr)
	{
		int32_t compareResult = strcmp(name, nodePtr->name);
		if (compareResult == 0)
		{
			if (nodePtr->ptype)
				if (nodePtr->ptype->form == FRM_ARRAY)
					if (nodePtr->ptype->info.array.elementTypePtr == CharTypePtr)
						return (nodePtr);
			nodePtr = nodePtr->right;
		}
		else if (compareResult < 0)
			nodePtr = nodePtr->left;
		else
			nodePtr = nodePtr->right;
	}
	return (nullptr);
}

//***************************************************************************

const std::unique_ptr<SymTableNode>&
searchLibrarySymTable(std::wstring_view name, const std::unique_ptr<SymTableNode>& nodePtr)
{
	//-------------------------------------------------------------
	// Since all libraries are at the symbol display 0-level, we'll
	// check the local symbol table of all libraries. WARNING: This
	// will find the FIRST instance of a symbol with that name,
	// so don't load two libraries with a similarly named function
	// or variable, otherwise you may not get the one you want
	// unless you explicitly reference the library you want
	// (e.g. testLib.fudge, rather than just fudge). This is WAY
	// inefficient compared to simply knowing the library we want,
	// so any ABL programmer that causes this function to be called
	// should be shot --gd 9/29/97
	if (nodePtr)
	{
		int32_t compareResult = strcmp(name, nodePtr->name);
		if (compareResult == 0)
			return (nodePtr);
		else
		{
			if (nodePtr->library && (nodePtr->defn.key == DFN_MODULE))
			{
				const std::unique_ptr<SymTableNode>& memberNodePtr =
					searchSymTable(name, nodePtr->defn.info.routine.localSymTable);
				if (memberNodePtr)
					return (memberNodePtr);
			}
			const std::unique_ptr<SymTableNode>& nodeFoundPtr = searchLibrarySymTable(name, nodePtr->left);
			if (nodeFoundPtr)
				return (nodeFoundPtr);
			nodeFoundPtr = searchLibrarySymTable(name, nodePtr->right);
			if (nodeFoundPtr)
				return (nodeFoundPtr);
		}
	}
	return (nullptr);
}

//***************************************************************************

const std::unique_ptr<SymTableNode>&
searchLibrarySymTableDisplay(std::wstring_view name)
{
	const std::unique_ptr<SymTableNode>& nodePtr = searchLibrarySymTable(name, SymTableDisplay[0]);
	return (nodePtr);
}

//***************************************************************************

const std::unique_ptr<SymTableNode>&
searchSymTableDisplay(std::wstring_view name)
{
	//---------------------------------------------------------------------
	// First check if this is an explicit library reference. If so, we need
	// to determine which library and which identifier in that library...
	std::wstring_view separator = strchr(name, '.');
	const std::unique_ptr<SymTableNode>& nodePtr = nullptr;
	if (separator)
	{
		*separator = nullptr;
		const std::unique_ptr<SymTableNode>& libraryNodePtr = searchSymTable(name, SymTableDisplay[0]);
		if (!libraryNodePtr)
			return (nullptr);
		//-------------------------------------
		// Now, search for the member symbol...
		std::wstring_view memberName = separator + 1;
		const std::unique_ptr<SymTableNode>& memberNodePtr =
			searchSymTable(memberName, libraryNodePtr->defn.info.routine.localSymTable);
		if (memberNodePtr)
			recordLibraryUsed(memberNodePtr);
		return (memberNodePtr);
	}
	else
	{
		for (size_t i = level; i >= 0; i--)
		{
			const std::unique_ptr<SymTableNode>& nodePtr = searchSymTable(name, SymTableDisplay[i]);
			if (nodePtr)
				return (nodePtr);
		}
		//------------------------------------------------------------
		// We haven't found it, so maybe it's from a library but just
		// is not explicitly called with the library name. Since all
		// libraries are at the symbol table's 0-level, we'll check
		// the local symbol table of all libraries. WARNING: This
		// will find the FIRST instance of a symbol with that name,
		// so don't load two libraries with a similarly named function
		// or variable, otherwise you may not get the one you want
		// unless you explicitly reference the library you want
		// (e.g. testLib.fudge, rather than just fudge)...
		nodePtr = searchLibrarySymTableDisplay(name);
		if (nodePtr)
			recordLibraryUsed(nodePtr);
	}
	return (nodePtr);
}

//***************************************************************************

const std::unique_ptr<SymTableNode>&
enterSymTable(std::wstring_view name, const std::unique_ptr<SymTableNode>&* ptrToNodePtr)
{
	//-------------------------------------
	// First, create the new symbol node...
	const std::unique_ptr<SymTableNode>& newNode = (const std::unique_ptr<SymTableNode>&)ABLSymbolMallocCallback(sizeof(SymTableNode));
	if (!newNode)
		ABL_Fatal(0, " ABL: Unable to AblSymTableHeap->malloc symbol ");
	newNode->name = (std::wstring_view)ABLSymbolMallocCallback(strlen(name) + 1);
	if (!newNode->name)
		ABL_Fatal(0, " ABL: Unable to AblSymTableHeap->malloc symbol name ");
	strcpy(newNode->name, name);
	newNode->left = nullptr;
	newNode->parent = nullptr;
	newNode->right = nullptr;
	newNode->next = nullptr;
	newNode->info = nullptr;
	newNode->defn.key = DFN_UNDEFINED;
	newNode->defn.info.data.varType = VAR_TYPE_NORMAL;
	newNode->defn.info.data.offset = 0;
	newNode->ptype = nullptr;
	newNode->level = (uint8_t)level;
	newNode->labelIndex = 0;
	//-------------------------------------
	// Find where to put this new symbol...
	const std::unique_ptr<SymTableNode>& curNode = *ptrToNodePtr;
	const std::unique_ptr<SymTableNode>& parentNode = nullptr;
	while (curNode)
	{
		if (strcmp(name, curNode->name) < 0)
			ptrToNodePtr = &(curNode->left);
		else
			ptrToNodePtr = &(curNode->right);
		parentNode = curNode;
		curNode = *ptrToNodePtr;
	}
	newNode->parent = parentNode;
	*ptrToNodePtr = newNode;
	return (newNode);
}

//***************************************************************************

const std::unique_ptr<SymTableNode>&
insertSymTable(const std::unique_ptr<SymTableNode>&* tableRoot, const std::unique_ptr<SymTableNode>& newNode)
{
	newNode->left = nullptr;
	newNode->parent = nullptr;
	newNode->right = nullptr;
	//------------------------------------
	// Find where to insert this symbol...
	const std::unique_ptr<SymTableNode>& curNode = *tableRoot;
	const std::unique_ptr<SymTableNode>& parentNode = nullptr;
	while (curNode)
	{
		if (strcmp(newNode->name, curNode->name) < 0)
			tableRoot = &(curNode->left);
		else
			tableRoot = &(curNode->right);
		parentNode = curNode;
		curNode = *tableRoot;
	}
	newNode->parent = parentNode;
	*tableRoot = newNode;
	return (newNode);
}

//***************************************************************************

const std::unique_ptr<SymTableNode>&
extractSymTable(const std::unique_ptr<SymTableNode>&* tableRoot, const std::unique_ptr<SymTableNode>& nodeKill)
{
	//------------------------------------------------------------------------
	// NOTE: While this routine extracts a node from the symbol table,
	// it does not account for other nodes in the table that may be pointing
	// to the now-extracted node. Currently, this is not a problem as this
	// routine is really just used to extract the module's Identifier at level
	// 0 in the SymTable Display. Do we want to eliminate the use of the
	// parent pointer, and just hardcode something that may be more efficient
	// for this level-0 special case?
	const std::unique_ptr<SymTableNode>& nodeX = nullptr;
	const std::unique_ptr<SymTableNode>& nodeY = nullptr;
	if ((nodeKill->left == nullptr) || (nodeKill->right == nullptr))
		nodeY = nodeKill;
	else
		nodeY = symTableSuccessor(nodeKill);
	if (nodeY->left)
		nodeX = nodeY->left;
	else
		nodeX = nodeY->right;
	if (nodeX)
		nodeX->parent = nodeY->parent;
	if (nodeY->parent == nullptr)
		*tableRoot = nodeX;
	else if (nodeY == nodeY->parent->left)
		nodeY->parent->left = nodeX;
	else
		nodeY->parent->right = nodeX;
	if (nodeY != nodeKill)
	{
		//--------------------------------------
		// Copy y data to nodeKill's position...
		nodeKill->next = nodeY->next;
		nodeKill->name = nodeY->name;
		nodeKill->info = nodeY->info;
		memcpy(&nodeKill->defn, &nodeY->defn, sizeof(Definition));
		nodeKill->ptype = nodeY->ptype;
		nodeKill->level = nodeY->level;
		nodeKill->labelIndex = nodeY->labelIndex;
	}
	return (nodeY);
}

//***************************************************************************

void enterStandardRoutine(std::wstring_view name, int32_t routineKey, bool isOrder, std::wstring_view paramList,
	std::wstring_view returnType, void (*callback)(void))
{
	int32_t tableIndex = routineKey;
	if (tableIndex == -1)
	{
		if (NumStandardFunctions == MAX_STANDARD_FUNCTIONS)
			ABL_Fatal(0, " ABL.enterStandardRoutine: Too Many Standard Functions ");
		tableIndex = NumStandardFunctions++;
	}
	const std::unique_ptr<SymTableNode>& routineIdPtr;
	enterNameLocalSymTable(routineIdPtr, name);
	routineIdPtr->defn.key = DFN_FUNCTION;
	routineIdPtr->defn.info.routine.key = (RoutineKey)tableIndex;
	routineIdPtr->defn.info.routine.flags = isOrder ? ROUTINE_FLAG_ORDER : 0;
	routineIdPtr->defn.info.routine.params = nullptr;
	routineIdPtr->defn.info.routine.localSymTable = nullptr;
	routineIdPtr->library = nullptr;
	routineIdPtr->ptype = nullptr;
	FunctionInfoTable[tableIndex].numParams = 0;
	if (paramList)
	{
		FunctionInfoTable[tableIndex].numParams = strlen(paramList);
		if (FunctionInfoTable[tableIndex].numParams > MAX_FUNCTION_PARAMS)
			ABL_Fatal(0,
				" ABL.enterStandardRoutine: Too Many Standard "
				"Function Params ");
		for (size_t i = 0; i < FunctionInfoTable[tableIndex].numParams; i++)
			switch (paramList[i])
			{
			case '?':
				FunctionInfoTable[tableIndex].params[i] = PARAM_TYPE_ANYTHING;
				break;
			case 'c':
				FunctionInfoTable[tableIndex].params[i] = PARAM_TYPE_CHAR;
				break;
			case 'i':
				FunctionInfoTable[tableIndex].params[i] = PARAM_TYPE_INTEGER;
				break;
			case 'r':
				FunctionInfoTable[tableIndex].params[i] = PARAM_TYPE_REAL;
				break;
			case 'b':
				FunctionInfoTable[tableIndex].params[i] = PARAM_TYPE_BOOLEAN;
				break;
			case '*':
				FunctionInfoTable[tableIndex].params[i] = PARAM_TYPE_INTEGER_REAL;
				break;
			case 'C':
				FunctionInfoTable[tableIndex].params[i] = PARAM_TYPE_CHAR_ARRAY;
				break;
			case 'I':
				FunctionInfoTable[tableIndex].params[i] = PARAM_TYPE_INTEGER_ARRAY;
				break;
			case 'R':
				FunctionInfoTable[tableIndex].params[i] = PARAM_TYPE_REAL_ARRAY;
				break;
			case 'B':
				FunctionInfoTable[tableIndex].params[i] = PARAM_TYPE_BOOLEAN_ARRAY;
				break;
			default:
			{
				// wchar_t err[255];
				// sprintf(err, " ABL.enterStandardRoutine: bad param type (%c)
				// for (%s)", paramList[i], name);  ABL_Fatal(0, err);
				NODEFAULT;
			}
			}
	}
	FunctionInfoTable[tableIndex].returnType = RETURN_TYPE_NONE;
	if (returnType)
		switch (returnType[0])
		{
		// case 'c':
		//	FunctionInfoTable[NumStandardFunctions].returnType =
		// RETURN_TYPE_CHAR; 	break;
		case 'i':
			FunctionInfoTable[tableIndex].returnType = RETURN_TYPE_INTEGER;
			break;
		case 'r':
			FunctionInfoTable[tableIndex].returnType = RETURN_TYPE_REAL;
			break;
		case 'b':
			FunctionInfoTable[tableIndex].returnType = RETURN_TYPE_BOOLEAN;
			break;
		default:
		{
			// wchar_t err[255];
			// sprintf(err, " ABL.enterStandardRoutine: bad return type for
			// (%s)", name);  ABL_Fatal(0, err);
			NODEFAULT;
		}
		}
	FunctionCallbackTable[tableIndex] = callback;
}

//***************************************************************************

void enterScope(const std::unique_ptr<SymTableNode>& symTableRoot)
{
	if (++level >= MAX_NESTING_LEVEL)
		syntaxError(ABL_ERR_SYNTAX_NESTING_TOO_DEEP);
	SymTableDisplay[level] = symTableRoot;
}

//***************************************************************************

const std::unique_ptr<SymTableNode>&
exitScope(void)
{
	return (SymTableDisplay[level--]);
}

//***************************************************************************
void initStandardRoutines();

void initSymTable(void)
{
	//---------------------------------
	// Init the level-0 symbol table...
	SymTableDisplay[0] = nullptr;
	//----------------------------------------------------------------------
	// Set up the basic variable types as identifiers in the symbol table...
	const std::unique_ptr<SymTableNode>& integerIdPtr;
	enterNameLocalSymTable(integerIdPtr, "integer");
	const std::unique_ptr<SymTableNode>& charIdPtr;
	enterNameLocalSymTable(charIdPtr, "wchar_t");
	const std::unique_ptr<SymTableNode>& realIdPtr;
	enterNameLocalSymTable(realIdPtr, "real");
	const std::unique_ptr<SymTableNode>& booleanIdPtr;
	enterNameLocalSymTable(booleanIdPtr, "boolean");
	const std::unique_ptr<SymTableNode>& falseIdPtr;
	enterNameLocalSymTable(falseIdPtr, "false");
	const std::unique_ptr<SymTableNode>& trueIdPtr;
	enterNameLocalSymTable(trueIdPtr, "true");
	//------------------------------------------------------------------
	// Now, create the basic variable TYPEs, and point their identifiers
	// to their proper type definition...
	IntegerTypePtr = createType();
	if (!IntegerTypePtr)
		ABL_Fatal(0, " ABL: Unable to AblSymTableHeap->malloc Integer Type ");
	CharTypePtr = createType();
	if (!CharTypePtr)
		ABL_Fatal(0, " ABL: Unable to AblSymTableHeap->malloc Char Type ");
	RealTypePtr = createType();
	if (!RealTypePtr)
		ABL_Fatal(0, " ABL: Unable to AblSymTableHeap->malloc Real Type ");
	BooleanTypePtr = createType();
	if (!BooleanTypePtr)
		ABL_Fatal(0, " ABL: Unable to AblSymTableHeap->malloc Boolean Type ");
	integerIdPtr->defn.key = DFN_TYPE;
	integerIdPtr->ptype = IntegerTypePtr;
	IntegerTypePtr->form = FRM_SCALAR;
	IntegerTypePtr->size = sizeof(int32_t);
	IntegerTypePtr->typeIdPtr = integerIdPtr;
	charIdPtr->defn.key = DFN_TYPE;
	charIdPtr->ptype = CharTypePtr;
	CharTypePtr->form = FRM_SCALAR;
	CharTypePtr->size = sizeof(wchar_t);
	CharTypePtr->typeIdPtr = charIdPtr;
	realIdPtr->defn.key = DFN_TYPE;
	realIdPtr->ptype = RealTypePtr;
	RealTypePtr->form = FRM_SCALAR;
	RealTypePtr->size = sizeof(float);
	RealTypePtr->typeIdPtr = realIdPtr;
	booleanIdPtr->defn.key = DFN_TYPE;
	booleanIdPtr->ptype = BooleanTypePtr;
	BooleanTypePtr->form = FRM_ENUM;
	BooleanTypePtr->size = sizeof(int32_t);
	BooleanTypePtr->typeIdPtr = booleanIdPtr;
	//----------------------------------------------------
	// Set up the FALSE identifier for the boolean type...
	BooleanTypePtr->info.enumeration.max = 1;
	((const std::unique_ptr<Type>&)(booleanIdPtr->ptype))->info.enumeration.constIdPtr = falseIdPtr;
	falseIdPtr->defn.key = DFN_CONST;
	falseIdPtr->defn.info.constant.value.integer = 0;
	falseIdPtr->ptype = BooleanTypePtr;
	//----------------------------------------------------
	// Set up the TRUE identifier for the boolean type...
	falseIdPtr->next = trueIdPtr;
	trueIdPtr->defn.key = DFN_CONST;
	trueIdPtr->defn.info.constant.value.integer = 1;
	trueIdPtr->ptype = BooleanTypePtr;
	//-------------------------------------------
	// Set up the standard, built-in functions...
	//(std::wstring_view name, int32_t routineKey, bool isOrder, std::wstring_view paramList, std::wstring_view
	// returnType, PVOID callback);
	enterStandardRoutine("return", RTN_RETURN, false, nullptr, nullptr, nullptr);
	enterStandardRoutine("print", RTN_PRINT, false, nullptr, nullptr, nullptr);
	enterStandardRoutine("concat", RTN_CONCAT, false, nullptr, nullptr, nullptr);
	enterStandardRoutine("getstatehandle", RTN_GET_STATE_HANDLE, false, nullptr, nullptr, nullptr);
	//-----------------------------------
	// Honor Bound-specific extensions...
	//-----------------------------------
	initStandardRoutines();
}

//***************************************************************************

} // namespace mclib::abl
