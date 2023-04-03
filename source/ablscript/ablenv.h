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

namespace mclib::abl
{

//***************************************************************************

struct SourceFile
{
	wchar_t fileName[MAXLEN_FILENAME];
	uint8_t fileNumber;
	ABLFile* pfile;
	int32_t lineNumber;
};

//---------------------------------------------------------------------------

#define MAX_USER_FILES 6
#define MAX_USER_FILE_LINES 50
#define MAX_USER_FILE_LINELEN 200

class UserFile
{

public:
	UserFile(void)
	{
		init(void);
	}
	~UserFile(void)
	{
		destroy(void);
	}

	//PVOID operator new(size_t mySize);
	//void operator delete(PVOID us);

	void init(void)
	{
		handle = -1;
		inUse = false;
		fileName[0] = nullptr;
		pfile = nullptr;
		numLines = 0;
		totalLines = 0;
		for (size_t i = 0; i < MAX_USER_FILE_LINES; i++)
			lines[i][0] = nullptr;
	}

	void destroy(void);
	void dump(void);
	void close(void);
	int32_t open(std::wstring_view fileName);
	void write(std::wstring_view s);
	static void setup(void);
	static void cleanup(void);
	static const std::unique_ptr<UserFile>& getNewFile(void);

protected:
	int32_t handle;
	bool inUse;
	wchar_t fileName[MAXLEN_FILENAME];
	ABLFile* pfile;
	int32_t numLines;
	int32_t totalLines;
	wchar_t lines[MAX_USER_FILE_LINES][MAX_USER_FILE_LINELEN];

	static std::array<UserFile, MAX_USER_FILES> files;
};

//---------------------------------------------------------------------------

#define MAX_ABLMODULE_NAME 5
#define MAX_SOURCE_FILES 256 // per module
#define MAX_LIBRARIES_USED 25 // per module
#define MAX_STATE_HANDLES_PER_MODULE 10

struct VariableInfo
{
	wchar_t name[128];
	int32_t size;
};

struct RoutineInfo
{
	wchar_t name[128];
	int32_t codeSegmentSize;
};

struct ModuleInfo
{
	wchar_t name[128];
	wchar_t fileName[128];
	int32_t numStaticVars;
	int32_t totalSizeStaticVars;
	VariableInfo largestStaticVar;
	int32_t totalCodeSegmentSize;
	int32_t numRoutines;
	RoutineInfo routineInfo[128];
};

struct ModuleEntry
{
	std::wstring_view fileName;
	const std::unique_ptr<SymTableNode>& moduleIdPtr;
	int32_t numSourceFiles;
	std::wstring_view* sourceFiles;
	int32_t numLibrariesUsed;
	const std::unique_ptr<ABLModule>&* librariesUsed;
	int32_t numStaticVars;
	int32_t numOrderCalls;
	int32_t numStateHandles;
	const std::unique_ptr<StateHandleInfo>& stateHandles;
	int32_t* sizeStaticVars;
	int32_t totalSizeStaticVars;
	int32_t numInstances;
};

// typedef ModuleEntry* ModuleEntryPtr;

class ABLModule
{

public:
	ABLModule(void)
	{
		init(void);
	}
	~ABLModule(void)
	{
		destroy(void);
	}

	//PVOID operator new(size_t mySize);
	//void operator delete(PVOID us);

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

	int32_t init(int32_t moduleHandle);
	void write(ABLFile* moduleFile);
	void read(ABLFile* moduleFile);
	int32_t getId(void)
	{
		return (id);
	}
	int32_t getRealId(void);
	int32_t getHandle(void)
	{
		return (handle);
	}
	const std::unique_ptr<StackItem>& getStaticData(void)
	{
		return (staticData);
	}
	void setInitCalled(bool called)
	{
		initCalled = called;
	}
	bool getInitCalled(void)
	{
		return (initCalled);
	}
	std::wstring_view getFileName(void);
	std::wstring_view getName(void)
	{
		return (name);
	}
	void setName(std::wstring_view _name);
	size_t* getOrderCallFlags(void)
	{
		return (orderCallFlags);
	}
	void setPrevState(const std::unique_ptr<SymTableNode>& stateSym)
	{
		prevState = stateSym;
	}
	const std::unique_ptr<SymTableNode>& getPrevState(void)
	{
		return (prevState);
	}
	int32_t getPrevStateHandle(void);
	void setState(const std::unique_ptr<SymTableNode>& stateSym)
	{
		state = stateSym;
	}
	const std::unique_ptr<SymTableNode>& getState(void)
	{
		return (state);
	}
	int32_t getStateHandle(void);
	bool isLibrary(void);
	void resetOrderCallFlags(void);
	void setOrderCallFlag(uint8_t dword, uint8_t bit);
	void clearOrderCallFlag(uint8_t orderDWord, uint8_t orderBitMask);
	bool getOrderCallFlag(uint8_t dword, uint8_t bit)
	{
		return ((orderCallFlags[dword] & (1 << bit)) != 0);
	}

	const std::unique_ptr<WatchManager>& getWatchManager(void)
	{
		return (watchManager);
	}
	const std::unique_ptr<BreakPointManager>& getBreakPointManager(void)
	{
		return (breakPointManager);
	}
	void setTrace(bool _trace)
	{
		trace = _trace;
		traceEntry = _trace;
		traceExit = _trace;
	}

	bool getTrace(void)
	{
		return (trace);
	}
	void setStep(bool _step)
	{
		step = _step;
	}
	bool getStep(void)
	{
		return (step);
	}
	int32_t execute(const std::unique_ptr<ABLParam>& paramList = nullptr);
	int32_t execute(const std::unique_ptr<ABLParam>& moduleParamList, const std::unique_ptr<SymTableNode>& functionIdPtr);
	const std::unique_ptr<SymTableNode>& findSymbol(std::wstring_view symbolName, const std::unique_ptr<SymTableNode>& curFunction = nullptr, bool searchLibraries = false);
	const std::unique_ptr<SymTableNode>& findFunction(std::wstring_view functionName, bool searchLibraries = false);
	const std::unique_ptr<SymTableNode>& findState(std::wstring_view stateName);
	int32_t findStateHandle(std::wstring_view stateName);
	std::wstring_view getSourceFile(int32_t fileNumber);
	std::wstring_view getSourceDirectory(int32_t fileNumber, std::wstring_view directory);
	void getInfo(ModuleInfo* moduleInfo);
	float getReal(void)
	{
		return (returnVal.real);
	}
	int32_t getInteger(void)
	{
		return (returnVal.integer);
	}
	int32_t setStaticInteger(std::wstring_view name, int32_t value);
	int32_t getStaticInteger(std::wstring_view name);
	int32_t setStaticReal(std::wstring_view name, float value);
	float getStaticReal(std::wstring_view name);
	int32_t setStaticIntegerArray(std::wstring_view name, int32_t size, int32_t* values);
	int32_t getStaticIntegerArray(std::wstring_view name, int32_t size, int32_t* values);
	int32_t setStaticRealArray(std::wstring_view name, int32_t size, float* values);
	int32_t getStaticRealArray(std::wstring_view name, int32_t size, float* values);
	void destroy(void);

private:
	int32_t id;
	wchar_t name[MAX_ABLMODULE_NAME];
	int32_t handle;
	const std::unique_ptr<StackItem>& staticData;
	size_t* orderCallFlags;
	StackItem returnVal;
	bool initCalled;
	const std::unique_ptr<SymTableNode>& prevState;
	const std::unique_ptr<SymTableNode>& state;
	const std::unique_ptr<WatchManager>& watchManager;
	const std::unique_ptr<BreakPointManager>& breakPointManager;
	bool trace;
	bool step;
	bool traceEntry;
	bool traceExit;

	// static int32_t numModules;
};

//*************************************************************************

void initModuleRegistry(int32_t maxModules);
void destroyModuleRegistry(void);
void initLibraryRegistry(int32_t maxLibraries);
void destroyLibraryRegistry(void);

//***************************************************************************

} // namespace mclib::abl

#endif
