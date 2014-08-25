//===========================================================================//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#pragma once

#ifndef MLR_MLRPOINTCLOUD_HPP
#define MLR_MLRPOINTCLOUD_HPP

#include <mlr/mlreffect.hpp>

namespace MidLevelRenderer {

	//##########################################################################
	//#######################    MLRPointCloud    ##############################
	//##########################################################################


	class MLRPointCloud:
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
		MLRPointCloud(int32_t, int32_t=0);
		~MLRPointCloud(void);

		void
			SetData(
			pcsize_t count,
			const Stuff::Point3D *point_data,
			const Stuff::RGBAColor *color_data
			);
		virtual int32_t
			GetType(int32_t) { return type; }

		void Draw (DrawEffectInformation*, GOSVertexPool*, MLRSorter*);

		void Transform(int32_t, int32_t);

		int32_t	Clip(MLRClippingState, GOSVertexPool*);		

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
		int32_t type;
		pcint32_t usedNrOfVertices;
	};

}
#endif
