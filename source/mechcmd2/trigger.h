//---------------------------------------------------------------------------
//
//	trigger.h - This file contains the class declaration for TriggerArea
//
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#pragma once

#ifndef TRIGGER_H
#define TRIGGER_H

//#include <terrain.h>
//#include "dmover.h"
//#include "dstd.h"

//---------------------------------------------------------------------------

#define MAX_TRIGGER_AREAS 16

typedef enum
{
	TRIGGER_AREA_NONE,
	TRIGGER_AREA_MOVER,
	TRIGGER_AREA_TEAM,
	TRIGGER_AREA_GROUP,
	TRIGGER_AREA_COMMANDER,
	NUM_TRIGGER_AREA_TYPES
} TriggerAreaType;

typedef struct
{
	char type;
	int32_t param;
	int32_t dim[4];
	bool hit;
} TriggerArea;

class TriggerAreaManager
{

  public:
	TriggerArea triggerAreas[MAX_TRIGGER_AREAS];
	uint8_t map[MAX_MAP_CELL_WIDTH / 3][MAX_MAP_CELL_WIDTH / 3];

  public:
	PVOID operator new(size_t mySize);

	void operator delete(PVOID us);

	void init(void)
	{
		for (size_t i = 0; i < MAX_TRIGGER_AREAS; i++)
			triggerAreas[i].type = TRIGGER_AREA_NONE;
		for (size_t r = 0; r < MAX_MAP_CELL_WIDTH / 3; r++)
			for (size_t c = 0; c < MAX_MAP_CELL_WIDTH / 3; c++)
				map[r][c] = 0;
	}

	TriggerAreaManager(void) { init(void); }

	void destroy(void);

	~TriggerAreaManager(void) { destroy(void); }

	int32_t add(int32_t ULrow, int32_t ULcol, int32_t LRrow, int32_t LRcol,
		int32_t type, int32_t param);

	void remove(int32_t areaHandle);

	void reset(int32_t areaHandle);

	bool isHit(int32_t areaHandle);

	void setHit(MoverPtr mover);
};

//---------------------------------------------------------------------------
#endif
