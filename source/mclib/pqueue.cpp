//***************************************************************************
//
//	PQueue.cpp -- Prototype for Priority Queue class
//
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

//----------------------------------------------------------------
// This implementation of a priority queue sorts with the SMALLEST
// item at the front of the queue (used for pathfinding purposes).

//***************************************************************************

#include "stdinc.h"

//--------------
// Include Files

#ifndef HEAP_H
#include "heap.h"
#endif

#ifndef PQUEUE_H
#include "pqueue.h"
#endif

//#include "gameos.hpp"
//***************************************************************************
// Class PriorityQueue
//***************************************************************************

int32_t
PriorityQueue::init(int32_t max, int32_t keyMinValue)
{
	//-------------------------
	// Create the queue list...
	pqList = (PQNode*)systemHeap->Malloc(sizeof(PQNode) * (max + 2));
	gosASSERT(pqList != nullptr);
	//----------------------------------------------------------------------
	// Note that two additional nodes are added, as the first and last nodes
	// of the queue list are used as sentinels (to assist implementation
	// execution speed)...
	maxItems = max + 2;
	keyMin = keyMinValue;
	return (0);
}

//---------------------------------------------------------------------------

void PriorityQueue::upHeap(int32_t curIndex)
{
	PQNode startNode = pqList[curIndex];
	int32_t stopKey = startNode.key;
	pqList[0].key = keyMin;
	pqList[0].id = 0xFFFFFFFF;
	//--------------------
	// sort up the heap...
	while (pqList[curIndex / 2].key >= stopKey)
	{
		pqList[curIndex] = pqList[curIndex / 2];
		curIndex /= 2;
	}
	pqList[curIndex] = startNode;
}

//---------------------------------------------------------------------------

int32_t
PriorityQueue::insert(PQNode& item)
{
	if (numItems == maxItems)
		return (1);
	pqList[++numItems] = item;
	upHeap(numItems);
	return (0);
}

//---------------------------------------------------------------------------

void PriorityQueue::downHeap(int32_t curIndex)
{
	//----------------------------------
	// Start at the top from curIndex...
	PQNode startNode = pqList[curIndex];
	int32_t stopKey = startNode.key;
	//----------------------
	// Sort down the heap...
	while (curIndex <= numItems / 2)
	{
		int32_t nextIndex = curIndex << 1;
		if ((nextIndex < numItems) && (pqList[nextIndex].key > pqList[nextIndex + 1].key))
			nextIndex++;
		if (stopKey <= pqList[nextIndex].key)
			break;
		pqList[curIndex] = pqList[nextIndex];
		curIndex = nextIndex;
	}
	pqList[curIndex] = startNode;
}

//---------------------------------------------------------------------------

void PriorityQueue::remove(PQNode& item)
{
	item = pqList[1];
	pqList[1] = pqList[numItems--];
	downHeap(1);
}

//---------------------------------------------------------------------------

void PriorityQueue::change(int32_t itemIndex, int32_t newValue)
{
	if (newValue > pqList[itemIndex].key)
	{
		pqList[itemIndex].key = newValue;
		downHeap(itemIndex);
	}
	else if (newValue < pqList[itemIndex].key)
	{
		pqList[itemIndex].key = newValue;
		upHeap(itemIndex);
	}
}

//---------------------------------------------------------------------------

int32_t
PriorityQueue::find(int32_t id)
{
	for (size_t index = 0; index <= numItems; index++)
		if (pqList[index].id == (uint32_t)id)
			return (index);
	return (0);
}

//---------------------------------------------------------------------------

void PriorityQueue::destroy(void)
{
	systemHeap->Free(pqList);
	pqList = nullptr;
	maxItems = 0;
	numItems = 0;
}

//***************************************************************************
