//===========================================================================//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#pragma once

#ifndef MLR_MLR_I_L_TMESH_HPP
#define MLR_MLR_I_L_TMESH_HPP

//#include <mlr/mlr.hpp>
//#include <mlr/mlr_i_c_tmesh.hpp>
//#include <mlr/mlr_i_l_pmesh.hpp>

namespace MidLevelRenderer {

	//##########################################################################
	//#### MLRIndexedTriMesh with no color no lighting one texture layer  #####
	//##########################################################################


	class MLR_I_L_TMesh:
		public MLR_I_C_TMesh
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
		MLR_I_L_TMesh(
			ClassData *class_data,
			Stuff::MemoryStream *stream,
			uint32_t version
			);
		~MLR_I_L_TMesh(void);

	public:
		MLR_I_L_TMesh(ClassData *class_data=MLR_I_L_TMesh::DefaultData);

		static MLR_I_L_TMesh*
			Make(
			Stuff::MemoryStream *stream,
			uint32_t version
			);

		void
			Save(Stuff::MemoryStream *stream);

	public:
		virtual void
			SetNormalData(
			const Stuff::Vector3D *array,
			int32_t point_count
			);
		virtual void
			GetNormalData(
			Stuff::Vector3D **array,
			pint32_t point_count
			);

#if COLOR_AS_DWORD
		virtual void
			SetColorData(
			pcuint32_t array,
			int32_t point_count
			);
#else
		virtual void
			SetColorData(
			const Stuff::RGBAColor *array,
			int32_t point_count
			);
#endif

		virtual void	Lighting(MLRLight* const*, int32_t nrLights);

		virtual void
#if COLOR_AS_DWORD
			PaintMe(pcuint32_t paintMe);
#else
			PaintMe(const Stuff::RGBAColor *paintMe);
#endif

		virtual int32_t	
			TransformAndClip(Stuff::Matrix4D *, MLRClippingState, GOSVertexPool*,bool=false);

		virtual void
			TransformNoClip(Stuff::Matrix4D*, GOSVertexPool*,bool=false);

		bool
			Copy(MLR_I_L_PMesh*);

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
			int32_t ret = MLR_I_C_TMesh::GetSize(void);
			ret += normals.GetSize(void);
			ret += litColors.GetSize(void);

			return ret;
		}

	protected:
		Stuff::DynamicArrayOf<Stuff::Vector3D> normals;		// Base address of normal list 

#if COLOR_AS_DWORD
		Stuff::DynamicArrayOf<uint32_t> litColors;
#else
		Stuff::DynamicArrayOf<Stuff::RGBAColor> litColors;
#endif
	};

	//	MLR_I_L_TMesh*
	//		CreateIndexedTriCube_NoColor_NoLit(float, MLRState*);
	MLRShape*
		CreateIndexedTriIcosahedron_Color_Lit(
		IcoInfo&,
		MLRState*
		);

}
#endif
