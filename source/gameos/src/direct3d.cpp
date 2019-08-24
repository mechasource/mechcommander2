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

 2014-07-24 Jerker Beck, created

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
/// <param name="D3Dvb7"></param>
/// <param name="dwFlags"></param>
/// <param name="lplpData"></param>
/// <param name="lpdwSize"></param>
/// <returns></returns>
HRESULT __stdcall wLock(
	LPDIRECT3DVERTEXBUFFER7 D3Dvb7, uint32_t dwFlags, PVOID* lplpData, puint32_t lpdwSize)
{

	// InternalFunctionSpew("GameOS_Direct3D", "Lock(0x%x)", D3Dvb7);

	HRESULT hResult = D3Dvb7->Lock(dwFlags, lplpData, lpdwSize);
	if (FAILED(hResult))
	{
		PSTR pszErrorMessage = ErrorNumberToMessage(hResult);
		if (InternalFunctionPause("FAILED (0x%x - %s) - Lock(0x%x)", hResult, pszErrorMessage, D3Dvb7))
			ENTER_DEBUGGER;
	}
	return hResult;
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
/// <param name="D3Dvb7"></param>
/// <returns></returns>
HRESULT __stdcall wUnlock(LPDIRECT3DVERTEXBUFFER7 D3Dvb7)
{
	// InternalFunctionSpew("GameOS_Direct3D", "Unlock(0x%x)", D3Dvb7);

	HRESULT hResult = D3Dvb7->Unlock();
	if (FAILED(hResult))
	{
		PSTR pszErrorMessage = ErrorNumberToMessage(hResult);
		if (InternalFunctionPause(
				"FAILED (0x%x - %s) - Unlock(0x%x)", hResult, pszErrorMessage, D3Dvb7))
			ENTER_DEBUGGER;
	}
	return hResult;
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
/// <param name="D3Dvb7"></param>
/// <param name="lpD3DDevice"></param>
/// <param name="dwFlags"></param>
/// <returns></returns>
HRESULT __stdcall wOptimize(
	LPDIRECT3DVERTEXBUFFER7 D3Dvb7, LPDIRECT3DDEVICE7 lpD3DDevice, uint32_t dwFlags)
{
	PSTR pszErrorMessage;
	HRESULT hResult;
	InternalFunctionSpew("GameOS_Direct3D", "Optimize(0x%x)", D3Dvb7);
	hResult = D3Dvb7->Optimize(lpD3DDevice, dwFlags);
	if (FAILED(hResult))
	{
		pszErrorMessage = ErrorNumberToMessage(hResult);
		if (InternalFunctionPause(
				"FAILED (0x%x - %s) - Optimize(0x%x)", hResult, pszErrorMessage, D3Dvb7))
			ENTER_DEBUGGER;
	}
	return hResult;
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
/// <param name="D3Dvb7"></param>
/// <param name="dwVertexOp"></param>
/// <param name="dwDestIndex"></param>
/// <param name="dwCount"></param>
/// <param name="lpSrcBuffer"></param>
/// <param name="dwSrcIndex"></param>
/// <param name="lpD3DDevice"></param>
/// <param name="dwFlags"></param>
/// <returns></returns>
HRESULT __stdcall wProcessVertices(LPDIRECT3DVERTEXBUFFER7 D3Dvb7, uint32_t dwVertexOp,
	uint32_t dwDestIndex, uint32_t dwCount, LPDIRECT3DVERTEXBUFFER7 lpSrcBuffer,
	uint32_t dwSrcIndex, LPDIRECT3DDEVICE7 lpD3DDevice, uint32_t dwFlags)
{
	PSTR pszErrorMessage;
	HRESULT hResult;
	InternalFunctionSpew("GameOS_Direct3D", "ProcessVertices(Dest=0x%x, Src=0x%x, Vertices=%d)",
		D3Dvb7, lpSrcBuffer, dwCount);
	hResult = D3Dvb7->ProcessVertices(
		dwVertexOp, dwDestIndex, dwCount, lpSrcBuffer, dwSrcIndex, lpD3DDevice, dwFlags);
	if (FAILED(hResult))
	{
		pszErrorMessage = ErrorNumberToMessage(hResult);
		if (InternalFunctionPause("FAILED (0x%x - %s) - "
								  "ProcessVertices(Dest=0x%x, Src=0x%x, "
								  "Vertices=%d)",
				hResult, pszErrorMessage, D3Dvb7, lpSrcBuffer, dwCount))
			ENTER_DEBUGGER;
	}
	return hResult;
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
/// <param name="lpDDS"></param>
/// <param name="lplpD3DDevice7"></param>
/// <returns></returns>
HRESULT __stdcall wCreateDevice(LPDIRECT3D7 d3d7, REFCLSID rclsid, LPDIRECTDRAWSURFACE7 lpDDS,
	LPDIRECT3DDEVICE7* lplpD3DDevice7)
{
	PSTR pszDevice;
	PSTR pszErrorMessage;
	HRESULT hResult;
	pszDevice = Get3DDevice(rclsid);
	InternalFunctionSpew("GameOS_Direct3D", "CreateDevice(%s from 0x%x)+", pszDevice, lpDDS);
	hResult = d3d7->CreateDevice(rclsid, lpDDS, lplpD3DDevice7);
	InternalFunctionSpew("GameOS_Direct3D", " -0x%x", *lplpD3DDevice7);
	if (FAILED(hResult))
	{
		pszDevice = Get3DDevice(rclsid);
		pszErrorMessage = ErrorNumberToMessage(hResult);
		if (InternalFunctionPause("FAILED (0x%x - %s) - CreateDevice(%s from 0x%x)", hResult,
				pszErrorMessage, pszDevice, lpDDS))
			ENTER_DEBUGGER;
	}
	return hResult;
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
/// <param name="lpUserArg"></param>
/// <returns></returns>
HRESULT __stdcall wEnumDevices(
	LPDIRECT3D7 d3d7, LPD3DENUMDEVICESCALLBACK7 lpEnumDevicesCallback, PVOID lpUserArg)
{
	PSTR pszErrorMessage;
	HRESULT hResult;
	InternalFunctionSpew("GameOS_Direct3D", "EnumDevices()");
	hResult = d3d7->EnumDevices(lpEnumDevicesCallback, lpUserArg);
	if (FAILED(hResult))
	{
		pszErrorMessage = ErrorNumberToMessage(hResult);
		if (InternalFunctionPause("FAILED (0x%x - %s) - EnumDevices()", hResult, pszErrorMessage))
			ENTER_DEBUGGER;
	}
	return hResult;
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
/// <param name="lpEnumCallback"></param>
/// <param name="lpContext"></param>
/// <returns></returns>
HRESULT __stdcall wEnumZBufferFormats(LPDIRECT3D7 d3d7, REFCLSID riidDevice,
	LPD3DENUMPIXELFORMATSCALLBACK lpEnumCallback, PVOID lpContext)
{
	PSTR pszDevice;
	PSTR pszErrorMessage;
	HRESULT hResult;
	pszDevice = Get3DDevice(riidDevice);
	InternalFunctionSpew("GameOS_Direct3D", "EnumZBufferFormats(%s)", pszDevice);
	hResult = d3d7->EnumZBufferFormats(riidDevice, lpEnumCallback, lpContext);
	if (FAILED(hResult))
	{
		pszDevice = Get3DDevice(riidDevice);
		pszErrorMessage = ErrorNumberToMessage(hResult);
		if (InternalFunctionPause(
				"FAILED (0x%x - %s) - EnumZBufferFormats(%s)", hResult, pszErrorMessage, pszDevice))
			ENTER_DEBUGGER;
	}
	return hResult;
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
/// <param name="dwFlags"></param>
/// <returns></returns>
HRESULT __stdcall wCreateVertexBuffer(LPDIRECT3D7 d3d7, LPD3DVERTEXBUFFERDESC lpVBDesc,
	LPDIRECT3DVERTEXBUFFER7* lplpD3DVertexBuffer, uint32_t dwFlags)
{
	uint32_t dwNumVertices;
	PSTR pszErrorMessage;
	HRESULT hResult;
	InternalFunctionSpew(
		"GameOS_Direct3D", "CreateVertexBuffer(%d Vertices)+", lpVBDesc->dwNumVertices);
	hResult = d3d7->CreateVertexBuffer(lpVBDesc, lplpD3DVertexBuffer, dwFlags);
	InternalFunctionSpew("GameOS_Direct3D", " -0x%x", *lplpD3DVertexBuffer);
	if (FAILED(hResult))
	{
		dwNumVertices = lpVBDesc->dwNumVertices;
		pszErrorMessage = ErrorNumberToMessage(hResult);
		if (InternalFunctionPause("FAILED (0x%x - %s) - CreateVertexBuffer(%d Vertices)", hResult,
				pszErrorMessage, dwNumVertices))
			ENTER_DEBUGGER;
	}
	return hResult;
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
/// <param name="d3dDevice7"></param>
/// <returns></returns>
HRESULT __stdcall wBeginScene(LPDIRECT3DDEVICE7 d3dDevice7)
{
	PSTR pszErrorMessage;
	HRESULT hResult;
	InternalFunctionSpew("GameOS_Direct3D", "BeginScene()");
	hResult = d3dDevice7->BeginScene();
	if (FAILED(hResult))
	{
		if (hResult != DDERR_SURFACELOST)
		{
			pszErrorMessage = ErrorNumberToMessage(hResult);
			if (InternalFunctionPause(
					"FAILED (0x%x - %s) - BeginScene()", hResult, pszErrorMessage))
				ENTER_DEBUGGER;
		}
	}
	return hResult;
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
/// <param name="d3dDevice7"></param>
/// <returns></returns>
HRESULT __stdcall wEndScene(LPDIRECT3DDEVICE7 d3dDevice7)
{
	PSTR pszErrorMessage;
	HRESULT hResult;
	InternalFunctionSpew("GameOS_Direct3D", "EndScene()");
	hResult = d3dDevice7->EndScene();
	if (FAILED(hResult))
	{
		pszErrorMessage = ErrorNumberToMessage(hResult);
		if (InternalFunctionPause("FAILED (0x%x - %s) - EndScene()", hResult, pszErrorMessage))
			ENTER_DEBUGGER;
	}
	return hResult;
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
/// <param name="d3dDevice7"></param>
/// <param name="dtstTransformStateType"></param>
/// <param name="lpD3DMatrix"></param>
/// <returns></returns>
HRESULT __stdcall wSetTransform(LPDIRECT3DDEVICE7 d3dDevice7,
	D3DTRANSFORMSTATETYPE dtstTransformStateType, LPD3DMATRIX lpD3DMatrix)
{
	PSTR pszTransformType;
	PSTR pszErrorMessage;
	HRESULT hResult;
	pszTransformType = GetTransformType(dtstTransformStateType);
	InternalFunctionSpew("GameOS_Direct3D", "SetTransform(%s)", pszTransformType);
	hResult = d3dDevice7->SetTransform(dtstTransformStateType, lpD3DMatrix);
	if (FAILED(hResult))
	{
		pszTransformType = GetTransformType(dtstTransformStateType);
		pszErrorMessage = ErrorNumberToMessage(hResult);
		if (InternalFunctionPause("FAILED (0x%x - %s) - SetTransform(%s)", hResult, pszErrorMessage,
				pszTransformType))
			ENTER_DEBUGGER;
	}
	return hResult;
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
/// <param name="d3dDevice7"></param>
/// <param name="dtstTransformStateType"></param>
/// <param name="lpD3DMatrix"></param>
/// <returns></returns>
HRESULT __stdcall wMultiplyTransform(LPDIRECT3DDEVICE7 d3dDevice7,
	D3DTRANSFORMSTATETYPE dtstTransformStateType, LPD3DMATRIX lpD3DMatrix)
{
	PSTR pszTransformType;
	PSTR pszErrorMessage;
	HRESULT hResult;
	pszTransformType = GetTransformType(dtstTransformStateType);
	InternalFunctionSpew("GameOS_Direct3D", "MultiplyTransform(%s)", pszTransformType);
	hResult = d3dDevice7->MultiplyTransform(dtstTransformStateType, lpD3DMatrix);
	if (FAILED(hResult))
	{
		pszTransformType = GetTransformType(dtstTransformStateType);
		pszErrorMessage = ErrorNumberToMessage(hResult);
		if (InternalFunctionPause("FAILED (0x%x - %s) - MultiplyTransform()", hResult,
				pszErrorMessage, pszTransformType))
			ENTER_DEBUGGER;
	}
	return hResult;
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
/// <param name="d3dDevice7"></param>
/// <param name="dwLightIndex"></param>
/// <param name="lpLight"></param>
/// <returns></returns>
HRESULT __stdcall wSetLight(
	LPDIRECT3DDEVICE7 d3dDevice7, uint32_t dwLightIndex, LPD3DLIGHT7 lpLight)
{
	PSTR pszLightInfo;
	PSTR pszErrorMessage;
	HRESULT hResult;
	pszLightInfo = GetLightInfo(lpLight);
	InternalFunctionSpew("GameOS_Direct3D", "SetLight(%d, %s)", dwLightIndex, pszLightInfo);
	hResult = d3dDevice7->SetLight(dwLightIndex, lpLight);
	if (FAILED(hResult))
	{
		pszLightInfo = GetLightInfo(lpLight);
		pszErrorMessage = ErrorNumberToMessage(hResult);
		if (InternalFunctionPause("FAILED (0x%x - %s) - SetLight(%d, %s)", hResult, pszErrorMessage,
				dwLightIndex, pszLightInfo))
			ENTER_DEBUGGER;
	}
	return hResult;
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
/// <param name="d3dDevice7"></param>
/// <param name="dwLightIndex"></param>
/// <param name="Enable"></param>
/// <returns></returns>
HRESULT __stdcall wLightEnable(LPDIRECT3DDEVICE7 d3dDevice7, uint32_t dwLightIndex, bool Enable)
{
	PSTR pszEnable;
	PSTR pszErrorMessage;
	HRESULT hResult;
	if (Enable)
		pszEnable = "enable";
	else
		pszEnable = "disable";
	InternalFunctionSpew("GameOS_Direct3D", "LightEnable(%d, %s)", dwLightIndex, pszEnable);
	hResult = d3dDevice7->LightEnable(dwLightIndex, Enable);
	if (FAILED(hResult))
	{
		if (Enable)
			pszEnable = "enable";
		else
			pszEnable = "disable";
		pszErrorMessage = ErrorNumberToMessage(hResult);
		if (InternalFunctionPause("FAILED (0x%x - %s) - LightEnable(%d, %s)", hResult,
				pszErrorMessage, dwLightIndex, pszEnable))
			ENTER_DEBUGGER;
	}
	return hResult;
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
/// <param name="d3dDevice7"></param>
/// <param name="pMaterial"></param>
/// <returns></returns>
HRESULT __stdcall wSetMaterial(LPDIRECT3DDEVICE7 d3dDevice7, LPD3DMATERIAL7 pMaterial)
{
	PSTR pszMaterialInfo;
	PSTR pszErrorMessage;
	HRESULT hResult;
	pszMaterialInfo = GetMaterialInfo(pMaterial);
	InternalFunctionSpew("GameOS_Direct3D", "SetMaterial(%s)", pszMaterialInfo);
	hResult = d3dDevice7->SetMaterial(pMaterial);
	if (FAILED(hResult))
	{
		pszMaterialInfo = GetMaterialInfo(pMaterial);
		pszErrorMessage = ErrorNumberToMessage(hResult);
		if (InternalFunctionPause(
				"FAILED (0x%x - %s) - SetMaterial(%s)", hResult, pszErrorMessage, pszMaterialInfo))
			ENTER_DEBUGGER;
	}
	return hResult;
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
/// <param name="d3dDevice7"></param>
/// <param name="lpViewport"></param>
/// <returns></returns>
HRESULT __stdcall wSetViewport(LPDIRECT3DDEVICE7 d3dDevice7, LPD3DVIEWPORT7 lpViewport)
{
	PSTR pszErrorMessage;
	HRESULT hResult;
	InternalFunctionSpew("GameOS_Direct3D", "SetViewport()");
	hResult = d3dDevice7->SetViewport(lpViewport);
	if (FAILED(hResult))
	{
		pszErrorMessage = ErrorNumberToMessage(hResult);
		if (InternalFunctionPause("FAILED (0x%x - %s) - SetViewport()", hResult, pszErrorMessage))
			ENTER_DEBUGGER;
	}
	return hResult;
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
/// <param name="d3dDevice7"></param>
/// <param name="dwCount"></param>
/// <param name="lpRects"></param>
/// <param name="dwFlags"></param>
/// <param name="dwColor"></param>
/// <param name="dvZ"></param>
/// <param name="dwStencil"></param>
/// <returns></returns>
HRESULT __stdcall wClear(LPDIRECT3DDEVICE7 d3dDevice7, uint32_t dwCount, LPD3DRECT lpRects,
	uint32_t dwFlags, uint32_t dwColor, D3DVALUE dvZ, uint32_t dwStencil)
{
	PSTR pszClearArea;
	PSTR pszErrorMessage;
	HRESULT hResult;
	pszClearArea = GetClearArea(dwCount, lpRects, dwFlags);
	InternalFunctionSpew("GameOS_Direct3D", "Clear(%s)", pszClearArea);
	hResult = d3dDevice7->Clear(dwCount, lpRects, dwFlags, dwColor, dvZ, dwStencil);
	if (FAILED(hResult))
	{
		if (hResult != DDERR_SURFACELOST)
		{
			pszClearArea = GetClearArea(dwCount, lpRects, dwFlags);
			pszErrorMessage = ErrorNumberToMessage(hResult);
			if (InternalFunctionPause(
					"FAILED (0x%x - %s) - Clear(%s)", hResult, pszErrorMessage, pszClearArea))
				ENTER_DEBUGGER;
		}
	}
	return hResult;
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
/// <param name="d3dDevice7"></param>
/// <param name="lpD3DHWDevDesc"></param>
/// <returns></returns>
HRESULT __stdcall wGetCaps(LPDIRECT3DDEVICE7 d3dDevice7, LPD3DDEVICEDESC7 lpD3DHWDevDesc)
{
	PSTR pszErrorMessage;
	HRESULT hResult;
	InternalFunctionSpew("GameOS_Direct3D", "GetCaps(0x%x)", d3dDevice7);
	hResult = d3dDevice7->GetCaps(lpD3DHWDevDesc);
	if (FAILED(hResult))
	{
		pszErrorMessage = ErrorNumberToMessage(hResult);
		if (InternalFunctionPause(
				"FAILED (0x%x - %s) - GetCaps(0x%x)", hResult, pszErrorMessage, d3dDevice7))
			ENTER_DEBUGGER;
	}
	return hResult;
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
/// <param name="d3dDevice7"></param>
/// <param name="dwRenderStateType"></param>
/// <param name="dwRenderState"></param>
/// <returns></returns>
HRESULT __stdcall wSetRenderState(
	LPDIRECT3DDEVICE7 d3dDevice7, D3DRENDERSTATETYPE dwRenderStateType, uint32_t dwRenderState)
{
	PSTR pszRenderState;
	PSTR pszErrorMessage;
	HRESULT hResult;
	++TotalRenderStateChanges;
	pszRenderState = GetRenderState(dwRenderStateType, dwRenderState);
	InternalFunctionSpew("GameOS_Direct3D", "SetRenderState(%s)", pszRenderState);
	hResult = d3dDevice7->SetRenderState(dwRenderStateType, dwRenderState);
	if (FAILED(hResult))
	{
		pszRenderState = GetRenderState(dwRenderStateType, dwRenderState);
		pszErrorMessage = ErrorNumberToMessage(hResult);
		if (InternalFunctionPause("FAILED (0x%x - %s) - SetRenderState(%s)", hResult,
				pszErrorMessage, pszRenderState))
			ENTER_DEBUGGER;
	}
	return hResult;
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
/// <param name="d3dDevice7"></param>
/// <param name="lpd3dEnumPixelProc"></param>
/// <param name="lpArg"></param>
/// <returns></returns>
HRESULT __stdcall wEnumTextureFormats(
	LPDIRECT3DDEVICE7 d3dDevice7, LPD3DENUMPIXELFORMATSCALLBACK lpd3dEnumPixelProc, PVOID lpArg)
{
	PSTR pszErrorMessage;
	HRESULT hResult;
	InternalFunctionSpew("GameOS_Direct3D", "EnumTextureFormats()");
	hResult = d3dDevice7->EnumTextureFormats(lpd3dEnumPixelProc, lpArg);
	if (FAILED(hResult))
	{
		pszErrorMessage = ErrorNumberToMessage(hResult);
		if (InternalFunctionPause(
				"FAILED (0x%x - %s) - EnumTextureFormats()", hResult, pszErrorMessage))
			ENTER_DEBUGGER;
	}
	return hResult;
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
/// <param name="d3dDevice7"></param>
/// <param name="dwStage"></param>
/// <param name="lpTexture"></param>
/// <returns></returns>
HRESULT __stdcall wSetTexture(
	LPDIRECT3DDEVICE7 d3dDevice7, uint32_t dwStage, LPDIRECTDRAWSURFACE7 lpTexture)
{
	PSTR pszErrorMessage;
	HRESULT hResult;
	InternalFunctionSpew("GameOS_Direct3D", "SetTexture(%d,0x%x)", dwStage, lpTexture);
	hResult = d3dDevice7->SetTexture(dwStage, lpTexture);
	if (FAILED(hResult))
	{
		if (hResult != DDERR_SURFACELOST)
		{
			pszErrorMessage = ErrorNumberToMessage(hResult);
			if (InternalFunctionPause("FAILED (0x%x - %s) - SetTexture(%d,0x%x)", hResult,
					pszErrorMessage, dwStage, lpTexture))
				ENTER_DEBUGGER;
		}
	}
	return hResult;
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
/// <param name="d3dDevice7"></param>
/// <param name="dwStage"></param>
/// <param name="dwState"></param>
/// <param name="dwValue"></param>
/// <returns></returns>
HRESULT __stdcall wSetTextureStageState(LPDIRECT3DDEVICE7 d3dDevice7, uint32_t dwStage,
	D3DTEXTURESTAGESTATETYPE dwState, uint32_t dwValue)
{
	PSTR pszStageSet;
	PSTR pszErrorMessage;
	HRESULT hResult;
	++TotalRenderStateChanges;
	pszStageSet = GetStageSet(dwState, dwValue);
	InternalFunctionSpew("GameOS_Direct3D", "SetTextureStageState(%d,%s)", dwStage, pszStageSet);
	hResult = d3dDevice7->SetTextureStageState(dwStage, dwState, dwValue);
	if (FAILED(hResult))
	{
		if (hResult != DDERR_SURFACELOST)
		{
			pszStageSet = GetStageSet(dwState, dwValue);
			pszErrorMessage = ErrorNumberToMessage(hResult);
			if (InternalFunctionPause("FAILED (0x%x - %s) - SetTextureStageState(%d,%s)", hResult,
					pszErrorMessage, dwStage, pszStageSet))
				ENTER_DEBUGGER;
		}
	}
	return hResult;
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
/// <param name="d3dDevice7"></param>
/// <param name="pExtraPasses"></param>
/// <returns></returns>
HRESULT __stdcall wValidateDevice(LPDIRECT3DDEVICE7 d3dDevice7, puint32_t pExtraPasses)
{
	PSTR pszErrorMessage;
	HRESULT hResult;
	InternalFunctionSpew("GameOS_Direct3D", "ValidateDevice()+");
	hResult = d3dDevice7->ValidateDevice(pExtraPasses);
	InternalFunctionSpew("GameOS_Direct3D", " -0x%x", *pExtraPasses);
	if (FAILED(hResult))
	{
		pszErrorMessage = ErrorNumberToMessage(hResult);
		if (InternalFunctionPause(
				"FAILED (0x%x - %s) - ValidateDevice()", hResult, pszErrorMessage))
			ENTER_DEBUGGER;
	}
	return hResult;
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
/// <param name="d3dDevice7"></param>
/// <param name="lpDestTex"></param>
/// <param name="lpDestPoint"></param>
/// <param name="lpSrcTex"></param>
/// <param name="lprcSrcRect"></param>
/// <param name="dwFlags"></param>
/// <returns></returns>
HRESULT __stdcall wLoad(LPDIRECT3DDEVICE7 d3dDevice7, LPDIRECTDRAWSURFACE7 lpDestTex,
	LPPOINT lpDestPoint, LPDIRECTDRAWSURFACE7 lpSrcTex, LPRECT lprcSrcRect, uint32_t dwFlags)
{
	PSTR pszErrorMessage;
	HRESULT hResult;
	InternalFunctionSpew("GameOS_Direct3D", "Load(0x%x with 0x%x)", lpDestTex, lpSrcTex);
	hResult = d3dDevice7->Load(lpDestTex, lpDestPoint, lpSrcTex, lprcSrcRect, dwFlags);
	if (FAILED(hResult))
	{
		if (hResult != DDERR_SURFACELOST)
		{
			pszErrorMessage = ErrorNumberToMessage(hResult);
			if (InternalFunctionPause("FAILED (0x%x - %s) - Load(0x%x with 0x%x)", hResult,
					pszErrorMessage, lpDestTex, lpSrcTex))
				ENTER_DEBUGGER;
		}
	}
	return hResult;
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
/// <param name="d3dDevice7"></param>
/// <param name="dptPrimitiveType"></param>
/// <param name="dvtVertexType"></param>
/// <param name="lpvVertices"></param>
/// <param name="dwVertexCount"></param>
/// <param name="dwFlags"></param>
/// <returns></returns>
HRESULT __stdcall wDrawPrimitive(LPDIRECT3DDEVICE7 d3dDevice7, D3DPRIMITIVETYPE dptPrimitiveType,
	uint32_t dvtVertexType, PVOID lpvVertices, uint32_t dwVertexCount, uint32_t dwFlags)
{
	PSTR pszPrimitiveData;
	PSTR pszErrorMessage;
	HRESULT hResult;
	pszPrimitiveData =
		GetDrawPrimitiveData(dptPrimitiveType, dvtVertexType, lpvVertices, dwVertexCount, dwFlags);
	InternalFunctionSpew("GameOS_Direct3D", "DrawPrimitive( %s)", pszPrimitiveData);
	hResult = d3dDevice7->DrawPrimitive(
		dptPrimitiveType, dvtVertexType, lpvVertices, dwVertexCount, dwFlags);
	if (FAILED(hResult))
	{
		if (hResult != DDERR_SURFACELOST)
		{
			pszPrimitiveData = GetDrawPrimitiveData(
				dptPrimitiveType, dvtVertexType, lpvVertices, dwVertexCount, dwFlags);
			pszErrorMessage = ErrorNumberToMessage(hResult);
			if (InternalFunctionPause("FAILED (0x%x - %s) - DrawPrimitive( %s)", hResult,
					pszErrorMessage, pszPrimitiveData))
				ENTER_DEBUGGER;
		}
	}
	return hResult;
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
/// <param name="d3dDevice7"></param>
/// <param name="d3dptPrimitiveType"></param>
/// <param name="dwVertexTypeDesc"></param>
/// <param name="pvVertices"></param>
/// <param name="dwVertexCount"></param>
/// <param name="pwIndices"></param>
/// <param name="dwIndexCount"></param>
/// <param name="dwFlags"></param>
/// <returns></returns>
HRESULT __stdcall wDrawIndexedPrimitive(LPDIRECT3DDEVICE7 d3dDevice7,
	D3DPRIMITIVETYPE d3dptPrimitiveType, uint32_t dwVertexTypeDesc, PVOID pvVertices,
	uint32_t dwVertexCount, puint16_t pwIndices, uint32_t dwIndexCount, uint32_t dwFlags)
{
	PSTR pszPrimitiveData;
	PSTR pszErrorMessage;
	HRESULT hResult;
	pszPrimitiveData = GetDrawIndexedPrimitiveData(d3dptPrimitiveType, dwVertexTypeDesc, pvVertices,
		dwVertexCount, pwIndices, dwIndexCount, dwFlags);
	InternalFunctionSpew("GameOS_Direct3D", "DrawIndexedPrimitive( %s)", pszPrimitiveData);
	hResult = d3dDevice7->DrawIndexedPrimitive(d3dptPrimitiveType, dwVertexTypeDesc, pvVertices,
		dwVertexCount, pwIndices, dwIndexCount, dwFlags);
	if (FAILED(hResult))
	{
		if (hResult != DDERR_SURFACELOST)
		{
			pszPrimitiveData = GetDrawIndexedPrimitiveData(d3dptPrimitiveType, dwVertexTypeDesc,
				pvVertices, dwVertexCount, pwIndices, dwIndexCount, dwFlags);
			pszErrorMessage = ErrorNumberToMessage(hResult);
			if (InternalFunctionPause("FAILED (0x%x - %s) - DrawIndexedPrimitive( %s)", hResult,
					pszErrorMessage, pszPrimitiveData))
				ENTER_DEBUGGER;
		}
	}
	return hResult;
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
/// <param name="d3dDevice7"></param>
/// <param name="d3dptPrimitiveType"></param>
/// <param name="lpd3dVertexBuffer"></param>
/// <param name="dwStartVertex"></param>
/// <param name="dwNumVertices"></param>
/// <param name="lpwIndices"></param>
/// <param name="dwIndexCount"></param>
/// <param name="dwFlags"></param>
/// <returns></returns>
HRESULT __stdcall wDrawIndexedPrimitiveVB(LPDIRECT3DDEVICE7 d3dDevice7,
	D3DPRIMITIVETYPE d3dptPrimitiveType, LPDIRECT3DVERTEXBUFFER7 lpd3dVertexBuffer,
	uint32_t dwStartVertex, uint32_t dwNumVertices, puint16_t lpwIndices, uint32_t dwIndexCount,
	uint32_t dwFlags)
{
	LPDIRECT3DVERTEXBUFFER7 pd3dVertexBufferUsed;
	PSTR pszErrorMessage;
	HRESULT hResult;

	//InternalFunctionSpew("GameOS_Direct3D", "DrawIndexedPrimitiveVB(0x%x)", lpd3dVertexBuffer);

	if (g_pGetDXVB)
	{
		pd3dVertexBufferUsed = g_pGetDXVB(lpd3dVertexBuffer, d3dDevice7);
	}
	else
	{
		pd3dVertexBufferUsed = lpd3dVertexBuffer;
	}
	hResult = d3dDevice7->DrawIndexedPrimitiveVB(d3dptPrimitiveType, pd3dVertexBufferUsed,
		dwStartVertex, dwNumVertices, lpwIndices, dwIndexCount, dwFlags);
	if (FAILED(hResult))
	{
		if (hResult != DDERR_SURFACELOST)
		{
			pszErrorMessage = ErrorNumberToMessage(hResult);
			if (InternalFunctionPause("FAILED (0x%x - %s) - DrawIndexedPrimitiveVB(0x%x)", hResult,
					pszErrorMessage, lpd3dVertexBuffer))
				ENTER_DEBUGGER;
		}
	}
	return hResult;
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
/// <param name="d3dDevice7"></param>
/// <param name="d3dptPrimitiveType"></param>
/// <param name="lpd3dVertexBuffer"></param>
/// <param name="dwStartVertex"></param>
/// <param name="dwNumVertices"></param>
/// <param name="dwFlags"></param>
/// <returns></returns>
HRESULT __stdcall wDrawPrimitiveVB(LPDIRECT3DDEVICE7 d3dDevice7,
	D3DPRIMITIVETYPE d3dptPrimitiveType, LPDIRECT3DVERTEXBUFFER7 lpd3dVertexBuffer,
	uint32_t dwStartVertex, uint32_t dwNumVertices, uint32_t dwFlags)
{
	LPDIRECT3DVERTEXBUFFER7 pd3dVertexBufferUsed;
	PSTR pszErrorMessage;
	HRESULT hResult;

	// InternalFunctionSpew("GameOS_Direct3D", "DrawPrimitiveVB(0x%x)", lpd3dVertexBuffer);

	if (g_pGetDXVB)
	{
		pd3dVertexBufferUsed = g_pGetDXVB(lpd3dVertexBuffer, d3dDevice7);
	}
	else
	{
		pd3dVertexBufferUsed = lpd3dVertexBuffer;
	}
	hResult = d3dDevice7->DrawPrimitiveVB(
		d3dptPrimitiveType, pd3dVertexBufferUsed, dwStartVertex, dwNumVertices, dwFlags);
	if (FAILED(hResult))
	{
		if (hResult != DDERR_SURFACELOST)
		{
			pszErrorMessage = ErrorNumberToMessage(hResult);
			if (InternalFunctionPause("FAILED (0x%x - %s) - DrawPrimitiveVB(0x%x)", hResult,
					pszErrorMessage, lpd3dVertexBuffer))
				ENTER_DEBUGGER;
		}
	}
	return hResult;
}
