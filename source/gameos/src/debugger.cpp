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
 debugger.cpp - gameos reference pseudo code

 MechCommander 2 source code

 2014-07-24 jerker_back, created

 $LastChangedBy$

================================================================================
 RcsID = $Id$ */

#include "stdinc.h"

// -----------------------------------------------------------------------------
// Global data exported from this module

// global referenced data not listed in headers

// local data

// -----------------------------------------------------------------------------
// global implemented functions in this module listed in headers

// global implemented functions not listed in headers

// local functions

// -----------------------------------------------------------------------------
// externals referenced in this file not specified in headers

uint32_t __stdcall CallDebuggerMenuItem(PCSTR Name, uint32_t MenuFunction);
void __stdcall DelDebuggerMenuItem(PSTR Name);
void __stdcall AddDebuggerMenuItem(PSTR Name,
	uint8_t(__stdcall* CheckMark)(void), void(__stdcall* Activated)(void),
	uint8_t(__stdcall* Greyed)(void)								= 0,
	uint32_t(__stdcall* Callback)(PSTR Name, uint32_t MenuFunction) = 0);
void __stdcall EndRenderMode(void);
void __stdcall RenderMenu(
	int32_t, int32_t, uint32_t, uint32_t, MenuItem*, int32_t);
void __stdcall DrawMenu(void);
void __stdcall OpenMenu(MenuItem*, int32_t, int32_t);
void __stdcall OpenSubMenu(MenuItem*, int32_t, int32_t);
void __stdcall InitDebugger(void);
void __stdcall CleanMenu(MenuItem*);
void __stdcall CleanUpDebugger(void);
void __stdcall DestroyDebugger(void);
void __stdcall SpewNextLine(void);
void __stdcall SpewToDebugger(PSTR Message);
float __stdcall DoUV(float);
int32_t __stdcall GetMipmapUsed(uint32_t Handle, pgos_VERTEX Pickv1,
	pgos_VERTEX Pickv2, pgos_VERTEX Pickv3);
void __stdcall ShowTriangleSizeInfo(void);
void __stdcall DoExamineMipmaps(void);
void __stdcall DoExamineMipmaps2(void);
void __stdcall DoExamineRenderer(void);
void __stdcall DrawSquare(
	int32_t TopX, int32_t TopY, int32_t Width, int32_t Height, uint32_t Color);
void __stdcall DrawLines(
	int32_t X1, int32_t Y1, int32_t X2, int32_t Y2, uint32_t Color);
void __stdcall DoExamineDepth(void);
void __stdcall DoExamineArea(void);
void __stdcall DoExamineDrawing(void);
uint32_t __stdcall GetColor(uint32_t, uint32_t);
PSTR __stdcall ShowColor(uint32_t);
void __stdcall DoExamineColor(void);
void __stdcall DisplayUV(float, float);
void __stdcall DoExamineScene(void);
void __stdcall DoDebugger(void);
void __stdcall UpdateDisplayInfo(void);
void __stdcall UpdateDebugger(void);
uint32_t __stdcall WalkUp(void);
void __stdcall WalkStack(uint32_t*, uint32_t, uint32_t);
PSTR __stdcall DecodeAddress(uint32_t, uint8_t);
void __stdcall DoExamineHeaps(uint32_t);
void __stdcall DoSpewInterface(char);
void __stdcall CheckSpewButton(uint32_t, uint32_t);

CTexInfo::Area(void);
CTexInfo::Width(void);
CTexInfo::Height(void);
CTexInfo::Hints(void);
CTexInfo::Detected(void);
CTexInfo::MipmapDisabled(void);
CTexInfo::MipFilter(void);
CTexInfo::Name(void);
CTexInfo::pInstance(void);
CTexInfo::Description(void);
CTexInfo::pRebuildFunc(void);
CTexInfo::Format(void);
CTexInfo::InVidMem(void);

// imports
struct _MenuItem* pLibrariesMenu;
struct _MenuItem* pRasterizerMenu;
struct _MenuItem* pDebuggerMenu;
struct _MenuItem* pGameMenu;
unsigned long PickNumber;
unsigned long EnableDisplayInfo;
unsigned long gStopRendering;

struct _TextureHeap* pTextureHeap;

int32_t __stdcall DrawTextA(
	HDC hdc, PCSTR lpchText, int32_t cchText, PRECT lprc, uint32_t format);

InternalFunctionStop(char const*, ...);
gos_Free(void*);
gos_Malloc(uint, gos_Heap*);
ErrorHandler(x, x);
wRelease(IUnknown*);
DrawChr(char);
InitTextDisplay(void);
gos_SetRenderState(gos_RenderState, int);
InternalFunctionSpew(char const*, char const*, ...);
fabs;
GetMipmapColor(int);
gos_SetupViewport(
	bool, float, bool, uint32_t, float, float, float, float, bool, uint32_t);
gos_GetKey(void);
End3DScene(void);
CopyBackBuffer(IDirectDrawSurface7*, IDirectDrawSurface7*);
gos_ClipDrawQuad(gos_VERTEX*);
gos_DrawLines(gos_VERTEX*, int);
GetGosPixelFormat(_DDPIXELFORMAT*);
gos_GetKeyStatus(gosEnum_KeyIndex);
gos_DrawQuads(gos_VERTEX*, int);
gos_DrawTriangles(gos_VERTEX*, int);
UpdateDebugWindow(void);
SetPerformanceMonitor(uint32_t);
UpdateGraphs(void);
gos_GetHiResTime(void);
timeGetTime();
CTexInfo::FreeVidMem(void);
RestoreStatistics(void);
UpdateDebugMouse(void);
UpdateCursor(void);
ShowFrameGraphs(void);
ShowLogFileControls(void);
gos_GetMouseInfo(float*, float*, int*, int*, int*, uint32_t*);
gos_SetViewport(uint32_t, uint32_t, uint32_t, uint32_t, float, float);
SaveStatistics(void);
IJLERR (*_ijlFree)(_JPEG_CORE_PROPERTIES*);
IJLERR (*_ijlWrite)(_JPEG_CORE_PROPERTIES*, IJLIOTYPE);
IJLERR (*_ijlInit)(_JPEG_CORE_PROPERTIES*);
InternalDoesFileExist(char const*);
GrabScreenImage(void);
CheckLogFile(void);
GetLocationFromAddress(_IMAGEHLP_LINE*, char*, int);
GetSymbolFromAddress(char*, int);
gosGetUserName(void);
gos_GetElapsedTime(int);
