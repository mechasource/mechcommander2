//===========================================================================//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#pragma once

#ifndef MLR_MLREFFECT_HPP
#define MLR_MLREFFECT_HPP

#include <stuff/marray.hpp>
#include <stuff/vector2d.hpp>
#include <stuff/vector4d.hpp>
#include <stuff/color.hpp>
#include <stuff/linearmatrix.hpp>
#include <mlr/mlrclippingstate.hpp>

//#include <mlr/mlr.hpp>
//#include <mlr/mlrstate.hpp>

namespace MidLevelRenderer
{

	struct EffectClipPolygon
	{
		void Init(void);
		void Destroy(void);

		Stuff::DynamicArrayOf<Stuff::Vector4D> coords;			//[Max_Number_Vertices_Per_Polygon];
		Stuff::DynamicArrayOf<Stuff::RGBAColor> colors;			//[Max_Number_Vertices_Per_Polygon];
		Stuff::DynamicArrayOf<Stuff::Vector2DScalar> texCoords;	//[Max_Number_Vertices_Per_Polygon];
		Stuff::DynamicArrayOf<MLRClippingState> clipPerVertex;	//[Max_Number_Vertices_Per_Polygon];
	};

	class DrawEffectInformation;
	class GOSVertex;
	class GOSVertexPool;
	class MLRSorter;

//##########################################################################
//#########################    MLREffect   #################################
//##########################################################################

	class MLREffect :
		public Stuff::RegisteredClass
	{
	public:
		static void __stdcall InitializeClass(void);
		static void __stdcall TerminateClass(void);

		MLREffect(uint32_t nr, ClassData* class_data);
		~MLREffect(void);

		virtual void SetData(
			pcsize_t pcount,
			const Stuff::Point3D* point_data,
			const Stuff::RGBAColor* color_data) = 0;

		virtual uint32_t GetType(uint32_t)
		{
			return 0;
		}

		//	add another effect
		virtual void Draw(DrawEffectInformation*, GOSVertexPool*, MLRSorter*) = 0;
		virtual void Transform(size_t, size_t);

		// switches single/all effects on or off
		void TurnAllOn(void);
		void TurnAllOff(void);
		void TurnOn(uint32_t nr)
		{
			Check_Object(this);
			Verify(nr < maxNrOf);
			testList[nr] |= 2;
		}
		void TurnOff(uint32_t nr)
		{
			Check_Object(this);
			Verify(nr < maxNrOf);
			testList[nr] &= ~2;
		}
		bool IsOn(size_t nr)
		{
			Check_Object(this);
			Verify(nr < maxNrOf);
			return (testList[nr] & 2) ? true : false;
		}

		virtual uint32_t Clip(MLRClippingState, GOSVertexPool*) = 0;

		void SetEffectToClipMatrix(
			const Stuff::LinearMatrix4D* effectToWorld,
			const Stuff::Matrix4D* worldToClipMatrix)
		{
			Check_Object(this);
			effectToClipMatrix.Multiply(*effectToWorld, *worldToClipMatrix);
		}

		GOSVertex* GetGOSVertices(void)
		{
			Check_Object(this);
			return gos_vertices;
		}

		size_t GetNumGOSVertices(void)
		{
			Check_Object(this);
			return numGOSVertices;
		}

		uint32_t GetSortDataMode(void)
		{
			Check_Object(this);
			return drawMode;
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
		void TestInstance(void) const {};

	protected:
		static EffectClipPolygon* clipBuffer;

		void TurnAllVisible(void);
		void TurnAllInVisible(void);
		void TurnVisible(uint32_t nr)
		{
			Check_Object(this);
			Verify(nr < maxNrOf);
			testList[nr] |= 1;
		}
		void TurnInVisible(uint32_t nr)
		{
			Check_Object(this);
			Verify(nr < maxNrOf);
			testList[nr] &= ~1;
		}
		uint32_t visible;
		uint32_t maxNrOf;

		const Stuff::Point3D* points;
		const Stuff::RGBAColor* colors;
		static Stuff::DynamicArrayOf<Stuff::Vector4D>* transformedCoords;
		Stuff::DynamicArrayOf<int32_t>	testList;

		uint32_t drawMode;

		Stuff::LinearMatrix4D worldToEffect;
		Stuff::Matrix4D effectToClipMatrix;

		GOSVertex* gos_vertices;
		uint16_t numGOSVertices;
	};

	struct EffectClipData
	{
		Stuff::Vector4D*		coords;
		Stuff::RGBAColor*		colors;
		Stuff::Vector2DScalar*	texCoords;
		MLRClippingState*		clipPerVertex;

		uint32_t flags;
		uint32_t length;
	};


}
#endif
