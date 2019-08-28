//===========================================================================//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//
//***************************************************************************
//
// ABLDBUG.H
//
//***************************************************************************

#pragma once

#ifndef ABLDBUG_H
#define ABLDBUG_H

//#include "dabldbug.h"
//#include "ablenv.h"

namespace mclib::abl {

//***************************************************************************

enum class DebugCommandCode : uint32_t
{
	DEBUG_COMMAND_SET_MODULE,
	DEBUG_COMMAND_TRACE,
	DEBUG_COMMAND_STEP,
	DEBUG_COMMAND_BREAKPOINT_SET,
	DEBUG_COMMAND_BREAKPOINT_REMOVE,
	DEBUG_COMMAND_WATCH_SET,
	DEBUG_COMMAND_WATCH_REMOVE_ALL,
	DEBUG_COMMAND_PRINT,
	DEBUG_COMMAND_CONTINUE,
	DEBUG_COMMAND_HELP,
	DEBUG_COMMAND_INFO,
	NUM_DEBUG_COMMANDS
} DebugCommandCode;

//***************************************************************************

typedef struct _Watch
{
	const std::unique_ptr<SymTableNode>& idPtr;
	bool store;
	bool breakOnStore;
	bool fetch;
	bool breakOnFetch;
} Watch;

typedef Watch* WatchPtr;

class WatchManager
{

protected:
	int32_t maxWatches;
	int32_t numWatches;
	WatchPtr watches;

public:
	PVOID operator new(size_t mySize);

	void operator delete(PVOID us);

	void init(void)
	{
		maxWatches = 0;
		maxWatches = 0;
		watches = nullptr;
	}

	int32_t init(int32_t max);

	void destroy(void);

	WatchManager(void) { init(void); }

	~WatchManager(void) { destroy(void); }

	WatchPtr add(const std::unique_ptr<SymTableNode>& idPtr);

	int32_t remove(const std::unique_ptr<SymTableNode>& idPtr);

	int32_t removeAll(void);

	int32_t setStore(const std::unique_ptr<SymTableNode>& idPtr, bool state, bool breakToDebug = false);

	int32_t setFetch(const std::unique_ptr<SymTableNode>& idPtr, bool state, bool breakToDebug = false);

	bool getStore(const std::unique_ptr<SymTableNode>& idPtr);

	bool getFetch(const std::unique_ptr<SymTableNode>& idPtr);

	void print(void);
};

//---------------------------------------------------------------------------

class BreakPointManager
{

protected:
	int32_t maxBreakPoints;
	int32_t numBreakPoints;
	int32_t* breakPoints;

public:
	PVOID operator new(size_t mySize);

	void operator delete(PVOID us);

	void init(void)
	{
		maxBreakPoints = 0;
		numBreakPoints = 0;
		breakPoints = nullptr;
	}

	int32_t init(int32_t max);

	void destroy(void);

	BreakPointManager(void) { init(void); }

	~BreakPointManager(void) { destroy(void); }

	int32_t add(int32_t lineNumber);

	int32_t remove(int32_t lineNumber);

	int32_t removeAll(void);

	bool isBreakPoint(int32_t lineNumber);

	void print(void);
};

//---------------------------------------------------------------------------

#define WATCH_STORE_OFF 1
#define WATCH_STORE_ON 2
#define WATCH_FETCH_OFF 4
#define WATCH_FETCH_ON 8
#define WATCH_BREAK 16

class Debugger
{

protected:
	const std::unique_ptr<ABLModule>& module; // Current executing module
	const std::unique_ptr<WatchManager>& watchManager; // Current executing watch manager
	const std::unique_ptr<BreakPointManager>& breakPointManager; // Current executing breakpt manager

	const std::unique_ptr<ABLModule>& debugModule; // Current module being debugged

	bool enabled;
	bool debugCommand;
	bool halt;
	bool trace;
	bool step;
	bool traceEntry;
	bool traceExit;

	static wchar_t message[512];

	void (*printCallback)(const std::wstring_view& s);

public:
	PVOID operator new(size_t mySize);

	void operator delete(PVOID us);

	void init(void)
	{
		module = nullptr;
		watchManager = nullptr;
		breakPointManager = nullptr;
		debugModule = module;
		enabled = false;
		debugCommand = false;
		halt = false;
		trace = false;
		step = false;
		traceEntry = false;
		traceExit = false;
		printCallback = nullptr;
	}

	int32_t init(void (*callback)(const std::wstring_view& s), const std::unique_ptr<ABLModule>& _module);

	void destroy(void);

	Debugger(void) { init(void); }

	~Debugger(void) { destroy(void); }

	void enable(void) { enabled = true; }

	void disable(void) { enabled = false; }

	bool isEnabled(void) { return (enabled); }

	int32_t print(const std::wstring_view& s);

	void setModule(const std::unique_ptr<ABLModule>& _module);

	int32_t setWatch(int32_t states);

	int32_t addBreakPoint(void);

	int32_t removeBreakPoint(void);

	void sprintStatement(const std::wstring_view& dest);

	void sprintLineNumber(const std::wstring_view& dest);

	void sprintDataValue(const std::wstring_view& dest, const std::unique_ptr<StackItem>& data, const std::unique_ptr<Type>& dataType);

	int32_t sprintSimpleValue(const std::wstring_view& dest, const std::unique_ptr<SymTableNode>& symbol);

	int32_t sprintArrayValue(const std::wstring_view& dest, const std::unique_ptr<SymTableNode>& symbol, const std::wstring_view& subscriptString);

	int32_t sprintValue(const std::wstring_view& dest, const std::wstring_view& exprString);

	int32_t traceStatementExecution(void);

	int32_t traceRoutineEntry(const std::unique_ptr<SymTableNode>& idPtr);

	int32_t traceRoutineExit(const std::unique_ptr<SymTableNode>& idPtr);

	int32_t traceDataStore(
		const std::unique_ptr<SymTableNode>& id, const std::unique_ptr<Type>& idType, const std::unique_ptr<StackItem>& target, const std::unique_ptr<Type>& targetType);

	int32_t traceDataFetch(const std::unique_ptr<SymTableNode>& id, const std::unique_ptr<Type>& idType, const std::unique_ptr<StackItem>& data);

	void showValue(void);

	void assignVariable(void);

	void displayModuleInstanceRegistry(void);

	void processCommand(
		int32_t commandId, const std::wstring_view& strParam1, int32_t numParam1, const std::unique_ptr<ABLModule>& moduleParam1);

	void debugMode(void);

	const std::unique_ptr<ABLModule>& getDebugModule(void) { return (debugModule); }
};

//***************************************************************************

} // namespace mclib::abl

#endif
