//========================================================================
// PERF.h
//===========================================================================//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#pragma once

//========================================================================
// types
//========================================================================
typedef HRESULT(__stdcall* PERFMONCALLBACK)(PSTR description, uint32_t flags, uint32_t id);

//========================================================================
// external functions
//========================================================================
extern HRESULT __stdcall InitPerformanceMonitorLibrary(void);
extern HRESULT __stdcall TermPerformanceMonitorLibrary(void);

extern HRESULT __stdcall EnumeratePerformanceMonitors(PERFMONCALLBACK pfcallback);
extern HRESULT __stdcall SetPerformanceMonitor(uint32_t id);
extern double __stdcall DecodePerformanceMonitors(int64_t* pregisters);
extern BOOL __stdcall ReadPerformanceMonitorRegisters(int64_t* presult);

//========================================================================
// errors
//========================================================================
#define PERF_ERROR_ALREADYINITIALIZED -1
#define PERF_ERROR_OWNED_BY_ANOTHER_APP -2
#define PERF_ERROR_CANTCREATEMUTEX -3
#define PERF_ERROR_NOTINITIALIZED -4
#define PERF_ERROR_INVALID_CPU -5
#define PERF_ERROR_CANT_LOAD_VXD -6
#define PERF_ERROR_SET_FAILED -7
