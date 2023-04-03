//===========================================================================//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#include "stdinc.h"

#include "mlr/mlrclippingstate.h"

namespace MidLevelRenderer
{

//#############################################################################
//#########################    MLRClippingState    ############################
//#############################################################################

uint32_t MLRClippingState::numberBitsLookUpTable[MLRClippingState::ClipMask + 1] = {0, 1, 1, 2, 1,
	2, 2, 3, 1, 2, 2, 3, 2, 3, 3, 4, 1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5, 1, 2, 2, 3, 2,
	3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5, 2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6};

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
void MLRClippingState::Save(std::iostream stream)
{
	// Check_Object(this);
	Check_Object(stream);
	//
	//-------------------------------------
	// Save the clippingState to the stream
	//-------------------------------------
	//
	*stream << clippingState;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
void MLRClippingState::Load(std::iostream stream)
{
	// Check_Object(this);
	Check_Object(stream);
	//
	//---------------------------------------
	// Load the clippingState from the stream
	//---------------------------------------
	//
	*stream >> clippingState;
}

} // namespace MidLevelRenderer
