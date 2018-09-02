/*******************************************************************************
 Copyright (c) 2011-2014, Jerker Back. All rights reserved.

 Permission to use, copy, modify, and distribute this software for any
 purpose with or without fee is hereby granted, provided that the following
 conditions are met (OSI approved BSD 2-clause license):

 1. Redistributions of source code must retain the above copyright notice,
	this list of conditions and the following disclaimer.
 2. Redistributions in binary form must reproduce the above copyright notice,
	this list of conditions and the following disclaimer in the documentation
	and/or other materials provided with the distribution.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*******************************************************************************/
/*******************************************************************************
 threads.cpp - gameos reference pseudo code

 MechCommander 2 source code

 2014-07-24 jerker_back, created

 $LastChangedBy$

================================================================================
 RcsID = $Id$ */

#include "stdinc.h"

#include <gameos.hpp>
#include <globals.hpp>
#include <toolos.hpp>

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
void __stdcall gos_SetThreadPriority(
	uint32_t ThreadHandle, gosThreadPriority Priority);
void __stdcall gos_DeleteThread(uint32_t ThreadHandle);
void __stdcall gos_TriggerThread(
	uint32_t ThreadHandle, bool* ThreadFinished, PVOID Context);

// global implemented functions not listed in headers

// local functions
uint32_t __stdcall WorkerThread(PVOID);

// -----------------------------------------------------------------------------
// externals referenced in this file not specified in headers
extern void __stdcall gosSetThreadName(uint32_t, PSTR);
