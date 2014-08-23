//===========================================================================//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//


#include "stdafx.h"
#include "mlrheaders.hpp"

uint32_t	gShowClippedPolys=0;
uint32_t	gShowBirdView=0;
uint32_t	gEnableDetailTexture=1;
uint32_t	gEnableTextureSort=1;
uint32_t	gEnableAlphaSort=1;
uint32_t	gEnableMultiTexture=1;
uint32_t	gEnableLightMaps=1;

static uint8_t __stdcall CheckDetailTexture(void)
{
	return uint8_t((gEnableDetailTexture!=0) ? 1 : 0);
}
static uint8_t __stdcall CheckTextureSort(void)
{
	return uint8_t((gEnableTextureSort!=0) ? 1 : 0);
}
static uint8_t __stdcall CheckAlphaSort(void)
{
	return uint8_t((gEnableAlphaSort!=0) ? 1 : 0);
}
static uint8_t __stdcall CheckMultiTexture(void)
{
	return uint8_t((gEnableMultiTexture!=0) ? 1 : 0);
}
static uint8_t __stdcall CheckLightMaps(void)
{
	return uint8_t((gEnableLightMaps!=0) ? 1 : 0);
}

static void __stdcall EnableDetailTexture(void)
{
	gEnableDetailTexture=!gEnableDetailTexture;
}
static void __stdcall EnableTextureSort(void)
{
	gEnableTextureSort=!gEnableTextureSort;
}
static void __stdcall EnableAlphaSort(void)
{
	gEnableAlphaSort=!gEnableAlphaSort;
}
static void __stdcall EnableMultiTexture(void)
{
	gEnableMultiTexture=!gEnableMultiTexture;
}
static void __stdcall EnableLightMaps(void)
{
	gEnableLightMaps=!gEnableLightMaps;
}

extern uint32_t gShowClippedPolys;
static uint8_t __stdcall Check_ShowClippedPolys(void) {return uint8_t((gShowClippedPolys!=0) ? 1 : 0);}
static void __stdcall Toggle_ShowClippedPolys(void) {gShowClippedPolys=!gShowClippedPolys;}

extern uint32_t gShowBirdView;
static uint8_t __stdcall Check_ShowBirdView(void) {return uint8_t((gShowBirdView!=0) ? 1 : 0);}
static void __stdcall Toggle_ShowBirdView(void) {gShowBirdView=!gShowBirdView;}

uint32_t
	Limits::Max_Number_Vertices_Per_Frame,
	Limits::Max_Number_Primitives_Per_Frame,
	Limits::Max_Number_ScreenQuads_Per_Frame,
	Limits::Max_Size_Of_LightMap_MemoryStream;

HGOSHEAP
	MidLevelRenderer::Heap = NULL,
	MidLevelRenderer::StaticHeap = NULL;

DEFINE_TIMER(MidLevelRenderer, Scene_Draw_Time);

DEFINE_TIMER(MidLevelRenderer, Transform_Time);
DEFINE_TIMER(MidLevelRenderer, Clipping_Time);
DEFINE_TIMER(MidLevelRenderer, GOS_Draw_Time);
DEFINE_TIMER(MidLevelRenderer, Vertex_Light_Time);
DEFINE_TIMER(MidLevelRenderer, LightMap_Light_Time);
DEFINE_TIMER(MidLevelRenderer, Texture_Sorting_Time);
DEFINE_TIMER(MidLevelRenderer, Alpha_Sorting_Time);
DEFINE_TIMER(MidLevelRenderer, Unlock_Texture_Time);

uint32_t MidLevelRenderer::Number_Of_Primitives;
uint32_t MidLevelRenderer::NumAllIndices;
uint32_t MidLevelRenderer::NumAllVertices;
float MidLevelRenderer::Index_Over_Vertex_Ratio;
uint32_t MidLevelRenderer::TransformedVertices;
uint32_t MidLevelRenderer::NumberOfAlphaSortedTriangles;
uint32_t MidLevelRenderer::LitVertices;
uint32_t MidLevelRenderer::NonClippedVertices;
uint32_t MidLevelRenderer::ClippedVertices;
uint32_t MidLevelRenderer::PolysClippedButOutside;
uint32_t MidLevelRenderer::PolysClippedButInside;
uint32_t MidLevelRenderer::PolysClippedButOnePlane;
uint32_t MidLevelRenderer::PolysClippedButGOnePlane;


bool MidLevelRenderer::ConvertToTriangleMeshes = true;
bool MidLevelRenderer::PerspectiveMode = true;

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
void
	MidLevelRenderer::InitializeClasses(
		uint32_t Max_Number_Vertices_Per_Frame,
		uint32_t Max_Number_Primitives_Per_Frame,
		uint32_t Max_Number_ScreenQuads_Per_Frame,
		uint32_t Max_Size_Of_LightMap_MemoryStream,
		bool Convert_To_Triangle_Meshes
	)
{
	Verify(FirstFreeMLRClassID <= LastMLRClassID);

	Verify(!StaticHeap);
	StaticHeap = gos_CreateMemoryHeap("MLR Static");
	Check_Pointer(StaticHeap);
	gos_PushCurrentHeap(StaticHeap);

	Verify(!Heap);
	Heap = gos_CreateMemoryHeap("MLR");
	Check_Pointer(Heap);

	Limits::Max_Number_Vertices_Per_Frame = Max_Number_Vertices_Per_Frame;
	Limits::Max_Number_Primitives_Per_Frame = Max_Number_Primitives_Per_Frame;
	Limits::Max_Number_ScreenQuads_Per_Frame = Max_Number_ScreenQuads_Per_Frame;
	Limits::Max_Size_Of_LightMap_MemoryStream = Max_Size_Of_LightMap_MemoryStream;

	ConvertToTriangleMeshes = Convert_To_Triangle_Meshes;

	MLRLight::InitializeClass();
	MLRTexturePool::InitializeClass();
	MLRClipper::InitializeClass();
	MLRSorter::InitializeClass();
	MLRSortByOrder::InitializeClass();
	MLRShape::InitializeClass();
	MLREffect::InitializeClass();
	MLRPointCloud::InitializeClass();
	MLRTriangleCloud::InitializeClass();
	MLRNGonCloud::InitializeClass();
	MLRCardCloud::InitializeClass();
	MLRAmbientLight::InitializeClass();
	MLRInfiniteLight::InitializeClass();
	MLRInfiniteLightWithFalloff::InitializeClass();
	MLRPointLight::InitializeClass();
	MLRSpotLight::InitializeClass();
//	MLRLightMap::InitializeClass();

	MLRPrimitiveBase::InitializeClass();
	MLRIndexedPrimitiveBase::InitializeClass();
	MLR_I_PMesh::InitializeClass();
	MLR_I_C_PMesh::InitializeClass();
	MLR_I_L_PMesh::InitializeClass();
	MLR_I_DT_PMesh::InitializeClass();
	MLR_I_C_DT_PMesh::InitializeClass();
	MLR_I_L_DT_PMesh::InitializeClass();

	MLR_I_MT_PMesh::InitializeClass();

	MLR_I_DeT_PMesh::InitializeClass();
	MLR_I_C_DeT_PMesh::InitializeClass();
	MLR_I_L_DeT_PMesh::InitializeClass();

	MLR_I_TMesh::InitializeClass();
	MLR_I_DeT_TMesh::InitializeClass();
	MLR_I_C_TMesh::InitializeClass();
	MLR_I_L_TMesh::InitializeClass();

//	MLR_Terrain::InitializeClass();
//	MLR_Terrain2::InitializeClass();

	MLRLineCloud::InitializeClass();
	MLRIndexedTriangleCloud::InitializeClass();

	MLR_I_DT_TMesh::InitializeClass();
	MLR_I_C_DT_TMesh::InitializeClass();
	MLR_I_L_DT_TMesh::InitializeClass();

	MLR_I_C_DeT_TMesh::InitializeClass();
	MLR_I_L_DeT_TMesh::InitializeClass();

	MLRLookUpLight::InitializeClass();

#if FOG_HACK
//	for(int32_t i=0;i<Limits::Max_Number_Of_FogStates;i++)
//	{
//		GOSVertex::SetFogTableEntry(i+1, 700.0f, 1000.0f, 0.0f);
//	}
#endif

	gos_PopCurrentHeap();

	//
	//-------------------------
	// Setup the debugger menus
	//-------------------------
	//
	AddDebuggerMenuItem(
		"Libraries\\MLR\\Show Clipped Polygons",
		Check_ShowClippedPolys,
		Toggle_ShowClippedPolys,
		NULL
	);
	AddDebuggerMenuItem(
		"Libraries\\MLR\\Show Bird View",
		Check_ShowBirdView,
		Toggle_ShowBirdView,
		NULL
	);

	AddDebuggerMenuItem("Libraries\\MLR\\Texture Sort", CheckTextureSort, EnableTextureSort, NULL );
	AddDebuggerMenuItem("Libraries\\MLR\\Enable Detail Texture", CheckDetailTexture, EnableDetailTexture, NULL );
	AddDebuggerMenuItem("Libraries\\MLR\\Alpha Sort", CheckAlphaSort, EnableAlphaSort, NULL );
	AddDebuggerMenuItem("Libraries\\MLR\\MultiTexture Enabled", CheckMultiTexture, EnableMultiTexture, NULL );
	AddDebuggerMenuItem("Libraries\\MLR\\LightMaps Enabled", CheckLightMaps, EnableLightMaps, NULL );

	//
	//---------------------
	// Setup the statistics
	//---------------------
	//
	StatisticFormat( "" );
	StatisticFormat( "Mid Level Renderer" );
	StatisticFormat( "==================" );
	StatisticFormat( "" );

	Initialize_Timer(Transform_Time, "Transform Time");
	Initialize_Timer(Clipping_Time, "Clipping Time");
	Initialize_Timer(GOS_Draw_Time, "GOS Draw Time");
	Initialize_Timer(Vertex_Light_Time, "Vertex Light Time");
	Initialize_Timer(LightMap_Light_Time, "LightMap Light Time");
	Initialize_Timer(Texture_Sorting_Time, "Texture Sorting Time");
	Initialize_Timer(Alpha_Sorting_Time, "Alpha Sorting Time");
	Initialize_Timer(Unlock_Texture_Time, "Unlock Texture Time");	

	AddStatistic( "MLR Primitives",			"prims",		gos_DWORD, &Number_Of_Primitives, Stat_AutoReset );
	AddStatistic( "Indices/Vertices",	"Ratio",	gos_float, &Index_Over_Vertex_Ratio, Stat_AutoReset+Stat_2DP );
	AddStatistic( "Transformed vertices",			"vertices",		gos_DWORD, &TransformedVertices, Stat_AutoReset );
	AddStatistic( "Number of alphasorted Tri",			"tri",		gos_DWORD, &NumberOfAlphaSortedTriangles, Stat_AutoReset );
	AddStatistic( "Lit vertices",			"vertices",		gos_DWORD, &LitVertices, Stat_AutoReset );
	AddStatistic( "Unclipped vertices",			"vertices",		gos_DWORD, &NonClippedVertices, Stat_AutoReset );
	AddStatistic( "Clipped vertices",			"vertices",		gos_DWORD, &ClippedVertices, Stat_AutoReset );

// Polygons in primitives which are clipped but polys are outside the viewing frustrum
	AddStatistic( "Clip: Offscreen",			"Poly",		gos_DWORD, &PolysClippedButOutside, Stat_AutoReset );
// Polygons in primitives which are clipped but polys are inside the viewing frustrum
	AddStatistic( "Clip: Onscreen",			"Poly",		gos_DWORD, &PolysClippedButInside, Stat_AutoReset );
// Polygons in primitives which are clipped, polys clipped against one plain
	AddStatistic( "Clip: One Plane",			"Poly",		gos_DWORD, &PolysClippedButOnePlane, Stat_AutoReset );
// Polygons in primitives which are clipped, polys clipped against more than one plain
	AddStatistic( "Clip: > One Plane",			"Poly",		gos_DWORD, &PolysClippedButGOnePlane, Stat_AutoReset );
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
void
	MidLevelRenderer::TerminateClasses(void)
{
	if ( !MLRLookUpLight::DefaultData ) //  make sure there is something to termiante
		return;

	MLRLookUpLight::TerminateClass();

	MLR_I_L_DeT_TMesh::TerminateClass();
	MLR_I_C_DeT_TMesh::TerminateClass();

	MLR_I_L_DT_TMesh::TerminateClass();
	MLR_I_C_DT_TMesh::TerminateClass();
	MLR_I_DT_TMesh::TerminateClass();

	MLRIndexedTriangleCloud::TerminateClass();
	MLRLineCloud::TerminateClass();

//	MLR_Terrain2::TerminateClass();
//	MLR_Terrain::TerminateClass();

	MLR_I_L_TMesh::TerminateClass();
	MLR_I_C_TMesh::TerminateClass();
	MLR_I_DeT_TMesh::TerminateClass();
	MLR_I_TMesh::TerminateClass();

	MLR_I_L_DeT_PMesh::TerminateClass();
	MLR_I_C_DeT_PMesh::TerminateClass();
	MLR_I_DeT_PMesh::TerminateClass();

	MLR_I_MT_PMesh::TerminateClass();

	MLR_I_L_DT_PMesh::TerminateClass();
	MLR_I_C_DT_PMesh::TerminateClass();
	MLR_I_DT_PMesh::TerminateClass();
	MLR_I_L_PMesh::TerminateClass();
	MLR_I_C_PMesh::TerminateClass();
	MLR_I_PMesh::TerminateClass();
	MLRIndexedPrimitiveBase::TerminateClass();
	MLRPrimitiveBase::TerminateClass();

//	MLRLightMap::TerminateClass();
	MLRSpotLight::TerminateClass();
	MLRPointLight::TerminateClass();
	MLRInfiniteLightWithFalloff::TerminateClass();
	MLRInfiniteLight::TerminateClass();
	MLRAmbientLight::TerminateClass();
	MLRCardCloud::TerminateClass();
	MLRNGonCloud::TerminateClass();
	MLRTriangleCloud::TerminateClass();
	MLRPointCloud::TerminateClass();
	MLREffect::TerminateClass();
	MLRShape::TerminateClass();
	MLRSortByOrder::TerminateClass();
	MLRSorter::TerminateClass();
	MLRClipper::TerminateClass();
	MLRTexturePool::TerminateClass();
	MLRLight::TerminateClass();

	Check_Pointer(Heap);
	gos_DestroyMemoryHeap(Heap);
	Heap = NULL;

	Check_Pointer(StaticHeap);
	gos_DestroyMemoryHeap(StaticHeap);
	StaticHeap = NULL;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
int32_t
	MidLevelRenderer::ReadMLRVersion(MemoryStream *erf_stream)
{
	Check_Object(erf_stream);

	//
	//------------------------------------------------------------------------
	// See if this file has an erf signature.  If so, the next int32_t will be the
	// version number.  If not, assume it is version 1 and rewind the file
	//------------------------------------------------------------------------
	//
	int32_t version = -1;
	int32_t erf_signature;
	*erf_stream >> erf_signature;
	if (erf_signature == 'MLR#')
		*erf_stream >> version;
	else
		erf_stream->RewindPointer(sizeof(erf_signature));
	if (version > Current_MLR_Version)
		STOP(("Application must be rebuilt to use this version of MLR data"));
	return version;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
void
	MidLevelRenderer::WriteMLRVersion(MemoryStream *erf_stream)
{
	Check_Object(erf_stream);
	*erf_stream << 'MLR#' << static_cast<int32_t>(Current_MLR_Version);
}
