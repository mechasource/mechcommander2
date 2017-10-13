//===========================================================================//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#pragma once

#ifndef MLR_MLRSORTBYORDER_HPP
#define MLR_MLRSORTBYORDER_HPP

#include <mlr/gosvertex.hpp>
#include <mlr/mlrsorter.hpp>

namespace MidLevelRenderer
{

class MLRState;

struct SortAlpha
{
	float distance;
	MLRState* state;
	GOSVertex triangle[3];
};

//##########################################################################
//######################    MLRSortByOrder    ##############################
//##########################################################################

class MLRSortByOrder : public MLRSorter
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
	~MLRSortByOrder(void);

	void AddPrimitive(MLRPrimitiveBase*, uint32_t = 0);
	void AddEffect(MLREffect*, const MLRState&);
	void AddScreenQuads(GOSVertex*, const DrawScreenQuadsInformation*);

	virtual void AddSortRawData(SortData*);

	//	starts the action
	void RenderNow(void);

	//	resets the sorting
	void Reset(void);

	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Testing
	//
  public:
	void TestInstance(void) const;

  protected:
	int32_t lastUsedInBucket[MLRState::PriorityCount];

	Stuff::DynamicArrayOf<SortData*> priorityBuckets
		[MLRState::PriorityCount]; //, Max_Number_Primitives_Per_Frame +
								   //Max_Number_ScreenQuads_Per_Frame
	Stuff::DynamicArrayOf<SortAlpha*>
		alphaSort; //, Max_Number_Primitives_Per_Frame +
				   //Max_Number_ScreenQuads_Per_Frame
};
}
#endif
