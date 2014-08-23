//===========================================================================//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#pragma once

#ifndef MLR_MLR_I_L_DT_PMESH_HPP
#define MLR_MLR_I_L_DT_PMESH_HPP

//#include <mlr/mlr.hpp>
//#include <mlr/mlr_i_c_dt_pmesh.hpp>

namespace MidLevelRenderer {

	//##########################################################################
	//###### MLRIndexedPolyMesh with color and lighting two texture layer  #####
	//##########################################################################


	class MLR_I_L_DT_PMesh:
		public MLR_I_C_DT_PMesh
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
		MLR_I_L_DT_PMesh(
			ClassData *class_data,
			Stuff::MemoryStream *stream,
			int version
			);
		~MLR_I_L_DT_PMesh();

	public:
		MLR_I_L_DT_PMesh(ClassData *class_data=MLR_I_L_DT_PMesh::DefaultData);

		static MLR_I_L_DT_PMesh*
			Make(
			Stuff::MemoryStream *stream,
			int version
			);

		void
			Save(Stuff::MemoryStream *stream);

	public:
		virtual void
			SetNormalData(
			const Stuff::Vector3D *array,
			int point_count
			);
		virtual void
			GetNormalData(
			Stuff::Vector3D **array,
			int *point_count
			);

#if COLOR_AS_DWORD
		virtual void
			SetColorData(
			const ULONG *array,
			int point_count
			);
#else
		virtual void
			SetColorData(
			const Stuff::RGBAColor *array,
			int point_count
			);
#endif

		virtual void	Lighting(MLRLight* const*, int nrLights);

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
			int ret = MLR_I_C_DT_PMesh::GetSize();
			ret += normals.GetSize();
			ret += litColors.GetSize();

			return ret;
		}

	protected:
		Stuff::DynamicArrayOf<Stuff::Vector3D> normals;		// Base address of normal list 

#if COLOR_AS_DWORD
		Stuff::DynamicArrayOf<ULONG> litColors;
		Stuff::DynamicArrayOf<ULONG> *actualColors;
#else
		Stuff::DynamicArrayOf<Stuff::RGBAColor> litColors;
		Stuff::DynamicArrayOf<Stuff::RGBAColor> *actualColors;
#endif

	};

	//	MLR_I_L_DT_PMesh*
	//		CreateIndexedCube(float, Stuff::RGBAColor*, Stuff::Vector3D*, MLRState*);
	MLRShape*
		CreateIndexedIcosahedron_Color_Lit_2Tex(
		IcoInfo&,
		MLRState*,
		MLRState*
		);
}
#endif
