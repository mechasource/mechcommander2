//===========================================================================//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#pragma once

#ifndef MLR_MLRINFINITELIGHTWITHFALLOFF_HPP
#define MLR_MLRINFINITELIGHTWITHFALLOFF_HPP

#include <mlr/mlrlight.hpp>

namespace MidLevelRenderer {

	//##########################################################################
	//##############    MLRInfiniteLightWithFalloff    #########################
	//##########################################################################

	class MLRInfiniteLightWithFalloff:
		public MLRLight
	{
	public:
		static void __stdcall InitializeClass(void);
		static void __stdcall TerminateClass(void);

		MLRInfiniteLightWithFalloff(ClassData *class_data=MLRInfiniteLightWithFalloff::DefaultData);
		MLRInfiniteLightWithFalloff(
			ClassData *class_data,
			Stuff::MemoryStream *stream,
			uint32_t version
			);
		MLRInfiniteLightWithFalloff(
			ClassData *class_data,
			Stuff::Page *page
			);
		~MLRInfiniteLightWithFalloff(void);

		void
			Save(Stuff::MemoryStream *stream);
		void
			Write(Stuff::Page *page);

		virtual void
			LightVertex(const MLRVertexData&);

		virtual LightType
			GetLightType() 
		{ Check_Object(this); return InfiniteLightWithFallOff; }

		//
		// light falloff.  The light is infinite if the GetFalloffDistance
		// function return false.  Lights default to infinite unless
		// SetFalloffDistance is called
		//
		void
			SetFalloffDistance(
			float n,
			float f
			);
		bool
			GetFalloffDistance(
			float& n,
			float& f
			);

		inline float
			GetFalloffNear()
		{ Check_Object(this); return innerRadius; }

		inline float
			GetFalloffFar()
		{ Check_Object(this); return outerRadius; }

		bool
			GetFalloff(const float& length, float& falloff)
		{
			Check_Object(this);

			Verify(length>0.0f);

			if(length <= innerRadius)
			{
				falloff = 1.0f;
				return true;
			}

			if (length >= outerRadius)
			{
				return false;
			}

			Verify(outerRadius - innerRadius > Stuff::SMALL);
			falloff = (outerRadius - length) * oneOverDistance;

			return true;
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
		void TestInstance(void);

	protected:
		float
			innerRadius, outerRadius, oneOverDistance;

	};
}
#endif
