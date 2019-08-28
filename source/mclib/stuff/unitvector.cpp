//===========================================================================//
// File:	unitvec.cc                                                       //
// Contents: Implementation details for unit vector class                    //
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#include "stdinc.h"
//#include "stuff/stuffheaders.h"

////#include "gameos.hpp"
#include "stuff/scalar.h"
#include "stuff/unitvector.h"

using namespace Stuff;

const UnitVector3D UnitVector3D::Forward(FORWARD_X, FORWARD_Y, FORWARD_Z);
const UnitVector3D UnitVector3D::Backward(BACKWARD_X, BACKWARD_Y, BACKWARD_Z);
const UnitVector3D UnitVector3D::Left(LEFT_X, LEFT_Y, LEFT_Z);
const UnitVector3D UnitVector3D::Right(RIGHT_X, RIGHT_Y, RIGHT_Z);
const UnitVector3D UnitVector3D::Up(UP_X, UP_Y, UP_Z);
const UnitVector3D UnitVector3D::Down(DOWN_X, DOWN_Y, DOWN_Z);

//
//#############################################################################
//#############################################################################
//
UnitVector3D&
UnitVector3D::Lerp(const UnitVector3D& v1, const UnitVector3D& v2, float t)
{
	// Check_Pointer(this);
	Check_Object(&v1);
	Check_Object(&v2);
	//
	//-------------------------------------------------------
	// Lerp the normals as a regular vector (yeah, I know)...
	//-------------------------------------------------------
	//
	Vector3D result;
	result.Lerp(v1, v2, t);
	Normalize(result);
	// Check_Object(this);
	return *this;
}

//
//#############################################################################
//#############################################################################
//
void
UnitVector3D::TestInstance(void) const
{
#ifdef _ARMOR
	float length = Vector3D::GetLengthSquared();
	float diff = 1.0f - length;
	// _ASSERT(Small_Enough(diff,3e-5f));
	_ASSERT(Small_Enough(diff));
#endif
}
