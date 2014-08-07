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
 controlmanager.cpp  - gameos

 MechCommander 2 source code

 2014-07-24 jerker_back, created
 
 $LastChangedBy$
 
================================================================================
 RcsID = $Id$ */

#include "stdafx.h"


CMInstall(void)                                                                             .text 000002BC 0000009A R . . . B T .
ReInitControlManager(void)                                                                  .text 0000035C 00000041 R . . . B T .
CMUninstall(void)                                                                           .text 000003AC 0000007A R . . . B T .
CMReleaseControls(void)                                                                     .text 0000042C 00000071 R . . . B T .
CMAcquireControls(void)                                                                     .text 000004AC 00000113 R . . . B T .
CMUnacquireControls(void)                                                                   .text 000005CC 0000006E R . . . B T .
gosJoystick_CountJoysticks(bool)                                                            .text 0000063C 00000179 R . . . B T .
gosJoystick_GetAxis(ulong,GOSJoystickAxis)                                                  .text 000007BC 0000011C R . . . B . .
gosJoystick_ButtonStatus(ulong,ulong)                                                       .text 000008DC 000000D3 R . . . B T .
gosJoystick_SetPolling(ulong,bool,float)                                                    .text 000009BC 0000005B R . . . B T .
CMUpdate(void)                                                                              .text 00000A1C 00000110 R . . . B T .
CMCreateJoysticks(void)                                                                     .text 00000B2C 0000003F R . . . B T .
CMCreateJoystick7(DIDEVICEINSTANCEA const *,void *)                                         .text 00000B6C 000005B7 R . . . B T .
CMReadJoystick(ulong)                                                                       .text 0000112C 0000026C R . . . B T .
gosJoystick_GetInfo(ulong,gosJoystick_Info *)                                               .text 0000139C 00000229 R . . . B T .
CMRestoreEffects(int)                                                                       .text 000015CC 000000C4 R . . . B T .
gosForceEffect::`scalar deleting destructor'(uint)                                          .text 00001690 0000002C R . . . B . .

CMCreateMouse(void)                                                                         UNDEF 000016E0 00000004 R . . . . T .
CMCreateKeyboard(void)                                                                      UNDEF 000016E4 00000004 R . . . . T .
wDirectInputCreateEx(HINSTANCE__ *,ulong,_GUID const &,void * *,IUnknown *)                 UNDEF 000016E8 00000004 R . . . . T .
_memset                                                                                     UNDEF 000016F4 00000004 R . . . . T .
gos_MathExceptions(bool,bool)                                                               UNDEF 000016FC 00000004 R . . . . T .
InternalFunctionSpew(char const *,char const *,...)                                         UNDEF 00001700 00000004 R . . . . T .
wRelease(IUnknown *)                                                                        UNDEF 00001718 00000004 R . . . . T .
operator delete(void *)                                                                     UNDEF 0000171C 00000004 R . . . . T .
gosForceEffect::~gosForceEffect(void)                                                       UNDEF 00001720 00000004 R . . . . . .
SetExtended(uchar,int)                                                                      UNDEF 00001724 00000004 R . . . . T .
wAcquire(IDirectInputDevice7A *)                                                            UNDEF 00001734 00000004 R . . . . T .
wUnacquire(IDirectInputDevice7A *)                                                          UNDEF 0000173C 00000004 R . . . . T .
WriteBitStream(ulong,ulong,int)                                                             UNDEF 00001740 00000004 R . . . . T .
ReadBitStream(ulong,int)                                                                    UNDEF 00001744 00000004 R . . . . T .
ErrorHandler(x,x)                                                                           UNDEF 00001750 00000004 R . . . . . .
gos_GetElapsedTime(int)                                                                     UNDEF 00001758 00000004 R . . . . T .
CMUpdateMouse(void)                                                                         UNDEF 0000175C 00000004 R . . . . T .
AddKeyEvent(ulong)                                                                          UNDEF 00001764 00000004 R . . . . T .
wEnumDevices(IDirectInput7A *,ulong,int (*)(DIDEVICEINSTANCEA const *,void *),void *,ulong) UNDEF 00001774 00000004 R . . . . T .
wSetProperty(IDirectInputDevice7A *,_GUID const &,DIPROPHEADER const *)                     UNDEF 0000177C 00000004 R . . . . T .
wGetCapabilities(IDirectInputDevice7A *,DIDEVCAPS *)                                        UNDEF 00001784 00000004 R . . . . T .
wQueryInterface(IUnknown *,_GUID const &,void * *)                                          UNDEF 00001788 00000004 R . . . . T .
__imp__strncpy                                                                              UNDEF 0000178C 00000004 R . . . . T .
wGetDeviceInfo(IDirectInputDevice7A *,DIDEVICEINSTANCEA *)                                  UNDEF 00001790 00000004 R . . . . T .
InternalFunctionStop(char const *,...)                                                      UNDEF 00001794 00000004 R . . . . T .
wSetDataFormat(IDirectInputDevice7A *,_DIDATAFORMAT const *)                                UNDEF 00001798 00000004 R . . . . T .
wSetCooperativeLevel(IDirectInputDevice7A *,HWND__ *,ulong)                                 UNDEF 000017A0 00000004 R . . . . T .
GetParent(x)                                                                                UNDEF 000017A4 00000004 R . . . . T .
wCreateDeviceEx(IDirectInput7A *,_GUID const &,_GUID const &,void * *,IUnknown *)           UNDEF 000017AC 00000004 R . . . . T .
__security_check_cookie(x)                                                                  UNDEF 000017B8 00000004 R . . . . . .
_memcpy                                                                                     UNDEF 000017BC 00000004 R . . . . T .
wGetDeviceState(IDirectInputDevice7A *,ulong,void *)                                        UNDEF 000017C4 00000004 R . . . . T .
wPoll(IDirectInputDevice7A *)                                                               UNDEF 000017CC 00000004 R . . . . T .
gosForceEffect::ReCreate(void)                                                              UNDEF 000017D4 00000004 R . . . . . .


typedef struct DIDEVCAPS {
	ULONG   dwSize;
	ULONG   dwFlags;
	ULONG   dwDevType;
	ULONG   dwAxes;
	ULONG   dwButtons;
	ULONG   dwPOVs;
	ULONG   dwFFSamplePeriod;
	ULONG   dwFFMinTimeResolution;
	ULONG   dwFirmwareRevision;
	ULONG   dwHardwareRevision;
	ULONG   dwFFDriverVersion;
} DIDEVCAPS;
typedef DIDEVCAPS *LPDIDEVCAPS;

typedef struct _DIOBJECTDATAFORMAT {
	const GUID *pguid;
	ULONG   dwOfs;
	ULONG   dwType;
	ULONG   dwFlags;
} DIOBJECTDATAFORMAT;
typedef  DIOBJECTDATAFORMAT *LPDIOBJECTDATAFORMAT;
typedef const DIOBJECTDATAFORMAT *LPCDIOBJECTDATAFORMAT;


typedef struct _DIDATAFORMAT {
	ULONG   dwSize;
	ULONG   dwObjSize;
	ULONG   dwFlags;
	ULONG   dwDataSize;
	ULONG   dwNumObjs;
	LPDIOBJECTDATAFORMAT rgodf;
} DIDATAFORMAT;
typedef DIDATAFORMAT *LPDIDATAFORMAT;
typedef const DIDATAFORMAT *LPCDIDATAFORMAT;
