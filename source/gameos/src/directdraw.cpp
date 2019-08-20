/*******************************************************************************
 This file consist of reversed and interpreted code from object source in the
 x86 debug build gameos.lib. The code in this file can not be used in any way
 other than serve as an information reference to the original released source of
 Mechcommander2. The code is a work of progress and there is no guarantee it is
 complete, accurate or useful in any way. The purpose is instead to make it
 possible to safely remove any dependencies of gameos.lib from Mechcommander2.
*******************************************************************************/
/*******************************************************************************
 directdraw.cpp - GameOS reference pseudo code

 MechCommander 2 source code

 2014-07-24 Jerker Beck, created

*******************************************************************************/

#include "stdinc.h"

#include <ddraw.h>

#include "gameos.hpp"
#include "directx.hpp"
#include "windows.hpp"
#include "directxdebugging.hpp"

// -----------------------------------------------------------------------------
// Global data exported from this module

// global referenced data not listed in headers

// local data

// -----------------------------------------------------------------------------
// global implemented functions in this module listed in headers
// clang-format off
HRESULT __stdcall wDirectDrawCreateEx(LPGUID pGUID, PVOID* ppDD, REFIID iid, IUnknown* pUnkOuter);
HRESULT __stdcall wDirectDrawEnumerate(LPDDENUMCALLBACKA pCallback, LPDDENUMCALLBACKEXA pCallbackEx, PVOID pContext);
HRESULT __stdcall wSetHWnd(LPDIRECTDRAWCLIPPER pddc, uint32_t dwFlags, HWND hWnd);
HRESULT __stdcall wGetAvailableVidMem(LPDIRECTDRAW7 pdd7, LPDDSCAPS2 pDDSCaps2, pulong32_t lpdwTotal, pulong32_t lpdwFree);
HRESULT __stdcall wGetFourCCCodes(LPDIRECTDRAW7 pdd7, pulong32_t pNumCodes, pulong32_t pCodes);
HRESULT __stdcall wGetDeviceIdentifier(LPDIRECTDRAW7 pdd7, LPDDDEVICEIDENTIFIER2 pdddi2, uint32_t dwFlags);
HRESULT __stdcall wGetCaps(LPDIRECTDRAW7 pdd7, LPDDCAPS lpDDDriverCaps, LPDDCAPS lpDDHELCaps);
HRESULT __stdcall wCreatePalette(LPDIRECTDRAW7 pdd7, uint32_t dwFlags, LPPALETTEENTRY pColorTable,LPDIRECTDRAWPALETTE* lplpDDPalette, IUnknown* pUnkOuter);
HRESULT __stdcall wEnumDisplayModes(LPDIRECTDRAW7 pdd7, uint32_t dwFlags,LPDDSURFACEDESC2 pDDSurfaceDesc, PVOID pContext, LPDDENUMMODESCALLBACK2 pEnumModesCallback);
HRESULT __stdcall wGetDisplayMode(LPDIRECTDRAW7 pdd7, LPDDSURFACEDESC2 pDDSurfaceDesc);
HRESULT __stdcall wSetDisplayMode(LPDIRECTDRAW7 pdd7, uint32_t dwWidth, uint32_t dwHeight, uint32_t dwBPP, uint32_t dwRefreshRate, uint32_t dwFlags);
HRESULT __stdcall wRestoreDisplayMode(LPDIRECTDRAW7 pdd7);
HRESULT __stdcall wCreateClipper(LPDIRECTDRAW7 pdd7, uint32_t dwFlags, LPDIRECTDRAWCLIPPER* ppDDClipper, IUnknown* pUnkOuter);
HRESULT __stdcall wCreateSurface(LPDIRECTDRAW7 pdd7, LPDDSURFACEDESC2 pDDSurfaceDesc, LPDIRECTDRAWSURFACE7* ppDDSurface7, IUnknown* pUnkOuter);
HRESULT __stdcall wSetCooperativeLevel(LPDIRECTDRAW7 pdd7, HWND hWnd, uint32_t dwFlags);
HRESULT __stdcall wIsLost(LPDIRECTDRAWSURFACE7 pdds7);
HRESULT __stdcall wGetDC(LPDIRECTDRAWSURFACE7 pdds7, HDC* phDC);
HRESULT __stdcall wReleaseDC(LPDIRECTDRAWSURFACE7 pdds7, HDC hDC);
HRESULT __stdcall wSetColorKey(LPDIRECTDRAWSURFACE7 pdd3, uint32_t dwFlags, LPDDCOLORKEY pDDColorKey);
HRESULT __stdcall wLock(LPDIRECTDRAWSURFACE7 pdd7, LPRECT pDestRect,LPDDSURFACEDESC2 pDDSurfaceDesc, uint32_t dwFlags, HANDLE hEvent);
HRESULT __stdcall wUnlock(LPDIRECTDRAWSURFACE7 pdds7, LPRECT prect);
HRESULT __stdcall wLock(LPDIRECTDRAWSURFACE pdds, LPRECT pDestRect, LPDDSURFACEDESC pDDSurfaceDesc, uint32_t dwFlags, HANDLE hEvent);
HRESULT __stdcall wUnlock(LPDIRECTDRAWSURFACE pdds, LPRECT prect);
HRESULT __stdcall wSetPalette(LPDIRECTDRAWSURFACE7 pdd7, LPDIRECTDRAWPALETTE pDDPalette);
HRESULT __stdcall wRestore(LPDIRECTDRAWSURFACE7 pdd7);
HRESULT __stdcall wBlt(LPDIRECTDRAWSURFACE7 pdd7, LPRECT pDestRect, LPDIRECTDRAWSURFACE7 lpDDSrcSurface, LPRECT lpSrcRect, uint32_t dwFlags, LPDDBLTFX lpDDBltFx);
HRESULT __stdcall wBltFast(LPDIRECTDRAWSURFACE7 pdd7, uint32_t dwX, uint32_t dwY, LPDIRECTDRAWSURFACE7 lpDDSrcSurface, LPRECT lpSrcRect, uint32_t dwTrans);
HRESULT __stdcall wFlip(LPDIRECTDRAWSURFACE7 pdd7, LPDIRECTDRAWSURFACE7 pDDSurfaceTargetOverride, uint32_t dwFlags);
HRESULT __stdcall wGetSurfaceDesc(LPDIRECTDRAWSURFACE7 pdd7, LPDDSURFACEDESC2 pDDSurfaceDesc);
HRESULT __stdcall wGetSurfaceDesc(LPDIRECTDRAWSURFACE pdds, LPDDSURFACEDESC pDDSurfaceDesc);
HRESULT __stdcall wGetPixelFormat(LPDIRECTDRAWSURFACE7 pdds7, LPDDPIXELFORMAT pDDPixelFormat);
HRESULT __stdcall wGetAttachedSurface(LPDIRECTDRAWSURFACE7 pdds7, LPDDSCAPS2 pDDSCaps2, LPDIRECTDRAWSURFACE7* ppDDAttachedSurface);
HRESULT __stdcall wAddAttachedSurface(LPDIRECTDRAWSURFACE7 pdd7, LPDIRECTDRAWSURFACE7 pDDSAttachedSurface);
HRESULT __stdcall wDeleteAttachedSurface(LPDIRECTDRAWSURFACE7 pdds7, uint32_t dwFlags, LPDIRECTDRAWSURFACE7 pDDSAttachedSurface);
HRESULT __stdcall wSetClipper(LPDIRECTDRAWSURFACE7 pdd7, LPDIRECTDRAWCLIPPER pDDClipper);
// clang-format on

// global implemented functions not listed in headers

// local functions

// -----------------------------------------------------------------------------
// externals referenced in this file not specified in headers


/*
Note: The InternalFunctionPause variadic variables often seem to be messed up,
missing or in wrong order. I corrected a few to what I think was intended
*/

/******************************************************************************/
/// <summary>
/// <c>wDirectDrawCreateEx</c>
/// </summary>
/// <remarks>
/// </remarks>
/// <param name="pGUID"></param>
/// <param name="ppDD"></param>
/// <param name="iid"></param>
/// <param name="pUnkOuter"></param>
/// <returns>HRESULT success</returns>
HRESULT __stdcall wDirectDrawCreateEx(LPGUID pGUID, PVOID* ppDD, REFIID iid, IUnknown* pUnkOuter)
{
	HRESULT hResult;

	// InternalFunctionSpew("GameOS_DirectDraw", "DirectDrawCreateEx()+", pGUID);
	hResult = _DirectDrawCreateEx(pGUID, ppDD, iid, pUnkOuter);
	// InternalFunctionSpew("GameOS_DirectDraw", " -0x%x", *ppDD);

	if (FAILED(hResult))
	{
		PSTR errormessage = ErrorNumberToMessage(hResult);
		if (InternalFunctionPause("FAILED (0x%x - %s) - DirectDrawCreateEx(0x%x)",
				hResult, errormessage, pGUID))
		{
			ENTER_DEBUGGER;
		}
	}

	return hResult;
}

/******************************************************************************/
/// <summary>
/// <c>wDirectDrawEnumerate</c>
/// </summary>
/// <remarks>
/// </remarks>
/// <param name="pCallback"></param>
/// <param name="pCallbackEx"></param>
/// <param name="pContext"></param>
/// <returns>HRESULT success</returns>
HRESULT __stdcall wDirectDrawEnumerate(
	LPDDENUMCALLBACKA pCallback, LPDDENUMCALLBACKEXA pCallbackEx, PVOID pContext)
{
	HRESULT hResult;

	if (_DirectDrawEnumerateEx != nullptr)
	{
		// InternalFunctionSpew("GameOS_DirectDraw", "DirectDrawEnumerateEx()");
		uint32_t dwFlags = DDENUM_NONDISPLAYDEVICES | DDENUM_DETACHEDSECONDARYDEVICES |
			DDENUM_ATTACHEDSECONDARYDEVICES;
		hResult = _DirectDrawEnumerateEx(pCallbackEx, pContext, dwFlags);
	}
	else
	{
		// InternalFunctionSpew("GameOS_DirectDraw", "DirectDrawEnumerate()");
		hResult = _DirectDrawEnumerate(pCallback, pContext);
	}
	if (FAILED(hResult))
	{
		PSTR errormessage = ErrorNumberToMessage(hResult);
		if (InternalFunctionPause("FAILED (0x%x - %s) - DirectDrawEnumerate()", 
			hResult, errormessage))
		{
			ENTER_DEBUGGER;
		}
	}

	return hResult;
}

/******************************************************************************/
/// <summary>
/// <c>wSetHWnd</c>
/// </summary>
/// <remarks>
/// </remarks>
/// <param name="pddc"></param>
/// <param name="dwFlags"></param>
/// <param name="hWnd"></param>
/// <returns>HRESULT success</returns>
HRESULT __stdcall wSetHWnd(LPDIRECTDRAWCLIPPER pddc, uint32_t dwFlags, HWND hWnd)
{
	HRESULT hResult;

	// InternalFunctionSpew("GameOS_DirectDraw", "SetHWnd(Flags=0x%x Window=0x%x)", dwFlags, hWnd);
	hResult = pddc->SetHWnd(dwFlags, hWnd);

	if (FAILED(hResult))
	{
		PSTR errormessage = ErrorNumberToMessage(hResult);
		if (InternalFunctionPause("FAILED (0x%x - %s) - SetHWnd(Flags=0x%x Window=0x%x)",
			hResult, errormessage, dwFlags, hWnd))
		{
			ENTER_DEBUGGER;
		}
	}

	return hResult;
}

/******************************************************************************/
/// <summary>
/// <c>wGetAvailableVidMem</c>
/// </summary>
/// <remarks>
/// </remarks>
/// <param name="pdd7"></param>
/// <param name="pDDSCaps2"></param>
/// <param name="pdwTotal"></param>
/// <param name="pdwFree"></param>
/// <returns>HRESULT success</returns>
HRESULT __stdcall wGetAvailableVidMem(
	LPDIRECTDRAW7 pdd7, LPDDSCAPS2 pDDSCaps2, pulong32_t pdwTotal, pulong32_t pdwFree)
{
	HRESULT hResult;

	// InternalFunctionSpew("GameOS_DirectDraw", "GetAvailableVidMem( 0x%x,%s)+", pdd7,
	// GetSurfaceCaps(pDDSCaps2));
	hResult = pdd7->GetAvailableVidMem(pDDSCaps2, pdwTotal, pdwFree);
	// InternalFunctionSpew("GameOS_DirectDraw", " -Total=%d, Free=%d", *lpdwTotal);

	if (FAILED(hResult))
	{
		PSTR errormessage = ErrorNumberToMessage(hResult);
		if (InternalFunctionPause("FAILED (0x%x - %s) - GetAvailableVidMem( 0x%x,%s)", 
			hResult, errormessage, pdd7, GetSurfaceCaps(pDDSCaps2)))
		{
			ENTER_DEBUGGER;
		}
	}

	return hResult;
}

/******************************************************************************/
/// <summary>
/// <c>wGetFourCCCodes</c>
/// </summary>
/// <remarks>
/// </remarks>
/// <param name="pdd7"></param>
/// <param name="pNumCodes"></param>
/// <param name="pCodes"></param>
/// <returns>HRESULT success</returns>
HRESULT __stdcall wGetFourCCCodes(LPDIRECTDRAW7 pdd7, pulong32_t pNumCodes, pulong32_t pCodes)
{
	HRESULT hResult;

	// InternalFunctionSpew("GameOS_DirectDraw", "GetFourCCCodes(0x%x)", pdd7);
	hResult = pdd7->GetFourCCCodes(pNumCodes, pCodes);

	if (FAILED(hResult))
	{
		PSTR errormessage = ErrorNumberToMessage(hResult);
		if (InternalFunctionPause("FAILED (0x%x - %s) - GetFourCCCodes(0x%x)",
			hResult, errormessage, pdd7))
		{
			ENTER_DEBUGGER;
		}
	}

	return hResult;
}

/******************************************************************************/
/// <summary>
/// <c>wGetDeviceIdentifier</c>
/// </summary>
/// <remarks>
/// </remarks>
/// <param name="pdd7"></param>
/// <param name="pdddi2"></param>
/// <param name="dwFlags"></param>
/// <returns>HRESULT success</returns>
HRESULT __stdcall wGetDeviceIdentifier(
	LPDIRECTDRAW7 pdd7, LPDDDEVICEIDENTIFIER2 pdddi2, uint32_t dwFlags)
{
	HRESULT hResult;

	// InternalFunctionSpew("GameOS_DirectDraw", "GetDeviceIdentifier(0x%x)", pdd7);
	hResult = pdd7->GetDeviceIdentifier(pdddi2, dwFlags);

	if (FAILED(hResult))
	{
		PSTR errormessage = ErrorNumberToMessage(hResult);
		if (InternalFunctionPause(
				"FAILED (0x%x - %s) - GetDeviceIdentifier(0x%x)",
				hResult, errormessage, pdd7))
		{
			ENTER_DEBUGGER;
		}
	}

	return hResult;
}

/******************************************************************************/
/// <summary>
/// <c>wGetCaps</c>
/// </summary>
/// <remarks>
/// </remarks>
/// <param name="pdd7"></param>
/// <param name="lpDDDriverCaps"></param>
/// <param name="lpDDHELCaps"></param>
/// <returns>HRESULT success</returns>
HRESULT __stdcall wGetCaps(LPDIRECTDRAW7 pdd7, LPDDCAPS lpDDDriverCaps, LPDDCAPS lpDDHELCaps)
{
	HRESULT hResult;

	// InternalFunctionSpew("GameOS_DirectDraw", "GetCaps(0x%x)", pdd7);
	hResult = pdd7->GetCaps(lpDDDriverCaps, lpDDHELCaps);

	if (FAILED(hResult))
	{
		PSTR errormessage = ErrorNumberToMessage(hResult);
		if (InternalFunctionPause("FAILED (0x%x - %s) - GetCaps(0x%x)", 
			hResult, errormessage, pdd7))
		{
			ENTER_DEBUGGER;
		}
	}

	return hResult;
}

/******************************************************************************/
/// <summary>
/// <c>wCreatePalette</c>
/// </summary>
/// <remarks>
/// </remarks>
/// <param name="pdd7"></param>
/// <param name="dwFlags"></param>
/// <param name="pColorTable"></param>
/// <param name="lplpDDPalette"></param>
/// <param name="pUnkOuter"></param>
/// <returns>HRESULT success</returns>
HRESULT __stdcall wCreatePalette(LPDIRECTDRAW7 pdd7, uint32_t dwFlags, LPPALETTEENTRY pColorTable,
	LPDIRECTDRAWPALETTE* pDDPalette, IUnknown* pUnkOuter)
{
	HRESULT hResult;

	// InternalFunctionSpew("GameOS_DirectDraw", "CreatePalette(%s)+",
	// GetCreatePaletteFlags(dwFlags));
	hResult = pdd7->CreatePalette(dwFlags, pColorTable, pDDPalette, pUnkOuter);
	// InternalFunctionSpew("GameOS_DirectDraw", " -0x%x");

	if (FAILED(hResult))
	{
		PSTR errormessage = ErrorNumberToMessage(hResult);
		if (InternalFunctionPause("FAILED (0x%x - %s) - CreatePalette(%s)", 
			hResult, errormessage, GetCreatePaletteFlags(dwFlags)))
		{
			ENTER_DEBUGGER;
		}
	}

	return hResult;
}

/******************************************************************************/
/// <summary>
/// <c>wEnumDisplayModes</c>
/// </summary>
/// <remarks>
/// </remarks>
/// <param name="pdd7"></param>
/// <param name="dwFlags"></param>
/// <param name="pDDSurfaceDesc"></param>
/// <param name="pContext"></param>
/// <param name="pEnumModesCallback"></param>
/// <returns>HRESULT success</returns>
HRESULT __stdcall wEnumDisplayModes(LPDIRECTDRAW7 pdd7, uint32_t dwFlags,
	LPDDSURFACEDESC2 pDDSurfaceDesc, PVOID pContext, LPDDENUMMODESCALLBACK2 pEnumModesCallback)
{
	HRESULT hResult;

	// InternalFunctionSpew("GameOS_DirectDraw", "EnumDisplayModes( %s)",
	// GetEnumDisplayModeFlags(dwFlags));
	hResult = pdd7->EnumDisplayModes(dwFlags, pDDSurfaceDesc, pContext, pEnumModesCallback);

	if (FAILED(hResult))
	{
		PSTR errormessage = ErrorNumberToMessage(hResult);
		if (InternalFunctionPause("FAILED (0x%x - %s) - EnumDisplayModes( %s)", 
			hResult, errormessage, GetEnumDisplayModeFlags(dwFlags)))
		{
			ENTER_DEBUGGER;
		}
	}

	return hResult;
}

/******************************************************************************/
/// <summary>
/// <c>wGetDisplayMode</c>
/// </summary>
/// <remarks>
/// </remarks>
/// <param name="pdd7"></param>
/// <param name="pDDSurfaceDesc"></param>
/// <returns>HRESULT success</returns>
HRESULT __stdcall wGetDisplayMode(LPDIRECTDRAW7 pdd7, LPDDSURFACEDESC2 pDDSurfaceDesc)
{
	HRESULT hResult;

	// InternalFunctionSpew("GameOS_DirectDraw", "GetDisplayMode()");
	hResult = pdd7->GetDisplayMode(pDDSurfaceDesc);
	if (FAILED(hResult))
	{
		PSTR errormessage = ErrorNumberToMessage(hResult);
		if (InternalFunctionPause("FAILED (0x%x - %s) - GetDisplayMode()", 
			hResult, errormessage))
		{
			ENTER_DEBUGGER;
		}
	}
	return hResult;
}

/******************************************************************************/
/// <summary>
/// <c>wSetDisplayMode</c>
/// </summary>
/// <remarks>
/// </remarks>
/// <param name="pdd7"></param>
/// <param name="dwWidth"></param>
/// <param name="dwHeight"></param>
/// <param name="dwBPP"></param>
/// <param name="dwRefreshRate"></param>
/// <param name="dwFlags"></param>
/// <returns>HRESULT success</returns>
HRESULT __stdcall wSetDisplayMode(LPDIRECTDRAW7 pdd7, uint32_t dwWidth, uint32_t dwHeight,
	uint32_t dwBPP, uint32_t dwRefreshRate, uint32_t dwFlags)
{
	HRESULT hResult;

	// InternalFunctionSpew("GameOS_DirectDraw", "SetDisplayMode(%dx%d %dbpp %dHz)", dwWidth,
	// dwHeight, dwBPP, dwRefreshRate);
	hResult = pdd7->SetDisplayMode(dwWidth, dwHeight, dwBPP, dwRefreshRate, dwFlags);

	if (FAILED(hResult))
	{
		PSTR errormessage = ErrorNumberToMessage(hResult);
		if (InternalFunctionPause("FAILED (0x%x - %s) - SetDisplayMode(%dx%d %dbpp %dHz)",
			hResult, errormessage, dwWidth, dwHeight, dwBPP, dwRefreshRate))
		{
			ENTER_DEBUGGER;
		}
	}

	return hResult;
}

/******************************************************************************/
/// <summary>
/// <c>wRestoreDisplayMode</c>
/// </summary>
/// <remarks>
/// </remarks>
/// <param name="pdd7"></param>
/// <returns>HRESULT success</returns>
HRESULT __stdcall wRestoreDisplayMode(LPDIRECTDRAW7 pdd7)
{
	HRESULT hResult;

	// InternalFunctionSpew("GameOS_DirectDraw", "RestoreDisplayMode()");
	hResult = pdd7->RestoreDisplayMode();
	if (FAILED(hResult))
	{
		PSTR errormessage = ErrorNumberToMessage(hResult);
		if (InternalFunctionPause("FAILED (0x%x - %s) - RestoreDisplayMode()", 
			hResult, errormessage))
		{
			ENTER_DEBUGGER;
		}
	}
	return hResult;
}

/******************************************************************************/
/// <summary>
/// <c>wCreateClipper</c>
/// </summary>
/// <remarks>
/// </remarks>
/// <param name="pdd7"></param>
/// <param name="dwFlags"></param>
/// <param name="ppDDClipper"></param>
/// <param name="pUnkOuter"></param>
/// <returns>HRESULT success</returns>
HRESULT __stdcall wCreateClipper(
	LPDIRECTDRAW7 pdd7, uint32_t dwFlags, LPDIRECTDRAWCLIPPER* ppDDClipper, IUnknown* pUnkOuter)
{
	HRESULT hResult;

	// InternalFunctionSpew("GameOS_DirectDraw", "CreateClipper()+");
	hResult = pdd7->CreateClipper(dwFlags, ppDDClipper, pUnkOuter);
	// InternalFunctionSpew("GameOS_DirectDraw", " -0x%x");

	if (FAILED(hResult))
	{
		PSTR errormessage = ErrorNumberToMessage(hResult);
		if (InternalFunctionPause("FAILED (0x%x - %s) - CreateClipper()",
			hResult, errormessage))
		{
			ENTER_DEBUGGER;
		}
	}
	return hResult;
}

/******************************************************************************/
/// <summary>
/// <c>wCreateSurface</c>
/// </summary>
/// <remarks>
/// </remarks>
/// <param name="pdd7"></param>
/// <param name="pDDSurfaceDesc"></param>
/// <param name="ppDDSurface7"></param>
/// <param name="pUnkOuter"></param>
/// <returns>HRESULT success</returns>
HRESULT __stdcall wCreateSurface(LPDIRECTDRAW7 pdd7, LPDDSURFACEDESC2 pDDSurfaceDesc,
	LPDIRECTDRAWSURFACE7* ppDDSurface7, IUnknown* pUnkOuter)
{
	HRESULT hResult;

	// InternalFunctionSpew("GameOS_DirectDraw", "CreateSurface( 0x%x:%s)+", pdd7,
	// GetSurfaceDescription(pDDSurfaceDesc));
	hResult = pdd7->CreateSurface(pDDSurfaceDesc, ppDDSurface7, pUnkOuter);
	// InternalFunctionSpew("GameOS_DirectDraw", " -0x%x");

	if (FAILED(hResult))
	{
		PSTR errormessage = ErrorNumberToMessage(hResult);
		if (InternalFunctionPause("FAILED (0x%x - %s) - CreateSurface( 0x%x:%s)", 
			hResult, errormessage, pdd7, GetSurfaceDescription(pDDSurfaceDesc)))
		{
			ENTER_DEBUGGER;
		}
	}
	return hResult;
}

/******************************************************************************/
/// <summary>
/// <c>wSetCooperativeLevel</c>
/// </summary>
/// <remarks>
/// </remarks>
/// <param name="pdd7"></param>
/// <param name="hWnd"></param>
/// <param name="dwFlags"></param>
/// <returns>HRESULT success</returns>
HRESULT __stdcall wSetCooperativeLevel(LPDIRECTDRAW7 pdd7, HWND hWnd, uint32_t dwFlags)
{
	HRESULT hResult;

	// InternalFunctionSpew("GameOS_DirectDraw", "SetCooperativeLevel( 0x%x,%s)", pdd7,
	// GetSetCooperativeLevelFlags(dwFlags));
	hResult = pdd7->SetCooperativeLevel(hWnd, dwFlags);

	if (FAILED(hResult))
	{
		PSTR errormessage = ErrorNumberToMessage(hResult);
		if (InternalFunctionPause("FAILED (0x%x - %s) - SetCooperativeLevel( 0x%x,%s)",
			hResult, errormessage, pdd7, GetSetCooperativeLevelFlags(dwFlags)))
		{
			ENTER_DEBUGGER;
		}
	}
	return hResult;
}

/******************************************************************************/
/// <summary>
/// <c>wIsLost</c>
/// </summary>
/// <remarks>
/// </remarks>
/// <param name="pdds7"></param>
/// <returns>HRESULT success</returns>
HRESULT __stdcall wIsLost(LPDIRECTDRAWSURFACE7 pdds7)
{
	HRESULT hResult;

	// InternalFunctionSpew("GameOS_DirectDraw", "IsLost(0x%x)+", pdds7);
	hResult = pdds7->IsLost();
	// InternalFunctionSpew("GameOS_DirectDraw", " -(%s)", (hResult == DDERR_SURFACELOST) ? "Yes" :
	// "No");

	if (FAILED(hResult) && hResult != DDERR_SURFACELOST)
	{
		PSTR errormessage = ErrorNumberToMessage(hResult);
		if (InternalFunctionPause("FAILED (0x%x - %s) - IsLost(0x%x)",
			hResult, errormessage, pdds7))
		{
			ENTER_DEBUGGER;
		}
	}
	return hResult;
}

/******************************************************************************/
/// <summary>
/// <c>wGetDC</c>
/// </summary>
/// <remarks>
/// </remarks>
/// <param name="pdds7"></param>
/// <param name="phDC"></param>
/// <returns>HRESULT success</returns>
HRESULT __stdcall wGetDC(LPDIRECTDRAWSURFACE7 pdds7, HDC* phDC)
{
	HRESULT hResult;

	// InternalFunctionSpew("GameOS_DirectDraw", "GetDC(0x%x)+", pdds7);
	hResult = pdds7->GetDC(phDC);
	// InternalFunctionSpew("GameOS_DirectDraw", " -0x%x");
	if (FAILED(hResult))
	{
		PSTR errormessage = ErrorNumberToMessage(hResult);
		if (InternalFunctionPause("FAILED (0x%x - %s) - GetDC(0x%x)",
			hResult, errormessage, pdds7))
		{
			ENTER_DEBUGGER;
		}
	}
	return hResult;
}

/******************************************************************************/
/// <summary>
/// <c>wReleaseDC</c>
/// </summary>
/// <remarks>
/// </remarks>
/// <param name="pdds7"></param>
/// <param name="hDC"></param>
/// <returns>HRESULT success</returns>
HRESULT __stdcall wReleaseDC(LPDIRECTDRAWSURFACE7 pdds7, HDC hDC)
{
	HRESULT hResult;

	// InternalFunctionSpew("GameOS_DirectDraw", "ReleaseDC(0x%x, 0x%x)", pdds7, hDC);
	hResult = pdds7->ReleaseDC(hDC);

	if (FAILED(hResult))
	{
		PSTR errormessage = ErrorNumberToMessage(hResult);
		if (InternalFunctionPause("FAILED (0x%x - %s) - ReleaseDC(0x%x, 0x%x)",
			hResult, errormessage, pdds7, hDC))
		{
			ENTER_DEBUGGER;
		}
	}
	return hResult;
}

/******************************************************************************/
/// <summary>
/// <c>wSetColorKey</c>
/// </summary>
/// <remarks>
/// </remarks>
/// <param name="pdd3"></param>
/// <param name="dwFlags"></param>
/// <param name="pDDColorKey"></param>
/// <returns>HRESULT success</returns>
HRESULT __stdcall wSetColorKey(
	LPDIRECTDRAWSURFACE7 pdds7, uint32_t dwFlags, LPDDCOLORKEY pDDColorKey)
{
	HRESULT hResult;

	// InternalFunctionSpew("GameOS_DirectDraw","SetColorKey(0x%x,%s(0x%x,0x%x))",pdds7,GetSetColorKeyFlags(dwFlags),pDDColorKey->dwColorSpaceLowValue,
	// pDDColorKey->dwColorSpaceHighValue);
	hResult = pdds7->SetColorKey(dwFlags, pDDColorKey);

	if (FAILED(hResult))
	{
		PSTR errormessage = ErrorNumberToMessage(hResult);
		if (InternalFunctionPause("FAILED (0x%x - %s) - SetColorKey(0x%x,%s(0x%x,0x%x))",
			hResult, errormessage, pdds7, GetSetColorKeyFlags(dwFlags),
			pDDColorKey->dwColorSpaceLowValue, pDDColorKey->dwColorSpaceHighValue))
		{
			ENTER_DEBUGGER;
		}
	}

	return hResult;
}

/******************************************************************************/
/// <summary>
/// <c>wLock</c>
/// </summary>
/// <remarks>
/// </remarks>
/// <param name="pdd7"></param>
/// <param name="pDestRect"></param>
/// <param name="pDDSurfaceDesc"></param>
/// <param name="dwFlags"></param>
/// <param name="hEvent"></param>
/// <returns>HRESULT success</returns>
HRESULT __stdcall wLock(LPDIRECTDRAWSURFACE7 pdd7, LPRECT pDestRect,
	LPDDSURFACEDESC2 pDDSurfaceDesc, uint32_t dwFlags, HANDLE hEvent)
{
	HRESULT hResult;

	// InternalFunctionSpew("GameOS_DirectDraw", "Lock(0x%x,%s)", pdd7, GetLockFlags(dwFlags));
	hResult = pdd7->Lock(pDestRect, pDDSurfaceDesc, dwFlags, hEvent);

	if (FAILED(hResult))
	{
		// unused = GetLockFlags(dwFlags);
		PSTR errormessage = ErrorNumberToMessage(hResult);
		if (InternalFunctionPause("FAILED (0x%x - %s) - Lock(0x%x,%s)",
			hResult, errormessage, pdd7))
		{
			ENTER_DEBUGGER;
		}
	}

	return hResult;
}

/******************************************************************************/
/// <summary>
/// <c>wUnlock</c>
/// </summary>
/// <remarks>
/// </remarks>
/// <param name="pdds7"></param>
/// <param name="prect"></param>
/// <returns>HRESULT success</returns>
HRESULT __stdcall wUnlock(LPDIRECTDRAWSURFACE7 pdds7, LPRECT prect)
{
	HRESULT hResult;
	char buffer[132];

	// InternalFunctionSpew("GameOS_DirectDraw", "Unlock(0x%x,%s)", pdds7, GetRectangle(buffer,
	// prect));
	hResult = pdds7->Unlock(prect);

	if (FAILED(hResult))
	{
		PSTR rectangle	= GetRectangle(buffer, prect);
		PSTR errormessage = ErrorNumberToMessage(hResult);
		if (InternalFunctionPause("FAILED (0x%x - %s) - Unlock(0x%x,%s)",
			hResult, errormessage, pdds7, rectangle))
		{
			ENTER_DEBUGGER;
		}
	}

	return hResult;
}

/******************************************************************************/
/// <summary>
/// <c>wLock</c>
/// </summary>
/// <remarks>
/// </remarks>
/// <param name="pdds"></param>
/// <param name="pDestRect"></param>
/// <param name="pDDSurfaceDesc"></param>
/// <param name="dwFlags"></param>
/// <param name="hEvent"></param>
/// <returns>HRESULT success</returns>
HRESULT __stdcall wLock(LPDIRECTDRAWSURFACE pdds, LPRECT pDestRect, LPDDSURFACEDESC pDDSurfaceDesc,
	uint32_t dwFlags, HANDLE hEvent)
{
	HRESULT hResult;

	// InternalFunctionSpew("GameOS_DirectDraw", "Lock(0x%x,%s)", pdds, GetLockFlags(dwFlags));
	hResult = pdds->Lock(pDestRect, pDDSurfaceDesc, dwFlags, hEvent);

	if (FAILED(hResult))
	{
		// unused = GetLockFlags(dwFlags);
		PSTR errormessage = ErrorNumberToMessage(hResult);
		if (InternalFunctionPause("FAILED (0x%x - %s) - Lock(0x%x,%s)",
			hResult, errormessage, pdds))
		{
			ENTER_DEBUGGER;
		}
	}

	return hResult;
}

/******************************************************************************/
/// <summary>
/// <c>wUnlock</c>
/// </summary>
/// <remarks>
/// </remarks>
/// <param name="pdds"></param>
/// <param name="prect"></param>
/// <returns>HRESULT success</returns>
HRESULT __stdcall wUnlock(LPDIRECTDRAWSURFACE pdds, LPRECT prect)
{
	HRESULT hResult;
	char buffer[132];

	// InternalFunctionSpew("GameOS_DirectDraw", "Unlock(0x%x,%s)", pdds, GetRectangle(buffer,
	// prect));
	hResult = pdds->Unlock(prect);

	if (FAILED(hResult))
	{
		PSTR rectangle	= GetRectangle(buffer, prect);
		PSTR errormessage = ErrorNumberToMessage(hResult);
		if (InternalFunctionPause("FAILED (0x%x - %s) - Unlock(0x%x,%s)",
			hResult, errormessage, pdds, rectangle))
		{
			ENTER_DEBUGGER;
		}
	}

	return hResult;
}

/******************************************************************************/
/// <summary>
/// <c>wSetPalette</c>
/// </summary>
/// <remarks>
/// </remarks>
/// <param name="pdd7"></param>
/// <param name="pDDPalette"></param>
/// <returns>HRESULT success</returns>
HRESULT __stdcall wSetPalette(LPDIRECTDRAWSURFACE7 pdd7, LPDIRECTDRAWPALETTE pDDPalette)
{
	HRESULT hResult;

	// InternalFunctionSpew("GameOS_DirectDraw", "SetPalette(0x%x,0x%x)", pdd7, pDDPalette);
	hResult = pdd7->SetPalette(pDDPalette);

	if (FAILED(hResult))
	{
		PSTR errormessage = ErrorNumberToMessage(hResult);
		if (InternalFunctionPause("FAILED (0x%x - %s) - SetPalette(0x%x,0x%x)",
			hResult, errormessage, pdd7, pDDPalette))
		{
			ENTER_DEBUGGER;
		}
	}

	return hResult;
}

/******************************************************************************/
/// <summary>
/// <c>wRestore</c>
/// </summary>
/// <remarks>
/// </remarks>
/// <param name="pdd7"></param>
/// <returns>HRESULT success</returns>
HRESULT __stdcall wRestore(LPDIRECTDRAWSURFACE7 pdd7)
{
	HRESULT hResult;

	// InternalFunctionSpew("GameOS_DirectDraw", "Restore(0x%x)", pdd7);
	hResult = pdd7->Restore();

	if (FAILED(hResult))
	{
		PSTR errormessage = ErrorNumberToMessage(hResult);
		if (InternalFunctionPause("FAILED (0x%x - %s) - Restore(0x%x)", 
			hResult, errormessage, pdd7))
		{
			ENTER_DEBUGGER;
		}
	}

	return hResult;
}

/******************************************************************************/
/// <summary>
/// <c>wBlt</c>
/// </summary>
/// <remarks>
/// </remarks>
/// <param name="pdd7"></param>
/// <param name="pDestRect"></param>
/// <param name="lpDDSrcSurface"></param>
/// <param name="lpSrcRect"></param>
/// <param name="dwFlags"></param>
/// <param name="lpDDBltFx"></param>
/// <returns>HRESULT success</returns>
HRESULT __stdcall wBlt(LPDIRECTDRAWSURFACE7 pdd7, LPRECT pDestRect,
	LPDIRECTDRAWSURFACE7 lpDDSrcSurface, LPRECT lpSrcRect, uint32_t dwFlags, LPDDBLTFX lpDDBltFx)
{
	HRESULT hResult;

	// InternalFunctionSpew("GameOS_DirectDraw", "Blt(%s)", GetBltInformation(pdd7, pDestRect,
	// lpDDSrcSurface, lpSrcRect, dwFlags, lpDDBltFx));
	hResult = pdd7->Blt(pDestRect, lpDDSrcSurface, lpSrcRect, dwFlags, lpDDBltFx);

	if (FAILED(hResult) && hResult != DDERR_SURFACELOST)
	{
		PSTR errormessage = ErrorNumberToMessage(hResult);
		if (InternalFunctionPause("FAILED (0x%x - %s) - Blt(%s)", 
			hResult, errormessage,
			GetBltInformation(pdd7, pDestRect, lpDDSrcSurface, lpSrcRect, dwFlags, lpDDBltFx)))
		{
			ENTER_DEBUGGER;
		}
	}

	return hResult;
}

/******************************************************************************/
/// <summary>
/// <c>wBltFast</c>
/// </summary>
/// <remarks>
/// </remarks>
/// <param name="pdd7"></param>
/// <param name="dwX"></param>
/// <param name="dwY"></param>
/// <param name="lpDDSrcSurface"></param>
/// <param name="lpSrcRect"></param>
/// <param name="dwTrans"></param>
/// <returns>HRESULT success</returns>
HRESULT __stdcall wBltFast(LPDIRECTDRAWSURFACE7 pdd7, uint32_t dwX, uint32_t dwY,
	LPDIRECTDRAWSURFACE7 lpDDSrcSurface, LPRECT lpSrcRect, uint32_t dwTrans)
{
	HRESULT hResult;

	// InternalFunctionSpew("GameOS_DirectDraw", "BltFast(%s)", GetBltFastInformation(pdd7, dwX,
	// dwY, lpDDSrcSurface, lpSrcRect, dwTrans));
	hResult = pdd7->BltFast(dwX, dwY, lpDDSrcSurface, lpSrcRect, dwTrans);

	if (FAILED(hResult))
	{
		PSTR errormessage = ErrorNumberToMessage(hResult);
		if (InternalFunctionPause("FAILED (0x%x - %s) - BltFast(%s)", 
			hResult, errormessage,
			GetBltFastInformation(pdd7, dwX, dwY, lpDDSrcSurface, lpSrcRect, dwTrans)))
		{
			ENTER_DEBUGGER;
		}
	}

	return hResult;
}

/******************************************************************************/
/// <summary>
/// <c>wFlip</c>
/// </summary>
/// <remarks>
/// </remarks>
/// <param name="pdd7"></param>
/// <param name="pDDSurfaceTargetOverride"></param>
/// <param name="dwFlags"></param>
/// <returns>HRESULT success</returns>
HRESULT __stdcall wFlip(
	LPDIRECTDRAWSURFACE7 pdd7, LPDIRECTDRAWSURFACE7 pDDSurfaceTargetOverride, uint32_t dwFlags)
{
	HRESULT hResult;

	// InternalFunctionSpew("GameOS_DirectDraw", "Flip( 0x%x,0x%x,%s)", pdd7,
	// pDDSurfaceTargetOverride, GetFlipFlags(dwFlags));
	hResult = pdd7->Flip(pDDSurfaceTargetOverride, dwFlags);

	if (FAILED(hResult) && hResult != DDERR_SURFACELOST)
	{
		PSTR errormessage = ErrorNumberToMessage(hResult);
		if (InternalFunctionPause("FAILED (0x%x - %s) - Flip( 0x%x,0x%x,%s)",
			hResult, errormessage,
			pdd7, pDDSurfaceTargetOverride, GetFlipFlags(dwFlags)))
		{
			ENTER_DEBUGGER;
		}
	}
	return hResult;
}

/******************************************************************************/
/// <summary>
/// <c>wGetSurfaceDesc</c>
/// </summary>
/// <remarks>
/// </remarks>
/// <param name="pdd7"></param>
/// <param name="pDDSurfaceDesc"></param>
/// <returns>HRESULT success</returns>
HRESULT __stdcall wGetSurfaceDesc(LPDIRECTDRAWSURFACE7 pdd7, LPDDSURFACEDESC2 pDDSurfaceDesc)
{
	HRESULT hResult;

	// InternalFunctionSpew("GameOS_DirectDraw", "GetSurfaceDesc(0x%x)+", pdd7);
	hResult = pdd7->GetSurfaceDesc(pDDSurfaceDesc);
	// InternalFunctionSpew("GameOS_DirectDraw", " -( %s)", GetSurfaceDescription(pDDSurfaceDesc));

	if (FAILED(hResult))
	{
		PSTR errormessage = ErrorNumberToMessage(hResult);
		if (InternalFunctionPause("FAILED (0x%x - %s) - GetSurfaceDesc(0x%x)",
			hResult, errormessage, pdd7))
		{
			ENTER_DEBUGGER;
		}
	}

	return hResult;
}

/******************************************************************************/
/// <summary>
/// <c>wGetSurfaceDesc</c>
/// </summary>
/// <remarks>
/// </remarks>
/// <param name="pdds"></param>
/// <param name="pDDSurfaceDesc"></param>
/// <returns>HRESULT success</returns>
HRESULT __stdcall wGetSurfaceDesc(LPDIRECTDRAWSURFACE pdds, LPDDSURFACEDESC pDDSurfaceDesc)
{
	HRESULT hResult;

	// InternalFunctionSpew("GameOS_DirectDraw", "GetSurfaceDesc(0x%x)+", pdds);
	hResult = pdds->GetSurfaceDesc(pDDSurfaceDesc);
	// InternalFunctionSpew("GameOS_DirectDraw", " -( %s)",
	// GetSurfaceDescriptionOld(pDDSurfaceDesc));

	if (FAILED(hResult))
	{
		PSTR errormessage = ErrorNumberToMessage(hResult);
		if (InternalFunctionPause("FAILED (0x%x - %s) - GetSurfaceDesc(0x%x)",
			hResult, errormessage, pdds))
		{
			ENTER_DEBUGGER;
		}
	}

	return hResult;
}

/******************************************************************************/
/// <summary>
/// <c>wGetPixelFormat</c>
/// </summary>
/// <remarks>
/// </remarks>
/// <param name="pdds7"></param>
/// <param name="pDDPixelFormat"></param>
/// <returns>HRESULT success</returns>
HRESULT __stdcall wGetPixelFormat(LPDIRECTDRAWSURFACE7 pdds7, LPDDPIXELFORMAT pDDPixelFormat)
{
	HRESULT hResult;

	// InternalFunctionSpew("GameOS_DirectDraw", "GetPixelFormat(0x%x)+", pdds7);
	hResult = pdds7->GetPixelFormat(pDDPixelFormat);
	// InternalFunctionSpew("GameOS_DirectDraw", " -( %s)", GetGosPixelFormat(pDDPixelFormat));

	if (FAILED(hResult))
	{
		PSTR errormessage = ErrorNumberToMessage(hResult);
		if (InternalFunctionPause("FAILED (0x%x - %s) - GetPixelFormat(0x%x)",
			hResult, errormessage, pdds7))
		{
			ENTER_DEBUGGER;
		}
	}

	return hResult;
}

/******************************************************************************/
/// <summary>
/// <c>wGetAttachedSurface</c>
/// </summary>
/// <remarks>
/// </remarks>
/// <param name="pdds7"></param>
/// <param name="pDDSCaps2"></param>
/// <param name="ppDDAttachedSurface"></param>
/// <returns>HRESULT success</returns>
HRESULT __stdcall wGetAttachedSurface(
	LPDIRECTDRAWSURFACE7 pdds7, LPDDSCAPS2 pDDSCaps2, LPDIRECTDRAWSURFACE7* ppDDAttachedSurface)
{
	HRESULT hResult;

	// InternalFunctionSpew("GameOS_DirectDraw", "GetAttachedSurface(0x%x)+", pdds7);
	hResult = pdds7->GetAttachedSurface(pDDSCaps2, ppDDAttachedSurface);
	// InternalFunctionSpew("GameOS_DirectDraw", " -0x%x");

	if (FAILED(hResult))
	{
		PSTR errormessage = ErrorNumberToMessage(hResult);
		if (InternalFunctionPause("FAILED (0x%x - %s) - GetAttachedSurface(0x%x)", 
			hResult, errormessage, pdds7))
		{
			ENTER_DEBUGGER;
		}
	}

	return hResult;
}

/******************************************************************************/
/// <summary>
/// <c>wAddAttachedSurface</c>
/// </summary>
/// <remarks>
/// </remarks>
/// <param name="pdd7"></param>
/// <param name="pDDSAttachedSurface"></param>
/// <returns>HRESULT success</returns>
HRESULT __stdcall wAddAttachedSurface(
	LPDIRECTDRAWSURFACE7 pdd7, LPDIRECTDRAWSURFACE7 pDDSAttachedSurface)
{
	HRESULT hResult;

	// InternalFunctionSpew("GameOS_DirectDraw", "AddAttachedSurface(0x%x,0x%x)", pdd7,
	// pDDSAttachedSurface);
	hResult = pdd7->AddAttachedSurface(pDDSAttachedSurface);

	if (FAILED(hResult))
	{
		PSTR errormessage = ErrorNumberToMessage(hResult);
		if (InternalFunctionPause("FAILED (0x%x - %s) - AddAttachedSurface(0x%x- 0x%x)",
			hResult, errormessage, pdd7, pDDSAttachedSurface))
		{
			ENTER_DEBUGGER;
		}
	}

	return hResult;
}

/******************************************************************************/
/// <summary>
/// <c>wDeleteAttachedSurface</c>
/// </summary>
/// <remarks>
/// </remarks>
/// <param name="pdds7"></param>
/// <param name="dwFlags"></param>
/// <param name="pDDSAttachedSurface"></param>
/// <returns>HRESULT success</returns>
HRESULT __stdcall wDeleteAttachedSurface(
	LPDIRECTDRAWSURFACE7 pdds7, uint32_t dwFlags, LPDIRECTDRAWSURFACE7 pDDSAttachedSurface)
{
	HRESULT hResult;

	// InternalFunctionSpew("GameOS_DirectDraw", "DeleteAttachedSurface(0x%x,0x%x)", pdds7,
	// pDDSAttachedSurface);
	hResult = pdds7->DeleteAttachedSurface(dwFlags, pDDSAttachedSurface);

	if (FAILED(hResult))
	{
		PSTR errormessage = ErrorNumberToMessage(hResult);
		if (InternalFunctionPause("FAILED (0x%x - %s) - DeleteAttachedSurface(0x%x,0x%x)",
			hResult, errormessage, pdds7, pDDSAttachedSurface))
		{
			ENTER_DEBUGGER;
		}
	}

	return hResult;
}

/******************************************************************************/
/// <summary>
/// <c>wSetClipper</c>
/// </summary>
/// <remarks>
/// </remarks>
/// <param name="pdd7"></param>
/// <param name="pDDClipper"></param>
/// <returns>HRESULT success</returns>
HRESULT __stdcall wSetClipper(LPDIRECTDRAWSURFACE7 pdd7, LPDIRECTDRAWCLIPPER pDDClipper)
{
	HRESULT hResult;

	// InternalFunctionSpew("GameOS_DirectDraw", "SetClipper(0x%x,0x%x)", pdd7, pDDClipper);
	hResult = pdd7->SetClipper(pDDClipper);
	if (FAILED(hResult))
	{
		PSTR errormessage = ErrorNumberToMessage(hResult);
		if (InternalFunctionPause("FAILED (0x%x - %s) - SetClipper(0x%x- 0x%x)",
			hResult, errormessage, pdd7, pDDClipper))
		{
			ENTER_DEBUGGER;
		}
	}

	return hResult;
}
