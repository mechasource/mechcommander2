//===========================================================================//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#pragma once

#ifndef MLR_MLRINFINITELIGHT_HPP
#define MLR_MLRINFINITELIGHT_HPP

#include <mlr/mlrlight.hpp>

namespace MidLevelRenderer {

	//##########################################################################
	//####################    MLRInfiniteLight    ##############################
	//##########################################################################

	class MLRInfiniteLight:
		public MLRLight
	{
	public:
		static void __stdcall InitializeClass(void);
		static void __stdcall TerminateClass(void);

		MLRInfiniteLight(ClassData *class_data=DefaultData);
		MLRInfiniteLight(
			ClassData *class_data,
			Stuff::MemoryStream *stream,
			uint32_t version
			);
		MLRInfiniteLight(
			ClassData *class_data,
			Stuff::Page *page
			);
		~MLRInfiniteLight(void);

		virtual void
			LightVertex(const MLRVertexData&);

		virtual LightType
			GetLightType() 
		{ Check_Object(this); return InfiniteLight; }

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
	};

}
#endif
