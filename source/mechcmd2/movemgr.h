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

#define MAX_MOVERS 255 // Should probably equal that in Collision System

struct PathQueueRec
{
	std::unique_ptr<MechWarrior> pilot;
	int32_t num = 0;
	int32_t selectionindex = 0;
	uint32_t moveparams;
	bool initPath;
	bool faceObject;
	//std::unique_ptr<PathQueueRec> prev;
	//std::unique_ptr<PathQueueRec> next;
};

//---------------------------------------------------------------------------

class MovePathManager
{
public:
	MovePathManager(void) noexcept { }
	~MovePathManager(void) noexcept = default;

	//PVOID operator new(size_t ourSize);
	//void operator delete(PVOID us);
	int32_t init(void);
	void remove(std::unique_ptr<PathQueueRec> rec);
	std::unique_ptr<PathQueueRec> remove(std::unique_ptr<MechWarrior> pilot);
	void request(std::unique_ptr<MechWarrior> pilot, int32_t selectionindex, uint32_t moveparams, int32_t source);
	void calcPath(void);
	void update(void);

protected:
	std::vector<PathQueueRec> m_pool; // PathQueueRec pool[MAX_MOVERS];
	//std::unique_ptr<PathQueueRec> queueFront;
	//std::unique_ptr<PathQueueRec> queueEnd;
	//std::unique_ptr<PathQueueRec> freeList;
	static int32_t m_numpaths;
	static int32_t m_peakpaths;
	std::array<int32_t, 50> m_sourcetally;
};

typedef MovePathManager* MovePathManagerPtr;

#endif

//***************************************************************************
