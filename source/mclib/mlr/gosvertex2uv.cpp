//===========================================================================//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#include "stdinc.h"

#include "mlr/gosvertex2uv.h"

namespace MidLevelRenderer {

//#############################################################################
//##########################    GOSVertex2UV   ################################
//#############################################################################

GOSVertex2UV::GOSVertex2UV()
{
	x = 0.0f;
	y = 0.0f;
	z = 0.0f;
	rhw = 1.0f;
	argb = 0xffffffff;
	u1 = 0.0f;
	v1 = 0.0f;
	u2 = 0.0f;
	v2 = 0.0f;
	frgb = 0xffffffff;
}

} // namespace MidLevelRenderer
