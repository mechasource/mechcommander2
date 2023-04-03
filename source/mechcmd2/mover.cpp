//******************************************************************************************
//
//	mover.cpp - This file contains the Mover Class code
//
//	MechCommander 2
//
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//
#include "stdinc.h"

#ifndef MCLIB_h
#include "mclib.h"
#endif

#ifndef GAMEOBJ_H
#include "gameobj.h"
#endif

#ifndef OBJMGR_H
#include "objmgr.h"
#endif

#include "mover.h"
#ifndef MOVE_H
#include "move.h"
#endif

#ifndef MECH_H
#include "mech.h"
#endif

#ifndef GVEHICL_H
#include "gvehicl.h"
#endif

#ifndef MECH3D_H
#include "mech3d.h"
#endif

#ifndef CMPONENT_H
#include "cmponent.h"
#endif

#ifndef WARRIOR_H
#include "warrior.h"
#endif

#ifdef USE_DEBRIS
#ifndef DEBRIS_H
#include "debris.h"
#endif
#endif

#ifndef GAMELOG_H
#include "gamelog.h"
#endif

#ifndef TACORDR_H
#include "tacordr.h"
#endif

#include "gamesound.h"
//#ifndef SOUNDSYS_H
//#include "soundsys.h"
//#endif

//#ifndef SOUNDS_H
//#include "sounds.h"
//#endif

#ifndef COLLSN_H
#include "collsn.h"
#endif

#ifndef MECHCLASS_H
#include "mechclass.h"
#endif

#ifndef UNITDESG_H
#include "unitdesg.h"
#endif

#ifndef MULTPLYR_H
#include "multplyr.h"
#endif

#ifndef TEAM_H
#include "team.h"
#endif

#ifndef COMNDR_H
#include "comndr.h"
#endif

#ifndef GROUP_H
#include "group.h"
#endif

#ifndef CONTACT_H
#include "contact.h"
#endif

#ifndef TRIGGER_H
#include "trigger.h"
#endif

#ifndef MISSION_H
#include "mission.h"
#endif

#ifndef LOGISTICSPILOT_H
#include "logisticspilot.h"
#endif

//--------
// DEFINES
#define GOALMAP_CELL_DIM 61

//--------
// GLOBALS
extern int32_t SimpleMovePathRange;
extern int32_t GroupMoveTrailLen[2];
extern int32_t tileMulMAPCELL_DIM[MAX_MAP_CELL_WIDTH];

extern int32_t RamObjectWID;

int32_t MaxMoveGoalChecks = 60;
float DelayedOrderTime = 1.0;
float GroupOrderGoalOffset = 127.0;

// BaseObjectPtr				MoverRoster[MAX_MOVER_PART_ID - MIN_MOVER_PART_ID +
// 1];

extern MoveMapPtr PathFindMap[2 /*NUM_PATHMAPS*/];
int32_t goalMap[GOALMAP_CELL_DIM * GOALMAP_CELL_DIM];
int32_t goalMapRowStart[GOALMAP_CELL_DIM] = {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1};
wchar_t goalMapRowCol[GOALMAP_CELL_DIM * GOALMAP_CELL_DIM][2];
int32_t Mover::numMovers = 0;
SortListPtr Mover::sortList = nullptr;

extern float DefaultAttackRadius;

extern float WeaponFireModifiers[NUM_WEAPONFIRE_MODIFIERS];
extern void
DebugWeaponFireChunk(
	WeaponFireChunkPtr chunk1, WeaponFireChunkPtr chunk2, GameObjectPtr attacker);

//--------
// EXTERNS

// extern ObjectMapPtr			GameObjectMap;
extern uint32_t NextIdNumber;

#ifdef USE_SOUNDS
extern bool friendlydestroyed;
extern bool enemydestroyed;
#endif

extern TeamPtr homeTeam;
extern bool JumpOnBlocked;
extern bool FindingEscapePath;

// extern float				FireOddsTable[FireOddsType::count];
extern float RankVersusChassisCombatModifier[WarriorRank::numberofranks][NUM_MECH_CLASSES];
float WeaponFireModifiers[NUM_WEAPONFIRE_MODIFIERS] = {0.0, // Short Range To Target
	-10.0, // Medium Range To Target
	-20.0, // Long Range To Target
	-98.0, // Aimed Shot To Head
	-85.0, // Aimed Shot To Torso
	-88.0, // Aimed Shot To Arm/Leg
	50.0, // Target is Stationary
	0.0, // Green/Light
	-5.0, // Green/Medium
	-15.0, // Green/Heavy
	-25.0, // Green/Assault
	5.0, // Regular/Light
	0.0, // Regular/Medium
	-5.0, // Regular/Heavy
	-15.0, // Regular/Assault
	10.0, // Veteran/Light
	5.0, // Veteran/Medium
	0.0, // Veteran/Heavy
	-5.0, // Veteran/Assault
	15.0, // Elite/Light
	10.0, // Elite/Medium
	5.0, // Elite/Heavy
	0.0, // Elite/Assault
	0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0};

extern float WarriorRankScale[WarriorRank::numberofranks];
extern std::wstring_view SpecialtySkillsTable[NUM_SPECIALTY_SKILLS];

float WeaponSpecialistModifier = 20.0f;

extern float MovementUpdateFrequency;
extern float CombatUpdateFrequency;
extern float CommandUpdateFrequency;
extern float PilotCheckUpdateFrequency;
extern int32_t PilotCheckModifierTable[2];
// extern float				ElementalTargetNoJumpDistance;
extern int32_t MechSalvageChance;
extern float MapCellDiagonal;

// extern int32_t OverlayWeightIndex[NUM_OVERLAY_TYPES];

extern int32_t NumLocationCriticalSpaces[NUM_BODY_LOCATIONS];

extern GlobalMapPtr GlobalMoveMap[3];

extern int32_t adjCellTable[terrain_const::MAPCELL_DIM * terrain_const::MAPCELL_DIM][8][4];

extern bool useUnlimitedAmmo;

extern bool CantBlowSalvage;
int32_t TargetRolo = -1;
WeaponFireChunk CurMoverWeaponFireChunk;

extern std::wstring_view ExceptionGameMsg;
extern wchar_t ChunkDebugMsg[5120];

extern wchar_t OverlayIsBridge[NUM_OVERLAY_TYPES];

float GroundVehiclePivotYawMultiplier = 0.25;

extern bool friendlydestroyed;
extern bool enemydestroyed;

extern GameLog* CombatLog;

//**********************************************************************************

float Mover::newThreatMultiplier = 3.0;
float Mover::marginOfError[2] = {5.0, 10.0};
float Mover::refitRange;
float Mover::refitTime;
float Mover::refitCostArray[NUM_COSTS][NUM_FIXERS];
float Mover::refitAmount;
float Mover::recoverRange;
float Mover::recoverTime;
float Mover::recoverCost;
float Mover::recoverAmount;
int32_t Mover::IndirectFireWeapons[20] = {
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1};

int32_t Mover::AreaEffectWeapons[20] = {
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1};

bool Mover::inRecoverUpdate = false;

int32_t StatusChunkUnpackErr = 0;

wchar_t AttackParameters[14][3] = {
	{AttackType::destroy, FireRangeType::optimal, 1}, // DESTROY
	{AttackType::destroy, FireRangeType::shortest, 0}, // STOP AND HOLD FIRE
	{AttackType::destroy, FireRangeType::shortest, 1}, // DESTROY - SR
	{AttackType::destroy, FireRangeType::medium, 1}, // DESTROY - MR
	{AttackType::destroy, FireRangeType::extended, 1}, // DESTROY - LR
	{AttackType::destroy, FireRangeType::medium, 0}, // DESTROY FROM HERE
	{AttackType::destroy, FireRangeType::medium, 1}, // DESTROY ALL OUT
	{AttackType::disable, FireRangeType::medium, 1}, // DISABLE
	{AttackType::disable, FireRangeType::shortest, 0}, // STOP AND HOLD FIRE
	{AttackType::disable, FireRangeType::shortest, 1}, // DISABLE - SR
	{AttackType::disable, FireRangeType::medium, 1}, // DISABLE - MR
	{AttackType::disable, FireRangeType::extended, 1}, // DISABLE - LR
	{AttackType::disable, FireRangeType::medium, 0}, // DISABLE FROM HERE
	{AttackType::disable, FireRangeType::medium, 1} // DISABLE MAX SALVAGE
};

int32_t rearAttackTable[NUM_DIRECTIONS][3][2] = {{{1, 0}, {1, -1}, {0, 1}},
	{{1, -1}, {0, -1}, {1, 1}}, {{0, -1}, {-1, -1}, {1, 0}}, {{-1, -1}, {-1, 0}, {1, -1}},
	{{-1, 0}, {-1, 1}, {0, -1}}, {{-1, 1}, {0, 1}, {-1, -1}}, {{1, 0}, {1, 1}, {-1, 0}},
	{{1, 1}, {1, 0}, {-1, 1}}};

//---------------------------------------------------------------------------
// Game System Tweakable Data
//---------------------------------------------------------------------------

float WeaponRange[FireRangeType::count] = {250, 500, 1000};

float WeaponRanges[NUM_WEAPON_RANGE_TYPES][2] = {{0, 100}, {50, 150}, {100, 225}, {0, 0}, {0, 0}};

float OptimalRangePoints[NUM_WEAPON_RANGE_TYPES] = {
	25, // Short
	125, // Med
	200, // Long
	75, // Short-Med
	125 // Med-Long
};

bool OptimalRangePointInRange[NUM_WEAPON_RANGE_TYPES][3];

uint8_t OptimalRangeTieTable[32] = {255, 0, 1, 1, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3, 4, 4, 4, 4, 4,
	4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4};

float DefaultAttackRange = 75.0;

float DisableAttackModifier = 10.0;
float DisableGunneryModifier = 5.0;
float SalvageAttackModifier = 30.0;

float PilotingCheckFactor = 1.0;

int32_t hitLevel[2] = {
	10, // weak hit is damage of <10
	20 // moderate hit is damage <20, else heavy hit
};

int32_t ClusterSizeSRM = 2; // eventually, let's just kill clusters!
int32_t ClusterSizeLRM = 5; // eventually, let's just kill clusters!

int32_t AntiMissileSystemStats[2][2] = {
	{1, 2}, // IS
	{2, 2} // CLAN
};

float PilotCheckHalfRate = 5.0;

float DamageRateFrequency = 10.0;

byte AttitudeEffect[AttitudeType::count][6] = {{50, 75, 5, -2, 0, 3}, {40, 60, 10, -1, 1, 5},
	{30, 50, 15, 0, 2, 10}, {20, 40, 20, 1, 3, 15}, {10, 25, 25, 1, 4, 20}, {0, 0, 32, 2, 5, 128}};

// extern bool SensorAutomaticSuccess;
// extern float SensorModifier[8];
float SensorBaseChance = 50.0;
float SensorSkillFactor = 10.0;

extern int32_t DebugMovePathType;

#ifdef USEHEAT
float SensorHeatMultiplier = 5.0;
float SensorWeaponHeatMultiplier = 5.0;
float SensorHeatSourceModifiers[2] = {10.0, -10.0};
extern float WeaponHeatCheckDelay;
#endif

float SensorBlockingObjectModifier = -5.0;
float SensorShutDownMechModifier = -50.0;
float SensorRangeModifier[4][2] = {{0.25f, 50.0}, {0.15f, 20.0}, {0.35f, 0.0}, {0.25f, -25.0}};
float SensorSizeModifier[3][2] = {{25.0, -10.0}, {60.0, 0.0}, {999.0, 15.0}};
float SensorBlockingTerrain[2] = {-25.0, -5.0};

int32_t AimedFireAbort;
int32_t AimedFireHitTable[3];
float FireArc[3];

// bool LongRangeMovementEnabled[MAX_TEAMS];

extern wchar_t ProfessionalismOffsetTable[NUM_OFFSET_RANGES][2];
extern wchar_t DecorumOffsetTable[NUM_OFFSET_RANGES][2];
extern wchar_t AmmoConservationModifiers[2][2];
// extern int32_t OverlayWeightTable[NUM_OVERLAY_WEIGHT_CLASSES *
// NUM_OVERLAY_TYPES * terrain_const::MAPCELL_DIM * terrain_const::MAPCELL_DIM];  extern int32_t
// OverlayWeightIndex[NUM_OVERLAY_TYPES];  extern float MoveMarginOfError[2];
extern float MoveTimeOut;
extern float MoveYieldTime;
extern int32_t DefaultMechJumpCost;

extern wchar_t SensorSkillMoveRange[4];
extern float SensorSkillMoveFactor[8];

float SkillTry[4];
float SkillSuccess[4];
float KillSkill[6];
float WeaponHit;
float SensorSkill;

int32_t MoveChunk::err = 0;

float MaxStationaryTime = 0.0;
float MaxTimeRevealed = 0.0f;

#define CLAN_SEEN_FLAG 2
#define IS_SEEN_FLAG 1

extern float worldUnitsPerMeter;
wchar_t Mover::optimalCells[MAX_ATTACK_CELLRANGE][MAX_ATTACK_INCREMENTS][2];
int32_t Mover::numOptimalIncrements = 0;
int16_t Mover::rangedCellsIndices[MAX_ATTACK_CELLRANGE][2];
wchar_t Mover::rangedCells[RANGED_CELLS_DIM][2];
TriggerAreaManager* Mover::triggerAreaMgr = nullptr;

uint32_t Mover::holdFireIconHandle = 0;

float HeadShotModifier = 0.0f;
float ArmShotModifier = 0.0f;
float LegShotModifier = 0.0f;

bool CalcValidAreaTable = true;

void DEBUGWINS_print(std::wstring_view s, int32_t window);

//***************************************************************************
// MISC routines
//***************************************************************************

inline std::unique_ptr<Mover>
getMoverFromHandle(int32_t handle)
{
	return (dynamic_cast<std::unique_ptr<Mover>>(ObjectManager->get(handle)));
}

//***************************************************************************
// MOVECHUNK class
//***************************************************************************

#define MOVECHUNK_CELLPOS_BITS 9
#define MOVECHUNK_NUMSTEPS_BITS 2
#define MOVECHUNK_RUN_BITS 1
#define MOVECHUNK_MOVING_BITS 1
#define MOVECHUNK_STEP_BITS 3

#define MOVECHUNK_CELLPOS_MASK 0x000001FF
#define MOVECHUNK_NUMSTEPS_MASK 0x00000003
#define MOVECHUNK_RUN_MASK 0x00000001
#define MOVECHUNK_MOVING_MASK 0x00000001
#define MOVECHUNK_STEP_MASK 0x00000007

#define MOVECHUNK_NUM_STEPS 3

//---------------------------------------------------------------------------

void DebugMoveChunk(std::unique_ptr<Mover> mover, MoveChunkPtr chunk1, MoveChunkPtr chunk2)
{
	ChunkDebugMsg[0] = nullptr;
	wchar_t outString[512];
	if (mover)
	{
		sprintf(outString, "Mover = %s (%d)\n", mover->getName(), mover->getPartId());
		strcat(ChunkDebugMsg, outString);
		sprintf(outString, "Mover World Pos = (%.4f, %.4f, %.4f)\n", mover->getPosition().x,
			mover->getPosition().y, mover->getPosition().z);
		strcat(ChunkDebugMsg, outString);
		int32_t cellPos[2];
		mover->getCellPosition(cellPos[0], cellPos[1]);
		sprintf(outString, "Mover Obj Cell Pos = [%d, %d]\n", cellPos[0], cellPos[1]);
		strcat(ChunkDebugMsg, outString);
		if (mover->getPilot() == nullptr)
			strcat(ChunkDebugMsg, "nullptr pilot!\n");
		if ((mover->getObjectClass() == BATTLEMECH) && ((BattleMechPtr)mover)->inJump)
		{
			int32_t jumpDest[2];
			land->worldToCell(((BattleMechPtr)mover)->jumpGoal, jumpDest[0], jumpDest[1]);
			sprintf(outString, "Jumping to [%d, %d]\n", jumpDest[0], jumpDest[1]);
			strcat(ChunkDebugMsg, outString);
		}
		strcat(ChunkDebugMsg, "\n");
	}
	if (chunk1)
	{
		strcat(ChunkDebugMsg, "CHUNK1\n");
		for (size_t i = 0; i < 4; i++)
		{
			sprintf(outString, "stepPos[%d] = (%d, %d)\n", i, chunk1->stepPos[i][0],
				chunk1->stepPos[i][1]);
			strcat(ChunkDebugMsg, outString);
		}
		sprintf(outString, "stepRelPos = %d, %d, %d\n", chunk1->stepRelPos[0],
			chunk1->stepRelPos[1], chunk1->stepRelPos[2]);
		strcat(ChunkDebugMsg, outString);
		sprintf(outString, "numSteps = %d\n", chunk1->numSteps);
		strcat(ChunkDebugMsg, outString);
		sprintf(outString, "run = %c\n", chunk1->run ? 'T' : 'F');
		strcat(ChunkDebugMsg, outString);
		sprintf(outString, "moving = %c\n", chunk1->moving ? 'T' : 'F');
		strcat(ChunkDebugMsg, outString);
	}
	if (chunk2)
	{
		strcat(ChunkDebugMsg, "\nCHUNK2\n");
		for (size_t i = 0; i < 4; i++)
		{
			sprintf(outString, "stepPos[%d] = (%d, %d)\n", i, chunk2->stepPos[i][0],
				chunk2->stepPos[i][1]);
			strcat(ChunkDebugMsg, outString);
		}
		sprintf(outString, "stepRelPos = %d, %d, %d\n", chunk2->stepRelPos[0],
			chunk2->stepRelPos[1], chunk2->stepRelPos[2]);
		strcat(ChunkDebugMsg, outString);
		sprintf(outString, "numSteps = %d\n", chunk2->numSteps);
		strcat(ChunkDebugMsg, outString);
		sprintf(outString, "moving = %c\n", chunk2->moving ? 'T' : 'F');
		strcat(ChunkDebugMsg, outString);
		sprintf(outString, "run = %c\n", chunk2->run ? 'T' : 'F');
		strcat(ChunkDebugMsg, outString);
	}
	File* debugFile = new File;
	debugFile->create("mvchunk.dbg");
	debugFile->writeString(ChunkDebugMsg);
	debugFile->close();
	delete debugFile;
	debugFile = nullptr;
	ExceptionGameMsg = ChunkDebugMsg;
}

//---------------------------------------------------------------------------

PVOID
MoveChunk::operator new(size_t ourSize)
{
	PVOID result = systemHeap->Malloc(ourSize);
	return (result);
}

//---------------------------------------------------------------------------

void MoveChunk::operator delete(PVOID us)
{
	systemHeap->Free(us);
}

//---------------------------------------------------------------------------

inline void
getAdjCell(int32_t row, int32_t col, int32_t dir, int32_t& adjRow, int32_t& adjCol)
{
	int32_t adjMod[8][2] = {{-1, 0}, {-1, 1}, {0, 1}, {1, 1}, {1, 0}, {1, -1}, {0, -1}, {-1, -1}};
	adjRow = row + adjMod[dir][0];
	adjCol = col + adjMod[dir][1];
}

//---------------------------------------------------------------------------

inline int32_t
cellDirToCell(
	int32_t fromCellRow, int32_t fromCellCol, int32_t toCellRow, int32_t toCellCol)
{
	//-------------------------------------------------
	// This routine assumes the two cells are adjacent!
	static int32_t deltaDir[3][3] = {{7, 0, 1}, {6, -1, 2}, {5, 4, 3}};
	int32_t rowCellDelta = toCellRow - fromCellRow + 1;
	int32_t colCellDelta = toCellCol - fromCellCol + 1;
	if ((rowCellDelta < 0) || (rowCellDelta > 2))
		return (-2);
	if ((colCellDelta < 0) || (colCellDelta > 2))
		return (-2);
	int32_t dir = deltaDir[rowCellDelta][colCellDelta];
	if (dir == -1)
		return (-2);
	return (dir);
}

//---------------------------------------------------------------------------

void MoveChunk::build(std::unique_ptr<Mover> mover, MovePathPtr path1, MovePathPtr path2)
{
	int32_t cellPos[2];
	mover->getCellPosition(cellPos[0], cellPos[1]);
	stepPos[0][0] = cellPos[0];
	stepPos[0][1] = cellPos[1];
	stepRelPos[0] = 0;
	stepRelPos[1] = 0;
	stepRelPos[2] = 0;
	numSteps = 1;
	moving = path1 && (path1->numSteps > 0);
	if (path1 && path1->numSteps > 0)
	{
		bool reachedEndOfPath1 = true;
		int32_t curStep = 1;
		if (path1->curStep < path1->numSteps)
		{
			reachedEndOfPath1 = false;
			int32_t curPath1Step = path1->curStep;
			bool startWithCurStep = false;
			if ((cellPos[0] == path1->stepList[curPath1Step].cell[0]) && (cellPos[1] == path1->stepList[curPath1Step].cell[1]))
				startWithCurStep = true;
			else
			{
				//--------------------------------------------------------------
				// Whacky case, since current object position is not next to the
				// next step in the path...
				int32_t relDir = cellDirToCell(cellPos[0], cellPos[1],
					path1->stepList[curPath1Step].cell[0], path1->stepList[curPath1Step].cell[1]);
				startWithCurStep = (relDir == -2);
				//			if (relDir == -2)
				//				OutputDebugString("***jumped cell***\n");
			}
			if (startWithCurStep)
			{
				//---------------------
				// step 0 = curStep
				// step 1 = curStep + 1
				// step 2 = curStep + 2
				// step 3 = curStep + 3
				stepPos[0][0] = path1->stepList[curPath1Step].cell[0];
				stepPos[0][1] = path1->stepList[curPath1Step].cell[1];
				curPath1Step++;
				numSteps = MOVECHUNK_NUM_STEPS;
				int32_t numStepsLeft = path1->numSteps - curPath1Step;
				if (numStepsLeft < MOVECHUNK_NUM_STEPS)
				{
					numSteps = numStepsLeft;
					reachedEndOfPath1 = true;
				}
				for (size_t i = 0; i < numSteps; i++)
				{
					stepPos[curStep][0] = path1->stepList[curPath1Step].cell[0];
					stepPos[curStep][1] = path1->stepList[curPath1Step].cell[1];
					if (curStep == 1)
						stepRelPos[curStep - 1] = cellDirToCell(
							stepPos[0][0], stepPos[0][1], stepPos[1][0], stepPos[1][1]);
					else
						stepRelPos[curStep - 1] = path1->stepList[curPath1Step].direction;
					curPath1Step++;
					curStep++;
				}
				//-----------------------------------------------------
				// Since we count the cur position as the first step...
				numSteps++;
			}
			else
			{
				//---------------------
				// step 0 = curPosition
				// step 1 = curStep
				// step 2 = curStep + 1
				// step 3 = curStep + 2
				numSteps = MOVECHUNK_NUM_STEPS;
				int32_t numStepsLeft = path1->numSteps - curPath1Step;
				if (numStepsLeft < MOVECHUNK_NUM_STEPS)
				{
					numSteps = numStepsLeft;
					reachedEndOfPath1 = true;
				}
				for (size_t i = 0; i < numSteps; i++)
				{
					stepPos[curStep][0] = path1->stepList[curPath1Step].cell[0];
					stepPos[curStep][1] = path1->stepList[curPath1Step].cell[1];
					if (curStep == 1)
						stepRelPos[curStep - 1] = cellDirToCell(
							stepPos[0][0], stepPos[0][1], stepPos[1][0], stepPos[1][1]);
					else
						stepRelPos[curStep - 1] = path1->stepList[curPath1Step].direction;
					curPath1Step++;
					curStep++;
				}
				//-----------------------------------------------------
				// Since we count the cur position as the first step...
				numSteps++;
			}
		}
		bool path2Continues =
			path2 && (path1->globalStep > -1) && (path2->globalStep == (path1->globalStep + 1));
		if (reachedEndOfPath1 && path2Continues && (path2->numStepsWhenNotPaused > 0))
		{
			//--------------------------------------------------------------
			// Splice second (queued) path into the chunk.
			// One assumption: if the first path is empty, it is assumed the
			// second path is irrelevant...
			//			Assert(curStep > 1, curStep, " MoveChunk.build: path2 and
			// curStep == 1 ");
			if (numSteps < (MOVECHUNK_NUM_STEPS + 1))
			{
				//-----------------------------------------------------
				// Let's fill up the remaining steps in this chunk with
				// steps from the second path...
				int32_t numAdditionalSteps = (MOVECHUNK_NUM_STEPS + 1) - numSteps;
				if (path2->numSteps < numAdditionalSteps)
					numAdditionalSteps = path2->numSteps;
				for (size_t i = 0; i < numAdditionalSteps; i++)
				{
					Assert(curStep <= MOVECHUNK_NUM_STEPS, curStep,
						" MoveChunk.build: path2 and bad curStep > "
						"MOVECHUNK_NUM_STEPS ");
					stepPos[curStep][0] = path2->stepList[i].cell[0];
					stepPos[curStep][1] = path2->stepList[i].cell[1];
					if (curStep == 1)
						stepRelPos[curStep - 1] = cellDirToCell(
							stepPos[0][0], stepPos[0][1], stepPos[1][0], stepPos[1][1]);
					else
						stepRelPos[curStep - 1] = path2->stepList[i].direction;
					curStep++;
				}
				numSteps += numAdditionalSteps;
				Assert(numSteps <= (MOVECHUNK_NUM_STEPS + 1), curStep,
					" MoveChunk.build: path2 and bad curStep > "
					"MOVECHUNK_NUM_STEPS ");
			}
		}
	}
	if ((numSteps < 1) || (numSteps > 4))
	{
		//-----------------------------------------------------------------
		// For now, a safety guard. However, it would be nice to catch this
		// sooner and avoid this HACK. EXPANSION DISK!
		stepPos[0][0] = cellPos[0];
		stepPos[0][1] = cellPos[1];
		stepRelPos[0] = 0;
		stepRelPos[1] = 0;
		stepRelPos[2] = 0;
		numSteps = 1;
	}
	run = mover->getPilot()->getMoveRun();
	if (run && (mover->getObjectClass() == BATTLEMECH))
		run = ((BattleMechPtr)mover)->canRun();
	data = 0;
}

//---------------------------------------------------------------------------

// DON'T USE THIS NOW, AS JUMPGOAL IS STORED IN THE STATUS CHUNK IF IT'S
// A JUMP ORDER!

void MoveChunk::build(std::unique_ptr<Mover> mover, Stuff::Vector3D jumpGoal)
{
	int32_t jumpCoords[2];
	land->worldToCell(jumpGoal, jumpCoords[0], jumpCoords[1]);
	stepPos[0][0] = jumpCoords[0];
	stepPos[0][1] = jumpCoords[1];
	stepRelPos[0] = 0;
	stepRelPos[1] = 0;
	stepRelPos[2] = 0;
	numSteps = 1;
	run = false;
	moving = false;
}

//---------------------------------------------------------------------------

#define DEBUG_MOVECHUNK

void MoveChunk::pack(std::unique_ptr<Mover> mover)
{
	data = stepPos[0][0];
	data <<= MOVECHUNK_CELLPOS_BITS;
	data |= stepPos[0][1];
	data <<= MOVECHUNK_NUMSTEPS_BITS;
	data |= (numSteps - 1);
	data <<= MOVECHUNK_RUN_BITS;
	if (run)
		data |= 1;
	data <<= MOVECHUNK_MOVING_BITS;
	if (moving)
		data |= 1;
	data <<= MOVECHUNK_STEP_BITS;
	data |= stepRelPos[0];
	data <<= MOVECHUNK_STEP_BITS;
	data |= stepRelPos[1];
	data <<= MOVECHUNK_STEP_BITS;
	data |= stepRelPos[2];
#ifdef DEBUG_MOVECHUNK
	//-------------------------
	// Lots'a error checking...
	if ((numSteps < 1) || (numSteps > 4))
	{
		DebugMoveChunk(mover, this, nullptr);
		wchar_t errMsg[1024];
		sprintf(errMsg, " MoveChunk.pack: bad numSteps %d (save mvchunk.dbg file) ", numSteps);
		Assert(false, numSteps, errMsg);
	}
#endif
}

//---------------------------------------------------------------------------

void MoveChunk::unpack(std::unique_ptr<Mover> mover)
{
	err = 0;
	stepRelPos[2] = (data & MOVECHUNK_STEP_MASK);
	data >>= MOVECHUNK_STEP_BITS;
	stepRelPos[1] = (data & MOVECHUNK_STEP_MASK);
	data >>= MOVECHUNK_STEP_BITS;
	stepRelPos[0] = (data & MOVECHUNK_STEP_MASK);
	data >>= MOVECHUNK_STEP_BITS;
	moving = ((data & MOVECHUNK_MOVING_MASK) != 0);
	data >>= MOVECHUNK_MOVING_BITS;
	run = ((data & MOVECHUNK_RUN_MASK) != 0);
	data >>= MOVECHUNK_RUN_BITS;
	numSteps = (data & MOVECHUNK_NUMSTEPS_MASK) + 1;
	data >>= MOVECHUNK_NUMSTEPS_BITS;
	stepPos[0][1] = (data & MOVECHUNK_CELLPOS_MASK);
	data >>= MOVECHUNK_CELLPOS_BITS;
	stepPos[0][0] = (data & MOVECHUNK_CELLPOS_MASK);
	//---------------------------------------------------------------
	// Finally, set the delta steps based upon the first step and the
	// relPos list...
	if ((numSteps < 1) || (numSteps > 4))
	{
		//--------------------------------------------------------------------
		// HACK!!!!!!!!!! All of this unpackerr stuff is a last minute hack...
		err = 1;
		return;
	}
	if (numSteps > 1)
	{
		for (size_t i = 0; i < (numSteps - 1); i++)
		{
			int32_t nextStep = i + 1;
			if ((i < 0) || (i > 2))
			{
				err = 2;
				return;
			}
			if ((stepRelPos[i] < 0) || (stepRelPos[i] > 7))
			{
				err = 3;
				return;
			}
			getAdjCell(stepPos[i][0], stepPos[i][1], stepRelPos[i], stepPos[nextStep][0],
				stepPos[nextStep][1]);
		}
	}
#ifdef DEBUG_MOVECHUNK
	//-------------------------
	// Lots'a error checking...
	if ((numSteps < 1) || (numSteps > 4))
	{
		DebugMoveChunk(mover, this, nullptr);
		wchar_t errMsg[1024];
		sprintf(errMsg, " MoveChunk.unpack: bad numSteps %d (save mvchunk.dbg file) ", numSteps);
		Assert(false, numSteps, errMsg);
	}
#endif
}

//---------------------------------------------------------------------------

bool MoveChunk::equalTo(std::unique_ptr<Mover> mover, MoveChunkPtr chunk)
{
	if (numSteps != chunk->numSteps)
	{
		DebugMoveChunk(mover, this, chunk);
		return (false);
	}
	if (run != chunk->run)
	{
		DebugMoveChunk(mover, this, chunk);
		return (false);
	}
	if (moving != chunk->moving)
	{
		DebugMoveChunk(mover, this, chunk);
		return (false);
	}
	for (size_t i = 0; i < numSteps; i++)
		if ((stepPos[i][0] != chunk->stepPos[i][0]) || (stepPos[i][1] != chunk->stepPos[i][1]))
		{
			DebugMoveChunk(mover, this, chunk);
			return (false);
		}
	for (i = 0; i < numSteps - 1; i++)
	{
		if (stepRelPos[i] != chunk->stepRelPos[i])
		{
			DebugMoveChunk(mover, this, chunk);
			return (false);
		}
	}
	return (true);
}

//---------------------------------------------------------------------------

void Mover::setMoveChunk(MovePathPtr path, MoveChunkPtr chunk)
{
	for (size_t i = 0; i < chunk->numSteps; i++)
	{
		PathStepPtr pathStep = &path->stepList[i];
		pathStep->cell[0] = chunk->stepPos[i][0];
		pathStep->cell[1] = chunk->stepPos[i][1];
		pathStep->distanceToGoal = 0.0;
		land->cellToWorld(pathStep->cell[0], pathStep->cell[1], pathStep->destination);
	}
	path->goal = path->stepList[chunk->numSteps - 1].destination;
	path->target.Zero();
	path->numSteps = path->numStepsWhenNotPaused = chunk->numSteps;
	path->curStep = 0;
	path->cost = -1;
	path->marked = false;
	path->globalStep = -1;
}

//***************************************************************************
// Status Chunk class
//***************************************************************************

void DebugStatusChunk(std::unique_ptr<Mover> mover, StatusChunkPtr chunk1, StatusChunkPtr chunk2)
{
	ChunkDebugMsg[0] = nullptr;
	wchar_t outString[512];
	if (mover)
	{
		sprintf(outString, "\nmover = %s (%d)\n", mover->name, mover->getPartId());
		strcat(ChunkDebugMsg, outString);
	}
	else
		strcat(ChunkDebugMsg, "\nmover = ???\n");
	if (chunk1)
	{
		strcat(ChunkDebugMsg, "\nCHUNK1\n");
		GameObjectPtr target = nullptr;
		Stuff::Vector3D targetpoint;
		targetpoint.Zero();
		bool isTargetPoint = false;
		if (chunk1->targetType == STATUSCHUNK_TARGET_MOVER)
			target = (GameObjectPtr)MPlayer->moverRoster[chunk1->targetId];
		else if (chunk1->targetType == STATUSCHUNK_TARGET_TERRAIN)
			target = ObjectManager->findByPartId(chunk1->targetId);
		else if (chunk1->targetType == STATUSCHUNK_TARGET_SPECIAL)
			target = ObjectManager->findByPartId(chunk1->targetId);
		else if (chunk1->targetType == STATUSCHUNK_TARGET_LOCATION)
		{
			targetpoint.x = (float)chunk1->targetCellRC[1] * Terrain::worldUnitsPerCell + Terrain::worldUnitsPerCell / 2 - Terrain::worldUnitsMapSide / 2;
			targetpoint.y = (Terrain::worldUnitsMapSide / 2) - ((float)chunk1->targetCellRC[0] * Terrain::worldUnitsPerCell) - Terrain::worldUnitsPerCell / 2;
			targetpoint.z = (float)land->getTerrainElevation(targetpoint);
			isTargetPoint = true;
		}
		if (target)
		{
			if (target->isMover())
			{
				sprintf(
					outString, "target = %s (%d)\n", ((std::unique_ptr<Mover>)target)->name, target->getPartId());
				strcat(ChunkDebugMsg, outString);
			}
			else
			{
				sprintf(outString, "target = objClass %d (%d)\n", target->getObjectClass(),
					target->getPartId());
				strcat(ChunkDebugMsg, outString);
			}
		}
		else if (isTargetPoint)
		{
			sprintf(outString, "target point = (%f, %f, %f)\n", targetpoint.x, targetpoint.y,
				targetpoint.z);
			strcat(ChunkDebugMsg, outString);
		}
		else if (chunk1->targetType == STATUSCHUNK_TARGET_NONE)
		{
			sprintf(outString, "target = NONE\n");
			strcat(ChunkDebugMsg, outString);
		}
		else
		{
			strcat(ChunkDebugMsg, "target = ???\n");
			if (chunk1->targetType == STATUSCHUNK_TARGET_TERRAIN)
			{
				//---------------------------------------------------------------------
				// If the TargetType is terrain, let's at least list all of the
				// terrain objects in this Block/Vertex...
				int32_t numObjsInTile = 0;
				int32_t partId = chunk1->targetId - chunk1->targetItemNumber;
				for (size_t i = 0; i < MAX_TERRAIN_TILE_ITEMS; i++)
				{
					GameObjectPtr curTarget = ObjectManager->findByPartId(partId + i);
					if (curTarget)
					{
						numObjsInTile++;
						sprintf(outString, "    %d: objClass %d (%d)\n", i,
							curTarget->getObjectClass(), curTarget->getPartId());
						strcat(ChunkDebugMsg, outString);
					}
				}
				if (numObjsInTile > 0)
				{
					sprintf(outString, "    There are %d terrain objects in this tile.\n",
						numObjsInTile);
					strcat(ChunkDebugMsg, outString);
				}
			}
		}
		sprintf(outString, "bodyState = %d\n", chunk1->bodyState);
		strcat(ChunkDebugMsg, outString);
		sprintf(outString, "targetType = %d\n", chunk1->targetType);
		strcat(ChunkDebugMsg, outString);
		sprintf(outString, "targetId = %d\n", chunk1->targetId);
		strcat(ChunkDebugMsg, outString);
		sprintf(outString, "targetBlockOrTrainNumber = %d\n", chunk1->targetBlockOrTrainNumber);
		strcat(ChunkDebugMsg, outString);
		sprintf(outString, "targetVertexOrCarNumber = %d\n", chunk1->targetVertexOrCarNumber);
		strcat(ChunkDebugMsg, outString);
		sprintf(outString, "targetItemNumber = %d\n", chunk1->targetItemNumber);
		strcat(ChunkDebugMsg, outString);
		sprintf(outString, "targetCellRC = (%d, %d)\n", chunk1->targetCellRC[0],
			chunk1->targetCellRC[1]);
		strcat(ChunkDebugMsg, outString);
		sprintf(outString, "ejectOrderGiven = %c\n", chunk1->ejectOrderGiven ? 'T' : 'F');
		strcat(ChunkDebugMsg, outString);
		sprintf(outString, "jumpOrder = %c\n", chunk1->jumpOrder ? 'T' : 'F');
		strcat(ChunkDebugMsg, outString);
		sprintf(outString, "data = %x\n", chunk1->data);
		strcat(ChunkDebugMsg, outString);
	}
	if (chunk2)
	{
		strcat(ChunkDebugMsg, "\nCHUNK2\n");
		GameObjectPtr target = nullptr;
		Stuff::Vector3D targetpoint;
		targetpoint.Zero();
		bool isTargetPoint = false;
		if (chunk1->targetType == STATUSCHUNK_TARGET_MOVER)
			target = (GameObjectPtr)MPlayer->moverRoster[chunk2->targetId];
		else if (chunk1->targetType == STATUSCHUNK_TARGET_TERRAIN)
			target = ObjectManager->findByPartId(chunk2->targetId);
		else if (chunk1->targetType == STATUSCHUNK_TARGET_SPECIAL)
			target = ObjectManager->findByPartId(chunk2->targetId);
		else if (chunk1->targetType == STATUSCHUNK_TARGET_LOCATION)
		{
			targetpoint.x = (float)chunk2->targetCellRC[1] * Terrain::worldUnitsPerCell + Terrain::worldUnitsPerCell / 2 - Terrain::worldUnitsMapSide / 2;
			targetpoint.y = (Terrain::worldUnitsMapSide / 2) - ((float)chunk2->targetCellRC[0] * Terrain::worldUnitsPerCell) - Terrain::worldUnitsPerCell / 2;
			targetpoint.z = (float)land->getTerrainElevation(targetpoint);
			isTargetPoint = true;
		}
		if (target)
		{
			if (target->isMover())
			{
				sprintf(
					outString, "target = %s (%d)\n", ((std::unique_ptr<Mover>)target)->name, target->getPartId());
				strcat(ChunkDebugMsg, outString);
			}
			else
			{
				sprintf(outString, "target = objClass %d (%d)\n", target->getObjectClass(),
					target->getPartId());
				strcat(ChunkDebugMsg, outString);
			}
		}
		else if (isTargetPoint)
		{
			sprintf(outString, "target point = (%f, %f, %f)\n", targetpoint.x, targetpoint.y,
				targetpoint.z);
			strcat(ChunkDebugMsg, outString);
		}
		else if (chunk1->targetType == STATUSCHUNK_TARGET_NONE)
		{
			sprintf(outString, "target = NONE\n");
			strcat(ChunkDebugMsg, outString);
		}
		else
		{
			strcat(ChunkDebugMsg, "target = ???\n");
			if (chunk2->targetType == STATUSCHUNK_TARGET_TERRAIN)
			{
				//---------------------------------------------------------------------
				// If the TargetType is terrain, let's at least list all of the
				// terrain objects in this Block/Vertex...
				int32_t numObjsInTile = 0;
				int32_t partId = chunk1->targetId - chunk2->targetItemNumber;
				for (size_t i = 0; i < MAX_TERRAIN_TILE_ITEMS; i++)
				{
					GameObjectPtr curTarget = ObjectManager->findByPartId(partId + i);
					if (curTarget)
					{
						numObjsInTile++;
						sprintf(outString, "    %d: objClass %d (%d)\n", i,
							curTarget->getObjectClass(), curTarget->getPartId());
						strcat(ChunkDebugMsg, outString);
					}
				}
				if (numObjsInTile > 0)
				{
					sprintf(outString, "    There are %d terrain objects in this tile.\n",
						numObjsInTile);
					strcat(ChunkDebugMsg, outString);
				}
			}
		}
		sprintf(outString, "bodyState = %d\n", chunk2->bodyState);
		strcat(ChunkDebugMsg, outString);
		sprintf(outString, "targetType = %d\n", chunk2->targetType);
		strcat(ChunkDebugMsg, outString);
		sprintf(outString, "targetId = %d\n", chunk2->targetId);
		strcat(ChunkDebugMsg, outString);
		sprintf(outString, "targetBlockOrTrainNumber = %d\n", chunk2->targetBlockOrTrainNumber);
		strcat(ChunkDebugMsg, outString);
		sprintf(outString, "targetVertexOrCarNumber = %d\n", chunk2->targetVertexOrCarNumber);
		strcat(ChunkDebugMsg, outString);
		sprintf(outString, "targetItemNumber = %d\n", chunk2->targetItemNumber);
		strcat(ChunkDebugMsg, outString);
		sprintf(outString, "targetCellRC = (%d, %d)\n", chunk2->targetCellRC[0],
			chunk2->targetCellRC[1]);
		strcat(ChunkDebugMsg, outString);
		sprintf(outString, "ejectOrderGiven = %c\n", chunk2->ejectOrderGiven ? 'T' : 'F');
		strcat(ChunkDebugMsg, outString);
		sprintf(outString, "jumpOrder = %c\n", chunk2->jumpOrder ? 'T' : 'F');
		strcat(ChunkDebugMsg, outString);
		sprintf(outString, "data = %x\n", chunk2->data);
		strcat(ChunkDebugMsg, outString);
	}
	File* debugFile = new File;
	debugFile->create("stchunk.dbg");
	debugFile->writeString(ChunkDebugMsg);
	debugFile->close();
	delete debugFile;
	debugFile = nullptr;
	ExceptionGameMsg = ChunkDebugMsg;
}

//---------------------------------------------------------------------------

PVOID
StatusChunk::operator new(size_t ourSize)
{
	PVOID result;
	result = systemHeap->Malloc(ourSize);
	return (result);
}

//---------------------------------------------------------------------------

void StatusChunk::operator delete(PVOID us)
{
	systemHeap->Free(us);
}

//---------------------------------------------------------------------------

void StatusChunk::build(std::unique_ptr<Mover> mover)
{
	bodyState = 0;
	data = 0;
}

//---------------------------------------------------------------------------

#define DEBUG_STATUSCHUNK
//#define	ASSERT_STATUSCHUNK

void StatusChunk::pack(std::unique_ptr<Mover> mover)
{
	data = 0;
	if (jumpOrder)
	{
		data |= targetCellRC[0];
		data <<= STATUSCHUNK_CELLPOS_BITS;
		data |= targetCellRC[1];
		data <<= STATUSCHUNK_TARGETTYPE_BITS;
	}
	else
	{
		switch (targetType)
		{
		case STATUSCHUNK_TARGET_MOVER:
			//----------------
			// Mover Target...
			data |= targetId;
			data <<= STATUSCHUNK_TARGETTYPE_BITS;
			break;
		case STATUSCHUNK_TARGET_TERRAIN:
			//-------------------------
			// Terrain Object Target...
			data |= (targetId - MIN_TERRAIN_PART_ID);
			data <<= STATUSCHUNK_TARGETTYPE_BITS;
			break;
		case STATUSCHUNK_TARGET_SPECIAL:
			data |= targetBlockOrTrainNumber;
			data <<= STATUSCHUNK_TRAINCAR_BITS;
			data |= targetVertexOrCarNumber;
			data <<= STATUSCHUNK_TARGETTYPE_BITS;
			break;
		case STATUSCHUNK_TARGET_LOCATION:
			//-----------------------------------------
			// Must be a Location Target (or a Miss)...
			data |= targetCellRC[0];
			data <<= STATUSCHUNK_CELLPOS_BITS;
			data |= targetCellRC[1];
			data <<= STATUSCHUNK_TARGETTYPE_BITS;
			break;
		}
	}
	data |= targetType;
	data <<= STATUSCHUNK_EJECTORDER_BITS;
	if (ejectOrderGiven)
		data |= 1;
	data <<= STATUSCHUNK_JUMPORDER_BITS;
	if (jumpOrder)
		data |= 1;
	data <<= STATUSCHUNK_BODYSTATE_BITS;
	data |= bodyState;
#ifdef DEBUG_STATUSCHUNK
	//-------------------------
	// Lots'a error checking...
	switch (targetType)
	{
	case STATUSCHUNK_TARGET_NONE:
		break;
	case STATUSCHUNK_TARGET_MOVER:
	{
		if ((targetId < 0) || (targetId >= MAX_MULTIPLAYER_MOVERS))
		{
#ifdef ASSERT_STATUSCHUNK
			DebugStatusChunk(mover, this, nullptr);
			wchar_t errMsg[1024];
			sprintf(
				errMsg, " StatusChunk.pack: bad targetId %d (save stchunk.dbg file) ", targetId);
			Assert(false, targetId, errMsg);
#else
			StatusChunkUnpackErr = 1;
#endif
		}
		GameObjectPtr target = (GameObjectPtr)MPlayer->moverRoster[targetId];
		//----------------------------------------------------------------------------
		// Mover targets could be nullptr now, since we free them when they're
		// destroyed.
		// if (!target) {
		//	#ifdef ASSERT_STATUSCHUNK
		//		DebugStatusChunk(mover, this, nullptr);
		//		wchar_t errMsg[1024];
		//		sprintf(errMsg, " StatusChunk.pack: nullptr Mover Target (save
		// stchunk.dbg file) "); 		Assert(false, targetId, errMsg);
		//	#else
		//		StatusChunkUnpackErr = 2;
		//	#endif
		//}
	}
	break;
	case STATUSCHUNK_TARGET_TERRAIN:
	{
		GameObjectPtr target = ObjectManager->findByPartId(targetId);
		if (!target)
		{
#ifdef ASSERT_STATUSCHUNK
			DebugStatusChunk(mover, this, nullptr);
			wchar_t errMsg[1024];
			sprintf(errMsg,
				" StatusChunk.pack: nullptr Terrain Target (save "
				"stchunk.dbg file) ");
			Assert(false, targetId, errMsg);
#else
			StatusChunkUnpackErr = 3;
#endif
		}
	}
	break;
	case STATUSCHUNK_TARGET_SPECIAL:
	{
		GameObjectPtr target = ObjectManager->findByPartId(targetId);
		if (!target)
		{
#ifdef ASSERT_STATUSCHUNK
			DebugStatusChunk(mover, this, nullptr);
			wchar_t errMsg[1024];
			sprintf(errMsg,
				" StatusChunk.pack: nullptr Special Target (save "
				"stchunk.dbg file) ");
			Assert(false, targetId, errMsg);
#else
			StatusChunkUnpackErr = 4;
#endif
		}
	}
	break;
	case STATUSCHUNK_TARGET_LOCATION:
		break;
	default:
		//#ifdef ASSERT_STATUSCHUNK
		//	DebugStatusChunk(mover, this, nullptr);
		//	wchar_t errMsg[1024];
		//	sprintf(errMsg, " StatusChunk.pack: bad targetType %d (save
		// stchunk.dbg file) ", targetType); 	Assert(false, targetType,
		// errMsg); #else 	StatusChunkUnpackErr = 5; #endif
		NODEFAULT;
	}
#endif
}

//---------------------------------------------------------------------------

void StatusChunk::unpack(std::unique_ptr<Mover> mover)
{
	StatusChunkUnpackErr = 0;
	uint32_t tempData = data;
	bodyState = (tempData & STATUSCHUNK_BODYSTATE_MASK);
	tempData >>= STATUSCHUNK_BODYSTATE_BITS;
	jumpOrder = ((tempData & STATUSCHUNK_JUMPORDER_MASK) != 0);
	tempData >>= STATUSCHUNK_JUMPORDER_BITS;
	//	if (jumpOrder)
	//		OutputDebugString("JUMP ORDER\n");
	ejectOrderGiven = ((tempData & STATUSCHUNK_EJECTORDER_MASK) != 0);
	tempData >>= STATUSCHUNK_EJECTORDER_BITS;
	targetType = (tempData & STATUSCHUNK_TARGETTYPE_MASK);
	tempData >>= STATUSCHUNK_TARGETTYPE_BITS;
	if (jumpOrder)
	{
		targetCellRC[1] = (tempData & STATUSCHUNK_CELLPOS_MASK);
		tempData >>= STATUSCHUNK_CELLPOS_BITS;
		targetCellRC[0] = (tempData & STATUSCHUNK_CELLPOS_MASK);
	}
	else
	{
		switch (targetType)
		{
		case STATUSCHUNK_TARGET_NONE:
			break;
		case STATUSCHUNK_TARGET_MOVER:
			//----------------
			// Mover Target...
			targetId = (tempData & STATUSCHUNK_MOVERINDEX_MASK);
			break;
		case STATUSCHUNK_TARGET_TERRAIN:
			//-------------------------
			// Terrain Object Target...
			targetId = (tempData & STATUSCHUNK_TERRAINPARTID_MASK) + MIN_TERRAIN_PART_ID;
			break;
		case STATUSCHUNK_TARGET_SPECIAL:
			//-----------------------
			// Train Object Target...
			targetVertexOrCarNumber = (tempData & STATUSCHUNK_TRAIN_MASK);
			tempData >>= STATUSCHUNK_TRAIN_BITS;
			targetBlockOrTrainNumber = (tempData & STATUSCHUNK_TRAINCAR_MASK);
			tempData >>= STATUSCHUNK_TRAINCAR_BITS;
			if (targetBlockOrTrainNumber == 128)
				targetId = MIN_CAMERA_DRONE_ID + targetVertexOrCarNumber;
			else
				targetId = MIN_TRAIN_PART_ID + targetBlockOrTrainNumber * MAX_TRAIN_CARS + targetVertexOrCarNumber;
			break;
		case STATUSCHUNK_TARGET_LOCATION:
			//-----------------------------------------
			// Must be a Location Target (or a Miss)...
			targetCellRC[1] = (tempData & STATUSCHUNK_CELLPOS_MASK);
			tempData >>= STATUSCHUNK_CELLPOS_BITS;
			targetCellRC[0] = (tempData & STATUSCHUNK_CELLPOS_MASK);
			break;
		}
	}
#ifdef DEBUG_STATUSCHUNK
	//-------------------------
	// Lots'a error checking...
	switch (targetType)
	{
	case STATUSCHUNK_TARGET_NONE:
		break;
	case STATUSCHUNK_TARGET_MOVER:
	{
		if ((targetId < 0) || (targetId >= MAX_MULTIPLAYER_MOVERS))
		{
#ifdef ASSERT_STATUSCHUNK
			DebugStatusChunk(mover, this, nullptr);
			wchar_t errMsg[1024];
			sprintf(
				errMsg, " StatusChunk.unpack: bad targetId %d (save stchunk.dbg file) ", targetId);
			Assert(false, targetId, errMsg);
#else
			StatusChunkUnpackErr = 1;
#endif
		}
		GameObjectPtr target = (GameObjectPtr)MPlayer->moverRoster[targetId];
		//----------------------------------------------------------------------------
		// Mover targets could be nullptr now, since we free them when they're
		// destroyed.
		// if (!target) {
		//	#ifdef ASSERT_STATUSCHUNK
		//		DebugStatusChunk(mover, this, nullptr);
		//		wchar_t errMsg[1024];
		//		sprintf(errMsg, " StatusChunk.unpack: nullptr Mover Target (save
		// stchunk.dbg file) "); 		Assert(false, targetId, errMsg);
		//	#else
		//		StatusChunkUnpackErr = 2;
		//	#endif
		//}
	}
	break;
	case STATUSCHUNK_TARGET_TERRAIN:
	{
		GameObjectPtr target = ObjectManager->findByPartId(targetId);
		if (!target)
		{
#ifdef ASSERT_STATUSCHUNK
			DebugStatusChunk(mover, this, nullptr);
			wchar_t errMsg[1024];
			sprintf(errMsg,
				" StatusChunk.unpack: nullptr Terrain Target (save "
				"stchunk.dbg file) ");
			Assert(false, targetId, errMsg);
#else
			StatusChunkUnpackErr = 3;
#endif
		}
	}
	break;
	case STATUSCHUNK_TARGET_SPECIAL:
	{
		GameObjectPtr target = ObjectManager->findByPartId(targetId);
		if (!target)
		{
#ifdef ASSERT_STATUSCHUNK
			DebugStatusChunk(mover, this, nullptr);
			wchar_t errMsg[1024];
			sprintf(errMsg,
				" StatusChunk.unpack: nullptr Special Target (save "
				"stchunk.dbg file) ");
			Assert(false, targetId, errMsg);
#else
			StatusChunkUnpackErr = 4;
#endif
		}
	}
	break;
	case STATUSCHUNK_TARGET_LOCATION:
		break;
	default:
		//#ifdef ASSERT_STATUSCHUNK
		//	DebugStatusChunk(mover, this, nullptr);
		//	wchar_t errMsg[1024];
		//	sprintf(errMsg, " StatusChunk.unpack: bad targetType %d (save
		// stchunk.dbg file) ", targetType); 	Assert(false, targetType,
		// errMsg); #else 	StatusChunkUnpackErr = 5; #endif
		NODEFAULT;
	}
#endif
}

//---------------------------------------------------------------------------

bool StatusChunk::equalTo(StatusChunkPtr chunk)
{
	if (bodyState != chunk->bodyState)
	{
		DebugStatusChunk(nullptr, this, chunk);
		return (false);
	}
	if (ejectOrderGiven != chunk->ejectOrderGiven)
	{
		DebugStatusChunk(nullptr, this, chunk);
		return (false);
	}
	if (jumpOrder != chunk->jumpOrder)
	{
		DebugStatusChunk(nullptr, this, chunk);
		return (false);
	}
	if (targetType != chunk->targetType)
	{
		DebugStatusChunk(nullptr, this, chunk);
		return (false);
	}
	if (targetId != chunk->targetId)
	{
		DebugStatusChunk(nullptr, this, chunk);
		return (false);
	}
	if (targetCellRC[0] != chunk->targetCellRC[0])
	{
		DebugStatusChunk(nullptr, this, chunk);
		return (false);
	}
	if (targetCellRC[1] != chunk->targetCellRC[1])
	{
		DebugStatusChunk(nullptr, this, chunk);
		return (false);
	}
	return (true);
}

//***************************************************************************
// MOVER DYNAMICS class
//***************************************************************************

void MoverDynamics::init(void)
{
	type = DYNAMICS_BASE;
}

//---------------------------------------------------------------------------

void MoverDynamics::init(DynamicsType newType)
{
	type = newType;
	switch (newType)
	{
	case DYNAMICS_MECH:
		max.mech.torsoYawRate = 0;
		max.mech.torsoYaw = 0;
		break;
	case DYNAMICS_GROUNDVEHICLE:
		max.groundVehicle.yawRate = 0;
		// cur.groundVehicle.yaw = 0.0;
		break;
	case DYNAMICS_ELEMENTAL:
		max.elemental.yawRate = 0;
		// cur.elemental.yaw = 0.0;
		break;
	default:
		// Fatal(newType, " ugh ");
		NODEFAULT;
	}
}

//---------------------------------------------------------------------------

void MoverDynamics::init(CSVFilePtr dynamicsFile)
{
	if (type == DYNAMICS_MECH)
	{
		// Its a new day!!!
		dynamicsFile->readLong(8, 5, max.mech.torsoYawRate);
		dynamicsFile->readLong(9, 5, max.mech.torsoYaw);
	}
	else if (type == DYNAMICS_GROUNDVEHICLE)
	{
		/*
			int32_t result = 0;
			result = dynamicsFile->seekBlock("VehicleDynamics");
			Assert(result == NO_ERROR, result, " MoverDynamics.init: vehicle
		   error 0 ");

			result =
		   dynamicsFile->readIdLong("maxTurretYawRate",max.groundVehicle.turretYawRate);
			Assert(result == NO_ERROR, result, " MoverDynamics.init: vehicle
		   error 1 ");

			result =
		   dynamicsFile->readIdLong("maxTurretYaw",max.groundVehicle.turretYaw);
			Assert(result == NO_ERROR, result, " MoverDynamics.init: vehicle
		   error 2 ");

			result =
		   dynamicsFile->readIdLong("maxVehicleYawRate",max.groundVehicle.yawRate);
			Assert(result == NO_ERROR, result, " MoverDynamics.init: vehicle
		   error 3 "); if (max.groundVehicle.yawRate < 720)
				max.groundVehicle.yawRate = 720;

			result =
		   dynamicsFile->readIdLong("maxVehiclePivotRate",max.groundVehicle.pivotRate);
			if (result != NO_ERROR)
				max.groundVehicle.pivotRate =
		   (int32_t)((float)max.groundVehicle.yawRate *
		   GroundVehiclePivotYawMultiplier);

			result =
		   dynamicsFile->readIdFloat("maxAccel",max.groundVehicle.accel);
			Assert(result == NO_ERROR, result, " MoverDynamics.init: vehicle
		   error 4 ");

			result =
		   dynamicsFile->readIdFloat("maxVelocity",max.groundVehicle.speed);
			Assert(result == NO_ERROR, result, " MoverDynamics.init: vehicle
		   error 5 ");
			// Frank Hack
			max.groundVehicle.accel = max.groundVehicle.speed * 5.0;
		*/
	}
	else if (type == DYNAMICS_ELEMENTAL)
	{
		Fatal(type, " MoverDynamics.init: bad elemental ");
	}
	else
		Fatal(type, " MoverDynamics.init: bad type ");
}

//---------------------------------------------------------------------------

void MoverDynamics::init(FitIniFilePtr dynamicsFile)
{
	if (type == DYNAMICS_MECH)
	{
	}
	else if (type == DYNAMICS_GROUNDVEHICLE)
	{
		int32_t result = 0;
		result = dynamicsFile->seekBlock("VehicleDynamics");
		Assert(result == NO_ERROR, result, " MoverDynamics.init: vehicle error 0 ");
		result = dynamicsFile->readIdLong("maxTurretYawRate", max.groundVehicle.turretYawRate);
		Assert(result == NO_ERROR, result, " MoverDynamics.init: vehicle error 1 ");
		result = dynamicsFile->readIdLong("maxTurretYaw", max.groundVehicle.turretYaw);
		Assert(result == NO_ERROR, result, " MoverDynamics.init: vehicle error 2 ");
		result = dynamicsFile->readIdLong("maxVehicleYawRate", max.groundVehicle.yawRate);
		Assert(result == NO_ERROR, result, " MoverDynamics.init: vehicle error 3 ");
		if (max.groundVehicle.yawRate < 720)
			max.groundVehicle.yawRate = 720;
		result = dynamicsFile->readIdLong("maxVehiclePivotRate", max.groundVehicle.pivotRate);
		if (result != NO_ERROR)
			max.groundVehicle.pivotRate =
				(int32_t)((float)max.groundVehicle.yawRate * GroundVehiclePivotYawMultiplier);
		result = dynamicsFile->readIdFloat("maxAccel", max.groundVehicle.accel);
		Assert(result == NO_ERROR, result, " MoverDynamics.init: vehicle error 4 ");
		result = dynamicsFile->readIdFloat("maxVelocity", max.groundVehicle.speed);
		Assert(result == NO_ERROR, result, " MoverDynamics.init: vehicle error 5 ");
		// Frank Hack
		max.groundVehicle.accel = max.groundVehicle.speed * 5.0;
	}
	else if (type == DYNAMICS_ELEMENTAL)
	{
		Fatal(type, " MoverDynamics.init: bad elemental ");
	}
	else
		Fatal(type, " MoverDynamics.init: bad type ");
}

//***************************************************************************
// MOVER CONTROL class
//***************************************************************************

void MoverControl::reset(void)
{
	switch (dataType)
	{
	case CONTROL_DATA_BASE:
		break;
	case CONTROL_DATA_MECH:
		// settings.mech.throttle = 100;
		settings.mech.rotate = 0.0;
		settings.mech.facingRotate = 0.0f;
		settings.mech.rotateTorso = 0;
		settings.mech.rotateLeftArm = 0;
		settings.mech.rotateRightArm = 0;
		settings.mech.gestureGoal = -1;
		settings.mech.blowLeftArm = false;
		settings.mech.blowRightArm = false;
		settings.mech.pivot = false;
		break;
	case CONTROL_DATA_GROUNDVEHICLE:
		settings.groundVehicle.rotate = 0.0;
		settings.groundVehicle.rotateTurret = 0.0;
		settings.groundVehicle.gestureGoal = -1;
		settings.groundVehicle.pivot = false;
		settings.groundVehicle.isWalking = false;
		settings.groundVehicle.throttle = 0.0;
		break;
	case CONTROL_DATA_ELEMENTAL:
		break;
	}
}

//---------------------------------------------------------------------------

void MoverControl::brake(void)
{
	switch (dataType)
	{
	case CONTROL_DATA_BASE:
		break;
	case CONTROL_DATA_MECH:
		settings.mech.throttle = 100;
		settings.mech.gestureGoal = MECH_STATE_STANDING;
		break;
	case CONTROL_DATA_GROUNDVEHICLE:
		settings.mech.throttle = 0;
		break;
	case CONTROL_DATA_ELEMENTAL:
		break;
	}
}

//---------------------------------------------------------------------------

void MoverControl::update(std::unique_ptr<Mover> mover)
{
	switch (type)
	{
	case CONTROL_AI:
		mover->updateAIControl();
		break;
	case CONTROL_NET:
		mover->updateNetworkControl();
		break;
	case CONTROL_PLAYER:
		mover->updatePlayerControl();
		break;
	default:
		// Fatal(type, " MoverControl.update: bad control type ");
		NODEFAULT;
	}
}

//---------------------------------------------------------------------------

int32_t
MoverControl::init(FitIniFilePtr controlFile)
{
	return (NO_ERROR);
}

//***************************************************************************
// MOVER class
//***************************************************************************

int32_t
Mover::loadGameSystem(FitIniFilePtr mechFile, float visualRange)
{
	int32_t result = 0;
	result = mechFile->seekBlock("Pathfinding");
	if (result != NO_ERROR)
		return (result);
	//	int32_t longRangeMoveEnabled[MAX_TEAMS];
	//	result = mechFile->readIdLongArray("LongRangeMovementEnabled",
	// longRangeMoveEnabled, NUM_TEAMS); 	if (result != NO_ERROR)
	//		return(result);
	//	for (size_t i = 0; i < NUM_TEAMS; i++)
	//		LongRangeMovementEnabled[i] = (longRangeMoveEnabled[i] == 1);
	result = mechFile->readIdLong("SimplePathTileRange", SimpleMovePathRange);
	if (result != NO_ERROR)
		return (result);
	result = mechFile->readIdFloat("DelayedOrderTime", DelayedOrderTime);
	if (result != NO_ERROR)
		return (result);
	result = mechFile->readIdFloat("MoveTimeOut", MoveTimeOut);
	if (result != NO_ERROR)
		MoveTimeOut = 30.0;
	result = mechFile->readIdFloat("MoveYieldTime", MoveYieldTime);
	if (result != NO_ERROR)
		MoveYieldTime = 1.5;
	result = mechFile->readIdLongArray("GroupMoveTrailLength", GroupMoveTrailLen, 2);
	if (result != NO_ERROR)
	{
		GroupMoveTrailLen[0] = 0;
		GroupMoveTrailLen[1] = 1;
	}
	result = mechFile->readIdLongArray("GroupMoveTrailLength", GroupMoveTrailLen, 2);
	if (result != NO_ERROR)
	{
		GroupMoveTrailLen[0] = 0;
		GroupMoveTrailLen[1] = 1;
	}
	result = mechFile->readIdFloat("GroupOrderGoalOffset", GroupOrderGoalOffset);
	if (result != NO_ERROR)
		return (result);
	result = mechFile->readIdFloatArray("MoveMarginOfError", Mover::marginOfError, 2);
	if (result != NO_ERROR)
		return (result);
	//	for (size_t i = 0; i < NUM_OVERLAY_TYPES; i++)
	//		OverlayWeightIndex[i] = i * terrain_const::MAPCELL_DIM * terrain_const::MAPCELL_DIM;
	//	result = mechFile->readIdLongArray("OverlayCellCosts",
	// OverlayWeightTable, NUM_OVERLAY_WEIGHT_CLASSES * NUM_OVERLAY_TYPES *
	// terrain_const::MAPCELL_DIM * terrain_const::MAPCELL_DIM); 	if (result != NO_ERROR)
	//		return(result);
	result = mechFile->seekBlock("Mover:General");
	Assert(result == NO_ERROR, 0, "Couldn't find Mover:General block in gamesys.fit");
	result = mechFile->readIdFloat("NewThreatMultiplier", newThreatMultiplier);
	if (result != NO_ERROR)
		newThreatMultiplier = 3.0;
	result = mechFile->readIdFloat("BlockCaptureRange", GameObject::blockCaptureRange);
	Assert(result == NO_ERROR, 0,
		"Couldn't find BlockCaptureRange in Mover:General block in "
		"gamesys.fit");
	result = mechFile->readIdFloat("RefitTime", Mover::refitTime);
	Assert(result == NO_ERROR, 0, "Couldn't find RefitTime in Mover:General block in gamesys.fit");
	result = mechFile->readIdFloat("RefitRange", Mover::refitRange);
	Assert(result == NO_ERROR, 0, "Couldn't find RefitRange in Mover:General block in gamesys.fit");
	result = mechFile->readIdFloat("RefitAmount", Mover::refitAmount);
	Assert(
		result == NO_ERROR, 0, "Couldn't find RefitAmount in Mover:General block in gamesys.fit");
	result = mechFile->readIdFloat(
		"RefitVehicleArmorCost", Mover::refitCostArray[ARMOR_REFIT_COST][REFIT_VEHICLE]);
	Assert(result == NO_ERROR, 0,
		"Couldn't find RefitVehicleArmorCost in Mover:General block in "
		"gamesys.fit");
	result = mechFile->readIdFloat("RecoverTime", Mover::recoverTime);
	Assert(
		result == NO_ERROR, 0, "Couldn't find RecoverTime in Mover:General block in gamesys.fit");
	result = mechFile->readIdFloat("RecoverRange", Mover::recoverRange);
	Assert(
		result == NO_ERROR, 0, "Couldn't find RecoverRange in Mover:General block in gamesys.fit");
	result = mechFile->readIdFloat("RecoverAmount", Mover::recoverAmount);
	Assert(
		result == NO_ERROR, 0, "Couldn't find RecoverAmount in Mover:General block in gamesys.fit");
	result = mechFile->readIdFloat("RecoverCost", Mover::recoverCost);
	Assert(
		result == NO_ERROR, 0, "Couldn't find RecoverCost in Mover:General block in gamesys.fit");
	result = mechFile->readIdFloat(
		"RefitVehicleInternalCost", Mover::refitCostArray[INTERNAL_REFIT_COST][REFIT_VEHICLE]);
	Assert(result == NO_ERROR, 0,
		"Couldn't find RefitVehicleInternalCost in Mover:General block in "
		"gamesys.fit");
	result = mechFile->readIdFloat(
		"RefitVehiclePointsToAmmo", Mover::refitCostArray[AMMO_REFIT_COST][REFIT_VEHICLE]);
	Assert(result == NO_ERROR, 0,
		"Couldn't find RefitVehiclePointsToAmmo in Mover:General block in "
		"gamesys.fit");
	result = mechFile->readIdFloat(
		"RefitBayArmorCost", Mover::refitCostArray[ARMOR_REFIT_COST][REFIT_BAY]);
	Assert(result == NO_ERROR, 0,
		"Couldn't find RefitBayArmorCost in Mover:General block in "
		"gamesys.fit");
	result = mechFile->readIdFloat(
		"RefitBayInternalCost", Mover::refitCostArray[INTERNAL_REFIT_COST][REFIT_BAY]);
	Assert(result == NO_ERROR, 0,
		"Couldn't find RefitBayInternalCost in Mover:General block in "
		"gamesys.fit");
	result = mechFile->readIdFloat(
		"RefitBayAmmoCost", Mover::refitCostArray[AMMO_REFIT_COST][REFIT_BAY]);
	Assert(result == NO_ERROR, 0,
		"Couldn't find RefitBayAmmoCost in Mover:General block in gamesys.fit");
	result = mechFile->readIdLongArray(
		"IndirectFireWeaponComponentIndex", Mover::IndirectFireWeapons, 20);
	Assert(result == NO_ERROR, 0,
		"Couldn't find IndirectFireWeaponComponentIndex in Mover:General block "
		"in gamesys.fit");
	result =
		mechFile->readIdLongArray("AreaEffectWeaponComponentIndex", Mover::AreaEffectWeapons, 20);
	Assert(result == NO_ERROR, 0,
		"Couldn't find AreaEffectWeaponComponentIndex in Mover:General block "
		"in gamesys.fit");
	result = mechFile->seekBlock("Mover:FireWeapon");
	if (result != NO_ERROR)
		return (result);
	//	result = mechFile->readIdFloatArray("WeaponFireModifiers",
	// WeaponFireModifiers, NUM_WEAPONFIRE_MODIFIERS); 	if (result == NO_ERROR)
	//{ 		for (size_t i = 0; i < WarriorRank::numberofranks; i++)
	//			for (int32_t j = 0; j < (NUM_MECH_CLASSES - 1); j++)
	//				RankVersusChassisCombatModifier[i][j + 1] = WeaponFireModifiers[7
	//+ i * WarriorRank::numberofranks + j];
	//	}
	result = mechFile->readIdFloatArray("FireArc", FireArc, 3);
	if (result != NO_ERROR)
		return (result);
	for (size_t arc = 0; arc < 3; arc++)
		FireArc[arc] /= 2.0;
	result = mechFile->readIdLong("AimedFireAbort", AimedFireAbort);
	if (result != NO_ERROR)
		return (result);
	result = mechFile->readIdLongArray("AimedFireHitTable", AimedFireHitTable, 3);
	if (result != NO_ERROR)
		return (result);
	result = mechFile->readIdFloat("DisableAttackModifier", DisableAttackModifier);
	if (result != NO_ERROR)
		return (result);
	result = mechFile->readIdFloat("DisableGunneryModifier", DisableGunneryModifier);
	if (result != NO_ERROR)
		return (result);
	result = mechFile->readIdFloat("SalvageAttackModifier", SalvageAttackModifier);
	if (result != NO_ERROR)
		return (result);
	result = mechFile->readIdFloat("MaxStationaryTime", MaxStationaryTime);
	if (result != NO_ERROR)
		return (result);
	result = mechFile->readIdFloat("MaxTimeRevealed", MaxTimeRevealed);
	if (result != NO_ERROR)
		return (result);
	result = mechFile->readIdFloat("HeadShotModifier", HeadShotModifier);
	if (result != NO_ERROR)
		HeadShotModifier = 0.08f;
	result = mechFile->readIdFloat("ArmShotModifier", ArmShotModifier);
	if (result != NO_ERROR)
		ArmShotModifier = 0.15f;
	result = mechFile->readIdFloat("LegShotModifier", LegShotModifier);
	if (result != NO_ERROR)
		LegShotModifier = 0.20f;
	result = mechFile->seekBlock("Mover:Damage");
	if (result != NO_ERROR)
		return (result);
	result = mechFile->readIdLongArray("HitLevel", hitLevel, 2);
	if (result != NO_ERROR)
		return (result);
	result = mechFile->readIdFloat("PilotingCheckFactor", PilotingCheckFactor);
	if (result != NO_ERROR)
		return (result);
	result = mechFile->seekBlock("Components");
	if (result != NO_ERROR)
		return (result);
	result = mechFile->readIdLong("ClusterSizeSRM", ClusterSizeSRM);
	if (result != NO_ERROR)
		return (result);
	// Hard code--ultimately, we kill clusters in game code!
	ClusterSizeSRM = 2;
	result = mechFile->readIdLong("ClusterSizeLRM", ClusterSizeLRM);
	if (result != NO_ERROR)
		return (result);
	// Hard code--ultimately, we kill clusters in game code!
	ClusterSizeLRM = 5;
	result = mechFile->readIdLongArray("InnerSphereAntiMissile", AntiMissileSystemStats[0], 2);
	if (result != NO_ERROR)
		return (result);
	result = mechFile->readIdLongArray("ClanAntiMissile", AntiMissileSystemStats[1], 2);
	if (result != NO_ERROR)
		return (result);
	result = mechFile->seekBlock("Warrior");
	if (result != NO_ERROR)
		return (result);
	result = mechFile->readIdFloat("DefaultAttackRadius", DefaultAttackRadius);
	if (result != NO_ERROR)
		DefaultAttackRadius = 275.0;
	result = mechFile->readIdFloatArray("WarriorRankScale", WarriorRankScale, WarriorRank::numberofranks);
	if (result != NO_ERROR)
		return (result);
	wchar_t profData[NUM_OFFSET_RANGES * 2];
	result = mechFile->readIdCharArray("ProfessionalismTable", profData, NUM_OFFSET_RANGES * 2);
	if (result != NO_ERROR)
		return (result);
	for (size_t i = 0; i < NUM_OFFSET_RANGES; i++)
		memcpy(&ProfessionalismOffsetTable[i], &profData[i * 2], 2);
	result = mechFile->readIdCharArray("DecorumTable", profData, NUM_OFFSET_RANGES * 2);
	if (result != NO_ERROR)
		return (result);
	for (i = 0; i < NUM_OFFSET_RANGES; i++)
		memcpy(&DecorumOffsetTable[i], &profData[i * 2], 2);
	result = mechFile->readIdCharArray("AmmoTable", profData, 2 * 2);
	if (result != NO_ERROR)
		return (result);
	for (i = 0; i < 2; i++)
		memcpy(&AmmoConservationModifiers[i], &profData[i * 2], 2);
#ifdef USEHEAT
	result = mechFile->readIdFloat("WeaponHeatCheckDelay", WeaponHeatCheckDelay);
	if (result != NO_ERROR)
		return (result);
#endif
	result = mechFile->readIdFloat("PilotCheckHalfRate", PilotCheckHalfRate);
	if (result != NO_ERROR)
		return (result);
	result = mechFile->readIdLongArray("PilotCheckModifiers", PilotCheckModifierTable, 2);
	if (result != NO_ERROR)
		return (result);
	result = mechFile->readIdFloat("DamageRateFrequency", DamageRateFrequency);
	if (result != NO_ERROR)
		return (result);
	wchar_t charData[AttitudeType::count * 6];
	result = mechFile->readIdCharArray("AttitudeEffect", charData, AttitudeType::count * 6);
	if (result != NO_ERROR)
		return (result);
	for (i = 0; i < AttitudeType::count; i++)
		memcpy(&AttitudeEffect[i], &charData[i * 6], 6);
	result = mechFile->readIdFloat("MovementUpdateFrequency", MovementUpdateFrequency);
	if (result != NO_ERROR)
		return (result);
	result = mechFile->readIdFloat("CombatUpdateFrequency", CombatUpdateFrequency);
	if (result != NO_ERROR)
		return (result);
	result = mechFile->readIdFloat("CommandUpdateFrequency", CommandUpdateFrequency);
	if (result != NO_ERROR)
		return (result);
	result = mechFile->readIdFloat("PilotCheckUpdateFrequency", PilotCheckUpdateFrequency);
	if (result != NO_ERROR)
		return (result);
	//	result = mechFile->readIdFloatArray("FireOddsTable", FireOddsTable,
	// FireOddsType::count); 	if (result != NO_ERROR) 		return(result);
	result = mechFile->readIdLong("SkillIncreaseCap", MechWarrior::increaseCap);
	Assert(result == NO_ERROR, result,
		" Couldn't find SkillCap variable in Warrior block of gamesys.fit ");
	result = mechFile->readIdFloat("SkillMax", MechWarrior::maxSkill);
	Assert(result == NO_ERROR, result,
		" Couldn't find SkillMax variable in Warrior block of gamesys.fit ");
	result = mechFile->readIdFloat("SkillMin", MechWarrior::minSkill);
	Assert(result == NO_ERROR, result,
		" Couldn't find SkillMin variable in Warrior block of gamesys.fit ");
	//	result = mechFile->readIdLong("JumpSkillMod", PilotJumpMod);
	//	Assert(result == NO_ERROR, result, " Couldn't find JumpSkillMod variable
	// in Warrior block of gamesys.fit ");
	result = mechFile->seekBlock("Sensors");
	if (result != NO_ERROR)
		return (result);
	//	result = mechFile->readIdFloatArray("SensorModifiers", SensorModifier,
	// 8); 	if (result != NO_ERROR) 		return(result);
	result = mechFile->readIdFloat("BaseSensorRollTarget", SensorBaseChance);
	if (result != NO_ERROR)
		return (result);
	result = mechFile->readIdFloat("SensorSkillFactor", SensorSkillFactor);
	if (result != NO_ERROR)
		return (result);
	result = mechFile->readIdFloat("BlockingObjectModifier", SensorBlockingObjectModifier);
	if (result != NO_ERROR)
		return (result);
	result = mechFile->readIdFloat("ShutdownMech", SensorShutDownMechModifier);
	if (result != NO_ERROR)
		return (result);
#ifdef USEHEAT
	result = mechFile->readIdFloat("HeatMultiplier", SensorHeatMultiplier);
	if (result != NO_ERROR)
		return (result);
	result = mechFile->readIdFloat("WeaponHeatMultiplier", SensorWeaponHeatMultiplier);
	if (result != NO_ERROR)
		return (result);
	result = mechFile->readIdFloatArray("HeatSourceModifiers", SensorHeatSourceModifiers, 2);
	if (result != NO_ERROR)
		return (result);
#endif
	float rangeTable[8];
	result = mechFile->readIdFloatArray("SensorRangeModifier", rangeTable, 8);
	if (result != NO_ERROR)
		return (result);
	for (i = 0; i < 4; i++)
	{
		SensorRangeModifier[i][0] = rangeTable[i * 2];
		SensorRangeModifier[i][1] = rangeTable[i * 2 + 1];
	}
	float sizeTable[6];
	result = mechFile->readIdFloatArray("SizeModifier", sizeTable, 6);
	if (result != NO_ERROR)
		return (result);
	for (i = 0; i < 3; i++)
	{
		SensorSizeModifier[i][0] = sizeTable[i * 2];
		SensorSizeModifier[i][1] = sizeTable[i * 2 + 1];
	}
	int32_t masterIDs[9];
	result = mechFile->readIdLongArray("SensorMasterIDs", masterIDs, 9);
	if (result != NO_ERROR)
		return (result);
	/*
		float rangeMultipliers[9];
		result = mechFile->readIdFloatArray("SensorRangeMultipliers",
	   rangeMultipliers, 9); if (result != NO_ERROR) return(result);
		//------------------------------------------------------------------------------
		// The following relies upon knowing the MasterComponentID for all of
	   the sensor
		// components...
		for (i = 0; i < 9; i++)
			if (masterIDs[i] > -1)
				MasterComponentList[masterIDs[i]].setSensorRange(visualRange *
	   rangeMultipliers[i]);
	*/
	result = mechFile->readIdFloatArray("BlockingTerrainModifiers", SensorBlockingTerrain, 2);
	if (result != NO_ERROR)
		return (result);
	result = mechFile->seekBlock("Skills");
	if (result != NO_ERROR)
		Fatal(result, "Couldn't find skill block in gamesys.fit");
	result = mechFile->readIdFloatArray("Skill Attempt", SkillTry, 4);
	if (result != NO_ERROR)
		return (result);
	result = mechFile->readIdFloatArray("Skill Success", SkillSuccess, 4);
	if (result != NO_ERROR)
		return (result);
	result = mechFile->readIdFloat("WeaponHit", WeaponHit);
	if (result != NO_ERROR)
		return (result);
	result = mechFile->readIdFloatArray("KillSkillValues", KillSkill, 6);
	if (result != NO_ERROR)
		return (result);
	result = mechFile->readIdFloat("Sensor Contact Skill", SensorSkill);
	if (result != NO_ERROR)
		return (result);
	return (NO_ERROR);
}

//---------------------------------------------------------------------------

void Mover::set(Mover copy)
{
	GameObject::set(copy);
	// ID = copy.ID;
	positionNormal = copy.positionNormal;
	velocity = copy.velocity;
	rotation = copy.rotation;
	strncpy(name, copy.name, MAXLEN_MOVER_NAME - 1);
	chassis = copy.chassis;
	damageRateTally = copy.damageRateTally;
	damageRateCheckTime = copy.damageRateCheckTime;
	pilotCheckDamageTally = copy.pilotCheckDamageTally;
	numBodyLocations = copy.numBodyLocations;
	for (size_t i = 0; i < numBodyLocations; i++)
		body[i] = copy.body[i];
	fieldedCV = copy.fieldedCV;
	numArmorLocations = copy.numArmorLocations;
	for (i = 0; i < numArmorLocations; i++)
		armor[i] = copy.armor[i];
}

//---------------------------------------------------------------------------

void Mover::init(bool create)
{
	GameObject::init(create);
	if (initialize)
	{
		killed = false;
		lost = false;
		salvaged = false;
		appearance = nullptr;
		lowestWeaponNodeID = -2;
		sensorSystem = nullptr;
		lastMapCell[0] = -1;
		lastMapCell[1] = -1;
		contactInfo = new ContactInfo;
		pilot = nullptr;
		pilotHandle = 0;
		numWeaponHitsHandled = 0;
		prevTeamId = -1;
		numMovers++;
	}
	objectClass = MOVER;
	unitGroup = 2;
	causeOfDeath = -1;
	iconPictureIndex = 0;
	lowestWeaponNodeZ = -9999.9f;
	positionNormal.Zero();
	velocity.Zero();
	name[0] = nullptr;
	chassis = 0;
	startDisabled = false;
	pathLocks = true;
	damageRateTally = 0;
	damageRateCheckTime = 1.0;
	pilotCheckDamageTally = 0;
	numBodyLocations = 0;
	fieldedCV = 0;
	numArmorLocations = 0;
	attackRange = FireRangeType::optimal;
	suppressionFire = 0;
	numArmorLocations = 0;
	numOther = 0;
	numWeapons = 0;
	numAmmos = 0;
	numAmmoTypes = 0;
	cockpit = 255;
	engine = 255;
	lifeSupport = 255;
	sensor = 255;
	ecm = 255;
	probe = 255;
	jumpJets = 255;
	nullSignature = 255;
	maxWeaponEffectiveness = 0.0;
	weaponEffectiveness = 0.0;
	minRange = 0.0;
	maxRange = 0.0;
	optimalRange = 0.0;
	numFunctionalWeapons = 0;
	numAntiMissileSystems = 0;
	engineBlowTime = -1.0;
	maxMoveSpeed = 0.0;
	shutDownThisFrame = false;
	startUpThisFrame = false;
	disableThisFrame = false;
	//
	teamId = -1;
	groupId = -1;
	squadId = -1;
	selectionindex = -1;
	teamRosterIndex = -1;
	commanderId = -1;
	unitGroup = 2; // the thing the user sets by hitting ctrl and a number
	pilotCheckModifier = -1;
	prevPilotCheckUpdate = 0.0;
	prevPilotCheckModifier = 0;
	prevPilotCheckDelta = 0;
	failedPilotingCheck = false;
	collisionFreeFromWID = 0;
	collisionFreeTime = -1.0;
	lastWeaponEffectivenessCalc = 0.0;
	lastOptimalRangeCalc = 0.0;
	challengerWID = 0;
	lastGesture = -1;
	control.init();
	dynamics.init();
	// netPlayerId = 0;
	if (MPlayer)
	{
#ifdef USE_STRING_RESOURCES
		cLoadString(thisInstance, IDS_LOSTPLAYER + languageOffset, netPlayerName, 254);
#else
		sprintf(netPlayerName, "%s", "NEED STRING RESOURCES");
#endif
	}
	else
		netPlayerName[0] = nullptr;
	localMoverId = -1;
	netRosterIndex = -1;
	statusChunk.init();
	newMoveChunk = false;
	moveChunk.init();
	numWeaponFireChunks[CHUNK_RECEIVE] = 0;
	numWeaponFireChunks[CHUNK_SEND] = 0;
	numCriticalHitChunks[CHUNK_RECEIVE] = 0;
	numCriticalHitChunks[CHUNK_SEND] = 0;
	numRadioChunks[CHUNK_RECEIVE] = 0;
	numRadioChunks[CHUNK_SEND] = 0;
	ejectOrderGiven = false;
	timeLeft = 1.0;
	exploding = false;
	withdrawing = false;
	yieldTimeLeft = 0.0;
	lastValidPosition.Zero();
	pivotDirection = 0;
	lastHustleTime = -999.0;
	salvageVehicle = false;
	markDistanceMoved = 0.0;
	refitBuddyWID = 0;
	recoverBuddyWID = 0;
	crashAvoidSelf = 1;
	crashAvoidPath = 1;
	crashBlockSelf = 1;
	crashBlockPath = 1;
	crashYieldTime = 1.5;
	pathLockLength = 0;
	moveType = MOVETYPE_GROUND;
	moveLevel = 0;
	moveCenter.Zero();
	moveRadius = -1.0;
	followRoads = false;
	overlayWeightClass = 0;
	timeToClearSelection = 0.0;
	timeSinceMoving = 0.0;
	timeSinceFiredLast = MaxTimeRevealed;
	lastMovingTargetWID = 0;
	mechSalvage = true;
	setTangible(true);
	setFlag(OBJECT_FLAG_SENSOR, true);
	if (!sortList)
	{
		sortList = new SortList;
		if (!sortList)
			Fatal(0, " Unable to create Mover::sortList ");
		sortList->init(4000);
	}
	teleportPosition.x = -999999.0;
	teleportPosition.y = -999999.0;
	teleportPosition.z = -999999.0;
	debugPage = 0;
	conStat = 0;
	fadeTime = 0.0f;
	alphaValue = 0x0;
	if (!holdFireIconHandle)
	{
		FullPathFileName path;
		path.init(artPath, "blip", ".tga");
		holdFireIconHandle = mcTextureManager->loadTexture(
			path, gos_Texture_Alpha, gosHint_DisableMipmap | gosHint_DontShrink);
	}
}

//---------------------------------------------------------------------------

void Mover::release(void)
{
	setTeamId(-1, false);
	if (getGroup())
		getGroup()->remove(this);
	setCommanderId(-1);
	MechWarrior::freeWarrior(pilot);
	pilot = nullptr;
}

//---------------------------------------------------------------------------
extern GameObjectPtr DebugGameObject[3];

void Mover::updateDebugWindow(GameDebugWindow* debugWindow)
{
	debugPage %= 3;
	debugWindow->clear();
	static wchar_t s[128];
	MovePathPtr path = getPilot()->getMovePath();
	int32_t distToObj1 = -1;
	if (DebugGameObject[2])
		distToObj1 = (int32_t)distanceFrom(DebugGameObject[2]->getPosition());
	int32_t sensorRange = 0;
	if (sensorSystem)
		sensorRange = (int32_t)sensorSystem->getEffectiveRange();
	int32_t contStat = contactInfo->getContactStatus(0, true);
	static std::wstring_view contactStr[NUM_CONTACT_STATUSES] = {
		"        ", "SENSOR_1", "SENSOR_2", "SENSOR_3", "SENSOR_4", "VISUAL  "};
	if (debugPage == 0)
	{
		sprintf(s, "%s (%s %02d)", getName(), getPilot()->getName(), getPilot()->getIndex());
		debugWindow->print(s);
		int32_t gestID = -1;
		if (getObjectClass() == BATTLEMECH)
			gestID = ((Mech3DAppearance*)appearance)->getCurrentGestureId();
		sprintf(s, "cmdr: %d, team: %d, gestID: %d, partID: %d", getCommanderId(), getTeamId(),
			gestID, getPartId());
		debugWindow->print(s);
		if (path->curStep < path->numStepsWhenNotPaused)
		{
			int32_t curDir = path->stepList[path->curStep].direction;
			if (curDir > 7)
				sprintf(s, "pos: [%d, %d](%d) snsr: %d[%s] JMP", cellPositionRow, cellPositionCol,
					distToObj1, sensorRange, contactStr[contStat]);
			else
				sprintf(s, "pos: [%d, %d](%d) snsr: %d[%s] MOV", cellPositionRow, cellPositionCol,
					distToObj1, sensorRange, contactStr[contStat]);
		}
		else
			sprintf(s, "pos: [%d, %d](%d) snsr: %d[%s]", cellPositionRow, cellPositionCol,
				distToObj1, sensorRange, contactStr[contStat]);
		debugWindow->print(s);
#if 0
		if(getObjectClass() == BATTLEMECH)
		{
			sprintf(s, "JMP: (%.f) cgid=%d, Su=%d, in=%d, Ar=%d",
					getJumpRange(),
					((Mech3DAppearance*)appearance)->getCurrentGestureId(),
					((Mech3DAppearance*)appearance)->isJumpSetup() ? 1 : 0,
					((Mech3DAppearance*)appearance)->isInJump() ? 1 : 0,
					((Mech3DAppearance*)appearance)->isJumpAirborne() ? 1 : 0);
			debugWindow->print(s);
		}
#else
		sprintf(s, "range: [%.f/%.f/%.f] %.f", getMinFireRange(), getOptimalFireRange(),
			getMaxFireRange(),
			getPilot()->getCurrentTarget()
				? distanceFrom(getPilot()->getCurrentTarget()->getPosition())
				: 0.0);
		debugWindow->print(s);
#endif
		strcpy(s, "order: ");
		if (getPilot()->getNumTacOrdersQueued())
		{
			if (getPilot()->getExecutingTacOrderQueue())
			{
				wchar_t ts[40];
				sprintf(ts, "[%d%c] ", getPilot()->getNumTacOrdersQueued(),
					getPilot()->getTacOrderQueueLooping() ? "@" : ".");
				strcat(s, ts);
			}
			else
			{
				wchar_t ts[40];
				sprintf(ts, "[%d] ", getPilot()->getNumTacOrdersQueued());
				strcat(s, ts);
			}
		}
		getPilot()->getCurTacOrder()->debugString(getPilot(), &s[7]);
		debugWindow->print(s);
		if (getPilot()->getBrain())
		{
			ModuleInfo brainInfo;
			getPilot()->getBrain()->getInfo(&brainInfo);
			debugWindow->print(" ");
			sprintf(s, "brain: %s", brainInfo.name);
			debugWindow->print(s);
			//-----------------------------------------------------------------
			// Skip the "data\missions\warriors" text when printing filename...
			sprintf(s, "file: %s", &brainInfo.fileName[23]);
			debugWindow->print(s);
		}
		sprintf(s, "cont:");
		if (sensorSystem)
		{
			// display up to 4...
			int32_t numConts = 4;
			if (sensorSystem->numContacts < 4)
				numConts = sensorSystem->numContacts;
			for (size_t i = 0; i < numConts; i++)
			{
				std::unique_ptr<Mover> contact = (std::unique_ptr<Mover>)ObjectManager->get(sensorSystem->contacts[i] & 0x7FFF);
				Assert(contact != nullptr, sensorSystem->contacts[i] & 0x7FFF, " null contact ");
				wchar_t s2[10];
				sprintf(s2, " %d.%d", sensorSystem->contacts[i] & 0x7FFF,
					contact->getContactStatus(teamId, true));
				strcat(s, s2);
			}
			if (sensorSystem->numContacts > 4)
				strcat(s, " +++");
		}
		debugWindow->print(s);
		if (getPilot()->getCurrentTarget())
		{
			wchar_t ts[50];
			getPilot()->printWeaponsStatus(ts);
			sprintf(s, "%s", ts);
		}
		else
			sprintf(s, "no current target");
		debugWindow->print(s);
		for (size_t i = 0; i < 5; i++)
		{
			sprintf(s, "%s", getPilot()->getDebugString(i));
			debugWindow->print(s);
		}
	}
	else if (debugPage == 1)
	{
		sprintf(s, "%s (%s %02d)", getName(), getPilot()->getName(), getPilot()->getIndex());
		debugWindow->print(s);
		sprintf(s, "team: %d, handle: %d, partID: %d, rank: %d", getTeamId(), getHandle(),
			getPartId(), getPilot()->getRank());
		debugWindow->print(s);
		if (path->curStep < path->numStepsWhenNotPaused)
		{
			int32_t curDir = path->stepList[path->curStep].direction;
			if (curDir > 7)
				sprintf(s, "pos: [%d, %d](%d) snsr: %d[%s] JMP", cellPositionRow, cellPositionCol,
					distToObj1, sensorRange, contactStr[contStat]);
			else
				sprintf(s, "pos: [%d, %d](%d) snsr: %d[%s] MOV", cellPositionRow, cellPositionCol,
					distToObj1, sensorRange, contactStr[contStat]);
		}
		else
			sprintf(s, "pos: [%d, %d](%d) snsr: %d[%s]", cellPositionRow, cellPositionCol,
				distToObj1, sensorRange, contactStr[contStat]);
		debugWindow->print(s);
		sprintf(s, "cur step = [%d, %d], area = %d", 0, 0,
			GlobalMoveMap[0]->calcArea(cellPositionRow, cellPositionCol));
		debugWindow->print(s);
		MoveOrders* moveOrders = getPilot()->getMoveOrders();
		if (moveOrders->numGlobalSteps > 0)
		{
			sprintf(s, "LR:");
			int32_t lastIndex = moveOrders->curGlobalStep + 8;
			if (lastIndex > moveOrders->numGlobalSteps)
				lastIndex = moveOrders->numGlobalSteps;
			for (size_t i = 0; i < lastIndex; i++)
			{
				wchar_t ss[15];
				if (i == moveOrders->curGlobalStep)
					sprintf(ss, " [%d]", moveOrders->globalPath[i].thruArea);
				else
					sprintf(ss, " %d", moveOrders->globalPath[i].thruArea);
				strcat(s, ss);
			}
			if (lastIndex < moveOrders->numGlobalSteps)
				strcat(s, " +");
			debugWindow->print(s);
		}
		static std::wstring_view moveStateStr[NUM_MOVESTATES] = {"SS", "FF", "RR", "*F", "*R", "*T"};
		sprintf(s, "MoveState Cur/Goal = %s[%s]", moveStateStr[pilot->getMoveState()],
			moveStateStr[pilot->getMoveStateGoal()]);
		debugWindow->print(s);
		GameObjectPtr target = pilot->getCurrentTarget();
		float torsoRelFacing = -1.0;
		if (objectClass == BATTLEMECH)
		{
			sprintf(s, "torsoRotation = %.2f", ((BattleMechPtr)this)->torsoRotation);
			debugWindow->print(s);
			if (target)
				torsoRelFacing =
					relFacingTo(target->getPosition()) + ((BattleMechPtr)this)->torsoRotation;
			else if (pilot->getCurTacOrder()->code == TacticalOrderCode::attackpoint)
				torsoRelFacing = relFacingTo(pilot->getAttackTargetPoint()) + ((BattleMechPtr)this)->torsoRotation;
			else
				torsoRelFacing = ((BattleMechPtr)this)->torsoRotation;
			sprintf(s, "C-Bills = %d", ((BattleMechPtr)this)->cBills);
			debugWindow->print(s);
		}
		else if (objectClass == GROUNDVEHICLE)
		{
			sprintf(s, "turretRotation = %.2f", ((GroundVehiclePtr)this)->turretRotation);
			debugWindow->print(s);
			if (target)
				torsoRelFacing =
					relFacingTo(target->getPosition()) + ((GroundVehiclePtr)this)->turretRotation;
			else if (pilot->getCurTacOrder()->code == TacticalOrderCode::attackpoint)
				torsoRelFacing = relFacingTo(pilot->getAttackTargetPoint()) + ((GroundVehiclePtr)this)->turretRotation;
			else
				torsoRelFacing = ((GroundVehiclePtr)this)->turretRotation;
		}
		sprintf(s, "torsoRelFacing = %.2f", torsoRelFacing);
		debugWindow->print(s);
	}
	else if (debugPage == 2)
	{
		static std::wstring_view locationStrings[] = {"head:     ", "c torso:  ", "l torso:  ", "r torso:  ",
			"l arm:    ", "r arm:    ", "l leg:    ", "r leg:    ", "rc torso: ", "rl torso: ",
			"rr torso: ", "front:    ", "left:     ", "right:    ", "rear:     ", "turret:   "};
		static std::wstring_view bodyState[] = {"normal", "DAM", "DEST"};
		sprintf(s, "%s (%s %02d)", getName(), getPilot()->getName(), getPilot()->getIndex());
		debugWindow->print(s);
		sprintf(s, "team: %d, partID: %d, threat: %d", getTeamId(), getPartId(), getThreatRating());
		debugWindow->print(s);
		debugWindow->print(" ");
		if (getObjectClass() == BATTLEMECH)
		{
			for (size_t i = 0; i < numBodyLocations; i++)
			{
				sprintf(s, "%s IS:%3.1f(%02d) AR:%3.1f(%02d) %s", locationStrings[i],
					body[i].curInternalStructure, body[i].maxInternalStructure, armor[i].curArmor,
					armor[i].maxArmor, bodyState[body[i].damageState]);
				debugWindow->print(s);
			}
			for (i = numArmorLocations; i < numArmorLocations; i++)
			{
				sprintf(s, "%s AR:%02d(%02d) %s", locationStrings[i], armor[i].curArmor,
					armor[i].maxArmor);
				debugWindow->print(s);
			}
		}
		else if (getObjectClass() == GROUNDVEHICLE)
		{
			for (size_t i = 0; i < numBodyLocations; i++)
			{
				sprintf(s, "%s IS:%3.1f(%02d) AR:%3.1f(%02d) %s", locationStrings[i + 11],
					body[i].curInternalStructure, body[i].maxInternalStructure, armor[i].curArmor,
					armor[i].maxArmor, bodyState[body[i].damageState]);
				debugWindow->print(s);
			}
		}
	}
}

//---------------------------------------------------------------------------

void Mover::setPartId(int32_t newPartId)
{
	partId = newPartId;
	// MoverRoster[newPartId - MIN_MOVER_PART_ID] = (BaseObjectPtr)this;
}

//---------------------------------------------------------------------------

int32_t
Mover::setTeamId(int32_t _teamId, bool setup)
{
	if (teamId > -1)
	{
		//----------------------------------
		// Remove me from my current team...
		prevTeamId = teamId;
		TeamPtr team = Team::teams[teamId];
		if (sensorSystem)
			SensorManager->removeTeamSensor(teamId, sensorSystem);
		// if (ecm != 255)
		//	SensorManager->removeEcm(this,);
		if (pilot)
			pilot->setTeam(nullptr);
		team->removeFromRoster(this);
	}
	teamId = _teamId;
	Assert(teamId > -2, teamId, " Mover.setTeamId: bad teamId ");
	if (setup)
	{
		if (teamId > -1)
		{
			TeamPtr team = Team::teams[teamId];
			Assert(team != nullptr, 0, " Mover.setTeamId: nullptr team ");
			if (sensorSystem)
				SensorManager->addTeamSensor(teamId, sensorSystem);
			if (pilot)
				pilot->setTeam(team);
		}
	}
	return (NO_ERROR);
}

//---------------------------------------------------------------------------

TeamPtr
Mover::getTeam(void)
{
	if (teamId == -1)
		return (nullptr);
	return (Team::teams[teamId]);
}

//---------------------------------------------------------------------------

CommanderPtr
Mover::getCommander(void)
{
	if (commanderId == -1)
		return (nullptr);
	return (Commander::commanders[commanderId]);
}

//---------------------------------------------------------------------------

bool Mover::isFriendly(TeamPtr team)
{
	if (teamId > -1 && team)
		return (Team::relations[teamId][team->getId()] == RELATION_FRIENDLY);
	return (false);
}

//---------------------------------------------------------------------------

bool Mover::isEnemy(TeamPtr team)
{
	if (teamId > -1 && team)
		return (Team::relations[teamId][team->getId()] == RELATION_ENEMY);
	return (false);
}

//---------------------------------------------------------------------------

bool Mover::isNeutral(TeamPtr team)
{
	if (teamId > -1 && team)
		return (Team::relations[teamId][team->getId()] == RELATION_NEUTRAL);
	return (true);
}

//---------------------------------------------------------------------------

int32_t
Mover::setGroupId(int32_t _groupId, bool setup)
{
	groupId = _groupId;
	if (setup && (groupId > -1) && pilot)
	{
		pilot->clearCurTacOrder(false);
		pilot->setOrderState(ORDERSTATE_GENERAL);
	}
	return (NO_ERROR);
}

//---------------------------------------------------------------------------

MoverGroupPtr
Mover::getGroup(void)
{
	if (groupId == -1)
		return (nullptr);
	return (Commander::commanders[commanderId]->getGroup(groupId));
}

//---------------------------------------------------------------------------

void Mover::setPosition(Stuff::Vector3D& newPosition)
{
	//-------------------------------------------
	// Confirm position as on map and move it
	// back on map if necessary
	float maxMap = Terrain::worldUnitsMapSide / 2.0;
	bool offMap = false;
	if (newPosition.x < -maxMap)
	{
		newPosition.x = -maxMap;
		offMap = true;
	}
	if (newPosition.x > maxMap)
	{
		newPosition.x = maxMap;
		offMap = true;
	}
	if (newPosition.y < -maxMap)
	{
		newPosition.y = -maxMap;
		offMap = true;
	}
	if (newPosition.y > maxMap)
	{
		newPosition.y = maxMap;
		offMap = true;
	}
	if (!offMap)
	{
		//--------------------------------------------------------------
		// New map clip code here.  Actually easy once thought out!
		float maxVisual =
			Terrain::worldUnitsPerVertex * Terrain::blocksMapSide * Terrain::verticesBlockSide / 2;
		maxVisual -= 1300.0;
		float clipChk1 = newPosition.y - newPosition.x;
		float clipChk2 = newPosition.y + newPosition.x;
		bool clipA = (clipChk1 > maxVisual);
		bool clipB = (clipChk1 < -maxVisual);
		bool clipC = (clipChk2 > maxVisual);
		bool clipD = (clipChk2 < -maxVisual);
		if (clipA || clipB || clipC || clipD)
			offMap = true;
	}
	//--------------------------------------------
	//-- Code to whack Object if it moves off of map
	//-- I.e. Withdrawing or a camera Drone.  hehehe.
	if (offMap && withdrawing)
	{
		ObjectManager->getObjectType(typeHandle)->handleDestruction(this, nullptr);
	}
	GameObject::setPosition(newPosition);
	triggerAreaMgr->setHit(this);
	//-------------------------------------------------------------------------------------
	// If we've marked our cell as occupied, and our position has changed, then
	// clear it...
	if (lastMapCell[0] != -1)
		if ((lastMapCell[0] != cellPositionRow) || (lastMapCell[1] != cellPositionCol))
		{
			GameMap->setMover(lastMapCell[0], lastMapCell[1], false);
			lastMapCell[0] = -1;
			lastMapCell[1] = -1;
		}
	//--------------------------------------------------
	// Now, mark our current cell if we're NOT moving...
	if (pilot->getMovePath()->numSteps == 0)
	{
		lastMapCell[0] = cellPositionRow;
		lastMapCell[1] = cellPositionCol;
		GameMap->setMover(cellPositionRow, cellPositionCol, true);
	}
	// if (objPosition)
	//	GameObjectMap->updateObject(this);
}

//---------------------------------------------------------------------------

void Mover::rotate(float angle)
{
	// Old function.  If only one is passed in, facing and rotation are equal!
	rotate(angle, angle);
}

//---------------------------------------------------------------------------
void Mover::rotate(float angle, float facingAngle)
{
	rotation += angle;
	if (rotation < -180.0)
		rotation += 360.0;
	else if (rotation > 180.0)
		rotation -= 360.0;
}

//---------------------------------------------------------------------------

void Mover::setAwake(bool set)
{
	GameObject::setAwake(set);
	if (set && pilot && (getStatus() == OBJECT_STATUS_SHUTDOWN))
		pilot->orderPowerUp(false, OrderOriginType::self);
}

//---------------------------------------------------------------------------

float Mover::relFacingDelta(Stuff::Vector3D goalPos, Stuff::Vector3D targetPos)
{
	float relFacingToGoal = relFacingTo(goalPos);
	float relFacingToTarget = relFacingTo(targetPos);
	float facingDelta = 0.0;
	if (relFacingToGoal >= 0)
	{
		if (relFacingToTarget >= 0)
			if (relFacingToGoal > relFacingToTarget)
				facingDelta = relFacingToGoal - relFacingToTarget;
			else
				facingDelta = relFacingToTarget - relFacingToGoal;
		else
		{
			facingDelta = relFacingToGoal - relFacingToTarget;
			if (facingDelta > 180)
				facingDelta = 360.0 - facingDelta;
		}
	}
	else
	{
		if (relFacingToTarget >= 0)
		{
			facingDelta = -relFacingToGoal + relFacingToTarget;
			if (facingDelta > 180)
				facingDelta = 360.0 - facingDelta;
		}
		else if (relFacingToGoal > relFacingToTarget)
			facingDelta = relFacingToGoal - relFacingToTarget;
		else
			facingDelta = relFacingToTarget - relFacingToGoal;
	}
	return (facingDelta);
}

//---------------------------------------------------------------------------

float Mover::relFacingTo(Stuff::Vector3D goal, int32_t bodyLocation)
{
#if 1
	Stuff::Vector3D facingVec = getRotationVector();
	Stuff::Vector3D goalVec;
	goalVec.Subtract(goal, position);
	goalVec.z = 0.0;
	float angle = angle_from(facingVec, goalVec);
	Stuff::Vector3D angleSign;
	angleSign.Cross(facingVec, goalVec);
	if (angleSign.z >= 0.0)
	{
		angle = -angle;
	}
	return (angle);
#else
	Stuff::Vector3D curPos = position;
	curPos.z = 0;
	goal.z = 0;
	//-------------------------
	// Create vector for facing
	frame_of_ref workFrame = frame;
	// float angle45 = 45.0;
	// workFrame.rotate_about_k(angle45);
	Stuff::Vector3D velVect = -(workFrame.j);
	Stuff::Vector3D goalVec;
	goalVec = goal - curPos;
	goalVec.normalize();
	float resultAngle = velVect.angle_from(goalVec);
	Stuff::Vector3D angleSign = velVect CROSS goalVec;
	if (angleSign.z >= 0.0)
	{
		resultAngle = -resultAngle;
	}
	/*
	if (angle < 180.0) {
		//---------------------------------------------------
		// Now, let's determine if it's to the right or left.
		// If to the right, return a positive angle.
		// If to the left, return a negative angle.
		// There has to be a cleaner way to get this, right?!
		float one = 1.0;
		workFrame.rotate_about_k(one);
		velVect = -(workFrame.j);
		float angle2 = velVect.angle_from(goal - curPos);

		if (angle2 < angle)
			return(-angle);
	}
	*/
	return (resultAngle);
#endif
}

//------------------------------------------------------------------------------------------

float Mover::getTerrainAngle(void)
{
	Stuff::Vector3D worldK(0.0, 0.0, 1.0);
	float result = positionNormal * worldK;
	result = my_acos(result) * RADS_TO_DEGREES;
	return (result);
}

//------------------------------------------------------------------------------------------

float Mover::getVelocityTilt(void)
{
	//---------------------------
	// Create vector for velocity
	Stuff::Vector3D facingVec = getRotationVector();
	float result = positionNormal * facingVec;
	result = my_acos(result) * RADS_TO_DEGREES;
	return (result);
}

//------------------------------------------------------------------------------------------

float Mover::getFireArc(void)
{
	switch (getObjectClass())
	{
	case BATTLEMECH:
		return (FireArc[0]);
		break;
	case GROUNDVEHICLE:
		return (FireArc[1]);
		break;
	case ELEMENTAL:
		return (FireArc[2]);
		break;
	}
	return (45.0);
}

//------------------------------------------------------------------------------------------

void Mover::destroy(void)
{
	// We only destroy on game end.  Most of this is caught in the heap frees!!
	//----------------------------------------------
	// This will free any memory the mover is using.
	if (appearance)
	{
		delete appearance;
		appearance = nullptr;
	}
	if (contactInfo)
	{
		delete contactInfo;
		contactInfo = nullptr;
	}
}

//----------------------------------------------------------------------------------

Stuff::Vector3D
Mover::relativePosition(float angle, float distance, uint32_t flags)
{
	//--------------------------------------------------------
	// Note that the angle should be -180 <= angle <= 180, and
	// the distance is in meters...
	distance *= -worldUnitsPerMeter;
	//--------------------------------------------
	// Absolute facing, based upon north facing...
	Stuff::Vector3D shiftVect(0.0, 1.0, 0.0);
	if ((flags & RELPOS_FLAG_ABS) == 0)
	{
		//----------------------------------------------------
		// Create vector for facing, rotated to adjust for the
		// relative angle...
		shiftVect = getRotationVector();
	}
	Rotate(shiftVect, angle);
	shiftVect *= distance;
	Stuff::Vector3D curPos(position.x, position.y, 0.0);
	Stuff::Vector3D relPos(curPos.x + shiftVect.x, curPos.y + shiftVect.y, 0.0);
	Stuff::Vector3D start;
	Stuff::Vector3D goal;
	Stuff::Vector3D deltaVector;
	if (flags & RELPOS_FLAG_PASSABLE_START)
	{
		start = curPos;
		goal = relPos;
	}
	else
	{
		start = relPos;
		goal = curPos;
	}
	deltaVector.Subtract(goal, start);
	//-------------------------------------------------------------
	// First, we need to calc the delta vector--how much we extend
	// the ray everytime we check the map cell for clear placement.
	deltaVector.Normalize(deltaVector);
	float cellLength = Terrain::worldUnitsPerCell;
	cellLength *= 0.5;
	deltaVector *= cellLength;
	if (deltaVector.GetLength() == 0.0)
		return (curPos);
	//-------------------------------------------------
	// Determine the max length the ray must be cast...
	float maxLength = distance_from(goal, start);
	//------------------------------------------------------------
	// We'll start at the target, and if it's blocked, we'll move
	// toward our start location, looking for the first valid/open
	// cell...
	Stuff::Vector3D curPoint = start;
	Stuff::Vector3D curRay;
	curRay.Zero();
	float rayLength = 0.0;
	int32_t cellR, cellC;
	land->worldToCell(curPoint, cellR, cellC);
	bool cellClear = GameMap->getPassable(cellR, cellC);
	Stuff::Vector3D lastGoodPoint = curPoint;
	if (flags & RELPOS_FLAG_PASSABLE_START)
		while (cellClear && (rayLength < maxLength))
		{
			lastGoodPoint = curPoint;
			curPoint += deltaVector;
			curRay.Subtract(curPoint, start);
			rayLength = curRay.GetLength();
			land->worldToCell(curPoint, cellR, cellC);
			cellClear = GameMap->getPassable(cellR, cellC);
		}
	else
		while (!cellClear && (rayLength < maxLength))
		{
			lastGoodPoint = curPoint;
			curPoint += deltaVector;
			curRay.Subtract(curPoint, start);
			rayLength = curRay.GetLength();
			land->worldToCell(curPoint, cellR, cellC);
			cellClear = GameMap->getPassable(cellR, cellC);
		}
	curPoint.x = lastGoodPoint.x;
	curPoint.y = lastGoodPoint.y;
	curPoint.z = land->getTerrainElevation(curPoint);
	return (curPoint);
}

//---------------------------------------------------------------------------

void Mover::setSensorRange(float range)
{
	if (sensorSystem)
		sensorSystem->setRange(range);
}

//---------------------------------------------------------------------------

bool Mover::hasActiveProbe(void)
{
	return (probe != 255);
}

//---------------------------------------------------------------------------

float Mover::getEcmRange(void)
{
	if ((ecm != 255))
		return 10.0f; // ANY ECM means I am invisible!!
	return (0.0);
}

//---------------------------------------------------------------------------

bool Mover::hasNullSignature(void)
{
	return (nullSignature != 255);
}

//---------------------------------------------------------------------------

extern bool InitWayPath;

int32_t
Mover::handleTacticalOrder(TacticalOrder tacOrder, int32_t priority, bool queuePlayerOrder)
{
	// queuePlayerOrder = true;
	if (queuePlayerOrder)
		tacOrder.pack(nullptr, nullptr);
#if 1
	if (MPlayer && !MPlayer->isServer())
	{
		//----------------------------
		// Simply for test purposes...
		tacOrder.pack(nullptr, nullptr);
		TacticalOrder tacOrder2;
		tacOrder2.data[0] = tacOrder.data[0];
		tacOrder2.data[1] = tacOrder.data[1];
		tacOrder2.unpack();
	}
#endif
	//----------------------------------------------------
	// Any tacorder that gets here IS NOT a lance order...
	bool processOrder = true;
	int32_t message = -1;
	switch (tacOrder.code)
	{
	case TacticalOrderCode::wait:
		suppressionFire = false;
		break;
	case TacticalOrderCode::movetopoint:
		//---------------------------------------------------
		// If we're part of a selected group, offset our goal
		// slightly...
		if (selectionindex != -1)
		{
			tacOrder.delayedTime = scenarioTime + (selectionindex * DelayedOrderTime);
		}
		if (!canMove())
		{
			// DO NOT send readio message.  Players will know artillery cannot
			// move.
			// BUT do not allow order through.  Will cancel out a forceFire
			// command!
			processOrder = false;
		}
		break;
	case TacticalOrderCode::jumptopoint:
	case TacticalOrderCode::jumptoobject:
	{
		bool canJump = (getObjectClass() == BATTLEMECH);
		GameObjectPtr target = tacOrder.getTarget();
		if (target && target->isMover() && (target->getTeam() == getTeam()))
			canJump = false;
		if (getJumpRange() < distanceFrom(tacOrder.getWayPoint(0)))
			canJump = false;
		if (getObjectClass() == BATTLEMECH)
		{
			int32_t cellR, cellC;
			land->worldToCell(tacOrder.getWayPoint(0), cellR, cellC);
			if (!GameMap->getPassable(cellR, cellC))
				canJump = false;
		}
		if (!canJump)
		{
			message = RADIO_ILLEGAL_ORDER;
			processOrder = false;
		}
		// else
		//	message = RADIO_JUMPTO;
	}
	//-----------------------------------------
	// No break here--should fall down below...
	case TacticalOrderCode::movetoobject:
	case TacticalOrderCode::traversepath:
	case TacticalOrderCode::patrolpath:
		//---------------------------------------------------
		// Tell everyone we have received the order.
		if (isDisabled() || !canMove())
		{
			processOrder = false;
		}
		break;
	case TacticalOrderCode::attackobject:
		// tacOrder.code = TacticalOrderCode::attackpoint;
		// tacOrder.attackParams.targetpoint =
		// (GameObjectPtr(BaseObjectPtr(tacOrder.target)))->getPosition();
		// tacOrder.target = nullptr;
		if (tacOrder.attackParams.method == AttackMethod::dfa)
		{
			//-------------------------------------------------
			// Let's just make it a move/jump order, for now...
			tacOrder.code = TacticalOrderCode::jumptoobject;
			// tacOrder.target = mouseObject;
			tacOrder.moveparams.wait = false;
			tacOrder.moveparams.wayPath.mode[0] = TravelModeType::slow;
			GameObjectPtr target = tacOrder.getTarget();
			if (target)
				tacOrder.setWayPoint(0, target->getPosition());
		}
		if (tacOrder.attackParams.aimLocation != -1)
		{
			message = RADIO_CALLED_SHOT;
		}
		break;
	case TacticalOrderCode::powerup:
	case TacticalOrderCode::powerdown:
	// tacOrder.delayedTime = scenarioTime + 5.0;
	// break;
	case TacticalOrderCode::escort:
	case TacticalOrderCode::follow:
	case TacticalOrderCode::guard:
	case TacticalOrderCode::stop:
	case TacticalOrderCode::waypointsdone:
	case TacticalOrderCode::eject:
	case TacticalOrderCode::holdfire:
	case TacticalOrderCode::withdraw:
	case TacticalOrderCode::attackpoint:
	case TacticalOrderCode::refit:
	case TacticalOrderCode::getfixed:
	case TacticalOrderCode::capture:
	case TacticalOrderCode::recover:
	case TacticalOrderCode::loadintocarrier:
	case TacticalOrderCode::deployelementals:
		break;
	default:
	{
		// wchar_t s[256];
		// sprintf(s, "Mover::handleTacticalOrder->Bad TacOrder Code (%d)",
		// tacOrder.code);  Assert(false, tacOrder.code, s);
		NODEFAULT;
		return (1);
	}
	}
	if (pilot)
		pilot->radioMessage(message, true);
	//-------------------------------------------------------
	// Acknowledgement of a good order is done when the order
	// is executed...
	if (processOrder)
		switch (tacOrder.origin)
		{
		case OrderOriginType::player:
			if (queuePlayerOrder)
				pilot->addQueuedTacOrder(tacOrder);
			else
			{
				//					if (pilot->getNumTacOrdersQueued())
				// This is a hack to simply trigger the execution of
				// the queued orders. The current order is ignored (and
				// is simply used for this trigger)...
				//						pilot->executeTacOrderQueue();
				//					else
				pilot->setPlayerTacOrder(tacOrder);
			}
			break;
		case OrderOriginType::commander:
			pilot->setGeneralTacOrder(tacOrder);
			break;
		case OrderOriginType::self:
			pilot->setAlarmTacOrder(tacOrder, priority);
			break;
		}
	return (NO_ERROR);
}

//----------------------------------------------------------------------------------

void Mover::reduceAntiMissileAmmo(int32_t numAntiMissiles)
{
	if (numAntiMissiles > 0)
		reduceAmmo(MasterComponent::masterList[inventory[antiMissileSystem[0]].masterID]
					   .getWeaponAmmoMasterId(),
			numAntiMissiles);
}

//----------------------------------------------------------------------------------

void Mover::pilotingCheck(uint32_t situation, float modifier)
{
	failedPilotingCheck = false;
}

//-------------------------------------------------------------------------------------------

void Mover::updateDamageTakenRate(void)
{
	if (damageRateCheckTime < scenarioTime)
	{
		int32_t damageRate = (damageRateTally / DamageRateFrequency);
		if (damageRate > 10 /*AttitudeEffect[pilot->getAttitude(OrderType::current)][5]*/)
			pilot->triggerAlarm(PILOT_ALARM_DAMAGE_TAKEN_RATE, damageRate);
		damageRateTally = 0;
		damageRateCheckTime += DamageRateFrequency;
	}
}

//-------------------------------------------------------------------------------------------

void Mover::setPilotHandle(int32_t _pilotHandle)
{
	if (pilot)
	{
		pilot->setVehicle(nullptr);
		pilot = nullptr;
	}
	pilotHandle = _pilotHandle;
	if (pilotHandle > 0)
	{
		pilot = MechWarrior::warriorList[pilotHandle];
		pilot->setVehicle((GameObjectPtr)this);
	}
}

//---------------------------------------------------------------------------

void Mover::loadPilot(std::wstring_view pilotFileName, std::wstring_view brainFileName, LogisticsPilot* lPilot)
{
	if (pilot)
	{
		MechWarrior::freeWarrior(pilot);
		pilot = nullptr;
	}
	//---------------------------------------------------------------
	// Loads a new pilot into the pilot manager and then puts it into
	// this mover. If a pilot already exists in this mover, it is
	// replaced...
	std::unique_ptr<MechWarrior> pilot = MechWarrior::newWarrior();
	if (!pilot)
		STOP(("Too many pilots in this mission!"));
	FullPathFileName pilotFullFileName;
	pilotFullFileName.init(warriorPath, pilotFileName, ".fit");
	FitIniFile* pilotFile = new FitIniFile;
	gosASSERT(pilotFile != nullptr);
	int32_t result = pilotFile->open(pilotFullFileName);
	gosASSERT(result == NO_ERROR);
	result = pilot->init(pilotFile);
	gosASSERT(result == NO_ERROR);
	pilotFile->close();
	delete pilotFile;
	pilotFile = nullptr;
	// Copy logistics data to pilot AFTER loading old data.
	// ONLY if we overrode the data in logistics!!
	if (lPilot)
	{
		pilot->skills[Skill::gunnery] = pilot->skillRank[Skill::gunnery] = lPilot->getGunnery();
		pilot->skills[Skill::piloting] = pilot->skillRank[Skill::piloting] = lPilot->getPiloting();
		memcpy(pilot->specialtySkills, lPilot->getSpecialtySkills(),
			sizeof(bool) * NUM_SPECIALTY_SKILLS);
		pilot->calcRank();
	}
	//*********************
	// NOTE: Need to send packet to other players in MP with new pilot and
	// mover data!
	//*********************
	int32_t numErrors, numLinesProcessed;
	FullPathFileName brainFullFileName;
	if (MPlayer)
	{
		pilot->setBrainName("pbrain");
		brainFullFileName.init(warriorPath, "pbrain", ".abl");
	}
	else
	{
		pilot->setBrainName(brainFileName);
		brainFullFileName.init(warriorPath, brainFileName, ".abl");
	}
	int32_t moduleHandle = ABLi_preProcess(brainFullFileName, &numErrors, &numLinesProcessed);
	gosASSERT(moduleHandle >= 0);
	pilot->setBrain(moduleHandle);
	setPilotHandle(pilot->getIndex());
}

//---------------------------------------------------------------------------

void Mover::setCommanderId(int32_t _commanderId)
{
	if (commanderId > -1)
		prevCommanderId = commanderId;
	commanderId = _commanderId;
}

//---------------------------------------------------------------------------

std::unique_ptr<Mover>
Mover::getPoint(void)
{
#ifdef USE_GROUPS
	if (group)
		return (group->getPoint());
#endif
	return (nullptr);
}

//---------------------------------------------------------------------------

bool Mover::hasWeaponNode(void)
{
	if ((lowestWeaponNodeID == -1) || (lowestWeaponNodeID == -2) || (turn < 6))
	{
		lowestWeaponNodeID = appearance->getLowestWeaponNode();
		Stuff::Vector3D nodePos = appearance->getWeaponNodePosition(lowestWeaponNodeID);
		lowestWeaponNodeZ = nodePos.z - land->getTerrainElevation(nodePos);
	}
	return (lowestWeaponNodeID != -1);
}

//---------------------------------------------------------------------------

Stuff::Vector3D
Mover::getLOSPosition(void)
{
	float jumpLowestNode = 0.0f;
	if (appearance)
	{
		bool oldInView = appearance->canBeSeen();
		appearance->setInView(true);
		if ((getStatus() == OBJECT_STATUS_NORMAL) && ((appearance->getCurrentGestureId() == 2) || (appearance->getCurrentGestureId() == 4) || (appearance->getCurrentGestureId() == 7) || !isMech()) && ((lowestWeaponNodeID == -1) || (lowestWeaponNodeID == -2) || (turn < 6)))
		{
			lowestWeaponNodeID = appearance->getLowestWeaponNode();
			if (lowestWeaponNodeID == -1)
			{
				// We must not have ever rendered this guy.  Probably
				// multiplayer.
				// Update his Geometry.
				appearance->update();
				lowestWeaponNodeID = appearance->getLowestWeaponNode();
			}
			Stuff::Vector3D nodePos = appearance->getWeaponNodePosition(lowestWeaponNodeID);
			lowestWeaponNodeZ = nodePos.z - land->getTerrainElevation(nodePos);
		}
		if (appearance->getCurrentGestureId() == 20)
		{
			Stuff::Vector3D nodePos = appearance->getWeaponNodePosition(lowestWeaponNodeID);
			jumpLowestNode = nodePos.z - land->getTerrainElevation(nodePos);
		}
		if (lowestWeaponNodeZ <= 0.0f)
		{
			Stuff::Vector3D nodePos = appearance->getWeaponNodePosition(lowestWeaponNodeID);
			lowestWeaponNodeZ = nodePos.z - land->getTerrainElevation(nodePos);
		}
		appearance->setInView(oldInView);
	}
	Stuff::Vector3D losPos = position;
	if (lowestWeaponNodeZ != -9999.9f)
		losPos.z += lowestWeaponNodeZ;
	else
		losPos.z += 15.0f; // Fudge in some higher value so shutdown mechs don't
			// just disappear!!
	if (jumpLowestNode > 0.0f)
	{
		losPos = position;
		losPos.z += jumpLowestNode;
	}
	return (losPos);
}

//---------------------------------------------------------------------------

void Mover::printFireWeaponDebugInfo(GameObjectPtr target, Stuff::Vector3D* targetpoint,
	int32_t chance, int32_t aimLocation, int32_t roll, WeaponShotInfo* shotInfo)
{
	if (!CombatLog)
		return;
	static std::wstring_view locationStrings[] = {"head", "center torso", "left torso", "right torso",
		"left arm", "right arm", "left leg", "right leg", "rear center torso", "rear left torso",
		"rear right torso"};
	if (roll < chance)
	{
		if (target)
		{
			std::wstring_view targetName = target->getName();
			wchar_t s[1024];
			if (getObjectClass() == BATTLEMECH)
				sprintf(s, "[%.2f] mech.fireWeapon HIT: (%05d)%s @ (%05d)%s", scenarioTime,
					getPartId(), name, target->getPartId(), targetName ? targetName : "unknown");
			else
				sprintf(s, "[%.2f] vehicle.fireWeapon HIT: (%05d)%s @ (%05d)%s", scenarioTime,
					getPartId(), name, target->getPartId(), targetName ? targetName : "unknown");
			CombatLog->write(s);
			sprintf(s, "     chance = %03d, roll = %03d", chance, roll);
			CombatLog->write(s);
			sprintf(s,
				"     weapon = (%03d)%s, hitLocation = (%d)%s, damage = %.2f, "
				"angle = %.2f",
				shotInfo->masterId, MasterComponent::masterList[shotInfo->masterId].getName(),
				shotInfo->hitLocation,
				(shotInfo->hitLocation > -1) ? locationStrings[shotInfo->hitLocation] : "none",
				shotInfo->damage, shotInfo->entryAngle);
			CombatLog->write(s);
			if (aimLocation != -1)
			{
				sprintf(s, "     aimed shot = (%d)%s", aimLocation, locationStrings[aimLocation]);
				CombatLog->write(s);
			}
			sprintf(
				s, "     attacker pilot = %s (team id = %d)", getPilot()->getName(), getTeamId());
			CombatLog->write(s);
			CombatLog->write(" ");
		}
		else if (targetpoint)
		{
		}
	}
	else
	{
		if (target)
		{
			std::wstring_view targetName = target->getName();
			wchar_t s[1024];
			if (getObjectClass() == BATTLEMECH)
				sprintf(s, "[%.2f] mech.fireWeapon MISS: (%05d)%s @ (%05d)%s", scenarioTime,
					getPartId(), name, target->getPartId(), targetName ? targetName : "unknown");
			else
				sprintf(s, "[%.2f] vehicle.fireWeapon HIT: (%05d)%s @ (%05d)%s", scenarioTime,
					getPartId(), name, target->getPartId(), targetName ? targetName : "unknown");
			CombatLog->write(s);
			sprintf(s, "     chance = %03d, roll = %03d", chance, roll);
			CombatLog->write(s);
			sprintf(s,
				"     weapon = (%03d)%s, hitLocation = (%d)%s, damage = %.2f, "
				"angle = %.2f",
				shotInfo->masterId, MasterComponent::masterList[shotInfo->masterId].getName(),
				shotInfo->hitLocation,
				(shotInfo->hitLocation > -1) ? locationStrings[shotInfo->hitLocation] : "none",
				shotInfo->damage, shotInfo->entryAngle);
			CombatLog->write(s);
			if (aimLocation != -1)
			{
				sprintf(s, "     aimed shot = (%d)%s", aimLocation, locationStrings[aimLocation]);
				CombatLog->write(s);
			}
			sprintf(
				s, "     attacker pilot = %s (team id = %d)", getPilot()->getName(), getTeamId());
			CombatLog->write(s);
			CombatLog->write(" ");
		}
		else if (targetpoint)
		{
		}
	}
}

//----------------------------------------------------------------------------

bool FromMP = false;

void Mover::printHandleWeaponHitDebugInfo(WeaponShotInfo* shotInfo)
{
	if (!CombatLog)
		return;
	static std::wstring_view locationStrings[] = {"head", "center torso", "left torso", "right torso",
		"left arm", "right arm", "left leg", "right leg", "rear center torso", "rear left torso",
		"rear right torso"};
	wchar_t s[1024];
	wchar_t statusStr[15];
	if (FromMP)
	{
		if (isDestroyed())
			sprintf(statusStr, " DESTROYED:");
		else if (getTeam() && Team::noPain[getTeamId()])
			sprintf(statusStr, " NO PAIN:");
		else
			sprintf(statusStr, ":");
	}
	else
	{
		if (isDestroyed())
			sprintf(statusStr, " *** DESTROYED:");
		else if (getTeam() && Team::noPain[getTeamId()])
			sprintf(statusStr, " *** NO PAIN:");
		else
			sprintf(statusStr, " ***:");
	}
	if (getObjectClass() == BATTLEMECH)
		sprintf(s, "[%.2f] mech.handleWeaponHit%s (%05d)%s ", scenarioTime, statusStr, getPartId(),
			name);
	else
		sprintf(s, "[%.2f] vehicle.handleWeaponHit%s (%05d)%s ", scenarioTime, statusStr,
			getPartId(), name);
	CombatLog->write(s);
	GameObjectPtr attacker = ObjectManager->getByWatchID(shotInfo->attackerWID);
	if (attacker)
		sprintf(s, "     attacker = (%05d)%s", attacker->getPartId(), attacker->getName());
	else
		sprintf(s, "     attacker = (%05)<unknown WID>", shotInfo->attackerWID);
	CombatLog->write(s);
	sprintf(s,
		"     weapon = (%03d)%s, hitLocation = (%d)%s, damage = %.2f, angle = "
		"%.2f",
		shotInfo->masterId, MasterComponent::masterList[shotInfo->masterId].getName(),
		shotInfo->hitLocation,
		(shotInfo->hitLocation > -1) ? locationStrings[shotInfo->hitLocation] : "none",
		shotInfo->damage, shotInfo->entryAngle);
	CombatLog->write(s);
	sprintf(s, "     num weaponhits = %d", numWeaponHitsHandled);
	CombatLog->write(s);
	CombatLog->write(" ");
}

//----------------------------------------------------------------------------

int32_t
Mover::clearWeaponFireChunks(int32_t which)
{
	int32_t numChunks = numWeaponFireChunks[which];
	numWeaponFireChunks[which] = 0;
	return (numChunks);
}

//---------------------------------------------------------------------------

int32_t
Mover::addWeaponFireChunk(int32_t which, WeaponFireChunkPtr chunk)
{
	if (numWeaponFireChunks[which] == MAX_WEAPONFIRE_CHUNKS)
		Fatal(0, " Mover::addWeaponFireChunk--Too many weaponfire chunks ");
	chunk->pack(this);
	WeaponFireChunk testChunk;
	testChunk.init();
	testChunk.data = chunk->data;
	testChunk.unpack(this);
	if (!chunk->equalTo(&testChunk))
	{
		DebugWeaponFireChunk(chunk, &testChunk, this);
		Assert(false, 0,
			" Mover.addWeaponFireChunk: bad weaponfire chunk (save wfchunk.dbg "
			"file) ");
	}
	weaponFireChunks[which][numWeaponFireChunks[which]++] = chunk->data;
	return (numWeaponFireChunks[which]);
}

//---------------------------------------------------------------------------

int32_t
Mover::addWeaponFireChunks(int32_t which, uint32_t* packedChunkBuffer, int32_t numChunks)
{
	if ((numWeaponFireChunks[which] + numChunks) >= MAX_WEAPONFIRE_CHUNKS)
		Fatal(0, " Mover::addWeaponFireChunks--Too many weaponfire chunks ");
#if 0
	memcpy(&weaponFireChunks[which][numWeaponFireChunks[which]], packedChunkBuffer, 4 * numChunks);
	numWeaponFireChunks[which] += numChunks;
#else
	for (size_t i = 0; i < numChunks; i++)
	{
		weaponFireChunks[which][numWeaponFireChunks[which]++] = packedChunkBuffer[i];
		//---------------
		// FOR TESTING...
		WeaponFireChunk chunk;
		chunk.init();
		chunk.data = packedChunkBuffer[i];
		chunk.unpack(this);
	}
#endif
	return (numWeaponFireChunks[which]);
}

//---------------------------------------------------------------------------

int32_t
Mover::grabWeaponFireChunks(int32_t which, uint32_t* packedChunkBuffer, int32_t maxChunks)
{
	int32_t numChunks = numWeaponFireChunks[which];
	if (numChunks > maxChunks)
		numChunks = maxChunks;
	if (numChunks > 0)
	{
		for (size_t i = 0; i < numChunks; i++)
			packedChunkBuffer[i] = weaponFireChunks[which][i];
		// memcpy(packedChunkBuffer, weaponFireChunks[which], 4 *
		// numWeaponFireChunks[which]);
	}
	numWeaponFireChunks[which] -= numChunks;
	return (numChunks);
}

//---------------------------------------------------------------------------

int32_t
Mover::updateWeaponFireChunks(int32_t which)
{
	for (size_t i = 0; i < numWeaponFireChunks[which]; i++)
	{
		WeaponFireChunk chunk;
		chunk.init();
		chunk.data = weaponFireChunks[which][i];
		chunk.unpack(this);
		//-----------------
		// For debugging...
		CurMoverWeaponFireChunk = chunk;
		static float entryQuadTable[4] = {0.0, 180.0, -90.0, 90.0};
		int32_t weaponIndex = chunk.weaponIndex + numOther;
		if (!isWeaponIndex(weaponIndex))
			continue;
		TargetRolo = chunk.targetType;
		if (chunk.targetType == 0 /*WEAPONFIRECHUNK_TARGET_MOVER*/)
		{
			GameObjectPtr target = (GameObjectPtr)MPlayer->moverRoster[chunk.targetId];
			//----------------------------------------------------------------------------
			// Mover targets could be nullptr now, since we free them when
			// they're destroyed.
			if (target)
				handleWeaponFire(weaponIndex, target, nullptr, chunk.hit,
					entryQuadTable[chunk.entryAngle], chunk.numMissiles, chunk.hitLocation);
		}
		else if (chunk.targetType == 1 /*WEAPONFIRECHUNK_TARGET_TERRAIN*/)
		{
			GameObjectPtr target = ObjectManager->findByPartId(chunk.targetId);
			if (target == nullptr)
			{
				DebugWeaponFireChunk(&chunk, nullptr, this);
				Assert(false, 0,
					" Mover.updateWeaponFireChunks: nullptr Terrain Target "
					"(save wfchunk.dbg file) ");
			}
			handleWeaponFire(weaponIndex, target, nullptr, chunk.hit,
				entryQuadTable[chunk.entryAngle], chunk.numMissiles, chunk.hitLocation);
		}
		else if (chunk.targetType == 2 /*WEAPONFIRECHUNK_TARGET_TRAIN*/)
		{
			GameObjectPtr target = ObjectManager->findByPartId(chunk.targetId);
			if (target == nullptr)
			{
				DebugWeaponFireChunk(&chunk, nullptr, this);
				Assert(false, 0,
					" Mover.updateWeaponFireChunks: nullptr Special Target "
					"(save wfchunk.dbg file) ");
			}
			handleWeaponFire(weaponIndex, target, nullptr, chunk.hit,
				entryQuadTable[chunk.entryAngle], chunk.numMissiles, chunk.hitLocation);
		}
		else if (chunk.targetType == 3 /*WEAPONFIRECHUNK_TARGET_LOCATION*/)
		{
			Stuff::Vector3D targetpoint;
			targetpoint.x = (float)chunk.targetCell[1] * Terrain::worldUnitsPerCell + Terrain::worldUnitsPerCell / 2 - Terrain::worldUnitsMapSide / 2;
			targetpoint.y = (Terrain::worldUnitsMapSide / 2) - ((float)chunk.targetCell[0] * Terrain::worldUnitsPerCell) - Terrain::worldUnitsPerCell / 2;
			targetpoint.z = (float)land->getTerrainElevation(targetpoint);
			handleWeaponFire(
				weaponIndex, nullptr, &targetpoint, chunk.hit, 0.0, chunk.numMissiles, 0);
		}
		else
			Fatal(0, " Mover.updateWeaponFireChunks: bad targetType ");
	}
	numWeaponFireChunks[which] = 0;
	return (NO_ERROR);
}

//---------------------------------------------------------------------------

int32_t
Mover::clearCriticalHitChunks(int32_t which)
{
	int32_t numChunks = numCriticalHitChunks[which];
	numCriticalHitChunks[which] = 0;
	return (numChunks);
}

//---------------------------------------------------------------------------

int32_t
Mover::addCriticalHitChunk(int32_t which, int32_t bodyLocation, int32_t criticalSpace)
{
	if (numCriticalHitChunks[which] == MAX_CRITICALHIT_CHUNKS)
		Fatal(0, " Mover::addCriticalHitChunk--Too many criticalhit chunks ");
	//-----------------------------------------------------------
	// Since the "chunk" is so simple, we'll just make it here...
	uint8_t chunkData = (uint8_t)((bodyLocation << 4) + criticalSpace);
	criticalHitChunks[which][numCriticalHitChunks[which]++] = chunkData;
	return (numCriticalHitChunks[which]);
}

//---------------------------------------------------------------------------

int32_t
Mover::addCriticalHitChunks(int32_t which, uint8_t* packedChunkBuffer, int32_t numChunks)
{
	if ((numCriticalHitChunks[which] + numChunks) >= MAX_CRITICALHIT_CHUNKS)
		Fatal(0, " Mover::addCriticalHitChunks--Too many criticalhit chunks ");
	memcpy(&criticalHitChunks[which][numCriticalHitChunks[which]], packedChunkBuffer, numChunks);
	numCriticalHitChunks[which] += numChunks;
	return (numCriticalHitChunks[which]);
}

//---------------------------------------------------------------------------

int32_t
Mover::grabCriticalHitChunks(int32_t which, uint8_t* packedChunkBuffer)
{
	if (numCriticalHitChunks[which] > 0)
		memcpy(packedChunkBuffer, criticalHitChunks[which], numCriticalHitChunks[which]);
	return (numCriticalHitChunks[which]);
}

//---------------------------------------------------------------------------

int32_t
Mover::updateCriticalHitChunks(int32_t which)
{
	numCriticalHitChunks[which] = 0;
	return (NO_ERROR);
}

//---------------------------------------------------------------------------

int32_t
Mover::clearRadioChunks(int32_t which)
{
	int32_t numChunks = numRadioChunks[which];
	numRadioChunks[which] = 0;
	return (numChunks);
}

//---------------------------------------------------------------------------

int32_t
Mover::addRadioChunk(int32_t which, uint8_t msg)
{
	if (numRadioChunks[which] == MAX_RADIO_CHUNKS)
		return (numRadioChunks[which]);
	// Fatal(0, " Mover::addRadioChunk--Too many radio chunks ");
	//-----------------------------------------------------------
	// Since the "chunk" is so simple, we'll just make it here...
	radioChunks[which][numRadioChunks[which]++] = msg;
	return (numRadioChunks[which]);
}

//---------------------------------------------------------------------------

int32_t
Mover::addRadioChunks(int32_t which, uint8_t* packedChunkBuffer, int32_t numChunks)
{
	//	if ((numRadioChunks[which] + numChunks) >= MAX_RADIO_CHUNKS)
	//		Fatal(0, " Mover::addRadioChunks--Too many radio chunks ");
	for (size_t i = 0; i < numChunks; i++)
		addRadioChunk(which, packedChunkBuffer[i]);
	// memcpy(&radioChunks[which][numRadioChunks[which]], packedChunkBuffer,
	// numChunks);  numRadioChunks[which] += numChunks;
	return (numRadioChunks[which]);
}

//---------------------------------------------------------------------------

int32_t
Mover::grabRadioChunks(int32_t which, uint8_t* packedChunkBuffer)
{
	if (numRadioChunks[which] > 0)
		memcpy(packedChunkBuffer, radioChunks[which], numRadioChunks[which]);
	return (numRadioChunks[which]);
}

//---------------------------------------------------------------------------

int32_t
Mover::updateRadioChunks(int32_t which)
{
	if (getCommander() == Commander::home)
		for (size_t i = 0; i < numRadioChunks[which]; i++)
			playMessage((RadioMessageType)radioChunks[which][i]);
	numRadioChunks[which] = 0;
	return (NO_ERROR);
}

//---------------------------------------------------------------------------

void Mover::playMessage(RadioMessageType messageId, bool propogateIfMultiplayer)
{
	if (pilot)
		pilot->radioMessage(messageId, propogateIfMultiplayer);
}

//---------------------------------------------------------------------------

void Mover::setThreatRating(int16_t rating)
{
	threatRating = rating;
	if (threatRating == -1)
	{
		//-----------
		// Calc it...
		threatRating = calcCV();
	}
}

//---------------------------------------------------------------------------

int32_t
Mover::getThreatRating(void)
{
	if ((scenarioTime - creationTime) < 5.0)
		return ((int32_t)((float)threatRating * newThreatMultiplier));
	return (threatRating);
}

//---------------------------------------------------------------------------

bool Mover::enemyRevealed(void)
{
	//-----------------------------------------------------
	// What is our block and vertex number?
	// NOTE: This function not used Anymore!!
	return (false);
}

//---------------------------------------------------------------------------

#if 0

void Mover::getDamageClass(int32_t& damageClass, bool& shutDown)
{
	//--------------------------------------------
	// DamageClass is based upon the current CV...
	float CV = (float)curCV / (float)maxCV;
	if(CV > 0.90)
		damageClass = DAMAGE_CLASS_NONE;
	else if(CV > 0.75)
		damageClass = DAMAGE_CLASS_LIGHT;
	else if(CV > 0.50)
		damageClass = DAMAGE_CLASS_MODERATE;
	else if(CV > 0.10)
		damageClass = DAMAGE_CLASS_SEVERE;
	else
		damageClass = DAMAGE_CLASS_WRECKAGE;
	//--------------------------------------------
	// Is the mech currently shutdown due to heat?
	shutDown = (status == OBJECT_STATUS_SHUTDOWN);
}

#endif

//------------------------------------------------------------------------------------------

void Mover::setTeleportPosition(Stuff::Vector3D& newPos)
{
	teleportPosition = newPos;
}

//------------------------------------------------------------------------------------------

int32_t
Mover::getInventoryDamage(int32_t itemIndex)
{
	if (itemIndex < (numOther + numWeapons + numAmmos))
		return (MasterComponent::masterList[inventory[itemIndex].masterID].getHealth() - inventory[itemIndex].health);
	return (0);
}

//------------------------------------------------------------------------------------------

float Mover::getVisualRange(void)
{
	return (MechWarrior::maxVisualRadius);
}

//------------------------------------------------------------------------------------------

void Mover::setChallenger(GameObjectPtr challenger)
{
	challengerWID = challenger->getWatchID();
}

//------------------------------------------------------------------------------------------

GameObjectPtr
Mover::getChallenger(void)
{
	//------------------------------------------------------
	// We ASSUME a disabled challenger is as good as none...
	GameObjectPtr obj = ObjectManager->get(challengerWID);
	if (obj && obj->isDisabled())
	{
		challengerWID = 0;
		return (nullptr);
	}
	return (obj);
}

//------------------------------------------------------------------------------------------

Stuff::Vector3D
Mover::calcOffsetMoveGoal(Stuff::Vector3D target)
{
	Stuff::Vector3D start = getPosition();
	Stuff::Vector2DOf<float> start2d;
	start2d.x = start.x;
	start2d.y = start.y;
	Stuff::Vector2DOf<float> target2d;
	target2d.x = target.x;
	target2d.y = target.y;
	//---------------------------------------------------------------
	// First, we need to calc the delta vector--how much we extend
	// the ray everytime we check the map cell for clear placement.
	Stuff::Vector2DOf<float> deltaVector;
	deltaVector.Subtract(start2d, target2d);
	if ((deltaVector.x == 0.0) && (deltaVector.y == 0.0))
	{
		return (start);
	}
	deltaVector.Normalize(deltaVector);
	float cellLength = (Terrain::worldUnitsPerCell);
	cellLength *= 0.5;
	deltaVector *= cellLength;
	// if (deltaVector.GetLength() == 0.0) {
	//	newGoal = start;
	//	return(NO_ERROR);
	//}
	//------------------------------------------
	// Determine how far the ray must be cast...
	float distanceToTarget = distance_from(start, target) /* * metersPerWorldUnit*/;
	//------------------------------------------------------------
	// We'll start at the target, and if it's blocked, we'll move
	// toward our start location, looking for the first valid/open
	// cell...
	Stuff::Vector2DOf<float> curPoint;
	curPoint.x = target.x;
	curPoint.y = target.y;
	Stuff::Vector2DOf<float> curRay;
	curRay.x = 0.0;
	curRay.y = 0.0;
	float rayLength = 0.0;
	int32_t cellR, cellC;
	Stuff::Vector3D curPoint3d(curPoint.x, curPoint.y, 0.0);
	land->worldToCell(curPoint3d, cellR, cellC);
	if (!GameMap->getPassable(cellR, cellC))
		while (rayLength < distanceToTarget)
		{
			curPoint3d.x = curPoint.x;
			curPoint3d.y = curPoint.y;
			curPoint3d.z = 0.0;
			land->worldToCell(curPoint3d, cellR, cellC);
			if (GameMap->getPassable(cellR, cellC))
				break;
			curPoint += deltaVector;
			curRay.Subtract(curPoint, start2d);
			rayLength = curRay.GetLength() /* * metersPerWorldUnit*/;
		}
	curPoint3d.z = land->getTerrainElevation(curPoint3d);
	return (curPoint3d);
}

//---------------------------------------------------------------------------

#define MAX_MOVE_GOALS 60

inline void
insertMoveGoal(int32_t goalList[MAX_MOVE_GOALS][3], int32_t r, int32_t c, int32_t wt)
{
	//------------------------------------------------------------
	// This routine assumes wt > goaList[MAX_MOVE_GOALS - 1][2]...
	// AND that MAX_MOVE_GOALS >= 2...
	for (size_t i = MAX_MOVE_GOALS - 2; i > -1; i--)
		if (wt < goalList[i][2])
			break;
	if (i < (MAX_MOVE_GOALS - 2))
		memmove(&goalList[i + 2][0], &goalList[i + 1][0],
			(MAX_MOVE_GOALS - 2 - i) * sizeof(int32_t) * 3);
	goalList[i + 1][0] = r;
	goalList[i + 1][1] = c;
	goalList[i + 1][2] = wt;
}

//---------------------------------------------------------------------------

inline bool
inMapBounds(int32_t r, int32_t c, int32_t mapheight, int32_t mapwidth)
{
	return ((r >= 0) && (r < mapheight) && (c >= 0) && (c < mapwidth));
}

//---------------------------------------------------------------------------

int32_t
calcBestGoalFromTarget(
	int32_t targetPos[2], int32_t maxPos[2], float minRange, float bestRange, int32_t* bestCell)
{
	float startLocal = 40.0f;
	//------------------------------------------------------------------------------------------
	// Within magic radius.  Check REAL LOS now.
	// Check is really simple.
	// Find deltaCellRow and deltaCellCol and iterate over them from source to
	// dest. If the magic line ever goes BELOW the terrainElevation PLUS
	// localElevation return false.
	Stuff::Vector3D startPos, endPos;
	startPos.Zero();
	endPos.Zero();
	land->getCellPos(maxPos[0], maxPos[1], endPos);
	land->getCellPos(targetPos[0], targetPos[1], startPos);
	Stuff::Vector3D deltaCellVec;
	deltaCellVec.y = maxPos[0] - targetPos[0];
	deltaCellVec.x = maxPos[1] - targetPos[1];
	deltaCellVec.z = 0.0f;
	float startheight = startPos.z;
	float cellLength = (Terrain::worldUnitsPerCell * metersPerWorldUnit);
	float length = deltaCellVec.GetApproximateLength();
	int32_t lastCellR = -1;
	int32_t lastCellC = -1;
	if (length > Stuff::SMALL)
	{
		float colLength = deltaCellVec.x / length;
		float rowLength = deltaCellVec.y / length;
		float heightLen = (endPos.z - startPos.z) / length;
		float lastCol = fabs(colLength * 2.0);
		float lastRow = fabs(rowLength * 2.0);
		float startCellRow = targetPos[0];
		float startCellCol = targetPos[1];
		float endCellRow = maxPos[0];
		float endCellCol = maxPos[1];
		Stuff::Vector3D currentPos = startPos;
		Stuff::Vector3D dist;
		dist.Subtract(endPos, currentPos);
		float remainingDist = dist.GetApproximateLength();
		float bestRemainingDist = remainingDist - (bestRange * worldUnitsPerMeter);
		bool colDone = false, rowDone = false;
		while (!colDone || !rowDone)
		{
			if (fabs(startCellRow - endCellRow) > lastRow) // DO NOT INCLUDE LAST CELL!!!!!
				startCellRow += rowLength;
			else
			{
				startCellRow = (endCellRow - lastRow);
				rowDone = true;
			}
			if (fabs(startCellCol - endCellCol) > lastCol) // DO NOT INCLUDE LAST CELL!!!!!
				startCellCol += colLength;
			else
			{
				startCellCol = (endCellCol - lastCol);
				colDone = true;
			}
			startheight += heightLen;
			int32_t startCellC = startCellCol;
			int32_t startCellR = startCellRow;
			land->getCellPos(startCellR, startCellC, currentPos);
			float localElev =
				(worldUnitsPerMeter * (float)GameMap->getLocalheight(startCellR, startCellC));
			currentPos.z += localElev;
			if (startheight + startLocal < currentPos.z)
				if (GameMap->inBounds(lastCellR, lastCellC) && GameMap->getCell(lastCellR, lastCellC)->getPassable())
				{
					bestCell[0] = lastCellR;
					bestCell[1] = lastCellC;
					return (1);
				}
			dist.Subtract(endPos, currentPos);
			remainingDist = dist.GetApproximateLength();
			if (fabs(remainingDist - bestRemainingDist) < cellLength)
				if (remainingDist > bestRemainingDist)
					if (GameMap->inBounds(startCellR, startCellC) && GameMap->getCell(startCellR, startCellC)->getPassable())
					{
						bestCell[0] = startCellR;
						bestCell[1] = startCellC;
						return (1);
					}
			if (remainingDist < 10.0)
			{
				if (GameMap->inBounds(startCellR, startCellC) && GameMap->getCell(startCellR, startCellC)->getPassable())
				{
					bestCell[0] = startCellR;
					bestCell[1] = startCellC;
					return (1);
				}
				else
					return (0);
			}
			if (GameMap->inBounds(startCellR, startCellC) && GameMap->getCell(startCellR, startCellC)->getPassable())
			{
				//-----------------------------------------------------------
				// Save the last passable cell checked so far that has LOS...
				lastCellR = startCellR;
				lastCellC = startCellC;
			}
		}
	}
	if (lastCellR != -1)
	{
		bestCell[0] = lastCellR;
		bestCell[1] = lastCellC;
		return (1);
	}
	return (0);
}

//----------------------------------------------------------------------------

int32_t
Mover::calcLineOfSightView(int32_t range)
{
	GameMap->clearCellDebugs(1);
	for (size_t r = cellPositionRow - range; r < cellPositionRow + range; r++)
		for (size_t c = cellPositionCol - range; c < cellPositionCol + range; c++)
		{
			if (GameMap->inBounds(r, c))
			{
				if (lineOfSight(r, c, false))
					GameMap->setCellDebug(r, c, 1, 1);
				else
				{
					GameMap->setCellDebug(r, c, 2, 1);
				}
			}
		}
	return (0);
}

//----------------------------------------------------------------------------

void Mover::setMoveType(int32_t type)
{
	moveType = type;
	//--------------------------------------------------------------------
	// For now, level = type. This can change in the future, so always set
	// it here!
	if (moveType == MOVETYPE_GROUND)
		moveLevel = 0;
	else if (moveType == MOVETYPE_AIR)
		moveLevel = 2;
	else
		STOP(("Mover.setMoveType: bad moveType %d", type));
}

//-----------------------------------------------------------------------------

// void Mover::precalcAttackZones (void) {
//}

//-----------------------------------------------------------------------------

int32_t
Mover::calcGlobalPath(
	GlobalPathStep* globalPath, GameObjectPtr obj, Stuff::Vector3D* location, bool useClosedAreas)
{
#if 1
	int32_t startArea = GlobalMoveMap[moveLevel]->calcArea(cellPositionRow, cellPositionCol);
	int32_t goalArea = -1;
	int32_t goalCell[2] = {-1, -1};
	if (obj)
	{
		goalArea = GlobalMoveMap[moveLevel]->calcArea(obj->cellPositionRow, obj->cellPositionCol);
		if (goalArea == -1)
		{
			Stuff::Vector3D goalPos = obj->getPosition();
			if (obj->isBuilding())
			{
				BuildingPtr building = (BuildingPtr)obj;
				int32_t goalRow = 0, goalCol = 0;
				bool foundGoal = building->calcAdjacentAreaCell(moveLevel, -1, goalRow, goalCol);
				if (foundGoal)
					land->cellToWorld(goalRow, goalCol, goalPos);
				else
				{
					Stuff::Vector3D objectPos = obj->getPosition();
					goalPos = calcOffsetMoveGoal(objectPos);
				}
			}
			else
			{
				Stuff::Vector3D objectPos = obj->getPosition();
				goalPos = calcOffsetMoveGoal(objectPos);
			}
			land->worldToCell(goalPos, goalCell[0], goalCell[1]);
			goalArea = GlobalMoveMap[moveLevel]->calcArea(goalCell[0], goalCell[1]);
		}
	}
	else if (location)
	{
		land->worldToCell(*location, goalCell[0], goalCell[1]);
		goalArea = GlobalMoveMap[moveLevel]->calcArea(goalCell[0], goalCell[1]);
	}
	if (goalArea == -1)
		return (0);
	bool startAreaOpen = (startArea >= 0) && GlobalMoveMap[moveLevel]->areas[startArea].open;
	int32_t numSteps = -1;
	if (startAreaOpen)
	{
		if (numFunctionalWeapons > 0)
			GlobalMoveMap[moveLevel]->useClosedAreas = useClosedAreas;
		GlobalMoveMap[moveLevel]->moverTeamID = getTeamId();
		numSteps = GlobalMoveMap[moveLevel]->calcPath(startArea, goalArea, globalPath,
			cellPositionRow, cellPositionCol, goalCell[0], goalCell[1]);
		GlobalMoveMap[moveLevel]->useClosedAreas = false;
	}
	return (numSteps);
#else
	return (0);
#endif
}

//-----------------------------------------------------------------------------

bool Mover::canMoveHere(Stuff::Vector3D worldPos)
{
	//-------------------------------------------
	// If I'm a coptor, terrain doesn't matter...
	if (moveLevel == 2)
		return (true);
	if (!GameMap->getPassable(worldPos))
		return (false);
	//---------------------------------------------------------------
	// If I'm a hovercraft and this cell is passable, I know I can...
	if (moveLevel == 1)
		return (true);
	//--------------------------------------------------------
	// Otherwise, I'm a ground mover and there may be water...
	return (land->getWater(worldPos) < 2);
}

//-----------------------------------------------------------------------------

#ifdef LAB_ONLY
extern int64_t MCTimeCalcGoal1Update;
extern int64_t MCTimeCalcGoal2Update;
extern int64_t MCTimeCalcGoal3Update;
extern int64_t MCTimeCalcGoal4Update;
extern int64_t MCTimeCalcGoal5Update;
extern int64_t MCTimeCalcGoal6Update;
#endif

int32_t
Mover::calcMoveGoal(GameObjectPtr target, Stuff::Vector3D moveCenter, float moveRadius,
	Stuff::Vector3D moveGoal, int32_t selectionindex, Stuff::Vector3D& newGoal,
	int32_t numValidAreas, int16_t* validAreas, uint32_t moveparams)
{
	int64_t startTime = 0;
	if (goalMapRowStart[0] == -1)
	{
		for (size_t i = 0; i < GOALMAP_CELL_DIM; i++)
			goalMapRowStart[i] = i * GOALMAP_CELL_DIM;
		for (size_t r = 0; r < GOALMAP_CELL_DIM; r++)
			for (size_t c = 0; c < GOALMAP_CELL_DIM; c++)
			{
				int32_t index = r * GOALMAP_CELL_DIM + c;
				goalMapRowCol[index][0] = r;
				goalMapRowCol[index][1] = c;
			}
	}
	bool playerMove = ((moveparams & MOVEPARAM_PLAYER) != 0);
	bool movingToRepair = (pilot->getCurTacOrder()->code == TacticalOrderCode::refit);
	bool movingToCapture = (pilot->getCurTacOrder()->code == TacticalOrderCode::capture);
	bool avoidStationaryMovers = movingToRepair;
	bool noTravelOffMap = !GameMap->getOffMap(cellPositionRow, cellPositionCol);
	if (moveparams & MOVEPARAM_MYSTERY_PARAM)
	{
		newGoal = moveGoal;
		return (NO_ERROR);
	}
	if (moveparams & MOVEPARAM_STEP_TOWARD_TARGET)
	{
		//--------------------------------------------------------------------------
		// We want to pick a goal just a couple cells away from our position,
		// but in the direction of our target...
		Stuff::Vector3D aimVector;
		aimVector.Subtract(moveGoal, position);
		aimVector.z = 0;
		if (aimVector.GetLength() > 0.0)
		{
			aimVector.Normalize(aimVector);
			float stepLength = (Terrain::worldUnitsPerVertex * 1.5);
			aimVector.x *= stepLength;
			aimVector.y *= stepLength;
			aimVector.z = land->getTerrainElevation(aimVector);
			Stuff::Vector3D targetGoal;
			targetGoal.Add(position, aimVector);
			newGoal = calcOffsetMoveGoal(targetGoal);
		}
		return (NO_ERROR);
	}
	if (moveparams & MOVEPARAM_STEP_ADJACENT_TARGET)
	{
		//--------------------------------------------------------------------------
		// We want to pick a goal just a couple cells away from our position,
		// but in the direction of our target...
		newGoal = calcOffsetMoveGoal(moveGoal);
		return (NO_ERROR);
	}
	//----------------------------
	// The Goal Map is in CELLS...
	memset(goalMap, 0, sizeof(int32_t) * GOALMAP_CELL_DIM * GOALMAP_CELL_DIM);
	int32_t centerCell[2];
	land->worldToCell(moveCenter, centerCell[0], centerCell[1]);
	int32_t goalCell[2];
	land->worldToCell(moveGoal, goalCell[0], goalCell[1]);
	int32_t mapCellUL[2];
	mapCellUL[0] = centerCell[0] - GOALMAP_CELL_DIM / 2;
	mapCellUL[1] = centerCell[1] - GOALMAP_CELL_DIM / 2;
	// DEBUG
	//=================================
	Stuff::Vector3D start;
	land->cellToWorld(mapCellUL[0], mapCellUL[1], start);
	//==============================
	bool isAttackOrder = pilot->getCurTacOrder()->isCombatOrder();
	if (!target && !isAttackOrder)
	{
		newGoal = calcOffsetMoveGoal(moveGoal);
		return (NO_ERROR);
	}
	//--------
	// HACK!!!
	if (target && (pilot->getCurTacOrder()->code == TacticalOrderCode::guard))
		isAttackOrder = true;
	if (!target)
		moveGoal.z = land->getTerrainElevation(moveGoal);
	int32_t optimalCellRange = 2;
	if (movingToRepair)
		optimalCellRange = 1;
	if (isAttackOrder)
	{
		float metersPerCell = Terrain::worldUnitsPerCell * metersPerWorldUnit;
		float fireRangeInMeters = pilot->getSituationFireRange();
		int32_t fireCellRange = 0;
		if (fireRangeInMeters > 0.0)
		{
			fireCellRange = (int32_t)(pilot->getSituationFireRange() / metersPerCell);
			if (fireCellRange < 1)
				fireCellRange = 1;
			if (fireCellRange > (MAX_ATTACK_CELLRANGE - 1) /*(GOALMAP_CELL_DIM / 2)*/)
				fireCellRange = (MAX_ATTACK_CELLRANGE - 1) /*(GOALMAP_CELL_DIM / 2)*/;
		}
		else if (fireRangeInMeters == -1.0)
			fireCellRange = 2;
		int32_t minCellRange = (int32_t)(getMinFireRange() / metersPerCell);
		if (minCellRange > (MAX_ATTACK_CELLRANGE - 1) /*(GOALMAP_CELL_DIM / 2)*/)
			minCellRange = (MAX_ATTACK_CELLRANGE - 1) /*(GOALMAP_CELL_DIM / 2)*/;
		int32_t maxCellRange = (int32_t)(getMaxFireRange() / metersPerCell);
		if (maxCellRange > (MAX_ATTACK_CELLRANGE - 1) /*(GOALMAP_CELL_DIM / 2)*/)
			maxCellRange = (MAX_ATTACK_CELLRANGE - 1) /*(GOALMAP_CELL_DIM / 2)*/;
		//----------------------------------------
		// Anything beyond our max range is bad...
		int32_t fireRange = (int32_t)pilot->getCurTacOrder()->attackParams.range;
		if (/*!playerMove && */ (fireRange != FireRangeType::shortest) && (fireRange != FireRangeType::medium) && (fireRange != FireRangeType::extended))
		{
			int32_t firstIndex = rangedCellsIndices[maxCellRange + 1][0];
			int32_t lastIndex = rangedCellsIndices[MAX_ATTACK_CELLRANGE - 1][1];
			for (size_t i = firstIndex; i < lastIndex; i++)
			{
				int32_t r = goalCell[0] - mapCellUL[0] + rangedCells[i][0];
				int32_t c = goalCell[1] - mapCellUL[1] + rangedCells[i][1];
				if (inMapBounds(r, c, GOALMAP_CELL_DIM, GOALMAP_CELL_DIM))
					goalMap[goalMapRowStart[r] + c] -= 500;
			}
		}
		//---------------------------------------------------
		// If we have a max move radius (i.e. guarding area),
		// anything beyond our radius is bad...
		startTime = GetCycles();
		if (moveRadius > 0.0)
		{
			int32_t moveCellRange = moveRadius / metersPerCell;
			if (moveCellRange < 1)
				moveCellRange = 1;
			if (moveCellRange > (MAX_ATTACK_CELLRANGE - 1))
				moveCellRange = (MAX_ATTACK_CELLRANGE - 1);
			int32_t firstIndex = rangedCellsIndices[moveCellRange + 1][0];
			int32_t lastIndex = rangedCellsIndices[MAX_ATTACK_CELLRANGE - 1][1];
			for (size_t i = firstIndex; i < lastIndex; i++)
			{
				int32_t r = centerCell[0] - mapCellUL[0] + rangedCells[i][0];
				int32_t c = centerCell[1] - mapCellUL[1] + rangedCells[i][1];
				if (inMapBounds(r, c, GOALMAP_CELL_DIM, GOALMAP_CELL_DIM))
					goalMap[goalMapRowStart[r] + c] -= 5000;
			}
		}
#ifdef LAB_ONLY
		MCTimeCalcGoal2Update += (GetCycles() - startTime);
#endif
		//---------------------------------------------------------------------------
		// If the pilot has a set fire range, let's use it in determining how
		// far out we would consider attacking. If we're ramming, fireCellrange
		// will stay at 0 since firerange would then be 0.0...
		if (fireRangeInMeters > 0.0)
		{
			//---------------------------------------------------------------
			// Due to new minimum range rule, we do NOT want to be within our
			// min range...
			if (minCellRange > 0)
			{
				int32_t lastIndex = rangedCellsIndices[minCellRange][0];
				for (size_t i = 0; i < lastIndex; i++)
				{
					int32_t r = goalCell[0] - mapCellUL[0] + rangedCells[i][0];
					int32_t c = goalCell[1] - mapCellUL[1] + rangedCells[i][1];
					if (inMapBounds(r, c, GOALMAP_CELL_DIM, GOALMAP_CELL_DIM))
						goalMap[goalMapRowStart[r] + c] -= 500;
				}
			}
		}
		else if (fireRangeInMeters == -1.0)
			fireCellRange = 2;
		optimalCellRange = fireCellRange;
	}
	//-----------------------------------------------------------------------
	// Optimal Range (based upon current fire range of pilot, if possible)...
	if (moveparams & MOVEPARAM_RANDOM_OPTIMAL)
		for (size_t i = 0; i < numOptimalIncrements; i++)
		{
			int32_t r = goalCell[0] - mapCellUL[0] + optimalCells[optimalCellRange][i][0];
			int32_t c = goalCell[0] - mapCellUL[0] + optimalCells[optimalCellRange][i][1];
			if (inMapBounds(r, c, GOALMAP_CELL_DIM, GOALMAP_CELL_DIM))
				goalMap[goalMapRowStart[r] + c] += (500 + RandomNumber(40) * 5);
		}
	else
		for (size_t i = 0; i < numOptimalIncrements; i++)
		{
			int32_t r = goalCell[0] - mapCellUL[0] + optimalCells[optimalCellRange][i][0];
			int32_t c = goalCell[0] - mapCellUL[0] + optimalCells[optimalCellRange][i][1];
			if (inMapBounds(r, c, GOALMAP_CELL_DIM, GOALMAP_CELL_DIM))
				goalMap[goalMapRowStart[r] + c] += 500;
		}
	//--------------------------------------
	// Calc the closest map cell to start...
	int32_t startCell[2];
	land->worldToCell(position, startCell[0], startCell[1]);
	startCell[0] -= mapCellUL[0];
	startCell[1] -= mapCellUL[1];
	if (startCell[0] < 0)
		startCell[0] = 0;
	else if (startCell[0] >= GOALMAP_CELL_DIM)
		startCell[0] = GOALMAP_CELL_DIM - 1;
	if (startCell[1] < 0)
		startCell[1] = 0;
	else if (startCell[1] >= GOALMAP_CELL_DIM)
		startCell[1] = GOALMAP_CELL_DIM - 1;
	//---------------------------------------------------------------------------
	// NOTE: Do we even want the following, now? I think this was more a hack
	// than  anything. Take it out and see what effect it has... -gd 7/24/00
	//--------------------------------------------------------------
	// The closer we are to the goal, the better (within reason:)...
	if (!playerMove)
		for (size_t cellR = -1; cellR < 2; cellR++)
			for (size_t cellC = -1; cellC < 2; cellC++)
			{
				int32_t r = goalCell[0] - mapCellUL[0] + cellR;
				int32_t c = goalCell[1] - mapCellUL[1] + cellC;
				if (inMapBounds(r, c, GOALMAP_CELL_DIM, GOALMAP_CELL_DIM))
					goalMap[goalMapRowStart[r] + c] += 100;
			}
	//----------------------------------
	// If we must move somewhere else...
	if (moveparams & MOVEPARAM_SOMEWHERE_ELSE)
		goalMap[goalMapRowStart[startCell[0]] + startCell[1]] -= 10000;
	//-------------------------------------
	// Cells closer to the start cell get a
	// slight bonus...
	for (size_t r = 0; r < GOALMAP_CELL_DIM; r++)
		for (size_t c = 0; c < GOALMAP_CELL_DIM; c++)
		{
			int32_t dist = 0;
			if (r > startCell[0])
				dist += (r - startCell[0]);
			else
				dist += (startCell[0] - r);
			if (c > startCell[1])
				dist += (c - startCell[1]);
			else
				dist += (startCell[1] - c);
			goalMap[goalMapRowStart[r] + c] -= (dist * 1);
		}
	//----------------------------------------------------------------------
	// If we're attacking this target, let's use a selectionindex based upon
	// the number of people in my unit attacking this target...
	if (getGroup())
	{
		std::unique_ptr<Mover> mates[MAX_MOVERGROUP_COUNT];
		int32_t numMates = getGroup()->getMovers(mates);
		for (size_t i = 0; i < numMates; i++)
		{
			if (mates[i] == this)
				continue;
			std::unique_ptr<MechWarrior> pilot = mates[i]->getPilot();
			Stuff::Vector3D goal;
			goal.x = goal.y = goal.z = 0.0f;
			if (pilot && pilot->getMoveGlobalGoal(goal))
			{
				int32_t worldCell[2];
				land->worldToCell(goal, worldCell[0], worldCell[1]);
				worldCell[0] -= mapCellUL[0];
				worldCell[1] -= mapCellUL[1];
				if (inMapBounds(worldCell[0], worldCell[1], GOALMAP_CELL_DIM, GOALMAP_CELL_DIM))
					goalMap[goalMapRowStart[worldCell[0]] + worldCell[1]] -= 100;
			}
		}
	}
	//-----------------------
	// Init the area lists...
	wchar_t validAreaTable[MAX_GLOBALMAP_AREAS];
	for (size_t i = 0; i < GlobalMoveMap[moveLevel]->numAreas; i++)
		validAreaTable[i] = -1;
	int32_t curArea = GlobalMoveMap[moveLevel]->calcArea(cellPositionRow, cellPositionCol);
	int32_t goalArea = GlobalMoveMap[moveLevel]->calcArea(goalCell[0], goalCell[1]);
	if (numValidAreas > 0)
	{
		for (size_t i = 0; i < numValidAreas; i++)
		{
			if (validAreas[i] == goalArea)
			{
				numValidAreas = i + 1;
				break;
			}
		}
	}
	for (i = 0; i < numValidAreas; i++)
		validAreaTable[validAreas[i]] = 1;
	startTime = GetCycles();
	int32_t deepWaterWeight = ((moveLevel == 1) ? 0 : 999999);
	//-----------------------------------------
	// Finally, lay down the terrain weights...
	for (r = 0; r < GOALMAP_CELL_DIM; r++)
		for (size_t c = 0; c < GOALMAP_CELL_DIM; c++)
		{
			int32_t curCellRow = mapCellUL[0] + r;
			int32_t curCellCol = mapCellUL[1] + c;
			if (GameMap->inBounds(curCellRow, curCellCol))
			{
				//------------------------------------------------------------
				// In the int32_t run, we should simply have a look-up table
				// that contains a weight for each terrain\tile type (including
				// transitions)...
				MapCellPtr mapCell = GameMap->getCell(curCellRow, curCellCol);
				//-----------------------
				// Tile (terrain) type...
				// int32_t tileType = curTile.getTileType();
				int32_t goalMapIndex = goalMapRowStart[r] + c;
				if (!mapCell->getPassable())
					goalMap[goalMapIndex] -= 10000;
				if (mapCell->getOffMap())
					goalMap[goalMapIndex] -= 10000;
				if (mapCell->getDeepWater())
					goalMap[goalMapIndex] -= deepWaterWeight;
				bool moverHere = mapCell->getMover();
				if (avoidStationaryMovers && moverHere)
					goalMap[goalMapIndex] -= 1000;
				if (noTravelOffMap && GameMap->getOffMap(curCellRow, curCellCol))
					goalMap[goalMapIndex] -= 50000;
				int32_t area = GlobalMoveMap[moveLevel]->calcArea(curCellRow, curCellCol);
				if ((area != curArea) && (area != -1) && (curArea != -1))
				{
					if (moveLevel < 2)
					{
						// if (GlobalMoveMap[moveLevel]->areas[area].type ==
						// AREA_TYPE_NORMAL) {
						if (GlobalMoveMap[moveLevel]->getPathExists(curArea, area) == GLOBALPATH_EXISTS_UNKNOWN)
						{
							if (CalcValidAreaTable)
							{
								GlobalPathStep globalPath[128];
								int32_t numSteps =
									GlobalMoveMap[moveLevel]->calcPath(curArea, area, globalPath);
								if (numSteps > 0)
								{
									for (size_t j = 0; j < numSteps; j++)
										for (size_t k = 0; k < numSteps; k++)
											GlobalMoveMap[moveLevel]->setPathExists(
												globalPath[j].thruArea, globalPath[k].thruArea,
												GLOBALPATH_EXISTS_TRUE);
								}
								else
								{
									GlobalMoveMap[moveLevel]->setPathExists(
										area, curArea, GLOBALPATH_EXISTS_FALSE);
									GlobalMoveMap[moveLevel]->setPathExists(
										curArea, area, GLOBALPATH_EXISTS_FALSE);
								}
							}
							else
								GlobalMoveMap[moveLevel]->setPathExists(
									curArea, area, GLOBALPATH_EXISTS_FALSE);
						}
						if (GlobalMoveMap[moveLevel]->getPathExists(curArea, area) == GLOBALPATH_EXISTS_FALSE)
							goalMap[goalMapRowStart[r] + c] -= 50000;
						//}
					}
				}
			}
			else
				goalMap[goalMapRowStart[r] + c] = -999999;
		}
#ifdef LAB_ONLY
	MCTimeCalcGoal1Update += (GetCycles() - startTime);
#endif
	startTime = GetCycles();
	int32_t goalList[MAX_MOVE_GOALS][2];
	//------------------
	// Setup the list...
	for (i = 0; i < MAX_MOVE_GOALS; i++)
	{
		goalList[i][0] = -1;
		goalList[i][1] = -99999;
	}
	for (i = 0; i < MAX_MOVE_GOALS; i++)
	{
		//-----------------------------------------
		// This assumes that MAX_MOVE_GOALS >= 2...
		int32_t weight = goalMap[i];
		for (size_t j = MAX_MOVE_GOALS - 2; j > -1; j--)
			if (weight < goalList[j][1])
				break;
		if (j < (MAX_MOVE_GOALS - 2))
			memmove(&goalList[j + 2][0], &goalList[j + 1][0],
				(MAX_MOVE_GOALS - 2 - j) * sizeof(int32_t) * 2);
		goalList[j + 1][0] = i;
		goalList[j + 1][1] = weight;
	}
	for (i = MAX_MOVE_GOALS; i < (GOALMAP_CELL_DIM * GOALMAP_CELL_DIM); i++)
	{
		//------------------------------------------------------------
		// This routine assumes wt > goaList[MAX_MOVE_GOALS - 1][2]...
		// AND that MAX_MOVE_GOALS >= 2...
		int32_t weight = goalMap[i];
		if (weight > goalList[MAX_MOVE_GOALS - 1][1])
		{
			for (size_t j = MAX_MOVE_GOALS - 2; j > -1; j--)
				if (weight < goalList[j][1])
					break;
			if (j < (MAX_MOVE_GOALS - 2))
				memmove(&goalList[j + 2][0], &goalList[j + 1][0],
					(MAX_MOVE_GOALS - 2 - j) * sizeof(int32_t) * 2);
			goalList[j + 1][0] = i;
			goalList[j + 1][1] = weight;
		}
	}
#ifdef LAB_ONLY
	MCTimeCalcGoal3Update += (GetCycles() - startTime);
#endif
	//----------------------------------------------------------------------
	// If we're attacking this target, let's use a selectionindex based upon
	// the number of people in my unit attacking this target...
	//#ifdef USE_GROUPS
	if ((getObjectClass() == ELEMENTAL) && target && getGroup())
	{
		std::unique_ptr<Mover> mates[MAX_MOVERGROUP_COUNT];
		int32_t numMates = getGroup()->getMovers(mates);
		selectionindex = 0;
		for (size_t i = 0; i < numMates; i++)
		{
			if (mates[i] == this)
				break;
			std::unique_ptr<MechWarrior> pilot = mates[i]->getPilot();
			if (pilot)
			{
				if (pilot->getCurrentTarget() == target)
					selectionindex++;
			}
		}
	}
	//#endif
	//---------------------------------
	// so they don't select wacky goals
	selectionindex = -1;
	//------------------------------
	// Now, pick the goal we want...
	int32_t curGoalIndex = 0;
	if ((selectionindex > 0) && (selectionindex < MAX_MOVE_GOALS))
		curGoalIndex = selectionindex;
	int32_t curGoalCell[2];
	bool noLOF = true;
	curGoalCell[0] = mapCellUL[0] + goalMapRowCol[goalList[0][0]][0];
	curGoalCell[1] = mapCellUL[1] + goalMapRowCol[goalList[0][0]][1];
	// Stuff::Vector3D goalPos = moveGoal;
	// goalPos.z += target->getAppearRadius();		//Lets not look along the
	// ground, shall we.  Target probably has some altitude
	if (hasWeaponNode())
	{
		Stuff::Vector3D curMoverPosition;
		curMoverPosition = position;
		int32_t curMoverRow = cellPositionRow;
		int32_t curMoverCol = cellPositionCol;
		startTime = GetCycles();
		int32_t i = 0;
		ObjectManager->useMoverLineOfSightTable = false;
		while (noLOF && (i < MaxMoveGoalChecks))
		{
			int32_t index = goalList[i][0];
			curGoalCell[0] = mapCellUL[0] + goalMapRowCol[index][0];
			curGoalCell[1] = mapCellUL[1] + goalMapRowCol[index][1];
			i++;
			if (goalList[i][1] > -900000)
			{
				Stuff::Vector3D start;
				land->cellToWorld(curGoalCell[0], curGoalCell[1], start);
				// start.x = (float)(mapCellUL[1] + curGoalCell[1]) *
				// Terrain::worldUnitsPerCell + Terrain::worldUnitsPerCell / 2 -
				// Terrain::worldUnitsMapSide / 2;  start.y =
				// (Terrain::worldUnitsMapSide / 2) - ((float)(mapCellUL[0] +
				// curGoalCell[0]) * Terrain::worldUnitsPerCell) -
				// Terrain::worldUnitsPerCell / 2;
				start.z =
					land->getTerrainElevation(start) /* + 25.0f*/; // For that matter, so do we!
				position = start;
				cellPositionRow = curGoalCell[0];
				cellPositionCol = curGoalCell[1];
				int64_t lstartTime = GetCycles();
				if (goalList[i][1] > -10000)
				{
					if (movingToCapture)
						noLOF = false;
					else if (target)
						noLOF = !lineOfSight(target, 0.0f, false);
					else
						noLOF = !lineOfSight(moveGoal, false);
				}
#ifdef LAB_ONLY
				MCTimeCalcGoal4Update += (GetCycles() - lstartTime);
#endif
			}
		}
#ifdef LAB_ONLY
		MCTimeCalcGoal5Update += (GetCycles() - startTime);
#endif
		ObjectManager->useMoverLineOfSightTable = true;
		position = curMoverPosition;
		cellPositionRow = curMoverRow;
		cellPositionCol = curMoverCol;
	}
	GameMap->clearCellDebugs(2);
	startTime = GetCycles();
	if (noLOF && hasWeaponNode())
	{
		int32_t targetPos[2], maxPos[2], bestCell[4][2];
		float castRange;
		int32_t result[4];
		int32_t resultCost[4] = {0, 0, 0, 0};
		int32_t confidence[4];
		int32_t bestBest = 0;
		if (moveRadius > 0.0)
		{
			targetPos[0] = centerCell[0];
			targetPos[1] = centerCell[1];
			maxPos[0] = goalCell[0];
			maxPos[1] = goalCell[1];
			castRange = moveRadius;
			result[0] = calcBestGoalFromTarget(targetPos, maxPos, 0.0, castRange, bestCell[0]);
			if (result[0])
				resultCost[0] = GlobalMoveMap[moveLevel]->getPathCost(
					GlobalMoveMap[moveLevel]->calcArea(cellPositionRow, cellPositionCol),
					GlobalMoveMap[moveLevel]->calcArea(bestCell[0][0], bestCell[0][1]), false,
					confidence[0], true);
			bestCell[1][0] = centerCell[0];
			bestCell[1][1] = centerCell[1];
			result[1] = 1;
			if (result[1])
				resultCost[1] = GlobalMoveMap[moveLevel]->getPathCost(
					GlobalMoveMap[moveLevel]->calcArea(cellPositionRow, cellPositionCol),
					GlobalMoveMap[moveLevel]->calcArea(bestCell[1][0], bestCell[1][1]), false,
					confidence[1], true);
			if (resultCost[1] > 0)
				if (resultCost[1] < resultCost[bestBest])
					bestBest = 1;
			if (resultCost[bestBest] > 0)
			{
				curGoalCell[0] = bestCell[bestBest][0];
				curGoalCell[1] = bestCell[bestBest][1];
			}
			if (DebugGameObject[0] == this)
			{
				if (result[0])
					GameMap->setCellDebug(bestCell[0][0], bestCell[0][1], 3, 2);
				if (result[1])
					GameMap->setCellDebug(bestCell[1][0], bestCell[1][1], 3, 2);
			}
		}
		else
		{
			targetPos[0] = goalCell[0];
			targetPos[1] = goalCell[1];
			maxPos[0] = targetPos[0] - 10;
			maxPos[1] = targetPos[1];
			castRange = pilot->getSituationFireRange();
			int32_t bestBest = 0;
			result[0] = calcBestGoalFromTarget(targetPos, maxPos, 0.0, castRange, bestCell[0]);
			if (result[0])
				resultCost[0] = GlobalMoveMap[moveLevel]->getPathCost(
					GlobalMoveMap[moveLevel]->calcArea(cellPositionRow, cellPositionCol),
					GlobalMoveMap[moveLevel]->calcArea(bestCell[0][0], bestCell[0][1]), false,
					confidence[0], true);
			maxPos[0] = targetPos[0];
			maxPos[1] = targetPos[1] + 10;
			result[1] = calcBestGoalFromTarget(
				targetPos, maxPos, 0.0, pilot->getSituationFireRange(), bestCell[1]);
			if (result[1])
				resultCost[1] = GlobalMoveMap[moveLevel]->getPathCost(
					GlobalMoveMap[moveLevel]->calcArea(cellPositionRow, cellPositionCol),
					GlobalMoveMap[moveLevel]->calcArea(bestCell[1][0], bestCell[1][1]), false,
					confidence[1], true);
			if (resultCost[1] > 0)
				if (resultCost[1] < resultCost[bestBest])
					bestBest = 1;
			maxPos[0] = targetPos[0] + 10;
			maxPos[1] = targetPos[1];
			result[2] = calcBestGoalFromTarget(
				targetPos, maxPos, 0.0, pilot->getSituationFireRange(), bestCell[2]);
			if (result[2])
				resultCost[2] = GlobalMoveMap[moveLevel]->getPathCost(
					GlobalMoveMap[moveLevel]->calcArea(cellPositionRow, cellPositionCol),
					GlobalMoveMap[moveLevel]->calcArea(bestCell[2][0], bestCell[2][1]), false,
					confidence[2], true);
			if (resultCost[2] > 0)
				if (resultCost[2] < resultCost[bestBest])
					bestBest = 2;
			maxPos[0] = targetPos[0];
			maxPos[1] = targetPos[1] - 10;
			result[3] = calcBestGoalFromTarget(
				targetPos, maxPos, 0.0, pilot->getSituationFireRange(), bestCell[3]);
			if (result[3])
				resultCost[3] = GlobalMoveMap[moveLevel]->getPathCost(
					GlobalMoveMap[moveLevel]->calcArea(cellPositionRow, cellPositionCol),
					GlobalMoveMap[moveLevel]->calcArea(bestCell[3][0], bestCell[3][1]), false,
					confidence[3], true);
			if (resultCost[3] > 0)
				if (resultCost[3] < resultCost[bestBest])
					bestBest = 3;
			if (resultCost[bestBest] > 0)
			{
				curGoalCell[0] = bestCell[bestBest][0];
				curGoalCell[1] = bestCell[bestBest][1];
			}
			if (DebugGameObject[0] == this)
			{
				if (result[0])
					GameMap->setCellDebug(bestCell[0][0], bestCell[0][1], 3, 2);
				if (result[1])
					GameMap->setCellDebug(bestCell[1][0], bestCell[1][1], 3, 2);
				if (result[2])
					GameMap->setCellDebug(bestCell[2][0], bestCell[2][1], 3, 2);
				if (result[3])
					GameMap->setCellDebug(bestCell[3][0], bestCell[3][1], 3, 2);
			}
		}
	}
	GameMap->setCellDebug(curGoalCell[0], curGoalCell[1], 3, 2);
#ifdef LAB_ONLY
	MCTimeCalcGoal6Update += (GetCycles() - startTime);
#endif
	//--------------------------------------------
	// Let's calc the woorld coord of this cell...
	land->cellToWorld(curGoalCell[0], curGoalCell[1], newGoal);
	newGoal = calcOffsetMoveGoal(newGoal);
	// float distToGoal = distance_from(newGoal, goal) * metersPerWorldUnit;
	return (NO_ERROR);
}

//---------------------------------------------------------------------------

int32_t
Mover::calcMovePath(MovePathPtr path, int32_t pathType, Stuff::Vector3D start,
	Stuff::Vector3D goal, int32_t* goalCell, uint32_t moveparams)
{
	//-------------------------------------------------------------------------
	// This assumes the goal is already the "optimum" goal (it should have been
	// passed thru "calcMoveGoal" before being sent here)...
	if (!PathFindMap[SECTOR_PATHMAP] || !PathFindMap[SIMPLE_PATHMAP])
		Fatal(0, " No PathFindMap in Mover::calcMovePath ");
	int32_t posCellR, posCellC;
	land->worldToCell(start, posCellR, posCellC);
	int32_t goalCellR, goalCellC;
	land->worldToCell(goal, goalCellR, goalCellC);
	path->clear();
	int32_t result = 0;
	if (pathType == MOVEPATH_SIMPLE)
	{
		int32_t mapULr = posCellR - SimpleMovePathRange;
		if (mapULr < 0)
			mapULr = 0;
		int32_t mapULc = posCellC - SimpleMovePathRange;
		if (mapULc < 0)
			mapULc = 0;
		float cellLength = (Terrain::worldUnitsPerCell * metersPerWorldUnit);
		int32_t clearCost = 0;
		if (maxMoveSpeed != 0.0)
			clearCost = (float2short)(cellLength / maxMoveSpeed * 50.0);
		if (clearCost > 0)
		{
			int32_t jumpCost = 0;
			int32_t numOffsets = 8;
			if (!pilot->onHomeTeam() && !MPlayer)
				getJumpRange(&numOffsets, &jumpCost);
#ifdef USE_ELEMENTALS
			if (getObjectClass() == ELEMENTAL)
			{
				GameObjectPtr target = pilot->getLastTarget();
				if (target && (distanceFrom(target->getPosition()) < ElementalTargetNoJumpDistance))
				{
					jumpCost = 0;
					numOffsets = 8;
				}
				else
					JumpOnBlocked = true;
			}
#endif
			if (isMineSweeper())
				moveparams |= MOVEPARAM_SWEEP_MINES;
			if (followRoads)
				moveparams |= MOVEPARAM_FOLLOW_ROADS;
			if (isMech())
				moveparams |= MOVEPARAM_WATER_SHALLOW;
			if (moveLevel == 1)
				moveparams |= (MOVEPARAM_WATER_SHALLOW + MOVEPARAM_WATER_DEEP);
			PathFindMap[SIMPLE_PATHMAP]->setMover(getWatchID(), getTeamId(), isLayingMines());
			PathFindMap[SIMPLE_PATHMAP]->setUp(mapULr, mapULc, SimpleMovePathRange * 2 + 1,
				SimpleMovePathRange * 2 + 1, moveLevel, &start, posCellR, posCellC, goal,
				goalCellR - mapULr, goalCellC - mapULc, clearCost, jumpCost, numOffsets,
				moveparams);
			//---------------------
			// Set up debug info...
			DebugMovePathType = pathType;
			int32_t goalCell[2];
			if (numOffsets > 8)
				result = PathFindMap[SIMPLE_PATHMAP]->calcPathJUMP(path, nullptr, goalCell);
			else
				result = PathFindMap[SIMPLE_PATHMAP]->calcPath(path, nullptr, goalCell);
			PathFindMap[SIMPLE_PATHMAP]->setMover(0);
			JumpOnBlocked = false;
		}
	}
	else
	{
		//--------------------------------------------------------------------------------------
		// We are now assuming all local pathfinding will fit within two
		// adjacent sectors. So, we can assume both the start and goal locations
		// are within the two adj sectors...
		float cellLength = (Terrain::worldUnitsPerCell * metersPerWorldUnit);
		int32_t clearCost = 0;
		if (maxMoveSpeed != 0.0)
			clearCost = (int32_t)(cellLength / maxMoveSpeed * 50.0);
		if (clearCost > 0)
		{
			int32_t jumpCost = 0;
			int32_t numOffsets = 8;
			if (!pilot->onHomeTeam() && !MPlayer)
				getJumpRange(&numOffsets, &jumpCost);
			int32_t startSector[2];
			startSector[0] = posCellR / SECTOR_DIM;
			startSector[1] = posCellC / SECTOR_DIM;
			int32_t goalSector[2];
			goalSector[0] = goalCellR / SECTOR_DIM;
			goalSector[1] = goalCellC / SECTOR_DIM;
			int32_t sectorULr = startSector[0] * SECTOR_DIM;
			if (startSector[0] > goalSector[0])
				sectorULr = goalSector[0] * SECTOR_DIM;
			int32_t sectorULc = startSector[1] * SECTOR_DIM;
			if (startSector[1] > goalSector[1])
				sectorULc = goalSector[1] * SECTOR_DIM;
#ifdef USE_ELEMENTALS
			if (getObjectClass() == ELEMENTAL)
			{
				GameObjectPtr target = pilot->getLastTarget();
				if (target && (distanceFrom(target->getPosition()) < ElementalTargetNoJumpDistance))
				{
					jumpCost = 0;
					numOffsets = 8;
				}
				else
					JumpOnBlocked = true;
			}
#endif
			if (isMineSweeper())
				moveparams |= MOVEPARAM_SWEEP_MINES;
			if (followRoads)
				moveparams |= MOVEPARAM_FOLLOW_ROADS;
			if (isMech())
				moveparams |= MOVEPARAM_WATER_SHALLOW;
			if (moveLevel == 1)
				moveparams |= (MOVEPARAM_WATER_SHALLOW + MOVEPARAM_WATER_DEEP);
			if (moveparams & MOVEPARAM_JUMP)
				moveparams |= 0;
			PathFindMap[SECTOR_PATHMAP]->setMover(getWatchID(), getTeamId(), isLayingMines());
			PathFindMap[SECTOR_PATHMAP]->setUp(sectorULr, sectorULc, SECTOR_DIM * 2, SECTOR_DIM * 2,
				moveLevel, &start, posCellR, posCellC, goal, goalCellR - sectorULr,
				goalCellC - sectorULc, clearCost, jumpCost, numOffsets, moveparams);
			//---------------------
			// Set up debug info...
			DebugMovePathType = pathType;
			if (numOffsets > 8)
				result = PathFindMap[SECTOR_PATHMAP]->calcPathJUMP(path, nullptr, goalCell);
			else
				result = PathFindMap[SECTOR_PATHMAP]->calcPath(path, nullptr, goalCell);
			PathFindMap[SECTOR_PATHMAP]->setMover(0);
			JumpOnBlocked = false;
		}
	}
#if 0
	File* pathDebugFile = new File;
	pathDebugFile->create("movemap1.dbg");
	PathFindMap->writeDebug(pathDebugFile);
	pathDebugFile->close();
	delete pathDebugFile;
	pathDebugFile = nullptr;
#endif
	return (result);
}

//---------------------------------------------------------------------------

int32_t
Mover::calcEscapePath(MovePathPtr path, Stuff::Vector3D start, Stuff::Vector3D goal,
	int32_t* goalCell, uint32_t moveparams, Stuff::Vector3D& escapeGoal)
{
	//------------------------------------------
	// If nothing else, clear the escape goal...
	escapeGoal.x = -999999.0;
	escapeGoal.y = -999999.0;
	escapeGoal.z = -999999.0;
	if (!PathFindMap[SECTOR_PATHMAP] || !PathFindMap[SIMPLE_PATHMAP])
		Fatal(0, " No PathFindMap in Mover::calcMovePath ");
	int32_t posCellR, posCellC;
	land->worldToCell(start, posCellR, posCellC);
	int32_t goalCellR, goalCellC;
	land->worldToCell(goal, goalCellR, goalCellC);
	path->clear();
	int32_t result = 0;
	int32_t mapULr = posCellR - SimpleMovePathRange;
	if (mapULr < 0)
		mapULr = 0;
	int32_t mapULc = posCellC - SimpleMovePathRange;
	if (mapULc < 0)
		mapULc = 0;
	float cellLength = (Terrain::worldUnitsPerCell * metersPerWorldUnit);
	int32_t clearCost = 0;
	if (maxMoveSpeed != 0.0)
		clearCost = (float2short)(cellLength / maxMoveSpeed * 50.0);
	if (clearCost > 0)
	{
		int32_t jumpCost = 0;
		int32_t numOffsets = 8;
		if (!pilot->onHomeTeam() && !MPlayer)
			getJumpRange(&numOffsets, &jumpCost);
#ifdef USE_ELEMENTALS
		if (getObjectClass() == ELEMENTAL)
		{
			GameObjectPtr target = pilot->getLastTarget();
			if (target && (distanceFrom(target->getPosition()) < ElementalTargetNoJumpDistance))
			{
				jumpCost = 0;
				numOffsets = 8;
			}
			else
				JumpOnBlocked = true;
		}
#endif
		if (isMineSweeper())
			moveparams |= MOVEPARAM_SWEEP_MINES;
		if (followRoads)
			moveparams |= MOVEPARAM_FOLLOW_ROADS;
		FindingEscapePath = true;
		PathFindMap[SIMPLE_PATHMAP]->setMover(getWatchID(), getTeamId(), isLayingMines());
		PathFindMap[SIMPLE_PATHMAP]->setUp(mapULr, mapULc, SimpleMovePathRange * 2 + 1,
			SimpleMovePathRange * 2 + 1, moveLevel, &start, posCellR, posCellC, goal,
			goalCellR - mapULr, goalCellC - mapULc, clearCost, jumpCost, numOffsets, moveparams);
		//---------------------
		// Set up debug info...
		DebugMovePathType = 0;
		int32_t goalCell[2];
		result = PathFindMap[SIMPLE_PATHMAP]->calcEscapePath(path, &escapeGoal, goalCell);
		JumpOnBlocked = false;
		FindingEscapePath = false;
	}
#if 0
	File* pathDebugFile = new File;
	pathDebugFile->create("movemap1.dbg");
	PathFindMap->writeDebug(pathDebugFile);
	pathDebugFile->close();
	delete pathDebugFile;
	pathDebugFile = nullptr;
#endif
	return (result);
}

//---------------------------------------------------------------------------

bool Mover::getAdjacentCellPathLocked(int32_t level, int32_t cellRow, int32_t cellCol, int32_t dir)
{
	static int32_t adjCellTable[8][2] = {
		{-1, 0}, {-1, 1}, {0, 1}, {1, 1}, {1, 0}, {1, -1}, {0, -1}, {-1, -1}};
	int32_t adjCellRow = cellRow + adjCellTable[dir][0];
	int32_t adjCellCol = cellCol + adjCellTable[dir][1];
	if (level < 0)
		return (false);
	if (level > (NUM_MOVE_LEVELS - 1))
		return (false);
	return (GameMap->getPathlock(level, adjCellRow, adjCellCol));
}

//---------------------------------------------------------------------------

bool Mover::getPathRangeLock(int32_t range, bool* reachedEnd)
{
	MovePathPtr path = pilot->getMovePath();
	if (path)
		return (path->isLocked((moveLevel == 2), -1, range, reachedEnd));
	return (false);
}

//---------------------------------------------------------------------------

int32_t
Mover::setPathRangeLock(bool set, int32_t range)
{
	MovePathPtr path = pilot->getMovePath();
	int32_t lockLevel = (moveLevel == 2);
	if (set)
	{
		if (pathLockLength > 0)
			setPathRangeLock(false);
		if (path && (path->numSteps > 0))
		{
			int32_t start = path->curStep;
			int32_t lastStep = start + range;
			if (lastStep >= path->numStepsWhenNotPaused)
				lastStep = path->numStepsWhenNotPaused;
			pathLockLength = 0;
			for (size_t i = start; i < lastStep; i++)
			{
				if (GameMap->getPathlock(
						lockLevel, path->stepList[i].cell[0], path->stepList[i].cell[1]))
					return (-1);
				GameMap->setPathlock(
					(moveLevel == 2), path->stepList[i].cell[0], path->stepList[i].cell[1], true);
				pathLockList[pathLockLength][0] = path->stepList[i].cell[0];
				pathLockList[pathLockLength][1] = path->stepList[i].cell[1];
				pathLockLength++;
			}
		}
	}
	else
	{
		for (size_t i = 0; i < pathLockLength; i++)
			GameMap->setPathlock(lockLevel, pathLockList[i][0], pathLockList[i][1], false);
		pathLockLength = 0;
	}
	return (NO_ERROR);
}

//---------------------------------------------------------------------------

void Mover::updatePathLock(bool set)
{
	if (getObjectClass() == BATTLEMECH)
		if (((BattleMechPtr)this)->inJump)
			return;
	// For movers which can be stepped on.
	if (!pathLocks)
		return;
	//--------------------------------
	// First, set the cell we're in...
	GameMap->setPathlock((moveLevel == 2), cellPositionRow, cellPositionCol, set);
	//------------------------------------------------------------------------------------------
	// If we're yielding, we should NOT set our path range on. If we are
	// yielding, we can always remove what we've already pathlocked...
	if (!set || !pilot->isYielding())
		setPathRangeLock(set, crashBlockPath);
}

//---------------------------------------------------------------------------

bool Mover::getPathRangeBlocked(int32_t range, bool* reachedEnd)
{
	if (moveLevel > 0)
		return (false);
	MovePathPtr path = pilot->getMovePath();
	if (path)
		return (path->isBlocked(-1, range, reachedEnd));
	return (false);
}

//---------------------------------------------------------------------------

void Mover::updateHustleTime(void)
{
#if 0 // Redo when bridges come into play.
	int32_t overlay = GameMap->getOverlay(cellPositionRow, cellPositionCol);
	//---------------------------------------------------------------------------------
	// To avoid blocking movement on bridges, we'll keep track of a "hustle" time
	// for each mover. This will keep us from blocking bridges (and, in the future,
	// any other weird little predicaments) for our buddies. If we're on a bridge (or
	// we're recently on a bridge), our hustle flag will get set so we move it (even if
	// we're in a group move) and don't clog up the tight path...
	switch(overlay)
	{
		case OVERLAY_WATER_BRIDGE_NS:
		case OVERLAY_WATER_BRIDGE_NS_DESTROYED:
		case OVERLAY_WATER_BRIDGE_EW:
		case OVERLAY_WATER_BRIDGE_EW_DESTROYED:
		case OVERLAY_RAILROAD_WATER_BRIDGE_NS:
		case OVERLAY_RAILROAD_WATER_BRIDGE_NS_DESTROYED:
		case OVERLAY_RAILROAD_WATER_BRIDGE_EW:
		case OVERLAY_RAILROAD_WATER_BRIDGE_EW_DESTROYED:
			lastHustleTime = scenarioTime;
			break;
	}
#endif
}

//---------------------------------------------------------------------------

int32_t
Mover::bounceToAdjCell(void)
{
	//--------------------------------------------------------------------
	// Bounces mover to an adjacent open cell. Convenient for collisions:)
	for (size_t dir = 0; dir < 8; dir++)
	{
		static int32_t adjCellTable[8][2] = {
			{-1, 0}, {-1, 1}, {0, 1}, {1, 1}, {1, 0}, {1, -1}, {0, -1}, {-1, -1}};
		int32_t adjRow = cellPositionRow + adjCellTable[dir][0];
		int32_t adjCol = cellPositionCol + adjCellTable[dir][1];
		bool cellPathLocked = GameMap->getPathlock((moveLevel == 2), adjRow, adjCol);
		bool cellPassable = GameMap->getPassable(adjRow, adjCol);
		if (!cellPathLocked && cellPassable)
		{
			//----------------------------------------
			// This is open. Let's set our position...
			bool pathLockMarked =
				GameMap->getPathlock((moveLevel == 2), cellPositionRow, cellPositionCol);
			if (pathLockMarked)
				updatePathLock(false);
			Stuff::Vector3D newPosition;
			land->cellToWorld(adjRow, adjCol, newPosition);
			setPosition(newPosition);
			pilot->pausePath();
			if (pathLockMarked)
				updatePathLock(true);
			return (dir);
		}
	}
	return (-1);
}

//---------------------------------------------------------------------------

int32_t
Mover::calcMovePath(MovePathPtr path, Stuff::Vector3D start, int32_t thruArea[2],
	int32_t goalDoor, Stuff::Vector3D finalGoal, Stuff::Vector3D* goal, int32_t* goalCell,
	uint32_t moveparams)
{
	//-------------------------------------------------------------------------
	// This assumes the goal is already the "optimum" goal (it should have been
	// passed thru "calcMoveGoal" before being sent here)...
	if (!PathFindMap[SECTOR_PATHMAP] || !PathFindMap[SIMPLE_PATHMAP])
		Fatal(0, " No PathFindMap in Mover::calcMovePath ");
	//---------------------------------------------------------------------------------
	// We are now assuming all local pathfinding will fit within one sector. So,
	// we can assume both the start and goal locations are within the sector...
	int32_t result = 0;
	path->clear();
	float cellLength = (Terrain::worldUnitsPerCell * metersPerWorldUnit);
	int32_t clearCost = 0;
	if (maxMoveSpeed != 0)
		clearCost = (int32_t)(cellLength / maxMoveSpeed * 50.0);
	if (clearCost > 0)
	{
		int32_t jumpCost = 0;
		int32_t numOffsets = 8;
		if (!pilot->onHomeTeam() && !MPlayer)
			getJumpRange(&numOffsets, &jumpCost);
		int32_t posCellR, posCellC;
		land->worldToCell(start, posCellR, posCellC);
#ifdef USE_ELEMENTALS
		if (getObjectClass() == ELEMENTAL)
		{
			GameObjectPtr target = pilot->getLastTarget();
			if (target && (distanceFrom(target->getPosition()) < ElementalTargetNoJumpDistance))
			{
				jumpCost = 0;
				numOffsets = 8;
			}
			else
				JumpOnBlocked = true;
		}
#endif
		if (isMineSweeper())
			moveparams |= MOVEPARAM_SWEEP_MINES;
		if (followRoads)
			moveparams |= MOVEPARAM_FOLLOW_ROADS;
		if (isMech())
			moveparams |= MOVEPARAM_WATER_SHALLOW;
		if (moveLevel == 1)
			moveparams |= (MOVEPARAM_WATER_SHALLOW + MOVEPARAM_WATER_DEEP);
		PathFindMap[SECTOR_PATHMAP]->setMover(getWatchID(), getTeamId(), isLayingMines());
		result = PathFindMap[SECTOR_PATHMAP]->setUp(moveLevel, &start, posCellR, posCellC, thruArea,
			goalDoor, finalGoal, clearCost, jumpCost, numOffsets, moveparams);
		if (result == -1)
		{
			//-------------------------------------------------------
			// Goal door is blocked. Can't get thru, at the moment...
			JumpOnBlocked = false;
			return (-999);
		}
		if (numOffsets > 8)
			result = PathFindMap[SECTOR_PATHMAP]->calcPathJUMP(path, goal, goalCell);
		else
			result = PathFindMap[SECTOR_PATHMAP]->calcPath(path, goal, goalCell);
		// if ((goalCell[0] == -1) || (goalCell[1] == -1))
		//	STOP(("Mover.calcMovePath: bad goal cell--get GLENN!"));
		PathFindMap[SECTOR_PATHMAP]->setMover(0);
		JumpOnBlocked = false;
	}
#if 0
	File* pathDebugFile = new File;
	pathDebugFile->create("movemap1.dbg");
	PathFindMap->writeDebug(pathDebugFile);
	pathDebugFile->close();
	delete pathDebugFile;
	pathDebugFile = nullptr;
#endif
	return (result);
}

//---------------------------------------------------------------------------

int32_t
Mover::getContacts(int32_t* contactList, int32_t contactCriteria, int32_t sortType)
{
	if (sensorSystem)
		return (sensorSystem->getTeamContacts(contactList, contactCriteria, sortType));
	return (0);
}

//------------------------------------------------------------------------------------------

int32_t
Mover::getContactStatus(int32_t scanningTeamID, bool includingAllies)
{
	if (getFlag(OBJECT_FLAG_REMOVED))
		return (CONTACT_NONE);
	return (getContactInfo()->getContactStatus(scanningTeamID, true));
}

//------------------------------------------------------------------------------------------

float Mover::weaponLocked(int32_t weaponIndex, Stuff::Vector3D targetposition)
{
	int32_t bodyLocation = inventory[weaponIndex].bodyLocation;
	return (relFacingTo(targetposition, bodyLocation));
}

//------------------------------------------------------------------------------------------

bool Mover::weaponInRange(int32_t weaponIndex, float metersToTarget, float buffer)
{
	MasterComponentPtr weapon = &MasterComponent::masterList[inventory[weaponIndex].masterID];
	float minRange = WeaponRanges[weapon->getWeaponRange()][0] - buffer;
	float maxRange = WeaponRanges[weapon->getWeaponRange()][1] + buffer;
	if (metersToTarget < minRange)
		return (false);
	if (metersToTarget > maxRange)
		return (false);
	return (true);
}

//------------------------------------------------------------------------------------------

int32_t
Mover::getWeaponsReady(int32_t* list, int32_t listSize)
{
	//-----------------------------------------
	// Make sure list is >= the number of ready
	// weapons!
	int32_t numReady = 0;
	if (listSize == -1)
		for (size_t item = numOther; item < (numOther + numWeapons); item++)
		{
			if (isWeaponReady(item))
			{
				if (list)
					list[numReady++] = item;
				else
					numReady++;
			}
		}
	else
		for (size_t item = 0; item < listSize; item++)
		{
			if (isWeaponReady(list[item]))
			{
				if (list)
					list[numReady++] = list[item];
				else
					numReady++;
			}
		}
	return (numReady);
}

//------------------------------------------------------------------------------------------

int32_t
Mover::getWeaponsLocked(int32_t* list, int32_t listSize)
{
	//------------------------------------------
	// Make sure list is >= the number of locked
	// weapons!
	GameObjectPtr target = pilot->getCurrentTarget();
	if (!target)
		return (-2);
	Stuff::Vector3D targetposition = target->getPosition();
	int32_t numLocked = 0;
	float fireArc = getFireArc();
	if (listSize == -1)
		for (size_t item = numOther; item < (numOther + numWeapons); item++)
		{
			float relAngle = weaponLocked(item, targetposition);
			if ((relAngle >= -fireArc) && (relAngle <= fireArc))
				list[numLocked++] = item;
		}
	else
		for (size_t item = 0; item < listSize; item++)
		{
			float relAngle = weaponLocked(list[item], targetposition);
			if ((relAngle >= -fireArc) && (relAngle <= fireArc))
				list[numLocked++] = list[item];
		}
	return (numLocked);
}

//------------------------------------------------------------------------------------------

int32_t
Mover::getWeaponsInRange(int32_t* list, int32_t listSize, float fireRangeOrder)
{
	//------------------------------------------
	// Make sure list is >= the number of locked
	// weapons!
	GameObjectPtr target = pilot->getCurrentTarget();
	if (!target)
		return (-2);
	float rangeToTarget = distanceFrom(target->getPosition());
	//------------------------------------------------
	// For now, we always obey the fire range order...
#ifdef STUPID_RULES
	if (rangeToTarget > fireRangeOrder)
		return (-3);
#endif
	int32_t numInRange = 0;
	if (listSize == -1)
	{
		for (size_t item = numOther; item < (numOther + numWeapons); item++)
			if (weaponInRange(item, rangeToTarget, MapCellDiagonal))
				list[numInRange++] = item;
	}
	else
	{
		for (size_t item = 0; item < listSize; item++)
			if (weaponInRange(list[item], rangeToTarget, MapCellDiagonal))
				list[numInRange++] = list[item];
	}
	return (numInRange);
}

//------------------------------------------------------------------------------------------

int32_t
Mover::getWeaponShots(int32_t weaponIndex)
{
	if (!isWeaponIndex(weaponIndex))
		return (-1);
	//--------------------------------------------------------------------
	// All ammo, as listed in the Master Component Table, is a fixed index
	// greater than the weapon itself.
	if (MasterComponent::masterList[inventory[weaponIndex].masterID].getWeaponAmmoType())
		return (ammoTypeTotal[inventory[weaponIndex].ammoIndex].curAmount);
	//--------------------------------------------------------------------------
	// If no ammotype, then it has unlimited ammo (energy weapons, for example).
	return (UNLIMITED_SHOTS);
}

//------------------------------------------------------------------------------------------
bool Mover::getWeaponIndirectFire(int32_t weaponIndex)
{
	if (!isWeaponIndex(weaponIndex))
		return (false);
	//--------------------------------------------------------------------
	for (size_t i = 0; i < 20; i++)
	{
		if (IndirectFireWeapons[i] == inventory[weaponIndex].masterID)
			return true;
	}
	//--------------------------------------------------------------------------
	// If no ammotype, then it has unlimited ammo (energy weapons, for example).
	return (false);
}

//------------------------------------------------------------------------------------------
bool Mover::getWeaponAreaEffect(int32_t weaponIndex)
{
	if (!isWeaponIndex(weaponIndex))
		return (false);
	//--------------------------------------------------------------------
	for (size_t i = 0; i < 20; i++)
	{
		if (AreaEffectWeapons[i] == inventory[weaponIndex].masterID)
			return true;
	}
	//--------------------------------------------------------------------------
	// If no ammotype, then it has unlimited ammo (energy weapons, for example).
	return (false);
}

//------------------------------------------------------------------------------------------

float Mover::getWeaponAmmoLevel(int32_t weaponIndex)
{
	if (!isWeaponIndex(weaponIndex))
		return (-1.0);
	return ((float)(ammoTypeTotal[inventory[weaponIndex].ammoIndex].curAmount) / (float)(ammoTypeTotal[inventory[weaponIndex].ammoIndex].maxAmount));
}

bool Mover::getWeaponIsEnergy(int32_t weaponIndex)
{
	return MasterComponent::masterList[inventory[weaponIndex].masterID].getForm() == COMPONENT_FORM_WEAPON_ENERGY;
}

//------------------------------------------------------------------------------------------

void Mover::calcWeaponEffectiveness(bool setMax)
{
	int32_t effectiveness = 0;
	float avgSkill;
	//----------------------------------
	// Record time of our latest calc...
	lastWeaponEffectivenessCalc = scenarioTime;
	if (pilot)
		avgSkill = (float)pilot->getSkill(Skill::gunnery) / 50.0;
	else
		avgSkill = 1.0;
	// Ammo is NOT part of weapon effectiveness anymore
	// Per Mike Lee
	// -fs
	for (size_t curWeapon = numOther; curWeapon < (numOther + numWeapons); curWeapon++)
	{
		if (setMax || (!inventory[curWeapon].disabled))
			effectiveness += inventory[curWeapon].effectiveness * avgSkill;
	}
	if (setMax)
		maxWeaponEffectiveness = effectiveness;
	else
	{
		weaponEffectiveness = effectiveness;
		if (weaponEffectiveness == 0)
			playMessage(RADIO_WEAPONS_OUT);
	}
}

//---------------------------------------------------------------------------

void Mover::calcAmmoTotals(void)
{
	AmmoTally totalList[100];
	numAmmoTypes = 0;
	if (numWeapons > 0)
	{
		//-----------------------------------------------------------
		// We have some ammo in our inventory, so let's count 'em up.
		// First, make a list of all weapon types we have...
		for (size_t i = numOther; i < (numOther + numWeapons); i++)
		{
			int32_t weaponMasterId = inventory[i].masterID;
			bool newAmmoType = true;
			for (size_t curAmmo = 0; curAmmo < (int32_t)numAmmoTypes; curAmmo++)
			{
				if (totalList[curAmmo].masterId == (int32_t)MasterComponent::masterList[weaponMasterId].getWeaponAmmoMasterId())
				{
					newAmmoType = false;
					break;
				}
			}
			if (newAmmoType)
			{
				totalList[numAmmoTypes].masterId =
					MasterComponent::masterList[weaponMasterId].getWeaponAmmoMasterId();
				if (MasterComponent::masterList[weaponMasterId].getWeaponAmmoType())
				{
					totalList[numAmmoTypes].curAmount = 0;
					totalList[numAmmoTypes].maxAmount = 0;
				}
				else
				{
					totalList[numAmmoTypes].curAmount = UNLIMITED_SHOTS;
					totalList[numAmmoTypes].maxAmount = UNLIMITED_SHOTS;
				}
				numAmmoTypes++;
			}
		}
		//--------------------------------------------------
		// Now, go through all ammo we have and tally 'em...
		for (i = numOther + numWeapons; i < (numOther + numWeapons + numAmmos); i++)
		{
			int32_t ammoMasterId = inventory[i].masterID;
			bool foundAmmoWeapon = false;
			for (size_t curAmmo = 0; curAmmo < numAmmoTypes; curAmmo++)
			{
				if (totalList[curAmmo].masterId == ammoMasterId)
				{
					foundAmmoWeapon = true;
					if (useUnlimitedAmmo)
					{
						totalList[curAmmo].curAmount = UNLIMITED_SHOTS;
						totalList[curAmmo].maxAmount = UNLIMITED_SHOTS;
						break;
					}
					else
					{
						totalList[curAmmo].curAmount += inventory[i].amount;
						totalList[curAmmo].maxAmount += inventory[i].amount;
						break;
					}
				}
			}
			if (!foundAmmoWeapon)
				STOP(("Mech %s has ammo for a weapon it is not carrying!  Ammo "
					  "Index is %d",
					name, ammoMasterId));
		}
		//---------------------------------------------------------------------------
		// Since we now know the totals, create the actual total list for this
		// mover...
		memcpy(ammoTypeTotal, totalList, sizeof(AmmoTally) * numAmmoTypes);
	}
}

//------------------------------------------------------------------------------------------

int32_t
Mover::calcFireRanges(void)
{
	lastOptimalRangeCalc = scenarioTime;
	//---------------------------
	// Calc min and max ranges...
	maxRange = 0;
	minRange = 1000000.0;
	numFunctionalWeapons = 0;
	for (size_t curWeapon = numOther; curWeapon < (numOther + numWeapons); curWeapon++)
	{
		if (!inventory[curWeapon].disabled && (getWeaponShots(curWeapon) > 0))
		{
			int32_t range = (int32_t)MasterComponent::masterList[inventory[curWeapon].masterID]
								.getWeaponRange();
			float minWeaponRange = WeaponRanges[range][0];
			float maxWeaponRange = WeaponRanges[range][1];
			if (maxWeaponRange > maxRange)
				maxRange = maxWeaponRange;
			if (minWeaponRange < minRange)
				minRange = minWeaponRange;
			numFunctionalWeapons++;
		}
	}
	//---------------------------
	// Now, calc optimal range...
	float rangeTotals[NUM_WEAPON_RANGE_TYPES] = {0, 0, 0, 0, 0};
	for (curWeapon = numOther; curWeapon < (numOther + numWeapons); curWeapon++)
	{
		MasterComponent* weapon = &MasterComponent::masterList[inventory[curWeapon].masterID];
		if (!inventory[curWeapon].disabled && (getWeaponShots(curWeapon) > 0))
		{
			float damageTimeRating = weapon->getWeaponDamage() / weapon->getWeaponRecycleTime();
			//---------------------------------
			// Go thru the five range points...
			for (size_t i = 0; i < 5; i++)
				if (OptimalRangePointInRange[i][weapon->getWeaponRange()])
					rangeTotals[i] += damageTimeRating;
		}
	}
	float lastOptimalRange = optimalRange;
	if (numFunctionalWeapons == 0)
	{
		optimalRange = 0.0;
		return (optimalRange != lastOptimalRange);
	}
	else
	{
		sortList->clear();
		for (size_t i = 0; i < NUM_WEAPON_RANGE_TYPES; i++)
		{
			sortList->setId(i, i);
			sortList->setValue(i, rangeTotals[i]);
		}
		sortList->sort();
		if (sortList->getValue(0) <= 0.0)
		{
			optimalRange = 0.0;
			return (optimalRange != lastOptimalRange);
		}
	}
	int32_t optimalRangeType = sortList->getId(0);
	if (sortList->getValue(0) == sortList->getValue(1))
	{
		//--------------------------------------------------------------
		// A tie, so pick one based upon the pre-set range priorities...
		uint8_t tieFlags = 0;
		for (size_t i = 0; i < 5; i++)
			if (sortList->getValue(i) == sortList->getValue(0))
				tieFlags |= (1 << sortList->getId(i));
		optimalRangeType = OptimalRangeTieTable[tieFlags];
		if (optimalRangeType == 255)
		{
			optimalRange = 0.0;
			return (optimalRange != lastOptimalRange);
		}
	}
	optimalRange = OptimalRangePoints[optimalRangeType];
	return (optimalRange != lastOptimalRange);
}

//------------------------------------------------------------------------------------------

float Mover::getOrderedFireRange(int32_t* attackRange)
{
	FireRangeType fireRange = pilot->getCurTacOrder()->attackParams.range;
	if (attackRange)
		fireRange = *attackRange;
	float orderedFireRange = -1.0;
	switch (fireRange)
	{
	case FireRangeType::normal:
		orderedFireRange = getOptimalFireRange();
		break;
	case FireRangeType::ramming:
		orderedFireRange = 0.0;
		break;
	case FireRangeType::longest:
		orderedFireRange = getMaxFireRange();
		break;
	case FireRangeType::optimal:
		orderedFireRange = getOptimalFireRange();
		break;
	case FireRangeType::shortest:
		orderedFireRange =
			(WeaponRanges[WEAPON_RANGE_SHORT][1] - WeaponRanges[WEAPON_RANGE_SHORT][0]) / 2.0;
		break;
	case FireRangeType::medium:
		orderedFireRange =
			(WeaponRanges[WEAPON_RANGE_MEDIUM][1] - WeaponRanges[WEAPON_RANGE_MEDIUM][0]) / 2.0;
		break;
	case FireRangeType::extended:
		orderedFireRange =
			(WeaponRanges[WEAPON_RANGE_LONG][1] - WeaponRanges[WEAPON_RANGE_LONG][0]) / 2.0;
		break;
	}
	return (orderedFireRange);
}

//------------------------------------------------------------------------------------------

float Mover::getMinFireRange(void)
{
	return (minRange);
}

//------------------------------------------------------------------------------------------

float Mover::getMaxFireRange(void)
{
	return (maxRange);
}

//------------------------------------------------------------------------------------------

float Mover::getOptimalFireRange(void)
{
	return (optimalRange);
}

//------------------------------------------------------------------------------------------
// COMBAT routines
//------------------------------------------------------------------------------------------

bool Mover::isWeaponIndex(int32_t itemIndex)
{
	return ((itemIndex >= numOther) && (itemIndex < (numOther + numWeapons)));
}

//---------------------------------------------------------------------------

bool Mover::isWeaponMissile(int32_t weaponIndex)
{
	return (MasterComponent::masterList[inventory[weaponIndex].masterID].getForm() == COMPONENT_FORM_WEAPON_MISSILE);
}

//---------------------------------------------------------------------------

bool Mover::isWeaponReady(int32_t weaponIndex)
{
	if (inventory[weaponIndex].disabled)
		return (false);
	else if (inventory[weaponIndex].readyTime > scenarioTime)
		return (false);
	return (true);
}

//------------------------------------------------------------------------------------------

void Mover::startWeaponRecycle(int32_t weaponIndex)
{
	inventory[weaponIndex].readyTime = scenarioTime + MasterComponent::masterList[inventory[weaponIndex].masterID].getWeaponRecycleTime();
}

//------------------------------------------------------------------------------------------

int32_t
Mover::tallyAmmo(int32_t ammoMasterId)
{
	int32_t tally = 0;
	int32_t firstAmmo = numOther + numWeapons;
	for (size_t item = firstAmmo; item < firstAmmo + numAmmos; item++)
	{
		if (inventory[item].masterID == ammoMasterId)
			tally += inventory[item].amount;
	}
	return (tally);
}

//---------------------------------------------------------------------------

bool Mover::needsRefit(void)
{
	bool result = false;
	if (!refitBuddyWID && getObjectClass() == BATTLEMECH)
	{
		for (size_t loc = 0; loc < numArmorLocations; loc++)
		{
			if (loc < numBodyLocations && (loc == MECH_ARMOR_LOCATION_LARM || loc == MECH_ARMOR_LOCATION_RARM) && body[loc].damageState == IS_DAMAGE_DESTROYED)
				continue;
			if ((loc < numBodyLocations && body[loc].curInternalStructure < body[loc].maxInternalStructure) || (armor[loc].curArmor < armor[loc].maxArmor))
			{
				result = true;
				break;
			}
		}
		if (!result)
			for (size_t ammo = 0; ammo < numAmmoTypes; ammo++)
				if (ammoTypeTotal[ammo].curAmount < ammoTypeTotal[ammo].maxAmount)
				{
					result = true;
					break;
				}
	}
	return result;
}

//---------------------------------------------------------------------------

int32_t
Mover::reduceAmmo(int32_t ammoMasterId, int32_t amount)
{
	int32_t amountUsed = amount;
	//----------------------------------------------------------
	// Deduct it from the first available ammo critical space...
	int32_t firstAmmo = numOther + numWeapons;
	for (size_t item = firstAmmo; item < firstAmmo + numAmmos; item++)
	{
		if (inventory[item].masterID == ammoMasterId)
			if (inventory[item].amount > amount)
			{
				inventory[item].amount -= amount;
				break;
			}
			else
			{
				amount -= inventory[item].amount;
				inventory[item].amount = 0;
			}
	}
	//-------------------------------------------------------------------
	// Make sure the ammoTypeTotal list, in which we track total ammo for
	// all of the ammo types this mover uses, is adjusted...
	for (size_t ammo = 0; ammo < numAmmoTypes; ammo++)
		if (ammoTypeTotal[ammo].masterId == ammoMasterId)
		{
			ammoTypeTotal[ammo].curAmount -= amountUsed;
			if (ammoTypeTotal[ammo].curAmount <= 0)
			{
				//-----------------------------------------------------
				// Ammo for this weapon type is spent, which may effect
				// our weapon effectiveness and optimal range...
				ammoTypeTotal[ammo].curAmount = 0;
				calcWeaponEffectiveness(false);
				calcFireRanges();
				pilot->radioMessage(RADIO_AMMO_OUT);
			}
			break;
		}
	return (amountUsed);
}

//---------------------------------------------------------------------------
/*
int32_t Mover::refillAmmo (int32_t ammoMasterId, int32_t amount) {

	int32_t amountUsed = amount;

	//----------------------------------------------------------
	// Deduct it from the first available ammo critical space...
	int32_t firstAmmo = numOther + numWeapons;
	for (int32_t item = firstAmmo; item < firstAmmo + numAmmos; item++) {
		if (inventory[item].masterID == ammoMasterId)
			if (inventory[item].amount > amount) {
				inventory[item].amount -= amount;
				break;
				}
			else {
				amount -= inventory[item].amount;
				inventory[item].amount = 0;
			}
	}

	//-------------------------------------------------------------------
	// Make sure the ammoTypeTotal list, in which we track total ammo for
	// all of the ammo types this mover uses, is adjusted...
	for (int32_t ammo = 0; ammo < numAmmoTypes; ammo++)
		if (ammoTypeTotal[ammo].masterId == ammoMasterId) {
			ammoTypeTotal[ammo].curAmount -= amountUsed;
			if (ammoTypeTotal[ammo].curAmount <= 0) {
				//-----------------------------------------------------
				// Ammo for this weapon type is spent, which may effect
				// our weapon effectiveness and optimal range...
				ammoTypeTotal[ammo].curAmount = 0;
				calcLongestRangeWeapon();
				calcWeaponEffectiveness(false);
				calcOptimalRange(nullptr);
			}
			break;
		}

	return(amountUsed);
}
*/
//---------------------------------------------------------------------------

void Mover::deductWeaponShot(int32_t weaponIndex, int32_t ammoAmount)
{
	if (ammoAmount > 0)
		reduceAmmo(
			MasterComponent::masterList[inventory[weaponIndex].masterID].getWeaponAmmoMasterId(),
			ammoAmount);
}

//---------------------------------------------------------------------------

int32_t
Mover::sortWeapons(
	int32_t* weaponList, int32_t* valueList, int32_t listSize, int32_t sortType, bool skillCheck)
{
	//------------------------------------------
	// Make sure list is >= the number of locked
	// weapons!
	GameObjectPtr target = pilot->getCurrentTarget();
	if (!target)
		return (-2);
	int32_t aimLocation = -1;
	if (pilot && pilot->getCurTacOrder()->isCombatOrder())
		aimLocation = pilot->getCurTacOrder()->attackParams.aimLocation;
	//----------------------------------------------------------------------
	// BIG ASSUMPTION HERE: That a mech will not have more than 100 weapons.
	sortList->clear();
	if (listSize == -1)
	{
		for (size_t item = numOther; item < (numOther + numWeapons); item++)
		{
			sortList->setId(item - numOther, item);
			switch (sortType)
			{
			case WEAPONSORT_ATTACKCHANCE:
				sortList->setValue(item - numOther,
					calcAttackChance(target, aimLocation, scenarioTime, item, 0.0, nullptr));
				break;
			default:
				//-----------------
				// Bad Sort Type...
				NODEFAULT;
				return (-3);
			}
		}
		sortList->sort();
		for (item = 0; item < numWeapons; item++)
		{
			weaponList[item] = sortList->getId(item);
			valueList[item] = sortList->getValue(item);
		}
	}
	else
	{
		for (size_t item = 0; item < listSize; item++)
		{
			sortList->setId(item, weaponList[item]);
			float sortValue = 0.0;
			if (weaponList[item] == -1)
				sortValue = -999.0;
			else
				switch (sortType)
				{
				case WEAPONSORT_ATTACKCHANCE:
					sortValue = calcAttackChance(
						target, aimLocation, scenarioTime, weaponList[item], 0.0, nullptr);
					break;
				default:
					//-----------------
					// Bad Sort Type...
					NODEFAULT;
					return (-3);
				}
			sortList->setValue(item, sortValue);
		}
		sortList->sort();
		for (item = 0; item < listSize; item++)
		{
			weaponList[item] = sortList->getId(item);
			valueList[item] = sortList->getValue(item);
		}
	}
	return (NO_ERROR);
}

//---------------------------------------------------------------------------
bool Mover::hasNonAreaWeapon(void)
{
	for (size_t i = numOther; i < numOther + numWeapons; i++)
	{
		if (!getWeaponAreaEffect(i))
			return true;
	}
	return false;
}

#define MAX_SHORT_RANGE (60.0f * worldUnitsPerMeter)
#define MIN_MEDIUM_RANGE (31.0f * worldUnitsPerMeter)
#define MAX_MEDIUM_RANGE (120.0f * worldUnitsPerMeter)
#define MIN_LONG_RANGE (61.0f * worldUnitsPerMeter)
#define MAX_LONG_RANGE (180.0f * worldUnitsPerMeter)

extern float
applyDifficultySkill(float chance, bool isPlayer);
//---------------------------------------------------------------------------
float Mover::calcAttackChance(GameObjectPtr target, int32_t aimLocation, float targetTime,
	int32_t weaponIndex, float modifiers, int32_t* range, Stuff::Vector3D* targetpoint)
{
	if ((weaponIndex < numOther) || (weaponIndex >= numOther + numWeapons))
		return (-9999.0);
	//-------------------------------------------------------------
	// First, let's find out what kind of object we're targeting...
	Stuff::Vector3D targetposition;
	BattleMechPtr mech = nullptr;
	GroundVehiclePtr vehicle = nullptr;
	if (target)
	{
		int32_t targetObjectClass = target->getObjectClass();
		switch (targetObjectClass)
		{
		case BATTLEMECH:
			mech = (BattleMechPtr)target;
			break;
		case GROUNDVEHICLE:
			vehicle = (GroundVehiclePtr)target;
			break;
		}
		targetposition = target->getPosition();
	}
	else if (targetpoint)
		targetposition = *targetpoint;
	else
		return (-9999.0);
	float attackChance = pilot->getSkill(Skill::gunnery);
	if (!MPlayer && (getCommanderId() == Commander::home->getId())) // Up the gunnery Skill by the difficulty modifier
		attackChance = applyDifficultySkill(attackChance, TRUE);
	else if (!MPlayer && (getCommanderId() == Commander::home->getId())) // Up the gunnery
		// Skill by the
		// difficulty
		// modifier
		attackChance = applyDifficultySkill(attackChance, FALSE);
	//----------------------
	// General fire range...
	float distanceToTarget = distanceFrom(targetposition);
	if (range)
	{
		if (distanceToTarget <= WeaponRange[FireRangeType::shortest])
			*range = FireRangeType::shortest;
		else if (distanceToTarget <= WeaponRange[FireRangeType::medium])
			*range = FireRangeType::medium;
		else
			*range = FireRangeType::extended;
	}
	//----------------------
	// Range (Per Weapon)...
	float weaponMinRange =
		WeaponRanges[MasterComponent::masterList[inventory[weaponIndex].masterID].getWeaponRange()]
					[0];
	float weaponLongRange =
		WeaponRanges[MasterComponent::masterList[inventory[weaponIndex].masterID].getWeaponRange()]
					[1];
	if (distanceToTarget <= (weaponMinRange - MapCellDiagonal))
		return (-1.0);
	if (distanceToTarget > (weaponLongRange + MapCellDiagonal))
	{
		//----------------
		// Out of range...
		return (-1.0);
	}
	// Non-movers get 100% attack chance.  Look for modifications for turrets
	// soon!
	// Nothing is better then 100%, no need to modify.  Exit here!!
	// -fs 9/8/2000
	if (!target || (target && !target->isMover()) || (target && target->isMover() && target->isDisabled()))
	{
		attackChance = 100.0f;
		return attackChance;
	}
	// Mech specialist modifiers
	if (mech && mech->getPilot())
	{
		if ((mech->tonnage < 40) && mech->getPilot()->isLightMechSpecialist())
			attackChance -= 30.0f;
		else if ((mech->tonnage >= 40) && (mech->tonnage < 60) && mech->getPilot()->isMediumMechSpecialist())
			attackChance -= 20.0f;
		else if ((mech->tonnage >= 60) && (mech->tonnage < 80) && mech->getPilot()->isHevayMechSpecialist())
			attackChance -= 10.0f;
		else if ((mech->tonnage >= 80) && (mech->tonnage < 100) && mech->getPilot()->isAssaultMechSpecialist())
			attackChance -= 10.0f;
	}
	// Weapon Specialist Modifiers
	if (pilot && pilot->isLaserSpecialist() && ((inventory[weaponIndex].masterID == 139) || (inventory[weaponIndex].masterID == 140) || (inventory[weaponIndex].masterID == 147) || (inventory[weaponIndex].masterID == 156)))
	{
		attackChance += WeaponSpecialistModifier;
	}
	if (pilot && pilot->isPulseLaserSpecialist() && ((inventory[weaponIndex].masterID == 142) || (inventory[weaponIndex].masterID == 144) || (inventory[weaponIndex].masterID == 153) || (inventory[weaponIndex].masterID == 151)))
	{
		attackChance += WeaponSpecialistModifier;
	}
	if (pilot && pilot->isERLaserSpecialist() && ((inventory[weaponIndex].masterID == 141) || (inventory[weaponIndex].masterID == 143) || (inventory[weaponIndex].masterID == 150) || (inventory[weaponIndex].masterID == 152)))
	{
		attackChance += WeaponSpecialistModifier;
	}
	if (pilot && pilot->isLightACSpecialist() && ((inventory[weaponIndex].masterID == 100) || (inventory[weaponIndex].masterID == 103) || (inventory[weaponIndex].masterID == 110)))
	{
		attackChance += WeaponSpecialistModifier;
	}
	if (pilot && pilot->isMediumACSpecialist() && ((inventory[weaponIndex].masterID == 101) || (inventory[weaponIndex].masterID == 108) || (inventory[weaponIndex].masterID == 111)))
	{
		attackChance += WeaponSpecialistModifier;
	}
	if (pilot && pilot->isHeavyACSpecialist() && ((inventory[weaponIndex].masterID == 102) || (inventory[weaponIndex].masterID == 109) || (inventory[weaponIndex].masterID == 112)))
	{
		attackChance += WeaponSpecialistModifier;
	}
	if (pilot && pilot->isLRMSpecialist() && ((inventory[weaponIndex].masterID == 120) || (inventory[weaponIndex].masterID == 123)))
	{
		attackChance += WeaponSpecialistModifier;
	}
	if (pilot && pilot->isSRMSpecialist() && ((inventory[weaponIndex].masterID == 124) || (inventory[weaponIndex].masterID == 125)))
	{
		attackChance += WeaponSpecialistModifier;
	}
	if (pilot && pilot->isPPCSpecialist() && ((inventory[weaponIndex].masterID == 145) || (inventory[weaponIndex].masterID == 146) || (inventory[weaponIndex].masterID == 154)))
	{
		attackChance += WeaponSpecialistModifier;
	}
	if (pilot && pilot->isGaussSpecialist() && (inventory[weaponIndex].masterID == 99))
	{
		attackChance += WeaponSpecialistModifier;
	}
	if (pilot && pilot->isSmallArmsSpecialist() && ((inventory[weaponIndex].masterID == 114) || (inventory[weaponIndex].masterID == 155)))
	{
		attackChance += WeaponSpecialistModifier;
	}
	// Range Specialist Modifiers
	if ((distanceToTarget <= MAX_SHORT_RANGE) && pilot && pilot->isShortRangeSpecialist())
	{
		attackChance += 15.0f;
	}
	if ((distanceToTarget >= MIN_MEDIUM_RANGE) && (distanceToTarget <= MAX_MEDIUM_RANGE) && pilot && pilot->isMediumRangeSpecialist())
	{
		attackChance += 15.0f;
	}
	if ((distanceToTarget >= MIN_LONG_RANGE) && (distanceToTarget <= MAX_LONG_RANGE) && pilot && pilot->isLongRangeSpecialist())
	{
		attackChance += 15.0f;
	}
	// Aimed shots go here.
	// Aimed or called shots are percentage of the actual attack chance
	// Therefore, must go here after all modifiers...
	if ((aimLocation > -1) && (mech))
	{
		switch (aimLocation)
		{
		case MECH_BODY_LOCATION_HEAD:
			attackChance *= HeadShotModifier;
			break;
		case MECH_BODY_LOCATION_CTORSO:
		case MECH_BODY_LOCATION_LTORSO:
		case MECH_BODY_LOCATION_RTORSO:
			break;
		case MECH_BODY_LOCATION_LARM:
		case MECH_BODY_LOCATION_RARM:
			attackChance *= ArmShotModifier;
			break;
		case MECH_BODY_LOCATION_LLEG:
		case MECH_BODY_LOCATION_RLEG:
			attackChance *= LegShotModifier;
			break;
		}
		if (pilot && pilot->isSharpShooter())
			attackChance += 20.0f;
	}
	// No more modifiers EXCEPT what is in here.
	if (attackChance > 95.0f)
		attackChance = 95.0f;
	if (attackChance < 5.0f)
		attackChance = 5.0f;
	return (attackChance);
}

//---------------------------------------------------------------------------

/*
void Mover::buildAttackChanceTable (void) {

	for (size_t i = 0; i <
}
*/

//---------------------------------------------------------------------------

void Mover::ammoExplosion(int32_t ammoIndex)
{
	pilot->injure(2);
	Assert(ammoIndex < (numOther + numWeapons + numAmmos), ammoIndex, " Ammo Index out of range ");
	Assert(ammoIndex >= (numOther + numWeapons), ammoIndex, " Ammo Index too low ");
	int32_t ammoMasterId = inventory[ammoIndex].masterID;
	int32_t bodyLocation = inventory[ammoIndex].bodyLocation;
	int32_t remainingAmmo = inventory[ammoIndex].amount;
	float damage =
		float(remainingAmmo) * MasterComponent::masterList[ammoMasterId].getAmmoExplosiveDamage();
	inventory[ammoIndex].amount = 0;
	//-------------------------------------------------------------------
	// Make sure the ammoTypeTotal list, in which we track total ammo for
	// all of the ammo types this mover uses, is adjusted...
	if (inventory[ammoIndex].ammoIndex == -1)
		Fatal(-1, " Bad Ammo Index in Ammo Explosion ");
	Assert(inventory[ammoIndex].ammoIndex < numAmmoTypes, inventory[ammoIndex].ammoIndex,
		" Too Many Ammo Types ");
	Assert(inventory[ammoIndex].ammoIndex >= 0, inventory[ammoIndex].ammoIndex,
		" not enough Ammo Types ");
	ammoTypeTotal[inventory[ammoIndex].ammoIndex].curAmount -= remainingAmmo;
	//------------------------------------------------------------------------
	// We can use the bodyLocation as the hitLocation since the location codes
	// for all body locations are the same as the armor location codes.
	// MULTIPLAYER NOTE: Since this is called ONLY by Mech.cpp from
	// hitInventoryItem, there should be no need to call it from the server
	// since it's triggered by a weaponhit already.
	WeaponShotInfo shotInfo;
	//-------------------------------------------------------
	// This is big ass damage for an ammo explosion!!!!
	// Happens with the Hunchback right now.
	if (damage > 255.0f)
		damage = 255.0f;
	shotInfo.init(nullptr, inventory[ammoIndex].masterID, damage, bodyLocation, 0.0);
	if (MPlayer)
	{
		if (MPlayer->isServer())
			handleWeaponHit(&shotInfo, true);
	}
	else
		handleWeaponHit(&shotInfo);
}

//---------------------------------------------------------------------------
void Mover::disable(uint32_t cause)
{
	//-------------------------------------------------
	// Immediately begin shutting down, then disable...
	if (!isDisabled())
	{
		if (pilot)
			pilot->handleAlarm(PILOT_ALARM_VEHICLE_INCAPACITATED, cause);
		if (getObjectClass() == BATTLEMECH)
			if (MPlayer && MPlayer->isServer() && !((BattleMech*)this)->lost)
			{
				MPlayer->addKillLossChunk(MAX_MC_PLAYERS, getCommanderId());
				((BattleMech*)this)->lost = true;
			}
		setStatus(OBJECT_STATUS_DISABLED);
		disableThisFrame = true;
		if (Team::home->isFriendly(getTeam()))
			friendlydestroyed = true;
		else
		{
			enemydestroyed = true;
		}
		//-----------------------------
		// Immediately lose contacts...
		if (sensorSystem)
			sensorSystem->disable();
		causeOfDeath = cause;
		if (isMech() && (getMoveType() != MOVETYPE_AIR))
		{
			// We now want to play a BUNCH of critical Hit Explosions on the
			// mech
			// To indicate that we are going down.
			Stuff::Vector3D explosionLoc;
			explosionLoc = appearance->getNodeNamePosition("cockpit");
			ObjectManager->createExplosion(MECH_CRITICAL_HIT_ID, nullptr, explosionLoc, 0.0f, 0.0f);
			explosionLoc = appearance->getNodeNamePosition("hit_left");
			ObjectManager->createExplosion(MECH_CRITICAL_HIT_ID, nullptr, explosionLoc, 0.0f, 0.0f);
			explosionLoc = appearance->getNodeNamePosition("hit_right");
			ObjectManager->createExplosion(MECH_CRITICAL_HIT_ID, nullptr, explosionLoc, 0.0f, 0.0f);
			explosionLoc = appearance->getNodeNamePosition("weapon_righttorso");
			ObjectManager->createExplosion(MECH_CRITICAL_HIT_ID, nullptr, explosionLoc, 0.0f, 0.0f);
			explosionLoc = appearance->getNodeNamePosition("weapon_lefttorso");
			ObjectManager->createExplosion(MECH_CRITICAL_HIT_ID, nullptr, explosionLoc, 0.0f, 0.0f);
		}
	}
}

//---------------------------------------------------------------------------

void Mover::shutDown(void)
{
	if (isDisabled())
		return;
	if ((status == OBJECT_STATUS_SHUTDOWN) || (status == OBJECT_STATUS_SHUTTING_DOWN))
		return;
	//-----------------------------------
	// Immediately begin shutting down...
	setStatus(OBJECT_STATUS_SHUTTING_DOWN);
	shutDownThisFrame = true;
}

//---------------------------------------------------------------------------

void Mover::startUp(void)
{
	if (isDisabled())
		return;
	if ((status == OBJECT_STATUS_STARTING_UP) || (status == OBJECT_STATUS_NORMAL))
		return;
	//---------------------------------
	// Immediately begin starting up...
	setStatus(OBJECT_STATUS_STARTING_UP);
	startUpThisFrame = true;
}

//---------------------------------------------------------------------------

void Mover::tradeRefresh(void)
{
	timeLeft = 15.0;
	exploding = false;
	withdrawing = false;
	engineBlowTime = -1.0;
	ejectOrderGiven = false;
	//	if (statusChunk.ejectOrderGiven)
	//		PAUSE(("whoops"));
	statusChunk.ejectOrderGiven = false;
}

//---------------------------------------------------------------------------

bool Mover::isWithdrawing(void)
{
	return (pilot->getCurTacOrder()->code == TacticalOrderCode::withdraw);
}

//---------------------------------------------------------------------------

bool Mover::refit(float pointsAvailable, float& pointsUsed, bool ammoOnly)
{
	bool result = false;
	float refitPoints = pointsAvailable;
	if (refitPoints > 0.0)
	{
		float areaMax = refitAmount / 3.0; // size of pool for each section
			// (armor, internals, and ammo)
			// before depletions
		int32_t rearmCount = 0;
		int32_t repairCount = 0;
		// how many locations need armor or IS repaired?
		if (!ammoOnly)
		{
			for (size_t i = 0; i < numArmorLocations; i++)
			{
				//-------------------------------
				// don't repair destroyed arms...
				if (((i == MECH_BODY_LOCATION_LARM) || (i == MECH_BODY_LOCATION_RARM)) && (body[i].damageState == IS_DAMAGE_DESTROYED))
					continue;
				if ((i < numBodyLocations) && (body[i].curInternalStructure < body[i].maxInternalStructure))
					repairCount++;
				if (armor[i].curArmor < armor[i].maxArmor)
					repairCount++;
			}
		}
		// how many ammo types need refilling, and how much?
		for (size_t i = 0; i < numAmmoTypes; i++)
			if (ammoTypeTotal[i].curAmount < ammoTypeTotal[i].maxAmount)
				rearmCount++;
		if (pilot && pilot->getRadio())
			pilot->getRadio()->resetAmmoMessage();
		// repair each armor & IS
		for (i = 0; i < numArmorLocations; i++)
		{
			if ((repairCount == 0) || (refitPoints <= 0.0))
				continue;
			if (((i == MECH_BODY_LOCATION_LARM) || (i == MECH_BODY_LOCATION_RARM)) && (body[i].damageState == IS_DAMAGE_DESTROYED))
				continue;
			if (armor[i].curArmor < armor[i].maxArmor)
			{
				float pointsToUse = areaMax / repairCount;
				if (pointsToUse > refitPoints)
					pointsToUse = refitPoints;
				float repairValue = pointsToUse * refitCostArray[ARMOR_REFIT_COST][REFIT_VEHICLE];
				if ((armor[i].curArmor + repairValue) > armor[i].maxArmor)
				{
					repairValue = armor[i].maxArmor - armor[i].curArmor;
					pointsToUse = repairValue / refitCostArray[ARMOR_REFIT_COST][REFIT_VEHICLE];
				}
				armor[i].curArmor += repairValue;
				refitPoints -= pointsToUse;
			}
			if ((i < numBodyLocations) && (body[i].curInternalStructure < body[i].maxInternalStructure))
			{
				float pointsToUse = areaMax / repairCount;
				if (pointsToUse > refitPoints)
					pointsToUse = refitPoints;
				float repairValue =
					pointsToUse * refitCostArray[INTERNAL_REFIT_COST][REFIT_VEHICLE];
				if ((body[i].curInternalStructure + repairValue) > body[i].maxInternalStructure)
				{
					repairValue = body[i].maxInternalStructure - body[i].curInternalStructure;
					pointsToUse = repairValue / refitCostArray[INTERNAL_REFIT_COST][REFIT_VEHICLE];
				}
				body[i].curInternalStructure += repairValue;
				int32_t newDamageState = IS_DAMAGE_NONE;
				if (body[i].curInternalStructure <= 0.0)
					newDamageState = IS_DAMAGE_DESTROYED;
				else if ((body[i].curInternalStructure / body[i].maxInternalStructure) <= 0.5)
					newDamageState = IS_DAMAGE_PARTIAL;
				if ((getObjectClass() == BATTLEMECH) /*&& (newDamageState != body[i].damageState)*/)
				{
					if ((i == MECH_BODY_LOCATION_LLEG) || (i == MECH_BODY_LOCATION_RLEG))
						((BattleMechPtr)this)->calcLegStatus();
					if (i == MECH_BODY_LOCATION_CTORSO)
						((BattleMechPtr)this)->calcTorsoStatus();
				}
				body[i].damageState = newDamageState;
				refitPoints -= pointsToUse;
			}
		}
		// refit each ammo that needs it
		for (i = 0; i < numAmmoTypes; i++)
		{
			if ((rearmCount > 0) && (refitPoints > 0.0) && (ammoTypeTotal[i].curAmount < ammoTypeTotal[i].maxAmount))
			{
				bool recalc = (ammoTypeTotal[i].curAmount == 0);
				float pointsToUse = areaMax / rearmCount;
				if (pointsToUse > refitPoints)
					pointsToUse = refitPoints;
				float repairValue = pointsToUse * refitCostArray[AMMO_REFIT_COST][REFIT_VEHICLE] * MasterComponent::masterList[ammoTypeTotal[i].masterId].getAmmoPerTon();
				if ((ammoTypeTotal[i].curAmount + repairValue) > ammoTypeTotal[i].maxAmount)
				{
					repairValue = ammoTypeTotal[i].maxAmount - ammoTypeTotal[i].curAmount;
					pointsToUse = repairValue / (refitCostArray[AMMO_REFIT_COST][REFIT_VEHICLE] * MasterComponent::masterList[ammoTypeTotal[i].masterId].getAmmoPerTon());
				}
				ammoTypeTotal[i].curAmount += repairValue;
				refitPoints -= pointsToUse;
				if (recalc)
				{
					calcWeaponEffectiveness(false);
					calcFireRanges();
				}
			}
		}
		// do any locations still need repair?
		rearmCount = 0;
		repairCount = 0;
		if (!ammoOnly)
		{
			for (i = 0; i < numArmorLocations; i++)
			{
				// don't repair destroyed arms
				if (((i == MECH_BODY_LOCATION_LARM) || (i == MECH_BODY_LOCATION_RARM)) && (body[i].damageState == IS_DAMAGE_DESTROYED))
					continue;
				if (((i < numBodyLocations) && (body[i].curInternalStructure < body[i].maxInternalStructure)) || (armor[i].curArmor < armor[i].maxArmor))
				{
					repairCount++;
					break;
				}
			}
		}
		// if there's no more repairing, do any ammo types need refilling?
		if (repairCount == 0)
			for (i = 0; i < numAmmoTypes; i++)
				if (ammoTypeTotal[i].curAmount < ammoTypeTotal[i].maxAmount)
				{
					rearmCount++;
					break;
				}
		if ((repairCount == 0) && (rearmCount == 0))
		{
			// done fixing, move on
			if (getCommanderId() == Commander::home->getId())
			{
				getPilot()->radioMessage(RADIO_REFIT_DONE, TRUE);
				soundSystem->playBettySample(BETTY_REPAIR_COMPLETE);
				if (appearance)
					appearance->startSmoking(-1); // Turn the smoke off if we successfully repaired.
			}
			result = true;
		}
	}
	else
	{
		// no points left, so move on
		if (getCommanderId() == Commander::home->getId())
		{
			getPilot()->radioMessage(RADIO_REFIT_INCOMPLETE, TRUE);
			soundSystem->playBettySample(BETTY_REPAIR_GONE);
		}
		result = true;
	}
	pointsUsed = pointsAvailable - refitPoints;
	//-----------------------------------------------------------------------
	// Points used should be rounded to nearest 1/4 point (for consistency in
	// MultiPlayer)...
	if ((pointsUsed > 0.0) && (pointsUsed < 0.25))
		pointsUsed = 0.25;
	pointsUsed = (float)((int32_t)((pointsUsed + 0.125) / 0.25)) / 4.0;
	return (result);
}

//---------------------------------------------------------------------------

float Mover::calcRecoverPrice(void)
{
	float repairValue = 0;
	for (size_t i = 0; i < numBodyLocations; i++)
	{
		if (((i == MECH_BODY_LOCATION_LARM) || (i == MECH_BODY_LOCATION_RARM)) && (body[i].damageState == IS_DAMAGE_DESTROYED))
			continue;
		if ((i < numBodyLocations) && (body[i].curInternalStructure < body[i].maxInternalStructure))
			repairValue += (body[i].maxInternalStructure - body[i].curInternalStructure);
	}
	float pointsToUse = repairValue / recoverCost;
	//-----------------------------------------------------------------------
	// Points used should be rounded to nearest 1/4 point (for consistency in
	// MultiPlayer)...
	if ((pointsToUse > 0.0) && (pointsToUse < 0.25))
		pointsToUse = 0.25;
	pointsToUse = (float)((int32_t)((pointsToUse + 0.125) / 0.25)) / 4.0;
	return (pointsToUse);
}

//---------------------------------------------------------------------------

bool Mover::recover(void)
{
	inRecoverUpdate = true;
	bool result = false;
	float recoverPoints = 100000.0;
	if (recoverPoints > 0.0)
	{
		int32_t repairCount = 0;
		// how many locations need IS repaired?
		for (size_t i = 0; i < numBodyLocations; i++)
		{
			//-------------------------------
			// don't repair destroyed arms...
			if (((i == MECH_BODY_LOCATION_LARM) || (i == MECH_BODY_LOCATION_RARM)) && (body[i].damageState == IS_DAMAGE_DESTROYED))
				continue;
			if ((i < numBodyLocations) && (body[i].curInternalStructure < body[i].maxInternalStructure))
				repairCount++;
		}
		// repair each IS
		for (i = 0; i < numBodyLocations; i++)
		{
			if ((repairCount == 0) || (recoverPoints <= 0.0))
				continue;
			if (((i == MECH_BODY_LOCATION_LARM) || (i == MECH_BODY_LOCATION_RARM)) && (body[i].damageState == IS_DAMAGE_DESTROYED))
				continue;
			if ((i < numBodyLocations) && (body[i].curInternalStructure < body[i].maxInternalStructure))
			{
				float pointsToUse = recoverAmount / repairCount;
				if (pointsToUse > recoverPoints)
					pointsToUse = recoverPoints;
				float repairValue = pointsToUse * recoverCost;
				if ((body[i].curInternalStructure + repairValue) > body[i].maxInternalStructure)
				{
					repairValue = body[i].maxInternalStructure - body[i].curInternalStructure;
					pointsToUse = repairValue / recoverCost;
				}
				body[i].curInternalStructure += repairValue;
				int32_t newDamageState = IS_DAMAGE_NONE;
				if (body[i].curInternalStructure <= 0.0)
					newDamageState = IS_DAMAGE_DESTROYED;
				else if ((body[i].curInternalStructure / body[i].maxInternalStructure) <= 0.5)
					newDamageState = IS_DAMAGE_PARTIAL;
				if ((getObjectClass() == BATTLEMECH) /*&& (newDamageState != body[i].damageState)*/)
				{
					if ((i == MECH_BODY_LOCATION_LLEG) || (i == MECH_BODY_LOCATION_RLEG))
						((BattleMechPtr)this)->calcLegStatus();
					if (i == MECH_BODY_LOCATION_CTORSO)
						((BattleMechPtr)this)->calcTorsoStatus();
				}
				body[i].damageState = newDamageState;
				recoverPoints -= pointsToUse;
			}
		}
		// do any locations still need repair?
		repairCount = 0;
		for (i = 0; i < numBodyLocations; i++)
		{
			// don't repair destroyed arms
			if (((i == MECH_BODY_LOCATION_LARM) || (i == MECH_BODY_LOCATION_RARM)) && (body[i].damageState == IS_DAMAGE_DESTROYED))
				continue;
			if (body[i].curInternalStructure < body[i].maxInternalStructure)
			{
				repairCount++;
				break;
			}
		}
		if (repairCount == 0)
		{
			// done fixing, move on
			for (size_t i = 0; i < numOther; i++)
			{
				MasterComponentPtr component = &MasterComponent::masterList[inventory[i].masterID];
				switch (component->getForm())
				{
				case COMPONENT_FORM_WEAPON:
				case COMPONENT_FORM_WEAPON_ENERGY:
				case COMPONENT_FORM_WEAPON_BALLISTIC:
				case COMPONENT_FORM_WEAPON_MISSILE:
					inventory[i].health = component->getHealth();
					inventory[i].disabled = false;
					calcWeaponEffectiveness(false);
					calcFireRanges();
					break;
				default:
					inventory[i].health = component->getHealth();
					inventory[i].disabled = false;
				}
			}
			for (i = 0; i < numBodyLocations; i++)
				for (size_t j = 0; j < NumLocationCriticalSpaces[i]; j++)
					if (body[i].criticalSpaces[j].inventoryID < 255)
						body[i].criticalSpaces[j].hit =
							inventory[body[i].criticalSpaces[j].inventoryID].disabled;
			if (getCommanderId() == Commander::home->getId())
			{
				getPilot()->radioMessage(RADIO_REFIT_DONE, TRUE);
				soundSystem->playBettySample(BETTY_REPAIR_COMPLETE);
			}
			result = true;
		}
	}
	else
	{
		// no points left, so move on
		if (getCommanderId() == Commander::home->getId())
		{
			getPilot()->radioMessage(RADIO_REFIT_INCOMPLETE, TRUE);
			soundSystem->playBettySample(BETTY_REPAIR_GONE);
			soundSystem->playBettySample(BETTY_REPAIR_INCOMPLETE);
		}
		result = true;
	}
	inRecoverUpdate = false;
	return (result);
}

//---------------------------------------------------------------------------

void Mover::drawSensorTextHelp(
	float screenX, float screenY, int32_t resID, uint32_t color, bool drawBOLD)
{
	uint32_t width, height;
	Stuff::Vector4D moveHere;
	moveHere.x = screenX;
	moveHere.y = screenY;
	wchar_t buffer[256];
	cLoadString(resID, buffer, 255);
	FloatHelp::getTextStringLength(buffer, color, 1.0f, true, drawBOLD, true, false, width, height);
	moveHere.x -= width / 2;
	moveHere.z = width;
	moveHere.w = height;
	globalFloatHelp->setFloatHelp(buffer, moveHere, color, 0x0, 1.0f, true, drawBOLD, true, false);
}

//---------------------------------------------------------------------------

void Mover::drawWaypointPath()
{
	getPilot()->drawWaypointPath();
}

//---------------------------------------------------------------------------

void Mover::updateDrawWaypointPath()
{
	getPilot()->updateDrawWaypointPath();
}

//---------------------------------------------------------------------------

void Mover::initOptimalCells(int32_t numIncrements)
{
	numOptimalIncrements = numIncrements;
	double degreesToRadians = (double)DEGREES_TO_RADS;
	double incrementDegrees = 360.0 / numOptimalIncrements;
	double doptimalCells[MAX_ATTACK_CELLRANGE][MAX_ATTACK_INCREMENTS][2];
	for (size_t r = 0; r < MAX_ATTACK_CELLRANGE; r++)
		for (size_t i = 0; i < numIncrements; i++)
		{
			double incrementRadians = incrementDegrees * (double)i * degreesToRadians;
			doptimalCells[r][i][0] = sin(incrementRadians) * (double)r;
			doptimalCells[r][i][1] = cos(incrementRadians) * (double)r;
		}
	//--------------------------------------------------------------------
	// Due to double rounding errors, we munge the numbers a little before
	// casting to chars. NOTE: due to this rounding, it's possible that
	// an "optimal" cell may be a cell or more further from the target than
	// the desired range (i.e. there's an error of 10 or so meters in these
	// calcs, in some situations). Therefore, it's imperative that this is
	// acceptable (re: max fire range for the mover), otherwise the mover
	// may select an "optimal" cell from which it's out of range. As int32_t
	// as these optimal ranges fall well within their weapon fire ranges,
	// this should be no problem.
	for (r = 0; r < MAX_ATTACK_CELLRANGE; r++)
		for (size_t i = 0; i < numIncrements; i++)
		{
			if (doptimalCells[r][i][0] < 0.00)
				optimalCells[r][i][0] = (wchar_t)(doptimalCells[r][i][0] - 0.50);
			else
				optimalCells[r][i][0] = (wchar_t)(doptimalCells[r][i][0] + 0.50);
			if (doptimalCells[r][i][1] < 0.00)
				optimalCells[r][i][1] = (wchar_t)(doptimalCells[r][i][1] - 0.50);
			else
				optimalCells[r][i][1] = (wchar_t)(doptimalCells[r][i][1] + 0.50);
		}
	//-----------------------------------------------
	// Now, initialize the tables for Ranged Areas...
	wchar_t rangedMap[MAX_ATTACK_CELLRANGE * 2 + 1][MAX_ATTACK_CELLRANGE * 2 + 1];
	for (r = 0; r < (MAX_ATTACK_CELLRANGE * 2 + 1); r++)
		for (size_t c = 0; c < (MAX_ATTACK_CELLRANGE * 2 + 1); c++)
		{
			Stuff::Vector3D start, goal;
			start.x = c;
			start.y = r;
			start.z = 0;
			goal.x = MAX_ATTACK_CELLRANGE;
			goal.y = MAX_ATTACK_CELLRANGE;
			goal.z = 0.0;
			double dist = distance_from(start, goal);
			rangedMap[r][c] = (wchar_t)(dist + 0.5);
		}
	int32_t index = 0;
	for (size_t range = 0; range < MAX_ATTACK_CELLRANGE; range++)
	{
		rangedCellsIndices[range][0] = index;
		for (size_t r = 0; r < (MAX_ATTACK_CELLRANGE * 2 + 1); r++)
			for (size_t c = 0; c < (MAX_ATTACK_CELLRANGE * 2 + 1); c++)
				if (rangedMap[r][c] == range)
				{
					rangedCells[index][0] = r - MAX_ATTACK_CELLRANGE;
					rangedCells[index][1] = c - MAX_ATTACK_CELLRANGE;
					index++;
				}
		rangedCellsIndices[range][1] = index - 1;
	}
}

//***************************************************************************
// MOVE ROUTINES CALLBACKS
//***************************************************************************

void GetBlockedDoorCells(int32_t moveLevel, int32_t door, std::wstring_view openCells)
{
	Assert((door > -1) && (door < GlobalMoveMap[moveLevel]->numDoors), 0, " FUDGE 1");
	Assert((GlobalMoveMap[moveLevel]->doors[door].direction[0] == 1) || (GlobalMoveMap[moveLevel]->doors[door].direction[0] == 2),
		0, " FUDGE 2");
	Assert((GlobalMoveMap[moveLevel]->doors[door].length > 0) && (GlobalMoveMap[moveLevel]->doors[door].length < 1024),
		0, " FUDGE 3");
	int32_t doorWorldCellRec[4] = {-1, -1, -1, -1};
	if (GlobalMoveMap[moveLevel]->doors[door].direction[0] == 1)
	{
		doorWorldCellRec[0] = GlobalMoveMap[moveLevel]->doors[door].row;
		doorWorldCellRec[1] = GlobalMoveMap[moveLevel]->doors[door].col;
		doorWorldCellRec[2] = doorWorldCellRec[0] + GlobalMoveMap[moveLevel]->doors[door].length;
		doorWorldCellRec[3] = doorWorldCellRec[1] + 2;
		// NOTE: DO NOT set cells for elementals--only mechs and vehicles.
		int32_t numMovers = ObjectManager->getNumMovers();
		for (size_t i = 0; i < numMovers; i++)
		{
			std::unique_ptr<Mover> mover = ObjectManager->getMover(i);
			if ((mover->getWatchID() != PathFindMap[SECTOR_PATHMAP]->moverWID) && (mover->getWatchID() != RamObjectWID) && !mover->isDisabled())
			{
				int32_t cellPos[2];
				mover->getCellPosition(cellPos[0], cellPos[1]);
				int32_t worldCellRow = cellPos[0];
				int32_t worldCellCol = cellPos[1];
				if ((worldCellRow >= doorWorldCellRec[0]) && (worldCellRow < doorWorldCellRec[2]))
					if ((worldCellCol >= doorWorldCellRec[1]) && (worldCellCol < doorWorldCellRec[3]))
					{
						int32_t cellIndex = worldCellRow - doorWorldCellRec[0];
						Assert((cellIndex > -1) && (cellIndex < GlobalMoveMap[moveLevel]->doors[door].length),
							0, " Bad Cell Index ");
						openCells[cellIndex] = 0;
					}
			}
		}
	}
	else
	{
		doorWorldCellRec[0] = GlobalMoveMap[moveLevel]->doors[door].row;
		doorWorldCellRec[1] = GlobalMoveMap[moveLevel]->doors[door].col;
		doorWorldCellRec[2] = doorWorldCellRec[0] + 2;
		doorWorldCellRec[3] = doorWorldCellRec[1] + GlobalMoveMap[moveLevel]->doors[door].length;
		int32_t numMovers = ObjectManager->getNumMovers();
		for (size_t i = 0; i < numMovers; i++)
		{
			std::unique_ptr<Mover> mover = ObjectManager->getMover(i);
			if ((mover->getObjectClass() != ELEMENTAL) && (mover->getWatchID() != PathFindMap[SECTOR_PATHMAP]->moverWID) && (mover->getWatchID() != RamObjectWID) && !mover->isDisabled())
			{
				int32_t cellPos[2];
				mover->getCellPosition(cellPos[0], cellPos[1]);
				int32_t worldCellRow = cellPos[0];
				int32_t worldCellCol = cellPos[1];
				if ((worldCellRow >= doorWorldCellRec[0]) && (worldCellRow < doorWorldCellRec[2]))
					if ((worldCellCol >= doorWorldCellRec[1]) && (worldCellCol < doorWorldCellRec[3]))
					{
						int32_t cellIndex = worldCellCol - doorWorldCellRec[1];
						Assert((cellIndex > -1) && (cellIndex < GlobalMoveMap[moveLevel]->doors[door].length),
							0, " Bad Cell Index ");
						openCells[cellIndex] = 0;
					}
			}
		}
	}
}

//---------------------------------------------------------------------------

void PlaceMovers(void)
{
	int32_t numMovers = ObjectManager->getNumMovers();
	for (size_t i = 0; i < numMovers; i++)
	{
		std::unique_ptr<Mover> mover = (std::unique_ptr<Mover>)ObjectManager->getMover(i);
		if (mover->getUseMe())
		{
			int32_t cellRow, cellCol;
			mover->getCellPosition(cellRow, cellCol);
			if (!GameMap->getPreserved(cellRow, cellCol))
				GameMap->setPreserved(cellRow, cellCol, true);
			GameMap->placeObject(mover->getPosition(), mover->getExtentRadius());
			GameMap->setPassable(cellRow, cellCol, false);
		}
	}
}

//---------------------------------------------------------------------------

void adjustMoveMapCellCost(MoveMapNodePtr cell, int32_t costAdj);

void PlaceStationaryMovers(MoveMap* map)
{
	int32_t numMovers = ObjectManager->getNumMovers();
	for (size_t i = 0; i < numMovers; i++)
	{
		std::unique_ptr<Mover> mover = ObjectManager->getMover(i);
		if ((mover->getObjectClass() != ELEMENTAL) && (mover->getWatchID() != map->moverWID) && (mover->getWatchID() != RamObjectWID) && !mover->isDisabled())
		{
			int32_t cellRow, cellCol;
			mover->getCellPosition(cellRow, cellCol);
			if ((cellRow >= 0) && (cellRow < map->height) && (cellCol >= 0) && (cellCol < map->width))
			{
				if ((cellRow != map->startR) || (cellCol != map->startC))
				{
					if ((map->map[cellRow * map->maxwidth + cellCol].flags & MOVEFLAG_GOAL) == 0)
					{
						MovePathPtr path = mover->getPilot()->getMovePath();
						if (path && (path->numSteps == 0))
						{
							map->map[cellRow * map->maxwidth + cellCol].setFlag(
								MOVEFLAG_MOVER_HERE);
#ifdef USE_OVERLAYS
							int32_t bridgeCost = COST_BLOCKED / 3;
							int32_t overlay = GameMap->getOverlay(cellRow, cellCol);
							if (OverlayIsBridge[overlay])
								map->adjustCellCost(cellRow, cellCol, bridgeCost);
							else
#endif
								map->adjustCost(cellRow, cellCol, COST_BLOCKED * 2);
						}
					}
				}
			}
		}
	}
}

bool Mover::isCloseToFirstTacOrder(Stuff::Vector3D& pos)
{
	return getPilot()->isCloseToFirstTacOrder(pos);
}

void Mover::removeFromUnitGroup(int32_t id)
{
	unitGroup = -1;
	// unitGroup &= (~(1<<id));
}
void Mover::addToUnitGroup(int32_t id)
{
	unitGroup = (1 << id); // now you only get to be in one at a time
}
bool Mover::isInUnitGroup(int32_t id)
{
	if (unitGroup == -1)
		return false;
	return unitGroup & (1 << id) ? true : false;
}
bool Mover::handleEjection()
{
	if (pilot && ((pilot->getStatus() == WARRIOR_STATUS_NORMAL) || (pilot->getStatus() == WARRIOR_STATUS_WITHDRAWING)))
	{
		//----------------
		// First, eject...
		getPilot()->eject();
		ejectOrderGiven = true;
		//------------------
		// The head blows...
		//		destroyBodyLocation(MECH_BODY_LOCATION_HEAD);
		//---------------------
		// Create the Eject FX
		// WHEN READY -fs
		//---------------------------------------------
		// If we aren't already disabled, we are now...
		disable(EJECTION_DEATH);
#ifdef USE_IFACE
		//------------------------------------------------------
		// The interface needs to know I'm not around anymore...
		theInterface->RemoveMech(getPartId());
#endif
#ifdef USE_MOODMUSIC
		//------------------------------------
		// What heroic music should be played?
		if (alignment == homeTeam->getAlignment())
			friendlydestroyed = true;
		else
			enemydestroyed = true;
#endif
	}
	return (true);
}

bool Mover::isWeaponWorking(int32_t weaponIndex)
{
	if (inventory[weaponIndex].disabled)
		return (FALSE);
	else if (getWeaponShots(weaponIndex) == 0)
		return (FALSE);
	return (TRUE);
}

//***************************************************************************
void Mover::Save(PacketFilePtr file, int32_t packetNum)
{
	MoverData data;
	CopyTo(&data);
	// PacketNum incremented in ObjectManager!!
	file->writePacket(packetNum, (uint8_t*)&data, sizeof(MoverData), STORAGE_TYPE_ZLIB);
}

//***************************************************************************
void Mover::CopyTo(MoverData* data)
{
	data->killed = killed;
	data->lost = lost;
	data->positionNormal = positionNormal;
	data->velocity = velocity;
	memcpy(data->name, name, sizeof(wchar_t) * MAXLEN_MOVER_NAME);
	data->chassis = chassis;
	data->startDisabled = startDisabled;
	data->creationTime = creationTime;
	data->moveType = moveType;
	data->moveLevel = moveLevel;
	data->followRoads = followRoads;
	memcpy(data->lastMapCell, lastMapCell, sizeof(int32_t) * 2);
	data->damageRateTally = damageRateTally;
	data->damageRateCheckTime = damageRateCheckTime;
	data->pilotCheckDamageTally = pilotCheckDamageTally;
	memcpy(data->body, body, sizeof(BodyLocation) * MAX_MOVER_BODY_LOCATIONS);
	data->numBodyLocations = numBodyLocations;
	data->fieldedCV = fieldedCV;
	data->attackRange = attackRange;
	memcpy(data->armor, armor, sizeof(ArmorLocation) * MAX_MOVER_ARMOR_LOCATIONS);
	data->numArmorLocations = numArmorLocations;
	memcpy(data->longName, longName, sizeof(wchar_t) * MAXLEN_MECH_LONGNAME);
	memcpy(data->inventory, inventory, sizeof(InventoryItem) * MAX_MOVER_INVENTORY_ITEMS);
	data->numOther = numOther;
	data->numWeapons = numWeapons;
	data->numAmmos = numAmmos;
	memcpy(data->ammoTypeTotal, ammoTypeTotal, sizeof(AmmoTally) * MAX_AMMO_TYPES);
	data->numAmmoTypes = numAmmoTypes;
	data->pilotHandle = pilotHandle;
	data->cockpit = cockpit;
	data->engine = engine;
	data->lifeSupport = lifeSupport;
	data->sensor = sensor;
	data->ecm = ecm;
	data->probe = probe;
	data->jumpJets = jumpJets;
	data->nullSignature = nullSignature;
	data->maxWeaponEffectiveness = maxWeaponEffectiveness;
	data->weaponEffectiveness = weaponEffectiveness;
	data->minRange = minRange;
	data->maxRange = maxRange;
	data->optimalRange = optimalRange;
	data->numFunctionalWeapons = numFunctionalWeapons;
	data->numAntiMissileSystems = numAntiMissileSystems;
	memcpy(data->antiMissileSystem, antiMissileSystem, sizeof(uint8_t) * MAX_ANTI_MISSILE_SYSTEMS);
	data->engineBlowTime = engineBlowTime;
	data->maxMoveSpeed = maxMoveSpeed;
	data->shutDownThisFrame = shutDownThisFrame;
	data->startUpThisFrame = startUpThisFrame;
	data->disableThisFrame = disableThisFrame;
	data->teamId = teamId;
	data->groupId = groupId;
	data->squadId = squadId;
	data->selectionindex = selectionindex;
	data->teamRosterIndex = teamRosterIndex;
	data->commanderId = commanderId;
	data->unitGroup = unitGroup;
	data->iconPictureIndex = iconPictureIndex;
	data->suppressionFire = suppressionFire;
	data->pilotCheckModifier = pilotCheckModifier;
	data->prevPilotCheckModifier = prevPilotCheckModifier;
	data->prevPilotCheckDelta = prevPilotCheckDelta;
	data->prevPilotCheckUpdate = prevPilotCheckUpdate;
	data->failedPilotingCheck = failedPilotingCheck;
	data->lastWeaponEffectivenessCalc = lastWeaponEffectivenessCalc;
	data->lastOptimalRangeCalc = lastOptimalRangeCalc;
	data->challengerWID = challengerWID;
	data->lastGesture = lastGesture;
	data->control = control;
	data->dynamics = dynamics;
	data->numWeaponHitsHandled = numWeaponHitsHandled;
	data->timeLeft = timeLeft;
	data->exploding = exploding;
	data->withdrawing = withdrawing;
	data->yieldTimeLeft = yieldTimeLeft;
	data->lastValidPosition = lastValidPosition;
	data->pivotDirection = pivotDirection;
	data->lastHustleTime = lastHustleTime;
	data->salvageVehicle = salvageVehicle;
	data->markDistanceMoved = markDistanceMoved;
	data->refitBuddyWID = refitBuddyWID;
	data->recoverBuddyWID = recoverBuddyWID;
	data->crashAvoidSelf = crashAvoidSelf;
	data->crashAvoidPath = crashAvoidPath;
	data->crashBlockSelf = crashBlockSelf;
	data->crashBlockPath = crashBlockPath;
	data->crashYieldTime = crashYieldTime;
	data->pathLockLength = pathLockLength;
	memcpy(data->pathLockList, pathLockList, sizeof(int32_t) * MAX_LOCK_RANGE * 2);
	data->moveCenter = moveCenter;
	data->moveRadius = moveRadius;
	data->overlayWeightClass = overlayWeightClass;
	data->timeToClearSelection = timeToClearSelection;
	data->timeSinceMoving = timeSinceMoving;
	data->timeSinceFiredLast = timeSinceFiredLast;
	data->lastMovingTargetWID = lastMovingTargetWID;
	data->mechSalvage = mechSalvage;
	data->teleportPosition = teleportPosition;
	data->debugPage = debugPage;
	data->pathLocks = pathLocks;
	data->isOnGui = isOnGui;
	data->conStat = conStat;
	data->fadeTime = fadeTime;
	data->alphaValue = alphaValue;
	data->causeOfDeath = causeOfDeath;
	data->lowestWeaponNodeID = lowestWeaponNodeID;
	if (getAppearance())
	{
		getAppearance()->getPaintScheme(data->psRed, data->psGreen, data->psBlue);
	}
	GameObject::CopyTo(dynamic_cast<GameObjectData*>(data));
}

//---------------------------------------------------------------------------
void Mover::Load(MoverData* data)
{
	GameObject::Load(dynamic_cast<GameObjectData*>(data));
	killed = data->killed;
	lost = data->lost;
	positionNormal = data->positionNormal;
	velocity = data->velocity;
	memcpy(name, data->name, sizeof(wchar_t) * MAXLEN_MOVER_NAME);
	chassis = data->chassis;
	startDisabled = data->startDisabled;
	creationTime = data->creationTime;
	moveType = data->moveType;
	moveLevel = data->moveLevel;
	followRoads = data->followRoads;
	memcpy(lastMapCell, data->lastMapCell, sizeof(int32_t) * 2);
	damageRateTally = data->damageRateTally;
	damageRateCheckTime = data->damageRateCheckTime;
	pilotCheckDamageTally = data->pilotCheckDamageTally;
	memcpy(body, data->body, sizeof(BodyLocation) * MAX_MOVER_BODY_LOCATIONS);
	numBodyLocations = data->numBodyLocations;
	fieldedCV = data->fieldedCV;
	attackRange = data->attackRange;
	memcpy(armor, data->armor, sizeof(ArmorLocation) * MAX_MOVER_ARMOR_LOCATIONS);
	numArmorLocations = data->numArmorLocations;
	memcpy(longName, data->longName, sizeof(wchar_t) * MAXLEN_MECH_LONGNAME);
	memcpy(inventory, data->inventory, sizeof(InventoryItem) * MAX_MOVER_INVENTORY_ITEMS);
	numOther = data->numOther;
	numWeapons = data->numWeapons;
	numAmmos = data->numAmmos;
	memcpy(ammoTypeTotal, data->ammoTypeTotal, sizeof(AmmoTally) * MAX_AMMO_TYPES);
	numAmmoTypes = data->numAmmoTypes;
	pilotHandle = data->pilotHandle;
	cockpit = data->cockpit;
	engine = data->engine;
	lifeSupport = data->lifeSupport;
	sensor = data->sensor;
	ecm = data->ecm;
	probe = data->probe;
	jumpJets = data->jumpJets;
	nullSignature = data->nullSignature;
	maxWeaponEffectiveness = data->maxWeaponEffectiveness;
	weaponEffectiveness = data->weaponEffectiveness;
	minRange = data->minRange;
	maxRange = data->maxRange;
	optimalRange = data->optimalRange;
	numFunctionalWeapons = data->numFunctionalWeapons;
	numAntiMissileSystems = data->numAntiMissileSystems;
	memcpy(antiMissileSystem, data->antiMissileSystem, sizeof(uint8_t) * MAX_ANTI_MISSILE_SYSTEMS);
	engineBlowTime = data->engineBlowTime;
	maxMoveSpeed = data->maxMoveSpeed;
	shutDownThisFrame = data->shutDownThisFrame;
	startUpThisFrame = data->startUpThisFrame;
	disableThisFrame = data->disableThisFrame;
	teamId = data->teamId;
	groupId = data->groupId;
	squadId = data->squadId;
	selectionindex = data->selectionindex;
	teamRosterIndex = data->teamRosterIndex;
	commanderId = data->commanderId;
	unitGroup = data->unitGroup;
	iconPictureIndex = data->iconPictureIndex;
	suppressionFire = data->suppressionFire;
	pilotCheckModifier = data->pilotCheckModifier;
	prevPilotCheckModifier = data->prevPilotCheckModifier;
	prevPilotCheckDelta = data->prevPilotCheckDelta;
	prevPilotCheckUpdate = data->prevPilotCheckUpdate;
	failedPilotingCheck = data->failedPilotingCheck;
	lastWeaponEffectivenessCalc = data->lastWeaponEffectivenessCalc;
	lastOptimalRangeCalc = data->lastOptimalRangeCalc;
	challengerWID = data->challengerWID;
	lastGesture = data->lastGesture;
	control = data->control;
	dynamics = data->dynamics;
	numWeaponHitsHandled = data->numWeaponHitsHandled;
	timeLeft = data->timeLeft;
	exploding = data->exploding;
	withdrawing = data->withdrawing;
	yieldTimeLeft = data->yieldTimeLeft;
	lastValidPosition = data->lastValidPosition;
	pivotDirection = data->pivotDirection;
	lastHustleTime = data->lastHustleTime;
	salvageVehicle = data->salvageVehicle;
	markDistanceMoved = data->markDistanceMoved;
	refitBuddyWID = data->refitBuddyWID;
	recoverBuddyWID = data->recoverBuddyWID;
	crashAvoidSelf = data->crashAvoidSelf;
	crashAvoidPath = data->crashAvoidPath;
	crashBlockSelf = data->crashBlockSelf;
	crashBlockPath = data->crashBlockPath;
	crashYieldTime = data->crashYieldTime;
	pathLockLength = data->pathLockLength;
	memcpy(pathLockList, data->pathLockList, sizeof(int32_t) * MAX_LOCK_RANGE * 2);
	moveCenter = data->moveCenter;
	moveRadius = data->moveRadius;
	overlayWeightClass = data->overlayWeightClass;
	timeToClearSelection = data->timeToClearSelection;
	timeSinceMoving = data->timeSinceMoving;
	timeSinceFiredLast = data->timeSinceFiredLast;
	lastMovingTargetWID = data->lastMovingTargetWID;
	mechSalvage = data->mechSalvage;
	teleportPosition = data->teleportPosition;
	debugPage = data->debugPage;
	pathLocks = data->pathLocks;
	isOnGui = data->isOnGui;
	conStat = data->conStat;
	fadeTime = data->fadeTime;
	alphaValue = data->alphaValue;
	causeOfDeath = data->causeOfDeath;
	lowestWeaponNodeID = data->lowestWeaponNodeID;
	// OK.  Must get pilotPtr back.
	pilot = MechWarrior::warriorList[pilotHandle];
	if (!pilot)
		STOP(("Could not find pilot for mover after in-mission load"));
	// Set the PaintScheme
	if (getAppearance())
	{
		getAppearance()->resetPaintScheme(data->psRed, data->psGreen, data->psBlue);
	}
}

//***************************************************************************
