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
 controlmanager.cpp - GameOS reference pseudo code

 MechCommander 2 source code

 2014-07-24 Jerker Back, created

*******************************************************************************/

#include "stdinc.h"

#include "gameos.hpp"
#include "controlmanager.hpp"

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
MECH_IMPEXP void __stdcall gosJoystick_SetPolling(
	uint32_t index, uint8_t yesNo, float howOften = 0.03333f);
MECH_IMPEXP void __stdcall gosJoystick_GetInfo(uint32_t index, gosJoystick_Info* gji);

// implemented functions not listed in headers
void __stdcall CMRestoreEffects(int32_t);

// -----------------------------------------------------------------------------
// externals not specified in headers
