//===========================================================================//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

/* this header is not used */

#pragma once

#ifndef MLR_MLRPOLYMESH_HPP
#define MLR_MLRPOLYMESH_HPP

#include <stuff/plane.hpp>
#include <mlr/mlrprimitive.hpp>

namespace Stuff{
	class Plane;
}

namespace MidLevelRenderer {

	//##########################################################################
	//########################    MLRPolyMesh    ###############################
	//##########################################################################


	class MLRPolyMesh:
		public MLRPrimitive
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
		MLRPolyMesh(
			Stuff::MemoryStream *stream,
			uint32_t version
			);
		~MLRPolyMesh(void);

	public:
		MLRPolyMesh(void);

		static MLRPolyMesh* Make(Stuff::MemoryStream *stream, uint32_t version);

		void Save(Stuff::MemoryStream *stream);

		virtual void	SetPrimitiveLength(puint8_t , size_t);
		virtual void	GetPrimitiveLength(puint8_t *, psize_t);

		void	FindFacePlanes(void);

		virtual int32_t	FindBackFace(const Stuff::Point3D&);

		const Stuff::Plane* GetPolygonPlane(size_t index)
		{
			Check_Object(this);
			Verify(index < facePlanes.GetLength());

			return &facePlanes[index];
		}

		virtual void Lighting(MLRLight**, uint32_t nrLights);

		virtual int32_t	Clip(MLRClippingState, GOSVertexPool*);

		void
			Transform(Stuff::Matrix4D*);

		virtual void
			TransformNoClip(Stuff::Matrix4D*, GOSVertexPool*);

		//	Initializes the visibility test list
		void
			ResetTestList(void);

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
			Check_Object(this);
			size_t ret = MLRPrimitive::GetSize();
			ret += testList.GetSize();
			ret += facePlanes.GetSize();

			return ret;
		}

	protected:
		Stuff::DynamicArrayOf<uint8_t>	testList;
		Stuff::DynamicArrayOf<Stuff::Plane> facePlanes;

	};
	MLRPolyMesh* CreateCube(float, Stuff::RGBAColor*, Stuff::Vector3D*, MLRState*);

}
#endif
