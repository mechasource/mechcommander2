//===========================================================================//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//
//***************************************************************************
//
// ABL.H
//
//***************************************************************************

#pragma once

#ifndef ABL_H
#define ABL_H

//#include "ablgen.h"
#include "ablexec.h"
//#include "ablsymt.h"
//#include "ablenv.h"
//#include "abldbug.h"

namespace mclib::abl
{

//***************************************************************************

#define DEBUGGING

void ABLi_init(size_t runtimeStackSize, // = 20480,
	size_t maxCodeBufferSize, // = 10240,
	size_t maxRegisteredModules, // = 200,
	size_t maxStaticVariables, // = 100,
	PVOID (*systemMallocCallback)(size_t memSize), PVOID (*stackMallocCallback)(size_t memSize),
	PVOID (*codeMallocCallback)(size_t memSize), PVOID (*symbolMallocCallback)(size_t memSize),
	void (*systemFreeCallback)(PVOID memBlock), void (*stackFreeCallback)(PVOID memBlock),
	void (*codeFreeCallback)(PVOID memBlock), void (*symbolFreeCallback)(PVOID memBlock),
	int32_t (*fileCreateCB)(PVOID* file, std::wstring_view filename),
	int32_t (*fileOpenCB)(PVOID* file, std::wstring_view filename), int32_t (*fileCloseCB)(PVOID* file),
	bool (*fileEofCB)(PVOID file),
	int32_t (*fileReadCB)(PVOID file, uint8_t* buffer, int32_t length),
	int32_t (*fileReadLongCB)(PVOID file), int32_t (*fileReadStringCB)(PVOID file, uint8_t* buffer),
	int32_t (*fileReadLineExCB)(PVOID file, uint8_t* buffer, int32_t maxLength),
	int32_t (*fileWriteCB)(PVOID file, uint8_t* buffer, int32_t length),
	int32_t (*fileWriteByteCB)(PVOID file, uint8_t byte),
	int32_t (*fileWriteLongCB)(PVOID file, int32_t value),
	int32_t (*fileWriteStringCB)(PVOID file, std::wstring_view buffer), void (*debuggerPrintCallback)(std::wstring_view s),
	void (*ablFatalCallback)(int32_t code, std::wstring_view s), bool debugInfo = false, bool debug = false,
	bool profile = false);

const std::unique_ptr<ABLParam>& ABLi_createParamList(int32_t numParameters);
void ABLi_setIntegerParam(const std::unique_ptr<ABLParam>& paramList, int32_t index, int32_t value);
void ABLi_setRealParam(const std::unique_ptr<ABLParam>& paramList, int32_t index, float value);
void ABLi_deleteParamList(const std::unique_ptr<ABLParam>& paramList);
int32_t ABLi_preProcess(std::wstring_view sourceFileName, int32_t* numErrors = nullptr,
	int32_t* numLinesProcessed = nullptr, int32_t* numFilesProcessed = nullptr,
	bool printLines = false);

const std::unique_ptr<ABLModule>&
ABLi_loadLibrary(std::wstring_view sourceFileName, int32_t* numErrors = nullptr,
	int32_t* numLinesProcessed = nullptr, int32_t* numFilesProcessed = nullptr,
	bool printLines = false, bool createInstance = true);

int32_t
ABLi_execute(const std::unique_ptr<SymTableNode>& moduleIdPtr, const std::unique_ptr<SymTableNode>& functionIdPtr = nullptr,
	const std::unique_ptr<ABLParam>& paramList = nullptr, const std::unique_ptr<StackItem>& returnVal = nullptr);

int32_t
ABLi_deleteModule(const std::unique_ptr<SymTableNode>& moduleIdPtr);

const std::unique_ptr<ABLModule>&
ABLi_getModule(int32_t id);
const std::unique_ptr<Debugger>& ABLi_getDebugger(void);
void ABLi_saveEnvironment(ABLFile* ablFile);
void ABLi_loadEnvironment(ABLFile* ablFile, bool malloc);
void ABLi_close(void);
bool ABLi_enabled(void);

void ABLi_addFunction(std::wstring_view name, bool isOrder, std::wstring_view paramList, std::wstring_view returnType, void (*codeCallback)(void));

void ABLi_setRandomCallbacks(void (*seedRandomCallback)(size_t seed), int32_t (*randomCallback)(int32_t range));
void ABLi_setDebugPrintCallback(void (*ABLDebugPrintCallback)(std::wstring_view s));
void ABLi_setGetTimeCallback(size_t (*ABLGetTimeCallback)(void));
void ABLi_setEndlessStateCallback(void (*endlessStateCallback)(UserFile* log));

wchar_t ABLi_popChar(void);
int32_t ABLi_popInteger(void);
float ABLi_popReal(void);
bool ABLi_popBoolean(void);
float ABLi_popIntegerReal(void);
int32_t ABLi_popAnything(ABLStackItem* value);

std::wstring_view ABLi_popCharPtr(void);
int32_t* ABLi_popIntegerPtr(void);
float* ABLi_popRealPtr(void);
std::wstring_view ABLi_popBooleanPtr(void);

void ABLi_pushInteger(int32_t value);
void ABLi_pushReal(float value);
void ABLi_pushBoolean(bool value);

int32_t ABLi_peekInteger(void);
float ABLi_peekReal(void);
bool ABLi_peekBoolean(void);

std::wstring_view ABLi_peekCharPtr(void);
int32_t* ABLi_peekIntegerPtr(void);
float* ABLi_peekRealPtr(void);

void ABLi_pokeChar(int32_t val);
void ABLi_pokeInteger(int32_t val);
void ABLi_pokeReal(float val);
void ABLi_pokeBoolean(bool val);

int32_t ABLi_registerInteger(std::wstring_view name, int32_t* address, int32_t numElements = 0);
int32_t ABLi_registerReal(std::wstring_view name, float* address, int32_t numElements = 0);

bool ABLi_getSkipOrder(void);
void ABLi_resetOrders(void);
int32_t ABLi_getCurrentState(void);
void ABLi_transState(int32_t newState);

//***************************************************************************

} // namespace mclib::abl

#endif
