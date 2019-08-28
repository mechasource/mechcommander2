//===========================================================================//
// File:	point3d.cc                                                       //
// Contents: Implementation details for the point class                      //
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#include "stdinc.h"
//#include "stuff/stuffheaders.h"

////#include "gameos.hpp"
#include "stuff/scalar.h"
#include "stuff/affinematrix.h"
#include "stuff/linearmatrix.h"
#include "stuff/matrix.h"
#include "stuff/vector4d.h"
#include "stuff/point3d.h"

using namespace Stuff;

const Point3D Point3D::Identity(0.0f, 0.0f, 0.0f);

//
//###########################################################################
//###########################################################################
//
Point3D&
Point3D::operator=(const Vector4D& v)
{
	// Check_Pointer(this);
	Check_Object(&v);
	_ASSERT(!Small_Enough(v.w));
	float scale = 1.0f / v.w;
	x = v.x * scale;
	y = v.y * scale;
	z = v.z * scale;
	return *this;
}

//
//###########################################################################
//###########################################################################
//
Point3D&
Point3D::Multiply(const Point3D& p, const AffineMatrix4D& m)
{
	// Check_Pointer(this);
	Check_Object(&p);
	Check_Object(&m);
	_ASSERT(this != &p);
	x = p.x * m(0, 0) + p.y * m(1, 0) + p.z * m(2, 0) + m(3, 0);
	y = p.x * m(0, 1) + p.y * m(1, 1) + p.z * m(2, 1) + m(3, 1);
	z = p.x * m(0, 2) + p.y * m(1, 2) + p.z * m(2, 2) + m(3, 2);
	return *this;
}

//
//###########################################################################
//###########################################################################
//
Point3D&
Point3D::MultiplyByInverse(const Point3D& p, const LinearMatrix4D& m)
{
	// Check_Pointer(this);
	Check_Object(&p);
	Check_Object(&m);
	_ASSERT(this != &p);
	x = p.x * m(0, 0) + p.y * m(0, 1) + p.z * m(0, 2) - m(3, 0) * m(0, 0) - m(3, 1) * m(0, 1) - m(3, 2) * m(0, 2);
	y = p.x * m(1, 0) + p.y * m(1, 1) + p.z * m(1, 2) - m(3, 0) * m(1, 0) - m(3, 1) * m(1, 1) - m(3, 2) * m(1, 2);
	z = p.x * m(2, 0) + p.y * m(2, 1) + p.z * m(2, 2) - m(3, 0) * m(2, 0) - m(3, 1) * m(2, 1) - m(3, 2) * m(2, 2);
	return *this;
}
