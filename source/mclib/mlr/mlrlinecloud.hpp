//===========================================================================//
// Copyright (C) Microsoft Corporation. All rights reserved. //
//===========================================================================//

#pragma once

#ifndef MLR_MLRLINECLOUD_HPP
#define MLR_MLRLINECLOUD_HPP

#include <mlr/mlreffect.hpp>

namespace MidLevelRenderer{

	//##########################################################################
	//####################### MLRLineCloud ##############################
	//##########################################################################


	class MLRLineCloud:
		public MLREffect
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
	public:
		MLRLineCloud(uint32_t, uint32_t=0);
		~MLRLineCloud(void);


		void SetData(
			pcsize_t count, 
			const Stuff::Point3D* point_data, 
			const Stuff::RGBAColor *color_data);

		virtual uint32_t GetType(uint32_t){ return type; }
		void Draw (DrawEffectInformation*, GOSVertexPool*, MLRSorter*);
		void Transform(size_t, size_t);
		uint32_t Clip(MLRClippingState, GOSVertexPool*);

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

	protected:
		uint32_t type;
		pcsize_t usedNrOfVertices;
	};

}
#endif
