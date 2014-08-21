//===========================================================================//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//
#include "stdafx.h"

#ifndef GOAL_H
#include "goal.h"
#endif

#ifndef GAMEOBJ_H
#include "gameobj.h"
#endif

#ifndef TERROBJ_H
#include "terrobj.h"
#endif

#ifndef GATE_H
#include "gate.h"
#endif

#ifndef OBJMGR_H
#include "objmgr.h"
#endif

#ifndef BLDNG_H
#include "bldng.h"
#endif

#include "heap.h"
#include <mclib.h>
extern UserHeapPtr missionHeap;

//***************************************************************************

int32_t adjCell[4][2] = {
	{-1, 0},
	{0, 1},
	{1, 0},
	{0, -1}
};

int32_t recurseCount = 0;

//***************************************************************************

PVOID GoalObject::operator new (size_t ourSize) {

	PVOID result = missionHeap->Malloc(ourSize);
	return(result);
}

//---------------------------------------------------------------------------

void GoalObject::operator delete (PVOID us) {

	missionHeap->Free(us);
}

//---------------------------------------------------------------------------

void GoalObject::init (void) {

	used = false;
	type = GOAL_OBJECT;
	id = -1;
	name[0] = NULL;
	links = NULL;
	controller = NULL;
	info.object.WID = 0;
	next = NULL;
	prev = NULL;
}

//---------------------------------------------------------------------------

void GoalObject::initObject (PSTR name, GameObjectPtr obj) {
}

//---------------------------------------------------------------------------

void GoalObject::initRegion (PSTR name, int32_t minRow, int32_t minCol, int32_t maxRow, int32_t maxCol) {

	init();
	used = true;
	type = GOAL_REGION;
	strcpy(name, "MAP");
	info.region.minRow = minRow;
}

//---------------------------------------------------------------------------

void GoalObject::destroy (void) {

}

//---------------------------------------------------------------------------

void GoalObject::addLink (GoalObjectPtr gobject, GoalLinkType type) {

	GoalLinkPtr newLink = (GoalLink*)missionHeap->Malloc(sizeof(GoalLink));
	
}

//---------------------------------------------------------------------------

void GoalObject::addController (GoalObjectPtr gobject) {
}

//***************************************************************************
// ACTION GOAL MANAGER
//***************************************************************************

//#ifdef USE_REGION_MAP

PVOID GoalManager::operator new (size_t ourSize) {

	PVOID result = missionHeap->Malloc(ourSize);
	return(result);
}

//---------------------------------------------------------------------------

void GoalManager::operator delete (PVOID us) {

	missionHeap->Free(us);
}

//---------------------------------------------------------------------------

void GoalManager::init (void) {

	numGoalObjects = 0;
	goalObjectPoolSize = 0;
	goalObjects = NULL;
	goalObjectPool = NULL;

#ifdef USE_REGION_MAP
	for (int32_t r = 0; r < GameMap->height; r++)
		for (int32_t c = 0; c < GameMap->width; c++)
			regionMap[r][c] = -1;
#endif
	numRegions = 0;

	fillStack = NULL;
	fillStackIndex = 0;
}

//---------------------------------------------------------------------------

void GoalManager::destroy (void) {

	if (goalObjectPool) {
		missionHeap->Free(goalObjectPool);
		goalObjectPool = NULL;
	}
	numGoalObjects = 0;
	goalObjectPoolSize = 0;
}

//---------------------------------------------------------------------------

void GoalManager::clear (void) {

	goalObjects = NULL;
	numGoalObjects = 0;
	for (int32_t i = 0; i < goalObjectPoolSize; i++)
		goalObjectPool[i].used = false;

#ifdef USE_REGION_MAP
	for (int32_t r = 0; r < GameMap->height; r++)
		for (int32_t c = 0; c < GameMap->width; c++)
			regionMap[r][c] = -1;
#endif
	numRegions = 0;
}

//---------------------------------------------------------------------------

GoalObjectPtr GoalManager::newGoalObject (void) {

	GoalObjectPtr goalObject = NULL;
	for (int32_t i = 0; i < goalObjectPoolSize; i++)
		if (!goalObjectPool[i].used) {
			goalObjectPool[i].used = true;
			goalObject = &goalObjectPool[i];
			break;
		}

	if (!goalObject)
		Fatal(0, " GoalManager.newGoalObject: no more GoalObjects allowed ");
	return(goalObject);
}

//---------------------------------------------------------------------------

void GoalManager::setup (int32_t poolSize) {

	goalObjectPoolSize = poolSize;
	if (goalObjectPoolSize  < 10)
		Fatal(0, " GoalManager.setup: goalObjectPoolSize must be greater than 10 ");

	goalObjectPool = (GoalObjectPtr)missionHeap->Malloc(sizeof(GoalObject) * goalObjectPoolSize);
	clear();
}

//---------------------------------------------------------------------------
#if 0
bool GlobalMap::fillNorthSouthBridgeArea (int32_t row, int32_t col, int32_t area) {

	//----------------------------------------------------------------------
	// It is assumed that the bridge is erected over non-passable terrain...
	if ((row < minRow) || (row >= maxRow) || (col < minCol) || (col >= maxCol))
		return(false);

	areaMap[row * width + col] = area;

	//----------------
	// Expand North...
	int32_t adjR = row - 1;
	int32_t adjC = col;
	if ((adjR >= minRow) && (adjR < maxRow) && (adjC >= minCol) && (adjC < maxCol))
		if (GameMap->getOverlay(adjR, adjC) == OVERLAY_WATER_BRIDGE_NS)
			if (areaMap[adjR * width + adjC] == -1)
				fillNorthSouthBridgeArea(adjR, adjC, area);

	//----------------
	// Expand South...
	adjR = row + 1;
	adjC = col;
	if ((adjR >= minRow) && (adjR < maxRow) && (adjC >= minCol) && (adjC < maxCol))
		if (GameMap->getOverlay(adjR, adjC) == OVERLAY_WATER_BRIDGE_NS)
			if (areaMap[adjR * width + adjC] == -1)
				fillNorthSouthBridgeArea(adjR, adjC, area);
	
	return(true);
}

//------------------------------------------------------------------------------------------

bool GlobalMap::fillEastWestBridgeArea (int32_t row, int32_t col, int32_t area) {

	//----------------------------------------------------------------------
	// It is assumed that the bridge is erected over non-passable terrain...
	if ((row < minRow) || (row >= maxRow) || (col < minCol) || (col >= maxCol))
		return(false);

	areaMap[row * width + col] = area;

	//---------------
	// Expand East...
	int32_t adjR = row;
	int32_t adjC = col + 1;
	if ((adjR >= minRow) && (adjR < maxRow) && (adjC >= minCol) && (adjC < maxCol)) {
		int32_t overlay = GameMap->getOverlay(adjR, adjC);
		if (overlay == OVERLAY_WATER_BRIDGE_EW)
			if (areaMap[adjR * width + adjC] == -1)
				fillEastWestBridgeArea(adjR, adjC, area);
	}

	//---------------
	// Expand West...
	adjR = row;
	adjC = col - 1;
	if ((adjR >= minRow) && (adjR < maxRow) && (adjC >= minCol) && (adjC < maxCol)) {
		int32_t overlay = GameMap->getOverlay(adjR, adjC);
		if (overlay == OVERLAY_WATER_BRIDGE_EW)
			if (areaMap[adjR * width + adjC] == -1)
				fillEastWestBridgeArea(adjR, adjC, area);
	}
	
	return(true);
}

//------------------------------------------------------------------------------------------

#endif

bool GoalManager::fillWallGateRegion (int32_t row, int32_t col, int32_t region) {

	recurseCount++;

	//----------------------------------------------------------------------
	// It is assumed that the bridge is erected over non-passable terrain...
	if ((row < 0) || (row >= GameMap->height) || (col < 0) || (col >= GameMap->width))
		return(false);

	if (!GameMap->getWall(row, col) && !GameMap->getGate(row, col))
		return(false);

	regionMap[row][col] = region;
	for (int32_t dir = 0; dir < 4; dir ++) {
		int32_t adjR = row + adjCell[dir][0];
		int32_t adjC = col + adjCell[dir][1];
		if ((adjR >= 0) && (adjR < GameMap->height) && (adjC >= 0) && (adjC < GameMap->width))
			if (regionMap[adjR][adjC] == -1)
				fillWallGateRegion(adjR, adjC, region);
	}
	
	return(true);
}

//------------------------------------------------------------------------------------------

bool GoalManager::fillRegion (int32_t row, int32_t col, int32_t region) {

#if 1

//	int32_t overlay = GameMap->getOverlay(row, col);
//	if ((overlay == OVERLAY_WATER_BRIDGE_EW) || (overlay == OVERLAY_WATER_BRIDGE_NS))
//		return(false);
		
	if (GameMap->getWall(row, col) || GameMap->getGate(row, col))
		return(false);
		
	if (!GameMap->getPassable(row, col)) {
		regionMap[row][col] = -2;
		return(false);
	}

	fillStack[fillStackIndex++] = row;
	fillStack[fillStackIndex++] = col;

	while (fillStackIndex > 0) {
		//--------------------------------
		// Pop 'em in the reverse order...
		int32_t col = fillStack[--fillStackIndex];
		int32_t row = fillStack[--fillStackIndex];

		bool filling = true;
		
		if ((row < 0) || (row >= GameMap->height) || (col < 0) || (col >= GameMap->width))
			filling = false;
		
//		int32_t overlay = GameMap->getOverlay(row, col);
//		if ((overlay == OVERLAY_WATER_BRIDGE_EW) || (overlay == OVERLAY_WATER_BRIDGE_NS))
//			filling = false;
		
		if (GameMap->getWall(row, col) || GameMap->getGate(row, col))
			filling = false;
		
		if (!GameMap->getPassable(row, col)) {
			regionMap[row][col] = -2;
			filling = false;
		}
		
		if (filling) {
			regionMap[row][col] = region;
			for (int32_t dir = 0; dir < 4; dir ++) {
				int32_t adjR = row + adjCell[dir][0];
				int32_t adjC = col + adjCell[dir][1];
				if ((adjR >= 0) && (adjR < GameMap->height) && (adjC >= 0) && (adjC < GameMap->width))
					if (regionMap[adjR][adjC] == -1) {
						//--------------------------------------------
						// Cell hasn't been visited yet, so push it...
						fillStack[fillStackIndex++] = adjR;
						fillStack[fillStackIndex++] = adjC;
						//----------------------------------------------------------------
						// Mark the cell as on the stack (so we do not push it again if we
						// hit it before popping it)...
						regionMap[adjR][adjC] = -3;
					}
			}
		}
	}
	return(true);
#else
	if ((row < 0) || (row >= GameMap->height) || (col < 0) || (col >= GameMap->width))
		return(false);

	//----------------------------------------------------------------------
	// If we hit a bridge cell, politely stop expanding this area into it...
	int32_t overlay = GameMap->getOverlay(row, col);
	if ((overlay == OVERLAY_WATER_BRIDGE_EW) || (overlay == OVERLAY_WATER_BRIDGE_NS))
		return(false);

	if (GameMap->getWall(row, col) || GameMap->getGate(row, col))
		return(false);

	if (!GameMap->getPassable(row, col)) {
		regionMap[row][col] = -2;
		return(false);
	}

	regionMap[row][col] = region;
	for (int32_t dir = 0; dir < 4; dir ++) {
		int32_t adjR = row + adjCell[dir][0];
		int32_t adjC = col + adjCell[dir][1];
		if ((adjR >= 0) && (adjR < GameMap->height) && (adjC >= 0) && (adjC < GameMap->width))
			if (regionMap[adjR][adjC] == -1)
				fillRegion(adjR, adjC, region);
	}
	
	return(true);
#endif
}

//------------------------------------------------------------------------------------------

void GoalManager::calcRegions (void) {

	//----------------------------------------------------------------------
	// This is the same method used in GlobalMap::calcAreas, so see notes...
	for (int32_t r = 0; r < GameMap->height; r++)
		for (int32_t c = 0; c < GameMap->width; c++)
			if (regionMap[r][c] == -1) {
				recurseCount = 0;
				#ifdef USE_OVERLAYS
				int32_t overlay = GameMap->getOverlay(r, c);
				if (overlay == OVERLAY_WATER_BRIDGE_NS) {
					if (fillNorthSouthBridgeArea(r, c, numAreas))
						numAreas++;
					}
				else if (overlay == OVERLAY_WATER_BRIDGE_EW) {
					if (fillEastWestBridgeArea(r, c, numAreas))
						numAreas++;
					}
				#endif
				if (GameMap->getWall(r, c) || GameMap->getGate(r, c)) {
					if (fillWallGateRegion(r, c, numRegions))
						numRegions++;
					}
				else if (fillRegion(r, c, numRegions))
					numRegions++;
			}
}

//------------------------------------------------------------------------------------------

void GoalManager::build (void) {
	
	//For temps, pull 'em off the windows heap.  IT can resize.  OURS cannot!!!!
	fillStack = (short*)malloc(FILL_STACK_SIZE * sizeof(short));
	gosASSERT(fillStack != NULL);
	fillStackIndex = 0;

	//--------------------------------
	// First, get the list of walls...
	GameObjectPtr wallObjects[MAX_WALL_OBJECTS];
	int32_t numWalls = ObjectManager->getSpecificObjects(BUILDING, BUILDING_SUBTYPE_WALL, wallObjects, MAX_WALL_OBJECTS);
/*	short cellList[MAX_CELL_COORDS];
	for (int32_t i = 0; i < numWalls; i++) {
		cellList[0] = MAX_CELL_COORDS;
		int32_t numCells = wallObjects[i]->appearance->markMoveMap(true, NULL, false, cellList);
		for (int32_t j = 0; j < numCells; j++)
			GameMap->setWall(cellList[j*2], cellList[j*2+1], true);
	}

	//--------------------------------------------------
	// Close all gates, before calcing the region map...
	for (i = 0; i < ObjectManager->getNumGates(); i++) {
		GatePtr gate = ObjectManager->getGate(i);
		gate->getAppearance()->markMoveMap(false, NULL);
	}
*/
//	calcRegions();


/*
	//--------------------------------------
	// Now, open the gates (for the game)...
	for (i = 0; i < ObjectManager->getNumGates(); i++) {
		GatePtr gate = ObjectManager->getGate(i);
		gate->getAppearance()->markMoveMap(true, NULL);
	}
*/
	free(fillStack);
	fillStack = NULL;
	fillStackIndex = 0;
}

//---------------------------------------------------------------------------

/*
int32_t GoalManager::setControl (ObstaclePtr controller, ObstaclePtr controllee) {

	if (controller && controllee) {
		controllee->parent = controller;
		controllee->prev = NULL;
		controllee->next = controller->controls;
		controllee->controls = NULL;
		controller->controls->prev = controllee;
		controller->controls = controllee;
	}

	return(0);
}
*/
//---------------------------------------------------------------------------

GoalObjectPtr GoalManager::addRegion (GoalObjectPtr parent, GoalLinkType linkType, PSTR name, int32_t minRow, int32_t minCol, int32_t maxRow, int32_t maxCol) {

	GoalObjectPtr newRegion = newGoalObject();
	newRegion->initRegion(name, minRow, minCol, maxRow, maxCol);
	if (parent)
		parent->addLink(newRegion, linkType);
	else
		goalObjects = newRegion;
	return(newRegion);
}

//---------------------------------------------------------------------------

GoalObjectPtr GoalManager::addObject (GoalObjectPtr parent, GoalLinkType linkType, PSTR name, GameObjectPtr object) {

	GoalObjectPtr newObject = newGoalObject();
	newObject->initObject(name, object);
	if (parent)
		parent->addLink(newObject, linkType);
	else
		goalObjects = newObject;
	return(newObject);
}

//---------------------------------------------------------------------------

/*
void GoalManager::setControlMap (int32_t row, int32_t col, GoalObjectPtr controller) {

	controlMap[row][col] = controller->id;
}
*/
//---------------------------------------------------------------------------

GoalObjectPtr GoalManager::calcGoal (int32_t startCell[2], int32_t goalCell[2]) {

	//No Warnings!
	//ObstaclePtr startObstacle = &obstaclePool[controlMap[startCell[0]][startCell[1]]];
	//ObstaclePtr goalObstacle = &obstaclePool[controlMap[goalCell[0]][goalCell[1]]];

	/* DO SEARCH HERE */

	return(NULL);
}

//---------------------------------------------------------------------------

GoalObjectPtr GoalManager::calcGoal (GameObjectPtr attacker, GameObjectPtr target) {

	return(NULL);
}

//---------------------------------------------------------------------------

GoalObjectPtr GoalManager::calcGoal (GameObjectPtr attacker, Stuff::Vector3D location) {

	return(NULL);
}

//---------------------------------------------------------------------------

GoalObjectPtr GoalManager::calcGoal (Stuff::Vector3D start, Stuff::Vector3D location) {

	int32_t startCell[2], locationCell[2];
	land->worldToCell(start, startCell[0], startCell[1]);
	land->worldToCell(location, locationCell[0], locationCell[1]);
	return(calcGoal(startCell, locationCell));
}

//***************************************************************************
