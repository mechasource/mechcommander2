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
		PSTR pszErrorMessage = ErrorNumberToMessage(hr);
		if (InternalFunctionPause("FAILED (0x%x - %s) - Lock(0x%x)", hr, pszErrorMessage, d3dvb7))
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
		PSTR pszErrorMessage = ErrorNumberToMessage(hr);
		if (InternalFunctionPause(
				"FAILED (0x%x - %s) - Unlock(0x%x)", hr, pszErrorMessage, d3dvb7))
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
/// <param name="lpD3DDevice"></param>
/// <param name="flags"></param>
/// <returns></returns>
HRESULT __stdcall wOptimize(
	LPDIRECT3DVERTEXBUFFER7 d3dvb7, LPDIRECT3DDEVICE7 lpD3DDevice, uint32_t flags)
{
	PSTR pszErrorMessage;
	HRESULT hr;
	InternalFunctionSpew("GameOS_Direct3D", "Optimize(0x%x)", d3dvb7);
	hr = d3dvb7->Optimize(lpD3DDevice, flags);
	if (FAILED(hr))
	{
		pszErrorMessage = ErrorNumberToMessage(hr);
		if (InternalFunctionPause(
				"FAILED (0x%x - %s) - Optimize(0x%x)", hr, pszErrorMessage, d3dvb7))
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
/// <param name="dwVertexOp"></param>
/// <param name="dwdestIndex"></param>
/// <param name="count"></param>
/// <param name="lpSrcBuffer"></param>
/// <param name="dwsrcIndex"></param>
/// <param name="lpD3DDevice"></param>
/// <param name="flags"></param>
/// <returns></returns>
HRESULT __stdcall wProcessVertices(LPDIRECT3DVERTEXBUFFER7 d3dvb7, uint32_t dwVertexOp,
	uint32_t dwdestIndex, uint32_t count, LPDIRECT3DVERTEXBUFFER7 lpSrcBuffer,
	uint32_t dwsrcIndex, LPDIRECT3DDEVICE7 lpD3DDevice, uint32_t flags)
{
	PSTR pszErrorMessage;
	HRESULT hr;
	InternalFunctionSpew("GameOS_Direct3D", "ProcessVertices(Dest=0x%x, Src=0x%x, Vertices=%d)",
		d3dvb7, lpSrcBuffer, count);
	hr = d3dvb7->ProcessVertices(
		dwVertexOp, dwdestIndex, count, lpSrcBuffer, dwsrcIndex, lpD3DDevice, flags);
	if (FAILED(hr))
	{
		pszErrorMessage = ErrorNumberToMessage(hr);
		if (InternalFunctionPause("FAILED (0x%x - %s) - "
								  "ProcessVertices(Dest=0x%x, Src=0x%x, "
								  "Vertices=%d)",
				hr, pszErrorMessage, d3dvb7, lpSrcBuffer, count))
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
	PSTR pszErrorMessage;
	HRESULT hr;
	pszDevice = Get3DDevice(rclsid);
	InternalFunctionSpew("GameOS_Direct3D", "CreateDevice(%s from 0x%x)+", pszDevice, pdds);
	hr = d3d7->CreateDevice(rclsid, pdds, ppd3ddevice7);
	InternalFunctionSpew("GameOS_Direct3D", " -0x%x", *ppd3ddevice7);
	if (FAILED(hr))
	{
		pszDevice = Get3DDevice(rclsid);
		pszErrorMessage = ErrorNumberToMessage(hr);
		if (InternalFunctionPause("FAILED (0x%x - %s) - CreateDevice(%s from 0x%x)", hr,
				pszErrorMessage, pszDevice, pdds))
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
	PSTR pszErrorMessage;
	HRESULT hr;
	InternalFunctionSpew("GameOS_Direct3D", "EnumDevices()");
	hr = d3d7->EnumDevices(lpEnumDevicesCallback, puserarg);
	if (FAILED(hr))
	{
		pszErrorMessage = ErrorNumberToMessage(hr);
		if (InternalFunctionPause("FAILED (0x%x - %s) - EnumDevices()", hr, pszErrorMessage))
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
	PSTR pszErrorMessage;
	HRESULT hr;
	pszDevice = Get3DDevice(riidDevice);
	InternalFunctionSpew("GameOS_Direct3D", "EnumZBufferFormats(%s)", pszDevice);
	hr = d3d7->EnumZBufferFormats(riidDevice, penumcallback, pcontext);
	if (FAILED(hr))
	{
		pszDevice = Get3DDevice(riidDevice);
		pszErrorMessage = ErrorNumberToMessage(hr);
		if (InternalFunctionPause(
				"FAILED (0x%x - %s) - EnumZBufferFormats(%s)", hr, pszErrorMessage, pszDevice))
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
/// <param name="lpVBDesc"></param>
/// <param name="lplpD3DVertexBuffer"></param>
/// <param name="flags"></param>
/// <returns></returns>
HRESULT __stdcall wCreateVertexBuffer(LPDIRECT3D7 d3d7, LPD3DVERTEXBUFFERDESC lpVBDesc,
	LPDIRECT3DVERTEXBUFFER7* lplpD3DVertexBuffer, uint32_t flags)
{
	uint32_t dwNumVertices;
	PSTR pszErrorMessage;
	HRESULT hr;
	InternalFunctionSpew(
		"GameOS_Direct3D", "CreateVertexBuffer(%d Vertices)+", lpVBDesc->dwNumVertices);
	hr = d3d7->CreateVertexBuffer(lpVBDesc, lplpD3DVertexBuffer, flags);
	InternalFunctionSpew("GameOS_Direct3D", " -0x%x", *lplpD3DVertexBuffer);
	if (FAILED(hr))
	{
		dwNumVertices = lpVBDesc->dwNumVertices;
		pszErrorMessage = ErrorNumberToMessage(hr);
		if (InternalFunctionPause("FAILED (0x%x - %s) - CreateVertexBuffer(%d Vertices)", hr,
				pszErrorMessage, dwNumVertices))
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
	PSTR pszErrorMessage;
	HRESULT hr;
	InternalFunctionSpew("GameOS_Direct3D", "BeginScene()");
	hr = d3ddevice7->BeginScene();
	if (FAILED(hr))
	{
		if (hr != DDERR_SURFACELOST)
		{
			pszErrorMessage = ErrorNumberToMessage(hr);
			if (InternalFunctionPause(
					"FAILED (0x%x - %s) - BeginScene()", hr, pszErrorMessage))
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
	PSTR pszErrorMessage;
	HRESULT hr;
	InternalFunctionSpew("GameOS_Direct3D", "EndScene()");
	hr = d3ddevice7->EndScene();
	if (FAILED(hr))
	{
		pszErrorMessage = ErrorNumberToMessage(hr);
		if (InternalFunctionPause("FAILED (0x%x - %s) - EndScene()", hr, pszErrorMessage))
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
	PSTR pszErrorMessage;
	HRESULT hr;
	pszTransformType = GetTransformType(dtstTransformStateType);
	InternalFunctionSpew("GameOS_Direct3D", "SetTransform(%s)", pszTransformType);
	hr = d3ddevice7->SetTransform(dtstTransformStateType, pd3dmatrix);
	if (FAILED(hr))
	{
		pszTransformType = GetTransformType(dtstTransformStateType);
		pszErrorMessage = ErrorNumberToMessage(hr);
		if (InternalFunctionPause("FAILED (0x%x - %s) - SetTransform(%s)", hr, pszErrorMessage,
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
	PSTR pszErrorMessage;
	HRESULT hr;
	pszTransformType = GetTransformType(dtstTransformStateType);
	InternalFunctionSpew("GameOS_Direct3D", "MultiplyTransform(%s)", pszTransformType);
	hr = d3ddevice7->MultiplyTransform(dtstTransformStateType, pd3dmatrix);
	if (FAILED(hr))
	{
		pszTransformType = GetTransformType(dtstTransformStateType);
		pszErrorMessage = ErrorNumberToMessage(hr);
		if (InternalFunctionPause("FAILED (0x%x - %s) - MultiplyTransform()", hr,
				pszErrorMessage, pszTransformType))
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
/// <param name="dwLightIndex"></param>
/// <param name="lpLight"></param>
/// <returns></returns>
HRESULT __stdcall wSetLight(
	LPDIRECT3DDEVICE7 d3ddevice7, uint32_t dwLightIndex, LPD3DLIGHT7 lpLight)
{
	PSTR pszLightInfo;
	PSTR pszErrorMessage;
	HRESULT hr;
	pszLightInfo = GetLightInfo(lpLight);
	InternalFunctionSpew("GameOS_Direct3D", "SetLight(%d, %s)", dwLightIndex, pszLightInfo);
	hr = d3ddevice7->SetLight(dwLightIndex, lpLight);
	if (FAILED(hr))
	{
		pszLightInfo = GetLightInfo(lpLight);
		pszErrorMessage = ErrorNumberToMessage(hr);
		if (InternalFunctionPause("FAILED (0x%x - %s) - SetLight(%d, %s)", hr, pszErrorMessage,
				dwLightIndex, pszLightInfo))
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
/// <param name="dwLightIndex"></param>
/// <param name="Enable"></param>
/// <returns></returns>
HRESULT __stdcall wLightEnable(LPDIRECT3DDEVICE7 d3ddevice7, uint32_t dwLightIndex, bool Enable)
{
	PSTR pszEnable;
	PSTR pszErrorMessage;
	HRESULT hr;
	if (Enable)
		pszEnable = "enable";
	else
		pszEnable = "disable";
	InternalFunctionSpew("GameOS_Direct3D", "LightEnable(%d, %s)", dwLightIndex, pszEnable);
	hr = d3ddevice7->LightEnable(dwLightIndex, Enable);
	if (FAILED(hr))
	{
		if (Enable)
			pszEnable = "enable";
		else
			pszEnable = "disable";
		pszErrorMessage = ErrorNumberToMessage(hr);
		if (InternalFunctionPause("FAILED (0x%x - %s) - LightEnable(%d, %s)", hr,
				pszErrorMessage, dwLightIndex, pszEnable))
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
	PSTR pszErrorMessage;
	HRESULT hr;
	pszMaterialInfo = GetMaterialInfo(pMaterial);
	InternalFunctionSpew("GameOS_Direct3D", "SetMaterial(%s)", pszMaterialInfo);
	hr = d3ddevice7->SetMaterial(pMaterial);
	if (FAILED(hr))
	{
		pszMaterialInfo = GetMaterialInfo(pMaterial);
		pszErrorMessage = ErrorNumberToMessage(hr);
		if (InternalFunctionPause(
				"FAILED (0x%x - %s) - SetMaterial(%s)", hr, pszErrorMessage, pszMaterialInfo))
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
	PSTR pszErrorMessage;
	HRESULT hr;
	InternalFunctionSpew("GameOS_Direct3D", "SetViewport()");
	hr = d3ddevice7->SetViewport(pviewport);
	if (FAILED(hr))
	{
		pszErrorMessage = ErrorNumberToMessage(hr);
		if (InternalFunctionPause("FAILED (0x%x - %s) - SetViewport()", hr, pszErrorMessage))
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
	PSTR pszErrorMessage;
	HRESULT hr;
	pszClearArea = GetClearArea(count, pd3drect, flags);
	InternalFunctionSpew("GameOS_Direct3D", "Clear(%s)", pszClearArea);
	hr = d3ddevice7->Clear(count, pd3drect, flags, colour, dvZ, stencil);
	if (FAILED(hr))
	{
		if (hr != DDERR_SURFACELOST)
		{
			pszClearArea = GetClearArea(count, pd3drect, flags);
			pszErrorMessage = ErrorNumberToMessage(hr);
			if (InternalFunctionPause(
					"FAILED (0x%x - %s) - Clear(%s)", hr, pszErrorMessage, pszClearArea))
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
	PSTR pszErrorMessage;
	HRESULT hr;
	InternalFunctionSpew("GameOS_Direct3D", "GetCaps(0x%x)", d3ddevice7);
	hr = d3ddevice7->GetCaps(pd3dhwdevdesc);
	if (FAILED(hr))
	{
		pszErrorMessage = ErrorNumberToMessage(hr);
		if (InternalFunctionPause(
				"FAILED (0x%x - %s) - GetCaps(0x%x)", hr, pszErrorMessage, d3ddevice7))
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
	PSTR pszErrorMessage;
	HRESULT hr;
	++TotalRenderStateChanges;
	pszRenderState = GetRenderState(renderstatetype, renderstate);
	InternalFunctionSpew("GameOS_Direct3D", "SetRenderState(%s)", pszRenderState);
	hr = d3ddevice7->SetRenderState(renderstatetype, renderstate);
	if (FAILED(hr))
	{
		pszRenderState = GetRenderState(renderstatetype, renderstate);
		pszErrorMessage = ErrorNumberToMessage(hr);
		if (InternalFunctionPause("FAILED (0x%x - %s) - SetRenderState(%s)", hr,
				pszErrorMessage, pszRenderState))
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
	PSTR pszErrorMessage;
	HRESULT hr;
	InternalFunctionSpew("GameOS_Direct3D", "EnumTextureFormats()");
	hr = d3ddevice7->EnumTextureFormats(pd3denumpixelproc, parg);
	if (FAILED(hr))
	{
		pszErrorMessage = ErrorNumberToMessage(hr);
		if (InternalFunctionPause(
				"FAILED (0x%x - %s) - EnumTextureFormats()", hr, pszErrorMessage))
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
	PSTR pszErrorMessage;
	HRESULT hr;
	InternalFunctionSpew("GameOS_Direct3D", "SetTexture(%d,0x%x)", stage, ptexture);
	hr = d3ddevice7->SetTexture(stage, ptexture);
	if (FAILED(hr))
	{
		if (hr != DDERR_SURFACELOST)
		{
			pszErrorMessage = ErrorNumberToMessage(hr);
			if (InternalFunctionPause("FAILED (0x%x - %s) - SetTexture(%d,0x%x)", hr,
					pszErrorMessage, stage, ptexture))
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
	PSTR pszErrorMessage;
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
			pszErrorMessage = ErrorNumberToMessage(hr);
			if (InternalFunctionPause("FAILED (0x%x - %s) - SetTextureStageState(%d,%s)", hr,
					pszErrorMessage, stage, pszStageSet))
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
	PSTR pszErrorMessage;
	HRESULT hr;
	InternalFunctionSpew("GameOS_Direct3D", "ValidateDevice()+");
	hr = d3ddevice7->ValidateDevice(pExtraPasses);
	InternalFunctionSpew("GameOS_Direct3D", " -0x%x", *pExtraPasses);
	if (FAILED(hr))
	{
		pszErrorMessage = ErrorNumberToMessage(hr);
		if (InternalFunctionPause(
				"FAILED (0x%x - %s) - ValidateDevice()", hr, pszErrorMessage))
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
	PSTR pszErrorMessage;
	HRESULT hr;
	InternalFunctionSpew("GameOS_Direct3D", "Load(0x%x with 0x%x)", lpDestTex, lpSrcTex);
	hr = d3ddevice7->Load(lpDestTex, lpDestPoint, lpSrcTex, lprcSrcRect, flags);
	if (FAILED(hr))
	{
		if (hr != DDERR_SURFACELOST)
		{
			pszErrorMessage = ErrorNumberToMessage(hr);
			if (InternalFunctionPause("FAILED (0x%x - %s) - Load(0x%x with 0x%x)", hr,
					pszErrorMessage, lpDestTex, lpSrcTex))
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
/// <param name="dptPrimitiveType"></param>
/// <param name="dvtVertexType"></param>
/// <param name="lpvVertices"></param>
/// <param name="dwVertexCount"></param>
/// <param name="flags"></param>
/// <returns></returns>
HRESULT __stdcall wDrawPrimitive(LPDIRECT3DDEVICE7 d3ddevice7, D3DPRIMITIVETYPE dptPrimitiveType,
	uint32_t dvtVertexType, PVOID lpvVertices, uint32_t dwVertexCount, uint32_t flags)
{
	PSTR pszPrimitiveData;
	PSTR pszErrorMessage;
	HRESULT hr;
	pszPrimitiveData =
		GetDrawPrimitiveData(dptPrimitiveType, dvtVertexType, lpvVertices, dwVertexCount, flags);
	InternalFunctionSpew("GameOS_Direct3D", "DrawPrimitive( %s)", pszPrimitiveData);
	hr = d3ddevice7->DrawPrimitive(
		dptPrimitiveType, dvtVertexType, lpvVertices, dwVertexCount, flags);
	if (FAILED(hr))
	{
		if (hr != DDERR_SURFACELOST)
		{
			pszPrimitiveData = GetDrawPrimitiveData(
				dptPrimitiveType, dvtVertexType, lpvVertices, dwVertexCount, flags);
			pszErrorMessage = ErrorNumberToMessage(hr);
			if (InternalFunctionPause("FAILED (0x%x - %s) - DrawPrimitive( %s)", hr,
					pszErrorMessage, pszPrimitiveData))
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
/// <param name="d3dptPrimitiveType"></param>
/// <param name="dwVertexTypeDesc"></param>
/// <param name="pvVertices"></param>
/// <param name="dwVertexCount"></param>
/// <param name="pwIndices"></param>
/// <param name="dwIndexCount"></param>
/// <param name="flags"></param>
/// <returns></returns>
HRESULT __stdcall wDrawIndexedPrimitive(LPDIRECT3DDEVICE7 d3ddevice7,
	D3DPRIMITIVETYPE d3dptPrimitiveType, uint32_t dwVertexTypeDesc, PVOID pvVertices,
	uint32_t dwVertexCount, uint16_t* pwIndices, uint32_t dwIndexCount, uint32_t flags)
{
	PSTR pszPrimitiveData;
	PSTR pszErrorMessage;
	HRESULT hr;
	pszPrimitiveData = GetDrawIndexedPrimitiveData(d3dptPrimitiveType, dwVertexTypeDesc, pvVertices,
		dwVertexCount, pwIndices, dwIndexCount, flags);
	InternalFunctionSpew("GameOS_Direct3D", "DrawIndexedPrimitive( %s)", pszPrimitiveData);
	hr = d3ddevice7->DrawIndexedPrimitive(d3dptPrimitiveType, dwVertexTypeDesc, pvVertices,
		dwVertexCount, pwIndices, dwIndexCount, flags);
	if (FAILED(hr))
	{
		if (hr != DDERR_SURFACELOST)
		{
			pszPrimitiveData = GetDrawIndexedPrimitiveData(d3dptPrimitiveType, dwVertexTypeDesc,
				pvVertices, dwVertexCount, pwIndices, dwIndexCount, flags);
			pszErrorMessage = ErrorNumberToMessage(hr);
			if (InternalFunctionPause("FAILED (0x%x - %s) - DrawIndexedPrimitive( %s)", hr,
					pszErrorMessage, pszPrimitiveData))
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
/// <param name="d3dptPrimitiveType"></param>
/// <param name="lpd3dVertexBuffer"></param>
/// <param name="dwStartVertex"></param>
/// <param name="dwNumVertices"></param>
/// <param name="lpwIndices"></param>
/// <param name="dwIndexCount"></param>
/// <param name="flags"></param>
/// <returns></returns>
HRESULT __stdcall wDrawIndexedPrimitiveVB(LPDIRECT3DDEVICE7 d3ddevice7,
	D3DPRIMITIVETYPE d3dptPrimitiveType, LPDIRECT3DVERTEXBUFFER7 lpd3dVertexBuffer,
	uint32_t dwStartVertex, uint32_t dwNumVertices, uint16_t* lpwIndices, uint32_t dwIndexCount,
	uint32_t flags)
{
	LPDIRECT3DVERTEXBUFFER7 pd3dVertexBufferUsed;
	PSTR pszErrorMessage;
	HRESULT hr;

	//InternalFunctionSpew("GameOS_Direct3D", "DrawIndexedPrimitiveVB(0x%x)", lpd3dVertexBuffer);

	if (g_pGetDXVB)
	{
		pd3dVertexBufferUsed = g_pGetDXVB(lpd3dVertexBuffer, d3ddevice7);
	}
	else
	{
		pd3dVertexBufferUsed = lpd3dVertexBuffer;
	}
	hr = d3ddevice7->DrawIndexedPrimitiveVB(d3dptPrimitiveType, pd3dVertexBufferUsed,
		dwStartVertex, dwNumVertices, lpwIndices, dwIndexCount, flags);
	if (FAILED(hr))
	{
		if (hr != DDERR_SURFACELOST)
		{
			pszErrorMessage = ErrorNumberToMessage(hr);
			if (InternalFunctionPause("FAILED (0x%x - %s) - DrawIndexedPrimitiveVB(0x%x)", hr,
					pszErrorMessage, lpd3dVertexBuffer))
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
/// <param name="d3dptPrimitiveType"></param>
/// <param name="lpd3dVertexBuffer"></param>
/// <param name="dwStartVertex"></param>
/// <param name="dwNumVertices"></param>
/// <param name="flags"></param>
/// <returns></returns>
HRESULT __stdcall wDrawPrimitiveVB(LPDIRECT3DDEVICE7 d3ddevice7,
	D3DPRIMITIVETYPE d3dptPrimitiveType, LPDIRECT3DVERTEXBUFFER7 lpd3dVertexBuffer,
	uint32_t dwStartVertex, uint32_t dwNumVertices, uint32_t flags)
{
	LPDIRECT3DVERTEXBUFFER7 pd3dVertexBufferUsed;
	PSTR pszErrorMessage;
	HRESULT hr;

	// InternalFunctionSpew("GameOS_Direct3D", "DrawPrimitiveVB(0x%x)", lpd3dVertexBuffer);

	if (g_pGetDXVB)
	{
		pd3dVertexBufferUsed = g_pGetDXVB(lpd3dVertexBuffer, d3ddevice7);
	}
	else
	{
		pd3dVertexBufferUsed = lpd3dVertexBuffer;
	}
	hr = d3ddevice7->DrawPrimitiveVB(
		d3dptPrimitiveType, pd3dVertexBufferUsed, dwStartVertex, dwNumVertices, flags);
	if (FAILED(hr))
	{
		if (hr != DDERR_SURFACELOST)
		{
			pszErrorMessage = ErrorNumberToMessage(hr);
			if (InternalFunctionPause("FAILED (0x%x - %s) - DrawPrimitiveVB(0x%x)", hr,
					pszErrorMessage, lpd3dVertexBuffer))
				ENTER_DEBUGGER;
		}
	}
	return hr;
}
