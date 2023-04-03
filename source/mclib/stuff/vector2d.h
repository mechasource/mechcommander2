//===========================================================================//
// File:	vector.hh                                                        //
// Contents: Interface specification for vector classes                      //
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#pragma once

#ifndef _VECTOR2D_HPP_
#define _VECTOR2D_HPP_

#include "stuff/style.h"

namespace Stuff
{

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Vector2D ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

template <class T>
class Vector2DOf
{
public:
	// static const Vector2DOf<T>
	// Identity;

	T x;
	T y;

	Vector2DOf(void) { }
	Vector2DOf(T x, T y)
		: x(x)
		, y(y)
	{
	}
	Vector2DOf(const Vector2DOf<T>& v)
	{
		x = v.x;
		y = v.y;
	}

	friend bool Small_Enough(const Vector2DOf<T>& v, float e);
	bool operator!(void) const
	{
		return Small_Enough(*this, SMALL);
	}

	friend bool Close_Enough(const Vector2DOf<T>& v1, const Vector2DOf<T>& v2, float e);
	bool operator==(const Vector2DOf<T>& v) const
	{
		return Close_Enough(*this, v, SMALL);
	}
	bool operator!=(const Vector2DOf<T>& v) const
	{
		return !Close_Enough(*this, v, SMALL);
	}

	const T& operator[](size_t index) const
	{
		// Check_Object(this);
		_ASSERT(static_cast<uint32_t>(index) <= Y_Axis);
		return (&x)[index];
	}
	T& operator[](size_t index)
	{
		// Check_Object(this);
		_ASSERT(static_cast<uint32_t>(index) <= Y_Axis);
		return (&x)[index];
	}

	//
	//-----------------------------------------------------------------------
	// The following operators all assume that this points to the destination
	// of the operation results
	//-----------------------------------------------------------------------
	//
	Vector2DOf<T>& Negate(const Vector2DOf<T>& v)
	{
		// Check_Object(this);
		Check_Object(&v);
		x = -v.x;
		y = -v.y;
		return *this;
	}

	Vector2DOf<T>& Add(const Vector2DOf<T>& v1, const Vector2DOf<T>& v2)
	{
		// Check_Object(this);
		Check_Object(&v1);
		Check_Object(&v2);
		x = v1.x + v2.x;
		y = v1.y + v2.y;
		return *this;
	}

	Vector2DOf<T>& operator+=(const Vector2DOf<T>& v)
	{
		return Add(*this, v);
	}
	Vector2DOf<T>& Subtract(const Vector2DOf<T>& v1, const Vector2DOf<T>& v2)
	{
		// Check_Object(this);
		Check_Object(&v1);
		Check_Object(&v2);
		x = v1.x - v2.x;
		y = v1.y - v2.y;
		return *this;
	}

	Vector2DOf<T>& operator-=(const Vector2DOf<T>& v)
	{
		return Subtract(*this, v);
	}

	T operator*(const Vector2DOf<T>& v) const
	{
		// Check_Object(this);
		Check_Object(&v);
		return x * v.x + y * v.y;
	}

	Vector2DOf<T>& Multiply(const Vector2DOf<T>& v, T scale)
	{
		// Check_Object(this);
		Check_Object(&v);
		x = v.x * scale;
		y = v.y * scale;
		return *this;
	}

	Vector2DOf<T>& operator*=(float v)
	{
		return Multiply(*this, v);
	}
	Vector2DOf<T>& Multiply(const Vector2DOf<T>& v1, const Vector2DOf<T>& v2)
	{
		// Check_Object(this);
		Check_Object(&v1);
		Check_Object(&v2);
		x = v1.x * v2.x;
		y = v1.y * v2.y;
		return *this;
	}

	Vector2DOf<T>& operator*=(const Vector2DOf<T>& v)
	{
		return Multiply(*this, v);
	}
	Vector2DOf<T>& Divide(const Vector2DOf<T>& v, T scale)
	{
		// Check_Object(this);
		Check_Object(&v);
		_ASSERT(!Small_Enough(static_cast<float>(scale)));
		x = v.x / scale;
		y = v.y / scale;
		return *this;
	}

	Vector2DOf<T>& operator/=(T v)
	{
		return Divide(*this, v);
	}
	Vector2DOf<T>& Divide(const Vector2DOf<T>& v1, const Vector2DOf<T>& v2)
	{
		// Check_Object(this);
		Check_Object(&v1);
		Check_Object(&v2);
		_ASSERT(!Small_Enough(static_cast<float>(v2.x)));
		_ASSERT(!Small_Enough(static_cast<float>(v2.y)));
		x = v1.x / v2.x;
		y = v1.y / v2.y;
		return *this;
	}

	Vector2DOf<T>& operator/=(const Vector2DOf<T>& v)
	{
		return Divide(*this, v);
	}
	T GetLengthSquared(void) const
	{
		// Check_Object(this);
		return operator*(*this);
	}
	float GetLength(void) const
	{
		// Check_Object(this);
		return Sqrt(static_cast<float>(GetLengthSquared()));
	}

	float GetApproximateLength(void) const
	{
		// Check_Object(this);
		return SqrtApproximate(static_cast<float>(GetLengthSquared()));
	}

	Vector2DOf<T>& Normalize(const Vector2DOf<T>& v)
	{
		// Check_Pointer(this);
		Check_Object(&v);
		float len = v.GetLength(void);
		_ASSERT(!Small_Enough(len));
		x = v.x / len;
		y = v.y / len;
		return *this;
	}

	Vector2DOf<T>& Combine(const Vector2DOf<T>& v1, float t1, const Vector2DOf<T>& v2, float t2)
	{
		// Check_Object(this);
		Check_Object(&v1);
		Check_Object(&v2);
		x = static_cast<T>(v1.x * t1 + v2.x * t2);
		y = static_cast<T>(v1.y * t1 + v2.y * t2);
		return *this;
	}

	void Zero(void)
	{
		x = y = 0;
	}

	Vector2DOf<T>& Lerp(const Vector2DOf<T>& v1, const Vector2DOf<T>& v2, float t)
	{
		// Check_Object(this);
		Check_Object(&v1);
		Check_Object(&v2);
		x = static_cast<T>(v1.x + t * (v2.x - v1.x));
		y = static_cast<T>(v1.y + t * (v2.y - v1.y));
		return *this;
	}

#if !defined(Spew)
	friend void Spew(std::wstring_view group, const Vector2DOf<T>& vector);
#endif
	void TestInstance(void) const
	{
	}
};

// template <class T> const Vector2DOf<T>
// Vector2DOf<T>::Identity(0.0f,0.0f);

template <class T>
bool Small_Enough(const Vector2DOf<T>& v, float e)
{
	Check_Object(&v);
	return Small_Enough(static_cast<float>(v.x), e) && Small_Enough(static_cast<float>(v.y), e);
}

template <class T>
bool Close_Enough(const Vector2DOf<T>& v1, const Vector2DOf<T>& v2, float e)
{
	Check_Object(&v1);
	Check_Object(&v2);
	return Close_Enough(static_cast<float>(v1.x), static_cast<float>(v2.x), e) && Close_Enough(static_cast<float>(v1.y), static_cast<float>(v2.y), e);
}

typedef Vector2DOf<float> Vector2DScalar;
} // namespace Stuff

#if !defined(Spew)
template <class T>
void Spew(std::wstring_view group, const Stuff::Vector2DOf<T>& vector)
{
	// Check_Object(this);
	SPEW((group, "<+"));
	Spew(group, vector.x);
	SPEW((group, ",+"));
	Spew(group, vector.y);
	SPEW((group, ">+"));
}
#endif

#endif
