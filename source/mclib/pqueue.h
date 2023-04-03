//***************************************************************************
//
//	PQueue.h -- Prototype for priority queues
//
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#ifndef PQUEUE_H
#define PQUEUE_H

//***************************************************************************

//--------------
// Include Files

//--------------------------------
// Structure and Class Definitions

typedef struct _PQNode
{
	int32_t key; // sort value
	uint32_t id; // hash value for this map position
	int32_t row; // HB-specific
	int32_t col; // HB-specific
} PQNode;

class PriorityQueue
{

protected:
	PQNode* pqList;
	int32_t maxItems;
	int32_t numItems;
	int32_t keyMin;

	void downHeap(int32_t curIndex);

	void upHeap(int32_t curIndex);

public:
	void init(void)
	{
		pqList = nullptr;
		numItems = 0;
	}

	PriorityQueue(void)
	{
		init(void);
	}

	int32_t init(int32_t maxItems, int32_t keyMinValue = -2000000);

	int32_t insert(PQNode& item);

	void remove(PQNode& item);

	void change(int32_t itemIndex, int32_t newValue);

	int32_t find(int32_t id);

	void clear(void)
	{
		numItems = 0;
	}

	int32_t getNumItems(void)
	{
		return (numItems);
	}

	bool isEmpty(void)
	{
		return (numItems == 0);
	}

	PQNode* getItem(int32_t itemIndex)
	{
		return (&pqList[itemIndex]);
	}

	void destroy(void);

	~PriorityQueue(void)
	{
		destroy(void);
	}
};

typedef PriorityQueue* PriorityQueuePtr;

//***************************************************************************

#endif
