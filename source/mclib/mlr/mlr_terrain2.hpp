//===========================================================================//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#pragma once
#define MLR_MLR_TERRAIN2_HPP

#if !defined(MLR_MLR_HPP)
	#include <mlr/mlr.hpp>
#endif

#if !defined(MLR_MLR_I_DeT_TMESH_HPP)
	#include <mlr/mlr_i_det_tmesh.hpp>
#endif

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
		static void
			InitializeClass();
		static void
			TerminateClass();

	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Constructors/Destructors
	//
	protected:
		MLR_Terrain2(
			ClassData *class_data,
			Stuff::MemoryStream *stream,
			int version
		);
		~MLR_Terrain2();

	public:
		MLR_Terrain2(ClassData *class_data=MLR_Terrain2::DefaultData);

		static MLR_Terrain2*
			Make(
				Stuff::MemoryStream *stream,
				int version
			);

		void
			Save(Stuff::MemoryStream *stream);

	public:
		virtual int
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
			SetFrame(int res, Scalar xMin, Scalar zMin, Scalar xMax, Scalar zMax)
				{ Check_Object(this); frame[res][0] = xMin; frame[res][1] = zMin; frame[res][2] = xMax; frame[res][3] = zMax; }

		Scalar
			GetFrame(int res, int p)
				{ Check_Object(this); return frame[res][p]; }

		void
			SetBorderPixel(Stuff::Scalar bp)
				{ Check_Object(this); borderPixelFun = bp; }

		void
			CalculateUVs();

		void
			SetLevelTexture(int lev, int handle);

		int
			GetLevelTexture(int lev)
				{ Check_Object(this); Verify(lev>=0 && lev<8); return textures[lev]; }

		void
			LightMapLighting(MLRLight*);

	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Class Data Support
	//
	public:
		static ClassData
			*DefaultData;

	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Testing
	//
	public:
		void
			TestInstance() const;

	virtual int
			GetSize()
		{ 
			Check_Object(this);
			int ret = MLR_I_DeT_TMesh::GetSize();

			return ret;
		}

	protected:
		int textures[8];
		Scalar frame[8][4];

		uint8_t tileX, tileZ;
		uint8_t currentDepth, maxDepth, maxAllDepth;

		Stuff::Scalar borderPixelFun;

		static Stuff::DynamicArrayOf<Vector2DScalar> *detailTexCoords;	// Base address of texture coordinate list 
	};
}
