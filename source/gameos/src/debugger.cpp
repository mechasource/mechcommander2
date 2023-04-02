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
 debugger.cpp - GameOS reference pseudo code

 MechCommander 2 source code

 2014-07-24 Jerker Back, created

*******************************************************************************/

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

uint32_t __stdcall CallDebuggerMenuItem(PSTR Name, uint32_t MenuFunction);
void __stdcall DelDebuggerMenuItem(PSTR Name);
void __stdcall AddDebuggerMenuItem(PSTR Name, BOOLEAN(__stdcall* CheckMark)(void),
	void(__stdcall* Activated)(void), BOOLEAN(__stdcall* Greyed)(void) = 0,
	uint32_t(__stdcall* Callback)(PSTR Name, uint32_t MenuFunction) = 0);
void __stdcall EndRenderMode(void);
void __stdcall RenderMenu(int32_t, int32_t, uint32_t, uint32_t, MenuItem*, int32_t);
void __stdcall DrawMenu(void);
void __stdcall OpenMenu(MenuItem*, int32_t, int32_t);
void __stdcall OpenSubMenu(MenuItem*, int32_t, int32_t);
void __stdcall InitDebugger(void);
void __stdcall CleanMenu(MenuItem*);
void __stdcall CleanUpDebugger(void);
void __stdcall DestroyDebugger(void);
void __stdcall SpewNextLine(void);
void __stdcall SpewToDebugger(PSTR message);
float __stdcall DoUV(float);
int32_t __stdcall GetMipmapUsed(
	uint32_t handle, pgos_VERTEX Pickv1, pgos_VERTEX Pickv2, pgos_VERTEX Pickv3);
void __stdcall ShowTriangleSizeInfo(void);
void __stdcall DoExamineMipmaps(void);
void __stdcall DoExamineMipmaps2(void);
void __stdcall DoExamineRenderer(void);
void __stdcall DrawSquare(
	int32_t topx, int32_t topy, int32_t width, int32_t height, uint32_t colour);
void __stdcall DrawLines(int32_t X1, int32_t Y1, int32_t X2, int32_t Y2, uint32_t colour);
void __stdcall DoExamineDepth(void);
void __stdcall DoExamineArea(void);
void __stdcall DoExamineDrawing(void);
uint32_t __stdcall Getcolour(uint32_t, uint32_t);
PSTR __stdcall Showcolour(uint32_t);
void __stdcall DoExaminecolour(void);
void __stdcall DisplayUV(float, float);
void __stdcall DoExamineScene(void);
void __stdcall DoDebugger(void);
void __stdcall UpdateDisplayInfo(void);
void __stdcall UpdateDebugger(void);
uint32_t __stdcall WalkUp(void);
void __stdcall WalkStack(uint32_t*, uint32_t, uint32_t);
PSTR __stdcall DecodeAddress(uint32_t, BOOLEAN);
void __stdcall DoExamineHeaps(uint32_t);
void __stdcall DoSpewInterface(char);
void __stdcall CheckSpewButton(uint32_t, uint32_t);

CTexInfo::Area(void);
CTexInfo::width(void);
CTexInfo::height(void);
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
uint32_t PickNumber;
uint32_t EnableDisplayInfo;
uint32_t gStopRendering;

struct _TextureHeap* pTextureHeap;

int32_t __stdcall DrawTextA(HDC hdc, PSTR lpchText, int32_t cchText, PRECT lprc, uint32_t format);

InternalFunctionStop(char const*, ...);
gos_Free(void*);
gos_Malloc(uint32_t, gos_Heap*);
ErrorHandler(x, x);
wRelease(IUnknown*);
DrawChr(char);
InitTextDisplay(void);
gos_SetRenderState(gos_RenderState, int);
InternalFunctionSpew(char const*, char const*, ...);
fabs;
GetMipmapcolour(int);
gos_SetupViewport(BOOLEAN, float, BOOLEAN, uint32_t, float, float, float, float, BOOLEAN, uint32_t);
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
IJLERR (*_ijlFree)
(_JPEG_CORE_PROPERTIES*);
IJLERR (*_ijlWrite)
(_JPEG_CORE_PROPERTIES*, IJLIOTYPE);
IJLERR (*_ijlInit)
(_JPEG_CORE_PROPERTIES*);
InternalDoesFileExist(char const*);
GrabScreenImage(void);
CheckLogFile(void);
GetLocationFromAddress(_IMAGEHLP_LINE*, char*, int);
GetSymbolFromAddress(char*, int);
gosGetUserName(void);
gos_GetElapsedTime(int);
