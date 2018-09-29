//***************************************************************************
//
//	movemgr.h -- Class definitions for the move path manager.
//
//	MechCommander 2
//
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#pragma once

#ifndef MOVEMGR_H
#define MOVEMGR_H

//---------------------------------------------------------------------------

//#include "dmovemgr.h"
//#include "dwarrior.h"

//***************************************************************************

typedef struct _PathQueueRec* PathQueueRecPtr;

typedef struct _PathQueueRec
{
	int32_t num;
	MechWarriorPtr pilot;
	int32_t selectionIndex;
	uint32_t moveParams;
	bool initPath;
	bool faceObject;
	PathQueueRecPtr prev;
	PathQueueRecPtr next;
} PathQueueRec;

//---------------------------------------------------------------------------

class MovePathManager
{

public:
	PathQueueRec pool[MAX_MOVERS];
	PathQueueRecPtr queueFront;
	PathQueueRecPtr queueEnd;
	PathQueueRecPtr freeList;
	static int32_t numPaths;
	static int32_t peakPaths;
	static int32_t sourceTally[50];

public:
	PVOID operator new(size_t ourSize);

	void operator delete(PVOID us);

	MovePathManager(void) { init(void); }

	~MovePathManager(void) { destroy(void); }

	void destroy(void);

	int32_t init(void);

	void remove(PathQueueRecPtr rec);

	PathQueueRecPtr remove(MechWarriorPtr pilot);

	void request(MechWarriorPtr pilot, int32_t selectionIndex, uint32_t moveParams, int32_t source);

	void calcPath(void);

	void update(void);
};

typedef MovePathManager* MovePathManagerPtr;

#endif

//***************************************************************************
