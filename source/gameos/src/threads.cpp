/*******************************************************************************
 This file consist of reversed and interpreted code from object source in the
 x86 debug build gameos.lib. The code in this file can not be used in any way
 other than serve as an information reference to the original released source of
 Mechcommander2. The code is a work of progress and there is no guarantee it is
 complete, accurate or useful in any way. The purpose is instead to make it
 possible to safely remove any dependencies of gameos.lib from Mechcommander2.
 All code is logically copyrighted to Microsoft
*******************************************************************************/
/*******************************************************************************
 threads.cpp - GameOS reference pseudo code

 MechCommander 2 source code

 2014-07-24 Jerker Back, created

*******************************************************************************/

#include "stdinc.h"

#include "gameos.hpp"
#include "globals.hpp"
#include "toolos.hpp"

// -----------------------------------------------------------------------------
// Global data exported from this module

// global referenced data not listed in headers

// local data
struct _gosThreadInfo* ListofThreads;

// -----------------------------------------------------------------------------
// global implemented functions in this module listed in headers
void __stdcall CheckThreads(void);
void __stdcall DestroyThreads(void);
uint32_t __stdcall gos_CreateThread(void(__stdcall* ThreadRoutine)(PVOID));
void __stdcall gos_SetThreadPriority(uint32_t ThreadHandle, gosThreadPriority Priority);
void __stdcall gos_DeleteThread(uint32_t ThreadHandle);
void __stdcall gos_TriggerThread(uint32_t ThreadHandle, BOOLEAN* ThreadFinished, PVOID Context);

// global implemented functions not listed in headers

// local functions
uint32_t __stdcall WorkerThread(PVOID);

// -----------------------------------------------------------------------------
// externals referenced in this file not specified in headers
extern void __stdcall gosSetThreadName(uint32_t, PSTR);
