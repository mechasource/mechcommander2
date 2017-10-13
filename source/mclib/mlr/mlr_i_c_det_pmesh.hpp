//===========================================================================//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#pragma once

#ifndef MLR_MLR_I_C_DeT_PMESH_HPP
#define MLR_MLR_I_C_DeT_PMESH_HPP

#include <mlr/mlr_i_det_pmesh.hpp>

namespace MidLevelRenderer
{

	class MLR_I_C_PMesh;

//##########################################################################
//#### MLRIndexedPolyMesh with color but no lighting two texture layers ####
//##########################################################################


	class MLR_I_C_DeT_PMesh:
		public MLR_I_DeT_PMesh
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
		MLR_I_C_DeT_PMesh(
			ClassData* class_data,
			Stuff::MemoryStream* stream,
			uint32_t version);
		~MLR_I_C_DeT_PMesh(void);

	public:
		MLR_I_C_DeT_PMesh(ClassData* class_data = MLR_I_C_DeT_PMesh::DefaultData);

		static MLR_I_C_DeT_PMesh*
		Make(
			Stuff::MemoryStream* stream,
			uint32_t version);

		void
		Save(Stuff::MemoryStream* stream);

	public:
		void
		Copy(
			MLR_I_C_PMesh*,
			MLRState detailState,
			float xOff,
			float yOff,
			float xFac,
			float yFac
		);

#if COLOR_AS_DWORD
		virtual void SetColorData(
			pcuint32_t array,
			size_t point_count
		);
		virtual void GetColorData(
			puint32_t* array,
			psize_t point_count
		);
#else
		virtual void SetColorData(
			const Stuff::RGBAColor* array,
			size_t point_count
		);
		virtual void GetColorData(
			Stuff::RGBAColor** array,
			psize_t point_count
		);
#endif

		virtual void
#if COLOR_AS_DWORD
		PaintMe(pcuint32_t paintMe);
#else
		PaintMe(const Stuff::RGBAColor* paintMe);
#endif

		virtual uint32_t TransformAndClip(Stuff::Matrix4D*, MLRClippingState, GOSVertexPool*, bool = false);

		virtual void TransformNoClip(Stuff::Matrix4D*, GOSVertexPool*, bool = false);

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

		virtual size_t GetSize(void)
		{
			// Check_Object(this);
			size_t ret = MLR_I_DeT_PMesh::GetSize();
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

	MLR_I_C_DeT_PMesh*
	CreateIndexedCube_Color_NoLit_DetTex(
		float,
		Stuff::RGBAColor*,
		MLRState*,
		MLRState*,
		float xOff,
		float yOff,
		float xFac,
		float yFac
	);
	MLRShape*
	CreateIndexedIcosahedron_Color_NoLit_DetTex(
		IcoInfo&,
		MLRState*,
		MLRState*
	);
}
#endif
