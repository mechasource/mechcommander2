//===========================================================================//
// Copyright (C) Microsoft Corporation. All rights reserved. //
//===========================================================================//

#pragma once

#ifndef MLR_MLRLIGHT_HPP
#define MLR_MLRLIGHT_HPP

#include <stuff/linearmatrix.hpp>
#include <stuff/color.hpp>
#include <stuff/mstring.hpp>
#include <mlr/mlrstate.hpp>
#include <mlr/mlr.hpp>

namespace Stuff{
	class Point3D;
	class Vector3D;
	class RGBAColor;
	class RGBColor;
	class MemoryStream;
	class Page;
}

namespace MidLevelRenderer{

	class GOSVertexPool;
	class MLRLightMap;

	struct MLRVertexData{
		Stuff::Point3D* point;
#if COLOR_AS_DWORD
		uint32_t* color;
#else
		Stuff::RGBAColor* color;
#endif
		Stuff::Vector3D* normal;

		int32_t index;
	};

	//##########################################################################
	//######################### MLRLight #################################
	//##########################################################################

	class MLRLight:
		public Stuff::RegisteredClass
	{
	public:
		static void __stdcall InitializeClass(void);
		static void __stdcall TerminateClass(void);

		MLRLight(ClassData *class_data);
		MLRLight(
			ClassData *class_data,
			Stuff::MemoryStream *stream,
			uint32_t version
			);
		MLRLight(
			ClassData *class_data,
			Stuff::Page* page
			);
		~MLRLight(void);

		static MLRLight*
			Make(
			Stuff::MemoryStream *stream,
			uint32_t version
			);
		static MLRLight*
			Make(Stuff::Page *page);

		virtual void
			Save(Stuff::MemoryStream *stream);
		virtual void
			Write(Stuff::Page *page);

		enum LightType{
			AmbientLight = 0,
			InfiniteLight,
			InfiniteLightWithFallOff,
			PointLight,
			SpotLight,
			LookUpLight
		};

		virtual LightType
			GetLightType() = 0;

		virtual void
			LightVertex(const MLRVertexData&) = 0;

		void SetIntensity (float _int)
		{ Check_Object(this); intensity = _int; };
		float GetIntensity ()
		{ Check_Object(this); return intensity; };

		void SetColor(Stuff::RGBColor col)
		{ Check_Object(this); color = col; }
		void SetColor(float, float, float);

		void GetColor(Stuff::RGBColor& col)
		{ Check_Object(this); col = color; };
		void GetColor(float&, float&, float&);

		void GetInWorldPosition(Stuff::Point3D& pos)
		{ Check_Object(this); pos = lightToWorld; };

		void GetInWorldDirection(Stuff::UnitVector3D& dir)
		{ Check_Object(this); lightToWorld.GetLocalForwardInWorld(&dir); };

		void GetInShapePosition(Stuff::Point3D& pos)
		{ Check_Object(this); pos = lightToShape; };

		void GetInShapePosition(Stuff::LinearMatrix4D& pos)
		{ Check_Object(this); pos = lightToShape; };

		void GetInShapeDirection(Stuff::UnitVector3D& dir)
		{ Check_Object(this); lightToShape.GetLocalForwardInWorld(&dir); };

		void SetLightToWorldMatrix(const Stuff::LinearMatrix4D&);
		const Stuff::LinearMatrix4D&
			GetLightToWorldMatrix()
		{ Check_Object(this); return lightToWorld; };

		virtual
			void SetLightToShapeMatrix(const Stuff::LinearMatrix4D&);
		const Stuff::LinearMatrix4D&
			GetLightToShapeMatrix()
		{ Check_Object(this); return lightToShape; };

		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		// Light Map Support
		//
		virtual MLRLightMap *
			GetLightMap()
		{Check_Object(this); return NULL; }

		int32_t
			GetLightMask()
		{Check_Object(this); return lightMask;}
		void
			SetDynamicLight()
		{Check_Object(this); lightMask |= MLRState::TerrainLightingMode;}
		void
			SetStaticLight()
		{Check_Object(this); lightMask &= ~MLRState::TerrainLightingMode;}

		void
			SetName(PCSTR name)
		{Check_Object(this); lightName = name;}

		PCSTR
			GetName()
		{Check_Object(this); return lightName;}

		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		// Class Data Support
		//
	public:
		static ClassData* DefaultData;

		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		// Testing
		//
	public:
		void TestInstance(void);

	protected:
		float
			intensity;

		Stuff::RGBColor
			color;

		Stuff::LinearMatrix4D
			lightToWorld, lightToShape;

		int32_t
			lightMask;

		Stuff::MString
			lightName;
	};

}
#endif
