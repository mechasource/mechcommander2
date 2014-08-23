//===========================================================================//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#pragma once

#ifndef MLR_MLRSORTBYORDER_HPP
#define MLR_MLRSORTBYORDER_HPP

//#include <mlr/mlr.hpp>
//#include <mlr/mlrstate.hpp>

namespace MidLevelRenderer {

	struct SortAlpha {
		float distance;
		MLRState *state;
		GOSVertex triangle[3];
	};

	//##########################################################################
	//######################    MLRSortByOrder    ##############################
	//##########################################################################

	class MLRSortByOrder :
		public MLRSorter
	{
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Initialization
	//
	public:
		static void __stdcall InitializeClass(void);
		static void __stdcall TerminateClass(void);
		static ClassData* DefaultData;

	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Constructors/Destructors
	//
	public:
		MLRSortByOrder(MLRTexturePool*);
		~MLRSortByOrder();

		void AddPrimitive(MLRPrimitiveBase*, int32_t=0);
		void AddEffect(MLREffect*, const MLRState&);
		void AddScreenQuads(GOSVertex*, const DrawScreenQuadsInformation*);

		virtual void AddSortRawData(SortData*);

		//	starts the action
		void RenderNow ();

		//	resets the sorting
		void Reset ();

	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Testing
	//
	public:
		void TestInstance(void) const;

	protected:
		int32_t
			lastUsedInBucket[MLRState::PriorityCount];

		Stuff::DynamicArrayOf<SortData*>  //, Max_Number_Primitives_Per_Frame + Max_Number_ScreenQuads_Per_Frame
			priorityBuckets[MLRState::PriorityCount];

		Stuff::DynamicArrayOf<SortAlpha*>  //, Max_Number_Primitives_Per_Frame + Max_Number_ScreenQuads_Per_Frame
			alphaSort;
	};

}
#endif
