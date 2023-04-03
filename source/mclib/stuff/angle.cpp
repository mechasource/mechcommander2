//===========================================================================//
// File:	angle.cc                                                         //
// Contents: Implementation details for angle classes                        //
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#include "stdinc.h"
//#include "stuff/stuffheaders.h"

////#include "gameos.hpp"
#include "stuff/angle.h"

// using namespace Stuff;

//
//#############################################################################
//#############################################################################
//
float Radian::Normalize(float value)
{
	float temp = static_cast<float>(fmod(value, Two_Pi));
	if (temp > Pi)
	{
		temp -= Two_Pi;
	}
	else if (temp <= -Pi)
	{
		temp += Two_Pi;
	}
	return temp;
}

//
//#############################################################################
//#############################################################################
//
Radian&
Radian::Normalize()
{
	// Check_Object(this);
	m_angle = static_cast<float>(fmod(m_angle, Two_Pi));
	if (m_angle > Pi)
	{
		m_angle -= Two_Pi;
	}
	else if (m_angle < -Pi)
	{
		m_angle += Two_Pi;
	}
	return *this;
}

//
//#############################################################################
//#############################################################################
//
Radian&
Radian::Lerp(const Radian& a, const Radian& b, float t)
{
	float a1, a2;
	// Check_Pointer(this);
	Check_Object(&a);
	Check_Object(&b);
	a1 = Radian::Normalize(a.m_angle);
	a2 = Radian::Normalize(b.m_angle);
	if (a2 - a1 > Pi)
	{
		a2 -= Two_Pi;
	}
	else if (a2 - a1 < -Pi)
	{
		a2 += Two_Pi;
	}
	m_angle = Stuff::Lerp(a1, a2, t);
	return *this;
}

#if 0
SinCosPair&
SinCosPair::operator=(const Radian& radian)
{
	//Check_Pointer(this);
	Check_Object(&radian);
#if USE_INLINE_ASSEMBLER_CODE
	float* f = &sine;
	_asm
	{
		push	ebx
		push	edx

		mov		ebx, f
		mov		edx, radian.angle

		fld		dword ptr [edx]
		fsincos
		fstp	dword ptr [ebx + 4]
		fstp	dword ptr [ebx]

		pop		edx
		pop		ebx
	}
#else
	cosine = cos(radian);
	sine = sin(radian);
#endif
	// Check_Object(this);
	return *this;
}
#endif

//
//#############################################################################
//#############################################################################
//
#if !defined(Spew)
void Spew(std::wstring_view group, const Radian& angle)
{
	Check_Object(&angle);
	SPEW((group, "%f rad+", angle.m_angle));
}

//
//#############################################################################
//#############################################################################
//
void Spew(std::wstring_view group, const Degree& angle)
{
	Check_Object(&angle);
	SPEW((group, "%f deg+", angle.m_angle));
}

//
//#############################################################################
//#############################################################################
//
void Spew(std::wstring_view group, const SinCosPair& angle)
{
	Check_Object(&angle);
	SPEW((group, "{%f,%f}+", angle.cosine, angle.sine));
}
#endif
