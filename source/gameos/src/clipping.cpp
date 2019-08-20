/*******************************************************************************
 This file consist of reversed and interpreted code from object source in the
 x86 debug build gameos.lib. The code in this file can not be used in any way
 other than serve as an information reference to the original released source of
 Mechcommander2. The code is a work of progress and there is no guarantee it is
 complete, accurate or useful in any way. The purpose is instead to make it
 possible to safely remove any dependencies of gameos.lib from Mechcommander2.
*******************************************************************************/
/*******************************************************************************
 clipping.cpp - GameOS reference pseudo code

 MechCommander 2 source code

 2014-07-24 Jerker Beck, created

*******************************************************************************/

#include "stdinc.h"

#include <gameos.hpp>
#include <directx.hpp>
#include <3drasterizer.hpp>
#include <dxrasterizer.hpp>
#include <texture_manager.hpp>

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

#pragma region gos_ClipDrawQuad
/*

*/
#pragma endregion local info
/// <summary>
/// <c>gos_ClipDrawQuad</c>
/// </summary>
/// <remarks>
/// </remarks>
/// <param name="pVertices"></param>
/// <returns></returns>
MECH_IMPEXP void MECH_CALL
gos_ClipDrawQuad(pgos_VERTEX pVertices)
{
	double fWidth;
	double fHeight;
	CTexInfo texinfo;
	if (Compatibility3D & 0x40)
	{
		if (RenderStates[1])
		{
			fWidth = 0.0625 / (double)CTexInfo::Width();
			fHeight = 0.0625 / (double)CTexInfo::Height();
			pVertices[0].u = (float)((double)pVertices[0].u + fWidth);
			pVertices[0].v = (float)((double)pVertices[0].v + fHeight);
			pVertices[1].u = (float)((double)pVertices[1].u + fWidth);
			pVertices[1].v = (float)((double)pVertices[1].v + fHeight);
			pVertices[2].u = (float)((double)pVertices[2].u + fWidth);
			pVertices[2].v = (float)((double)pVertices[2].v + fHeight);
			pVertices[3].u = (float)((double)pVertices[3].u + fWidth);
			pVertices[3].v = (float)((double)pVertices[3].v + fHeight);
		}
	}
	else
	{
		pVertices[0].x = (float)((double)pVertices[0].x - 0.4375);
		pVertices[0].y = (float)((double)pVertices[0].y - 0.4375);
		pVertices[1].x = (float)((double)pVertices[1].x - 0.4375);
		pVertices[1].y = (float)((double)pVertices[1].y - 0.4375);
		pVertices[2].x = (float)((double)pVertices[2].x - 0.4375);
		pVertices[2].y = (float)((double)pVertices[2].y - 0.4375);
		pVertices[3].x = (float)((double)pVertices[3].x - 0.4375);
		pVertices[3].y = (float)((double)pVertices[3].y - 0.4375);
	}
	fWidth = (double)Environment.screenWidth;
	fHeight = (double)Environment.screenHeight;
	if (Environment.Renderer == 3 && RenderStates[19] & 0x100)
	{
		fWidth = fWidth / 2.0;
		fHeight = fHeight / 2.0;
	}
	if (fWidth > (double)pVertices[1].x && pVertices[3].x > 0.0 && fHeight > (double)pVertices[3].y && pVertices[1].y > 0.0)
	{
		if (DirtyStates)
			FlushRenderStates();
		if (pVertices[1].x < 0.0 || fWidth < (double)pVertices[3].x || pVertices[3].y < 0.0 || fHeight < (double)pVertices[1].y)
		{
			PrimitivesRendered += 2;
			++QuadsRendered;
			if (HasGuardBandClipping)
			{
				++NumGuardBandClipped;
				wDrawPrimitive(d3dDevice7, D3DPT_TRIANGLEFAN, 0x1C4u, pVertices, 4u, 0);
				GameOSFPU();
			}
			else
			{
				NumClipped += 2;
				wSetRenderState(d3dDevice7, D3DRS_CLIPPING, 1u);
				wDrawPrimitive(d3dDevice7, D3DPT_TRIANGLEFAN, 0x1C4u, pVertices, 4u, 0);
				wSetRenderState(d3dDevice7, D3DRS_CLIPPING, 0);
			}
		}
		else
		{
			gos_DrawQuads(pVertices, 4);
		}
	}
}
