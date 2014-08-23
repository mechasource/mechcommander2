//===========================================================================//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#pragma once

#ifndef MLR_MLR_TERRAIN2_HPP
#define MLR_MLR_TERRAIN2_HPP

//#include <mlr/mlr.hpp>
//#include <mlr/mlr_i_det_tmesh.hpp>

namespace MidLevelRenderer {

	//##########################################################################
	//#### MLRIndexedPolyMesh with no color no lighting w/ detail texture  #####
	//##########################################################################


	class MLR_Terrain2:
		public MLR_I_DeT_TMesh
	{
		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		// Initialization
		//
	public:
		static void __stdcall InitializeClass(void);
		static void __stdcall TerminateClass(void);

		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		// Constructors/Destructors
		//
	protected:
		MLR_Terrain2(
			ClassData *class_data,
			Stuff::MemoryStream *stream,
			int32_t version
			);
		~MLR_Terrain2();

	public:
		MLR_Terrain2(ClassData *class_data=MLR_Terrain2::DefaultData);

		static MLR_Terrain2*
			Make(
			Stuff::MemoryStream *stream,
			int32_t version
			);

		void
			Save(Stuff::MemoryStream *stream);

	public:
		virtual int32_t
			TransformAndClip(Stuff::Matrix4D *, MLRClippingState, GOSVertexPool*,bool=false);

		virtual void
			TransformNoClip(Stuff::Matrix4D*, GOSVertexPool*,bool=false);

		void
			SetCurrentDepth(uint8_t d);

		uint8_t
			GetCurrentDepth()
		{ Check_Object(this); return currentDepth; }

		void
			SetDepthData(uint8_t md, uint8_t mad)
		{ Check_Object(this); maxDepth = md; maxAllDepth = mad; }

		void
			SetTileData(uint8_t tx, uint8_t tz)
		{ Check_Object(this); tileX = tx; tileZ = tz; }

		void
			SetFrame(int32_t res, Scalar xMin, Scalar zMin, Scalar xMax, Scalar zMax)
		{ Check_Object(this); frame[res][0] = xMin; frame[res][1] = zMin; frame[res][2] = xMax; frame[res][3] = zMax; }

		Scalar
			GetFrame(int32_t res, int32_t p)
		{ Check_Object(this); return frame[res][p]; }

		void
			SetBorderPixel(float bp)
		{ Check_Object(this); borderPixelFun = bp; }

		void
			CalculateUVs();

		void
			SetLevelTexture(int32_t lev, int32_t handle);

		int32_t
			GetLevelTexture(int32_t lev)
		{ Check_Object(this); Verify(lev>=0 && lev<8); return textures[lev]; }

		void
			LightMapLighting(MLRLight*);

		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		// Class Data Support
		//
	public:
		static ClassData* DefaultData;

		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		// Testing
		//
	public:
		void TestInstance(void) const;

		virtual int32_t
			GetSize()
		{ 
			Check_Object(this);
			int32_t ret = MLR_I_DeT_TMesh::GetSize();

			return ret;
		}

	protected:
		int32_t textures[8];
		Scalar frame[8][4];

		uint8_t tileX, tileZ;
		uint8_t currentDepth, maxDepth, maxAllDepth;

		float borderPixelFun;

		static Stuff::DynamicArrayOf<Vector2DScalar> *detailTexCoords;	// Base address of texture coordinate list 
	};
}
#endif
