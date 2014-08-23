//===========================================================================//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#pragma once

#ifndef MLR_MLR_I_C_PMESH_HPP
#define MLR_MLR_I_C_PMESH_HPP

//#include <mlr/mlr.hpp>
//#include <mlr/mlr_i_pmesh.hpp>

namespace MidLevelRenderer {

	//##########################################################################
	//#### MLRIndexedPolyMesh with color but no lighting one texture layer  ####
	//##########################################################################


	class MLR_I_C_PMesh:
		public MLR_I_PMesh
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
		MLR_I_C_PMesh(
			ClassData *class_data,
			Stuff::MemoryStream *stream,
			int32_t version
			);
		~MLR_I_C_PMesh();

	public:
		MLR_I_C_PMesh(ClassData *class_data=MLR_I_C_PMesh::DefaultData);

		static MLR_I_C_PMesh*
			Make(
			Stuff::MemoryStream *stream,
			int32_t version
			);

		void
			Save(Stuff::MemoryStream *stream);

	public:
		//		void Copy(MLRIndexedPolyMesh*);

#if COLOR_AS_DWORD
		virtual void
			SetColorData(
			pcuint32_t array,
			int32_t point_count
			);
		virtual void
			GetColorData(
			uint32_t **array,
			pint32_t point_count
			);
#else
		virtual void
			SetColorData(
			const Stuff::RGBAColor *array,
			int32_t point_count
			);
		virtual void
			GetColorData(
			Stuff::RGBAColor **array,
			pint32_t point_count
			);
#endif

		virtual void
#if COLOR_AS_DWORD
			PaintMe(pcuint32_t paintMe);
#else
			PaintMe(const Stuff::RGBAColor *paintMe);
#endif

		virtual int32_t	TransformAndClip(Stuff::Matrix4D *, MLRClippingState, GOSVertexPool*,bool=false);

		virtual void
			TransformNoClip(Stuff::Matrix4D*, GOSVertexPool*,bool=false);

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
			int32_t ret = MLR_I_PMesh::GetSize();
			ret += colors.GetSize();

			return ret;
		}

	protected:
#if COLOR_AS_DWORD
		Stuff::DynamicArrayOf<uint32_t> colors;	// Base address of color list 
#else
		Stuff::DynamicArrayOf<Stuff::RGBAColor> colors;	// Base address of color list 
#endif
	};

	MLR_I_C_PMesh*
		CreateIndexedCube_Color_NoLit(float, Stuff::RGBAColor*, MLRState*);

	MLR_I_PMesh*
		CreateIndexedViewFrustrum_Color_NoLit(
		float near_clip,
		float far_clip,
		float left_clip,
		float right_clip,
		float top_clip,
		float bottom_clip,
		Stuff::RGBAColor&, 
		MLRState*
		);

	MLRShape*
		CreateIndexedIcosahedron_Color_NoLit(
		IcoInfo&,
		MLRState*
		);
}
#endif
