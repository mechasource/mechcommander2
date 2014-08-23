//===========================================================================//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#pragma once

#ifndef MLR_MLR_I_C_TMESH_HPP
#define MLR_MLR_I_C_TMESH_HPP

//#include <mlr/mlr.hpp>
//#include <mlr/mlr_i_tmesh.hpp>
//#include <mlr/mlr_i_c_pmesh.hpp>

namespace MidLevelRenderer {

	//##########################################################################
	//#### MLRIndexedTriMesh with no color no lighting one texture layer  #####
	//##########################################################################


	class MLR_I_C_TMesh:
		public MLR_I_TMesh
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
		MLR_I_C_TMesh(
			ClassData *class_data,
			Stuff::MemoryStream *stream,
			int version
			);
		~MLR_I_C_TMesh();

	public:
		MLR_I_C_TMesh(ClassData *class_data=MLR_I_C_TMesh::DefaultData);

		static MLR_I_C_TMesh*
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

		bool
			Copy(MLR_I_C_PMesh*);

		void
			HurtMe(const Stuff::LinearMatrix4D& pain, float radius);

		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		// This functions using the static buffers
		//
		void
#if COLOR_AS_DWORD
			SetClipColor(ULONG &color, size_t index)
#else
			SetClipColor(Stuff::RGBAColor &color, size_t index)
#endif
		{
			Check_Object(this); Verify(clipExtraColors->GetLength() > index);
			(*clipExtraColors)[index] = color;
		}

		void
			FlashClipColors(size_t num)
		{
			Check_Object(this); Verify(clipExtraTexCoords->GetLength() > num);
			colors.SetLength(num);
			visibleIndexedVertices.SetLength(num);
#if COLOR_AS_DWORD
			Mem_Copy(colors.GetData(), clipExtraColors->GetData(), 
				sizeof(ULONG)*num, sizeof(ULONG)*num);
#else
			Mem_Copy(colors.GetData(), clipExtraColors->GetData(), 
				sizeof(Stuff::RGBAColor)*num, sizeof(Stuff::RGBAColor)*num);
#endif
		}
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
			int ret = MLR_I_TMesh::GetSize();
			ret += colors.GetSize();

			return ret;
		}

	protected:
#if COLOR_AS_DWORD
		Stuff::DynamicArrayOf<ULONG> colors;	// Base address of color list 
		Stuff::DynamicArrayOf<ULONG> *actualColors;
#else
		Stuff::DynamicArrayOf<Stuff::RGBAColor> colors;	// Base address of color list 
		Stuff::DynamicArrayOf<Stuff::RGBAColor> *actualColors;
#endif
	};

	MLR_I_C_TMesh*
		CreateIndexedTriCube_Color_NoLit(float, MLRState*);
	MLRShape*
		CreateIndexedTriIcosahedron_Color_NoLit(
		IcoInfo&,
		MLRState*
		);

}

#endif	// MLR_MLR_I_C_TMESH_HPP