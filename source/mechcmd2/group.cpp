//******************************************************************************************
//
//	group.cpp - This file contains the MoverGroup Class header
//
//	MechCOmmander 2
//
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//
#include "stdafx.h"

#ifndef MCLIB_H
#include <mclib.h>
#endif

#ifndef GROUP_H
#include "group.h"
#endif

#ifndef GAMEOBJ_H
#include "gameobj.h"
#endif

#ifndef MOVER_H
#include "mover.h"
#endif

#ifndef OBJMGR_H
#include "objmgr.h"
#endif

#ifndef GVEHICL_H
#include "gvehicl.h"
#endif

#ifndef UNITDESG_H
#include "unitdesg.h"
#endif

#ifndef TACORDR_H
#include "tacordr.h"
#endif

#ifndef MOVE_H
#include "move.h"
#endif

//---------------------------------------------------------------------------

extern float	DelayedOrderTime;
extern int32_t		tileMulMAPCELL_DIM[MAX_MAP_CELL_WIDTH];

//#define	MAX_GROUPMOVE_OFFSETS	4

/*
int32_t GroupMoveOffsetsIndex[MAX_GROUPMOVE_OFFSETS] = {0, 1, 3, 6};
float GroupMoveOffsets[10][2] = {
	// 2-member Group
	{180.0, 50.0},
	// 3-member Group
	{-135.0, 50.0},
	{135.0, 50.0},
	// 4-member Group
	{-135.0, 50.0},
	{135.0, 50.0},
	{180.0, 50.0},
	// 5-member Group
	{-135.0, 50.0},
	{135.0, 50.0},
	{180.0, 50.0},
	{180.0, 75.0}
};
*/
extern char OverlayIsBridge[NUM_OVERLAY_TYPES];
extern PriorityQueuePtr	openList;
GoalMapNode* MoverGroup::goalMap = NULL;

//***************************************************************************
// MOVERGROUP class
//***************************************************************************

PVOID MoverGroup::operator new (size_t ourSize) {

	PVOID result = systemHeap->Malloc(ourSize);
	return(result);
}

//---------------------------------------------------------------------------

void MoverGroup::operator delete (PVOID us) {

	systemHeap->Free(us);
}	

//---------------------------------------------------------------------------

void MoverGroup::destroy (void) {
}

//---------------------------------------------------------------------------

bool MoverGroup::add (MoverPtr mover) {

	if (numMovers == MAX_MOVERGROUP_COUNT) {
		Fatal(0, " MoverGroup.add: Group too big ");
		//----------------------------------------
		// Should we choose to remove the fatal...
		return(false);
	}

	moverWIDs[numMovers++] = mover->getWatchID();
	mover->setGroupId(id, true);

	return(true);
}

//---------------------------------------------------------------------------

bool MoverGroup::remove (MoverPtr mover) {

	GameObjectWatchID moverWID = mover->getWatchID();
	if (moverWID == pointWID) {
		disband();
		return(true);
		}
	else {
		for (int32_t i = 0; i < numMovers; i++)
			if (moverWIDs[i] == moverWID) {
				mover->setGroupId(-1, true);
				moverWIDs[i] = moverWIDs[numMovers - 1];
				moverWIDs[numMovers - 1] = 0;
				numMovers--;
				return(true);
			}
	}
	return(false);
}

//---------------------------------------------------------------------------

bool MoverGroup::isMember (MoverPtr mover) {

	if (!mover)
		return(false);

	GameObjectWatchID moverWID = mover->getWatchID();
	for (int32_t i = 0; i < numMovers; i++)
		if (moverWIDs[i] == moverWID)
			return(true);
	return(false);
}

//---------------------------------------------------------------------------

int32_t MoverGroup::disband (void) {

	for (int32_t i = 0; i < numMovers; i++) {
		MoverPtr mover = getMover(i);
		mover->setGroupId(-1, true);
	}
#ifdef USE_IFACE
	if (pointHandle)
		theInterface->setPoint(pointHandle, false);
#endif
	pointWID = 0;
	numMovers = 0;

	return(NO_ERROR);
}

//---------------------------------------------------------------------------

int32_t MoverGroup::setPoint (MoverPtr mover) {

	if (isMember(mover)) {
#ifdef USE_IFACE
		if (pointHandle)
			theInterface->setPoint(pointHandle, false);
#endif
		pointWID = mover->getWatchID();
#ifdef USE_IFACE
		theInterface->setPoint(mover->getPartId(), true);
#endif
	}

	return(NO_ERROR);
}

//---------------------------------------------------------------------------

MoverPtr MoverGroup::getPoint (void) {

	return(dynamic_cast<MoverPtr>(ObjectManager->getByWatchID(pointWID)));
}

//---------------------------------------------------------------------------

MoverPtr MoverGroup::getMover (int32_t i) {

	MoverPtr mover = dynamic_cast<MoverPtr>(ObjectManager->getByWatchID(moverWIDs[i]));
	if (!mover)
		Fatal(0, " MoverGroup.getMover: NULL mover ");
	return(mover);
}

//---------------------------------------------------------------------------

MoverPtr MoverGroup::selectPoint (bool excludePoint) {

	for (int32_t i = 0; i < numMovers; i++)
		if (!excludePoint || (moverWIDs[i] != pointWID)) {
			MoverPtr mover = getMover(i);
			MechWarriorPtr pilot = mover->getPilot();
			if (pilot && pilot->alive()) {
				//----------------------------------------
				// Found a legitimate point, so set him...
				setPoint(mover);
				return(mover);
			}
		}

	//-----------------------
	// No legitimate point...
	setPoint(NULL);
	return(NULL);
}

//---------------------------------------------------------------------------

int32_t MoverGroup::getMovers (MoverPtr* moverList) {

	if (numMovers > 0)
		for (int32_t i = 0; i < numMovers; i++)
			moverList[i] = getMover(i);
	return(numMovers);
}

//---------------------------------------------------------------------------

MechWarriorPtr MoverGroup::getPointPilot (void) {

	if (pointWID)
		return(ObjectManager->getByWatchID(pointWID)->getPilot());
	return(NULL);
}

//---------------------------------------------------------------------------

void MoverGroup::statusCount (int32_t* statusTally) {

	for (int32_t i = 0; i < numMovers; i++) {
		MoverPtr mover = getMover(i);
		MechWarriorPtr pilot = mover->getPilot();
		if (!mover->getExists())
			statusTally[8]++;
		else if (!mover->getAwake())
			statusTally[7]++;
		else if (pilot && (pilot->getStatus() == WARRIOR_STATUS_WITHDRAWN))
			statusTally[6]++;
		else
			statusTally[mover->getStatus()]++;
	}
}

//---------------------------------------------------------------------------

void MoverGroup::addToGUI (bool visible) {

#ifdef USE_IFACE
	for (int32_t i = 0; i < numMovers; i++)
		theInterface->AddMech(movers[i]->getPartId(), id, movers[i]->getAwake(), visible);
#endif
}

//---------------------------------------------------------------------------

inline bool inMapBounds (int32_t r, int32_t c, int32_t mapHeight, int32_t mapWidth) {

	return((r >= 0) && (r < mapHeight) && (c >= 0) && (c < mapWidth));
}

//---------------------------------------------------------------------------

#define	JUMPMAP_TILE_DIM	3
#define	JUMPMAP_CELL_DIM	MAPCELL_DIM * JUMPMAP_TILE_DIM

char CellSpiralIncrement[JUMPMAP_CELL_DIM * JUMPMAP_CELL_DIM * 2] = {
	-1, 0,
	0, 1,
	1, 0,
	1, 0,
	0, -1,
	0, -1,
	-1, 0,
	-1, 0,

	-1, 0,
	0, 1,
	0, 1,
	0, 1,
	1, 0,
	1, 0,
	1, 0,
	1, 0,
	0, -1,
	0, -1,
	0, -1,
	0, -1,
	-1, 0,
	-1, 0,
	-1, 0,
	-1, 0,

	-1, 0,
	0, 1,
	0, 1,
	0, 1,
	0, 1,
	0, 1,
	1, 0,
	1, 0,
	1, 0,
	1, 0,
	1, 0,
	1, 0,
	0, -1,
	0, -1,
	0, -1,
	0, -1,
	0, -1,
	0, -1,
	-1, 0,
	-1, 0,
	-1, 0,
	-1, 0,
	-1, 0,
	-1, 0,

	-1, 0,
	0, 1,
	0, 1,
	0, 1,
	0, 1,
	0, 1,
	0, 1,
	0, 1,
	1, 0,
	1, 0,
	1, 0,
	1, 0,
	1, 0,
	1, 0,
	1, 0,
	1, 0,
	0, -1,
	0, -1,
	0, -1,
	0, -1,
	0, -1,
	0, -1,
	0, -1,
	0, -1,
	-1, 0,
	-1, 0,
	-1, 0,
	-1, 0,
	-1, 0,
	-1, 0,
	-1, 0,
	-1, 0
};

//---------------------------------------------------------------------------

void MoverGroup::sortMovers (int32_t numMoversInGroup, MoverPtr* moverList, Stuff::Vector3D destination) {

	Mover::sortList->clear();
	for (int32_t i = 0; i < numMoversInGroup; i++) {
		int32_t index = -1;
		float dist = (float)3.48E+37;
		if (moverList[i]) {
			index = i;
			Stuff::Vector3D resultVector;
			resultVector.Subtract(moverList[i]->getPosition(), destination);
			dist = resultVector.GetLength();
		}
		Mover::sortList->setId(i, index);
		Mover::sortList->setValue(i, dist);
	}
	Mover::sortList->sort(false);
	for (i = 0; i < numMoversInGroup; i++) {
		int32_t moverIndex = Mover::sortList->getId(i);
		if (moverIndex != -1)
			moverList[moverIndex]->selectionIndex = i;
	}
}

//---------------------------------------------------------------------------

#define	GOALMAP_DIM	61

int32_t MoverGroup::calcMoveGoals (Stuff::Vector3D goal, int32_t numMovers, Stuff::Vector3D* goalList) {

	if ( !numMovers ) // 07/24/HKG: crashes if no movers
		return 0;

	if (!goalMap) {
		goalMap = (GoalMapNode*)systemHeap->Malloc(sizeof(GoalMapNode) * GOALMAP_DIM * GOALMAP_DIM);
		if (!goalMap)
			Fatal(0, " MoverGroup.calcMoveGoals: unable to malloc goalMap ");
	}

	int32_t goalRow, goalCol;
	land->worldToCell(goal, goalRow, goalCol);
	int32_t topLeftRow = goalRow - GOALMAP_DIM / 2;
	int32_t topLeftCol = goalCol - GOALMAP_DIM / 2;
	for (int32_t r = 0; r < GOALMAP_DIM; r++)
		for (int32_t c = 0; c < GOALMAP_DIM; c++) {
			if (!inMapBounds(topLeftRow + r, topLeftCol + c, GameMap->height, GameMap->width))
				continue;
			int32_t index = r * GOALMAP_DIM + c;
			goalMap[index].cost = GameMap->getPassable(topLeftRow + r, topLeftCol + c) ? 100 : COST_BLOCKED;
			goalMap[index].flags = GOALFLAG_AVAILABLE + GOALFLAG_NO_NEIGHBORS;
			goalMap[index].g = 0;
		}

	//-----------------------------------------------
	// If we haven't already, create the OPEN list...
	if (!openList) {
		openList = new PriorityQueue;
		gosASSERT(openList != NULL);
		openList->init(5000);
	}

	int32_t curRow = GOALMAP_DIM / 2;
	int32_t curCol = GOALMAP_DIM / 2;
	
	GoalMapNode* curMapNode = &goalMap[curRow * GOALMAP_DIM + curCol];

	//-----------------------------------------------------------------
	// Put the START (the goal, in this case) on the empty OPEN list...
	PQNode initialVertex;
	initialVertex.key = curMapNode->g;
	initialVertex.id = curRow * GOALMAP_DIM + curCol;
	initialVertex.row = curRow;
	initialVertex.col = curCol;
	openList->clear();
	openList->insert(initialVertex);
	curMapNode->setFlag(GOALFLAG_OPEN);

	int32_t numGoalsFound = 0;
	while (!openList->isEmpty()) {

		//----------------------
		// Grab the best node...
		PQNode bestPQNode;
		openList->remove(bestPQNode);
		curRow = bestPQNode.row;
		curCol = bestPQNode.col;
		GoalMapNode* bestMapNode = &goalMap[curRow * GOALMAP_DIM + curCol];
		bestMapNode->clearFlag(GOALFLAG_OPEN);

		//----------------------------
		// Now, close the best node...
		bestMapNode->setFlag(GOALFLAG_CLOSED);

		bool markNeighbors = false;
		//------------------------------
		// Have we found a valid goal...
		if (bestMapNode->cost < COST_BLOCKED)
			if (bestMapNode->flags & GOALFLAG_NO_NEIGHBORS)
				if (bestMapNode->flags & GOALFLAG_AVAILABLE) {
					bestMapNode->clearFlag(GOALFLAG_AVAILABLE);
					land->cellToWorld(topLeftRow + curRow, topLeftCol + curCol, goalList[numGoalsFound++]);
					markNeighbors = true;
				}
		if (numGoalsFound == numMovers)
			break;

		if (markNeighbors) {
			static int32_t cellShift[8][2] = {{-1, 0}, {-1, 1}, {0, 1}, {1, 1}, {1, 0}, {1, -1}, {0, -1}, {-1, -1}};
			for (int32_t i = 0; i < 8; i++) {
				int32_t nRow = curRow + cellShift[i][0];
				int32_t nCol = curRow + cellShift[i][1];
				if (inMapBounds(nRow, nCol, GOALMAP_DIM, GOALMAP_DIM))
					goalMap[nRow * GOALMAP_DIM + nCol].clearFlag(GOALFLAG_NO_NEIGHBORS);
			}
		}

		int32_t bestNodeG = bestMapNode->g;
		for (int32_t dir = 0; dir < 4; dir++) {
			//------------------------------------------------------------
			// First, make sure this is a legit direction to go. We do NOT
			// move diagonally, when checking these cells...
			//------------------------------------------------------------
			// Now, process this direction. First, calc the cell to check,
			// offset from the current cell...
			static int32_t cellShift[4][2] = {{-1, 0}, {0, 1}, {1, 0}, {0, -1}};
			int32_t succRow = curRow + cellShift[dir][0];
			int32_t succCol = curCol + cellShift[dir][1];
			//--------------------------------
			// If it's on the map, check it...
			if (inMapBounds(succRow, succCol, GOALMAP_DIM, GOALMAP_DIM)) {
				GoalMapNode* succMapNode = &goalMap[succRow * GOALMAP_DIM + succCol];
				if (succMapNode->cost < COST_BLOCKED) {
					int32_t succNodeG = bestNodeG + succMapNode->cost;
					if ((succMapNode->flags & (GOALFLAG_OPEN + GOALFLAG_CLOSED)) == 0) {
						//-------------------------------------------------
						// This node is neither OPEN nor CLOSED, so toss it
						// into the OPEN list...
						//succMapNode->parent = dirToParent;
						succMapNode->g = succNodeG;
						PQNode succPQNode;
						succPQNode.key = succMapNode->g;
						succPQNode.id = succRow * GOALMAP_DIM + succCol;
						succPQNode.row = succRow;
						succPQNode.col = succCol;
						openList->insert(succPQNode);
						succMapNode->setFlag(MOVEFLAG_OPEN);
					}
				}
			}
		}
	}		

	return(numGoalsFound);
}

//---------------------------------------------------------------------------

#define DEBUGJUMPGOALS TRUE

int32_t MoverGroup::calcJumpGoals (Stuff::Vector3D goal, int32_t numMovers, Stuff::Vector3D* goalList, GameObjectPtr DFATarget) {

	int32_t numJumping = 0;

	//-----------------------------
	// First, build the jump map...
	int32_t jumpMap[JUMPMAP_CELL_DIM][JUMPMAP_CELL_DIM];

	//------------------------------------------------------------
	// The initial goal tile is placed at the center of the map...
	int32_t goalCell[2] = {0, 0};
	land->worldToCell(goal, goalCell[0], goalCell[1]);

	int32_t mapCellUL[2] = {0, 0};
	mapCellUL[0] = goalCell[0] - JUMPMAP_CELL_DIM / 2;
	mapCellUL[1] = goalCell[1] - JUMPMAP_CELL_DIM / 2;

	// -1 = OPEN
	// -2 = BLOCKED
	// 0 thru # = already selected for that # mover in the group
	for (int32_t r = 0; r < JUMPMAP_CELL_DIM; r++)
		for (int32_t c = 0; c < JUMPMAP_CELL_DIM; c++) {
			int32_t cellRow = mapCellUL[0] + r;
			int32_t cellCol = mapCellUL[1] + c;
			if (GameMap->inBounds(cellRow, cellCol)) {
				MapCellPtr mapCell = GameMap->getCell(cellRow, cellCol);

				//-----------------------
				// Tile (terrain) type...
				//int32_t tileType = curTile.getTileType();
				if (!mapCell->getPassable())
					jumpMap[r][c] = -2;
				else
					jumpMap[r][c] = -1;

#ifdef USE_OVERLAYS_IN_MC2
				int32_t overlay = mapCell->getOverlay();
				if (OverlayIsBridge[overlay]) {
					switch (overlay) {
						case OVERLAY_WATER_BRIDGE_NS:
						case OVERLAY_RAILROAD_WATER_BRIDGE_NS:
							jumpMap[row][col] = -2;
							jumpMap[row][col + 2] = -2;
							jumpMap[row + 1][col] = -2;
							jumpMap[row + 1][col + 2] = -2;
							jumpMap[row + 2][col] = -2;
							jumpMap[row + 2][col + 2] = -2;
							break;
						case OVERLAY_WATER_BRIDGE_EW:
						case OVERLAY_RAILROAD_WATER_BRIDGE_EW:
							jumpMap[row][col] = -2;
							jumpMap[row][col + 1] = -2;
							jumpMap[row][col + 2] = -2;
							jumpMap[row + 2][col] = -2;
							jumpMap[row + 2][col + 1] = -2;
							jumpMap[row + 2][col + 2] = -2;
							break;
						case OVERLAY_WATER_BRIDGE_NS_DESTROYED:
						case OVERLAY_RAILROAD_WATER_BRIDGE_NS_DESTROYED:
						case OVERLAY_WATER_BRIDGE_EW_DESTROYED:
						case OVERLAY_RAILROAD_WATER_BRIDGE_EW_DESTROYED:
							jumpMap[row][col] = -2;
							jumpMap[row][col + 1] = -2;
							jumpMap[row][col + 2] = -2;
							jumpMap[row + 1][col] = -2;
							jumpMap[row + 1][col + 1] = -2;
							jumpMap[row + 1][col + 2] = -2;
							jumpMap[row + 2][col] = -2;
							jumpMap[row + 2][col + 1] = -2;
							jumpMap[row + 2][col + 2] = -2;
							break;
					}
				}
#endif
				}
			else
				jumpMap[r][c] = -2;
		}

	int32_t moverCount = ObjectManager->getNumMovers();
	for (int32_t i = 0; i < moverCount; i++) {
		MoverPtr mover = ObjectManager->getMover(i);
		if ((mover->getObjectClass() != ELEMENTAL) && (mover != DFATarget) && !mover->isDisabled()) {
			int32_t mapCellRow, mapCellCol;
			mover->getCellPosition(mapCellRow, mapCellCol);
			mapCellRow -= mapCellUL[0];
			mapCellCol -= mapCellUL[1];
			if (inMapBounds(mapCellRow, mapCellCol, JUMPMAP_CELL_DIM, JUMPMAP_CELL_DIM))
				jumpMap[mapCellRow][mapCellCol] = -2;
		}
	}

#ifdef _DEBUG
#if DEBUGJUMPGOALS
	char debugStr[256];
	sprintf(debugStr, "GROUP JUMP(%.2f,%.2f,%.2f)--UL = %d,%d: ", goal.x, goal.y, goal.z,mapCellUL[0], mapCellUL[1]);
#endif
#endif

	//-----------------------------------------------------------------
	// Now, for each jumper, select a closest cell to the goal, mark it
	// as theirs and close it...
	for (i = 0; i < numMovers; i++) {
		//int32_t startCellRow = 0;
		//int32_t startCellCol = 0;

		int32_t curCellRow = goalCell[0] - mapCellUL[0];
		int32_t curCellCol = goalCell[1] - mapCellUL[1];
		bool notFound = true;
		int32_t spiralIndex = 0;
		while (notFound) {
			if (jumpMap[curCellRow][curCellCol] == -1) {
				// Should check to see if the cell is within range...
				//----------------------------------
				// Found an open cell, so take it...
				jumpMap[curCellRow][curCellCol] = i;
				land->cellToWorld(mapCellUL[0] + curCellRow, mapCellUL[1] + curCellCol, goalList[i]);
				numJumping++;
				notFound = false;
#ifdef _DEBUG
#if DEBUGJUMPGOALS
				char s[30];
				sprintf(s, "[%d,%d] ", curCellRow, curCellCol);
				strcat(debugStr, s);
#endif
#endif
				}
			else {
				//-------------------------------------
				// Go to the next cell in our search...
				do {
					if (spiralIndex == (JUMPMAP_CELL_DIM * JUMPMAP_CELL_DIM * 2)) {
						goalList[i].x = -99999.0;
						goalList[i].y = -99999.0;
						goalList[i].z = -99999.0;
						notFound = false;
						break;
					}
					curCellRow += CellSpiralIncrement[spiralIndex++];
					curCellCol += CellSpiralIncrement[spiralIndex++];
				} while (!inMapBounds(curCellRow, curCellCol, JUMPMAP_CELL_DIM, JUMPMAP_CELL_DIM));
			}
		}
	}

	return(numJumping);
}

//---------------------------------------------------------------------------

int32_t MoverGroup::calcJumpGoals (Stuff::Vector3D goal, Stuff::Vector3D* goalList, GameObjectPtr DFATarget) {

	return(calcJumpGoals(goal, numMovers, goalList, DFATarget));
}

//---------------------------------------------------------------------------

int32_t MoverGroup::handleTacticalOrder (TacticalOrder tacOrder, int32_t priority, Stuff::Vector3D* jumpGoalList, bool queueGroupOrder) {

	if (numMovers == 0)
		return(NO_ERROR);

	if (queueGroupOrder)
		tacOrder.pack(NULL, NULL);

	//bool processOrder = true;
	bool isJump = false;
	bool isMove = false;
	Stuff::Vector3D goalList[MAX_MOVERGROUP_COUNT];
	Stuff::Vector3D location = tacOrder.getWayPoint(0);
	//MoverPtr pointVehicle = getPoint();

	if (tacOrder.code == TACTICAL_ORDER_ATTACK_OBJECT)
		if (tacOrder.attackParams.method == ATTACKMETHOD_DFA) {
			//-------------------------------------------------
			// Let's just make it a move/jump order, for now...
			tacOrder.code = TACTICAL_ORDER_JUMPTO_OBJECT;
			tacOrder.moveParams.wait = false;
			tacOrder.moveParams.wayPath.mode[0] = TRAVEL_MODE_SLOW;
			GameObjectPtr target = ObjectManager->getByWatchID(tacOrder.targetWID);
			Assert(tacOrder.targetWID != 0, 0, " DFA AttackObject WID is 0 ");
			if (!target)
				return(NO_ERROR);
			tacOrder.setWayPoint(0, target->getPosition());
		}
	if (tacOrder.code == TACTICAL_ORDER_JUMPTO_OBJECT) {
		tacOrder.code = TACTICAL_ORDER_JUMPTO_POINT;
		GameObjectPtr target = ObjectManager->get(tacOrder.targetWID);
		Assert(tacOrder.targetWID != 0, 0, " DFA AttackObject WID is 0 ");
		if (!target)
			return(NO_ERROR);
		tacOrder.setWayPoint(0, target->getPosition());
	}

	//vector_3d offsetTable[MAX_GROUPMOVE_OFFSETS];
	//int32_t numOffsets = 0;

	switch (tacOrder.code) {
		case TACTICAL_ORDER_WAIT:
			break;

		case TACTICAL_ORDER_MOVETO_POINT:
		case TACTICAL_ORDER_MOVETO_OBJECT: {
Fatal(0, "Need to support jumpGoalList (and goalList) for MOVETO as well in mc2 ");
			isMove = true;
			//-----------------------------------------------------------
			// Sort by distance to destination. Their selectionIndex will
			// be set to modify this goal...
			SortListPtr list = Mover::sortList;
			if (list) {
				list->clear(false);
				int32_t moverCount = 0;
				for (int32_t i = 0; i < numMovers; i++) {
					MoverPtr mover = getMover(i);
					Assert(mover != NULL, moverWIDs[i], " MoverGroup.handleTacticalOrder: NULL mover ");
					if (!mover->isDisabled()) {
						list->setId(moverCount, i);
						list->setValue(moverCount, mover->distanceFrom(location));
						moverCount++;
					}
				}
				list->sort(false);
				//--------------------------------
				// Let's build the offset table...
				/*
				numOffsets = moverCount - 1;
				if (numOffsets > MAX_GROUPMOVE_OFFSETS)
					numOffsets = MAX_GROUPMOVE_OFFSETS;
				int32_t offsetsStart = GroupMoveOffsetsIndex[numOffsets - 1];
				for (i = 0; i < numOffsets; i++)
					offsetTable[i] = relativePositionToPoint(location, GroupMoveOffsets[offsetsStart + i][0], GroupMoveOffsets[offsetsStart + i][1], RELPOS_FLAG_PASSABLE_START);
				*/
				//-----------------------------------
				// Now, calc the order of movement...
				int32_t curIndex = 1;
				for (i = 0; i < moverCount; i++) {
					MoverPtr mover = getMover(list->getId(i));
					if (mover->getWatchID() == pointWID)
						mover->selectionIndex = 0;
					else
						mover->selectionIndex = curIndex++;
				}
			}
			}
			break;

		case TACTICAL_ORDER_JUMPTO_POINT:
		case TACTICAL_ORDER_JUMPTO_OBJECT: {
			//-----------------------------------------------------------
			// Sort by distance to destination. Their selectionIndex will
			// be set to modify this goal...
			isJump = true;
			//-------------------------------------------------------------------------
			// We can assume that all movers in this group are jump-capable. Otherwise,
			// the group wouldn't be allowed to jump by the interface. In addition,
			// we KNOW that all movers in this group can jump to the selected
			// goal (of course, they won't due to terrain and crowding)...
			GameObjectPtr target = ObjectManager->getByWatchID(tacOrder.targetWID);
			if (jumpGoalList)
				for (int32_t j = 0; j < numMovers; j++)
					goalList[j] = jumpGoalList[j];
			else
				calcJumpGoals(tacOrder.getWayPoint(0), goalList, target);

			for (int32_t i = 0; i < numMovers; i++) {
				MoverPtr mover = getMover(i);
				bool canJump = (goalList[i].x > -99000.0);
				if (canJump)
					mover->selectionIndex = 0;
				else
					mover->selectionIndex = -2;
			}
			}
			break;

		case TACTICAL_ORDER_TRAVERSE_PATH:
		case TACTICAL_ORDER_PATROL_PATH:
		case TACTICAL_ORDER_ESCORT:
		case TACTICAL_ORDER_FOLLOW:
		case TACTICAL_ORDER_GUARD:
		case TACTICAL_ORDER_STOP:
		case TACTICAL_ORDER_POWERUP:
		case TACTICAL_ORDER_POWERDOWN:
		case TACTICAL_ORDER_WAYPOINTS_DONE:
		case TACTICAL_ORDER_EJECT:
		case TACTICAL_ORDER_ATTACK_OBJECT:
		case TACTICAL_ORDER_ATTACK_POINT:
		case TACTICAL_ORDER_HOLD_FIRE:
		case TACTICAL_ORDER_WITHDRAW:
		case TACTICAL_ORDER_CAPTURE:
		case TACTICAL_ORDER_LOAD_INTO_CARRIER:
		case TACTICAL_ORDER_REFIT:
		case TACTICAL_ORDER_RECOVER:
		case TACTICAL_ORDER_GETFIXED:
			break;

		default: {
			char s[256];
			sprintf(s, "Unit::handleTacticalOrder->Bad TacOrder Code (%d)", tacOrder.code);
			Assert(false, tacOrder.code, s);
			return(1);
			}
	}

	tacOrder.unitOrder = true;
	for (int32_t i = 0; i < numMovers; i++) {
		MoverPtr mover = getMover(i);
		if (mover && !mover->isDisabled()) {
			if (mover->selectionIndex == -2) {
				mover->selectionIndex = -1;
				continue;
			}
			tacOrder.selectionIndex = mover->selectionIndex;
			if (tacOrder.selectionIndex != -1) {
				if (isMove)
					tacOrder.setWayPoint(0, location);
				else if (isJump)
					tacOrder.setWayPoint(0, goalList[i]);
				tacOrder.delayedTime = scenarioTime + (mover->selectionIndex * DelayedOrderTime);
			}
			switch (tacOrder.origin) {
				case ORDER_ORIGIN_PLAYER:
					if (queueGroupOrder)
						mover->getPilot()->addQueuedTacOrder(tacOrder);
					else {
						if (mover->getPilot()->getNumTacOrdersQueued())
							//This is a hack to simply trigger the execution of
							//the queued orders. The current order is ignored (and
							//is simply used for this trigger)...
							mover->getPilot()->executeTacOrderQueue();
						else
							mover->getPilot()->setPlayerTacOrder(tacOrder);
					}
					break;
				case ORDER_ORIGIN_COMMANDER:
					mover->getPilot()->setGeneralTacOrder(tacOrder);
					break;
				case ORDER_ORIGIN_SELF:
					mover->getPilot()->setAlarmTacOrder(tacOrder, priority);
					break;
			}
			mover->selectionIndex = -1;
		}
	}

	return(NO_ERROR);
}

//---------------------------------------------------------------------------
// TAC ORDERS
//---------------------------------------------------------------------------

int32_t MoverGroup::orderMoveToPoint (bool setTacOrder, int32_t origin, Stuff::Vector3D location, uint32_t params) {

	int32_t result = TACORDER_FAILURE;

	for (int32_t i = 0; i < numMovers; i++) {
		Assert(getMover(i) != NULL, 0, " MoverGroup.orderMoveToPoint: NULL mover ");
		MechWarriorPtr pilot = getMover(i)->getPilot();
		if (pilot)
			result = pilot->orderMoveToPoint(true, setTacOrder, origin, location, -1, params);
	}

	return(result);
}

//---------------------------------------------------------------------------

int32_t MoverGroup::orderMoveToObject (bool setTacOrder, int32_t origin, GameObjectPtr target, int32_t fromArea, uint32_t params) {

	int32_t result = TACORDER_FAILURE;

	for (int32_t i = 0; i < numMovers; i++) {
		Assert(getMover(i) != NULL, 0, " MoverGroup.orderMoveToObject: NULL mover ");
		MechWarriorPtr pilot = getMover(i)->getPilot();
		if (pilot)
			result = pilot->orderMoveToObject(true, setTacOrder, origin, target, fromArea, -1, params);
	}

	return(result);
}

//---------------------------------------------------------------------------

int32_t MoverGroup::orderTraversePath (int32_t origin, WayPathPtr wayPath, uint32_t params) {

	int32_t result = TACORDER_FAILURE;

	for (int32_t i = 0; i < numMovers; i++) {
		Assert(getMover(i) != NULL, 0, " MoverGroup.orderTraversePath: NULL mover ");
		MechWarriorPtr pilot = getMover(i)->getPilot();
		if (pilot)
			result = pilot->orderTraversePath(true, true, origin, wayPath, params);
	}

	return(result);
}

//---------------------------------------------------------------------------

int32_t MoverGroup::orderPatrolPath (int32_t origin, WayPathPtr wayPath) {

	int32_t result = TACORDER_FAILURE;

	for (int32_t i = 0; i < numMovers; i++) {
		Assert(getMover(i) != NULL, 0, " MoverGroup.orderPatrolPath: NULL mover ");
		MechWarriorPtr pilot = getMover(i)->getPilot();
		if (pilot)
			result = pilot->orderPatrolPath(true, true, origin, wayPath);
	}

	return(result);
}

//---------------------------------------------------------------------------

int32_t MoverGroup::orderPowerDown (int32_t origin) {

	int32_t result = TACORDER_FAILURE;

	for (int32_t i = 0; i < numMovers; i++) {
		Assert(getMover(i) != NULL, 0, " MoverGroup.orderPowerDown: NULL mover ");
		MechWarriorPtr pilot = getMover(i)->getPilot();
		if (pilot)
			result = pilot->orderPowerDown(true, origin);
	}

	return(result);
}

//---------------------------------------------------------------------------

int32_t MoverGroup::orderPowerUp (int32_t origin) {

	int32_t result = TACORDER_FAILURE;

	for (int32_t i = 0; i < numMovers; i++) {
		Assert(getMover(i) != NULL, 0, " MoverGroup.orderPowerUp: NULL mover ");
		MechWarriorPtr pilot = getMover(i)->getPilot();
		if (pilot)
			result = pilot->orderPowerUp(true, origin);
	}

	return(result);
}

//---------------------------------------------------------------------------

int32_t MoverGroup::orderAttackObject (int32_t origin, GameObjectPtr target, int32_t attackType, int32_t attackMethod, int32_t attackRange, int32_t aimLocation, int32_t fromArea, uint32_t params) {

	int32_t result = TACORDER_FAILURE;

	for (int32_t i = 0; i < numMovers; i++) {
		Assert(getMover(i) != NULL, 0, " MoverGroup.orderAttackObject: NULL mover ");
		MechWarriorPtr pilot = getMover(i)->getPilot();
		if (pilot)
			result = pilot->orderAttackObject(true, origin, target, attackType, attackMethod, attackRange, aimLocation, fromArea, params);
	}

	return(result);
}

//---------------------------------------------------------------------------

int32_t MoverGroup::orderWithdraw (int32_t origin, Stuff::Vector3D location) {

	int32_t result = TACORDER_FAILURE;

	for (int32_t i = 0; i < numMovers; i++) {
		Assert(getMover(i) != NULL, 0, " MoverGroup.orderWithdraw: NULL mover ");
		MechWarriorPtr pilot = getMover(i)->getPilot();
		if (pilot)
			result = pilot->orderWithdraw(true, origin, location);
	}

	return(result);
}

//---------------------------------------------------------------------------

int32_t MoverGroup::orderEject (int32_t origin) {

	int32_t result = TACORDER_FAILURE;

	for (int32_t i = 0; i < numMovers; i++) {
		Assert(getMover(i) != NULL, 0, " MoverGroup.orderEject: NULL mover ");
		MechWarriorPtr pilot = getMover(i)->getPilot();
		if (pilot)
			result = pilot->orderEject(true, true, origin);
	}

	return(result);
}

//---------------------------------------------------------------------------
// COMBAT EVENTS
//---------------------------------------------------------------------------

void MoverGroup::triggerAlarm (int32_t alarmCode, uint32_t triggerId) {

	for (int32_t i = 0; i < numMovers; i++) {
		MechWarriorPtr pilot = getMover(i)->getPilot();
		if (pilot)
			pilot->triggerAlarm(alarmCode, triggerId);
	}
}

//---------------------------------------------------------------------------

int32_t MoverGroup::handleMateCrippled (uint32_t mateWID) {

	triggerAlarm(PILOT_ALARM_FRIENDLY_VEHICLE_CRIPPLED, mateWID);
	return(NO_ERROR);
}

//---------------------------------------------------------------------------

int32_t MoverGroup::handleMateDisabled (uint32_t mateWID) {

	//triggerAlarm(PILOT_ALARM_DEATH_OF_MATE, mateId);
	return(NO_ERROR);
}

//---------------------------------------------------------------------------

int32_t MoverGroup::handleMateDestroyed (uint32_t mateWID) {

	triggerAlarm(PILOT_ALARM_DEATH_OF_MATE, mateWID);
	return(NO_ERROR);
}

//---------------------------------------------------------------------------

int32_t MoverGroup::handleMateEjected (uint32_t mateWID) {

	//triggerAlarm(PILOT_ALARM_DEATH_OF_MATE, mateId);
	return(NO_ERROR);
}

//---------------------------------------------------------------------------

void MoverGroup::handleMateFiredWeapon (uint32_t mateWID) {

	triggerAlarm(PILOT_ALARM_MATE_FIRED_WEAPON, mateWID);
}

//***************************************************************************
void MoverGroup::copyTo (MoverGroupData &data)
{
	data.id = id;
	data.numMovers = numMovers;
	memcpy(data.moverWIDs,moverWIDs,sizeof(GameObjectWatchID) * MAX_MOVERGROUP_COUNT);
	data.pointWID = pointWID;
	data.disbandOnNoPoint = disbandOnNoPoint;
}

//***************************************************************************
void MoverGroup::init (MoverGroupData &data)
{
	id = data.id;
	numMovers = data.numMovers;
	memcpy(moverWIDs,data.moverWIDs,sizeof(GameObjectWatchID) * MAX_MOVERGROUP_COUNT);
	pointWID = data.pointWID;
	disbandOnNoPoint = data.disbandOnNoPoint;
}

//***************************************************************************

