//===========================================================================//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#pragma once

#ifndef MLR_MLR_I_PMESH_HPP
#define MLR_MLR_I_PMESH_HPP

//#include <mlr/mlr.hpp>
//#include <mlr/mlrindexedprimitivebase.hpp>

namespace MidLevelRenderer {

	//##########################################################################
	//#### MLRIndexedPolyMesh with no color no lighting one texture layer  #####
	//##########################################################################


	class MLR_I_PMesh:
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
		MLR_I_PMesh(
			ClassData *class_data,
			Stuff::MemoryStream *stream,
			int32_t version
			);
		~MLR_I_PMesh();

	public:
		MLR_I_PMesh(ClassData *class_data=MLR_I_PMesh::DefaultData);

		static MLR_I_PMesh*
			Make(
			Stuff::MemoryStream *stream,
			int32_t version
			);

		void
			Save(Stuff::MemoryStream *stream);

	public:
		//		void Copy(MLRIndexedPolyMesh*);

		virtual	void	InitializeDrawPrimitive(uint8_t, int32_t=0);

		virtual void	SetSubprimitiveLengths(puint8_t , int32_t);
		virtual void	GetSubprimitiveLengths(puint8_t *, pint32_t);

		void	FindFacePlanes();

		virtual int32_t	FindBackFace(const Stuff::Point3D&);

		const Stuff::Plane *GetPolygonPlane(size_t i)
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

		virtual int32_t	TransformAndClip(Stuff::Matrix4D*, MLRClippingState, GOSVertexPool*,bool=false);

		bool
			CastRay(
			Stuff::Line3D *line,
			Stuff::Normal3D *normal
			);

		virtual void
			TransformNoClip(Stuff::Matrix4D*, GOSVertexPool*,bool=false);

		//	Initializes the visibility test list
		void
			ResetTestList();

		//	find which vertices are visible which not - returns nr of visible vertices
		//	the result is stored in the visibleIndexedVertices array
		int32_t
			FindVisibleVertices();

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
		void
			Transform(Stuff::Matrix4D*);

		Stuff::DynamicArrayOf<uint8_t>	testList;

		Stuff::DynamicArrayOf<Stuff::Plane> facePlanes;

	};

#define ICO_X 0.525731112119133606f
#define ICO_Z 0.850650808352039932f

	extern float vdata[12][3];
	extern uint32_t tindices [20][3];

	extern int32_t triDrawn;

	void
		subdivide (
		Stuff::Point3D *coords,
		Stuff::Point3D& v1,
		Stuff::Point3D& v2,
		Stuff::Point3D& v3,
		int32_t depth,
		int32_t tri2draw, 
		float rad = 1.0f
		);

	MLR_I_PMesh*
		CreateIndexedCube_NoColor_NoLit(float, MLRState*);
	MLRShape*
		CreateIndexedIcosahedron_NoColor_NoLit(
		IcoInfo&,
		MLRState*
		);

}
#endif
