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
 3d_tl.cpp - gameos

 MechCommander 2 source code

 2014-07-24 jerker_back, created
 
 $LastChangedBy$
 
================================================================================
 RcsID = $Id$ */

#include "stdafx.h"
#include <gameos.hpp>
#include <directx.hpp>

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
void __stdcall gos_SetTransform(
	D3DTRANSFORMSTATETYPE TransformType, LPD3DMATRIX pMatrixData)
{
	gosASSERT(InsideBeginScene);
	gosASSERT(TransformType>=D3DTRANSFORMSTATE_WORLD && TransformType<=D3DTRANSFORMSTATE_WORLD1);

	wSetTransform(d3dDevice7, TransformType, pMatrixData);

	GameOSFPU();
}

#pragma region gos_MultiplyTransform
// Multiplies the current WORLD/VIEW or PROJECTION matrix by another
#pragma endregion local info
/// <summary>
/// <c>gos_MultiplyTransform</c> multiplies the current WORLD/VIEW or PROJECTION 
/// matrix by another
/// </summary>
/// <remarks>
/// </remarks>
/// <param name="TransformType"></param>
/// <param name="pMatrixData"></param>
/// <returns></returns>
void __stdcall gos_MultiplyTransform(
	D3DTRANSFORMSTATETYPE TransformType, LPD3DMATRIX pMatrixData)
{
	gosASSERT(InsideBeginScene);
	gosASSERT(TransformType>=D3DTRANSFORMSTATE_WORLD && TransformType<=D3DTRANSFORMSTATE_WORLD1);

	wMultiplyTransform(d3dDevice7, TransformType, pMatrixData);

	GameOSFPU();
}

#pragma region gos_SetLight
// Sets the parameters of a light (LightNumber can be any number)
#pragma endregion local info
/// <summary>
/// <c>gos_SetLight</c> sets the parameters of a light (LightNumber can be any number)
/// </summary>
/// <remarks>
/// </remarks>
/// <param name="LightNumber"></param>
/// <param name="pLightData"></param>
/// <returns></returns>
void __stdcall gos_SetLight(uint32_t LightNumber, LPD3DLIGHT7 pLightData)
{
	gosASSERT(InsideBeginScene);

	wSetLight(d3dDevice7, LightNumber, pLightData);

	GameOSFPU();
}

#pragma region gos_LightEnable
// Enables / Disables a light (Maximum of gos_GetMachineInformation( gos_Info_GetMaximumActiveLights ) lights enabled at once)
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

	wLightEnable(d3dDevice7, LightNumber, Enable);

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

	wSetMaterial(d3dDevice7, pMaterialData);

	GameOSFPU();
}
