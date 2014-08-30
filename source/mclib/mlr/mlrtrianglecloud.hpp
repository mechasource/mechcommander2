//===========================================================================//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#pragma once

#ifndef MLR_MLRTRIANGLECLOUD_HPP
#define MLR_MLRTRIANGLECLOUD_HPP

#include <mlr/mlreffect.hpp>

namespace MidLevelRenderer {

	//##########################################################################
	//#####################    MLRTriangleCloud    #############################
	//##########################################################################


	class MLRTriangleCloud:
		public MLREffect
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
		MLRTriangleCloud(int32_t);
		~MLRTriangleCloud(void);

		void SetData(
			pcsize_t count,
			const Stuff::Point3D *point_data,
			const Stuff::RGBAColor *color_data
			);

		// added due to warning C4263 in MLRIndexedTriangleCloud
		virtual void SetData(
			pcsize_t tri_count,
			pcsize_t point_count,
			pcuint16_t index_data,
			const Stuff::Point3D* point_data,
			const Stuff::RGBAColor* color_data,
			const Stuff::Vector2DScalar* uv_data);

		void Draw (DrawEffectInformation*, GOSVertexPool*, MLRSorter*);

		uint32_t Clip(MLRClippingState, GOSVertexPool*);		

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
		pcsize_t usedNrOfTriangles;

		static Stuff::DynamicArrayOf<Stuff::RGBAColor> *clipExtraColors; // , Max_Number_Vertices_Per_Mesh

		static Stuff::DynamicArrayOf<MLRClippingState> *clipPerVertex; // , Max_Number_Vertices_Per_Mesh

		static Stuff::DynamicArrayOf<Stuff::Vector4D> *clipExtraCoords; // , Max_Number_Vertices_Per_Mesh

		static Stuff::DynamicArrayOf<int32_t> *clipExtraLength; // , Max_Number_Primitives_Per_Frame
	};
}
#endif
