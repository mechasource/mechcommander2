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
 dxrasterizer.cpp - GameOS reference pseudo code

 MechCommander 2 source code

 2014-07-24 Jerker Back, created

*******************************************************************************/

#include "stdinc.h"

#include <ddraw.h>

// -----------------------------------------------------------------------------
// Uninitialised global data exported from this module
struct _D3DDeviceDesc7 CapsDirect3D
gosViewportAddY
struct _D3DVIEWPORT7 viewData
unsigned char * GammaTable
DepthResults
// initialised global data exported from this module
uint32_t FirstTime = 1;
float UserGamma = 1.0:
COLORREF gosColorTable[19]
{
	0xFFFFFFFF, 0xFFFF00FF, 0xFF0000FF, 0xFF00FFFF, 0xFF00FF00, 
	0xFFFFFF00, 0xFFFF0000, 0xFF000000, 0xFF808080, 0xFF800080,
	0xFF000080, 0xFF008080, 0xFF008000, 0xFF808000, 0xFF800000,
	0xFFFF80FF, 0xFF8080FF, 0xFF80FFFF, 0xFF80FF80
};
// global referenced data not listed in headers

// local data


// -----------------------------------------------------------------------------
// global implemented functions in this module listed in headers

void __stdcall gos_SetBrightnessValue(uint32_t brightness);
void __stdcall gos_SetContrastValue(uint32_t contrast); 
void __stdcall gos_SetGammaValue(float gamma);
bool __stdcall CheckWindow(void);
void __stdcall gos_SetScreenMode(
	uint32_t width, uint32_t height, uint32_t bitdepth = 16, uint32_t device = 0,
	bool disablezbuffer = 0, bool antialias = 0, bool rendertovram = 0, bool gotofullscreen = 0, 
	int32_t dirtyrectangle = 0, bool gotowindowmode = 0, bool enablestencil = 0, uint32_t renderer = 0);
void __stdcall CopyBackBuffer(LPDIRECTDRAWSURFACE7 dest, LPDIRECTDRAWSURFACE7 source);
void __stdcall RenderWithReferenceRasterizer(uint32_t type);
void __stdcall DirectDrawInstall(void);
void __stdcall DirectDrawUninstall(void);
void __stdcall EnterWindowMode(void);
void __stdcall EnterFullScreenMode(void);
void __stdcall End3DScene(void);
void __stdcall DisplayBackBuffer(void);
void __stdcall DirectDrawCreateDDObject(void);
void __stdcall DirectDrawCreateAllBuffers(void);
float __cdecl pow(_In_ float x,_In_  float y);			// inline?
int32_t __stdcall powf(_In_ float x,_In_  float y);		// sigh
inline int32_t __stdcall float2long(float x);			// inline?
inline void __stdcall CheckPreloadTextures(void);
inline void __stdcall ReleaseTextureHeap(bool releasesysmem);

// global implemented functions not listed in headers
/*static*/ void __stdcall DisplayModeInfo(void);				// ref DirectDrawCreateAllBuffers
/*static*/ BOOL __stdcall WinEnum(HWND hwnd, LPARAM lparam);	// ref EnterFullScreenMode
/*static*/ void __stdcall DirectDrawRelease(void);				// ref DirectDrawUninstall
/*static*/ bool __stdcall SetupMode(bool, uint32_t);			// ref DirectDrawCreateAllBuffers
/*static*/ void __stdcall GetModeCaps(void);					// ref DirectDrawCreateAllBuffers


// local functions

// -----------------------------------------------------------------------------
// externals referenced in this file not specified in headers

// -----------------------------------------------------------------------------
// externals referenced in this file specified in headers
int32_t __stdcall ErrorHandler(int32_t flags, PSTR errortext);
bool __stdcall gos_RecreateTextureHeaps(void);

uint8_t UpdatedState;
uintptr_t __security_cookie;
int32_t _fltused;
void __fastcall __security_check_cookie(uintptr_t StackCookie);
double __cdecl pow(double X, double Y);
uint32_t DoUpdateWindow;
HRESULT __stdcall wIsLost(LPDIRECTDRAWSURFACE7 pdds7);
BOOL __stdcall IsIconic(HWND hwnd);
HWND hWindow;
void InternalFunctionSpew(PCSTR, PCSTR, ...);
bool gActive;
struct gosEnvironment Environment;
uint32_t Want32;
uint32_t WantHW;
uint32_t WantSW;
uint32_t WantSW1;
uint32_t WantRes;
uint32_t WantAA;
uint32_t InUpdateRenderers;
uint32_t wReleaseDC;
BOOL __stdcall BitBlt(HDC hdc, int32_t x, int32_t y, int32_t cx, int32_t cy, HDC hdcsrc, int32_t x1, int32_t y1, uint32_t rop);
BOOL __stdcall StretchBlt(HDC hdcdest, int32_t xdest, int32_t ydest, int32_t wdest, int32_t hdest, HDC hdcsrc, int32_t xsrc, int32_t ysrc, int32_t wsrc, int32_t hsrc, uint32_t rop);
int32_t __stdcall SetStretchBltMode(HDC hdc, int32_t mode);
uint32_t DesktopBpp;
int32_t __stdcall wGetDC(struct IDirectDrawSurface7 *, HDC *);
int32_t __stdcall wBlt(struct IDirectDrawSurface7 *, struct tagRECT *, struct IDirectDrawSurface7 *, struct tagRECT *, uint32_t, struct _DDBLTFX *);
uint32_t __stdcall wRelease(struct IUnknown *);
HRESULT __stdcall wUnlock(LPDIRECTDRAWSURFACE7 pdds7, PRECT prect);
uint32_t __stdcall GetPixelColor(uint32_t);
HRESULT __stdcall wLock(LPDIRECTDRAWSURFACE7 pdd7, PRECT pdestrect,LPDDSURFACEDESC2 pddsurfacedesc, uint32_t flags, HANDLE hevent);
int32_t __stdcall wLock(struct IDirectDrawSurface7 *, struct tagRECT *, struct _DDSURFACEDESC2 *, uint32_t, PVOID);
void __stdcall gos_UpdateDisplay(bool);
int32_t __stdcall wClear(struct IDirect3DDevice7 *, uint32_t, struct _D3DRECT *, uint32_t, uint32_t, float, uint32_t);
HRESULT __stdcall wClear(LPDIRECT3DDEVICE7 d3ddevice7, uint32_t count, LPD3DRECT pd3drect, uint32_t flags, uint32_t colour, D3DVALUE dvZ, uint32_t stencil);
DirectDrawColorControl
uint32_t AlphaInvAlpha;
uint32_t * RenderStates;
bool NeedToInitRenderStates;
int32_t __stdcall wSetViewport(struct IDirect3DDevice7 *, struct _D3DVIEWPORT7 *);
void __stdcall gos_MathExceptions(bool, bool);
int32_t __stdcall wCreateDevice(struct IDirect3D7 *, const GUID *, struct IDirectDrawSurface7 *, struct IDirect3DDevice7 **);
GUID IID_IDirect3DRefDevice;
int32_t __stdcall wAddAttachedSurface(struct IDirectDrawSurface7 *, struct IDirectDrawSurface7 *);
void __stdcall DrawTextA(uint32_t, PSTR );

int32_t CurrentY;
int32_t CurrentX;
void __stdcall InitTextDisplay(void);
void __stdcall DrawSquare(int32_t, int32_t, int32_t, int32_t, uint32_t);
void __stdcall __high gos_SetRenderState(enum gos_RenderState, int32_t);
void __stdcall gos_SetupViewport(bool, float, bool, uint32_t, float, float, float, float, bool, uint32_t);
int32_t __stdcall wCreateSurface(struct IDirectDraw7 *, struct _DDSURFACEDESC2 *, struct IDirectDrawSurface7 **, struct IUnknown *);
PVOID __cdecl memset(PVOID Dst, int32_t Val, size_t Size);
bool g_DDperformFlip;
uint32_t g_DDstate;
bool gDumpMachineInfo;
void __stdcall InitVertexBuffers(void);
uint32_t WantRVRAM;
uint32_t Want32T;
void __stdcall FreeVideoCards(void);
BOOL __stdcall DestroyWindow(HWND hwnd);
void __stdcall EndRenderMode(void);
bool gNoBlade;
int32_t __cdecl sprintf(PSTR Dest, PCSTR Format, ...);
struct DeviceInfo * DeviceArray;
void __stdcall VideoManagerRestore(void);
void __stdcall SoundRendererContinue(void);
uint32_t TerminateGame;
BOOL __stdcall SetWindowPos(HWND hwnd, HWND hWndInsertAfter, int32_t X, int32_t Y, int32_t cx, int32_t cy, UINT uFlags);
BOOL __stdcall AdjustWindowRect(LPRECT lpRect, uint32_t dwStyle, BOOL bMenu);
LONG __stdcall GetWindowLongA(HWND hwnd, int32_t nIndex);
void __stdcall SoundRendererPause(void);
void __stdcall VideoManagerRelease(void);
int32_t __cdecl _strnicmp(PCSTR Str1, PCSTR Str, size_t MaxCount);
size_t __cdecl strlen(PCSTR Str);
PSTR __stdcall gos_GetResourceString(uint32_t, uint32_t);
size_t gLanguageDLL;
int32_t __stdcall GetWindowTextA(HWND hwnd, LPSTR lpString, int32_t nMaxCount);
BOOL __stdcall ClipCursor(const RECT *lpRect);
BOOL __stdcall GetWindowRect(HWND hwnd, LPRECT lpRect);
HWND __stdcall SetFocus(HWND hwnd);
uint16_t volatile FPUControl;
int32_t InternalFunctionPause(PCSTR, ...);
BOOL __stdcall EnumWindows(WNDENUMPROC lpEnumFunc, LPARAM lParam);
int32_t __stdcall wSetDisplayMode(struct IDirectDraw7 *, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t);
HRESULT __stdcall wSetDisplayMode(LPDIRECTDRAW7 pdd7, uint32_t width, uint32_t height,uint32_t bpp, uint32_t refreshrate, uint32_t flags);

bool AllowFail;
int32_t __stdcall wSetCooperativeLevel(struct IDirectDraw7 *, HWND, uint32_t);
LONG __stdcall SetWindowLongA(HWND hwnd, int32_t nIndex, LONG dwNewLong);
uint16_t FPUDefault;
uint32_t NumHWDevices;
int32_t __stdcall wDirectDrawCreateEx(GUID *, PVOID*, const GUID *, struct IUnknown *);
const GUID IID_IDirectDraw7;
uint32_t __stdcall wAddRef(struct IUnknown *);
BOOL __stdcall GetWindowPlacement(HWND hwnd, WINDOWPLACEMENT *lpwndpl);
struct _SAVESTATE * pStateStack;
int32_t InternalFunctionStop(PCSTR, ...);
uint32_t VertexBuffersLocked;
uint32_t gCaptureScreen;
uint32_t gEnableParallel;
int32_t DBMouseX;
int32_t DBMouseY;
enum  RenderModeType RenderMode;
int32_t __stdcall wEndScene(struct IDirect3DDevice7 *);
bool InsideBeginScene;
void __stdcall MouseTimer(uint32_t timerid, uint32_t msg, uint32_t user, uint32_t dw1, uint32_t dw2);
int32_t __stdcall ReleaseDC(HWND hwnd, HDC hdc);
HDC __stdcall GetDC(HWND hwnd);
int32_t __stdcall wFlip(struct IDirectDrawSurface7 *, struct IDirectDrawSurface7 *, uint32_t);
struct tagPOINT clientToScreen;
uint32_t gForceNoFlip;
uint32_t GameNoVsync;
uint32_t FrameAdvance;
void __stdcall static CTexInfo::PreloadTextures(void);
BOOL __stdcall SetWindowPlacement(HWND hwnd, const WINDOWPLACEMENT* pwndpl);
int32_t __stdcall wRestoreDisplayMode(struct IDirectDraw7 *);
int32_t __stdcall wDeleteAttachedSurface(struct IDirectDrawSurface7 *, uint32_t, struct IDirectDrawSurface7 *);
void __stdcall DestroyDirtyRectangles(void);
void __stdcall DestroyRenderToTexture(void);
void __stdcall InvalidateVertexBuffers(void);
void __stdcall gos_Free(PVOID);
struct _VidMemHeap * pFreeVidMemTextures;
int32_t __stdcall wSetTexture(struct IDirect3DDevice7 *, uint32_t, struct IDirectDrawSurface7 *);
int32_t __stdcall wSetClipper(struct IDirectDrawSurface7 *, struct IDirectDrawClipper *);
void __stdcall static CTexInfo::ReleaseTextures(bool);
void __stdcall FindVideoCards(void);
void __stdcall CreateCopyBuffers(void);
bool gUse3D;
int32_t __stdcall wEnumTextureFormats(struct IDirect3DDevice7 *, int32_t (__stdcall *)(struct _DDPIXELFORMAT *, PVOID), PVOID);
int32_t __stdcall CheckEnumProc(struct _DDPIXELFORMAT *, PVOID);
uint32_t ValidTextures;
int32_t __stdcall wGetCaps(struct IDirect3DDevice7 *, struct _D3DDeviceDesc7 *);
GUID IID_IDirect3DTnLHalDevice;
GUID IID_IDirect3DHALDevice;
uint32_t gDisableHardwareTandL;
GUID IID_IDirect3DBladeDevice;
GUID IID_IDirect3DRGBDevice;
int32_t __stdcall wQueryInterface(struct IUnknown *, const GUID *, PVOID*);
GUID IID_IDirect3D7;
uint32_t __stdcall GetColor(uint32_t, uint32_t);
int32_t __stdcall wGetCaps(struct IDirectDraw7 *, struct _DDCAPS_DX7 *, struct _DDCAPS_DX7 *);
int32_t __stdcall wGetSurfaceDesc(struct IDirectDrawSurface7 *, struct _DDSURFACEDESC2 *);
int32_t __stdcall wSetHWnd(struct IDirectDrawClipper *, uint32_t, HWND);
int32_t __stdcall wCreateClipper(struct IDirectDraw7 *, uint32_t, struct IDirectDrawClipper **, struct IUnknown *);
BOOL __stdcall WaitMessage(void);
int32_t __stdcall wGetAttachedSurface(struct IDirectDrawSurface7 *, struct _DDSCAPS2 *, struct IDirectDrawSurface7 **);
PSTR __stdcall ErrorNumberToMessage(int32_t);
int32_t __stdcall GetDeviceCaps(HDC hdc, int32_t index);
HDC DesktopDC;
bool WindowsNT;
uint32_t Compatibility3D;
uint32_t gCompatFlags1;
struct CardInfo * KnownCards;
uint32_t gCompatFlags;
uint32_t gNumberCompatFlags;
int32_t HasMaxUV;
int32_t HasMipMap;
int32_t HasAdd;
int32_t HasModulateAlpha;
int32_t HasDecal;
int32_t HasMipLodBias;
int32_t HasAntiAlias;
int32_t HasDither;
int32_t HasSpecular;
int32_t HasAGP;
float MaxGuardBandClip;
float MinGuardBandClip;
int32_t HasGuardBandClipping;
void __stdcall InitRenderToTexture(void);
void __stdcall UpdateBackBufferFormat(void);
void __stdcall ReCreateVertexBuffers(void);
const GUID IID_IDirectDrawColorControl;
const GUID IID_IDirectDrawGammaControl;
uint32_t NumDevices;
void __stdcall ExitGameOS(void);
int32_t ErrorReturn;
void __stdcall DoColorDialog(void);
bool gNoDialogs;
PSTR  ErrorMessageTitle;
bool RunFullScreen;
