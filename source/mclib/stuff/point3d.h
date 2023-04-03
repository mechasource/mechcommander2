//===========================================================================//
// File:	point.hh                                                         //
// Contents: Interface specification of the point class                      //
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#pragma once

#ifndef _POINT3D_HPP_
#define _POINT3D_HPP_

#include "stuff/vector3d.h"

namespace Stuff
{

class Origin3D;
class Vector4D;

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Point3D ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

class Point3D : public Vector3D
{
public:
	static const Point3D Identity; // ??? strange

	//
	// Constructors
	//
	Point3D(void) { }
	Point3D(float x, float y, float z)
		: Vector3D(x, y, z)
	{
	}
	Point3D(const Point3D& p)
		: Vector3D(p)
	{
	}
	explicit Point3D(const Vector3D& v)
	{
		*this = v;
	}
	explicit Point3D(const Vector4D& v)
	{
		*this = v;
	}
	explicit Point3D(const Origin3D& origin)
	{
		*this = origin;
	}
	explicit Point3D(const AffineMatrix4D& matrix)
	{
		*this = matrix;
	}
	explicit Point3D(const YawPitchRange& polar)
	{
		*this = polar;
	}

	//
	// Assignment operators
	//
	Point3D& operator=(const Vector3D& v)
	{
		Vector3D::operator=(v);
		return *this;
	}
	Point3D& operator=(const Vector4D& v);
	Point3D& operator=(const Origin3D& p);
	Point3D& operator=(const AffineMatrix4D& matrix);
	Point3D& operator=(const YawPitchRange& polar)
	{
		Vector3D::operator=(polar);
		return *this;
	}

	//
	// Math operators
	//
	Point3D& Negate(const Vector3D& v)
	{
		Vector3D::Negate(v);
		return *this;
	}

	Point3D& Add(const Vector3D& v1, const Vector3D& v2)
	{
		Vector3D::Add(v1, v2);
		return *this;
	}
	Point3D& operator+=(const Vector3D& v)
	{
		return Add(*this, v);
	}
	Point3D& Subtract(const Point3D& p, const Vector3D& v)
	{
		Vector3D::Subtract(p, v);
		return *this;
	}
	Point3D& operator-=(const Vector3D& v)
	{
		return Subtract(*this, v);
	}

	float operator*(const Vector3D& v) const
	{
		return Vector3D::operator*(v);
	}

	Point3D& Multiply(const Vector3D& v, float scale)
	{
		Vector3D::Multiply(v, scale);
		return *this;
	}
	Point3D& Multiply(const Point3D& p, float scale)
	{
		Vector3D::Multiply(p, scale);
		return *this;
	}
	Point3D& operator*=(float value)
	{
		return Multiply(*this, value);
	}
	Point3D& Multiply(const Point3D& p, const Vector3D& v)
	{
		Vector3D::Multiply(p, v);
		return *this;
	}
	Point3D& operator*=(const Vector3D& v)
	{
		return Multiply(*this, v);
	}

	Point3D& Divide(const Vector3D& v, float scale)
	{
		Vector3D::Divide(v, scale);
		return *this;
	}
	Point3D& operator/=(float value)
	{
		return Divide(*this, value);
	}

	Point3D& Divide(const Vector3D& v1, const Vector3D& v2)
	{
		Vector3D::Divide(v1, v2);
		return *this;
	}
	Point3D& operator/=(const Vector3D& v)
	{
		return Divide(*this, v);
	}

	//
	// Transforms
	//
	Point3D& Multiply(const Vector3D& v, const AffineMatrix4D& m)
	{
		Vector3D::Multiply(v, m);
		return *this;
	}
	Point3D& Multiply(const Point3D& p, const AffineMatrix4D& m);
	Point3D& operator*=(const AffineMatrix4D& m)
	{
		Point3D src(*this);
		return Multiply(src, m);
	}
	Point3D& MultiplyByInverse(const Vector3D& v, const LinearMatrix4D& m)
	{
		Vector3D::MultiplyByInverse(v, m);
		return *this;
	}
	Point3D& MultiplyByInverse(const Point3D& p, const LinearMatrix4D& m);

	//
	// Miscellaneous functions
	//
	Point3D& Combine(const Vector3D& v1, float t1, const Vector3D& v2, float t2)
	{
		Vector3D::Combine(v1, t1, v2, t2);
		return *this;
	}

	//
	// Template support functions
	//
	Point3D& Lerp(const Vector3D& v1, const Vector3D& v2, float t)
	{
		Vector3D::Lerp(v1, v2, t);
		return *this;
	}

	static bool TestClass(void);
};
} // namespace Stuff
// #include "stuff/affinematrix.h"
#endif
