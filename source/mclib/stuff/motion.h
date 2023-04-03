//===========================================================================//
// File:	motion.hh                                                        //
// Contents: Implementation details for the position class                   //
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#pragma once

#ifndef _MOTION_HPP_
#define _MOTION_HPP_

#include "stuff/vector3d.h"

namespace Stuff
{
class Motion3D;
}

#if !defined(Spew)
void Spew(std::wstring_view group, const Stuff::Motion3D& motion);
#endif

namespace Stuff
{

//~~~~~~~~~~~~~~~~~~~~~~~~~~~ Motion3D ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

class Motion3D
{
public:
	Vector3D linearMotion;
	Vector3D angularMotion;

	static const Motion3D Identity;

	//
	// Constructors
	//
	Motion3D() { }
	Motion3D(const Motion3D& motion);
	Motion3D(const Vector3D& t, const Vector3D& q)
	{
		Check_Object(&t);
		Check_Object(&q);
		linearMotion = t;
		angularMotion = q;
	}

	//
	// Assignment operators
	//
	Motion3D& operator=(const Motion3D& p);

	friend bool Close_Enough(const Motion3D& a1, const Motion3D& a2, float e = SMALL);
	bool operator==(const Motion3D& a) const
	{
		return Close_Enough(*this, a, SMALL);
	}
	bool operator!=(const Motion3D& a) const
	{
		return !Close_Enough(*this, a, SMALL);
	}

	//
	// Origin3D motion
	//
	Motion3D& AddScaled(const Motion3D& source, const Motion3D& delta, float t);

	//
	// Support functions
	//
#if !defined(Spew)
	friend void ::Spew(std::wstring_view group, const Motion3D& motion);
#endif
	void TestInstance(void) const;
	static bool TestClass(void);
};
} // namespace Stuff

namespace MemoryStreamIO
{
#if CONSIDERED_DISABLED
inline std::istream&
Read(std::istream& stream, Stuff::Motion3D* output)
{
	return stream.read(output, sizeof(*output));
}
inline std::ostream&
Write(std::ostream& stream, const Stuff::Motion3D* input)
{
	return stream.write(input, sizeof(*input));
}
#endif
} // namespace MemoryStreamIO
#endif
