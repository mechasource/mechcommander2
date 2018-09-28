//***************************************************************************
//
//	SortList.cpp -- Sort List class
//
//	MechCommander II
//
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//
#include "stdinc.h"
//#include <stdlib.h>

//--------------
// Include Files

#ifndef SORTLIST_H
#include "sortlist.h"
#endif

#ifndef ERR_H
#include "err.h"
#endif

#ifndef HEAP_H
#include "heap.h"
#endif

#include <gameos.hpp>
//***************************************************************************
// Class SortList
//***************************************************************************

int32_t SortList::init(int32_t _numItems)
{
	//-------------------------
	// Create the sort list...
	numItems = _numItems;
	list	 = (SortListNode*)systemHeap->Malloc(sizeof(SortListNode) * numItems);
	if (!list)
		Fatal(0, " Unable to init sortList ");
	return (list == nullptr);
}

//---------------------------------------------------------------------------

void SortList::clear(bool setToMin)
{
	int32_t i;
	for (i = 0; i < numItems; i++)
		list[i].id = i;
	if (setToMin)
		for (i = 0; i < numItems; i++)
			list[i].value = float(-3.4E38);
	else
		for (i = 0; i < numItems; i++)
			list[i].value = float(3.4E38);
}

//---------------------------------------------------------------------------

int32_t cdecl descendingCompare(PCVOID arg1, PCVOID arg2)
{
	float value1 = ((SortListNode*)arg1)->value;
	float value2 = ((SortListNode*)arg2)->value;
	if (value1 > value2)
		return (-1);
	else if (value1 < value2)
		return (1);
	else
		return (0);
}

//---------------------------------------------------------------------------

int32_t cdecl ascendingCompare(PCVOID arg1, PCVOID arg2)
{
	float value1 = ((SortListNode*)arg1)->value;
	float value2 = ((SortListNode*)arg2)->value;
	if (value1 > value2)
		return (1);
	else if (value1 < value2)
		return (-1);
	else
		return (0);
}

//---------------------------------------------------------------------------

void SortList::sort(bool descendingOrder)
{
	//------------------------------------------------------------------
	// For now, just use ANSI C's built-in qsort (ugly, but functional).
	if (descendingOrder)
		qsort((PVOID)list, (size_t)numItems, sizeof(SortListNode), descendingCompare);
	else
		qsort((PVOID)list, (size_t)numItems, sizeof(SortListNode), ascendingCompare);
}

//---------------------------------------------------------------------------

void SortList::destroy(void)
{
	systemHeap->Free(list);
	list = nullptr;
}

//***************************************************************************
