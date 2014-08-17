//===========================================================================//
// File:	 3DRasterizer.hpp												 //
// Contents: Low level 3D rasterizer										 //
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#pragma once


typedef struct _SAVESTATE {
	struct _SAVESTATE*	pNext;
	ULONG				SaveState[gos_MaxState];
} SAVESTATE;
typedef SAVESTATE *PSAVESTATE;

extern SAVESTATE*	pStateStack;
extern ULONG		StackDepth;

//
// Structure to hold information about vertex buffers created
//
typedef struct _VertexBuffer {
	ULONG					Magic;					// Magic number to verify handle valid
	gosVERTEXTYPE			VertexType;				// Type of vertex the buffer contains
	struct _VertexBuffer*	pNext;
	LPDIRECT3DVERTEXBUFFER7	vBuffer;				// D3D handle
#if defined(LAB_ONLY)
	LPDIRECT3DVERTEXBUFFER7	DebugvBuffer;			// D3D handle to a system memory vertex buffer for debugging D3DTLVERTEX data
	LPDIRECT3DVERTEXBUFFER7	CopyvBuffer;			// D3D handle to a system memory vertex buffer for debugging (Readable copy of D3DVERTEX data)
#endif
	ULONG					NumberVertices;			// Number of vertices when created
	UCHAR					Locked;					// True when locked
	UCHAR					WantOptimize;			// True to optimize after the next unlock
	UCHAR					Optimized;				// True once buffer is optimized
	UCHAR					Lost;					// True when mode changed and buffer invalid
	PVOID					Pointer;				// Pointer to start of data when locked
} VertexBuffer;
typedef VertexBuffer *PVertexBuffer;

//
// Variables that effect rendering
//
extern USHORT QuadIndex[192];
extern ULONG AlphaInvAlpha;			// Set when alpha blend mode is AlphaInvAlpha

extern UCHAR ViewPortChanged;			// Set when game changes viewport
extern ULONG InUpdateRenderers;		// True when in 'Update Renderers'
extern ULONG DrawingPolys;			// Current polygon
extern ULONG gCulledTriangles;		// Number of culled triangles
extern ULONG CulledMax;
extern ULONG DrawingMax;			// Maximum polys to draw
extern ULONG DrawingHighest;
extern ULONG gForceNoClear;
extern ULONG VertexBuffersLocked;
extern ULONG gDisableLinesPoints;
extern ULONG gDisablePrimitives;

// Render to texture variables
void __stdcall InitRenderToTexture(void);
void __stdcall DestroyRenderToTexture(void);

// RenderStates.cpp
extern UCHAR DirtyStates;
extern UCHAR UpdatedState[gos_MaxState];
void __stdcall FlushRenderStates(void);

// 3DRasterizer.hpp
void __stdcall InitRenderStates(void);
void __stdcall  InitRenderer(void);
void __stdcall Save3DState(void);
void __stdcall Restore3DState(void);
void __stdcall ReInit3D(void);
void __stdcall Destroy3D(void);
void __stdcall CheckVertices( pgos_VERTEX pVertexArray, ULONG NumberVertices, UCHAR PointsLines=0 );
void __stdcall CheckVertices2( pgos_VERTEX_2UV pVertexArray, ULONG NumberVertices );
void __stdcall CheckVertices3( pgos_VERTEX_3UV pVertexArray, ULONG NumberVertices );
void __stdcall DebugTriangle( pgos_VERTEX v1, pgos_VERTEX v2, pgos_VERTEX v3 );
void __stdcall DebugTriangle_2UV( pgos_VERTEX_2UV v1, pgos_VERTEX_2UV v2, pgos_VERTEX_2UV v3 );
void __stdcall DebugTriangle_3UV( pgos_VERTEX_3UV v1, pgos_VERTEX_3UV v2, pgos_VERTEX_3UV v3 );
ULONG __stdcall GetMipmapColor( int Mipmap );

// clipping.cpp
MECH_IMPEXP void MECH_CALL gos_ClipDrawQuad(pgos_VERTEX pVertices);

// Statistics
extern ULONG	NumSpecular;
extern ULONG	NumPerspective;
extern ULONG	NumAlpha;
extern ULONG	NumTextured;
extern ULONG	PrimitivesRendered;
extern ULONG	PointsRendered;
extern ULONG	LinesRendered;
extern ULONG	TrianglesRendered;
extern ULONG	QuadsRendered;
extern ULONG	IndexedTriangleCalls;
extern float	IndexedTriangleLength;
extern ULONG	IndexedVBTriangleCalls;
extern float	IndexedVBTriangleLength;
extern ULONG	LastSpecularPrim;
extern ULONG	LastPerspectivePrim;
extern ULONG	LastAlphaPrim;
extern ULONG	LastTexturePrim;
extern ULONG	LastFilterPrim;
extern ULONG	NumPointSampled;
extern ULONG	NumBilinear;
extern ULONG	NumTrilinear;
extern ULONG	LastZComparePrim;
extern ULONG	NumZCompare;
extern ULONG	LastZWritePrim;
extern ULONG	NumZWrite;
extern ULONG	LastAlphaTestPrim;
extern ULONG	NumAlphaTest;
extern ULONG	LastDitheredPrim;
extern ULONG	NumDithered;
extern ULONG	LastCulledPrim;
extern ULONG	NumCulled;
extern ULONG	LastTextureAddressPrim;
extern ULONG	NumWrapped;
extern ULONG	NumClamped;
extern ULONG	LastShadePrim;
extern ULONG	NumFlat;
extern ULONG	NumGouraud;
extern ULONG	LastBlendPrim;
extern ULONG	NumDecal;
extern ULONG	NumModulate;
extern ULONG	NumModulateAlpha;
extern ULONG	LastMonoPrim;
extern ULONG	NumMono;
extern ULONG	LastFogPrim;
extern ULONG	NumFog;
extern ULONG	LastClipped;
extern ULONG	NumGuardBandClipped;
extern ULONG	NumClipped;

extern gos_VERTEX PickZoom[3];

// Capabilities of the mode
extern int HasGuardBandClipping;
extern float MinGuardBandClip;		// Minimum guard band clip
extern float MaxGuardBandClip;		// Maximum guard band clip
extern int HasAGP;					// AGP memory available?
extern int HasSpecular;				// Specular available
extern int HasDither;				// Dithering available
extern int HasAntiAlias;			// Can do sort independant antialias
extern int HasMipLodBias;			// MipMap LOD bias can be altered
extern int HasDecal;				// Can DECAL
extern int HasModulateAlpha;		// Can MODULATEALPHA
extern int HasAdd;					// Can ADD
extern int HasMipMap;				// Can do mip mapping

extern int	 HasMaxUV;				// There is a maximum UV coord for this render mode (and texture)
extern float MaxUCoord;			// The maximum u coord (for current texture)//
extern float MinUCoord;			// The minimum u coord (for current texture)// Various renderstates based on the capabilities of the current video card
extern float MaxVCoord;			// The maximum v coord (for current texture)//
extern float MinVCoord;			// The minimum v coord (for current texture)// Various renderstates based on the capabilities of the current video card

// Various modes the renderer can be in
typedef enum RenderModeType {
	Normal=0,						// Normal rendering
	WireframeGlobalColor,			// Only wireframe, no texture, global wireframe color
	WireframeGlobalTexture,			// Global wireframe color ontop of texture
	ExamineScene,					// Examine, zoom into scene mode
	ExamineMipmaps,					// Examine Mipmaps mode
	ExamineRenderer,				// Compare with reference rasterizer
	ExamineDepth,					// Examine depth complexity
	ExamineTriangleSize,			// Examine triangle size
	ExamineTriangleSize1,			// Examine triangle size and only draw 1 or 2 pixel triangles
	ExamineArea,					// Examine an area of the screen
	ExamineDrawing,					// Examine scnene being drawn
	ShowTriangleColors,				// Show each triangle as a different color
	ExamineColor,					// Examine scene, show color information
	ExamineMipmaps2,				// Examine mipmaps using colored mipmap (alter u,v)
} RenderModeType;

extern RenderModeType RenderMode;	// In a special debugger mode

extern ULONG RenderStates[gos_MaxState];			// Current User settings
extern ULONG PreviousStates[gos_MaxState];			// Previously set renderstates

// Various renderstates based on the capabilities of the current video card
extern ULONG	ModeMagBiLinear;		// MAG mode for Bilinear
extern ULONG	ModeMinNone;			// MIN mode for non filtering
extern ULONG	ModeMinBiLinear;		// MIN mode for bilinear
extern ULONG	ModeMinTriLinear;		// MIN mode for trilinear
