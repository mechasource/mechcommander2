//===========================================================================//
// Copyright (C) Microsoft Corporation. All rights reserved. //
//===========================================================================//

/* this header is not used */

#pragma once

#ifndef MLR_MLRINDEXEDPOLYMESH_HPP
#define MLR_MLRINDEXEDPOLYMESH_HPP

#include <stuff/memoryblock.hpp>
#include <stuff/plane.hpp>
#include <mlr/mlrindexedprimitive.hpp>

namespace Stuff{
	class MemoryBlock;
	class MemoryStream;
	class Plane;
	class Line3D;
	class Normal3D;
}

namespace MidLevelRenderer{

	//##########################################################################
	//#################### MLRIndexedPolyMesh ############################
	//##########################################################################


	class MLRIndexedPolyMesh:
		public MLRIndexedPrimitive
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
		MLRIndexedPolyMesh(Stuff::MemoryStream *stream, uint32_t version);
		~MLRIndexedPolyMesh(void);

	public:
		MLRIndexedPolyMesh(void);

		static MLRIndexedPolyMesh* Make(Stuff::MemoryStream *stream, uint32_t version);
		void Save(Stuff::MemoryStream *stream);

		PVOID operator new(size_t size)
		{
			Verify(size == sizeof(MLRIndexedPolyMesh));
			return AllocatedMemory->New();
		}
		void operator delete(PVOID where)
		{
			AllocatedMemory->Delete(where);
		}

	private:
		static Stuff::MemoryBlock* AllocatedMemory;

	public:
		virtual void InitializeDrawPrimitive(int32_t, int32_t=0);

		virtual void SetPrimitiveLength(puint8_t , int32_t);
		virtual void GetPrimitiveLength(puint8_t *, pint32_t);

		void FindFacePlanes(void);

		virtual int32_t FindBackFace(const Stuff::Point3D&);

		const Stuff::Plane* GetPolygonPlane(uint32_t i)
		{
			Check_Object(this);
			Verify(i<facePlanes.GetLength());

			return &facePlanes[i];
		}

		virtual void Lighting(MLRLight**, uint32_t nrLights);

		MLRPrimitive* LightMapLighting(MLRLight*);

		virtual int32_t Clip(MLRClippingState, GOSVertexPool*);

		bool CastRay(Stuff::Line3D* line, Stuff::Normal3D* normal);

		void Transform(Stuff::Matrix4D*);

		virtual void TransformNoClip(Stuff::Matrix4D*, GOSVertexPool*);

		// Initializes the visibility test list
		void ResetTestList(void);

		// find which vertices are visible which not - returns nr of visible vertices
		// the result is stored in the visibleIndexedVertices array
		int32_t FindVisibleVertices(void);

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
			size_t ret = MLRIndexedPrimitive::GetSize();
			ret += testList.GetSize();
			ret += facePlanes.GetSize();

			return ret;
		}

	protected:
		Stuff::DynamicArrayOf<uint8_t> testList;
		Stuff::DynamicArrayOf<Stuff::Plane> facePlanes;

	};

	MLRIndexedPolyMesh* CreateIndexedCube(
		float, Stuff::RGBAColor*, Stuff::Vector3D*, MLRState*);
}
#endif
