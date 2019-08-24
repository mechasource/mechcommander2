//***************************************************************************
//
//	movemgr.cpp -- File contains the MovePathManager class code
//
//	MechCommander 2
//
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//
#include "stdinc.h"

// #include "mclib.h"

#ifndef MOVEMGR_H
#include "movemgr.h"
#endif

#ifndef WARRIOR_H
#include "warrior.h"
#endif



//#include "gameos.hpp"

int32_t MovePathManager::m_numpaths = 0;
int32_t MovePathManager::m_peakpaths = 0;
int32_t MovePathManager::m_sourcetally[50];
MovePathManagerPtr PathManager = nullptr;

//***************************************************************************
// PATH MANAGER class
//***************************************************************************

PVOID
MovePathManager::operator new(size_t mySize)
{
	PVOID result = systemHeap->Malloc(mySize);
	return (result);
}

//---------------------------------------------------------------------------

void
MovePathManager::operator delete(PVOID us)
{
	systemHeap->Free(us);
}

//---------------------------------------------------------------------------

int32_t
MovePathManager::init(void)
{
	int32_t i;
	for (i = 0; i < MAX_MOVERS; i++)
	{
		m_pool[i].pilot = nullptr;
		m_pool[i].selectionindex = 0;
		m_pool[i].moveparams = 0;
		if (i > 0)
			m_pool[i].prev = &m_pool[i - 1];
		else
			m_pool[i].prev = nullptr;
		if (i < (MAX_MOVERS - 1))
			m_pool[i].next = &m_pool[i + 1];
		else
			m_pool[i].next = nullptr;
	}
	//------------------------------
	// All start on the free list...
	queueFront = nullptr;
	queueEnd = nullptr;
	freeList = &m_pool[0];
	m_numpaths = 0;
	m_peakpaths = 0;
	for (i = 0; i < 50; i++)
		m_sourcetally[i] = 0;
	return (NO_ERROR);
}

//---------------------------------------------------------------------------

void
MovePathManager::remove(std::unique_ptr<PathQueueRec> rec)
{
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
	m_sourcetally[rec->num]--;
	m_numpaths--;
}

//---------------------------------------------------------------------------

std::unique_ptr<PathQueueRec>
MovePathManager::remove(std::unique_ptr<MechWarrior> pilot)
{
	std::unique_ptr<PathQueueRec> rec = pilot->getMovePathRequest();
	if (rec)
	{
		remove(rec);
		pilot->setMovePathRequest(nullptr);
		return (rec);
	}
	return (nullptr);
}

//---------------------------------------------------------------------------

#define DEBUG_MOVEPATH_QUEUE 0

void
MovePathManager::request(
	std::unique_ptr<MechWarrior> pilot,
	int32_t selectionindex,
	uint32_t moveparams,
	int32_t source)
{
	//-----------------------------------------------------
	// If the pilot is already awaiting a calc, purge it...
	remove(pilot);
	if (!freeList)
		Fatal(0, " Too many pilots calcing paths ");
	//---------------------------------------------
	// Grab the first free move path rec in line...
	std::unique_ptr<PathQueueRec> pathQRec = freeList;
	//-----------------------------------------
	// Cut the new record from the free list...
	freeList = freeList->next;
	if (freeList)
		freeList->prev = nullptr;
	//---------------------------------------------------
	// New record has no next. Already has no previous...
	pathQRec->num = source;
	pathQRec->pilot = pilot;
	pathQRec->selectionindex = selectionindex;
	pathQRec->moveparams = moveparams;
	if (queueEnd)
	{
		queueEnd->next = pathQRec;
		pathQRec->prev = queueEnd;
		pathQRec->next = nullptr;
		queueEnd = pathQRec;
	}
	else
	{
		pathQRec->prev = nullptr;
		pathQRec->next = nullptr;
		queueFront = queueEnd = pathQRec;
	}
	pilot->setMovePathRequest(pathQRec);
	m_numpaths++;
	m_sourcetally[source]++;
	if (m_numpaths > m_peakpaths)
		m_peakpaths = m_numpaths;
}

//---------------------------------------------------------------------------

void
MovePathManager::calcPath(void)
{
	if (queueFront)
	{
		//------------------------------
		// Grab the next in the queue...
		std::unique_ptr<PathQueueRec> curQRec = queueFront;
		remove(queueFront);
		//--------------------------------------------------
		// If the mover is no longer around, don't bother...
		std::unique_ptr<MechWarrior> pilot = curQRec->pilot;
		pilot->setMovePathRequest(nullptr);
		std::unique_ptr<Mover> mover = pilot->getVehicle();
		/*int32_t err = */ pilot->calcMovePath(curQRec->selectionindex, curQRec->moveparams);
	}
}

#if CONSIDERED_OBSOLETE
//----------------------------------------------------------------------------------
void
DEBUGWINS_print(const std::wstring_view& s, int32_t window);
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
void
MovePathManager::update(void)
{
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
	// if (numpaths > 15)
	//	numPathsToProcess = 10;
	for (size_t i = 0; i < numPathsToProcess; i++)
	{
		if (!queueFront)
			break;
		calcPath();
	}
//	char s[50];
//	sprintf(s, "num paths = %d", numpaths);
//	DEBUGWINS_print(s, 0);
#ifdef MC_PROFILE
	QueryPerformanceCounter(endCk);
	srMvPathUpd += (endCk.LowPart - startCk.LowPart);
#endif
}

#endif

//***************************************************************************
