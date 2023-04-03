//===========================================================================//
// File:	unitvec.hh                                                       //
// Contents: Interface specification for unit vector class                   //
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#pragma once

#ifndef _UNITVECTOR_HPP_
#define _UNITVECTOR_HPP_

#include "stuff/vector3d.h"

namespace Stuff
{

//~~~~~~~~~~~~~~~~~~~~~~~~~~ UnitVector3D ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

class LinearMatrix4D;

class UnitVector3D : public Vector3D
{
public:
	//
	// Constructors
	//
	UnitVector3D() { }
	UnitVector3D(float x, float y, float z)
		: Vector3D(x, y, z)
	{
	}
	UnitVector3D(const UnitVector3D& v)
		: Vector3D(v)
	{
	}
	explicit UnitVector3D(const Vector3D& v)
	{
		*this = v;
	}

	static const UnitVector3D Forward;
	static const UnitVector3D Backward;
	static const UnitVector3D Left;
	static const UnitVector3D Right;
	static const UnitVector3D Up;
	static const UnitVector3D Down;

	//
	// Assignment operators
	//
	UnitVector3D& operator=(const UnitVector3D& vector)
	{
		Check_Object(&vector);
		Vector3D::operator=(vector);
		return *this;
	}
	UnitVector3D& operator=(const Vector3D& v)
	{
		Vector3D::Normalize(v);
		return *this;
	}

	//
	// Math operations
	//
	UnitVector3D& Negate(const UnitVector3D& v)
	{
		Check_Object(&v);
		Vector3D::Negate(v);
		return *this;
	}

	float operator*(const Vector3D& v) const
	{
		return Vector3D::operator*(v);
	}

	//
	// Transforms
	//
	UnitVector3D& Multiply(const UnitVector3D& v, const LinearMatrix4D& m);
	UnitVector3D& operator*=(const LinearMatrix4D& m);
	UnitVector3D& MultiplyByInverse(const UnitVector3D& v, const LinearMatrix4D& m)
	{
		Vector3D::MultiplyByInverse(v, m);
		return *this;
	}

	//
	// Template support
	//
	UnitVector3D& Lerp(const UnitVector3D& v1, const UnitVector3D& v2, float t);

	//
	// Support functions
	//
	void TestInstance(void) const;
	static bool TestClass(void);

private:
	static const UnitVector3D identity;
	UnitVector3D& Negate(const Vector3D& V);
	UnitVector3D& Add(const Vector3D& V1, const Vector3D& V2);
	UnitVector3D& operator+=(const Vector3D& V);
	UnitVector3D& Subtract(const Vector3D& V1, const Vector3D& V2);
	UnitVector3D& operator-=(const Vector3D& V);
	UnitVector3D& Cross(const Vector3D& V1, const Vector3D& V2);
	UnitVector3D& Multiply(const Vector3D& V, float Scale);
	UnitVector3D& operator*=(float value);
	UnitVector3D& Multiply(const Vector3D& V1, const Vector3D& V2);
	UnitVector3D& operator*=(const Vector3D& V);
	UnitVector3D& Multiply(const Vector3D& Source, const AffineMatrix4D& M);
	UnitVector3D& MultiplyByInverse(const Vector3D& Source, const LinearMatrix4D& M);
	UnitVector3D& Divide(const Vector3D& V, float Scale);
	UnitVector3D& operator/=(float value);
	UnitVector3D& Combine(const Vector3D& V1, float t1, const Vector3D& V2, float t2);
};

inline UnitVector3D&
UnitVector3D::Multiply(const UnitVector3D& v, const LinearMatrix4D& m)
{
	Check_Object(&v);
	Vector3D::Multiply((Vector3D&)v, (AffineMatrix4D&)m);
	return *this;
}

inline UnitVector3D&
UnitVector3D::operator*=(const LinearMatrix4D& m)
{
	UnitVector3D src(*this);
	return Multiply(src, m);
}
} // namespace Stuff
#endif
