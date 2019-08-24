//===========================================================================//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//
//***************************************************************************
//
// ENVIRON.H
//
//***************************************************************************

#pragma once

#ifndef ABLENV_H
#define ABLENV_H

//#include "dablenv.h"
//#include "ablgen.h"
//#include "ablsymt.h"
//#include "ablexec.h"
//#include "dabldbug.h"
//#include "ablscan.h"

//***************************************************************************

typedef struct _SourceFile
{
	char fileName[MAXLEN_FILENAME];
	uint8_t fileNumber;
	ABLFile* filePtr;
	int32_t lineNumber;
} SourceFile;

//---------------------------------------------------------------------------

#define MAX_USER_FILES 6
#define MAX_USER_FILE_LINES 50
#define MAX_USER_FILE_LINELEN 200

class UserFile
{

public:
	int32_t handle;
	bool inUse;
	char fileName[MAXLEN_FILENAME];
	ABLFile* filePtr;
	int32_t numLines;
	int32_t totalLines;
	char lines[MAX_USER_FILE_LINES][MAX_USER_FILE_LINELEN];

	static UserFilePtr files[MAX_USER_FILES];

public:
	PVOID operator new(size_t mySize);

	void operator delete(PVOID us);

	void init(void)
	{
		handle = -1;
		inUse = false;
		fileName[0] = nullptr;
		filePtr = nullptr;
		numLines = 0;
		totalLines = 0;
		for (size_t i = 0; i < MAX_USER_FILE_LINES; i++)
			lines[i][0] = nullptr;
	}

	UserFile(void) { init(void); }

	void destroy(void);

	~UserFile(void) { destroy(void); }

	void dump(void);

	void close(void);

	int32_t open(PSTR fileName);

	void write(PSTR s);

	static void setup(void);

	static void cleanup(void);

	static UserFilePtr getNewFile(void);
};

//---------------------------------------------------------------------------

#define MAX_ABLMODULE_NAME 5
#define MAX_SOURCE_FILES 256 // per module
#define MAX_LIBRARIES_USED 25 // per module
#define MAX_STATE_HANDLES_PER_MODULE 10

typedef struct
{
	char name[128];
	int32_t size;
} VariableInfo;

typedef struct
{
	char name[128];
	int32_t codeSegmentSize;
} RoutineInfo;

typedef struct
{
	char name[128];
	char fileName[128];
	int32_t numStaticVars;
	int32_t totalSizeStaticVars;
	VariableInfo largestStaticVar;
	int32_t totalCodeSegmentSize;
	int32_t numRoutines;
	RoutineInfo routineInfo[128];
} ModuleInfo;

typedef struct
{
	PSTR fileName;
	SymTableNodePtr moduleIdPtr;
	int32_t numSourceFiles;
	PSTR* sourceFiles;
	int32_t numLibrariesUsed;
	ABLModulePtr* librariesUsed;
	int32_t numStaticVars;
	int32_t numOrderCalls;
	int32_t numStateHandles;
	StateHandleInfoPtr stateHandles;
	int32_t* sizeStaticVars;
	int32_t totalSizeStaticVars;
	int32_t numInstances;
} ModuleEntry;

typedef ModuleEntry* ModuleEntryPtr;

class ABLModule
{

private:
	int32_t id;
	char name[MAX_ABLMODULE_NAME];
	int32_t handle;
	StackItemPtr staticData;
	size_t* orderCallFlags;
	StackItem returnVal;
	bool initCalled;
	SymTableNodePtr prevState;
	SymTableNodePtr state;
	WatchManagerPtr watchManager;
	BreakPointManagerPtr breakPointManager;
	bool trace;
	bool step;
	bool traceEntry;
	bool traceExit;

	// static int32_t numModules;

public:
	PVOID operator new(size_t mySize);
	void operator delete(PVOID us);

	void init(void)
	{
		id = -1;
		name[0] = nullptr;
		handle = -1;
		staticData = nullptr;
		returnVal.integer = 0;
		initCalled = false;
		prevState = nullptr;
		state = nullptr;
		watchManager = nullptr;
		breakPointManager = nullptr;
		trace = false;
		step = false;
		traceEntry = false;
		traceExit = false;
	}

	ABLModule(void) { init(void); }

	int32_t init(int32_t moduleHandle);

	void write(ABLFile* moduleFile);

	void read(ABLFile* moduleFile);

	int32_t getId(void) { return (id); }

	int32_t getRealId(void);

	int32_t getHandle(void) { return (handle); }

	StackItemPtr getStaticData(void) { return (staticData); }

	void setInitCalled(bool called) { initCalled = called; }

	bool getInitCalled(void) { return (initCalled); }

	PSTR getFileName(void);

	PSTR getName(void) { return (name); }

	void setName(PSTR _name);

	size_t* getOrderCallFlags(void) { return (orderCallFlags); }

	void setPrevState(SymTableNodePtr stateSym) { prevState = stateSym; }

	SymTableNodePtr getPrevState(void) { return (prevState); }

	int32_t getPrevStateHandle(void);

	void setState(SymTableNodePtr stateSym) { state = stateSym; }

	SymTableNodePtr getState(void) { return (state); }

	int32_t getStateHandle(void);

	bool isLibrary(void);

	void resetOrderCallFlags(void);

	void setOrderCallFlag(uint8_t dword, uint8_t bit);

	void clearOrderCallFlag(uint8_t orderDWord, uint8_t orderBitMask);

	bool getOrderCallFlag(uint8_t dword, uint8_t bit)
	{
		return ((orderCallFlags[dword] & (1 << bit)) != 0);
	}

	WatchManagerPtr getWatchManager(void) { return (watchManager); }

	BreakPointManagerPtr getBreakPointManager(void) { return (breakPointManager); }

	void setTrace(bool _trace)
	{
		trace = _trace;
		traceEntry = _trace;
		traceExit = _trace;
	}

	bool getTrace(void) { return (trace); }

	void setStep(bool _step) { step = _step; }

	bool getStep(void) { return (step); }

	int32_t execute(ABLParamPtr paramList = nullptr);
	int32_t execute(ABLParamPtr moduleParamList, SymTableNodePtr functionIdPtr);

	SymTableNodePtr findSymbol(
		PSTR symbolName, SymTableNodePtr curFunction = nullptr, bool searchLibraries = false);

	SymTableNodePtr findFunction(PSTR functionName, bool searchLibraries = false);

	SymTableNodePtr findState(PSTR stateName);

	int32_t findStateHandle(PSTR stateName);

	PSTR getSourceFile(int32_t fileNumber);

	PSTR getSourceDirectory(int32_t fileNumber, PSTR directory);

	void getInfo(ModuleInfo* moduleInfo);

	float getReal(void) { return (returnVal.real); }

	int32_t getInteger(void) { return (returnVal.integer); }

	int32_t setStaticInteger(PSTR name, int32_t value);

	int32_t getStaticInteger(PSTR name);

	int32_t setStaticReal(PSTR name, float value);

	float getStaticReal(PSTR name);

	int32_t setStaticIntegerArray(PSTR name, int32_t size, int32_t* values);

	int32_t getStaticIntegerArray(PSTR name, int32_t size, int32_t* values);

	int32_t setStaticRealArray(PSTR name, int32_t size, float* values);

	int32_t getStaticRealArray(PSTR name, int32_t size, float* values);

	void destroy(void);

	~ABLModule(void) { destroy(void); }
};

//*************************************************************************

void
initModuleRegistry(int32_t maxModules);
void
destroyModuleRegistry(void);
void
initLibraryRegistry(int32_t maxLibraries);
void
destroyLibraryRegistry(void);

//***************************************************************************

#endif
