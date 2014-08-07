//========================================================================
// PERF.h
//===========================================================================//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#pragma once

//========================================================================
// types
//========================================================================
typedef HRESULT (__stdcall * PERFMONCALLBACK)(char *szDescription, DWORD dwFlags, DWORD dwId);

//========================================================================
// external functions
//========================================================================
extern HRESULT __stdcall InitPerformanceMonitorLibrary(void);
extern HRESULT __stdcall TermPerformanceMonitorLibrary(void);

extern HRESULT __stdcall EnumeratePerformanceMonitors(PERFMONCALLBACK lpfCallback);
extern HRESULT __stdcall SetPerformanceMonitor(DWORD dwId);
extern double __stdcall DecodePerformanceMonitors( __int64 *Registers );
extern BOOL __stdcall ReadPerformanceMonitorRegisters(__int64 *result);


//========================================================================
// errors
//========================================================================
#define PERF_ERROR_ALREADYINITIALIZED			-1
#define PERF_ERROR_OWNED_BY_ANOTHER_APP			-2
#define PERF_ERROR_CANTCREATEMUTEX				-3
#define PERF_ERROR_NOTINITIALIZED				-4
#define PERF_ERROR_INVALID_CPU					-5
#define PERF_ERROR_CANT_LOAD_VXD				-6
#define PERF_ERROR_SET_FAILED					-7

