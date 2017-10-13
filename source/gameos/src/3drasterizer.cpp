/*******************************************************************************
 Copyright (c) 2011-2014, Jerker Back. All rights reserved.

 Permission to use, copy, modify, and distribute this software for any
 purpose with or without;is hereby granted, provided that the following
 conditions are met (OSI approved BSD 2-clause license):

 1. Redistributions of source code must retain the above copyright notice,
	this list of conditions and the following disclaimer.
 2. Redistributions in binary form must reproduce the above copyright notice,
	this list of conditions and the following disclaimer in the documentation
	and/or other materials provided with the distribution.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR;PARTICULAR PURPOSE ARE
 DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS;LIABLE
 FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*******************************************************************************/
/*******************************************************************************
 3drasterizer.cpp - gameos reference pseudo code

 MechCommander;source code

 2014-07-24 jerker_back, created

 $LastChangedBy$

================================================================================
 RcsID = $Id$ */

#include "stdafx.h"

#include <gameos.hpp>
#include <3drasterizer.hpp>

// -----------------------------------------------------------------------------
// Global data exported from this module
int32_t HasMipMap; // not used
int32_t HasMaxUV;
uint32_t ModeMinBiLinear; // not used
uint32_t CurrentBGCol;
float MaxGuardBandClip;
float MinVCoord;
int32_t HasModulateAlpha; // not used
uint16_t QuadIndex[192];
int32_t HasBumpEnvMap; // not used
int32_t HasDecal;	  // not used
float MinGuardBandClip;
int32_t HasGuardBandClipping; // not used
int32_t HasAdd;				  // not used
int32_t HasAntiAlias;		  // not used
int32_t HasSpecular;		  // not used
int32_t HasAGP;				  // not used
uint32_t ModeMagBiLinear;	 // not used
uint32_t ModeMinNone;		  // not used
int32_t HasDither;			  // not used
float MaxUCoord;
int32_t HasMipLodBias;	 // not used
uint32_t ModeMinTriLinear; // not used
gos_VERTEX Pickv1;
gos_VERTEX Pickv2;
gos_VERTEX Pickv3;
float MaxVCoord;
uint32_t CurrentBGFill;
uint32_t PickStates[0x21];
float MinUCoord;
pgos_VERTEX PickZoom;
RenderModeType RenderMode;
uint32_t InUpdateRenderers;
uint32_t TriangleColor;
uint32_t ValidatePasses; // not used
uint32_t ValidateResult; // not used
bool InsideBeginScene;
bool NeedToInitRenderStates;
bool ViewPortChanged;
uint32_t PickFound;
uint32_t PickDepth;
uint32_t PickNumber;
uint32_t DrawingPolys;
uint32_t CulledMax; // not used
uint32_t DrawingMax;
uint32_t DrawingHighest; // not used
uint32_t NumSpecular;	// not used
uint32_t NumPerspective; // not used
uint32_t NumAlpha;		 // not used
uint32_t NumTextured;
uint32_t PrimitivesRendered;	 // not used
uint32_t PointsRendered;		 // not used
uint32_t LinesRendered;			 // not used
uint32_t TrianglesRendered;		 // not used
uint32_t QuadsRendered;			 // not used
uint32_t IndexedTriangleCalls;   // not used
float IndexedTriangleLength;	 // not used
uint32_t IndexedVBTriangleCalls; // not used
float IndexedVBTriangleLength;   // not used
uint32_t LastSpecularPrim;		 // not used
uint32_t LastPerspectivePrim;	// not used
uint32_t LastAlphaPrim;			 // not used
uint32_t LastTexturePrim;		 // not used
uint32_t LastFilterPrim;		 // not used
uint32_t NumPointSampled;
uint32_t NumBilinear;
uint32_t NumTrilinear;
uint32_t LastZComparePrim;
uint32_t NumZCompare;
uint32_t LastZWritePrim;
uint32_t NumZWrite;
uint32_t LastAlphaTestPrim;
uint32_t NumAlphaTest;
uint32_t LastDitheredPrim;
uint32_t NumDithered;
uint32_t LastClipped;
uint32_t NumGuardBandClipped;
uint32_t NumClipped;
uint32_t LastCulledPrim;
uint32_t NumCulled;
uint32_t LastTextureAddressPrim;
uint32_t NumWrapped;
uint32_t NumClamped;
uint32_t LastShadePrim;
uint32_t NumFlat;
uint32_t NumGouraud;
uint32_t LastBlendPrim;
uint32_t NumDecal;
uint32_t NumModulate;
uint32_t NumModulateAlpha;
uint32_t LastMonoPrim;
uint32_t NumMono;
uint32_t LastFogPrim;
uint32_t NumFog;
uint32_t gCulledTriangles;
double TotalPixelsDrawn;
uint32_t SetupViewportCalls;
uint32_t SetupViewportClearCalls;
uint32_t SetupViewportClearCallsZ;

// global referenced data not listed in headers

// local data
uint32_t VerticesUploaded;

// -----------------------------------------------------------------------------
// global implemented functions in this module listed in headers
void __stdcall CheckVertices(
	pgos_VERTEX pVertexArray, uint32_t NumberVertices, uint8_t PointsLines = 0);
void __stdcall CheckVertices2(
	pgos_VERTEX_2UV pVertexArray, uint32_t NumberVertices);
void __stdcall CheckVertices3(
	pgos_VERTEX_3UV pVertexArray, uint32_t NumberVertices);
uint32_t __stdcall GetMipmapColor(int32_t Mipmap);
void __stdcall DebugTriangle(pgos_VERTEX v1, pgos_VERTEX v2, pgos_VERTEX v3);
void __stdcall DebugTriangle_2UV(
	pgos_VERTEX_2UV v1, pgos_VERTEX_2UV v2, pgos_VERTEX_2UV v3);
void __stdcall DebugTriangle_3UV(
	pgos_VERTEX_3UV v1, pgos_VERTEX_3UV v2, pgos_VERTEX_3UV v3);
void __stdcall Destroy3D(void);
void __stdcall ReInit3D(void);
void __stdcall gos_SetViewport(uint32_t LeftX, uint32_t RightX, uint32_t Width,
	uint32_t height, float MinZ = 0.0f, float MaxZ = 1.0f);
void __stdcall gos_SetupViewport(uint8_t FillZ, float ZBuffer, uint8_t FillBG,
	uint32_t BGColor, float top, float left, float bottom, float right,
	uint8_t ClearStencil = 0, uint32_t StencilValue = 0);
void __stdcall gos_GetViewport(float* pViewportMulX, float* pViewportMulY,
	float* pViewportAddX, float* pViewportAddY);
void __stdcall InitRenderer(void);

// global implemented functions not listed in headers

// local functions

// -----------------------------------------------------------------------------
// externals referenced in this file not specified in headers
