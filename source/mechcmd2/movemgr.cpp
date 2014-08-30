//***************************************************************************
//
//	movemgr.cpp -- File contains the MovePathManager class code
//
//	MechCommander 2
//
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//
#include "stdafx.h"

#ifndef MCLIB_H
#include <mclib.h>
#endif

#ifndef MOVEMGR_H
#include "movemgr.h"
#endif

#ifndef WARRIOR_H
#include "warrior.h"
#endif

//#include <gameos.hpp>


int32_t MovePathManager::numPaths = 0;
int32_t MovePathManager::peakPaths = 0;
int32_t MovePathManager::sourceTally[50];
MovePathManagerPtr PathManager = nullptr;

//***************************************************************************
// PATH MANAGER class
//***************************************************************************

PVOID MovePathManager::operator new (size_t mySize) {

	PVOID result = systemHeap->Malloc(mySize);
	return(result);
}

//---------------------------------------------------------------------------

void MovePathManager::operator delete (PVOID us) {

	systemHeap->Free(us);
}

//---------------------------------------------------------------------------

int32_t MovePathManager::init (void)
{
	int32_t i;
	for (i = 0; i < MAX_MOVERS; i++) {
		pool[i].pilot = nullptr;
		pool[i].selectionIndex = 0;
		pool[i].moveParams = 0;
		if (i > 0)
			pool[i].prev = &pool[i - 1];
		else
			pool[i].prev = nullptr;
		if (i < (MAX_MOVERS - 1))
			pool[i].next = &pool[i + 1];
		else
			pool[i].next = nullptr;
	}

	//------------------------------
	// All start on the free list...
	queueFront = nullptr;
	queueEnd = nullptr;
	freeList = &pool[0];

	numPaths = 0;
	peakPaths = 0;
	for (i =0; i < 50; i++)
		sourceTally[i] = 0;

	return(NO_ERROR);
}

//---------------------------------------------------------------------------

void MovePathManager::destroy (void) {

}

//---------------------------------------------------------------------------

void MovePathManager::remove (PathQueueRecPtr rec) {

	//------------------------------------
	// Remove it from the pending queue...
	if (rec->prev)
		rec->prev->next = rec->next;
	else
		queueFront = rec->next;
	if (rec->next)
		rec->next->prev = rec->prev;
	else
		queueEnd = rec->prev;

	//------------------------------------
	// Return the QRec to the free list...
	rec->prev = nullptr;
	rec->next = freeList;
	freeList = rec;

	sourceTally[rec->num]--;
	numPaths--;
}

//---------------------------------------------------------------------------

PathQueueRecPtr MovePathManager::remove (MechWarriorPtr pilot) {

	PathQueueRecPtr rec = pilot->getMovePathRequest();
	if (rec) {
		remove(rec);
		pilot->setMovePathRequest(nullptr);
		return(rec);
	}
	return(nullptr);
}

//---------------------------------------------------------------------------

#define	DEBUG_MOVEPATH_QUEUE	0

void MovePathManager::request (MechWarriorPtr pilot, int32_t selectionIndex, uint32_t moveParams, int32_t source) {

	//-----------------------------------------------------
	// If the pilot is already awaiting a calc, purge it...
	remove(pilot);

	if (!freeList)
		Fatal(0, " Too many pilots calcing paths ");

	//---------------------------------------------
	// Grab the first free move path rec in line...
	PathQueueRecPtr pathQRec = freeList;

	//-----------------------------------------
	// Cut the new record from the free list...
	freeList = freeList->next;
	if (freeList)
		freeList->prev = nullptr;

	//---------------------------------------------------
	// New record has no next. Already has no previous...
	pathQRec->num = source;
	pathQRec->pilot = pilot;
	pathQRec->selectionIndex = selectionIndex;
	pathQRec->moveParams = moveParams;

	if (queueEnd) {
		queueEnd->next = pathQRec;
		pathQRec->prev = queueEnd;
		pathQRec->next = nullptr;
		queueEnd = pathQRec;
		}
	else {
		pathQRec->prev = nullptr;
		pathQRec->next = nullptr;
		queueFront = queueEnd = pathQRec;
	}
	pilot->setMovePathRequest(pathQRec);
	
	numPaths++;
	sourceTally[source]++;

	if (numPaths > peakPaths)
		peakPaths = numPaths;
}

//---------------------------------------------------------------------------

void MovePathManager::calcPath (void) {

	if (queueFront) {
		//------------------------------
		// Grab the next in the queue...
		PathQueueRecPtr curQRec = queueFront;
		remove(queueFront);

		//--------------------------------------------------
		// If the mover is no longer around, don't bother...
		MechWarriorPtr pilot = curQRec->pilot;
		pilot->setMovePathRequest(nullptr);
		MoverPtr mover = pilot->getVehicle();
		if (!mover)
			return;

		/*int32_t err = */pilot->calcMovePath(curQRec->selectionIndex, curQRec->moveParams);
	}
}

//----------------------------------------------------------------------------------
void DEBUGWINS_print (PSTR s, int32_t window);
#ifdef LAB_ONLY
extern int64_t MCTimePath1Update;
extern int64_t MCTimePath2Update;
extern int64_t MCTimePath3Update;
extern int64_t MCTimePath4Update;
extern int64_t MCTimePath5Update;
extern int64_t MCTimeCalcGoal1Update;
extern int64_t MCTimeCalcGoal2Update;
extern int64_t MCTimeCalcGoal3Update;
extern int64_t MCTimeCalcGoal4Update;
extern int64_t MCTimeCalcGoal5Update;
extern int64_t MCTimeCalcGoal6Update;
#endif
void MovePathManager::update (void) {

	#ifdef LAB_ONLY
	MCTimePath1Update = 0;
	MCTimePath2Update = 0;
	MCTimePath3Update = 0;
	MCTimePath4Update = 0;
	MCTimePath5Update = 0;

	MCTimeCalcGoal1Update = 0;
	MCTimeCalcGoal2Update = 0;
	MCTimeCalcGoal3Update = 0;
	MCTimeCalcGoal4Update = 0;
	MCTimeCalcGoal5Update = 0;
	MCTimeCalcGoal6Update = 0;
#endif

	#ifdef MC_PROFILE
	QueryPerformanceCounter(startCk);
	#endif

	int32_t numPathsToProcess = 6;
	//if (numPaths > 15)
	//	numPathsToProcess = 10;
	for (int32_t i = 0; i < numPathsToProcess; i++) {
		if (!queueFront)
			break;
		calcPath();
	}

//	char s[50];
//	sprintf(s, "num paths = %d", numPaths);
//	DEBUGWINS_print(s, 0);

	#ifdef MC_PROFILE
	QueryPerformanceCounter(endCk);
	srMvPathUpd += (endCk.LowPart - startCk.LowPart);
	#endif
}

//***************************************************************************
