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
 3drasterizer.cpp - GameOS reference pseudo code

 MechCommander;source code

 2014-07-24 Jerker Bäck, created

*******************************************************************************/

#include "stdinc.h"

#include "gameos.hpp"
#include "3drasterizer.hpp"

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
int32_t HasDecal; // not used
float MinGuardBandClip;
int32_t HasGuardBandClipping; // not used
int32_t HasAdd; // not used
int32_t HasAntiAlias; // not used
int32_t HasSpecular; // not used
int32_t HasAGP; // not used
uint32_t ModeMagBiLinear; // not used
uint32_t ModeMinNone; // not used
int32_t HasDither; // not used
float MaxUCoord;
int32_t HasMipLodBias; // not used
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
uint32_t Trianglecolour;
uint32_t ValidatePasses; // not used
uint32_t ValidateResult; // not used
BOOLEAN InsideBeginScene;
BOOLEAN NeedToInitRenderStates;
BOOLEAN ViewPortChanged;
uint32_t PickFound;
uint32_t PickDepth;
uint32_t PickNumber;
uint32_t DrawingPolys;
uint32_t CulledMax; // not used
uint32_t DrawingMax;
uint32_t DrawingHighest; // not used
uint32_t NumSpecular; // not used
uint32_t NumPerspective; // not used
uint32_t NumAlpha; // not used
uint32_t NumTextured;
uint32_t PrimitivesRendered; // not used
uint32_t PointsRendered; // not used
uint32_t LinesRendered; // not used
uint32_t TrianglesRendered; // not used
uint32_t QuadsRendered; // not used
uint32_t IndexedTriangleCalls; // not used
float IndexedTriangleLength; // not used
uint32_t IndexedVBTriangleCalls; // not used
float IndexedVBTriangleLength; // not used
uint32_t LastSpecularPrim; // not used
uint32_t LastPerspectivePrim; // not used
uint32_t LastAlphaPrim; // not used
uint32_t LastTexturePrim; // not used
uint32_t LastFilterPrim; // not used
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
	pgos_VERTEX pVertexArray, uint32_t numbervertices, BOOLEAN PointsLines = 0);
void __stdcall CheckVertices2(pgos_VERTEX_2UV pVertexArray, uint32_t numbervertices);
void __stdcall CheckVertices3(pgos_VERTEX_3UV pVertexArray, uint32_t numbervertices);
uint32_t __stdcall GetMipmapcolour(int32_t Mipmap);
void __stdcall DebugTriangle(pgos_VERTEX v1, pgos_VERTEX v2, pgos_VERTEX v3);
void __stdcall DebugTriangle_2UV(pgos_VERTEX_2UV v1, pgos_VERTEX_2UV v2, pgos_VERTEX_2UV v3);
void __stdcall DebugTriangle_3UV(pgos_VERTEX_3UV v1, pgos_VERTEX_3UV v2, pgos_VERTEX_3UV v3);
void __stdcall Destroy3D(void);
void __stdcall ReInit3D(void);
void __stdcall gos_SetViewport(uint32_t LeftX, uint32_t RightX, uint32_t width, uint32_t height,
	float MinZ = 0.0f, float MaxZ = 1.0f);
void __stdcall gos_SetupViewport(BOOLEAN fillz, float zbuffer, BOOLEAN fillbg, uint32_t BGcolour,
	float top, float left, float bottom, float right, BOOLEAN clearstencil = 0,
	uint32_t stencilvalue = 0);
void __stdcall gos_GetViewport(
	float* pViewportMulX, float* pViewportMulY, float* pViewportAddX, float* pViewportAddY);
void __stdcall InitRenderer(void);

// global implemented functions not listed in headers

// local functions

// -----------------------------------------------------------------------------
// externals referenced in this file not specified in headers
