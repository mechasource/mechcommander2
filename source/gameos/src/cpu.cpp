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
 cpu.cpp - gameos

 MechCommander 2 source code

 2014-07-24 jerker_back, created

 $LastChangedBy$

================================================================================
 RcsID = $Id$ */

#include "stdafx.h"

int sprintf(PSTR buffer, PCSTR format, ...);

sprintf
free
wGetCaps(IDirectDraw7*, _DDCAPS_DX7*, _DDCAPS_DX7*)                                UNDEF 000021F8 00000004                   R . . . . T .
wRelease(IUnknown*)                                                                UNDEF 000021FC 00000004                   R . . . . T .
wUnlock(IDirectDrawSurface7*, tagRECT*)                                            UNDEF 00002200 00000004                   R . . . . T .
BOOL __stdcall IsBadReadPtr(PCVOID lp, uintptr_t ucb);
UNDEF 00002204 00000004 00000000 00000000 R . . . . T .
wLock(IDirectDrawSurface7*, tagRECT*, _DDSURFACEDESC2*, ulong, void*)               UNDEF 00002208 00000004                   R . . . . T .
wCreateSurface(IDirectDraw7*, _DDSURFACEDESC2*, IDirectDrawSurface7**, IUnknown*) UNDEF 0000220C 00000004                   R . . . . T .
_memset                                                                             UNDEF 00002210 00000004                   R . . . . T .
wSetCooperativeLevel(IDirectDraw7*, HWND__*, ulong)                                 UNDEF 00002214 00000004                   R . . . . T .
wDirectDrawCreateEx(_GUID*, void**, _GUID const&, IUnknown*)                      UNDEF 0000221C 00000004                   R . . . . T .
BOOL __stdcall QueryPerformanceCounter(PLARGE_INTEGER pPerformanceCount);
BOOL __stdcall QueryPerformanceFrequency(PLARGE_INTEGER pFrequency);

UNDEF 0000222C 00000004 00000000 00000000 R . . . . T .
UNDEF 00002230 00000004 00000000 00000000 R . . . . T .
SetThreadPriority(x, x)                                                              UNDEF 00002234 00000004 00000000 00000000 R . . . . T .
SetPriorityClass(x, x)                                                               UNDEF 00002238 00000004 00000000 00000000 R . . . . T .
Sleep(x)                                                                            UNDEF 0000223C 00000004 00000000 00000000 R . . . . T .
GetPriorityClass(x)                                                                 UNDEF 00002240 00000004 00000000 00000000 R . . . . T .
GetThreadPriority(x)                                                                UNDEF 00002244 00000004 00000000 00000000 R . . . . T .
GetCurrentProcess()                                                                 UNDEF 00002248 00000004                   R . . . . T .
GetCurrentThread()                                                                  UNDEF 0000224C 00000004                   R . . . . T .
__imp__malloc                                                                       UNDEF 00002250 00000004                   R . . . . T .
__alldiv                                                                            UNDEF 00002258 00000004                   R . . . . . .
GetSystemInfo(x)                                                                    UNDEF 0000225C 00000004 00000000 00000000 R . . . . T .
_strcat                                                                             UNDEF 0000226C 00000004                   R . . . . T .
_strcpy                                                                             UNDEF 00002270 00000004                   R . . . . T .
__imp__strncmp                                                                      UNDEF 00002274 00000004                   R . . . . T .
__security_check_cookie(x)                                                          UNDEF 0000228C 00000004                   R . . . . . .