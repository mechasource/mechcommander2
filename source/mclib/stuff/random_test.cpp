//===========================================================================//
// File:	random.tst                                                       //
// Contents: test function for the random number generator                   //
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#include "stdinc.h"
#include "stuff/stuffheaders.h"

bool Random::TestClass()
{
	SPEW((GROUP_STUFF_TEST, "Starting Random::Instance test..."));
#define RANDOM_TEST_COUNT 10000
	int32_t i;
	for (i = 0; i < RANDOM_TEST_COUNT; ++i)
	{
		float r = Random::Instance->GetFraction();
		Test_Assumption(r >= 0.0f && r < 1.0f);
	}
	int32_t array[10];
	for (i = 0; i < ELEMENTS(array); ++i)
	{
		array[i] = 0;
	}
	for (i = 0; i < RANDOM_TEST_COUNT; ++i)
	{
		int32_t r = Random::Instance->GetLessThan(ELEMENTS(array));
		Test_Assumption(r >= 0 && r < ELEMENTS(array));
		++array[r];
	}
	return true;
}
