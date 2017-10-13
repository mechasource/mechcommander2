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
 controlmanager.cpp - gameos reference pseudo code

 MechCommander 2 source code

 2014-07-24 jerker_back, created

 $LastChangedBy$

================================================================================
 RcsID = $Id$ */

#include "stdafx.h"

#include <gameos.hpp>
#include <controlmanager.hpp>

// -----------------------------------------------------------------------------
// Global data exported from this module

MECH_IMPEXP float XDelta;
MECH_IMPEXP float YDelta;
MECH_IMPEXP uint32_t gNumberOfJoystickCounts;
MECH_IMPEXP bool DisablePolling;

// -----------------------------------------------------------------------------
// implemented functions in this module listed in headers

MECH_IMPEXP void __stdcall CMInstall(void);
MECH_IMPEXP void __stdcall CMUninstall(void);
MECH_IMPEXP void __stdcall CMReadJoystick(uint32_t index);
MECH_IMPEXP void __stdcall CMReleaseControls(void);
MECH_IMPEXP void __stdcall CMAcquireControls(void);
MECH_IMPEXP void __stdcall CMUnacquireControls(void);
MECH_IMPEXP void __stdcall CMUpdate(void);
MECH_IMPEXP void __stdcall CMCreateJoysticks(void);
MECH_IMPEXP BOOL __stdcall CMCreateJoystick7(LPCDIDEVICEINSTANCEA pdinst, PVOID pvRef);
MECH_IMPEXP void __stdcall ReInitControlManager(void);

MECH_IMPEXP uint32_t __stdcall gosJoystick_CountJoysticks(uint8_t ReDetect = 1);
MECH_IMPEXP float __stdcall gosJoystick_GetAxis(uint32_t index, GOSJoystickAxis axis);
MECH_IMPEXP uint8_t __stdcall gosJoystick_ButtonStatus(uint32_t index, uint32_t button);
MECH_IMPEXP void __stdcall gosJoystick_SetPolling(uint32_t index, uint8_t yesNo, float howOften = 0.03333f);
MECH_IMPEXP void __stdcall gosJoystick_GetInfo(uint32_t index, gosJoystick_Info* gji);

// implemented functions not listed in headers
void __stdcall CMRestoreEffects(int32_t);

// -----------------------------------------------------------------------------
// externals not specified in headers

