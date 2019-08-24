//===========================================================================//
// File:	 3DRasterizer.hpp												 //
// Contents: Low level 3D rasterizer										 //
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#pragma once

#ifndef _3DRASTERIZER_HPP_
#define _3DRASTERIZER_HPP_

#ifndef MECH_IMPEXP
#define MECH_IMPEXP extern
#endif

#define LAB_ONLY 1
#define _DEBUG 1

typedef struct _SAVESTATE
{
	struct _SAVESTATE* pNext;
	uint32_t SaveState[gos_MaxState];
} SAVESTATE;
typedef SAVESTATE* PSAVESTATE;

extern PSAVESTATE pStateStack;
extern uint32_t StackDepth;

//
// Structure to hold information about vertex buffers created
//
typedef struct _VertexBuffer
{
	uint32_t Magic; // Magic number to verify handle valid
	gosVERTEXTYPE VertexType; // Type of vertex the buffer contains
	struct _VertexBuffer* pNext;
	LPDIRECT3DVERTEXBUFFER7 vBuffer; // D3D handle
#if defined(LAB_ONLY)
	LPDIRECT3DVERTEXBUFFER7
	DebugvBuffer; // D3D handle to a system memory vertex buffer for debugging D3DTLVERTEX data
	LPDIRECT3DVERTEXBUFFER7 CopyvBuffer; // D3D handle to a system memory vertex buffer for
		// debugging (Readable copy of D3DVERTEX data)
#endif
	uint32_t NumberVertices; // Number of vertices when created
	uint8_t Locked; // True when locked
	uint8_t WantOptimize; // True to optimize after the next unlock
	uint8_t Optimized; // True once buffer is optimized
	uint8_t Lost; // True when mode changed and buffer invalid
	PVOID Pointer; // Pointer to start of data when locked
} VertexBuffer;
typedef VertexBuffer* PVertexBuffer;

//
// Variables that effect rendering
//
extern uint16_t QuadIndex[192];
extern uint32_t AlphaInvAlpha; // Set when alpha blend mode is AlphaInvAlpha

extern uint8_t ViewPortChanged; // Set when game changes viewport
extern uint32_t InUpdateRenderers; // True when in 'Update Renderers'
extern uint32_t DrawingPolys; // Current polygon
extern uint32_t gCulledTriangles; // Number of culled triangles
extern uint32_t CulledMax;
extern uint32_t DrawingMax; // Maximum polys to draw
extern uint32_t DrawingHighest;
extern uint32_t gForceNoClear;
extern uint32_t VertexBuffersLocked;
extern uint32_t gDisableLinesPoints;
extern uint32_t gDisablePrimitives;

// Render to texture variables
void __stdcall InitRenderToTexture(void);
void __stdcall DestroyRenderToTexture(void);

// RenderStates.cpp
extern uint8_t DirtyStates;
extern uint8_t UpdatedState[gos_MaxState];
void __stdcall FlushRenderStates(void);

// 3DRasterizer.hpp
void __stdcall InitRenderStates(void);
void __stdcall InitRenderer(void);
void __stdcall Save3DState(void);
void __stdcall Restore3DState(void);
void __stdcall ReInit3D(void);
void __stdcall Destroy3D(void);
void __stdcall CheckVertices(
	pgos_VERTEX pVertexArray, uint32_t NumberVertices, uint8_t PointsLines = 0);
void __stdcall CheckVertices2(pgos_VERTEX_2UV pVertexArray, uint32_t NumberVertices);
void __stdcall CheckVertices3(pgos_VERTEX_3UV pVertexArray, uint32_t NumberVertices);
void __stdcall DebugTriangle(pgos_VERTEX v1, pgos_VERTEX v2, pgos_VERTEX v3);
void __stdcall DebugTriangle_2UV(pgos_VERTEX_2UV v1, pgos_VERTEX_2UV v2, pgos_VERTEX_2UV v3);
void __stdcall DebugTriangle_3UV(pgos_VERTEX_3UV v1, pgos_VERTEX_3UV v2, pgos_VERTEX_3UV v3);
uint32_t __stdcall GetMipmapColor(int32_t Mipmap);

// clipping.cpp
MECH_IMPEXP void __stdcall gos_ClipDrawQuad(pgos_VERTEX pVertices);

// Statistics
extern uint32_t NumSpecular;
extern uint32_t NumPerspective;
extern uint32_t NumAlpha;
extern uint32_t NumTextured;
extern uint32_t PrimitivesRendered;
extern uint32_t PointsRendered;
extern uint32_t LinesRendered;
extern uint32_t TrianglesRendered;
extern uint32_t QuadsRendered;
extern uint32_t IndexedTriangleCalls;
extern float IndexedTriangleLength;
extern uint32_t IndexedVBTriangleCalls;
extern float IndexedVBTriangleLength;
extern uint32_t LastSpecularPrim;
extern uint32_t LastPerspectivePrim;
extern uint32_t LastAlphaPrim;
extern uint32_t LastTexturePrim;
extern uint32_t LastFilterPrim;
extern uint32_t NumPointSampled;
extern uint32_t NumBilinear;
extern uint32_t NumTrilinear;
extern uint32_t LastZComparePrim;
extern uint32_t NumZCompare;
extern uint32_t LastZWritePrim;
extern uint32_t NumZWrite;
extern uint32_t LastAlphaTestPrim;
extern uint32_t NumAlphaTest;
extern uint32_t LastDitheredPrim;
extern uint32_t NumDithered;
extern uint32_t LastCulledPrim;
extern uint32_t NumCulled;
extern uint32_t LastTextureAddressPrim;
extern uint32_t NumWrapped;
extern uint32_t NumClamped;
extern uint32_t LastShadePrim;
extern uint32_t NumFlat;
extern uint32_t NumGouraud;
extern uint32_t LastBlendPrim;
extern uint32_t NumDecal;
extern uint32_t NumModulate;
extern uint32_t NumModulateAlpha;
extern uint32_t LastMonoPrim;
extern uint32_t NumMono;
extern uint32_t LastFogPrim;
extern uint32_t NumFog;
extern uint32_t LastClipped;
extern uint32_t NumGuardBandClipped;
extern uint32_t NumClipped;

extern gos_VERTEX PickZoom[3];

// Capabilities of the mode
extern int32_t HasGuardBandClipping;
extern float MinGuardBandClip; // Minimum guard band clip
extern float MaxGuardBandClip; // Maximum guard band clip
extern int32_t HasAGP; // AGP memory available?
extern int32_t HasSpecular; // Specular available
extern int32_t HasDither; // Dithering available
extern int32_t HasAntiAlias; // Can do sort independant antialias
extern int32_t HasMipLodBias; // MipMap LOD bias can be altered
extern int32_t HasDecal; // Can DECAL
extern int32_t HasModulateAlpha; // Can MODULATEALPHA
extern int32_t HasAdd; // Can ADD
extern int32_t HasMipMap; // Can do mip mapping

extern int32_t HasMaxUV; // There is a maximum UV coord for this render mode (and texture)
extern float MaxUCoord; // The maximum u coord (for current texture)//
extern float MinUCoord; // The minimum u coord (for current texture)// Various
	// renderstates based on the capabilities of the current
	// video card
extern float MaxVCoord; // The maximum v coord (for current texture)//
extern float MinVCoord; // The minimum v coord (for current texture)// Various
	// renderstates based on the capabilities of the current
	// video card

// Various modes the renderer can be in
enum class RenderModeType
{
	Normal = 0, // Normal rendering
	WireframeGlobalColor, // Only wireframe, no texture, global wireframe color
	WireframeGlobalTexture, // Global wireframe color ontop of texture
	ExamineScene, // Examine, zoom into scene mode
	ExamineMipmaps, // Examine Mipmaps mode
	ExamineRenderer, // Compare with reference rasterizer
	ExamineDepth, // Examine depth complexity
	ExamineTriangleSize, // Examine triangle size
	ExamineTriangleSize1, // Examine triangle size and only draw 1 or 2 pixel
	// triangles
	ExamineArea, // Examine an area of the screen
	ExamineDrawing, // Examine scnene being drawn
	ShowTriangleColors, // Show each triangle as a different color
	ExamineColor, // Examine scene, show color information
	ExamineMipmaps2, // Examine mipmaps using colored mipmap (alter u,v)
} RenderModeType;

extern RenderModeType RenderMode; // In a special debugger mode

extern uint32_t RenderStates[gos_MaxState]; // Current User settings
extern uint32_t PreviousStates[gos_MaxState]; // Previously set renderstates

// Various renderstates based on the capabilities of the current video card
extern uint32_t ModeMagBiLinear; // MAG mode for Bilinear
extern uint32_t ModeMinNone; // MIN mode for non filtering
extern uint32_t ModeMinBiLinear; // MIN mode for bilinear
extern uint32_t ModeMinTriLinear; // MIN mode for trilinear

#endif
