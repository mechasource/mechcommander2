//===========================================================================//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//
//***************************************************************************
//
//								ABLENV.CPP
//
//***************************************************************************
#include "stdinc.h"

//#include "ablgen.h"
//#include "ablerr.h"
//#include "ablscan.h"
//#include "ablexec.h"
//#include "ablenv.h"
//#include "abldbug.h"

namespace mclib::abl
{

//***************************************************************************
int32_t
ABLi_preProcess(std::wstring_view sourceFileName, int32_t* numErrors = nullptr,
	int32_t* numLinesProcessed = nullptr, int32_t* numFilesProcessed = nullptr,
	bool printLines = false);

const std::unique_ptr<ABLModule>&
ABLi_loadLibrary(std::wstring_view sourceFileName, int32_t* numErrors = nullptr,
	int32_t* numLinesProcessed = nullptr, int32_t* numFilesProcessed = nullptr,
	bool printLines = false, bool createInstance = true);

//-------------------
// EXTERNAL variables
extern int32_t lineNumber;
extern int32_t errorCount;
extern int32_t execStatementCount;

extern TokenCodeType curToken;
extern wchar_t wordString[];
extern const std::unique_ptr<SymTableNode>& symTableDisplay[];
extern int32_t level;
extern bool blockFlag;
extern BlockType blockType;
extern bool printFlag;
extern const std::unique_ptr<SymTableNode>& CurModuleIdPtr;
extern const std::unique_ptr<SymTableNode>& CurRoutineIdPtr;
extern int32_t CurModuleHandle;
extern bool CallModuleInit;
extern int32_t FileNumber;

extern Type DummyType;
extern std::wstring_view codeBuffer;
extern std::wstring_view codeBufferPtr;
extern StackItem* stack;
// extern StackItem*		eternalStack;
extern const std::unique_ptr<StackItem>& tos;
extern const std::unique_ptr<StackItem>& stackFrameBasePtr;
extern int32_t eternalOffset;

extern TokenCodeType statementStartList[];
extern TokenCodeType statementEndList[];
extern TokenCodeType declarationStartList[];

extern wchar_t tokenString[MAXLEN_TOKENSTRING];

extern CharCodeType charTable[256];
extern ABLFile* sourceFile;

extern wchar_t sourceBuffer[MAXLEN_SOURCELINE];
extern int32_t bufferOffset;
extern std::wstring_view bufferp;
extern std::wstring_view tokenp;

extern int32_t digitCount;
extern bool countError;

extern bool eofFlag;
extern int32_t pageNumber;

extern const std::unique_ptr<SymTableNode>& SymTableDisplay[MAX_NESTING_LEVEL];
extern const std::unique_ptr<Type>& IntegerTypePtr;
extern const std::unique_ptr<Type>& RealTypePtr;
extern const std::unique_ptr<Type>& BooleanTypePtr;

extern uint32_t* OrderCompletionFlags;
extern const std::unique_ptr<StackItem>& StaticDataPtr;
extern StackItem returnValue;

extern const std::unique_ptr<Debugger>& debugger;
extern int32_t* EternalVariablesSizes;

//-----------------------
// CLASS static variables
int32_t NumModules = 0;

//-----------------
// GLOBAL variables
const std::unique_ptr<ModuleEntry>& ModuleRegistry = nullptr;
const std::unique_ptr<ABLModule>&* ModuleInstanceRegistry = nullptr;
int32_t MaxModules = 0;
int32_t NumModulesRegistered = 0;
int32_t NumModuleInstances = 0;
int32_t MaxWatchesPerModule = 20;
int32_t MaxBreakPointsPerModule = 20;
const std::unique_ptr<ABLModule>& CurModule = nullptr;
const std::unique_ptr<ABLModule>& CurFSM = nullptr;
const std::unique_ptr<ABLModule>& CurLibrary = nullptr;
const std::unique_ptr<ABLModule>&* LibraryInstanceRegistry = nullptr;
int32_t NumStateTransitions = 0;
int32_t MaxLibraries = 0;
bool NewStateSet = false;
extern int32_t numLibrariesLoaded;

extern int32_t NumExecutions;
int32_t CallStackLevel = 0;

#define MAX_PROFILE_LINELEN 128
#define MAX_PROFILE_LINES 256

int32_t NumProfileLogLines = 0;
int32_t TotalProfileLogLines = 0;
wchar_t ProfileLogBuffer[MAX_PROFILE_LINES][MAX_PROFILE_LINELEN];
ABLFile* ProfileLog = nullptr;
int32_t ProfileLogFunctionTimeLimit = 5;

const std::unique_ptr<UserFile>& UserFile::files[MAX_USER_FILES];

//***************************************************************************
// PROFILING LOG routines
//***************************************************************************

void DumpProfileLog(void)
{
	//----------------
	// Dump to file...
	for (size_t i = 0; i < NumProfileLogLines; i++)
		ProfileLog->writeString(ProfileLogBuffer[i]);
	NumProfileLogLines = 0;
}

//---------------------------------------------------------------------------

void ABL_CloseProfileLog(void)
{
	if (ProfileLog)
	{
		DumpProfileLog();
		wchar_t s[512];
		sprintf(s, "\nNum Total Lines = %d\n", TotalProfileLogLines);
		ProfileLog->writeString(s);
		ProfileLog->close();
		delete ProfileLog;
		ProfileLog = nullptr;
		NumProfileLogLines = 0;
		TotalProfileLogLines = 0;
	}
}

//---------------------------------------------------------------------------

void ABL_OpenProfileLog(void)
{
	if (ProfileLog)
		ABL_CloseProfileLog();
	NumProfileLogLines = 0;
	ProfileLog = new ABLFile;
	if (!ProfileLog)
		ABL_Fatal(0, " unable to malloc ABL ProfileLog ");
	if (ProfileLog->create("abl.log") != ABL_NO_ERR)
		ABL_Fatal(0, " unable to create ABL ProfileLog ");
}

//---------------------------------------------------------------------------

void ABL_AddToProfileLog(std::wstring_view profileString)
{
	if (NumProfileLogLines == MAX_PROFILE_LINES)
		DumpProfileLog();
	strncpy(ProfileLogBuffer[NumProfileLogLines], profileString, MAX_PROFILE_LINELEN - 1);
	ProfileLogBuffer[NumProfileLogLines][MAX_PROFILE_LINELEN] = nullptr;
	NumProfileLogLines++;
	TotalProfileLogLines++;
}

//***************************************************************************
// USER FILE routines
//***************************************************************************

PVOID
UserFile::operator new(size_t mySize)
{
	PVOID result = nullptr;
	result = ABLSystemMallocCallback(mySize);
	return (result);
}

//---------------------------------------------------------------------------

void UserFile::operator delete(PVOID us)
{
	ABLSystemFreeCallback(us);
}

//---------------------------------------------------------------------------

void UserFile::dump(void)
{
	//----------------
	// Dump to file...
	for (size_t i = 0; i < numLines; i++)
		pfile->writeString(lines[i]);
	numLines = 0;
}

//---------------------------------------------------------------------------

void UserFile::close(void)
{
	if (pfile && inUse)
	{
		dump();
		wchar_t s[512];
		sprintf(s, "\nNum Total Lines = %d\n", totalLines);
		pfile->writeString(s);
		pfile->close();
		inUse = false;
		numLines = 0;
		totalLines = 0;
	}
}

//---------------------------------------------------------------------------

int32_t
UserFile::open(std::wstring_view fileName)
{
	numLines = 0;
	totalLines = 0;
	if (pfile->create(fileName) != ABL_NO_ERR)
		return (-1);
	inUse = true;
	return (0);
}

//---------------------------------------------------------------------------

void UserFile::write(std::wstring_view s)
{
	static wchar_t buffer[MAX_USER_FILE_LINELEN];
	if (numLines == MAX_USER_FILE_LINES)
		dump();
	if (strlen(s) > (MAX_USER_FILE_LINELEN - 1))
		s[MAX_USER_FILE_LINELEN - 1] = nullptr;
	sprintf(buffer, "%s\n", s);
	strncpy(lines[numLines], buffer, MAX_USER_FILE_LINELEN - 1);
	numLines++;
	totalLines++;
}

//---------------------------------------------------------------------------

UserFile*
UserFile::getNewFile(void)
{
	int32_t fileHandle = -1;
	int32_t i;
	for (i = 0; i < MAX_USER_FILES; i++)
		if (!files[i]->inUse)
		{
			fileHandle = i;
			break;
		}
	return (files[i]);
}

//---------------------------------------------------------------------------

void UserFile::setup(void)
{
	for (size_t i = 0; i < MAX_USER_FILES; i++)
	{
		files[i] = (UserFile*)ABLSystemMallocCallback(sizeof(UserFile));
		files[i]->init();
		files[i]->handle = i;
		files[i]->inUse = false;
		files[i]->pfile = new ABLFile;
		if (!files[i]->pfile)
			ABL_Fatal(0, " ABL: Unable to malloc UserFiles ");
	}
}

//---------------------------------------------------------------------------

void UserFile::cleanup(void)
{
	for (size_t i = 0; i < MAX_USER_FILES; i++)
	{
		if (files[i])
		{
			if (files[i]->inUse)
				files[i]->close();
			// Should actually free the memory allocated above here!
			delete files[i]->pfile;
			files[i]->pfile = nullptr;
			ABLSystemFreeCallback(files[i]);
			files[i] = nullptr;
		}
	}
}

//***************************************************************************
// MODULE REGISTRY routines
//***************************************************************************

void initModuleRegistry(int32_t maxModules)
{
	//---------------------------------------------------------------------
	// First, set the max number of modules that may be loaded into the ABL
	// environment at a time...
	MaxModules = maxModules;
	//------------------------------
	// Create the module registry...
	ModuleRegistry = (const std::unique_ptr<ModuleEntry>&)ABLStackMallocCallback(sizeof(ModuleEntry) * MaxModules);
	if (!ModuleRegistry)
		ABL_Fatal(0, " ABL: Unable to AblStackHeap->malloc Module Registry ");
	memset(ModuleRegistry, 0, sizeof(ModuleEntry) * MaxModules);
	//-------------------------------------------------
	// Create the active (ABLModule) module registry...
	ModuleInstanceRegistry =
		(const std::unique_ptr<ABLModule>&*)ABLStackMallocCallback(sizeof(const std::unique_ptr<ABLModule>&) * MaxModules);
	if (!ModuleInstanceRegistry)
		ABL_Fatal(0, " ABL: Unable to malloc AblStackHeap->Module Instance Registry ");
	memset(ModuleInstanceRegistry, 0, sizeof(const std::unique_ptr<ABLModule>&) * MaxModules);
}

//***************************************************************************

void destroyModuleRegistry(void)
{
	//-----------------------------------------------------------
	// First, go through the registry, free'n each module and its
	// associated data...
	for (size_t i = 0; i < NumModulesRegistered; i++)
	{
		ABLStackFreeCallback(ModuleRegistry[i].fileName);
		ModuleRegistry[i].fileName = nullptr;
		ModuleRegistry[i].moduleIdPtr = nullptr;
		for (size_t j = 0; j < ModuleRegistry[i].numSourceFiles; j++)
		{
			ABLStackFreeCallback(ModuleRegistry[i].sourceFiles[j]);
			ModuleRegistry[i].sourceFiles[j] = nullptr;
		}
	}
	ABLStackFreeCallback(ModuleRegistry);
	ModuleRegistry = nullptr;
	ABLStackFreeCallback(ModuleInstanceRegistry);
	ModuleInstanceRegistry = nullptr;
}

//***************************************************************************
// LIBRARY REGISTRY routines
//***************************************************************************

void initLibraryRegistry(int32_t maxLibraries)
{
	//-----------------------------------------------------------------------
	// First, set the max number of libraries that may be loaded into the ABL
	// environment at a time...
	MaxLibraries = maxLibraries;
	//--------------------------------------------------
	// Create the active (ABLModule) library registry...
	LibraryInstanceRegistry =
		(const std::unique_ptr<ABLModule>&*)ABLStackMallocCallback(sizeof(const std::unique_ptr<ABLModule>&) * MaxLibraries);
	if (!LibraryInstanceRegistry)
		ABL_Fatal(0, " ABL: Unable to malloc AblStackHeap->Library Instance Registry ");
	memset(LibraryInstanceRegistry, 0, sizeof(const std::unique_ptr<ABLModule>&) * MaxLibraries);
}

//***************************************************************************

void destroyLibraryRegistry(void)
{
	//-----------------------------------------------------------------
	// Kinda need to do the same thing here as in the normal Registry.
	// Or leak o RAMA!!!!!!!
	// The actual data held by the pointer is removed in destroyModuleRegistry.
	// However, the classes holding the actual const std::unique_ptr<ABLModule>& are then responsible
	// for deleting the const std::unique_ptr<ABLModule>&.  Libraries have no owner class which, I'm
	// guessing, this class was supposed to do.  This class now does that!!!!
	// -fs		1/25/98
	for (size_t i = 0; i < numLibrariesLoaded; i++)
	{
		delete LibraryInstanceRegistry[i];
		LibraryInstanceRegistry[i] = nullptr;
	}
	ABLStackFreeCallback(LibraryInstanceRegistry);
	LibraryInstanceRegistry = nullptr;
}

//***************************************************************************
// ABLMODULE class
//***************************************************************************

PVOID
ABLModule::operator new(size_t mySize)
{
	PVOID result = nullptr;
	result = ABLSystemMallocCallback(mySize);
	return (result);
}

//---------------------------------------------------------------------------

void ABLModule::operator delete(PVOID us)
{
	ABLSystemFreeCallback(us);
}

//---------------------------------------------------------------------------
int32_t
ABLModule::getRealId(void)
{
	// Scan through the ModuleInstanceRegistry and find the pointer that matches
	// this.  DO NOT COUNT ANY NULLs!!!!  These will go away when we reload a
	// QuickSave!!
	int32_t actualCount = 0;
	bool foundBrain = false;
	for (size_t i = 0; i < NumModuleInstances; i++)
	{
		if (this == ModuleInstanceRegistry[i])
		{
			foundBrain = true;
			break;
		}
		if (ModuleInstanceRegistry[i])
			actualCount++;
	}
	if (!foundBrain)
		ABL_Fatal(0, "Could not find this Brain in the ModuleInstanceRegistry");
	return actualCount;
}

//---------------------------------------------------------------------------

int32_t
ABLModule::init(int32_t moduleHandle)
{
	if (moduleHandle == -1)
	{
		//----------
		// Clean up!
		return (-1);
	}
	id = NumModules++;
	handle = moduleHandle;
	staticData = nullptr;
	int32_t numStatics = ModuleRegistry[handle].numStaticVars;
	if (numStatics)
	{
		staticData = (const std::unique_ptr<StackItem>&)ABLStackMallocCallback(sizeof(StackItem) * numStatics);
		if (!staticData)
		{
			wchar_t err[255];
			sprintf(err, "ABL: Unable to AblStackHeap->malloc staticData [Module %d]", id);
			ABL_Fatal(0, err);
		}
		int32_t* sizeList = ModuleRegistry[handle].sizeStaticVars;
		for (size_t i = 0; i < numStatics; i++)
			if (sizeList[i] > 0)
			{
				staticData[i].address = (std::wstring_view)ABLStackMallocCallback(sizeList[i]);
				if (!staticData)
				{
					wchar_t err[255];
					sprintf(err,
						"ABL: Unable to AblStackHeap->malloc staticData "
						"address [Module %d]",
						id);
					ABL_Fatal(0, err);
				}
			}
			else
				staticData[i].integer = 0;
	}
	if (ModuleRegistry[handle].numOrderCalls)
	{
		int32_t numLongs = 1 + ModuleRegistry[handle].numOrderCalls / 32;
		orderCallFlags = (uint32_t*)ABLStackMallocCallback(sizeof(uint32_t) * numLongs);
		if (!orderCallFlags)
		{
			wchar_t err[255];
			sprintf(err,
				"ABL: Unable to AblStackHeap->malloc orderCallFlags [Module "
				"%d]",
				id);
			ABL_Fatal(0, err);
		}
		for (size_t i = 0; i < numLongs; i++)
			orderCallFlags[i] = 0;
	}
	ModuleRegistry[handle].numInstances++;
	initCalled = false;
	//------------------------------------------------------
	// This Active Module is now on the instance registry...
	ModuleInstanceRegistry[NumModuleInstances++] = this;
	if (debugger)
	{
		watchManager = new WatchManager;
		if (!watchManager)
			ABL_Fatal(0, " Unable to AblStackHeap->malloc WatchManager ");
		int32_t result = watchManager->init(MaxWatchesPerModule);
		if (result != ABL_NO_ERR)
			ABL_Fatal(0, " Unable to AblStackHeap->malloc WatchManager ");
		breakPointManager = new BreakPointManager;
		if (!breakPointManager)
			ABL_Fatal(0, " Unable to AblStackHeap->malloc BreakPointManager ");
		result = breakPointManager->init(MaxBreakPointsPerModule);
		if (result != ABL_NO_ERR)
			ABL_Fatal(0, " Unable to AblStackHeap->malloc BreakPointManager ");
	}
	if (ModuleRegistry[handle].moduleIdPtr->defn.info.routine.flags & ROUTINE_FLAG_FSM)
	{
		//--------------------------------
		// Always starts in START state...
		const std::unique_ptr<SymTableNode>& startState = searchSymTable(
			"start", ModuleRegistry[handle].moduleIdPtr->defn.info.routine.localSymTable);
		if (!startState)
		{
			wchar_t err[255];
			sprintf(err, "ABL: FSM has no Start state [%s]", CurModule->getName());
			ABL_Fatal(0, err);
		}
		prevState = nullptr;
		state = startState;
	}
	//--------------------
	// Can this ever fail?
	return (ABL_NO_ERR);
}

//---------------------------------------------------------------------------

void ABLModule::write(ABLFile* moduleFile)
{
	moduleFile->writeString(name);
	moduleFile->writeByte(nullptr);
	moduleFile->writeLong(handle);
	if (prevState == nullptr)
		moduleFile->writeString("NULLPrevState");
	else
		moduleFile->writeString(prevState->name);
	moduleFile->writeByte(nullptr);
	if (state == nullptr)
		moduleFile->writeString("NULLState");
	else
		moduleFile->writeString(state->name);
	moduleFile->writeByte(nullptr);
	moduleFile->writeLong(initCalled ? 1 : 0);
	int32_t numStatics = ModuleRegistry[handle].numStaticVars;
	int32_t* sizeList = ModuleRegistry[handle].sizeStaticVars;
	for (size_t i = 0; i < numStatics; i++)
	{
		if (sizeList[i] > 0)
			moduleFile->write((uint8_t*)staticData[i].address, sizeList[i]);
		else
			moduleFile->write((uint8_t*)&staticData[i], sizeof(StackItem));
	}
}

//---------------------------------------------------------------------------

void ABLModule::read(ABLFile* moduleFile)
{
	//----------------------------------------------------------------------------
	// If this is called on a newly init'd module, then it will do all
	// appropriate memory alloc, etc. If it's being called on a module that's
	// already been setup (via a call to init(moduleHandle)), then it simply
	// loads the module's data...
	bool fresh = (id == -1);
	if (fresh)
	{
		id = NumModules++;
		moduleFile->readString((uint8_t*)name);
		handle = moduleFile->readLong();
		staticData = nullptr;
	}
	else
	{
		wchar_t tempName[1024];
		moduleFile->readString((uint8_t*)tempName);
		// int32_t ignore = moduleFile->readLong();
	}
	wchar_t stateName[256];
	memset(stateName, 0, 256);
	moduleFile->readString((uint8_t*)stateName);
	prevState = nullptr;
	if (strcmp(stateName, "NULLPrevState"))
		prevState = findState(stateName);
	memset(stateName, 0, 256);
	moduleFile->readString((uint8_t*)stateName);
	state = nullptr;
	if (strcmp(stateName, "NULLState"))
		state = findState(stateName);
	bool savedInitCalled = (moduleFile->readLong() == 1);
	int32_t numStatics = ModuleRegistry[handle].numStaticVars;
	if (numStatics)
	{
		if (fresh)
		{
			staticData = (const std::unique_ptr<StackItem>&)ABLStackMallocCallback(sizeof(StackItem) * numStatics);
			if (!staticData)
			{
				wchar_t err[255];
				sprintf(err,
					"ABL: Unable to AblStackHeap->malloc staticData [Module "
					"%d]",
					id);
				ABL_Fatal(0, err);
			}
		}
		int32_t* sizeList = ModuleRegistry[handle].sizeStaticVars;
		for (size_t i = 0; i < numStatics; i++)
			if (sizeList[i] > 0)
			{
				if (fresh)
				{
					staticData[i].address = (std::wstring_view)ABLStackMallocCallback(sizeList[i]);
					if (!staticData)
					{
						wchar_t err[255];
						sprintf(err,
							"ABL: Unable to AblStackHeap->malloc staticData "
							"address [Module %d]",
							id);
						ABL_Fatal(0, err);
					}
				}
				int32_t result = moduleFile->read((uint8_t*)staticData[i].address, sizeList[i]);
				if (!result)
				{
					wchar_t err[255];
					sprintf(err, "ABL: Unable to read staticData.address [Module %d]", id);
					ABL_Fatal(0, err);
				}
			}
			else
			{
				staticData[i].integer = 0;
				int32_t result = moduleFile->read((uint8_t*)&staticData[i], sizeof(StackItem));
				if (!result)
				{
					wchar_t err[255];
					sprintf(err, "ABL: Unable to read staticData [Module %d]", id);
					ABL_Fatal(0, err);
				}
			}
	}
	if (ModuleRegistry[handle].numOrderCalls)
	{
		int32_t numLongs = 1 + ModuleRegistry[handle].numOrderCalls / 32;
		orderCallFlags = (uint32_t*)ABLStackMallocCallback(sizeof(uint32_t) * numLongs);
		if (!orderCallFlags)
		{
			wchar_t err[255];
			sprintf(err,
				"ABLModule.read: Unable to AblStackHeap->malloc orderCallFlags "
				"[Module %d]",
				id);
			ABL_Fatal(0, err);
		}
		for (size_t i = 0; i < numLongs; i++)
			orderCallFlags[i] = 0;
	}
	if (fresh)
	{
		ModuleRegistry[handle].numInstances++;
		initCalled = savedInitCalled;
		//------------------------------------------------------
		// This Active Module is now on the instance registry...
		ModuleInstanceRegistry[NumModuleInstances++] = this;
		if (debugger)
		{
			watchManager = new WatchManager;
			if (!watchManager)
				ABL_Fatal(0, " Unable to AblStackHeap->malloc WatchManager ");
			int32_t result = watchManager->init(MaxWatchesPerModule);
			if (result != ABL_NO_ERR)
				ABL_Fatal(0, " Unable to AblStackHeap->malloc WatchManager ");
			breakPointManager = new BreakPointManager;
			if (!breakPointManager)
				ABL_Fatal(0, " Unable to AblStackHeap->malloc BreakPointManager ");
			result = breakPointManager->init(MaxBreakPointsPerModule);
			if (result != ABL_NO_ERR)
				ABL_Fatal(0, " Unable to AblStackHeap->malloc BreakPointManager ");
		}
	}
}

//---------------------------------------------------------------------------

std::wstring_view
ABLModule::getFileName(void)
{
	return (ModuleRegistry[handle].fileName);
}

//---------------------------------------------------------------------------

void ABLModule::setName(std::wstring_view _name)
{
	strncpy(name, _name, MAX_ABLMODULE_NAME);
	name[MAX_ABLMODULE_NAME] = nullptr;
}

//---------------------------------------------------------------------------

bool ABLModule::isLibrary(void)
{
	return (ModuleRegistry[handle].moduleIdPtr->library != nullptr);
}

//---------------------------------------------------------------------------

void ABLModule::resetOrderCallFlags(void)
{
	if (ModuleRegistry[handle].numOrderCalls == 0)
		return;
	int32_t numLongs = 1 + ModuleRegistry[handle].numOrderCalls / 32;
	for (size_t i = 0; i < numLongs; i++)
		orderCallFlags[i] = 0;
}

//---------------------------------------------------------------------------

void ABLModule::setOrderCallFlag(uint8_t dword, uint8_t bit)
{
	orderCallFlags[dword] |= (1 << bit);
}

//---------------------------------------------------------------------------

void ABLModule::clearOrderCallFlag(uint8_t dword, uint8_t bit)
{
	orderCallFlags[dword] &= ((1 << bit) ^ 0xFFFFFFFF);
}

//---------------------------------------------------------------------------

int32_t
ABLModule::getPrevStateHandle(void)
{
	if (!prevState)
		return (0);
	for (size_t i = 0; i < ModuleRegistry[handle].numStateHandles; i++)
		if (strcmp(prevState->name, ModuleRegistry[handle].stateHandles[i].name) == 0)
			return (i);
	return (0);
}

//---------------------------------------------------------------------------

int32_t
ABLModule::getStateHandle(void)
{
	if (!state)
		return (0);
	for (size_t i = 0; i < ModuleRegistry[handle].numStateHandles; i++)
		if (strcmp(state->name, ModuleRegistry[handle].stateHandles[i].name) == 0)
			return (i);
	return (0);
}

//---------------------------------------------------------------------------

int32_t
ABLModule::execute(const std::unique_ptr<ABLParam>& paramList)
{
	CurModule = this;
	if (debugger)
		debugger->setModule(this);
	//--------------------------
	// Execute the ABL module...
	const std::unique_ptr<SymTableNode>& moduleIdPtr = ModuleRegistry[handle].moduleIdPtr;
	if (moduleIdPtr->defn.info.routine.flags & ROUTINE_FLAG_FSM)
		CurFSM = this;
	else
		CurFSM = nullptr;
	NumStateTransitions = 0;
	//--------------------------------------------
	// Point to this module's static data space...
	StaticDataPtr = staticData;
	OrderCompletionFlags = orderCallFlags;
	//---------------------------------
	// Init some important variables...
	CurModuleIdPtr = nullptr;
	CurRoutineIdPtr = nullptr;
	FileNumber = -1;
	errorCount = 0;
	execStatementCount = 0;
	NumExecutions++;
	//------------------
	// Init the stack...
	stackFrameBasePtr = tos = (stack + eternalOffset);
	//---------------------------------------
	// Initialize the module's stack frame...
	level = 1;
	CallStackLevel = 0;
	stackFrameBasePtr = tos + 1;
	//-------------------------
	// Function return value...
	pushInteger(0);
	//---------------
	// Static Link...
	pushAddress(nullptr);
	//----------------
	// Dynamic Link...
	pushAddress(nullptr);
	//------------------
	// Return Address...
	pushAddress(nullptr);
	// initDebugger();
	//----------
	// Run it...
	if (paramList)
	{
		//------------------------------------------------------------------------------
		// NOTE: Currently, parameter passing of arrays is not functioning. This
		// MUST be done...
		int32_t curParam = 0;
		for (const std::unique_ptr<SymTableNode>& formalIdPtr = (const std::unique_ptr<SymTableNode>&)(moduleIdPtr->defn.info.routine.params);
			 formalIdPtr != nullptr; formalIdPtr = formalIdPtr->next)
		{
			const std::unique_ptr<Type>& formalTypePtr = (const std::unique_ptr<Type>&)(formalIdPtr->ptype);
			if (formalIdPtr->defn.key == DFN_VALPARAM)
			{
				if (formalTypePtr == RealTypePtr)
				{
					if (paramList[curParam].type == ABL_PARAM_INTEGER)
					{
						//---------------------------------------------
						// Real formal parameter, but integer actual...
						pushReal((float)(paramList[curParam].integer));
					}
					else if (paramList[curParam].type == ABL_PARAM_REAL)
						pushReal(paramList[curParam].real);
				}
				else if (formalTypePtr == IntegerTypePtr)
				{
					if (paramList[curParam].type == ABL_PARAM_INTEGER)
						pushInteger(paramList[curParam].integer);
					else
						return (0);
				}
				//----------------------------------------------------------
				// Formal parameter is an array or record, so make a copy...
				if ((formalTypePtr->form == FRM_ARRAY) /* || (formalTypePtr->form == FRM_RECORD)*/)
				{
					//------------------------------------------------------------------------------
					// The following is a little inefficient, but is kept this
					// way to keep it clear. Once it's verified to work,
					// optimize...
					int32_t size = formalTypePtr->size;
					std::wstring_view dest = (std::wstring_view)ABLStackMallocCallback((size_t)size);
					if (!dest)
					{
						wchar_t err[255];
						sprintf(err,
							"ABL: Unable to AblStackHeap->malloc array "
							"parameter [Module %d]",
							id);
						ABL_Fatal(0, err);
					}
					std::wstring_view src = tos->address;
					std::wstring_view savePtr = dest;
					memcpy(dest, src, size);
					tos->address = savePtr;
				}
			}
			else
			{
				//-------------------------------
				// pass by reference parameter...
				if (formalTypePtr == RealTypePtr)
					pushAddress((Address) & (paramList[curParam].real));
				else if (formalTypePtr == IntegerTypePtr)
					pushAddress((Address) & (paramList[curParam].integer));
				else
					return (0);
				// const std::unique_ptr<SymTableNode>& idPtr = getCodeSymTableNodePtr();
				// execVariable(idPtr, USE_REFPARAM);
			}
			curParam++;
		}
	}
	CurModuleHandle = handle;
	//--------------------------------------------------------------------
	// No init function in FSM. Put all init stuff into the start state...
	CallModuleInit = !initCalled;
	initCalled = true;
	NewStateSet = false;
	::execute(moduleIdPtr);
	memcpy(&returnVal, &returnValue, sizeof(StackItem));
	//-----------
	// Summary...
	return (execStatementCount);
}

//---------------------------------------------------------------------------

int32_t
ABLModule::execute(const std::unique_ptr<ABLParam>& moduleParamList, const std::unique_ptr<SymTableNode>& functionIdPtr)
{
	CurModule = this;
	if (debugger)
		debugger->setModule(this);
	//--------------------------
	// Execute the ABL module...
	const std::unique_ptr<SymTableNode>& moduleIdPtr = ModuleRegistry[handle].moduleIdPtr;
	if (moduleIdPtr->defn.info.routine.flags & ROUTINE_FLAG_FSM)
		CurFSM = this;
	else
		CurFSM = nullptr;
	NumStateTransitions = 0;
	//--------------------------------------------
	// Point to this module's static data space...
	StaticDataPtr = staticData;
	OrderCompletionFlags = orderCallFlags;
	//---------------------------------
	// Init some important variables...
	CurModuleIdPtr = nullptr;
	CurRoutineIdPtr = nullptr;
	FileNumber = -1;
	errorCount = 0;
	execStatementCount = 0;
	NumExecutions++;
	NewStateSet = false;
	//------------------
	// Init the stack...
	stackFrameBasePtr = tos = (stack + eternalOffset);
	//---------------------------------------
	// Initialize the module's stack frame...
	level = 1;
	CallStackLevel = 0;
	stackFrameBasePtr = tos + 1;
	//-------------------------
	// Function return value...
	pushInteger(0);
	//---------------
	// Static Link...
	pushAddress(nullptr);
	//----------------
	// Dynamic Link...
	pushAddress(nullptr);
	//------------------
	// Return Address...
	pushAddress(nullptr);
	// initDebugger();
	//----------
	// Run it...
	if (moduleParamList)
	{
		//------------------------------------------------------------------------------
		// NOTE: Currently, parameter passing of arrays is not functioning. This
		// MUST be done...
		int32_t curParam = 0;
		for (const std::unique_ptr<SymTableNode>& formalIdPtr = (const std::unique_ptr<SymTableNode>&)(moduleIdPtr->defn.info.routine.params);
			 formalIdPtr != nullptr; formalIdPtr = formalIdPtr->next)
		{
			const std::unique_ptr<Type>& formalTypePtr = (const std::unique_ptr<Type>&)(formalIdPtr->ptype);
			if (formalIdPtr->defn.key == DFN_VALPARAM)
			{
				if (formalTypePtr == RealTypePtr)
				{
					if (moduleParamList[curParam].type == ABL_PARAM_INTEGER)
					{
						//---------------------------------------------
						// Real formal parameter, but integer actual...
						pushReal((float)(moduleParamList[curParam].integer));
					}
					else if (moduleParamList[curParam].type == ABL_PARAM_REAL)
						pushReal(moduleParamList[curParam].real);
				}
				else if (formalTypePtr == IntegerTypePtr)
				{
					if (moduleParamList[curParam].type == ABL_PARAM_INTEGER)
						pushInteger(moduleParamList[curParam].integer);
					else
						return (0);
				}
				//----------------------------------------------------------
				// Formal parameter is an array or record, so make a copy...
				if ((formalTypePtr->form == FRM_ARRAY) /* || (formalTypePtr->form == FRM_RECORD)*/)
				{
					//------------------------------------------------------------------------------
					// The following is a little inefficient, but is kept this
					// way to keep it clear. Once it's verified to work,
					// optimize...
					int32_t size = formalTypePtr->size;
					std::wstring_view dest = (std::wstring_view)ABLStackMallocCallback((size_t)size);
					if (!dest)
					{
						wchar_t err[255];
						sprintf(err,
							"ABL: Unable to AblStackHeap->malloc array "
							"parameter [Module %d]",
							id);
						ABL_Fatal(0, err);
					}
					std::wstring_view src = tos->address;
					std::wstring_view savePtr = dest;
					memcpy(dest, src, size);
					tos->address = savePtr;
				}
			}
			else
			{
				//-------------------------------
				// pass by reference parameter...
				if (formalTypePtr == RealTypePtr)
					pushAddress((Address) & (moduleParamList[curParam].real));
				else if (formalTypePtr == IntegerTypePtr)
					pushAddress((Address) & (moduleParamList[curParam].integer));
				else
					return (0);
			}
			curParam++;
		}
	}
	CurModuleHandle = handle;
	CallModuleInit = !initCalled;
	initCalled = true;
	::executeChild(moduleIdPtr, functionIdPtr);
	memcpy(&returnVal, &returnValue, sizeof(StackItem));
	//-----------
	// Summary...
	return (execStatementCount);
}

//---------------------------------------------------------------------------

const std::unique_ptr<SymTableNode>&
ABLModule::findSymbol(
	std::wstring_view symbolName, const std::unique_ptr<SymTableNode>& curFunction, bool searchLibraries)
{
	if (curFunction)
	{
		const std::unique_ptr<SymTableNode>& symbol =
			searchSymTable(strlwr(symbolName), curFunction->defn.info.routine.localSymTable);
		if (symbol)
			return (symbol);
	}
	const std::unique_ptr<SymTableNode>& symbol = searchSymTable(
		strlwr(symbolName), ModuleRegistry[handle].moduleIdPtr->defn.info.routine.localSymTable);
	if (!symbol && searchLibraries)
	{
		for (size_t i = 0; i < ModuleRegistry[handle].numLibrariesUsed; i++)
		{
			symbol = searchSymTable(strlwr(symbolName),
				ModuleRegistry[ModuleRegistry[handle].librariesUsed[i]->handle]
					.moduleIdPtr->defn.info.routine.localSymTable);
			if (symbol)
				break;
		}
	}
	return (symbol);
}

//---------------------------------------------------------------------------

const std::unique_ptr<SymTableNode>&
ABLModule::findFunction(std::wstring_view functionName, bool searchLibraries)
{
	const std::unique_ptr<SymTableNode>& symbol = searchSymTableForFunction(
		functionName, ModuleRegistry[handle].moduleIdPtr->defn.info.routine.localSymTable);
	if (!symbol && searchLibraries)
	{
		for (size_t i = 0; i < ModuleRegistry[handle].numLibrariesUsed; i++)
		{
			wchar_t temp[1024];
			memset(temp, 0, 1024);
			strncpy(
				temp, functionName, (strlen(functionName) > 1020) ? 1020 : strlen(functionName));
			symbol = searchSymTable(_strlwr(temp),
				ModuleRegistry[ModuleRegistry[handle].librariesUsed[i]->handle]
					.moduleIdPtr->defn.info.routine.localSymTable);
			if (symbol)
				break;
		}
	}
	return (symbol);
}

//---------------------------------------------------------------------------

const std::unique_ptr<SymTableNode>&
ABLModule::findState(std::wstring_view stateName)
{
	const std::unique_ptr<SymTableNode>& symbol = searchSymTableForState(
		stateName, ModuleRegistry[handle].moduleIdPtr->defn.info.routine.localSymTable);
	return (symbol);
}

//---------------------------------------------------------------------------

int32_t
ABLModule::findStateHandle(std::wstring_view stateName)
{
	for (size_t i = 1; i < ModuleRegistry[handle].numStateHandles; i++)
		if (strcmp(stateName, ModuleRegistry[handle].stateHandles[i].name) == 0)
			return (i);
	return (0);
}

//---------------------------------------------------------------------------

int32_t
ABLModule::setStaticInteger(std::wstring_view name, int32_t value)
{
	const std::unique_ptr<SymTableNode>& symbol = findSymbol(name);
	if (!symbol)
		return (1);
	if (symbol->ptype != IntegerTypePtr)
		return (2);
	if (symbol->defn.info.data.varType != VAR_TYPE_STATIC)
		return (3);
	const std::unique_ptr<StackItem>& dataPtr = staticData + symbol->defn.info.data.offset;
	*((int32_t*)dataPtr) = value;
	return (0);
}

//---------------------------------------------------------------------------

int32_t
ABLModule::getStaticInteger(std::wstring_view name)
{
	const std::unique_ptr<SymTableNode>& symbol = findSymbol(name);
	if (!symbol)
		return (0xFFFFFFFF);
	if (symbol->ptype != IntegerTypePtr)
		return (0xFFFFFFFF);
	if (symbol->defn.info.data.varType != VAR_TYPE_STATIC)
		return (0xFFFFFFFF);
	const std::unique_ptr<StackItem>& dataPtr = staticData + symbol->defn.info.data.offset;
	return (*((int32_t*)dataPtr));
}

//---------------------------------------------------------------------------

int32_t
ABLModule::setStaticReal(std::wstring_view name, float value)
{
	const std::unique_ptr<SymTableNode>& symbol = findSymbol(name);
	if (!symbol)
		return (1);
	if (symbol->ptype != RealTypePtr)
		return (2);
	if (symbol->defn.info.data.varType != VAR_TYPE_STATIC)
		return (3);
	const std::unique_ptr<StackItem>& dataPtr = staticData + symbol->defn.info.data.offset;
	*((float*)dataPtr) = value;
	return (0);
}

//---------------------------------------------------------------------------

float ABLModule::getStaticReal(std::wstring_view name)
{
	const std::unique_ptr<SymTableNode>& symbol = findSymbol(name);
	if (!symbol)
		return (-999999.0);
	if (symbol->ptype != RealTypePtr)
		return (-999999.0);
	if (symbol->defn.info.data.varType != VAR_TYPE_STATIC)
		return (-999999.0);
	const std::unique_ptr<StackItem>& dataPtr = staticData + symbol->defn.info.data.offset;
	return (*((float*)dataPtr));
}

//---------------------------------------------------------------------------

int32_t
ABLModule::setStaticIntegerArray(std::wstring_view name, int32_t numValues, int32_t* values)
{
	const std::unique_ptr<SymTableNode>& symbol = findSymbol(name);
	if (!symbol)
		return (1);
	//--------------------------------------------------------------------------
	// NOTE: This function is not dummy-proof. Essentially, this routine copies
	// the values data into the array's data space WITHOUT checking to make sure
	// the array really is an array (single or mult-dimensional) of reals. User
	// beware!
	if (symbol->defn.info.data.varType != VAR_TYPE_STATIC)
		return (3);
	const std::unique_ptr<StackItem>& dataPtr = staticData + symbol->defn.info.data.offset;
	memcpy(dataPtr->address, values, 4 * numValues);
	return (0);
}

//---------------------------------------------------------------------------

int32_t
ABLModule::getStaticIntegerArray(std::wstring_view name, int32_t numValues, int32_t* values)
{
	const std::unique_ptr<SymTableNode>& symbol = findSymbol(name);
	if (!symbol)
		return (0);
	//--------------------------------------------------------------------------
	// NOTE: This function is not dummy-proof. Essentially, this routine copies
	// the values data into the array's data space WITHOUT checking to make sure
	// the array really is an array (single or mult-dimensional) of reals. User
	// beware!
	if (symbol->defn.info.data.varType != VAR_TYPE_STATIC)
		return (0);
	const std::unique_ptr<StackItem>& dataPtr = staticData + symbol->defn.info.data.offset;
	memcpy(values, dataPtr->address, 4 * numValues);
	return (1);
}

//---------------------------------------------------------------------------

int32_t
ABLModule::setStaticRealArray(std::wstring_view name, int32_t numValues, float* values)
{
	const std::unique_ptr<SymTableNode>& symbol = findSymbol(name);
	if (!symbol)
		return (1);
	//--------------------------------------------------------------------------
	// NOTE: This function is not dummy-proof. Essentially, this routine copies
	// the values data into the array's data space WITHOUT checking to make sure
	// the array really is an array (single or mult-dimensional) of reals. User
	// beware!
	if (symbol->defn.info.data.varType != VAR_TYPE_STATIC)
		return (3);
	const std::unique_ptr<StackItem>& dataPtr = staticData + symbol->defn.info.data.offset;
	memcpy(dataPtr->address, values, 4 * numValues);
	return (0);
}

//---------------------------------------------------------------------------

int32_t
ABLModule::getStaticRealArray(std::wstring_view name, int32_t numValues, float* values)
{
	const std::unique_ptr<SymTableNode>& symbol = findSymbol(name);
	if (!symbol)
		return (0);
	//--------------------------------------------------------------------------
	// NOTE: This function is not dummy-proof. Essentially, this routine copies
	// the values data into the array's data space WITHOUT checking to make sure
	// the array really is an array (single or mult-dimensional) of reals. User
	// beware!
	if (symbol->defn.info.data.varType != VAR_TYPE_STATIC)
		return (0);
	const std::unique_ptr<StackItem>& dataPtr = staticData + symbol->defn.info.data.offset;
	memcpy(values, dataPtr->address, 4 * numValues);
	return (1);
}

//---------------------------------------------------------------------------

std::wstring_view
ABLModule::getSourceFile(int32_t fileNumber)
{
	return (ModuleRegistry[handle].sourceFiles[fileNumber]);
}

//---------------------------------------------------------------------------

std::wstring_view
ABLModule::getSourceDirectory(int32_t fileNumber, std::wstring_view directory)
{
	std::wstring_view fileName = ModuleRegistry[handle].sourceFiles[fileNumber];
	int32_t curChar = strlen(fileName);
	while ((curChar > -1) && (fileName[curChar] != '\\'))
		curChar--;
	if (curChar == -1)
		return (nullptr);
	strcpy(directory, fileName);
	directory[curChar + 1] = nullptr;
	return (directory);
}

//---------------------------------------------------------------------------

void buildRoutineList(const std::unique_ptr<SymTableNode>& curSymbol, ModuleInfo* moduleInfo)
{
	if (curSymbol)
	{
		buildRoutineList(curSymbol->left, moduleInfo);
		if (curSymbol->defn.key == DFN_FUNCTION)
		{
			if (moduleInfo->numRoutines < 1024)
			{
				strcpy(moduleInfo->routineInfo[moduleInfo->numRoutines].name, curSymbol->name);
				moduleInfo->routineInfo[moduleInfo->numRoutines].codeSegmentSize =
					curSymbol->defn.info.routine.codeSegmentSize;
				moduleInfo->numRoutines++;
			}
		}
		buildRoutineList(curSymbol->right, moduleInfo);
	}
}

//---------------------------------------------------------------------------

void ABLModule::getInfo(ModuleInfo* moduleInfo)
{
	int32_t i;
	strcpy(moduleInfo->name, name);
	strcpy(moduleInfo->fileName, ModuleRegistry[handle].fileName);
	moduleInfo->numRoutines = 0;
	buildRoutineList(
		ModuleRegistry[handle].moduleIdPtr->defn.info.routine.localSymTable, moduleInfo);
	for (i = 0; i < moduleInfo->numRoutines; i++)
		moduleInfo->totalCodeSegmentSize += moduleInfo->routineInfo[i].codeSegmentSize;
	moduleInfo->numStaticVars = ModuleRegistry[handle].numStaticVars;
	moduleInfo->totalSizeStaticVars = ModuleRegistry[handle].totalSizeStaticVars;
	int32_t largest = 0;
	for (i = 0; i < moduleInfo->numStaticVars; i++)
	{
		if (ModuleRegistry[handle].sizeStaticVars[i] > ModuleRegistry[handle].sizeStaticVars[largest])
			largest = i;
	}
	moduleInfo->largestStaticVar.size = 0;
	if (ModuleRegistry[handle].sizeStaticVars)
		moduleInfo->largestStaticVar.size = ModuleRegistry[handle].sizeStaticVars[largest];
	moduleInfo->largestStaticVar.name[0] = nullptr;
}

//---------------------------------------------------------------------------

void ABLModule::destroy(void)
{
	int32_t i;
	if ((id > -1) && ModuleInstanceRegistry)
	{
		//-----------------------------------------------
		// It's on the active registry, so pull it off...
		for (i = 0; i < NumModuleInstances; i++)
			if (ModuleInstanceRegistry[i] == this)
			{
				ModuleInstanceRegistry[i] = ModuleInstanceRegistry[NumModuleInstances - 1];
				ModuleInstanceRegistry[NumModuleInstances - 1] = nullptr;
				NumModuleInstances--;
				NumModules--;
				break;
			}
	}
	if (watchManager)
	{
		delete watchManager;
		watchManager = nullptr;
	}
	if (breakPointManager)
	{
		delete breakPointManager;
		breakPointManager = nullptr;
	}
	if (staticData)
	{
		ABLStackFreeCallback(staticData);
		staticData = nullptr;
	}
}

//***************************************************************************
// MISC routines
//***************************************************************************

void ABLi_saveEnvironment(ABLFile* ablFile)
{
	int32_t i;
	ablFile->writeLong(numLibrariesLoaded);
	ablFile->writeLong(NumModulesRegistered);
	ablFile->writeLong(NumModules);
	for (i = 0; i < NumModulesRegistered; i++)
	{
		ablFile->writeString(ModuleRegistry[i].fileName);
		ablFile->writeByte(nullptr);
	}
	ablFile->writeLong(999);
	for (i = 0; i < eternalOffset; i++)
	{
		const std::unique_ptr<StackItem>& dataPtr = (const std::unique_ptr<StackItem>&)stack + i;
		if (EternalVariablesSizes[i] > 0)
			ablFile->write((uint8_t*)dataPtr->address, EternalVariablesSizes[i]);
		else
			ablFile->write((uint8_t*)dataPtr, sizeof(StackItem));
	}
	for (i = 0; i < NumModules; i++)
	{
		if (ModuleInstanceRegistry[i])
			ModuleInstanceRegistry[i]->write(ablFile);
	}
}

//---------------------------------------------------------------------------

void ABLi_loadEnvironment(ABLFile* ablFile, bool malloc)
{
	int32_t numLibs = ablFile->readLong();
	int32_t numModsRegistered = ablFile->readLong();
	int32_t numMods = ablFile->readLong();
	int32_t i;
	for (i = 0; i < numLibs; i++)
	{
		uint8_t fileName[1024];
		int32_t result = ablFile->readString(fileName);
		if (!result)
		{
			wchar_t err[255];
			sprintf(err, "ABLi_loadEnvironment: Unable to read filename [Module %d]", i);
			ABL_Fatal(0, err);
		}
		if (malloc)
		{
			int32_t numErrors, numLinesProcessed;
			const std::unique_ptr<ABLModule>& library = ABLi_loadLibrary(
				(std::wstring_view)fileName, &numErrors, &numLinesProcessed, nullptr, false, false);
			if (!library)
			{
				wchar_t err[255];
				sprintf(err, "ABLi_loadEnvironment: Unable to load library [Module %d]", i);
				ABL_Fatal(0, err);
			}
		}
	}
	for (i = 0; i < (numModsRegistered - numLibs); i++)
	{
		uint8_t fileName[1024];
		int32_t result = ablFile->readString(fileName);
		if (!result)
		{
			wchar_t err[255];
			sprintf(err, "ABLi_loadEnvironment: Unable to read filename [Module %d]", i);
			ABL_Fatal(0, err);
		}
		int32_t numErrors, numLinesProcessed;
		if (malloc)
		{
			int32_t handle = ABLi_preProcess((std::wstring_view)fileName, &numErrors, &numLinesProcessed);
			if (handle < 0)
			{
				wchar_t err[255];
				sprintf(err, "ABLi_loadEnvironment: Unable to preprocess [Module %d]", i);
				ABL_Fatal(0, err);
			}
		}
	}
	// int32_t mark = ablFile->readLong();
	for (i = 0; i < eternalOffset; i++)
	{
		const std::unique_ptr<StackItem>& dataPtr = (const std::unique_ptr<StackItem>&)stack + i;
		if (EternalVariablesSizes[i] > 0)
			ablFile->read((uint8_t*)dataPtr->address, EternalVariablesSizes[i]);
		else
			ablFile->read((uint8_t*)dataPtr, sizeof(StackItem));
	}
	for (i = 0; i < numLibs; i++)
	{
		const std::unique_ptr<ABLModule>& library = LibraryInstanceRegistry[i];
		library->read(ablFile);
	}
	for (i = 0; i < (numMods - numLibs); i++)
	{
		const std::unique_ptr<ABLModule>& module = nullptr;
		if (malloc)
			module = new ABLModule;
		else
			module = ModuleInstanceRegistry[numLibs + i];
		module->read(ablFile);
	}
}

//***************************************************************************

} // namespace mclib::abl
