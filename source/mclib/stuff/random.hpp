//===========================================================================//
// File:	random.cc                                                        //
// Contents: Interface specification for the random number generator         //
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#pragma once

#ifndef _RANDOM_HPP_
#define _RANDOM_HPP_

#include <stuff/style.hpp>

namespace Stuff {

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Random ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	class Random
#if defined(_ARMOR)
		: public Stuff::Signature
#endif
	{
	public:
		static void InitializeClass();
		static void TerminateClass();

	private:
		static int Numbers[250]; // the random number table
		static int Index; // the current entry within the table
		static void Init(void);
		static int GetRandomInt(void);
		Random(void) 
		{
			Verify(Index == -1); Init();
		}
		static Random*	Instance;

		void TestInstance(void) const {}

	public:
		//
		//------------------------
		// Random number functions
		//------------------------
		//
		static int GetInt(void)				// returns 0 .. RAND_MAX
		{
			return GetRandomInt();
		}
		static float GetFraction(void);		// returns 0.0f <= x < 1.0f
		static int GetLessThan(int Range);	// returns 0 .. Range-1

		static bool TestClass(void);
	};

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Die ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	class Die
#if defined(_ARMOR)
		: public Stuff::Signature
#endif
	{
	private:
		int highestRandom;	// the highest random number giving a uniform dist.
		int dieSides;		// the number of sides on the die (starting from 1)

	public:
		Die(int sides);
		operator int();		// returns 1 .. dieSides
	};

}
#endif
