//***************************************************************************
//
//	move.cp - This file contains the Movement\Pathfinding routines
//
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

//--------------
// Include Files
// #include "mclib.h"

#ifndef MOVE_H
#include "move.h"
#endif

#define MOVELOG

#ifdef MOVELOG
#ifndef GAMELOG_H
#include "gamelog.h"
#endif
#endif

//***************************************************************************

#define USE_SEPARATE_WATER_MAPS FALSE

#ifndef TEAM_H
enum class
{
	RELATION_FRIENDLY,
	RELATION_NEUTRAL,
	RELATION_ENEMY,
	NUM_RELATIONS
} Relation;
#define MAX_TEAMS 8
#endif

//------------
// EXTERN vars

// extern void memclear(PVOID Dest,int32_t Length);
extern UserHeapPtr systemHeap;
extern float metersPerWorldUnit;

extern std::wstring_view ExceptionGameMsg;
extern wchar_t ChunkDebugMsg[5120];

int32_t GlobalMap::minRow = 0;
int32_t GlobalMap::maxRow = 0;
int32_t GlobalMap::minCol = 0;
int32_t GlobalMap::maxCol = 0;
GameLogPtr GlobalMap::log = nullptr;
bool GlobalMap::logEnabled = false;
//------------
// GLOBAL vars
bool ZeroHPrime = false;
bool CullPathAreas = false;

const float LOSincrement = 0.33f; // based upon cell size
const float LOFincrement = 0.33f; // based upon cell size
const float LOSensorIncrement = 2.0; // based upon cell size (big, since we care only about tiles)
bool debugMoveMap = false;
bool ClearBridgeTiles = false;

int32_t RamObjectWID = 0;

PriorityQueuePtr openList = nullptr;
bool JumpOnBlocked = false;
bool FindingEscapePath = false;
bool BlockWallTiles = true;
MissionMapPtr GameMap = nullptr;
GlobalMapPtr GlobalMoveMap[3] = {nullptr, nullptr, nullptr};

extern float worldUnitsPerMeter; // Assumes 90 pixel mechs
extern float metersPerWorldUnit; // Assumes 90 pixel mechs

int32_t DebugMovePathType = 0;

int32_t SimpleMovePathRange = 21;

wchar_t reverseDir[NUM_DIRECTIONS] = {4, 5, 6, 7, 0, 1, 2, 3};
wchar_t rowShift[NUM_DIRECTIONS] = {-1, -1, 0, 1, 1, 1, 0, -1};
wchar_t colShift[NUM_DIRECTIONS] = {0, 1, 1, 1, 0, -1, -1, -1};

#define NUM_CELL_OFFSETS 128
int32_t cellShift[NUM_CELL_OFFSETS * 2] = {-1, 0, -1, 1, 0, 1, 1, 1, 1, 0, 1, -1, 0, -1, -1, -1,

	-2, 0, -2, 2, 0, 2, 2, 2, 2, 0, 2, -2, 0, -2, -2, -2,

	-3, 0, -3, 3, 0, 3, 3, 3, 3, 0, 3, -3, 0, -3, -3, -3,

	-3, 2, -2, 3, 2, 3, 3, 2, 3, -2, 2, -3, -2, -3, -3, -2,

	-4, 0, -4, 4, 0, 4, 4, 4, 4, 0, 4, -4, 0, -4, -4, -4,

	-5, 0, -5, 5, 0, 5, 5, 5, 5, 0, 5, -5, 0, -5, -5, -5,

	-6, 0, -6, 6, 0, 6, 6, 6, 6, 0, 6, -6, 0, -6, -6, -6,

	-7, 0, -7, 7, 0, 7, 7, 7, 7, 0, 7, -7, 0, -7, -7, -7,

	-8, 0, -8, 8, 0, 8, 8, 8, 8, 0, 8, -8, 0, -8, -8, -8,

	-9, 0, -9, 9, 0, 9, 9, 9, 9, 0, 9, -9, 0, -9, -9, -9,

	-10, 0, -10, 10, 0, 10, 10, 10, 10, 0, 10, -10, 0, -10, -10, -10,

	-11, 0, -11, 11, 0, 11, 11, 11, 11, 0, 11, -11, 0, -11, -11, -11,

	-12, 0, -12, 12, 0, 12, 12, 12, 12, 0, 12, -12, 0, -12, -12, -12,

	-13, 0, -13, 13, 0, 13, 13, 13, 13, 0, 13, -13, 0, -13, -13, -13,

	-14, 0, -14, 14, 0, 14, 14, 14, 14, 0, 14, -14, 0, -14, -14, -14,

	-15, 0, -15, 15, 0, 15, 15, 15, 15, 0, 15, -15, 0, -15, -15, -15};

float cellShiftDistance[NUM_CELL_OFFSETS];
wchar_t reverseShift[NUM_CELL_OFFSETS] = {4, 5, 6, 7, 0, 1, 2, 3, 12, 13, 14, 15, 8, 9, 10, 11, 20, 21,
	22, 23, 16, 17, 18, 19, 28, 29, 30, 31, 24, 25, 26, 27, 36, 37, 38, 39, 32, 33, 34, 35, 44, 45,
	46, 47, 40, 41, 42, 43, 52, 53, 54, 55, 48, 49, 50, 51, 60, 61, 62, 63, 56, 57, 58, 59, 68, 69,
	70, 71, 64, 65, 66, 67, 76, 77, 78, 79, 72, 73, 74, 75, 84, 85, 86, 87, 80, 81, 82, 83, 92, 93,
	94, 95, 88, 89, 90, 91, 100, 101, 102, 103, 96, 97, 98, 99, 108, 109, 110, 111, 104, 105, 106,
	107, 116, 117, 118, 119, 112, 113, 114, 115, 124, 125, 126, 127, 120, 121, 122, 123};

int32_t tileMulMAPCELL_DIM[MAX_MAP_CELL_WIDTH];
float MapCellDiagonal = 0.0;
float HalfMapCell = 0.0;
float VerticesMapSideDivTwo = 0.0;
float MetersMapSideDivTwo = 0.0;

bool IsDiagonalStep[NUM_CELL_OFFSETS] = {false, true, false, true, false, true, false, true, false,
	false, false, false, false, false, false, false, false, false, false, false, false, false,
	false, false, false, false, false, false, false, false, false, false, false, false, false,
	false, false, false, false, false, false, false, false, false, false, false, false, false,
	false, false, false, false, false, false, false, false, false, false, false, false, false,
	false, false, false, false, false, false, false, false, false, false, false, false, false,
	false, false, false, false, false, false, false, false, false, false, false, false, false,
	false, false, false, false, false, false, false, false, false, false, false, false, false,
	false, false, false, false, false, false, false, false, false, false, false, false, false,
	false, false, false, false, false, false, false, false, false, false, false, false, false,
	false, false};

int32_t StepAdjDir[9] = {-1, 0, 2, 2, 4, 4, 6, 6, 0};

int32_t adjTile[4][2] = {{-1, 0}, {0, 1}, {1, 0}, {0, -1}};

inline float
agsqrt(float _a, float _b)
{
	return sqrt(_a * _a + _b * _b);
}

//-------------------------------------------------
// Only pattern 2 is used now.
wchar_t mineLayout[4][terrain_const::MAPCELL_DIM * terrain_const::MAPCELL_DIM] = {
	{0, 0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0, 0}, {1, 0, 1, 0, 1, 0, 1, 0, 1},
	{0, 0, 0, 0, 0, 0, 0, 0, 0}};

wchar_t TeamRelations[MAX_TEAMS][MAX_TEAMS] = {{0, 2, RELATION_NEUTRAL, 2, 2, 2, 2, 2},
	{2, 0, 2, 2, 2, 2, 2, 2}, {RELATION_NEUTRAL, 2, 0, 2, 2, 2, 2, 2}, {2, 2, 2, 0, 2, 2, 2, 2},
	{2, 2, 2, 2, 0, 2, 2, 2}, {2, 2, 2, 2, 2, 0, 2, 2}, {2, 2, 2, 2, 2, 2, 0, 2},
	{2, 2, 2, 2, 2, 2, 2, 0}};

bool GoalIsDoor = false;
int32_t numNodesVisited = 0;
int32_t topOpenNodes = 0;
int32_t MaxHPrime = 1000;
bool PreserveMapTiles = false;

MoveMapPtr PathFindMap[2] = {nullptr, nullptr};

float MoveMap::distanceFloat[DISTANCE_TABLE_DIM][DISTANCE_TABLE_DIM];
int32_t MoveMap::distanceLong[DISTANCE_TABLE_DIM][DISTANCE_TABLE_DIM];
int32_t MoveMap::forestCost = 50;

#ifdef LAB_ONLY
int64_t MCTimeCalcPath1Update = 0;
int64_t MCTimeCalcPath2Update = 0;
int64_t MCTimeCalcPath3Update = 0;
int64_t MCTimeCalcPath4Update = 0;
int64_t MCTimeCalcPath5Update = 0;
#endif

//***************************************************************************
// MISC routines
//***************************************************************************

inline void
calcAdjNode(int32_t& r, int32_t& c, int32_t direction)
{
	r += rowShift[direction];
	c += colShift[direction];
}

//---------------------------------------------------------------------------

inline bool
inMapBounds(int32_t r, int32_t c, int32_t mapheight, int32_t mapwidth)
{
	return ((r >= 0) && (r < mapheight) && (c >= 0) && (c < mapwidth));
}

//---------------------------------------------------------------------------

Stuff::Vector3D
relativePositionToPoint(
	Stuff::Vector3D point, float angle, float distance, uint32_t flags)
{
	//--------------------------------------------------------
	// Note that the angle should be -180 <= angle <= 180, and
	// the distance is in meters...
	Stuff::Vector3D curPos;
	curPos.x = point.x;
	curPos.y = point.y;
	curPos.z = 0.0;
	distance *= -worldUnitsPerMeter;
	Stuff::Vector3D shiftVect;
	shiftVect.x = 0.0;
	shiftVect.y = 1.0;
	shiftVect.z = 0.0;
	Rotate(shiftVect, angle);
	shiftVect *= distance;
	Stuff::Vector3D relPos;
	relPos.Zero();
	relPos.Add(curPos, shiftVect);
	Stuff::Vector3D start2d;
	Stuff::Vector3D goal2d;
	Stuff::Vector3D deltaVector;
	if (flags & RELPOS_FLAG_PASSABLE_START)
	{
		start2d = curPos;
		goal2d = relPos;
	}
	else
	{
		start2d = relPos;
		goal2d = curPos;
	}
	deltaVector.Subtract(goal2d, start2d);
	//-------------------------------------------------------------
	// First, we need to calc the delta vector--how much we extend
	// the ray everytime we check the map cell for clear placement.
	deltaVector.Normalize(deltaVector);
	deltaVector *= Terrain::halfWorldUnitsPerCell;
	if (deltaVector.GetLength() == 0.0)
		return (curPos);
	//-------------------------------------------------
	// Determine the max length the ray must be cast...
	float maxLength = distance_from(goal2d, start2d);
	//------------------------------------------------------------
	// We'll start at the target, and if it's blocked, we'll move
	// toward our start location, looking for the first valid/open
	// cell...
	Stuff::Vector3D curPoint = start2d;
	Stuff::Vector3D curRay;
	curRay.Zero();
	float rayLength = 0.0;
	int32_t cellR, cellC;
	Stuff::Vector3D curPoint3d(curPoint.x, curPoint.y, 0.0);
	land->worldToCell(curPoint3d, cellR, cellC);
	uint32_t cellClear = GameMap->getPassable(cellR, cellC);
	Stuff::Vector3D lastGoodPoint = curPoint;
	if (flags & RELPOS_FLAG_PASSABLE_START)
		while (cellClear && (rayLength < maxLength))
		{
			lastGoodPoint = curPoint;
			curPoint += deltaVector;
			curRay.Subtract(curPoint, start2d);
			rayLength = curRay.GetLength();
			curPoint3d.x = curPoint.x;
			curPoint3d.x = curPoint.y;
			curPoint3d.z = 0.0;
			land->worldToCell(curPoint3d, cellR, cellC);
			cellClear = GameMap->getPassable(cellR, cellC);
		}
	else
		while (!cellClear && (rayLength < maxLength))
		{
			lastGoodPoint = curPoint;
			curPoint += deltaVector;
			curRay.Subtract(curPoint, start2d);
			rayLength = curRay.GetLength();
			curPoint3d.x = curPoint.x;
			curPoint3d.x = curPoint.y;
			curPoint3d.z = 0.0;
			land->worldToCell(curPoint3d, cellR, cellC);
			cellClear = GameMap->getPassable(cellR, cellC);
		}
	//----------------------------------------
	// lastGoodPoint MUST be on Map or BOOM!!
	// back on map if necessary
	float maxMap = Terrain::worldUnitsMapSide / 2.0;
	float safety = maxMap - Terrain::worldUnitsPerVertex;
	bool offMap = false;
	if (lastGoodPoint.x < -safety)
	{
		lastGoodPoint.x = -safety;
		offMap = true;
	}
	if (lastGoodPoint.x > safety)
	{
		lastGoodPoint.x = safety;
		offMap = true;
	}
	if (lastGoodPoint.y < -safety)
	{
		lastGoodPoint.y = -safety;
		offMap = true;
	}
	if (lastGoodPoint.y > safety)
	{
		lastGoodPoint.y = safety;
		offMap = true;
	}
	curPoint3d.x = lastGoodPoint.x;
	curPoint3d.x = lastGoodPoint.y;
	curPoint3d.z = 0.0;
	curPoint3d.z = land->getTerrainElevation(curPoint3d);
	return (curPoint3d);
}

//***************************************************************************
// MISSION MAP class (new Game Map class)
//***************************************************************************
//#define DEBUG_GLOBALMAP_BUILD

void MOVE_init(int32_t moveRange)
{
	if (PathFindMap[SECTOR_PATHMAP])
		Fatal(0, " MOVE_Init: Already called this! ");
	PathFindMap[SECTOR_PATHMAP] = new MoveMap;
	if (!PathFindMap[SECTOR_PATHMAP])
		Fatal(0, " MOVE_Init: Cannot initialize PathFindMap ");
	PathFindMap[SECTOR_PATHMAP]->init(SECTOR_DIM * 2, SECTOR_DIM * 2);
	PathFindMap[SIMPLE_PATHMAP] = new MoveMap;
	if (!PathFindMap[SIMPLE_PATHMAP])
		Fatal(0, " MOVE_Init: Cannot initialize PathFindMap ");
	SimpleMovePathRange = moveRange;
	if (SimpleMovePathRange <= 20)
		Fatal(0, " MOVE_Init: MoveRange TOO SMALL. Go see Glenn. ");
	PathFindMap[SIMPLE_PATHMAP]->init(SimpleMovePathRange * 2 + 1, SimpleMovePathRange * 2 + 1);
}

//---------------------------------------------------------------------------

bool EditorSave = false;
int32_t tempNumSpecialAreas = 0;
GameObjectFootPrint* tempSpecialAreaFootPrints = nullptr;

void MOVE_buildData(int32_t height, int32_t width, MissionMapCellInfo* mapData,
	int32_t numSpecialAreas, GameObjectFootPrint* specialAreaFootPrints)
{
	EditorSave = true;
	if (GameMap)
		delete GameMap;
	GameMap = new MissionMap;
	gosASSERT(GameMap != nullptr);
	if (mapData)
		GameMap->init(height, width, 0, mapData);
	else
		GameMap->init(height, width);
	int32_t numOffMap = 0;
	for (size_t r = 0; r < height; r++)
		for (size_t c = 0; c < width; c++)
		{
			Stuff::Vector3D worldPos;
			land->cellToWorld(r, c, worldPos);
			bool set = (land->IsEditorSelectTerrainPosition(worldPos) && !land->IsGameSelectTerrainPosition(worldPos));
			GameMap->setOffMap(r, c, set);
			if (set)
				numOffMap++;
		}
	if (tempSpecialAreaFootPrints)
	{
		systemHeap->Free(tempSpecialAreaFootPrints);
		tempSpecialAreaFootPrints = nullptr;
	}
	tempNumSpecialAreas = numSpecialAreas;
	if (tempNumSpecialAreas)
	{
		tempSpecialAreaFootPrints = (GameObjectFootPrint*)systemHeap->Malloc(
			sizeof(GameObjectFootPrint) * tempNumSpecialAreas);
		memcpy(tempSpecialAreaFootPrints, specialAreaFootPrints,
			sizeof(GameObjectFootPrint) * tempNumSpecialAreas);
	}
	if (!PathFindMap[SECTOR_PATHMAP])
		Fatal(0, " MOVE_BuildData: Must call MOVE_Init()! ");
	if (GlobalMoveMap[0])
	{
		delete GlobalMoveMap[0];
		GlobalMoveMap[0] = nullptr;
	}
	if (GlobalMoveMap[1])
	{
		delete GlobalMoveMap[1];
		GlobalMoveMap[1] = nullptr;
	}
	if (GlobalMoveMap[2])
	{
		delete GlobalMoveMap[2];
		GlobalMoveMap[2] = nullptr;
	}
	GlobalMoveMap[0] = new GlobalMap;
	if (!GlobalMoveMap[0])
		Fatal(0, " MOVE_Init: Cannot initialize GlobalMoveMap ");
	GlobalMoveMap[0]->build(mapData);
	GlobalMoveMap[1] = new GlobalMap;
	if (!GlobalMoveMap[1])
		Fatal(0, " MOVE_Init: Cannot initialize GlobalMoveMap ");
	GlobalMoveMap[1]->hover = true;
	GlobalMoveMap[1]->build(mapData);
	GlobalMoveMap[2] = new GlobalMap;
	if (!GlobalMoveMap[2])
		Fatal(0, " MOVE_Init: Cannot initialize HeliGlobalMoveMap ");
	GlobalMoveMap[2]->blank = true;
	GlobalMoveMap[2]->build(nullptr);
	//-----------------------
	// Just some debugging...
	// for (r = 0; r < height; r++)
	//	for (int32_t c = 0; c < width; c++)
	//		if (GameMap->getOffMap(r, c))
	//			numOffMap++;
	//-------------------------------------------------------------------------------------
	// Since we use this bitfield during the globalmap build for something else,
	// we can now reset it...
	for (size_t row = 0; row < GameMap->height; row++)
		for (size_t col = 0; col < GameMap->width; col++)
			for (size_t i = 0; i < NUM_MOVE_LEVELS; i++)
				GameMap->setPathlock(i, row, col, false);
	EditorSave = false;
}

//---------------------------------------------------------------------------

int32_t
MOVE_saveData(PacketFile* packetFile, int32_t whichPacket)
{
	if (!GameMap)
		Fatal(0, " MOVE_SaveData: Cannot initialize GameMap ");
	if (!GlobalMoveMap[0])
		Fatal(0, " MOVE_SaveData: Cannot initialize GlobalMoveMap ");
	if (!packetFile)
		return (GameMap->write(nullptr) + GlobalMoveMap[0]->write(nullptr) + GlobalMoveMap[1]->write(nullptr) + GlobalMoveMap[2]->write(nullptr) + 2);
	//-----------------------
	// Just some debugging...
	// int32_t numOffMap = 0;
	// for (int32_t r = 0; r < GameMap->height; r++)
	//	for (int32_t c = 0; c < GameMap->width; c++)
	//		if (GameMap->getOffMap(r, c))
	//			numOffMap++;
	int32_t numMissionMapPackets = GameMap->write(packetFile, whichPacket);
	int32_t numGlobalMap0Packets =
		GlobalMoveMap[0]->write(packetFile, whichPacket + numMissionMapPackets);
	int32_t numGlobalMap1Packets = GlobalMoveMap[1]->write(
		packetFile, whichPacket + numMissionMapPackets + numGlobalMap0Packets);
	int32_t numGlobalMap2Packets = GlobalMoveMap[2]->write(packetFile,
		whichPacket + numMissionMapPackets + numGlobalMap0Packets + numGlobalMap1Packets);
	int32_t result = packetFile->writePacket(whichPacket + numMissionMapPackets + numGlobalMap0Packets + numGlobalMap1Packets + numGlobalMap2Packets,
		(uint8_t*)&tempNumSpecialAreas, sizeof(int32_t));
	if (result <= 0)
		Fatal(result, " MOVE_saveData: Unable to write num special area ");
	//-------------------------------------------------------------------------------------------------
	// If there are no special areas, we just write the specialArea count as a
	// filler for the packet...
	if (tempNumSpecialAreas == 0)
		result = packetFile->writePacket(whichPacket + numMissionMapPackets + numGlobalMap0Packets + numGlobalMap1Packets + numGlobalMap2Packets + 1,
			(uint8_t*)&tempNumSpecialAreas, sizeof(int32_t));
	else
		result = packetFile->writePacket(whichPacket + numMissionMapPackets + numGlobalMap0Packets + numGlobalMap1Packets + numGlobalMap2Packets + 1,
			(uint8_t*)tempSpecialAreaFootPrints, sizeof(GameObjectFootPrint) * tempNumSpecialAreas);
	if (result <= 0)
		Fatal(result, " MOVE_saveData: Unable to write special area footprints ");
	return (numMissionMapPackets + numGlobalMap0Packets + numGlobalMap1Packets + numGlobalMap2Packets + 2);
}

//---------------------------------------------------------------------------

int32_t
MOVE_readData(PacketFile* packetFile, int32_t whichPacket)
{
	if (GameMap)
		delete GameMap;
	GameMap = new MissionMap;
	gosASSERT(GameMap != nullptr);
	int32_t numMissionMapPackets = GameMap->init(packetFile, whichPacket);
	//-----------------------
	// Just some debugging...
	// int32_t numOffMap[2] = {0,0};
	// for (int32_t r = 0; r < GameMap->height; r++)
	//	for (int32_t c = 0; c < GameMap->width; c++) {
	//		if (GameMap->getOffMap(r, c))
	//			numOffMap[0]++;
	//		Stuff::Vector3D worldPos;
	//		land->cellToWorld(r, c, worldPos);
	//		bool set = (land->IsEditorSelectTerrainPosition(worldPos) &&
	//! land->IsGameSelectTerrainPosition(worldPos)); 		if (set)
	//			numOffMap[1]++;
	//		if (set && !GameMap->getOffMap(r, c))
	//			set = false;
	//	}
	if (!PathFindMap[SECTOR_PATHMAP])
		Fatal(0, " MOVE_BuildData: Must call MOVE_Init()! ");
	if (GlobalMoveMap[0])
		delete GlobalMoveMap[0];
	GlobalMoveMap[0] = new GlobalMap;
	if (!GlobalMoveMap[0])
		Fatal(0, " MOVE_Init: Cannot initialize GlobalMoveMap0 ");
	int32_t numGlobalMap0Packets =
		GlobalMoveMap[0]->init(packetFile, whichPacket + numMissionMapPackets);
	int32_t numGlobalMap1Packets = 0;
	int32_t numGlobalMap2Packets = 0;
	if (!GlobalMoveMap[0]->badLoad)
	{
		//---------------------------------
		// Let's read the hover move map...
		if (GlobalMoveMap[1])
		{
			delete GlobalMoveMap[1];
			GlobalMoveMap[1] = nullptr;
		}
		GlobalMoveMap[1] = new GlobalMap;
		if (!GlobalMoveMap[1])
			Fatal(0, " MOVE_Init: Cannot initialize GlobalMoveMap1 ");
		GlobalMoveMap[1]->hover = true;
		numGlobalMap1Packets = GlobalMoveMap[1]->init(
			packetFile, whichPacket + numMissionMapPackets + numGlobalMap0Packets);
		//--------------------------------------
		// Let's read the helicoptor move map...
		if (GlobalMoveMap[2])
		{
			delete GlobalMoveMap[2];
			GlobalMoveMap[2] = nullptr;
		}
		GlobalMoveMap[2] = new GlobalMap;
		if (!GlobalMoveMap[2])
			Fatal(0, " MOVE_Init: Cannot initialize GlobalMoveMap2 ");
		GlobalMoveMap[2]->blank = true;
		numGlobalMap2Packets = GlobalMoveMap[2]->init(packetFile,
			whichPacket + numMissionMapPackets + numGlobalMap0Packets + numGlobalMap1Packets);
		//------------------------------------------------------------------
		// Delete the pathExistsTable for this one, since we don't use it...
		systemHeap->Free(GlobalMoveMap[2]->pathExistsTable);
		GlobalMoveMap[2]->pathExistsTable = nullptr;
		int32_t numBytes = packetFile->readPacket(whichPacket + numMissionMapPackets + numGlobalMap0Packets + numGlobalMap1Packets + numGlobalMap2Packets,
			(uint8_t*)&tempNumSpecialAreas);
		if (numBytes <= 0)
			Fatal(numBytes, " MOVE_readData: Unable to read num special areas ");
		if (tempSpecialAreaFootPrints)
		{
			systemHeap->Free(tempSpecialAreaFootPrints);
			tempSpecialAreaFootPrints = nullptr;
		}
		if (tempNumSpecialAreas > 0)
		{
			tempSpecialAreaFootPrints = (GameObjectFootPrint*)systemHeap->Malloc(
				sizeof(GameObjectFootPrint) * tempNumSpecialAreas);
			numBytes = packetFile->readPacket(whichPacket + numMissionMapPackets + numGlobalMap0Packets + numGlobalMap1Packets + numGlobalMap2Packets + 1,
				(uint8_t*)tempSpecialAreaFootPrints);
			if (numBytes <= 0)
				Fatal(numBytes, " MOVE_readData: Unable to read num special areas ");
		}
	}
	//-----------------------
	// Just some debugging...
	// numOffMap[0] = 0;
	// for (r = 0; r < GameMap->height; r++)
	//	for (int32_t c = 0; c < GameMap->width; c++)
	//		if (GameMap->getOffMap(r, c))
	//			numOffMap[0]++;
	return (numMissionMapPackets + numGlobalMap0Packets + numGlobalMap1Packets + numGlobalMap2Packets + 2);
}

//---------------------------------------------------------------------------

void MOVE_cleanup(void)
{
	if (GameMap)
	{
		delete GameMap;
		GameMap = nullptr;
	}
	if (GlobalMoveMap[0])
	{
		delete GlobalMoveMap[0];
		GlobalMoveMap[0] = nullptr;
	}
	if (GlobalMoveMap[1])
	{
		delete GlobalMoveMap[1];
		GlobalMoveMap[1] = nullptr;
	}
	if (GlobalMoveMap[2])
	{
		delete GlobalMoveMap[2];
		GlobalMoveMap[2] = nullptr;
	}
	if (PathFindMap[SECTOR_PATHMAP])
	{
		delete PathFindMap[SECTOR_PATHMAP];
		PathFindMap[SECTOR_PATHMAP] = nullptr;
	}
	if (PathFindMap[SIMPLE_PATHMAP])
	{
		delete PathFindMap[SIMPLE_PATHMAP];
		PathFindMap[SIMPLE_PATHMAP] = nullptr;
	}
#ifdef DEBUG_GLOBALMAP_BUILD
	GameLog::cleanup();
#endif
}

//---------------------------------------------------------------------------

PVOID
MissionMap::operator new(size_t ourSize)
{
	PVOID result = systemHeap->Malloc(ourSize);
	return (result);
}

//---------------------------------------------------------------------------

void MissionMap::operator delete(PVOID us)
{
	systemHeap->Free(us);
}

//---------------------------------------------------------------------------

void MissionMap::init(int32_t h, int32_t w)
{
	height = h;
	width = w;
	VerticesMapSideDivTwo = (Terrain::blocksMapSide * Terrain::verticesBlockSide) / 2.0;
	MetersMapSideDivTwo = Terrain::worldUnitsMapSide / 2;
	MapCellDiagonal = Terrain::worldUnitsPerCell * 1.4142 * metersPerWorldUnit;
	HalfMapCell = Terrain::worldUnitsPerCell / 2.0;
	int32_t i;
	for (i = 0; i < MAX_MAP_CELL_WIDTH; i++)
		tileMulMAPCELL_DIM[i] = i * terrain_const::MAPCELL_DIM;
	int32_t tilewidth = width / terrain_const::MAPCELL_DIM;
	int32_t tileheight = height / terrain_const::MAPCELL_DIM;
	for (i = 0; i < tileheight; i++)
		Terrain::tileRowToWorldCoord[i] =
			(Terrain::worldUnitsMapSide / 2.0) - (i * Terrain::worldUnitsPerVertex);
	for (i = 0; i < tilewidth; i++)
		Terrain::tileColToWorldCoord[i] =
			(i * Terrain::worldUnitsPerVertex) - (Terrain::worldUnitsMapSide / 2.0);
	for (i = 0; i < terrain_const::MAPCELL_DIM; i++)
		Terrain::cellToWorldCoord[i] =
			(Terrain::worldUnitsPerVertex / (float)terrain_const::MAPCELL_DIM) * i;
	for (i = 0; i < height; i++)
		Terrain::cellRowToWorldCoord[i] =
			(Terrain::worldUnitsMapSide / 2.0) - (i * Terrain::worldUnitsPerCell);
	for (i = 0; i < width; i++)
		Terrain::cellColToWorldCoord[i] =
			(i * Terrain::worldUnitsPerCell) - (Terrain::worldUnitsMapSide / 2.0);
	if (map)
	{
		systemHeap->Free(map);
		map = nullptr;
	}
	int32_t mapSize = sizeof(MapCell) * width * height;
	if (mapSize > 0)
		map = (MapCellPtr)systemHeap->Malloc(sizeof(MapCell) * width * height);
	gosASSERT(map != nullptr);
	// memclear(map, sizeof(MapCell) * width * height);
	memset(map, 0, sizeof(MapCell) * width * height);
}

//---------------------------------------------------------------------------

#define NUM_MISSIONMAP_PACKETS 4

int32_t
MissionMap::init(PacketFile* packetFile, int32_t whichPacket)
{
	packetFile->readPacket(whichPacket++, (uint8_t*)&height);
	packetFile->readPacket(whichPacket++, (uint8_t*)&width);
	packetFile->readPacket(whichPacket++, (uint8_t*)&planet);
	init(height, width);
	packetFile->readPacket(whichPacket++, (uint8_t*)map);
	//-----------------------
	// Just some debugging...
	// int32_t numOffMap = 0;
	// for (int32_t r = 0; r < height; r++)
	//	for (int32_t c = 0; c < width; c++) {
	//		Stuff::Vector3D worldPos;
	//		land->cellToWorld(r, c, worldPos);
	//		bool set = (land->IsEditorSelectTerrainPosition(worldPos) &&
	//! land->IsGameSelectTerrainPosition(worldPos));
	//		GameMap->setOffMap(r, c, set);
	//		if (set)
	//			numOffMap++;
	//	}
	for (size_t r = 0; r < height; r++)
		for (size_t c = 0; c < width; c++)
		{
			GameMap->setMover(r, c, false);
		}
	return (NUM_MISSIONMAP_PACKETS);
}

//---------------------------------------------------------------------------

int32_t
MissionMap::write(PacketFile* packetFile, int32_t whichPacket)
{
	//-------------------------------------------
	// Return number of packets if !packetFile...
	if (!packetFile)
		return (NUM_MISSIONMAP_PACKETS);
	int32_t result = packetFile->writePacket(whichPacket++, (uint8_t*)&height, sizeof(int32_t));
	if (result <= 0)
		Fatal(result, " MissionMap.write: Unable to write height packet ");
	result = packetFile->writePacket(whichPacket++, (uint8_t*)&width, sizeof(int32_t));
	if (result <= 0)
		Fatal(result, " MissionMap.write: Unable to write width packet ");
	result = packetFile->writePacket(whichPacket++, (uint8_t*)&planet, sizeof(int32_t));
	if (result <= 0)
		Fatal(result, " MissionMap.write: Unable to write planet packet ");
	result =
		packetFile->writePacket(whichPacket++, (uint8_t*)map, sizeof(MapCell) * height * width);
	if (result <= 0)
		Fatal(result, " MissionMap.write: Unable to write map packet ");
	return (NUM_MISSIONMAP_PACKETS);
}

//---------------------------------------------------------------------------

bool MissionMap::getPassable(Stuff::Vector3D cellPosition)
{
	int32_t row, col;
	land->worldToCell(cellPosition, row, col);
	return (getPassable(row, col));
}

//---------------------------------------------------------------------------

int32_t
MissionMap::init(
	int32_t cellheight, int32_t cellwidth, int32_t curPlanet, MissionMapCellInfo* mapData)
{
	if (!map)
		init(cellheight, cellwidth);
	planet = curPlanet;
	//---------------------------------------------------------------------------
	// Be damned sure map is empty.  This is required or mines will NOT go away.
	memset(map, 0, sizeof(MapCell) * width * height);
	int32_t terrainTypes[1024];
	int32_t overlayTypes[1024];
	for (size_t i = 0; i < 256; i++)
		terrainTypes[i] = overlayTypes[i] = 0;
	if (mapData)
		for (size_t row = 0; row < height; row++)
			for (size_t col = 0; col < width; col++)
			{
				setTerrain(row, col, mapData[row * width + col].terrain);
				terrainTypes[mapData[row * width + col].terrain]++;
				setOverlay(row, col, mapData[row * width + col].overlay);
				overlayTypes[mapData[row * width + col].overlay]++;
				setGate(row, col, mapData[row * width + col].gate);
				setRoad(row, col, mapData[row * width + col].road);
				setPassable(row, col, mapData[row * width + col].passable);
				setLocalheight(row, col, mapData[row * width + col].lineOfSight);
				setPreserved(row, col, false);
				setMine(row, col, mapData[row * width + col].mine);
				setForest(row, col, mapData[row * width + col].forest);
				setBuildWall(row, col, false);
				setBuildGate(row, col, false);
				Stuff::Vector3D worldPos;
				land->cellToWorld(row, col, worldPos);
				int32_t cellWaterType = land->getWater(worldPos);
				if (cellWaterType == WATER_TYPE_SHALLOW)
					setShallowWater(row, col, true);
				if (cellWaterType == WATER_TYPE_DEEP)
					setDeepWater(row, col, true);
				for (size_t i = 0; i < NUM_MOVE_LEVELS; i++)
					GameMap->setPathlock(i, row, col, false);
				switch (mapData[row * width + col].specialType)
				{
				case SPECIAL_WALL:
					setBuildWall(row, col, true);
					break;
				case SPECIAL_GATE:
					setBuildGate(row, col, true);
					break;
				case SPECIAL_LAND_BRIDGE:
					setBuildLandBridge(row, col, true);
					break;
					// case SPECIAL_FOREST:
					//	setBuildForest(row, col, true);
					//	break;
				}
			}
	else
	{
		for (size_t row = 0; row < height; row++)
			for (size_t col = 0; col < width; col++)
			{
				setTerrain(row, col, MC_GRASS_TYPE);
				setOverlay(row, col, 0);
				setGate(row, col, false);
				if ((row == 0) || (col == 0) || (row == (height - 1)) || (col == (width - 1)))
					setPassable(row, col, false);
				else
					setPassable(row, col, true);
				for (size_t i = 0; i < NUM_MOVE_LEVELS; i++)
					GameMap->setPathlock(i, row, col, false);
				setPreserved(row, col, false);
				setMine(row, col, 0);
			}
	}
	return (0);
}

//---------------------------------------------------------------------------

void MissionMap::setPassable(int32_t row, int32_t col, std::wstring_view footPrint, bool passable)
{
	/* FootPrint Data format:
			r1, c1, l1, r2, c2, l2, r3, c3, l3, ...
			where r1 and c1 are offsets from row,col and l1 is the runlength of
			the passable/impassable block of cells. This function keeps reading
	   in the (r, c, l) sets of data until (-1, -1, -1) is read in. Obviously,
	   the data is a block of chars, so all values must be -127 < x < 127. This
	   should allow foot prints big enuff for all buildings we have.
	*/
	std::wstring_view data = footPrint;
	int32_t r = row + *data++;
	int32_t c = col + *data++;
	int32_t len = *data++;
	while (len != -1)
	{
		MapCellPtr cell = &map[r * width + c];
		for (size_t i = 0; i < len; i++)
		{
			cell->setPassable(passable);
			cell++;
		}
		r = row + *data++;
		c = col + *data++;
		len = *data++;
	}
}

//---------------------------------------------------------------------------

void MissionMap::spreadState(int32_t cellRow, int32_t cellCol, int32_t radius)
{
	//---------------------------------------------------------------------------
	// This does a simple depth-first spread from the center. The way it's
	// coded, it essentially grows a box from the center cell.
	if (inMapBounds(cellRow, cellCol, height, width) && (radius > 0))
	{
		//-------------------------------------------------------------------
		// If we're placing moving objects for this frame only, we'll want to
		// preserve the cell states of this tile...
		if (!getPreserved(cellRow, cellCol))
			setPreserved(cellRow, cellCol, true);
		map[cellRow * width + cellCol].setPassable(false);
		for (size_t dir = 0; dir < NUM_DIRECTIONS; dir++)
		{
			int32_t adjR = cellRow;
			int32_t adjC = cellCol;
			calcAdjNode(adjR, adjC, dir);
			spreadState(adjR, adjC, radius - 1);
		}
	}
}

//---------------------------------------------------------------------------

int32_t
MissionMap::placeObject(Stuff::Vector3D position, float radius)
{
	int32_t cellRow, cellCol;
	land->worldToCell(position, cellRow, cellCol);
	//---------------------------------------------------------
	// Now, set the cell states based upon the object's size...
	float cellRadius = (radius / (Terrain::worldUnitsPerCell * metersPerWorldUnit));
	if ((cellRadius > 0.5) && (cellRadius < 1.0))
		cellRadius = 1.0;
	spreadState(cellRow, cellCol, (int32_t)cellRadius);
	return (NO_ERROR);
}

//---------------------------------------------------------------------------

void MissionMap::placeTerrainObject(int32_t objectClass, int32_t objectTypeID, int32_t cellRow,
	int32_t cellCol, int64_t footPrint, bool blocksLineOfFire, int32_t mineType)
{
#if 0
	Assert(objectClass != -1, 0, " MissionMap.placeTerrainObject: bad ObjectClass ");
	int32_t maskTemplateLo = footPrint;
	int32_t maskTemplateHi = footPrint >> 32;
	int32_t posTileR = cellRow / terrain_const::MAPCELL_DIM;
	int32_t posTileC = cellCol / terrain_const::MAPCELL_DIM;
	int32_t tileOriginR = posTileR;
	int32_t tileOriginC = posTileC;
	int32_t startCellR = cellRow % terrain_const::MAPCELL_DIM;
	int32_t startCellC = cellCol % terrain_const::MAPCELL_DIM;
	int32_t cellR = startCellR - 4;
	int32_t cellC = startCellC - 4;
	while(cellR < 0)
	{
		posTileR--;
		cellR += terrain_const::MAPCELL_DIM;
	}
	while(cellC < 0)
	{
		posTileC--;
		cellC += terrain_const::MAPCELL_DIM;
	}
	if(objectClass == GATE)
		blocksLineOfFire = false;
	int32_t startCol = posTileC;
	int32_t firstCellC = cellC;
	//------------------------------------------------------------------
	// We now use the objectClass to do this correctly.
	if((objectClass == BUILDING) ||
			(objectClass == TREEBUILDING) ||
			(objectClass == GATE) ||
			(objectClass == TURRET) ||
			(objectClass == TERRAINOBJECT))
	{
		if(maskTemplateLo || maskTemplateHi)
		{
			//---------------------------------------------------------------------
			// We also need to set the BUILDING flag in the scenario map to indicate
			// a building exists in this tile...
			//map[tileOriginR * width + tileOriginC].setBuildingHere(true);
			switch(objectTypeID)
			{
				case 644:
				case 647:
				case 690:
				case 691:
					map[tileOriginR * width + tileOriginC].setOverlayType(OVERLAY_GATE_CLAN_EW_CLOSED);
					break;
				case 645:
				case 646:
				case 693:
				case 692:
					map[tileOriginR * width + tileOriginC].setOverlayType(OVERLAY_GATE_CLAN_NS_CLOSED);
					break;
			}
			int32_t bits = 0;
			for(size_t totalCelRSet = 0; totalCelRSet < 8; totalCelRSet++)
			{
				for(size_t totalCelCSet = 0; totalCelCSet < 8; totalCelCSet++)
				{
					int32_t bitMask = 1 << bits;
					int32_t maskTemplate = maskTemplateLo;
					if(bits >= 32)
					{
						bitMask = 1 << (bits - 32);
						maskTemplate = maskTemplateHi;
					}
					if((bitMask & maskTemplate) == bitMask)
					{
						map[posTileR * width + posTileC].setCellPassable(cellR, cellC, 0);
						if(blocksLineOfFire)
							map[posTileR * width + posTileC].setCellLOS(cellR, cellC, 0);
					}
					cellC++;
					if(cellC == terrain_const::MAPCELL_DIM)
					{
						cellC = 0;
						posTileC++;
					}
					bits++;
				}
				posTileC = startCol;
				cellC = firstCellC;
				cellR++;
				if(cellR == terrain_const::MAPCELL_DIM)
				{
					cellR = 0;
					posTileR++;
				}
			}
		}
	}
	else if(objectClass == MINE)
	{
		//-----------------------------------------------------------
		// We need to know what cell is in the center of this MINE
		// We then mark the tile as MINED!!!!
		// We base it on the MineTypeNumber stored in the Mine Class.
		if(mineType == 3)		//Force into only one slots instead of three
			mineType = 2;
		if(mineType == 1)		//Force into only one slots instead of three
			mineType = 2;
		if(mineType == 13)		//Force into only one slots instead of three
			mineType = 12;
		if(mineType == 11)		//Force into only one slots instead of three
			mineType = 12;
		if(mineType > 10)  // 10-07-98 HKG hack!  Mines are off by a tile in each direction
			map[(tileOriginR) * width + tileOriginC].setInnerSphereMine(mineType - 10);
		else
			map[(tileOriginR)* width + tileOriginC].setClanMine(mineType);
	}
#endif
}

//---------------------------------------------------------------------------

int32_t
MissionMap::getOverlayWeight(
	int32_t cellR, int32_t cellC, int32_t moverOverlayWeightClass, int32_t moverRelation)
{
	// int32_t overlay = getOverlay(cellR, cellC);
	/*
		if (overlay) {
			int32_t* overlayWeightTable =
	   &OverlayWeightTable[moverOverlayWeightClass * NUM_OVERLAY_TYPES *
	   terrain_const::MAPCELL_DIM * terrain_const::MAPCELL_DIM]; if ((overlayType >=
	   OVERLAY_GATE_CLAN_NS_OPEN) && (overlayType <= OVERLAY_GATE_IS_EW_CLOSED))
	   { overlayType = GateOverlayTable[moverRelation][overlayType -
	   OVERLAY_GATE_CLAN_NS_OPEN]; if (overlayType == -1) return(COST_BLOCKED *
	   2); else { int32_t overlayCostIndex = OverlayWeightIndex[overlayType] +
	   cellR * terrain_const::MAPCELL_DIM + cellC;
					return(overlayWeightTable[overlayCostIndex]);
				}
				}
			else {
				int32_t overlayCostIndex = OverlayWeightIndex[overlayType] +
	   cellR * terrain_const::MAPCELL_DIM + cellC;
				return(overlayWeightTable[overlayCostIndex]);
			}
		}
	*/
	return (0);
}

//---------------------------------------------------------------------------

void MissionMap::print(std::wstring_view fileName)
{
	MechFile* debugFile = new MechFile;
	debugFile->create(fileName);
	for (size_t cellR = 0; cellR < height; cellR++)
	{
		wchar_t outString[1024];
		outString[0] = nullptr;
		for (size_t cellC = 0; cellC < width; cellC++)
		{
			if (!map[cellR * width + cellC].getPassable())
				strcat(outString, "X");
			else
				strcat(outString, ".");
		}
		strcat(outString, "\n");
		debugFile->writeString(outString);
	}
	debugFile->writeString("\n");
	debugFile->close();
	delete debugFile;
	debugFile = nullptr;
}

//---------------------------------------------------------------------------

void MissionMap::destroy(void)
{
	if (map)
	{
		systemHeap->Free(map);
		map = nullptr;
	}
}

//***************************************************************************
// MOVE PATH class
//***************************************************************************

PVOID
MovePath::operator new(size_t ourSize)
{
	PVOID result;
	result = systemHeap->Malloc(ourSize);
	return (result);
}

//---------------------------------------------------------------------------

void MovePath::operator delete(PVOID us)
{
	systemHeap->Free(us);
}

//---------------------------------------------------------------------------

int32_t
MovePath::init(int32_t numberOfSteps)
{
	numSteps = numStepsWhenNotPaused = numberOfSteps;
	static int32_t maxNumberOfSteps = 0;
	if (numberOfSteps > maxNumberOfSteps)
	{
		maxNumberOfSteps = numberOfSteps;
		return (maxNumberOfSteps);
	}
	for (size_t i = 0; i < MAX_STEPS_PER_MOVEPATH; i++)
	{
		stepList[i].distanceToGoal = 0.0;
		stepList[i].destination.x = 0.0;
		stepList[i].destination.y = 0.0;
		stepList[i].destination.z = 0.0;
		stepList[i].direction = 0;
	}
	return (-1);
}

//---------------------------------------------------------------------------

void MovePath::clear(void)
{
	init();
}

//---------------------------------------------------------------------------

void MovePath::destroy(void)
{
	numSteps = 0;
}

//---------------------------------------------------------------------------

float MovePath::getDistanceLeft(Stuff::Vector3D position, int32_t stepNumber)
{
	if (stepNumber == -1)
		stepNumber = curStep;
	float distance = distance_from(position, stepList[stepNumber].destination);
	distance *= metersPerWorldUnit;
	distance += stepList[stepNumber].distanceToGoal;
	return (distance);
}

//---------------------------------------------------------------------------

void MovePath::lock(int32_t level, int32_t start, int32_t range, bool setting)
{
#ifdef _DEBUG
	if (level > 1)
		STOP(("MovePath.lock: bad level %d", level));
#endif
	if (start == -1)
		start = curStep;
	int32_t lastStep = start + range;
	if (lastStep >= numStepsWhenNotPaused)
		lastStep = numStepsWhenNotPaused;
	for (size_t i = start; i < lastStep; i++)
		GameMap->setPathlock(level, stepList[i].cell[0], stepList[i].cell[1], setting);
}

//---------------------------------------------------------------------------

bool MovePath::isLocked(int32_t level, int32_t start, int32_t range, bool* reachedEnd)
{
#ifdef _DEBUG
	if (level > 1)
		STOP(("MovePath.isLocked: bad moveLevel %d", level));
#endif
	if (start == -1)
		start = curStep;
	int32_t lastStep = start + range;
	if (reachedEnd)
		*reachedEnd = false;
	if (lastStep >= numStepsWhenNotPaused)
	{
		if (reachedEnd)
			*reachedEnd = true;
		lastStep = numStepsWhenNotPaused;
	}
	for (size_t i = start; i < lastStep; i++)
		if (GameMap->getPathlock(level, stepList[i].cell[0], stepList[i].cell[1]))
			return (true);
	return (false);
}

//---------------------------------------------------------------------------

bool MovePath::isBlocked(int32_t start, int32_t range, bool* reachedEnd)
{
	if (start == -1)
		start = curStep;
	int32_t lastStep = start + range;
	if (reachedEnd)
		*reachedEnd = false;
	if (lastStep >= numStepsWhenNotPaused)
	{
		if (reachedEnd)
			*reachedEnd = true;
		lastStep = numStepsWhenNotPaused;
	}
	for (size_t i = start; i < lastStep; i++)
		if (!GameMap->getPassable(stepList[i].cell[0], stepList[i].cell[1]))
			return (true);
	return (false);
}

//---------------------------------------------------------------------------

int32_t
MovePath::crossesBridge(int32_t start, int32_t range)
{
	if (start == -1)
		start = curStep;
	int32_t lastStep = start + range;
	if (lastStep >= numStepsWhenNotPaused)
		lastStep = numStepsWhenNotPaused;
		// We need to redo this code when bridges are in cause they are objects
		// now!!
#if 0
	for(size_t i = start; i < lastStep; i++)
	{
		int32_t overlayType = GameMap->getOverlay(stepList[i].cell[0], stepList[i].cell[1]);
		if(OverlayIsBridge[overlayType])
			return(GlobalMoveMap[0]->calcArea(stepList[i].cell[0], stepList[i].cell[1]));
	}
#endif
	return (-1);
}

//---------------------------------------------------------------------------

int32_t
MovePath::crossesCell(int32_t start, int32_t range, int32_t cellR, int32_t cellC)
{
	if (start == -1)
		start = curStep;
	int32_t lastStep = start + range;
	if (lastStep >= numStepsWhenNotPaused)
		lastStep = numStepsWhenNotPaused;
	for (size_t i = start; i < lastStep; i++)
	{
		if ((cellR == stepList[i].cell[0]) && (cellC == stepList[i].cell[1]))
			return (i);
	}
	return (-1);
}

//---------------------------------------------------------------------------

int32_t
MovePath::crossesClosedClanGate(int32_t start, int32_t range)
{
	return (-1);
}

//---------------------------------------------------------------------------

int32_t
MovePath::crossesClosedISGate(int32_t start, int32_t range)
{
	return (-1);
}

//---------------------------------------------------------------------------

int32_t
MovePath::crossesClosedGate(int32_t start, int32_t range)
{
	if (start == -1)
		start = curStep;
	int32_t lastStep = start + range;
	if (lastStep >= numStepsWhenNotPaused)
		lastStep = numStepsWhenNotPaused;
		// Probably should have redone this code cause gates aren't overlays
		// anymore?
#if 0
	for(size_t i = start; i < lastStep; i++)
	{
		int32_t overlayType = GameMap->getOverlay(stepList[i].cell[0], stepList[i].cell[1]);
		if(OverlayIsClosedGate[overlayType])
			return(i);
	}
#endif
	return (-1);
}

//**********************************************************************************
// GLOBAL MAP class
//**********************************************************************************

PVOID
GlobalMap::operator new(size_t ourSize)
{
	PVOID result = systemHeap->Malloc(ourSize);
	return (result);
}

//---------------------------------------------------------------------------

void GlobalMap::operator delete(PVOID us)
{
	systemHeap->Free(us);
}

//------------------------------------------------------------------------------------------

void GlobalMap::init(int32_t w, int32_t h)
{
	width = w;
	height = h;
	areaMap = (int16_t*)systemHeap->Malloc(sizeof(int16_t) * w * h);
	gosASSERT(areaMap != nullptr);
	for (size_t r = 0; r < height; r++)
		for (size_t c = 0; c < width; c++)
			areaMap[r * width + c] = -1;
	gosASSERT(((width % SECTOR_DIM) == 0) && ((height % SECTOR_DIM) == 0));
	sectorwidth = w / SECTOR_DIM;
	sectorheight = w / SECTOR_DIM;
	numAreas = 0;
	areas = nullptr;
	numDoors = 0;
	doors = nullptr;
	doorBuildList = nullptr;
	pathExistsTable = nullptr;
#ifdef USE_PATH_COST_TABLE
	pathCostTable = nullptr;
#endif
	blank = false;
}

//------------------------------------------------------------------------------------------

#define GLOBALMAP_VERSION_NUMBER 0x022523 // My 51st birthday:)

int32_t
GlobalMap::init(PacketFilePtr packetFile, int32_t whichPacket)
{
	int32_t startPacket = whichPacket;
	uint32_t version = 0;
	int32_t result = packetFile->readPacket(whichPacket++, (uint8_t*)&version);
	if (result == 0)
		Fatal(result, " GlobalMap.init: unable to read version packet ");
	bool badVersion = false;
	if (version != GLOBALMAP_VERSION_NUMBER)
		badVersion = true;
	result = packetFile->readPacket(whichPacket++, (uint8_t*)&height);
	if (result == 0)
		Fatal(result, " GlobalMap.init: unable to read height packet ");
	result = packetFile->readPacket(whichPacket++, (uint8_t*)&width);
	if (result == 0)
		Fatal(result, " GlobalMap.init: unable to read width packet ");
	int32_t sectorDim; // this is no longer used...
	result = packetFile->readPacket(whichPacket++, (uint8_t*)&sectorDim);
	if (result == 0)
		Fatal(result, " GlobalMap.init: unable to read sectorDim packet ");
	result = packetFile->readPacket(whichPacket++, (uint8_t*)&sectorheight);
	if (result == 0)
		Fatal(result, " GlobalMap.init: unable to read sectorheight packet ");
	result = packetFile->readPacket(whichPacket++, (uint8_t*)&sectorwidth);
	if (result == 0)
		Fatal(result, " GlobalMap.init: unable to read sectorwidth packet ");
	result = packetFile->readPacket(whichPacket++, (uint8_t*)&numAreas);
	if (result == 0)
		Fatal(result, " GlobalMap.init: unable to read numAreas packet ");
	result = packetFile->readPacket(whichPacket++, (uint8_t*)&numDoors);
	if (result == 0)
		Fatal(result, " GlobalMap.init: unable to read numDoors packet ");
	result = packetFile->readPacket(whichPacket++, (uint8_t*)&numDoorInfos);
	if (result == 0)
		Fatal(result, " GlobalMap.init: unable to read numDoorInfos packet ");
	result = packetFile->readPacket(whichPacket++, (uint8_t*)&numDoorLinks);
	if (result == 0)
		Fatal(result, " GlobalMap.init: unable to read numDoorLinks packet ");
	if (badVersion)
	{
		//---------------------------------------------------------
		// Bad version map, so return number of packets but bail...
		badLoad = true;
#ifdef USE_PATH_COST_TABLE
		pathCostTable = nullptr;
		return (14 + numDoorInfos + (numDoors + NUM_DOOR_OFFSETS) * 2);
#else
		return (13 + numDoorInfos + (numDoors + NUM_DOOR_OFFSETS) * 2);
#endif
	}
	areaMap = (int16_t*)systemHeap->Malloc(sizeof(int16_t) * height * width);
	gosASSERT(areaMap != nullptr);
	result = packetFile->readPacket(whichPacket++, (uint8_t*)areaMap);
	if (result == 0)
		Fatal(result, " GlobalMap.init: unable to read areaMap packet ");
	areas = (GlobalMapAreaPtr)systemHeap->Malloc(sizeof(GlobalMapArea) * numAreas);
	gosASSERT(areas != nullptr);
	result = packetFile->readPacket(whichPacket++, (uint8_t*)areas);
	if (result == 0)
		Fatal(result, " GlobalMap.init: unable to read areas packet ");
	doorInfos = (DoorInfoPtr)systemHeap->Malloc(sizeof(DoorInfo) * numDoorInfos);
	gosASSERT(doorInfos != nullptr);
	int32_t curDoorInfo = 0;
	int32_t i;
	for (i = 0; i < numAreas; i++)
		if (areas[i].numDoors)
		{
			result = packetFile->readPacket(whichPacket++, (uint8_t*)&doorInfos[curDoorInfo]);
			if (result == 0)
				Fatal(result, " GlobalMap.init: unable to read doorInfos packet ");
			curDoorInfo += areas[i].numDoors;
		}
	Assert(numDoorInfos == curDoorInfo, 0, " GlobalMap.init: bad doorInfo count ");
	//------------------------------------------------------------
	// Set up the areas so they point to the correct door infos...
	curDoorInfo = 0;
	for (size_t curArea = 0; curArea < numAreas; curArea++)
	{
		areas[curArea].doors = &doorInfos[curDoorInfo];
		curDoorInfo += areas[curArea].numDoors;
	}
	doors =
		(GlobalMapDoorPtr)systemHeap->Malloc(sizeof(GlobalMapDoor) * (numDoors + NUM_DOOR_OFFSETS));
	gosASSERT(doors != nullptr);
	result = packetFile->readPacket(whichPacket++, (uint8_t*)doors);
	if (result == 0)
		Fatal(result, " GlobalMap.init: unable to read doors packet ");
	//--------------
	// Door Links...
	doorLinks = (DoorLinkPtr)systemHeap->Malloc(sizeof(DoorLink) * numDoorLinks);
	gosASSERT(doorLinks != nullptr);
	int32_t numLinksRead = 0;
	for (i = 0; i < (numDoors + NUM_DOOR_OFFSETS); i++)
	{
		int32_t numLinks = doors[i].numLinks[0] + NUM_EXTRA_DOOR_LINKS;
		gosASSERT(numLinks >= 2);
		result = packetFile->readPacket(whichPacket++, (uint8_t*)&doorLinks[numLinksRead]);
		if (result <= 0)
			Fatal(result, " GlobalMap.init: Unable to write doorLinks packet ");
		doors[i].links[0] = &doorLinks[numLinksRead];
		numLinksRead += numLinks;
		numLinks = doors[i].numLinks[1] + NUM_EXTRA_DOOR_LINKS;
		gosASSERT(numLinks >= 2);
		result = packetFile->readPacket(whichPacket++, (uint8_t*)&doorLinks[numLinksRead]);
		if (result <= 0)
			Fatal(result, " GlobalMap.init: Unable to write doorLinks packet ");
		doors[i].links[1] = &doorLinks[numLinksRead];
		numLinksRead += numLinks;
	}
	Assert(numLinksRead == numDoorLinks, 0, " GlobalMap.init: Incorrect Links count ");
	numOffMapAreas = 0;
	for (i = 0; i < numAreas; i++)
		if (areas[i].offMap)
		{
			offMapAreas[numOffMapAreas++] = i;
			closeArea(i);
		}
		else
			openArea(i);
	//--------------------------
	// Create pathExistsTable...
	pathExistsTable = (uint8_t*)systemHeap->Malloc(numAreas * (numAreas / 4 + 1));
	if (!pathExistsTable)
		STOP(("GlobalMap.init: unable to malloc pathExistsTable"));
	clearPathExistsTable();
	if (logEnabled && !blank)
	{
		wchar_t s[256];
		for (i = 0; i < numDoors; i++)
		{
			sprintf(s, "door %05d, %s(%d), areas %d & %d", i, doors[i].open ? "opened" : "CLOSED",
				doors[i].teamID, doors[i].area[0], doors[i].area[1]);
			log->write(s);
			for (size_t side = 0; side < 2; side++)
			{
				sprintf(s, "     side %d", side);
				log->write(s);
				for (size_t j = 0; j < doors[i].numLinks[side]; j++)
				{
					sprintf(s, "          link %03d, to door %05d, cost %d", j,
						doors[i].links[side][j].doorIndex, doors[i].links[side][j].cost);
					log->write(s);
				}
			}
		}
		log->write(" ");
		for (i = 0; i < numAreas; i++)
		{
			sprintf(
				s, "area %05d, %s(%d)", i, areas[i].open ? "opened" : "CLOSED", areas[i].teamID);
			log->write(s);
			if (areas[i].ownerWID > 0)
			{
				sprintf(s, "     ownerWID is %d", areas[i].ownerWID);
				log->write(s);
			}
			static std::wstring_view typeString[] = {
				"normal area", "wall area", "gate area", "land bridge area", "forest area"};
			sprintf(s, "     %s", typeString[areas[i].type]);
			log->write(s);
			for (size_t d = 0; d < areas[i].numDoors; d++)
			{
				sprintf(s, "     door %03d is %d (%d:%d & %d) ", d, areas[i].doors[d].doorIndex,
					areas[i].doors[d].doorSide, doors[areas[i].doors[d].doorIndex].area[0],
					doors[areas[i].doors[d].doorIndex].area[1]);
				log->write(s);
			}
		}
		log->write(" ");
	}
	return (whichPacket - startPacket);
}

//------------------------------------------------------------------------------------------

int32_t
GlobalMap::write(PacketFilePtr packetFile, int32_t whichPacket)
{
	//---------------------------------------------------------
	// If mapFile nullptr, return the number of packets we need...
	int32_t numDrInfos = 0;
	int32_t i;
	for (i = 0; i < numAreas; i++)
		if (areas[i].numDoors)
			numDrInfos++;
#ifdef USE_PATH_COST_TABLE
	int32_t numPackets = 14 + numDrInfos + (numDoors + NUM_DOOR_OFFSETS) * 2;
#else
	int32_t numPackets = 13 + numDrInfos + (numDoors + NUM_DOOR_OFFSETS) * 2;
#endif
	if (!packetFile)
		return (numPackets);
	uint32_t version = GLOBALMAP_VERSION_NUMBER;
	int32_t result = packetFile->writePacket(whichPacket++, (uint8_t*)&version, sizeof(int32_t));
	if (result <= 0)
		Fatal(result, " GlobalMap.write: Unable to write version packet ");
	result = packetFile->writePacket(whichPacket++, (uint8_t*)&height, sizeof(int32_t));
	if (result <= 0)
		Fatal(result, " GlobalMap.write: Unable to write height packet ");
	result = packetFile->writePacket(whichPacket++, (uint8_t*)&width, sizeof(int32_t));
	if (result <= 0)
		Fatal(result, " GlobalMap.write: Unable to write width packet ");
	int32_t sectorDim = SECTOR_DIM;
	result = packetFile->writePacket(whichPacket++, (uint8_t*)&sectorDim, sizeof(int32_t));
	if (result <= 0)
		Fatal(result, " GlobalMap.write: Unable to write sectorDim packet ");
	result = packetFile->writePacket(whichPacket++, (uint8_t*)&sectorheight, sizeof(int32_t));
	if (result <= 0)
		Fatal(result, " GlobalMap.write: Unable to write sectorheight packet ");
	result = packetFile->writePacket(whichPacket++, (uint8_t*)&sectorwidth, sizeof(int32_t));
	if (result <= 0)
		Fatal(result, " GlobalMap.write: Unable to write sectorwidth packet ");
	result = packetFile->writePacket(whichPacket++, (uint8_t*)&numAreas, sizeof(int32_t));
	if (result <= 0)
		Fatal(result, " GlobalMap.write: Unable to write numAreas packet ");
	result = packetFile->writePacket(whichPacket++, (uint8_t*)&numDoors, sizeof(int32_t));
	if (result <= 0)
		Fatal(result, " GlobalMap.write: Unable to write numDoors packet ");
	result = packetFile->writePacket(whichPacket++, (uint8_t*)&numDoorInfos, sizeof(int32_t));
	if (result <= 0)
		Fatal(result, " GlobalMap.write: Unable to write numDoorInfos packet ");
	result = packetFile->writePacket(whichPacket++, (uint8_t*)&numDoorLinks, sizeof(int32_t));
	if (result <= 0)
		Fatal(result, " GlobalMap.write: Unable to write numDoorLinks packet ");
	result =
		packetFile->writePacket(whichPacket++, (uint8_t*)areaMap, sizeof(int16_t) * height * width);
	if (result <= 0)
		Fatal(result, " GlobalMap.write: Unable to write areaMap packet ");
	//----------------------------------
	// Write out the areas themselves...
	DoorInfoPtr doorInfoSave[MAX_GLOBALMAP_AREAS];
	for (i = 0; i < numAreas; i++)
	{
		//-------------------------
		// clear it for the save...
		doorInfoSave[i] = areas[i].doors;
		areas[i].doors = nullptr;
		areas[i].ownerWID = 0;
		areas[i].teamID = -1;
		// areas[i].offMap = false;
		areas[i].open = true;
	}
	result =
		packetFile->writePacket(whichPacket++, (uint8_t*)areas, sizeof(GlobalMapArea) * numAreas);
	for (i = 0; i < numAreas; i++)
	{
		//---------------------
		// restore the doors...
		areas[i].doors = doorInfoSave[i];
	}
	if (result <= 0)
		Fatal(result, " GlobalMap.write: Unable to write areas packet ");
	//------------------------------------
	// Now, write out all the DoorInfos...
	int32_t maxDoors = 0;
	for (size_t d = 0; d < numDoors; d++)
	{
		for (size_t s = 0; s < 2; s++)
		{
			int32_t areaNumDoors = areas[doors[d].area[s]].numDoors;
			if (areaNumDoors > maxDoors)
				maxDoors = areaNumDoors;
		}
	}
	doors[numDoors + DOOR_OFFSET_START].numLinks[0] = maxDoors;
	doors[numDoors + DOOR_OFFSET_START].numLinks[1] = 0;
	doors[numDoors + DOOR_OFFSET_GOAL].numLinks[0] = maxDoors;
	doors[numDoors + DOOR_OFFSET_GOAL].numLinks[1] = 0;
	int32_t numDoorInfosWritten = 0;
	for (i = 0; i < numAreas; i++)
		if (areas[i].numDoors)
		{
			int32_t packetSize = sizeof(DoorInfo) * areas[i].numDoors;
			result = packetFile->writePacket(whichPacket++, (uint8_t*)areas[i].doors, packetSize);
			if (result <= 0)
				Fatal(result, " GlobalMap.write: Unable to write doorInfos packet ");
			numDoorInfosWritten += areas[i].numDoors;
		}
	Assert(numDoorInfosWritten == numDoorInfos, 0, " GlobalMap.write: Error in writing DoorInfos ");
	DoorLinkPtr doorLinkSave[MAX_GLOBALMAP_DOORS][2];
	for (i = 0; i < numDoors + NUM_DOOR_OFFSETS; i++)
	{
		//-------------------------
		// clear it for the save...
		doorLinkSave[i][0] = doors[i].links[0];
		doorLinkSave[i][1] = doors[i].links[1];
		// doors[i].cost = 0;
		doors[i].links[0] = nullptr;
		doors[i].links[1] = nullptr;
		doors[i].parent = 0;
		doors[i].fromAreaIndex = 0;
		doors[i].flags = 0;
		doors[i].g = 0;
		doors[i].hPrime = 0;
		doors[i].fPrime = 0;
	}
	result = packetFile->writePacket(
		whichPacket++, (uint8_t*)doors, sizeof(GlobalMapDoor) * (numDoors + NUM_DOOR_OFFSETS));
	for (i = 0; i < (numDoors + NUM_DOOR_OFFSETS); i++)
	{
		//---------------------
		// restore the links...
		doors[i].links[0] = doorLinkSave[i][0];
		doors[i].links[1] = doorLinkSave[i][1];
	}
	if (result <= 0)
		Fatal(result, " GlobalMap.write: Unable to write doors packet ");
	//------------------------------------
	// Now, write out all the DoorLinks...
	int32_t numberL = 0;
	for (i = 0; i < (numDoors + NUM_DOOR_OFFSETS); i++)
	{
		int32_t numLinks = doors[i].numLinks[0] + NUM_EXTRA_DOOR_LINKS;
		gosASSERT(numLinks >= 2);
		result = packetFile->writePacket(
			whichPacket++, (uint8_t*)doors[i].links[0], sizeof(DoorLink) * numLinks);
		if (result <= 0)
			Fatal(result, " GlobalMap.write: Unable to write doorLinks packet ");
		numberL += numLinks;
		numLinks = doors[i].numLinks[1] + NUM_EXTRA_DOOR_LINKS;
		gosASSERT(numLinks >= 2);
		result = packetFile->writePacket(
			whichPacket++, (uint8_t*)doors[i].links[1], sizeof(DoorLink) * numLinks);
		if (result <= 0)
			Fatal(result, " GlobalMap.write: Unable to write doorLinks packet ");
		numberL += numLinks;
	}
	if (numberL != numDoorLinks)
		PAUSE(("Number of DoorLinks Calculated does not match numDoorLinks"));
#ifdef USE_PATH_COST_TABLE
	if (blank)
		calcPathCostTable();
	else
		initPathCostTable();
	result = packetFile->writePacket(whichPacket++, (uint8_t*)pathCostTable, numAreas * numAreas);
	if (result <= 0)
		Fatal(result, " GlobalMap.write: Unable to write doors packet ");
#endif
	return (numPackets);
}

//------------------------------------------------------------------------------------------

void GlobalMap::calcSpecialAreas(MissionMapCellInfo* mapData)
{
	bool areaLogged[64000];
	memset(areaLogged, false, 64000);
	int32_t biggestID = -1;
	int32_t smallestID = 99999;
	for (size_t r = 0; r < height; r++)
		for (size_t c = 0; c < width; c++)
		{
			if (mapData[r * width + c].specialType != SPECIAL_NONE)
			{
				int32_t ID = mapData[r * width + c].specialID;
				if (ID > biggestID)
					biggestID = ID;
				if (ID < smallestID)
					smallestID = ID;
				switch (mapData[r * width + c].specialType)
				{
				case SPECIAL_GATE:
					if (!areaLogged[ID])
					{
						specialAreas[ID].type = SPECIAL_GATE;
						specialAreas[ID].numSubAreas = 0;
						specialAreas[ID].numCells = 0;
						areaLogged[ID] = true;
					}
					areaMap[r * width + c] = ID;
					GameMap->setBuildGate(r, c, true);
					specialAreas[ID].cells[specialAreas[ID].numCells][0] = r;
					specialAreas[ID].cells[specialAreas[ID].numCells][1] = c;
					specialAreas[ID].numCells++;
					break;
				case SPECIAL_WALL:
					if (!areaLogged[ID])
					{
						specialAreas[ID].type = SPECIAL_WALL;
						specialAreas[ID].numSubAreas = 0;
						specialAreas[ID].numCells = 0;
						areaLogged[ID] = true;
					}
					areaMap[r * width + c] = ID;
					GameMap->setBuildWall(r, c, true);
					specialAreas[ID].cells[specialAreas[ID].numCells][0] = r;
					specialAreas[ID].cells[specialAreas[ID].numCells][1] = c;
					specialAreas[ID].numCells++;
					break;
				case SPECIAL_LAND_BRIDGE:
					if (!areaLogged[ID])
					{
						specialAreas[ID].type = SPECIAL_LAND_BRIDGE;
						specialAreas[ID].numSubAreas = 0;
						specialAreas[ID].numCells = 0;
						areaLogged[ID] = true;
					}
					areaMap[r * width + c] = ID;
					GameMap->setBuildLandBridge(r, c, true);
					specialAreas[ID].cells[specialAreas[ID].numCells][0] = r;
					specialAreas[ID].cells[specialAreas[ID].numCells][1] = c;
					specialAreas[ID].numCells++;
					break;
					// case SPECIAL_FOREST:
					//	if (!areaLogged[ID]) {
					//		specialAreas[ID].type = SPECIAL_FOREST;
					//		specialAreas[ID].numSubAreas = 0;
					//		specialAreas[ID].numCells = 0;
					//		areaLogged[ID] = true;
					//	}
					//	areaMap[r * width + c] = ID;
					//	GameMap->setBuildForest(r, c, true);
					//	break;
				}
			}
		}
	if (biggestID > -1)
		numSpecialAreas = (biggestID + 1);
	for (size_t i = 0; i < numSpecialAreas; i++)
	{
		specialAreas[i].numSubAreas = 0;
		// if (!areaLogged[i])
		//	STOP(("GlobalMap.calcSpecialAreas: unlogged special area %d", i));
	}
}

//------------------------------------------------------------------------------------------

int32_t
GlobalMap::setTempArea(int32_t tileR, int32_t tileC, int32_t cost)
{
	int32_t numStartDoors = 0;
	for (size_t dir = 0; dir < 4; dir++)
	{
		int32_t adjR = tileR + adjTile[dir][0];
		int32_t adjC = tileC + adjTile[dir][1];
		int32_t adjArea = calcArea(adjR, adjC);
		if (adjArea > -1)
		{
			// SET TEMP DOORS HERE...
			//			areas[numAreas].doors[numStartDoors].r = tileR;
			//			areas[numAreas].doors[numStartDoors].c = tileC;
			//			areas[numAreas].doors[numStartDoors].length = 1;
			//			areas[numAreas].doors[numStartDoors].direction = dir;
			//			areas[numAreas].doors[numStartDoors].area = adjArea;
			//			numStartDoors++;
		}
	}
	areas[numAreas].numDoors = numStartDoors;
	//----------------------------
	// Now, setup the temp area...
	areas[numAreas].sectorR = tileR / SECTOR_DIM;
	areas[numAreas].sectorC = tileC / SECTOR_DIM;
	// areas[numAreas].cost = 1;
	return (numAreas);
}

//------------------------------------------------------------------------------------------

bool GlobalMap::fillSpecialArea(int32_t row, int32_t col, int32_t area, int32_t specialID)
{
	//----------------------------------------------------------------------
	// This is used to fill any dynamic or "special" areas. Currently, these
	// are: wall, gate and forest.
	areaMap[row * width + col] = area;
	GameMap->setBuildNotSet(row, col, false);
	for (size_t dir = 0; dir < 4; dir++)
	{
		int32_t adjR = row + adjTile[dir][0];
		int32_t adjC = col + adjTile[dir][1];
		if ((adjR >= minRow) && (adjR < maxRow) && (adjC >= minCol) && (adjC < maxCol))
			if (GameMap->getBuildNotSet(adjR, adjC) && (areaMap[adjR * width + adjC] == specialID))
				fillSpecialArea(adjR, adjC, area, specialID);
	}
	return (true);
}

//------------------------------------------------------------------------------------------

bool GlobalMap::fillArea(int32_t row, int32_t col, int32_t area, bool offMap)
{
	if ((row < minRow) || (row >= maxRow) || (col < minCol) || (col >= maxCol))
		return (false);
	//---------------------------------------------------------------------
	// If we hit a special (wall/gate/forest) tile, politely stop expanding
	// this area into it...
	if (!blank)
		if (GameMap->getBuildSpecial(row, col))
			return (false);
	//-----------------------------------------------------------------------------
	// If we're processing an offMap area, then stop if we hit a non-offMap
	// cell...
	if (offMap)
	{
		if (!GameMap->getOffMap(row, col))
			return (false);
	}
	else
	{
		if (GameMap->getOffMap(row, col))
			return (false);
	}
	GameMap->setBuildNotSet(row, col, false);
	if (!blank)
	{
		if (!hover && GameMap->getDeepWater(row, col))
		{
			// GameMap->setPassable(row, col, false);
			areaMap[row * width + col] = -2;
			return (false);
		}
		if (!offMap && !GameMap->getPassable(row, col))
		{
			areaMap[row * width + col] = -2;
			return (false);
		}
	}
	gosASSERT(area != -1);
	areaMap[row * width + col] = area;
	for (size_t dir = 0; dir < 4; dir++)
	{
		int32_t adjR = row + adjTile[dir][0];
		int32_t adjC = col + adjTile[dir][1];
		if ((adjR >= minRow) && (adjR < maxRow) && (adjC >= minCol) && (adjC < maxCol))
			if (GameMap->getBuildNotSet(adjR, adjC))
				fillArea(adjR, adjC, area, offMap);
	}
	return (true);
}

//------------------------------------------------------------------------------------------

void GlobalMap::calcSectorAreas(int32_t sectorR, int32_t sectorC)
{
	minRow = sectorR * SECTOR_DIM;
	maxRow = minRow + SECTOR_DIM;
	minCol = sectorC * SECTOR_DIM;
	maxCol = minCol + SECTOR_DIM;
	if (blank)
	{
		for (size_t r = minRow; r < maxRow; r++)
			for (size_t c = minCol; c < maxCol; c++)
				if (GameMap->getBuildNotSet(r, c))
				{
					if (fillArea(r, c, numAreas, GameMap->getOffMap(r, c)))
						numAreas++;
				}
	}
	else
	{
		for (size_t r = minRow; r < maxRow; r++)
			for (size_t c = minCol; c < maxCol; c++)
				if (GameMap->getBuildNotSet(r, c))
				{
					if (GameMap->getBuildSpecial(r, c))
					{
						int32_t specialHere = areaMap[r * width + c];
						gosASSERT(specialHere > -1);
						//-------------------------------------------------
						// List this new area as part of the region covered
						// by this gate or wall or forest...
						if (specialAreas[specialHere].numSubAreas == MAX_SPECIAL_SUB_AREAS)
							Fatal(MAX_SPECIAL_SUB_AREAS,
								" GlobalMap.calcSectorAreas: too many special "
								"subareas ");
						specialAreas[specialHere].subAreas[specialAreas[specialHere].numSubAreas] =
							numAreas;
						specialAreas[specialHere].numSubAreas++;
						//-------------------
						// Now, fill it in...
						fillSpecialArea(r, c, numAreas++, specialHere);
					}
					else if (fillArea(r, c, numAreas, GameMap->getOffMap(r, c)))
						numAreas++;
				}
	}
}

//------------------------------------------------------------------------------------------

void GlobalMap::calcAreas(void)
{
	//----------------------------------------------------------------------
	// Large area maps use -1 and -2 to indicate blocked area. CalcArea will
	// always return -1 for blocked-area cells.
	//-----------------------------------------
	// First, process each sector's area map...
	for (size_t r = 0; r < sectorheight; r++)
		for (size_t c = 0; c < sectorwidth; c++)
			calcSectorAreas(r, c);
	//-----------------------------------------------------------------------
	// NOTE: We allocate one extra area--this is used by the calcPath routine
	// as a "scratch" area in some cases...
	gosASSERT(numAreas <= MAX_GLOBALMAP_AREAS);
	areas = (GlobalMapAreaPtr)systemHeap->Malloc(sizeof(GlobalMapArea) * (numAreas + 1));
	gosASSERT(areas != nullptr);
	for (size_t i = 0; i < (numAreas + 1); i++)
	{
		areas[i].sectorR = 0;
		areas[i].sectorC = 0;
		areas[i].type = AREA_TYPE_NORMAL;
		areas[i].numDoors = 0;
		areas[i].doors = nullptr;
		areas[i].ownerWID = 0;
		areas[i].teamID = -1;
		areas[i].offMap = false;
		areas[i].open = true;
		areas[i].cellsCovered = nullptr;
	}
	//-----------------------------------------
	// Set each area's sector row and column...
	for (size_t sectorR = 0; sectorR < sectorheight; sectorR++)
		for (size_t sectorC = 0; sectorC < sectorwidth; sectorC++)
		{
			minRow = sectorR * SECTOR_DIM;
			maxRow = minRow + SECTOR_DIM;
			minCol = sectorC * SECTOR_DIM;
			maxCol = minCol + SECTOR_DIM;
			for (size_t r = minRow; r < maxRow; r++)
				for (size_t c = minCol; c < maxCol; c++)
				{
					int32_t curArea = areaMap[r * width + c];
					if (curArea > -1)
					{
						areas[curArea].sectorR = sectorR;
						areas[curArea].sectorC = sectorC;
					}
				}
		}
}

//------------------------------------------------------------------------------------------

void GlobalMap::calcCellsCovered(void)
{
	/*	for (int32_t r = 0; r < height; r++)
			for (int32_t c = 0; c < width; c++) {
				int32_t area = areaMap[r * width + c];
				if (areas[area].type
			}
	*/
}

//------------------------------------------------------------------------------------------

void GlobalMap::calcSpecialTypes(void)
{
	// systemHeap->walkHeap();
	for (size_t i = 0; i < numSpecialAreas; i++)
	{
		if (specialAreas[i].type == SPECIAL_WALL)
			for (size_t j = 0; j < specialAreas[i].numSubAreas; j++)
				areas[specialAreas[i].subAreas[j]].type = AREA_TYPE_WALL;
		else if (specialAreas[i].type == SPECIAL_GATE)
			for (size_t j = 0; j < specialAreas[i].numSubAreas; j++)
				areas[specialAreas[i].subAreas[j]].type = AREA_TYPE_GATE;
		else if (specialAreas[i].type == SPECIAL_LAND_BRIDGE)
			for (size_t j = 0; j < specialAreas[i].numSubAreas; j++)
				areas[specialAreas[i].subAreas[j]].type = AREA_TYPE_LAND_BRIDGE;
		else if (specialAreas[i].type == SPECIAL_FOREST)
			for (size_t j = 0; j < specialAreas[i].numSubAreas; j++)
				areas[specialAreas[i].subAreas[j]].type = AREA_TYPE_FOREST;
	}
	for (size_t r = 0; r < height; r++)
		for (size_t c = 0; c < width; c++)
			if (GameMap->getOffMap(r, c))
			{
				int32_t area = calcArea(r, c);
				if (area > -1)
					areas[area].offMap = true;
			}
	// systemHeap->walkHeap();
}

//------------------------------------------------------------------------------------------

void GlobalMap::beginDoorProcessing(void)
{
	// systemHeap->walkHeap();
	doorBuildList =
		(GlobalMapDoorPtr)systemHeap->Malloc(sizeof(GlobalMapDoor) * MAX_GLOBALMAP_DOORS);
	for (size_t i = 0; i < MAX_GLOBALMAP_DOORS; i++)
	{
		doorBuildList[i].row = -1;
		doorBuildList[i].col = -1;
		doorBuildList[i].length = -1;
		doorBuildList[i].open = true;
		doorBuildList[i].teamID = -1;
		doorBuildList[i].numLinks[0] = 0;
		doorBuildList[i].numLinks[1] = 0;
	}
	gosASSERT(doorBuildList != nullptr);
}

//------------------------------------------------------------------------------------------

void GlobalMap::addDoor(
	int32_t area1, int32_t area2, int32_t row, int32_t col, int32_t length, int32_t dir)
{
	/*
	if (area1 > area2) {
		int32_t savedArea = area1;
		area1 = area2;
		area2 = savedArea;
		dir = (dir + 2) % 4;
	}
	*/
	//-------------------------------------------------------
	// First, make sure the door isn't already in the list...
	bool newDoor = true;
	for (size_t curDoorIndex = 0; curDoorIndex < numDoors; curDoorIndex++)
	{
		GlobalMapDoorPtr curDoor = &doorBuildList[curDoorIndex];
		if ((curDoor->row == row) && (curDoor->col == col))
			if ((curDoor->length == length) && (curDoor->direction[0] == dir))
				newDoor = false;
	}
	if (newDoor)
	{
		/*
		if (area1 > area2) {
			//-----------------------------
			// Smaller area number first...
			int32_t savedArea1 = area1;
			area1 = area2;
			area2 = savedArea1;
			dir = (dir + 2) % 4;
		}
		*/
		doorBuildList[numDoors].row = row;
		doorBuildList[numDoors].col = col;
		doorBuildList[numDoors].length = length;
		doorBuildList[numDoors].open = true;
		doorBuildList[numDoors].teamID = -1;
		doorBuildList[numDoors].area[0] = area1;
		doorBuildList[numDoors].areaCost[0] = 1;
		doorBuildList[numDoors].direction[0] = dir;
		doorBuildList[numDoors].area[1] = area2;
		doorBuildList[numDoors].areaCost[1] = 1;
		doorBuildList[numDoors].direction[1] = (dir + 2) % 4;
		numDoors++;
		if (numDoors >= MAX_GLOBALMAP_DOORS)
			Fatal(numDoors, " Too many Global Doors ");
	}
	if (numDoors >= MAX_GLOBALMAP_DOORS)
		STOP(("Too Many Doors %d", numDoors));
}

//------------------------------------------------------------------------------------------

void GlobalMap::endDoorProcessing(void)
{
	//	systemHeap->walkHeap();
	if (doorBuildList)
	{
		//-----------------------------------------------------------------------
		// First, save the door list. Note that we make 2 extra doors, to be
		// used by the pathfinder...
		doors = (GlobalMapDoorPtr)systemHeap->Malloc(
			sizeof(GlobalMapDoor) * (numDoors + NUM_DOOR_OFFSETS));
		memcpy(doors, doorBuildList, sizeof(GlobalMapDoor) * (numDoors + NUM_DOOR_OFFSETS));
		//----------------------------
		// Free the temp build list...
		systemHeap->Free(doorBuildList);
		doorBuildList = nullptr;
	}
	//	systemHeap->walkHeap();
}

//------------------------------------------------------------------------------------------

int32_t
GlobalMap::numAreaDoors(int32_t area)
{
	int32_t doorCount = 0;
	for (size_t doorIndex = 0; doorIndex < numDoors; doorIndex++)
		if ((doors[doorIndex].area[0] == area) || (doors[doorIndex].area[1] == area))
			doorCount++;
	if (doorCount > 255)
		Fatal(doorCount, " Too many area doors ");
	return (doorCount);
}

//------------------------------------------------------------------------------------------

void GlobalMap::getAreaDoors(int32_t area, DoorInfoPtr doorList)
{
	int32_t doorCount = 0;
	for (size_t doorIndex = 0; doorIndex < numDoors; doorIndex++)
		if ((doors[doorIndex].area[0] == area) || (doors[doorIndex].area[1] == area))
		{
			doorList[doorCount].doorIndex = doorIndex;
			int32_t doorSide = (doors[doorIndex].area[1] == area);
			doorList[doorCount].doorSide = doorSide;
			doorCount++;
		}
}

//------------------------------------------------------------------------------------------

#define MAX_DOORS_PER_SECTOR 500 // This should be WAY more than we'll ever need...

int32_t maxNumDoors = 0;

void GlobalMap::calcGlobalDoors(void)
{
	int16_t doorMap[SECTOR_DIM][SECTOR_DIM];
	beginDoorProcessing();
	for (size_t sectorR = 0; sectorR < sectorheight; sectorR++)
	{
		for (size_t sectorC = 0; sectorC < sectorwidth; sectorC++)
		{
			for (size_t dir = 1; dir < 3; dir++)
			{
				for (size_t x = 0; x < SECTOR_DIM; x++)
					for (size_t y = 0; y < SECTOR_DIM; y++)
						doorMap[x][y] = -1;
				minRow = sectorR * SECTOR_DIM;
				maxRow = minRow + SECTOR_DIM;
				minCol = sectorC * SECTOR_DIM;
				maxCol = minCol + SECTOR_DIM;
				//-------------------------------------------
				// First, find all doors in this direction...
				for (size_t r = minRow; r < maxRow; r++)
					for (size_t c = minCol; c < maxCol; c++)
					{
						//-------------------------------------------------------
						// First, check each cell in the sector to see if
						// it's a "door" cell (is open and adjacent to an
						// open cell in another area in the current
						// direction)...
						int32_t curArea = areaMap[r * width + c];
						if (curArea > -1)
						{
							//---------------------------------------------------------
							// The doorMap tracks, for each tile, what area is
							// adjacent to it thru a door...
							int32_t adjR = r + adjTile[dir][0];
							int32_t adjC = c + adjTile[dir][1];
							if ((adjR >= 0) && (adjR < height) && (adjC >= 0) && (adjC < width))
							{
								int32_t adjArea = areaMap[adjR * width + adjC];
								if ((adjArea > -1) && (curArea != adjArea))
								{
									AreaType curAreaType = areas[curArea].type;
									AreaType adjAreaType = areas[adjArea].type;
									bool validDoor = false;
									if ((curAreaType == AREA_TYPE_NORMAL) && (adjAreaType == AREA_TYPE_NORMAL))
										validDoor = true;
									if (validDoor)
										doorMap[r - minRow][c - minCol] = adjArea;
								}
							}
						}
					}
				//--------------------------------------------------------------------------
				// Now, process the doors and add them to the global door list.
				// The direction we're currently processing will dictate how to
				// make our sweep across the sector map. So, we have a chunk a
				// code for each direction (THIS MUST BE MODIFIED IF WE INCLUDE
				// DIAGONAL DIRECTIONS WHEN MOVING ON THE GLOBAL MAP)...
				if (dir == 1)
				{
					for (size_t c = maxCol - 1; c >= minCol; c--)
					{
						int32_t r = minRow;
						while (r < maxRow)
						{
							int32_t adjArea = doorMap[r - minRow][c - minCol];
							if (adjArea > -1)
							{
								int32_t curArea = areaMap[r * width + c];
								//-----------------------------------
								// We have a door. Calc its length...
								int32_t length = 0;
								while ((r < maxRow) && (areaMap[r * width + c] == curArea) && (doorMap[r - minRow][c - minCol] == adjArea))
								{
									length++;
									r++;
								}
								//---------------------------------------
								// Now, add it to the global door list...
								addDoor(curArea, adjArea, r - length, c, length, dir);
							}
							else
								r++;
						}
					}
				}
				else
				{
					for (size_t r = maxRow - 1; r >= minRow; r--)
					{
						int32_t c = minCol;
						while (c < maxCol)
						{
							int32_t adjArea = doorMap[r - minRow][c - minCol];
							if (adjArea > -1)
							{
								int32_t curArea = areaMap[r * width + c];
								//-----------------------------------
								// We have a door. Calc its length...
								int32_t length = 0;
								while ((c < maxCol) && (areaMap[r * width + c] == curArea) && (doorMap[r - minRow][c - minCol] == adjArea))
								{
									length++;
									c++;
								}
								//-----------------------------------------
								// Now, add it to the sector's door list...
								addDoor(curArea, adjArea, r, c - length, length, dir);
							}
							else
								c++;
						}
					}
				}
			}
		}
	}
	endDoorProcessing();
}

//------------------------------------------------------------------------------------------

void GlobalMap::calcAreaDoors(void)
{
	numDoorInfos = 0;
	for (size_t area = 0; area < numAreas; area++)
	{
		areas[area].numDoors = numAreaDoors(area);
		numDoorInfos += areas[area].numDoors;
		if (areas[area].numDoors)
		{
			areas[area].doors =
				(DoorInfoPtr)systemHeap->Malloc(sizeof(DoorInfo) * areas[area].numDoors);
			getAreaDoors(area, areas[area].doors);
		}
		else
			areas[area].doors = nullptr;
	}
}

//------------------------------------------------------------------------------------------

int32_t
GlobalMap::calcLinkCost(int32_t startDoor, int32_t thruArea, int32_t goalDoor)
{
	if ((doors[startDoor].area[0] != thruArea) && (doors[startDoor].area[1] != thruArea))
		return (-1);
	int32_t startSide = (doors[startDoor].area[1] == thruArea);
	int32_t startRow = doors[startDoor].row;
	int32_t startCol = doors[startDoor].col;
	if (doors[startDoor].direction[0] == 1)
	{
		//-------------------------------------------------------------------------------
		// Door goes from west to east
		// For now, use the midpoint of the door for purposes of calcing the
		// cost between doors...
		startRow += (doors[startDoor].length / 2);
		startCol += startSide;
	}
	else
	{
		//------------------------------
		// Door goes from north to south
		// For now, use the midpoint of the door for purposes of calcing the
		// cost between doors...
		startRow += startSide;
		startCol += (doors[startDoor].length / 2);
	}
	if ((doors[goalDoor].area[0] != thruArea) && (doors[goalDoor].area[1] != thruArea))
		return (-2);
	int32_t goalSide = (doors[goalDoor].area[1] == thruArea);
	int32_t goalRow = doors[goalDoor].row;
	int32_t goalCol = doors[goalDoor].col;
	if (doors[goalDoor].direction[0] == 1)
	{
		//-------------------------------------------------------------------
		// Door goes from west to east. For now, use the midpoint of the door
		// for purposes of calcing the cost between doors...
		goalRow += (doors[goalDoor].length / 2);
		goalCol += goalSide;
	}
	else
	{
		//------------------------------
		// Door goes from north to south
		goalRow += goalSide;
		goalCol += (doors[goalDoor].length / 2);
	}
	Stuff::Vector3D goalWorldPos;
	land->cellToWorld(goalRow, goalCol, goalWorldPos);
	// goalWorldPos.x = (float)goalCol * Terrain::worldUnitsPerCell +
	// Terrain::worldUnitsPerCell / 2 - Terrain::worldUnitsMapSide / 2;
	// goalWorldPos.y = (Terrain::worldUnitsMapSide / 2) - ((float)goalCellR *
	// Terrain::worldUnitsPerCell) - Terrain::worldUnitsPerCell / 2;
	// goalWorldPos.z = (float)0; // How do we get the elevation for this point?
	// Do we care?
	int32_t cost = 9999;
	if (!PathFindMap[SECTOR_PATHMAP])
	{
		PathFindMap[SECTOR_PATHMAP] = new MoveMap;
		PathFindMap[SECTOR_PATHMAP]->init(30, 30);
	}
	if (!PathFindMap[SIMPLE_PATHMAP])
	{
		PathFindMap[SIMPLE_PATHMAP] = new MoveMap;
		PathFindMap[SIMPLE_PATHMAP]->init(SimpleMovePathRange * 2 + 1, SimpleMovePathRange * 2 + 1);
	}
	//-------------------------------------------------------------------------
	// Let's make sure the bridge tiles are NOT blocked here (since, during the
	// game, they won't be, unless destroyed, in which case we won't care since
	// they're their own area)...
	MovePath newPath;
	if (blank)
	{
		newPath.numSteps = 10;
		newPath.cost = 10;
	}
	else if ((areas[thruArea].type == AREA_TYPE_WALL) || (areas[thruArea].type == AREA_TYPE_GATE) || (areas[thruArea].type == AREA_TYPE_FOREST))
	{
		//---------------------------------------------
		// These costs are for when the area is open...
		newPath.numSteps = 10;
		newPath.cost = 10;
	}
	else
	{
		ClearBridgeTiles = true;
		int32_t mapULr = areas[thruArea].sectorR * SECTOR_DIM;
		int32_t mapULc = areas[thruArea].sectorC * SECTOR_DIM;
		int32_t moveparams = MOVEPARAM_NONE;
		if (hover)
			moveparams |= (MOVEPARAM_WATER_SHALLOW + MOVEPARAM_WATER_DEEP);
		else if (!blank)
			moveparams |= MOVEPARAM_WATER_SHALLOW;
#if USE_SEPARATE_WATER_MAPS
		// To REALLY fix the separate water maps, we should do this for vehicles
		// and mechs separately (and vehicles don't use shallow). But, if
		// nothing else, this fixes the bug where we weren't passing in the
		// moveparams.
		PathFindMap[SECTOR_PATHMAP]->setUp(mapULr, mapULc, SECTOR_DIM, SECTOR_DIM, hover ? 1 : 0,
			nullptr, startRow, startCol, goalWorldPos, goalRow - mapULr, goalCol - mapULc, 10, 0, 8,
			moveparams);
#else
		PathFindMap[SECTOR_PATHMAP]->setUp(mapULr, mapULc, SECTOR_DIM, SECTOR_DIM, hover ? 1 : 0,
			nullptr, startRow, startCol, goalWorldPos, goalRow - mapULr, goalCol - mapULc, 10, 0, 8,
			MOVEPARAM_NONE);
#endif
		int32_t goalCell[2];
		PathFindMap[SECTOR_PATHMAP]->calcPath(&newPath, nullptr, goalCell);
		ClearBridgeTiles = false;
	}
	//-------------------------------------------------------------------------
	// If there is no path, find out why! In theory, this should never occur...
	if (newPath.numSteps == 0)
	{
		/*
		File* pathDebugFile = new File;
		pathDebugFile->create("movemap1.dbg");
		PathFindMap->writeDebug(pathDebugFile);
		pathDebugFile->close();
		delete pathDebugFile;
		pathDebugFile = nullptr;
		*/
		//--------------------------------------------------------------------------
		// If the cost is 1, then our start and goal cells are the same. Thus,
		// there is a path, it's just REALLY int16_t :)
		if (newPath.cost == 1)
			return (1);
		else
			return (9999);
	}
	else
		cost = newPath.getCost();
	//#endif
	gosASSERT(cost != 0);
	return (cost);
}

//------------------------------------------------------------------------------------------

void GlobalMap::changeAreaLinkCost(int32_t area, int32_t cost)
{
	GlobalMapAreaPtr thruArea = &areas[area];
	int32_t numDoors = thruArea->numDoors;
	for (size_t i = 0; i < numDoors; i++)
	{
		GlobalMapDoorPtr curDoor = &doors[thruArea->doors[i].doorIndex];
		int32_t doorSide = thruArea->doors[i].doorSide;
		for (size_t j = 0; j < curDoor->numLinks[doorSide]; j++)
			curDoor->links[doorSide][j].cost = cost;
	}
}

//------------------------------------------------------------------------------------------

void GlobalMap::calcDoorLinks(void)
{
	numDoorLinks = 0;
	int32_t maxDoors = 0;
	for (size_t d = 0; d < numDoors; d++)
	{
		GlobalMapDoorPtr thisDoor = &doors[d];
		for (size_t s = 0; s < 2; s++)
		{
			thisDoor->numLinks[s] = 0;
			thisDoor->links[s] = nullptr;
			int32_t area = thisDoor->area[s];
			int32_t areaNumDoors = areas[area].numDoors;
			// if (areaNumDoors >= 0) {
			thisDoor->numLinks[s] = areaNumDoors - 1;
			//----------------------------------------------------------------
			// Allocate enough links for all links to this door plus a scratch
			// link used during path calc...
			thisDoor->links[s] = (DoorLinkPtr)systemHeap->Malloc(
				sizeof(DoorLink) * (thisDoor->numLinks[s] + NUM_EXTRA_DOOR_LINKS));
			numDoorLinks += (thisDoor->numLinks[s] + NUM_EXTRA_DOOR_LINKS);
			gosASSERT(thisDoor->links[s] != nullptr);
			int32_t linkIndex = 0;
			for (size_t areaDoor = 0; areaDoor < areaNumDoors; areaDoor++)
			{
				int32_t doorIndex = areas[area].doors[areaDoor].doorIndex;
				GlobalMapDoorPtr curDoor = &doors[doorIndex];
				if (curDoor != thisDoor)
				{
					thisDoor->links[s][linkIndex].doorIndex = doorIndex;
					thisDoor->links[s][linkIndex].doorSide = (curDoor->area[1] == area);
					thisDoor->links[s][linkIndex].cost = calcLinkCost(d, area, doorIndex);
					thisDoor->links[s][linkIndex].openCost = thisDoor->links[s][linkIndex].cost;
					linkIndex++;
				}
			}
			//}
			if (areaNumDoors > maxDoors)
				maxDoors = areaNumDoors;
		}
	}
	//----------------------------------------
	// Now, set up the start and goal doors...
	doors[numDoors + DOOR_OFFSET_START].numLinks[0] = maxDoors;
	numDoorLinks += (doors[numDoors + DOOR_OFFSET_START].numLinks[0] + NUM_EXTRA_DOOR_LINKS);
	doors[numDoors + DOOR_OFFSET_START].links[0] =
		(DoorLinkPtr)systemHeap->Malloc(sizeof(DoorLink) * (doors[numDoors + DOOR_OFFSET_START].numLinks[0] + NUM_EXTRA_DOOR_LINKS));
	doors[numDoors + DOOR_OFFSET_START].numLinks[1] = 0;
	numDoorLinks += (doors[numDoors + DOOR_OFFSET_START].numLinks[1] + NUM_EXTRA_DOOR_LINKS);
	doors[numDoors + DOOR_OFFSET_START].links[1] =
		(DoorLinkPtr)systemHeap->Malloc(sizeof(DoorLink) * (doors[numDoors + DOOR_OFFSET_START].numLinks[1] + NUM_EXTRA_DOOR_LINKS));
	doors[numDoors + DOOR_OFFSET_GOAL].numLinks[0] = maxDoors;
	numDoorLinks += (doors[numDoors + DOOR_OFFSET_GOAL].numLinks[0] + NUM_EXTRA_DOOR_LINKS);
	doors[numDoors + DOOR_OFFSET_GOAL].links[0] = (DoorLinkPtr)systemHeap->Malloc(
		sizeof(DoorLink) * (doors[numDoors + DOOR_OFFSET_GOAL].numLinks[0] + NUM_EXTRA_DOOR_LINKS));
	doors[numDoors + DOOR_OFFSET_GOAL].numLinks[1] = 0;
	numDoorLinks += (doors[numDoors + DOOR_OFFSET_GOAL].numLinks[1] + NUM_EXTRA_DOOR_LINKS);
	doors[numDoors + DOOR_OFFSET_GOAL].links[1] = (DoorLinkPtr)systemHeap->Malloc(
		sizeof(DoorLink) * (doors[numDoors + DOOR_OFFSET_GOAL].numLinks[1] + NUM_EXTRA_DOOR_LINKS));
	int32_t numberL = 0;
	for (size_t i = 0; i < (numDoors + NUM_DOOR_OFFSETS); i++)
	{
		int32_t numLinks = doors[i].numLinks[0] + NUM_EXTRA_DOOR_LINKS;
		gosASSERT(numLinks >= 2);
		// result = packetFile->writePacket(whichPacket++,
		// (uint8_t*)doors[i].links[0], sizeof(DoorLink) * numLinks);  if (result
		// <= 0) 	Fatal(result, " GlobalMap.write: Unable to write doorLinks
		// packet ");
		numberL += numLinks;
		numLinks = doors[i].numLinks[1] + NUM_EXTRA_DOOR_LINKS;
		gosASSERT(numLinks >= 2);
		// result = packetFile->writePacket(whichPacket++,
		// (uint8_t*)doors[i].links[1], sizeof(DoorLink) * numLinks);  if (result
		// <= 0) 	Fatal(result, " GlobalMap.write: Unable to write doorLinks
		// packet ");
		numberL += numLinks;
	}
	Assert(numberL == numDoorLinks, 0, " HUH ");
}

//------------------------------------------------------------------------------------------

int32_t
GlobalMap::getPathCost(
	int32_t startArea, int32_t goalArea, bool withSpecialAreas, int32_t& confidence, bool calcIt)
{
	//------------------------------------------------------------------------
	// This could be cleaned up if we re-save this data, 0 being no path and 1
	// indicating startArea == goalArea.
	if (startArea < 0)
		return (0);
	if (goalArea < 0)
		return (0);
	if (!areas[startArea].open)
		return (0);
	if (!areas[goalArea].open)
		return (0);
	if (startArea == goalArea)
		return (1);
#if 1
	GlobalPathStep path[MAX_GLOBAL_PATH];
	if (withSpecialAreas)
		useClosedAreas = true;
	else
		useClosedAreas = false;
	int32_t cost = calcPath(startArea, goalArea, path);
	useClosedAreas = false;
	confidence = GLOBAL_CONFIDENCE_GOOD;
	return (cost);
#else
	calcedPathCost = false;
	uint8_t data = pathCostTable[startArea * numAreas + goalArea];
	if (withSpecialAreas)
	{
		uint8_t cost = (data & 0x0C) >> 2;
		if (data & GLOBAL_FLAG_SPECIAL_IMPOSSIBLE)
		{
			cost = 0;
			confidence = GLOBAL_CONFIDENCE_GOOD;
		}
		else if (data & GLOBAL_FLAG_SPECIAL_CALC)
		{
			if (calcIt)
			{
				GlobalPathStep path[MAX_GLOBAL_PATH];
				useClosedAreas = true;
				calcPath(startArea, goalArea, path);
				useClosedAreas = false;
				cost = getPathCost(startArea, goalArea, true, confidence, false);
				calcedPathCost = true;
				confidence = GLOBAL_CONFIDENCE_GOOD;
			}
			else
				confidence = GLOBAL_CONFIDENCE_BAD;
		}
		else
			confidence = GLOBAL_CONFIDENCE_GOOD;
		return (cost);
	}
	else
	{
		uint8_t cost = (data & 0x03);
		if (data & GLOBAL_FLAG_SPECIAL_IMPOSSIBLE)
		{
			cost = 0;
			confidence = GLOBAL_CONFIDENCE_GOOD;
		}
		else if (data & GLOBAL_FLAG_NORMAL_CLOSES)
		{
			if (calcIt)
			{
				GlobalPathStep path[MAX_GLOBAL_PATH];
				calcPath(startArea, goalArea, path);
				cost = getPathCost(startArea, goalArea, false, confidence, false);
				calcedPathCost = true;
				confidence = GLOBAL_CONFIDENCE_GOOD;
			}
			else if (cost)
				confidence = GLOBAL_CONFIDENCE_BAD;
			else
				confidence = GLOBAL_CONFIDENCE_GOOD;
		}
		else if (data & GLOBAL_FLAG_NORMAL_OPENS)
		{
			if (calcIt)
			{
				GlobalPathStep path[MAX_GLOBAL_PATH];
				calcPath(startArea, goalArea, path);
				cost = getPathCost(startArea, goalArea, false, confidence, false);
				calcedPathCost = true;
				confidence = GLOBAL_CONFIDENCE_GOOD;
			}
			else
				confidence = GLOBAL_CONFIDENCE_AT_LEAST;
		}
		else
			confidence = GLOBAL_CONFIDENCE_GOOD;
		return (cost);
	}
	return (0);
#endif
}

//---------------------------------------------------------------------------

#if USE_PATH_COST_TABLE

void GlobalMap::setPathFlag(int32_t startArea, int32_t goalArea, uint8_t flag, bool set)
{
	int32_t index = startArea * numAreas + goalArea;
	pathCostTable[index] &= (flag ^ 0xFF);
	if (set)
		pathCostTable[index] |= flag;
}

//---------------------------------------------------------------------------

int32_t
GlobalMap::getPathFlag(int32_t startArea, int32_t goalArea, uint8_t flag)
{
	return (pathCostTable[startArea * numAreas + goalArea] & flag);
}

//---------------------------------------------------------------------------

void GlobalMap::setPathCost(
	int32_t startArea, int32_t goalArea, bool withSpecialAreas, uint8_t cost)
{
	if (cost > 0)
	{
		if (cost < 6)
			cost = 1;
		else if (cost < 11)
			cost = 2;
		else
			cost = 3;
	}
	int32_t index = startArea * numAreas + goalArea;
	if (withSpecialAreas)
	{
		pathCostTable[index] &= 0xF3;
		cost <<= 2;
	}
	else
		pathCostTable[index] &= 0xFC;
	pathCostTable[index] |= cost;
}

//------------------------------------------------------------------------------------------

void GlobalMap::initPathCostTable(void)
{
	if (pathCostTable)
	{
		systemHeap->Free(pathCostTable);
		pathCostTable = nullptr;
	}
	int32_t oldNumAreas = numAreas;
	//	if (numAreas > 600)
	//		numAreas = 600;
	pathCostTable = (uint8_t*)systemHeap->Malloc(numAreas * numAreas);
	gosASSERT(pathCostTable != nullptr);
	for (size_t startArea = 0; startArea < numAreas; startArea++)
		for (goalArea = 0; goalArea < numAreas; goalArea++)
		{
			setPathCost(startArea, goalArea, false, 0);
			setPathCost(startArea, goalArea, true, 0);
			setPathFlag(startArea, goalArea, GLOBAL_FLAG_SPECIAL_IMPOSSIBLE, false);
			setPathFlag(startArea, goalArea, GLOBAL_FLAG_SPECIAL_CALC, true);
			setPathFlag(startArea, goalArea, GLOBAL_FLAG_NORMAL_OPENS, true);
			setPathFlag(startArea, goalArea, GLOBAL_FLAG_NORMAL_CLOSES, true);
		}
	closes = false;
	opens = false;
	numAreas = oldNumAreas;
}

//------------------------------------------------------------------------------------------

void GlobalMap::resetPathCostTable(void)
{
	if (!pathCostTable)
		return;
	if (!closes && !opens)
		return;
	for (size_t startArea = 0; startArea < numAreas; startArea++)
		for (goalArea = 0; goalArea < numAreas; goalArea++)
		{
			setPathFlag(startArea, goalArea, GLOBAL_FLAG_NORMAL_OPENS, opens);
			setPathFlag(startArea, goalArea, GLOBAL_FLAG_NORMAL_CLOSES, closes);
		}
	closes = false;
	opens = false;
}

//------------------------------------------------------------------------------------------

void GlobalMap::calcPathCostTable(void)
{
	if (pathCostTable)
	{
		systemHeap->Free(pathCostTable);
		pathCostTable = nullptr;
	}
	pathCostTable = (uint8_t*)systemHeap->Malloc(numAreas * numAreas);
	gosASSERT(pathCostTable != nullptr);
	for (size_t i = 0; i < numAreas; i++)
		openArea(i);
	GlobalPathStep globalPath[MAX_GLOBAL_PATH];
	for (size_t startArea = 0; startArea < numAreas; startArea++)
		for (goalArea = 0; goalArea < numAreas; goalArea++)
		{
			int32_t numSteps = calcPath(startArea, goalArea, globalPath);
			//----------------------------------------------------------
			// For now, we'll just store the number of area steps in the
			// table. If no path (or start and goal are the same), we'll store
			// 0. 255 is reserved to mark the need to re-calc during the game...
			setPathCost(startArea, goalArea, true, numSteps);
			setPathFlag(startArea, goalArea, GLOBAL_FLAG_SPECIAL_IMPOSSIBLE, false);
			setPathFlag(startArea, goalArea, GLOBAL_FLAG_SPECIAL_CALC, true);
			setPathFlag(startArea, goalArea, GLOBAL_FLAG_NORMAL_OPENS, false);
			setPathFlag(startArea, goalArea, GLOBAL_FLAG_NORMAL_CLOSES, false);
		}
	for (i = 0; i < numAreas; i++)
		if ((areas[i].type == AREA_TYPE_WALL) || (areas[i].type == AREA_TYPE_GATE) || (areas[i].type == AREA_TYPE_FOREST))
			openArea(i);
	for (startArea = 0; startArea < numAreas; startArea++)
		for (goalArea = 0; goalArea < numAreas; goalArea++)
		{
			int32_t numSteps = calcPath(startArea, goalArea, globalPath);
			//----------------------------------------------------------
			// For now, we'll just store the number of area steps in the
			// table. If no path (or start and goal are the same), we'll store
			// 0. 255 is reserved to mark the need to re-calc during the game...
			setPathCost(startArea, goalArea, true, numSteps);
		}
	for (i = 0; i < numAreas; i++)
		if ((areas[i].type == AREA_TYPE_WALL) || (areas[i].type == AREA_TYPE_GATE) || (areas[i].type == AREA_TYPE_FOREST))
			closeArea(i);
	closes = false;
	opens = false;
}

#endif
//------------------------------------------------------------------------------------------

void GlobalMap::clearPathExistsTable(void)
{
	int32_t tableSize = numAreas * (numAreas / 4 + 1);
	memset(pathExistsTable, GLOBALPATH_EXISTS_UNKNOWN, tableSize);
}

//------------------------------------------------------------------------------------------

void GlobalMap::setPathExists(int32_t fromArea, int32_t toArea, uint8_t set)
{
	if (!pathExistsTable)
		return;
	if (fromArea < 0)
		return;
	if (toArea < 0)
		return;
	int32_t rowwidth = numAreas / 4 + 1;
	uint8_t* pathByte = pathExistsTable;
	pathByte += (rowwidth * fromArea + (toArea / 4));
	uint8_t pathShift = (toArea % 4) * 2;
	uint8_t pathBit = 0x03 << pathShift;
	*pathByte &= (pathBit ^ 0xFF);
	if (set)
		*pathByte |= (set << pathShift);
}

//------------------------------------------------------------------------------------------

uint8_t
GlobalMap::getPathExists(int32_t fromArea, int32_t toArea)
{
	if (!pathExistsTable)
		return (false);
	if (fromArea < 0)
		return (false);
	if (toArea < 0)
		return (false);
	int32_t rowwidth = numAreas / 4 + 1;
	uint8_t* pathByte = pathExistsTable;
	pathByte += (rowwidth * fromArea + (toArea / 4));
	uint8_t pathShift = (toArea % 4) * 2;
	uint8_t pathBit = 0x03 << pathShift;
	return (*pathByte & pathBit);
}

//------------------------------------------------------------------------------------------

int32_t
GlobalMap::exitDirection(int32_t doorIndex, int32_t fromArea)
{
	if (doors[doorIndex].area[0] == fromArea)
		return (doors[doorIndex].direction[0]);
	else if (doors[doorIndex].area[1] == fromArea)
		return (doors[doorIndex].direction[1]);
	return (-1);
}

//------------------------------------------------------------------------------------------

void GlobalMap::getDoorTiles(int32_t area, int32_t door, GlobalMapDoorPtr areaDoor)
{
	*areaDoor = doors[areas[area].doors[door].doorIndex];
}

//------------------------------------------------------------------------------------------

bool GlobalMap::getAdjacentAreaCell(
	int32_t area, int32_t adjacentArea, int32_t& cellRow, int32_t& cellCol)
{
	for (size_t i = 0; i < areas[area].numDoors; i++)
	{
		int32_t doorIndex = areas[area].doors[i].doorIndex;
		int32_t doorSide = areas[area].doors[i].doorSide;
		if (doors[doorIndex].area[doorSide % 1] == adjacentArea)
		{
			if ((doors[doorIndex].area[0] != area) && (doors[doorIndex].area[1] != area))
				STOP(("bad adjacent area door", 0));
			int32_t goalSide = (doors[doorIndex].area[1] == adjacentArea);
			cellRow = doors[doorIndex].row;
			cellCol = doors[doorIndex].col;
			if (doors[doorIndex].direction[0] == 1)
			{
				//-------------------------------------------------------------------
				// Door goes from west to east. For now, use the midpoint of the
				// door for purposes of calcing the cost between doors...
				cellRow += (doors[doorIndex].length / 2);
				cellCol += goalSide;
			}
			else
			{
				//------------------------------
				// Door goes from north to south
				cellRow += goalSide;
				cellCol += (doors[doorIndex].length / 2);
			}
			return (true);
		}
	}
	return (false);
}

//------------------------------------------------------------------------------------------

Stuff::Vector3D
GlobalMap::getDoorWorldPos(int32_t area, int32_t door, int32_t* prevGoalCell)
{
	int32_t cellR, cellC;
	cellR = prevGoalCell[0]; // + adjTile[areas[area].doors[door].direction][0];
	cellC = prevGoalCell[1]; // += adjTile[areas[area].doors[door].direction][1];
	Stuff::Vector3D pos;
	pos.Zero();
	pos.x = (float)cellC * Terrain::worldUnitsPerCell + Terrain::worldUnitsPerCell / 2 - Terrain::worldUnitsMapSide / 2;
	pos.y = (Terrain::worldUnitsMapSide / 2) - ((float)cellR * Terrain::worldUnitsPerCell) - Terrain::worldUnitsPerCell / 2;
	pos.z = (float)land->getTerrainElevation(
		pos); // How do we get the elevation for this point? Do we care?
	return (pos);
}

//------------------------------------------------------------------------------------------

int32_t
GlobalMap::build(MissionMapCellInfo* mapData)
{
#ifdef _DEBUG
	// systemHeap->walkHeap(false,false,"GlobalMap BAD HEAP1\n");
#endif
	//--------------------------------
	// Note that h and w are in tiles.
	bool wasBlank = blank;
	init(GameMap->height, GameMap->width);
	blank = wasBlank;
	for (size_t r = 0; r < height; r++)
		for (size_t c = 0; c < width; c++)
			GameMap->setBuildNotSet(r, c, true);
	numAreas = 0;
	numSpecialAreas = 0;
	if (specialAreas)
	{
		systemHeap->Free(specialAreas);
		specialAreas = nullptr;
	}
	if (mapData)
	{
		specialAreas = (GlobalSpecialAreaInfo*)systemHeap->Malloc(
			sizeof(GlobalSpecialAreaInfo) * MAX_SPECIAL_AREAS);
		if (specialAreas == nullptr)
			Fatal(0, " GlobalMap.build: unable to malloc specialAreas ");
		calcSpecialAreas(mapData);
	}
	calcAreas();
	calcCellsCovered();
	calcGlobalDoors();
	calcSpecialTypes();
	calcAreaDoors();
	calcDoorLinks();
	//----------------------------------------------
	// Now, build the path tables for each sector...
	/*	for (r = 0; r < tileheight; r++)
			for (int32_t c = 0; c < tilewidth; c++)
				calcSectorPaths(scenarioMap, r, c);
	*/
#ifdef _DEBUG
	// systemHeap->walkHeap(false,false,"GlobalMap BAD HEAP2\n");
#endif
	// if (blank)
	//	calcPathCostTable();
	if (specialAreas)
	{
		systemHeap->Free(specialAreas);
		specialAreas = nullptr;
	}
#ifdef DEBUG_GLOBALMAP_BUILD
	if (logEnabled && !blank)
	{
		wchar_t s[256];
		for (size_t i = 0; i < numDoors; i++)
		{
			sprintf(s, "door %05d, %s(%d), areas %d & %d", i, doors[i].open ? "opened" : "CLOSED",
				doors[i].teamID, doors[i].area[0], doors[i].area[1]);
			log->write(s);
			for (size_t side = 0; side < 2; side++)
			{
				sprintf(s, "     side %d", side);
				log->write(s);
				for (size_t j = 0; j < doors[i].numLinks[side]; j++)
				{
					sprintf(s, "          link %03d, to door %05d, cost %d", j,
						doors[i].links[side][j].doorIndex, doors[i].links[side][j].cost);
					log->write(s);
				}
			}
		}
		log->write(" ");
		for (i = 0; i < numAreas; i++)
		{
			sprintf(
				s, "area %05d, %s(%d)", i, areas[i].open ? "opened" : "CLOSED", areas[i].teamID);
			log->write(s);
			if (areas[i].ownerWID > 0)
			{
				sprintf(s, "     ownerWID is %d", areas[i].ownerWID);
				log->write(s);
			}
			static std::wstring_view typeString[] = {
				"normal area", "wall area", "gate area", "land bridge area", "forest area"};
			sprintf(s, "     %s", typeString[areas[i].type]);
			log->write(s);
			for (size_t d = 0; d < areas[i].numDoors; d++)
			{
				sprintf(s, "     door %03d is %d (%d:%d & %d) ", d, areas[i].doors[d].doorIndex,
					areas[i].doors[d].doorSide, doors[areas[i].doors[d].doorIndex].area[0],
					doors[areas[i].doors[d].doorIndex].area[1]);
				log->write(s);
			}
		}
		log->write(" ");
	}
#endif
	return (NO_ERROR);
}

//---------------------------------------------------------------------------

void GlobalMap::setStartDoor(int32_t startArea)
{
	GlobalMapDoorPtr startDoor = &doors[numDoors + DOOR_OFFSET_START];
	startDoor->row = 0;
	startDoor->col = 0;
	startDoor->length = 0;
	startDoor->open = true;
	startDoor->teamID = -1;
	startDoor->area[0] = startArea;
	startDoor->area[1] = startArea;
	startDoor->areaCost[0] = 1;
	startDoor->areaCost[1] = 1;
	startDoor->direction[0] = -1;
	startDoor->direction[1] = -1;
	startDoor->numLinks[0] = areas[startArea].numDoors;
	startDoor->numLinks[1] = 0;
	startDoor->fromAreaIndex = 1;
	for (size_t curLink = 0; curLink < startDoor->numLinks[0]; curLink++)
	{
		//---------------------------------------------
		// Point the goal "door" to its area's doors...
		int32_t doorIndex = areas[startArea].doors[curLink].doorIndex;
		int32_t doorSide = areas[startArea].doors[curLink].doorSide;
		GlobalMapDoorPtr curDoor = &doors[areas[startArea].doors[curLink].doorIndex];
		int32_t costSum = 1;
		if (startCell[0] > -1)
		{
			if (startCell[0] > curDoor->row)
				costSum += (startCell[0] - curDoor->row);
			else
				costSum += (curDoor->row - startCell[0]);
			if (startCell[1] > curDoor->col)
				costSum += (startCell[1] - curDoor->col);
			else
				costSum += (curDoor->col - startCell[1]);
		}
		startDoor->links[0][curLink].doorIndex = doorIndex;
		startDoor->links[0][curLink].doorSide = doorSide;
		startDoor->links[0][curLink].cost = costSum;
		startDoor->links[0][curLink].openCost = costSum;
		//----------------------------------------------------
		// Make sure this area door points to the goal door...
		curDoor->links[doorSide][curDoor->numLinks[doorSide]].doorIndex =
			numDoors + DOOR_OFFSET_START;
		curDoor->links[doorSide][curDoor->numLinks[doorSide]].doorSide = 0;
		curDoor->links[doorSide][curDoor->numLinks[doorSide]].cost = costSum;
		curDoor->links[doorSide][curDoor->numLinks[doorSide]].openCost = costSum;
		curDoor->numLinks[doorSide]++;
	}
}

//---------------------------------------------------------------------------

void GlobalMap::resetStartDoor(int32_t startArea)
{
	GlobalMapDoorPtr startDoor = &doors[numDoors + DOOR_OFFSET_START];
	for (size_t curLink = 0; curLink < startDoor->numLinks[0]; curLink++)
	{
		int32_t doorSide = areas[startArea].doors[curLink].doorSide;
		doors[areas[startArea].doors[curLink].doorIndex].numLinks[doorSide]--;
	}
}

//---------------------------------------------------------------------------

void GlobalMap::setAreaTeamID(int32_t area, wchar_t teamID)
{
	//-----------------------------------------------------------------------
	// NOTE: This assumes there won't be adjacent areas with team alignments.
	// If there are, we should record two teamIDs for each door (to track
	// the teamID for each "side" of the door).
	if (area < 0)
		return;
	GlobalMapAreaPtr curArea = &areas[area];
	curArea->teamID = teamID;
	for (size_t d = 0; d < curArea->numDoors; d++)
	{
		doors[curArea->doors[d].doorIndex].teamID = teamID;
	}
	// opens = true;
}

//---------------------------------------------------------------------------

void GlobalMap::setAreaOwnerWID(int32_t area, int32_t objWID)
{
	if (area < 0)
		return;
	areas[area].ownerWID = objWID;
}

//---------------------------------------------------------------------------

void GlobalMap::setGoalDoor(int32_t goalArea)
{
	if ((goalArea < 0) || (goalArea >= numAreas))
	{
		wchar_t errMsg[256];
		sprintf(errMsg, " GlobalMap.setGoalDoor: bad goalArea (%d of %d) ", goalArea, numAreas);
		gosASSERT((goalArea >= 0) || (goalArea < numAreas));
	}
	GlobalMapDoorPtr goalDoor = &doors[numDoors + DOOR_OFFSET_GOAL];
	goalSector[0] = areas[goalArea].sectorR;
	goalSector[1] = areas[goalArea].sectorC;
	goalDoor->row = 0;
	goalDoor->col = 0;
	goalDoor->length = 0;
	goalDoor->open = true;
	goalDoor->teamID = -1;
	goalDoor->area[0] = goalArea;
	goalDoor->area[1] = goalArea;
	goalDoor->areaCost[0] = 1;
	goalDoor->areaCost[1] = 1;
	goalDoor->direction[0] = -1;
	goalDoor->direction[1] = -1;
	goalDoor->numLinks[0] = areas[goalArea].numDoors;
	goalDoor->numLinks[1] = 0;
	for (size_t curLink = 0; curLink < goalDoor->numLinks[0]; curLink++)
	{
		//---------------------------------------------
		// Point the goal "door" to its area's doors...
		int32_t doorIndex = areas[goalArea].doors[curLink].doorIndex;
		int32_t doorSide = areas[goalArea].doors[curLink].doorSide;
		gosASSERT((doorIndex >= 0) && (doorIndex < (numDoors + NUM_DOOR_OFFSETS)));
		GlobalMapDoorPtr curDoor = &doors[doorIndex];
		int32_t costSum = 1;
		if (goalCell[0] > -1)
		{
			if (goalCell[0] > curDoor->row)
				costSum += (goalCell[0] - curDoor->row);
			else
				costSum += (curDoor->row - goalCell[0]);
			if (goalCell[1] > curDoor->col)
				costSum += (goalCell[1] - curDoor->col);
			else
				costSum += (curDoor->col - goalCell[1]);
		}
		goalDoor->links[0][curLink].doorIndex = doorIndex;
		goalDoor->links[0][curLink].doorSide = doorSide;
		goalDoor->links[0][curLink].cost = costSum;
		goalDoor->links[0][curLink].openCost = costSum;
		//----------------------------------------------------
		// Make sure this area door points to the goal door...
		curDoor->links[doorSide][curDoor->numLinks[doorSide]].doorIndex =
			numDoors + DOOR_OFFSET_GOAL;
		curDoor->links[doorSide][curDoor->numLinks[doorSide]].doorSide = 0;
		curDoor->links[doorSide][curDoor->numLinks[doorSide]].cost = costSum;
		curDoor->links[doorSide][curDoor->numLinks[doorSide]].openCost = costSum;
		curDoor->numLinks[doorSide]++;
	}
}

//---------------------------------------------------------------------------

void GlobalMap::resetGoalDoor(int32_t goalArea)
{
	GlobalMapDoorPtr goalDoor = &doors[numDoors + DOOR_OFFSET_GOAL];
	for (size_t curLink = 0; curLink < goalDoor->numLinks[0]; curLink++)
	{
		int32_t doorSide = areas[goalArea].doors[curLink].doorSide;
		doors[areas[goalArea].doors[curLink].doorIndex].numLinks[doorSide]--;
	}
}

//---------------------------------------------------------------------------

#define AREA_ID_BASE 1000000

int32_t
GlobalMap::calcHPrime(int32_t door)
{
	gosASSERT((door > -1) && (door < (numDoors + NUM_DOOR_OFFSETS)));
	int32_t sectorR = (areas[doors[door].area[0]].sectorR + areas[doors[door].area[1]].sectorR) / 2;
	int32_t sectorC = (areas[doors[door].area[0]].sectorC + areas[doors[door].area[1]].sectorC) / 2;
	int32_t sum = 0;
	if (sectorR > goalSector[0])
		sum += (sectorR - goalSector[0]);
	else
		sum += (goalSector[0] - sectorR);
	if (sectorC > goalSector[1])
		sum += (sectorC - goalSector[1]);
	else
		sum += (goalSector[1] - sectorC);
	return (sum);
}

//---------------------------------------------------------------------------

inline void
GlobalMap::propogateCost(int32_t door, int32_t cost, int32_t fromAreaIndex, int32_t g)
{
	gosASSERT((door >= 0) && (door < (numDoors + NUM_DOOR_OFFSETS)) && ((fromAreaIndex == 0) || (fromAreaIndex == 1)));
	GlobalMapDoorPtr curMapDoor = &doors[door];
	if (curMapDoor->g > (g + cost))
	{
		curMapDoor->g = g + cost;
		curMapDoor->fPrime = curMapDoor->g + curMapDoor->hPrime;
		if (curMapDoor->flags & MOVEFLAG_OPEN)
		{
			int32_t openIndex = openList->find(door);
			if (!openIndex)
			{
				wchar_t s[128];
				sprintf(s,
					"GlobalMap.propogateCost: Cannot find globalmap door [%d, "
					"%d, %d, %d] for change\n",
					door, cost, fromAreaIndex, g);
				gosASSERT(openIndex != nullptr);
			}
			openList->change(door, curMapDoor->fPrime);
		}
		else
		{
			int32_t toAreaIndex = 1 - fromAreaIndex;
			int32_t numLinks = curMapDoor->numLinks[toAreaIndex];
			for (size_t curLink = 0; curLink < numLinks; curLink++)
			{
				int32_t succDoor = curMapDoor->links[toAreaIndex][curLink].doorIndex;
				gosASSERT((succDoor >= 0) && (succDoor < numDoors + NUM_DOOR_OFFSETS));
				GlobalMapDoorPtr succMapDoor = &doors[succDoor];
				int32_t succDoorCost = curMapDoor->links[toAreaIndex][curLink].cost;
				if (useClosedAreas)
				{
					if ((succMapDoor->teamID > -1) && (succMapDoor->teamID != moverTeamID))
						succDoorCost = 1000;
				}
				else
				{
					if ((succMapDoor->teamID > -1) && (succMapDoor->teamID != moverTeamID))
						succDoorCost = COST_BLOCKED;
					//--------------------------------------------------
					// Whether the door is even open is another issue...
					if (!succMapDoor->open)
						succDoorCost = COST_BLOCKED;
				}
				int32_t succFromAreaIndex = (succMapDoor->area[1] == curMapDoor->area[toAreaIndex]);
				if ((succMapDoor->open || useClosedAreas) && (succDoorCost < COST_BLOCKED))
					if ((succMapDoor->hPrime != HPRIME_NOT_CALCED) /* && (succMapDoor->hPrime < MaxHPrime)*/)
					{
						if (door == succMapDoor->parent)
							propogateCost(succDoor, succDoorCost, toAreaIndex, curMapDoor->g);
						else if ((curMapDoor->g + succDoorCost) < succMapDoor->g)
						{
							succMapDoor->cost = succDoorCost;
							succMapDoor->parent = door;
							succMapDoor->fromAreaIndex = succFromAreaIndex;
							propogateCost(succDoor, succDoorCost, succFromAreaIndex, curMapDoor->g);
						}
					}
			}
		}
	}
}

//---------------------------------------------------------------------------
#ifdef TERRAINEDIT
#define MAX_GLOBAL_PATH 50
#endif

int32_t
GlobalMap::calcPath(int32_t startArea, int32_t goalArea, GlobalPathStepPtr path,
	int32_t startRow, int32_t startCol, int32_t goalRow, int32_t goalCol)
{
#ifdef _DEBUG
	// systemHeap->walkHeap(false,false,"GlobalMap:calc BAD HEAP1\n");
#endif
	if ((startArea == -1) || (goalArea == -1))
		return (-1);
	startCell[0] = startRow;
	startCell[1] = startCol;
	goalCell[0] = goalRow;
	goalCell[1] = goalCol;
	if (logEnabled)
	{
		wchar_t s[50];
		sprintf(s, "     start = %d, goal = %d", startArea, goalArea);
		log->write(s);
		if (useClosedAreas)
			log->write("     USE CLOSED AREAS");
	}
	//----------------------------------------------------------------------
	// Door costs should be set here, if we want to modify them real-time...
	//-------------------------------------------------------------------------
	// We will not limit the search, as the global map should never be so big
	// that we'd WANT to limit the search. If so, time to change the sectorDim,
	// etc...
	//-----------------------------------------------
	// If we haven't already, create the OPEN list...
	if (!openList)
	{
		openList = new PriorityQueue;
		gosASSERT(openList != nullptr);
		openList->init(5000);
	}
	//---------------------------------------------------------------
	// NOTE: The last 6 doors are reserved for use by the pathfinder:
	//			numDoors + 0 = startArea
	//			numDoors + 1 = goalArea
	//			numDoors + 2 thru 4 = doors for "blocked" start area
	const int32_t startDoor = numDoors + DOOR_OFFSET_START;
	const int32_t goalDoor = numDoors + DOOR_OFFSET_GOAL;
	//---------------------------------------------
	// Clear the doors and prep 'em for the calc...
	int32_t initHPrime = ZeroHPrime ? 0 : HPRIME_NOT_CALCED;
	for (size_t d = 0; d < numDoors + NUM_DOOR_OFFSETS; d++)
	{
		doors[d].cost = 1;
		doors[d].parent = -1;
		doors[d].fromAreaIndex = -1;
		doors[d].flags = 0;
		doors[d].g = 0;
		doors[d].hPrime = initHPrime;
		doors[d].fPrime = 0;
	}
	setStartDoor(startArea);
	setGoalDoor(goalArea);
	if (areas[startArea].offMap)
		openArea(startArea);
	if (areas[goalArea].offMap)
		openArea(goalArea);
	if (!isGateOpenCallback || !isGateDisabledCallback)
		STOP(("Globalmap.calcPath: nullptr gate callback"));
	for (size_t i = 0; i < numAreas; i++)
		if (areas[i].type == AREA_TYPE_GATE)
		{
			if ((areas[i].teamID == moverTeamID) || (areas[i].teamID == -1))
			{
				if (areas[i].ownerWID > 0)
				{
					if (isGateDisabledCallback(areas[i].ownerWID))
						closeArea(i);
					else
						openArea(i);
				}
				else
					openArea(i);
			}
			else if (isGateOpenCallback(areas[i].ownerWID))
				openArea(i);
			else
				closeArea(i);
		}
	//-------------------------------------------------------------
	// Start with the area we're in, and process the possible doors
	// we can start thru...
	//-----------------------------------------------
	// Put the START vertex on the empty OPEN list...
	PQNode initialVertex;
	initialVertex.key = 0;
	initialVertex.id = startDoor;
	openList->clear();
#ifdef _DEBUG
	int32_t insertErr =
#endif
		openList->insert(initialVertex);
	gosASSERT(insertErr == NO_ERROR);
	doors[startDoor].flags |= MOVEFLAG_OPEN;
	//******************
	// THROW THE STARTING LINKS ON THE QUEUE...
	//******************
	bool goalFound = false;
	while (!openList->isEmpty())
	{
		//----------------------
		// Grab the best node...
		PQNode bestPQNode;
		openList->remove(bestPQNode);
		int32_t bestDoor = bestPQNode.id;
		GlobalMapDoorPtr bestMapDoor = &doors[bestDoor];
		bestMapDoor->flags &= (MOVEFLAG_OPEN ^ 0xFFFFFFFF);
		int32_t bestDoorG = bestMapDoor->g;
		int32_t fromAreaIndex = bestMapDoor->fromAreaIndex;
		//----------------------------
		// Now, close the best node...
		bestMapDoor->flags |= MOVEFLAG_CLOSED;
		//--------------------------
		// Have we found the goal...
		if (bestDoor == goalDoor)
		{
			goalFound = true;
			break;
		}
		//-------------------------------------------
		// Now, check the door links for this door...
		int32_t toAreaIndex = 1 - fromAreaIndex;
		int32_t thruArea = bestMapDoor->area[toAreaIndex];
		int32_t numLinks = bestMapDoor->numLinks[toAreaIndex];
		if (logEnabled)
		{
			wchar_t s[50];
			sprintf(s, "     thruArea = %d, bestDoor = %d, numLinks = %d", thruArea, bestDoor,
				numLinks);
			log->write(s);
		}
		for (size_t curLink = 0; curLink < numLinks; curLink++)
		{
			//------------------------------------------------------
			// If we want to limit the doors thru which the path may
			// travel, we must check right here...
			//--------------------------
			// Now, process this door...
			int32_t succDoor = bestMapDoor->links[toAreaIndex][curLink].doorIndex;
			gosASSERT((succDoor >= 0) && (succDoor < numDoors + NUM_DOOR_OFFSETS));
			GlobalMapDoorPtr succMapDoor = &doors[succDoor];
			if (logEnabled)
			{
				wchar_t s[50];
				sprintf(s, "          %02d) succDoor = %d", curLink, succDoor);
				log->write(s);
			}
			int32_t succDoorCost = bestMapDoor->links[toAreaIndex][curLink].cost;
			//----------------------------------------------------------------------------
			// If this is an aligned door, make it more expensive for unfriendly
			// movers...
			if (useClosedAreas)
			{
				if (succMapDoor->teamID > -1)
					if (TeamRelations[succMapDoor->teamID][moverTeamID] == RELATION_FRIENDLY)
						succDoorCost = 50;
				if (!succMapDoor->open)
					succDoorCost = 1000;
			}
			else
			{
				if ((succMapDoor->teamID > -1) && (succMapDoor->teamID != moverTeamID))
					succDoorCost = COST_BLOCKED;
				//--------------------------------------------------
				// Whether the door is even open is another issue...
				if (!succMapDoor->open)
					succDoorCost = COST_BLOCKED;
			}
			if (logEnabled)
			{
				wchar_t s[50];
				sprintf(s, "                  succDoorCost = %d", succDoorCost);
				log->write(s);
			}
			if (succDoorCost < COST_BLOCKED)
			{
				if (succMapDoor->hPrime == HPRIME_NOT_CALCED)
					succMapDoor->hPrime = calcHPrime(succDoor);
				//----------------------------------------------------
				// What's our cost to go from START to this SUCCESSOR?
				int32_t succDoorG = bestDoorG + succDoorCost;
				int32_t succFromAreaIndex = (succMapDoor->area[1] == thruArea);
				if (succMapDoor->flags & MOVEFLAG_OPEN)
				{
					//----------------------------------------------
					// This node is already in the OPEN queue to be
					// be processed. Let's check if we have a better
					// path thru this route...
					if (succDoorG < succMapDoor->g)
					{
						//----------------------------
						// This new path is cheaper...
						succMapDoor->cost = succDoorCost;
						succMapDoor->parent = bestDoor;
						succMapDoor->fromAreaIndex = succFromAreaIndex;
						succMapDoor->g = succDoorG;
						succMapDoor->fPrime = succDoorG + succMapDoor->hPrime;
						int32_t openIndex = openList->find(succDoor);
						if (!openIndex)
						{
							wchar_t s[128];
							sprintf(s,
								"GlobalMap.calcPath: Cannot find globalmap "
								"door [%d, %d, %d, %d] for change\n",
								succDoor, curLink, succFromAreaIndex, succDoorCost);
#ifdef USE_OBJECTS
							DebugOpenList(s);
#endif
							gosASSERT(openIndex != nullptr);
						}
						openList->change(openIndex, succMapDoor->fPrime);
					}
				}
				else if (succMapDoor->flags & MOVEFLAG_CLOSED)
				{
					//-------------------------------------------------
					// This path may be better than this node's current
					// path. If so, we may have to propogate thru...
					if (succDoorG < succMapDoor->g)
					{
						//----------------------------------
						// This new path is cheaper. We
						// have to propogate the new cost...
						succMapDoor->cost = succDoorCost;
						succMapDoor->parent = bestDoor;
						succMapDoor->fromAreaIndex = succFromAreaIndex;
						propogateCost(succDoor, succDoorCost, succFromAreaIndex, bestDoorG);
					}
				}
				else
				{
					//-------------------------------------------------
					// This node is neither OPEN nor CLOSED, so toss it
					// into the OPEN list...
					succMapDoor->cost = succDoorCost;
					succMapDoor->parent = bestDoor;
					succMapDoor->fromAreaIndex = succFromAreaIndex;
					succMapDoor->g = succDoorG;
					succMapDoor->fPrime = succDoorG + succMapDoor->hPrime;
					PQNode succPQNode;
					succPQNode.key = succMapDoor->fPrime;
					succPQNode.id = succDoor;
#ifdef _DEBUG
					int32_t insertErr =
#endif
						openList->insert(succPQNode);
					gosASSERT(insertErr == NO_ERROR);
					succMapDoor->flags |= MOVEFLAG_OPEN;
				}
			}
		}
	}
	resetStartDoor(startArea);
	resetGoalDoor(goalArea);
	if (areas[startArea].offMap)
		closeArea(startArea);
	if (areas[goalArea].offMap)
		closeArea(goalArea);
	if (goalFound)
	{
		//-------------------------------------------
		// First, let's count how int32_t the path is...
		int32_t curDoor = goalDoor;
		int32_t numSteps = 1;
		while (curDoor != startDoor)
		{
			numSteps++;
			curDoor = doors[curDoor].parent;
		}
		//-----------------------------------------------------------------------
		// Number of doors to travel to, including the "goal door". We don't
		// care about the "start door" in the path...
		int32_t numDoors = numSteps - 1;
		gosASSERT((numDoors < MAX_GLOBAL_PATH));
		//-----------------------------
		// Now, let's build the path...
		curDoor = goalDoor;
		int32_t curPathDoor = numDoors;
		//--------------------------------
		// Do the doors leading to goal...
		int32_t costToGoal = 0;
		while (curPathDoor > 0)
		{
			path[curPathDoor - 1].thruArea = doors[curDoor].area[doors[curDoor].fromAreaIndex];
			path[curPathDoor - 1].goalDoor = curDoor;
			path[curPathDoor - 1].costToGoal = costToGoal;
			costToGoal += doors[curDoor].cost;
			curDoor = doors[curDoor].parent;
			curPathDoor--;
		}
#ifdef _DEBUG
		// systemHeap->walkHeap(false,false,"GlobalMap:calc BAD HEAP2\n");
#endif
#ifdef USE_PATH_COST_TABLE
		if (pathCostTable)
		{
			setPathCost(startArea, goalArea, useClosedAreas, numDoors);
			if (useClosedAreas)
			{
				setPathFlag(startArea, goalArea, GLOBAL_FLAG_SPECIAL_IMPOSSIBLE, false);
				setPathFlag(startArea, goalArea, GLOBAL_FLAG_SPECIAL_CALC, false);
			}
			else
			{
				setPathFlag(startArea, goalArea, GLOBAL_FLAG_NORMAL_OPENS, false);
				setPathFlag(startArea, goalArea, GLOBAL_FLAG_SPECIAL_CALC, false);
			}
		}
#endif
		if (logEnabled)
		{
			wchar_t s[50];
			sprintf(s, "     PATH FOUND: %d steps", numDoors);
			log->write(s);
			log->write(" ");
		}
		return (numDoors);
	}
	else
	{
#ifdef USE_PATH_COST_TABLE
		if (pathCostTable)
		{
			setPathCost(startArea, goalArea, useClosedAreas, 0);
			if (useClosedAreas)
			{
				setPathFlag(startArea, goalArea, GLOBAL_FLAG_SPECIAL_IMPOSSIBLE, true);
				setPathFlag(startArea, goalArea, GLOBAL_FLAG_SPECIAL_CALC, false);
			}
			else
			{
				setPathFlag(startArea, goalArea, GLOBAL_FLAG_NORMAL_OPENS, false);
				setPathFlag(startArea, goalArea, GLOBAL_FLAG_SPECIAL_CALC, false);
			}
		}
#endif
		if (logEnabled)
			log->write("     NO PATH FOUND");
	}
	if (logEnabled)
		log->write(" ");
	return (0);
}

//------------------------------------------------------------------------------------------

int32_t
GlobalMap::calcPath(Stuff::Vector3D start, Stuff::Vector3D goal, GlobalPathStepPtr path)
{
	int32_t startR, startC;
	land->worldToCell(start, startR, startC);
	int32_t goalR, goalC;
	land->worldToCell(goal, goalR, goalC);
	int32_t numSteps = calcPath(
		calcArea(startR, startC), calcArea(goalR, goalC), path, startR, startC, goalR, goalC);
	return (numSteps);
}

//---------------------------------------------------------------------------

void GlobalMap::openDoor(int32_t door)
{
	doors[door].cost = 10;
	doors[door].open = true;
}

//---------------------------------------------------------------------------

void GlobalMap::closeDoor(int32_t door)
{
	doors[door].open = false;
}

//---------------------------------------------------------------------------

void GlobalMap::closeArea(int32_t area)
{
	if (area < 0)
		return;
	GlobalMapAreaPtr closedArea = &areas[area];
	closedArea->open = false;
	for (size_t d = 0; d < closedArea->numDoors; d++)
		closeDoor(closedArea->doors[d].doorIndex);
	for (size_t i = 0; i < closedArea->numDoors; i++)
	{
		GlobalMapDoorPtr curDoor = &doors[closedArea->doors[i].doorIndex];
		int32_t doorSide = closedArea->doors[i].doorSide;
		for (size_t j = 0; j < curDoor->numLinks[doorSide]; j++)
			curDoor->links[doorSide][j].cost = 1000;
	}
	closes = true;
}

//---------------------------------------------------------------------------

void GlobalMap::openArea(int32_t area)
{
	if (area < 0)
		return;
	GlobalMapAreaPtr openedArea = &areas[area];
	openedArea->open = true;
	for (size_t d = 0; d < openedArea->numDoors; d++)
	{
		int32_t areaSide1 = doors[openedArea->doors[d].doorIndex].area[0];
		int32_t areaSide2 = doors[openedArea->doors[d].doorIndex].area[1];
		if (!isClosedArea(areaSide1) && !isClosedArea(areaSide2))
			openDoor(openedArea->doors[d].doorIndex);
	}
	for (size_t i = 0; i < openedArea->numDoors; i++)
	{
		GlobalMapDoorPtr curDoor = &doors[openedArea->doors[i].doorIndex];
		int32_t doorSide = openedArea->doors[i].doorSide;
		for (size_t j = 0; j < curDoor->numLinks[doorSide]; j++)
			curDoor->links[doorSide][j].cost = curDoor->links[doorSide][j].openCost;
	}
	opens = true;
}

//---------------------------------------------------------------------------

void GlobalMap::print(std::wstring_view fileName)
{
	if (areaMap)
		return;
	MechFile* debugFile = new MechFile;
	debugFile->create(fileName);
	wchar_t outString[500];
	for (size_t row = 0; row < height; row++)
	{
		outString[0] = nullptr;
		for (size_t col = 0; col < width; col++)
		{
			if (areaMap[row * width + col] == -2)
				strcat(outString, ">< ");
			else if (areaMap[row * width + col] == -1)
				strcat(outString, "** ");
			else
			{
				wchar_t chStr[8];
				sprintf(chStr, "%02x ", areaMap[row * width + col]);
				strcat(outString, chStr);
			}
		}
		strcat(outString, "\n");
		debugFile->writeString(outString);
	}
	debugFile->writeString("\n");
	debugFile->close();
	delete debugFile;
	debugFile = nullptr;
}

//---------------------------------------------------------------------------

void GlobalMap::destroy(void)
{
#ifdef _DEBUG
//	systemHeap->walkHeap(false,false,"GlobalMap BAD HEAP1\n");
#endif
	if (areaMap)
	{
		systemHeap->Free(areaMap);
		areaMap = nullptr;
	}
	if (areas)
	{
		if (!doorInfos)
		{
			for (size_t i = 0; i < (numAreas + 1); i++)
			{
				if (areas[i].cellsCovered)
				{
					systemHeap->Free(areas[i].cellsCovered);
					areas[i].cellsCovered = nullptr;
				}
				if (areas[i].doors)
				{
					systemHeap->Free(areas[i].doors);
					areas[i].doors = nullptr;
				}
			}
		}
		systemHeap->Free(areas);
		areas = nullptr;
	}
	if (doors)
	{
		if (!doorLinks)
		{
			for (size_t i = 0; i < (numDoors + NUM_DOOR_OFFSETS); i++)
				for (size_t s = 0; s < 2; s++)
				{
					if (doors[i].links[s])
					{
						systemHeap->Free(doors[i].links[s]);
						doors[i].links[s] = nullptr;
					}
				}
		}
		systemHeap->Free(doors);
		doors = nullptr;
	}
	if (doorInfos)
	{
		systemHeap->Free(doorInfos);
		doorInfos = nullptr;
	}
	if (doorLinks)
	{
		systemHeap->Free(doorLinks);
		doorLinks = nullptr;
	}
	if (pathExistsTable)
	{
		systemHeap->Free(pathExistsTable);
		pathExistsTable = nullptr;
	}
#ifdef USE_PATH_COST_TABLE
	if (pathCostTable)
	{
		systemHeap->Free(pathCostTable);
		pathCostTable = nullptr;
	}
#endif
}

//----------------------------------------------------------------------------------

bool GlobalMap::toggleLog(void)
{
	int32_t i;
	if (!log)
	{
		GameLog::setup();
		log = GameLog::getNewFile();
		if (!log)
			Fatal(0, " Couldn't create lrmove log ");
		int32_t err = log->open("lrmove.log");
		if (err)
			Fatal(0, " Couldn't open lrmove log ");
#if 1
		GlobalMapPtr map = GlobalMoveMap[0];
		if (true)
		{
			wchar_t s[256];
			for (i = 0; i < map->numDoors; i++)
			{
				sprintf(s, "door %05d, %s(%d), areas %d & %d", i,
					map->doors[i].open ? "opened" : "CLOSED", map->doors[i].teamID,
					map->doors[i].area[0], map->doors[i].area[1]);
				log->write(s);
				for (size_t side = 0; side < 2; side++)
				{
					sprintf(s, "     side %d", side);
					log->write(s);
					for (size_t j = 0; j < map->doors[i].numLinks[side]; j++)
					{
						sprintf(s, "          link %03d, to door %05d, cost %d", j,
							map->doors[i].links[side][j].doorIndex,
							map->doors[i].links[side][j].cost);
						log->write(s);
					}
				}
			}
			log->write(" ");
			for (i = 0; i < map->numAreas; i++)
			{
				sprintf(s, "area %05d, %s(%d)", i, map->areas[i].open ? "opened" : "CLOSED",
					map->areas[i].teamID);
				log->write(s);
				if (map->areas[i].ownerWID > 0)
				{
					sprintf(s, "     ownerWID is %d", map->areas[i].ownerWID);
					log->write(s);
				}
				static std::wstring_view typeString[] = {
					"normal area", "wall area", "gate area", "land bridge area", "forest area"};
				sprintf(s, "     %s", typeString[map->areas[i].type]);
				log->write(s);
				for (size_t d = 0; d < map->areas[i].numDoors; d++)
				{
					sprintf(s, "     door %03d is %d (%d:%d & %d) ", d,
						map->areas[i].doors[d].doorIndex, map->areas[i].doors[d].doorSide,
						map->doors[map->areas[i].doors[d].doorIndex].area[0],
						map->doors[map->areas[i].doors[d].doorIndex].area[1]);
					log->write(s);
				}
			}
			log->write(" ");
		}
#endif
	}
	logEnabled = !logEnabled;
	return (logEnabled);
}

//----------------------------------------------------------------------------------

void GlobalMap::writeLog(std::wstring_view s)
{
	if (log)
		log->write(s);
}

//**********************************************************************************
// MOVE MAP class
//**********************************************************************************

PVOID
MoveMap::operator new(size_t ourSize)
{
	PVOID result = systemHeap->Malloc(ourSize);
	return (result);
}

//---------------------------------------------------------------------------

void MoveMap::operator delete(PVOID us)
{
	systemHeap->Free(us);
}

//---------------------------------------------------------------------------

void MoveMap::init(int32_t maxW, int32_t maxH)
{
	width = maxwidth = maxW;
	height = maxheight = maxH;
	int32_t mapByteSize = sizeof(MoveMapNode) * maxwidth * maxheight;
	map = (MoveMapNodePtr)systemHeap->Malloc(mapByteSize);
	gosASSERT(map != nullptr);
	mapRowStartTable = (int32_t*)systemHeap->Malloc(maxheight * sizeof(int32_t));
	gosASSERT(mapRowStartTable != nullptr);
	int32_t r;
	for (r = 0; r < maxheight; r++)
		mapRowStartTable[r] = r * maxwidth;
	mapRowTable = (int32_t*)systemHeap->Malloc(maxheight * maxwidth * sizeof(int32_t));
	gosASSERT(mapRowTable != nullptr);
	mapColTable = (int32_t*)systemHeap->Malloc(maxheight * maxwidth * sizeof(int32_t));
	gosASSERT(mapColTable != nullptr);
	for (r = 0; r < maxheight; r++)
		for (size_t c = 0; c < maxwidth; c++)
		{
			int32_t index = mapRowStartTable[r] + c;
			mapRowTable[index] = r;
			mapColTable[index] = c;
		}
	for (r = 0; r < maxheight; r++)
		for (size_t c = 0; c < maxwidth; c++)
		{
			int32_t mapCellIndex = r * maxwidth + c;
			for (size_t d = 0; d < NUM_ADJ_CELLS; d++)
			{
				int32_t indexStart = d * 2;
				int32_t adjRow = r + cellShift[indexStart];
				int32_t adjCol = c + cellShift[indexStart + 1];
				if (inMapBounds(adjRow, adjCol, height, width))
					map[mapCellIndex].adjCells[d] = adjRow * maxwidth + adjCol;
				else
					map[mapCellIndex].adjCells[d] = -1;
			}
		}
	float cellLength = Terrain::worldUnitsPerCell * metersPerWorldUnit;
	for (size_t i = 0; i < DISTANCE_TABLE_DIM; i++)
		for (size_t j = 0; j < DISTANCE_TABLE_DIM; j++)
		{
			distanceFloat[i][j] = agsqrt(i, j) * cellLength;
			distanceLong[i][j] = (int32_t)distanceFloat[i][j];
		}
	clear();
}

//---------------------------------------------------------------------------

void MoveMap::clear(void)
{
	int32_t numMapCells = maxwidth * height;
	int32_t initHPrime = ZeroHPrime ? 0 : HPRIME_NOT_CALCED;
	for (size_t i = 0; i < numMapCells; i++)
	{
		MoveMapNodePtr node = &map[i];
		//------------------------------------------------
		// DON'T NEED TO SET THIS SINCE IS SET IN SETUP...
		node->cost = COST_BLOCKED;
		node->parent = -1;
		node->flags = 0;
		node->hPrime = initHPrime;
	}
	goal.Zero();
	target.x = -999999.0;
	target.y = -999999.0;
	target.z = -999999.0;
}

//---------------------------------------------------------------------------

void MoveMap::setTarget(Stuff::Vector3D targetPos)
{
	target = targetPos;
}

//---------------------------------------------------------------------------

void MoveMap::setStart(Stuff::Vector3D* startPos, int32_t startRow, int32_t startCol)
{
	if (startPos)
		start = *startPos;
	else
	{
		start.x = -999999.0;
		start.y = -999999.0;
		start.z = -999999.0;
	}
	if (startRow == -1)
	{
		land->worldToCell(*startPos, startR, startC);
		startR -= ULr;
		startC -= ULc;
	}
	else
	{
		startR = startRow;
		Assert(startRow > -1, 0, " HUH ");
		startC = startCol;
	}
}

//---------------------------------------------------------------------------

void MoveMap::setGoal(Stuff::Vector3D goalPos, int32_t goalRow, int32_t goalCol)
{
	goal = goalPos;
	if (goalRow == -1)
	{
		land->worldToCell(goal, goalR, goalC);
		goalR -= ULr;
		goalC -= ULc;
	}
	else
	{
		goalR = goalRow;
		goalC = goalCol;
	}
	doorDirection = -1;
	GoalIsDoor = false;
}

//---------------------------------------------------------------------------

inline void
adjustMoveMapCellCost(MoveMapNodePtr cell, int32_t costAdj)
{
	int32_t cost = cell->cost + costAdj;
	if (cost < 1)
		cost = 1;
	cell->cost = cost;
}

//---------------------------------------------------------------------------

void MoveMap::setGoal(int32_t thruArea, int32_t goalDoor)
{
	//------------------------------------------------------------------------------
	// We need to set goalR, goalC for the calcHPrime routine (until we come up
	// with a better one that uses the door range. If the global path's final
	// goal is on the other side of this door, we should select THAT cell of the
	// door...
	goal.x = -999999.0;
	goal.y = -999999.0;
	goal.z = -999999.0;
	GoalIsDoor = true;
	door = goalDoor;
	doorSide = (GlobalMoveMap[moveLevel]->doors[goalDoor].area[1] == thruArea);
	int32_t doorDirTable[4][2] = {{-1, -1}, {1, 3}, {2, 0}, {-1, -1}};
	int32_t goalDoorDir = GlobalMoveMap[moveLevel]->doors[goalDoor].direction[0];
	gosASSERT((goalDoorDir == 1) || (goalDoorDir == 2));
	doorDirection = doorDirTable[goalDoorDir][doorSide];
	int32_t doorLength = GlobalMoveMap[moveLevel]->doors[goalDoor].length;
	if ((doorDirection == 0) || (doorDirection == 2))
	{
		//----------------------------------------------------------------
		// We need to set goalR and goalC for the calcHPrime function used
		// in pathfinding...
		goalR = GlobalMoveMap[moveLevel]->doors[goalDoor].row + doorSide - ULr;
		goalC = GlobalMoveMap[moveLevel]->doors[goalDoor].col + (doorLength / 2) - ULc;
	}
	else if ((doorDirection == 1) || (doorDirection == 3))
	{
		//----------------------------------------------------------------
		// We need to set goalR and goalC for the calcHPrime function used
		// in pathfinding...
		goalR = GlobalMoveMap[moveLevel]->doors[goalDoor].row + (doorLength / 2) - ULr;
		goalC = GlobalMoveMap[moveLevel]->doors[goalDoor].col + doorSide - ULc;
	}
}

//---------------------------------------------------------------------------

inline int32_t
MoveMap::markGoals(Stuff::Vector3D finalGoal)
{
	//--------------------------------------
	// First, mark the blocked goal cells...
	static wchar_t doorCellState[1024];
	for (size_t j = 0; j < GlobalMoveMap[moveLevel]->doors[door].length; j++)
		doorCellState[j] = 1;
	if (blockedDoorCallback)
		(*blockedDoorCallback)(moveLevel, door, &doorCellState[0]);
	//-------------------------------------------------------------------------
	// Ultimately, we should do this conversion once for the finalGoal and then
	// store it in the moveOrders data...
	int32_t finalGoalR, finalGoalC;
	land->worldToCell(finalGoal, finalGoalR, finalGoalC);
	//-----------------------------------------
	// Localize the coords for this move map...
	finalGoalR -= ULr;
	finalGoalC -= ULc;
	int32_t doorLength = GlobalMoveMap[moveLevel]->doors[door].length;
	int32_t numGoalCells = 0;
	if ((doorDirection == 0) || (doorDirection == 2))
	{
		//--------------------------------
		// Mark the door cells as goals...
		int32_t cellR = goalR;
		int32_t cellC = goalC - (doorLength / 2);
		bool nextToGoal = false;
		if (doorSide == 0)
		{
			if (finalGoalR == (cellR + 1))
				//				if (!doorCellBlocked(cellR, cellC))
				if ((finalGoalC >= cellC) && (finalGoalC < (cellC + doorLength)))
				{
					//-----------------------------------------------------
					// Our global goal is on the other side of this door...
					map[cellR * width + finalGoalC].setFlag(MOVEFLAG_GOAL);
					numGoalCells++;
					nextToGoal = true;
				}
		}
		else
		{
			if (finalGoalR == (cellR - 1))
				//				if (!doorCellBlocked(cellR, cellC)
				if ((finalGoalC >= cellC) && (finalGoalC < (cellC + doorLength)))
				{
					//-----------------------------------------------------
					// Our global goal is on the other side of this door...
					map[cellR * maxwidth + finalGoalC].setFlag(MOVEFLAG_GOAL);
					numGoalCells++;
					nextToGoal = true;
				}
		}
		int32_t c;
		if (!nextToGoal)
		{
			int32_t adjCost = clearCost / 2;
			int32_t doorCenter = doorLength / 2;
			int32_t cellIndex = cellR * maxwidth + cellC;
			int32_t curCost = doorCenter * adjCost;
			for (c = 0; c < doorCenter; c++)
			{
				if (doorCellState[c])
				{
					map[cellIndex].setFlag(MOVEFLAG_GOAL);
					numGoalCells++;
					adjustMoveMapCellCost(&map[cellIndex], curCost);
				}
				cellIndex++;
				curCost -= adjCost;
			}
			cellIndex = cellR * maxwidth + cellC + doorCenter;
			curCost = 0;
			for (c = doorCenter; c < doorLength; c++)
			{
				if (doorCellState[c])
				{
					map[cellIndex].setFlag(MOVEFLAG_GOAL);
					numGoalCells++;
					adjustMoveMapCellCost(&map[cellIndex], curCost);
				}
				cellIndex++;
				curCost += adjCost;
			}
		}
	}
	else if ((doorDirection == 1) || (doorDirection == 3))
	{
		int32_t cellR = goalR - (doorLength / 2);
		int32_t cellC = goalC;
		bool nextToGoal = false;
		if (finalGoalC == (cellC + 1 - doorSide * 2))
			if ((finalGoalR >= cellR) && (finalGoalR < (cellR + doorLength)))
			{
				//-----------------------------------------------------
				// Our global goal is on the other side of this door...
				map[finalGoalR * maxwidth + cellC].setFlag(MOVEFLAG_GOAL);
				numGoalCells++;
				nextToGoal = true;
			}
		int32_t r;
		if (!nextToGoal)
		{
			int32_t adjCost = clearCost / 2;
			int32_t doorCenter = doorLength / 2;
			int32_t cellIndex = cellR * maxwidth + cellC;
			int32_t curCost = doorCenter * adjCost;
			for (r = 0; r < doorCenter; r++)
			{
				if (doorCellState[r])
				{
					map[cellIndex].setFlag(MOVEFLAG_GOAL);
					numGoalCells++;
					adjustMoveMapCellCost(&map[cellIndex], curCost);
				}
				cellIndex += maxwidth;
				curCost -= adjCost;
			}
			curCost = 0;
			cellIndex = (cellR + doorCenter) * maxwidth + cellC;
			for (r = doorCenter; r < doorLength; r++)
			{
				if (doorCellState[r])
				{
					map[cellIndex].setFlag(MOVEFLAG_GOAL);
					numGoalCells++;
					adjustMoveMapCellCost(&map[cellIndex], curCost);
				}
				cellIndex += maxwidth;
				curCost += adjCost;
			}
		}
	}
	//	if (numGoalCells == 0)
	//		OutputDebugString("No Goal Cells");
	return (numGoalCells);
}

//---------------------------------------------------------------------------

inline int32_t
MoveMap::markEscapeGoals(Stuff::Vector3D finalGoal)
{
	int32_t numGoalCells = 0;
	//-------------------------------------------------------------------------
	// Ultimately, we should do this conversion once for the finalGoal and then
	// store it in the moveOrders data...
	int32_t finalGoalR, finalGoalC;
	land->worldToCell(finalGoal, finalGoalR, finalGoalC);
	int32_t finalGoalArea = GlobalMoveMap[moveLevel]->calcArea(finalGoalR, finalGoalC);
	//------------------------------------------------------------------------
	// For each tile, mark its cells as valid goals if:
	//		1) the tile's areaId == the areaId of the finalGoal
	//		2) OR, if a LR path exists between the tile and the finalGoal tile
	int32_t cellIndex = 0;
	for (size_t row = 0; row < height; row++)
		for (size_t col = 0; col < width; col++)
		{
			if (GameMap->inBounds(ULr + row, ULc + col))
			{
				int32_t curArea = GlobalMoveMap[moveLevel]->calcArea(ULr + row, ULc + col);
				int32_t confidence;
				int32_t numLRSteps = GlobalMoveMap[moveLevel]->getPathCost(
					curArea, finalGoalArea, false, confidence, true);
				bool validGoal = (numLRSteps > 0);
				if (validGoal)
					map[cellIndex].setFlag(MOVEFLAG_GOAL);
			}
			cellIndex++;
		}
	return (numGoalCells);
}

//---------------------------------------------------------------------------

int32_t
MoveMap::setUp(int32_t mapULr, int32_t mapULc, int32_t mapwidth, int32_t mapheight,
	int32_t level, Stuff::Vector3D* startPos, int32_t startRow, int32_t startCol,
	Stuff::Vector3D goalPos, int32_t goalRow, int32_t goalCol, int32_t clearCellCost,
	int32_t jumpCellCost, int32_t offsets, uint32_t params)
{
	//-----------------------------------------------------------------------------
	// If the map has not been allocated yet, then the tile height and width
	// passed is used as both the max and current dimensions. Otherwise, they
	// are only used as the current dimensions...
	if ((params & MOVEPARAM_JUMP) && (jumpCellCost > 0))
		jumpCellCost = clearCellCost;
	if (!map)
	{
		init(mapheight, mapwidth);
		setClearCost(clearCellCost);
		setJumpCost(jumpCellCost, offsets);
	}
	else
	{
		width = mapwidth;
		height = mapheight;
		setClearCost(clearCellCost);
		setJumpCost(jumpCellCost, offsets);
		clear();
	}
	thruAreas[0] = -1;
	thruAreas[1] = -1;
	//---------------------------------------------
	// First, gotta set some params for the calc...
	ULr = mapULr;
	ULc = mapULc;
	minRow = 0;
	minCol = 0;
	maxRow = mapheight - 1;
	maxCol = mapwidth - 1;
	moveLevel = level;
	startRow -= ULr;
	startCol -= ULc;
	Assert(startRow > -1, 0, " huh ");
	setStart(startPos, startRow, startCol);
	if (FindingEscapePath)
	{
		setGoal(goalPos, goalRow, goalCol);
	}
	else
		setGoal(goalPos, goalRow, goalCol);
	travelOffMap = false;
	cannotEnterOffMap = true;
	if (GameMap->getOffMap(ULr + startRow, ULc + startCol))
		travelOffMap = true;
	if (moverWithdrawing)
	{
		travelOffMap = true;
		cannotEnterOffMap = false;
	}
	bool followRoads = ((params & MOVEPARAM_FOLLOW_ROADS) != 0);
	bool traverseShallowWater = ((params & (MOVEPARAM_WATER_SHALLOW + MOVEPARAM_WATER_DEEP)) != 0);
	bool traverseDeepWater = ((params & MOVEPARAM_WATER_DEEP) != 0);
	bool avoidMines = true;
	if (params & MOVEPARAM_SWEEP_MINES)
		avoidMines = false;
	//-------------------------------------------------
	// Now that the params are set up, build the map...
	int32_t pathLockCost = clearCost << 3;
	//--------------------------------------------------------------
	// Set the map costs based upon the tiles in the scenario map...
	bool groundMover = ((moveLevel == 0) || (moveLevel == 1));
	for (size_t cellRow = 0; cellRow < height; cellRow++)
		for (size_t cellCol = 0; cellCol < width; cellCol++)
		{
			if (GameMap->inBounds(ULr + cellRow, ULc + cellCol))
			{
				MapCellPtr mapCell = GameMap->getCell(ULr + cellRow, ULc + cellCol);
				int32_t moveMapIndex = cellRow * maxwidth + cellCol;
				int32_t cost = clearCost;
				bool offMapCell = mapCell->getOffMap();
				if (offMapCell)
					map[moveMapIndex].setFlag(MOVEFLAG_OFFMAP);
				//-----------------------
				// Tile (terrain) type...
				// int32_t tileType = curTile.getTileType();
				if (offMapCell && !travelOffMap)
					cost = COST_BLOCKED;
				else if (groundMover)
				{
					if (mapCell->getShallowWater())
					{
						if (!traverseShallowWater)
							cost = COST_BLOCKED;
						else if (!mapCell->getPassable())
							cost = COST_BLOCKED;
					}
					else if (mapCell->getDeepWater())
					{
						if (!traverseDeepWater)
							cost = COST_BLOCKED;
						else if (!mapCell->getPassable())
							cost = COST_BLOCKED;
					}
					else if (mapCell->getGate())
					{
						int32_t areaID =
							GlobalMoveMap[moveLevel]->calcArea(ULr + cellRow, ULc + cellCol);
						int32_t teamID = -1;
						if (areaID > -1)
						{
							teamID = GlobalMoveMap[moveLevel]->areas[areaID].teamID;
							// Its possible for the ownerWIDs to be invalid for
							// one or two frames after a quick save.
							// We handle this ok later on!!  OwnerWIDs
							// restablish themselves right after the first
							// unpaused update!
							/*
							if (!EditorSave)
								if
							(GlobalMoveMap[moveLevel]->areas[areaID].ownerWID <
							1) PAUSE(("Gate has no ownerWID.  CellR: %d, CellC:
							%d, WID: %d, MoveLvl:
							%d",cellRow,cellCol,GlobalMoveMap[moveLevel]->areas[areaID].ownerWID,moveLevel));
							*/
						}
						if (!EditorSave && (areaID > -1) && GlobalMoveMap[moveLevel]->isGateDisabledCallback(GlobalMoveMap[moveLevel]->areas[areaID].ownerWID))
							cost = COST_BLOCKED;
						else if ((teamID > -1) && (TeamRelations[teamID][moverTeamID] != RELATION_FRIENDLY))
						{
							if (mapCell->getPassable())
								cost <<= 2;
							else
								cost = COST_BLOCKED;
						}
						else if (mapCell->getRoad() && followRoads)
							cost >>= 2;
					}
					else
					{
						if (!mapCell->getPassable())
							cost = COST_BLOCKED;
						else if (mapCell->getRoad() && followRoads)
							cost >>= 2;
					}
					if (mapCell->getForest())
						cost += forestCost;
				}
				map[moveMapIndex].cost = cost;
				//---------------------------------------------------------------
				// NOTE: With gates, we may want them to set the cell cost
				// rather than just adjust it. Let's see how they play. Since
				// they're set as an overlay, we'll just treat them as such for
				// now.
				if (mapCell->getPathlock(moveLevel == 2))
					adjustMoveMapCellCost(&map[cellRow * maxwidth + cellCol], pathLockCost);
			}
		}
	if (FindingEscapePath)
		markEscapeGoals(goalPos);
	else
		map[goalR * maxwidth + goalC].setFlag(MOVEFLAG_GOAL);
#ifdef LAB_ONLY
	int64_t startTime = GetCycles();
#endif
	if (params & MOVEPARAM_STATIONARY_MOVERS)
		if (placeStationaryMoversCallback)
			(*placeStationaryMoversCallback)(this);
#ifdef LAB_ONLY
	MCTimeCalcPath1Update += (GetCycles() - startTime);
#endif
	return (NO_ERROR);
}

//---------------------------------------------------------------------------

int32_t
MoveMap::setUp(int32_t level, Stuff::Vector3D* startPos, int32_t startRow, int32_t startCol,
	int32_t thruArea[2], int32_t goalDoor, Stuff::Vector3D finalGoal, int32_t clearCellCost,
	int32_t jumpCellCost, int32_t offsets, uint32_t params)
{
#ifdef LAB_ONLY
	int64_t startTime = GetCycles();
#endif
	//-----------------------------------------------------------------------------
	// If the map has not been allocated yet, then the tile height and width
	// passed is used as both the max and current dimensions. Otherwise, they
	// are only used as the current dimensions...
	if (!map)
		init(SECTOR_DIM * 2, SECTOR_DIM * 2);
	else
	{
		width = SECTOR_DIM * 2;
		height = SECTOR_DIM * 2;
		clear();
	}
	thruAreas[0] = thruArea[0];
	thruAreas[1] = thruArea[1];
	//---------------------------------------------
	// First, gotta set some params for the calc...
	int32_t firstSectorRow = GlobalMoveMap[level]->areas[thruArea[0]].sectorR;
	int32_t firstSectorCol = GlobalMoveMap[level]->areas[thruArea[0]].sectorC;
	if (thruArea[1] == -1)
	{
		ULr = firstSectorRow * SECTOR_DIM;
		ULc = firstSectorCol * SECTOR_DIM;
		minRow = 0;
		minCol = 0;
		maxRow = SECTOR_DIM - 1;
		maxCol = SECTOR_DIM - 1;
	}
	else
	{
		int32_t secondSectorRow = GlobalMoveMap[level]->areas[thruArea[1]].sectorR;
		int32_t secondSectorCol = GlobalMoveMap[level]->areas[thruArea[1]].sectorC;
		if (secondSectorRow > firstSectorRow)
		{
			if (secondSectorCol == firstSectorCol)
			{
				ULr = firstSectorRow * SECTOR_DIM;
				ULc = firstSectorCol * SECTOR_DIM;
				minRow = 0;
				minCol = 0;
				maxRow = SECTOR_DIM * 2 - 1;
				maxCol = SECTOR_DIM - 1;
			}
			else
				STOP(("MoveMap.setup: diagonal sectors not allowed #1"));
		}
		else if (secondSectorRow < firstSectorRow)
		{
			if (secondSectorCol == firstSectorCol)
			{
				ULr = secondSectorRow * SECTOR_DIM;
				ULc = secondSectorCol * SECTOR_DIM;
				minRow = 0;
				minCol = 0;
				maxRow = SECTOR_DIM * 2 - 1;
				maxCol = SECTOR_DIM - 1;
			}
			else
				STOP(("MoveMap.setup: diagonal sectors not allowed #2"));
		}
		else
		{
			if (secondSectorCol > firstSectorCol)
			{
				ULr = firstSectorRow * SECTOR_DIM;
				ULc = firstSectorCol * SECTOR_DIM;
				minRow = 0;
				minCol = 0;
				maxRow = SECTOR_DIM - 1;
				maxCol = SECTOR_DIM * 2 - 1;
			}
			else if (secondSectorCol < firstSectorCol)
			{
				ULr = secondSectorRow * SECTOR_DIM;
				ULc = secondSectorCol * SECTOR_DIM;
				minRow = 0;
				minCol = 0;
				maxRow = SECTOR_DIM - 1;
				maxCol = SECTOR_DIM * 2 - 1;
			}
			else
			{
				ULr = firstSectorRow * SECTOR_DIM;
				ULc = firstSectorCol * SECTOR_DIM;
				minRow = 0;
				minCol = 0;
				maxRow = SECTOR_DIM - 1;
				maxCol = SECTOR_DIM - 1;
				// STOP(("MoveMap.setup: same sectors not allowed"));
			}
		}
	}
	moveLevel = level;
	setClearCost(clearCellCost);
	if ((params & MOVEPARAM_JUMP) && (jumpCellCost > 0))
		jumpCellCost = clearCellCost;
	setJumpCost(jumpCellCost, offsets);
	startRow -= ULr;
	startCol -= ULc;
	setStart(startPos, startRow, startCol);
	if (thruArea[1] == -1)
		setGoal(thruArea[0], goalDoor);
	else
		setGoal(thruArea[1], goalDoor);
	int32_t pathLockCost = clearCost << 3;
	travelOffMap = false;
	cannotEnterOffMap = true;
	if (GameMap->getOffMap(ULr + startRow, ULc + startCol))
		travelOffMap = true;
	if (moverWithdrawing)
	{
		travelOffMap = true;
		cannotEnterOffMap = false;
	}
	//-------------------------------------------------
	// Now that the params are set up, build the map...
	bool followRoads = ((params & MOVEPARAM_FOLLOW_ROADS) != 0);
	bool traverseShallowWater = ((params & (MOVEPARAM_WATER_SHALLOW + MOVEPARAM_WATER_DEEP)) != 0);
	bool traverseDeepWater = ((params & MOVEPARAM_WATER_DEEP) != 0);
	bool avoidMines = true;
	if (params & MOVEPARAM_SWEEP_MINES)
		avoidMines = false;
	//--------------------------------------------------------------
	// Set the map costs based upon the tiles in the scenario map...
	bool groundMover = ((moveLevel == 0) || (moveLevel == 1));
	for (size_t cellRow = 0; cellRow < height; cellRow++)
		for (size_t cellCol = 0; cellCol < width; cellCol++)
		{
			if (GameMap->inBounds(ULr + cellRow, ULc + cellCol) && inBounds(cellRow, cellCol))
			{
				MapCellPtr mapCell = GameMap->getCell(ULr + cellRow, ULc + cellCol);
				int32_t moveMapIndex = cellRow * maxwidth + cellCol;
				int32_t cost = clearCost;
				bool offMapCell = mapCell->getOffMap();
				if (offMapCell)
					map[moveMapIndex].setFlag(MOVEFLAG_OFFMAP);
				int32_t areaID = GlobalMoveMap[moveLevel]->calcArea(ULr + cellRow, ULc + cellCol);
				if (CullPathAreas && (areaID != thruAreas[0]) && (areaID != thruAreas[1]))
					cost = COST_BLOCKED;
				else if (offMapCell && !travelOffMap)
					cost = COST_BLOCKED;
				else if (groundMover)
				{
					if (mapCell->getShallowWater())
					{
						if (!traverseShallowWater)
							cost = COST_BLOCKED;
						else if (!mapCell->getPassable())
							cost = COST_BLOCKED;
					}
					else if (mapCell->getDeepWater())
					{
						if (!traverseDeepWater)
							cost = COST_BLOCKED;
						else if (!mapCell->getPassable())
							cost = COST_BLOCKED;
					}
					else if (mapCell->getGate())
					{
						int32_t areaID =
							GlobalMoveMap[moveLevel]->calcArea(ULr + cellRow, ULc + cellCol);
						int32_t teamID = -1;
						if (areaID > -1)
						{
							teamID = GlobalMoveMap[moveLevel]->areas[areaID].teamID;
							/*  See Above
							if (!EditorSave)
								if
							(GlobalMoveMap[moveLevel]->areas[areaID].ownerWID <
							1) PAUSE(("Gate has no ownerWID"));
							*/
						}
						if (!EditorSave && (areaID > -1) && GlobalMoveMap[moveLevel]->isGateDisabledCallback(GlobalMoveMap[moveLevel]->areas[areaID].ownerWID))
							cost = COST_BLOCKED;
						else if ((teamID > -1) && (TeamRelations[teamID][moverTeamID] != RELATION_FRIENDLY))
						{
							if (mapCell->getPassable())
								cost <<= 2;
							else
								cost = COST_BLOCKED;
						}
						else if (mapCell->getRoad() && followRoads)
							cost >>= 2;
					}
					else
					{
						if (!mapCell->getPassable())
							cost = COST_BLOCKED;
						else if (mapCell->getRoad() && followRoads)
							cost >>= 2;
					}
					if (mapCell->getForest())
						cost += forestCost;
				}
				setCost(cellRow, cellCol, cost);
				if (mapCell->getPathlock(moveLevel == 2))
					adjustMoveMapCellCost(&map[cellRow * maxwidth + cellCol], pathLockCost);
			}
		}
	if (markGoals(finalGoal) == 0)
		return (-1);
	if (params & MOVEPARAM_STATIONARY_MOVERS)
		if (placeStationaryMoversCallback)
			(*placeStationaryMoversCallback)(this);
#ifdef LAB_ONLY
	MCTimeCalcPath2Update += (GetCycles() - startTime);
#endif
	return (NO_ERROR);
}

//---------------------------------------------------------------------------

inline bool
MoveMap::adjacentCellOpen(int32_t mapCellIndex, int32_t dir)
{
	int32_t adjCellIndex = map[mapCellIndex].adjCells[dir];
	if (adjCellIndex == -1)
		return (false);
	if (map[adjCellIndex].flags & MOVEFLAG_MOVER_HERE)
		return (false);
#ifdef USE_MINES_IN_MC2
	if (moverRelation == RELATION_ENEMY)
	{
		if (GameMap->getTile(adjRow, adjCol).getClanMineMove())
			return (false);
	}
	else
	{
		if (GameMap->getTile(adjRow, adjCol).getInnerSphereMineMove())
			return (false);
	}
#endif
	return (map[adjCellIndex].cost < COST_BLOCKED);
}

//---------------------------------------------------------------------------

inline bool
MoveMap::adjacentCellOpenJUMP(int32_t r, int32_t c, int32_t dir)
{
	int32_t indexStart = dir * 2;
	int32_t adjRow = r + cellShift[indexStart];
	int32_t adjCol = c + cellShift[indexStart + 1];
	if (!inMapBounds(adjRow, adjCol, height, width))
		return (false);
	if (map[adjRow * maxwidth + adjCol].flags & MOVEFLAG_MOVER_HERE)
		return (false);
#ifdef USE_MINES_IN_MC2
	if (moverRelation == RELATION_ENEMY)
	{
		if (GameMap->getTile(adjRow, adjCol).getClanMineMove())
			return (false);
	}
	else
	{
		if (GameMap->getTile(adjRow, adjCol).getInnerSphereMineMove())
			return (false);
	}
#endif
	return (map[adjRow * maxwidth + adjCol].cost < COST_BLOCKED);
}

//---------------------------------------------------------------------------

inline int32_t
MoveMap::calcHPrime(int32_t r, int32_t c)
{
#ifdef LAB_ONLY
	int64_t startTime = GetCycles();
#endif
	int32_t sum = 0;
	if (r > goalR)
		sum += (r - goalR);
	else
		sum += (goalR - r);
	if (c > goalC)
		sum += (c - goalC);
	else
		sum += (goalC - c);
#ifdef LAB_ONLY
	MCTimeCalcPath3Update += (GetCycles() - startTime);
#endif
	return (sum);
}

#define MAX_MAPWIDTH 1000

//---------------------------------------------------------------------------

inline void
MoveMap::propogateCost(int32_t mapCellIndex, int32_t cost, int32_t g)
{
	gosASSERT(cost > 0);
	gosASSERT(g >= 0);
	MoveMapNodePtr curMapNode = &map[mapCellIndex];
	if (curMapNode->g > (g + cost))
	{
		curMapNode->cost = cost;
		curMapNode->g = g + cost;
		curMapNode->fPrime = curMapNode->g + curMapNode->hPrime;
		if (curMapNode->flags & MOVEFLAG_OPEN)
		{
			int32_t openIndex = openList->find(mapCellIndex);
			if (!openIndex)
			{
				wchar_t s[128];
				sprintf(s,
					"MoveMap.propogateCost: Cannot find movemap node [%d] for "
					"change\n",
					mapCellIndex);
				gosASSERT(openIndex != nullptr);
			}
			else
				openList->change(openIndex, curMapNode->fPrime);
		}
		else
		{
			for (size_t dir = 0; dir < numOffsets; dir++)
			{
				//------------------------------------------------------------
				// First, make sure this is a legit direction to go. We do NOT
				// want to clip corners, so we'll check that here...
				bool isDiagonalWalk = IsDiagonalStep[dir];
				if (isDiagonalWalk)
				{
					// MINE CHECK should go in these adj tests...
					bool adj1Open = false;
					int32_t adjCellIndex = map[mapCellIndex].adjCells[StepAdjDir[dir]];
					if (adjCellIndex > -1)
						if ((map[adjCellIndex].flags & MOVEFLAG_MOVER_HERE) == 0)
							adj1Open = (map[adjCellIndex].cost < COST_BLOCKED);
					bool adj2Open = false;
					adjCellIndex = map[mapCellIndex].adjCells[StepAdjDir[dir + 1]];
					if (adjCellIndex > -1)
						if ((map[adjCellIndex].flags & MOVEFLAG_MOVER_HERE) == 0)
							adj2Open = (map[adjCellIndex].cost < COST_BLOCKED);
					if (!adj1Open && !adj2Open)
						continue;
					// if (!adjacentCellOpen(mapCellIndex, StepAdjDir[dir]) &&
					// !adjacentCellOpen(mapCellIndex, StepAdjDir[dir + 1]))
					//	continue;
				}
				//----------------------------------------------------------
				// Calc the cell we're checking, offset from current cell...
				int32_t succCellIndex = curMapNode->adjCells[dir];
				//--------------------------------
				// If it's on the map, check it...
				if (succCellIndex > -1)
				{
					MoveMapNodePtr succMapNode = &map[succCellIndex];
					if (succMapNode->cost < COST_BLOCKED)
						if ((succMapNode->hPrime != HPRIME_NOT_CALCED) && (succMapNode->hPrime < MaxHPrime))
						{
							wchar_t dirToParent = reverseShift[dir];
							int32_t cost = succMapNode->cost;
							//------------------------------------
							// Diagonal movement is more costly...
							gosASSERT(cost > 0);
							if (isDiagonalWalk)
								cost += (cost << 1);
							if (dirToParent == succMapNode->parent)
								propogateCost(succCellIndex, cost, curMapNode->g);
							else if ((curMapNode->g + cost) < succMapNode->g)
							{
								succMapNode->parent = dirToParent;
								propogateCost(succCellIndex, cost, curMapNode->g);
							}
						}
				}
			}
		}
	}
}

//---------------------------------------------------------------------------

inline void
MoveMap::propogateCostJUMP(int32_t r, int32_t c, int32_t cost, int32_t g)
{
	gosASSERT(cost > 0);
	gosASSERT(g >= 0);
	MoveMapNodePtr curMapNode = &map[r * maxwidth + c];
	if (curMapNode->g > (g + cost))
	{
		curMapNode->cost = cost;
		curMapNode->g = g + cost;
		curMapNode->fPrime = curMapNode->g + curMapNode->hPrime;
		if (curMapNode->flags & MOVEFLAG_OPEN)
		{
			int32_t openIndex = openList->find(r * MAX_MAPWIDTH + c);
			if (!openIndex)
			{
				wchar_t s[128];
				sprintf(s,
					"MoveMap.propogateCost: Cannot find movemap node [%d, %d, "
					"%d] for change\n",
					r, c, r * MAX_MAPWIDTH + c);
				gosASSERT(openIndex != nullptr);
			}
			else
				openList->change(openIndex, curMapNode->fPrime);
		}
		else
		{
			int32_t cellOffsetIndex = 0;
			for (size_t dir = 0; dir < numOffsets; dir++)
			{
				//------------------------------------------------------------
				// First, make sure this is a legit direction to go. We do NOT
				// want to clip corners, so we'll check that here...
				bool isDiagonalWalk = IsDiagonalStep[dir];
				if (isDiagonalWalk)
				{
					if (!adjacentCellOpenJUMP(r, c, StepAdjDir[dir]) && !adjacentCellOpenJUMP(r, c, StepAdjDir[dir + 1]))
					{
						cellOffsetIndex += 2;
						continue;
					}
				}
				//----------------------------------------------------------
				// Calc the cell we're checking, offset from current cell...
				int32_t succRow = r + cellShift[cellOffsetIndex++];
				int32_t succCol = c + cellShift[cellOffsetIndex++];
				//--------------------------------
				// If it's on the map, check it...
				if (inMapBounds(succRow, succCol, height, width))
				{
					MoveMapNodePtr succMapNode = &map[succRow * maxwidth + succCol];
					if (succMapNode->cost < COST_BLOCKED)
						if ((succMapNode->hPrime != HPRIME_NOT_CALCED) && (succMapNode->hPrime < MaxHPrime))
						{
							wchar_t dirToParent = reverseShift[dir];
							bool jumping = false;
							int32_t cost = succMapNode->cost;
							//------------------------------------
							// Diagonal movement is more costly...
							gosASSERT(cost > 0);
							if (dir > 7)
							{
								jumping = true;
								if (JumpOnBlocked)
									cost = jumpCost;
								else
									cost += jumpCost;
							}
							else if (isDiagonalWalk)
								cost += (cost / 2);
							if (dirToParent == succMapNode->parent)
								propogateCostJUMP(succRow, succCol, cost, curMapNode->g);
							else if ((curMapNode->g + cost) < succMapNode->g)
							{
								succMapNode->parent = dirToParent;
								propogateCostJUMP(succRow, succCol, cost, curMapNode->g);
							}
						}
				}
			}
		}
	}
}

//---------------------------------------------------------------------------

//#define DEBUG_PATH
//#define BLOCKED_PATH_TEST
//#define TIME_PATH
//#define DEBUG_MOVE_MAP

int32_t
MoveMap::calcPath(MovePathPtr path, Stuff::Vector3D* goalWorldPos, int32_t* goalCell)
{
#ifdef TIME_PATH
	L_INTEGER calcStart, calcStop;
	QueryPerformanceCounter(calcStart);
#endif
	if ((goalR < 0) || (goalR >= height) || (goalC < 0) || (goalC >= width))
	{
		Stuff::Vector3D p;
		p.x = (float)(goalC)*Terrain::worldUnitsPerCell + Terrain::worldUnitsPerCell / 2 - Terrain::worldUnitsMapSide / 2;
		p.y = (Terrain::worldUnitsMapSide / 2) - ((float)(goalR)*Terrain::worldUnitsPerCell) - Terrain::worldUnitsPerCell / 2;
		p.z = (float)0; // How do we get the elevation for this point? Do we care?
		wchar_t msg[200];
		sprintf(msg, " Bad Move Goal: %d [%d(%d), %d(%d)], (%.2f, %.2f, %.2f)", DebugMovePathType,
			goalR, height, goalC, width, p.x, p.y, p.z);
		gosASSERT((goalR >= 0) && (goalR < height) && (goalC >= 0) && (goalC < width));
	}
	//------------------------------------------------------------------
	// Let's use their hPrime as a barrier for cutting off the search...
	MaxHPrime = calcHPrime(startR, startC) * 2.5;
	if (MaxHPrime < 500)
		MaxHPrime = 500;
	//-----------------------------------------------
	// If we haven't already, create the OPEN list...
	if (!openList)
	{
		openList = new PriorityQueue;
		gosASSERT(openList != nullptr);
		openList->init(5000);
	}
	int32_t curCol = startC;
	int32_t curRow = startR;
	MoveMapNodePtr curMapNode = &map[mapRowStartTable[curRow] + curCol];
	curMapNode->g = 0;
	if (!ZeroHPrime)
		curMapNode->hPrime = calcHPrime(curRow, curCol);
	curMapNode->fPrime = curMapNode->hPrime;
	//-----------------------------------------------
	// Put the START vertex on the empty OPEN list...
	PQNode initialVertex;
	initialVertex.key = curMapNode->fPrime;
	initialVertex.id = mapRowStartTable[curRow] + curCol;
	initialVertex.row = curRow;
	initialVertex.col = curCol;
	openList->clear();
#ifdef _DEBUG
	int32_t insertErr =
#endif
		openList->insert(initialVertex);
	gosASSERT(insertErr == NO_ERROR);
	curMapNode->setFlag(MOVEFLAG_OPEN);
	bool goalFound = false;
	int32_t bestRow = -1;
	int32_t bestCol = -1;
#ifdef DEBUG_PATH
	topOpenNodes = 1;
	numNodesVisited = 1;
#endif
	while (!openList->isEmpty())
	{
#ifdef DEBUG_MOVE_MAP
		if (debugMoveMap)
		{
			MechFile* pathDebugFile = new MechFile;
			pathDebugFile->create("mm.dbg");
			writeDebug(pathDebugFile);
			pathDebugFile->close();
			delete pathDebugFile;
			pathDebugFile = nullptr;
		}
#endif
#ifdef DEBUG_PATH
		if (topOpenNodes < openList->getNumItems())
			topOpenNodes = openList->getNumItems();
#endif
		//----------------------
		// Grab the best node...
		PQNode bestPQNode;
		openList->remove(bestPQNode);
		bestRow = bestPQNode.row;
		bestCol = bestPQNode.col;
		MoveMapNodePtr bestMapNode = &map[bestPQNode.id];
		bestMapNode->clearFlag(MOVEFLAG_OPEN);
		int32_t bestNodeG = bestMapNode->g;
		//----------------------------
		// Now, close the best node...
		bestMapNode->setFlag(MOVEFLAG_CLOSED);
		//--------------------------
		// Have we found the goal...
		if (bestMapNode->flags & MOVEFLAG_GOAL)
		{
			goalFound = true;
			break;
		}
		for (size_t dir = 0; dir < 8; dir++)
		{
			//------------------------------------------------------------
			// First, make sure this is a legit direction to go. We do NOT
			// want to clip corners, so we'll check that here...
			bool isDiagonalWalk = IsDiagonalStep[dir];
			if (isDiagonalWalk)
			{
				// MINE CHECK should go in these adj tests...
				bool adj1Open = false;
				int32_t adjCellIndex = map[bestPQNode.id].adjCells[StepAdjDir[dir]];
				if (adjCellIndex > -1)
					if ((map[adjCellIndex].flags & MOVEFLAG_MOVER_HERE) == 0)
						adj1Open = (map[adjCellIndex].cost < COST_BLOCKED);
				bool adj2Open = false;
				adjCellIndex = map[bestPQNode.id].adjCells[StepAdjDir[dir + 1]];
				if (adjCellIndex > -1)
					if ((map[adjCellIndex].flags & MOVEFLAG_MOVER_HERE) == 0)
						adj2Open = (map[adjCellIndex].cost < COST_BLOCKED);
				if (!adj1Open && !adj2Open)
					continue;
				// if (!adjacentCellOpen(bestPQNode.id, StepAdjDir[dir]) &&
				// !adjacentCellOpen(bestPQNode.id, StepAdjDir[dir + 1]))
				//	continue;
			}
			//-------------------------------
			// Now, process this direction...
			int32_t succCellIndex = bestMapNode->adjCells[dir];
			//-----------------------------------------------------------------------------------
			// If we're doing offMapTravel, make sure we aren't going back into
			// an offMap cell...
			if (map[succCellIndex].flags & MOVEFLAG_OFFMAP)
				if (cannotEnterOffMap)
					if ((map[bestPQNode.id].flags & MOVEFLAG_OFFMAP) == 0)
						continue;
			//--------------------------------
			// If it's on the map, check it...
			if (succCellIndex > -1)
			{
				if (!inBounds(mapRowTable[succCellIndex], mapColTable[succCellIndex]))
					continue;
				MoveMapNodePtr succMapNode = &map[succCellIndex];
				if (succMapNode->cost < COST_BLOCKED)
				{
					if (succMapNode->hPrime == HPRIME_NOT_CALCED)
						succMapNode->hPrime =
							calcHPrime(mapRowTable[succCellIndex /*bestPQNode.id*/],
								mapColTable[succCellIndex /*bestPQNode.id*/]);
					if (succMapNode->hPrime < MaxHPrime)
					{
#ifdef DEBUG_PATH
						numNodesVisited++;
#endif
						//--------------------------------------------------
						// How can we get back to BESTNODE from SUCCESSOR...
						wchar_t dirToParent = reverseShift[dir];
						//----------------------------------------------------
						// What's our cost to go from START to this SUCCESSOR?
						int32_t cost = succMapNode->cost;
						//------------------------------------
						// Diagonal movement is more costly...
						gosASSERT(cost > 0);
						if (isDiagonalWalk)
							cost += (cost / 2);
						gosASSERT(cost > 0);
						int32_t succNodeG = bestNodeG + cost;
						if (succMapNode->flags & MOVEFLAG_OPEN)
						{
							//----------------------------------------------
							// This node is already in the OPEN queue to be
							// be processed. Let's check if we have a better
							// path thru this route...
							if (succNodeG < succMapNode->g)
							{
								//----------------------------
								// This new path is cheaper...
								succMapNode->parent = dirToParent;
								succMapNode->g = succNodeG;
								succMapNode->fPrime = succNodeG + succMapNode->hPrime;
								int32_t openIndex = openList->find(succCellIndex);
								if (!openIndex)
								{
									wchar_t s[128];
									sprintf(s,
										"MoveMap.calcPath: Cannot find movemap "
										"node [%d, %d] for change\n",
										succCellIndex, dir);
#ifdef USE_OBJECTS
									DebugOpenList(s);
#endif
									gosASSERT(openIndex != nullptr);
								}
								else
									openList->change(openIndex, succMapNode->fPrime);
							}
						}
						else if (succMapNode->flags & MOVEFLAG_CLOSED)
						{
							//-------------------------------------------------
							// This path may be better than this node's current
							// path. If so, we may have to propogate thru...
							if (succNodeG < succMapNode->g)
							{
								//----------------------------
								// This new path is cheaper...
								succMapNode->parent = dirToParent;
								//------------------------------------------
								// Now, we have to propogate the new cost...
								propogateCost(succCellIndex, cost, bestNodeG);
							}
						}
						else
						{
							//-------------------------------------------------
							// This node is neither OPEN nor CLOSED, so toss it
							// into the OPEN list...
							succMapNode->parent = dirToParent;
							succMapNode->g = succNodeG;
							succMapNode->fPrime = succNodeG + succMapNode->hPrime;
							PQNode succPQNode;
							succPQNode.key = succMapNode->fPrime;
							succPQNode.id = succCellIndex;
							succPQNode.row = mapRowTable[succCellIndex];
							succPQNode.col = mapColTable[succCellIndex];
#ifdef _DEBUG
							int32_t insertErr =
#endif
								openList->insert(succPQNode);
							gosASSERT(insertErr == NO_ERROR);
							succMapNode->setFlag(MOVEFLAG_OPEN);
						}
					}
				}
			}
		}
	}
#ifdef DEBUG_MOVE_MAP
	if (debugMoveMap)
	{
		MechFile* pathDebugFile = new MechFile;
		pathDebugFile->create("mm.dbg");
		writeDebug(pathDebugFile);
		pathDebugFile->close();
		delete pathDebugFile;
		pathDebugFile = nullptr;
	}
#endif
	if (goalFound)
	{
		//-------------------------------------------
		// First, let's count how int32_t the path is...
		int32_t curRow = goalCell[0] = (int32_t)bestRow;
		int32_t curCol = goalCell[1] = (int32_t)bestCol;
		int32_t numCells = 0;
		while ((curRow != startR) || (curCol != startC))
		{
			numCells += 1;
			int32_t cellOffsetIndex = (map[mapRowStartTable[curRow] + curCol].parent << 1);
			// if ((cellOffsetIndex < 0) || (cellOffsetIndex > 14))
			//	OutputDebugString("PathFinder: whoops\n");
			curRow += cellShift[cellOffsetIndex++];
			curCol += cellShift[cellOffsetIndex];
		}
		//---------------------------------------------------------------
		// If our goal is a door, the path will be one step longer, since
		// we need to walk "thru" the door...
		if (doorDirection != -1)
			numCells++;
#ifdef _DEBUG
		if (numCells > MAX_STEPS_PER_MOVEPATH)
		{
			MechFile* pathDebugFile = new MechFile;
			pathDebugFile->create("longpath.dbg");
			wchar_t s[512];
			sprintf(s,
				"Path Too Long: %d steps (Max = %d) (please save longpath.dbg "
				"file:)",
				numCells, MAX_STEPS_PER_MOVEPATH);
			pathDebugFile->writeString(s);
			pathDebugFile->writeString("\n\n");
			writeDebug(pathDebugFile);
			pathDebugFile->close();
			delete pathDebugFile;
			pathDebugFile = nullptr;
			gosASSERT(numCells <= MAX_STEPS_PER_MOVEPATH);
		}
#endif
		//-----------------------------
		// Now, let's build the path...
		//-------------------------------
		// Grab the path and return it...
		path->init();
		if (numCells)
		{
#ifdef _DEBUG
			int32_t maxSteps =
#endif
				path->init(numCells);
#ifdef _DEBUG
			if (maxSteps > -1)
			{
				MechFile* pathDebugFile = new MechFile;
				pathDebugFile->create("longpath.dbg");
				wchar_t s[512];
				sprintf(s, "New Longest Path: %d steps (Max = %d)\n\n", numCells,
					MAX_STEPS_PER_MOVEPATH);
				pathDebugFile->writeString(s);
				writeDebug(pathDebugFile);
				pathDebugFile->close();
				delete pathDebugFile;
				pathDebugFile = nullptr;
			}
#endif
			path->target = target;
			path->cost = map[mapRowStartTable[bestRow] + bestCol].g;
			curRow = (int32_t)bestRow;
			curCol = (int32_t)bestCol;
			int32_t curCell = numCells;
			if (doorDirection == -1)
			{
				if (goalWorldPos)
				{
					goalWorldPos->x = (float)(ULc + bestCol) * Terrain::worldUnitsPerCell + Terrain::worldUnitsPerCell / 2 - Terrain::worldUnitsMapSide / 2;
					goalWorldPos->y = (Terrain::worldUnitsMapSide / 2) - ((float)(ULr + bestRow) * Terrain::worldUnitsPerCell) - Terrain::worldUnitsPerCell / 2;
					goalWorldPos->z = (float)0; // How do we get the elevation
						// for this point? Do we care?
					path->goal = *goalWorldPos;
				}
				else
					path->goal = goal;
			}
			else
			{
				//--------------------------------------------------
				// It's a door, so it's the last on the path list...
				curCell--;
				path->setDirection(curCell, /*reverseShift[*/ doorDirection * 2 /*]*/);
				int32_t doorR = bestRow + adjTile[doorDirection][0];
				int32_t doorC = bestCol + adjTile[doorDirection][1];
				goalCell[0] = ULr + doorR;
				goalCell[1] = ULc + doorC;
				Stuff::Vector3D stepDest;
				stepDest.x = (float)(ULc + doorC) * Terrain::worldUnitsPerCell + Terrain::worldUnitsPerCell / 2 - Terrain::worldUnitsMapSide / 2;
				stepDest.y = (Terrain::worldUnitsMapSide / 2) - ((float)(ULr + doorR) * Terrain::worldUnitsPerCell) - Terrain::worldUnitsPerCell / 2;
				stepDest.z = (float)0; // How do we get the elevation for this
					// point? Do we care?
				path->setDestination(curCell, stepDest);
				path->setDistanceToGoal(curCell, 0.0);
				path->setCell(curCell, goalCell[0], goalCell[1]);
				path->goal = stepDest;
				if (goalWorldPos)
				{
					//--------------------------------------------------------------------
					// We didn't know the exact goal world pos coming into this,
					// since are goal was an area door....
					*goalWorldPos = stepDest;
				}
			}
			//---------------------------------------------------------
			// We need to set this table up our first time thru here...
			static bool setTable = false;
			if (!setTable)
			{
				float cellLength = Terrain::worldUnitsPerCell * metersPerWorldUnit;
				for (size_t i = 0; i < NUM_CELL_OFFSETS; i++)
				{
					float distance = agsqrt(cellShift[i * 2], cellShift[i * 2 + 1]) * cellLength;
					cellShiftDistance[i] = distance;
				}
				setTable = true;
			}
			while ((curRow != startR) || (curCol != startC))
			{
				curCell--;
				int32_t parent = reverseShift[map[mapRowStartTable[curRow] + curCol].parent];
				if (parent > 7)
					path->setDirection(curCell, parent);
				else
					path->setDirection(curCell, parent);
				Stuff::Vector3D stepDest;
				int32_t cell[2];
				cell[0] = ULr + curRow;
				cell[1] = ULc + curCol;
				stepDest.x = (float)(cell[1]) * Terrain::worldUnitsPerCell + Terrain::worldUnitsPerCell / 2 - Terrain::worldUnitsMapSide / 2;
				stepDest.y = (Terrain::worldUnitsMapSide / 2) - ((float)(cell[0]) * Terrain::worldUnitsPerCell) - Terrain::worldUnitsPerCell / 2;
				stepDest.z = (float)0; // How do we get the elevation for this
					// point? Do we care?
				if (curCell == (numCells - 1))
					path->setDistanceToGoal(curCell, 0.0);
				else
				{
					int32_t tempDir = path->getDirection(curCell + 1);
					float tempDist = path->getDistanceToGoal(curCell + 1);
					path->setDistanceToGoal(curCell, cellShiftDistance[tempDir] + tempDist);
				}
				path->setDestination(curCell, stepDest);
				path->setCell(curCell, cell[0], cell[1]);
				path->stepList[curCell].area = GlobalMoveMap[moveLevel]->calcArea(cell[0], cell[1]);
				map[curRow * maxwidth + curCol].setFlag(MOVEFLAG_STEP);
				int32_t cellOffsetIndex = map[mapRowStartTable[curRow] + curCol].parent << 1;
				curRow += cellShift[cellOffsetIndex++];
				curCol += cellShift[cellOffsetIndex];
				// calcAdjNode(curRow, curCol, map[curRow * maxCellwidth +
				// curCol].parent);
			}
			if (thruAreas[1] != -1)
				if (thruAreas[1] != path->stepList[path->numSteps - 1].area)
				{
					for (size_t i = 0; i < path->numSteps; i++)
					{
						if (path->stepList[i].area == thruAreas[1])
						{
							path->numStepsWhenNotPaused = path->numSteps = i + 1;
							goalCell[0] = path->stepList[i].cell[0];
							goalCell[1] = path->stepList[i].cell[1];
							land->cellToWorld(goalCell[0], goalCell[1], *goalWorldPos);
							break;
						}
					}
				}
#ifdef BLOCKED_PATH_TEST
			//---------------------------------------------------------------------
			// Let's test every path point to make sure it is not a blocked
			// cell...
			for (size_t i = 0; i < path->numSteps; i++)
			{
				int32_t tile[2], cell[2];
				GameMap->worldToMapPos(
					path->stepList[i].destination, tile[0], tile[1], cell[0], cell[1]);
				bool cellPassable = GameMap->cellPassable(tile[0], tile[0], cell[0], cell[1]);
				if (!cellPassable)
					cellPassable = GameMap->cellPassable(tile[0], tile[0], cell[0], cell[1]);
				Assert(cellPassable || (i < 3), 0, " Bad Path Point ");
			}
#endif
		}
#ifdef TIME_PATH
		QueryPerformanceCounter(calcStop);
		calcTime = float(calcStop.LowPart - calcStart.LowPart) / float(countsPerSecond.LowPart);
		if (debugger && (scenarioTime > 0.0))
		{
			wchar_t s[50];
			sprintf(s, "path calc: %.4f\n", calcTime);
			OutputDebugString(s);
			debugger->print(s);
		}
#endif
		//------------------------------------------------------------------------------------
		// If we're starting on the goal cell, set the cost to 1 as an indicator
		// that there is a path--it's just that we were already there!
		if (numCells == 0)
			path->cost = 1;
		return (path->numSteps);
	}
#ifdef TIME_PATH
	QueryPerformanceCounter(calcStop);
	calcTime = float(calcStop.LowPart - calcStart.LowPart) / float(countsPerSecond.LowPart);
	if (debugger && (scenarioTime > 0.0))
	{
		wchar_t s[50];
		sprintf(s, "path calc: %.4f", calcTime);
		debugger->print(s);
	}
#endif
	return (0);
}

//---------------------------------------------------------------------------

int32_t
MoveMap::calcPathJUMP(MovePathPtr path, Stuff::Vector3D* goalWorldPos, int32_t* goalCell)
{
#ifdef TIME_PATH
	L_INTEGER calcStart, calcStop;
	QueryPerformanceCounter(calcStart);
#endif
	//------------------------------------------------------------------
	// Clear the start cell, in case we're starting in a blocked cell...
	// map[startR * maxCellwidth + startC].cost = clearCost;
	//-------------------------------------------------------------------
	// If the start tile is blocked, let's clear it so we may move off of
	// it, at least. Change the blocked tiles to just barely passable...
	/*	int32_t startULr = startR - 1;
		int32_t startULc = startC - 1;
		for (int32_t cellR = 0; cellR < terrain_const::MAPCELL_DIM; cellR++)
			for (int32_t cellC = 0; cellC < terrain_const::MAPCELL_DIM; cellC++) {
				int32_t curCellR = startULr + cellR;
				int32_t curCellC = startULc + cellC;
				int32_t mapIndex = curCellR * maxCellwidth + curCellC;
				if ((curCellR >= 0) && (curCellR < cellheight) && (curCellC >=
	   0) && (curCellC < cellwidth)) if (map[mapIndex].cost >= COST_BLOCKED)
						map[mapIndex].cost = COST_BLOCKED - 1;
			}
	*/
	if ((goalR < 0) || (goalR >= height) || (goalC < 0) || (goalC >= width))
	{
		Stuff::Vector3D p;
		p.x = (float)(goalC)*Terrain::worldUnitsPerCell + Terrain::worldUnitsPerCell / 2 - Terrain::worldUnitsMapSide / 2;
		p.y = (Terrain::worldUnitsMapSide / 2) - ((float)(goalR)*Terrain::worldUnitsPerCell) - Terrain::worldUnitsPerCell / 2;
		p.z = (float)0; // How do we get the elevation for this point? Do we care?
		wchar_t msg[200];
		sprintf(msg, " Bad Move Goal: %d [%d(%d), %d(%d)], (%.2f, %.2f, %.2f)", DebugMovePathType,
			goalR, height, goalC, width, p.x, p.y, p.z);
		gosASSERT((goalR >= 0) && (goalR < height) && (goalC >= 0) && (goalC < width));
	}
	//------------------------------------------------------------------
	// Let's use their hPrime as a barrier for cutting off the search...
	MaxHPrime = calcHPrime(startR, startC) * 2.5;
	if (MaxHPrime < 500)
		MaxHPrime = 500;
	//-----------------------------------------------
	// If we haven't already, create the OPEN list...
	if (!openList)
	{
		openList = new PriorityQueue;
		gosASSERT(openList != nullptr);
		openList->init(5000);
	}
	int32_t curCol = startC;
	int32_t curRow = startR;
	MoveMapNodePtr curMapNode = &map[curRow * maxwidth + curCol];
	curMapNode->g = 0;
	if (!ZeroHPrime)
		curMapNode->hPrime = calcHPrime(curRow, curCol);
	curMapNode->fPrime = curMapNode->hPrime;
	//-----------------------------------------------
	// Put the START vertex on the empty OPEN list...
	PQNode initialVertex;
	initialVertex.key = curMapNode->fPrime;
	initialVertex.id = curRow * MAX_MAPWIDTH + curCol;
	initialVertex.row = curRow;
	initialVertex.col = curCol;
	openList->clear();
#ifdef _DEBUG
	int32_t insertErr =
#endif
		openList->insert(initialVertex);
	gosASSERT(insertErr == NO_ERROR);
	curMapNode->setFlag(MOVEFLAG_OPEN);
	bool goalFound = false;
	int32_t bestRow = -1;
	int32_t bestCol = -1;
#ifdef DEBUG_PATH
	topOpenNodes = 1;
	numNodesVisited = 1;
#endif
	while (!openList->isEmpty())
	{
#ifdef DEBUG_MOVE_MAP
		if (debugMoveMap)
		{
			MechFile* pathDebugFile = new MechFile;
			pathDebugFile->create("mm.dbg");
			writeDebug(pathDebugFile);
			pathDebugFile->close();
			delete pathDebugFile;
			pathDebugFile = nullptr;
		}
#endif
#ifdef DEBUG_PATH
		if (topOpenNodes < openList->getNumItems())
			topOpenNodes = openList->getNumItems();
#endif
		//----------------------
		// Grab the best node...
		PQNode bestPQNode;
		openList->remove(bestPQNode);
		bestRow = bestPQNode.row;
		bestCol = bestPQNode.col;
		MoveMapNodePtr bestMapNode = &map[bestRow * maxwidth + bestCol];
		bestMapNode->clearFlag(MOVEFLAG_OPEN);
		int32_t bestNodeG = bestMapNode->g;
		//----------------------------
		// Now, close the best node...
		bestMapNode->setFlag(MOVEFLAG_CLOSED);
		//--------------------------
		// Have we found the goal...
		if (bestMapNode->flags & MOVEFLAG_GOAL)
		{
			goalFound = true;
			break;
		}
		int32_t cellOffsetIndex = 0;
		for (size_t dir = 0; dir < numOffsets; dir++)
		{
			//------------------------------------------------------------
			// First, make sure this is a legit direction to go. We do NOT
			// want to clip corners, so we'll check that here...
			bool isDiagonalWalk = IsDiagonalStep[dir];
			if (isDiagonalWalk)
			{
				if (!adjacentCellOpenJUMP(bestRow, bestCol, StepAdjDir[dir]) && !adjacentCellOpenJUMP(bestRow, bestCol, StepAdjDir[dir + 1]))
				{
					cellOffsetIndex += 2;
					continue;
				}
			}
			//------------------------------------------------------------
			// Now, process this direction. First, calc the cell to check,
			// offset from the current cell...
			int32_t succRow = bestRow + cellShift[cellOffsetIndex++];
			int32_t succCol = bestCol + cellShift[cellOffsetIndex++];
			//--------------------------------
			// If it's on the map, check it...
			if (inMapBounds(succRow, succCol, height, width))
			{
				MoveMapNodePtr succMapNode = &map[succRow * maxwidth + succCol];
				if (succMapNode->cost < COST_BLOCKED)
				{
					if (succMapNode->hPrime == HPRIME_NOT_CALCED)
						succMapNode->hPrime = calcHPrime(succRow, succCol);
					if (succMapNode->hPrime < MaxHPrime)
					{
#ifdef DEBUG_PATH
						numNodesVisited++;
#endif
						//--------------------------------------------------
						// How can we get back to BESTNODE from SUCCESSOR...
						wchar_t dirToParent = reverseShift[dir];
						//----------------------------------------------------
						// What's our cost to go from START to this SUCCESSOR?
						bool jumping = false;
						int32_t cost = succMapNode->cost;
						//------------------------------------
						// Diagonal movement is more costly...
						gosASSERT(cost > 0);
						if (dir > 7)
						{
							jumping = true;
							if (JumpOnBlocked)
								cost = jumpCost;
							else
								cost += jumpCost;
						}
						else
						{
							if (isDiagonalWalk)
								cost += (cost / 2);
						}
						gosASSERT(cost > 0);
						int32_t succNodeG = bestNodeG + cost;
						if (succMapNode->flags & MOVEFLAG_OPEN)
						{
							//----------------------------------------------
							// This node is already in the OPEN queue to be
							// be processed. Let's check if we have a better
							// path thru this route...
							if (succNodeG < succMapNode->g)
							{
								//----------------------------
								// This new path is cheaper...
								succMapNode->parent = dirToParent;
								succMapNode->g = succNodeG;
								succMapNode->fPrime = succNodeG + succMapNode->hPrime;
								int32_t openIndex =
									openList->find(succRow * MAX_MAPWIDTH + succCol);
								if (!openIndex)
								{
									wchar_t s[128];
									sprintf(s,
										"MoveMap.calcPath: Cannot find movemap "
										"node [%d, %d, %d, %d] for change\n",
										succRow, succCol, succRow * MAX_MAPWIDTH + succCol, dir);
#ifdef USE_OBJECTS
									DebugOpenList(s);
#endif
									gosASSERT(openIndex != nullptr);
								}
								else
									openList->change(openIndex, succMapNode->fPrime);
							}
						}
						else if (succMapNode->flags & MOVEFLAG_CLOSED)
						{
							//-------------------------------------------------
							// This path may be better than this node's current
							// path. If so, we may have to propogate thru...
							if (succNodeG < succMapNode->g)
							{
								//----------------------------
								// This new path is cheaper...
								succMapNode->parent = dirToParent;
								//------------------------------------------
								// Now, we have to propogate the new cost...
								propogateCostJUMP(succRow, succCol, cost, bestNodeG);
							}
						}
						else
						{
							//-------------------------------------------------
							// This node is neither OPEN nor CLOSED, so toss it
							// into the OPEN list...
							succMapNode->parent = dirToParent;
							succMapNode->g = succNodeG;
							succMapNode->fPrime = succNodeG + succMapNode->hPrime;
							PQNode succPQNode;
							succPQNode.key = succMapNode->fPrime;
							succPQNode.id = succRow * MAX_MAPWIDTH + succCol;
							succPQNode.row = succRow;
							succPQNode.col = succCol;
#ifdef _DEBUG
							int32_t insertErr =
#endif
								openList->insert(succPQNode);
							gosASSERT(insertErr == NO_ERROR);
							succMapNode->setFlag(MOVEFLAG_OPEN);
						}
					}
				}
			}
		}
	}
#ifdef DEBUG_MOVE_MAP
	if (debugMoveMap)
	{
		MechFile* pathDebugFile = new MechFile;
		pathDebugFile->create("mm.dbg");
		writeDebug(pathDebugFile);
		pathDebugFile->close();
		delete pathDebugFile;
		pathDebugFile = nullptr;
	}
#endif
#if 0 // REdo when bridges are done
	if(ClearBridgeTiles)
	{
		overlayWeightTable[OVERLAY_WATER_BRIDGE_NS * terrain_const::MAPCELL_DIM * terrain_const::MAPCELL_DIM + 1] += COST_BLOCKED;
		overlayWeightTable[OVERLAY_WATER_BRIDGE_NS * terrain_const::MAPCELL_DIM * terrain_const::MAPCELL_DIM + 4] += COST_BLOCKED;
		overlayWeightTable[OVERLAY_WATER_BRIDGE_NS * terrain_const::MAPCELL_DIM * terrain_const::MAPCELL_DIM + 7] += COST_BLOCKED;
		overlayWeightTable[OVERLAY_WATER_BRIDGE_EW * terrain_const::MAPCELL_DIM * terrain_const::MAPCELL_DIM + 1] += COST_BLOCKED;
		overlayWeightTable[OVERLAY_WATER_BRIDGE_EW * terrain_const::MAPCELL_DIM * terrain_const::MAPCELL_DIM + 4] += COST_BLOCKED;
		overlayWeightTable[OVERLAY_WATER_BRIDGE_EW * terrain_const::MAPCELL_DIM * terrain_const::MAPCELL_DIM + 7] += COST_BLOCKED;
		overlayWeightTable[OVERLAY_RAILROAD_WATER_BRIDGE_NS * terrain_const::MAPCELL_DIM * terrain_const::MAPCELL_DIM + 1] += COST_BLOCKED;
		overlayWeightTable[OVERLAY_RAILROAD_WATER_BRIDGE_NS * terrain_const::MAPCELL_DIM * terrain_const::MAPCELL_DIM + 4] += COST_BLOCKED;
		overlayWeightTable[OVERLAY_RAILROAD_WATER_BRIDGE_NS * terrain_const::MAPCELL_DIM * terrain_const::MAPCELL_DIM + 7] += COST_BLOCKED;
		overlayWeightTable[OVERLAY_RAILROAD_WATER_BRIDGE_EW * terrain_const::MAPCELL_DIM * terrain_const::MAPCELL_DIM + 1] += COST_BLOCKED;
		overlayWeightTable[OVERLAY_RAILROAD_WATER_BRIDGE_EW * terrain_const::MAPCELL_DIM * terrain_const::MAPCELL_DIM + 4] += COST_BLOCKED;
		overlayWeightTable[OVERLAY_RAILROAD_WATER_BRIDGE_EW * terrain_const::MAPCELL_DIM * terrain_const::MAPCELL_DIM + 7] += COST_BLOCKED;
	}
#endif
	if (goalFound)
	{
		//-------------------------------------------
		// First, let's count how int32_t the path is...
		int32_t curRow = goalCell[0] = (int32_t)bestRow;
		int32_t curCol = goalCell[1] = (int32_t)bestCol;
		int32_t numCells = 0;
		while ((curRow != startR) || (curCol != startC))
		{
			numCells += 1;
			int32_t cellOffsetIndex = map[curRow * maxwidth + curCol].parent * 2;
			// if ((cellOffsetIndex < 0) || (cellOffsetIndex > 14))
			//	OutputDebugString("PathFinder: whoops\n");
			curRow += cellShift[cellOffsetIndex++];
			curCol += cellShift[cellOffsetIndex];
		}
		//---------------------------------------------------------------
		// If our goal is a door, the path will be one step longer, since
		// we need to walk "thru" the door...
		if (doorDirection != -1)
			numCells++;
#ifdef _DEBUG
		if (numCells > MAX_STEPS_PER_MOVEPATH)
		{
			MechFile* pathDebugFile = new MechFile;
			pathDebugFile->create("longpath.dbg");
			wchar_t s[512];
			sprintf(s,
				"Path Too Long: %d steps (Max = %d) (please save longpath.dbg "
				"file:)",
				numCells, MAX_STEPS_PER_MOVEPATH);
			pathDebugFile->writeString(s);
			pathDebugFile->writeString("\n\n");
			writeDebug(pathDebugFile);
			pathDebugFile->close();
			delete pathDebugFile;
			pathDebugFile = nullptr;
			gosASSERT(numCells <= MAX_STEPS_PER_MOVEPATH);
		}
#endif
		//-----------------------------
		// Now, let's build the path...
		//-------------------------------
		// Grab the path and return it...
		path->init();
		if (numCells)
		{
#ifdef _DEBUG
			int32_t maxSteps =
#endif
				path->init(numCells);
#ifdef _DEBUG
			if (maxSteps > -1)
			{
				MechFile* pathDebugFile = new MechFile;
				pathDebugFile->create("longpath.dbg");
				wchar_t s[512];
				sprintf(s, "New Longest Path: %d steps (Max = %d)\n\n", numCells,
					MAX_STEPS_PER_MOVEPATH);
				pathDebugFile->writeString(s);
				writeDebug(pathDebugFile);
				pathDebugFile->close();
				delete pathDebugFile;
				pathDebugFile = nullptr;
			}
#endif
			path->target = target;
			path->cost = map[bestRow * maxwidth + bestCol].g;
			curRow = (int32_t)bestRow;
			curCol = (int32_t)bestCol;
			int32_t curCell = numCells;
			if (doorDirection == -1)
			{
				if (goalWorldPos)
				{
					goalWorldPos->x = (float)(ULc + bestCol) * Terrain::worldUnitsPerCell + Terrain::worldUnitsPerCell / 2 - Terrain::worldUnitsMapSide / 2;
					goalWorldPos->y = (Terrain::worldUnitsMapSide / 2) - ((float)(ULr + bestRow) * Terrain::worldUnitsPerCell) - Terrain::worldUnitsPerCell / 2;
					goalWorldPos->z = (float)0; // How do we get the elevation
						// for this point? Do we care?
					path->goal = *goalWorldPos;
				}
				else
					path->goal = goal;
			}
			else
			{
				//--------------------------------------------------
				// It's a door, so it's the last on the path list...
				curCell--;
				path->setDirection(curCell, /*reverseShift[*/ doorDirection * 2 /*]*/);
				int32_t doorR = bestRow + adjTile[doorDirection][0];
				int32_t doorC = bestCol + adjTile[doorDirection][1];
				goalCell[0] = ULr + doorR;
				goalCell[1] = ULc + doorC;
				Stuff::Vector3D stepDest;
				stepDest.x = (float)(ULc + doorC) * Terrain::worldUnitsPerCell + Terrain::worldUnitsPerCell / 2 - Terrain::worldUnitsMapSide / 2;
				stepDest.y = (Terrain::worldUnitsMapSide / 2) - ((float)(ULr + doorR) * Terrain::worldUnitsPerCell) - Terrain::worldUnitsPerCell / 2;
				stepDest.z = (float)0; // How do we get the elevation for this
					// point? Do we care?
				path->setDestination(curCell, stepDest);
				path->setDistanceToGoal(curCell, 0.0);
				path->setCell(curCell, goalCell[0], goalCell[1]);
				path->goal = stepDest;
				if (goalWorldPos)
				{
					//--------------------------------------------------------------------
					// We didn't know the exact goal world pos coming into this,
					// since are goal was an area door....
					*goalWorldPos = stepDest;
				}
			}
			//---------------------------------------------------------
			// We need to set this table up our first time thru here...
			static bool setTable = false;
			if (!setTable)
			{
				float cellLength = Terrain::worldUnitsPerCell * metersPerWorldUnit;
				for (size_t i = 0; i < NUM_CELL_OFFSETS; i++)
				{
					float distance = agsqrt(cellShift[i * 2], cellShift[i * 2 + 1]) * cellLength;
					cellShiftDistance[i] = distance;
				}
				setTable = true;
			}
			while ((curRow != startR) || (curCol != startC))
			{
				curCell--;
				int32_t parent = reverseShift[map[curRow * maxwidth + curCol].parent];
				if (parent > 7)
					path->setDirection(curCell, parent);
				else
					path->setDirection(curCell, parent);
				Stuff::Vector3D stepDest;
				int32_t cell[2];
				cell[0] = ULr + curRow;
				cell[1] = ULc + curCol;
				stepDest.x = (float)(cell[1]) * Terrain::worldUnitsPerCell + Terrain::worldUnitsPerCell / 2 - Terrain::worldUnitsMapSide / 2;
				stepDest.y = (Terrain::worldUnitsMapSide / 2) - ((float)(cell[0]) * Terrain::worldUnitsPerCell) - Terrain::worldUnitsPerCell / 2;
				stepDest.z = (float)0; // How do we get the elevation for this
					// point? Do we care?
				if (curCell == (numCells - 1))
					path->setDistanceToGoal(curCell, 0.0);
				else
				{
					int32_t tempDir = path->getDirection(curCell + 1);
					float tempDist = path->getDistanceToGoal(curCell + 1);
					path->setDistanceToGoal(curCell, cellShiftDistance[tempDir] + tempDist);
				}
				path->setDestination(curCell, stepDest);
				path->setCell(curCell, cell[0], cell[1]);
				map[curRow * maxwidth + curCol].setFlag(MOVEFLAG_STEP);
				int32_t cellOffsetIndex = map[curRow * maxwidth + curCol].parent << 1;
				curRow += cellShift[cellOffsetIndex++];
				curCol += cellShift[cellOffsetIndex];
				// calcAdjNode(curRow, curCol, map[curRow * maxCellwidth +
				// curCol].parent);
			}
#ifdef BLOCKED_PATH_TEST
			//---------------------------------------------------------------------
			// Let's test every path point to make sure it is not a blocked
			// cell...
			for (size_t i = 0; i < path->numSteps; i++)
			{
				int32_t tile[2], cell[2];
				GameMap->worldToMapPos(
					path->stepList[i].destination, tile[0], tile[1], cell[0], cell[1]);
				bool cellPassable = GameMap->cellPassable(tile[0], tile[0], cell[0], cell[1]);
				if (!cellPassable)
					cellPassable = GameMap->cellPassable(tile[0], tile[0], cell[0], cell[1]);
				Assert(cellPassable || (i < 3), 0, " Bad Path Point ");
			}
#endif
		}
#ifdef TIME_PATH
		QueryPerformanceCounter(calcStop);
		calcTime = float(calcStop.LowPart - calcStart.LowPart) / float(countsPerSecond.LowPart);
		if (debugger && (scenarioTime > 0.0))
		{
			wchar_t s[50];
			sprintf(s, "path calc: %.4f\n", calcTime);
			OutputDebugString(s);
			debugger->print(s);
		}
#endif
		return (path->numSteps);
	}
#ifdef TIME_PATH
	QueryPerformanceCounter(calcStop);
	calcTime = float(calcStop.LowPart - calcStart.LowPart) / float(countsPerSecond.LowPart);
	if (debugger && (scenarioTime > 0.0))
	{
		wchar_t s[50];
		sprintf(s, "path calc: %.4f", calcTime);
		debugger->print(s);
	}
#endif
	return (0);
}

//---------------------------------------------------------------------------

int32_t
MoveMap::calcEscapePath(MovePathPtr path, Stuff::Vector3D* goalWorldPos, int32_t* goalCell)
{
#ifdef TIME_PATH
	L_INTEGER calcStart, calcStop;
	QueryPerformanceCounter(calcStart);
#endif
	//------------------------------------------------------------------
	// Clear the start cell, in case we're starting in a blocked cell...
	// map[startR * maxCellwidth + startC].cost = clearCost;
	//-------------------------------------------------------------------
	// If the start tile is blocked, let's clear it so we may move off of
	// it, at least. Change the blocked tiles to just barely passable...
	/*	int32_t startULr = startR - 1;
		int32_t startULc = startC - 1;
		for (int32_t cellR = 0; cellR < terrain_const::MAPCELL_DIM; cellR++)
			for (int32_t cellC = 0; cellC < terrain_const::MAPCELL_DIM; cellC++) {
				int32_t curCellR = startULr + cellR;
				int32_t curCellC = startULc + cellC;
				int32_t mapIndex = curCellR * maxCellwidth + curCellC;
				if ((curCellR >= 0) && (curCellR < cellheight) && (curCellC >=
	   0) && (curCellC < cellwidth)) if (map[mapIndex].cost >= COST_BLOCKED)
						map[mapIndex].cost = COST_BLOCKED - 1;
			}
	*/
	/*
		if ((goalR < 0) || (goalR >= cellheight) || (goalC < 0) || (goalC >=
	   cellwidth)) { Stuff::Vector3D p; p.x = (float)(goalC) * MetersPerCell +
	   MetersPerCell / 2 - Terrain::worldUnitsMapSide / 2; p.y =
	   (Terrain::worldUnitsMapSide / 2) - ((float)(goalR) * MetersPerCell) -
	   MetersPerCell / 2; p.z = (float)0; // How do we get the elevation for
	   this point? Do we care? wchar_t msg[200]; sprintf(msg, " Bad Move Goal: %d
	   [%d(%d), %d(%d)], (%.2f, %.2f, %.2f)", DebugMovePathType, goalR,
	   cellheight, goalC, cellwidth, p.x, p.y, p.z); Fatal(0, msg);
		}
	*/
	//------------------------------------------------------------------
	// Let's use their hPrime as a barrier for cutting off the search...
	MaxHPrime = 500; // float2short(calcHPrime(startR, startC) * 2.5);
	if (MaxHPrime < 500)
		MaxHPrime = 500;
	//-----------------------------------------------
	// If we haven't already, create the OPEN list...
	if (!openList)
	{
		openList = new PriorityQueue;
		gosASSERT(openList != nullptr);
		openList->init(5000);
	}
	int32_t curCol = startC;
	int32_t curRow = startR;
	MoveMapNodePtr curMapNode = &map[curRow * maxwidth + curCol];
	curMapNode->g = 0;
	curMapNode->hPrime = 10; // calcHPrime(curRow, curCol);
	curMapNode->fPrime = curMapNode->hPrime;
	//-----------------------------------------------
	// Put the START vertex on the empty OPEN list...
	PQNode initialVertex;
	initialVertex.key = curMapNode->fPrime;
	initialVertex.id = curRow * MAX_MAPWIDTH + curCol;
	initialVertex.row = curRow;
	initialVertex.col = curCol;
	openList->clear();
#ifdef _DEBUG
	int32_t insertErr =
#endif
		openList->insert(initialVertex);
	gosASSERT(insertErr == NO_ERROR);
	curMapNode->setFlag(MOVEFLAG_OPEN);
	bool goalFound = false;
	int32_t bestRow = -1;
	int32_t bestCol = -1;
#ifdef DEBUG_PATH
	topOpenNodes = 1;
	numNodesVisited = 1;
#endif
	while (!openList->isEmpty())
	{
#ifdef DEBUG_MOVE_MAP
		if (debugMoveMap)
		{
			MechFile* pathDebugFile = new MechFile;
			pathDebugFile->create("mm.dbg");
			writeDebug(pathDebugFile);
			pathDebugFile->close();
			delete pathDebugFile;
			pathDebugFile = nullptr;
		}
#endif
#ifdef DEBUG_PATH
		if (topOpenNodes < openList->getNumItems())
			topOpenNodes = openList->getNumItems();
#endif
		//----------------------
		// Grab the best node...
		PQNode bestPQNode;
		openList->remove(bestPQNode);
		bestRow = bestPQNode.row;
		bestCol = bestPQNode.col;
		MoveMapNodePtr bestMapNode = &map[bestRow * maxwidth + bestCol];
		bestMapNode->clearFlag(MOVEFLAG_OPEN);
		int32_t bestNodeG = bestMapNode->g;
		//----------------------------
		// Now, close the best node...
		bestMapNode->setFlag(MOVEFLAG_CLOSED);
		//--------------------------
		// Have we found the goal...
		if (bestMapNode->flags & MOVEFLAG_GOAL)
		{
			goalFound = true;
			break;
		}
		int32_t cellOffsetIndex = 0;
		for (size_t dir = 0; dir < numOffsets; dir++)
		{
			//------------------------------------------------------------
			// First, make sure this is a legit direction to go. We do NOT
			// want to clip corners, so we'll check that here...
			bool isDiagonalWalk = IsDiagonalStep[dir];
			if (isDiagonalWalk)
			{
				if (!adjacentCellOpenJUMP(bestRow, bestCol, StepAdjDir[dir]) && !adjacentCellOpenJUMP(bestRow, bestCol, StepAdjDir[dir + 1]))
				{
					cellOffsetIndex += 2;
					continue;
				}
			}
			//------------------------------------------------------------
			// Now, process this direction. First, calc the cell to check,
			// offset from the current cell...
			int32_t succRow = bestRow + cellShift[cellOffsetIndex++];
			int32_t succCol = bestCol + cellShift[cellOffsetIndex++];
			//--------------------------------
			// If it's on the map, check it...
			if (inMapBounds(succRow, succCol, height, width))
			{
				MoveMapNodePtr succMapNode = &map[succRow * maxwidth + succCol];
				if (succMapNode->cost < COST_BLOCKED)
				{
					if (succMapNode->hPrime == HPRIME_NOT_CALCED)
						succMapNode->hPrime = 10; // calcHPrime(succRow, succCol);
					if (succMapNode->hPrime < MaxHPrime)
					{
#ifdef DEBUG_PATH
						numNodesVisited++;
#endif
						//--------------------------------------------------
						// How can we get back to BESTNODE from SUCCESSOR...
						wchar_t dirToParent = reverseShift[dir];
						//----------------------------------------------------
						// What's our cost to go from START to this SUCCESSOR?
						bool jumping = false;
						int32_t cost = succMapNode->cost;
						//------------------------------------
						// Diagonal movement is more costly...
						if (dir > 7)
						{
							jumping = true;
							if (JumpOnBlocked)
								cost = jumpCost;
							else
								cost += jumpCost;
						}
						else
						{
							if (isDiagonalWalk)
								cost += (cost / 2);
						}
						gosASSERT(cost > 0);
						int32_t succNodeG = bestNodeG + cost;
						if (succMapNode->flags & MOVEFLAG_OPEN)
						{
							//----------------------------------------------
							// This node is already in the OPEN queue to be
							// be processed. Let's check if we have a better
							// path thru this route...
							if (succNodeG < succMapNode->g)
							{
								//----------------------------
								// This new path is cheaper...
								succMapNode->parent = dirToParent;
								succMapNode->g = succNodeG;
								succMapNode->fPrime = succNodeG + succMapNode->hPrime;
								int32_t openIndex =
									openList->find(succRow * MAX_MAPWIDTH + succCol);
								if (!openIndex)
								{
									wchar_t s[128];
									sprintf(s,
										"MoveMap.calcEscapePath: Cannot find "
										"movemap node [%d, %d, %d, %d] for "
										"change\n",
										succRow, succCol, succRow * MAX_MAPWIDTH + succCol, dir);
#ifdef USE_OBJECTS
									DebugOpenList(s);
#endif
									gosASSERT(openIndex != nullptr);
								}
								else
									openList->change(openIndex, succMapNode->fPrime);
							}
						}
						else if (succMapNode->flags & MOVEFLAG_CLOSED)
						{
							//-------------------------------------------------
							// This path may be better than this node's current
							// path. If so, we may have to propogate thru...
							if (succNodeG < succMapNode->g)
							{
								//----------------------------
								// This new path is cheaper...
								succMapNode->parent = dirToParent;
								//------------------------------------------
								// Now, we have to propogate the new cost...
								propogateCostJUMP(succRow, succCol, cost, bestNodeG);
							}
						}
						else
						{
							//-------------------------------------------------
							// This node is neither OPEN nor CLOSED, so toss it
							// into the OPEN list...
							succMapNode->parent = dirToParent;
							succMapNode->g = succNodeG;
							succMapNode->fPrime = succNodeG + succMapNode->hPrime;
							PQNode succPQNode;
							succPQNode.key = succMapNode->fPrime;
							succPQNode.id = succRow * MAX_MAPWIDTH + succCol;
							succPQNode.row = succRow;
							succPQNode.col = succCol;
#ifdef _DEBUG
							int32_t insertErr =
#endif
								openList->insert(succPQNode);
							gosASSERT(insertErr == NO_ERROR);
							succMapNode->setFlag(MOVEFLAG_OPEN);
						}
					}
				}
			}
		}
	}
#ifdef DEBUG_MOVE_MAP
	if (debugMoveMap)
	{
		MechFile* pathDebugFile = new MechFile;
		pathDebugFile->create("mm.dbg");
		writeDebug(pathDebugFile);
		pathDebugFile->close();
		delete pathDebugFile;
		pathDebugFile = nullptr;
	}
#endif
#if 0 // Redo when bridges are done
	if(ClearBridgeTiles)
	{
		overlayWeightTable[OVERLAY_WATER_BRIDGE_NS * terrain_const::MAPCELL_DIM * terrain_const::MAPCELL_DIM + 1] += COST_BLOCKED;
		overlayWeightTable[OVERLAY_WATER_BRIDGE_NS * terrain_const::MAPCELL_DIM * terrain_const::MAPCELL_DIM + 4] += COST_BLOCKED;
		overlayWeightTable[OVERLAY_WATER_BRIDGE_NS * terrain_const::MAPCELL_DIM * terrain_const::MAPCELL_DIM + 7] += COST_BLOCKED;
		overlayWeightTable[OVERLAY_WATER_BRIDGE_EW * terrain_const::MAPCELL_DIM * terrain_const::MAPCELL_DIM + 1] += COST_BLOCKED;
		overlayWeightTable[OVERLAY_WATER_BRIDGE_EW * terrain_const::MAPCELL_DIM * terrain_const::MAPCELL_DIM + 4] += COST_BLOCKED;
		overlayWeightTable[OVERLAY_WATER_BRIDGE_EW * terrain_const::MAPCELL_DIM * terrain_const::MAPCELL_DIM + 7] += COST_BLOCKED;
		overlayWeightTable[OVERLAY_RAILROAD_WATER_BRIDGE_NS * terrain_const::MAPCELL_DIM * terrain_const::MAPCELL_DIM + 1] += COST_BLOCKED;
		overlayWeightTable[OVERLAY_RAILROAD_WATER_BRIDGE_NS * terrain_const::MAPCELL_DIM * terrain_const::MAPCELL_DIM + 4] += COST_BLOCKED;
		overlayWeightTable[OVERLAY_RAILROAD_WATER_BRIDGE_NS * terrain_const::MAPCELL_DIM * terrain_const::MAPCELL_DIM + 7] += COST_BLOCKED;
		overlayWeightTable[OVERLAY_RAILROAD_WATER_BRIDGE_EW * terrain_const::MAPCELL_DIM * terrain_const::MAPCELL_DIM + 1] += COST_BLOCKED;
		overlayWeightTable[OVERLAY_RAILROAD_WATER_BRIDGE_EW * terrain_const::MAPCELL_DIM * terrain_const::MAPCELL_DIM + 4] += COST_BLOCKED;
		overlayWeightTable[OVERLAY_RAILROAD_WATER_BRIDGE_EW * terrain_const::MAPCELL_DIM * terrain_const::MAPCELL_DIM + 7] += COST_BLOCKED;
	}
#endif
	if (goalFound)
	{
		//-------------------------------------------
		// First, let's count how int32_t the path is...
		int32_t curRow = goalCell[0] = (int32_t)bestRow;
		int32_t curCol = goalCell[1] = (int32_t)bestCol;
		int32_t numCells = 0;
		while ((curRow != startR) || (curCol != startC))
		{
			numCells += 1;
			int32_t cellOffsetIndex = map[curRow * maxwidth + curCol].parent * 2;
			// if ((cellOffsetIndex < 0) || (cellOffsetIndex > 14))
			//	OutputDebugString("PathFinder: whoops\n");
			curRow += cellShift[cellOffsetIndex++];
			curCol += cellShift[cellOffsetIndex];
		}
		//---------------------------------------------------------------
		// If our goal is a door, the path will be one step longer, since
		// we need to walk "thru" the door...
		if (doorDirection != -1)
			numCells++;
#ifdef _DEBUG
		if (numCells > MAX_STEPS_PER_MOVEPATH)
		{
			MechFile* pathDebugFile = new MechFile;
			pathDebugFile->create("longpath.dbg");
			wchar_t s[512];
			sprintf(s,
				"Path Too Long: %d steps (Max = %d) (please save longpath.dbg "
				"file:)",
				numCells, MAX_STEPS_PER_MOVEPATH);
			pathDebugFile->writeString(s);
			pathDebugFile->writeString("\n\n");
			writeDebug(pathDebugFile);
			pathDebugFile->close();
			delete pathDebugFile;
			pathDebugFile = nullptr;
			gosASSERT(numCells <= MAX_STEPS_PER_MOVEPATH);
		}
#endif
		//-----------------------------
		// Now, let's build the path...
		//-------------------------------
		// Grab the path and return it...
		path->init();
		if (numCells)
		{
#ifdef _DEBUG
			int32_t maxSteps = path->init(numCells);
			if (maxSteps > -1)
			{
				MechFile* pathDebugFile = new MechFile;
				pathDebugFile->create("longpath.dbg");
				wchar_t s[512];
				sprintf(s, "New Longest Path: %d steps (Max = %d)\n\n", numCells,
					MAX_STEPS_PER_MOVEPATH);
				pathDebugFile->writeString(s);
				writeDebug(pathDebugFile);
				pathDebugFile->close();
				delete pathDebugFile;
				pathDebugFile = nullptr;
			}
#endif
			path->target = target;
			path->cost = map[bestRow * maxwidth + bestCol].g;
			curRow = (int32_t)bestRow;
			curCol = (int32_t)bestCol;
			int32_t curCell = numCells;
			if (doorDirection == -1)
			{
				if (goalWorldPos)
				{
					goalWorldPos->x = (float)(ULc + bestCol) * Terrain::worldUnitsPerCell + Terrain::worldUnitsPerCell / 2 - Terrain::worldUnitsMapSide / 2;
					goalWorldPos->y = (Terrain::worldUnitsMapSide / 2) - ((float)(ULr + bestRow) * Terrain::worldUnitsPerCell) - Terrain::worldUnitsPerCell / 2;
					goalWorldPos->z = (float)0; // How do we get the elevation
						// for this point? Do we care?
					path->goal = *goalWorldPos;
				}
				else
					path->goal = goal;
			}
			else
			{
				//--------------------------------------------------
				// It's a door, so it's the last on the path list...
				curCell--;
				path->setDirection(curCell, /*reverseShift[*/ doorDirection * 2 /*]*/);
				int32_t doorR = bestRow + adjTile[doorDirection][0];
				int32_t doorC = bestCol + adjTile[doorDirection][1];
				goalCell[0] = ULr + doorR;
				goalCell[1] = ULc + doorC;
				Stuff::Vector3D stepDest;
				stepDest.x = (float)(ULc + doorC) * Terrain::worldUnitsPerCell + Terrain::worldUnitsPerCell / 2 - Terrain::worldUnitsMapSide / 2;
				stepDest.y = (Terrain::worldUnitsMapSide / 2) - ((float)(ULr + doorR) * Terrain::worldUnitsPerCell) - Terrain::worldUnitsPerCell / 2;
				stepDest.z = (float)0; // How do we get the elevation for this
					// point? Do we care?
				path->setDestination(curCell, stepDest);
				path->setDistanceToGoal(curCell, 0.0);
				path->setCell(curCell, goalCell[0], goalCell[1]);
				path->goal = stepDest;
				if (goalWorldPos)
				{
					//--------------------------------------------------------------------
					// We didn't know the exact goal world pos coming into this,
					// since are goal was an area door....
					*goalWorldPos = stepDest;
				}
			}
			//---------------------------------------------------------
			// We need to set this table up our first time thru here...
			static bool setTable = false;
			if (!setTable)
			{
				float cellLength = Terrain::worldUnitsPerCell * metersPerWorldUnit;
				for (size_t i = 0; i < NUM_CELL_OFFSETS; i++)
				{
					float distance = agsqrt(cellShift[i * 2], cellShift[i * 2 + 1]) * cellLength;
					cellShiftDistance[i] = distance;
				}
				setTable = true;
			}
			while ((curRow != startR) || (curCol != startC))
			{
				curCell--;
				int32_t parent = reverseShift[map[curRow * maxwidth + curCol].parent];
				if (parent > 7)
					path->setDirection(curCell, parent);
				else
					path->setDirection(curCell, parent);
				Stuff::Vector3D stepDest;
				int32_t cell[2];
				cell[0] = ULr + curRow;
				cell[1] = ULc + curCol;
				stepDest.x = (float)(cell[1]) * Terrain::worldUnitsPerCell + Terrain::worldUnitsPerCell / 2 - Terrain::worldUnitsMapSide / 2;
				stepDest.y = (Terrain::worldUnitsMapSide / 2) - ((float)(cell[0]) * Terrain::worldUnitsPerCell) - Terrain::worldUnitsPerCell / 2;
				stepDest.z = (float)0; // How do we get the elevation for this
					// point? Do we care?
				if (curCell == (numCells - 1) && (parent <= 7))
					path->setDistanceToGoal(curCell, 0.0);
				else
					path->setDistanceToGoal(curCell,
						cellShiftDistance[path->getDirection(curCell + 1)] + path->getDistanceToGoal(curCell + 1));
				path->setDestination(curCell, stepDest);
				path->setCell(curCell, cell[0], cell[1]);
				map[curRow * maxwidth + curCol].setFlag(MOVEFLAG_STEP);
				int32_t cellOffsetIndex = map[curRow * maxwidth + curCol].parent << 1;
				curRow += cellShift[cellOffsetIndex++];
				curCol += cellShift[cellOffsetIndex];
				// calcAdjNode(curRow, curCol, map[curRow * maxCellwidth +
				// curCol].parent);
			}
#ifdef BLOCKED_PATH_TEST
			//---------------------------------------------------------------------
			// Let's test every path point to make sure it is not a blocked
			// cell...
			for (size_t i = 0; i < path->numSteps; i++)
			{
				int32_t tile[2], cell[2];
				GameMap->worldToMapPos(
					path->stepList[i].destination, tile[0], tile[1], cell[0], cell[1]);
				bool cellPassable = GameMap->cellPassable(tile[0], tile[0], cell[0], cell[1]);
				if (!cellPassable)
					cellPassable = GameMap->cellPassable(tile[0], tile[0], cell[0], cell[1]);
				Assert(cellPassable || (i < 3), 0, " Bad Path Point ");
			}
#endif
		}
#ifdef TIME_PATH
		QueryPerformanceCounter(calcStop);
		calcTime = float(calcStop.LowPart - calcStart.LowPart) / float(countsPerSecond.LowPart);
		if (debugger && (scenarioTime > 0.0))
		{
			wchar_t s[50];
			sprintf(s, "path calc: %.4f\n", calcTime);
			OutputDebugString(s);
			debugger->print(s);
		}
#endif
		return (path->numSteps);
	}
#ifdef TIME_PATH
	QueryPerformanceCounter(calcStop);
	calcTime = float(calcStop.LowPart - calcStart.LowPart) / float(countsPerSecond.LowPart);
	if (debugger && (scenarioTime > 0.0))
	{
		wchar_t s[50];
		sprintf(s, "path calc: %.4f", calcTime);
		debugger->print(s);
	}
#endif
	return (0);
}

//---------------------------------------------------------------------------

void MoveMap::writeDebug(MechFile* debugFile)
{
	wchar_t outString[512];
	sprintf(outString, "Time = %.6f\n\n", calcTime);
	debugFile->writeString(outString);
	sprintf(outString, "Start = (%d, %d)\n", startR, startC);
	debugFile->writeString(outString);
	sprintf(outString, "Goal = (%d, %d)\n", goalR, goalC);
	debugFile->writeString(outString);
	strcpy(outString, "\n");
	debugFile->writeString(outString);
	debugFile->writeString("PARENT:\n");
	debugFile->writeString("-------\n");
	int32_t r;
	for (r = 0; r < height; r++)
	{
		outString[0] = nullptr;
		wchar_t numStr[10];
		for (size_t c = 0; c < width; c++)
		{
			/*if ((goalR == r) && (goalC == c))
				sprintf(numStr, "G");
			else*/
			if ((startR == r) && (startC == c))
				sprintf(numStr, "S");
			else if (map[r * maxwidth + c].parent == -1)
				sprintf(numStr, ".");
			else if (map[r * maxwidth + c].flags & MOVEFLAG_STEP)
				sprintf(numStr, "X");
			else
				sprintf(numStr, "%d", map[r * maxwidth + c].parent);
			strcat(outString, numStr);
		}
		strcat(outString, "\n");
		debugFile->writeString(outString);
	}
	debugFile->writeString("\n");
	debugFile->writeString("MAP:\n");
	debugFile->writeString("-------\n");
	for (r = 0; r < height; r++)
	{
		outString[0] = nullptr;
		wchar_t numStr[10];
		for (size_t c = 0; c < width; c++)
		{
			if ((goalR == r) && (goalC == c))
				sprintf(numStr, "G");
			else if ((startR == r) && (startC == c))
				sprintf(numStr, "S");
			else if (map[r * maxwidth + c].cost == clearCost)
				sprintf(numStr, ".");
			else if (map[r * maxwidth + c].cost >= COST_BLOCKED)
				sprintf(numStr, " ");
			else if (map[r * maxwidth + c].cost < 256)
				sprintf(numStr, "%x", map[r * maxwidth + c].cost);
			strcat(outString, numStr);
		}
		strcat(outString, "\n");
		debugFile->writeString(outString);
	}
	debugFile->writeString("\n");
	debugFile->writeString("PATH:\n");
	debugFile->writeString("-------\n");
	for (r = 0; r < height; r++)
	{
		outString[0] = nullptr;
		wchar_t numStr[10];
		for (size_t c = 0; c < width; c++)
		{
			if ((goalR == r) && (goalC == c))
				sprintf(numStr, "G");
			else if ((startR == r) && (startC == c))
				sprintf(numStr, "S");
			else if (map[r * maxwidth + c].flags & MOVEFLAG_STEP)
				sprintf(numStr, "*");
			else if (map[r * maxwidth + c].cost == clearCost)
				sprintf(numStr, ".");
			else if (map[r * maxwidth + c].cost >= COST_BLOCKED)
				sprintf(numStr, " ");
			else
				sprintf(numStr, "%d", map[r * maxwidth + c].cost);
			strcat(outString, numStr);
		}
		strcat(outString, "\n");
		debugFile->writeString(outString);
	}
	debugFile->writeString("\n");
}

//---------------------------------------------------------------------------

void MoveMap::destroy(void)
{
	if (map)
	{
		systemHeap->Free(map);
		map = nullptr;
	}
	if (mapRowStartTable)
	{
		systemHeap->Free(mapRowStartTable);
		mapRowStartTable = nullptr;
	}
	if (mapRowTable)
	{
		systemHeap->Free(mapRowTable);
		mapRowTable = nullptr;
	}
	if (mapColTable)
	{
		systemHeap->Free(mapColTable);
		mapColTable = nullptr;
	}
}

//***************************************************************************
