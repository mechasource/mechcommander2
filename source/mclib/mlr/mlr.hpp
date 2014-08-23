//===========================================================================//
// File:	MLRStuff.hpp                                                     //
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#pragma once

#ifndef MLR_MLR_HPP
#define MLR_MLR_HPP

#include <stuff/memorystream.hpp>

#ifndef _GAMEOS_HPP_
typedef struct	gos_Heap*		HGOSHEAP;
#endif

namespace MidLevelRenderer
{
	typedef enum __mlr_version {
		Current_MLR_Version = 9
	};

	struct Limits {
		static uint32_t
			Max_Number_Vertices_Per_Frame,
			Max_Number_Primitives_Per_Frame,
			Max_Number_ScreenQuads_Per_Frame,
			Max_Size_Of_LightMap_MemoryStream;

		typedef enum __mlr_hpp_constants {
			Max_Number_Vertices_Per_Mesh = 1024,
			Max_Number_Vertices_Per_Polygon = 32,
			Max_Number_Of_Texture_Bits = 14,
			Max_Number_Textures = 1 << Max_Number_Of_Texture_Bits,
			Max_Number_Of_NGon_Vertices = 9,
			Max_Number_Of_Multitextures	= 8,
			Max_Number_Of_Lights_Per_Primitive = 16,
			Max_Number_Of_FogStates = 4
		};
	};

	int32_t __stdcall ReadMLRVersion(Stuff::MemoryStream *erf_stream);
	void __stdcall WriteMLRVersion(Stuff::MemoryStream *erf_stream);

	void __stdcall InitializeClasses(
		uint32_t Max_Number_Vertices_Per_Frame = 8192,
		uint32_t Max_Number_Primitives_Per_Frame = 1024,
		uint32_t Max_Number_ScreenQuads_Per_Frame = 512,
		uint32_t Max_Size_Of_LightMap_MemoryStream = 32768,
		bool Convert_To_Triangle_Meshes = true
		);
	void __stdcall TerminateClasses(void);

	extern HGOSHEAP Heap;
	extern HGOSHEAP StaticHeap;

	extern bool ConvertToTriangleMeshes;

	DECLARE_TIMER(extern, Scene_Draw_Time);
	DECLARE_TIMER(extern, Transform_Time);
	DECLARE_TIMER(extern, Clipping_Time);
	DECLARE_TIMER(extern, GOS_Draw_Time);
	DECLARE_TIMER(extern, Vertex_Light_Time);
	DECLARE_TIMER(extern, LightMap_Light_Time);
	DECLARE_TIMER(extern, Texture_Sorting_Time);
	DECLARE_TIMER(extern, Alpha_Sorting_Time);
	DECLARE_TIMER(extern, Unlock_Texture_Time);

	extern uint32_t Number_Of_Primitives;
	extern uint32_t NumAllIndices;
	extern uint32_t NumAllVertices;
	extern float Index_Over_Vertex_Ratio;
	extern uint32_t TransformedVertices;
	extern uint32_t NumberOfAlphaSortedTriangles;
	extern uint32_t LitVertices;
	extern uint32_t NonClippedVertices;
	extern uint32_t ClippedVertices;
	extern uint32_t PolysClippedButOutside;
	extern uint32_t PolysClippedButInside;
	extern uint32_t PolysClippedButOnePlane;
	extern uint32_t PolysClippedButGOnePlane;
	extern bool PerspectiveMode;
}

#define COLOR_AS_DWORD	0
#define	EFECT_CLIPPED	0
#define FOG_HACK		1

#define	TO_DO	Abort_Program("Here has work to be done !");
#undef	MLR_TRACE

//#include <mlr/mlrstate.hpp>
//#include <mlr/mlrtexture.hpp>
//#include <mlr/mlrtexturepool.hpp>
//#include <mlr/mlrclippingstate.hpp>
//#include <mlr/mlrclipper.hpp>
//#include <mlr/mlrsorter.hpp>
//#include <mlr/mlrsortbyorder.hpp>
//#include <mlr/mlrlight.hpp>
//#include <mlr/mlrshape.hpp>
//#include <mlr/mlreffect.hpp>

#endif