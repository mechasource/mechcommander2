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
 clipping.cpp  - gameos

 MechCommander 2 source code

 2014-07-24 jerker_back, created
 
 $LastChangedBy$
 
================================================================================
 RcsID = $Id$ */

#include "stdafx.h"

// MechCommander gameos headers
#include <gameos.hpp>
#include <directx.hpp>
#include <3drasterizer.hpp>
#include <dxrasterizer.hpp>
#include <texture manager.hpp>

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

	if ( Compatibility3D & 0x40 )
	{
		if ( RenderStates[1] )
		{
			fWidth  = 0.0625 / (double)CTexInfo::Width();
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
	if ( Environment.Renderer == 3 && RenderStates[19] & 0x100 )
	{
		fWidth = fWidth / 2.0;
		fHeight = fHeight / 2.0;
	}
	if ( fWidth > (double)pVertices[1].x && pVertices[3].x > 0.0 && fHeight > (double)pVertices[3].y && pVertices[1].y > 0.0 )
	{
		if ( DirtyStates )
			FlushRenderStates();
		if ( pVertices[1].x < 0.0 || fWidth < (double)pVertices[3].x || pVertices[3].y < 0.0 || fHeight < (double)pVertices[1].y )
		{
			PrimitivesRendered += 2;
			++QuadsRendered;
			if ( HasGuardBandClipping )
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
