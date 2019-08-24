//===========================================================================//
// File:	scalar.cpp                                                       //
// Contents: Base information used by all MUNGA source files                 //
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#include "stdinc.h"
//#include "stuffheaders.hpp"

//#include "gameos.hpp"
#include "stuff/scalar.h"

using namespace Stuff;

int32_t
Stuff::Round(float value)
{
	int32_t whole_part = static_cast<int32_t>(floor(value));
	float fractional_part = value - whole_part;
	if (fractional_part >= 0.5)
	{
		return whole_part + 1;
	}
	else
	{
		return whole_part;
	}
}

void
Stuff::Find_Roots(float a, // a*x*x + b*x + c = 0
	float b, float c, float* center, float* range)
{
	//
	//---------------------------------
	// See if the quadratic is solvable
	//---------------------------------
	//
	*range = b * b - 4.0f * a * c;
	if (*range < 0.0f || Small_Enough(a))
	{
		*range = -1.0f;
	}
	else
	{
		//
		//---------------------------
		// Solve the single root case
		//---------------------------
		//
		a *= 2.0f;
		*center = -b / a;
		if (*range < SMALL)
		{
			*range = 0.0f;
		}
		//
		//--------------------------
		// Find the two-root extents
		//--------------------------
		//
		else
		{
			*range = Sqrt(*range);
			*range /= a;
		}
	}
}

uint32_t
Stuff::Scaled_Float_To_Bits(float in, float min, float max, uint32_t bits)
{
	_ASSERT(bits < 32);
	_ASSERT(bits > 0);
	_ASSERT(min < max);
	_ASSERT(in <= max);
	_ASSERT(in >= min);
	uint32_t biggest_number = (0xffffffff >> (32 - bits));
	float local_in = in - min;
	float range = (max - min);
	uint32_t return_value = (uint32_t)((local_in / range) * (float)biggest_number);
	// _ASSERT((uint32_t)return_value >= 0x00000000);
	_ASSERT((uint32_t)return_value <= (uint32_t)biggest_number);
	return return_value;
}

float
Stuff::Scaled_Float_From_Bits(uint32_t in, float min, float max, uint32_t bits)
{
	_ASSERT(bits < 32);
	_ASSERT(bits > 0);
	_ASSERT(min < max);
	in &= (0xffffffff >> (32 - bits));
	uint32_t biggest_number = (0xffffffff >> (32 - bits));
	float ratio = in / (float)biggest_number;
	float range = (max - min);
	float return_value = (ratio * range) + min;
	return return_value;
}

uint32_t
Stuff::Scaled_Int_To_Bits(int32_t in, int32_t min, int32_t max, uint32_t bits)
{
	_ASSERT(bits < 32);
	_ASSERT(bits > 0);
	_ASSERT(min < max);
	_ASSERT(in <= max);
	_ASSERT(in >= min);
	uint32_t biggest_number = (0xffffffff >> (32 - bits));
	int32_t local_in = in - min;
	int32_t range = (max - min);
	uint32_t return_value = (uint32_t)(((float)local_in / (float)range) * (float)biggest_number);
	// _ASSERT((uint32_t)return_value >= 0x00000000);
	_ASSERT((uint32_t)return_value < (uint32_t)biggest_number);
	return return_value;
}

int32_t
Stuff::Scaled_Int_From_Bits(uint32_t in, int32_t min, int32_t max, uint32_t bits)
{
	_ASSERT(bits < 32);
	_ASSERT(bits > 0);
	_ASSERT(min < max);
	uint32_t biggest_number = (0xffffffff >> (32 - bits));
	float ratio = (float)in / (float)biggest_number;
	int32_t range = (max - min);
	int32_t return_value = ((int32_t)(ratio * (float)range)) + min;
	return return_value;
}
