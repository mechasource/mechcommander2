//===========================================================================//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//
#include "stdafx.h"

#ifndef TRIGGER_H
#include "trigger.h"
#endif

#include "mover.h"
//***************************************************************************

PVOID TriggerAreaManager::operator new(size_t ourSize)
{
	PVOID result = systemHeap->Malloc(ourSize);
	return (result);
}

//---------------------------------------------------------------------------

void TriggerAreaManager::operator delete(PVOID us) { systemHeap->Free(us); }

//---------------------------------------------------------------------------

void TriggerAreaManager::destroy(void) {}

//---------------------------------------------------------------------------

int32_t TriggerAreaManager::add(int32_t ULrow, int32_t ULcol, int32_t LRrow,
	int32_t LRcol, int32_t type, int32_t param)
{
	for (size_t i = 1; i < MAX_TRIGGER_AREAS; i++)
		if (triggerAreas[i].type == TRIGGER_AREA_NONE)
			break;
	if (i < MAX_TRIGGER_AREAS)
	{
		triggerAreas[i].dim[0] = ULrow;
		triggerAreas[i].dim[1] = ULcol;
		triggerAreas[i].dim[2] = LRrow;
		triggerAreas[i].dim[3] = LRcol;
		triggerAreas[i].type   = type;
		triggerAreas[i].param  = param;
		triggerAreas[i].hit	= false;
		int32_t tileDims[4];
		tileDims[0] = ULrow / 3;
		tileDims[1] = ULcol / 3 + 1;
		tileDims[2] = LRrow / 3;
		tileDims[3] = LRcol / 3 + 1;
		for (size_t r = tileDims[0]; r < tileDims[2]; r++)
			for (size_t c = tileDims[1]; c < tileDims[3]; c++)
			{
				// sert((map[r][c] == 0) || (map[r][c] == i), i," Duplicate
				// Trigger ");
				map[r][c] = i;
			}
		return (i);
	}
	return (0);
}

//---------------------------------------------------------------------------

void TriggerAreaManager::remove(int32_t areaHandle)
{
	if (triggerAreas[areaHandle].param == TRIGGER_AREA_NONE)
		return;
	int32_t tileDims[4];
	tileDims[0] = triggerAreas[areaHandle].dim[0] / 3;
	tileDims[1] = triggerAreas[areaHandle].dim[1] / 3 + 1;
	tileDims[2] = triggerAreas[areaHandle].dim[2] / 3;
	tileDims[3] = triggerAreas[areaHandle].dim[3] / 3 + 1;
	for (size_t r = tileDims[0]; r < tileDims[2]; r++)
		for (size_t c = tileDims[1]; c < tileDims[3]; c++)
			if (map[r][c] == areaHandle)
				map[r][c] = 0;
	triggerAreas[areaHandle].type = TRIGGER_AREA_NONE;
	triggerAreas[areaHandle].hit  = false;
}

//---------------------------------------------------------------------------

void TriggerAreaManager::reset(int32_t areaHandle)
{
	triggerAreas[areaHandle].hit = false;
}

//---------------------------------------------------------------------------

bool TriggerAreaManager::isHit(int32_t areaHandle)
{
	if (triggerAreas[areaHandle].type == TRIGGER_AREA_NONE)
		return (false);
	return (triggerAreas[areaHandle].hit);
}

//---------------------------------------------------------------------------
void DEBUGWINS_print(PSTR s, int32_t window);

void TriggerAreaManager::setHit(MoverPtr mover)
{
#if 1
	for (size_t i = 0; i < MAX_TRIGGER_AREAS; i++)
	{
		if (triggerAreas[i].type == TRIGGER_AREA_NONE)
			continue;
		if (mover->cellPositionRow >= triggerAreas[i].dim[0])
			if (mover->cellPositionRow < triggerAreas[i].dim[2])
				if (mover->cellPositionCol >= triggerAreas[i].dim[1])
					if (mover->cellPositionCol < triggerAreas[i].dim[3])
						switch (triggerAreas[i].type)
						{
						case TRIGGER_AREA_MOVER:
							if (triggerAreas[i].param > 0)
							{
								if (mover->getWatchID() ==
									triggerAreas[i].param)
									triggerAreas[i].hit = true;
							}
							else
								triggerAreas[i].hit = true;
							break;
						case TRIGGER_AREA_TEAM:
							if (mover->getTeamId() == triggerAreas[i].param)
							{
								triggerAreas[i].hit = true;
								// sprintf(s, "AREA HIT: %d", i);
								// DEBUGWINS_print(s, 0);
							}
							break;
						case TRIGGER_AREA_GROUP:
							if (mover->getGroupId() == triggerAreas[i].param)
								triggerAreas[i].hit = true;
							break;
						case TRIGGER_AREA_COMMANDER:
							if (mover->getCommanderId() ==
								triggerAreas[i].param)
								triggerAreas[i].hit = true;
							break;
						}
	}
#else
	int32_t areaHandle =
		map[mover->cellPositionRow / 3][mover->cellPositionCol / 3];
	if (areaHandle > 0)
	{
		switch (triggerAreas[areaHandle].type)
		{
		case TRIGGER_AREA_MOVER:
			if (triggerAreas[areaHandle].param > 0)
			{
				if (mover->getWatchID() == triggerAreas[areaHandle].param)
					triggerAreas[areaHandle].hit = true;
			}
			else
				triggerAreas[areaHandle].hit = true;
			break;
		case TRIGGER_AREA_TEAM:
			if (mover->getTeamId() == triggerAreas[areaHandle].param)
				triggerAreas[areaHandle].hit = true;
			break;
		case TRIGGER_AREA_GROUP:
			if (mover->getGroupId() == triggerAreas[areaHandle].param)
				triggerAreas[areaHandle].hit = true;
			break;
		case TRIGGER_AREA_COMMANDER:
			if (mover->getCommanderId() == triggerAreas[areaHandle].param)
				triggerAreas[areaHandle].hit = true;
			break;
		}
	}
#endif
}

//***************************************************************************
