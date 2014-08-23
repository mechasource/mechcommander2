//===========================================================================//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#pragma once

#ifndef MLR_MLR_I_C_DT_PMESH_HPP
#define MLR_MLR_I_C_DT_PMESH_HPP

//#include <mlr/mlr.hpp>
//#include <mlr/mlr_i_dt_pmesh.hpp>

namespace MidLevelRenderer {

	//##########################################################################
	//#### MLRIndexedPolyMesh with color but no lighting two texture layers ####
	//##########################################################################


	class MLR_I_C_DT_PMesh:
		public MLR_I_DT_PMesh
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
		MLR_I_C_DT_PMesh(
			ClassData *class_data,
			Stuff::MemoryStream *stream,
			int version
			);
		~MLR_I_C_DT_PMesh();

	public:
		MLR_I_C_DT_PMesh(ClassData *class_data=MLR_I_C_DT_PMesh::DefaultData);

		static MLR_I_C_DT_PMesh*
			Make(
			Stuff::MemoryStream *stream,
			int version
			);

		void
			Save(Stuff::MemoryStream *stream);

	public:
#if COLOR_AS_DWORD
		virtual void
			SetColorData(
			const ULONG *array,
			int point_count
			);
		virtual void
			GetColorData(
			ULONG **array,
			int *point_count
			);
#else
		virtual void
			SetColorData(
			const Stuff::RGBAColor *array,
			int point_count
			);
		virtual void
			GetColorData(
			Stuff::RGBAColor **array,
			int *point_count
			);
#endif

		virtual void
#if COLOR_AS_DWORD
			PaintMe(const ULONG *paintMe);
#else
			PaintMe(const Stuff::RGBAColor *paintMe);
#endif

		virtual int	TransformAndClip(Stuff::Matrix4D *, MLRClippingState, GOSVertexPool*,bool=false);

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

		virtual int
			GetSize()
		{ 
			Check_Object(this);
			int ret = MLR_I_DT_PMesh::GetSize();
			ret += colors.GetSize();

			return ret;
		}

	protected:
#if COLOR_AS_DWORD
		Stuff::DynamicArrayOf<ULONG> colors;	// Base address of color list 
#else
		Stuff::DynamicArrayOf<Stuff::RGBAColor> colors;	// Base address of color list 
#endif
	};

	MLR_I_C_DT_PMesh*
		CreateIndexedCube_Color_NoLit_2Tex(float, Stuff::RGBAColor*, MLRState*, MLRState*);
	MLRShape*
		CreateIndexedIcosahedron_Color_NoLit_2Tex(
		IcoInfo&,
		MLRState*,
		MLRState*
		);
}
#endif
