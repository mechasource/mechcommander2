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
 directdraw.cpp - GameOS reference pseudo code

 MechCommander 2 source code

 2014-07-24 Jerker Back, created

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
HRESULT __stdcall wDirectDrawCreateEx(LPGUID pGUID, PVOID* ppDD, REFIID iid, IUnknown* punkouter);
HRESULT __stdcall wDirectDrawEnumerate(LPDDENUMCALLBACKA pCallback, LPDDENUMCALLBACKEXA pCallbackEx, PVOID pcontext);
HRESULT __stdcall wSetHWnd(LPDIRECTDRAWCLIPPER pddc, uint32_t flags, HWND hwnd);
HRESULT __stdcall wGetAvailableVidMem(LPDIRECTDRAW7 pdd7, LPDDSCAPS2 pDDSCaps2, ulong32_t* lpdwTotal, ulong32_t* lpdwFree);
HRESULT __stdcall wGetFourCCCodes(LPDIRECTDRAW7 pdd7, ulong32_t* pNumCodes, ulong32_t* pCodes);
HRESULT __stdcall wGetDeviceIdentifier(LPDIRECTDRAW7 pdd7, LPDDDEVICEIDENTIFIER2 pdddi2, uint32_t flags);
HRESULT __stdcall wGetCaps(LPDIRECTDRAW7 pdd7, LPDDCAPS pdddrivercaps, LPDDCAPS pddhelcaps);
HRESULT __stdcall wCreatePalette(LPDIRECTDRAW7 pdd7, uint32_t flags, LPPALETTEENTRY pcolourTable,LPDIRECTDRAWPALETTE* lplpDDPalette, IUnknown* punkouter);
HRESULT __stdcall wEnumDisplayModes(LPDIRECTDRAW7 pdd7, uint32_t flags,LPDDSURFACEDESC2 pDDSurfaceDesc, PVOID pcontext, LPDDENUMMODESCALLBACK2 pEnumModesCallback);
HRESULT __stdcall wGetDisplayMode(LPDIRECTDRAW7 pdd7, LPDDSURFACEDESC2 pDDSurfaceDesc);
HRESULT __stdcall wSetDisplayMode(LPDIRECTDRAW7 pdd7, uint32_t width, uint32_t height, uint32_t bpp, uint32_t refreshrate, uint32_t flags);
HRESULT __stdcall wRestoreDisplayMode(LPDIRECTDRAW7 pdd7);
HRESULT __stdcall wCreateClipper(LPDIRECTDRAW7 pdd7, uint32_t flags, LPDIRECTDRAWCLIPPER* ppDDClipper, IUnknown* punkouter);
HRESULT __stdcall wCreateSurface(LPDIRECTDRAW7 pdd7, LPDDSURFACEDESC2 pDDSurfaceDesc, LPDIRECTDRAWSURFACE7* ppDDSurface7, IUnknown* punkouter);
HRESULT __stdcall wSetCooperativeLevel(LPDIRECTDRAW7 pdd7, HWND hwnd, uint32_t flags);
HRESULT __stdcall wIsLost(LPDIRECTDRAWSURFACE7 pdds7);
HRESULT __stdcall wGetDC(LPDIRECTDRAWSURFACE7 pdds7, HDC* phDC);
HRESULT __stdcall wReleaseDC(LPDIRECTDRAWSURFACE7 pdds7, HDC hdc);
HRESULT __stdcall wSetcolourKey(LPDIRECTDRAWSURFACE7 pdd3, uint32_t flags, LPDDCOLORKEY pDDcolourKey);
HRESULT __stdcall wLock(LPDIRECTDRAWSURFACE7 pdd7, LPRECT pDestRect,LPDDSURFACEDESC2 pDDSurfaceDesc, uint32_t flags, HANDLE hevent);
HRESULT __stdcall wUnlock(LPDIRECTDRAWSURFACE7 pdds7, LPRECT prect);
HRESULT __stdcall wLock(LPDIRECTDRAWSURFACE pdds, LPRECT pDestRect, LPDDSURFACEDESC pDDSurfaceDesc, uint32_t flags, HANDLE hevent);
HRESULT __stdcall wUnlock(LPDIRECTDRAWSURFACE pdds, LPRECT prect);
HRESULT __stdcall wSetPalette(LPDIRECTDRAWSURFACE7 pdd7, LPDIRECTDRAWPALETTE pDDPalette);
HRESULT __stdcall wRestore(LPDIRECTDRAWSURFACE7 pdd7);
HRESULT __stdcall wBlt(LPDIRECTDRAWSURFACE7 pdd7, LPRECT pDestRect, LPDIRECTDRAWSURFACE7 pddsrcSurface, LPRECT psrcrect, uint32_t flags, LPDDBLTFX lpDDBltFx);
HRESULT __stdcall wBltFast(LPDIRECTDRAWSURFACE7 pdd7, uint32_t dwX, uint32_t dwY, LPDIRECTDRAWSURFACE7 pddsrcSurface, LPRECT psrcrect, uint32_t dwTrans);
HRESULT __stdcall wFlip(LPDIRECTDRAWSURFACE7 pdd7, LPDIRECTDRAWSURFACE7 pDDSurfaceTargetOverride, uint32_t flags);
HRESULT __stdcall wGetSurfaceDesc(LPDIRECTDRAWSURFACE7 pdd7, LPDDSURFACEDESC2 pDDSurfaceDesc);
HRESULT __stdcall wGetSurfaceDesc(LPDIRECTDRAWSURFACE pdds, LPDDSURFACEDESC pDDSurfaceDesc);
HRESULT __stdcall wGetPixelFormat(LPDIRECTDRAWSURFACE7 pdds7, LPDDPIXELFORMAT pDDPixelFormat);
HRESULT __stdcall wGetAttachedSurface(LPDIRECTDRAWSURFACE7 pdds7, LPDDSCAPS2 pDDSCaps2, LPDIRECTDRAWSURFACE7* ppDDAttachedSurface);
HRESULT __stdcall wAddAttachedSurface(LPDIRECTDRAWSURFACE7 pdd7, LPDIRECTDRAWSURFACE7 pDDSAttachedSurface);
HRESULT __stdcall wDeleteAttachedSurface(LPDIRECTDRAWSURFACE7 pdds7, uint32_t flags, LPDIRECTDRAWSURFACE7 pDDSAttachedSurface);
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
/// <param name="punkouter"></param>
/// <returns>HRESULT success</returns>
HRESULT __stdcall wDirectDrawCreateEx(LPGUID pGUID, PVOID* ppDD, REFIID iid, IUnknown* punkouter)
{
	HRESULT hr;

	// InternalFunctionSpew("GameOS_DirectDraw", "DirectDrawCreateEx()+", pGUID);
	hr = _DirectDrawCreateEx(pGUID, ppDD, iid, punkouter);
	// InternalFunctionSpew("GameOS_DirectDraw", " -0x%x", *ppDD);

	if (FAILED(hr))
	{
		PSTR errormessage = ErrorNumberToMessage(hr);
		if (InternalFunctionPause("FAILED (0x%x - %s) - DirectDrawCreateEx(0x%x)",
				hr, errormessage, pGUID))
		{
			ENTER_DEBUGGER;
		}
	}

	return hr;
}

/******************************************************************************/
/// <summary>
/// <c>wDirectDrawEnumerate</c>
/// </summary>
/// <remarks>
/// </remarks>
/// <param name="pCallback"></param>
/// <param name="pCallbackEx"></param>
/// <param name="pcontext"></param>
/// <returns>HRESULT success</returns>
HRESULT __stdcall wDirectDrawEnumerate(
	LPDDENUMCALLBACKA pCallback, LPDDENUMCALLBACKEXA pCallbackEx, PVOID pcontext)
{
	HRESULT hr;

	if (_DirectDrawEnumerateEx != nullptr)
	{
		// InternalFunctionSpew("GameOS_DirectDraw", "DirectDrawEnumerateEx()");
		uint32_t flags = DDENUM_NONDISPLAYDEVICES | DDENUM_DETACHEDSECONDARYDEVICES | DDENUM_ATTACHEDSECONDARYDEVICES;
		hr = _DirectDrawEnumerateEx(pCallbackEx, pcontext, flags);
	}
	else
	{
		// InternalFunctionSpew("GameOS_DirectDraw", "DirectDrawEnumerate()");
		hr = _DirectDrawEnumerate(pCallback, pcontext);
	}
	if (FAILED(hr))
	{
		PSTR errormessage = ErrorNumberToMessage(hr);
		if (InternalFunctionPause("FAILED (0x%x - %s) - DirectDrawEnumerate()",
				hr, errormessage))
		{
			ENTER_DEBUGGER;
		}
	}

	return hr;
}

/******************************************************************************/
/// <summary>
/// <c>wSetHWnd</c>
/// </summary>
/// <remarks>
/// </remarks>
/// <param name="pddc"></param>
/// <param name="flags"></param>
/// <param name="hwnd"></param>
/// <returns>HRESULT success</returns>
HRESULT __stdcall wSetHWnd(LPDIRECTDRAWCLIPPER pddc, uint32_t flags, HWND hwnd)
{
	HRESULT hr;

	// InternalFunctionSpew("GameOS_DirectDraw", "SetHWnd(flags=0x%x Window=0x%x)", flags, hwnd);
	hr = pddc->SetHWnd(flags, hwnd);

	if (FAILED(hr))
	{
		PSTR errormessage = ErrorNumberToMessage(hr);
		if (InternalFunctionPause("FAILED (0x%x - %s) - SetHWnd(flags=0x%x Window=0x%x)",
				hr, errormessage, flags, hwnd))
		{
			ENTER_DEBUGGER;
		}
	}

	return hr;
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
	LPDIRECTDRAW7 pdd7, LPDDSCAPS2 pDDSCaps2, ulong32_t* pdwTotal, ulong32_t* pdwFree)
{
	HRESULT hr;

	// InternalFunctionSpew("GameOS_DirectDraw", "GetAvailableVidMem( 0x%x,%s)+", pdd7,
	// GetSurfaceCaps(pDDSCaps2));
	hr = pdd7->GetAvailableVidMem(pDDSCaps2, pdwTotal, pdwFree);
	// InternalFunctionSpew("GameOS_DirectDraw", " -Total=%d, Free=%d", *lpdwTotal);

	if (FAILED(hr))
	{
		PSTR errormessage = ErrorNumberToMessage(hr);
		if (InternalFunctionPause("FAILED (0x%x - %s) - GetAvailableVidMem( 0x%x,%s)",
				hr, errormessage, pdd7, GetSurfaceCaps(pDDSCaps2)))
		{
			ENTER_DEBUGGER;
		}
	}

	return hr;
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
HRESULT __stdcall wGetFourCCCodes(LPDIRECTDRAW7 pdd7, ulong32_t* pNumCodes, ulong32_t* pCodes)
{
	HRESULT hr;

	// InternalFunctionSpew("GameOS_DirectDraw", "GetFourCCCodes(0x%x)", pdd7);
	hr = pdd7->GetFourCCCodes(pNumCodes, pCodes);

	if (FAILED(hr))
	{
		PSTR errormessage = ErrorNumberToMessage(hr);
		if (InternalFunctionPause("FAILED (0x%x - %s) - GetFourCCCodes(0x%x)",
				hr, errormessage, pdd7))
		{
			ENTER_DEBUGGER;
		}
	}

	return hr;
}

/******************************************************************************/
/// <summary>
/// <c>wGetDeviceIdentifier</c>
/// </summary>
/// <remarks>
/// </remarks>
/// <param name="pdd7"></param>
/// <param name="pdddi2"></param>
/// <param name="flags"></param>
/// <returns>HRESULT success</returns>
HRESULT __stdcall wGetDeviceIdentifier(
	LPDIRECTDRAW7 pdd7, LPDDDEVICEIDENTIFIER2 pdddi2, uint32_t flags)
{
	HRESULT hr;

	// InternalFunctionSpew("GameOS_DirectDraw", "GetDeviceIdentifier(0x%x)", pdd7);
	hr = pdd7->GetDeviceIdentifier(pdddi2, flags);

	if (FAILED(hr))
	{
		PSTR errormessage = ErrorNumberToMessage(hr);
		if (InternalFunctionPause(
				"FAILED (0x%x - %s) - GetDeviceIdentifier(0x%x)",
				hr, errormessage, pdd7))
		{
			ENTER_DEBUGGER;
		}
	}

	return hr;
}

/******************************************************************************/
/// <summary>
/// <c>wGetCaps</c>
/// </summary>
/// <remarks>
/// </remarks>
/// <param name="pdd7"></param>
/// <param name="pdddrivercaps"></param>
/// <param name="pddhelcaps"></param>
/// <returns>HRESULT success</returns>
HRESULT __stdcall wGetCaps(LPDIRECTDRAW7 pdd7, LPDDCAPS pdddrivercaps, LPDDCAPS pddhelcaps)
{
	HRESULT hr;

	// InternalFunctionSpew("GameOS_DirectDraw", "GetCaps(0x%x)", pdd7);
	hr = pdd7->GetCaps(pdddrivercaps, pddhelcaps);

	if (FAILED(hr))
	{
		PSTR errormessage = ErrorNumberToMessage(hr);
		if (InternalFunctionPause("FAILED (0x%x - %s) - GetCaps(0x%x)",
				hr, errormessage, pdd7))
		{
			ENTER_DEBUGGER;
		}
	}

	return hr;
}

/******************************************************************************/
/// <summary>
/// <c>wCreatePalette</c>
/// </summary>
/// <remarks>
/// </remarks>
/// <param name="pdd7"></param>
/// <param name="flags"></param>
/// <param name="pcolourTable"></param>
/// <param name="lplpDDPalette"></param>
/// <param name="punkouter"></param>
/// <returns>HRESULT success</returns>
HRESULT __stdcall wCreatePalette(LPDIRECTDRAW7 pdd7, uint32_t flags, LPPALETTEENTRY pcolourTable,
	LPDIRECTDRAWPALETTE* pDDPalette, IUnknown* punkouter)
{
	HRESULT hr;

	// InternalFunctionSpew("GameOS_DirectDraw", "CreatePalette(%s)+",
	// GetCreatePaletteFlags(flags));
	hr = pdd7->CreatePalette(flags, pcolourTable, pDDPalette, punkouter);
	// InternalFunctionSpew("GameOS_DirectDraw", " -0x%x");

	if (FAILED(hr))
	{
		PSTR errormessage = ErrorNumberToMessage(hr);
		if (InternalFunctionPause("FAILED (0x%x - %s) - CreatePalette(%s)",
				hr, errormessage, GetCreatePaletteFlags(flags)))
		{
			ENTER_DEBUGGER;
		}
	}

	return hr;
}

/******************************************************************************/
/// <summary>
/// <c>wEnumDisplayModes</c>
/// </summary>
/// <remarks>
/// </remarks>
/// <param name="pdd7"></param>
/// <param name="flags"></param>
/// <param name="pDDSurfaceDesc"></param>
/// <param name="pcontext"></param>
/// <param name="pEnumModesCallback"></param>
/// <returns>HRESULT success</returns>
HRESULT __stdcall wEnumDisplayModes(LPDIRECTDRAW7 pdd7, uint32_t flags,
	LPDDSURFACEDESC2 pDDSurfaceDesc, PVOID pcontext, LPDDENUMMODESCALLBACK2 pEnumModesCallback)
{
	HRESULT hr;

	// InternalFunctionSpew("GameOS_DirectDraw", "EnumDisplayModes( %s)",
	// GetEnumDisplayModeFlags(flags));
	hr = pdd7->EnumDisplayModes(flags, pDDSurfaceDesc, pcontext, pEnumModesCallback);

	if (FAILED(hr))
	{
		PSTR errormessage = ErrorNumberToMessage(hr);
		if (InternalFunctionPause("FAILED (0x%x - %s) - EnumDisplayModes( %s)",
				hr, errormessage, GetEnumDisplayModeFlags(flags)))
		{
			ENTER_DEBUGGER;
		}
	}

	return hr;
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
	HRESULT hr;

	// InternalFunctionSpew("GameOS_DirectDraw", "GetDisplayMode()");
	hr = pdd7->GetDisplayMode(pDDSurfaceDesc);
	if (FAILED(hr))
	{
		PSTR errormessage = ErrorNumberToMessage(hr);
		if (InternalFunctionPause("FAILED (0x%x - %s) - GetDisplayMode()",
				hr, errormessage))
		{
			ENTER_DEBUGGER;
		}
	}
	return hr;
}

/******************************************************************************/
/// <summary>
/// <c>wSetDisplayMode</c>
/// </summary>
/// <remarks>
/// </remarks>
/// <param name="pdd7"></param>
/// <param name="width"></param>
/// <param name="height"></param>
/// <param name="bpp"></param>
/// <param name="refreshrate"></param>
/// <param name="flags"></param>
/// <returns>HRESULT success</returns>
HRESULT __stdcall wSetDisplayMode(LPDIRECTDRAW7 pdd7, uint32_t width, uint32_t height,
	uint32_t bpp, uint32_t refreshrate, uint32_t flags)
{
	HRESULT hr;

	// InternalFunctionSpew("GameOS_DirectDraw", "SetDisplayMode(%dx%d %dbpp %dHz)", width,
	// height, bpp, refreshrate);
	hr = pdd7->SetDisplayMode(width, height, bpp, refreshrate, flags);

	if (FAILED(hr))
	{
		PSTR errormessage = ErrorNumberToMessage(hr);
		if (InternalFunctionPause("FAILED (0x%x - %s) - SetDisplayMode(%dx%d %dbpp %dHz)",
				hr, errormessage, width, height, bpp, refreshrate))
		{
			ENTER_DEBUGGER;
		}
	}

	return hr;
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
	HRESULT hr;

	// InternalFunctionSpew("GameOS_DirectDraw", "RestoreDisplayMode()");
	hr = pdd7->RestoreDisplayMode();
	if (FAILED(hr))
	{
		PSTR errormessage = ErrorNumberToMessage(hr);
		if (InternalFunctionPause("FAILED (0x%x - %s) - RestoreDisplayMode()",
				hr, errormessage))
		{
			ENTER_DEBUGGER;
		}
	}
	return hr;
}

/******************************************************************************/
/// <summary>
/// <c>wCreateClipper</c>
/// </summary>
/// <remarks>
/// </remarks>
/// <param name="pdd7"></param>
/// <param name="flags"></param>
/// <param name="ppDDClipper"></param>
/// <param name="punkouter"></param>
/// <returns>HRESULT success</returns>
HRESULT __stdcall wCreateClipper(
	LPDIRECTDRAW7 pdd7, uint32_t flags, LPDIRECTDRAWCLIPPER* ppDDClipper, IUnknown* punkouter)
{
	HRESULT hr;

	// InternalFunctionSpew("GameOS_DirectDraw", "CreateClipper()+");
	hr = pdd7->CreateClipper(flags, ppDDClipper, punkouter);
	// InternalFunctionSpew("GameOS_DirectDraw", " -0x%x");

	if (FAILED(hr))
	{
		PSTR errormessage = ErrorNumberToMessage(hr);
		if (InternalFunctionPause("FAILED (0x%x - %s) - CreateClipper()",
				hr, errormessage))
		{
			ENTER_DEBUGGER;
		}
	}
	return hr;
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
/// <param name="punkouter"></param>
/// <returns>HRESULT success</returns>
HRESULT __stdcall wCreateSurface(LPDIRECTDRAW7 pdd7, LPDDSURFACEDESC2 pDDSurfaceDesc,
	LPDIRECTDRAWSURFACE7* ppDDSurface7, IUnknown* punkouter)
{
	HRESULT hr;

	// InternalFunctionSpew("GameOS_DirectDraw", "CreateSurface( 0x%x:%s)+", pdd7,
	// GetSurfaceDescription(pDDSurfaceDesc));
	hr = pdd7->CreateSurface(pDDSurfaceDesc, ppDDSurface7, punkouter);
	// InternalFunctionSpew("GameOS_DirectDraw", " -0x%x");

	if (FAILED(hr))
	{
		PSTR errormessage = ErrorNumberToMessage(hr);
		if (InternalFunctionPause("FAILED (0x%x - %s) - CreateSurface( 0x%x:%s)",
				hr, errormessage, pdd7, GetSurfaceDescription(pDDSurfaceDesc)))
		{
			ENTER_DEBUGGER;
		}
	}
	return hr;
}

/******************************************************************************/
/// <summary>
/// <c>wSetCooperativeLevel</c>
/// </summary>
/// <remarks>
/// </remarks>
/// <param name="pdd7"></param>
/// <param name="hwnd"></param>
/// <param name="flags"></param>
/// <returns>HRESULT success</returns>
HRESULT __stdcall wSetCooperativeLevel(LPDIRECTDRAW7 pdd7, HWND hwnd, uint32_t flags)
{
	HRESULT hr;

	// InternalFunctionSpew("GameOS_DirectDraw", "SetCooperativeLevel( 0x%x,%s)", pdd7,
	// GetSetCooperativeLevelFlags(flags));
	hr = pdd7->SetCooperativeLevel(hwnd, flags);

	if (FAILED(hr))
	{
		PSTR errormessage = ErrorNumberToMessage(hr);
		if (InternalFunctionPause("FAILED (0x%x - %s) - SetCooperativeLevel( 0x%x,%s)",
				hr, errormessage, pdd7, GetSetCooperativeLevelFlags(flags)))
		{
			ENTER_DEBUGGER;
		}
	}
	return hr;
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
	HRESULT hr;

	// InternalFunctionSpew("GameOS_DirectDraw", "IsLost(0x%x)+", pdds7);
	hr = pdds7->IsLost();
	// InternalFunctionSpew("GameOS_DirectDraw", " -(%s)", (hr == DDERR_SURFACELOST) ? "Yes" :
	// "No");

	if (FAILED(hr) && hr != DDERR_SURFACELOST)
	{
		PSTR errormessage = ErrorNumberToMessage(hr);
		if (InternalFunctionPause("FAILED (0x%x - %s) - IsLost(0x%x)",
				hr, errormessage, pdds7))
		{
			ENTER_DEBUGGER;
		}
	}
	return hr;
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
	HRESULT hr;

	// InternalFunctionSpew("GameOS_DirectDraw", "GetDC(0x%x)+", pdds7);
	hr = pdds7->GetDC(phDC);
	// InternalFunctionSpew("GameOS_DirectDraw", " -0x%x");
	if (FAILED(hr))
	{
		PSTR errormessage = ErrorNumberToMessage(hr);
		if (InternalFunctionPause("FAILED (0x%x - %s) - GetDC(0x%x)",
				hr, errormessage, pdds7))
		{
			ENTER_DEBUGGER;
		}
	}
	return hr;
}

/******************************************************************************/
/// <summary>
/// <c>wReleaseDC</c>
/// </summary>
/// <remarks>
/// </remarks>
/// <param name="pdds7"></param>
/// <param name="hdc"></param>
/// <returns>HRESULT success</returns>
HRESULT __stdcall wReleaseDC(LPDIRECTDRAWSURFACE7 pdds7, HDC hdc)
{
	HRESULT hr;

	// InternalFunctionSpew("GameOS_DirectDraw", "ReleaseDC(0x%x, 0x%x)", pdds7, hdc);
	hr = pdds7->ReleaseDC(hdc);

	if (FAILED(hr))
	{
		PSTR errormessage = ErrorNumberToMessage(hr);
		if (InternalFunctionPause("FAILED (0x%x - %s) - ReleaseDC(0x%x, 0x%x)",
				hr, errormessage, pdds7, hdc))
		{
			ENTER_DEBUGGER;
		}
	}
	return hr;
}

/******************************************************************************/
/// <summary>
/// <c>wSetcolourKey</c>
/// </summary>
/// <remarks>
/// </remarks>
/// <param name="pdd3"></param>
/// <param name="flags"></param>
/// <param name="pDDcolourKey"></param>
/// <returns>HRESULT success</returns>
HRESULT __stdcall wSetcolourKey(
	LPDIRECTDRAWSURFACE7 pdds7, uint32_t flags, LPDDCOLORKEY pDDcolourKey)
{
	HRESULT hr;

	// InternalFunctionSpew("GameOS_DirectDraw","SetcolourKey(0x%x,%s(0x%x,0x%x))",pdds7,GetSetcolourKeyFlags(flags),pDDcolourKey->colourSpaceLowValue,
	// pDDcolourKey->colourSpaceHighValue);
	hr = pdds7->SetcolourKey(flags, pDDcolourKey);

	if (FAILED(hr))
	{
		PSTR errormessage = ErrorNumberToMessage(hr);
		if (InternalFunctionPause("FAILED (0x%x - %s) - SetcolourKey(0x%x,%s(0x%x,0x%x))",
				hr, errormessage, pdds7, GetSetcolourKeyFlags(flags),
				pDDcolourKey->colourSpaceLowValue, pDDcolourKey->colourSpaceHighValue))
		{
			ENTER_DEBUGGER;
		}
	}

	return hr;
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
/// <param name="flags"></param>
/// <param name="hevent"></param>
/// <returns>HRESULT success</returns>
HRESULT __stdcall wLock(LPDIRECTDRAWSURFACE7 pdd7, LPRECT pDestRect,
	LPDDSURFACEDESC2 pDDSurfaceDesc, uint32_t flags, HANDLE hevent)
{
	HRESULT hr;

	// InternalFunctionSpew("GameOS_DirectDraw", "Lock(0x%x,%s)", pdd7, GetLockFlags(flags));
	hr = pdd7->Lock(pDestRect, pDDSurfaceDesc, flags, hevent);

	if (FAILED(hr))
	{
		// unused = GetLockFlags(flags);
		PSTR errormessage = ErrorNumberToMessage(hr);
		if (InternalFunctionPause("FAILED (0x%x - %s) - Lock(0x%x,%s)",
				hr, errormessage, pdd7))
		{
			ENTER_DEBUGGER;
		}
	}

	return hr;
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
	HRESULT hr;
	char buffer[132];

	// InternalFunctionSpew("GameOS_DirectDraw", "Unlock(0x%x,%s)", pdds7, GetRectangle(buffer,
	// prect));
	hr = pdds7->Unlock(prect);

	if (FAILED(hr))
	{
		PSTR rectangle = GetRectangle(buffer, prect);
		PSTR errormessage = ErrorNumberToMessage(hr);
		if (InternalFunctionPause("FAILED (0x%x - %s) - Unlock(0x%x,%s)",
				hr, errormessage, pdds7, rectangle))
		{
			ENTER_DEBUGGER;
		}
	}

	return hr;
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
/// <param name="flags"></param>
/// <param name="hevent"></param>
/// <returns>HRESULT success</returns>
HRESULT __stdcall wLock(LPDIRECTDRAWSURFACE pdds, LPRECT pDestRect, LPDDSURFACEDESC pDDSurfaceDesc,
	uint32_t flags, HANDLE hevent)
{
	HRESULT hr;

	// InternalFunctionSpew("GameOS_DirectDraw", "Lock(0x%x,%s)", pdds, GetLockFlags(flags));
	hr = pdds->Lock(pDestRect, pDDSurfaceDesc, flags, hevent);

	if (FAILED(hr))
	{
		// unused = GetLockFlags(flags);
		PSTR errormessage = ErrorNumberToMessage(hr);
		if (InternalFunctionPause("FAILED (0x%x - %s) - Lock(0x%x,%s)",
				hr, errormessage, pdds))
		{
			ENTER_DEBUGGER;
		}
	}

	return hr;
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
	HRESULT hr;
	char buffer[132];

	// InternalFunctionSpew("GameOS_DirectDraw", "Unlock(0x%x,%s)", pdds, GetRectangle(buffer,
	// prect));
	hr = pdds->Unlock(prect);

	if (FAILED(hr))
	{
		PSTR rectangle = GetRectangle(buffer, prect);
		PSTR errormessage = ErrorNumberToMessage(hr);
		if (InternalFunctionPause("FAILED (0x%x - %s) - Unlock(0x%x,%s)",
				hr, errormessage, pdds, rectangle))
		{
			ENTER_DEBUGGER;
		}
	}

	return hr;
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
	HRESULT hr;

	// InternalFunctionSpew("GameOS_DirectDraw", "SetPalette(0x%x,0x%x)", pdd7, pDDPalette);
	hr = pdd7->SetPalette(pDDPalette);

	if (FAILED(hr))
	{
		PSTR errormessage = ErrorNumberToMessage(hr);
		if (InternalFunctionPause("FAILED (0x%x - %s) - SetPalette(0x%x,0x%x)",
				hr, errormessage, pdd7, pDDPalette))
		{
			ENTER_DEBUGGER;
		}
	}

	return hr;
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
	HRESULT hr;

	// InternalFunctionSpew("GameOS_DirectDraw", "Restore(0x%x)", pdd7);
	hr = pdd7->Restore();

	if (FAILED(hr))
	{
		PSTR errormessage = ErrorNumberToMessage(hr);
		if (InternalFunctionPause("FAILED (0x%x - %s) - Restore(0x%x)",
				hr, errormessage, pdd7))
		{
			ENTER_DEBUGGER;
		}
	}

	return hr;
}

/******************************************************************************/
/// <summary>
/// <c>wBlt</c>
/// </summary>
/// <remarks>
/// </remarks>
/// <param name="pdd7"></param>
/// <param name="pDestRect"></param>
/// <param name="pddsrcSurface"></param>
/// <param name="psrcrect"></param>
/// <param name="flags"></param>
/// <param name="lpDDBltFx"></param>
/// <returns>HRESULT success</returns>
HRESULT __stdcall wBlt(LPDIRECTDRAWSURFACE7 pdd7, LPRECT pDestRect,
	LPDIRECTDRAWSURFACE7 pddsrcSurface, LPRECT psrcrect, uint32_t flags, LPDDBLTFX lpDDBltFx)
{
	HRESULT hr;

	// InternalFunctionSpew("GameOS_DirectDraw", "Blt(%s)", GetBltInformation(pdd7, pDestRect,
	// pddsrcSurface, psrcrect, flags, lpDDBltFx));
	hr = pdd7->Blt(pDestRect, pddsrcSurface, psrcrect, flags, lpDDBltFx);

	if (FAILED(hr) && hr != DDERR_SURFACELOST)
	{
		PSTR errormessage = ErrorNumberToMessage(hr);
		if (InternalFunctionPause("FAILED (0x%x - %s) - Blt(%s)",
				hr, errormessage,
				GetBltInformation(pdd7, pDestRect, pddsrcSurface, psrcrect, flags, lpDDBltFx)))
		{
			ENTER_DEBUGGER;
		}
	}

	return hr;
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
/// <param name="pddsrcSurface"></param>
/// <param name="psrcrect"></param>
/// <param name="dwTrans"></param>
/// <returns>HRESULT success</returns>
HRESULT __stdcall wBltFast(LPDIRECTDRAWSURFACE7 pdd7, uint32_t dwX, uint32_t dwY,
	LPDIRECTDRAWSURFACE7 pddsrcSurface, LPRECT psrcrect, uint32_t dwTrans)
{
	HRESULT hr;

	// InternalFunctionSpew("GameOS_DirectDraw", "BltFast(%s)", GetBltFastInformation(pdd7, dwX,
	// dwY, pddsrcSurface, psrcrect, dwTrans));
	hr = pdd7->BltFast(dwX, dwY, pddsrcSurface, psrcrect, dwTrans);

	if (FAILED(hr))
	{
		PSTR errormessage = ErrorNumberToMessage(hr);
		if (InternalFunctionPause("FAILED (0x%x - %s) - BltFast(%s)",
				hr, errormessage,
				GetBltFastInformation(pdd7, dwX, dwY, pddsrcSurface, psrcrect, dwTrans)))
		{
			ENTER_DEBUGGER;
		}
	}

	return hr;
}

/******************************************************************************/
/// <summary>
/// <c>wFlip</c>
/// </summary>
/// <remarks>
/// </remarks>
/// <param name="pdd7"></param>
/// <param name="pDDSurfaceTargetOverride"></param>
/// <param name="flags"></param>
/// <returns>HRESULT success</returns>
HRESULT __stdcall wFlip(
	LPDIRECTDRAWSURFACE7 pdd7, LPDIRECTDRAWSURFACE7 pDDSurfaceTargetOverride, uint32_t flags)
{
	HRESULT hr;

	// InternalFunctionSpew("GameOS_DirectDraw", "Flip( 0x%x,0x%x,%s)", pdd7,
	// pDDSurfaceTargetOverride, GetFlipFlags(flags));
	hr = pdd7->Flip(pDDSurfaceTargetOverride, flags);

	if (FAILED(hr) && hr != DDERR_SURFACELOST)
	{
		PSTR errormessage = ErrorNumberToMessage(hr);
		if (InternalFunctionPause("FAILED (0x%x - %s) - Flip( 0x%x,0x%x,%s)",
				hr, errormessage,
				pdd7, pDDSurfaceTargetOverride, GetFlipFlags(flags)))
		{
			ENTER_DEBUGGER;
		}
	}
	return hr;
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
	HRESULT hr;

	// InternalFunctionSpew("GameOS_DirectDraw", "GetSurfaceDesc(0x%x)+", pdd7);
	hr = pdd7->GetSurfaceDesc(pDDSurfaceDesc);
	// InternalFunctionSpew("GameOS_DirectDraw", " -( %s)", GetSurfaceDescription(pDDSurfaceDesc));

	if (FAILED(hr))
	{
		PSTR errormessage = ErrorNumberToMessage(hr);
		if (InternalFunctionPause("FAILED (0x%x - %s) - GetSurfaceDesc(0x%x)",
				hr, errormessage, pdd7))
		{
			ENTER_DEBUGGER;
		}
	}

	return hr;
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
	HRESULT hr;

	// InternalFunctionSpew("GameOS_DirectDraw", "GetSurfaceDesc(0x%x)+", pdds);
	hr = pdds->GetSurfaceDesc(pDDSurfaceDesc);
	// InternalFunctionSpew("GameOS_DirectDraw", " -( %s)",
	// GetSurfaceDescriptionOld(pDDSurfaceDesc));

	if (FAILED(hr))
	{
		PSTR errormessage = ErrorNumberToMessage(hr);
		if (InternalFunctionPause("FAILED (0x%x - %s) - GetSurfaceDesc(0x%x)",
				hr, errormessage, pdds))
		{
			ENTER_DEBUGGER;
		}
	}

	return hr;
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
	HRESULT hr;

	// InternalFunctionSpew("GameOS_DirectDraw", "GetPixelFormat(0x%x)+", pdds7);
	hr = pdds7->GetPixelFormat(pDDPixelFormat);
	// InternalFunctionSpew("GameOS_DirectDraw", " -( %s)", GetGosPixelFormat(pDDPixelFormat));

	if (FAILED(hr))
	{
		PSTR errormessage = ErrorNumberToMessage(hr);
		if (InternalFunctionPause("FAILED (0x%x - %s) - GetPixelFormat(0x%x)",
				hr, errormessage, pdds7))
		{
			ENTER_DEBUGGER;
		}
	}

	return hr;
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
	HRESULT hr;

	// InternalFunctionSpew("GameOS_DirectDraw", "GetAttachedSurface(0x%x)+", pdds7);
	hr = pdds7->GetAttachedSurface(pDDSCaps2, ppDDAttachedSurface);
	// InternalFunctionSpew("GameOS_DirectDraw", " -0x%x");

	if (FAILED(hr))
	{
		PSTR errormessage = ErrorNumberToMessage(hr);
		if (InternalFunctionPause("FAILED (0x%x - %s) - GetAttachedSurface(0x%x)",
				hr, errormessage, pdds7))
		{
			ENTER_DEBUGGER;
		}
	}

	return hr;
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
	HRESULT hr;

	// InternalFunctionSpew("GameOS_DirectDraw", "AddAttachedSurface(0x%x,0x%x)", pdd7,
	// pDDSAttachedSurface);
	hr = pdd7->AddAttachedSurface(pDDSAttachedSurface);

	if (FAILED(hr))
	{
		PSTR errormessage = ErrorNumberToMessage(hr);
		if (InternalFunctionPause("FAILED (0x%x - %s) - AddAttachedSurface(0x%x- 0x%x)",
				hr, errormessage, pdd7, pDDSAttachedSurface))
		{
			ENTER_DEBUGGER;
		}
	}

	return hr;
}

/******************************************************************************/
/// <summary>
/// <c>wDeleteAttachedSurface</c>
/// </summary>
/// <remarks>
/// </remarks>
/// <param name="pdds7"></param>
/// <param name="flags"></param>
/// <param name="pDDSAttachedSurface"></param>
/// <returns>HRESULT success</returns>
HRESULT __stdcall wDeleteAttachedSurface(
	LPDIRECTDRAWSURFACE7 pdds7, uint32_t flags, LPDIRECTDRAWSURFACE7 pDDSAttachedSurface)
{
	HRESULT hr;

	// InternalFunctionSpew("GameOS_DirectDraw", "DeleteAttachedSurface(0x%x,0x%x)", pdds7,
	// pDDSAttachedSurface);
	hr = pdds7->DeleteAttachedSurface(flags, pDDSAttachedSurface);

	if (FAILED(hr))
	{
		PSTR errormessage = ErrorNumberToMessage(hr);
		if (InternalFunctionPause("FAILED (0x%x - %s) - DeleteAttachedSurface(0x%x,0x%x)",
				hr, errormessage, pdds7, pDDSAttachedSurface))
		{
			ENTER_DEBUGGER;
		}
	}

	return hr;
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
	HRESULT hr;

	// InternalFunctionSpew("GameOS_DirectDraw", "SetClipper(0x%x,0x%x)", pdd7, pDDClipper);
	hr = pdd7->SetClipper(pDDClipper);
	if (FAILED(hr))
	{
		PSTR errormessage = ErrorNumberToMessage(hr);
		if (InternalFunctionPause("FAILED (0x%x - %s) - SetClipper(0x%x- 0x%x)",
				hr, errormessage, pdd7, pDDClipper))
		{
			ENTER_DEBUGGER;
		}
	}

	return hr;
}
