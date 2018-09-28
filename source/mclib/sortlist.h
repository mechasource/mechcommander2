//***************************************************************************
//
//	Sortlist.h -- Sort List defines
//
//	MechCommander II
//
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#pragma once

#ifndef SORTLIST_H
#define SORTLIST_H

//***************************************************************************

// #include "dstd.h"

//--------------------------------
// Structure and Class Definitions

typedef struct _SortListNode
{
	float value; // sort value
	uint32_t id; // item
} SortListNode;

class SortList
{

protected:
	SortListNode* list;
	uint32_t numItems;

public:
	void init(void)
	{
		list	 = nullptr;
		numItems = 0;
	}

	SortList(void) { init(void); }

	int32_t init(uint32_t numItems);

	void setId(uint32_t index, uint32_t id)
	{
		if (/*(index >= 0) &&*/ (index < numItems))
			list[index].id = id;
	}

	void setValue(uint32_t index, float value)
	{
		if (/*(index >= 0) &&*/ (index < numItems))
			list[index].value = value;
	}

	uint32_t getId(uint32_t index) { return (list[index].id); }

	float getValue(uint32_t index) { return (list[index].value); }

	void clear(bool setToMin = true);

	uint32_t getNumItems(void) { return (numItems); }

	void sort(bool descendingOrder = true);

	void destroy(void);

	~SortList(void) { destroy(void); }
};

typedef SortList* SortListPtr;

//***************************************************************************

#endif
