//===========================================================================//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#pragma once

#ifndef MLR_MLRPOINTCLOUD_HPP
#define MLR_MLRPOINTCLOUD_HPP

//#include <mlr/mlr.hpp>
//#include <mlr/mlreffect.hpp>

namespace MidLevelRenderer {

	//##########################################################################
	//#######################    MLRLineCloud    ##############################
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
		MLRLineCloud(int, int=0);
		~MLRLineCloud();

		void
			SetData(
			pcint32_t count,
			const Stuff::Point3D *point_data,
			const Stuff::RGBAColor *color_data
			);
		virtual int
			GetType(int) { return type; }

		void Draw (DrawEffectInformation*, GOSVertexPool*, MLRSorter*);

		void Transform(int, int);

		int	Clip(MLRClippingState, GOSVertexPool*);		

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
		int type;
		pcint32_t usedNrOfVertices;
	};

}
#endif
