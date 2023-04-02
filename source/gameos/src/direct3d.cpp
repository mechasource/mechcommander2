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
 direct3d.cpp - GameOS reference pseudo code

 MechCommander 2 source code

 2014-07-24 Jerker Back, created

*******************************************************************************/

#include "stdinc.h"

#include "gameos.hpp"
#include "directx.hpp"
#include "directxdebugging.hpp"
#include "errorhandler.hpp"

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

extern uint32_t TotalRenderStateChanges;

#pragma region wLock
/*
Appears unused
*/
#pragma endregion local info
/// <summary>
/// <c>wLock</c>
/// </summary>
/// <remarks>
/// </remarks>
/// <param name="d3dvb7"></param>
/// <param name="flags"></param>
/// <param name="lppdata"></param>
/// <param name="lpsize"></param>
/// <returns></returns>
HRESULT __stdcall wLock(
	LPDIRECT3DVERTEXBUFFER7 d3dvb7, uint32_t flags, PVOID* lppdata, uint32_t* lpsize)
{

	// InternalFunctionSpew("GameOS_Direct3D", "Lock(0x%x)", d3dvb7);

	HRESULT hr = d3dvb7->Lock(flags, lppdata, lpsize);
	if (FAILED(hr))
	{
		PSTR errormessage = ErrorNumberToMessage(hr);
		if (InternalFunctionPause("FAILED (0x%x - %s) - Lock(0x%x)", hr, errormessage, d3dvb7))
			ENTER_DEBUGGER;
	}
	return hr;
}

#pragma region wUnlock
/*

*/
#pragma endregion local info
/// <summary>
/// <c>wUnlock</c>
/// </summary>
/// <remarks>
/// </remarks>
/// <param name="d3dvb7"></param>
/// <returns></returns>
HRESULT __stdcall wUnlock(LPDIRECT3DVERTEXBUFFER7 d3dvb7)
{
	// InternalFunctionSpew("GameOS_Direct3D", "Unlock(0x%x)", d3dvb7);

	HRESULT hr = d3dvb7->Unlock();
	if (FAILED(hr))
	{
		PSTR errormessage = ErrorNumberToMessage(hr);
		if (InternalFunctionPause(
				"FAILED (0x%x - %s) - Unlock(0x%x)", hr, errormessage, d3dvb7))
			ENTER_DEBUGGER;
	}
	return hr;
}

#pragma region wOptimize
/*

*/
#pragma endregion local info
/// <summary>
/// <c>wOptimize</c>
/// </summary>
/// <remarks>
/// </remarks>
/// <param name="d3dvb7"></param>
/// <param name="pd3ddevice"></param>
/// <param name="flags"></param>
/// <returns></returns>
HRESULT __stdcall wOptimize(
	LPDIRECT3DVERTEXBUFFER7 d3dvb7, LPDIRECT3DDEVICE7 pd3ddevice, uint32_t flags)
{
	PSTR errormessage;
	HRESULT hr;
	InternalFunctionSpew("GameOS_Direct3D", "Optimize(0x%x)", d3dvb7);
	hr = d3dvb7->Optimize(pd3ddevice, flags);
	if (FAILED(hr))
	{
		errormessage = ErrorNumberToMessage(hr);
		if (InternalFunctionPause(
				"FAILED (0x%x - %s) - Optimize(0x%x)", hr, errormessage, d3dvb7))
			ENTER_DEBUGGER;
	}
	return hr;
}

#pragma region wProcessVertices
/*

*/
#pragma endregion local info
/// <summary>
/// <c>wProcessVertices</c>
/// </summary>
/// <remarks>
/// </remarks>
/// <param name="d3dvb7"></param>
/// <param name="vertexop"></param>
/// <param name="destindex"></param>
/// <param name="count"></param>
/// <param name="psrcbuffer"></param>
/// <param name="srcindex"></param>
/// <param name="pd3ddevice"></param>
/// <param name="flags"></param>
/// <returns></returns>
HRESULT __stdcall wProcessVertices(LPDIRECT3DVERTEXBUFFER7 d3dvb7, uint32_t vertexop,
	uint32_t destindex, uint32_t count, LPDIRECT3DVERTEXBUFFER7 psrcbuffer,
	uint32_t srcindex, LPDIRECT3DDEVICE7 pd3ddevice, uint32_t flags)
{
	PSTR errormessage;
	HRESULT hr;
	InternalFunctionSpew("GameOS_Direct3D", "ProcessVertices(Dest=0x%x, Src=0x%x, Vertices=%d)",
		d3dvb7, psrcbuffer, count);
	hr = d3dvb7->ProcessVertices(
		vertexop, destindex, count, psrcbuffer, srcindex, pd3ddevice, flags);
	if (FAILED(hr))
	{
		errormessage = ErrorNumberToMessage(hr);
		if (InternalFunctionPause("FAILED (0x%x - %s) - "
								  "ProcessVertices(Dest=0x%x, Src=0x%x, "
								  "Vertices=%d)",
				hr, errormessage, d3dvb7, psrcbuffer, count))
			ENTER_DEBUGGER;
	}
	return hr;
}

#pragma region wCreateDevice
/*

*/
#pragma endregion local info
/// <summary>
/// <c>wCreateDevice</c>
/// </summary>
/// <remarks>
/// </remarks>
/// <param name="d3d7"></param>
/// <param name="rclsid"></param>
/// <param name="pdds"></param>
/// <param name="ppd3ddevice7"></param>
/// <returns></returns>
HRESULT __stdcall wCreateDevice(LPDIRECT3D7 d3d7, REFCLSID rclsid, LPDIRECTDRAWSURFACE7 pdds,
	LPDIRECT3DDEVICE7* ppd3ddevice7)
{
	PSTR pszDevice;
	PSTR errormessage;
	HRESULT hr;
	pszDevice = Get3DDevice(rclsid);
	InternalFunctionSpew("GameOS_Direct3D", "CreateDevice(%s from 0x%x)+", pszDevice, pdds);
	hr = d3d7->CreateDevice(rclsid, pdds, ppd3ddevice7);
	InternalFunctionSpew("GameOS_Direct3D", " -0x%x", *ppd3ddevice7);
	if (FAILED(hr))
	{
		pszDevice = Get3DDevice(rclsid);
		errormessage = ErrorNumberToMessage(hr);
		if (InternalFunctionPause("FAILED (0x%x - %s) - CreateDevice(%s from 0x%x)", hr,
				errormessage, pszDevice, pdds))
			ENTER_DEBUGGER;
	}
	return hr;
}

#pragma region wEnumDevices
/*

*/
#pragma endregion local info
/// <summary>
/// <c>wEnumDevices</c>
/// </summary>
/// <remarks>
/// </remarks>
/// <param name="d3d7"></param>
/// <param name="lpEnumDevicesCallback"></param>
/// <param name="puserarg"></param>
/// <returns></returns>
HRESULT __stdcall wEnumDevices(
	LPDIRECT3D7 d3d7, LPD3DENUMDEVICESCALLBACK7 lpEnumDevicesCallback, PVOID puserarg)
{
	PSTR errormessage;
	HRESULT hr;
	InternalFunctionSpew("GameOS_Direct3D", "EnumDevices()");
	hr = d3d7->EnumDevices(lpEnumDevicesCallback, puserarg);
	if (FAILED(hr))
	{
		errormessage = ErrorNumberToMessage(hr);
		if (InternalFunctionPause("FAILED (0x%x - %s) - EnumDevices()", hr, errormessage))
			ENTER_DEBUGGER;
	}
	return hr;
}

#pragma region wEnumZBufferFormats
/*

*/
#pragma endregion local info
/// <summary>
/// <c>wEnumZBufferFormats</c>
/// </summary>
/// <remarks>
/// </remarks>
/// <param name="d3d7"></param>
/// <param name="riidDevice"></param>
/// <param name="penumcallback"></param>
/// <param name="pcontext"></param>
/// <returns></returns>
HRESULT __stdcall wEnumZBufferFormats(LPDIRECT3D7 d3d7, REFCLSID riidDevice,
	LPD3DENUMPIXELFORMATSCALLBACK penumcallback, PVOID pcontext)
{
	PSTR pszDevice;
	PSTR errormessage;
	HRESULT hr;
	pszDevice = Get3DDevice(riidDevice);
	InternalFunctionSpew("GameOS_Direct3D", "EnumZBufferFormats(%s)", pszDevice);
	hr = d3d7->EnumZBufferFormats(riidDevice, penumcallback, pcontext);
	if (FAILED(hr))
	{
		pszDevice = Get3DDevice(riidDevice);
		errormessage = ErrorNumberToMessage(hr);
		if (InternalFunctionPause(
				"FAILED (0x%x - %s) - EnumZBufferFormats(%s)", hr, errormessage, pszDevice))
			ENTER_DEBUGGER;
	}
	return hr;
}

#pragma region wCreateVertexBuffer
/*

*/
#pragma endregion local info
/// <summary>
/// <c>wCreateVertexBuffer</c>
/// </summary>
/// <remarks>
/// </remarks>
/// <param name="d3d7"></param>
/// <param name="pvbdesc"></param>
/// <param name="plpd3dvertexbuffer"></param>
/// <param name="flags"></param>
/// <returns></returns>
HRESULT __stdcall wCreateVertexBuffer(LPDIRECT3D7 d3d7, LPD3DVERTEXBUFFERDESC pvbdesc,
	LPDIRECT3DVERTEXBUFFER7* plpd3dvertexbuffer, uint32_t flags)
{
	uint32_t numvertices;
	PSTR errormessage;
	HRESULT hr;
	InternalFunctionSpew(
		"GameOS_Direct3D", "CreateVertexBuffer(%d Vertices)+", pvbdesc->numvertices);
	hr = d3d7->CreateVertexBuffer(pvbdesc, plpd3dvertexbuffer, flags);
	InternalFunctionSpew("GameOS_Direct3D", " -0x%x", *plpd3dvertexbuffer);
	if (FAILED(hr))
	{
		numvertices = pvbdesc->numvertices;
		errormessage = ErrorNumberToMessage(hr);
		if (InternalFunctionPause("FAILED (0x%x - %s) - CreateVertexBuffer(%d Vertices)", hr,
				errormessage, numvertices))
			ENTER_DEBUGGER;
	}
	return hr;
}

#pragma region wBeginScene
/*

*/
#pragma endregion local info
/// <summary>
/// <c>wBeginScene</c>
/// </summary>
/// <remarks>
/// </remarks>
/// <param name="d3ddevice7"></param>
/// <returns></returns>
HRESULT __stdcall wBeginScene(LPDIRECT3DDEVICE7 d3ddevice7)
{
	PSTR errormessage;
	HRESULT hr;
	InternalFunctionSpew("GameOS_Direct3D", "BeginScene()");
	hr = d3ddevice7->BeginScene();
	if (FAILED(hr))
	{
		if (hr != DDERR_SURFACELOST)
		{
			errormessage = ErrorNumberToMessage(hr);
			if (InternalFunctionPause(
					"FAILED (0x%x - %s) - BeginScene()", hr, errormessage))
				ENTER_DEBUGGER;
		}
	}
	return hr;
}

#pragma region wEndScene
/*

*/
#pragma endregion local info
/// <summary>
/// <c>wEndScene</c>
/// </summary>
/// <remarks>
/// </remarks>
/// <param name="d3ddevice7"></param>
/// <returns></returns>
HRESULT __stdcall wEndScene(LPDIRECT3DDEVICE7 d3ddevice7)
{
	PSTR errormessage;
	HRESULT hr;
	InternalFunctionSpew("GameOS_Direct3D", "EndScene()");
	hr = d3ddevice7->EndScene();
	if (FAILED(hr))
	{
		errormessage = ErrorNumberToMessage(hr);
		if (InternalFunctionPause("FAILED (0x%x - %s) - EndScene()", hr, errormessage))
			ENTER_DEBUGGER;
	}
	return hr;
}

#pragma region wSetTransform
/*

*/
#pragma endregion local info
/// <summary>
/// <c>wSetTransform</c>
/// </summary>
/// <remarks>
/// </remarks>
/// <param name="d3ddevice7"></param>
/// <param name="dtstTransformStateType"></param>
/// <param name="pd3dmatrix"></param>
/// <returns></returns>
HRESULT __stdcall wSetTransform(LPDIRECT3DDEVICE7 d3ddevice7,
	D3DTRANSFORMSTATETYPE dtstTransformStateType, LPD3DMATRIX pd3dmatrix)
{
	PSTR pszTransformType;
	PSTR errormessage;
	HRESULT hr;
	pszTransformType = GetTransformType(dtstTransformStateType);
	InternalFunctionSpew("GameOS_Direct3D", "SetTransform(%s)", pszTransformType);
	hr = d3ddevice7->SetTransform(dtstTransformStateType, pd3dmatrix);
	if (FAILED(hr))
	{
		pszTransformType = GetTransformType(dtstTransformStateType);
		errormessage = ErrorNumberToMessage(hr);
		if (InternalFunctionPause("FAILED (0x%x - %s) - SetTransform(%s)", hr, errormessage,
				pszTransformType))
			ENTER_DEBUGGER;
	}
	return hr;
}

#pragma region wMultiplyTransform
/*

*/
#pragma endregion local info
/// <summary>
/// <c>wMultiplyTransform</c>
/// </summary>
/// <remarks>
/// </remarks>
/// <param name="d3ddevice7"></param>
/// <param name="dtstTransformStateType"></param>
/// <param name="pd3dmatrix"></param>
/// <returns></returns>
HRESULT __stdcall wMultiplyTransform(LPDIRECT3DDEVICE7 d3ddevice7,
	D3DTRANSFORMSTATETYPE dtstTransformStateType, LPD3DMATRIX pd3dmatrix)
{
	PSTR pszTransformType;
	PSTR errormessage;
	HRESULT hr;
	pszTransformType = GetTransformType(dtstTransformStateType);
	InternalFunctionSpew("GameOS_Direct3D", "MultiplyTransform(%s)", pszTransformType);
	hr = d3ddevice7->MultiplyTransform(dtstTransformStateType, pd3dmatrix);
	if (FAILED(hr))
	{
		pszTransformType = GetTransformType(dtstTransformStateType);
		errormessage = ErrorNumberToMessage(hr);
		if (InternalFunctionPause("FAILED (0x%x - %s) - MultiplyTransform()", hr,
				errormessage, pszTransformType))
			ENTER_DEBUGGER;
	}
	return hr;
}

#pragma region wSetLight
/*

*/
#pragma endregion local info
/// <summary>
/// <c>wSetLight</c>
/// </summary>
/// <remarks>
/// </remarks>
/// <param name="d3ddevice7"></param>
/// <param name="lightindex"></param>
/// <param name="plight"></param>
/// <returns></returns>
HRESULT __stdcall wSetLight(
	LPDIRECT3DDEVICE7 d3ddevice7, uint32_t lightindex, LPD3DLIGHT7 plight)
{
	PSTR pszLightInfo;
	PSTR errormessage;
	HRESULT hr;
	pszLightInfo = GetLightInfo(plight);
	InternalFunctionSpew("GameOS_Direct3D", "SetLight(%d, %s)", lightindex, pszLightInfo);
	hr = d3ddevice7->SetLight(lightindex, plight);
	if (FAILED(hr))
	{
		pszLightInfo = GetLightInfo(plight);
		errormessage = ErrorNumberToMessage(hr);
		if (InternalFunctionPause("FAILED (0x%x - %s) - SetLight(%d, %s)", hr, errormessage,
				lightindex, pszLightInfo))
			ENTER_DEBUGGER;
	}
	return hr;
}

#pragma region wLightEnable
/*

*/
#pragma endregion local info
/// <summary>
/// <c>wLightEnable</c>
/// </summary>
/// <remarks>
/// </remarks>
/// <param name="d3ddevice7"></param>
/// <param name="lightindex"></param>
/// <param name="Enable"></param>
/// <returns></returns>
HRESULT __stdcall wLightEnable(LPDIRECT3DDEVICE7 d3ddevice7, uint32_t lightindex, BOOLEAN Enable)
{
	PSTR pszEnable;
	PSTR errormessage;
	HRESULT hr;
	if (Enable)
		pszEnable = "enable";
	else
		pszEnable = "disable";
	InternalFunctionSpew("GameOS_Direct3D", "LightEnable(%d, %s)", lightindex, pszEnable);
	hr = d3ddevice7->LightEnable(lightindex, Enable);
	if (FAILED(hr))
	{
		if (Enable)
			pszEnable = "enable";
		else
			pszEnable = "disable";
		errormessage = ErrorNumberToMessage(hr);
		if (InternalFunctionPause("FAILED (0x%x - %s) - LightEnable(%d, %s)", hr,
				errormessage, lightindex, pszEnable))
			ENTER_DEBUGGER;
	}
	return hr;
}

#pragma region wSetMaterial
/*

*/
#pragma endregion local info
/// <summary>
/// <c>wSetMaterial</c>
/// </summary>
/// <remarks>
/// </remarks>
/// <param name="d3ddevice7"></param>
/// <param name="pMaterial"></param>
/// <returns></returns>
HRESULT __stdcall wSetMaterial(LPDIRECT3DDEVICE7 d3ddevice7, LPD3DMATERIAL7 pMaterial)
{
	PSTR pszMaterialInfo;
	PSTR errormessage;
	HRESULT hr;
	pszMaterialInfo = GetMaterialInfo(pMaterial);
	InternalFunctionSpew("GameOS_Direct3D", "SetMaterial(%s)", pszMaterialInfo);
	hr = d3ddevice7->SetMaterial(pMaterial);
	if (FAILED(hr))
	{
		pszMaterialInfo = GetMaterialInfo(pMaterial);
		errormessage = ErrorNumberToMessage(hr);
		if (InternalFunctionPause(
				"FAILED (0x%x - %s) - SetMaterial(%s)", hr, errormessage, pszMaterialInfo))
			ENTER_DEBUGGER;
	}
	return hr;
}

#pragma region wSetViewport
/*

*/
#pragma endregion local info
/// <summary>
/// <c>wSetViewport</c>
/// </summary>
/// <remarks>
/// </remarks>
/// <param name="d3ddevice7"></param>
/// <param name="pviewport"></param>
/// <returns></returns>
HRESULT __stdcall wSetViewport(LPDIRECT3DDEVICE7 d3ddevice7, LPD3DVIEWPORT7 pviewport)
{
	PSTR errormessage;
	HRESULT hr;
	InternalFunctionSpew("GameOS_Direct3D", "SetViewport()");
	hr = d3ddevice7->SetViewport(pviewport);
	if (FAILED(hr))
	{
		errormessage = ErrorNumberToMessage(hr);
		if (InternalFunctionPause("FAILED (0x%x - %s) - SetViewport()", hr, errormessage))
			ENTER_DEBUGGER;
	}
	return hr;
}

#pragma region wClear
/*

*/
#pragma endregion local info
/// <summary>
/// <c>wClear</c>
/// </summary>
/// <remarks>
/// </remarks>
/// <param name="d3ddevice7"></param>
/// <param name="count"></param>
/// <param name="pd3drect"></param>
/// <param name="flags"></param>
/// <param name="colour"></param>
/// <param name="dvZ"></param>
/// <param name="stencil"></param>
/// <returns></returns>
HRESULT __stdcall wClear(LPDIRECT3DDEVICE7 d3ddevice7, uint32_t count, LPD3DRECT pd3drect,
	uint32_t flags, uint32_t colour, D3DVALUE dvZ, uint32_t stencil)
{
	PSTR pszClearArea;
	PSTR errormessage;
	HRESULT hr;
	pszClearArea = GetClearArea(count, pd3drect, flags);
	InternalFunctionSpew("GameOS_Direct3D", "Clear(%s)", pszClearArea);
	hr = d3ddevice7->Clear(count, pd3drect, flags, colour, dvZ, stencil);
	if (FAILED(hr))
	{
		if (hr != DDERR_SURFACELOST)
		{
			pszClearArea = GetClearArea(count, pd3drect, flags);
			errormessage = ErrorNumberToMessage(hr);
			if (InternalFunctionPause(
					"FAILED (0x%x - %s) - Clear(%s)", hr, errormessage, pszClearArea))
				ENTER_DEBUGGER;
		}
	}
	return hr;
}

#pragma region wGetCaps
/*

*/
#pragma endregion local info
/// <summary>
/// <c>wGetCaps</c>
/// </summary>
/// <remarks>
/// </remarks>
/// <param name="d3ddevice7"></param>
/// <param name="pd3dhwdevdesc"></param>
/// <returns></returns>
HRESULT __stdcall wGetCaps(LPDIRECT3DDEVICE7 d3ddevice7, LPD3DDEVICEDESC7 pd3dhwdevdesc)
{
	PSTR errormessage;
	HRESULT hr;
	InternalFunctionSpew("GameOS_Direct3D", "GetCaps(0x%x)", d3ddevice7);
	hr = d3ddevice7->GetCaps(pd3dhwdevdesc);
	if (FAILED(hr))
	{
		errormessage = ErrorNumberToMessage(hr);
		if (InternalFunctionPause(
				"FAILED (0x%x - %s) - GetCaps(0x%x)", hr, errormessage, d3ddevice7))
			ENTER_DEBUGGER;
	}
	return hr;
}

#pragma region wSetRenderState
/*

*/
#pragma endregion local info
/// <summary>
/// <c>wSetRenderState</c>
/// </summary>
/// <remarks>
/// </remarks>
/// <param name="d3ddevice7"></param>
/// <param name="renderstatetype"></param>
/// <param name="renderstate"></param>
/// <returns></returns>
HRESULT __stdcall wSetRenderState(
	LPDIRECT3DDEVICE7 d3ddevice7, D3DRENDERSTATETYPE renderstatetype, uint32_t renderstate)
{
	PSTR pszRenderState;
	PSTR errormessage;
	HRESULT hr;
	++TotalRenderStateChanges;
	pszRenderState = GetRenderState(renderstatetype, renderstate);
	InternalFunctionSpew("GameOS_Direct3D", "SetRenderState(%s)", pszRenderState);
	hr = d3ddevice7->SetRenderState(renderstatetype, renderstate);
	if (FAILED(hr))
	{
		pszRenderState = GetRenderState(renderstatetype, renderstate);
		errormessage = ErrorNumberToMessage(hr);
		if (InternalFunctionPause("FAILED (0x%x - %s) - SetRenderState(%s)", hr,
				errormessage, pszRenderState))
			ENTER_DEBUGGER;
	}
	return hr;
}

#pragma region wEnumTextureFormats
/*

*/
#pragma endregion local info
/// <summary>
/// <c>wEnumTextureFormats</c>
/// </summary>
/// <remarks>
/// </remarks>
/// <param name="d3ddevice7"></param>
/// <param name="pd3denumpixelproc"></param>
/// <param name="parg"></param>
/// <returns></returns>
HRESULT __stdcall wEnumTextureFormats(
	LPDIRECT3DDEVICE7 d3ddevice7, LPD3DENUMPIXELFORMATSCALLBACK pd3denumpixelproc, PVOID parg)
{
	PSTR errormessage;
	HRESULT hr;
	InternalFunctionSpew("GameOS_Direct3D", "EnumTextureFormats()");
	hr = d3ddevice7->EnumTextureFormats(pd3denumpixelproc, parg);
	if (FAILED(hr))
	{
		errormessage = ErrorNumberToMessage(hr);
		if (InternalFunctionPause(
				"FAILED (0x%x - %s) - EnumTextureFormats()", hr, errormessage))
			ENTER_DEBUGGER;
	}
	return hr;
}

#pragma region wSetTexture
/*

*/
#pragma endregion local info
/// <summary>
/// <c>wSetTexture</c>
/// </summary>
/// <remarks>
/// </remarks>
/// <param name="d3ddevice7"></param>
/// <param name="stage"></param>
/// <param name="ptexture"></param>
/// <returns></returns>
HRESULT __stdcall wSetTexture(
	LPDIRECT3DDEVICE7 d3ddevice7, uint32_t stage, LPDIRECTDRAWSURFACE7 ptexture)
{
	PSTR errormessage;
	HRESULT hr;
	InternalFunctionSpew("GameOS_Direct3D", "SetTexture(%d,0x%x)", stage, ptexture);
	hr = d3ddevice7->SetTexture(stage, ptexture);
	if (FAILED(hr))
	{
		if (hr != DDERR_SURFACELOST)
		{
			errormessage = ErrorNumberToMessage(hr);
			if (InternalFunctionPause("FAILED (0x%x - %s) - SetTexture(%d,0x%x)", hr,
					errormessage, stage, ptexture))
				ENTER_DEBUGGER;
		}
	}
	return hr;
}

#pragma region wSetTextureStageState
/*

*/
#pragma endregion local info
/// <summary>
/// <c>wSetTextureStageState</c>
/// </summary>
/// <remarks>
/// </remarks>
/// <param name="d3ddevice7"></param>
/// <param name="stage"></param>
/// <param name="state"></param>
/// <param name="value"></param>
/// <returns></returns>
HRESULT __stdcall wSetTextureStageState(LPDIRECT3DDEVICE7 d3ddevice7, uint32_t stage,
	D3DTEXTURESTAGESTATETYPE state, uint32_t value)
{
	PSTR pszStageSet;
	PSTR errormessage;
	HRESULT hr;
	++TotalRenderStateChanges;
	pszStageSet = GetStageSet(state, value);
	InternalFunctionSpew("GameOS_Direct3D", "SetTextureStageState(%d,%s)", stage, pszStageSet);
	hr = d3ddevice7->SetTextureStageState(stage, state, value);
	if (FAILED(hr))
	{
		if (hr != DDERR_SURFACELOST)
		{
			pszStageSet = GetStageSet(state, value);
			errormessage = ErrorNumberToMessage(hr);
			if (InternalFunctionPause("FAILED (0x%x - %s) - SetTextureStageState(%d,%s)", hr,
					errormessage, stage, pszStageSet))
				ENTER_DEBUGGER;
		}
	}
	return hr;
}

#pragma region wValidateDevice
/*

*/
#pragma endregion local info
/// <summary>
/// <c>wValidateDevice</c>
/// </summary>
/// <remarks>
/// </remarks>
/// <param name="d3ddevice7"></param>
/// <param name="pExtraPasses"></param>
/// <returns></returns>
HRESULT __stdcall wValidateDevice(LPDIRECT3DDEVICE7 d3ddevice7, uint32_t* pExtraPasses)
{
	PSTR errormessage;
	HRESULT hr;
	InternalFunctionSpew("GameOS_Direct3D", "ValidateDevice()+");
	hr = d3ddevice7->ValidateDevice(pExtraPasses);
	InternalFunctionSpew("GameOS_Direct3D", " -0x%x", *pExtraPasses);
	if (FAILED(hr))
	{
		errormessage = ErrorNumberToMessage(hr);
		if (InternalFunctionPause(
				"FAILED (0x%x - %s) - ValidateDevice()", hr, errormessage))
			ENTER_DEBUGGER;
	}
	return hr;
}

#pragma region wLoad
/*

*/
#pragma endregion local info
/// <summary>
/// <c>wLoad</c>
/// </summary>
/// <remarks>
/// </remarks>
/// <param name="d3ddevice7"></param>
/// <param name="lpDestTex"></param>
/// <param name="lpDestPoint"></param>
/// <param name="lpSrcTex"></param>
/// <param name="lprcSrcRect"></param>
/// <param name="flags"></param>
/// <returns></returns>
HRESULT __stdcall wLoad(LPDIRECT3DDEVICE7 d3ddevice7, LPDIRECTDRAWSURFACE7 lpDestTex,
	LPPOINT lpDestPoint, LPDIRECTDRAWSURFACE7 lpSrcTex, LPRECT lprcSrcRect, uint32_t flags)
{
	PSTR errormessage;
	HRESULT hr;
	InternalFunctionSpew("GameOS_Direct3D", "Load(0x%x with 0x%x)", lpDestTex, lpSrcTex);
	hr = d3ddevice7->Load(lpDestTex, lpDestPoint, lpSrcTex, lprcSrcRect, flags);
	if (FAILED(hr))
	{
		if (hr != DDERR_SURFACELOST)
		{
			errormessage = ErrorNumberToMessage(hr);
			if (InternalFunctionPause("FAILED (0x%x - %s) - Load(0x%x with 0x%x)", hr,
					errormessage, lpDestTex, lpSrcTex))
				ENTER_DEBUGGER;
		}
	}
	return hr;
}

#pragma region wDrawPrimitive
/*

*/
#pragma endregion local info
/// <summary>
/// <c>wDrawPrimitive</c>
/// </summary>
/// <remarks>
/// </remarks>
/// <param name="d3ddevice7"></param>
/// <param name="dptprimitivetype"></param>
/// <param name="dvtvertextype"></param>
/// <param name="pvvertices"></param>
/// <param name="vertexcount"></param>
/// <param name="flags"></param>
/// <returns></returns>
HRESULT __stdcall wDrawPrimitive(LPDIRECT3DDEVICE7 d3ddevice7, D3DPRIMITIVETYPE dptprimitivetype,
	uint32_t dvtvertextype, PVOID pvvertices, uint32_t vertexcount, uint32_t flags)
{
	PSTR primitivedatastring;
	PSTR errormessage;
	HRESULT hr;

	primitivedatastring = GetDrawPrimitiveData(dptprimitivetype, dvtvertextype, pvvertices, vertexcount, flags);
	InternalFunctionSpew("GameOS_Direct3D", "DrawPrimitive( %s)", primitivedatastring);
	hr = d3ddevice7->DrawPrimitive(
		dptprimitivetype, dvtvertextype, pvvertices, vertexcount, flags);
	if (FAILED(hr))
	{
		if (hr != DDERR_SURFACELOST)
		{
			primitivedatastring = GetDrawPrimitiveData(
				dptprimitivetype, dvtvertextype, pvvertices, vertexcount, flags);
			errormessage = ErrorNumberToMessage(hr);
			if (InternalFunctionPause("FAILED (0x%x - %s) - DrawPrimitive( %s)", hr,
					errormessage, primitivedatastring))
				ENTER_DEBUGGER;
		}
	}
	return hr;
}

#pragma region wDrawIndexedPrimitive
/*

*/
#pragma endregion local info
/// <summary>
/// <c>wDrawIndexedPrimitive</c>
/// </summary>
/// <remarks>
/// </remarks>
/// <param name="d3ddevice7"></param>
/// <param name="d3dptprimitivetype"></param>
/// <param name="vertextypedesc"></param>
/// <param name="pvVertices"></param>
/// <param name="vertexcount"></param>
/// <param name="pwindices"></param>
/// <param name="indexcount"></param>
/// <param name="flags"></param>
/// <returns></returns>
HRESULT __stdcall wDrawIndexedPrimitive(LPDIRECT3DDEVICE7 d3ddevice7,
	D3DPRIMITIVETYPE d3dptprimitivetype, uint32_t vertextypedesc, PVOID pvVertices,
	uint32_t vertexcount, uint16_t* pwindices, uint32_t indexcount, uint32_t flags)
{
	PSTR primitivedatastring;
	PSTR errormessage;
	HRESULT hr;
	primitivedatastring = GetDrawIndexedPrimitiveData(d3dptprimitivetype, vertextypedesc, pvVertices,
		vertexcount, pwindices, indexcount, flags);
	InternalFunctionSpew("GameOS_Direct3D", "DrawIndexedPrimitive( %s)", primitivedatastring);
	hr = d3ddevice7->DrawIndexedPrimitive(d3dptprimitivetype, vertextypedesc, pvVertices,
		vertexcount, pwindices, indexcount, flags);
	if (FAILED(hr))
	{
		if (hr != DDERR_SURFACELOST)
		{
			primitivedatastring = GetDrawIndexedPrimitiveData(d3dptprimitivetype, vertextypedesc,
				pvVertices, vertexcount, pwindices, indexcount, flags);
			errormessage = ErrorNumberToMessage(hr);
			if (InternalFunctionPause("FAILED (0x%x - %s) - DrawIndexedPrimitive( %s)", hr,
					errormessage, primitivedatastring))
				ENTER_DEBUGGER;
		}
	}
	return hr;
}

#pragma region wDrawIndexedPrimitiveVB
/*
 */
#pragma endregion local info
/// <summary>
/// <c>wDrawIndexedPrimitiveVB</c>
/// </summary>
/// <remarks>
/// </remarks>
/// <param name="d3ddevice7"></param>
/// <param name="d3dptprimitivetype"></param>
/// <param name="pd3dvertexbuffer"></param>
/// <param name="startvertex"></param>
/// <param name="numvertices"></param>
/// <param name="pwindices"></param>
/// <param name="indexcount"></param>
/// <param name="flags"></param>
/// <returns></returns>
HRESULT __stdcall wDrawIndexedPrimitiveVB(LPDIRECT3DDEVICE7 d3ddevice7,
	D3DPRIMITIVETYPE d3dptprimitivetype, LPDIRECT3DVERTEXBUFFER7 pd3dvertexbuffer,
	uint32_t startvertex, uint32_t numvertices, uint16_t* pwindices, uint32_t indexcount,
	uint32_t flags)
{
	LPDIRECT3DVERTEXBUFFER7 pd3dVertexBufferUsed;
	PSTR errormessage;
	HRESULT hr;

	//InternalFunctionSpew("GameOS_Direct3D", "DrawIndexedPrimitiveVB(0x%x)", pd3dvertexbuffer);

	if (g_pGetDXVB)
	{
		pd3dVertexBufferUsed = g_pGetDXVB(pd3dvertexbuffer, d3ddevice7);
	}
	else
	{
		pd3dVertexBufferUsed = pd3dvertexbuffer;
	}
	hr = d3ddevice7->DrawIndexedPrimitiveVB(d3dptprimitivetype, pd3dVertexBufferUsed,
		startvertex, numvertices, pwindices, indexcount, flags);
	if (FAILED(hr))
	{
		if (hr != DDERR_SURFACELOST)
		{
			errormessage = ErrorNumberToMessage(hr);
			if (InternalFunctionPause("FAILED (0x%x - %s) - DrawIndexedPrimitiveVB(0x%x)", hr,
					errormessage, pd3dvertexbuffer))
				ENTER_DEBUGGER;
		}
	}
	return hr;
}

#pragma region wDrawPrimitiveVB
/*

*/
#pragma endregion local info
/// <summary>
/// <c>wDrawPrimitiveVB</c>
/// </summary>
/// <remarks>
/// </remarks>
/// <param name="d3ddevice7"></param>
/// <param name="d3dptprimitivetype"></param>
/// <param name="pd3dvertexbuffer"></param>
/// <param name="startvertex"></param>
/// <param name="numvertices"></param>
/// <param name="flags"></param>
/// <returns></returns>
HRESULT __stdcall wDrawPrimitiveVB(LPDIRECT3DDEVICE7 d3ddevice7,
	D3DPRIMITIVETYPE d3dptprimitivetype, LPDIRECT3DVERTEXBUFFER7 pd3dvertexbuffer,
	uint32_t startvertex, uint32_t numvertices, uint32_t flags)
{
	LPDIRECT3DVERTEXBUFFER7 pd3dVertexBufferUsed;
	PSTR errormessage;
	HRESULT hr;

	// InternalFunctionSpew("GameOS_Direct3D", "DrawPrimitiveVB(0x%x)", pd3dvertexbuffer);

	if (g_pGetDXVB)
	{
		pd3dVertexBufferUsed = g_pGetDXVB(pd3dvertexbuffer, d3ddevice7);
	}
	else
	{
		pd3dVertexBufferUsed = pd3dvertexbuffer;
	}
	hr = d3ddevice7->DrawPrimitiveVB(
		d3dptprimitivetype, pd3dVertexBufferUsed, startvertex, numvertices, flags);
	if (FAILED(hr))
	{
		if (hr != DDERR_SURFACELOST)
		{
			errormessage = ErrorNumberToMessage(hr);
			if (InternalFunctionPause("FAILED (0x%x - %s) - DrawPrimitiveVB(0x%x)", hr,
					errormessage, pd3dvertexbuffer))
				ENTER_DEBUGGER;
		}
	}
	return hr;
}
