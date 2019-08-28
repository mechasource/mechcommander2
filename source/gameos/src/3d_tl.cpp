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
 3d_tl.cpp - GameOS reference pseudo code

 MechCommander 2 source code

 2014-07-24 Jerker Back, created

*******************************************************************************/

#include "stdinc.h"

#include "gameos.hpp"
#include "directx.hpp"
#include "dxrasterizer.hpp"

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

#pragma region gos_SetTransform
// Sets the WORLD/VIEW or PROJECTION matrix
#pragma endregion local info
/// <summary>
/// <c>gos_SetTransform</c> sets the WORLD/VIEW or PROJECTION matrix
/// </summary>
/// <remarks>
/// </remarks>
/// <param name="TransformType"></param>
/// <param name="pMatrixData"></param>
/// <returns></returns>
void __stdcall gos_SetTransform(D3DTRANSFORMSTATETYPE TransformType, LPD3DMATRIX pMatrixData)
{
	gosASSERT(InsideBeginScene);
	gosASSERT(TransformType >= D3DTRANSFORMSTATE_WORLD && TransformType <= D3DTRANSFORMSTATE_WORLD1);
	wSetTransform(d3ddevice7, TransformType, pMatrixData);
	GameOSFPU();
}

#pragma region gos_MultiplyTransform
// Multiplies the current WORLD/VIEW or PROJECTION matrix by another
#pragma endregion local info
/// <summary>
/// <c>gos_MultiplyTransform</c> multiplies the current WORLD/VIEW or PROJECTION
/// matrix by another
/// </summary>
/// <remarks> not referenced in Mechcommander 2
/// </remarks>
/// <param name="TransformType"></param>
/// <param name="pMatrixData"></param>
/// <returns></returns>
void __stdcall gos_MultiplyTransform(D3DTRANSFORMSTATETYPE TransformType, LPD3DMATRIX pMatrixData)
{
	gosASSERT(InsideBeginScene);
	gosASSERT(TransformType >= D3DTRANSFORMSTATE_WORLD && TransformType <= D3DTRANSFORMSTATE_WORLD1);
	wMultiplyTransform(d3ddevice7, TransformType, pMatrixData);
	GameOSFPU();
}

#pragma region gos_SetLight
// Sets the parameters of a light (LightNumber can be any number)
#pragma endregion local info
/// <summary>
/// <c>gos_SetLight</c> sets the parameters of a light (LightNumber can be any
/// number)
/// </summary>
/// <remarks>
/// </remarks>
/// <param name="LightNumber"></param>
/// <param name="pLightData"></param>
/// <returns></returns>
void __stdcall gos_SetLight(uint32_t LightNumber, LPD3DLIGHT7 pLightData)
{
	gosASSERT(InsideBeginScene);
	wSetLight(d3ddevice7, LightNumber, pLightData);
	GameOSFPU();
}

#pragma region gos_LightEnable
// Enables / Disables a light (Maximum of gos_GetMachineInformation(
// gos_Info_GetMaximumActiveLights ) lights enabled at once)
#pragma endregion local info
/// <summary>
/// <c>gos_LightEnable</c> enables or disables a light
/// </summary>
/// <remarks>
/// </remarks>
/// <param name="LightNumber"></param>
/// <param name="Enable"></param>
/// <returns></returns>
void __stdcall gos_LightEnable(uint32_t LightNumber, uint8_t Enable)
{
	gosASSERT(InsideBeginScene);
	wLightEnable(d3ddevice7, LightNumber, Enable);
	GameOSFPU();
}

#pragma region gos_SetMaterial
// Sets the parameters of the current material
#pragma endregion local info
/// <summary>
/// <c>gos_SetMaterial</c>
/// </summary>
/// <remarks>
/// </remarks>
/// <param name="pMaterialData"></param>
/// <returns></returns>
void __stdcall gos_SetMaterial(LPD3DMATERIAL7 pMaterialData)
{
	gosASSERT(InsideBeginScene);
	wSetMaterial(d3ddevice7, pMaterialData);
	GameOSFPU();
}
