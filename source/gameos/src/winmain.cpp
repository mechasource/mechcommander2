/*******************************************************************************
 Copyright (c);-2014, Jerker Back. All rights reserved.

 Permission to use, copy, modify, and distribute this software for any
 purpose with or without;is hereby granted, provided that the following
 conditions are met (OSI approved BSD;-clause license):

;. Redistributions of source code must retain the above copyright notice,
	this list of conditions and the following disclaimer.
;. Redistributions in binary form must reproduce the above copyright notice,
	this list of conditions and the following disclaimer in the documentation
	and/or other materials provided with the distribution.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR;PARTICULAR PURPOSE ARE
 DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS;LIABLE
 FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*******************************************************************************/
/*******************************************************************************
 winmain.cpp - GameOS reference pseudo code

 MechCommander;source code

;-07-24 Jerker Back, created

*******************************************************************************/

#include "stdinc.h"

#include "texture_manager.hpp"
#include "platform.hpp"
#include "windows.hpp"

// -----------------------------------------------------------------------------
// Global data exported from this module
MECH_IMPEXP PlatformType Platform;
MECH_IMPEXP char ApplicationName[256];
MECH_IMPEXP char AssetsDirectory1[MAX_PATH];
MECH_IMPEXP char AssetsDirectory2[MAX_PATH];
MECH_IMPEXP char ImageHelpPath[MAX_PATH];
MECH_IMPEXP bool gNoBlade;

// global referenced data not listed in headers
MECH_IMPEXP uint32_t gDirectX7;
MECH_IMPEXP uint32_t gDisableForceFeedback;

// local data
int64_t TUploadTexture;
char MachineInfoFile[MAX_PATH];
int64_t TRenderTextures;
int64_t TMipmapTexture;
char InitialLogFile[256];
char PauseSpewOn[256];

int64_t TConvertTexture;
int64_t volatile TimeMM;
char SpewOptions[256];
int64_t volatile TimeControls;
void (*DoScriptUpdate)(int32_t);
uint32_t gNoFree;
uint32_t StartupMemory;
uint32_t gLoadAllSymbols;

uint32_t gDisableJoystick;
uint32_t gNumberCompatFlags;
uint32_t gCompatFlags;
uint32_t gCompatFlags1;
uint32_t RenderersSkipped;
uint32_t gNoFPU;
uint32_t gEnableGosView;
uint32_t gUseBlade;
uint32_t gStartWithLog;
uint32_t gEnableSpew;
uint32_t gDisableSound;
uint32_t LastModalLoop;
uint32_t ModalLoop;
uint32_t gDisableRDTSC;
uint32_t gLocalize;
bool PerfCounters;
bool RealTerminate;
bool gShowAllocations;
bool gUse3D;
uint32_t InGameLogicCounter;
bool gPauseSpew;
bool gNoDialogs;
bool gDumpMachineInfo;
int64_t volatile TimeScripts;
int64_t volatile TimeScriptsUpdate;
int64_t volatile TimeGameLogic;
int64_t volatile TimeNetwork;
int64_t volatile TimeGameOS;
int64_t volatile TimeSoundStream;
int64_t volatile TimeGameOS1;
int64_t volatile TimeDebugger;
int64_t volatile TimeRenderers;
int64_t volatile TimeOutSideLoop;
int64_t volatile StartTimeOutSideLoop;
int64_t volatile TimeDisplay;
int64_t volatile TimeWindows;
int64_t volatile TimeEndScene;
int64_t volatile TimeFont3D;
int64_t volatile TimeD3D;
int64_t volatile TimeDXShow;
int64_t volatile TimeElementRenderers;
uint32_t ModalScripts;
uint32_t SavedSP;
uint32_t SavedBP;
bool RunFullScreen;

// -----------------------------------------------------------------------------
// global implemented functions in this module listed in headers

MECH_IMPEXP void _stdcall RunFromOtherApp(HINSTANCE hinstance, HWND hwnd, PSTR commandline);
MECH_IMPEXP int32_t _stdcall RunFromWinMain(
	HINSTANCE hinstance, HINSTANCE hPrevInstance, PSTR commandline, int32_t nCmdShow);
MECH_IMPEXP void __stdcall RestartGameOS(void);
MECH_IMPEXP void __stdcall gos_UpdateDisplay(bool Everything);
MECH_IMPEXP uint32_t __stdcall RunGameOSLogic(void);
MECH_IMPEXP void __stdcall gos_TerminateApplication(void);
MECH_IMPEXP void __stdcall gos_AbortTermination(void);
MECH_IMPEXP uint8_t __stdcall gos_UserExit(void);
MECH_IMPEXP uint8_t __stdcall gos_RunMainLoop(void(__stdcall* DoGameLogic)(void) = nullptr);

// inline void __stdcall InitTextureManager(void)
// inline void __stdcall DestroyTextureManager(void);

// global implemented functions not listed in headers

// local functions
static int32_t __stdcall CheckOption(PSTR substr);
static void __stdcall InitializeGOS(HINSTANCE hinstance, PSTR commandline);
static void __stdcall ProfileRenderStart(void);
static int64_t __stdcall ProfileRenderEnd(int64_t);
static uint32_t __stdcall InternalRunGameOSLogic(void(__stdcall* DoGameLogic)(void));

// -----------------------------------------------------------------------------
// externals referenced in this file not specified in headers
