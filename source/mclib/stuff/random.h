//===========================================================================//
// File:	random.cc                                                        //
// Contents: Interface specification for the random number generator         //
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#pragma once

#ifndef _RANDOM_HPP_
#define _RANDOM_HPP_

#include "stuff/style.h"

namespace Stuff
{

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Random ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

class Random
#if defined(_ARMOR)
	: public Stuff::Signature
#endif
{
public:
	static void __stdcall InitializeClass(void);
	static void TerminateClass(void);

private:
	static int32_t Numbers[250]; // the random number table
	static int32_t Index; // the current entry within the table
	static void Init(void);
	static int32_t GetRandomInt(void);
	Random(void)
	{
		_ASSERT(Index == -1);
		Init();
	}
	static Random* Instance;

	void TestInstance(void) const {}

public:
	//
	//------------------------
	// Random number functions
	//------------------------
	//
	static int32_t GetInt(void) // returns 0 .. RAND_MAX
	{
		return GetRandomInt();
	}
	static float GetFraction(void); // returns 0.0f <= x < 1.0f
	static int32_t GetLessThan(int32_t Range); // returns 0 .. Range-1

	static bool TestClass(void);
};

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Die ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

class Die
#if defined(_ARMOR)
	: public Stuff::Signature
#endif
{
private:
	int32_t highestRandom; // the highest random number giving a uniform dist.
	int32_t dieSides; // the number of sides on the die (starting from 1)

public:
	Die(int32_t sides);
	operator int32_t(void); // returns 1 .. dieSides
};
} // namespace Stuff
#endif
