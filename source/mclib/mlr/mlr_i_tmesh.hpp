//===========================================================================//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#pragma once

#ifndef MLR_MLR_I_TMESH_HPP
#define MLR_MLR_I_TMESH_HPP

//#include <mlr/mlr.hpp>
//#include <mlr/mlr_i_pmesh.hpp>

namespace MidLevelRenderer {

	//##########################################################################
	//#### MLRIndexedTriMesh with no color no lighting one texture layer  #####
	//##########################################################################


	class MLR_I_TMesh:
		public MLRIndexedPrimitiveBase
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
		MLR_I_TMesh(
			ClassData *class_data,
			Stuff::MemoryStream *stream,
			int32_t version
			);
		~MLR_I_TMesh(void);

	public:
		MLR_I_TMesh(ClassData *class_data=MLR_I_TMesh::DefaultData);

		static MLR_I_TMesh*
			Make(
			Stuff::MemoryStream *stream,
			int32_t version
			);

		void
			Save(Stuff::MemoryStream *stream);

	public:
		virtual	void	InitializeDrawPrimitive(uint8_t, int32_t=0);

		virtual int32_t
			GetNumPrimitives()
		{ Check_Object(this); return numOfTriangles; }

		virtual void
			SetSubprimitiveLengths(
			puint8_t length_array,
			int32_t subprimitive_count
			)
		{
			Check_Object(this);(void)length_array;
			Verify(gos_GetCurrentHeap() == Heap);
			numOfTriangles = subprimitive_count;
			testList.SetLength(numOfTriangles);
			facePlanes.SetLength(numOfTriangles);
		}

		void	FindFacePlanes();

		virtual int32_t	FindBackFace(const Stuff::Point3D&);

		const Stuff::Plane *GetTrianglePlane(size_t i)
		{
			Check_Object(this);
			Verify(i<facePlanes.GetLength());

			return &facePlanes[i];
		}

		virtual void	Lighting(MLRLight* const*, int32_t nrLights);

		virtual void LightMapLighting(MLRLight*);

		virtual void
#if COLOR_AS_DWORD
			PaintMe(pcuint32_t  paintMe) {(void)paintMe;};
#else
			PaintMe(const Stuff::RGBAColor* paintMe) {(void)paintMe;};
#endif

		virtual int32_t	TransformAndClip(Stuff::Matrix4D *, MLRClippingState, GOSVertexPool*,bool=false);

		bool
			CastRay(
			Stuff::Line3D *line,
			Stuff::Normal3D *normal
			);

		//		void
		//			Transform(Stuff::Matrix4D*);

		virtual void
			TransformNoClip(Stuff::Matrix4D*, GOSVertexPool*,bool=false);

		//	Initializes the visibility test list
		void
			ResetTestList();

		//	find which vertices are visible which not - returns nr of visible vertices
		//	the result is stored in the visibleIndexedVertices array
		int32_t
			FindVisibleVertices();

		bool
			Copy(MLR_I_PMesh*);

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
			int32_t ret = MLRIndexedPrimitiveBase::GetSize();
			ret += testList.GetSize();
			ret += facePlanes.GetSize();

			return ret;
		}

	protected:
		int32_t numOfTriangles;
		Stuff::DynamicArrayOf<uint8_t>	testList;
		Stuff::DynamicArrayOf<Stuff::Plane> facePlanes;
	};

#define ICO_X 0.525731112119133606f
#define ICO_Z 0.850650808352039932f

	extern float vdata[12][3];
	extern uint32_t tindices [20][3];
	extern int32_t triDrawn;
	MLR_I_TMesh* CreateIndexedTriCube_NoColor_NoLit(float, MLRState*);
	MLRShape* CreateIndexedTriIcosahedron_NoColor_NoLit(IcoInfo&, MLRState*);
}
#endif
