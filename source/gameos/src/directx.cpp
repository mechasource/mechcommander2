/*******************************************************************************
 This file consist of reversed and interpreted code from object source in the
 x86 debug build gameos.lib. The code in this file can not be used in any way
 other than serve as an information reference to the original released source of
 Mechcommander2. The code is a work of progress and there is no guarantee it is
 complete, accurate or useful in any way. The purpose is instead to make it
 possible to safely remove any dependencies of gameos.lib from Mechcommander2.
 All code is logically copyrighted by Microsoft
*******************************************************************************/
/*******************************************************************************
 directx.cpp - GameOS reference pseudo code

 MechCommander 2 source code

 2014-07-24 Jerker Bäck, created

*******************************************************************************/

#include "stdinc.h"

// -----------------------------------------------------------------------------
// Global data exported from this module

// global referenced data not listed in headers

// local data

// -----------------------------------------------------------------------------
// global implemented functions in this module listed in headers
HRESULT __stdcall wCoCreateInstance(REFCLSID rclsid, LPUNKNOWN punkouter, uint32_t clscontext, REFIID riid, PVOID* ppv);
HRESULT __stdcall wQueryInterface(IUnknown* iun, REFIID riid, PVOID* obp);
uint32_t __stdcall wRelease(IUnknown* iun);
uint32_t __stdcall wAddRef(IUnknown* iun);

/// <summary>
/// <c>wCoCreateInstance</c>
/// </summary>
/// <remarks>
/// </remarks>
/// <param name=""></param>
/// <returns></returns>
HRESULT STDMETHODCALLTYPE
wCoCreateInstance(REFCLSID rclsid, LPUNKNOWN punkouter, uint32_t clscontext, REFIID riid, PVOID* ppv)
{
	PSTR iface;
	PSTR message;
	HRESULT hr;

	iface = GetReturnInterface(riid);
	InternalFunctionSpew("GameOS_DirectX", "CoCreateInstance(%s)+", iface);
	hr = CoCreateInstance(rclsid, punkouter, clscontext, riid, ppv);
	InternalFunctionSpew("GameOS_DirectX", " -0x%x", *ppv);
	if (FAILED(hr))
	{
		iface = GetReturnInterface(riid);
		message = ErrorNumberToMessage(hr);
		if (InternalFunctionPause("FAILED (0x%x - %s) - CoCreateInstance(%s)\nTry ", hr, message, iface))
			__debugbreak();
	}
	return hr;
}

/// <summary>
/// <c>wQueryInterface</c>
/// </summary>
/// <remarks>
/// </remarks>
/// <param name=""></param>
/// <returns></returns>
HRESULT STDMETHODCALLTYPE
wQueryInterface(IUnknown* punk, REFIID riid, PVOID* ppv)
{
	PSTR iface;
	PSTR message;
	HRESULT hr;

	iface = GetReturnInterface(riid);
	InternalFunctionSpew("GameOS_DirectX", "QueryInterface(%s from 0x%x)+", iface, punk);
	hr = punk->QueryInterface(punk, riid, ppv);
	InternalFunctionSpew("GameOS_DirectX", " -0x%x");
	if (FAILED(hr))
	{
		iface = GetReturnInterface(riid);
		message = ErrorNumberToMessage((HRESULT)punk);
		if (InternalFunctionPause("FAILED (0x%x - %s) - QueryInterface(%s from 0x%x)", punk, message, iface))
			__debugbreak();
	}
	return (HRESULT)hr;
}

/// <summary>
/// <c>wRelease</c>
/// </summary>
/// <remarks>
/// </remarks>
/// <param name=""></param>
/// <returns></returns>
ULONG __stdcall wRelease(IUnknown* punk)
{
	ULONG ur;

	InternalFunctionSpew("GameOS_DirectX", "Release(0x%x)+", punk);
	ur = punk->Release(punk);
	InternalFunctionSpew("GameOS_DirectX", " - %d", v1);
	return ur;
}

/// <summary>
/// <c>wAddRef</c>
/// </summary>
/// <remarks>
/// </remarks>
/// <param name=""></param>
/// <returns></returns>
ULONG __stdcall wAddRef(IUnknown* punk)
{
	ULONG ur;

	InternalFunctionSpew("GameOS_DirectX", "AddRef(0x%x)+", punk);
	ur = punk->AddRef(punk);
	InternalFunctionSpew("GameOS_DirectX", " - %d", v1);
	return ur;
}
