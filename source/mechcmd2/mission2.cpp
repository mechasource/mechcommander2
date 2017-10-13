#define MISSION2_CPP
/*************************************************************************************************\
mission2.cpp			: The parts of mission.cpp that we need for the mech
viewer
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//
\*************************************************************************************************/
#include "stdafx.h"

#include "mission.h"

bool Mission::statisticsInitialized = 0;

#ifdef LAB_ONLY
int64_t MCTimeABLLoad		 = 0;
int64_t MCTimeMiscToTeamLoad = 0;
int64_t MCTimeTeamLoad		 = 0;
int64_t MCTimeObjectLoad	 = 0;
int64_t MCTimeTerrainLoad	= 0;
int64_t MCTimeMoveLoad		 = 0;
int64_t MCTimeMissionABLLoad = 0;
int64_t MCTimeWarriorLoad	= 0;
int64_t MCTimeMoverPartsLoad = 0;
int64_t MCTimeObjectiveLoad  = 0;
int64_t MCTimeCommanderLoad  = 0;
int64_t MCTimeMiscLoad		 = 0;
int64_t MCTimeGUILoad		 = 0;

int64_t x1;

int64_t MCTimeMultiplayerUpdate = 0;
int64_t MCTimeTerrainUpdate		= 0;
int64_t MCTimeCameraUpdate		= 0;
int64_t MCTimeWeatherUpdate		= 0;
int64_t MCTimePathManagerUpdate = 0;
int64_t MCTimeRunBrainUpdate	= 0;
int64_t MCTimePath1Update		= 0;
int64_t MCTimePath2Update		= 0;
int64_t MCTimePath3Update		= 0;
int64_t MCTimePath4Update		= 0;
int64_t MCTimePath5Update		= 0;

int64_t MCTimeCalcGoal1Update = 0;
extern int64_t MCTimeCalcPath1Update;
extern int64_t MCTimeCalcPath2Update;
extern int64_t MCTimeCalcPath3Update;
extern int64_t MCTimeCalcPath4Update;
extern int64_t MCTimeCalcPath5Update;
int64_t MCTimeCalcGoal2Update = 0;
int64_t MCTimeCalcGoal3Update = 0;
int64_t MCTimeCalcGoal4Update = 0;
int64_t MCTimeCalcGoal5Update = 0;
int64_t MCTimeCalcGoal6Update = 0;

int64_t MCTimeTerrainGeometry  = 0;
int64_t MCTimeCraterUpdate	 = 0;
int64_t MCTimeTXMManagerUpdate = 0;
int64_t MCTimeSensorUpdate	 = 0;
int64_t MCTimeLOSUpdate		   = 0;
int64_t MCTimeCollisionUpdate  = 0;
int64_t MCTimeMissionScript	= 0;
int64_t MCTimeInterfaceUpdate  = 0;
int64_t MCTimeMissionTotal	 = 0;

extern int64_t MCTimeTerrainGeometry;
extern int64_t MCTimeCraterUpdate;
extern int64_t MCTimeTXMManagerUpdate;
extern int64_t MCTimeSensorUpdate;
extern int64_t MCTimeLOSUpdate;
extern int64_t MCTimeCollisionUpdate;
extern int64_t MCTimeMissionScript;
extern int64_t MCTimeInterfaceUpdate;
extern int64_t MCTimeMissionTotal;

extern int64_t MCTimeLOSCalc;
extern int64_t MCTimeTerrainObjectsUpdate;
extern int64_t MCTimeMechsUpdate;
extern int64_t MCTimeVehiclesUpdate;
extern int64_t MCTimeTurretsUpdate;
extern int64_t MCTimeAllElseUpdate;

extern int64_t MCTimeTerrainObjectsTL;
extern int64_t MCTimeMechsTL;
extern int64_t MCTimeVehiclesTL;
extern int64_t MCTimeTurretsTL;

extern int64_t MCTimeAnimationCalc;

extern int64_t x;

extern float OneOverProcessorSpeed;
#endif

void Mission::initBareMinimum()
{
	int32_t result = 0;
	result;
	if (!mcTextureManager)
	{
		mcTextureManager = new MC_TextureManager;
		mcTextureManager->start();
	}
	// Startup the vertex array pool
	mcTextureManager->startVertices(100000);
	initTGLForLogistics();
	//----------------------------------------------
	// Start Appearance Type Lists.
	uint32_t spriteHeapSize = 3072000;
	if (!appearanceTypeList)
	{
		appearanceTypeList = new AppearanceTypeList;
		gosASSERT(appearanceTypeList != nullptr);
		appearanceTypeList->init(spriteHeapSize);
		gosASSERT(result == NO_ERROR);
	}
	if (!weaponEffects)
	{
		weaponEffects = new WeaponEffects;
		weaponEffects->init("Effects");
	}
	if (statisticsInitialized == 0)
	{
		initializeStatistics();
	}
}

void Mission::initializeStatistics()
{
#ifdef LAB_ONLY
	// Add Mission Load statistics to GameOS Debugger screen!
	MCTimeABLLoad *= OneOverProcessorSpeed;
	MCTimeMiscToTeamLoad *= OneOverProcessorSpeed;
	MCTimeTeamLoad *= OneOverProcessorSpeed;
	MCTimeObjectLoad *= OneOverProcessorSpeed;
	MCTimeTerrainLoad *= OneOverProcessorSpeed;
	MCTimeMoveLoad *= OneOverProcessorSpeed;
	MCTimeMissionABLLoad *= OneOverProcessorSpeed;
	MCTimeWarriorLoad *= OneOverProcessorSpeed;
	MCTimeMoverPartsLoad *= OneOverProcessorSpeed;
	MCTimeObjectiveLoad *= OneOverProcessorSpeed;
	MCTimeCommanderLoad *= OneOverProcessorSpeed;
	MCTimeMiscLoad *= OneOverProcessorSpeed;
	MCTimeGUILoad *= OneOverProcessorSpeed;
	// Add Mission Run statistics to GameOS Debugger screen!
	StatisticFormat("");
	StatisticFormat("MechCommander 2 GameLogic");
	StatisticFormat("=========================");
	StatisticFormat("");
	AddStatistic(
		"Terrain Update", "%", gos_timedata, (PVOID)&MCTimeTerrainUpdate, 0);
	AddStatistic(
		"Camera Update", "%", gos_timedata, (PVOID)&MCTimeCameraUpdate, 0);
	AddStatistic(
		"Weather Update", "%", gos_timedata, (PVOID)&MCTimeWeatherUpdate, 0);
	AddStatistic("RunBrain Path Update", "%", gos_timedata,
		(PVOID)&MCTimeRunBrainUpdate, 0);
	AddStatistic("PathManager Update", "%", gos_timedata,
		(PVOID)&MCTimePathManagerUpdate, 0);
	AddStatistic(
		"   Path1 Update", "%", gos_timedata, (PVOID)&MCTimePath1Update, 0);
	AddStatistic(
		"   Path2 Update", "%", gos_timedata, (PVOID)&MCTimePath2Update, 0);
	AddStatistic(
		"   Path3 Update", "%", gos_timedata, (PVOID)&MCTimePath3Update, 0);
	AddStatistic(
		"   Path4 Update", "%", gos_timedata, (PVOID)&MCTimePath4Update, 0);
	AddStatistic(
		"   Path5 Update", "%", gos_timedata, (PVOID)&MCTimePath5Update, 0);
	AddStatistic("   CalcPath1 Update", "%", gos_timedata,
		(PVOID)&MCTimeCalcPath1Update, 0);
	AddStatistic("   CalcPath2 Update", "%", gos_timedata,
		(PVOID)&MCTimeCalcPath2Update, 0);
	AddStatistic("   CalcPath3 Update", "%", gos_timedata,
		(PVOID)&MCTimeCalcPath3Update, 0);
	AddStatistic("   CalcPath4 Update", "%", gos_timedata,
		(PVOID)&MCTimeCalcPath4Update, 0);
	AddStatistic("   CalcPath5 Update", "%", gos_timedata,
		(PVOID)&MCTimeCalcPath5Update, 0);
	AddStatistic("   CalcGoal1 Update", "%", gos_timedata,
		(PVOID)&MCTimeCalcGoal1Update, 0);
	AddStatistic("   CalcGoal2 Update", "%", gos_timedata,
		(PVOID)&MCTimeCalcGoal2Update, 0);
	AddStatistic("   CalcGoal3 Update", "%", gos_timedata,
		(PVOID)&MCTimeCalcGoal3Update, 0);
	AddStatistic("   CalcGoal4 Update", "%", gos_timedata,
		(PVOID)&MCTimeCalcGoal4Update, 0);
	AddStatistic("   CalcGoal5 Update", "%", gos_timedata,
		(PVOID)&MCTimeCalcGoal5Update, 0);
	AddStatistic("   CalcGoal6 Update", "%", gos_timedata,
		(PVOID)&MCTimeCalcGoal6Update, 0);
	AddStatistic("Terrain Geometry", "%", gos_timedata,
		(PVOID)&MCTimeTerrainGeometry, 0);
	AddStatistic("Interface Update", "%", gos_timedata,
		(PVOID)&MCTimeInterfaceUpdate, 0);
	AddStatistic(
		"Crater Update", "%", gos_timedata, (PVOID)&MCTimeCraterUpdate, 0);
	AddStatistic(
		"TXM Mgr Update", "%", gos_timedata, (PVOID)&MCTimeTXMManagerUpdate, 0);
	AddStatistic(
		"Sensor Update", "%", gos_timedata, (PVOID)&MCTimeSensorUpdate, 0);
	AddStatistic("LOS Update", "%", gos_timedata, (PVOID)&MCTimeLOSUpdate, 0);
	AddStatistic("Collision Update", "%", gos_timedata,
		(PVOID)&MCTimeCollisionUpdate, 0);
	AddStatistic(
		"Mission Script", "%", gos_timedata, (PVOID)&MCTimeMissionScript, 0);
	AddStatistic("Multiplayer Update", "%", gos_timedata,
		(PVOID)&MCTimeMultiplayerUpdate, 0);
	StatisticFormat("=========================");
	AddStatistic("TerrainObject Update", "%", gos_timedata,
		(PVOID)&MCTimeTerrainObjectsUpdate, 0);
	AddStatistic("TerrainObject T&L", "%", gos_timedata,
		(PVOID)&MCTimeTerrainObjectsTL, 0);
	AddStatistic(
		"Mech Update", "%", gos_timedata, (PVOID)&MCTimeMechsUpdate, 0);
	AddStatistic("Mech T&L", "%", gos_timedata, (PVOID)&MCTimeMechsTL, 0);
	AddStatistic(
		"Vehicle Update", "%", gos_timedata, (PVOID)&MCTimeVehiclesUpdate, 0);
	AddStatistic("Vehicle T&L", "%", gos_timedata, (PVOID)&MCTimeVehiclesTL, 0);
	AddStatistic(
		"Turret Update", "%", gos_timedata, (PVOID)&MCTimeTurretsUpdate, 0);
	AddStatistic("Turret T&L", "%", gos_timedata, (PVOID)&MCTimeTurretsTL, 0);
	AddStatistic("Everything else Update", "%", gos_timedata,
		(PVOID)&MCTimeAllElseUpdate, 0);
	StatisticFormat("=========================");
	AddStatistic(
		"Total Mission Time", "%", gos_timedata, (PVOID)&MCTimeMissionTotal, 0);
	StatisticFormat("=========================");
	AddStatistic(
		"Total LOS Calc Time", "%", gos_timedata, (PVOID)&MCTimeLOSCalc, 0);
	StatisticFormat("=========================");
	AddStatistic("Total Anim Calc Time", "%", gos_timedata,
		(PVOID)&MCTimeAnimationCalc, 0);
	statisticsInitialized = true;
	HeapList::initializeStatistics();
	TerrainTextures::initializeStatistics();
#endif
}

void Mission::initTGLForLogistics()
{
	//---------------------------------------------------------
	uint32_t tglHeapSize = 4 * 1024 * 1024;
	//---------------------------------------------------------
	// Reset the lightening in case they exitted with a flash on screen!!
	TG_Shape::lighteningLevel = 0;
	//---------------------------------------------------------
	// End the Tiny Geometry Layer Heap for the Mission
	if (TG_Shape::tglHeap)
	{
		// Shut down the TGL RAM pools.
		if (colorPool)
		{
			colorPool->destroy();
			delete colorPool;
			colorPool = nullptr;
		}
		if (vertexPool)
		{
			vertexPool->destroy();
			delete vertexPool;
			vertexPool = nullptr;
		}
		if (facePool)
		{
			facePool->destroy();
			delete facePool;
			facePool = nullptr;
		}
		if (shadowPool)
		{
			shadowPool->destroy();
			delete shadowPool;
			shadowPool = nullptr;
		}
		if (trianglePool)
		{
			trianglePool->destroy();
			delete trianglePool;
			trianglePool = nullptr;
		}
		TG_Shape::tglHeap->destroy();
		delete TG_Shape::tglHeap;
		TG_Shape::tglHeap = nullptr;
	}
	//------------------------------------------------------
	// Start the Tiny Geometry Layer Heap for Logistics
	if (!TG_Shape::tglHeap)
	{
		TG_Shape::tglHeap = new UserHeap;
		TG_Shape::tglHeap->init(tglHeapSize, "TinyGeom");
		// Start up the TGL RAM pools.
		colorPool = new TG_VertexPool;
		colorPool->init(2000);
		vertexPool = new TG_GOSVertexPool;
		vertexPool->init(2000);
		facePool = new TG_DWORDPool;
		facePool->init(4000);
		shadowPool = new TG_ShadowPool;
		shadowPool->init(2000);
		trianglePool = new TG_TrianglePool;
		trianglePool->init(2000);
	}
}

//*************************************************************************************************
// end of file ( mission2.cpp )
