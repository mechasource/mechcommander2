//===========================================================================//
// File:	iterator.hh                                                      //
// Contents: Interface specifications for the abstract iterator              //
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#pragma once

#ifndef _ITERATOR_HPP_
#define _ITERATOR_HPP_

#include <stuff/style.hpp>

namespace Stuff
{

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Iterator ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	typedef size_t CollectionSize;
	typedef size_t IteratorPosition;

	class _declspec(novtable) Iterator
#if defined(_ARMOR)
		: public Stuff::Signature
#endif
	{
	public:
		//
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		// Public interface to abstract class
		//
		// NOTE: All unsafe, untyped public methods are named XXXItem.  The
		// safe, typed public methods are named XXX and are declared in the
		// template sub-classes.
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		//

		//
		//--------------------------------------------------------------------
		// Destructor and testing
		//--------------------------------------------------------------------
		//
		virtual ~Iterator(void) {}

		virtual Iterator* MakeClone(void) = 0;

		void TestInstance(void) {}

		//
		//--------------------------------------------------------------------
		// First						- Moves to next item
		// Last						- Moves to last item
		//--------------------------------------------------------------------
		//
		virtual void First(void);
		virtual void Last(void);

		//
		//--------------------------------------------------------------------
		// Next			 			- Moves to next item
		// Previous			 		- Moves to previous item
		//--------------------------------------------------------------------
		//
		virtual void Next(void);
		virtual void Previous(void);

		//
		//--------------------------------------------------------------------
		// ReadAndNextItem		- Returns current item and moves to next item
		// ReadAndPreviousItem	- Returns current item and moves to prev item
		// GetCurrentItem			- Returns current item
		//--------------------------------------------------------------------
		//
		PVOID ReadAndNextItem(void)
		{
			return ReadAndNextImplementation();
		}
		PVOID ReadAndPreviousItem(void)
		{
			return ReadAndPreviousImplementation();
		}
		PVOID GetCurrentItem(void)
		{
			return GetCurrentImplementation();
		}

		//
		//--------------------------------------------------------------------
		// GetSize					- Returns number of items
		// GetNthItem				- Returns nth item
		//--------------------------------------------------------------------
		//
		virtual CollectionSize GetSize(void);
		PVOID GetNthItem(CollectionSize index)
		{
			return GetNthImplementation(index);
		}

		//
		//--------------------------------------------------------------------
		// BeginIterator			- Moves to first item and returns iterator
		// EndIterator				- Moves to last item and returns iterator
		//--------------------------------------------------------------------
		//
		virtual Iterator& BeginIterator(void)
		{
			return BeginImplementation();
		}
		virtual Iterator& EndIterator(void)
		{
			return EndImplementation();
		}

		//
		//--------------------------------------------------------------------
		// ForwardIterator	  	- Moves to next item and returns iterator
		// BackwardIterator	  	- Moves to previous item and returns iterator
		//--------------------------------------------------------------------
		//
		virtual Iterator& ForwardIterator(void)
		{
			return ForwardImplementation();
		}
		virtual Iterator& BackwardIterator(void)
		{
			return BackwardImplementation();
		}

	protected:
		//
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		// Protected interface to abstract class
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		//
		Iterator(void) {}

		//
		//--------------------------------------------------------------------
		// Untyped implementations
		//--------------------------------------------------------------------
		//
		virtual PVOID ReadAndNextImplementation(void);
		virtual PVOID ReadAndPreviousImplementation(void);
		virtual PVOID GetCurrentImplementation(void);
		virtual PVOID GetNthImplementation(CollectionSize index);

		virtual Iterator& BeginImplementation(void)
		{
			First();
			return *this;
		}
		virtual Iterator& EndImplementation(void)
		{
			Last();
			return *this;
		}
		virtual Iterator& ForwardImplementation(void)
		{
			Next();
			return *this;
		}
		virtual Iterator& BackwardImplementation(void)
		{
			Previous();
			return *this;
		}
	};

}
#endif
