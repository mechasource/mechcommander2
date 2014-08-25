//===========================================================================//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#pragma once

#ifndef MLR_MLRSHAPE_HPP
#define MLR_MLRSHAPE_HPP

#include <stuff/plug.hpp>
#include <stuff/matrix.hpp>
#include <stuff/marray.hpp>
#include <mlr/mlrstate.hpp>

namespace Stuff {
	class LinearMatrix4D;
	class Line3D;
	class Normal3D;
	class Point3D;
}

namespace MidLevelRenderer {

	class MLRPrimitiveBase;
	class MLRClipper;
	class MLRLight;

	//##########################################################################
	//##########################    MLRShape    ################################
	//##########################################################################
	// This class is a container for MLRPrimitve's. A shape has a Matrix and is
	// attached to the hierarchy

	class MLRShape :
		public Stuff::Plug
	{
		friend class MLRClipper;

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
		MLRShape(
			Stuff::MemoryStream *stream,
			uint32_t version
			);
		~MLRShape(void);

	public:
		MLRShape(int32_t);

		static MLRShape*
			Make(
			Stuff::MemoryStream *stream,
			uint32_t version
			);

		void
			Save(Stuff::MemoryStream *stream);

	public:
		void Add (MLRPrimitiveBase*);
		MLRPrimitiveBase* Find (int32_t);
		int32_t Find (MLRPrimitiveBase*);

		// use this functions with care --- they are slow
		MLRPrimitiveBase *Remove(MLRPrimitiveBase*);
		MLRPrimitiveBase *Remove(int32_t);
		int32_t Insert(MLRPrimitiveBase*, int32_t);

		bool
			Replace(MLRPrimitiveBase*, MLRPrimitiveBase*);

		// returns the number of primitives in the container
		size_t GetNum(void)
		{ Check_Object(this); return numPrimitives; };

		// returns the number of faces overall in the shape
		size_t GetNumPrimitives(void);

		// returns the number of drawn triangles in the shape
		int32_t GetNumDrawnTriangles(void);

		// is to call at begin of every frame 
		void	InitializePrimitives(uint8_t, const MLRState& master, int32_t=0);

		// clips the geometry and fills the data into the vertex pool
		// the clipping states defines the planes against the shape might have be culled
		//	now done only on primitive level - int32_t	Clip(MLRClippingState, GOSVertexPool*);

		// lights the geometry, uses the worldToShape matrix and an array of lights which
		// affect the shape in this frame and the number of lights in this array
		void	Lighting(const Stuff::LinearMatrix4D&, MLRLight* const*, uint32_t nrLights);

		// casts an ray against the geometry contained in shape
		bool CastRay(Stuff::Line3D* line, Stuff::Normal3D* normal);

		void HurtMe(const Stuff::LinearMatrix4D&, float radius);

		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		// Class Data Support
		//
	public:
		static ClassData* DefaultData;

		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		// Reference counting
		//
	public:
		void
			AttachReference()
		{Check_Object(this); ++referenceCount;}
		void
			DetachReference()
		{
			Check_Object(this); Verify(referenceCount > 0);
			if ((--referenceCount) == 0)
			{
				Unregister_Object(this);
				delete this;
			}
		}

		int32_t
			GetReferenceCount()
		{return referenceCount;}

	protected:
		int32_t
			referenceCount;

		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		// Testing
		//
	public:
		void TestInstance(void) const {};
		virtual size_t GetSize()
		{
			Check_Object(this); 
			size_t ret = allPrimitives.GetSize();

			return ret;
		}

	protected:
		int32_t FindBackFace(const Stuff::Point3D&);

		//		void
		//			Transform(Stuff::Matrix4D*);

		//		void
		//			Transform(void);

		Stuff::DynamicArrayOf<MLRPrimitiveBase*>	allPrimitives;
		const Stuff::LinearMatrix4D*				worldToShape;
		Stuff::Matrix4D								shapeToClipMatrix;

	private:
		size_t	numPrimitives;
	};

}
#endif
