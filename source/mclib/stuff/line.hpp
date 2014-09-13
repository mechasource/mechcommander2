//=======================================================================//
// File:		line.hpp                                                 //
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#pragma once

#ifndef _LINE_HPP_
#define _LINE_HPP_

#include <stuff/ray.hpp>

namespace Stuff
{

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Line3D3D ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	class Line3D:
		public Ray3D
	{
	public:
		float
		length;

		Line3D()
		{}
		Line3D(
			const Ray3D& ray,
			float length
		):
			Ray3D(ray),
			length(length)
		{}
		Line3D(
			const Point3D& start,
			const UnitVector3D& direction,
			float length
		):
			Ray3D(start, direction),
			length(length)
		{}

		//
		// Assignment operators
		//
		Line3D&
		SetDirection(const Vector3D& vector);

		void
		FindEnd(Point3D* result)
		{
			Check_Object(this);
			Check_Pointer(result);
			Ray3D::Project(length, result);
		}

		//
		// Intersection functions
		//
		float
		GetDistanceTo(
			const Plane& plane,
			float* product
		) const
		{
			Check_Object(this);
			return Ray3D::GetDistanceTo(plane, product);
		}
		float
		GetDistanceTo(
			const Sphere& sphere,
			float* penetration
		) const;
		float
		GetDistanceTo(const OBB& box);
		float
		GetDistanceTo(
			const OBB& box,
			size_t* axis);
	};

}
#endif
