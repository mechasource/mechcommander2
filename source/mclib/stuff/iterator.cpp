//===========================================================================//
// File:	iterator.cc                                                      //
// Contents: Implementation details of base iterator                         //
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#include "stdinc.h"
//#include "stuffheaders.hpp"

#include <gameos.hpp>
#include <stuff/iterator.hpp>

using namespace Stuff;

//
//###########################################################################
// First
//###########################################################################
//
void Iterator::First() { STOP(("Iterator::First - Should never reach here")); }

//
//###########################################################################
// Last
//###########################################################################
//
void Iterator::Last() { STOP(("Iterator::Last - Should never reach here")); }

//
//###########################################################################
// Next
//###########################################################################
//
void Iterator::Next() { STOP(("Iterator::Next - Should never reach here")); }

//
//###########################################################################
// Previous
//###########################################################################
//
void Iterator::Previous()
{
	STOP(("Iterator::Previous - Should never reach here"));
}

//
//###########################################################################
// GetSize
//###########################################################################
//
CollectionSize Iterator::GetSize()
{
	CollectionSize i = 0;
	PVOID item;
	First();
	while ((item = GetCurrentImplementation()) != nullptr)
	{
		i++;
		Next();
	}
	return i;
}

//
//###########################################################################
// ReadAndNextImplementation
//###########################################################################
//
void* Iterator::ReadAndNextImplementation()
{
	PVOID item;
	if ((item = GetCurrentImplementation()) != nullptr)
	{
		Next();
	}
	return item;
}

//
//###########################################################################
// ReadAndPreviousImplementation
//###########################################################################
//
void* Iterator::ReadAndPreviousImplementation()
{
	PVOID item;
	if ((item = GetCurrentImplementation()) != nullptr)
	{
		Previous();
	}
	return item;
}

//
//###########################################################################
// GetCurrentImplementation
//###########################################################################
//
PVOID
Iterator::GetCurrentImplementation()
{
	STOP(("Iterator::GetCurrentImplementation - Should never reach here"));
	return nullptr;
}

//
//###########################################################################
// GetNthImplementation
//###########################################################################
//
void* Iterator::GetNthImplementation(CollectionSize index)
{
	CollectionSize i = 0;
	PVOID item;
	First();
	while ((item = GetCurrentImplementation()) != nullptr)
	{
		if (i == index)
			return item;
		Next();
		i++;
	}
	return nullptr;
}
