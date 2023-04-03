//***************************************************************************
//
//	mech.cpp - This file contains the BattleMech Class header
//
//	MechCommander 2
//
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//
#include "stdinc.h"

// #include "mclib.h"

#ifndef GAMEOBJ_H
#include "gameobj.h"
#endif

#ifndef MECH_H
#include "mech.h"
#endif

#ifndef GVEHICL_H
#include "gvehicl.h"
#endif

#ifndef MISSION_H
#include "mission.h"
#endif

#ifndef CMPONENT_H
#include "cmponent.h"
#endif

#ifndef WARRIOR_H
#include "warrior.h"
#endif

#ifndef WEAPONBOLT_H
#include "weaponbolt.h"
#endif

#ifdef USE_DEBRIS
#ifndef DEBRIS_H
#include "debris.h"
#endif
#endif

#ifdef USE_JETS
#ifndef JET_H
#include <jet.h>
#endif
#endif

#ifndef TACORDR_H
#include "tacordr.h"
#endif

#include "gamesound.h"
#ifndef SOUNDS_H
#include "sounds.h"
#endif

#ifndef COLLSN_H
#include "collsn.h"
#endif

#ifndef WEAPONFX_H
#include "weaponfx.h"
#endif

#ifndef OBJMGR_H
#include "objmgr.h"
#endif

#ifndef MULTPLYR_H
#include "multplyr.h"
#endif

#ifdef OBJTYPE_H
#include "objtype.h"
#endif

#ifndef TEAM_H
#include "team.h"
#endif

#ifndef MECH3D_H
#include "mech3d.h"
#endif

#ifndef GROUP_H
#include "group.h"
#endif

#ifndef UNITDESG_H
#include "unitdesg.h"
#endif

#ifdef USE_ELEMENTALS
#ifndef ELEMNTL_H
#include "elemntl.h"
#endif
#endif

#ifndef CONTACT_H
#include "contact.h"
#endif

#ifndef TURRET_h
#include "turret.h"
#endif

#ifndef GAMEOLOG_H
#include "gamelog.h"
#endif

#ifndef COMNDR_H
#include "comndr.h"
#endif

#ifndef LOGISTICSDATA_H
#include "LogisticsData.h"
#endif

#include "resource.h"

//--------
// DEFINES
#define GOALMAP_DIM 21
#define BLIP_FRAME_RATE 0.067
//--------
// GLOBALS

extern float MaxVisualRadius;

//--------
// EXTERNS

extern uint32_t NextIdNumber;
extern float MetersPerCell;
extern int32_t AttitudeEffectOnMovePath[AttitudeType::count][3];
extern bool useSound;
extern bool friendlydestroyed;
extern bool enemydestroyed;
extern TeamPtr homeTeam;

extern int32_t MechSalvageChance;

extern int32_t woundFatigue[5];
extern float FireOddsTable[FireOddsType::count];

extern float MovementUpdateFrequency;
extern float CombatUpdateFrequency;
extern float CommandUpdateFrequency;
extern float ContactUpdateFrequency;
extern float OrderRequestFrequency;
extern float elmDamageOnImpact;
extern bool drawTerrainGrid;
extern uint8_t footPrints;

extern float MaxTimeRevealed;

extern GameLog* CombatLog;

extern float MineDamage;
extern float MineSplashDamage;
extern float MineSplashRange;
extern int32_t MineExplosion;

extern float SkillTry[4];
extern float SkillSuccess[4];

extern void
addMoverToList(int32_t blockNum);
extern wchar_t mechSpeedStateArray[28];

extern int32_t TargetRolo;
extern float MapCellDiagonal;
extern GlobalMapPtr GlobalMoveMap[3];

extern bool CantTouchThis;

extern WeaponFireChunk CurMoverWeaponFireChunk;
extern void
DebugWeaponFireChunk(
	WeaponFireChunkPtr chunk1, WeaponFireChunkPtr chunk2, GameObjectPtr attacker);

extern int32_t StatusChunkUnpackErr;
extern float worldUnitsPerMeter;

float ContactFadeTime = 2.0f;

bool invulnerableON = false;

//**********************************************************************************

int32_t NumLocationCriticalSpaces[NUM_BODY_LOCATIONS] = {NUM_CRITICAL_SPACES_HEAD,
	NUM_CRITICAL_SPACES_CTORSO, NUM_CRITICAL_SPACES_LTORSO, NUM_CRITICAL_SPACES_RTORSO,
	NUM_CRITICAL_SPACES_LARM, NUM_CRITICAL_SPACES_RARM, NUM_CRITICAL_SPACES_LLEG,
	NUM_CRITICAL_SPACES_RLEG};

int32_t MechHitSectionTable[NUM_ATTACKSOURCES] = {
	MECH_HIT_SECTION_MIDDLE, // Weapon Fire
	MECH_HIT_SECTION_MIDDLE, // Collision
	MECH_HIT_SECTION_TOP, // Death From Above
	MECH_HIT_SECTION_BOTTOM, // Mine
	MECH_HIT_SECTION_MIDDLE // Artillery
};

#define numHotSpots 7
#define DRAW_EVERY_TIME 1
#define DRAW_ONCE -1
#define DRAW_NEVER 0

#define MECH_BASE_ROTATIONS 9
#define MECH_BASE_ANGLE float(40)
#define MECH_MAGIC_ALIAS_CONVERSION_FACTOR float(20.0)

#define NUM_FEET 2
#define OTHER_POSITIONS 7
#define MAX_FRAMES 60

#define GestureGoalFallenBackward 8

#ifdef USEHEAT
HeatEffectEntryPtr HeatEffectTable = nullptr;
int32_t NumHeatLevels = 0;
#endif

#define NUM_DAMAGE_SPOTS 3

#define MECH_FOOTPRINT_BASE_ANGLE 22.5
//=================
// DEBUG HACK!!!!!
extern GameObjectPtr mech1;
extern GameObjectPtr mech2;
//=================

// OLD One... In case someone changes their mind.  I know that never happens!!
/*
float tonnageTurnRate[110] =
{
	360.0f,360.0f,360.0f,360.0f,360.0f,360.0f,360.0f,360.0f,360.0f,360.0f, //0
through 9 tons
	360.0f,360.0f,360.0f,360.0f,360.0f,360.0f,360.0f,360.0f,360.0f,360.0f, //10
through 19 tons
	360.0f,360.0f,360.0f,360.0f,360.0f,360.0f,360.0f,360.0f,360.0f,360.0f, //20
through 29 tons
	360.0f,360.0f,360.0f,360.0f,360.0f,360.0f,360.0f,360.0f,360.0f,360.0f, //30
through 39 tons
	270.0f,270.0f,270.0f,270.0f,270.0f,270.0f,270.0f,270.0f,270.0f,270.0f, //40
through 49 tons
	240.0f,240.0f,240.0f,240.0f,240.0f,240.0f,240.0f,240.0f,240.0f,240.0f, //50
through 59 tons
	210.0f,210.0f,210.0f,210.0f,210.0f,210.0f,210.0f,210.0f,210.0f,210.0f, //60
through 69 tons
	140.0f,140.0f,140.0f,140.0f,140.0f,140.0f,140.0f,140.0f,140.0f,140.0f, //70
through 79 tons
	120.0f,120.0f,120.0f,120.0f,120.0f,120.0f,120.0f,120.0f,120.0f,120.0f, //80
through 89 tons 90.0f, 90.0f, 90.0f, 90.0f, 90.0f, 90.0f, 90.0f, 90.0f,
90.0f, 90.0f, //90 through 99 tons 90.0f, 90.0f, 90.0f, 90.0f, 90.0f,
90.0f, 90.0f, 90.0f, 90.0f, 90.0f //100 through 109 tons
};
*/

float tonnageTurnRate[110] = {
	720.0f, 720.0f, 720.0f, 720.0f, 720.0f, 720.0f, 720.0f, 720.0f, 720.0f,
	720.0f, // 0 through 9 tons
	720.0f, 720.0f, 720.0f, 720.0f, 720.0f, 720.0f, 720.0f, 720.0f, 720.0f,
	720.0f, // 10 through 19 tons
	720.0f, 720.0f, 720.0f, 720.0f, 720.0f, 720.0f, 720.0f, 720.0f, 720.0f,
	720.0f, // 20 through 29 tons
	720.0f, 720.0f, 720.0f, 720.0f, 720.0f, 720.0f, 720.0f, 720.0f, 720.0f,
	720.0f, // 30 through 39 tons
	720.0f, 720.0f, 720.0f, 720.0f, 720.0f, 720.0f, 720.0f, 720.0f, 720.0f,
	720.0f, // 40 through 49 tons
	720.0f, 720.0f, 720.0f, 720.0f, 720.0f, 720.0f, 720.0f, 720.0f, 720.0f,
	720.0f, // 50 through 59 tons
	720.0f, 720.0f, 720.0f, 720.0f, 720.0f, 720.0f, 720.0f, 720.0f, 720.0f,
	720.0f, // 60 through 69 tons
	720.0f, 720.0f, 720.0f, 720.0f, 720.0f, 720.0f, 720.0f, 720.0f, 720.0f,
	720.0f, // 70 through 79 tons
	720.0f, 720.0f, 720.0f, 720.0f, 720.0f, 720.0f, 720.0f, 720.0f, 720.0f,
	720.0f, // 80 through 89 tons
	720.0f, 720.0f, 720.0f, 720.0f, 720.0f, 720.0f, 720.0f, 720.0f, 720.0f,
	720.0f, // 90 through 99 tons
	720.0f, 720.0f, 720.0f, 720.0f, 720.0f, 720.0f, 720.0f, 720.0f, 720.0f,
	720.0f // 100 through 109 tons
};

int32_t stupidNewSageColumns[8] = {2, 7, 5, 6, 3, 4, 8, 9};

#define ARMOR_POINTS_PER_NEWARMOR 32.0f
#define ARMOR_HEAD_PERCENT 0.0f
#define ARMOR_CTORSO_PERCENT 0.17f
#define ARMOR_RTORSO_PERCENT 0.125f
#define ARMOR_LTORSO_PERCENT 0.125f
#define ARMOR_LARM_PERCENT 0.12f
#define ARMOR_RARM_PERCENT 0.12f
#define ARMOR_LLEG_PERCENT 0.13f
#define ARMOR_RLEG_PERCENT 0.13f
#define ARMOR_RCTORSO_PERCENT 0.03f
#define ARMOR_RLTORSO_PERCENT 0.02f
#define ARMOR_RRTORSO_PERCENT 0.02f
//---------------------------------------------------------------------------
// Game System Tweakable Data
//---------------------------------------------------------------------------

extern float PilotingCheckFactor;

extern int32_t hitLevel[2];

extern int32_t PilotCheckModifierTable[2];

extern int32_t ClusterSizeSRM;
extern int32_t ClusterSizeLRM;

extern int32_t AntiMissileSystemStats[2][2];

extern float PilotCheckHalfRate;

extern float WeaponRange[FireRangeType::count];
extern float WeaponRanges[NUM_WEAPON_RANGE_TYPES][2];

extern float DisableAttackModifier;
extern float DisableGunneryModifier;
extern float SalvageAttackModifier;

int32_t adjClippedCell[8][2] = {{0, 0}, {0, 2}, {2, 2}, {2, 4}, {4, 4}, {4, 6}, {6, 6}, {6, 0}};

float MechClassScale[NUM_MECH_CLASSES] = {0.0, 15.0, 40.0, 75.0, 200.0};

float RankVersusChassisCombatModifier[WarriorRank::numberofranks][NUM_MECH_CLASSES] = {
	{0.0, 0.0, -5.0, -15.0, -25.0}, {0.0, 5.0, 0.0, -5.0, -15.0}, {0.0, 10.0, 5.0, 0.0, -5.0},
	{0.0, 15.0, 10.0, 5.0, 0.0}};

float WeaponFireJumpTime = 3.0;
extern float WeaponFireModifiers[NUM_WEAPONFIRE_MODIFIERS];

int32_t AttackerMoveModifier[NUM_MECH_STATES] = {
	0, // Parked
	0, // Standing
	10, // Walking
	20, // Running
	10, // Reverse
	5, // Limping
	30, // Jumping
	0, // Fallen Forward
	0 // Fallen Backward
};

wchar_t CriticalHitTable[4] = {
	58, // 0 Hits
	83, // 1 Hits
	97, // 2 Hits
	100 // Head\Limb Destroyed or 3 Hits if Torso
};

int32_t TargetMoveModifierTable[5][2] = {{6, 0}, {12, 1}, {18, 2}, {27, 3}, {999, 4}};

float MechClassWeights[NUM_MECH_CLASSES] = {0.0, 35.0, 55.0, 75.0, 100.0};

#ifdef USEHEAT
float HeatCheckFrequency = 10.0;
float StandUpHeat = 15.0;
float BodyStateHeat[NUM_MECH_STATES] = {
	0.0, // Parked
	0.0, // Standing
	0.15, // Walking
	0.3, // Running
	0.15, // Reverse
	0.1, // Limping
	0.75, // Jumping
	0.0, // Fallen Forward
	0.0 // Fallen Backward
};
#endif

wchar_t MechHitLocationTable[NUM_MECH_HIT_SECTIONS * NUM_MECH_HIT_ARCS * NUM_MECH_ARMOR_LOCATIONS] = {
	30, 20, 25, 25, 0, 0, 0, 0, 0, 0, 0, // Top Front
	30, 0, 0, 0, 0, 0, 0, 0, 20, 25, 25, // Top Rear
	20, 15, 25, 0, 0, 0, 0, 0, 15, 25, 0, // Top Left
	20, 15, 0, 25, 0, 0, 0, 0, 15, 0, 25, // Top Right
	0, 20, 20, 20, 20, 20, 0, 0, 0, 0, 0, // Middle Front
	0, 0, 0, 0, 20, 20, 0, 0, 20, 20, 20, // Middle Rear
	0, 0, 25, 0, 50, 0, 0, 0, 0, 25, 0, // Middle Left
	0, 0, 0, 25, 0, 50, 0, 0, 0, 0, 25, // Middle Right
	0, 10, 0, 0, 0, 0, 45, 45, 0, 0, 0, // Bottom Front
	0, 0, 0, 0, 0, 0, 45, 45, 10, 0, 0, // Bottom Rear
	0, 0, 0, 0, 0, 0, 100, 0, 0, 0, 0, // Bottom Left
	0, 0, 0, 0, 0, 0, 0, 100, 0, 0, 0 // Bottom Right
};

wchar_t MechTransferHitTable[NUM_MECH_BODY_LOCATIONS] = {MECH_BODY_LOCATION_CTORSO, -1,
	MECH_BODY_LOCATION_CTORSO, MECH_BODY_LOCATION_CTORSO, MECH_BODY_LOCATION_LTORSO,
	MECH_BODY_LOCATION_RTORSO, MECH_BODY_LOCATION_LTORSO, MECH_BODY_LOCATION_RTORSO};

wchar_t MechArmorToBodyLocation[NUM_MECH_ARMOR_LOCATIONS] = {
	MECH_BODY_LOCATION_HEAD,
	MECH_BODY_LOCATION_CTORSO,
	MECH_BODY_LOCATION_LTORSO,
	MECH_BODY_LOCATION_RTORSO,
	MECH_BODY_LOCATION_LARM,
	MECH_BODY_LOCATION_RARM,
	MECH_BODY_LOCATION_LLEG,
	MECH_BODY_LOCATION_RLEG,
	MECH_BODY_LOCATION_CTORSO,
	MECH_BODY_LOCATION_LTORSO,
	MECH_BODY_LOCATION_RTORSO,
};

int32_t MechPilotCheckConditions[2] = {25, 25};

int32_t MechPilotCheckTerrainEffect[64] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

#ifdef USEHEAT
int32_t HeatInjuryTable[2][2] = {{15, 1}, {26, 2}};
float heatSinkEfficiency = 1.0;
int32_t heatShutdown = 30;
#endif

extern int32_t AimedFireAbort;
extern int32_t AimedFireHitTable[3];

float mechCollisionThreshold = 0.0;
float objectCollisionThreshold = 0.0;
float tonnageCollisionThreshold = 0.0;
float treeDeflection = 0.0;
float mechPivotAngle = 0.0;
float mechPivotThrottle = 0.0;

float MaxVelocityMag = 0.0;
float NewRotation = 0.0;

int32_t DefaultMechCrashAvoidSelf = 1;
int32_t DefaultMechCrashAvoidPath = 1;
int32_t DefaultMechCrashBlockSelf = 1;
int32_t DefaultMechCrashBlockPath = 1;
float DefaultMechCrashYieldTime = 2.0;
int32_t DefaultMechJumpCost = COST_BLOCKED / 3;

GameObjectPtr BadGuy = nullptr;
extern const std::unique_ptr<Debugger>& debugger;

//#define USE_MECHDEBUGFILE
#ifdef USE_MECHDEBUGFILE
UserFile* MechDebugFile = nullptr;
#endif

extern bool ShowMovers;
extern void
LogWeaponFireChunk(
	WeaponFireChunkPtr chunk, GameObjectPtr attacker, GameObjectPtr target);

//**********************************************************************************
// BATTLEMECH TYPE class
//**********************************************************************************

int32_t
BattleMech::loadGameSystem(FitIniFilePtr mechFile)
{
	int32_t result = 0;
	result = mechFile->seekBlock("Mech:Class");
	if (result != NO_ERROR)
		return (result);
	result = mechFile->readIdFloat("MaxLightMech", MechClassWeights[MECH_CLASS_LIGHT]);
	if (result != NO_ERROR)
		return (result);
	result = mechFile->readIdFloat("MaxHeavyMech", MechClassWeights[MECH_CLASS_MEDIUM]);
	if (result != NO_ERROR)
		return (result);
	result = mechFile->readIdFloat("MaxHeavyMech", MechClassWeights[MECH_CLASS_HEAVY]);
	if (result != NO_ERROR)
		return (result);
	result = mechFile->seekBlock("Mech:Movement");
	if (result != NO_ERROR)
		return (result);
	int32_t jumpCost = 0;
	result = mechFile->readIdLong("JumpCost", jumpCost);
	if (result == NO_ERROR)
		DefaultMechJumpCost = jumpCost;
	int32_t crashSize = 0;
	result = mechFile->readIdLong("CrashAvoidSelf", crashSize);
	if (result == NO_ERROR)
		DefaultMechCrashAvoidSelf = crashSize;
	result = mechFile->readIdLong("CrashAvoidPath", crashSize);
	if (result == NO_ERROR)
		DefaultMechCrashAvoidPath = crashSize;
	result = mechFile->readIdLong("CrashBlockSelf", crashSize);
	if (result == NO_ERROR)
		DefaultMechCrashBlockSelf = crashSize;
	result = mechFile->readIdLong("CrashBlockPath", crashSize);
	if (result == NO_ERROR)
		DefaultMechCrashBlockPath = crashSize;
	float crashYield = 0.0;
	result = mechFile->readIdFloat("CrashYieldTime", crashYield);
	if (result == NO_ERROR)
		DefaultMechCrashYieldTime = crashYield;
	result = mechFile->readIdLongArray(
		"PilotCheckConditions", MechPilotCheckConditions, NUM_MECH_PILOT_CHECK_CONDITIONS);
	if (result != NO_ERROR)
		return (result);
	result = mechFile->readIdLongArray("PilotCheckTerrainEffect", MechPilotCheckTerrainEffect, 64);
	if (result != NO_ERROR)
		return (result);
	result = mechFile->seekBlock("Mech:FireWeapon");
	if (result != NO_ERROR)
		return (result);
	result =
		mechFile->readIdLongArray("AttackerMoveModifier", AttackerMoveModifier, NUM_MECH_STATES);
	if (result != NO_ERROR)
		return (result);
	result = mechFile->readIdCharArray("HitLocationTable", MechHitLocationTable,
		NUM_MECH_HIT_SECTIONS * NUM_MECH_HIT_ARCS * NUM_MECH_ARMOR_LOCATIONS);
	if (result != NO_ERROR)
		return (result);
	int32_t longData[5 * 2];
	result = mechFile->readIdLongArray("TargetMoveModifierTable", longData, 5 * 2);
	if (result != NO_ERROR)
		return (result);
	for (size_t i = 0; i < 5; i++)
	{
		TargetMoveModifierTable[i][0] = longData[2 * i];
		TargetMoveModifierTable[i][1] = longData[2 * i + 1];
	}
	result = mechFile->seekBlock("Mech:Damage");
	if (result != NO_ERROR)
		return (result);
	result = mechFile->readIdCharArray("CriticalHitTable", CriticalHitTable, 4);
	if (result != NO_ERROR)
		return (result);
	result =
		mechFile->readIdCharArray("MechTransferHitTable", MechTransferHitTable, NUM_BODY_LOCATIONS);
	if (result != NO_ERROR)
		return (result);
	result = mechFile->readIdLong("MechSalvageChance", MechSalvageChance);
	if (result != NO_ERROR)
		return (result);
	result = mechFile->seekBlock("Mech:Collision");
	if (result != NO_ERROR)
		return (result);
	result = mechFile->readIdFloat("collisionThreshold", mechCollisionThreshold);
	if (result != NO_ERROR)
		return (result);
	result = mechFile->readIdFloat("objectThreshold", objectCollisionThreshold);
	if (result != NO_ERROR)
		return (result);
	result = mechFile->readIdFloat("tonnageThreshold", tonnageCollisionThreshold);
	if (result != NO_ERROR)
		return (result);
	result = mechFile->readIdFloat("treeDeflection", treeDeflection);
	if (result != NO_ERROR)
		return (result);
	result = mechFile->readIdFloat("pivotAngle", mechPivotAngle);
	if (result != NO_ERROR)
		return (result);
	result = mechFile->readIdFloat("pivotThrottle", mechPivotThrottle);
	if (result != NO_ERROR)
		return (result);
	return (NO_ERROR);
}

//---------------------------------------------------------------------------

void BattleMechType::init(void)
{
	objectTypeClass = BATTLEMECH_TYPE;
	objectClass = BATTLEMECH;
	anim = nullptr;
	rightArmDebrisId = -1;
	leftArmDebrisId = -1;
	destroyedDebrisId = -1;
	crashAvoidSelf = DefaultMechCrashAvoidSelf;
	crashAvoidPath = DefaultMechCrashAvoidPath;
	crashBlockSelf = DefaultMechCrashBlockSelf;
	crashBlockPath = DefaultMechCrashBlockPath;
	crashYieldTime = DefaultMechCrashYieldTime;
	moveType = MOVETYPE_GROUND;
	explDmg = 0.0;
	explRad = 0.0;
	LOSFactor = 1.0f;
	destructDamage = 0.0f;
}

//--------------------------------------------------------------------------

int32_t
BattleMechType::init(std::unique_ptr<File> objFile, uint32_t fileSize)
{
	int32_t result = 0;
	FitIniFile mechFile;
	result = mechFile.open(objFile, fileSize);
	if (result != NO_ERROR)
		return (result);
	result = mechFile.seekBlock("MechProfile");
	if (result != NO_ERROR)
		return (result);
	wchar_t fileType[512];
	result = mechFile.readIdString("ProfileName", fileType, 511);
	if (result != NO_ERROR)
		return (result);
	result = mechFile.readIdLong("MoveType", moveType);
	if (result != NO_ERROR)
		moveType = MOVETYPE_GROUND;
	result = mechFile.readIdFloat("LOSFactor", LOSFactor);
	if (result != NO_ERROR)
		LOSFactor = 1.0f;
	result = mechFile.readIdFloat("DestructDamage", destructDamage);
	if (result != NO_ERROR)
		destructDamage = 20.0f;
	result = mechFile.readIdFloat("ExplosionDamage", explDmg);
	if (result != NO_ERROR)
		explDmg = 10.0f;
	result = mechFile.readIdFloat("ExplosionRadius", explRad);
	if (result != NO_ERROR)
		explRad = 25.0f;
	//--------------------------------------------------------------
	// New from here on down.  Now gets data from Mech CSV file.
	FullPathFileName mechCSVName;
	mechCSVName.init(objectPath, fileType, ".csv");
	CSVFile mechCSVFile;
	result = mechCSVFile.open(mechCSVName);
	if (result != NO_ERROR)
		return result;
	//----------------------------------------------------
	// No splash damage until designers say they need it.
	// explRad = 0.0;
	// explDmg = 0.0;
	//------------------------------------------------------------------
	// Now, read in the max internal structure for each body location...
	// Reader reads rows from 1 just like spreadsheet.
	// Row 4, Cols 2,4,6,8,10,12,14,16
	for (size_t curLocation = 0; curLocation < NUM_BODY_LOCATIONS; curLocation++)
	{
		result = mechCSVFile.readUCHAR(
			17, stupidNewSageColumns[curLocation], maxInternalStructure[curLocation]);
		if (result != NO_ERROR)
			return (result);
	}
	//-----------------------------------------------------------
	// No more debris.
	rightArmDebrisId = leftArmDebrisId = destroyedDebrisId = -1;
	// Assume Mech Dynamics
	dynamics.setType(DYNAMICS_MECH);
	dynamics.init(&mechCSVFile);
	//------------------------------------------------------------------
	// Initialize the base object Type from the current file.
	result = ObjectType::init(&mechFile);
	return (result);
}

//-----------------------------------------------------------------------------------

void BattleMechType::destroy(void)
{
	ObjectType::destroy();
}

//----------------------------------------------------------------------------------

bool BattleMechType::handleCollision(GameObjectPtr collidee, GameObjectPtr collider)
{
	//---------------------------------------------
	// The default reaction of any object in the world
	// is to simply explode.  This just returns true
	// to facilitate this behaviour.
	//
	// Don't blow us up unless we hit another mech.
	if (MPlayer && !MPlayer->isServer())
		return (false);
	bool sameTeam = false;
	bool collide = true;
	bool collideeJumping = ((BattleMechPtr)collidee)->isJumping();
	bool colliderJumping = false;
	BattleMechPtr mech = (BattleMechPtr)collidee;
	switch (collider->getObjectClass())
	{
	case TRAINCAR:
	{
		if ((collidee->getCollisionFreeFromWID() != collider->getWatchID()) || (collidee->getCollisionFreeTime() < scenarioTime))
			collide = true;
		else
			collide = false;
		if (collide)
		{
			//----------------------------------------------------------------------
			// Don't allow collision between these two again for a period of
			// time...
			collidee->setCollisionFreeFromWID(collider->getWatchID());
			collidee->setCollisionFreeTime(scenarioTime + 2.0);
			//------------------------------------
			// Adjust my velocity and direction...
			// collidee->setVelocity(collider->getVelocity());
			/*
			frame_of_ref workFrame = collidee->getFrame();
			float turnAround = 90.0;
			workFrame.rotate_about_k(turnAround);
			collidee->setFrame(workFrame);
			*/
			collidee->rotate(90.0);
			//--------------------------------------------------------------------------
			// Check to be sure at least one is moving.  If not, move them
			// apart!!! They must be moved at least extentRadius1 +
			// extentRadius2 apart!!
			/*
							vector_3d collideeVel = collidee->getVelocity();
							float collideeSpeed = collideeVel.magnitude();
							if (collideeSpeed <= mechCollisionThreshold)
							{
								float extentRadius1 =
			   collider->getObjectType()->getExtentRadius(); extentRadius1 +=
			   extentRadius;

								//------------------------------
								// Find vector to move collidee
								vector_3d cPosition = collidee->getPosition();
								vector_3d direction = cPosition -
			   collider->getPosition(); direction.normalize(); direction *=
			   extentRadius1; cPosition += direction;
								collidee->setPosition(cPosition);
							}
			*/
			((std::unique_ptr<Mover>)collidee)->bounceToAdjCell();
			//---------------------------------------------
			// Train will take care of administering damage from this
			// collision...
			soundSystem->playDigitalSample(MECH_COLLIDE, collidee->getPosition());
			return (false);
		}
	}
	break;
	case BATTLEMECH:
		if (collidee->getTeam() != collider->getTeam())
		{
			if (((std::unique_ptr<Mover>)collider)->getPilot()->getCurTacOrder()->code == TacticalOrderCode::attackobject)
				((std::unique_ptr<Mover>)collider)
					->getPilot()
					->numPhysicalAttacks[PHYSICAL_ATTACK_RAM][COMBAT_STAT_MISSION]++;
			else if (((std::unique_ptr<Mover>)collider)->getPilot()->getCurTacOrder()->code == TacticalOrderCode::jumptopoint)
			{
				if (mech->getPilot()->getCurTacOrder()->getJumpTarget() == collidee)
					mech->getPilot()
						->numPhysicalAttacks[PHYSICAL_ATTACK_DFA][COMBAT_STAT_MISSION]++;
			}
		}
		colliderJumping = ((BattleMechPtr)collider)->isJumping();
		if (!colliderJumping && (((BattleMechPtr)collider)->lastJumpTime >= 0.0))
		{
			float timeSinceLanding = scenarioTime - ((BattleMechPtr)collider)->lastJumpTime;
			//((BattleMechPtr)collider)->lastJumpTime = -1.0;
			colliderJumping = (timeSinceLanding < 0.5);
		}
	case GROUNDVEHICLE:
	{
		//--------------------------
		// Was a jump involved here?
		if (!collideeJumping && (((BattleMechPtr)collidee)->lastJumpTime >= 0.0))
		{
			float timeSinceLanding = scenarioTime - ((BattleMechPtr)collidee)->lastJumpTime;
			((BattleMechPtr)collidee)->lastJumpTime = -1.0;
			collideeJumping = (timeSinceLanding < 0.5);
		}
		bool jumpCollision = (collideeJumping || colliderJumping);
		//-----------------------------------------------------------------------
		// Typically, same-alignment movers don't collide unless a DFA or RAM
		// was involved...
		if (collidee->isFriendly(collider))
			sameTeam = true;
		if (sameTeam)
			collide = jumpCollision;
		else if (jumpCollision)
			collide = true;
		else
		{
			GameObjectPtr collideeRamTarget =
				((std::unique_ptr<Mover>)collidee)->getPilot()->getCurTacOrder()->getRamTarget();
			GameObjectPtr colliderRamTarget =
				((std::unique_ptr<Mover>)collider)->getPilot()->getCurTacOrder()->getRamTarget();
			collide = ((collideeRamTarget == collider) || (colliderRamTarget == collidee));
		}
		if (collide)
			collide = ((collidee->getCollisionFreeFromWID() != collider->getWatchID()) || (collidee->getCollisionFreeTime() < scenarioTime));
		if (collide)
		{
			//----------------------------------------------------------------------
			// Don't allow collision between these two again for a period of
			// time...
			collidee->setCollisionFreeFromWID(collider->getWatchID());
			collidee->setCollisionFreeTime(scenarioTime + 2.0);
			collidee->rotate(90.0);
			//--------------------------------------------------------------------------
			// Check to be sure at least one is moving.  If not, move them
			// apart!!! They must be moved at least extentRadius1 +
			// extentRadius2 apart!!
			/*
							vector_3d collideeVel = collidee->getVelocity();
							float collideeSpeed = collideeVel.magnitude();
							if (collideeSpeed <= mechCollisionThreshold)
							{
								float extentRadius1 =
			   collider->getObjectType()->getExtentRadius(); extentRadius1 +=
			   extentRadius;

								//------------------------------
								// Find vector to move collidee
								vector_3d cPosition = collidee->getPosition();
								vector_3d direction = cPosition -
			   collider->getPosition(); direction.normalize(); direction *=
			   extentRadius1; cPosition += direction;
								collidee->setPosition(cPosition);
							}
			*/
			//---------------------------------------------
			// Administer the damage from this collision...
			if (jumpCollision)
			{
				//-----------------------------------------------------------
				// Since we reset the jumping mech's lastJumpTime right here,
				// we should apply the damage to the collider here, as well.
				// If both are jumping...well then, double damage is taken!
				if (collideeJumping && !sameTeam)
				{
					WeaponShotInfo shotInfo;
					//-----------------------
					// First, the collidee...
					// I jumped on the below, therefore, my legs should be
					// preferentially damaged!!!
					int32_t hitLocation =
						collidee->calcHitLocation(collider, -1, ATTACKSOURCE_MINE, 0);
					if (sameTeam)
						shotInfo.init(collider->getWatchID(), -1,
							collider->getTonnage() / 100.0 + 0.5, hitLocation,
							collidee->relFacingTo(collider->getPosition()));
					else
						shotInfo.init(collider->getWatchID(), -1,
							collider->getTonnage() / 10.0 + 0.5, hitLocation,
							collidee->relFacingTo(collider->getPosition()));
					collidee->handleWeaponHit(&shotInfo, (MPlayer != nullptr));
					//----------------------
					// Next, the collider...
					hitLocation = collider->calcHitLocation(collidee, -1, ATTACKSOURCE_DFA, 0);
					if (sameTeam)
						shotInfo.init(collidee->getWatchID(), -1,
							collider->getTonnage() / 100.0 + 0.5, hitLocation,
							collider->relFacingTo(collidee->getPosition()));
					else
						shotInfo.init(collidee->getWatchID(), -1,
							collider->getTonnage() / 10.0 + 0.5, hitLocation,
							collider->relFacingTo(collidee->getPosition()));
					collider->handleWeaponHit(&shotInfo, (MPlayer != nullptr));
					Stuff::Vector3D tmpPosition = collider->getPosition();
					ObjectManager->createExplosion(DFA_BOOM_EFFECT, nullptr, tmpPosition);
				}
			}
			else
			{
				if (!sameTeam)
				{
					int32_t hitLocation =
						collidee->calcHitLocation(collider, -1, ATTACKSOURCE_COLLISION, 0);
					WeaponShotInfo shotInfo;
					shotInfo.init(collider->getWatchID(), -1, collider->getTonnage() / 10.0 + 0.5,
						hitLocation, collidee->relFacingTo(collider->getPosition()));
					collidee->handleWeaponHit(&shotInfo, (MPlayer != nullptr));
					collidee->handleWeaponHit(&shotInfo, (MPlayer != nullptr));
				}
			}
			int32_t bounceDir = ((std::unique_ptr<Mover>)collidee)->bounceToAdjCell();
			if (jumpCollision && (bounceDir == -1))
			{
				//----------------------
				// Nowhere to bounce to!
			}
			//---------------------------------------------
			if (!sameTeam && (collidee->getWindowsVisible() == turn))
				soundSystem->playDigitalSample(MECH_COLLIDE, collidee->getPosition());
			return (false);
		}
	}
	break;
	case BUILDING:
	case TREEBUILDING:
	{
		// Should Never collide with Buildings.  Only Steppables which ARE not
		// this king of building!!
	}
	break;
	case TREE:
	case TERRAINOBJECT:
	{
		if ((collidee->getCollisionFreeFromWID() != collider->getWatchID()) || (collidee->getCollisionFreeTime() < scenarioTime))
		{
			//----------------------------------------------------------------------
			// Don't allow collision between these two again for a period of
			// time...
			collidee->setCollisionFreeFromWID(collider->getWatchID());
			collidee->setCollisionFreeTime(scenarioTime + 2.0);
			//------------------------------------
			// Adjust my velocity and direction...
			// collidee->setVelocity(collider->getVelocity());
			float turnAround = 0.0;
			float tonnageClass = collidee->getTonnage();
			if (tonnageClass < tonnageCollisionThreshold)
				turnAround = (tonnageCollisionThreshold / tonnageClass) * treeDeflection;
			if (turnAround > 0.0)
				collidee->rotate(turnAround);
			//---------------------------------------------
			if (collidee->getWindowsVisible() == turn)
				soundSystem->playDigitalSample(MECH_COLLIDE, collidee->getPosition());
			return (false);
		}
	}
	break;
	}
	return (false);
}

//-----------------------------------------------------------------------------------

bool BattleMechType::handleDestruction(GameObjectPtr collidee, GameObjectPtr collider)
{
	//-------------------------------------------------------------------
	// BUG ALERT!!!!!!!!!
	//-------------------------------------------------------------------
	// This could get called multiple times for a mech before it's
	// truly destroyed (if multiple weapon hits are taken in an update).
	// This should be prevented by checking if the mech is already
	// destroyed. BUT, for fear that this could break something this late
	// in the game (4/4/98), it would be best to address the symptoms
	// for now (e.g. # of kills in handleTargetKilled). After release
	// (or demo), fix this and seriously test it doesn't "break"
	// anything!!!!!!!!!!! Should also be fixed for other objects'
	// handleDestruction().
	//-------------------------------------------------------------------
	//-------------------------------------------------------
	// For now, a BattleMech will play a default explosion
	// and disappear after the explosion is half way through
	BattleMechPtr mech = (BattleMechPtr)collidee;
	if (!mech->getPilot())
		Fatal(0, " No Pilot in this mech! ");
	//--------------------------------------------------------
	// Let's let the unit know we're dying if we're a point...
	if (mech->getPoint() == mech)
	{
		mech->getGroup()->setPoint(nullptr);
		//--------------------------------------------------------
		// If there is no new point, all units must be blown away.
		// How do we want to handle this?
	}
	//-----------------------------
	// Immediately lose contacts...
	if (mech->sensorSystem)
		mech->sensorSystem->disable();
	if (mech->withdrawing)
	{
		mech->timeLeft = 0.8f;
		//-----------------------------------------------
		// Let the pilot know we are about to withdraw...
		mech->getPilot()->handleAlarm(PILOT_ALARM_VEHICLE_WITHDRAWN, 0);
	}
	else
	{
		if (!mech->startDisabled)
			mech->timeLeft = 0.8f;
		//------------------------------------------
		// Let the pilot know we are about to die...
		mech->getPilot()->handleAlarm(
			PILOT_ALARM_VEHICLE_DESTROYED, collider ? collider->getWatchID() : 0);
		// ONLY the server may DESTROY a mech.  All other clients must await the
		// statusChunk!!
		if (!MPlayer || (MPlayer && MPlayer->isServer()))
			mech->setStatus(OBJECT_STATUS_DESTROYED);
		mech->fallen = false;
		mech->exploding = false;
		for (size_t i = 0; i < mech->numBodyLocations;
			 i++) // Blow all of the components.  NO SALVAGE!!
			mech->destroyBodyLocation(i);
#ifdef USE_MOODMUSIC
		//------------------------------------
		// What heroic music should be played?
		if (collidee->getAlignment() == Team::home->getAlignment())
			friendlydestroyed = true;
		else
			enemydestroyed = true;
#endif
		if (CombatLog)
		{
			wchar_t s[1024];
			sprintf(s, "[%.2f] mech.destroyed HD: [%05d]%s", scenarioTime, mech->getPartId(),
				mech->getName());
			CombatLog->write(s);
			CombatLog->write(" ");
		}
	}
	return (true);
}

//----------------------------------------------------------------------------------

GameObjectPtr
BattleMechType::createInstance(void)
{
	BattleMechPtr newMech = new BattleMech;
	if (!newMech)
		return (nullptr);
	newMech->init(true, this);
	// newMech->setIdNumber(NextIdNumber++);
	return newMech;
}

//**********************************************************************************
// BATTLEMECH class
//**********************************************************************************

void BattleMech::operator=(BattleMech copy)
{
}

//----------------------------------------------------------------------------------

void BattleMech::handleStaticCollision(void)
{
	bool endOfJump = false;
	if (getTangible() || endOfJump)
	{
		//-----------------------------------------------------
		// What is our block and vertex number?
		int32_t blockNumber = 0;
		int32_t vertexNumber = 0;
		getBlockAndVertexNumber(blockNumber, vertexNumber);
		int32_t numCollidables = ObjectManager->getObjBlockNumCollidables(blockNumber);
		int32_t terrainObjHandle = ObjectManager->getObjBlockFirstHandle(blockNumber);
		int32_t colliderBlockNumber = -1;
		int32_t colliderVertexNumber = -1;
		for (size_t i = 0; i < numCollidables; i++)
		{
			GameObjectPtr terrainObj = ObjectManager->get(terrainObjHandle + i);
			bool isTangible = false;
			switch (terrainObj->getObjectClass())
			{
			case TREE:
			case TREEBUILDING:
			case TERRAINOBJECT:
			case BUILDING:
				terrainObj->getBlockAndVertexNumber(colliderBlockNumber, colliderVertexNumber);
				isTangible = terrainObj->getTangible();
				break;
			case CAMERADRONE:
			case MINE:
				break;
			}
			if (isTangible)
				ObjectManager->detectStaticCollision(this, terrainObj);
		}
	}
}

//---------------------------------------------------------------------------

void BattleMech::init(bool create)
{
	// Mover::init(create);
	objectClass = BATTLEMECH;
	overlayWeightClass = OVERLAY_WEIGHT_MECH;
	cBills = 0;
	playedHeloDestruct = false;
	numBodyLocations = NUM_MECH_BODY_LOCATIONS;
	for (size_t i = 0; i < numBodyLocations; i++)
	{
		body[i].CASE = false;
		body[i].totalSpaces = 0;
		body[i].curInternalStructure = 0.0;
		body[i].hotSpotNumber = 0;
		body[i].maxInternalStructure = 0;
		body[i].damageState = IS_DAMAGE_NONE;
	}
	numArmorLocations = NUM_MECH_ARMOR_LOCATIONS;
#ifdef USEHEAT
	heatDissipation = 0.0;
	heatCheckTime = 0.0;
#endif
	chassisClass = MECH_CLASS_LIGHT;
	chassisBR = 0;
	damageThisFrame = 0.0f;
	numArmorComponents = 0;
	playedCriticalHit = false;
	legStatus = LEG_STATUS_NORMAL;
	torsoStatus = TORSO_STATUS_NORMAL;
	sentCrippledMsg = false;
	for (i = 0; i < NUM_ACTUATORS; i++)
		actuator[i] = 255;
	gyro = 255;
	numJumpJets = 0;
	lastJumpTime = -100.0;
	inJump = false;
	jumpGoal.Zero();
	centerTorsoBlowTime = -1.0;
	hitFromBehindThisFrame = false;
	hitFromFrontThisFrame = false;
	leftArmBlownThisFrame = false;
	rightArmBlownThisFrame = false;
	torsoRotation = 0;
	leftArmRotation = 0;
	rightArmRotation = 0;
	fallen = false;
	mechRemoved = false;
	pivotTurnRate = 0.0;
#ifdef USE_JETS
	jumpJets[0] = nullptr;
	jumpJets[1] = nullptr;
#endif
	maxWeaponDamage = 0.0;
	longName[0] = nullptr;
	pilotNum = 0;
	captureable = false;
	notMineYet = true;
	descID = -1;
	//	variant = 0;
	exploding = false;
	blipFrame = 0;
	drawFlags = 0;
	lowestWeaponNodeID = -2;
	damageAfterDisabled = 0.0f;
	for (size_t l = 0; l < 6; l++)
		rotateValues[l] = 0;
#ifdef USE_MECHDEBUGFILE
	if (!MechDebugFile)
	{
		MechDebugFile = UserFile::getNewFile();
		if (!MechDebugFile)
			Fatal(0, " Couldn't create Mech Debug File ");
		int32_t err = MechDebugFile->open("mech.dbg");
		if (err)
			Fatal(0, " Couldn't open Mech Debug File ");
		MechDebugFile->write("Name           Target Turn Rot   RelFacing "
							 "torsoRot newRot torsoRelFacing turnRate");
	}
#endif
}

//----------------------------------------------------------------------------------

void BattleMech::clear(void)
{
}

//----------------------------------------------------------------------------------

void BattleMech::init(bool create, ObjectTypePtr _type)
{
	// Call down the chain to init everybody else.
	GameObject::init(create, _type);
	creationTime = scenarioTime;
	//-------------------------------------------------------------
	// In here will reside the code which initializes all of the
	// variables for a battleMech based upon the type.
	// For starters, these are tangible objects
	setTangible(true);
	//-------------------------------------------------
	// Set some mech traits based upon the Mech Type...
	BattleMechTypePtr mechT = (BattleMechTypePtr)_type;
	for (size_t curLocation = 0; curLocation < NUM_BODY_LOCATIONS; curLocation++)
		body[curLocation].maxInternalStructure = mechT->maxInternalStructure[curLocation];
	crashAvoidSelf = mechT->crashAvoidSelf;
	crashAvoidPath = mechT->crashAvoidPath;
	crashBlockSelf = mechT->crashBlockSelf;
	crashBlockPath = mechT->crashBlockPath;
	crashYieldTime = mechT->crashYieldTime;
	setMoveType(mechT->moveType);
	control.init();
	dynamics = ((BattleMechTypePtr)_type)->dynamics;
	//-------------------------------------------------------------
	// The appearance is initialized here using data from the type
	std::wstring_view appearanceName = _type->getAppearanceTypeName();
	//--------------------------------------------------------------
	// New code!!!
	// We need to append the sprite type to the appearance num now.
	// The MechEdit tool does not assume a sprite type, nor should it.
	// MechCmdr2 features much simpler objects which only use 1 type of sprite!
	int32_t appearanceType = (MECH_TYPE << 24);
	AppearanceTypePtr mechAppearanceType =
		appearanceTypeList->getAppearance(appearanceType, appearanceName);
	if (!mechAppearanceType)
	{
		//------------------------------------------------
		// This mech probably is not done.  Use the MADCAT
		mechAppearanceType = appearanceTypeList->getAppearance(appearanceType, "MADCAT");
	}
	//------------------------------------------------------------
	// Ultimately, try to re-use rather then keep re-allocating...
	if (appearance)
		delete appearance;
	appearance = new Mech3DAppearance;
	gosASSERT(appearance != nullptr);
	//--------------------------------------------------------------
	// The only kind of appearanceType the mechs currently know how
	// to work with is a spriteTree.  Anything else is wrong.
	appearance->init((Mech3DAppearanceTypePtr)mechAppearanceType, this);
	appearance->initFX();
	appearance->setAlphaValue(alphaValue);
	objectClass = BATTLEMECH;
	setObscured(true);
	markDistanceMoved = 1000.0;
}

//----------------------------------------------------------------------------------

float BattleMech::getStatusRating(void)
{
	//-------------------------------
	// calculate Weapon effectiveness
	float weaponEffect = (float)weaponEffectiveness / (float)maxWeaponEffectiveness;
	//----------------
	// Calculate armor
	float armorHead =
		armor[MECH_ARMOR_LOCATION_HEAD].curArmor / armor[MECH_ARMOR_LOCATION_HEAD].maxArmor * 0.6 + 0.4;
	float armorCTorso = armor[MECH_ARMOR_LOCATION_CTORSO].curArmor / armor[MECH_ARMOR_LOCATION_CTORSO].maxArmor * 0.5 + 0.5;
	if (armor[MECH_ARMOR_LOCATION_CTORSO].curArmor > armor[MECH_ARMOR_LOCATION_RCTORSO].curArmor)
		armorCTorso = armor[MECH_ARMOR_LOCATION_RCTORSO].curArmor / armor[MECH_ARMOR_LOCATION_RCTORSO].maxArmor * 0.5 + 0.5;
	float curTorso = armor[MECH_ARMOR_LOCATION_RLTORSO].curArmor + armor[MECH_ARMOR_LOCATION_LTORSO].curArmor + armor[MECH_ARMOR_LOCATION_RRTORSO].curArmor + armor[MECH_ARMOR_LOCATION_RTORSO].curArmor;
	float maxTorso = armor[MECH_ARMOR_LOCATION_RLTORSO].maxArmor + armor[MECH_ARMOR_LOCATION_LTORSO].maxArmor + armor[MECH_ARMOR_LOCATION_RRTORSO].maxArmor + armor[MECH_ARMOR_LOCATION_RTORSO].maxArmor;
	float armorTorso = curTorso / maxTorso * 0.25 + 0.75;
	float curArm =
		armor[MECH_ARMOR_LOCATION_LARM].curArmor + armor[MECH_ARMOR_LOCATION_RARM].curArmor;
	float maxArm =
		armor[MECH_ARMOR_LOCATION_LARM].maxArmor + armor[MECH_ARMOR_LOCATION_RARM].maxArmor;
	float armorArm = curArm / maxArm * 0.25 + 0.75;
	float curLeg =
		armor[MECH_ARMOR_LOCATION_LLEG].curArmor + armor[MECH_ARMOR_LOCATION_RLEG].curArmor;
	float maxLeg =
		armor[MECH_ARMOR_LOCATION_LLEG].maxArmor + armor[MECH_ARMOR_LOCATION_RLEG].maxArmor;
	float armorLeg = curLeg / maxLeg * 0.25 + 0.75;
	float armorEffect = armorHead * armorCTorso * armorTorso * armorArm * armorLeg;
	//-----------------------
	// Calculate pilot Wounds
	float pilotWoundTable[7] = {1.00f, 0.95f, 0.85f, 0.75f, 0.50f, 0.30f, 0.00f};
	float pilotEffect = pilotWoundTable[(int32_t)getPilot()->getWounds()];
	if (isDestroyed() || isDisabled())
		pilotEffect = 0.0;
	float rating = weaponEffect * armorEffect * pilotEffect;
	return (rating);
}

//----------------------------------------------------------------------------------

void BattleMech::setControl(ControlType ctrlType)
{
	control.init(ctrlType, CONTROL_DATA_MECH);
}

//----------------------------------------------------------------------------------
int32_t
BattleMech::init(uint32_t variantNum)
{
	//----------------------------------------------
	// Open the mech's CSV file.
	CSVFile* mechFile = new CSVFile;
	FullPathFileName csvName;
	csvName.init(objectPath, getObjectType()->getAppearanceTypeName(), ".csv");
	variantID = variantNum;
	int32_t result = mechFile->open(csvName);
	if (result != NO_ERROR)
	{
		delete mechFile;
		mechFile = nullptr;
		return result;
	}
	//--------------------------------------------------------------------
	// CSV file open.  Variant Num controls offsets to data in the file.
	// Same rules as before,  Rows start at one.  Columns start at zero.
	wchar_t thisMechName[128];
	mechFile->readString(3, 2, thisMechName, 127);
	strncpy(name, thisMechName, MAXLEN_MOVER_NAME - 1);
	name[MAXLEN_MOVER_NAME] = nullptr;
	mechFile->readLong(10, 5, chassisBR);
	mechFile->readFloat(3, 5, tonnage);
	mechFile->readLong(5, 2, descID);
	cLoadString(descID, thisMechName, 127);
	wchar_t tmp[256];
	cLoadString(IDS_MFDMCH_PRINTSTRING, tmp, 256);
	sprintf(longName, tmp, thisMechName, tonnage);
	mechFile->readLong(12, 2, iconPictureIndex);
	uint8_t speed;
	mechFile->readUCHAR(7, 5, speed);
	maxMoveSpeed = (float)speed;
	result = mechFile->readString(23 + (97 * variantNum), 2, variantName, 63);
	if (result == 1)
	{
		PAUSE(("Variant Number %d does not exist for mech %s.  Please set "
			   "variant in editor!",
			variantNum, csvName));
		variantNum = 0;
		variantID = 0;
		result = mechFile->readString(23 + (97 * variantNum), 2, variantName, 63);
		if (result == 1)
			STOP(("Can't even find Variant 0 for mech %s", csvName));
	}
	status = 0;
	mechFile->readUCHAR(18, 2, armor[MECH_ARMOR_LOCATION_HEAD].maxArmor);
	mechFile->readUCHAR(18, 7, armor[MECH_ARMOR_LOCATION_CTORSO].maxArmor);
	mechFile->readUCHAR(18, 5, armor[MECH_ARMOR_LOCATION_LTORSO].maxArmor);
	mechFile->readUCHAR(18, 6, armor[MECH_ARMOR_LOCATION_RTORSO].maxArmor);
	mechFile->readUCHAR(18, 3, armor[MECH_ARMOR_LOCATION_LARM].maxArmor);
	mechFile->readUCHAR(18, 4, armor[MECH_ARMOR_LOCATION_RARM].maxArmor);
	mechFile->readUCHAR(18, 8, armor[MECH_ARMOR_LOCATION_LLEG].maxArmor);
	mechFile->readUCHAR(18, 9, armor[MECH_ARMOR_LOCATION_RLEG].maxArmor);
	mechFile->readUCHAR(18, 12, armor[MECH_ARMOR_LOCATION_RCTORSO].maxArmor);
	mechFile->readUCHAR(18, 10, armor[MECH_ARMOR_LOCATION_RLTORSO].maxArmor);
	mechFile->readUCHAR(18, 11, armor[MECH_ARMOR_LOCATION_RRTORSO].maxArmor);
	uint8_t currentArmor;
	mechFile->readUCHAR(115 + (97 * variantNum), 2, currentArmor);
	armor[MECH_ARMOR_LOCATION_HEAD].curArmor = currentArmor;
	mechFile->readUCHAR(115 + (97 * variantNum), 7, currentArmor);
	armor[MECH_ARMOR_LOCATION_CTORSO].curArmor = currentArmor;
	mechFile->readUCHAR(115 + (97 * variantNum), 5, currentArmor);
	armor[MECH_ARMOR_LOCATION_LTORSO].curArmor = currentArmor;
	mechFile->readUCHAR(115 + (97 * variantNum), 6, currentArmor);
	armor[MECH_ARMOR_LOCATION_RTORSO].curArmor = currentArmor;
	mechFile->readUCHAR(115 + (97 * variantNum), 3, currentArmor);
	armor[MECH_ARMOR_LOCATION_LARM].curArmor = currentArmor;
	mechFile->readUCHAR(115 + (97 * variantNum), 4, currentArmor);
	armor[MECH_ARMOR_LOCATION_RARM].curArmor = currentArmor;
	mechFile->readUCHAR(115 + (97 * variantNum), 8, currentArmor);
	armor[MECH_ARMOR_LOCATION_LLEG].curArmor = currentArmor;
	mechFile->readUCHAR(115 + (97 * variantNum), 9, currentArmor);
	armor[MECH_ARMOR_LOCATION_RLEG].curArmor = currentArmor;
	mechFile->readUCHAR(115 + (97 * variantNum), 12, currentArmor);
	armor[MECH_ARMOR_LOCATION_RCTORSO].curArmor = currentArmor;
	mechFile->readUCHAR(115 + (97 * variantNum), 10, currentArmor);
	armor[MECH_ARMOR_LOCATION_RLTORSO].curArmor = currentArmor;
	mechFile->readUCHAR(115 + (97 * variantNum), 11, currentArmor);
	armor[MECH_ARMOR_LOCATION_RRTORSO].curArmor = currentArmor;
	//---------------------------------------------------------------------------
	// Build the mech's inventory (all components, and where they are
	// located)... Check through all 30 possible.  Create the numOther,
	// numWeapons and numAmmo from that. Sanity check what the designers have
	// input.
	numAntiMissileSystems = numOther = numWeapons = numAmmos = 0;
	numArmorComponents = 0;
	//-----------------------------------------------------
	// Read in the mech's non-weapon/non-ammo components...
	int32_t realItemNum = 0;
	memset(ItemLocationToInvLocation, 0xff, sizeof(int32_t) * MAX_MOVER_INVENTORY_ITEMS);
	// Read in everything but weapons and AMMO
	for (size_t curItem = 0; curItem < MAX_MOVER_INVENTORY_ITEMS; curItem++)
	{
		mechFile->readUCHAR((26 + (97 * variantNum)) + curItem, 5, inventory[realItemNum].masterID);
		if (inventory[realItemNum].masterID && (inventory[realItemNum].masterID != 255))
		{
			//---------------------------------------------------------------
			// If the component is a JumpJet, increment our jump jet count...
			switch (MasterComponent::masterList[inventory[realItemNum].masterID].getForm())
			{
			case COMPONENT_FORM_JUMPJET:
				numJumpJets = 5;
			case COMPONENT_FORM_COCKPIT:
			case COMPONENT_FORM_SENSOR:
			case COMPONENT_FORM_LIFESUPPORT:
			case COMPONENT_FORM_GYROSCOPE:
			case COMPONENT_FORM_POWER_AMPLIFIER:
			case COMPONENT_FORM_ECM:
			case COMPONENT_FORM_PROBE:
			case COMPONENT_FORM_JAMMER:
			case COMPONENT_FORM_CASE:
			case COMPONENT_FORM_ACTUATOR:
			case COMPONENT_FORM_ENGINE:
			case COMPONENT_FORM_HEATSINK:
				numOther++;
				inventory[realItemNum].health =
					MasterComponent::masterList[inventory[realItemNum].masterID].getHealth();
				inventory[realItemNum].disabled = false;
				inventory[realItemNum].amount = 1;
				inventory[realItemNum].ammoIndex = -1;
				inventory[realItemNum].readyTime = 0.0;
				inventory[realItemNum].bodyLocation = 255;
				ItemLocationToInvLocation[curItem] = realItemNum;
				realItemNum++;
				break;
			case COMPONENT_FORM_BULK: // Special case now.
				// This is an armor component.
				// DO NOT store with other components
				// It becomes a free hit then!!
				numArmorComponents++;
				break;
			case COMPONENT_FORM_WEAPON:
			case COMPONENT_FORM_WEAPON_ENERGY:
			case COMPONENT_FORM_WEAPON_BALLISTIC:
			case COMPONENT_FORM_WEAPON_MISSILE:
				// Ignore weapons until second pass
				break;
			case COMPONENT_FORM_AMMO:
				// Ignore AMMO until third pass
				break;
			}
		}
	}
	// NOW read in the weapons.
	for (curItem = 0; curItem < MAX_MOVER_INVENTORY_ITEMS; curItem++)
	{
		mechFile->readUCHAR((26 + (97 * variantNum)) + curItem, 5, inventory[realItemNum].masterID);
		if (inventory[realItemNum].masterID && (inventory[realItemNum].masterID != 255))
		{
			//---------------------------------------------------------------
			// If the component is a JumpJet, increment our jump jet count...
			int32_t sourceWeaponType;
			if (MasterComponent::masterList[inventory[realItemNum].masterID].getForm() == COMPONENT_FORM_WEAPON_MISSILE)
				sourceWeaponType = MECH3D_WEAPONTYPE_MISSILE;
			else if (MasterComponent::masterList[inventory[realItemNum].masterID].getForm() == COMPONENT_FORM_WEAPON_BALLISTIC)
				sourceWeaponType = MECH3D_WEAPONTYPE_BALLISTIC;
			else if (MasterComponent::masterList[inventory[realItemNum].masterID].getForm() == COMPONENT_FORM_WEAPON_ENERGY)
				sourceWeaponType = MECH3D_WEAPONTYPE_ENERGY;
			else
				sourceWeaponType = MECH3D_WEAPONTYPE_ANY;
			switch (MasterComponent::masterList[inventory[realItemNum].masterID].getForm())
			{
			case COMPONENT_FORM_JUMPJET:
			case COMPONENT_FORM_COCKPIT:
			case COMPONENT_FORM_SENSOR:
			case COMPONENT_FORM_LIFESUPPORT:
			case COMPONENT_FORM_GYROSCOPE:
			case COMPONENT_FORM_POWER_AMPLIFIER:
			case COMPONENT_FORM_ECM:
			case COMPONENT_FORM_PROBE:
			case COMPONENT_FORM_JAMMER:
			case COMPONENT_FORM_BULK:
			case COMPONENT_FORM_CASE:
			case COMPONENT_FORM_ACTUATOR:
			case COMPONENT_FORM_ENGINE:
			case COMPONENT_FORM_HEATSINK:
				// We did these Above here.
				break;
			case COMPONENT_FORM_WEAPON:
			case COMPONENT_FORM_WEAPON_ENERGY:
			case COMPONENT_FORM_WEAPON_BALLISTIC:
			case COMPONENT_FORM_WEAPON_MISSILE:
				numWeapons++;
				inventory[realItemNum].health =
					MasterComponent::masterList[inventory[realItemNum].masterID].getHealth();
				inventory[realItemNum].disabled = false;
				inventory[realItemNum].amount = 1;
				inventory[realItemNum].ammoIndex = -1;
				inventory[realItemNum].facing = appearance->getWeaponNode(sourceWeaponType);
				inventory[realItemNum].readyTime = 0.0;
				inventory[realItemNum].bodyLocation = 255;
				inventory[realItemNum].effectiveness = (int16_t)(MasterComponent::masterList[inventory[realItemNum].masterID].getWeaponDamage() * 10.0 / // damage over 10 seconds
					MasterComponent::masterList[inventory[realItemNum].masterID]
						.getWeaponRecycleTime());
				inventory[realItemNum].effectiveness *=
					WeaponRanges[MasterComponent::masterList[inventory[realItemNum].masterID]
									 .getWeaponRange()][1]
					/ 24;
				ItemLocationToInvLocation[curItem] = realItemNum;
				realItemNum++;
				//-------------------------------------
				// Cache in the weapon special effect.
				break;
			case COMPONENT_FORM_AMMO:
				// Do these next.
				break;
			}
		}
	}
	// NOW read in the AMMO
	for (curItem = 0; curItem < MAX_MOVER_INVENTORY_ITEMS; curItem++)
	{
		mechFile->readUCHAR((26 + (97 * variantNum)) + curItem, 5, inventory[realItemNum].masterID);
		if (inventory[realItemNum].masterID && (inventory[realItemNum].masterID != 255))
		{
			//---------------------------------------------------------------
			// If the component is a JumpJet, increment our jump jet count...
			switch (MasterComponent::masterList[inventory[realItemNum].masterID].getForm())
			{
			case COMPONENT_FORM_JUMPJET:
			case COMPONENT_FORM_COCKPIT:
			case COMPONENT_FORM_SENSOR:
			case COMPONENT_FORM_LIFESUPPORT:
			case COMPONENT_FORM_GYROSCOPE:
			case COMPONENT_FORM_POWER_AMPLIFIER:
			case COMPONENT_FORM_ECM:
			case COMPONENT_FORM_PROBE:
			case COMPONENT_FORM_JAMMER:
			case COMPONENT_FORM_BULK:
			case COMPONENT_FORM_CASE:
			case COMPONENT_FORM_ACTUATOR:
			case COMPONENT_FORM_ENGINE:
			case COMPONENT_FORM_HEATSINK:
				// Did these above.
				break;
			case COMPONENT_FORM_WEAPON:
			case COMPONENT_FORM_WEAPON_ENERGY:
			case COMPONENT_FORM_WEAPON_BALLISTIC:
			case COMPONENT_FORM_WEAPON_MISSILE:
				// Did These Above, too...
				break;
			case COMPONENT_FORM_AMMO:
				numAmmos++;
				inventory[realItemNum].health =
					MasterComponent::masterList[inventory[realItemNum].masterID].getHealth();
				inventory[realItemNum].disabled = false;
				inventory[realItemNum].amount = 1;
				inventory[realItemNum].ammoIndex = -1;
				inventory[realItemNum].readyTime = 0.0;
				inventory[realItemNum].bodyLocation = 255;
				//-----------------------------------------------------------------------
				// Since the ammo amount in the profile is amount per ton, let's
				// make the amount equal to the number of
				// missiles/bullets/whatever. If it's set to 255, use the
				// default amount per ton as defined in the component table...
				//
				// Ammo per ton is always fully stocked.  I.e. always -1 or 255
				inventory[realItemNum].amount =
					MasterComponent::masterList[inventory[realItemNum].masterID].getAmmoPerTon();
				inventory[realItemNum].ammoIndex = -1;
				inventory[realItemNum].startAmount = inventory[realItemNum].amount;
				inventory[realItemNum].health =
					MasterComponent::masterList[inventory[realItemNum].masterID].getHealth();
				inventory[realItemNum].disabled = false;
				inventory[realItemNum].readyTime = 0.0;
				inventory[realItemNum].bodyLocation = 255;
				ItemLocationToInvLocation[curItem] = realItemNum;
				realItemNum++;
				break;
			}
		}
	}
	//------------------------------------------------------------
	// Now, read in the component layout for each body location...
	for (size_t curLocation = 0; curLocation < NUM_MECH_BODY_LOCATIONS; curLocation++)
	{
		body[curLocation].CASE = true; // ALL MC2 Mechs have CASE everywhere.
		uint8_t internalStructure;
		mechFile->readUCHAR(
			114 + (97 * variantNum), stupidNewSageColumns[curLocation], internalStructure);
		body[curLocation].curInternalStructure = internalStructure;
		//---------------------------------------------------------
		// Now, determine the damage state for the body location...
		float damageLevel =
			(float)body[curLocation].curInternalStructure / body[curLocation].maxInternalStructure;
		if (damageLevel <= 0.0)
			body[curLocation].damageState = IS_DAMAGE_DESTROYED;
		else if (damageLevel <= 0.5)
			body[curLocation].damageState = IS_DAMAGE_PARTIAL;
		else
			body[curLocation].damageState = IS_DAMAGE_NONE;
		int32_t numSpaces = NumLocationCriticalSpaces[curLocation];
		body[curLocation].totalSpaces = 0;
		for (size_t curSpace = 0; curSpace < numSpaces; curSpace++)
		{
			uint8_t spaceData;
			mechFile->readUCHAR(
				(102 + (97 * variantNum)) + curSpace, stupidNewSageColumns[curLocation], spaceData);
			spaceData = ItemLocationToInvLocation[spaceData];
			body[curLocation].criticalSpaces[curSpace].inventoryID = spaceData;
			body[curLocation].criticalSpaces[curSpace].hit =
				false; // Everything always repaired now.
			if (spaceData < 255)
			{
				if (spaceData >= numOther + numWeapons + numAmmos)
					// Just ignore this space.  Sage has filled them all in now.
					// ACTUALLY, we should stop here.
					continue;
				inventory[spaceData].bodyLocation = curLocation;
				//--------------------------------------------------------
				// The following line assumes the "new" crit hit system...
				body[curLocation].totalSpaces +=
					MasterComponent::masterList[inventory[spaceData].masterID].getSize();
				//--------------------------------------------------------
				// Preserve critical component indices for quick access...
				switch (MasterComponent::masterList[inventory[spaceData].masterID].getForm())
				{
				case COMPONENT_FORM_COCKPIT:
					cockpit = spaceData;
					break;
				case COMPONENT_FORM_JUMPJET:
					jumpJets = spaceData;
					break;
				case COMPONENT_FORM_SENSOR:
					sensor = spaceData;
					sensorSystem = SensorManager->newSensor();
					sensorSystem->setOwner(this);
					sensorSystem->setRange(
						MasterComponent::masterList[inventory[sensor].masterID].getSensorRange());
					break;
				case COMPONENT_FORM_HEATSINK:
					inventory[spaceData].bodyLocation = curLocation;
					break;
				case COMPONENT_FORM_AMMO:
					inventory[spaceData].bodyLocation = curLocation;
					break;
				case COMPONENT_FORM_WEAPON:
				case COMPONENT_FORM_WEAPON_ENERGY:
					inventory[spaceData].bodyLocation = curLocation;
					break;
				case COMPONENT_FORM_WEAPON_BALLISTIC:
					inventory[spaceData].bodyLocation = curLocation;
					if ((inventory[spaceData].masterID == MasterComponent::clanAntiMissileSystemID) || (inventory[spaceData].masterID == MasterComponent::innerSphereAntiMissileSystemID))
					{
						//------------------------------------------------------
						// Add to Anti-Missile System list for fast reference...
						if (numAntiMissileSystems == MAX_ANTI_MISSILE_SYSTEMS)
							Fatal(0, "Too many Anti-Missile Systems");
						antiMissileSystem[numAntiMissileSystems++] = spaceData;
					}
					break;
				case COMPONENT_FORM_WEAPON_MISSILE:
					inventory[spaceData].bodyLocation = curLocation;
					break;
				case COMPONENT_FORM_ACTUATOR:
					if (inventory[spaceData].masterID == MasterComponent::armActuatorID)
					{
						if (curLocation == MECH_BODY_LOCATION_LARM)
							actuator[ACTUATOR_LSHOULDER] = spaceData;
						else if (curLocation == MECH_BODY_LOCATION_RARM)
							actuator[ACTUATOR_RSHOULDER] = spaceData;
					}
					else if (inventory[spaceData].masterID == MasterComponent::legActuatorID)
					{
						if (curLocation == MECH_BODY_LOCATION_LLEG)
							actuator[ACTUATOR_LHIP] = spaceData;
						else if (curLocation == MECH_BODY_LOCATION_RLEG)
							actuator[ACTUATOR_RHIP] = spaceData;
					}
					break;
				case COMPONENT_FORM_ENGINE:
					engine = spaceData;
					break;
				case COMPONENT_FORM_LIFESUPPORT:
					lifeSupport = spaceData;
					break;
				case COMPONENT_FORM_GYROSCOPE:
					gyro = spaceData;
					break;
				case COMPONENT_FORM_ECM:
					ecm = spaceData;
					break;
				case COMPONENT_FORM_JAMMER:
					break;
				case COMPONENT_FORM_PROBE:
					probe = spaceData;
					break;
				}
			}
		}
	}
	calcAmmoTotals();
	for (size_t item = numOther; item < (numOther + numWeapons); item++)
	{
		//----------------------------------------------------------
		// Each weapon should point to its appropriate ammo total in
		// the ammo type total list...
		for (size_t ammoIndex = 0; ammoIndex < numAmmoTypes; ammoIndex++)
		{
			if ((int32_t)MasterComponent::masterList[inventory[item].masterID]
					.getWeaponAmmoMasterId()
				== ammoTypeTotal[ammoIndex].masterId)
			{
				inventory[item].ammoIndex = ammoIndex;
				break;
			}
		}
	}
	for (item = numOther + numWeapons; item < (numOther + numWeapons + numAmmos); item++)
	{
		//----------------------------------------------------------
		// Each weapon should point to its appropriate ammo total in
		// the ammo type total list...
		for (size_t ammoIndex = 0; ammoIndex < numAmmoTypes; ammoIndex++)
		{
			if (inventory[item].masterID == ammoTypeTotal[ammoIndex].masterId)
			{
				inventory[item].ammoIndex = ammoIndex;
				break;
			}
		}
	}
	for (item = 0; item < numOther; item++)
	{
		if ((inventory[item].masterID == MasterComponent::clanAntiMissileSystemID) || (inventory[item].masterID == MasterComponent::innerSphereAntiMissileSystemID))
		{
			for (size_t ammoIndex = 0; ammoIndex < numAmmoTypes; ammoIndex++)
			{
				if ((int32_t)MasterComponent::masterList[inventory[item].masterID]
						.getWeaponAmmoMasterId()
					== ammoTypeTotal[ammoIndex].masterId)
				{
					inventory[item].ammoIndex = ammoIndex;
					break;
				}
			}
		}
	}
	//--------------------------------------------------------------------------
	// If we added BULK to the mech, adjust the max and current armor settings.
	if (numArmorComponents)
	{
		float totalArmorPoints = ARMOR_POINTS_PER_NEWARMOR * numArmorComponents;
		float armorPercent = ARMOR_HEAD_PERCENT * totalArmorPoints;
		armor[MECH_ARMOR_LOCATION_HEAD].maxArmor += armorPercent;
		armor[MECH_ARMOR_LOCATION_HEAD].curArmor = armor[MECH_ARMOR_LOCATION_HEAD].maxArmor;
		armorPercent = ARMOR_CTORSO_PERCENT * totalArmorPoints;
		armor[MECH_ARMOR_LOCATION_CTORSO].maxArmor += armorPercent;
		armor[MECH_ARMOR_LOCATION_CTORSO].curArmor = armor[MECH_ARMOR_LOCATION_CTORSO].maxArmor;
		armorPercent = ARMOR_LTORSO_PERCENT * totalArmorPoints;
		armor[MECH_ARMOR_LOCATION_LTORSO].maxArmor += armorPercent;
		armor[MECH_ARMOR_LOCATION_LTORSO].curArmor = armor[MECH_ARMOR_LOCATION_LTORSO].maxArmor;
		armorPercent = ARMOR_RTORSO_PERCENT * totalArmorPoints;
		armor[MECH_ARMOR_LOCATION_RTORSO].maxArmor += armorPercent;
		armor[MECH_ARMOR_LOCATION_RTORSO].curArmor = armor[MECH_ARMOR_LOCATION_RTORSO].maxArmor;
		armorPercent = ARMOR_LARM_PERCENT * totalArmorPoints;
		armor[MECH_ARMOR_LOCATION_LARM].maxArmor += armorPercent;
		armor[MECH_ARMOR_LOCATION_LARM].curArmor = armor[MECH_ARMOR_LOCATION_LARM].maxArmor;
		armorPercent = ARMOR_RARM_PERCENT * totalArmorPoints;
		armor[MECH_ARMOR_LOCATION_RARM].maxArmor += armorPercent;
		armor[MECH_ARMOR_LOCATION_RARM].curArmor = armor[MECH_ARMOR_LOCATION_RARM].maxArmor;
		armorPercent = ARMOR_LLEG_PERCENT * totalArmorPoints;
		armor[MECH_ARMOR_LOCATION_LLEG].maxArmor += armorPercent;
		armor[MECH_ARMOR_LOCATION_LLEG].curArmor = armor[MECH_ARMOR_LOCATION_LLEG].maxArmor;
		armorPercent = ARMOR_RLEG_PERCENT * totalArmorPoints;
		armor[MECH_ARMOR_LOCATION_RLEG].maxArmor += armorPercent;
		armor[MECH_ARMOR_LOCATION_RLEG].curArmor = armor[MECH_ARMOR_LOCATION_RLEG].maxArmor;
		armorPercent = ARMOR_RCTORSO_PERCENT * totalArmorPoints;
		armor[MECH_ARMOR_LOCATION_RCTORSO].maxArmor += armorPercent;
		armor[MECH_ARMOR_LOCATION_RCTORSO].curArmor = armor[MECH_ARMOR_LOCATION_RCTORSO].maxArmor;
		armorPercent = ARMOR_RLTORSO_PERCENT * totalArmorPoints;
		armor[MECH_ARMOR_LOCATION_RLTORSO].maxArmor += armorPercent;
		armor[MECH_ARMOR_LOCATION_RLTORSO].curArmor = armor[MECH_ARMOR_LOCATION_RLTORSO].maxArmor;
		armorPercent = ARMOR_RRTORSO_PERCENT * totalArmorPoints;
		armor[MECH_ARMOR_LOCATION_RRTORSO].maxArmor += armorPercent;
		armor[MECH_ARMOR_LOCATION_RRTORSO].curArmor = armor[MECH_ARMOR_LOCATION_RRTORSO].maxArmor;
	}
	//---------------------------------------------------------------------------
	// We need to set the status states for legs and torso based upon our
	// current condition...
	calcLegStatus();
	calcTorsoStatus();
	calcFireRanges();
	maxCV = calcCV(true);
	curCV = calcCV(false);
	setThreatRating(-1);
	maxWeaponDamage = calcMaxTargetDamage();
	ObjectTypePtr type = ObjectManager->getObjectType(typeHandle);
	if (type->getExplosionObject() > 0)
		ObjectManager->objTypeManager->load(type->getExplosionObject(), true);
	//-----------------------------
	// Calc mech's chassis class...
	chassisClass = getMechClass();
	// logistics data needs to be initialized if you're going to get values
	LogisticsData::instance->init();
	cBills = 10;
	LogisticsVariant* mechVariant = LogisticsData::instance->getVariant(variantName);
	if (mechVariant)
		cBills = mechVariant->getCost();
	mechFile->close();
	delete mechFile;
	mechFile = nullptr;
	return (NO_ERROR);
}

typedef struct _LogisticsMasterLocationTableEntry
{
	bool clearOut;
	int32_t bodyLocation;
	int32_t slotNumber; // Starting at zero!
} LogisticsMasterLocationTableEntry;

LogisticsMasterLocationTableEntry logisticsTable[MAX_MOVER_INVENTORY_ITEMS] = {
	false, MECH_BODY_LOCATION_CTORSO, 0, // 0-Always Fusion Engine
	false, MECH_BODY_LOCATION_CTORSO, 1, // 1-Always Gyroscope
	false, MECH_BODY_LOCATION_LARM, 1, // 2-Always Arm Actuator
	false, MECH_BODY_LOCATION_RARM, 1, // 3-Always Arm Actuator
	false, MECH_BODY_LOCATION_LLEG, 1, // 4-Always Leg Actuator
	false, MECH_BODY_LOCATION_RLEG, 1, // 5-Always Leg Actuator
	false, MECH_BODY_LOCATION_HEAD, 0, // 6-Always COckpit
	false, MECH_BODY_LOCATION_HEAD, 1, // 7-Always Life Support
	false, MECH_BODY_LOCATION_LARM, 0, // 8-Always Shoulder
	false, MECH_BODY_LOCATION_RARM, 0, // 9-Always Shoulder
	false, MECH_BODY_LOCATION_LLEG, 0, // 10-Always Hip
	false, MECH_BODY_LOCATION_RLEG, 0, // 11-Always Hip
	false, MECH_BODY_LOCATION_HEAD, 2, // 12-Sensor
	// -Change back to true to be able
	// to replace these.
	false, MECH_BODY_LOCATION_HEAD,
	3, // 13-Other Electronic Warfare Component
	false, MECH_BODY_LOCATION_HEAD,
	4, // 14-Other Electronic Warfare Component
	false, MECH_BODY_LOCATION_HEAD,
	5, // 15-Other Electronic Warfare Component
	true, MECH_BODY_LOCATION_LLEG, 3, // 16-JumpJet
	false, MECH_BODY_LOCATION_LLEG, 4, // 17-Always NOTHING
	false, MECH_BODY_LOCATION_LLEG, 5, // 18-Always NOTHING
	false, MECH_BODY_LOCATION_RLEG, 3, // 19-Always NOTHING
	false, MECH_BODY_LOCATION_RLEG, 4, // 20-Always NOTHING
	false, MECH_BODY_LOCATION_RLEG, 5, // 21-Always NOTHING
	true, MECH_BODY_LOCATION_LARM, 4, // 22-Weapon or Ammo	- 22
	true, MECH_BODY_LOCATION_LTORSO, 0, // 38-Weapon or Ammo - 23
	true, MECH_BODY_LOCATION_CTORSO, 2, // 62-Weapon or Ammo - 24
	true, MECH_BODY_LOCATION_RTORSO, 0, // 50-Weapon or Ammo - 25
	true, MECH_BODY_LOCATION_RARM, 4, // 30-Weapon or Ammo - 26
	true, MECH_BODY_LOCATION_LARM, 5, // 23-Weapon or Ammo - 27
	true, MECH_BODY_LOCATION_LTORSO, 1, // 39-Weapon or Ammo - 28
	true, MECH_BODY_LOCATION_CTORSO, 3, // 63-Weapon or Ammo - 29
	true, MECH_BODY_LOCATION_RTORSO, 1, // 51-Weapon or Ammo - 30
	true, MECH_BODY_LOCATION_RARM, 5, // 31-Weapon or Ammo - 31
	true, MECH_BODY_LOCATION_LARM, 6, // 24-Weapon or Ammo - 32
	true, MECH_BODY_LOCATION_LTORSO, 2, // 40-Weapon or Ammo - 33
	true, MECH_BODY_LOCATION_CTORSO, 4, // 64-Weapon or Ammo - 34
	true, MECH_BODY_LOCATION_RTORSO, 2, // 52-Weapon or Ammo - 35
	true, MECH_BODY_LOCATION_RARM, 6, // 32-Weapon or Ammo - 36
	true, MECH_BODY_LOCATION_LARM, 7, // 25-Weapon or Ammo - 37
	true, MECH_BODY_LOCATION_LTORSO, 3, // 41-Weapon or Ammo - 38
	true, MECH_BODY_LOCATION_CTORSO, 5, // 65-Weapon or Ammo - 39
	true, MECH_BODY_LOCATION_RTORSO, 3, // 53-Weapon or Ammo - 40
	true, MECH_BODY_LOCATION_RARM, 7, // 33-Weapon or Ammo - 41
	true, MECH_BODY_LOCATION_LARM, 8, // 26-Weapon or Ammo - 42
	true, MECH_BODY_LOCATION_LTORSO, 4, // 42-Weapon or Ammo - 43
	true, MECH_BODY_LOCATION_CTORSO, 6, // 66-Weapon or Ammo - 44
	true, MECH_BODY_LOCATION_RTORSO, 4, // 54-Weapon or Ammo - 45
	true, MECH_BODY_LOCATION_RARM, 8, // 34-Weapon or Ammo - 46
	true, MECH_BODY_LOCATION_LARM, 9, // 27-Weapon or Ammo - 47
	true, MECH_BODY_LOCATION_LTORSO, 5, // 43-Weapon or Ammo - 48
	true, MECH_BODY_LOCATION_CTORSO, 7, // 67-Weapon or Ammo - 49
	true, MECH_BODY_LOCATION_RTORSO, 5, // 55-Weapon or Ammo - 50
	true, MECH_BODY_LOCATION_RARM, 9, // 35-Weapon or Ammo - 51
	true, MECH_BODY_LOCATION_LARM, 10, // 28-Weapon or Ammo- 52
	true, MECH_BODY_LOCATION_LTORSO, 6, // 44-Weapon or Ammo - 53
	true, MECH_BODY_LOCATION_CTORSO, 8, // 68-Weapon or Ammo - 54
	true, MECH_BODY_LOCATION_RTORSO, 6, // 56-Weapon or Ammo - 55
	true, MECH_BODY_LOCATION_RARM, 10, // 36-Weapon or Ammo- 56
	true, MECH_BODY_LOCATION_LARM, 11, // 29-Weapon or Ammo- 57
	true, MECH_BODY_LOCATION_LTORSO, 7, // 45-Weapon or Ammo - 58
	true, MECH_BODY_LOCATION_CTORSO, 9, // 69-Weapon or Ammo - 59
	true, MECH_BODY_LOCATION_RTORSO, 7, // 57-Weapon or Ammo - 60
	true, MECH_BODY_LOCATION_RARM, 11, // 37-Weapon or Ammo- 61
	true, MECH_BODY_LOCATION_LTORSO, 8, // 46-Weapon or Ammo - 62
	true, MECH_BODY_LOCATION_CTORSO, 10, // 70-Weapon or Ammo - 63
	true, MECH_BODY_LOCATION_RTORSO, 8, // 58-Weapon or Ammo - 64
	true, MECH_BODY_LOCATION_LTORSO, 9, // 47-Weapon or Ammo - 65
	true, MECH_BODY_LOCATION_CTORSO, 11, // 71-Weapon or Ammo  - 66
	true, MECH_BODY_LOCATION_RTORSO, 9, // 59-Weapon or Ammo - 67
	true, MECH_BODY_LOCATION_LTORSO, 10, // 48-Weapon or Ammo - 68
	true, MECH_BODY_LOCATION_RTORSO, 10, // 60-Weapon or Ammo - 69
	true, MECH_BODY_LOCATION_LTORSO, 11, // 49-Weapon or Ammo - 70
	true, MECH_BODY_LOCATION_RTORSO, 11 // 61-Weapon or Ammo - 71
};

#define JUMPJET_SLOT 16
#define SENSOR_SLOT 12
#define ELECTRONIC_SLOT 13
#define WEAPONAMMO_SLOT 17
//----------------------------------------------------------------------------------
void BattleMech::resetComponents(int32_t totalComponents, int32_t* componentList)
{
	//------------------------------------------------------------
	// Create a master component list for this mech.
	// Remove all of the components logistics can replace.
	// Add back in all of the components logistics replaced.
	// Put in the right body locations using the above table.
	int32_t localMasterComponentList[MAX_MOVER_INVENTORY_ITEMS];
	memset(localMasterComponentList, 0xff, sizeof(int32_t) * MAX_MOVER_INVENTORY_ITEMS);
	// Copy current itemList to localLists
	// Remove all logistics replacable components
	for (size_t item = 0; item < MAX_MOVER_INVENTORY_ITEMS; item++)
	{
		localMasterComponentList[item] = inventory[item].masterID;
		if (inventory[item].masterID != 255)
		{
			int32_t ItemLocation = -1;
			for (size_t i = 0; i < MAX_MOVER_INVENTORY_ITEMS; i++)
			{
				if (ItemLocationToInvLocation[i] == item)
				{
					ItemLocation = i;
					break;
				}
			}
			if (ItemLocation == -1)
				STOP(("Could Not Find Item %d on Mech", item));
			if (logisticsTable[ItemLocation].clearOut)
			{
				localMasterComponentList[item] = 0xff;
			}
		}
	}
	//-----------------------------------------------------------------------------
	// Reset the weapon Node Data in the mech Appearance..�.
	appearance->resetWeaponNodes();
	//--------------------------------------------------------------------------
	// If we added Armor to the mech, remove the armor so that player's new
	// armor
	// components are all that is added, adjust the max and current armor
	// settings.
	//
	if (numArmorComponents)
	{
		float totalArmorPoints = ARMOR_POINTS_PER_NEWARMOR * numArmorComponents;
		int32_t armorPercent = ARMOR_HEAD_PERCENT * totalArmorPoints;
		armor[MECH_ARMOR_LOCATION_HEAD].maxArmor -= armorPercent;
		armor[MECH_ARMOR_LOCATION_HEAD].curArmor = armor[MECH_ARMOR_LOCATION_HEAD].maxArmor;
		armorPercent = ARMOR_CTORSO_PERCENT * totalArmorPoints;
		armor[MECH_ARMOR_LOCATION_CTORSO].maxArmor -= armorPercent;
		armor[MECH_ARMOR_LOCATION_CTORSO].curArmor = armor[MECH_ARMOR_LOCATION_CTORSO].maxArmor;
		armorPercent = ARMOR_LTORSO_PERCENT * totalArmorPoints;
		armor[MECH_ARMOR_LOCATION_LTORSO].maxArmor -= armorPercent;
		armor[MECH_ARMOR_LOCATION_LTORSO].curArmor = armor[MECH_ARMOR_LOCATION_LTORSO].maxArmor;
		armorPercent = ARMOR_RTORSO_PERCENT * totalArmorPoints;
		armor[MECH_ARMOR_LOCATION_RTORSO].maxArmor -= armorPercent;
		armor[MECH_ARMOR_LOCATION_RTORSO].curArmor = armor[MECH_ARMOR_LOCATION_RTORSO].maxArmor;
		armorPercent = ARMOR_LARM_PERCENT * totalArmorPoints;
		armor[MECH_ARMOR_LOCATION_LARM].maxArmor -= armorPercent;
		armor[MECH_ARMOR_LOCATION_LARM].curArmor = armor[MECH_ARMOR_LOCATION_LARM].maxArmor;
		armorPercent = ARMOR_RARM_PERCENT * totalArmorPoints;
		armor[MECH_ARMOR_LOCATION_RARM].maxArmor -= armorPercent;
		armor[MECH_ARMOR_LOCATION_RARM].curArmor = armor[MECH_ARMOR_LOCATION_RARM].maxArmor;
		armorPercent = ARMOR_LLEG_PERCENT * totalArmorPoints;
		armor[MECH_ARMOR_LOCATION_LLEG].maxArmor -= armorPercent;
		armor[MECH_ARMOR_LOCATION_LLEG].curArmor = armor[MECH_ARMOR_LOCATION_LLEG].maxArmor;
		armorPercent = ARMOR_RLEG_PERCENT * totalArmorPoints;
		armor[MECH_ARMOR_LOCATION_RLEG].maxArmor -= armorPercent;
		armor[MECH_ARMOR_LOCATION_RLEG].curArmor = armor[MECH_ARMOR_LOCATION_RLEG].maxArmor;
		armorPercent = ARMOR_RCTORSO_PERCENT * totalArmorPoints;
		armor[MECH_ARMOR_LOCATION_RCTORSO].maxArmor -= armorPercent;
		armor[MECH_ARMOR_LOCATION_RCTORSO].curArmor = armor[MECH_ARMOR_LOCATION_RCTORSO].maxArmor;
		armorPercent = ARMOR_RLTORSO_PERCENT * totalArmorPoints;
		armor[MECH_ARMOR_LOCATION_RLTORSO].maxArmor -= armorPercent;
		armor[MECH_ARMOR_LOCATION_RLTORSO].curArmor = armor[MECH_ARMOR_LOCATION_RLTORSO].maxArmor;
		armorPercent = ARMOR_RRTORSO_PERCENT * totalArmorPoints;
		armor[MECH_ARMOR_LOCATION_RRTORSO].maxArmor -= armorPercent;
		armor[MECH_ARMOR_LOCATION_RRTORSO].curArmor = armor[MECH_ARMOR_LOCATION_RRTORSO].maxArmor;
	}
	int32_t numNewArmors = 0;
	numJumpJets = 0;
	int32_t numElectronics = 0;
	int32_t numWeaponsAmmo = 0;
	// Add Heidi's new Components to open locations.
	for (size_t newItems = 0; newItems < totalComponents; newItems++)
	{
		switch (MasterComponent::masterList[componentList[newItems]].getForm())
		{
		case COMPONENT_FORM_HEATSINK:
			continue;
			break;
		case COMPONENT_FORM_COCKPIT:
		case COMPONENT_FORM_LIFESUPPORT:
		case COMPONENT_FORM_GYROSCOPE:
		case COMPONENT_FORM_POWER_AMPLIFIER:
		case COMPONENT_FORM_CASE:
		case COMPONENT_FORM_ACTUATOR:
		case COMPONENT_FORM_ENGINE:
			// If any of these are in Heidi's list its REALLY BAD
			STOP(("Invalid component added in Logistics %d", componentList[newItems]));
			break;
		case COMPONENT_FORM_SENSOR:
			localMasterComponentList[SENSOR_SLOT] = componentList[newItems];
			break;
		case COMPONENT_FORM_ECM:
		case COMPONENT_FORM_PROBE:
		case COMPONENT_FORM_JAMMER:
			localMasterComponentList[ELECTRONIC_SLOT + numElectronics] = componentList[newItems];
			numElectronics++;
			if (numElectronics > 3)
				STOP(("TOO many electronic warfare components on this mech"));
			break;
		case COMPONENT_FORM_WEAPON:
		case COMPONENT_FORM_WEAPON_ENERGY:
		case COMPONENT_FORM_WEAPON_BALLISTIC:
		case COMPONENT_FORM_WEAPON_MISSILE:
		case COMPONENT_FORM_AMMO:
			localMasterComponentList[WEAPONAMMO_SLOT + numWeaponsAmmo] = componentList[newItems];
			numWeaponsAmmo++;
			// Heidi does not give me ammo.  Add manually here.
			if (MasterComponent::masterList[componentList[newItems]].getWeaponAmmoMasterId())
			{
				localMasterComponentList[WEAPONAMMO_SLOT + numWeaponsAmmo] =
					MasterComponent::masterList[componentList[newItems]].getWeaponAmmoMasterId();
				numWeaponsAmmo++;
			}
			if (numWeaponsAmmo > 54)
				STOP(("TOO many weapons and/or Ammo components on this mech"));
			break;
		case COMPONENT_FORM_BULK:
			numNewArmors++;
			break;
		case COMPONENT_FORM_JUMPJET:
			localMasterComponentList[JUMPJET_SLOT] = componentList[newItems];
			break;
		}
	}
	// Now use the above lists to re-component out the mech!
	numAntiMissileSystems = numOther = numWeapons = numAmmos = 0;
	//-----------------------------------------------------
	// Read in the mech's non-weapon/non-ammo components...
	int32_t realItemNum = 0;
	memset(ItemLocationToInvLocation, 0xff, sizeof(int32_t) * MAX_MOVER_INVENTORY_ITEMS);
	// Read in everything but weapons and AMMO
	for (size_t curItem = 0; curItem < MAX_MOVER_INVENTORY_ITEMS; curItem++)
	{
		inventory[realItemNum].masterID = localMasterComponentList[curItem];
		if (inventory[realItemNum].masterID && (inventory[realItemNum].masterID != 255))
		{
			//---------------------------------------------------------------
			// If the component is a JumpJet, increment our jump jet count...
			switch (MasterComponent::masterList[inventory[realItemNum].masterID].getForm())
			{
			case COMPONENT_FORM_JUMPJET:
				numJumpJets = 5;
			case COMPONENT_FORM_COCKPIT:
			case COMPONENT_FORM_SENSOR:
			case COMPONENT_FORM_LIFESUPPORT:
			case COMPONENT_FORM_GYROSCOPE:
			case COMPONENT_FORM_POWER_AMPLIFIER:
			case COMPONENT_FORM_ECM:
			case COMPONENT_FORM_PROBE:
			case COMPONENT_FORM_JAMMER:
			case COMPONENT_FORM_BULK:
			case COMPONENT_FORM_CASE:
			case COMPONENT_FORM_ACTUATOR:
			case COMPONENT_FORM_ENGINE:
			case COMPONENT_FORM_HEATSINK:
				numOther++;
				inventory[realItemNum].health =
					MasterComponent::masterList[inventory[realItemNum].masterID].getHealth();
				inventory[realItemNum].disabled = false;
				inventory[realItemNum].amount = 1;
				inventory[realItemNum].ammoIndex = -1;
				inventory[realItemNum].readyTime = 0.0;
				inventory[realItemNum].bodyLocation = 255;
				ItemLocationToInvLocation[curItem] = realItemNum;
				realItemNum++;
				break;
			case COMPONENT_FORM_WEAPON:
			case COMPONENT_FORM_WEAPON_ENERGY:
			case COMPONENT_FORM_WEAPON_BALLISTIC:
			case COMPONENT_FORM_WEAPON_MISSILE:
				// Ignore weapons until second pass
				break;
			case COMPONENT_FORM_AMMO:
				// Ignore AMMO until third pass
				break;
			}
		}
	}
	// NOW read in the weapons.
	for (curItem = 0; curItem < MAX_MOVER_INVENTORY_ITEMS; curItem++)
	{
		inventory[realItemNum].masterID = localMasterComponentList[curItem];
		if (inventory[realItemNum].masterID && (inventory[realItemNum].masterID != 255))
		{
			//---------------------------------------------------------------
			// If the component is a JumpJet, increment our jump jet count...
			int32_t sourceWeaponType;
			if (MasterComponent::masterList[inventory[realItemNum].masterID].getForm() == COMPONENT_FORM_WEAPON_MISSILE)
				sourceWeaponType = MECH3D_WEAPONTYPE_MISSILE;
			else if (MasterComponent::masterList[inventory[realItemNum].masterID].getForm() == COMPONENT_FORM_WEAPON_BALLISTIC)
				sourceWeaponType = MECH3D_WEAPONTYPE_BALLISTIC;
			else if (MasterComponent::masterList[inventory[realItemNum].masterID].getForm() == COMPONENT_FORM_WEAPON_ENERGY)
				sourceWeaponType = MECH3D_WEAPONTYPE_ENERGY;
			else
				sourceWeaponType = MECH3D_WEAPONTYPE_ANY;
			//---------------------------------------------------------------
			// If the component is a JumpJet, increment our jump jet count...
			switch (MasterComponent::masterList[inventory[realItemNum].masterID].getForm())
			{
			case COMPONENT_FORM_JUMPJET:
			case COMPONENT_FORM_COCKPIT:
			case COMPONENT_FORM_SENSOR:
			case COMPONENT_FORM_LIFESUPPORT:
			case COMPONENT_FORM_GYROSCOPE:
			case COMPONENT_FORM_POWER_AMPLIFIER:
			case COMPONENT_FORM_ECM:
			case COMPONENT_FORM_PROBE:
			case COMPONENT_FORM_JAMMER:
			case COMPONENT_FORM_BULK:
			case COMPONENT_FORM_CASE:
			case COMPONENT_FORM_ACTUATOR:
			case COMPONENT_FORM_ENGINE:
			case COMPONENT_FORM_HEATSINK:
				// We did these Above here.
				break;
			case COMPONENT_FORM_WEAPON:
			case COMPONENT_FORM_WEAPON_ENERGY:
			case COMPONENT_FORM_WEAPON_BALLISTIC:
			case COMPONENT_FORM_WEAPON_MISSILE:
				numWeapons++;
				inventory[realItemNum].health =
					MasterComponent::masterList[inventory[realItemNum].masterID].getHealth();
				inventory[realItemNum].disabled = false;
				inventory[realItemNum].amount = 1;
				inventory[realItemNum].ammoIndex = -1;
				inventory[realItemNum].facing = appearance->getWeaponNode(sourceWeaponType);
				inventory[realItemNum].readyTime = 0.0;
				inventory[realItemNum].bodyLocation = 255;
				inventory[realItemNum].effectiveness = (int16_t)(MasterComponent::masterList[inventory[realItemNum].masterID].getWeaponDamage() * 10.0 / // damage over 10 seconds
					MasterComponent::masterList[inventory[realItemNum].masterID]
						.getWeaponRecycleTime());
				inventory[realItemNum].effectiveness *=
					WeaponRanges[MasterComponent::masterList[inventory[realItemNum].masterID]
									 .getWeaponRange()][1]
					/ 24;
				ItemLocationToInvLocation[curItem] = realItemNum;
				realItemNum++;
				//-------------------------------------
				// Cache in the weapon special effect.
				break;
			case COMPONENT_FORM_AMMO:
				// Do these next.
				break;
			}
		}
	}
	// NOW read in the AMMO
	for (curItem = 0; curItem < MAX_MOVER_INVENTORY_ITEMS; curItem++)
	{
		inventory[realItemNum].masterID = localMasterComponentList[curItem];
		if (inventory[realItemNum].masterID && (inventory[realItemNum].masterID != 255))
		{
			//---------------------------------------------------------------
			// If the component is a JumpJet, increment our jump jet count...
			switch (MasterComponent::masterList[inventory[realItemNum].masterID].getForm())
			{
			case COMPONENT_FORM_JUMPJET:
			case COMPONENT_FORM_COCKPIT:
			case COMPONENT_FORM_SENSOR:
			case COMPONENT_FORM_LIFESUPPORT:
			case COMPONENT_FORM_GYROSCOPE:
			case COMPONENT_FORM_POWER_AMPLIFIER:
			case COMPONENT_FORM_ECM:
			case COMPONENT_FORM_PROBE:
			case COMPONENT_FORM_JAMMER:
			case COMPONENT_FORM_BULK:
			case COMPONENT_FORM_CASE:
			case COMPONENT_FORM_ACTUATOR:
			case COMPONENT_FORM_ENGINE:
			case COMPONENT_FORM_HEATSINK:
				// Did these above.
				break;
			case COMPONENT_FORM_WEAPON:
			case COMPONENT_FORM_WEAPON_ENERGY:
			case COMPONENT_FORM_WEAPON_BALLISTIC:
			case COMPONENT_FORM_WEAPON_MISSILE:
				// Did These Above, too...
				break;
			case COMPONENT_FORM_AMMO:
				numAmmos++;
				inventory[realItemNum].health =
					MasterComponent::masterList[inventory[realItemNum].masterID].getHealth();
				inventory[realItemNum].disabled = false;
				inventory[realItemNum].amount = 1;
				inventory[realItemNum].ammoIndex = -1;
				inventory[realItemNum].readyTime = 0.0;
				inventory[realItemNum].bodyLocation = 255;
				//-----------------------------------------------------------------------
				// Since the ammo amount in the profile is amount per ton, let's
				// make the amount equal to the number of
				// missiles/bullets/whatever. If it's set to 255, use the
				// default amount per ton as defined in the component table...
				//
				// Ammo per ton is always fully stocked.  I.e. always -1 or 255
				inventory[realItemNum].amount =
					MasterComponent::masterList[inventory[realItemNum].masterID].getAmmoPerTon();
				inventory[realItemNum].ammoIndex = -1;
				inventory[realItemNum].startAmount = inventory[realItemNum].amount;
				inventory[realItemNum].health =
					MasterComponent::masterList[inventory[realItemNum].masterID].getHealth();
				inventory[realItemNum].disabled = false;
				inventory[realItemNum].readyTime = 0.0;
				inventory[realItemNum].bodyLocation = 255;
				ItemLocationToInvLocation[curItem] = realItemNum;
				realItemNum++;
				break;
			}
		}
	}
	//--------------------------------------------------------------------------
	// If we added BULK to the mech, adjust the max and current armor settings.
	if (numNewArmors)
	{
		float totalArmorPoints = ARMOR_POINTS_PER_NEWARMOR * numNewArmors;
		float armorPercent = ARMOR_HEAD_PERCENT * totalArmorPoints;
		armor[MECH_ARMOR_LOCATION_HEAD].maxArmor += armorPercent;
		armor[MECH_ARMOR_LOCATION_HEAD].curArmor = armor[MECH_ARMOR_LOCATION_HEAD].maxArmor;
		armorPercent = ARMOR_CTORSO_PERCENT * totalArmorPoints;
		armor[MECH_ARMOR_LOCATION_CTORSO].maxArmor += armorPercent;
		armor[MECH_ARMOR_LOCATION_CTORSO].curArmor = armor[MECH_ARMOR_LOCATION_CTORSO].maxArmor;
		armorPercent = ARMOR_LTORSO_PERCENT * totalArmorPoints;
		armor[MECH_ARMOR_LOCATION_LTORSO].maxArmor += armorPercent;
		armor[MECH_ARMOR_LOCATION_LTORSO].curArmor = armor[MECH_ARMOR_LOCATION_LTORSO].maxArmor;
		armorPercent = ARMOR_RTORSO_PERCENT * totalArmorPoints;
		armor[MECH_ARMOR_LOCATION_RTORSO].maxArmor += armorPercent;
		armor[MECH_ARMOR_LOCATION_RTORSO].curArmor = armor[MECH_ARMOR_LOCATION_RTORSO].maxArmor;
		armorPercent = ARMOR_LARM_PERCENT * totalArmorPoints;
		armor[MECH_ARMOR_LOCATION_LARM].maxArmor += armorPercent;
		armor[MECH_ARMOR_LOCATION_LARM].curArmor = armor[MECH_ARMOR_LOCATION_LARM].maxArmor;
		armorPercent = ARMOR_RARM_PERCENT * totalArmorPoints;
		armor[MECH_ARMOR_LOCATION_RARM].maxArmor += armorPercent;
		armor[MECH_ARMOR_LOCATION_RARM].curArmor = armor[MECH_ARMOR_LOCATION_RARM].maxArmor;
		armorPercent = ARMOR_LLEG_PERCENT * totalArmorPoints;
		armor[MECH_ARMOR_LOCATION_LLEG].maxArmor += armorPercent;
		armor[MECH_ARMOR_LOCATION_LLEG].curArmor = armor[MECH_ARMOR_LOCATION_LLEG].maxArmor;
		armorPercent = ARMOR_RLEG_PERCENT * totalArmorPoints;
		armor[MECH_ARMOR_LOCATION_RLEG].maxArmor += armorPercent;
		armor[MECH_ARMOR_LOCATION_RLEG].curArmor = armor[MECH_ARMOR_LOCATION_RLEG].maxArmor;
		armorPercent = ARMOR_RCTORSO_PERCENT * totalArmorPoints;
		armor[MECH_ARMOR_LOCATION_RCTORSO].maxArmor += armorPercent;
		armor[MECH_ARMOR_LOCATION_RCTORSO].curArmor = armor[MECH_ARMOR_LOCATION_RCTORSO].maxArmor;
		armorPercent = ARMOR_RLTORSO_PERCENT * totalArmorPoints;
		armor[MECH_ARMOR_LOCATION_RLTORSO].maxArmor += armorPercent;
		armor[MECH_ARMOR_LOCATION_RLTORSO].curArmor = armor[MECH_ARMOR_LOCATION_RLTORSO].maxArmor;
		armorPercent = ARMOR_RRTORSO_PERCENT * totalArmorPoints;
		armor[MECH_ARMOR_LOCATION_RRTORSO].maxArmor += armorPercent;
		armor[MECH_ARMOR_LOCATION_RRTORSO].curArmor = armor[MECH_ARMOR_LOCATION_RRTORSO].maxArmor;
	}
	//---------------------------------------------------------------
	// Reset the quick Access components in case they removed them in logistics.
	// SHould only need to do jump jets!!
	jumpJets = 255;
	//------------------------------------------------------------
	// Now, read in the component layout for each body location...
	for (size_t curLocation = 0; curLocation < NUM_MECH_BODY_LOCATIONS; curLocation++)
	{
		body[curLocation].CASE = true; // ALL MC2 Mechs have CASE everywhere.
		// Internal Structure is ALREADY set!
		//---------------------------------------------------------
		// Damage and states are already set.
		int32_t numSpaces = NumLocationCriticalSpaces[curLocation];
		body[curLocation].totalSpaces = 0;
		for (size_t curSpace = 0; curSpace < numSpaces; curSpace++)
		{
			// Find out where in the space table this location lies.
			uint8_t spaceData = 0xff;
			for (size_t curTable = 0; curTable < MAX_MOVER_INVENTORY_ITEMS; curTable++)
			{
				if ((logisticsTable[curTable].bodyLocation == curLocation) && (logisticsTable[curTable].slotNumber == curSpace))
				{
					spaceData = curTable;
					break;
				}
			}
			spaceData = ItemLocationToInvLocation[spaceData];
			body[curLocation].criticalSpaces[curSpace].inventoryID = spaceData;
			body[curLocation].criticalSpaces[curSpace].hit =
				false; // Everything always repaired now.
			if (spaceData < 255)
			{
				if (spaceData >= numOther + numWeapons + numAmmos)
					// Just ignore this space.  Sage has filled them all in now.
					// ACTUALLY, we should stop here.
					continue;
				inventory[spaceData].bodyLocation = curLocation;
				//--------------------------------------------------------
				// The following line assumes the "new" crit hit system...
				body[curLocation].totalSpaces +=
					MasterComponent::masterList[inventory[spaceData].masterID].getSize();
				//--------------------------------------------------------
				// Preserve critical component indices for quick access...
				switch (MasterComponent::masterList[inventory[spaceData].masterID].getForm())
				{
				case COMPONENT_FORM_COCKPIT:
					cockpit = spaceData;
					break;
				case COMPONENT_FORM_JUMPJET:
					jumpJets = spaceData;
					break;
				case COMPONENT_FORM_SENSOR:
					sensor = spaceData;
					// Sensor already defined in the first component pass.
					// Not possible to replace sensors or EW components in
					// logistics now!!
					/*
					sensorSystem = SensorManager->newSensor();
					sensorSystem->setOwner(this);
					sensorSystem->setRange(MasterComponent::masterList[inventory[sensor].masterID].getSensorRange());
					*/
					break;
				case COMPONENT_FORM_HEATSINK:
					inventory[spaceData].bodyLocation = curLocation;
					break;
				case COMPONENT_FORM_AMMO:
					inventory[spaceData].bodyLocation = curLocation;
					break;
				case COMPONENT_FORM_WEAPON:
				case COMPONENT_FORM_WEAPON_ENERGY:
					inventory[spaceData].bodyLocation = curLocation;
					break;
				case COMPONENT_FORM_WEAPON_BALLISTIC:
					inventory[spaceData].bodyLocation = curLocation;
					if ((inventory[spaceData].masterID == MasterComponent::clanAntiMissileSystemID) || (inventory[spaceData].masterID == MasterComponent::innerSphereAntiMissileSystemID))
					{
						//------------------------------------------------------
						// Add to Anti-Missile System list for fast reference...
						if (numAntiMissileSystems == MAX_ANTI_MISSILE_SYSTEMS)
							Fatal(0, "Too many Anti-Missile Systems");
						antiMissileSystem[numAntiMissileSystems++] = spaceData;
					}
					break;
				case COMPONENT_FORM_WEAPON_MISSILE:
					inventory[spaceData].bodyLocation = curLocation;
					break;
				case COMPONENT_FORM_ACTUATOR:
					if (inventory[spaceData].masterID == MasterComponent::armActuatorID)
					{
						if (curLocation == MECH_BODY_LOCATION_LARM)
							actuator[ACTUATOR_LSHOULDER] = spaceData;
						else if (curLocation == MECH_BODY_LOCATION_RARM)
							actuator[ACTUATOR_RSHOULDER] = spaceData;
					}
					else if (inventory[spaceData].masterID == MasterComponent::legActuatorID)
					{
						if (curLocation == MECH_BODY_LOCATION_LLEG)
							actuator[ACTUATOR_LHIP] = spaceData;
						else if (curLocation == MECH_BODY_LOCATION_RLEG)
							actuator[ACTUATOR_RHIP] = spaceData;
					}
					break;
				case COMPONENT_FORM_ENGINE:
					engine = spaceData;
					break;
				case COMPONENT_FORM_LIFESUPPORT:
					lifeSupport = spaceData;
					break;
				case COMPONENT_FORM_GYROSCOPE:
					gyro = spaceData;
					break;
				case COMPONENT_FORM_ECM:
					ecm = spaceData;
					break;
				case COMPONENT_FORM_JAMMER:
					break;
				case COMPONENT_FORM_PROBE:
					probe = spaceData;
					break;
				}
			}
		}
	}
	calcAmmoTotals();
	for (item = numOther; item < (numOther + numWeapons); item++)
	{
		//----------------------------------------------------------
		// Each weapon should point to its appropriate ammo total in
		// the ammo type total list...
		for (size_t ammoIndex = 0; ammoIndex < numAmmoTypes; ammoIndex++)
		{
			if ((int32_t)MasterComponent::masterList[inventory[item].masterID]
					.getWeaponAmmoMasterId()
				== ammoTypeTotal[ammoIndex].masterId)
			{
				inventory[item].ammoIndex = ammoIndex;
				break;
			}
		}
	}
	for (item = numOther + numWeapons; item < (numOther + numWeapons + numAmmos); item++)
	{
		//----------------------------------------------------------
		// Each weapon should point to its appropriate ammo total in
		// the ammo type total list...
		for (size_t ammoIndex = 0; ammoIndex < numAmmoTypes; ammoIndex++)
		{
			if (inventory[item].masterID == ammoTypeTotal[ammoIndex].masterId)
			{
				inventory[item].ammoIndex = ammoIndex;
				break;
			}
		}
	}
	for (item = 0; item < numOther; item++)
	{
		if ((inventory[item].masterID == MasterComponent::clanAntiMissileSystemID) || (inventory[item].masterID == MasterComponent::innerSphereAntiMissileSystemID))
		{
			for (size_t ammoIndex = 0; ammoIndex < numAmmoTypes; ammoIndex++)
			{
				if ((int32_t)MasterComponent::masterList[inventory[item].masterID]
						.getWeaponAmmoMasterId()
					== ammoTypeTotal[ammoIndex].masterId)
				{
					inventory[item].ammoIndex = ammoIndex;
					break;
				}
			}
		}
	}
	//---------------------------------------------------------------------------
	// We need to set the status states for legs and torso based upon our
	// current condition...
	calcFireRanges();
	maxCV = calcCV(true);
	curCV = calcCV(false);
	setThreatRating(-1);
	maxWeaponDamage = calcMaxTargetDamage();
	// local variable needs to be set for class
	numJumpJets = numJumpJets;
}

int32_t
BattleMech::init(FitIniFile* mechFile)
{
	std::wstring_view BodyLocationBlockString[NUM_BODY_LOCATIONS] = {"Head", "CenterTorso", "LeftTorso",
		"RightTorso", "LeftArm", "RightArm", "LeftLeg", "RightLeg"};
	//-----------------------
	// Read in the mech data.
	int32_t result = mechFile->seekBlock("Header");
	if (result != NO_ERROR)
		return (result);
	wchar_t fileType[128];
	result = mechFile->readIdString("FileType", fileType, 127);
	if (result != NO_ERROR)
		return (result);
	if (strcmp(fileType, "MechProfile"))
		return (-1);
	result = mechFile->seekBlock("General");
	if (result != NO_ERROR)
		return (result);
	wchar_t thisMechName[128];
	result = mechFile->readIdString("Name", thisMechName, 127);
	strncpy(name, thisMechName, MAXLEN_MOVER_NAME - 1);
	name[MAXLEN_MOVER_NAME] = nullptr;
	result = mechFile->readIdLong("ChassisBR", chassisBR);
	if (result != NO_ERROR)
		chassisBR = 100;
	result = mechFile->readIdFloat("CurTonnage", tonnage);
	if (result != NO_ERROR)
		return (result);
	result = mechFile->readIdLong("DescIndex", descID);
	if (result != NO_ERROR)
		descID = -1;
	longName[0] = nullptr;
	//	result = mechFile->readIdLong("NameIndex", nameIndex);
	//	if (result != NO_ERROR)
	//		return result;
	//	result = mechFile->readIdLong("NameVariant", variant);
	//	if (result != NO_ERROR)
	//		return result;
	result = mechFile->readIdLong("Pilot", pilotNum);
	if (result != NO_ERROR)
		pilotNum = -1;
	wchar_t cStatus = 0;
	// result = mechFile->readIdChar("Status", cStatus);
	// if (result != NO_ERROR)
	//	return(result);
	status = cStatus;
	result = mechFile->readIdBoolean("NotMineYet", notMineYet);
	if (result != NO_ERROR)
		notMineYet = true; // We weren't written by logistics.  I.E. Not mine Yet.
#ifdef USEHEAT
	result = mechFile->seekBlock("HeatSinks");
	if (result != NO_ERROR)
		return (result);
	result = mechFile->readIdUChar("HeatSinkType", heatSinkType);
	if (result != NO_ERROR)
		return (result);
	result = mechFile->readIdUChar("ExtraHeatSinks", extraHeatSinks);
	if (result != NO_ERROR)
		return (result);
	uint8_t heat;
	result = mechFile->readIdUChar("HeatBuildUp", heat);
	if (result != NO_ERROR)
		return (result);
	heat = (float)heat;
	disabledHeatSinks = 0;
	curLegHeatSinks = 0;
	heatDissipation = calcHeatDissipation();
#endif
	result = mechFile->seekBlock("Engine");
	if (result != NO_ERROR)
		return (result);
	uint8_t speed;
	result = mechFile->readIdUChar("MaxRunSpeed", speed);
	if (result != NO_ERROR)
		return (result);
	maxMoveSpeed = (float)speed;
	result = mechFile->seekBlock("MovementSystem");
	if (result == NO_ERROR)
	{
		int32_t crashSize = 0;
		result = mechFile->readIdLong("CrashAvoidSelf", crashSize);
		if (result == NO_ERROR)
			crashAvoidSelf = crashSize;
		result = mechFile->readIdLong("CrashAvoidPath", crashSize);
		if (result == NO_ERROR)
			crashAvoidPath = crashSize;
		result = mechFile->readIdLong("CrashBlockSelf", crashSize);
		if (result == NO_ERROR)
			crashBlockSelf = crashSize;
		result = mechFile->readIdLong("CrashBlockPath", crashSize);
		if (result == NO_ERROR)
			crashBlockPath = crashSize;
		float crashYield = 0.0;
		result = mechFile->readIdFloat("CrashYieldTime", crashYield);
		if (result == NO_ERROR)
			crashYieldTime = crashSize;
	}
	result = mechFile->seekBlock("MaxArmorPoints");
	if (result != NO_ERROR)
		return (result);
	result = mechFile->readIdUChar("Head", armor[MECH_ARMOR_LOCATION_HEAD].maxArmor);
	if (result != NO_ERROR)
		return (result);
	result = mechFile->readIdUChar("CenterTorso", armor[MECH_ARMOR_LOCATION_CTORSO].maxArmor);
	if (result != NO_ERROR)
		return (result);
	result = mechFile->readIdUChar("LeftTorso", armor[MECH_ARMOR_LOCATION_LTORSO].maxArmor);
	if (result != NO_ERROR)
		return (result);
	result = mechFile->readIdUChar("RightTorso", armor[MECH_ARMOR_LOCATION_RTORSO].maxArmor);
	if (result != NO_ERROR)
		return (result);
	result = mechFile->readIdUChar("LeftArm", armor[MECH_ARMOR_LOCATION_LARM].maxArmor);
	if (result != NO_ERROR)
		return (result);
	result = mechFile->readIdUChar("RightArm", armor[MECH_ARMOR_LOCATION_RARM].maxArmor);
	if (result != NO_ERROR)
		return (result);
	result = mechFile->readIdUChar("LeftLeg", armor[MECH_ARMOR_LOCATION_LLEG].maxArmor);
	if (result != NO_ERROR)
		return (result);
	result = mechFile->readIdUChar("RightLeg", armor[MECH_ARMOR_LOCATION_RLEG].maxArmor);
	if (result != NO_ERROR)
		return (result);
	result = mechFile->readIdUChar("RearCenterTorso", armor[MECH_ARMOR_LOCATION_RCTORSO].maxArmor);
	if (result != NO_ERROR)
		return (result);
	result = mechFile->readIdUChar("RearLeftTorso", armor[MECH_ARMOR_LOCATION_RLTORSO].maxArmor);
	if (result != NO_ERROR)
		return (result);
	result = mechFile->readIdUChar("RearRightTorso", armor[MECH_ARMOR_LOCATION_RRTORSO].maxArmor);
	if (result != NO_ERROR)
		return (result);
	result = mechFile->seekBlock("CurArmorPoints");
	if (result != NO_ERROR)
		return (result);
	uint8_t currentArmor;
	result = mechFile->readIdUChar("Head", currentArmor);
	if (result != NO_ERROR)
		return (result);
	armor[MECH_ARMOR_LOCATION_HEAD].curArmor = currentArmor;
	result = mechFile->readIdUChar("CenterTorso", currentArmor);
	if (result != NO_ERROR)
		return (result);
	armor[MECH_ARMOR_LOCATION_CTORSO].curArmor = currentArmor;
	result = mechFile->readIdUChar("LeftTorso", currentArmor);
	if (result != NO_ERROR)
		return (result);
	armor[MECH_ARMOR_LOCATION_LTORSO].curArmor = currentArmor;
	result = mechFile->readIdUChar("RightTorso", currentArmor);
	if (result != NO_ERROR)
		return (result);
	armor[MECH_ARMOR_LOCATION_RTORSO].curArmor = currentArmor;
	result = mechFile->readIdUChar("LeftArm", currentArmor);
	if (result != NO_ERROR)
		return (result);
	armor[MECH_ARMOR_LOCATION_LARM].curArmor = currentArmor;
	result = mechFile->readIdUChar("RightArm", currentArmor);
	if (result != NO_ERROR)
		return (result);
	armor[MECH_ARMOR_LOCATION_RARM].curArmor = currentArmor;
	result = mechFile->readIdUChar("LeftLeg", currentArmor);
	if (result != NO_ERROR)
		return (result);
	armor[MECH_ARMOR_LOCATION_LLEG].curArmor = currentArmor;
	result = mechFile->readIdUChar("RightLeg", currentArmor);
	if (result != NO_ERROR)
		return (result);
	armor[MECH_ARMOR_LOCATION_RLEG].curArmor = currentArmor;
	result = mechFile->readIdUChar("RearCenterTorso", currentArmor);
	if (result != NO_ERROR)
		return (result);
	armor[MECH_ARMOR_LOCATION_RCTORSO].curArmor = currentArmor;
	result = mechFile->readIdUChar("RearLeftTorso", currentArmor);
	if (result != NO_ERROR)
		return (result);
	armor[MECH_ARMOR_LOCATION_RLTORSO].curArmor = currentArmor;
	result = mechFile->readIdUChar("RearRightTorso", currentArmor);
	if (result != NO_ERROR)
		return (result);
	armor[MECH_ARMOR_LOCATION_RRTORSO].curArmor = currentArmor;
	//---------------------------------------------------------------------------
	// Build the mech's inventory (all components, and where they are
	// located)...
	result = mechFile->seekBlock("InventoryInfo");
	if (result != NO_ERROR)
		return (result);
	result = mechFile->readIdUChar("NumOther", numOther);
	if (result != NO_ERROR)
		return (result);
	result = mechFile->readIdUChar("NumWeapons", numWeapons);
	if (result != NO_ERROR)
		return (result);
	result = mechFile->readIdUChar("NumAmmo", numAmmos);
	if (result != NO_ERROR)
		return (result);
	if ((numOther + numWeapons + numAmmos) > MAX_MOVER_INVENTORY_ITEMS)
		Fatal(numOther + numWeapons + numAmmos, " Battlemech.init: too many inventory items ");
	numAntiMissileSystems = 0;
	//-----------------------------------------------------
	// Read in the mech's non-weapon/non-ammo components...
	int32_t curItem = 0;
	while (curItem < numOther)
	{
		wchar_t itemString[128];
		sprintf(itemString, "Item:%d", curItem);
		result = mechFile->seekBlock(itemString);
		if (result != NO_ERROR)
			return (result);
		result = mechFile->readIdUChar("MasterID", inventory[curItem].masterID);
		if (result != NO_ERROR)
			return (result);
		inventory[curItem].health =
			MasterComponent::masterList[inventory[curItem].masterID].getHealth();
		inventory[curItem].disabled = false;
		inventory[curItem].amount = 1;
		inventory[curItem].ammoIndex = -1;
		inventory[curItem].readyTime = 0.0;
		inventory[curItem].bodyLocation = 255;
#ifdef USEHEAT
		inventory[curItem].heatPerSec = 0.0;
#endif
		//---------------------------------------------------------------
		// If the component is a JumpJet, increment our jump jet count...
		switch (MasterComponent::masterList[inventory[curItem].masterID].getForm())
		{
		case COMPONENT_FORM_JUMPJET:
			numJumpJets = 5;
			break;
		case COMPONENT_FORM_HEATSINK:
			// maxHeatSinks++;
			break;
		}
		curItem++;
	}
	//------------------------------
	// Read in the mech's weapons...
	while (curItem < (numOther + numWeapons))
	{
		wchar_t itemString[128];
		sprintf(itemString, "Item:%d", curItem);
		result = mechFile->seekBlock(itemString);
		if (result != NO_ERROR)
			return (result);
		result = mechFile->readIdUChar("MasterID", inventory[curItem].masterID);
		if (result != NO_ERROR)
			return (result);
		result = mechFile->readIdUChar("FacesForward", inventory[curItem].facing);
		if (result != NO_ERROR)
			return (result);
		inventory[curItem].health =
			MasterComponent::masterList[inventory[curItem].masterID].getHealth();
		inventory[curItem].disabled = false;
		inventory[curItem].amount = 1;
		inventory[curItem].ammoIndex = -1;
		inventory[curItem].readyTime = 0.0;
		inventory[curItem].bodyLocation = 255;
		inventory[curItem].effectiveness =
			(int16_t)(MasterComponent::masterList[inventory[curItem].masterID].getWeaponDamage() * 10.0 / // damage over 10 seconds
				MasterComponent::masterList[inventory[curItem].masterID].getWeaponRecycleTime());
		inventory[curItem].effectiveness *=
			WeaponRanges[MasterComponent::masterList[inventory[curItem].masterID].getWeaponRange()]
						[1]
			/ 24;
		//-------------------------------------
		// Cache in the weapon special effect.
		curItem++;
	}
	//---------------------------
	// Read in the mech's ammo...
	while (curItem < (numOther + numWeapons + numAmmos))
	{
		wchar_t itemString[128];
		sprintf(itemString, "Item:%d", curItem);
		result = mechFile->seekBlock(itemString);
		if (result != NO_ERROR)
			return (result);
		result = mechFile->readIdUChar("MasterID", inventory[curItem].masterID);
		if (result != NO_ERROR)
			return (result);
		int32_t itemAmount;
		result = mechFile->readIdLong("Amount", itemAmount);
		if (result != NO_ERROR)
		{
			uint8_t itemAmount2;
			result = mechFile->readIdUChar("Amount", itemAmount2);
			if (result != NO_ERROR)
				return (result);
			else
				itemAmount = itemAmount2;
		}
		//-----------------------------------------------------------------------
		// Since the ammo amount in the profile is amount per ton, let's make
		// the amount equal to the number of missiles/bullets/whatever. If it's
		// set to 255, use the default amount per ton as defined in the
		// component table...
		if (itemAmount == -1)
			inventory[curItem].amount =
				MasterComponent::masterList[inventory[curItem].masterID].getAmmoPerTon();
		else
			inventory[curItem].amount = itemAmount;
		inventory[curItem].ammoIndex = -1;
		inventory[curItem].startAmount = inventory[curItem].amount;
		inventory[curItem].health =
			MasterComponent::masterList[inventory[curItem].masterID].getHealth();
		inventory[curItem].disabled = false;
		inventory[curItem].readyTime = 0.0;
		inventory[curItem].bodyLocation = 255;
#ifdef USEHEAT
		inventory[curItem].heatPerSec = 0.0;
#endif
		curItem++;
	}
	//------------------------------------------------------------
	// Now, read in the component layout for each body location...
	for (size_t curLocation = 0; curLocation < NUM_MECH_BODY_LOCATIONS; curLocation++)
	{
		result = mechFile->seekBlock(BodyLocationBlockString[curLocation]);
		if (result != NO_ERROR)
			return (result);
		uint8_t caseHere;
		result = mechFile->readIdUChar("CASE", caseHere);
		if (result != NO_ERROR)
			return (result);
		body[curLocation].CASE = caseHere ? true : false;
		uint8_t internalStructure;
		result = mechFile->readIdUChar("CurInternalStructure", internalStructure);
		if (result != NO_ERROR)
			return (result);
		body[curLocation].curInternalStructure = internalStructure;
		result = mechFile->readIdUChar("HotSpotNumber", body[curLocation].hotSpotNumber);
		if (result != NO_ERROR)
			return (result);
		//---------------------------------------------------------
		// Now, determine the damage state for the body location...
		float damageLevel =
			(float)body[curLocation].curInternalStructure / body[curLocation].maxInternalStructure;
		if (damageLevel <= 0.0)
			body[curLocation].damageState = IS_DAMAGE_DESTROYED;
		else if (damageLevel <= 0.5)
			body[curLocation].damageState = IS_DAMAGE_PARTIAL;
		else
			body[curLocation].damageState = IS_DAMAGE_NONE;
		int32_t numSpaces = NumLocationCriticalSpaces[curLocation];
		body[curLocation].totalSpaces = 0;
		for (size_t curSpace = 0; curSpace < numSpaces; curSpace++)
		{
			wchar_t componentString[128];
			sprintf(componentString, "Component:%d", curSpace);
			uint8_t spaceData[2];
			result = mechFile->readIdUCHARArray(componentString, spaceData, 2);
			if (result != NO_ERROR)
				return (result);
			body[curLocation].criticalSpaces[curSpace].inventoryID = spaceData[0];
			body[curLocation].criticalSpaces[curSpace].hit = spaceData[1] ? true : false;
			if (spaceData[0] < 255)
			{
#ifdef _DEBUG
				wchar_t msg[256];
				sprintf(msg, " Bad Mech Profile : %s ", mechFile->getFilename());
				Assert(spaceData[0] < numOther + numWeapons + numAmmos, spaceData[0], msg);
#endif
				inventory[spaceData[0]].bodyLocation = curLocation;
				//--------------------------------------------------------
				// The following line assumes the "new" crit hit system...
				body[curLocation].totalSpaces +=
					MasterComponent::masterList[inventory[spaceData[0]].masterID].getSize();
#if 0
				if(body[curLocation].totalSpaces > NumLocationCriticalSpaces[curLocation])
				{
					wchar_t fatalMsg[250];
					sprintf(fatalMsg, "Too Many Critical Spaces in %s", mechFile->getFilename());
					Fatal(curLocation, fatalMsg);
				}
#endif
				//--------------------------------------------------------
				// Preserve critical component indices for quick access...
				switch (MasterComponent::masterList[inventory[spaceData[0]].masterID].getForm())
				{
				case COMPONENT_FORM_COCKPIT:
					cockpit = spaceData[0];
					break;
				case COMPONENT_FORM_JUMPJET:
					jumpJets = spaceData[0];
					break;
				case COMPONENT_FORM_SENSOR:
					sensor = spaceData[0];
					sensorSystem = SensorManager->newSensor();
					sensorSystem->setOwner(this);
					sensorSystem->setRange(
						MasterComponent::masterList[inventory[sensor].masterID].getSensorRange());
					break;
				case COMPONENT_FORM_HEATSINK:
					inventory[spaceData[0]].bodyLocation = curLocation;
					break;
				case COMPONENT_FORM_AMMO:
					inventory[spaceData[0]].bodyLocation = curLocation;
					break;
				case COMPONENT_FORM_WEAPON:
				case COMPONENT_FORM_WEAPON_ENERGY:
					inventory[spaceData[0]].bodyLocation = curLocation;
					break;
				case COMPONENT_FORM_WEAPON_BALLISTIC:
					inventory[spaceData[0]].bodyLocation = curLocation;
					if ((inventory[spaceData[0]].masterID == MasterComponent::clanAntiMissileSystemID) || (inventory[spaceData[0]].masterID == MasterComponent::innerSphereAntiMissileSystemID))
					{
						//------------------------------------------------------
						// Add to Anti-Missile System list for fast reference...
						if (numAntiMissileSystems == MAX_ANTI_MISSILE_SYSTEMS)
							Fatal(0, "Too many Anti-Missile Systems");
						antiMissileSystem[numAntiMissileSystems++] = spaceData[0];
					}
					break;
				case COMPONENT_FORM_WEAPON_MISSILE:
					inventory[spaceData[0]].bodyLocation = curLocation;
					break;
				case COMPONENT_FORM_ACTUATOR:
					if (inventory[spaceData[0]].masterID == MasterComponent::armActuatorID)
					{
						if (curLocation == MECH_BODY_LOCATION_LARM)
							actuator[ACTUATOR_LSHOULDER] = spaceData[0];
						else if (curLocation == MECH_BODY_LOCATION_RARM)
							actuator[ACTUATOR_RSHOULDER] = spaceData[0];
					}
					else if (inventory[spaceData[0]].masterID == MasterComponent::legActuatorID)
					{
						if (curLocation == MECH_BODY_LOCATION_LLEG)
							actuator[ACTUATOR_LHIP] = spaceData[0];
						else if (curLocation == MECH_BODY_LOCATION_RLEG)
							actuator[ACTUATOR_RHIP] = spaceData[0];
					}
					break;
				case COMPONENT_FORM_ENGINE:
					engine = spaceData[0];
					break;
				case COMPONENT_FORM_LIFESUPPORT:
					lifeSupport = spaceData[0];
					break;
				case COMPONENT_FORM_GYROSCOPE:
					gyro = spaceData[0];
					break;
				case COMPONENT_FORM_ECM:
					ecm = spaceData[0];
					break;
				case COMPONENT_FORM_JAMMER:
					break;
				case COMPONENT_FORM_PROBE:
					probe = spaceData[0];
					break;
				}
			}
		}
	}
	calcAmmoTotals();
	for (size_t item = numOther; item < (numOther + numWeapons); item++)
	{
		//----------------------------------------------------------
		// Each weapon should point to its appropriate ammo total in
		// the ammo type total list...
		for (size_t ammoIndex = 0; ammoIndex < numAmmoTypes; ammoIndex++)
		{
			if ((int32_t)MasterComponent::masterList[inventory[item].masterID]
					.getWeaponAmmoMasterId()
				== ammoTypeTotal[ammoIndex].masterId)
			{
				inventory[item].ammoIndex = ammoIndex;
				break;
			}
		}
	}
	for (item = numOther + numWeapons; item < (numOther + numWeapons + numAmmos); item++)
	{
		//----------------------------------------------------------
		// Each weapon should point to its appropriate ammo total in
		// the ammo type total list...
		for (size_t ammoIndex = 0; ammoIndex < numAmmoTypes; ammoIndex++)
		{
			if (inventory[item].masterID == ammoTypeTotal[ammoIndex].masterId)
			{
				inventory[item].ammoIndex = ammoIndex;
				break;
			}
		}
	}
	for (item = 0; item < numOther; item++)
	{
		if ((inventory[item].masterID == MasterComponent::clanAntiMissileSystemID) || (inventory[item].masterID == MasterComponent::innerSphereAntiMissileSystemID))
		{
			for (size_t ammoIndex = 0; ammoIndex < numAmmoTypes; ammoIndex++)
			{
				if ((int32_t)MasterComponent::masterList[inventory[item].masterID]
						.getWeaponAmmoMasterId()
					== ammoTypeTotal[ammoIndex].masterId)
				{
					inventory[item].ammoIndex = ammoIndex;
					break;
				}
			}
		}
	}
#ifdef USEHEAT
	//---------------------------
	// Calc the leg heat sinks...
	curLegHeatSinks =
		calcHeatSinks(MECH_BODY_LOCATION_RLEG, true) + calcHeatSinks(MECH_BODY_LOCATION_LLEG, true);
#endif
	//------------------------------------------------------------------------------
	// Now that we've loaded inventory, let's set aside which weapon has the
	// longest range...
	//---------------------------------------------------------------------------
	// We need to set the status states for legs and torso based upon our
	// current condition...
	calcLegStatus();
	calcTorsoStatus();
	calcFireRanges();
	maxCV = calcCV(true);
	curCV = calcCV(false);
	maxWeaponDamage = calcMaxTargetDamage();
	ObjectTypePtr type = ObjectManager->getObjectType(typeHandle);
	if (type->getExplosionObject() > 0)
		ObjectManager->objTypeManager->load(type->getExplosionObject(), true);
	//-----------------------------
	// Calc mech's chassis class...
	chassisClass = getMechClass();
	return (NO_ERROR);
}

//---------------------------------------------------------------------------

int32_t
BattleMech::init(std::unique_ptr<File> mechFile)
{
	return (NO_ERROR);
}

//---------------------------------------------------------------------------

int32_t
BattleMech::write(std::unique_ptr<File> mechFile)
{
#if 0
	GameObject::write(mechFile);
	mechFile->writeString(moverName);
	mechFile->writeString(icon);
	mechFile->writeByte(chassis);
	mechFile->writeLong((int32_t)isEndoSteel);
	mechFile->writeFloat(maxTonnage);
	mechFile->writeFloat(tonsInternalStructure);
	//mechFile->writeByte(NUM_MECH_BODY_LOCATIONS); //obviously we know this...
	for(size_t i = 0; i < NUM_MECH_BODY_LOCATIONS; i++)
	{
		mechFile->writeLong((int32_t)body[i].CASE);
		mechFile->write((uint8_t*)body[i].criticalSpaces, sizeof(CriticalSpace) * NumLocationCriticalSpaces[i]);
		mechFile->writeFloat(body[i].curInternalStructure);
		mechFile->writeByte((byte)body[i].maxInternalStructure);
		mechFile->writeByte((byte)body[i].hotSpotNumber);
		//mechFile->writeByte((byte)damageState); //calc this upon load
	}
	//mechFile->writeLong(maxCV); //calc upon load
	//mechFile->writeLong(curCV); //calc upon load
	//mechFile->writeLong(fieldedCV); // only used in combat
	mechFile->writeByte((byte)armorType);
	mechFile->writeFloat(armorTonnage);
	//mechFile->writeByte(NUM_MECH_ARMOR_LOCATIONS); //again, we know this for mechs...
	mechFile->write((uint8_t*)armor, sizeof(ArmorLocation) * NUM_MECH_ARMOR_LOCATIONS);
	//--------------------------------------------------------------------------
	// NOTE: The following line will be needed once we have the Team Data Files.
	//mechFile->writeByte(pilotIndex); //points to pilot on PlayerTeam, or -1 if none
	mechFile->writeLong(numOther);
	mechFile->writeLong(numWeapons);
	mechFile->writeLong(numAmmos);
	for(i = 0; i < numOther; i++)
	{
		mechFile->writeByte((byte)inventory[i].masterID);
		mechFile->writeByte((byte)inventory[i].health);
		mechFile->writeByte((byte)(inventory[i].disabled == true));
		mechFile->writeByte((byte)inventory[i].facing);
		mechFile->writeShort(inventory[i].amount);
		mechFile->writeByte((byte)inventory[i].bodyLocation);
	}
	for(i = 0; i < numWeapons; i++)
	{
		mechFile->writeByte((byte)inventory[i].masterID);
		mechFile->writeByte((byte)inventory[i].health);
		mechFile->writeByte((byte)(inventory[i].disabled == true));
		mechFile->writeByte((byte)inventory[i].facing);
		mechFile->writeShort(inventory[i].amount);
		mechFile->writeByte((byte)inventory[i].bodyLocation);
	}
	for(i = 0; i < numAmmos; i++)
	{
		mechFile->writeByte((byte)inventory[i].masterID);
		mechFile->writeByte((byte)inventory[i].health);
		mechFile->writeByte((byte)(inventory[i].disabled == true));
		mechFile->writeByte((byte)inventory[i].facing);
		mechFile->writeShort(inventory[i].amount);
		mechFile->writeByte((byte)inventory[i].bodyLocation);
	}
	mechFile->writeByte((byte)cockpit);
	mechFile->writeByte((byte)engine);
	mechFile->writeByte((byte)lifeSupport);
	mechFile->writeByte((byte)sensor);
	mechFile->writeByte((byte)ecm);
	mechFile->writeByte((byte)probe);
	mechFile->writeByte((byte)jammer);
	mechFile->writeByte((byte)numAntiMissileSystems);
	mechFile->write((uint8_t*)antiMissileSystem, MAX_ANTI_MISSILE_SYSTEMS);
	mechFile->writeFloat(maxMoveSpeed);
	//---------------------------------------------------------------------------------
	// Do we need to preserve the object type data? Yes, at least the type id so we can
	// recreate the pointer to it upon load...
#endif
	return (NO_ERROR);
}

//---------------------------------------------------------------------------

#ifdef USEHEAT
int32_t
BattleMech::calcHeatSinks(int32_t location, bool calcCurrent)
{
	//--------------------------------------------------
	// IMPORTANT: This needs to be properly implemented!
	int32_t tally = 0;
	for (size_t item = 0; item < numOther; item++)
	{
		if (MasterComponent::masterList[inventory[item].masterID].getForm() == COMPONENT_FORM_HEATSINK)
			if (inventory[item].bodyLocation == location)
				if (!calcCurrent || !inventory[item].disabled)
					tally++;
	}
	return (tally);
}
#endif

//------------------------------------------------------------------------------------------

int32_t
BattleMech::calcCV(bool calcMax)
{
	//------------------------------------------------------------------------
	// Due to quality communication, the components have float BR ratings, but
	// the mechs have integer BR ratings. Ahhhh, love those casts...
	float totalCV = (float)chassisBR;
	//-----------------------
	// Total Component BRs...
	int32_t numComponents = numWeapons + numAmmos + numOther;
	for (size_t itemIndex = 0; itemIndex < numComponents; itemIndex++)
		if (calcMax || !inventory[itemIndex].disabled)
			totalCV += MasterComponent::masterList[inventory[itemIndex].masterID].getCV();
	return ((int32_t)totalCV);
}

//-------------------------------------------------------------------------------------------
int32_t
BattleMech::calcLegStatus(void)
{
	if (body[MECH_BODY_LOCATION_RLEG].damageState == IS_DAMAGE_DESTROYED)
	{
		if (body[MECH_BODY_LOCATION_LLEG].damageState == IS_DAMAGE_DESTROYED)
		{
			legStatus = LEG_STATUS_DESTROYED;
			if (pilot && !inRecoverUpdate)
			{
				pilot->triggerAlarm(PILOT_ALARM_VEHICLE_INCAPACITATED, 66);
			}
		}
		else
		{
			if (!sentCrippledMsg && !inRecoverUpdate)
			{
				pilot->radioMessage(RADIO_CRIPPLED);
				legStatus = LEG_STATUS_IMPAIRED_RIGHT;
				sentCrippledMsg = true;
			}
		}
	}
	else if (body[MECH_BODY_LOCATION_LLEG].damageState == IS_DAMAGE_DESTROYED)
	{
		if (!sentCrippledMsg && !inRecoverUpdate)
		{
			pilot->radioMessage(RADIO_CRIPPLED);
			legStatus = LEG_STATUS_IMPAIRED_LEFT;
			sentCrippledMsg = true;
		}
	}
	else
	{
		legStatus = LEG_STATUS_NORMAL;
	}
	return (legStatus);
}

//-------------------------------------------------------------------------------------------
int32_t
BattleMech::calcTorsoStatus(void)
{
	if (body[MECH_BODY_LOCATION_CTORSO].damageState == IS_DAMAGE_PARTIAL)
		torsoStatus = TORSO_STATUS_IMPAIRED;
	else
		torsoStatus = TORSO_STATUS_NORMAL;
	return (torsoStatus);
}

//-------------------------------------------------------------------------------------------

void BattleMech::pilotingCheck(uint32_t situation, float modifier)
{
	if (MPlayer && !MPlayer->isServer())
		return;
	if (failedPilotingCheck)
		return;
	float pilotRoll = RandomNumber(100);
	//--------------------
	// Mech's situation...
	//	if (situation & PILOTCHECK_SITUATION_JUMPING)
	//		pilotRoll += 20.0;
	if (situation & PILOTCHECK_SITUATION_COLLISION)
		pilotRoll += 20.0;
	//---------------------------------
	// If leg(s) gone, it's hopeless...
	if ((body[MECH_BODY_LOCATION_RLEG].curInternalStructure == 0) || (body[MECH_BODY_LOCATION_LLEG].curInternalStructure == 0))
		pilotRoll += 100.0;
	//-----------------
	// Is gyro damaged?
	if (gyro >= (numOther + numWeapons + numAmmos))
		STOP(("Gyro Was not loaded correctly and would have crashed here %d", gyro));
	int32_t gyroHealth = inventory[gyro].health;
	if (gyroHealth == 0)
		pilotRoll += 100.0;
	else if (gyroHealth < getInventoryMax(gyro))
		pilotRoll += 30.0;
	//---------------------------
	// Leg actuator(s) destroyed?
	if (inventory[actuator[ACTUATOR_LHIP]].health == 0)
		pilotRoll += 10.0;
	if (inventory[actuator[ACTUATOR_RHIP]].health == 0)
		pilotRoll += 10.0;
		//----------------
		// Terrain type...
		// int32_t terrainType = GameMap->getTileType(tilePosition[0],
		// tilePosition[1]);
		//-----------------
		// So, did we pass?
#if 0
	if(situation & PILOTCHECK_SITUATION_JUMPING)
	{
		failedPilotingCheck = (pilotRoll >= pilot->getSkill(MWS_JUMPING) + PilotJumpMod);
		pilot->skillPoints[MWS_JUMPING] += SkillTry[MWS_JUMPING];
		if(!failedPilotingCheck)
			pilot->skillPoints[MWS_JUMPING] += SkillSuccess[MWS_JUMPING];
	}
	else
#endif
	{
		failedPilotingCheck = (pilotRoll >= pilot->getSkill(Skill::piloting));
		pilot->skillPoints[Skill::piloting] += SkillTry[Skill::piloting];
		if (!failedPilotingCheck)
			pilot->skillPoints[Skill::piloting] += SkillSuccess[Skill::piloting];
	}
}

//-------------------------------------------------------------------------------------------

bool BattleMech::canPowerUp(void)
{
#ifdef USEHEAT
	int32_t heatIndex = (int32_t)heat;
	if (heatIndex > 0)
	{
		if (heatIndex >= NumHeatLevels)
			heatIndex = NumHeatLevels - 1;
		//-------------------------------------------
		// Check if we've cooled enough to restart...
		if (heatIndex >= heatShutdown)
			return (false);
	}
#endif
	return (true);
}

//-------------------------------------------------------------------------------------------

#ifdef USEHEAT
void BattleMech::updateHeat(void)
{
	float heatChange = 0.0;
	if (!isDisabled())
	{
		if (((MechActor*)appearance)->currentGestureId == GestureGetUp)
			heatChange += (StandUpHeat * frameLength);
		else
			heatChange += (BodyStateHeat[getBodyState()] * frameLength);
		//----------------------------------------
		// Engine damage may cause heat buildup...
		int32_t numEngineHits = getInventoryDamage(engine);
		if (numEngineHits > 2)
			numEngineHits = 2;
		heatChange += (numEngineHits * 0.6 * frameLength);
	}
	//--------------------------------------------
	// The terrain we're in may affect our heat...
	int32_t terrainHere = GameMap->getTerrain(position);
	switch (terrainHere)
	{
	case TILE_WATER /*_SHALLOW*/:
	{
		float heatSinkEffect = (curLegHeatSinks * -0.15);
		if (heatSinkType == HEATSINK_TYPE_DOUBLE)
			heatSinkEffect *= 2.0;
		if (heatSinkEffect < -0.9)
			heatSinkEffect = -0.9;
		heatChange += heatSinkEffect;
	}
	break;
	}
	//---------------------
	// Apply weapon heat...
	for (size_t weaponIndex = numOther; weaponIndex < (numOther + numWeapons); weaponIndex++)
		if (inventory[weaponIndex].readyTime > scenarioTime)
		{
			//------------------------------------------
			// Weapon is recycling, so apply the heat...
			heatChange += (inventory[weaponIndex].heatPerSec * frameLength);
		}
	//-------------------------------------------------------------------
	// Now, apply the dissipation amount (never allowing heat to go below
	// zero, of course)...
	heatChange -= (heatDissipation * frameLength * heatSinkEfficiency);
	heat += heatChange;
	if (heat < 0.0)
		heat = 0.0;
	int32_t heatIndex = (int32_t)heat;
	if (!isDisabled())
	{
		if (inventory[lifeSupport].disabled)
		{
			if (heatIndex >= HeatInjuryTable[1][0])
				pilot->injure(HeatInjuryTable[1][1] * frameLength, true);
			else if (heatIndex >= HeatInjuryTable[0][0])
				pilot->injure(HeatInjuryTable[0][1] * frameLength, true);
		}
	}
	//-------------------------------------------------------------------------
	// Now, let's make a HEAT CHECK.
	// NOTE: Disabled vehicles CAN still overheat and explode...
	if ((heatCheckTime < scenarioTime) && !isDisabled())
	{
		if (heatIndex >= NumHeatLevels)
			heatIndex = NumHeatLevels - 1;
		if (heatIndex == 0)
		{
			if (status == OBJECT_STATUS_SHUTDOWN)
			{
				if (pilot->getCurTacOrder()->code != TacticalOrderCode::powerdown)
					startUp();
			}
		}
		else if (heatIndex > 0)
		{
			if (status == OBJECT_STATUS_SHUTDOWN)
			{
				if ((pilot->getCurTacOrder()->code != TacticalOrderCode::powerdown) && (pilot->getCurTacOrder()->code != TacticalOrderCode::powerup))
				{
					//-------------------------------------------
					// Check if we've cooled enough to restart...
					if (canPowerUp())
						startUp();
				}
			}
			else
			{
				if ((status != OBJECT_STATUS_DISABLED) && !canPowerUp())
				{
					//-------------
					// Shut Down...
					shutDown();
					pilot->triggerAlarm(PILOT_ALARM_OVERHEAT, heatIndex);
				}
				//----------------------------------------------------------------------------------
				// This can't happen here this way.  No randoms!!!!!!!!!!!!!
				// This should set a flag a in ControlData to inform the update
				// to blow the ammo!! In this way, we can still play network
				// MechCommander.
				if (RandomNumber(100) < HeatEffectTable[heatIndex - 1].ammoExplosionChance)
				{
					int32_t numExplodables = 0;
					int32_t ammoList[100];
					for (size_t ammoIndex = numOther + numWeapons;
						 ammoIndex < (numOther + numWeapons + numAmmos); ammoIndex++)
					{
						if (!inventory[ammoIndex].disabled)
							ammoList[numExplodables++] = ammoIndex;
					}
					if (numExplodables > 0)
					{
						MechControlDataPtr mechCtrlData =
							(MechControlDataPtr)(control->controlData);
						mechCtrlData->blowAmmo = true;
						mechCtrlData->ammoLocation = ammoList[RandomNumber(numExplodables)];
					}
				}
			}
		}
		heatCheckTime += HeatCheckFrequency;
	}
}
#endif

//-------------------------------------------------------------------------------------------

void BattleMech::destroy(void)
{
	if (appearance)
	{
		delete appearance;
		appearance = nullptr;
	}
}

//---------------------------------------------------------------------------

int32_t
BattleMech::getResourcePointValue(void)
{
	int32_t resourcePointsForMechs[15][2] = {{30, 1500}, {35, 1750}, {40, 2000}, {45, 2250},
		{50, 3000}, {55, 3300}, {60, 3600}, {65, 3900}, {70, 4200}, {75, 5250}, {80, 5600},
		{85, 5950}, {90, 6300}, {95, 6650}, {100, 7000}};
	for (size_t i = 0; i < 15; i++)
		if (tonnage <= resourcePointsForMechs[i][0])
			return (resourcePointsForMechs[i][1]);
	return (resourcePointsForMechs[14][1]);
};

//---------------------------------------------------------------------------
void BattleMech::mineCheck(void)
{
	if (MPlayer && !MPlayer->isServer())
		return;
	if (isJumping()) // Can't set 'em off while flying!!
		return;
	// Can't have those damned helicopters setting stuff off.
	if ((getMoveLevel() == 2) && (status != OBJECT_STATUS_SHUTDOWN))
		return;
	//------------------------------------------------------------
	// Must mark the end of the path as desirable or much badness
	// NOT NEEDED ANYMORE.  Everyone can see mines all of the time.
	uint32_t mine = 0;
	mine = GameMap->getMine(cellPositionRow, cellPositionCol);
	if (mine == 1)
	{
		//---------------------------
		// Mine here, deal with it...
		if ((tonnage > 35.0f))
		{
			//---------------------------------------------------------
			//  Mark this tile as empty.  (we just set the mine off!!)
			GameMap->setMine(cellPositionRow, cellPositionCol, 2);
			if (MPlayer)
				MPlayer->addMineChunk(cellPositionRow, cellPositionCol, 1, 2, 2);
			pilot->clearCurTacOrder(); // Force the pilot to recalc based on new
				// data.
			//---------------------------
			// Mine here, deal with it...
			Stuff::Vector3D explosionPosition;
			explosionPosition = getPosition();
			ObjectManager->createExplosion(MINE_EXPLOSION_ID, nullptr, explosionPosition,
				MineSplashDamage, MineSplashRange * worldUnitsPerMeter);
			WeaponShotInfo shot;
			shot.init(
				nullptr, -2, MineDamage, calcHitLocation(nullptr, -1, ATTACKSOURCE_MINE, 0), 0);
			handleWeaponHit(&shot, (MPlayer != nullptr));
		}
	}
}

//---------------------------------------------------------------------------
// AI MOVEMENT UPDATE ROUTINES
//---------------------------------------------------------------------------

bool BattleMech::updateJump(void)
{
	//-----------------------------------------------------------------------------
	// We can turn after lifting off in a jump.
	if (!isJumping())
		return (false);
	if (appearance->isJumpSetup())
	{
		//--------------------------------
		// We should be jumping, dammit...
		if (MPlayer && !MPlayer->isServer())
		{
			//---------------------------------
			// Handled elsewhere, for client...
			if (distanceFrom(jumpGoal) > 8.0)
			{
				appearance->setJumpParameters(jumpGoal);
				appearance->setGestureGoal(6);
				control.settings.mech.throttle = 100;
			}
		}
		else
		{
			// appearance->setJumpParameters(jumpGoal);
			appearance->setGestureGoal(6);
			control.settings.mech.throttle = 100;
			return (true);
		}
	}
	else if (appearance->isJumpAirborne())
	{
		if (!playedJumpSFX)
		{
			playedJumpSFX = true;
			soundSystem->playDigitalSample(JUMPJETS, getPosition(), true);
		}
		/*
		//---------------------------------------------
		float turnRate = 0.0;
		float relFacing = relFacingTo(jumpGoal);
		if ((relFacing < -2.0) || (relFacing > 2.0))
		{
			//-----------------------------------------------
			// We can and will shift facing to destination...
			turnRate = -relFacing * frameLength;
			float maxRate = tonnageTurnRate[int32_t(tonnage)] * frameLength;
			if (turnRate < 0.0f)
				maxRate = -maxRate;

			if (turnRate > 0.0f)
			{
				if (maxRate > turnRate)
					maxRate = turnRate;
			}
			else if (turnRate < 0.0f)
			{
				if (maxRate < turnRate)
					maxRate = turnRate;
			}

		}
		*/
		// Do not rotate mech anymore in the jumps!
		// Mech's velocity take it right to point.
		control.settings.mech.rotate = 0.0f;
		return (true);
	}
	else if (!appearance->isInJump())
	{
		//------------------------------------------------------------
		// If we made it into here, then we're at the end of the jump.
		inJump = false;
		playedJumpSFX = false;
		lastJumpTime = scenarioTime;
		MovePathPtr path = pilot->getMovePath();
		// path->numSteps = path->numStepsWhenNotPaused;
		pilot->resumePath();
		path->curStep++;
		//---------------------------------------------------------
		// Let's record our last valid position, in case we need to
		// crawl back from impassable terrain we get knocked onto.
		// Ultimately, this SHOULD NOT be necessary once we agree
		// on a new gesture/movement system... gd 6/2/97
		lastValidPosition = position;
		//----------------------------------------
		// We've landed, so do a piloting check...
		pilotingCheck(PILOTCHECK_SITUATION_JUMPING);
		pilot->getCurTacOrder()->setStage(3); // We're done.
		return (true);
	}
	return (true);
}

//---------------------------------------------------------------------------

bool BattleMech::pivotTo(void)
{
	MovePathPtr path = pilot->getMovePath();
	int32_t moveState = pilot->getMoveState();
	int32_t moveStateGoal = pilot->getMoveStateGoal();
	bool isRunning = false;
	if (MPlayer && !MPlayer->isServer())
		isRunning = moveChunk.run;
	else
		isRunning = (pilot->getMovePath()->numStepsWhenNotPaused > 0) && pilot->getMoveRun();
	bool hasTarget = false;
	Stuff::Vector3D targetposition;
	GameObjectPtr target = pilot->getCurrentTarget();
	float relFacingToTarget = 0.0;
	if (target)
	{
		targetposition = target->getPosition();
		relFacingToTarget = relFacingTo(targetposition);
		hasTarget = true;
	}
	else if (pilot->getCurTacOrder()->code == TacticalOrderCode::attackpoint)
	{
		targetposition = pilot->getAttackTargetPoint();
		relFacingToTarget = relFacingTo(targetposition);
		hasTarget = true;
	}
	if (moveState == MOVESTATE_PIVOT_FORWARD)
	{
		if ((moveStateGoal == MOVESTATE_PIVOT_FORWARD) || (moveStateGoal == MOVESTATE_FORWARD))
		{
			//----------------------------------------------------------
			// We want to pivot forward (if we have a path to follow)...
			if ((path->numStepsWhenNotPaused > 0) && (path->curStep < path->numStepsWhenNotPaused))
			{
				Stuff::Vector3D wayPt = path->stepList[path->curStep].destination;
				appearance->setGestureGoal(MECH_STATE_STANDING);
				control.settings.mech.throttle = 100;
				float relFacingToWayPt = relFacingTo(wayPt);
				if ((relFacingToWayPt < -5.0) || (relFacingToWayPt > 5.0))
				{
					float turnRate = -relFacingToWayPt;
					float maxRate = tonnageTurnRate[int32_t(tonnage)] * frameLength;
					if (turnRate < 0.0f)
						maxRate = -maxRate;
					if (fabs(turnRate) > maxRate)
					{
						if (turnRate > 0)
							turnRate = maxRate;
						else
							turnRate = -maxRate;
					}
					control.settings.mech.rotate = turnRate;
					control.settings.mech.pivot = true;
					NewRotation = turnRate;
					return (true);
				}
				else
				{
					pilot->setMoveState(MOVESTATE_FORWARD);
					if (pilot->getMoveTwisting())
						pilot->setMoveTwisting(false);
				}
			}
			else
				pilot->setMoveStateGoal(MOVESTATE_FORWARD /*MOVESTATE_STAND*/);
		}
		else
			pilot->setMoveState(MOVESTATE_FORWARD /*MOVESTATE_STAND*/);
	}
	else if (moveState == MOVESTATE_PIVOT_REVERSE)
	{
		if ((moveStateGoal == MOVESTATE_PIVOT_REVERSE) || (moveStateGoal == MOVESTATE_REVERSE))
		{
			//----------------------------------------------------------
			// We want to pivot forward (if we have a path to follow)...
			if ((path->numStepsWhenNotPaused > 0) && (path->curStep < path->numStepsWhenNotPaused))
			{
				Stuff::Vector3D wayPt = path->stepList[path->curStep].destination;
				appearance->setGestureGoal(MECH_STATE_STANDING);
				control.settings.mech.throttle = 100;
				float relFacingToWayPt = relFacingTo(wayPt);
				if ((relFacingToWayPt > -175.0) && (relFacingToWayPt < 175.0))
				{
					float turnRate = 0.0;
					if (hasTarget && !isRunning)
					{
						if (pivotDirection == -1)
						{
							if (relFacingToTarget < 0)
								pivotDirection = 0;
							else
								pivotDirection = 1;
						}
						if (pivotDirection == 0)
							turnRate = 180.0 - relFacingToWayPt;
						else
							turnRate = -180.0 - relFacingToWayPt;
					}
					else
					{
						if (relFacingToWayPt < 0)
							turnRate = -180.0 - relFacingToWayPt;
						else
							turnRate = 180.0 - relFacingToWayPt;
					}
					//------------------------------------------------------------------------
					float maxRate = tonnageTurnRate[int32_t(tonnage)] * frameLength;
					if (fabs(turnRate) > maxRate)
					{
						if (turnRate > 0)
							turnRate = maxRate;
						else
							turnRate = -maxRate;
					}
					control.settings.mech.rotate = turnRate;
					control.settings.mech.pivot = true;
					NewRotation = turnRate;
					return (true);
				}
				else
				{
					//--------------------------
					// Facing reverse. Now what?
					if (pilot->getMoveTwisting())
						pilot->setMoveTwisting(false);
					if (moveStateGoal == MOVESTATE_REVERSE)
						pilot->setMoveState(MOVESTATE_REVERSE);
					else
						pilot->setMoveStateGoal(MOVESTATE_FORWARD /*MOVESTATE_STAND*/);
				}
			}
			else
				pilot->setMoveStateGoal(MOVESTATE_FORWARD /*MOVESTATE_STAND*/);
		}
		else
			pilot->setMoveState(MOVESTATE_FORWARD /*MOVESTATE_STAND*/);
	}
	else if (moveState == MOVESTATE_PIVOT_TARGET)
	{
		if (moveStateGoal == MOVESTATE_PIVOT_TARGET)
		{
			//------------------------------------------
			// We want to pivot to our current target...
			if (isRunning || !hasTarget)
			{
				pilot->setMoveStateGoal(MOVESTATE_FORWARD /*MOVESTATE_STAND*/);
				if (!pilot->isYielding() && !pilot->isWaitingForPoint())
					pilot->resumePath();
				pivotDirection = -1;
				return (false);
			}
			appearance->setGestureGoal(MECH_STATE_STANDING);
			control.settings.mech.throttle = 100;
			float fireArc = getFireArc();
			if ((relFacingToTarget < -fireArc) || (relFacingToTarget > fireArc))
			{
				float turnRate = -relFacingToTarget;
				float maxRate = tonnageTurnRate[int32_t(tonnage)] * frameLength;
				if (fabs(turnRate) > maxRate)
				{
					if (turnRate > 0)
						turnRate = maxRate;
					else
						turnRate = -maxRate;
				}
				control.settings.mech.rotate = turnRate;
				control.settings.mech.pivot = true;
				NewRotation = turnRate;
				return (true);
			}
			else
				pilot->setMoveStateGoal(MOVESTATE_FORWARD /*MOVESTATE_STAND*/);
		}
		else
			pilot->setMoveState(MOVESTATE_FORWARD /*MOVESTATE_STAND*/);
	}
	else
	{
		//--------------------------------------------------------
		// If we're supposed to be pivoting, set the move state...
		if ((moveStateGoal == MOVESTATE_PIVOT_TARGET) || (moveStateGoal == MOVESTATE_PIVOT_FORWARD) || (moveStateGoal == MOVESTATE_PIVOT_REVERSE))
			pilot->setMoveState(moveStateGoal);
	}
	if (!pilot->isYielding() && !pilot->isWaitingForPoint())
		pilot->resumePath();
	pivotDirection = -1;
	return (false);
}

//---------------------------------------------------------------------------

int32_t
BattleMech::getSpeedState(void)
{
	return (mechSpeedStateArray[appearance->getCurrentGestureId()]);
}

//---------------------------------------------------------------------------

void BattleMech::updateMoveStateGoal(void)
{
	//----------------------------------------------------------------
	// Have we reached the destination? Make sure we're looking at the
	// right step in the path...
	MovePathPtr path = pilot->getMovePath();
#if 0
	if(path->numSteps > 0)
	{
		Stuff::Vector3D wayPt = path->stepList[path->curStep].destination;
		float distanceFromWayPt = distanceFrom(wayPt);
		float cushion = Mover::marginOfError[0];
		if(path->curStep == (path->numSteps - 1))
			cushion = Mover::marginOfError[1];
		if(distanceFromWayPt < cushion)
		{
			//-------------------------------------------
			// Reached it, so go to the next waypoint...
			//pilot->setMoveTimeOfLastStep(scenarioTime);
			if((path->curStep + 1) < path->numSteps)
			{
				int32_t curDir = path->stepList[path->curStep + 1].direction;
				if(curDir > 7)
				{
					//--------------------------
					// Jump to next path step...
					//newGestureStateGoal = 6;
					//return(false);
				}
				else
					wayPt = path->stepList[path->curStep + 1].destination;
			}
		}
	}
#endif
	Stuff::Vector3D targetposition;
	targetposition.Zero();
	GameObjectPtr target = pilot->getLastTarget();
	bool hasTarget = false;
	if (target)
	{
		targetposition = target->getPosition();
		hasTarget = true;
	}
	else if (pilot->getCurTacOrder()->code == TacticalOrderCode::attackpoint)
	{
		targetposition = pilot->getAttackTargetPoint();
		hasTarget = true;
	}
	int32_t moveStateGoal = pilot->getMoveStateGoal();
	if (path->numSteps > 0)
	{
		if (MPlayer && !MPlayer->isServer())
		{
			if (moveChunk.run || (legStatus == LEG_STATUS_IMPAIRED_LEFT) || (legStatus == LEG_STATUS_IMPAIRED_RIGHT))
			{
				pilot->setMoveStateGoal(MOVESTATE_FORWARD);
				return;
			}
		}
		else
		{
			if (pilot->getMoveRun() || (legStatus == LEG_STATUS_IMPAIRED_LEFT) || (legStatus == LEG_STATUS_IMPAIRED_RIGHT))
			{
				pilot->setMoveStateGoal(MOVESTATE_FORWARD);
				return;
			}
		}
		//----------------------------------------
		// Facing choice for movement goes here...
		if (hasTarget)
		{
			//------------------------------------------------
			// Check if we want to go forward or in reverse...
			if ((path->numStepsWhenNotPaused > 0) && (path->curStep < path->numStepsWhenNotPaused))
			{
				Stuff::Vector3D wayPt = path->stepList[path->curStep].destination;
				float facingDelta = relFacingDelta(wayPt, targetposition);
				float totalFireArc = getFireArc() + dynamics.max.mech.torsoYaw;
				int32_t moveStateGoal = pilot->getMoveStateGoal();
				if (moveStateGoal == MOVESTATE_FORWARD)
				{
					if (facingDelta > totalFireArc)
					{
						if ((180.0 - facingDelta) <= totalFireArc)
						{
							if (!pilot->getMoveTwisting())
							{
								pilot->setMoveTwisting(true);
								pilot->setMoveStateGoal(MOVESTATE_REVERSE);
							}
						}
					}
				}
				else
				{
					if ((180.0 - facingDelta) > totalFireArc)
					{
						if (facingDelta <= totalFireArc)
						{
							if (!pilot->getMoveTwisting())
							{
								pilot->setMoveTwisting(true);
								pilot->setMoveStateGoal(MOVESTATE_FORWARD);
							}
						}
					}
				}
			}
		}
		else
			pilot->setMoveStateGoal(MOVESTATE_FORWARD);
	}
	else if ((moveStateGoal != MOVESTATE_PIVOT_TARGET) && (moveStateGoal != MOVESTATE_PIVOT_FORWARD) && (moveStateGoal != MOVESTATE_PIVOT_REVERSE))
	{
		if (!hasTarget && (path->numStepsWhenNotPaused == 0))
			pilot->setMoveStateGoal(MOVESTATE_FORWARD);
	}
}

//---------------------------------------------------------------------------

#define DEBUG_MECH_UPDATE 0
float DistanceToWaypoint;

bool BattleMech::updateMovePath(float& newRotate, wchar_t& newThrottleSetting,
	int32_t& newGestureStateGoal, int32_t& newMoveState, int32_t& minThrottle, int32_t& maxThrottle,
	float& facingRotate)
{
	DistanceToWaypoint = 9999.0;
	MovePathPtr path = pilot->getMovePath();
	TacticalOrderPtr curOrder = pilot->getCurTacOrder();
	bool allowedToRun = canRun() && pilot->getMoveRun();
	int32_t curThrottleSetting = control.settings.mech.throttle;
	newThrottleSetting = curThrottleSetting;
	newRotate = 0.0;
#if DEBUG_MECH_UPDATE
	if (selected)
	{
		int32_t area = GlobalMoveMap->calcArea(objPosition->tileRow, objPosition->tileCol);
		wchar_t debugStr[256];
		sprintf(debugStr, "Area = %d\n", area);
		OutputDebugString(debugStr);
	}
#endif
	updateHustleTime();
	bool hustle = (lastHustleTime + 2.0) > scenarioTime;
	//-----------------------------------------------
	// Am I ahead of my point vehicle, if I have one?
	bool aheadOfPointVehicle = false;
	bool stopForPointVehicle = false;
	std::unique_ptr<Mover> pointVehicle = (std::unique_ptr<Mover>)pilot->getPoint();
	bool hasGroupMoveOrder = (curOrder->isGroupOrder() && curOrder->isMoveOrder());
	if (!allowedToRun && !hustle && pointVehicle && !pointVehicle->isDisabled() && (pointVehicle != this) && hasGroupMoveOrder)
	{
		std::unique_ptr<MechWarrior> pointPilot = pointVehicle->getPilot();
		float pointDistanceFromGoal = pointPilot->getMoveDistanceLeft();
		float myDistanceFromGoal = pilot->getMoveDistanceLeft();
		aheadOfPointVehicle = (myDistanceFromGoal < pointDistanceFromGoal);
		if (aheadOfPointVehicle)
		{
			allowedToRun = false;
			if (getSpeedState() == SPEED_STATE_MOVING_FAST)
			{
				//-------------------------------------------------
				// Running. So, slow to a walk for a few seconds...
				if (!pilot->isWaitingForPoint())
					pilot->setMoveWaitForPointTime(scenarioTime + 5.0);
			}
			else
			{
				//--------------------------------------------------------------
				// Already walking, so let's just stop until point catches up...
				if (pilot->getMoveWaitForPointTime() < scenarioTime)
				{
					stopForPointVehicle = true;
					pilot->pausePath(); // path->numSteps = 0;
					pilot->setMoveWaitForPointTime(999999.0);
				}
			}
		}
		else
		{
			//-------------------
			// No need to wait...
			pilot->setMoveWaitForPointTime(-1.0);
			if (!pilot->isYielding())
			{
				pilot->resumePath();
			}
		}
	}
	else
	{
		pilot->setMoveWaitForPointTime(-1.0);
	}
	//-----------------------------------------------------------------------------
	// May want to call separate updateMovement() routines based upon the
	// legStatus to begin with. For now, we'll handle all cases here...
	bool goalReached = false;
	if ((legStatus == LEG_STATUS_NORMAL) || (legStatus == LEG_STATUS_HURTING) || (legStatus == LEG_STATUS_IMPAIRED_LEFT) || (legStatus == LEG_STATUS_IMPAIRED_RIGHT))
	{
		if (path->numSteps > 0)
		{
			//---------------------------------------------------------------
			// First, make sure we are not already at the end of this path...
			if (path->curStep == path->numSteps)
				goalReached = true;
			else
			{
				Stuff::Vector3D wayPt = path->stepList[path->curStep].destination;
				//---------------------------------------------------------
				// Let's record our last valid position, in case we need to
				// crawl back from impassable terrain we get knocked onto.
				// Ultimately, this SHOULD NOT be necessary once we agree
				// on a new gesture/movement system... gd 6/2/97
				lastValidPosition = wayPt;
				//---------------------------------
				// Have we reached the destination?
				float distanceFromWayPt = distanceFrom(wayPt);
				DistanceToWaypoint = distanceFromWayPt;
				//--------------------------------------------------
				// Calculate how far the mech will move this frame.
				// Vel is in m/s
				float vel = appearance->getVelocityMagnitude();
				float distanceThisFrame = vel * frameLength;
				float cushion = Mover::marginOfError[0];
				if (path->curStep == (path->numSteps - 1))
					cushion = Mover::marginOfError[1];
				if (cushion < distanceThisFrame)
				{
					//------------------------------------------------------------------------
					// We are going to move farther then the current cushion
					// would allow for.
					// Make the cushion larger.
					// Maybe we should make cushion this value all of the time?
					cushion = distanceThisFrame;
				}
				if (distanceFromWayPt < cushion)
				{
					//-------------------------------------------
					// Reached it, so go to the next waypoint...
					path->curStep++;
					pilot->setMoveTimeOfLastStep(scenarioTime);
					if (path->curStep < path->numSteps)
					{
						int32_t curDir = path->stepList[path->curStep].direction;
						if (curDir > 7)
						{
							//--------------------------
							// Jump to next path step...
							newGestureStateGoal = 6;
							return (false);
						}
						else
							wayPt = path->stepList[path->curStep].destination;
					}
					else
						goalReached = true;
				}
				else
				{
					//-------------------------------------
					// Not there yet. Should we be jumping?
					int32_t curDir = path->stepList[path->curStep].direction;
					if (curDir > 7)
					{
						//--------------------------
						// Jump to next path step...
						newGestureStateGoal = 6;
						return (false);
					}
				}
				// MaxVelocityMag = distanceFromWayPt * worldUnitsPerMeter;
				if (!goalReached)
				{
					//---------------------------------------------
					// First, rotate the mech's body (if moving)...
					float relFacingToWayPt = relFacingTo(wayPt, MECH_BODY_LOCATION_LLEG);
					int32_t moveState = pilot->getMoveState();
					int32_t moveStateGoal = pilot->getMoveStateGoal();
					if (moveState == MOVESTATE_FORWARD)
					{
						if (moveStateGoal == MOVESTATE_FORWARD)
						{
							//----------------------
							// Keep going forward...
							if (legStatus == LEG_STATUS_IMPAIRED_LEFT)
							{
								newGestureStateGoal = MECH_STATE_LIMPING_LEFT;
							}
							else if (legStatus == LEG_STATUS_IMPAIRED_RIGHT)
							{
								newGestureStateGoal = MECH_STATE_LIMPING_RIGHT;
							}
							else if (allowedToRun)
							{
								newGestureStateGoal = MECH_STATE_RUNNING;
							}
							else
								newGestureStateGoal = MECH_STATE_WALKING;
							if ((relFacingToWayPt < -5.0) || (relFacingToWayPt > 5.0))
							{
								//-----------------------------------------------
								// We can and will shift facing to
								// destination... Don't know that we need to
								// force walk to turn!! -fs
								newRotate = -relFacingToWayPt;
								float maxRate = tonnageTurnRate[int32_t(tonnage)] * frameLength;
								if (fabs(newRotate) > maxRate)
								{
									if (fabs(newRotate) < 50.0f)
										newThrottleSetting = 100.0f - fabs(newRotate);
									else
										newThrottleSetting = 0.0f;
									if (newRotate > 0.0)
										newRotate = maxRate;
									else
										newRotate = -maxRate;
								}
								else
								{
									//------------------------------------
									// Try to attain current goal speed...
									newThrottleSetting = pilot->calcMoveSpeedThrottle(
										getBodyState(), control.settings.mech.throttle);
								}
							}
							else
								newThrottleSetting = 100.0f;
						}
						else if (moveStateGoal == MOVESTATE_REVERSE)
						{
							//---------------------------------------------------
							// Stop, and pivot fully reverse to next path
							// step...
							//((MechActor*)appearance)->forceStop();
							pilot->pausePath(); // pilot->getMovePath()->numSteps
								// = 0;
							newMoveState = MOVESTATE_PIVOT_REVERSE;
						}
						else if (moveStateGoal == MOVESTATE_PIVOT_FORWARD)
						{
							//---------------------------------------------------
							// Stop, and pivot fully forward to next path
							// step...
							//((MechActor*)appearance)->forceStop();
							pilot->pausePath(); // pilot->getMovePath()->numSteps
								// = 0;
							newMoveState = MOVESTATE_PIVOT_FORWARD;
						}
						else if (moveStateGoal == MOVESTATE_PIVOT_REVERSE)
						{
							//---------------------------------------------------
							// Stop, and pivot fully reverse to next path
							// step...
							//((MechActor*)appearance)->forceStop();
							pilot->pausePath(); // pilot->getMovePath()->numSteps
								// = 0;
							newMoveState = MOVESTATE_PIVOT_REVERSE;
						}
						else
						{
							//--------
							// Stop...
							//((MechActor*)appearance)->forceStop();
							pilot->pausePath(); // pilot->getMovePath()->numSteps
								// = 0;
							newMoveState = MOVESTATE_FORWARD /*MOVESTATE_STAND*/;
						}
					}
					else if (moveState == MOVESTATE_REVERSE)
					{
						if (moveStateGoal == MOVESTATE_FORWARD)
						{
							//---------------------------------------------------
							// Stop, and pivot fully forward to next path
							// step...
							//((MechActor*)appearance)->forceStop();
							pilot->pausePath(); // pilot->getMovePath()->numSteps
								// = 0;
							newMoveState = MOVESTATE_PIVOT_FORWARD;
						}
						else if (moveStateGoal == MOVESTATE_REVERSE)
						{
							newGestureStateGoal = MECH_STATE_REVERSE;
							//--------------------------
							// Keep moving in reverse...
							if (relFacingToWayPt < 0)
								newRotate = -(relFacingToWayPt + 180.0);
							else
								newRotate = -(relFacingToWayPt - 180.0);
							//-----------------------------------------------
							// We can and will shift facing to destination...
							// Don't know that we need to force walk to turn!!
							// -fs
							float maxRate = tonnageTurnRate[int32_t(tonnage)] * frameLength;
							if (fabs(newRotate) > maxRate)
							{
								if (fabs(newRotate) < 50.0f)
									newThrottleSetting = 100.0f - fabs(newRotate);
								else
									newThrottleSetting = 0.0f;
								if (newRotate > 0.0)
									newRotate = maxRate;
								else
									newRotate = -maxRate;
							}
							else
							{
								//------------------------------------
								// Try to attain current goal speed...
								newThrottleSetting = pilot->calcMoveSpeedThrottle(
									getBodyState(), control.settings.mech.throttle);
							}
						}
						else if (moveStateGoal == MOVESTATE_PIVOT_FORWARD)
						{
							//---------------------------------------------------
							// Stop, and pivot fully forward to next path
							// step...
							//((MechActor*)appearance)->forceStop();
							pilot->pausePath(); // pilot->getMovePath()->numSteps
								// = 0;
							newMoveState = MOVESTATE_PIVOT_FORWARD;
						}
						else if (moveStateGoal == MOVESTATE_PIVOT_REVERSE)
						{
							//---------------------------------------------------
							// Stop, and pivot fully reverse to next path
							// step...
							//((MechActor*)appearance)->forceStop();
							pilot->pausePath(); // pilot->getMovePath()->numSteps
								// = 0;
							newMoveState = MOVESTATE_PIVOT_REVERSE;
						}
						else
						{
							//--------
							// Stop...
							//((MechActor*)appearance)->forceStop();
							pilot->pausePath(); // pilot->getMovePath()->numSteps
								// = 0;
							newMoveState = MOVESTATE_FORWARD /*MOVESTATE_STAND*/;
						}
					}
					else
					{
						//--------------------------
						// Not moving--should we be?
						if ((moveStateGoal == MOVESTATE_FORWARD) || (moveStateGoal == MOVESTATE_PIVOT_FORWARD))
						{
							//((MechActor*)appearance)->forceStop();
							pilot->pausePath(); // pilot->getMovePath()->numSteps
								// = 0;
							newMoveState = MOVESTATE_PIVOT_FORWARD;
						}
						else if ((moveStateGoal == MOVESTATE_REVERSE) || (moveStateGoal == MOVESTATE_PIVOT_REVERSE))
						{
							//((MechActor*)appearance)->forceStop();
							pilot->pausePath(); // pilot->getMovePath()->numSteps
								// = 0;
							newMoveState = MOVESTATE_PIVOT_REVERSE;
						}
					}
				}
			}
		}
		else
		{
			//-------------------------------------------
			// We better not be walking or running, then!
			newGestureStateGoal = MECH_STATE_STANDING;
		}
	}
	else
	{
		//---------------------
		// Cannot move, period!
		newGestureStateGoal = MECH_STATE_STANDING;
	}
	if (goalReached)
	{
		//--------------------------------------------------
		// Did we finish just a local part of a global path?
		if (pilot->getMovePathType() == MOVEPATH_COMPLEX)
		{
			if (pilot->getMovePathGlobalStep() < (pilot->getMoveNumGlobalSteps() - 1))
			{
				//-----------------------------------------------------------
				// We've reached the end of our current int16_t-range path, but
				// we have more to go before we've reached our int32_t-range
				// goal, so calc our next int16_t-range path...
				goalReached = false;
			}
		}
		pilot->clearMovePath(0);
	}
	return (goalReached);
}

//---------------------------------------------------------------------------

void BattleMech::setNextMovePath(wchar_t& newThrottleSetting, int32_t& newGestureStateGoal)
{
	//----------------------------------------
	// If this is only an intermediate path,
	// let's check where we need to go next...
	// pilot->clearMovePath(OrderType::current);
	Stuff::Vector3D nextWayPoint;
	bool haveWayPoint = pilot->getNextWayPoint(nextWayPoint, true);
	if (haveWayPoint)
	{
		pilot->setMoveGoal(MOVEGOAL_LOCATION, &nextWayPoint);
		TacticalOrderPtr curTacOrder = pilot->getCurTacOrder();
		pilot->requestMovePath(
			curTacOrder->selectionindex, MOVEPARAM_FACE_TARGET /*+MOVEPARAM_INIT*/, 1);
	}
	else
	{
		//-----------------------------------------------------------
		// This may be a hack--do we want to be able to follow moving
		// objects that we don't necessarily have tracked? We will
		// for now...
		// GameObjectPtr moveGoalObject = nullptr;
		// int32_t goalType = pilot->getMoveGoal(OrderType::current, nullptr,
		// &moveGoalObject);  pilot->setMoveGoal(OrderType::current, MOVEGOAL_NONE,
		// nullptr);
		pilot->clearMoveOrders();
		newGestureStateGoal = MECH_STATE_STANDING;
	}
}

//---------------------------------------------------------------------------

void BattleMech::updateTorso(float newRotatePerSec)
{
	//---------------------------------------------------
	// Now, rotate the torso toward our current target...
	float torsoRelFacing = 0.0;
	GameObjectPtr target = pilot->getCurrentTarget();
	rotateValues[0] = target ? relFacingTo(target->getPosition()) : 0.0;
	rotateValues[1] = torsoRotation;
	rotateValues[2] = newRotatePerSec;
	if (target)
		torsoRelFacing = relFacingTo(target->getPosition()) + torsoRotation + newRotatePerSec;
	else if (pilot->getCurTacOrder()->code == TacticalOrderCode::attackpoint)
		torsoRelFacing =
			relFacingTo(pilot->getAttackTargetPoint()) + torsoRotation + newRotatePerSec;
	else
		torsoRelFacing = torsoRotation;
	rotateValues[3] = torsoRelFacing;
	// if (torsoRelFacing < -180.0)
	//	torsoRelFacing += 360.0;
	// else if (torsoRelFacing > 180)
	//	torsoRelFacing -= 360.0;
	rotateValues[4] = torsoRelFacing;
	// if (torsoRelFacing < -180.0)
	//	Assert(0, 0, "oops");
	// else if (torsoRelFacing > 180)
	//	Assert(0, 0, "oops");
	float fireArc = getFireArc();
	if ((torsoRelFacing < -fireArc) || (torsoRelFacing > fireArc))
	{
		float turnRate = -torsoRelFacing;
		rotateValues[5] = turnRate;
		//-----------------------------------------------
		// We can and will shift facing to destination...
		float maxRate = (float)dynamics.max.mech.torsoYawRate * frameLength;
		if (fabs(turnRate) > maxRate)
		{
			if (turnRate < 0.0)
				turnRate = -maxRate;
			else
				turnRate = maxRate;
		}
		control.settings.mech.rotateTorso = turnRate;
	}
	/*
		if (target && CombatLog) {
			static wchar_t moveStateChar[NUM_MOVESTATES] = {' ', ' ', ' ', '*',
	   '*', '*'}; static wchar_t s[512]; sprintf(s, "TORSO ROTATE: %-15s %-06d
	   %-05d %-5c, relF = %-06d    torR = %06d   nRot = %06d torRF = %06d(%06d)
	   turnR = %06d", getName(), target->getWatchID(), turn,
				moveStateChar[getMoveState()],
				(int32_t)rotateValues[0],
				(int32_t)rotateValues[1],
				(int32_t)rotateValues[2],
				(int32_t)rotateValues[3],
				(int32_t)rotateValues[4],
				(int32_t)rotateValues[5]);
			CombatLog->write(s);
			CombatLog->write(" ");
		}
	*/
}

//---------------------------------------------------------------------------

void BattleMech::setControlSettings(float& newRotate, wchar_t& newThrottleSetting,
	int32_t& newGestureStateGoal, int32_t& minThrottle, int32_t& maxThrottle, float& facingRotate)
{
	//-----------------------------------------
	// Clean up and handle any jump settings...
	if (inJump && !appearance->isInJump())
	{
		inJump = false;
		// MovePathPtr path = pilot->getMovePath();
		// path->numSteps = path->numStepsWhenNotPaused;
		pilot->resumePath();
	}
	if (newGestureStateGoal == 6)
	{
		MovePathPtr path = pilot->getMovePath();
		pilot->pausePath(); // path->numSteps = 0;
		jumpGoal = path->stepList[path->curStep].destination;
		appearance->setJumpParameters(jumpGoal);
	}
	if (MPlayer && !MPlayer->isServer())
	{
		if (statusChunk.jumpOrder && !inJump)
		{
			land->cellToWorld(statusChunk.targetCellRC[0], statusChunk.targetCellRC[1], jumpGoal);
			if (distanceFrom(jumpGoal) > 8.0)
			{
				newGestureStateGoal = 6;
				appearance->setJumpParameters(jumpGoal);
			}
		}
	}
	else
	{
		if (pilot->getCurTacOrder()->isJumpOrder() && !inJump)
		{
			newGestureStateGoal = 6;
			jumpGoal.x = pilot->getCurTacOrder()->moveparams.wayPath.points[0];
			jumpGoal.y = pilot->getCurTacOrder()->moveparams.wayPath.points[1];
			jumpGoal.z = pilot->getCurTacOrder()->moveparams.wayPath.points[2];
			appearance->setJumpParameters(jumpGoal);
		}
	}
	if (newGestureStateGoal != -1)
	{
		control.settings.mech.gestureGoal = newGestureStateGoal;
		// appearance->setGestureGoal(newGestureStateGoal);
		switch (newGestureStateGoal)
		{
		case 6:
			control.settings.mech.throttle = 100.0f;
			inJump = true;
			break;
		case MECH_STATE_WALKING:
			if (newThrottleSetting != -1)
			{
				if (newThrottleSetting < minThrottle)
					newThrottleSetting = minThrottle;
				else if (newThrottleSetting > maxThrottle)
					newThrottleSetting = maxThrottle;
				control.settings.mech.throttle = newThrottleSetting;
			}
			break;
		default:
			control.settings.mech.throttle = newThrottleSetting;
		}
	}
	if (newRotate != 0.0f)
		control.settings.mech.rotate = newRotate;
	if (facingRotate != 0.0f)
		control.settings.mech.facingRotate = facingRotate;
}

//---------------------------------------------------------------------------
void BattleMech::startShutDown(void)
{
	appearance->setGesture(0); // Force us to parked
	shutDownThisFrame = false;
	startUpThisFrame = false;
	setStatus(OBJECT_STATUS_SHUTDOWN);
	sensorSystem->setShutdown(true);
}

//---------------------------------------------------------------------------

void BattleMech::updateMovement(void)
{
	int32_t minThrottle = 35;
	int32_t maxThrottle = 100;
	NewRotation = 0.0;
	if (disableThisFrame)
	{
		int32_t fallGesture =
			RandomNumber(2) ? MECH_STATE_FALLEN_FORWARD : MECH_STATE_FALLEN_BACKWARD;
		if (hitFromBehindThisFrame)
			fallGesture = MECH_STATE_FALLEN_FORWARD;
		else if (hitFromFrontThisFrame)
			fallGesture = MECH_STATE_FALLEN_BACKWARD;
		appearance->setGestureGoal(fallGesture);
		//---------------------------------------------------------
		// If we just switched to running, we need to make sure the
		// throttle is set to max...
		disableThisFrame = false;
		//------------------------------------------------------
		// In case we shut down this frame, as well, clear it...
		shutDownThisFrame = false;
		startUpThisFrame = false;
		hitFromFrontThisFrame = false;
		hitFromBehindThisFrame = false;
		control.settings.mech.throttle = maxThrottle;
		return;
	}
	if (shutDownThisFrame)
	{
		// Confusing, but is used to display the helicopter dust cloud.
		// Could also use as a fall dust cloud?
		if (getMoveType() == MOVETYPE_AIR)
			appearance->playEjection();
		appearance->setGestureGoal(MECH_STATE_PARKED);
		if ((scenarioTime > 8.0) || (getTeam() == Team::home))
		{
			if (getMoveType() != MOVETYPE_AIR)
				soundSystem->playDigitalSample(POWERDOWN_SFX, getPosition());
			else
				soundSystem->playDigitalSample(COPTER_POWERUP, getPosition());
		}
		shutDownThisFrame = false;
		startUpThisFrame = false;
		setStatus(OBJECT_STATUS_SHUTDOWN);
		control.settings.mech.throttle = maxThrottle;
		sensorSystem->setShutdown(true);
		return;
	}
	int32_t gID = appearance->getCurrentGestureId();
	if (pilot && pilot->getCurTacOrder()->code == TacticalOrderCode::powerup)
	{
		if (gID != 2)
			appearance->setGestureGoal(MECH_STATE_STANDING);
		else // Otherwise, we were ordered to powerUp and we are.  Clear iT!!
			pilot->clearCurTacOrder();
	}
	if (startUpThisFrame)
	{
		// Confusing, but is used to display the helicopter dust cloud.
		// Could also use as a fall dust cloud?
		if (getMoveType() == MOVETYPE_AIR)
			appearance->playEjection();
		if (getMoveType() != MOVETYPE_AIR)
			soundSystem->playDigitalSample(POWERUP_SFX, getPosition());
		else
			soundSystem->playDigitalSample(COPTER_POWERDN, getPosition());
		startUpThisFrame = false;
		shutDownThisFrame = false;
		setStatus(OBJECT_STATUS_NORMAL);
		control.settings.mech.throttle = maxThrottle;
		if (useSound && !MPlayer && Team::home->isEnemy(getTeam()) && (getMoveType() != MOVETYPE_AIR))
			soundSystem->playBettySample(BETTY_POWERUP);
		sensorSystem->setShutdown(false);
		return;
	}
	if ((status == OBJECT_STATUS_SHUTTING_DOWN) || (status == OBJECT_STATUS_SHUTDOWN) || (status == OBJECT_STATUS_DISABLED))
		return;
	if (isCaptured())
		return;
	if (engineBlowTime > -1.0)
		return;
	if (failedPilotingCheck && (getMoveType() != MOVETYPE_AIR))
	{
		int32_t fallGesture =
			RandomNumber(2) ? MECH_STATE_FALLEN_FORWARD : MECH_STATE_FALLEN_BACKWARD;
		if (hitFromBehindThisFrame)
			fallGesture = MECH_STATE_FALLEN_FORWARD;
		else if (hitFromFrontThisFrame)
			fallGesture = MECH_STATE_FALLEN_BACKWARD;
		appearance->setGestureGoal(fallGesture);
		failedPilotingCheck = false;
		control.settings.mech.throttle = maxThrottle;
		return;
	}
	if (updateJump())
		return;
	if (pivotTo())
		return;
	float newRotate = 0.0;
	float facingRotate = 0.0f;
	int32_t newGestureStateGoal = -1;
	int32_t newMoveState = -1;
	wchar_t newThrottleSetting = -1;
	bool goalReached = false;
	goalReached = updateMovePath(newRotate, newThrottleSetting, newGestureStateGoal, newMoveState,
		minThrottle, maxThrottle, facingRotate);
	if (goalReached)
		setNextMovePath(newThrottleSetting, newGestureStateGoal);
	if (newMoveState != -1)
		pilot->setMoveState(newMoveState);
	setControlSettings(
		newRotate, newThrottleSetting, newGestureStateGoal, minThrottle, maxThrottle, facingRotate);
	updateMoveStateGoal();
	NewRotation = newRotate;
}

//---------------------------------------------------------------------------
// NETWORK MOVEMENT UPDATE ROUTINES
//---------------------------------------------------------------------------

bool BattleMech::netUpdateMovePath(float& newRotate, wchar_t& newThrottleSetting,
	int32_t& newGestureStateGoal, int32_t& newMoveState, int32_t& minThrottle, int32_t& maxThrottle)
{
	DistanceToWaypoint = 9999.0;
	MovePathPtr path = pilot->getMovePath();
	bool allowedToRun = canRun() && moveChunk.run;
	int32_t curThrottleSetting = control.settings.mech.throttle;
	newThrottleSetting = curThrottleSetting;
	newRotate = 0.0;
	bool goalReached = false;
	if ((legStatus == LEG_STATUS_NORMAL) || (legStatus == LEG_STATUS_HURTING) || (legStatus == LEG_STATUS_IMPAIRED_LEFT) || (legStatus == LEG_STATUS_IMPAIRED_RIGHT))
	{
		if (path->numSteps > 0)
		{
			//---------------------------------------------------------------
			// First, make sure we are not already at the end of this path...
			if (path->curStep == path->numSteps)
				goalReached = true;
			else
			{
				Stuff::Vector3D wayPt = path->stepList[path->curStep].destination;
				//---------------------------------------------------------
				// Let's record our last valid position, in case we need to
				// crawl back from impassable terrain we get knocked onto.
				// Ultimately, this SHOULD NOT be necessary once we agree
				// on a new gesture/movement system... gd 6/2/97
				lastValidPosition = wayPt;
				//---------------------------------
				// Have we reached the destination?
				float distanceFromWayPt = distanceFrom(wayPt);
				DistanceToWaypoint = distanceFromWayPt;
				float cushion = Mover::marginOfError[0];
				if (path->curStep == (path->numSteps - 1))
					cushion = Mover::marginOfError[1];
				if (distanceFromWayPt < cushion)
				{
					//-------------------------------------------
					// Reached it, so go to the next waypoint...
					path->curStep++;
					pilot->setMoveTimeOfLastStep(scenarioTime);
					if (path->curStep < path->numSteps)
					{
						int32_t curDir = path->stepList[path->curStep].direction;
						if (!MPlayer && (curDir > 7))
						{
							//--------------------------
							// Jump to next path step...
							newGestureStateGoal = 6;
							return (false);
						}
						else
							wayPt = path->stepList[path->curStep].destination;
					}
					else
						goalReached = true;
				}
				else
				{
					//-------------------------------------
					// Not there yet. Should we be jumping?
					int32_t curDir = path->stepList[path->curStep].direction;
					if (!MPlayer && (curDir > 7))
					{
						//--------------------------
						// Jump to next path step...
						newGestureStateGoal = 6;
						return (false);
					}
				}
				// MaxVelocityMag = distanceFromWayPt * worldUnitsPerMeter;
				if (!goalReached)
				{
					//---------------------------------------------
					// First, rotate the mech's body (if moving)...
					float relFacingToWayPt = relFacingTo(wayPt, MECH_BODY_LOCATION_LLEG);
					int32_t moveState = pilot->getMoveState();
					int32_t moveStateGoal = pilot->getMoveStateGoal();
					if (moveState == MOVESTATE_FORWARD)
					{
						if (moveStateGoal == MOVESTATE_FORWARD)
						{
							//----------------------
							// Keep going forward...
							if (legStatus == LEG_STATUS_IMPAIRED_LEFT)
							{
								newGestureStateGoal = MECH_STATE_LIMPING_LEFT;
								newThrottleSetting = 100;
							}
							else if (legStatus == LEG_STATUS_IMPAIRED_RIGHT)
							{
								newGestureStateGoal = MECH_STATE_LIMPING_RIGHT;
								newThrottleSetting = 100;
							}
							else if (allowedToRun)
							{
								newThrottleSetting = 100;
								newGestureStateGoal = MECH_STATE_RUNNING;
							}
							else
								newGestureStateGoal = MECH_STATE_WALKING;
							if ((relFacingToWayPt < -5.0) || (relFacingToWayPt > 5.0))
							{
								//-----------------------------------------------
								// We can and will shift facing to
								// destination... Don't know that we need to
								// force walk to turn!! -fs
								newRotate = -relFacingToWayPt;
								float maxRate = tonnageTurnRate[int32_t(tonnage)] * frameLength;
								if (fabs(newRotate) > maxRate)
								{
									if (newRotate > 0.0)
										newRotate = maxRate;
									else
										newRotate = -maxRate;
								}
								else
								{
									if (newGestureStateGoal == MECH_STATE_WALKING)
									{
										//------------------------------------
										// Try to attain current goal speed...
										newThrottleSetting = pilot->calcMoveSpeedThrottle(
											getBodyState(), control.settings.mech.throttle);
									}
								}
							}
						}
						else if (moveStateGoal == MOVESTATE_REVERSE)
						{
							//---------------------------------------------------
							// Stop, and pivot fully reverse to next path
							// step...
							//((MechActor*)appearance)->forceStop();
							pilot->pausePath(); // pilot->getMovePath()->numSteps
								// = 0;
							newMoveState = MOVESTATE_PIVOT_REVERSE;
						}
						else if (moveStateGoal == MOVESTATE_PIVOT_FORWARD)
						{
							//---------------------------------------------------
							// Stop, and pivot fully forward to next path
							// step...
							//((MechActor*)appearance)->forceStop();
							pilot->pausePath(); // pilot->getMovePath()->numSteps
								// = 0;
							newMoveState = MOVESTATE_PIVOT_FORWARD;
						}
						else if (moveStateGoal == MOVESTATE_PIVOT_REVERSE)
						{
							//---------------------------------------------------
							// Stop, and pivot fully reverse to next path
							// step...
							//((MechActor*)appearance)->forceStop();
							pilot->pausePath(); // pilot->getMovePath()->numSteps
								// = 0;
							newMoveState = MOVESTATE_PIVOT_REVERSE;
						}
						else
						{
							//--------
							// Stop...
							//((MechActor*)appearance)->forceStop();
							pilot->pausePath(); // pilot->getMovePath()->numSteps
								// = 0;
							newMoveState = MOVESTATE_FORWARD /*MOVESTATE_STAND*/;
						}
					}
					else if (moveState == MOVESTATE_REVERSE)
					{
						if (moveStateGoal == MOVESTATE_FORWARD)
						{
							//---------------------------------------------------
							// Stop, and pivot fully forward to next path
							// step...
							//((MechActor*)appearance)->forceStop();
							pilot->pausePath(); // pilot->getMovePath()->numSteps
								// = 0;
							newMoveState = MOVESTATE_PIVOT_FORWARD;
						}
						else if (moveStateGoal == MOVESTATE_REVERSE)
						{
							newGestureStateGoal = MECH_STATE_REVERSE;
							//--------------------------
							// Keep moving in reverse...
							/*
							if ((relFacingToWayPt < -5.0) || (relFacingToWayPt
							> 5.0))
							{
								//-----------------------------------------------
								// We can and will shift facing to
							destination... float maxRate = dyn->maxMechYawRate;
								if (relFacingToWayPt < 0.0)
									newTurnRate = maxRate*frameLength;
								else
									newTurnRate = -maxRate*frameLength;

								relFacingToWayPt = -(relFacingToWayPt + 180.0);
								if (relFacingToWayPt > 0.0)
									relFacingToWayPt = -(relFacingToWayPt -
							180.0);

								if ((newTurnRate > 0.0) && (newTurnRate >
							relFacingToWayPt)) newTurnRate = relFacingToWayPt;
								else if ((newTurnRate > 0.0) && (newTurnRate <
							relFacingToWayPt)) newTurnRate = relFacingToWayPt;
								else
								{
									if (newGestureStateGoal ==
							MECH_STATE_WALKING) {
										//------------------------------------
										// Try to attain current goal speed...
										newThrottleSetting =
							pilot->calcMoveSpeedThrottle(getBodyState(),
							data->throttleSetting);
									}
								}

								newMechYaw = newTurnRate;
							}
							*/
							if (relFacingToWayPt < 0)
								newRotate = -(relFacingToWayPt + 180.0);
							else
								newRotate = -(relFacingToWayPt - 180.0);
							float maxRate = tonnageTurnRate[int32_t(tonnage)] * frameLength;
							if (fabs(newRotate) > maxRate)
							{
								if (newRotate > 0.0)
									newRotate = maxRate;
								else
									newRotate = -maxRate;
								newThrottleSetting = control.settings.mech.throttle - 10;
							}
							else
							{
								//------------------------------------
								// Try to attain current goal speed...
								newThrottleSetting = pilot->calcMoveSpeedThrottle(
									getBodyState(), control.settings.mech.throttle);
								// newThrottleSetting = data->throttleSetting +
								// 10;
							}
						}
						else if (moveStateGoal == MOVESTATE_PIVOT_FORWARD)
						{
							//---------------------------------------------------
							// Stop, and pivot fully forward to next path
							// step...
							//((MechActor*)appearance)->forceStop();
							pilot->pausePath(); // pilot->getMovePath()->numSteps
								// = 0;
							newMoveState = MOVESTATE_PIVOT_FORWARD;
						}
						else if (moveStateGoal == MOVESTATE_PIVOT_REVERSE)
						{
							//---------------------------------------------------
							// Stop, and pivot fully reverse to next path
							// step...
							//((MechActor*)appearance)->forceStop();
							pilot->pausePath(); // pilot->getMovePath()->numSteps
								// = 0;
							newMoveState = MOVESTATE_PIVOT_REVERSE;
						}
						else
						{
							//--------
							// Stop...
							//((MechActor*)appearance)->forceStop();
							pilot->pausePath(); // pilot->getMovePath()->numSteps
								// = 0;
							newMoveState = MOVESTATE_FORWARD /*MOVESTATE_STAND*/;
						}
					}
					else
					{
						//--------------------------
						// Not moving--should we be?
						if ((moveStateGoal == MOVESTATE_FORWARD) || (moveStateGoal == MOVESTATE_PIVOT_FORWARD))
						{
							//((MechActor*)appearance)->forceStop();
							pilot->pausePath(); // pilot->getMovePath()->numSteps
								// = 0;
							newMoveState = MOVESTATE_PIVOT_FORWARD;
						}
						else if ((moveStateGoal == MOVESTATE_REVERSE) || (moveStateGoal == MOVESTATE_PIVOT_REVERSE))
						{
							//((MechActor*)appearance)->forceStop();
							pilot->pausePath(); // pilot->getMovePath()->numSteps
								// = 0;
							newMoveState = MOVESTATE_PIVOT_REVERSE;
						}
					}
				}
			}
		}
		else
		{
			//-------------------------------------------
			// We better not be walking or running, then!
			newGestureStateGoal = MECH_STATE_STANDING;
		}
	}
	else
	{
		//---------------------
		// Cannot move, period!
		newGestureStateGoal = MECH_STATE_STANDING;
	}
	return (goalReached);
}

//---------------------------------------------------------------------------

void BattleMech::netUpdateMovement(void)
{
	int32_t minThrottle = 35;
	int32_t maxThrottle = 100;
	int32_t bodyState = getBodyState();
	MovePathPtr path = pilot->getMovePath();
	bool atEndOfPath = (path->curStep == (path->numSteps - 1)); // was >=
	Stuff::Vector3D wayPt;
	float distanceFromWayPt = 1000000.0;
	if ((path->curStep > -1) && (path->curStep < path->numSteps))
	{
		wayPt = path->stepList[path->curStep].destination;
		distanceFromWayPt = distanceFrom(wayPt);
	}
	if (moveChunk.moving &&
		/*(path->numStepsWhenNotPaused > 0) &&*/ (bodyState == MECH_STATUSCHUNK_BODYSTATE_PARKED))
		startUpThisFrame = true;
	//	MECH_STATUSCHUNK_BODYSTATE_NORMAL,
	//	MECH_STATUSCHUNK_BODYSTATE_STANDING,
	//	MECH_STATUSCHUNK_BODYSTATE_PARKED,
	//	MECH_STATUSCHUNK_BODYSTATE_POWERING_UP,
	//	MECH_STATUSCHUNK_BODYSTATE_POWERING_DOWN,
	//	MECH_STATUSCHUNK_BODYSTATE_FALLEN_BACKWARD,
	//	MECH_STATUSCHUNK_BODYSTATE_FALLEN_FORWARD
	if (atEndOfPath && (distanceFromWayPt < Mover::marginOfError[1]))
	{
		startUpThisFrame = false;
		switch (statusChunk.bodyState)
		{
		case MECH_STATUSCHUNK_BODYSTATE_FALLEN_FORWARD:
			if (!isDisabled())
			{
				setStatus(OBJECT_STATUS_NORMAL);
				sensorSystem->setShutdown(false);
			}
			if (bodyState != MECH_STATUSCHUNK_BODYSTATE_FALLEN_FORWARD)
			{
				pilot->clearMoveOrders();
				appearance->setGestureGoal(MECH_STATE_FALLEN_FORWARD);
				control.settings.mech.throttle = maxThrottle;
				return;
			}
			break;
		case MECH_STATUSCHUNK_BODYSTATE_FALLEN_BACKWARD:
			if (!isDisabled())
			{
				setStatus(OBJECT_STATUS_NORMAL);
				sensorSystem->setShutdown(false);
			}
			if (bodyState != MECH_STATUSCHUNK_BODYSTATE_FALLEN_BACKWARD)
			{
				pilot->clearMoveOrders();
				appearance->setGestureGoal(MECH_STATE_FALLEN_BACKWARD);
				control.settings.mech.throttle = maxThrottle;
				return;
			}
			break;
		case MECH_STATUSCHUNK_BODYSTATE_PARKED:
			if (!isDisabled())
			{
				setStatus(OBJECT_STATUS_SHUTDOWN);
				sensorSystem->setShutdown(true);
			}
			if (bodyState != MECH_STATUSCHUNK_BODYSTATE_PARKED)
			{
				soundSystem->playDigitalSample(POWERDOWN_SFX, getPosition());
				pilot->clearMoveOrders();
				appearance->setGestureGoal(MECH_STATE_PARKED);
				control.settings.mech.throttle = maxThrottle;
				return;
			}
			break;
		case MECH_STATUSCHUNK_BODYSTATE_STANDING:
			if (!isDisabled())
			{
				setStatus(OBJECT_STATUS_NORMAL);
				sensorSystem->setShutdown(false);
			}
			if (bodyState != MECH_STATUSCHUNK_BODYSTATE_STANDING)
			{
				if (bodyState == MECH_STATUSCHUNK_BODYSTATE_PARKED)
					soundSystem->playDigitalSample(POWERUP_SFX, getPosition());
				pilot->clearMoveOrders();
				appearance->setGestureGoal(MECH_STATE_STANDING);
				control.settings.mech.throttle = maxThrottle;
				return;
			}
			break;
		}
	}
	if (disableThisFrame)
	{
		int32_t fallGesture =
			RandomNumber(2) ? MECH_STATE_FALLEN_FORWARD : MECH_STATE_FALLEN_BACKWARD;
		if (hitFromBehindThisFrame)
			fallGesture = MECH_STATE_FALLEN_FORWARD;
		else if (hitFromFrontThisFrame)
			fallGesture = MECH_STATE_FALLEN_BACKWARD;
		appearance->setGestureGoal(fallGesture);
		disableThisFrame = false;
		shutDownThisFrame = false;
		startUpThisFrame = false;
		hitFromFrontThisFrame = false;
		hitFromBehindThisFrame = false;
		control.settings.mech.throttle = maxThrottle;
		return;
	}
	if (shutDownThisFrame)
	{
		appearance->setGestureGoal(MECH_STATE_PARKED);
		shutDownThisFrame = false;
		startUpThisFrame = false;
		setStatus(OBJECT_STATUS_SHUTDOWN);
		control.settings.mech.throttle = maxThrottle;
		return;
	}
	if (startUpThisFrame)
	{
		appearance->setGestureGoal(MECH_STATE_STANDING);
		startUpThisFrame = false;
		shutDownThisFrame = false;
		setStatus(OBJECT_STATUS_NORMAL);
		control.settings.mech.throttle = maxThrottle;
		return;
	}
	if ((status == OBJECT_STATUS_SHUTTING_DOWN) || (status == OBJECT_STATUS_SHUTDOWN) || (status == OBJECT_STATUS_DISABLED))
		return;
	if (isCaptured())
		return;
	if (engineBlowTime > -1.0)
		return;
	if (updateJump())
		return;
	if (pivotTo())
		return;
	float newRotate = 0.0;
	int32_t newGestureStateGoal = -1;
	int32_t newMoveState = -1;
	wchar_t newThrottleSetting = -1;
	bool goalReached = false;
	goalReached = netUpdateMovePath(
		newRotate, newThrottleSetting, newGestureStateGoal, newMoveState, minThrottle, maxThrottle);
	if (newMoveState != -1)
		pilot->setMoveState(newMoveState);
	setControlSettings(
		newRotate, newThrottleSetting, newGestureStateGoal, minThrottle, maxThrottle, newRotate);
	// updateTorso(newRotate);
	updateMoveStateGoal();
	NewRotation = newRotate;
}

//----------------------------------------------------------------------------------
// END OF MOVEMENT UPDATE ROUTINES
//----------------------------------------------------------------------------------

Stuff::Vector3D
BattleMech::getPositionFromHS(int32_t nodeId)
{
	//-----------------------------------------------
	return (appearance->getWeaponNodePosition(nodeId));
}

//----------------------------------------------------------------------------------

#ifdef PROFILE
extern int32_t srCtrlUpd;
extern int32_t srApprUpd;
extern int32_t srDyneUpd;
extern int32_t srWeapUpd;
extern int32_t srObjtUpd;
#endif

// L_INTEGER startCk;
// L_INTEGER endCk;

#define JUMP_FX 454
//---------------------------------------------------------------------------

void BattleMech::createJumpFX(void)
{
#ifdef USE_JETS
	if (!jumpJets[0] && !jumpJets[1])
	{
		jumpJets[0] = (JetPtr)createObject(JUMP_FX);
		jumpJets[0]->setOwner(this);
		jumpJets[1] = (JetPtr)createObject(JUMP_FX);
		jumpJets[1]->setOwner(this);
		craterManager->addCrater(SCORCHMARKS, position, 0);
	}
#endif
}

//---------------------------------------------------------------------------

void BattleMech::endJumpFX(void)
{
#ifdef USE_JETS
	if (jumpJets[0] || jumpJets[1])
	{
		delete jumpJets[0];
		jumpJets[0] = nullptr;
		delete jumpJets[1];
		jumpJets[1] = nullptr;
	}
#endif
}

//---------------------------------------------------------------------------

Stuff::Vector3D
BattleMech::getJumpPosition(void)
{
	Stuff::Vector3D result;
	result.x = result.y = result.z = 0.0f;
	return result;
}

//---------------------------------------------------------------------------

bool BattleMech::crashAvoidanceSystem(void)
{
	if (MPlayer && !MPlayer->isServer())
		return (false);
	MovePathPtr path = pilot->getMovePath();
	if (path->numStepsWhenNotPaused == 0)
		return (false);
	if (pilot->getMoveWaitForPointTime() > 999990.0)
		return (false);
	// float velMag = getSpeed();
	// //((MechActor*)appearance)->getVelocityMagnitude();
	Stuff::Vector3D velocity = getVelocity();
	velocity.z = 0.0;
	Stuff::Vector3D relVelocity = velocity;
	relVelocity *= frameLength;
	relVelocity *= worldUnitsPerMeter;
	Stuff::Vector3D newPosition;
	newPosition.Add(position, relVelocity);
	//---------------------------------------------------------------------
	// Is this new position in a pathlocked area? If so, put on the brakes!
	int32_t cellR, cellC;
	land->worldToCell(newPosition, cellR, cellC);
	bool reachedEnd;
	bool blockReachedEnd;
	//-------------------------
	// To avoid corner stops...
	bool clippingCorner = false;
	int32_t dir = path->getDirection(path->curStep);
	if ((dir == 1) || (dir == 3) || (dir == 5) || (dir == 7))
	{
		bool firstCornerClipped = getAdjacentCellPathLocked(
			(moveLevel == 2), cellPositionRow, cellPositionCol, adjClippedCell[dir][0]);
		bool secondCornerClipped = getAdjacentCellPathLocked(
			(moveLevel == 2), cellPositionRow, cellPositionCol, adjClippedCell[dir][1]);
		clippingCorner = (firstCornerClipped && secondCornerClipped);
	}
	// bool stepOnMover = getPathLocked(tileR, tileC, cellR, cellC,
	// crashAvoidSelf);
	bool nearingMover = getPathRangeLock(crashAvoidPath, &reachedEnd);
	bool pathBlocked = getPathRangeBlocked(crashAvoidPath, &blockReachedEnd);
	bool pathLocked = /*stepOnMover ||*/ nearingMover;
	int32_t stepIntoGate = (path->crossesClosedGate(-1, 2) > 0);
	if (pilot->isYielding())
	{
		if (pathLocked || pathBlocked || clippingCorner || stepIntoGate)
		{
			pilot->pausePath(); // path->numSteps = 0; //in theory, this should
				// already be zero, yet is sometimes not. WHY?!
			return (true);
		}
		else
		{
			//-------------------
			// It's clear, now...
			pilot->resumePath();
			pilot->setMoveYieldTime(-1.0);
		}
	}
	else
	{
		if (pathLocked || pathBlocked || clippingCorner || stepIntoGate)
		{
			//-------------------------------------------------------------------
			// What exactly is wrong here? Is our goal currently occupied? If
			// so, and it's our final goal, then stop!
			if (reachedEnd || blockReachedEnd)
			{
				//-----------------------------------------------------------------------
				// Since we're practically there, let's just kill the current
				// move orders and let the pilot's movementDecision update
				// decide whether a new one should be set...
				/*
				pilot->setMoveWayPath(OrderType::current, nullptr, 0);
				for (size_t i = 0; i < 2; i++)
					pilot->clearMovePath(OrderType::current, i);
				pilot->setMoveGlobalPath(OrderType::current, nullptr, 0);
				*/
				// pilot->clearMoveOrders(OrderType::current);
				pilot->rethinkPath(0);
				// pilot->pausePath();
				// pilot->setMoveYieldTime(scenarioTime + crashYieldTime);
				control.brake();
			}
			else
			{
				pilot->pausePath();
				pilot->setMoveYieldTime(scenarioTime + crashYieldTime);
				control.brake();
			}
			return (true);
		}
	}
	return (false);
}

//---------------------------------------------------------------------------

void BattleMech::updateAIControl(void)
{
	control.reset();
	if (getAwake())
	{
		updateDamageTakenRate();
		if (!isDisabled() && pilot->alive() && !pilot->hasEjected())
		{
			if (getTeamId() > -1)
				pilot->mainDecisionTree();
			updateMovement();
		}
		else if (shutDownThisFrame || disableThisFrame)
			updateMovement();
	}
}

//---------------------------------------------------------------------------

void BattleMech::updateNetworkControl(void)
{
	control.reset();
	if (getAwake())
	{
		//-----------------------------------------
		// Update any weaponfire chunks received...
		updateWeaponFireChunks(CHUNK_RECEIVE);
		updateCriticalHitChunks(CHUNK_RECEIVE);
		updateRadioChunks(CHUNK_RECEIVE);
		if (!isDisabled() && pilot->alive() && !pilot->hasEjected())
		{
			pilot->checkAlarms();
			netUpdateMovement();
		}
		else if (shutDownThisFrame || disableThisFrame)
		{
			netUpdateMovement();
		}
	}
}

//---------------------------------------------------------------------------

void BattleMech::updatePlayerControl(void)
{
	control.reset();
	//-----------------------------------------------------------------
	// Poll the joystick and keyboards here so player can control mech.
	if (userInput->getKeyDown(KEY_T))
		control.settings.mech.rotate = tonnageTurnRate[int32_t(tonnage)] / 4.0 * frameLength;
	if (userInput->getKeyDown(KEY_Y))
		control.settings.mech.rotate = -tonnageTurnRate[int32_t(tonnage)] / 4.0 * frameLength;
	if (userInput->getKeyDown(KEY_DIVIDE))
		control.settings.mech.rotateTorso = dynamics.max.mech.torsoYawRate / 4.0 * frameLength;
	if (userInput->getKeyDown(KEY_MULTIPLY))
		control.settings.mech.rotateTorso = -dynamics.max.mech.torsoYawRate / 4.0 * frameLength;
	if (userInput->getKeyDown(KEY_NEXT))
	{
		appearance->setNextFrame();
	}
	if (userInput->getKeyDown(KEY_PRIOR))
	{
		appearance->setPrevFrame();
	}
	if (userInput->getKeyDown(KEY_END))
	{
		appearance->setSingleStepMode();
	}
	if (userInput->getKeyDown(KEY_U))
		control.settings.mech.rotateLeftArm = tonnageTurnRate[int32_t(tonnage)] / 4.0 * frameLength;
	if (userInput->getKeyDown(KEY_I))
		control.settings.mech.rotateLeftArm =
			-tonnageTurnRate[int32_t(tonnage)] / 4.0 * frameLength;
	if (userInput->getKeyDown(KEY_O))
		control.settings.mech.rotateRightArm =
			tonnageTurnRate[int32_t(tonnage)] / 4.0 * frameLength;
	if (userInput->getKeyDown(KEY_P))
		control.settings.mech.rotateRightArm =
			-tonnageTurnRate[int32_t(tonnage)] / 4.0 * frameLength;
	if (userInput->getKeyDown(KEY_1))
		appearance->setGestureGoal(0);
	if (userInput->getKeyDown(KEY_2))
		appearance->setGestureGoal(1); // Stand
	if (userInput->getKeyDown(KEY_3))
		appearance->setGestureGoal(2); // Walk
	if (userInput->getKeyDown(KEY_4))
		appearance->setGestureGoal(3); // Run
	if (userInput->getKeyDown(KEY_R))
		appearance->setGestureGoal(4); // Reverse
	if (userInput->getKeyDown(KEY_5))
		appearance->setGestureGoal(5); // Limp LEFT!
	if (userInput->getKeyDown(KEY_9))
		appearance->setGestureGoal(9); // Limp RIGHT!
	if (userInput->getKeyDown(KEY_6))
		appearance->setGestureGoal(7); // Fall Forward
	if (userInput->getKeyDown(KEY_7))
		appearance->setGestureGoal(8); // Fall Backward
	if (userInput->getKeyDown(KEY_8))
		appearance->hitFront();
	if (userInput->getKeyDown(KEY_B))
		appearance->hitBack();
	if (userInput->getKeyDown(KEY_N))
		appearance->hitRight();
	if (userInput->getKeyDown(KEY_M))
		appearance->hitLeft();
	if (userInput->getKeyDown(KEY_Z))
		ObjectManager->getObjectType(typeHandle)
			->handleDestruction(this, nullptr); // Blow self up!!
	if (userInput->getKeyDown(KEY_X))
		control.settings.mech.blowLeftArm = true;
	if (userInput->getKeyDown(KEY_J))
	{
		Stuff::Vector3D jumpDest = getRotationVector();
		jumpDest *= 150.0;
		jumpDest += position;
		appearance->setJumpParameters(jumpDest);
		appearance->setGestureGoal(6); // Jump
	}
	if (userInput->getKeyDown(KEY_G))
		appearance->setCombatMode(1);
	if (userInput->getKeyDown(KEY_F))
		appearance->setCombatMode(0);
	if (userInput->getKeyDown(KEY_C))
		control.settings.mech.blowRightArm = true;
}

//---------------------------------------------------------------------------

void BattleMech::updateDynamics(void)
{
	float yawRate = control.settings.mech.rotate;
	float facingYawRate = control.settings.mech.facingRotate;
	float torsoYawRate = control.settings.mech.rotateTorso;
	float rightArmYawRate = control.settings.mech.rotateRightArm;
	float leftArmYawRate = control.settings.mech.rotateLeftArm;
	//----------------------------------------------------------------
	// The gesture is how we control speed.  So, at this point, check
	// the mechControlData->gestureGoal and see if we need to change goals
	bool bottomNotAllowedToYaw = false;
	bool topNotAllowedToYaw = false;
	if (appearance)
	{
		if ((control.settings.mech.gestureGoal != -1) || ((control.settings.mech.gestureGoal == 7) && (getMoveType() != MOVETYPE_AIR)) || ((control.settings.mech.gestureGoal == 8) && (getMoveType() != MOVETYPE_AIR)))
			appearance->setGestureGoal(control.settings.mech.gestureGoal);
	}
	//--------------------------------------------------------------------
	// Rotate torso by torso yaw rate, making sure we don't turn too far.
	if ((torsoYawRate != 0.0) && !topNotAllowedToYaw)
	{
		float newTorsoRotation = torsoRotation;
		if (newTorsoRotation > dynamics.max.mech.torsoYaw)
		{
			newTorsoRotation = dynamics.max.mech.torsoYaw;
			torsoYawRate = 0.0;
		}
		if (newTorsoRotation < -dynamics.max.mech.torsoYaw)
		{
			newTorsoRotation = -dynamics.max.mech.torsoYaw;
			torsoYawRate = 0.0;
		}
		if ((newTorsoRotation + torsoYawRate) > dynamics.max.mech.torsoYaw)
			torsoYawRate = dynamics.max.mech.torsoYaw - newTorsoRotation;
		if ((newTorsoRotation + torsoYawRate) < -dynamics.max.mech.torsoYaw)
			torsoYawRate = -dynamics.max.mech.torsoYaw - newTorsoRotation;
		newTorsoRotation += torsoYawRate;
		torsoRotation = newTorsoRotation;
	}
	//--------------------------------------------------------------------------
	// Rotate rightArm by rightArm yaw rate, making sure we don't turn too far.
	if ((rightArmYawRate != 0.0) && !topNotAllowedToYaw)
	{
		float newRightArmRotation = rightArmRotation;
		if (newRightArmRotation > tonnageTurnRate[int32_t(tonnage)])
		{
			newRightArmRotation = tonnageTurnRate[int32_t(tonnage)];
			rightArmYawRate = 0.0;
		}
		if (newRightArmRotation < -tonnageTurnRate[int32_t(tonnage)])
		{
			newRightArmRotation = -tonnageTurnRate[int32_t(tonnage)];
			rightArmYawRate = 0.0;
		}
		if ((newRightArmRotation + rightArmYawRate) > tonnageTurnRate[int32_t(tonnage)])
		{
			newRightArmRotation = tonnageTurnRate[int32_t(tonnage)];
			rightArmYawRate = 0.0;
		}
		if ((newRightArmRotation + rightArmYawRate) < -tonnageTurnRate[int32_t(tonnage)])
		{
			newRightArmRotation = -tonnageTurnRate[int32_t(tonnage)];
			rightArmYawRate = 0.0;
		}
		newRightArmRotation += rightArmYawRate;
		rightArmRotation = newRightArmRotation;
	}
	//--------------------------------------------------------------------------
	// Rotate rightArm by rightArm yaw rate, making sure we don't turn too far.
	if ((leftArmYawRate != 0.0) && !topNotAllowedToYaw)
	{
		float newLeftArmRotation = leftArmRotation;
		if (newLeftArmRotation < -tonnageTurnRate[int32_t(tonnage)])
		{
			newLeftArmRotation = -tonnageTurnRate[int32_t(tonnage)];
			leftArmYawRate = 0.0;
		}
		if (newLeftArmRotation > tonnageTurnRate[int32_t(tonnage)])
		{
			newLeftArmRotation = tonnageTurnRate[int32_t(tonnage)];
			leftArmYawRate = 0.0;
		}
		if ((newLeftArmRotation + leftArmYawRate) < -tonnageTurnRate[int32_t(tonnage)])
		{
			newLeftArmRotation = -tonnageTurnRate[int32_t(tonnage)];
			leftArmYawRate = 0.0;
		}
		if ((newLeftArmRotation + leftArmYawRate) > tonnageTurnRate[int32_t(tonnage)])
		{
			newLeftArmRotation = tonnageTurnRate[int32_t(tonnage)];
			leftArmYawRate = 0.0;
		}
		newLeftArmRotation += leftArmYawRate;
		leftArmRotation = newLeftArmRotation;
	}
	if (!bottomNotAllowedToYaw)
		rotate(yawRate, facingYawRate);
}

//---------------------------------------------------------------------------
extern GameObjectPtr DebugGameObject[3];

int32_t
BattleMech::update(void)
{
	positionNormal = land->getTerrainNormal(position);
	getPilot()->getIndex();
	// Reset every frame to avoid multiples
	playedCriticalHit = false;
	if (getTeam() == Team::home)
	{
		((ObjectAppearance*)appearance)->pilotNameID = getPilot()->descID;
		((ObjectAppearance*)appearance)->pilotName[0] = 0;
		appearance->setMechName(variantName);
	}
	else if (MPlayer)
	{
		strcpy(((ObjectAppearance*)appearance)->pilotName,
			(commanderId > -1) ? MPlayer->playerInfo[commanderId].name : " ");
	}
	else
		((ObjectAppearance*)appearance)->pilotNameID = -1;
	if (isDestroyed() || isDisabled())
		setTangible(false);
	if (withdrawing && (pilot->getStatus() == WARRIOR_STATUS_WITHDRAWN))
	{
		setTangible(false);
		return (1); // This guy is gone.
	}
	//----------------------------------------------------------------------
	// Blow off the arms if we need to.
	// MUST be here so that they get blown off regardless of mech status
	if (leftArmBlownThisFrame)
	{
		leftArmBlownThisFrame = false;
		//-------------------------------------------
		// Frank says, blow the right one anyway --gd
		control.settings.mech.blowRightArm = true;
	}
	if (rightArmBlownThisFrame)
	{
		rightArmBlownThisFrame = false;
		//------------------------------------------
		// Frank says, blow the left one anyway --gd
		control.settings.mech.blowLeftArm = true;
	}
	if (control.settings.mech.blowLeftArm)
	{
		appearance->blowLeftArm();
		soundSystem->playDigitalSample(CRITICAL_HIT_SFX, getPosition(), true);
		control.settings.mech.blowLeftArm = false;
	}
	if (control.settings.mech.blowRightArm)
	{
		appearance->blowRightArm();
		soundSystem->playDigitalSample(CRITICAL_HIT_SFX, getPosition(), true);
		control.settings.mech.blowRightArm = false;
	}
	//----------------------------------------------------------------------
	if (!isDestroyed() && !isDisabled())
	{
#ifdef MC_PROFILE
		QueryPerformanceCounter(startCk);
#endif
		// Be damned sure our legs are marked correctly!!
		calcLegStatus();
		updatePathLock(false);
		// Check if we are SHUTDOWN and NOT onGui.
		// If we are both, we probably are waiting to be linked up with.
		// See if anything on our side is close enough to link up with us.
		// If it is close enough, powerup and ourselves to the GUI!
		if ((status == OBJECT_STATUS_SHUTDOWN) && !isOnGUI())
		{
			// Check if anyone on our side is within range.
			// Until otherwise noted, if I have LOS to them, thats enough.
			// Powerup, add ourselves to the GUI and select ourselves
			// Maybe we should send a Radio message saying "hi"?
			if (Team::home->teamLineOfSight(position, getAppearRadius()))
			{
				mission->missionInterface->addMover(this);
				setSelected(1);
				pilot->corePower(true);
				setOnGUI(true);
				sensorSystem->broken = false;
			}
			else
			{
				sensorSystem->broken = true;
			}
		}
		if ((status == OBJECT_STATUS_SHUTDOWN) || (status == OBJECT_STATUS_SHUTTING_DOWN))
			appearance->setGestureGoal(MECH_STATE_PARKED);
		if (getAwake() && !isDisabled() && getTeam() && !godMode)
		{
			BattleMechTypePtr mechType =
				(BattleMechTypePtr)ObjectManager->getObjectType(typeHandle);
			getTeam()->markSeen(position, mechType->LOSFactor);
			markDistanceMoved = 0.0;
		}
		if (timeToClearSelection != 0.0 && scenarioTime > timeToClearSelection)
		{
			timeToClearSelection = 0.0;
			setSelected(false);
			setTargeted(false);
		}
		control.update(this);
		if (!getAwake())
		{
			appearance->setGestureGoal(MECH_STATE_PARKED);
			shutDownThisFrame = false;
		}
#ifdef MC_PROFILE
		QueryPerformanceCounter(endCk);
		srCtrlUpd += (endCk.LowPart - startCk.LowPart);
#endif
#ifdef MC_PROFILE
		QueryPerformanceCounter(startCk);
#endif
		// updateDynamics();
		bool emergencyStop = false;
		if (!isDisabled())
		{
			std::unique_ptr<Mover> ramTarget = nullptr;
			if ((getPilot()->getCurTacOrder()->code == TacticalOrderCode::attackobject) && (getPilot()->getCurTacOrder()->attackParams.method == AttackMethod::ramming))
				ramTarget = (std::unique_ptr<Mover>)getPilot()->getCurTacOrder()->getRamTarget();
			if (ramTarget)
				ramTarget->updatePathLock(false);
			emergencyStop = crashAvoidanceSystem();
			if (ramTarget)
				ramTarget->updatePathLock(true);
		}
#ifdef MC_PROFILE
		QueryPerformanceCounter(endCk);
		srDyneUpd += (endCk.LowPart - startCk.LowPart);
#endif
		//--------------------------------------------------------------------
		// At this point, the other updates have completed, its time to
		// apply the velocity and rotations to the mech.
		// NOTE:
		// All positions in the game are in World Units.  All velocities
		// are in Meters/sec.  This is to provide easy data entry for designers
		// We can't store positions as meters because terrain tiles are fixed
		// pixel(world unit) sizes which would require a boatload of math
		// with its crazy floating point inconsistencies.  This will mess up
		// the terrain.  So, whenever you want a position derived from
		// a velocity, multiply velocity by worldUnitsPerMeter.
#ifdef MC_PROFILE
		QueryPerformanceCounter(startCk);
#endif
		if (teleportPosition.x > -999990.0)
		{
			setPosition(teleportPosition);
			teleportPosition.x = -999990.0;
			teleportPosition.y = -999990.0;
			teleportPosition.z = -999990.0;
		}
		//-----------------------------------------------------------
		// Find magnitude of current velocity. Store this in dynamics
		// Adjust position based on mech Velocity which is based on gesture
		Stuff::Vector3D newPosition;
		Stuff::Vector3D vel = getRotationVector();
		if (!appearance->isJumpAirborne())
		{
			float velMag = appearance->getVelocityMagnitude();
			if (emergencyStop)
				velMag = 0.0;
			velMag *= 0.01f * control.settings.mech.throttle;
			float velFactor = 1.0;
			// if (moveLevel == 0) {
			float velAngle = land->getTerrainAngle(position);
			velAngle *= DEGREES_TO_RADS;
			velFactor = cos(velAngle);
			//}
			//--------------------------------------------
			float velMult = velMag * velFactor * frameLength;
			if (velMult > DistanceToWaypoint)
				velMult = DistanceToWaypoint;
			vel *= velMult * worldUnitsPerMeter;
			setVelocity(vel);
			newPosition.Add(position, vel);
			newPosition.z = land->getTerrainElevation(newPosition);
			if ((getMoveType() == MOVETYPE_AIR) && (newPosition.z < MapData::waterDepth))
				newPosition.z = MapData::waterDepth;
		}
		else
		{
			// Just move mech in direction of jump.
			vel = appearance->getVelocity();
			vel *= frameLength;
			setVelocity(vel);
			newPosition.Add(position, vel);
			newPosition.z = land->getTerrainElevation(newPosition);
		}
		if (newMoveChunk)
		{
			if (!statusChunk.jumpOrder)
			{
				//------------------------------------------------------------
				// If we're too far from where we should be, perhaps we should
				// just warp to it...
				Stuff::Vector3D correctPos;
				land->cellToWorld(moveChunk.stepPos[0][0], moveChunk.stepPos[0][1], correctPos);
				correctPos.z = 0.0;
				Stuff::Vector3D curPos = position;
				curPos.z = 0.0;
				curPos.Subtract(curPos, correctPos);
				float posDelta = curPos.GetLength();
				if (posDelta > MPlayer->warpFactor)
					newPosition = correctPos;
				if (!GameMap->inBounds(moveChunk.stepPos[0][0], moveChunk.stepPos[0][1]))
					Fatal(0, " mech.update: newMoveChunk stepPos not on map! ");
			}
			newMoveChunk = false;
		}
		setPosition(newPosition);
		updateTorso(NewRotation);
		// Don't let them move if they are shutdown!!
		if (status != OBJECT_STATUS_SHUTDOWN)
			updateDynamics();
		markDistanceMoved += vel.GetLength();
		if (!isDisabled())
			updatePathLock(true);
		mineCheck();
		float zPos = land->getTerrainElevation(position);
		position.z = zPos;
		if ((getMoveType() == MOVETYPE_AIR) && (zPos < MapData::waterDepth))
			position.z = MapData::waterDepth;
			// float inverseAngle = 180.0;
#ifdef MC_PROFILE
		QueryPerformanceCounter(endCk);
		srObjtUpd += (endCk.LowPart - startCk.LowPart);
#endif
#ifdef MC_PROFILE
		QueryPerformanceCounter(startCk);
#endif
		bool inView = appearance->recalcBounds();
		if (inView)
			windowsVisible = turn;
		if (withdrawing && !inView && (pilot->getStatus() != WARRIOR_STATUS_WITHDRAWN))
			ObjectManager->getObjectType(typeHandle)->handleDestruction(this, nullptr);
		if (appearance)
		{
			MovePathPtr path = pilot->getMovePath();
			bool brake = true;
			if (path && path->numSteps && (path->curStep < path->numSteps))
				brake = false;
			appearance->setBrake(brake);
			if ((pilot->getCurrentTarget()) || (pilot->getCurTacOrder()->code == TacticalOrderCode::attackobject) || (pilot->getCurTacOrder()->code == TacticalOrderCode::attackpoint))
				appearance->setCombatMode(true);
			else
				appearance->setCombatMode(false);
			if (isJumping())
				setTangible(false);
			else if (!isDestroyed() && !isDisabled())
				setTangible(true);
		}
#ifdef MC_PROFILE
		QueryPerformanceCounter(endCk);
		srApprUpd += (endCk.LowPart - startCk.LowPart);
#endif
#ifdef MC_PROFILE
		QueryPerformanceCounter(endCk);
		srObjtUpd += (endCk.LowPart - startCk.LowPart);
#endif
		if (getDebugFlag(OBJECT_DFLAG_DISABLE))
			disable(DEBUGGER_DEATH);
	}
	else if (isDisabled() && !isDestroyed())
	{
		updatePathLock(false);
		((ObjectAppearance*)appearance)->pilotNameID = IDS_NOPILOT;
		//--------------------------------------------------------
		// We are disabled now.  Stop trying to animate, dammit!
		control.settings.mech.gestureGoal = -1;
		updateDynamics();
		//-----------------------------------------------------------
		// Find magnitude of current velocity. Store this in dynamics
		float velMag = appearance->getVelocityMagnitude();
		//---------------------------
		// Create vector for velocity
		Stuff::Vector3D vel = getRotationVector();
		vel *= velMag * worldUnitsPerMeter;
		vel *= frameLength;
		setVelocity(vel);
		Stuff::Vector3D newPosition;
		newPosition.Add(position, vel);
		newPosition.z = land->getTerrainElevation(newPosition);
		setPosition(newPosition);
		bool inView = appearance->recalcBounds();
		if (inView)
			windowsVisible = turn;
		if (appearance)
		{
			appearance->setInView(inView);
			appearance->setCombatMode(false);
		}
		if (!fallen)
		{
			fallen = appearance->haveFallen();
		}
		if (!startDisabled)
		{
			timeLeft -= frameLength;
			if ((timeLeft < 1.0f) && !exploding)
			{
				//-------------------------------------------------------
				// Disable just plays an eject effect now.
				// NO crater or it will look dumb.
				//
				// DO NOT play disable effect if pilot is dead!!
				if (getPilot()->alive())
				{
					Stuff::Vector3D cockpitPos = appearance->getNodeNamePosition("cockpit");
					ObjectManager->createExplosion(
						MECH_DISABLE_EXPLOSION_ID, nullptr, cockpitPos, 0.0f, 0.0f);
				}
				appearance->startSmoking(-1);
				exploding = true;
			}
		}
		if (getMoveType() == MOVETYPE_AIR)
		{
			setStatus(OBJECT_STATUS_DESTROYED);
			timeLeft = 0.0f;
		}
	}
	else
	{
		//--------------------------------------------------------
		// We are destroyed now.  Stop trying to animate, dammit!
		control.settings.mech.gestureGoal = -1;
		updateDynamics();
		//-----------------------------------------------------------
		// Find magnitude of current velocity. Store this in dynamics
		float velMag = appearance->getVelocityMagnitude();
		//---------------------------
		// Create vector for velocity
		Stuff::Vector3D vel = getRotationVector();
		vel *= velMag * worldUnitsPerMeter;
		vel *= frameLength;
		setVelocity(vel);
		Stuff::Vector3D newPosition;
		newPosition.Add(position, vel);
		newPosition.z = land->getTerrainElevation(newPosition);
		setPosition(newPosition);
		if (!playedHeloDestruct)
		{
			soundSystem->playDigitalSample(HELICOPTER_DEATH, getPosition());
			playedHeloDestruct = true;
		}
		bool inView = appearance->recalcBounds();
		if (inView)
			windowsVisible = turn;
		if (appearance)
		{
			appearance->setGestureGoal(GestureGoalFallenBackward);
			appearance->setInView(inView);
			appearance->setCombatMode(false);
		}
		if (!fallen)
		{
			fallen = appearance->haveFallen();
			timeLeft = 1.0f;
		}
		if (fallen)
		{
			timeLeft -= frameLength;
			if ((timeLeft < 0.4) && !exploding)
			{
				//-------------------------------------------------------
				// Blow the Mech and leave a crater.
				Stuff::Vector3D cockpitPos = appearance->getNodeNamePosition("cockpit");
				if (getMoveType() != MOVETYPE_AIR)
					ObjectManager->createExplosion(MECH_EXPLOSION_ID, nullptr, cockpitPos,
						((BattleMechTypePtr)getObjectType())->explDmg,
						((BattleMechTypePtr)getObjectType())->explRad);
				else
					ObjectManager->createExplosion(
						VEHICLE_EXPLOSION_ID, nullptr, cockpitPos, 0.0f, 0.0f);
				craterManager->addCrater(CRATER_4, position, RandomNumber(180));
				exploding = true;
			}
			else if (((timeLeft < -2.5f) && !mechRemoved && (getMoveType() != MOVETYPE_AIR)) || ((timeLeft < -1.5f) && !mechRemoved && (getMoveType() == MOVETYPE_AIR)))
			{
				mechRemoved = true;
			}
			else if (mechRemoved)
				appearance->setObjStatus(getStatus());
		}
	}
// TEMP BLOCK OUT FOR MC2 port...
#if 0
	//-------------------------------------------------------------------------------
	// Let the ObjectManager know which block the Mech is in for Collision Purposes
	float xCoord = position.x - Terrain::mapTopLeft3d100.x;
	float yCoord = Terrain::mapTopLeft3d100.y + position.y ;
	float divisor = (Terrain::verticesBlockSide * Terrain::metersPerVertex);
	xCoord /= divisor;
	yCoord /= divisor;
	int32_t blockNumber = agfloor(xCoord) + (agfloor(yCoord) * Terrain::blocksMapSide);
	addMoverToList(blockNumber);
#endif
	//---------------------------------------------------------------------------------
	// Setup the Render stuff here.  Must do in Update to avoid texture lock in
	// render!
	int32_t relationship = 0;
	if (getTeamId() == Team::home->getId())
	{
		// Two Possibilities.  Either we have the same commander or we don't.
		// If we do, we are on the same "team", if not we are ALLIES!!!
		if (getCommanderId() != Commander::home->getId())
		{
			relationship = 1;
		}
	}
	else
	{
		relationship = 2;
	}
	int32_t homeRelations = 2;
	if (getTeam() == Team::home)
	{
		if (getCommander() == Commander::home)
			homeRelations = 0;
		else
			homeRelations = 1;
	}
	appearance->setObjectParameters(position, rotation, drawFlags, getTeamId(), homeRelations);
	if (getMoveType() == MOVETYPE_AIR)
		appearance->setMoverParameters(torsoRotation, 0.0f, 0.0f, true);
	else
		appearance->setMoverParameters(torsoRotation);
	if (!mechRemoved && !isDestroyed())
		appearance->setObjStatus(getStatus());
	if (control.getType() == CONTROL_PLAYER)
		appearance->setDebugMoveMode();
	appearance->setVisibility(true, true);
	// Start and stop the water wakes here.
	int32_t watercellR, watercellC;
	land->worldToCell(position, watercellR, watercellC);
	if (GameMap->getDeepWater(watercellR, watercellC) || GameMap->getShallowWater(watercellR, watercellC))
	{
		appearance->startWaterWake();
	}
	else
	{
		appearance->stopWaterWake();
	}
	//-----------------------------------------------------
	// Must save sensor data HERE now so fades happen OK.
	if (getTeamId() != Team::home->getId())
	{
		//---------------------------------------------------------------
		conStat = getContactStatus(Team::home->getId(), true);
		if ((conStat == CONTACT_VISUAL) || isDestroyed() || isDisabled() || ShowMovers || (MPlayer && MPlayer->allUnitsDestroyed[MPlayer->commanderid]))
		{
			if (alphaValue != 0xff)
			{
				fadeTime += frameLength;
				if (fadeTime > ContactFadeTime)
				{
					fadeTime = ContactFadeTime;
					alphaValue = 0xff;
				}
				else
				{
					float fadeValue = fadeTime / ContactFadeTime * 255.0f;
					alphaValue = fadeValue;
				}
			}
			appearance->setAlphaValue(alphaValue);
			appearance->setSensorLevel(0);
		}
		else // Pretty much anything else is fading out.
		{
			if (alphaValue != 0x0)
			{
				// We are fading.  Move it down.
				fadeTime -= frameLength;
				if (fadeTime < 0.0f)
				{
					fadeTime = 0.0f;
					alphaValue = 0x0;
				}
				else
				{
					float fadeValue = fadeTime / ContactFadeTime * 255.0f;
					alphaValue = fadeValue;
				}
			}
			appearance->setAlphaValue(alphaValue);
			if (conStat == CONTACT_SENSOR_QUALITY_1)
			{
				appearance->setSensorLevel(1);
			}
			else if (conStat == CONTACT_SENSOR_QUALITY_2)
			{
				appearance->setSensorLevel(2);
			}
			else if (conStat == CONTACT_SENSOR_QUALITY_3)
			{
				appearance->setSensorLevel(3);
			}
			else if (conStat == CONTACT_SENSOR_QUALITY_4)
			{
				appearance->setSensorLevel(4);
			}
		}
	}
	else
	{
		if (isOnGui)
		{
			alphaValue = 0xff;
			appearance->setSensorLevel(5);
			appearance->setAlphaValue(alphaValue);
		}
	}
	appearance->update();
	appearance->updateFootprints();
	float barStatus = getTotalEffectiveness();
	uint32_t color = 0xff7f7f7f;
	if ((teamId > -1) && (teamId < 8))
	{
		if (getTeam()->isFriendly(Team::home))
			color = SB_GREEN;
		else if (getTeam()->isEnemy(Team::home))
			color = SB_RED;
		else
			color = SB_BLUE;
	}
	appearance->setBarcolour(color);
	appearance->setBarStatus(barStatus);
	// Must make room for holdFireIcons on HUD if this mech might need one!
	if (isOnGui && !mechRemoved && (attackRange == FireRangeType::current) && !isDisabled())
	{
		mcTextureManager->addTriangle(holdFireIconHandle, MC2_ISHUDLMNT);
		mcTextureManager->addTriangle(holdFireIconHandle, MC2_ISHUDLMNT);
	}
	//---------------------------------------------------------------------------------
	return (1); // Mech is still around,  false means whack this mech from list.
}

//---------------------------------------------------------------------------
void BattleMech::render(void)
{
	if (Terrain::IsGameSelectTerrainPosition(position))
	{
		if (getTeamId() != Team::home->getId())
		{
			//---------------------------------------------------------------
			// Sensor contact is now same as during update.
			int32_t cStat = conStat;
			if ((cStat == CONTACT_VISUAL) || isDestroyed() || isDisabled() || ShowMovers || (MPlayer && MPlayer->allUnitsDestroyed[MPlayer->commanderid]))
			{
				float barStatus = getTotalEffectiveness();
				uint32_t color = 0xff7f7f7f;
				if ((teamId > -1) && (teamId < 8))
				{
					if (getTeam()->isFriendly(Team::home))
						color = SB_GREEN;
					else if (getTeam()->isEnemy(Team::home))
						color = SB_RED;
					else
						color = SB_BLUE;
				}
				appearance->setBarcolour(color);
				appearance->setBarStatus(barStatus);
				appearance->setObjectNameId(descID);
				appearance->setVisibility(true, true);
				appearance->render();
			}
			else if (cStat == CONTACT_SENSOR_QUALITY_1)
			{
				appearance->setBarcolour(SB_RED);
				appearance->setVisibility(true, true);
				appearance->render();
				// No more text help for this level
			}
			else if (cStat == CONTACT_SENSOR_QUALITY_2)
			{
				appearance->setBarcolour(SB_RED);
				appearance->setVisibility(true, true);
				appearance->render();
				// No more text help for this level
			}
			else if (cStat == CONTACT_SENSOR_QUALITY_3)
			{
				appearance->setBarcolour(SB_RED);
				appearance->setVisibility(true, true);
				appearance->render();
				if (appearance->canBeSeen())
				{
					if (tonnage < 40)
						drawSensorTextHelp(appearance->getScreenPos().x,
							appearance->getScreenPos().y + 20.0f, IDS_SENSOR_LIGHT_MECH, SD_RED,
							false);
					else if (tonnage < 65)
						drawSensorTextHelp(appearance->getScreenPos().x,
							appearance->getScreenPos().y + 20.0f, IDS_SENSOR_MEDIUM_MECH, SD_RED,
							false);
					else if (tonnage < 80)
						drawSensorTextHelp(appearance->getScreenPos().x,
							appearance->getScreenPos().y + 20.0f, IDS_SENSOR_HEAVY_MECH, SD_RED,
							false);
					else
						drawSensorTextHelp(appearance->getScreenPos().x,
							appearance->getScreenPos().y + 20.0f, IDS_SENSOR_ASSAULT_MECH, SD_RED,
							false);
				}
			}
			else if (cStat == CONTACT_SENSOR_QUALITY_4)
			{
				appearance->setBarcolour(SB_RED);
				appearance->setVisibility(true, true);
				appearance->render();
				if (appearance->canBeSeen())
					drawSensorTextHelp(appearance->getScreenPos().x,
						appearance->getScreenPos().y + 20.0f, descID, SD_RED, false);
			}
		}
		else
		{
			if (isOnGui)
			{
				float barStatus = getTotalEffectiveness();
				uint32_t color = 0x0000ff00;
				if (getTeamId() == Team::home->getId())
				{
					// Two Possibilities.  Either we have the same commander or
					// we don't.
					// If we do, we are on the same "team", if not we are
					// ALLIES!!!
					if (getCommanderId() != Commander::home->getId())
					{
						color = 0x000000ff;
					}
				}
				else
				{
					color = 0x00ff0000;
				}
				appearance->setBarcolour(color);
				appearance->setBarStatus(barStatus);
				appearance->setObjectNameId(descID);
				appearance->setVisibility(true, true);
				appearance->setSensorLevel(0);
				appearance->render();
				if (attackRange == FireRangeType::current && !isDisabled() && appearance->canBeSeen())
					appearance->drawIcon(holdFireIconHandle, 5, 5, color | 0xff000000);
			}
		}
	}
	if (drawTerrainGrid)
	{
		MovePathPtr path = pilot->getMovePath();
		gosASSERT(path != nullptr);
		if (path->numSteps)
		{
			Stuff::Vector4D lineStart, lineEnd;
			for (size_t i = 0; i < path->numSteps; i++)
			{
				if (i != (path->numSteps - 1))
				{
					Stuff::Vector3D startPos = path->stepList[i].destination;
					Stuff::Vector3D endPos = path->stepList[i + 1].destination;
					startPos.z = land->getTerrainElevation(startPos);
					endPos.z = land->getTerrainElevation(endPos);
					eye->projectZ(startPos, lineStart);
					eye->projectZ(endPos, lineEnd);
					lineStart.z = lineEnd.z = HUD_DEPTH;
					LineElement newElement(lineStart, lineEnd, SD_BLUE, nullptr, -1);
					newElement.draw();
				}
			}
		}
	}
	damageThisFrame = 0.0f; // always reset here for the next frame.
}

//----------------------------------------------------------------------------------
void BattleMech::renderShadows(void)
{
	if (Terrain::IsGameSelectTerrainPosition(position))
	{
		if ((appearance->getCurrentGestureId() == 14) || (appearance->getCurrentGestureId() == 15) || (appearance->getCurrentGestureId() >= 21))
		{
			return; // No shadows on a fall!
		}
		if (getTeamId() != Team::home->getId())
		{
			//---------------------------------------------------------------
			// DO Sensor contact stuff here.  GLENN, Update with new whatevers
			// when done and let me know! -fs
			int32_t cStat = conStat;
			if ((cStat == CONTACT_VISUAL) && (alphaValue == 0xff))
			{
				appearance->renderShadows();
			}
		}
		else // We are Home Team and we always see ourselves.  Unless we haven't
			// linked up yet!
		{
			if (isOnGui && (alphaValue == 0xff))
			{
				appearance->renderShadows();
			}
		}
	}
}

//----------------------------------------------------------------------------------

float BattleMech::relFacingTo(Stuff::Vector3D goal, int32_t bodyLocation)
{
	float relFacing = Mover::relFacingTo(goal);
	switch (bodyLocation)
	{
	case MECH_BODY_LOCATION_HEAD:
	case MECH_BODY_LOCATION_CTORSO:
	case MECH_BODY_LOCATION_LTORSO:
	case MECH_BODY_LOCATION_RTORSO:
		relFacing += torsoRotation;
		break;
	case MECH_ARMOR_LOCATION_RCTORSO:
	case MECH_ARMOR_LOCATION_RLTORSO:
	case MECH_ARMOR_LOCATION_RRTORSO:
		relFacing += (torsoRotation /* + 180.0*/);
		break;
	case MECH_BODY_LOCATION_LARM:
		relFacing += (torsoRotation + leftArmRotation);
		break;
	case MECH_BODY_LOCATION_RARM:
		relFacing += (torsoRotation + rightArmRotation);
		break;
	case MECH_BODY_LOCATION_RLEG:
	case MECH_BODY_LOCATION_LLEG:
		break;
	default:
		NODEFAULT;
		//--------------------------------------------------------------------
		// Facing for a mech is ALWAYS based on Torso Rotation by default!!!
		// relFacing += torsoRotation;
		break;
	}
	// if (relFacing < -180.0)
	//	relFacing += 360.0;
	// else if (relFacing > 180)
	//	relFacing -= 360.0;
	return (relFacing);
}

//---------------------------------------------------------------------------
// COMBAT routines
//---------------------------------------------------------------------------

int32_t
BattleMech::getBodyState(void)
{
	int32_t gID = appearance->getCurrentGestureId();
	return (MechStateByGesture[gID]);
}

//---------------------------------------------------------------------------

bool BattleMech::isWeaponReady(int32_t weaponIndex)
{
	//	int32_t bodyLocation = inventory[weaponIndex].bodyLocation;
	//	if ((bodyLocation == MECH_BODY_LOCATION_LARM) &&
	// inventory[actuator[ACTUATOR_LSHOULDER]].disabled) 		return(false);
	//	else if ((bodyLocation == MECH_BODY_LOCATION_RARM) &&
	// inventory[actuator[ACTUATOR_RSHOULDER]].disabled) 		return(false);
	//	else
	if (inventory[weaponIndex].disabled || !(inventory[weaponIndex].health))
		return (false);
	else if (inventory[weaponIndex].readyTime > scenarioTime)
		return (false);
	return (true);
}

//---------------------------------------------------------------------------

float BattleMech::calcAttackChance(GameObjectPtr target, int32_t aimLocation, float targetTime,
	int32_t weaponIndex, float modifiers, int32_t* range, Stuff::Vector3D* targetpoint)
{
	if ((weaponIndex < numOther) || (weaponIndex >= numOther + numWeapons))
		return (-1000.0);
	//	if (pilot)
	//		modifiers +=
	// RankVersusChassisCombatModifier[pilot->getRank()][chassisClass];
	float attackChance = Mover::calcAttackChance(
		target, aimLocation, targetTime, weaponIndex, modifiers, range, targetpoint);
	return (attackChance);
}

//---------------------------------------------------------------------------

int32_t
BattleMech::calcHitLocation(
	GameObjectPtr attacker, int32_t weaponIndex, int32_t attackSource, int32_t attackType)
{
	//---------------------------------------------------------------------
	// Need to use the Line-of-fire params to calc section and arc of hit.
	// For now, just use the arc of entry to the target, and select section
	// without using obstructions...
	int32_t armorLocation = -1;
	//-----------------------------------------
	// Section depends upon source of attack...
	int32_t section = MechHitSectionTable[attackSource];
	//------------
	// Find arc...
	float entryAngle = 0.0;
	if (attacker)
		entryAngle = relFacingTo(attacker->getPosition());
	else
		entryAngle = RandomNumber(360) - 180;
	int32_t arc;
	if ((entryAngle >= -45.0) && (entryAngle <= 45.0))
		arc = MECH_HIT_ARC_FRONT;
	else if ((entryAngle > -135.0) && (entryAngle < -45.0))
		arc = MECH_HIT_ARC_LEFT;
	else if ((entryAngle > 45.0) && (entryAngle < 135))
		arc = MECH_HIT_ARC_RIGHT;
	else
		arc = MECH_HIT_ARC_REAR;
	if (attackSource == ATTACKSOURCE_MINE) // if this came from a mine
	{
		if (getAppearance()->getOldGestureGoal() == MECH_STATE_FALLEN_FORWARD)
		{
			arc = MECH_HIT_ARC_FRONT;
			section = MECH_HIT_SECTION_MIDDLE;
		}
		if (getAppearance()->getOldGestureGoal() == MECH_STATE_FALLEN_BACKWARD)
		{
			arc = MECH_HIT_ARC_REAR;
			section = MECH_HIT_SECTION_MIDDLE;
		}
	}
	//-------------------------------
	// Roll, and see where it hits...
	int32_t locationRoll = RandomNumber(100);
	int32_t startEntry =
		section * (NUM_MECH_ARMOR_LOCATIONS * NUM_MECH_HIT_ARCS) + (arc * NUM_MECH_ARMOR_LOCATIONS);
	for (armorLocation = 0; armorLocation < NUM_MECH_ARMOR_LOCATIONS; armorLocation++)
	{
		if (locationRoll < MechHitLocationTable[startEntry + armorLocation])
			break;
		locationRoll -= MechHitLocationTable[startEntry + armorLocation];
	}
	return (armorLocation);
}

//---------------------------------------------------------------------------

int32_t
BattleMech::transferHitLocation(int32_t hitLocation)
{
	Assert((hitLocation >= 0) && (hitLocation < NUM_MECH_BODY_LOCATIONS), hitLocation,
		" BattleMech.transferHitLocation: bad hitLocation ");
	return (MechTransferHitTable[hitLocation]);
}

//---------------------------------------------------------------------------

bool BattleMech::isJumping(Stuff::Vector3D* goal)
{
	if (goal)
		*goal = jumpGoal;
	return (inJump);
}

//---------------------------------------------------------------------------

float BattleMech::getJumpRange(int32_t* numOffsets, int32_t* jumpCost)
{
	// static int32_t jetsToOffsets[7] = {8, 16, 40, 56, 72, 88, 104};
	static int32_t jetsToOffsets[6] = {8, 32, 56, 80, 104, 128};
	int32_t numJets = numJumpJets;
	if (getPilot()->isLongJump())
		numJets++;
	if (numOffsets)
	{
		if (numJets > 5)
			*numOffsets = jetsToOffsets[5];
		else
			*numOffsets = jetsToOffsets[numJets];
	}
	if (jumpCost)
	{
		if (numJets > 0)
			*jumpCost = DefaultMechJumpCost;
		else
			*jumpCost = 0;
	}
	return ((float)numJets * Terrain::worldUnitsPerVertex);
}

//---------------------------------------------------------------------------

bool BattleMech::handleFall(bool forward)
{
	return (false);
}

//---------------------------------------------------------------------------

bool BattleMech::handleEjection(void)
{
	//-------------------------------------------------------
	// Let's let the unit know we're gone if we're a point...
	// Don't do this!!! if we loose the point, we have no point! Simple, no?
	//	if (getPoint() == this) {
	//		std::unique_ptr<Mover> newPointVehicle = getGroup()->selectPoint(true);
	//--------------------------------------------------------
	// If there is no new point, all units must be blown away.
	// How do we want to handle this?
	//	}
	if (pilot && ((pilot->getStatus() == WARRIOR_STATUS_NORMAL) || (pilot->getStatus() == WARRIOR_STATUS_WITHDRAWING)))
	{
		//----------------
		// First, eject...
		getPilot()->eject();
		ejectOrderGiven = true;
		//------------------
		// The head blows...
		destroyBodyLocation(MECH_BODY_LOCATION_HEAD);
		//---------------------
		// Create the Eject FX
		// WHEN READY -fs
		if (CombatLog)
		{
			wchar_t s[1024];
			sprintf(s, "[%.2f] mech.Ejected HE: [%05d]%s", scenarioTime, getPartId(), getName());
			CombatLog->write(s);
			CombatLog->write(" ");
		}
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

//---------------------------------------------------------------------------

bool BattleMech::hitInventoryItem(int32_t itemIndex, bool setupOnly)
{
	// HOW ABOUT...
	// BEFORE we go Trapsing thrugh memory, we assure ourselves
	// that this itemIndex is in the mech?
	if ((itemIndex < 0) || (itemIndex >= (numOther + numWeapons + numAmmos)))
		return false;
	(inventory[itemIndex].health)--;
	bool itemDisabled = false;
	int32_t masterID = inventory[itemIndex].masterID;
	int32_t bodyLocation = inventory[itemIndex].bodyLocation;
	//---------------------------------------------------------------------------
	// First, apply any effects caused whenever this component is hit
	// (regardless of whether it's been disabled or destroyed with this hit)...
	switch (MasterComponent::masterList[masterID].getForm())
	{
	case COMPONENT_FORM_SIMPLE:
		break;
	case COMPONENT_FORM_COCKPIT:
		break;
	case COMPONENT_FORM_SENSOR:
		break;
	case COMPONENT_FORM_ACTUATOR:
		if ((bodyLocation == MECH_BODY_LOCATION_LLEG) || (MECH_BODY_LOCATION_RLEG))
			pilotingCheck(0); // pilotCheckModifier += PilotCheckModifierTable[0];
		break;
	case COMPONENT_FORM_ENGINE:
		break;
	case COMPONENT_FORM_HEATSINK:
	case COMPONENT_FORM_WEAPON:
	case COMPONENT_FORM_WEAPON_ENERGY:
	case COMPONENT_FORM_WEAPON_BALLISTIC:
	case COMPONENT_FORM_WEAPON_MISSILE:
		break;
	case COMPONENT_FORM_AMMO:
		break;
	case COMPONENT_FORM_JUMPJET:
		break;
	case COMPONENT_FORM_CASE:
		break;
	case COMPONENT_FORM_LIFESUPPORT:
		break;
	case COMPONENT_FORM_GYROSCOPE:
		pilotingCheck(0); // pilotCheckModifier += PilotCheckModifierTable[1];
		break;
	case COMPONENT_FORM_POWER_AMPLIFIER:
	case COMPONENT_FORM_BULK:
		break;
	}
	if (getInventoryDamage(itemIndex) >= MasterComponent::masterList[masterID].getDisableLevel())
	{
		//---------------------------------------
		// Component is disabled with this hit...
		inventory[itemIndex].disabled = true;
		itemDisabled = true;
		int32_t damageHotSpot = 1;
		//---------------------------------------------------------------------
		// Now, we need to apply any special effects caused by this component's
		// disabling blow...
		switch (MasterComponent::masterList[masterID].getForm())
		{
		case COMPONENT_FORM_SIMPLE:
		case COMPONENT_FORM_COCKPIT:
			damageHotSpot = 2;
			break;
		case COMPONENT_FORM_SENSOR:
			// Sensors CANNOT be destroyed anymore.  Non-Sensor mechs lose all
			// line of sight then.
			// If you want to destroy the sensor mech, blow it up!
			//				if (sensorSystem)
			//					sensorSystem->broken = true;
			break;
		case COMPONENT_FORM_ECM:
#if 0
				Team::teams[teamId]->removeECM(teamEcmTracker);
				teamEcmTracker = nullptr;
#endif
			break;
		case COMPONENT_FORM_JAMMER:
#if 0
				Team::teams[teamId]->removeJammer(teamJammerTracker);
				teamJammerTracker = nullptr;
#endif
			break;
		case COMPONENT_FORM_ACTUATOR:
			damageHotSpot = 0;
			break;
		case COMPONENT_FORM_ENGINE:
			engineBlowTime = scenarioTime + 5.0;
			damageHotSpot = 1;
			disable(ENGINE_DEATH);
			break;
		case COMPONENT_FORM_HEATSINK:
#ifdef USEHEAT
			if ((inventory[itemIndex].bodyLocation == MECH_BODY_LOCATION_LLEG) || (inventory[itemIndex].bodyLocation == MECH_BODY_LOCATION_RLEG))
				curLegHeatSinks--;
			disabledHeatSinks++;
			heatDissipation = calcHeatDissipation();
			damageHotSpot = 1;
#endif
			break;
		case COMPONENT_FORM_WEAPON:
		case COMPONENT_FORM_WEAPON_ENERGY:
		case COMPONENT_FORM_WEAPON_BALLISTIC:
		case COMPONENT_FORM_WEAPON_MISSILE:
			calcWeaponEffectiveness(false);
			calcFireRanges();
			damageHotSpot = 0;
			pilot->radioMessage(RADIO_WEAPON_DOWN);
			break;
		case COMPONENT_FORM_AMMO:
			break;
		case COMPONENT_FORM_JUMPJET:
			// numJumpJets--;
			break;
		case COMPONENT_FORM_CASE:
			body[inventory[itemIndex].bodyLocation].CASE = false;
			break;
		case COMPONENT_FORM_LIFESUPPORT:
		case COMPONENT_FORM_GYROSCOPE:
			break;
		case COMPONENT_FORM_POWER_AMPLIFIER:
		case COMPONENT_FORM_BULK:
			damageHotSpot = 1;
			break;
		}
	}
	if (inventory[itemIndex].health == 0)
	{
		switch (MasterComponent::masterList[masterID].getForm())
		{
		case COMPONENT_FORM_SIMPLE:
			break;
		case COMPONENT_FORM_COCKPIT:
			//------------------
			// Kill the pilot...
			pilot->injure(6, false);
			break;
		case COMPONENT_FORM_SENSOR:
			break;
		case COMPONENT_FORM_ACTUATOR:
			if ((bodyLocation == MECH_BODY_LOCATION_LLEG) || (bodyLocation == MECH_BODY_LOCATION_RLEG))
				pilotingCheck(PILOTCHECK_SITUATION_NONE,
					100.0); // pilotCheckModifier += 1000;
			break;
		case COMPONENT_FORM_ENGINE:
			break;
		case COMPONENT_FORM_HEATSINK:
		case COMPONENT_FORM_WEAPON:
		case COMPONENT_FORM_WEAPON_ENERGY:
		case COMPONENT_FORM_WEAPON_BALLISTIC:
		case COMPONENT_FORM_WEAPON_MISSILE:
			break;
		case COMPONENT_FORM_AMMO:
			ammoExplosion(itemIndex);
			break;
		case COMPONENT_FORM_JUMPJET:
		case COMPONENT_FORM_CASE:
		case COMPONENT_FORM_LIFESUPPORT:
			break;
		case COMPONENT_FORM_GYROSCOPE:
			// shutDown();
			break;
		case COMPONENT_FORM_POWER_AMPLIFIER:
		case COMPONENT_FORM_BULK:
			break;
		}
		if (!setupOnly && !playedCriticalHit)
		{
			//----------------------------------------------
			// Play critical Hit effect here.
			Stuff::Vector3D smokePos = appearance->getNodePosition(0);
			ObjectManager->createExplosion(MECH_CRITICAL_HIT_ID, nullptr, smokePos);
			appearance->startSmoking(1);
			playedCriticalHit = true;
		}
	}
	return (false);
}

//---------------------------------------------------------------------------

void BattleMech::destroyBodyLocation(int32_t bodyLocation)
{
	if (body[bodyLocation].damageState == IS_DAMAGE_DESTROYED)
		return;
	//	wchar_t report[50];
	//	sprintf(report, "body location %d destroyed.", bodyLocation);
	//	pilot->postReport(REPORTCODE_DEBUGGING, report);
	//----------------------------------------------------------------------
	// Effectively wipe out all critical spaces in the destroyed location...
	body[bodyLocation].curInternalStructure = 0;
	body[bodyLocation].damageState = IS_DAMAGE_DESTROYED;
	armor[bodyLocation].curArmor = 0;
	if ((bodyLocation == MECH_BODY_LOCATION_LLEG) || (bodyLocation == MECH_BODY_LOCATION_RLEG))
	{
		calcLegStatus();
		//----------------
		// Force a fall...
		pilotingCheck(PILOTCHECK_SITUATION_NONE, 100.0); // pilotCheckModifier += 1000;
	}
	else if (bodyLocation == MECH_BODY_LOCATION_CTORSO)
		calcTorsoStatus();
	for (size_t curSpace = 0; curSpace < NumLocationCriticalSpaces[bodyLocation]; curSpace++)
	{
		if (!body[bodyLocation].criticalSpaces[curSpace].hit)
			if (body[bodyLocation].criticalSpaces[curSpace].inventoryID != 255)
			{
				// int32_t form =
				// MasterComponent::masterList[inventory[body[bodyLocation].criticalSpaces[curSpace].inventoryID].masterID].getForm();
				// if ((form != COMPONENT_FORM_BULK) && (form !=
				// COMPONENT_FORM_CASE)) {
				body[bodyLocation].criticalSpaces[curSpace].hit = true;
				hitInventoryItem(body[bodyLocation].criticalSpaces[curSpace].inventoryID);
				//}
			}
	}
	//----------------------------------------------------------------------
	// If the location is a side torso, destroy the attached arm, as well...
	if (bodyLocation == MECH_BODY_LOCATION_CTORSO)
	{
		//----------------------------------------------------------------
		// Mech essentially destroyed. Does it explode or simply shutdown?
		// For now, it's always exploding...
		if ((scenarioTime <= centerTorsoBlowTime) || ((engineBlowTime > -1.0) && (scenarioTime <= engineBlowTime)))
		{
			//------------------
			// Should explode...
			pilot->handleAlarm(PILOT_ALARM_VEHICLE_INCAPACITATED, 0);
			ObjectManager->getObjectType(typeHandle)->handleDestruction(this, nullptr);
		}
		else
		{
			//-------------------
			// Should shutdown...
			disable(ENGINE_DEATH);
		}
	}
	else if (bodyLocation == MECH_BODY_LOCATION_RTORSO)
		destroyBodyLocation(MECH_BODY_LOCATION_RARM);
	else if (bodyLocation == MECH_BODY_LOCATION_LTORSO)
		destroyBodyLocation(MECH_BODY_LOCATION_LARM);
	else if (bodyLocation == MECH_BODY_LOCATION_LARM)
		leftArmBlownThisFrame = true;
	else if (bodyLocation == MECH_BODY_LOCATION_RARM)
		rightArmBlownThisFrame = true;
}

//---------------------------------------------------------------------------

void BattleMech::calcCriticalHit(int32_t hitLocation)
{
	//----------------------------------------------------------
	// If we're playing multiplayer and we're not the server,
	// exit--wait for the critical hit chunks from the server...
	if (MPlayer && !MPlayer->isServer())
		return;
	//----------------------------------------------
	// Cannot critically hit nothing.!!
	if (body[hitLocation].totalSpaces == 0)
		return;
	int32_t numCriticalHits = 0;
	int32_t bodyLocation = MechArmorToBodyLocation[hitLocation];
	int32_t numBodySpaces = NumLocationCriticalSpaces[bodyLocation];
	int32_t criticalRoll = RandomNumber(100);
	if (criticalRoll < CriticalHitTable[0])
		return;
	else if (criticalRoll < CriticalHitTable[1])
		numCriticalHits = 1;
	else if (criticalRoll < CriticalHitTable[2])
		numCriticalHits = 2;
	else if (isTorso(bodyLocation))
		numCriticalHits = 3;
	if (numCriticalHits > 0)
	{
		for (size_t curHit = 0; curHit < numCriticalHits; curHit++)
		{
			int32_t critHitRoll = RandomNumber(body[bodyLocation].totalSpaces);
			int32_t hitSpace = 0;
			int32_t numSpaces = 0;
			for (hitSpace = 0; hitSpace < numBodySpaces; hitSpace++)
			{
				numSpaces = 0;
				if (body[bodyLocation].criticalSpaces[hitSpace].inventoryID < 255)
					numSpaces =
						MasterComponent::masterList
							[inventory[body[bodyLocation].criticalSpaces[hitSpace].inventoryID]
									.masterID]
								.getSize();
				if (critHitRoll < numSpaces)
					break;
				else
					critHitRoll -= numSpaces;
			}
			// If we are out of range, no critical hit!
			if (bodyLocation < 0 || bodyLocation > 7)
				return;
			//				STOP(("Bad bodyLocation in CriticalHit
			//%d",bodyLocation));
			if (hitSpace < 0 || hitSpace >= NumLocationCriticalSpaces[bodyLocation])
				return;
			//				STOP(("Bad Critical Hit Space %d",hitSpace));
			body[bodyLocation].criticalSpaces[hitSpace].hit = 1;
			hitInventoryItem(body[bodyLocation].criticalSpaces[hitSpace].inventoryID);
			if (MPlayer)
				addCriticalHitChunk(CHUNK_SEND, bodyLocation, hitSpace);
		}
	}
	else
	{
		destroyBodyLocation(bodyLocation);
		if (MPlayer)
			addCriticalHitChunk(CHUNK_SEND, bodyLocation, 15);
	}
}

//---------------------------------------------------------------------------

void BattleMech::handleCriticalHit(int32_t bodyLocation, int32_t criticalSpace)
{
	if (criticalSpace == 15)
		destroyBodyLocation(bodyLocation);
	else
		hitInventoryItem(body[bodyLocation].criticalSpaces[criticalSpace].inventoryID);
}

//---------------------------------------------------------------------------

int32_t
BattleMech::updateCriticalHitChunks(int32_t which)
{
	for (size_t i = 0; i < numCriticalHitChunks[which]; i++)
	{
		uint8_t chunkData = criticalHitChunks[which][i];
		int32_t criticalSpace = chunkData & 0x0F;
		int32_t bodyLocation = (chunkData >> 4);
		handleCriticalHit(bodyLocation, criticalSpace);
	}
	numCriticalHitChunks[which] = 0;
	return (NO_ERROR);
}

//---------------------------------------------------------------------------

int32_t
BattleMech::buildStatusChunk(void)
{
	//-----------------------------------------------------------------------
	// For now, we'll just track a failed piloting check for 3 status updates.
	// If all three updates are never received, then the clients won't know
	// to fall! This should be adequate. If it proves otherwise, we can
	// change this...
	statusChunk.init();
	//--------
	// hack...
	//	if (appearance->getCurrentGestureId() == 1)
	//		bodyState = MECH_STATE_PARKED;
	if (isDestroyed())
		statusChunk.bodyState = MECH_STATUSCHUNK_BODYSTATE_DESTROYED;
	else
		statusChunk.bodyState = getBodyState();
	if (pilot)
	{
		if (inJump)
		{
			statusChunk.jumpOrder = true;
			int32_t cellRC[2];
			land->worldToCell(jumpGoal, cellRC[0], cellRC[1]);
			statusChunk.targetCellRC[0] = cellRC[0];
			statusChunk.targetCellRC[1] = cellRC[1];
		}
		else
		{
			GameObjectPtr curTarget = pilot->getLastTarget();
			if (curTarget)
			{
				if (curTarget->isMover())
				{
					statusChunk.targetType = STATUSCHUNK_TARGET_MOVER;
					statusChunk.targetId = ((std::unique_ptr<Mover>)curTarget)->getNetRosterIndex();
				}
				else if (curTarget->isTerrainObject())
				{
					statusChunk.targetType = STATUSCHUNK_TARGET_TERRAIN;
					statusChunk.targetId = curTarget->getPartId();
				}
				else
					Fatal(curTarget->getObjectClass(),
						" BattleMech.buildStatusChunk: bad target type ");
			}
		}
	}
	statusChunk.ejectOrderGiven = ejectOrderGiven;
	statusChunk.pack(this);
#ifdef DEBUG_CHUNKS
	StatusChunk chunk2;
	chunk2.data = statusChunk.data;
	chunk2.unpack(this);
	if (!statusChunk.equalTo(&chunk2))
		Fatal(0, " BAD status chunk in mech: save stchunk.dbg file! ");
#endif
	return (NO_ERROR);
}

//---------------------------------------------------------------------------

int32_t
BattleMech::handleStatusChunk(int32_t updateAge, uint32_t chunk)
{
	statusChunk.init();
	statusChunk.data = chunk;
	statusChunk.unpack(this);
	//------------------------------------------------------------
	// HACK!!!!!!!!!!!!!! If bad packet, no statuschunk for you...
	if (StatusChunkUnpackErr != 0)
		return (NO_ERROR); // Assert(0, " BAD PACKET! ");
	int32_t lastTargetId = 0;
	if (!statusChunk.jumpOrder)
	{
		if (statusChunk.targetType > STATUSCHUNK_TARGET_NONE)
		{
			switch (statusChunk.targetType)
			{
			case STATUSCHUNK_TARGET_MOVER:
				if (MPlayer->moverRoster[statusChunk.targetId])
					lastTargetId = MPlayer->moverRoster[statusChunk.targetId]->getPartId();
				break;
			case STATUSCHUNK_TARGET_TERRAIN:
			case STATUSCHUNK_TARGET_SPECIAL:
				lastTargetId = statusChunk.targetId;
				break;
			}
		}
	}
	if (pilot)
	{
		if (lastTargetId == 0)
			pilot->setLastTarget(nullptr);
		else
		{
			GameObjectPtr curTarget = pilot->getLastTarget();
			if (!curTarget || (curTarget->getPartId() != lastTargetId))
			{
				curTarget = ObjectManager->findByPartId(lastTargetId);
				pilot->setLastTarget(curTarget);
			}
		}
		if (!ejectOrderGiven && statusChunk.ejectOrderGiven)
		{
			//--------------------------
			// Just given eject order...
			ejectOrderGiven = true;
			handleEjection();
		}
	}
	if (statusChunk.bodyState == MECH_STATUSCHUNK_BODYSTATE_DESTROYED)
		setStatus(OBJECT_STATUS_DESTROYED);
	return (NO_ERROR);
}

//---------------------------------------------------------------------------

#pragma optimize("", off)

int32_t
BattleMech::buildMoveChunk(void)
{
	moveChunk.init();
	if (pilot)
	{
		//--------------------------------------------
		// First, make sure we have the latest path...
		pilot->getMovePath();
		//--------------------------------------------------
		// Now, use it (and any queued one, if available)...
		moveChunk.build(this, pilot->getMovePath(0), pilot->getMovePath(1));
	}
	moveChunk.pack(this);
#ifdef DEBUG_CHUNKS
	MoveChunk chunk2;
	chunk2.data = moveChunk.data;
	chunk2.unpack(this);
	//----------------------------------------------------------
	// HACK!!!!!!!!!!!!!! If bad packet, no movechunk for you...
	if (MoveChunk::err != 0)
	{
		moveChunk.init();
		moveChunk.build(this, nullptr, nullptr);
		moveChunk.pack(this);
		return (NO_ERROR); // Assert(0, " BAD PACKET! ");
	}
	if (!moveChunk.equalTo(this, &chunk2))
		Fatal(0, " Bad mech movechunk: save mvchunk.dbg file! ");
#endif
	return (NO_ERROR);
}

//---------------------------------------------------------------------------

int32_t
BattleMech::handleMoveChunk(uint32_t chunk)
{
	moveChunk.init();
	moveChunk.data = chunk;
	moveChunk.unpack(this);
	//----------------------------------------------------------
	// HACK!!!!!!!!!!!!!! If bad packet, no movechunk for you...
	if (MoveChunk::err != 0)
		return (NO_ERROR); // Assert(0, " BAD PACKET! ");
	MovePathPtr path = getPilot()->getMovePath();
	setMoveChunk(path, &moveChunk);
	//-------------------------------------------------------------
	// Now, let's determine which opening step we're closest to so
	// we don't loop back to the first step everytime we get a new
	// chunk of the path...
	if (path->numStepsWhenNotPaused > 1)
	{
		int32_t i = 0;
		for (i = path->numStepsWhenNotPaused - 1; i > 0; i--)
		{
			float distanceFromPt = distanceFrom(path->stepList[i].destination);
			if (distanceFromPt <= MapCellDiagonal)
				break;
		}
		path->setCurStep(i);
	}
	newMoveChunk = true;
	return (NO_ERROR);
}

#pragma optimize("", on)

//---------------------------------------------------------------------------

bool BattleMech::injureBodyLocation(int32_t bodyLocation, float damage)
{
	BodyLocationPtr location = &body[bodyLocation];
	if ((bodyLocation == MECH_BODY_LOCATION_CTORSO) && (centerTorsoBlowTime < 0.0))
		centerTorsoBlowTime = scenarioTime;
	if (damage > location->curInternalStructure)
		location->curInternalStructure = 0;
	else
		location->curInternalStructure -= damage;
	if (location->curInternalStructure > 0)
	{
		float damageLevel = (float)body[bodyLocation].curInternalStructure / body[bodyLocation].maxInternalStructure;
		if (damageLevel <= 0.5)
			body[bodyLocation].damageState = IS_DAMAGE_PARTIAL;
		else
			body[bodyLocation].damageState = IS_DAMAGE_NONE;
		if ((bodyLocation == MECH_BODY_LOCATION_LLEG) || (bodyLocation == MECH_BODY_LOCATION_RLEG))
			calcLegStatus();
		else if (bodyLocation == MECH_BODY_LOCATION_CTORSO)
			calcTorsoStatus();
		calcCriticalHit(bodyLocation);
	}
	else
	{
		destroyBodyLocation(bodyLocation);
		return (true);
	}
	//-----------------------------------------------
	// Location was not destroyed, so return false...
	return (false);
}

//---------------------------------------------------------------------------

float BattleMech::weaponLocked(int32_t weaponIndex, Stuff::Vector3D targetposition)
{
	return (relFacingTo(targetposition, inventory[weaponIndex].bodyLocation));
}

//---------------------------------------------------------------------------

#if 0
// UNDER CONSTRUCTION :)
bool weaponsLockedWithApproach(vector_3d goalPos, vector_3d targetPos, bool forwardApproached)
{
	GameObjectPtr target = pilot->getCurrentTarget();
	if(!target)
		return(-2);
	vector_3d targetposition = target->getPosition();
	int32_t numLocked = 0;
	float fireArc = getFireArc();
	if(listSize == -1)
		for(size_t item = numOther; item < (numOther + numWeapons); item++)
		{
			float relAngle = weaponLocked(item, targetposition);
			if((relAngle >= -fireArc) && (relAngle <= fireArc))
				list[numLocked++] = item;
		}
	else
		for(size_t item = 0; item < listSize; item++)
		{
			float relAngle = weaponLocked(list[item], targetposition);
			if((relAngle >= -fireArc) && (relAngle <= fireArc))
				list[numLocked++] = list[item];
		}
	return(numLocked);
}
#endif

//---------------------------------------------------------------------------

int32_t
BattleMech::handleWeaponHit(WeaponShotInfoPtr shotInfo, bool addMultiplayChunk)
{
	bool localInvulnerable = false;
	if (invulnerableON && Team::home == getTeam())
		localInvulnerable = true;
	if (!shotInfo)
		return (NO_ERROR);
	printHandleWeaponHitDebugInfo(shotInfo);
	if ((getTeam() && Team::noPain[getTeamId()]) || localInvulnerable)
		return (NO_ERROR);
	if (!MPlayer && CantTouchThis && pilot->onHomeTeam())
		return (NO_ERROR);
	//-----------------
	// FOR DEBUGGING...
	GameObjectPtr attacker = ObjectManager->getByWatchID(shotInfo->attackerWID);
	BadGuy = attacker;
	//----------------------------------
	// Is this possible? Just in case...
	if ((shotInfo->damage <= 0) || (shotInfo->hitLocation == -1))
		return (NO_ERROR);
	//-------------------------------------------------------------------
	// This may not be possible, but just in case. Possibly someone nails
	// a mech after it's been destroyed, but before it's deleted from
	// the object list...
	if (isDestroyed())
		return (NO_ERROR);
	numWeaponHitsHandled++;
	if (addMultiplayChunk)
		MPlayer->addWeaponHitChunk(this, shotInfo);
	if (isDisabled() && fallen)
	{
		if (getMoveType() == MOVETYPE_GROUND)
		{
			damageAfterDisabled += shotInfo->damage;
			if (damageAfterDisabled > ((BattleMechTypePtr)getObjectType())->destructDamage)
			{
				// Once the server does this, the status chunk will update the
				// clients!!
				if (!MPlayer || (MPlayer && MPlayer->isServer()))
				{
					timeLeft = 1.0f;
					setStatus(OBJECT_STATUS_DESTROYED);
					exploding = false;
					for (size_t i = 0; i < numBodyLocations;
						 i++) // Blow all of the components.  NO SALVAGE!!
						destroyBodyLocation(i);
					if (CombatLog)
					{
						wchar_t s[1024];
						sprintf(s, "[%.2f] mech.destroyed HWH: [%05d]%s", scenarioTime, getPartId(),
							getName());
						CombatLog->write(s);
						CombatLog->write(" ");
					}
				}
			}
		}
		else
		{
			// Its a helicopter.  Blow it up.  No salvage
			setStatus(OBJECT_STATUS_DESTROYED);
			exploding = false;
			for (size_t i = 0; i < numBodyLocations;
				 i++) // Blow all of the components.  NO SALVAGE!!
				destroyBodyLocation(i);
#ifdef USE_MOODMUSIC
			//------------------------------------
			// What heroic music should be played?
			if (collidee->getAlignment() == Team::home->getAlignment())
				friendlydestroyed = true;
			else
				enemydestroyed = true;
#endif
			if (CombatLog)
			{
				wchar_t s[1024];
				sprintf(s, "[%.2f] helicopter.destroyed: [%05d]%s", scenarioTime, getPartId(),
					getName());
				CombatLog->write(s);
				CombatLog->write(" ");
			}
		}
		return (NO_ERROR);
	}
	//------------------------------------------------------------
	// Since Multiplayer still needs this, preserve it and restore
	// at the end of this function!
	WeaponShotInfo startShotInfo = *shotInfo;
	bool alreadyDisabled = isDisabled();
	bool isAmmoExplosion = false;
	isAmmoExplosion = (shotInfo->masterId == -4);
	if (shotInfo->masterId > 0)
		isAmmoExplosion =
			(MasterComponent::masterList[shotInfo->masterId].getForm() == COMPONENT_FORM_AMMO);
	// pilotCheckModifier += (shotInfo->damage * PilotingCheckFactor);
	//---------------------------------------------------
	// Tally the damage for rate check and pilot check...
	damageRateTally += shotInfo->damage;
	pilotCheckDamageTally += shotInfo->damage;
	//---------------------------------
	// Did the hit cause us to stumble?
	float torsoEntryAngle = shotInfo->entryAngle + torsoRotation;
	if ((torsoEntryAngle >= -90.0 /*-45.0*/) && (torsoEntryAngle <= 90.0 /*45.0*/))
		hitFromFrontThisFrame = true;
	else if ((torsoEntryAngle <= -91.0 /*-135.0*/) || (torsoEntryAngle >= 91.0 /*135.0*/))
		hitFromBehindThisFrame = true;
	//-----------------------------------
	// First, what type of hit is this...
	damageThisFrame += shotInfo->damage;
	int32_t totalHitType = 0;
	if (damageThisFrame < hitLevel[0])
		totalHitType = HIT_TYPE_WEAK;
	else if (damageThisFrame < hitLevel[1])
		totalHitType = HIT_TYPE_MODERATE;
	else
		totalHitType = HIT_TYPE_HEAVY;
	int32_t hitType = 0;
	if (shotInfo->damage < hitLevel[0])
		hitType = HIT_TYPE_WEAK;
	else if (shotInfo->damage < hitLevel[1])
		hitType = HIT_TYPE_MODERATE;
	else
		hitType = HIT_TYPE_HEAVY;
	//------------------------------------------------
	// If this was a heavy hit, play the hit gesture.
	if (!MPlayer && (totalHitType == HIT_TYPE_MODERATE) && (getCommanderId() != Commander::home->getId()))
	{
		if ((torsoEntryAngle >= -45.0) && (torsoEntryAngle <= 45.0))
			appearance->hitFront();
		else if ((torsoEntryAngle <= -45.0) && (torsoEntryAngle >= -135.0))
			appearance->hitLeft();
		else if ((torsoEntryAngle >= 45.0) && (torsoEntryAngle <= 135.0))
			appearance->hitRight();
		else if ((torsoEntryAngle <= -135.0) || (torsoEntryAngle >= 135.0))
			appearance->hitBack();
	}
	//-------------------------------------------------------
	// Note that the hitLocation is the ARMOR hit location...
	int32_t bodyLocation = MechArmorToBodyLocation[shotInfo->hitLocation];
	//----------------------------------------
	// Check for any special pilot injuries...
	if ((bodyLocation == MECH_BODY_LOCATION_HEAD) && (shotInfo->damage >= 2))
		pilot->injure(1);
	bool armorHoled = false;
	if ((armor[shotInfo->hitLocation].curArmor > 0) && !isAmmoExplosion)
	{
		if (shotInfo->damage > armor[shotInfo->hitLocation].curArmor)
		{
			shotInfo->setDamage(shotInfo->damage - armor[shotInfo->hitLocation].curArmor);
			armor[shotInfo->hitLocation].curArmor = 0;
			if (!armorHoled)
				pilot->radioMessage(RADIO_ARMORHOLE);
			armorHoled = true;
			// Check for Tutorials here.
			// IN the tutorial missions, your mechs can only have armor damaged.
			// NO INTERNALS ALLOWED!! ONLY the tutorials should set this flag
			// through ABL!!!
			if (!localInvulnerable)
			{
				if (shotInfo->damage >= body[bodyLocation].curInternalStructure)
				{
					shotInfo->setDamage(shotInfo->damage - body[bodyLocation].curInternalStructure);
					injureBodyLocation(bodyLocation, body[bodyLocation].curInternalStructure);
					if (shotInfo->damage > 0)
					{
						if ((bodyLocation == MECH_BODY_LOCATION_CTORSO) || (bodyLocation == MECH_BODY_LOCATION_HEAD))
						{
							//----------------------------------------------------------------------
							// If hit in the center torso, no where else to
							// go... If hit in the head, it is disabled. What
							// that means at this point...? We'll probably want
							// to put a disabled check in there, somewhere :)
							// Most likely, the mech will have been blown up
							// and/or destroyed before it's "disabled"...
						}
						else
						{
							WeaponShotInfo newShotInfo = *shotInfo;
							newShotInfo.hitLocation =
								transferHitLocation(bodyLocation /*shotInfo->hitLocation*/);
							if (MPlayer)
							{
								if (MPlayer->isServer())
								{
									if (CombatLog)
										CombatLog->write("     HIT TRANSFER 1...");
									handleWeaponHit(&newShotInfo, true);
								}
							}
							else
							{
								if (CombatLog)
									CombatLog->write("     HIT TRANSFER 1...");
								handleWeaponHit(&newShotInfo);
							}
						}
					}
				}
				else
				{
					injureBodyLocation(bodyLocation, shotInfo->damage);
				}
			}
		}
		else
		{
			armor[shotInfo->hitLocation].curArmor -= shotInfo->damage;
		}
	}
	else if (!localInvulnerable) // Again, for tutorials.  See above check.
	{
		if (body[bodyLocation].curInternalStructure > 0)
		{
			if (shotInfo->damage >= body[bodyLocation].curInternalStructure)
			{
				shotInfo->setDamage(shotInfo->damage - body[bodyLocation].curInternalStructure);
				injureBodyLocation(bodyLocation, body[bodyLocation].curInternalStructure);
				if (shotInfo->damage > 0)
				{
					if (isAmmoExplosion && body[bodyLocation].CASE)
					{
						//---------------------------------------------
						// Have CASE here, so vent the excess damage...
						int32_t armorLocation = MECH_ARMOR_LOCATION_RCTORSO;
						switch (bodyLocation)
						{
						case MECH_BODY_LOCATION_LTORSO:
						case MECH_BODY_LOCATION_LARM:
						case MECH_BODY_LOCATION_LLEG:
							armorLocation = MECH_ARMOR_LOCATION_RLTORSO;
							break;
						case MECH_BODY_LOCATION_RTORSO:
						case MECH_BODY_LOCATION_RARM:
						case MECH_BODY_LOCATION_RLEG:
							armorLocation = MECH_ARMOR_LOCATION_RRTORSO;
							break;
						}
						if (shotInfo->damage > armor[armorLocation].curArmor)
						{
							armor[armorLocation].curArmor = 0;
							armorHoled = true;
						}
						else
							armor[armorLocation].curArmor -= shotInfo->damage;
						shotInfo->setDamage(0);
					}
					else
					{
						if ((bodyLocation == MECH_BODY_LOCATION_CTORSO) || (bodyLocation == MECH_BODY_LOCATION_HEAD))
						{
							//------------------------------
							// Hit center torso, already, or
							// Head wiped out... what to do?
						}
						else
						{
							WeaponShotInfo newShotInfo = *shotInfo;
							newShotInfo.hitLocation =
								transferHitLocation(bodyLocation /*shotInfo->hitLocation*/);
							if (MPlayer)
							{
								if (MPlayer->isServer())
								{
									if (CombatLog)
										CombatLog->write("     HIT TRANSFER 2...");
									handleWeaponHit(&newShotInfo, true);
								}
							}
							else
							{
								if (CombatLog)
									CombatLog->write("     HIT TRANSFER 2...");
								handleWeaponHit(&newShotInfo);
							}
						}
					}
				}
			}
			else
			{
				injureBodyLocation(bodyLocation, shotInfo->damage);
			}
		}
		else
		{
			if (isAmmoExplosion && body[bodyLocation].CASE)
			{
				//---------------------------------------------
				// Have CASE here, so vent the excess damage...
				int32_t armorLocation = MECH_ARMOR_LOCATION_RCTORSO;
				switch (bodyLocation)
				{
				case MECH_BODY_LOCATION_LTORSO:
				case MECH_BODY_LOCATION_LARM:
				case MECH_BODY_LOCATION_LLEG:
					armorLocation = MECH_ARMOR_LOCATION_RLTORSO;
					break;
				case MECH_BODY_LOCATION_RTORSO:
				case MECH_BODY_LOCATION_RARM:
				case MECH_BODY_LOCATION_RLEG:
					armorLocation = MECH_ARMOR_LOCATION_RRTORSO;
					break;
				}
				if (shotInfo->damage > armor[armorLocation].curArmor)
				{
					armor[armorLocation].curArmor = 0;
					armorHoled = true;
				}
				else
					armor[armorLocation].curArmor -= shotInfo->damage;
				shotInfo->setDamage(0);
			}
			else
			{
				if ((bodyLocation == MECH_BODY_LOCATION_CTORSO) || (bodyLocation == MECH_BODY_LOCATION_HEAD))
				{
					//------------------------------
					// Already in center torso, or
					// Head wiped out... what to do?
				}
				else
				{
					WeaponShotInfo newShotInfo = *shotInfo;
					newShotInfo.hitLocation =
						transferHitLocation(bodyLocation /*shotInfo->hitLocation*/);
					if (MPlayer)
					{
						if (MPlayer->isServer())
						{
							if (CombatLog)
								CombatLog->write("     HIT TRANSFER 3...");
							handleWeaponHit(&newShotInfo, true);
						}
					}
					else
					{
						if (CombatLog)
							CombatLog->write("     HIT TRANSFER 3...");
						handleWeaponHit(&newShotInfo);
					}
				}
			}
		}
	}
	//------------------------------------------------------
	// Now, let's apply any remaining effects that should be
	// applied as a result of this hit...
	if (inventory[gyro].disabled)
	{
		switch (hitType)
		{
		case HIT_TYPE_WEAK:
			//-----------------------------------------
			// Torso twists are caused by the impact...
			break;
		case HIT_TYPE_MODERATE:
			//-------------------------------------------------------------
			// A piloting roll should be made to check for possible fall...
			break;
		}
	}
	//---------------------------------------------
	// Let anyone who should know that I was hit...
	//--------------------------------
	// Trigger the WEAPON HIT event...
	if (attacker)
	{
		if (shotInfo->masterId > -1)
			pilot->triggerAlarm(PILOT_ALARM_HIT_BY_WEAPONFIRE, attacker->getWatchID());
		else if (shotInfo->masterId == -4)
			pilot->triggerAlarm(PILOT_ALARM_HIT_BY_WEAPONFIRE, attacker->getWatchID());
		else
			pilot->triggerAlarm(PILOT_ALARM_COLLISION, attacker->getWatchID());
	}
	else
	{
		if (shotInfo->masterId == -4)
			pilot->triggerAlarm(PILOT_ALARM_HIT_BY_WEAPONFIRE, 0);
		else if (shotInfo->masterId < 0)
		{
			//------------------------------------------------------------
			// If no attacker and a negative weapon masterId, then it must
			// be a mine or artillery...
			pilot->triggerAlarm(PILOT_ALARM_HIT_BY_WEAPONFIRE, shotInfo->masterId);
		}
		else
			pilot->triggerAlarm(PILOT_ALARM_HIT_BY_WEAPONFIRE, 0);
	}
	//--------------------------------------------
	// Calc current CV, after this damage taken...
	curCV = calcCV(false);
	if (isDisabled())
		setThreatRating(1);
	else
		setThreatRating(-1);
	//-----------------------------------------------------
	// If this mech was destroyed, let the attacker know...
	if (!alreadyDisabled && isDisabled())
	{
		if (attacker && attacker->isMover())
		{
			((std::unique_ptr<Mover>)attacker)->getPilot()->triggerAlarm(PILOT_ALARM_KILLED_TARGET, getWatchID());
			if (!killed && MPlayer && MPlayer->isServer())
			{
				if (moveType != MOVETYPE_AIR)
				{
					if (MPlayer->missionSettings.resourceForMechs)
						if (attacker->getCommanderId() != getCommanderId())
							MPlayer->sendReinforcement(getResourcePointValue(), 0, "noname",
								attacker->getCommanderId(), attacker->getPosition(), 6);
					// MPlayer->playerInfo[attacker->getCommanderId()].resourcePoints
					// += getResourcePointValue();
					if (MPlayer->missionSettings.missionType == MISSION_TYPE_ELIMINATION)
					{
						if (attacker->getCommanderId() != getCommanderId())
							MPlayer->addTeamScore(
								MPlayer->playerInfo[attacker->getCommanderId()].team, cBills / 100);
					}
				}
				int32_t attackerCID = attacker->getCommanderId();
				if (attackerCID == -1)
					attackerCID = ((std::unique_ptr<Mover>)attacker)->prevCommanderId;
				if ((attackerCID == -1) || (attackerCID == getCommanderId()))
					attackerCID = MAX_MC_PLAYERS;
				MPlayer->addKillLossChunk(attackerCID, !lost ? getCommanderId() : MAX_MC_PLAYERS);
				lost = true;
				killed = true;
			}
		}
		else if (attacker && (attacker->getObjectClass() == ARTILLERY))
		{
			if (!killed && MPlayer && MPlayer->isServer())
			{
				if (moveType != MOVETYPE_AIR)
				{
					if (MPlayer->missionSettings.resourceForMechs)
						if (attacker->getCommanderId() != getCommanderId())
							MPlayer->sendReinforcement(getResourcePointValue(), 0, "noname",
								attacker->getCommanderId(), attacker->getPosition(), 6);
					// MPlayer->playerInfo[attacker->getCommanderId()].resourcePoints
					// += getResourcePointValue();
					if (MPlayer->missionSettings.missionType == MISSION_TYPE_ELIMINATION)
					{
						if (attacker->getCommanderId() != getCommanderId())
							MPlayer->addTeamScore(
								MPlayer->playerInfo[attacker->getCommanderId()].team, cBills / 100);
					}
				}
				int32_t attackerCID = attacker->getCommanderId();
				if (attackerCID == -1)
					attackerCID = ((std::unique_ptr<Mover>)attacker)->prevCommanderId;
				if ((attackerCID == -1) || (attackerCID == getCommanderId()))
					attackerCID = MAX_MC_PLAYERS;
				MPlayer->addKillLossChunk(attackerCID, !lost ? getCommanderId() : MAX_MC_PLAYERS);
				lost = true;
				killed = true;
			}
		}
	}
	*shotInfo = startShotInfo;
	return (NO_ERROR);
}

//---------------------------------------------------------------------------

int32_t
BattleMech::fireWeapon(GameObjectPtr target, float targetTime, int32_t weaponIndex,
	int32_t attackType, int32_t aimLocation, Stuff::Vector3D* targetpoint, float& dmgDone)
{
	//	if (getTeam() != Team::home)
	//		return(1);
	if ((status == OBJECT_STATUS_SHUTDOWN) || (status == OBJECT_STATUS_SHUTTING_DOWN) || (status == OBJECT_STATUS_DISABLED) || (status == OBJECT_STATUS_DESTROYED))
		return (1);
	if ((getBodyState() == MECH_STATUSCHUNK_BODYSTATE_PARKED) || (getBodyState() == MECH_STATUSCHUNK_BODYSTATE_FALLEN_FORWARD) || (getBodyState() == MECH_STATUSCHUNK_BODYSTATE_FALLEN_BACKWARD))
		return (1);
	if (!isWeaponReady(weaponIndex))
		return (3);
	if (!Terrain::IsGameSelectTerrainPosition(getPosition()))
		return (1);
	float distanceToTarget = 0.0;
	if (target)
	{
		if (target->isDestroyed())
			return (4);
		// We did this in calcWeaponsStatus.  Do we really need to do it
		// again??!
		// I have decided NO as of now!
		/*
		if (getWeaponIndirectFire(weaponIndex))
		{
			if (!getTeam()->teamLineOfSight(target->getPosition()))
				return(4);
		}
		else
		{
			if (!lineOfSight(target))
				return(4);
		}
		*/
		distanceToTarget = distanceFrom(target->getPosition());
	}
	else if (targetpoint)
	{
		// We did this in calcWeaponsStatus.  Do we really need to do it
		// again??!
		// I have decided NO as of now!
		/*
		if (getWeaponIndirectFire(weaponIndex))
		{
			if (!getTeam()->teamLineOfSight(*targetpoint))
				return(4);
		}
		else
		{
			if (!lineOfSight(*targetpoint))
				return(4);
		}
		*/
		distanceToTarget = distanceFrom(*targetpoint);
	}
	else
		return (4);
	bool inRange = weaponInRange(weaponIndex, distanceToTarget, MapCellDiagonal);
	//-----------------------------------------
	// Let's make sure the target's in range...
	if (MPlayer)
	{
		if (MPlayer->isServer() && !inRange)
			return (4);
	}
	else if (!inRange)
		return (4);
	int32_t numShots = getWeaponShots(weaponIndex);
	if (numShots == 0)
		return (4);
	//------------------------------------------
	// As of 4/9/97, missiles cannot be aimed...
	// As of 4/30/98, they can be again... -fs
	// if ((aimLocation != -1) &&
	// (MasterComponent::masterList[inventory[weaponIndex].masterID].getForm() ==
	// COMPONENT_FORM_WEAPON_MISSILE)) 	return(4);
	//-------------------------------------------
	// Ripple Fire
	// Figure out what weapon Node on the Mech we are going to use
	// And see if its recycle time is Green.  If not, just return, we'll shoot
	// again!
	int32_t sourceHotSpot;
	if (MasterComponent::masterList[inventory[weaponIndex].masterID].getForm() == COMPONENT_FORM_WEAPON_MISSILE)
		sourceHotSpot = MECH3D_WEAPONTYPE_MISSILE;
	else if (MasterComponent::masterList[inventory[weaponIndex].masterID].getForm() == COMPONENT_FORM_WEAPON_BALLISTIC)
		sourceHotSpot = MECH3D_WEAPONTYPE_BALLISTIC;
	else
		sourceHotSpot = MECH3D_WEAPONTYPE_ENERGY;
	sourceHotSpot = inventory[weaponIndex].facing;
	if (appearance->getRightArmOff() && (sourceHotSpot == 1))
		sourceHotSpot = 3; // Force into right torso.  Arm is gone.
	if (appearance->getLeftArmOff() && (sourceHotSpot == 2))
		sourceHotSpot = 5; // Force into left torso.  Arm is gone.
	float recycleTime = appearance->getWeaponNodeRecycle(sourceHotSpot);
	if (recycleTime > 0.0f)
		return (3);
	// Set the hotspot for the target
	int32_t targetHotSpot = 0;
	if (target && target->getAppearance())
	{
		targetHotSpot = target->getAppearance()->getWeaponNode(MECH3D_WEAPONTYPE_ANY);
	}
	//-------------------------------------------
	float entryAngle = 0.0;
	if (target)
		entryAngle = target->relFacingTo(position);
	// int32_t weaponType =
	// MasterComponent::masterList[inventory[weaponIndex].masterID].getWeaponType();
	bool isStreakMissile =
		MasterComponent::masterList[inventory[weaponIndex].masterID].getWeaponStreak();
	int32_t range;
	int32_t attackChance = (int32_t)calcAttackChance(
		target, aimLocation, targetTime, weaponIndex, 0.0, &range, targetpoint);
	int32_t hitRoll = RandomNumber(100);
	if (target)
	{
		if (target->getTeamId() == TEAM2)
		{
			pilot->incNumSkillUses(COMBAT_STAT_MISSION, Skill::gunnery);
			pilot->skillPoints[Skill::gunnery] += SkillTry[Skill::gunnery];
		}
		else
			pilot->skillPoints[Skill::gunnery] += SkillTry[Skill::gunnery] / 10;
	}
	//---------------------------------------------------------------
	// HACK: If aiming a shot and moving, make chance to hit equal to
	// zero, yet still take shot (denny request: 4/22/98)
	// if ((aimLocation != -1) && (getSpeed() > 0.0))
	//	attackChance = 0;
	int32_t hitLocation = -2;
	if (target && (hitRoll < attackChance))
	{
		if (target->getTeamId() == TEAM2)
		{
			pilot->incNumSkillSuccesses(COMBAT_STAT_MISSION, Skill::gunnery);
			pilot->skillPoints[Skill::gunnery] += SkillSuccess[Skill::gunnery];
		}
		else
			pilot->skillPoints[Skill::gunnery] += SkillSuccess[Skill::gunnery] / 10;
		//------------------------------------------------------------------
		// If it's an aimed shot, we need to calc whether we hit the desired
		// location on the target...
		if (aimLocation != -1)
			hitLocation = aimLocation;
	}
	std::unique_ptr<MechWarrior> targetPilot = nullptr;
	if (target && target->isMover())
	{
		targetPilot = ((std::unique_ptr<Mover>)target)->getPilot();
		targetPilot->updateAttackerStatus(getWatchID(), scenarioTime);
	}
	//-----------------------
	// Weapon must recycle...
	startWeaponRecycle(weaponIndex);
	WeaponBoltPtr weaponFX = nullptr;
	if (hitRoll < attackChance)
	{
		if (numShots != UNLIMITED_SHOTS)
		{
			//-------------------------------------------------------
			// We're taking the shot, so reduce our ammo inventory...
			deductWeaponShot(weaponIndex);
		}
		//------------
		// Attack hit.
		if (MasterComponent::masterList[inventory[weaponIndex].masterID].getForm() == COMPONENT_FORM_WEAPON_MISSILE)
		{
			//---------------------------------------------------------
			// It's a missile weapon. We need to determine how many hit
			// (and missed) the target, and in how many clusters...
			int32_t missileAmount =
				MasterComponent::masterList[inventory[weaponIndex].masterID].getWeaponAmmoAmount();
			int32_t numMissiles = 0;
			if (isStreakMissile)
				numMissiles = missileAmount;
			else
			{
				numMissiles = (int32_t)(missileAmount / 2.0 + 0.5);
				if (numMissiles < 1)
					numMissiles = 1;
				if (numMissiles > missileAmount)
					numMissiles = missileAmount;
			}
			//--------------------------------------------------------
			// a MissileGen Object is ALL missiles--clusters no more:)
			uint8_t effectType = MasterComponent::masterList[inventory[weaponIndex].masterID]
									 .getWeaponSpecialEffect();
			//--------------------------------------------------
			// Need to know which hotspot this comes from.
			// Also need to know which hotspot this is going to.
			if (target)
			{
				if (aimLocation == -1)
					hitLocation = target->calcHitLocation(
						this, weaponIndex, ATTACKSOURCE_WEAPONFIRE, attackType);
			}
			else
				hitLocation = -1;
			Assert(hitLocation != -2, 0, " Mech.FireWeapon: Bad Hit Location ");
			//--------------------------------------
			// For now, always use a bullet effect...
			WeaponShotInfo curShotInfo;
			curShotInfo.init(getWatchID(), inventory[weaponIndex].masterID,
				numMissiles * MasterComponent::masterList[inventory[weaponIndex].masterID].getWeaponDamage(),
				hitLocation, entryAngle);
			Assert(((float)((int32_t)(curShotInfo.damage / 0.25)) * 0.25) == curShotInfo.damage, 0,
				" WeaponHitChunk.build: damage round error ");
			dmgDone = curShotInfo.damage;
			//-------------------------------------------------------------------------
			// If I'm in a multiplayer game and I'm the server, record this
			// weapon fire so it may be broadcast to all clients...
			if (MPlayer && MPlayer->isServer())
			{
				WeaponFireChunk chunk;
				chunk.init();
				if (target)
				{
					if (target->isMover())
						chunk.buildMoverTarget(target, weaponIndex - numOther, true, entryAngle,
							numMissiles, hitLocation);
					else if (target->getObjectClass() == CAMERADRONE)
						chunk.buildCameraDroneTarget(
							target, weaponIndex - numOther, true, entryAngle, numMissiles);
					else
						chunk.buildTerrainTarget(target, weaponIndex - numOther, true, numMissiles);
				}
				else
					chunk.buildLocationTarget(
						*targetpoint, weaponIndex - numOther, true, numMissiles);
				chunk.pack(this);
				WeaponFireChunk chunk2;
				chunk2.init();
				chunk2.data = chunk.data;
				chunk2.unpack(this);
				if (!chunk.equalTo(&chunk2))
					Fatal(0,
						" Mech.fireWeapon: Bad WeaponFireChunk (save "
						"wfchunk.dbg file now) ");
				addWeaponFireChunk(CHUNK_SEND, &chunk);
				LogWeaponFireChunk(&chunk, this, target);
			}
			//-------------------------------------------------------------------
			// This code will mess up if the object is not a BULLET!!!!!!!!!!!
			weaponFX = ObjectManager->createWeaponBolt(effectType);
			if (!weaponFX)
			{
				if (targetpoint)
				{
					//-----------------------------------------
					// Check for Mine hit and MOVE ON!!!
					int32_t cellRow, cellCol;
					land->worldToCell(*targetpoint, cellRow, cellCol);
					if (GameMap->getMine(cellRow, cellCol) == 1)
					{
						ObjectManager->createExplosion(MINE_EXPLOSION_ID, nullptr, *targetpoint,
							MineSplashDamage, MineSplashRange * worldUnitsPerMeter);
						GameMap->setMine(cellRow, cellCol, 2);
					}
				}
			}
			else
			{
				if (target)
					weaponFX->connect(this, target, &curShotInfo, sourceHotSpot, targetHotSpot);
				else
				{
					weaponFX->connect(this, *targetpoint, &curShotInfo, sourceHotSpot);
					// pilot->clearCurTacOrder();
				}
				printFireWeaponDebugInfo(
					target, targetpoint, attackChance, aimLocation, hitRoll, &curShotInfo);
			}
		}
		else
		{
			//----------------------------------------------------
			// Non-missile weapon, so just one weapon hit spawn...
			// For now, always use a laser effect...
			if (target)
			{
				if (aimLocation == -1)
					hitLocation = target->calcHitLocation(
						this, weaponIndex, ATTACKSOURCE_WEAPONFIRE, attackType);
			}
			else
				hitLocation = -1;
			Assert(hitLocation != -2, 1, " Mech.FireWeapon: Bad Hit Location ");
			WeaponShotInfo shotInfo;
			shotInfo.init(getWatchID(), inventory[weaponIndex].masterID,
				MasterComponent::masterList[inventory[weaponIndex].masterID].getWeaponDamage(),
				hitLocation, entryAngle);
			dmgDone = shotInfo.damage;
			//-------------------------------------------------------------------------
			// If I'm in a multiplayer game and I'm the server, record this
			// weapon fire so it may be broadcast to all clients...
			if (MPlayer && MPlayer->isServer())
			{
				WeaponFireChunk chunk;
				chunk.init();
				if (target)
				{
					if (target->isMover())
						chunk.buildMoverTarget(
							target, weaponIndex - numOther, true, entryAngle, 0, hitLocation);
					else
						chunk.buildTerrainTarget(target, weaponIndex - numOther, true, 0);
				}
				else
				{
					chunk.buildLocationTarget(*targetpoint, weaponIndex - numOther, true, 0);
				}
				chunk.pack(this);
				WeaponFireChunk chunk2;
				chunk2.init();
				chunk2.data = chunk.data;
				chunk2.unpack(this);
				if (!chunk.equalTo(&chunk2))
					Fatal(0,
						" Mech.fireWeapon: Bad WeaponFireChunk (save "
						"wfchunk.dbg file now) ");
				addWeaponFireChunk(CHUNK_SEND, &chunk);
				LogWeaponFireChunk(&chunk, this, target);
			}
			uint8_t effectType = MasterComponent::masterList[inventory[weaponIndex].masterID]
									 .getWeaponSpecialEffect();
			weaponFX = ObjectManager->createWeaponBolt(effectType);
			if (!weaponFX)
			{
				if (targetpoint)
				{
					//-----------------------------------------
					// Check for Mine hit and MOVE ON!!!
					int32_t cellRow, cellCol;
					land->worldToCell(*targetpoint, cellRow, cellCol);
					if (GameMap->getMine(cellRow, cellCol) == 1)
					{
						ObjectManager->createExplosion(MINE_EXPLOSION_ID, nullptr, *targetpoint,
							MineSplashDamage, MineSplashRange * worldUnitsPerMeter);
						GameMap->setMine(cellRow, cellCol, 2);
					}
				}
			}
			else
			{
				if (target)
				{
					weaponFX->connect(this, target, &shotInfo, sourceHotSpot, targetHotSpot);
				}
				else
				{
					//--------------------------------
					// Hit the target point/terrain...
					weaponFX->connect(this, *targetpoint, &shotInfo, sourceHotSpot);
				}
				printFireWeaponDebugInfo(
					target, targetpoint, attackChance, aimLocation, hitRoll, &shotInfo);
			}
			if (!target)
			{
				//-----------------------------
				// Now, cancel the tac order...
				// pilot->clearCurTacOrder();
			}
		}
	}
	else
	{
		// if ((aimLocation != -1) && (RandomNumber(100) < AimedFireAbort))
		//	return(NO_ERROR);
		if (!isStreakMissile)
		{
			if (numShots != UNLIMITED_SHOTS)
			{
				//-------------------------------------------------------
				// We're taking the shot, so reduce our ammo inventory...
				deductWeaponShot(weaponIndex);
			}
			//----------------------------------------------------
			// Miss, so check for possible miss resolution here...
			if (MasterComponent::masterList[inventory[weaponIndex].masterID].getForm() == COMPONENT_FORM_WEAPON_MISSILE)
			{
				//---------------------------------------------------------
				// It's a missile weapon. We need to determine how many hit
				// (and missed) the target, and in how many clusters...
				int32_t missileAmount = MasterComponent::masterList[inventory[weaponIndex].masterID]
											.getWeaponAmmoAmount();
				int32_t numMissiles = ((float)missileAmount / 2.0) + 0.5;
				if (numMissiles < 1)
					numMissiles = 1;
				if (numMissiles > missileAmount)
					numMissiles = missileAmount;
				if (numMissiles > 0)
				{
					//-----------------------------------------------
					// No more clusters!
					WeaponShotInfo curShotInfo;
					curShotInfo.init(getWatchID(), inventory[weaponIndex].masterID,
						numMissiles * MasterComponent::masterList[inventory[weaponIndex].masterID].getWeaponDamage(),
						-1, entryAngle);
					Assert(((float)((int32_t)(curShotInfo.damage / 0.25)) * 0.25) == curShotInfo.damage,
						0, " WeaponHitChunk.build: damage round error ");
					//-------------------------------------------------------------------------------------------
					// If we missed, pick sights away from the target and check
					// LOS to each one.  If all are invisible, just hit the
					// target with zero points of damage.
					float missRadius = target ? 25.0 : 5.0;
					Stuff::Vector3D positionOffset;
					if (target)
						positionOffset = target->getPosition();
					else if (targetpoint)
						positionOffset = *targetpoint;
					positionOffset.x += missRadius;
					positionOffset.z = land->getTerrainElevation(positionOffset);
					bool canSeeHit = lineOfSight(positionOffset, true);
					if (!canSeeHit)
					{
						positionOffset.x -= (missRadius * 2.0f);
						positionOffset.z = land->getTerrainElevation(positionOffset);
						canSeeHit = lineOfSight(positionOffset, true);
						if (!canSeeHit)
						{
							positionOffset.x += missRadius;
							positionOffset.y += missRadius;
							positionOffset.z = land->getTerrainElevation(positionOffset);
							canSeeHit = lineOfSight(positionOffset, true);
							if (!canSeeHit)
							{
								positionOffset.y -= (missRadius * 2.0f);
								positionOffset.z = land->getTerrainElevation(positionOffset);
								canSeeHit = lineOfSight(positionOffset, true);
								if (!canSeeHit)
								{
									// OK, no miss location is visible.  Hit the
									// target with ZERO damage!!
									curShotInfo.init(getWatchID(), inventory[weaponIndex].masterID,
										0.0f, -1, entryAngle);
								}
							}
						}
					}
					//-------------------------------------------------------------------------
					// If I'm in a multiplayer game and I'm the server, record
					// this weapon fire so it may be broadcast to all clients...
					if (MPlayer && MPlayer->isServer())
					{
						WeaponFireChunk chunk;
						chunk.init();
						chunk.buildLocationTarget(
							positionOffset, weaponIndex - numOther, false, numMissiles);
						chunk.pack(this);
						WeaponFireChunk chunk2;
						chunk2.init();
						chunk2.data = chunk.data;
						chunk2.unpack(this);
						if (!chunk.equalTo(&chunk2))
							Fatal(0,
								" Mech.fireWeapon: Bad WeaponFireChunk "
								"(save wfchunk.dbg file now) ");
						addWeaponFireChunk(CHUNK_SEND, &chunk);
						LogWeaponFireChunk(&chunk, this, target);
					}
					//-------------------------------------------------------------------
					uint8_t effectType =
						MasterComponent::masterList[inventory[weaponIndex].masterID]
							.getWeaponSpecialEffect();
					weaponFX = ObjectManager->createWeaponBolt(effectType);
					if (!weaponFX)
					{
						//-----------------------------------------
						// Check for Mine hit and MOVE ON!!!
						int32_t cellRow, cellCol;
						land->worldToCell(positionOffset, cellRow, cellCol);
						if (GameMap->getMine(cellRow, cellCol) == 1)
						{
							ObjectManager->createExplosion(MINE_EXPLOSION_ID, nullptr,
								positionOffset, MineSplashDamage,
								MineSplashRange * worldUnitsPerMeter);
							GameMap->setMine(cellRow, cellCol, 2);
						}
					}
					else
					{
						if (canSeeHit) // miss location is in LOS.  Hit the
							// ground
							weaponFX->connect(this, positionOffset, &curShotInfo, sourceHotSpot);
						else if (target) // Miss location is NOT in LOS.  Hit
							// Target with ZERO damage!!!
							weaponFX->connect(this, target, &curShotInfo, sourceHotSpot);
						// OTHERWISE, we tried to hit the ground but we can't
						// see the location we shot at.
						// DRAW NO WEAPON EFFECT!!!!
						printFireWeaponDebugInfo(target, &positionOffset, attackChance, aimLocation,
							hitRoll, &curShotInfo);
					}
				}
			}
			else
			{
				//----------------------------------------------------
				// Non-missile weapon, so just one weapon hit spawn...
				// For now, always use a laser effect...
				WeaponShotInfo shotInfo;
				shotInfo.init(getWatchID(), inventory[weaponIndex].masterID,
					MasterComponent::masterList[inventory[weaponIndex].masterID].getWeaponDamage(),
					-1, entryAngle);
				//-------------------------------------------------------------------------------------------
				// If we missed, pick sights away from the target and check LOS
				// to each one.  If all are invisible, just hit the target with
				// zero points of damage.
				float missRadius = target ? 25.0 : 5.0;
				Stuff::Vector3D positionOffset;
				if (target)
					positionOffset = target->getPosition();
				else if (targetpoint)
					positionOffset = *targetpoint;
				positionOffset.x += missRadius;
				positionOffset.z = land->getTerrainElevation(positionOffset);
				bool canSeeHit = lineOfSight(positionOffset, true);
				if (!canSeeHit)
				{
					positionOffset.x -= (missRadius * 2.0f);
					positionOffset.z = land->getTerrainElevation(positionOffset);
					canSeeHit = lineOfSight(positionOffset, true);
					if (!canSeeHit)
					{
						positionOffset.x += missRadius;
						positionOffset.y += missRadius;
						positionOffset.z = land->getTerrainElevation(positionOffset);
						canSeeHit = lineOfSight(positionOffset, true);
						if (!canSeeHit)
						{
							positionOffset.y -= (missRadius * 2.0f);
							positionOffset.z = land->getTerrainElevation(positionOffset);
							canSeeHit = lineOfSight(positionOffset, true);
							if (!canSeeHit)
							{
								// OK, no miss location is visible.  Hit the
								// target with ZERO damage!!
								shotInfo.init(getWatchID(), inventory[weaponIndex].masterID, 0.0f,
									-1, entryAngle);
							}
						}
					}
				}
				//-------------------------------------------------------------------------
				// If I'm in a multiplayer game and I'm the server, record this
				// weapon fire so it may be broadcast to all clients...
				if (MPlayer && MPlayer->isServer())
				{
					WeaponFireChunk chunk;
					chunk.init();
					chunk.buildLocationTarget(positionOffset, weaponIndex - numOther, false, 0);
					chunk.pack(this);
					WeaponFireChunk chunk2;
					chunk2.init();
					chunk2.data = chunk.data;
					chunk2.unpack(this);
					if (!chunk.equalTo(&chunk2))
						Fatal(0,
							" Mech.fireWeapon: Bad WeaponFireChunk (save "
							"wfchunk.dbg file now) ");
					addWeaponFireChunk(CHUNK_SEND, &chunk);
					LogWeaponFireChunk(&chunk, this, target);
				}
				uint8_t effectType = MasterComponent::masterList[inventory[weaponIndex].masterID]
										 .getWeaponSpecialEffect();
				weaponFX = ObjectManager->createWeaponBolt(effectType);
				if (!weaponFX)
				{
					//-----------------------------------------
					// Check for Mine hit and MOVE ON!!!
					int32_t cellRow, cellCol;
					land->worldToCell(positionOffset, cellRow, cellCol);
					if (GameMap->getMine(cellRow, cellCol) == 1)
					{
						ObjectManager->createExplosion(MINE_EXPLOSION_ID, nullptr, positionOffset,
							MineSplashDamage, MineSplashRange * worldUnitsPerMeter);
						GameMap->setMine(cellRow, cellCol, 2);
					}
				}
				else
				{
					if (canSeeHit) // miss location is in LOS.  Hit the ground
						weaponFX->connect(this, positionOffset, &shotInfo, sourceHotSpot);
					else if (target) // Miss location is NOT in LOS.  Hit Target
						// with ZERO damage!!!
						weaponFX->connect(this, target, &shotInfo, sourceHotSpot);
					printFireWeaponDebugInfo(
						target, &positionOffset, attackChance, aimLocation, hitRoll, &shotInfo);
				}
			}
		}
	}
	//-------------------------------------------------------------------
	// Trigger the WEAPON TARGET event. For now, this assumes the target
	// KNOWS we were targeting him. Of course, the target wouldn't always
	// be aware of this, would they?
	pilot->triggerAlarm(PILOT_ALARM_FIRED_WEAPON, target ? target->getWatchID() : 0);
	if (targetPilot)
		targetPilot->triggerAlarm(PILOT_ALARM_TARGET_OF_WEAPONFIRE, getWatchID());
	timeSinceFiredLast = 0.0f;
	if (getGroup())
		getGroup()->handleMateFiredWeapon(getWatchID());
	return (NO_ERROR);
}

#define MAX_WEAPON_FX 200
//---------------------------------------------------------------------------

int32_t
BattleMech::handleWeaponFire(int32_t weaponIndex, GameObjectPtr target,
	Stuff::Vector3D* targetpoint, bool hit, float entryAngle, int32_t numMissiles,
	int32_t hitLocation)
{
	//--------------------------------------------------------
	// If it's already been fired, assume the ammo is there...
	int32_t numShots = getWeaponShots(weaponIndex);
	// Assert(numShots > 0, numShots, " handleWeaponFire: numShots is negative!
	// ");
	//-----------------------
	// Weapon must recycle...
	startWeaponRecycle(weaponIndex);
	bool isStreakMissile =
		MasterComponent::masterList[inventory[weaponIndex].masterID].getWeaponStreak();
	WeaponBoltPtr weaponFX = nullptr;
	//----------------------------------------------------
	// Need to know which hotspot this comes from.
	// Also need to know which hotspot this is going to.
	int32_t sourceHotSpot;
	if (MasterComponent::masterList[inventory[weaponIndex].masterID].getForm() == COMPONENT_FORM_WEAPON_MISSILE)
		sourceHotSpot = MECH3D_WEAPONTYPE_MISSILE;
	else if (MasterComponent::masterList[inventory[weaponIndex].masterID].getForm() == COMPONENT_FORM_WEAPON_BALLISTIC)
		sourceHotSpot = MECH3D_WEAPONTYPE_BALLISTIC;
	else
		sourceHotSpot = MECH3D_WEAPONTYPE_ENERGY;
	sourceHotSpot = appearance->getWeaponNode(sourceHotSpot);
	// No Ripple fire in MPLayer.  Must just fire the weapon when we get the
	// packet.
	appearance->setWeaponNodeUsed(sourceHotSpot);
	// Set the hotspot for the target
	int32_t targetHotSpot = 0;
	if (target && target->getAppearance())
	{
		targetHotSpot = target->getAppearance()->getWeaponNode(MECH3D_WEAPONTYPE_ANY);
	}
	if (hit)
	{
		//------------
		// Attack hit.
		if (numShots != UNLIMITED_SHOTS)
		{
			//-------------------------------------------------------
			// We're taking the shot, so reduce our ammo inventory...
			deductWeaponShot(weaponIndex);
		}
		if (MasterComponent::masterList[inventory[weaponIndex].masterID].getForm() == COMPONENT_FORM_WEAPON_MISSILE)
		{
			//---------------------------------------------------------
			// It's a missile weapon. We need to determine how many hit
			// (and missed) the target, and in how many clusters...
			//------------------
			// Clusters no more!
			uint8_t effectType = MasterComponent::masterList[inventory[weaponIndex].masterID]
									 .getWeaponSpecialEffect();
			if (numMissiles > 0)
			{
				//-------------------------------------------------------------------
				// This code will mess up if the object is not a
				// BULLET!!!!!!!!!!!
				weaponFX = ObjectManager->createWeaponBolt(effectType);
				if (!weaponFX)
				{
					if (targetpoint)
					{
						//-----------------------------------------
						// Check for Mine hit and MOVE ON!!!
						int32_t cellRow, cellCol;
						land->worldToCell(*targetpoint, cellRow, cellCol);
						if (GameMap->getMine(cellRow, cellCol) == 1)
						{
							ObjectManager->createExplosion(MINE_EXPLOSION_ID, nullptr, *targetpoint,
								MineSplashDamage, MineSplashRange * worldUnitsPerMeter);
							GameMap->setMine(cellRow, cellCol, 2);
						}
					}
				}
				else
				{
					Assert(
						hitLocation != -2, TargetRolo, " Mech.handleWeaponFire: Bad Hit Location ");
					//--------------------------------------
					// For now, always use a bullet effect...
					WeaponShotInfo curShotInfo;
					curShotInfo.init(getWatchID(), inventory[weaponIndex].masterID,
						numMissiles * MasterComponent::masterList[inventory[weaponIndex].masterID].getWeaponDamage(),
						hitLocation, entryAngle);
					Assert(((float)((int32_t)(curShotInfo.damage / 0.25)) * 0.25) == curShotInfo.damage,
						0, " WeaponHitChunk.build: damage round error ");
					if (target)
						weaponFX->connect(this, target, &curShotInfo, sourceHotSpot, targetHotSpot);
					else
					{
						weaponFX->connect(this, *targetpoint, &curShotInfo, sourceHotSpot);
						//-----------------------------
						// Now, cancel the tac order...
						// pilot->clearCurTacOrder();
					}
				}
			}
		}
		else
		{
			//----------------------------------------------------
			// Non-missile weapon, so just one weapon hit spawn...
			// For now, always use a laser effect...
			WeaponShotInfo shotInfo;
			shotInfo.init(getWatchID(), inventory[weaponIndex].masterID,
				MasterComponent::masterList[inventory[weaponIndex].masterID].getWeaponDamage(),
				hitLocation, entryAngle);
			uint8_t effectType = MasterComponent::masterList[inventory[weaponIndex].masterID]
									 .getWeaponSpecialEffect();
			weaponFX = ObjectManager->createWeaponBolt(effectType);
			if (!weaponFX)
			{
				if (targetpoint)
				{
					//-----------------------------------------
					// Check for Mine hit and MOVE ON!!!
					int32_t cellRow, cellCol;
					land->worldToCell(*targetpoint, cellRow, cellCol);
					if (GameMap->getMine(cellRow, cellCol) == 1)
					{
						ObjectManager->createExplosion(MINE_EXPLOSION_ID, nullptr, *targetpoint,
							MineSplashDamage, MineSplashRange * worldUnitsPerMeter);
						GameMap->setMine(cellRow, cellCol, 2);
					}
				}
			}
			else
			{
				if (target)
					weaponFX->connect(this, target, &shotInfo, sourceHotSpot, targetHotSpot);
				else
				{
					weaponFX->connect(this, *targetpoint, &shotInfo, sourceHotSpot);
					//-----------------------------
					// Now, cancel the tac order...
					// pilot->clearCurTacOrder();
				}
			}
		}
	}
	else
	{
		Assert(target == nullptr, 0,
			" Mech.handleWeaponFire: target should be nullptr with network "
			"miss! ");
		Assert(targetpoint != nullptr, 0,
			" Mech.handleWeaponFire: MUST have targetpoint with network "
			"miss! ");
		if (isStreakMissile)
		{
			CurMoverWeaponFireChunk.unpack(this);
			DebugWeaponFireChunk(&CurMoverWeaponFireChunk, nullptr, this);
			Assert(false, 0,
				" Mech.handleWeaponFire: streaks shouldn't miss! (save "
				"wfchunk.dbg file) ");
		}
		if (numShots != UNLIMITED_SHOTS)
		{
			//-------------------------------------------------------
			// We're taking the shot, so reduce our ammo inventory...
			deductWeaponShot(weaponIndex);
		}
		//----------------------------------------------------
		// Miss, so check for possible miss resolution here...
		if (MasterComponent::masterList[inventory[weaponIndex].masterID].getForm() == COMPONENT_FORM_WEAPON_MISSILE)
		{
			if (numMissiles)
			{
				//------------------
				// Clusters no more!
				uint8_t effectType = MasterComponent::masterList[inventory[weaponIndex].masterID]
										 .getWeaponSpecialEffect();
				weaponFX = ObjectManager->createWeaponBolt(effectType);
				if (!weaponFX)
				{
					if (targetpoint)
					{
						//-----------------------------------------
						// Check for Mine hit and MOVE ON!!!
						int32_t cellRow, cellCol;
						land->worldToCell(*targetpoint, cellRow, cellCol);
						if (GameMap->getMine(cellRow, cellCol) == 1)
						{
							ObjectManager->createExplosion(MINE_EXPLOSION_ID, nullptr, *targetpoint,
								MineSplashDamage, MineSplashRange * worldUnitsPerMeter);
							GameMap->setMine(cellRow, cellCol, 2);
						}
					}
				}
				else
				{
					WeaponShotInfo curShotInfo;
					curShotInfo.init(getWatchID(), inventory[weaponIndex].masterID,
						numMissiles * MasterComponent::masterList[inventory[weaponIndex].masterID].getWeaponDamage(),
						-1, entryAngle);
					Assert(((float)((int32_t)(curShotInfo.damage / 0.25)) * 0.25) == curShotInfo.damage,
						0, " WeaponHitChunk.build: damage round error ");
					weaponFX->connect(this, *targetpoint, &curShotInfo, sourceHotSpot);
				}
			}
		}
		else
		{
			//----------------------------------------------------
			// Non-missile weapon, so just one weapon hit spawn...
			// For now, always use a laser effect...
			WeaponShotInfo shotInfo;
			shotInfo.init(getWatchID(), inventory[weaponIndex].masterID,
				MasterComponent::masterList[inventory[weaponIndex].masterID].getWeaponDamage(), -1,
				entryAngle);
			uint8_t effectType = MasterComponent::masterList[inventory[weaponIndex].masterID]
									 .getWeaponSpecialEffect();
			weaponFX = ObjectManager->createWeaponBolt(effectType);
			if (!weaponFX)
			{
				if (targetpoint)
				{
					//-----------------------------------------
					// Check for Mine hit and MOVE ON!!!
					int32_t cellRow, cellCol;
					land->worldToCell(*targetpoint, cellRow, cellCol);
					if (GameMap->getMine(cellRow, cellCol) == 1)
					{
						ObjectManager->createExplosion(MINE_EXPLOSION_ID, nullptr, *targetpoint,
							MineSplashDamage, MineSplashRange * worldUnitsPerMeter);
						GameMap->setMine(cellRow, cellCol, 2);
					}
				}
			}
			else
			{
				weaponFX->connect(this, *targetpoint, &shotInfo, sourceHotSpot);
			}
		}
	}
	//-------------------------------------------------------------------
	// Trigger the WEAPON TARGET event. For now, this assumes the target
	// KNOWS we were targeting him. Of course, the target wouldn't always
	// be aware of this, would they?
	std::unique_ptr<MechWarrior> targetPilot = nullptr;
	if (target && target->isMover())
	{
		targetPilot = ((std::unique_ptr<Mover>)target)->getPilot();
		targetPilot->updateAttackerStatus(getWatchID(), scenarioTime);
		targetPilot->triggerAlarm(PILOT_ALARM_TARGET_OF_WEAPONFIRE, getWatchID());
	}
	if (getGroup())
		getGroup()->handleMateFiredWeapon(getWatchID());
	return (NO_ERROR);
}

//---------------------------------------------------------------------------

float BattleMech::calcMaxSpeed(void)
{
	if (canRun())
		return (appearance->getVelocityOfGesture(GestureRun));
	else if (canWalk())
		return (appearance->getVelocityOfGesture(GestureWalk));
	else if (canLimp())
		return (appearance->getVelocityOfGesture(GestureLimpLeft));
	return (0.0);
}

//---------------------------------------------------------------------------

float BattleMech::calcSlowSpeed(void)
{
	if (canWalk())
		return (maxMoveSpeed * 0.25);
	else if (canLimp())
		return (maxMoveSpeed * 0.2);
	return (0.0);
}

//---------------------------------------------------------------------------

float BattleMech::calcModerateSpeed(void)
{
	if (canWalk())
		return (maxMoveSpeed * 0.4);
	else if (canLimp())
		return (maxMoveSpeed * 0.3);
	return (0.0);
}

//---------------------------------------------------------------------------

int32_t
BattleMech::calcSpriteSpeed(float speed, uint32_t flags, int32_t& state, int32_t& throttle)
{
	state = MECH_STATE_RUNNING;
	throttle = 100;
	//------------------------------------------------------------------------
	// If the speed we want falls between the max walk speed and the run speed
	// of the mech, we use the run speed (we'd rather have them stop and wait
	// for others to catch up than always fall behind). If the speed is
	// greater than the mech's fastest speed, we select the run state, but
	// also return an error code...
	int32_t errCode = 0;
	float maxWalkSpeed = appearance->getVelocityOfGesture(GestureWalk);
	float minWalkSpeed = maxWalkSpeed / 2.0;
	float runSpeed = appearance->getVelocityOfGesture(GestureRun);
	if (speed == 0.0)
		state = MECH_STATE_STANDING;
	else if (speed < minWalkSpeed)
	{
		state = MECH_STATE_WALKING;
		throttle = 50;
		errCode = 1;
	}
	else if (speed <= maxWalkSpeed)
	{
		state = MECH_STATE_WALKING;
		throttle = (int32_t)(speed / maxWalkSpeed * 100.0);
	}
	else if (speed < runSpeed)
	{
		if (flags & MECH_SPRITESPEED_FLAG_GO_SLOW)
		{
			state = MECH_STATE_WALKING;
			throttle = (int32_t)(speed / maxWalkSpeed * 100.0);
		}
		else
			state = MECH_STATE_RUNNING;
		errCode = 2;
	}
	else if (speed > runSpeed)
	{
		state = MECH_STATE_RUNNING;
		errCode = 3;
	}
	return (errCode);
}

//---------------------------------------------------------------------------

bool BattleMech::isCaptureable(int32_t capturingTeamID)
{
	// The following seems wrong, but we've opted to leave it so we don't upset
	// the BETA gods. This function should never get called anyway, we
	// think...hehe...3/20/01
	return (getFlag(OBJECT_FLAG_CAPTURABLE) && (getTeam() == Team::home) && !isDestroyed());
}

//---------------------------------------------------------------------------
float BattleMech::calcMaxTargetDamage(void)
{
	float damage = 0;
	for (size_t curWeapon = numOther; curWeapon < numWeapons + numOther; curWeapon++)
	{
		int32_t numClusters, clusterSize;
		float thisDamage;
		numClusters = clusterSize = 1;
		if (MasterComponent::masterList[inventory[curWeapon].masterID].getForm() == COMPONENT_FORM_WEAPON_MISSILE)
		{
			//---------------------------------------------------------
			// It's a missile weapon. We need to determine how many hit
			// (and missed) the target, and in how many clusters...
			int32_t numMissiles =
				MasterComponent::masterList[inventory[curWeapon].masterID].getWeaponAmmoAmount() / 2;
			switch (MasterComponent::masterList[inventory[curWeapon].masterID].getWeaponAmmoType())
			{
			case WEAPON_AMMO_SRM:
				numClusters = numMissiles / ClusterSizeSRM;
				if (numMissiles % ClusterSizeSRM)
					numClusters++;
				clusterSize = ClusterSizeSRM;
				break;
			case WEAPON_AMMO_LRM:
				numClusters = numMissiles / ClusterSizeLRM;
				if (numMissiles % ClusterSizeLRM)
					numClusters++;
				clusterSize = ClusterSizeLRM;
				break;
			};
		}
		thisDamage = clusterSize * numClusters * MasterComponent::masterList[inventory[curWeapon].masterID].getWeaponDamage() * 10;
		thisDamage /=
			MasterComponent::masterList[inventory[curWeapon].masterID].getWeaponRecycleTime();
		thisDamage *= 100; // 100% chance to hit
		if (thisDamage > 0)
			damage += thisDamage;
	}
	maxWeaponDamage = damage;
	return damage;
}

//---------------------------------------------------------------------------

bool BattleMech::isWeaponWorking(int32_t weaponIndex)
{
	// int32_t bodyLocation = inventory[weaponIndex].bodyLocation;
	//	if ((bodyLocation == MECH_BODY_LOCATION_LARM) &&
	// inventory[actuator[ACTUATOR_LSHOULDER]].disabled) 		return(false);
	//	else if ((bodyLocation == MECH_BODY_LOCATION_RARM) &&
	// inventory[actuator[ACTUATOR_RSHOULDER]].disabled) 		return(false);
	//	else
	if (inventory[weaponIndex].disabled)
		return (false);
	else if (getWeaponShots(weaponIndex) == 0)
		return (false);
	return (true);
}

//---------------------------------------------------------------------------
float BattleMech::getTotalEffectiveness(void)
{
	float weaponEffect, result;
	float curCTorso, maxCTorso, armorCTorso;
	float maxHead, curHead, armorHead, armorEffect, curArm, maxArm, armorArm, curLeg, maxLeg,
		armorLeg;
	float curTorso, maxTorso, armorTorso, pilotEffect;
	//-------------------------------------------------------------------------
	// calculate Weapon effectiveness
	if (0.0 == maxWeaponEffectiveness)
	{
		gosASSERT(false);
		maxWeaponEffectiveness = 1.0;
	}
	if (weaponEffectiveness > maxWeaponEffectiveness)
		maxWeaponEffectiveness = weaponEffectiveness;
	weaponEffect = weaponEffectiveness / maxWeaponEffectiveness;
	//---------------------------------------------------------------
	// Calculate armor effectiveness.
	// DO NOT CHANGE!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	// COmpiler does funky things if we just return 0.0!
	if (isDestroyed() || isDisabled())
	{
		armorEffect = 0.0;
	}
	else
	{
		maxHead = armor[MECH_ARMOR_LOCATION_HEAD].maxArmor;
		curHead = armor[MECH_ARMOR_LOCATION_HEAD].curArmor;
		armorHead = curHead / maxHead * 0.6 + (0.4);
		curCTorso = armor[MECH_ARMOR_LOCATION_CTORSO].curArmor;
		maxCTorso = armor[MECH_ARMOR_LOCATION_CTORSO].maxArmor;
		if (curCTorso > armor[MECH_ARMOR_LOCATION_RCTORSO].curArmor)
		{
			curCTorso = armor[MECH_ARMOR_LOCATION_RCTORSO].curArmor;
			maxCTorso = armor[MECH_ARMOR_LOCATION_RCTORSO].maxArmor;
		}
		armorCTorso = curCTorso / maxCTorso * 0.5 + (0.5);
		curTorso = armor[MECH_ARMOR_LOCATION_RLTORSO].curArmor + armor[MECH_ARMOR_LOCATION_LTORSO].curArmor + armor[MECH_ARMOR_LOCATION_RRTORSO].curArmor + armor[MECH_ARMOR_LOCATION_RTORSO].curArmor;
		maxTorso = armor[MECH_ARMOR_LOCATION_RLTORSO].maxArmor + armor[MECH_ARMOR_LOCATION_LTORSO].maxArmor + armor[MECH_ARMOR_LOCATION_RRTORSO].maxArmor + armor[MECH_ARMOR_LOCATION_RTORSO].maxArmor;
		armorTorso = curTorso / maxTorso * 0.25 + (0.75);
		curArm =
			armor[MECH_ARMOR_LOCATION_LARM].curArmor + armor[MECH_ARMOR_LOCATION_RARM].curArmor;
		maxArm =
			armor[MECH_ARMOR_LOCATION_LARM].maxArmor + armor[MECH_ARMOR_LOCATION_RARM].maxArmor;
		armorArm = curArm / maxArm * 0.25 + (0.75);
		curLeg =
			armor[MECH_ARMOR_LOCATION_LLEG].curArmor + armor[MECH_ARMOR_LOCATION_RLEG].curArmor;
		maxLeg =
			armor[MECH_ARMOR_LOCATION_LLEG].maxArmor + armor[MECH_ARMOR_LOCATION_RLEG].maxArmor;
		armorLeg = curArm / maxArm * 0.4 + (0.6);
		armorEffect = armorHead * armorCTorso * armorTorso * armorArm * armorLeg;
	}
	float pilotWoundTable[7] = {1.0, 0.95f, 0.85f, 0.75f, 0.50f, 0.30f, 0.00};
	uint32_t pilotWounds = float2long(getPilot()->getWounds());
	if (pilotWounds > 6)
		pilotWounds = 6;
	pilotEffect = pilotWoundTable[pilotWounds];
	result = weaponEffect * armorEffect * pilotEffect;
	return result;
}

//---------------------------------------------------------------------------
void BattleMech::damageLoadedComponents(void)
{
	for (size_t curLocation = 0; curLocation < NUM_MECH_BODY_LOCATIONS; curLocation++)
	{
		int32_t numSpaces = NumLocationCriticalSpaces[curLocation];
		for (size_t curSpace = 0; curSpace < numSpaces; curSpace++)
		{
			if (body[curLocation].criticalSpaces[curSpace].hit)
				hitInventoryItem(body[curLocation].criticalSpaces[curSpace].inventoryID, true);
		}
	}
}

//***************************************************************************
void BattleMech::Save(PacketFilePtr file, int32_t packetNum)
{
	MechData data;
	CopyTo(&data);
	// PacketNum incremented in ObjectManager!!
	file->writePacket(packetNum, (uint8_t*)&data, sizeof(MechData), STORAGE_TYPE_ZLIB);
}

//***************************************************************************
void BattleMech::CopyTo(MechData* data)
{
	data->chassisClass = chassisClass;
	data->chassisBR = chassisBR;
	data->variantID = variantID;
	memcpy(data->variantName, variantName, sizeof(wchar_t) * 64);
	data->legStatus = legStatus;
	data->torsoStatus = torsoStatus;
	memcpy(data->actuator, actuator, sizeof(uint8_t) * NUM_ACTUATORS);
	data->gyro = gyro;
	if (sensorSystem)
		data->sensorOK = !sensorSystem->broken;
	else
		data->sensorOK = false;
	data->numJumpJets = numJumpJets;
	data->lastJumpTime = lastJumpTime;
	data->inJump = inJump;
	data->jumpGoal = jumpGoal;
	data->centerTorsoBlowTime = centerTorsoBlowTime;
	data->hitFromBehindThisFrame = hitFromBehindThisFrame;
	data->hitFromFrontThisFrame = hitFromFrontThisFrame;
	data->leftArmBlownThisFrame = leftArmBlownThisFrame;
	data->rightArmBlownThisFrame = rightArmBlownThisFrame;
	data->torsoRotation = torsoRotation;
	data->rightArmRotation = rightArmRotation;
	data->leftArmRotation = leftArmRotation;
	data->fallen = fallen;
	data->mechRemoved = mechRemoved;
	data->pivotTurnRate = pivotTurnRate;
	data->playedJumpSFX = playedJumpSFX;
	data->playedCriticalHit = playedCriticalHit;
	data->maxWeaponDamage = maxWeaponDamage;
	memcpy(data->longName, longName, sizeof(wchar_t) * MAXLEN_MECH_LONGNAME);
	data->pilotNum = pilotNum;
	data->captureable = captureable;
	data->notMineYet = notMineYet;
	data->descID = descID;
	data->damageThisFrame = damageThisFrame;
	data->sentCrippledMsg = sentCrippledMsg;
	memcpy(data->rotateValues, rotateValues, sizeof(float) * 6);
	memcpy(data->ItemLocationToInvLocation, ItemLocationToInvLocation,
		sizeof(int32_t) * MAX_MOVER_INVENTORY_ITEMS);
	data->damageAfterDisabled = damageAfterDisabled;
	data->numArmorComponents = numArmorComponents;
	data->cBills = cBills;
	if (appearance)
		static_cast<Mech3DAppearance*>(appearance)->copyTo(&(data->apData));
	Mover::CopyTo(dynamic_cast<MoverData*>(data));
}

//---------------------------------------------------------------------------
void BattleMech::Load(MechData* data)
{
	Mover::Load(dynamic_cast<MoverData*>(data));
	chassisClass = data->chassisClass;
	chassisBR = data->chassisBR;
	variantID = data->variantID;
	memcpy(variantName, data->variantName, sizeof(wchar_t) * 64);
	legStatus = data->legStatus;
	torsoStatus = data->torsoStatus;
	memcpy(actuator, data->actuator, sizeof(uint8_t) * NUM_ACTUATORS);
	gyro = data->gyro;
	numJumpJets = data->numJumpJets;
	lastJumpTime = data->lastJumpTime;
	inJump = data->inJump;
	jumpGoal = data->jumpGoal;
	centerTorsoBlowTime = data->centerTorsoBlowTime;
	hitFromBehindThisFrame = data->hitFromBehindThisFrame;
	hitFromFrontThisFrame = data->hitFromFrontThisFrame;
	leftArmBlownThisFrame = data->leftArmBlownThisFrame;
	rightArmBlownThisFrame = data->rightArmBlownThisFrame;
	torsoRotation = data->torsoRotation;
	rightArmRotation = data->rightArmRotation;
	leftArmRotation = data->leftArmRotation;
	fallen = data->fallen;
	mechRemoved = data->mechRemoved;
	pivotTurnRate = data->pivotTurnRate;
	playedJumpSFX = data->playedJumpSFX;
	playedCriticalHit = data->playedCriticalHit;
	maxWeaponDamage = data->maxWeaponDamage;
	memcpy(longName, data->longName, sizeof(wchar_t) * MAXLEN_MECH_LONGNAME);
	pilotNum = data->pilotNum;
	captureable = data->captureable;
	notMineYet = data->notMineYet;
	descID = data->descID;
	damageThisFrame = data->damageThisFrame;
	sentCrippledMsg = data->sentCrippledMsg;
	memcpy(rotateValues, data->rotateValues, sizeof(float) * 6);
	memcpy(ItemLocationToInvLocation, data->ItemLocationToInvLocation,
		sizeof(int32_t) * MAX_MOVER_INVENTORY_ITEMS);
	damageAfterDisabled = data->damageAfterDisabled;
	numArmorComponents = data->numArmorComponents;
	cBills = data->cBills;
	if (appearance)
		static_cast<Mech3DAppearance*>(appearance)->copyFrom(&(data->apData));
	sensorSystem = SensorManager->newSensor();
	sensorSystem->setOwner(this);
	sensorSystem->setRange(
		MasterComponent::masterList[inventory[sensor].masterID].getSensorRange());
	SensorManager->addTeamSensor(teamId, sensorSystem);
	if (!data->sensorOK)
		sensorSystem->broken = true;
}

//***************************************************************************
void BattleMech::repairAll(void)
{
	armor[MECH_ARMOR_LOCATION_HEAD].curArmor = armor[MECH_ARMOR_LOCATION_HEAD].maxArmor;
	armor[MECH_ARMOR_LOCATION_CTORSO].curArmor = armor[MECH_ARMOR_LOCATION_CTORSO].maxArmor;
	armor[MECH_ARMOR_LOCATION_LTORSO].curArmor = armor[MECH_ARMOR_LOCATION_LTORSO].maxArmor;
	armor[MECH_ARMOR_LOCATION_RTORSO].curArmor = armor[MECH_ARMOR_LOCATION_RTORSO].maxArmor;
	armor[MECH_ARMOR_LOCATION_LARM].curArmor = armor[MECH_ARMOR_LOCATION_LARM].maxArmor;
	armor[MECH_ARMOR_LOCATION_RARM].curArmor = armor[MECH_ARMOR_LOCATION_RARM].maxArmor;
	armor[MECH_ARMOR_LOCATION_LLEG].curArmor = armor[MECH_ARMOR_LOCATION_LLEG].maxArmor;
	armor[MECH_ARMOR_LOCATION_RLEG].curArmor = armor[MECH_ARMOR_LOCATION_RLEG].maxArmor;
	armor[MECH_ARMOR_LOCATION_RCTORSO].curArmor = armor[MECH_ARMOR_LOCATION_RCTORSO].maxArmor;
	armor[MECH_ARMOR_LOCATION_RLTORSO].curArmor = armor[MECH_ARMOR_LOCATION_RLTORSO].maxArmor;
	armor[MECH_ARMOR_LOCATION_RRTORSO].curArmor = armor[MECH_ARMOR_LOCATION_RRTORSO].maxArmor;
	//------------------------------------------------------------
	// Now, read in the component layout for each body location...
	for (size_t curLocation = 0; curLocation < NUM_MECH_BODY_LOCATIONS; curLocation++)
	{
		body[curLocation].curInternalStructure = body[curLocation].maxInternalStructure;
		body[curLocation].damageState = IS_DAMAGE_NONE;
		int32_t numSpaces = NumLocationCriticalSpaces[curLocation];
		for (size_t curSpace = 0; curSpace < numSpaces; curSpace++)
		{
			body[curLocation].criticalSpaces[curSpace].hit = false;
		}
	}
	for (size_t curItem = 0; curItem < MAX_MOVER_INVENTORY_ITEMS; curItem++)
	{
		inventory[curItem].health = 255;
		inventory[curItem].disabled = false;
	}
	calcAmmoTotals();
	//---------------------------------------------------------------------------
	// We need to set the status states for legs and torso based upon our
	// current condition...
	calcLegStatus();
	calcTorsoStatus();
	calcFireRanges();
	maxCV = calcCV(true);
	curCV = calcCV(false);
	setThreatRating(-1);
	maxWeaponDamage = calcMaxTargetDamage();
	sensorSystem->broken = false;
}

//***************************************************************************
