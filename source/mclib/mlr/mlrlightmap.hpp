//===========================================================================//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#pragma once

#ifndef MLR_MLRLIGHTMAP_HPP
#define MLR_MLRLIGHTMAP_HPP

//#include <mlr/mlr.hpp>

namespace MidLevelRenderer {

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
		~MLRLightMap();

		enum MemoryStreamData {
			Matrix4D=0,
			ClippingState,
			MasterRenderState,
			LightMapRenderState,
			Polygon,
			PolygonWithColor
		};

		static void
			DrawLightMaps(MLRSorter*);

		static void
			SetDrawData 
			(
			GOSVertexPool*,
			Stuff::Matrix4D*,
			MLRClippingState&,
			MLRState&
			);

		static MLRShape*
			CreateLightMapShape();

		void
			SetState(MLRState new_state)
		{ Check_Object(this); state = new_state; }
		MLRState
			GetState()
		{ Check_Object(this); return state; }

		inline void
			SetPolygonMarker (int type)
		{ Check_Object(this); Check_Object(stream); *stream << (type ? ((int)((MemoryStreamData)PolygonWithColor)) : ((int)((MemoryStreamData)Polygon)) ); }

		inline void
			AddColor(Stuff::RGBAColor color)
		{ Check_Object(this); Check_Object(stream); *stream << color; }

		inline void
			AddColor(float red, float green, float blue, float alpha)
		{ Check_Object(this); Check_Object(stream); *stream << red << green << blue << alpha; }

		inline void
			AddCoord(Stuff::Point3D coord)
		{ Check_Object(this); Check_Object(stream); *stream << coord; }

		inline void
			AddUVs(float u, float v)
		{ Check_Object(this); Check_Object(stream); *stream << u << v; }

		inline void
			AddInt(int i)
		{ Check_Object(this); Check_Object(stream); *stream << i; }

		inline void
			AddUShort(uint16_t i)
		{ Check_Object(this); Check_Object(stream); *stream << i; }

		inline void
			AddState(int priority)
		{
			Check_Object(this);
			Check_Object(stream);  
			*stream << (int)LightMapRenderState;
			state.SetPriority(priority);
			state.Save(stream);
		}

		const Vector2DScalar*
			GetCurrentUVPointer()
		{ Check_Object(this); Check_Object(stream); return Cast_Pointer(Vector2DScalar*, stream->GetPointer()); }

		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		// Class Data Support
		//
	public:
		static ClassData* DefaultData;

		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		// Testing
		//
	public:
		void
			TestInstance();

	protected:
		static ClipPolygon2
			*clipBuffer;

		MLRState
			state;

		static Stuff::MemoryStream
			*stream;
		static GOSVertexPool*
			vertexPool;
	};

}
#endif
