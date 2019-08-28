//===========================================================================//
// File:	plane.hh                                                         //
// Contents: Interface specification of the plane class                      //
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#pragma once

#ifndef _PLANE_HPP_
#define _PLANE_HPP_

#include "stuff/normal.h"
#include "stuff/point3d.h"
//#include "stuff/marray.h"

namespace Stuff
{
class Plane;
}

#if !defined(Spew)
void
Spew(const std::wstring_view& group, const Stuff::Plane& plane);
#endif

namespace Stuff
{

class Sphere;
class ExtentBox;
class OBB;
class Line3D;

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Plane ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

class Plane
{
public:
	//
	// The plane equation is P*normal = offset, where P is a homogeneous
	//
	Normal3D normal;
	float offset;

	//
	// Constructors
	//
	Plane(void) {}
	Plane(float x, float y, float z, float offset) :
		normal(x, y, z), offset(offset) {}
	Plane(const Normal3D& n, float offset) :
		normal(n), offset(offset) {}
	Plane(const Point3D& p0, const Point3D& p1, const Point3D& p2) { BuildPlane(p0, p1, p2); }

	void BuildPlane(const Point3D& p0, const Point3D& p1, const Point3D& p2);

	//
	// Transform functions
	//
	Plane& Multiply(const Plane& p, const LinearMatrix4D& m);

	Plane& operator*=(const LinearMatrix4D& m)
	{
		// Check_Object(this);
		Plane t(*this);
		return Multiply(t, m);
	}

	//
	// half-space division functions
	//
	bool IsSeenBy(const Vector3D& A_Vector) const
	{
		return ((normal * A_Vector) < 0.0) ? true : false;
	}
	bool IsSeenBy(const Point3D& A_Point) const
	{
		return ((normal * A_Point) > offset) ? true : false;
	}
	float GetDistanceTo(const Point3D& A_Point) const { return ((normal * A_Point) - offset); }

	float GetDistanceTo(const Sphere& sphere) const;
	float GetDistanceTo(const OBB& box) const;

	//
	// half-space containment functions
	//
	bool Contains(const Point3D& point, float thickness = SMALL) const
	{
		// Check_Object(this);
		return normal * point <= offset + thickness;
	}
	bool ContainsSomeOf(const Sphere& sphere, float thickness = SMALL) const;
	bool ContainsAllOf(const Sphere& sphere, float thickness = SMALL) const;
	bool ContainsSomeOf(const ExtentBox& box, float thickness = SMALL) const;
	bool ContainsAllOf(const ExtentBox& box, float thickness = SMALL) const;
	bool ContainsSomeOf(const OBB& box, float thickness = SMALL) const;
	bool ContainsAllOf(const OBB& box, float thickness = SMALL) const;

	//
	// plane surface intersection functions
	//
	bool Intersects(const Sphere& sphere, float thickness = SMALL) const;
	bool Intersects(const ExtentBox& box, float thickness = SMALL) const;
	bool Intersects(const OBB& box, float thickness = SMALL) const;

#if !defined(Spew)
	friend void ::Spew(const std::wstring_view& group, const Plane& plane);
#endif
	void TestInstance(void) const
	{
	}
	static bool TestClass(void);

	//
	// Equation solutions
	//
	float CalculateX(float y, float z)
	{
		// Check_Object(this);
		_ASSERT(!Small_Enough(normal.x));
		float result = (offset - y * normal.y - z * normal.z) / normal.x;
		return result;
	}
	float CalculateY(float x, float z)
	{
		// Check_Object(this);
		_ASSERT(!Small_Enough(normal.y));
		float result = (offset - x * normal.x - z * normal.z) / normal.y;
		return result;
	}
	float CalculateZ(float x, float y)
	{
		// Check_Object(this);
		_ASSERT(!Small_Enough(normal.z));
		float result = (offset - x * normal.x - y * normal.y) / normal.z;
		return result;
	}

	//
	// Reflection code
	//
	void Reflect(Vector3D* vector)
	{
		// Check_Object(this);
		vector->AddScaled(*vector, normal, -2.0f * ((*vector) * normal));
	}
	bool ComputeBestDividingPlane(std::vector<Stuff::Point3D>& points);
};
} // namespace Stuff
#endif
