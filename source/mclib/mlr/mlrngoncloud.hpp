//===========================================================================//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#pragma once

#ifndef MLR_MLRNGONCLOUD_HPP
#define MLR_MLRNGONCLOUD_HPP

//#include <mlr/mlr.hpp>
//#include <mlr/mlreffect.hpp>

namespace MidLevelRenderer {

	//##########################################################################
	//#####################    MLRNGonCloud    #############################
	//##########################################################################


	class MLRNGonCloud:
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
		MLRNGonCloud(int vertices, int number);
		~MLRNGonCloud();

		void
			SetData(
			pcint32_t count,
			const Stuff::Point3D *point_data,
			const Stuff::RGBAColor *color_data
			);

		void Draw (DrawEffectInformation*, GOSVertexPool*, MLRSorter*);

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
		int numOfVertices;
		pcint32_t usedNrOfNGons;

		static Stuff::DynamicArrayOf<Stuff::RGBAColor> *clipExtraColors;
		Stuff::DynamicArrayOf<Stuff::RGBAColor> specialClipColors;

		static Stuff::DynamicArrayOf<MLRClippingState> *clipPerVertex;

		static Stuff::DynamicArrayOf<Stuff::Vector4D> *clipExtraCoords;

		static Stuff::DynamicArrayOf<int> *clipExtraLength;
	};
}
#endif
