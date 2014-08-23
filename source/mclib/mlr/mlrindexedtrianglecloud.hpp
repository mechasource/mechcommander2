//===========================================================================//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#pragma once

#ifndef MLR_MLRINDEXEDTRIANGLECLOUD_HPP
#define MLR_MLRINDEXEDTRIANGLECLOUD_HPP

//#include <mlr/mlr.hpp>
//#include <mlr/mlrtrianglecloud.hpp>

namespace MidLevelRenderer {

	//##########################################################################
	//##################    MLRIndexedTriangleCloud    #########################
	//##########################################################################


	class MLRIndexedTriangleCloud:
		public MLRTriangleCloud
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
	public:
		MLRIndexedTriangleCloud(int);
		~MLRIndexedTriangleCloud();

		void
			SetData(
			pcint32_t				tri_count,
			pcint32_t				point_count,
			pcuint16_t				index_data,
			const Stuff::Point3D*	point_data,
			const Stuff::RGBAColor*	color_data,
			const Vector2DScalar*	uv_data
			);

		void Draw (DrawEffectInformation*, GOSVertexPool*, MLRSorter*);

		int	Clip(MLRClippingState, GOSVertexPool*);		

		virtual puint16_t
			GetGOSIndices(int=0)
		{ Check_Object(this); return gos_indices; }

		int
			GetNumGOSIndices()
		{ Check_Object(this); return numGOSIndices; }

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

	protected:
		pcint32_t	usedNrOfPoints;
		pcuint16_t	index;
		const Vector2DScalar *texCoords;

		static Stuff::DynamicArrayOf<Vector2DScalar> *clipExtraTexCoords; // , Max_Number_Vertices_Per_Mesh

		static Stuff::DynamicArrayOf<uint16_t> *clipExtraIndex; // , Max_Number_Vertices_Per_Mesh

		static Stuff::DynamicArrayOf<uint8_t>	*visibleIndexedVertices;

		puint16_t gos_indices;
		uint16_t	numGOSIndices;
	};
}
#endif
