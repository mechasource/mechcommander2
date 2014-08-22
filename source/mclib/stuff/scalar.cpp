//===========================================================================//
// File:	scalar.cpp                                                       //
// Contents: Base information used by all MUNGA source files                 //
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#include "stdafx.h"
//#include "stuffheaders.hpp"

//#include <gameos.hpp>
#include <stuff/scalar.hpp>

using namespace Stuff;


int
	Stuff::Round(Scalar value)
{
	int whole_part = static_cast<int>(floor(value));
	Scalar fractional_part = value - whole_part;
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
	Stuff::Find_Roots(
		Scalar a,		// a*x*x + b*x + c = 0
		Scalar b,
		Scalar c,
		Scalar *center,
		Scalar *range
	)
{

	//
	//---------------------------------
	// See if the quadratic is solvable
	//---------------------------------
	//
	*range = b*b - 4.0f*a*c;
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


ULONG
	Stuff::Scaled_Float_To_Bits(float in, float min, float max, ULONG bits)
{
	Verify(bits < 32);
	Verify(bits > 0);

	Verify(min < max);
	Verify(in <= max);
	Verify(in >= min);
	

	uint32_t biggest_number = (0xffffffff>>(32-bits));
	float local_in =  in - min;
	float range = (max-min);

	
	ULONG return_value = (ULONG)((local_in/range) * (float)biggest_number);
	
	//Verify((ULONG)return_value >= 0x00000000);
	Verify((ULONG)return_value <= (ULONG)biggest_number);

	return return_value;

}

float
	Stuff::Scaled_Float_From_Bits(ULONG in, float min, float max, ULONG bits)
{
	Verify(bits < 32);
	Verify(bits > 0);

	Verify(min < max);


	in &= (0xffffffff>>(32-bits));

	uint32_t biggest_number = (0xffffffff>>(32-bits));

	float ratio = in/(float)biggest_number;
	float range = (max-min);
	float return_value = (ratio * range)+min;


	return return_value;
}

ULONG
	Stuff::Scaled_Int_To_Bits(int in, int min, int max, ULONG bits)
{

	Verify(bits < 32);
	Verify(bits > 0);

	Verify(min < max);
	Verify(in <= max);
	Verify(in >= min);

	uint32_t biggest_number = (0xffffffff>>(32-bits));
	int local_in =  in - min;
	int range = (max-min);

	
	ULONG return_value = (ULONG)(((float)local_in/(float)range) * (float)biggest_number);
	
	//Verify((ULONG)return_value >= 0x00000000);
	Verify((ULONG)return_value < (ULONG)biggest_number);

	return return_value;

}

int
	Stuff::Scaled_Int_From_Bits(ULONG in, int min, int max, ULONG bits)
{
	Verify(bits < 32);
	Verify(bits > 0);

	Verify(min < max);


	uint32_t biggest_number = (0xffffffff>>(32-bits));

	float ratio = (float)in/(float)biggest_number;
	int range = (max-min);
	int return_value = ((int)(ratio * (float)range))+min;

	return return_value;
}


