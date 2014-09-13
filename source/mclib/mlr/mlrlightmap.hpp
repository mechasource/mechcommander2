//===========================================================================//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#pragma once

#ifndef MLR_MLRLIGHTMAP_HPP
#define MLR_MLRLIGHTMAP_HPP

#include <stuff/vector2d.hpp>
#include <mlr/mlrstate.hpp>
#include <mlr/mlrprimitivebase.hpp>

namespace Stuff
{
	class Point3D;
	class RGBAColor;
	class RGBColor;
	class Matrix4D;
}

namespace MidLevelRenderer
{

	class MLRTexture;
	class MLRSorter;
	class MLRClippingState;
	class MLRShape;
	class GOSVertexPool;

//##########################################################################
//#########################    MLRLightMap    ##############################
//##########################################################################

	class MLRLightMap:
		public Stuff::RegisteredClass
	{
	public:
		static void __stdcall InitializeClass(void);
		static void __stdcall TerminateClass(void);

		MLRLightMap(MLRTexture*);
		~MLRLightMap(void);

		enum MemoryStreamData
		{
			Matrix4D = 0,
			ClippingState,
			MasterRenderState,
			LightMapRenderState,
			Polygon,
			PolygonWithColor
		};

		static void DrawLightMaps(MLRSorter*);

		static void SetDrawData(GOSVertexPool*, Stuff::Matrix4D*, MLRClippingState&, MLRState&);

		static MLRShape* CreateLightMapShape(void);

		void SetState(MLRState new_state)
		{
			Check_Object(this);
			state = new_state;
		}
		MLRState GetState(void)
		{
			Check_Object(this);
			return state;
		}

		inline void SetPolygonMarker(int32_t type)
		{
			Check_Object(this);
			Check_Object(stream);
			*stream << (type ? ((int32_t)((MemoryStreamData)PolygonWithColor))
						: ((int32_t)((MemoryStreamData)Polygon)));
		}
		inline void AddColor(Stuff::RGBAColor color)
		{
			Check_Object(this);
			Check_Object(stream);
			*stream << color;
		}

		inline void AddColor(float red, float green, float blue, float alpha)
		{
			Check_Object(this);
			Check_Object(stream);
			*stream << red << green << blue << alpha;
		}

		inline void AddCoord(Stuff::Point3D coord)
		{
			Check_Object(this);
			Check_Object(stream);
			*stream << coord;
		}

		inline void AddUVs(float u, float v)
		{
			Check_Object(this);
			Check_Object(stream);
			*stream << u << v;
		}

		inline void AddInt(int32_t i)
		{
			Check_Object(this);
			Check_Object(stream);
			*stream << i;
		}

		inline void AddUShort(uint16_t i)
		{
			Check_Object(this);
			Check_Object(stream);
			*stream << i;
		}

		inline void AddState(uint32_t priority)
		{
			Check_Object(this);
			Check_Object(stream);
			*stream << (int32_t)LightMapRenderState;
			state.SetPriority(priority);
			state.Save(stream);
		}

		const Stuff::Vector2DScalar* GetCurrentUVPointer(void)
		{
			Check_Object(this);
			Check_Object(stream);
			return Cast_Pointer(Stuff::Vector2DScalar*, stream->GetPointer());
		}

		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		// Class Data Support
		//
	public:
		static ClassData* DefaultData;

		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		// Testing
		//
	public:
		void TestInstance(void);

	protected:
		static ClipPolygon2* clipBuffer;

		MLRState state;

		static Stuff::MemoryStream* stream;
		static GOSVertexPool* vertexPool;
	};

}
#endif
