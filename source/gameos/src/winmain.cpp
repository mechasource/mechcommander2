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
 winmain.cpp  - gameos

 MechCommander 2 source code

 2014-07-24 jerker_back, created
 
 $LastChangedBy$
 
================================================================================
 RcsID = $Id$ */

#include "stdafx.h"
#include <texture_manager.hpp>


extern void _stdcall RunFromOtherApp(HINSTANCE hInstance, HWND hWnd, PSTR pCmdLine);
extern int _stdcall RunFromWinMain( HINSTANCE hInst, HINSTANCE hPrevInst, PSTR pCmdLine, int nCmdShow );
CheckOption(PSTR )                                                                         .text    00001A30 00000189 R . . . B T .
InitializeGOS(HINSTANCE__ *,PSTR )                                                         .text    00001BC0 000008A0 R . . . B T .
RestartGameOS(void)                                                                         .text    00002460 0000037A R . . . B T .
ProfileRenderStart(void)                                                                    .text    000027E0 00000097 R . . . B T .
ProfileRenderEnd(int64_t)                                                                   .text    00002880 0000015A R . . . B T .
gos_UpdateDisplay(bool)                                                                     .text    000029E0 00000A00 R . . . B T .
InternalRunGameOSLogic(void (*)(void))                                                      .text    000033E0 00000840 R . . . B T .
RunGameOSLogic(void)                                                                        .text    00003C20 00000010 R . . . B T .
gos_TerminateApplication(void)                                                              .text    00003C30 0000000F R . . . B T .
gos_AbortTermination(void)                                                                  .text    00003C40 0000000F R . . . B T .
gos_UserExit(void)                                                                          .text    00003C50 00000013 R . . . B T .
gos_RunMainLoop(void (*)(void))                                                             .text    00003C70 000000F6 R . . . B T .
strstr(PSTR ,char const *)                                                                 .text    00003D68 00000016 R . . . B T .

TerminateGame
FixedLengthString::FixedLengthString(int)                                                   .text    00004114 00000059 R . . . B T .
FixedLengthString::operator PSTR (void)                                                    .text    0000417C 00000011 R . . . B . .
InitTextureManager(void)                                                                    .text    00004190 0000000A R . . . B T .
`InitializeGOS(HINSTANCE__ *,PSTR )'::`103'::`dynamic atexit destuctor for 'Buffer''(void) .text$yd 0000419C 0000000F R . . . B T .
FixedLengthString::~FixedLengthString(void)                                                 .text    000041AC 0000001B R . . . B . .
DestroyTextureManager(void)                                                                 .text    000041C8 0000000A R . . . B T .
GosLogRef::operator ulong(void)                                                             .text    00004228 00000011 R . . . B . .
GosLogFuncScope::GosLogFuncScope(ulong)                                                     .text    0000423C 00000031 R . . . B . .
GosEventLog::Log(ulong)                                                                     .text    00004270 00000060 R . . . B . .
GosLogFuncScope::~GosLogFuncScope(void)                                                     .text    000042D0 00000029 R . . . B . .

// -----------------------------------------------------------------------------

ExitGameOS(void)                                                                            UNDEF    00004344 00000004 R . . . . T .
_strlen                                                                                     UNDEF    0000434C 00000004 R . . . . T .
__imp__strstr                                                                               UNDEF    00004350 00000004 R . . . . T .
InternalFunctionStop(char const *,...)                                                      UNDEF    00004354 00000004 R . . . . T .
gos_PopCurrentHeap(void)                                                                    UNDEF    00004358 00000004 R . . . . T .
gos_PushCurrentHeap(gos_Heap *)                                                             UNDEF    0000435C 00000004 R . . . . T .
InternalFunctionSpew(char const *,char const *,...)                                         UNDEF    00004368 00000004 R . . . . T .
EnterFullScreenMode(void)                                                                   UNDEF    0000436C 00000004 R . . . . T .
CloseHandle(x)                                                                              UNDEF    00004370 00000004 R . . . . T .
WriteFile(x,x,x,x,x)                                                                        UNDEF    00004374 00000004 R . . . . T .
SetFilePointer(x,x,x,x)                                                                     UNDEF    00004378 00000004 R . . . . T .
CreateFileA(x,x,x,x,x,x,x)                                                                  UNDEF    0000437C 00000004 R . . . . T .
GetInstalledAudioVideoCodecs(FixedLengthString &)                                           UNDEF    00004380 00000004 R . . . . T .
GetDirectXDetails(FixedLengthString &)                                                      UNDEF    00004384 00000004 R . . . . T .
GetMachineDetails(FixedLengthString &)                                                      UNDEF    00004388 00000004 R . . . . T .
_atexit                                                                                     UNDEF    0000438C 00000004 R . . . . T .
StartTimeManager(void)                                                                      UNDEF    00004394 00000004 R . . . . T .
InitLogging(void)                                                                           UNDEF    00004398 00000004 R . . . . T .
InitNetworking(void)                                                                        UNDEF    0000439C 00000004 R . . . . T .
SoundRendererInstall(int)                                                                   UNDEF    000043A0 00000004 R . . . . T .
CMInstall(void)                                                                             UNDEF    000043A4 00000004 R . . . . T .
InitDebuggerMenus(void)                                                                     UNDEF    000043A8 00000004 R . . . . T .
DirectDrawInstall(void)                                                                     UNDEF    000043AC 00000004 R . . . . T .
VideoManagerInstall(void)                                                                   UNDEF    000043B0 00000004 R . . . . T .
InitializeWindows(void)                                                                     UNDEF    000043B4 00000004 R . . . . T .
InitRenderer(void)                                                                          UNDEF    000043B8 00000004 R . . . . T .
InitStatistics(void)                                                                        UNDEF    000043BC 00000004 R . . . . T .
InitPerformanceMonitorLibrary(void)                                                         UNDEF    000043C0 00000004 R . . . . T .
Init3DFont(void)                                                                            UNDEF    000043C8 00000004 R . . . . T .
StartGosViewServerThreads(void)                                                             UNDEF    000043CC 00000004 R . . . . T .
gos_MathExceptions(bool,bool)                                                               UNDEF    000043D0 00000004 R . . . . T .
Init_FileSystem(void)                                                                       UNDEF    000043D4 00000004 R . . . . T .
RegistryManagerInstall(void)                                                                UNDEF    000043D8 00000004 R . . . . T .
MM_Startup(void)                                                                            UNDEF    000043DC 00000004 R . . . . T .
SetupVirtualMemory(void)                                                                    UNDEF    000043E0 00000004 R . . . . T .
TimeManagerInstall(void)                                                                    UNDEF    000043E4 00000004 R . . . . T .
InitializeSpew(void)                                                                        UNDEF    000043E8 00000004 R . . . . T .
InitDebugger(void)                                                                          UNDEF    000043EC 00000004 R . . . . T .
InitLibraries(void)                                                                         UNDEF    000043F0 00000004 R . . . . T .
AlreadyRunning(void)                                                                        UNDEF    000043F8 00000004 R . . . . T .
InitLocalization(void)                                                                      UNDEF    000043FC 00000004 R . . . . T .
InitExceptionHandler(PSTR )                                                                UNDEF    00004400 00000004 R . . . . T .
MessageBoxA(x,x,x,x)                                                                        UNDEF    00004408 00000004 R . . . . T .
__imp__sscanf                                                                               UNDEF    0000440C 00000004 R . . . . T .
gosEnvironment Environment                                                                  UNDEF    00004410 00000004 R . . . . T .
_strcpy                                                                                     UNDEF    00004418 00000004 R . . . . T .
GetVMSize(void)                                                                             UNDEF    00004420 00000004 R . . . . T .
___CxxFrameHandler3                                                                         UNDEF    00004430 00000004 R . . . . . .
__security_check_cookie(x)                                                                  UNDEF    00004434 00000004 R . . . . . .
__chkstk                                                                                    UNDEF    00004438 00000004 R . . . . . .
__imp__malloc                                                                               UNDEF    0000443C 00000004 R . . . . T .
ErrorHandler(x,x)                                                                           UNDEF    00004440 00000004 R . . . . . .
CTexInfo::InitializeTextureManager(void)                                                    UNDEF    00004444 00000004 R . . . . . .
__imp__free                                                                                 UNDEF    00004448 00000004 R . . . . T .
UpdateGraphs(void)                                                                          UNDEF    0000444C 00000004 R . . . . T .
ReInitControlManager(void)                                                                  UNDEF    00004454 00000004 R . . . . T .
gos_RecreateTextureHeaps(void)                                                              UNDEF    00004458 00000004 R . . . . T .
ReInit3D(void)                                                                              UNDEF    0000445C 00000004 R . . . . T .
GetEnvironmentSettings(PSTR )                                                              UNDEF    00004470 00000004 R . . . . T .
CleanUpDebugger(void)                                                                       UNDEF    00004478 00000004 R . . . . T .
DestroyStatistics(void)                                                                     UNDEF    0000447C 00000004 R . . . . T .
_memset                                                                                     UNDEF    00004484 00000004 R . . . . T .
DestroyLocalization(void)                                                                   UNDEF    0000448C 00000004 R . . . . T .
VideoManagerUninstall(void)                                                                 UNDEF    00004490 00000004 R . . . . T .
SoundRendererUninstall(void)                                                                UNDEF    00004494 00000004 R . . . . T .
Destroy3DFont(void)                                                                         UNDEF    00004498 00000004 R . . . . T .
Destroy3D(void)                                                                             UNDEF    000044A0 00000004 R . . . . T .
EndLogging(void)                                                                            UNDEF    000044A4 00000004 R . . . . T .
CTexInfo::DestroyTextureManager(void)                                                       UNDEF    000044B0 00000004 R . . . . . .
GetCycles(void)                                                                             UNDEF    000044B8 00000004 R . . . . T .
DisplayBackBuffer(void)                                                                     UNDEF    000044D8 00000004 R . . . . T .
End3DScene(void)                                                                            UNDEF    000044DC 00000004 R . . . . T .
UpdateDebugger(void)                                                                        UNDEF    000044E0 00000004 R . . . . T .
SoundRendererEndFrame(void)                                                                 UNDEF    000044E4 00000004 R . . . . T .
RenderIME(void)                                                                             UNDEF    000044E8 00000004 R . . . . T .
gos_SetupViewport(bool,float,bool,ulong,float,float,float,float,bool,ulong)                 UNDEF    000044EC 00000004 R . . . . T .
MusicManagerUpdate(void)                                                                    UNDEF    000044F4 00000004 R . . . . T .
VideoManagerUpdate(void)                                                                    UNDEF    000044F8 00000004 R . . . . T .
CheckWindow(void)                                                                           UNDEF    00004504 00000004 R . . . . T .
GosLogRef::GosLogRef(GosLogRef::EventType,PSTR ,PSTR ,int)                                UNDEF    0000451C 00000004 R . . . . . .
ShowFTOL(void)                                                                              UNDEF    00004534 00000004 R . . . . T .
LocalizationPurge(void)                                                                     UNDEF    00004538 00000004 R . . . . T .
LocalizationSetPurgeFlags(void)                                                             UNDEF    0000453C 00000004 R . . . . T .
CheckLogInSync(void)                                                                        UNDEF    00004540 00000004 R . . . . T .
UpDateLogFile(void)                                                                         UNDEF    00004554 00000004 R . . . . T .
UpdateNetwork(void)                                                                         UNDEF    00004558 00000004 R . . . . T .
CMUpdate(void)                                                                              UNDEF    0000455C 00000004 R . . . . T .
Update(void)                                                                                UNDEF    00004564 00000004 R . . . . T .
SaveOldKeyState(void)                                                                       UNDEF    00004568 00000004 R . . . . T .
TimeManagerUpdate(void)                                                                     UNDEF    0000456C 00000004 R . . . . T .
ResetStatistics(void)                                                                       UNDEF    00004574 00000004 R . . . . T .
CheckThreads(void)                                                                          UNDEF    00004578 00000004 R . . . . T .
gos_Free(PVOID)                                                                            UNDEF    00004584 00000004 R . . . . T .
AnalyzeWS(PSTR )                                                                           UNDEF    00004588 00000004 R . . . . T .
int (*_GetWsChanges)(PVOID,_PSAPI_WS_WATCH_INFORMATION *,ulong)                            UNDEF    0000458C 00000004 R . . . . . .
int (*_InitializeProcessForWsWatch)(PVOID)                                                 UNDEF    00004590 00000004 R . . . . . .
int (*_EmptyWorkingSet)(PVOID)                                                             UNDEF    00004594 00000004 R . . . . . .
GetCurrentProcess()                                                                         UNDEF    0000459C 00000004 R . . . . T .
gos_Malloc(uint,gos_Heap *)                                                                 UNDEF    000045A4 00000004 R . . . . T .
GosEventLog::LogStartPoint(void)                                                            UNDEF    000045B0 00000004 R . . . . . .
