//===========================================================================//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#pragma once

#ifndef GOAL_H
#define GOAL_H

//#include "dgoal.h"
//#include "dgameobj.h"
//#include "move.h"

#define	MAX_GATES					30
#define	MAX_MOVEGATES_CONTROLLED	10
#define	MAX_MOVEGATES_PER_ROOM		4
#define	MAX_OBJECTS_PER_ROOM		10
#define	MAX_CONTROLLED_OBJECTS		200

typedef enum {
	GOAL_NONE,
	GOAL_OBJECT,
	GOAL_REGION,
	NUM_GOAL_TYPES
} GoalType;

typedef enum {
	GOAL_LINK_NONE,
	GOAL_LINK_MOVE,
	GOAL_LINK_CONTROLS,
	GOAL_LINK_CONTROLLED,
	NUM_GOAL_LINK_TYPES
} GoalLinkType;

typedef union {
	struct {
		uint32_t		WID;
	} object;
	struct {
		int16_t				minRow;
		int16_t				minCol;
		int16_t				maxRow;
		int16_t				maxCol;
	} region;
} GoalInfo;

typedef struct _GoalLink {
	GoalObjectPtr			goalObject;
	GoalLinkType			type;
	int32_t					cost;
	struct _GoalLink*		next;
} GoalLink;

typedef struct _GoalPathFindInfo {
	int32_t					cost;
	int32_t					parent;
	int32_t					fromIndex;
	uint32_t			flags;
	int32_t					g;
	int32_t					hPrime;
	int32_t					fPrime;
} GoalPathFindInfo;

typedef GoalLink* GoalLinkPtr;

class GoalObject {

public:

	bool				used;
	GoalType			type;
	uint16_t		id;
	char				name[20];
	GoalLinkPtr			links;
	GoalObjectPtr		controller;
	GoalInfo			info;
	GoalObjectPtr		next;
	GoalObjectPtr		prev;
	GoalPathFindInfo	pathInfo;

public:

	PVOID operator new (size_t ourSize);

	void operator delete (PVOID us);

	void init (void);

	GoalObject (void) {
		init (void);
	}

	void destroy (void);

	~GoalObject (void)	{
		destroy(void);
	}

	void initObject (PSTR name, GameObjectPtr obj);

	void initRegion (PSTR name, int32_t minRow, int32_t minCol, int32_t maxRow, int32_t maxCol);

	void addLink (GoalObjectPtr gobject, GoalLinkType linkType);

	void addController (GoalObjectPtr gobject);
};

class GoalManager {

public:

	int32_t			numGoalObjects;
	GoalObjectPtr	goalObjects;
	int32_t			goalObjectPoolSize;
	GoalObjectPtr	goalObjectPool;
	int16_t			regionMap[2/*MAX_MAP_CELL_WIDTH*/][2/*MAX_MAP_CELL_WIDTH*/];
	int32_t			numRegions;
	pint16_t			fillStack;
	int32_t			fillStackIndex;

public:

	PVOID operator new (size_t ourSize);

	void operator delete (PVOID us);

	void init (void);

	GoalManager (void) {
		init (void);
	}

	void destroy (void);

	~GoalManager (void)	{
		destroy(void);
	}

	void setup (int32_t poolSize);

	void build (void);

	bool fillWallGateRegion (int32_t row, int32_t col, int32_t region);

	bool fillRegion (int32_t row, int32_t col, int32_t region);

	void calcRegions (void);

	int32_t addLinks (GoalObjectPtr gobject, int32_t numObjs, GameObjectPtr* objList);

	//int32_t setControl (GoalObjectPtr controller, GoalObjectPtr controllee);

	GoalObjectPtr addRegion (GoalObjectPtr parent, GoalLinkType linkType, PSTR name, int32_t minRow, int32_t minCol, int32_t maxRow, int32_t maxCol);

	GoalObjectPtr addObject (GoalObjectPtr parent, GoalLinkType linkType, PSTR name, GameObjectPtr object);

	void clear (void);

	GoalObjectPtr newGoalObject (void);

	GoalObjectPtr calcGoal (int32_t startCell[2], int32_t goalCell[2]);

	GoalObjectPtr calcGoal (GameObjectPtr attacker, GameObjectPtr target);

	GoalObjectPtr calcGoal (GameObjectPtr attacker, Stuff::Vector3D location);

	GoalObjectPtr calcGoal (Stuff::Vector3D start, Stuff::Vector3D location);
};

#endif
