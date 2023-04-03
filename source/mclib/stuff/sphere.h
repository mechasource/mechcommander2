//===========================================================================//
// File:		sphere.hpp                                                   //
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//
#pragma once

#ifndef _SPHERE_HPP_
#define _SPHERE_HPP_

#include "stuff/point3d.h"

namespace Stuff
{
class Sphere;
}

#if !defined(Spew)
void Spew(std::wstring_view group, const Stuff::Sphere& sphere);
#endif

namespace Stuff
{

class Plane;
class OBB;

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Sphere ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

class Sphere
{
public:
	Point3D center;
	float radius;

	Sphere() { }
	Sphere(const Point3D& A_Point, float Radius)
		: center(A_Point)
		, radius(Radius)
	{
	}
	Sphere(float X, float Y, float Z, float Radius)
		: center(X, Y, Z)
		, radius(Radius)
	{
	}
	Sphere(const Sphere& sphere)
		: center(sphere.center)
		, radius(sphere.radius)
	{
	}
	explicit Sphere(const OBB& obb)
	{
		*this = obb;
	}

	Sphere& operator=(const Sphere& sphere)
	{
		// Check_Pointer(this);
		Check_Object(&sphere);
		center = sphere.center;
		radius = sphere.radius;
		return *this;
	}
	Sphere& operator=(const OBB& obb);

	Sphere& Union(const Sphere& sphere1, const Sphere& sphere2);

	//
	// Intersection functions
	//
	bool Contains(const Point3D& point) const
	{
		// Check_Object(this);
		Check_Object(&point);
		Vector3D diff;
		diff.Subtract(center, point);
		return radius * radius - diff.GetLengthSquared() > -SMALL;
	}
	bool Intersects(const Sphere& sphere) const
	{
		// Check_Object(this);
		Check_Object(&sphere);
		float r = radius + sphere.radius;
		Vector3D temp;
		temp.Subtract(center, sphere.center);
		return r * r - temp.GetLengthSquared() >= -SMALL;
	}
	bool Intersects(const Plane& plane) const;

#if !defined(Spew)
	friend void ::Spew(std::wstring_view group, const Sphere& sphere);
#endif
	void TestInstance(void) const
	{
	}
};
} // namespace Stuff
#endif
