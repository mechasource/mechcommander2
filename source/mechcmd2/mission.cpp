//******************************************************************************************
//	mission.cpp - This file contains the mission class code
//		Missions are what scenarios were in MechCommander 1.
//
//	MechCommander 2
//
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//
#include "stdafx.h"

//----------------------------------------------------------------------------------
// Include Files
// #include <mclib.h>

#ifndef MISSION_H
#include "mission.h"
#endif

//#ifndef MOVE_H
//#include "move.h"
//#endif

#ifndef MOVEMGR_H
#include "movemgr.h"
#endif

#include "gamesound.h"
#ifndef SOUNDS_H
#include "sounds.h"
#endif

#ifndef COLLSN_H
#include "collsn.h"
#endif

#ifndef CMPONENT_H
#include "cmponent.h"
#endif

#ifndef OBJMGR_H
#include "objmgr.h"
#endif

#include "mover.h"
#ifndef MECH_H
#include "mech.h"
#endif

#ifndef GVEHICL_H
#include "gvehicl.h"
#endif

#ifndef CONTACT_H
#include "contact.h"
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

#ifndef GAMECAM_H
#include "gamecam.h"
#endif

#ifndef MULTPLYR_H
#include "multplyr.h"
#endif

#ifndef WEAPONFX_H
#include "weaponfx.h"
#endif

#ifndef WEATHER_H
#include "weather.h"
#endif

#ifndef GATE_H
#include "gate.h"
#endif

#ifndef LOGISTICSDATA_H
#include "logisticsdata.h"
#endif

#ifndef LOGISTICSPILOT_H
#include "logisticsPilot.h"
#endif

#ifndef GAMELOG_H
#include "gamelog.h"
#endif

#ifndef CELLIP_H
#include "cellip.h"
#endif

#ifndef PREFS_H
#include "prefs.h"
#endif

#include "..\resource.h"

#include <gameos.hpp>

//----------------------------------------------------------------------------------
// Macro Definitions
//#define NUM_FIRERANGES		3

//----------------------------------------------------------------------------------
// Static globals
float minFrameRate   = 4.0;
float minFrameLength = 1.0 / 4.0;

int32_t MissionStartTime = 0; // No Idea

Mission* mission		= nullptr;
uint32_t scenarioResult = mis_PLAYING;
int32_t scenarioEndTurn = -1;

extern int32_t GameDifficulty;
int32_t MechSalvageChance = 100;

int32_t globalPlayerSkills[4]; // Per spec.  Integer value = percentage * 100
int32_t globalPlayerWeapons[4];
int32_t globalEnemySkills[4];
int32_t globalEnemyWeapons[4];

extern float WeaponRanges[NUM_WEAPON_RANGE_TYPES][2];
extern float OptimalRangePoints[NUM_WEAPON_RANGE_TYPES];
extern bool OptimalRangePointInRange[NUM_WEAPON_RANGE_TYPES][3];

float globalMissionValues[MAX_GLOBAL_MISSION_VALUES];

extern Stuff::Vector3D debugMechActorPosition[];
extern float mechDebugAngle[];
extern float torsoDebugAngle[];

extern Stuff::Vector3D debugGVActorPosition;

extern GameObjectFootPrint* tempSpecialAreaFootPrints;
extern int32_t tempNumSpecialAreas;
extern uint32_t ServerPlayerNum;

extern bool useNonWeaponEffects;

extern uint32_t elementHeapSize;
extern uint32_t maxElements;
extern uint32_t maxGroups;
extern uint32_t missionHeapSize;
extern uint32_t polyHeapSize;
extern uint32_t spriteDataHeapSize;
extern uint32_t spriteHeapSize;

extern int32_t CurMultiplayCode;
extern int32_t CurMultiplayParam;

extern bool quitGame;
extern float MaxExtractUnitDistance;

extern bool useFog;
extern bool useShadows;
extern bool inViewMode;
extern uint32_t viewObject;
extern float loadProgress;

extern char TeamRelations[MAX_TEAMS][MAX_TEAMS];

ByteFlag* VisibleBits = nullptr; // What can currently be seen
ByteFlag* SeenBits = nullptr;	// What HAS been seen

UserHeapPtr missionHeap = nullptr;

uint32_t MultiPlayTeamId	  = 0xFFFFFFFF;
uint32_t MultiPlayCommanderId = 0xFFFFFFFF;

bool useSensors			   = true;
bool useCollisions		   = true;
int32_t missionLineChanged = 0;
bool GeneralAlarm		   = false;

extern bool KillAmbientLight;

extern GameLog* CombatLog;
#ifndef FINAL
float CheatHitDamage = 0.0f;
#endif

bool neverEndingStory = false;

void GetBlockedDoorCells(int32_t moveLevel, int32_t door, PSTR openCells);
void PlaceStationaryMovers(MoveMap* map);
void PlaceMovers();
//---------------------------------------------------------------------------
void initABL();
void closeABL();

#define MAX_DISABLE_AT_START 100
extern int32_t NumDisableAtStart;
extern int32_t DisableAtStart[MAX_DISABLE_AT_START];
bool showFrameRate = false;

bool Mission::terminationCounterStarted = false;
double Mission::missionTerminationTime  = -1.0;
uint32_t Mission::terminationResult		= mis_PLAYING;

extern float OneOverProcessorSpeed;
extern PriorityQueuePtr openList;

//
// Macro used for statistic timing of main functions
//
#ifdef LAB_ONLY
#define ProfileTime(x, y)                                                      \
	x = GetCycles();                                                           \
	y;                                                                         \
	x = GetCycles() - x;
extern int64_t MCTimeTerrainUpdate;
extern int64_t MCTimeCameraUpdate;
extern int64_t MCTimeWeatherUpdate;
extern int64_t MCTimePathManagerUpdate;
extern int64_t MCTimeRunBrainUpdate;
extern int64_t MCTimePath1Update;
extern int64_t MCTimePath2Update;
extern int64_t MCTimePath3Update;
extern int64_t MCTimePath4Update;
extern int64_t MCTimePath5Update;
extern int64_t MCTimeCalcGoal1Update;
extern int64_t MCTimeCalcPath1Update;
extern int64_t MCTimeCalcPath2Update;
extern int64_t MCTimeCalcPath3Update;
extern int64_t MCTimeCalcPath4Update;
extern int64_t MCTimeCalcPath5Update;
extern int64_t MCTimeCalcGoal2Update;
extern int64_t MCTimeCalcGoal3Update;
extern int64_t MCTimeCalcGoal4Update;
extern int64_t MCTimeCalcGoal5Update;
extern int64_t MCTimeCalcGoal6Update;
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

extern int64_t MCTimeTerrainObjectsTL;
extern int64_t MCTimeMechsTL;
extern int64_t MCTimeVehiclesTL;
extern int64_t MCTimeTurretsTL;

extern int64_t MCTimeAllElseUpdate;

extern int64_t MCTimeAnimationCalc;

extern int64_t MCTimeABLLoad;
extern int64_t MCTimeMiscToTeamLoad;
extern int64_t MCTimeTeamLoad;
extern int64_t MCTimeObjectLoad;
extern int64_t MCTimeTerrainLoad;
extern int64_t MCTimeMoveLoad;
extern int64_t MCTimeMissionABLLoad;
extern int64_t MCTimeWarriorLoad;
extern int64_t MCTimeMoverPartsLoad;
extern int64_t MCTimeObjectiveLoad;
extern int64_t MCTimeCommanderLoad;
extern int64_t MCTimeMiscLoad;
extern int64_t MCTimeGUILoad;

extern int64_t x1;

extern int64_t x;

#else
#define ProfileTime(x, y) y;
#endif

int32_t GameVisibleVertices = 60;
float BaseHeadShotElevation = 1.0f;

bool DisplayCameraAngle = false;
extern int32_t MaxResourcePoints;
extern int32_t resolution;
extern int32_t renderer;

bool loadInMissionSave = false;
bool saveInMissionSave = false;

float forcedFrameRate = -1.0f;

extern bool invulnerableON; // Used for tutorials so mechs can take damage, but
							// look like they are taking damage!  Otherwise, I'd
							// just use NOPAIN!!

#define DEFAULT_SKY 1
//----------------------------------------------------------------------------------
// class Mission
int32_t Mission::update(void)
{
	if (active)
	{
		turn++;
		memset(ObjectManager->moverLineOfSightTable, -1,
			ObjectManager->maxMovers * ObjectManager->maxMovers);
#ifdef LAB_ONLY
		MCTimeLOSCalc		= 0;
		MCTimeAnimationCalc = 0;
#endif
		if (forcedFrameRate != -1.0f)
			frameLength /= forcedFrameRate;
		if ((missionLineChanged + 50) < turn)
		{
#ifndef FINAL
			if (userInput->getKeyDown(KEY_X) && userInput->ctrl() &&
				!userInput->alt() && !userInput->shift())
			{
				useCollisions ^= true;
				missionLineChanged = turn;
			}
			if (userInput->getKeyDown(KEY_X) && userInput->ctrl() &&
				userInput->alt() && userInput->shift())
			{
				saveInMissionSave  = true;
				missionLineChanged = turn;
			}
			if (userInput->getKeyDown(KEY_Z) && userInput->ctrl() &&
				userInput->alt() && userInput->shift())
			{
				loadInMissionSave  = true;
				missionLineChanged = turn;
			}
			if (userInput->getKeyDown(KEY_Y) && userInput->ctrl() &&
				!userInput->alt() && !userInput->shift())
			{
				DisplayCameraAngle ^= true;
				missionLineChanged = turn;
			}
			if (userInput->getKeyDown(KEY_N) && userInput->ctrl() &&
				!userInput->alt() && !userInput->shift())
			{
				useSensors ^= true;
				missionLineChanged = turn;
			}
			if (userInput->getKeyDown(KEY_P) && userInput->ctrl() &&
				userInput->alt() && userInput->shift())
			{
				LogisticsData::instance->setResourcePoints(100000);
				missionLineChanged = turn;
			}
			if (userInput->getKeyDown(KEY_R) && userInput->ctrl() &&
				userInput->alt() && userInput->shift())
			{
				if (forcedFrameRate == -1.0f)
					forcedFrameRate = 2.0f;
				else if (forcedFrameRate == 2.0f)
					forcedFrameRate = 5.0f;
				else if (forcedFrameRate == 5.0f)
					forcedFrameRate = 10.0f;
				else if (forcedFrameRate == 10.0f)
					forcedFrameRate = -1.0f;
				missionLineChanged = turn;
			}
			if (userInput->getKeyDown(KEY_C) && userInput->ctrl() &&
				userInput->alt() && userInput->shift())
			{
				neverEndingStory   = true;
				missionLineChanged = turn;
			}
#endif
		}
		//---------------------------------------
		// The game believes we will drop well below par for a frame or two.
		// put the game into slo-mo so we don't lose any frames of animation.
		dynamicFrameTiming = true;
		//---------------------------------------
		// Save the game in-Mission if requested
		if (saveInMissionSave)
		{
			save("data\\savegame\\testgame.ims");
			saveInMissionSave = false;
		}
		//--------------------------------------------------
		// Update length of time scenario has been running.
		if (!missionInterface->isPaused() || MPlayer)
		{
			// First Frame we just set LastTimeGetTime.
			// After that, it increments based on System Time.
			// NOT the crazy GameOS frameRate.
			uint32_t currentTimeGetTime = timeGetTime();
			if (LastTimeGetTime != 0xffffffff)
			{
				float milliseconds = currentTimeGetTime - LastTimeGetTime;
				scenarioTime += (milliseconds / 1000.0f);
			}
			LastTimeGetTime = currentTimeGetTime;
			soundSystem->clearIsPaused();
		}
		else
		{
			// Keep track of system time.  Just don't add it to scenarioTime!!
			uint32_t currentTimeGetTime = timeGetTime();
			LastTimeGetTime				= currentTimeGetTime;
			soundSystem->setIsPaused();
		}
		//--------------------------------------------------
		//------------------------------------------------------------------------
		// There is a TINYYYYYYYYYY chance this will never go if timeGetTime()
		// happens to return 0 (one millisecond every approx. 49 days). I can
		// live with that...
		//		if (MPlayer && (MPlayer->startTime >= 0.0))
		//			runningTime = (float)(gos_GetElapsedTime()) -
		//MPlayer->startTime; 		else if (MPlayer && (scenarioTime > 10.0))
		//			Fatal(MissionStartTime, " runningTime is not working...why?
		//"); 		if (MPlayer) 			actualTime = runningTime; 		else
		// Try ALWAYS using SCENARIO Time.  This has been specially coded to be
		// nummies!!
		actualTime = scenarioTime;
#if 0
		static bool tested = false;
		if((scenarioTime > 15.0) && !tested)
		{
			tested = true;
			ABLFile* ablSaveFile;
			ABLi_saveEnvironment(ABLFile * ablFile)
			{
			}
#endif
		mcTextureManager->clearArrays();
		if (missionInterface)
			ProfileTime(MCTimeInterfaceUpdate, missionInterface->update());
		ProfileTime(MCTimeCameraUpdate, eye->update());
		missionInterface->updateVTol();
		ProfileTime(MCTimeTerrainUpdate, land->update());
		// ALWAYS update weather AFTER the camera.  May change the lights!
		if (useNonWeaponEffects)
			ProfileTime(MCTimeWeatherUpdate,
				weather->update()); // Should the rain fall during a pause?
		missionInterface->updateWaypoints();
#ifdef USE_PATH_COST_TABLE
		GlobalMoveMap[0]->resetPathCostTable();
#endif
		ProfileTime(MCTimePathManagerUpdate, PathManager->update());
		if (KillAmbientLight)
		{
			//		ambientRed<<16)+(ambientGreen<<8)+ambientBlue;
		}
		//-----------------------------------------------------------
		// Lastly, process the terrain geometry which loads textures
		// Must do this to keep from Locking during the updateRenders phase
		// Also reset the object flags because we recalc those during geometry!
		land->clearObjBlocksActive();
		land->clearObjVerticesActive();
		land->terrainTextures->update();
		ProfileTime(MCTimeTerrainGeometry, land->geometry());
		if (missionInterface->isPaused() && !MPlayer)
			ObjectManager->updateAppearancesOnly(true, true, true);
		else
			ObjectManager->update(true, true, true);
		ProfileTime(MCTimeCraterUpdate, craterManager->update());
		// Do not UPDATE the textures during a pause.
		// This uncaches things which only objectManager->update can cache back
		// in!!!!!
		if (!missionInterface->isPaused() || MPlayer)
			ProfileTime(MCTimeTXMManagerUpdate, mcTextureManager->update());
		//--------------------------------------
		// update sensor and contact managers...
		if (useSensors && (!missionInterface->isPaused() || MPlayer))
			ProfileTime(MCTimeSensorUpdate, SensorManager->update());
		if (useCollisions && (!missionInterface->isPaused() || MPlayer))
			ProfileTime(
				MCTimeCollisionUpdate, ObjectManager->updateCollisions());
		if (missionBrain)
		{
			if (!missionInterface->isPaused() || MPlayer)
			{
				ProfileTime(MCTimeMissionScript, missionBrain->execute());
				int32_t missionResult = missionBrain->getInteger();
				if (missionResult == 9999)
					return (terminationResult = 9999);
				if (!MPlayer)
					terminationResult = missionResult;
			}
		}
		//----------------------------------------------------
		// Check is all player forces dead/disabled.
		if (!MPlayer && !terminationCounterStarted)
		{
			bool playerForceAllDead = true;
			for (size_t i = 0; i < ObjectManager->getNumMovers(); i++)
			{
				MoverPtr mover = ObjectManager->getMover(i);
				if (mover->getCommanderId() == Commander::home->getId())
					if (!mover->isDisabled() && !mover->isDestroyed() &&
						mover->isOnGUI())
					{
						playerForceAllDead = false;
						break;
					}
			}
			if (playerForceAllDead && !neverEndingStory)
			{
				terminationResult		  = mis_PLAYER_LOST_BIG;
				terminationCounterStarted = true;
				missionTerminationTime	= actualTime + 5.0 /*seconds*/;
			}
		}
		if (!neverEndingStory && terminationCounterStarted)
		{
			if (missionTerminationTime <= actualTime)
			{
				if (ControlGui::instance->resultsDone())
				{
					/* if the gui is finished rendering objective results then
					 * end it */
					scenarioResult = terminationResult;
					if (!scenarioResult)
					{
						// mission brain frequently trashes the termination
						// result
						if (!MPlayer)
						{
							int32_t status = Team::home->objectives.Status();
							if (OS_SUCCESSFUL == status)
								scenarioResult = mis_PLAYER_WIN_BIG;
							else
								scenarioResult = mis_PLAYER_LOST_BIG;
						}
					}
					// need to reset the termination result
				}
				else if (!MPlayer)
				{
					/* if all the player's dudes are dead then end it */
					bool playerForceAllDead = true;
					for (size_t i = 0; i < ObjectManager->getNumMovers(); i++)
					{
						MoverPtr mover = ObjectManager->getMover(i);
						if (mover->getCommanderId() == Commander::home->getId())
							if (!mover->isDisabled() && !mover->isDestroyed() &&
								mover->isOnGUI())
							{
								playerForceAllDead = false;
								break;
							}
					}
					if (playerForceAllDead)
					{
						terminationResult		  = mis_PLAYER_LOST_BIG;
						terminationCounterStarted = true;
						missionTerminationTime = actualTime + 5.0 /*seconds*/;
					}
				}
			}
		}
		else
		{
			// As int32_t as the ABL script returns -1, we won't do this.
			// NEEDED for Tutorials
			if (terminationResult != -1)
			{
				if (MPlayer)
				{
					if (MPlayer->calcMissionStatus())
					{
						terminationCounterStarted = true;
						missionTerminationTime = actualTime + 5.0 /*seconds*/;
						if (MPlayer->playerInfo[MPlayer->commanderID].winner)
							terminationResult = mis_PLAYER_WIN_BIG;
						else
							terminationResult = mis_PLAYER_LOST_BIG;
					}
				}
				else if (1 <= Team::home->objectives.Count())
				{
					int32_t status = Team::home->objectives.Status();
					if (0 < Team::home->numPrimaryObjectives)
					{
						if (OS_UNDETERMINED != status)
						{
							if (!neverEndingStory)
							{
								if (OS_SUCCESSFUL == status)
								{
									terminationResult = mis_PLAYER_WIN_BIG;
									terminationCounterStarted = true;
									missionTerminationTime =
										actualTime + 5.0 /*seconds*/;
								}
								else
								{
									terminationResult = mis_PLAYER_LOST_BIG;
									terminationCounterStarted = true;
									missionTerminationTime =
										actualTime + 5.0 /*seconds*/;
								}
							}
						}
					}
				}
			}
		}
		if (!MPlayer && !terminationCounterStarted)
		{
			if ((m_timeLimit != -1.0f) && (actualTime > m_timeLimit) &&
				!neverEndingStory)
			{
				terminationResult		  = mis_PLAYER_LOST_BIG;
				terminationCounterStarted = true;
				missionTerminationTime	= actualTime + 5.0 /*seconds*/;
			}
		}
		if (userInput->getKeyDown(KEY_F) && !userInput->ctrl() &&
			userInput->alt() && !userInput->shift())
		{
			showFrameRate ^= true;
			missionLineChanged = turn;
		}
#ifndef FINAL
		bool cheatWin  = (userInput->getKeyDown(KEY_W) && userInput->shift() &&
						  userInput->ctrl() && userInput->alt());
		bool cheatLose = (userInput->getKeyDown(KEY_L) && userInput->shift() &&
						  userInput->ctrl() && userInput->alt());
		if (cheatWin)
			scenarioResult = mis_PLAYER_WIN_BIG;
		else if (cheatLose)
			scenarioResult = mis_PLAYER_LOST_BIG;
		// FORCE these to play to thwart any ABL bugs.
		// OPPOSITE for a reason!!!!!!!!
		// Duane decided to switch the order for our conveniance.  Thanks!!
		if ((terminationResult) && (terminationResult != -1) &&
			(terminationResult < mis_PLAYER_DRAW))
		{
			soundSystem->playDigitalMusic(WIN_TUNE_0);
		}
		else if (terminationResult > mis_PLAYER_DRAW)
		{
			soundSystem->playDigitalMusic(LOSE_TUNE_0);
		}
		if (showFrameRate)
		{
			char text[1024];
			sprintf(text, "FrameRate: %f", 1.0f / frameLength);
			uint32_t width, height;
			Stuff::Vector4D moveHere;
			moveHere.x = 0.0f;
			moveHere.y = 0.0f;
			gos_TextSetAttributes(
				gosFontHandle, 0, gosFontScale, false, true, false, false);
			gos_TextStringLength(&width, &height, text);
			moveHere.z = width;
			moveHere.w = height;
			globalFloatHelp->setFloatHelp(text, moveHere, SD_GREEN, XP_BLACK,
				1.0f, true, false, false, false);
		}
#endif
#ifdef LAB_ONLY
		MCTimeMissionTotal =
			MCTimeTerrainUpdate + MCTimeCameraUpdate + MCTimeWeatherUpdate +
			MCTimePathManagerUpdate + MCTimeRunBrainUpdate + MCTimePath1Update +
			MCTimePath2Update + MCTimePath3Update + MCTimePath4Update +
			MCTimePath5Update + MCTimeCalcPath1Update + MCTimeCalcPath2Update +
			MCTimeCalcPath3Update + MCTimeCalcPath4Update +
			MCTimeCalcPath5Update + MCTimeCalcGoal1Update +
			MCTimeCalcGoal2Update + MCTimeCalcGoal3Update +
			MCTimeCalcGoal4Update + MCTimeCalcGoal5Update +
			MCTimeCalcGoal6Update + MCTimeTerrainGeometry +
			MCTimeInterfaceUpdate + MCTimeCraterUpdate +
			MCTimeTXMManagerUpdate + MCTimeSensorUpdate + MCTimeLOSUpdate +
			MCTimeCollisionUpdate + MCTimeMissionScript +
			MCTimeTerrainObjectsUpdate + MCTimeMechsUpdate +
			MCTimeVehiclesUpdate + MCTimeTurretsUpdate + MCTimeAllElseUpdate;
#endif
	}
	return scenarioResult;
}
//----------------------------------------------------------------------------------
int32_t Mission::getStatus(void)
{
	if (terminationCounterStarted)
		return (terminationResult);
	return (mis_PLAYING);
}
//----------------------------------------------------------------------------------
int32_t Mission::render(void)
{
	if (active)
	{
		uint8_t tempAmbientLight[3];
		if (KillAmbientLight)
		{
			tempAmbientLight[0] = eye->ambientRed;
			tempAmbientLight[1] = eye->ambientGreen;
			tempAmbientLight[2] = eye->ambientBlue;
			eye->ambientRed		= 0xFF;
			eye->ambientGreen   = 0xFF;
			eye->ambientBlue	= 0xFF;
		}
		eye->render();
		GameMap->clearCellDebugs(0);
		//-----------------------------------------------------
		// FOG time.  Set Render state to FOG on!
		uint32_t fogColor = eye->fogColor;
		if (useFog)
		{
			gos_SetRenderState(gos_State_Fog, (int32_t)&fogColor);
		}
		else
		{
			gos_SetRenderState(gos_State_Fog, 0);
		}
		gos_SetRenderState(gos_State_Fog, 0);
		FloatHelp::renderAll();
		currentFloatHelp = 0;
		if (missionInterface)
			missionInterface->render();
		if (KillAmbientLight)
		{
			eye->ambientRed   = tempAmbientLight[0];
			eye->ambientGreen = tempAmbientLight[1];
			eye->ambientBlue  = tempAmbientLight[2];
		}
		// reset the TGL RAM pools.
		colorPool->reset();
		vertexPool->reset();
		facePool->reset();
		shadowPool->reset();
		trianglePool->reset();
	}
	return scenarioResult;
}
//----------------------------------------------------------------------------------
float applyDifficultySkill(float chance, bool isPlayer)
{
	if (isPlayer)
	{
		switch (GameDifficulty)
		{
		case 0: // Easy
		case 1: // Medium
		case 2: // Hard
		case 3: // Really Hard
			chance *= (float(globalPlayerSkills[GameDifficulty]) / 100.0);
			return (chance);
			break;
		default:
			// return (chance);
			NODEFAULT;
			break;
		}
	}
	else
	{
		switch (GameDifficulty)
		{
		case 0: // Easy
		case 1: // Medium
		case 2: // Hard
		case 3: // Really Hard
			chance *= (float(globalEnemySkills[GameDifficulty]) / 100.0);
			return (chance);
			break;
		default:
			// return(chance);
			NODEFAULT;
			break;
		}
	}
}
//----------------------------------------------------------------------------------
float applyDifficultyWeapon(float dmg, bool isPlayer)
{
	if (isPlayer)
	{
		switch (GameDifficulty)
		{
		case 0: // Easy
		case 1: // Medium -- NO CHANGE!
		case 2: // Hard
		case 3: // Very Hard
			dmg *= (float(globalPlayerWeapons[GameDifficulty]) / 100.0);
			break;
		default:
			NODEFAULT;
			break;
		}
	}
	else
	{
		switch (GameDifficulty)
		{
		case 0: // Easy
		case 1: // Medium -- NO CHANGE!
		case 2: // Hard
		case 3: // Very Hard
			dmg *= (float(globalEnemyWeapons[GameDifficulty]) / 100.0);
			break;
		default:
			NODEFAULT;
			break;
		}
	}
	//------------------------------------------------------------------
	// Must keep damage an integral part of .25 or bad things happen
	// In theory only needed for Multiplayer but better safe then...
	if (((float)((int32_t)(dmg / 0.25)) * 0.25) != dmg)
	{
		dmg = ((float)((int32_t)(dmg / 0.25)) * 0.25);
	}
	if (dmg < 0.0)
		dmg = 0.0;
	if (dmg > 255.0)
		dmg = 255.0;
	return dmg;
}
//----------------------------------------------------------------------------------
void InitDifficultySettings(FitIniFile* gameSystemFile)
{
	int32_t result = gameSystemFile->seekBlock("DifficultySettings");
	gosASSERT(result == NO_ERROR);
	result =
		gameSystemFile->readIdLongArray("PlayerSkills", globalPlayerSkills, 4);
	gosASSERT(result == NO_ERROR);
	result =
		gameSystemFile->readIdLongArray("EnemySkills", globalEnemySkills, 4);
	gosASSERT(result == NO_ERROR);
	result = gameSystemFile->readIdLongArray(
		"PlayerWeapons", globalPlayerWeapons, 4);
	gosASSERT(result == NO_ERROR);
	result =
		gameSystemFile->readIdLongArray("EnemyWeapons", globalEnemyWeapons, 4);
	gosASSERT(result == NO_ERROR);
}
//--------------------------------------------------
// Game System Constants -- Definitions here.
float maxVisualRange		 = 0.0;
uint32_t MaxTreeLOSCellBlock = 0;
float MaxVisualRadius		 = 0.0;
float fireVisualRange		 = 0.0;
extern float WeaponRange[];		 // MOVER
extern float DefaultAttackRange; // MOVER
float baseSensorRange = 0.0;
extern int32_t visualRangeTable[];
extern uint8_t godMode;   // Can I simply see everything, enemy and friendly?
uint8_t revealTacMap = 0; // Is Tacmap always revealed?
uint8_t footPrints = 0;   // Do we use/draw footprints?
bool CantTouchThis = false;
int32_t tonnageDivisor =
	1.0; // Amount we divide tons under drop weight by to calc resource bonus
int32_t resourcesPerTonDivided = 0; // Tons per above divided number
float MineDamage = 0.0;				// Damage mine does per tile
float MineSplashDamage = 0.0;	// Damage mine explosion does to all in tile.
float MineSplashRange  = 0.0;	// Range of Splash Damage
int32_t MineExplosion = -1;		 // Object ID of mine explosion
int32_t MineLayThrottle = 0;	 // Throttle setting of mine Layer
int32_t MineSweepThrottle = 0;   // Throttle setting of mine sweeper
float StrikeWaitTime	 = 0.0f; // Time between aerospace Spotter strike calls
float StrikeTimeToImpact = 0.0f; // Time it takes air strike to come in.
float MineWaitTime =
	0.0; // Time Mine Layer must sit in Tile before mine is laid.
int32_t totalSmokeSpheres = 0; // Maximum number of smoke spheres in world.
int32_t totalSmokeShapeSize =
	0; // Amount we allocate to smoke heap for spheres/textures.
int32_t maxFiresBurning = 0; // Maximum number of fires allowed in world.
float maxFireBurnTime =
	0.0;			   // Maximum time a fire can be burning before it dies out.
bool force64MB = TRUE; // Do we have between 64 and 32 Mb of memory?
bool force32Mb = FALSE;		  // Do we have 32Mb of memory only?
float InfluenceRadius = -1.0; // Capture Radius
float InfluenceTime   = 0.0;  // Time inside to Capture
//----------------------------------------------------------------------------------
void Mission::createPartObject(int32_t partIndex, MoverPtr mover)
{
	//-----------------------------------
	// All parts are movers in this game!
	//------------------------------------------------------------------
	// Create the object -- If object is Not DESTROYED Already OR
	// it doesn't exist yet.  (parts[i].object = nullptr
	//
	// This could NEVER have worked.  Destroyeds still need to be created.
	// Otherwise, why is there code inside this IF which checks if destroyed?  I
	// blame Glenn's Crazy Bracing Style! Create the part just like always, just
	// start it disabled.
	//
	// If the mech is not on OUR team, you can recover it during the mission.
	// If the mech IS on our team, it starts shutdown and NOT added to Heidi's
	// interface. When we get close enough to it, it adds itself to Heidi's
	// world and we can command it!
	if (parts[partIndex].objectWID == 0)
	{
		ObjectTypePtr objType =
			ObjectManager->getObjectType(parts[partIndex].objNumber);
		Assert(objType != nullptr, partIndex,
			" Mission.createPartObject: unable to get objType ");
		parts[partIndex].objectWID = mover->getWatchID();
		mover->init(true, objType);
		mover->setAwake(parts[partIndex].active ? true : false);
		mover->setHandle(ObjectManager->getHandle(mover));
		//----------------------------------------------
		// Load the profile data into the game object...
		// This is MONDO DIFFERENT NOW.
		// Profiles are in the mech's CSV file.  You should just init with mover
		// with the variant number and the rest is done in the mover itself.
		//
		// Heidi, this is what needs to change in the mission.fit files!
		// -fs 12/7/99		A date which will live in Infamy.
		// Must also load Ground Vehicles the OLDen WAY until further notice.
		switch (mover->getObjectClass())
		{
		case BATTLEMECH:
			mover->init(parts[partIndex].variantNum);
			break;
		case GROUNDVEHICLE:
		case ELEMENTAL:
		{
			if (ObjectTypeManager::objectFile->seekPacket(
					parts[partIndex].objNumber) == NO_ERROR)
			{
				//--------------------------------------------------------
				// All new code here.  This will ask the objectType it is
				// loading what kind of objectType it is and create it
				// based on that instead of objTypeNum.
				FitIniFile profileFile;
				int32_t result = profileFile.open(ObjectTypeManager::objectFile,
					ObjectTypeManager::objectFile->getPacketSize());
				if (result != NO_ERROR)
					STOP((" Mission.createPartObject: can't open file "));
				if (result == NO_ERROR)
				{
					//-------------------------------------------
					result = mover->init(&profileFile);
					if (result != NO_ERROR)
						STOP((" Mission.createPartObject: bad Profile File "));
				}
				profileFile.close();
			}
		}
		break;
		}
		mover->setPilotHandle(parts[partIndex].pilot);
		mover->setTeamId(parts[partIndex].teamId, true);
		mover->setSquadId(parts[partIndex].squadId);
		mover->calcWeaponEffectiveness(TRUE);
		mover->calcWeaponEffectiveness(FALSE);
		if (parts[partIndex].controlType == CONTROL_PLAYER)
			mover->setControl(CONTROL_PLAYER);
		else if (MPlayer)
			mover->setControl(MPlayer->isServer() ? CONTROL_AI : CONTROL_NET);
		else
			mover->setControl(CONTROL_AI);
		mover->setPosition(parts[partIndex].position);
		mover->setLastValidPosition(parts[partIndex].position);
		mover->setRotation(parts[partIndex].rotation);
		mover->setCommanderId(parts[partIndex].commanderID);
		switch (mover->getObjectClass())
		{
		case BATTLEMECH:
		{
			((BattleMechPtr)mover)->captureable = parts[partIndex].captureable;
			//-----------------------------------------
			// Make sure the pilot paints his Mech!
			// Use editor colors UNLESS this is on Commander::home
			AppearancePtr myActor = mover->getAppearance();
			if (MPlayer)
				myActor->resetPaintScheme(
					MPlayer->colors[MPlayer->playerInfo[mover->commanderId]
										.stripeColor],
					MPlayer->colors[MPlayer->playerInfo[mover->commanderId]
										.stripeColor],
					MPlayer->colors[MPlayer->playerInfo[mover->commanderId]
										.baseColor[BASECOLOR_TEAM]]);
			else
			{
				if (mover->getCommanderId() == Commander::home->getId())
					myActor->resetPaintScheme(prefs.highlightColor,
						prefs.highlightColor, prefs.baseColor);
				else
					myActor->resetPaintScheme(parts[partIndex].highlightColor1,
						parts[partIndex].highlightColor2,
						parts[partIndex].baseColor);
				// myActor->setGesture(parts[partIndex].gestureId);  DOn't do
				// this unless you want the mechs to start shutdown and power up!
			}
		}
		break;
		case GROUNDVEHICLE:
		{
			AppearancePtr myActor = mover->getAppearance();
			if (MPlayer)
				myActor->resetPaintScheme(
					MPlayer->colors[MPlayer->playerInfo[mover->commanderId]
										.stripeColor],
					MPlayer->colors[MPlayer->playerInfo[mover->commanderId]
										.stripeColor],
					MPlayer->colors[MPlayer->playerInfo[mover->commanderId]
										.baseColor[BASECOLOR_TEAM]]);
			else
			{
				if (mover->getCommanderId() == Commander::home->getId())
					myActor->resetPaintScheme(prefs.highlightColor,
						prefs.highlightColor, prefs.baseColor);
				else
					myActor->resetPaintScheme(parts[partIndex].highlightColor1,
						parts[partIndex].highlightColor2,
						parts[partIndex].baseColor);
				// myActor->setGesture(parts[partIndex].gestureId);  DOn't do
				// this unless you want the mechs to start shutdown and power up!
			}
		}
		break;
		case ELEMENTAL:
			break;
		}
		//-----------------------------------------------------------------
		// Set object Unique ID to be Part Number
		// parts[i].object->setIdNumber(i);
		if (MPlayer)
		{
			MPlayer->addToMoverRoster((MoverPtr)ObjectManager->getByWatchID(
				parts[partIndex].objectWID));
			MPlayer->addToPlayerMoverRoster(parts[partIndex].commanderID,
				(MoverPtr)ObjectManager->getByWatchID(
					parts[partIndex].objectWID));
			if (parts[partIndex].commanderID == MPlayer->commanderID)
				MPlayer->addToLocalMovers((MoverPtr)ObjectManager->getByWatchID(
					parts[partIndex].objectWID));
		}
		if (parts[partIndex].exists)
		{
			mover->setExists(true);
		}
		else
			mover->setExists(false);
		if (parts[partIndex].destroyed)
		{
			if (mover->getCommanderId() == Commander::home->getId())
			{
				mover->setStatus(OBJECT_STATUS_SHUTDOWN);
				mover->startDisabled = false;
				mover->setOnGUI(false); // Even for non-player Team Parts.  It
										// only FALSE if we link up with it
										// later!!!!!
			}
			else
			{
				mover->setStatus(OBJECT_STATUS_DISABLED);
				mover->disable(UNDETERMINED_DEATH);
				mover->startDisabled = true;
				if (mover->sensorSystem)
					mover->sensorSystem->disable();
				mover->setOnGUI(true); // Even for non-player Team Parts.  It
									   // only FALSE if we link up with it
									   // later!!!!!
			}
		}
		else
		{
			mover->setOnGUI(true); // Even for non-player Team Parts.  It only
								   // FALSE if we link up with it later!!!!!
		}
	}
}
//---------------------------------------------------------------------------
int32_t Mission::addMover(MoverInitData* moverSpec)
{
	//--------------------------------------
	// Load the mechwarrior into the mech...
	MechWarriorPtr pilot = MechWarrior::newWarrior();
	if (!pilot)
		STOP(("Too many pilots in this mission!"));
	FullPathFileName pilotFullFileName;
	pilotFullFileName.init(warriorPath, moverSpec->pilotFileName, ".fit");
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
	if (moverSpec->overrideLoadedPilot)
	{
		pilot->skills[MWS_GUNNERY] = pilot->skillRank[MWS_GUNNERY] =
			moverSpec->gunnerySkill;
		pilot->skills[MWS_PILOTING] = pilot->skillRank[MWS_PILOTING] =
			moverSpec->pilotingSkill;
		memcpy(pilot->specialtySkills, moverSpec->specialtySkills,
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
		pilot->setBrainName(moverSpec->brainFileName);
		brainFullFileName.init(warriorPath, moverSpec->brainFileName, ".abl");
	}
	int32_t moduleHandle =
		ABLi_preProcess(brainFullFileName, &numErrors, &numLinesProcessed);
	gosASSERT(moduleHandle >= 0);
	pilot->setBrain(moduleHandle);
	/*
	typedef struct _MoverInitData {
		char			pilotFileName[50];
		char			brainFileName[50];
		char			profileName[50];
		int32_t			objNumber;
		uint32_t	controlType;
		uint32_t	controlDataType;
		uint32_t	variant;
		float			position[2];
		int32_t			rotation;
		char			teamID;
		char			commanderID;
		int32_t			paintScheme;
		bool			active;
		bool			exists;
		char			icon;
		bool			capturable;
	} MoverInitData;
	*/
	// parts[i].gestureId = 2; // this has never changed
	ObjectTypePtr objType = ObjectManager->loadObjectType(moverSpec->objNumber);
	if (!objType)
		objType = ObjectManager->getObjectType(moverSpec->objNumber);
	if (objType)
	{
		MoverPtr mover = nullptr;
		switch (objType->getObjectTypeClass())
		{
		case BATTLEMECH_TYPE:
			mover = (MoverPtr)ObjectManager->newMech();
			break;
		case VEHICLE_TYPE:
			mover = (MoverPtr)ObjectManager->newVehicle();
			break;
		}
		if (mover)
		{
			mover->init(true, objType);
			mover->setAwake(moverSpec->active);
			mover->setHandle(ObjectManager->getHandle(mover));
			//----------------------------------------------
			// Load the profile data into the game object...
			// This is MONDO DIFFERENT NOW.
			// Profiles are in the mech's CSV file.  You should just init with
			// mover with the variant number and the rest is done in the mover
			// itself.
			//
			// Heidi, this is what needs to change in the mission.fit files!
			// -fs 12/7/99		A date which will live in Infamy.
			// Must also load Ground Vehicles the OLDen WAY until further
			// notice.
			switch (mover->getObjectClass())
			{
			case BATTLEMECH:
				mover->init(moverSpec->variant);
				break;
			case GROUNDVEHICLE:
			case ELEMENTAL:
			{
				if (ObjectTypeManager::objectFile->seekPacket(
						moverSpec->objNumber) == NO_ERROR)
				{
					//--------------------------------------------------------
					// All new code here.  This will ask the objectType it is
					// loading what kind of objectType it is and create it
					// based on that instead of objTypeNum.
					FitIniFile profileFile;
					int32_t result =
						profileFile.open(ObjectTypeManager::objectFile,
							ObjectTypeManager::objectFile->getPacketSize());
					if (result != NO_ERROR)
						STOP((" Mission.createPartObject: can't open file "));
					if (result == NO_ERROR)
					{
						//-------------------------------------------
						result = mover->init(&profileFile);
						if (result != NO_ERROR)
							STOP((" Mission.createPartObject: bad Profile "
								  "File "));
					}
					profileFile.close();
				}
			}
			break;
			}
			mover->setPilotHandle(pilot->getIndex());
			mover->setTeamId(moverSpec->teamID, true);
			mover->calcWeaponEffectiveness(true);
			mover->calcWeaponEffectiveness(false);
			if (moverSpec->controlType == CONTROL_PLAYER)
				mover->setControl(CONTROL_PLAYER);
			else if (MPlayer)
				mover->setControl(
					MPlayer->isServer() ? CONTROL_AI : CONTROL_NET);
			else
				mover->setControl(CONTROL_AI);
			mover->setPosition(moverSpec->position);
			mover->setLastValidPosition(moverSpec->position);
			mover->setRotation(moverSpec->rotation);
			mover->setCommanderId(moverSpec->commanderID);
			switch (mover->getObjectClass())
			{
			case BATTLEMECH:
			{
				((BattleMechPtr)mover)->captureable = moverSpec->capturable;
				//-----------------------------------------
				// Make sure the pilot paints his Mech!!
				AppearancePtr myActor = mover->getAppearance();
				myActor->resetPaintScheme(moverSpec->highlightColor1,
					moverSpec->highlightColor2, moverSpec->baseColor);
				// myActor->setGesture(moverSpec->gestureID);  Don't do this
				// unless you want things to start shutdown and power up!
			}
			break;
			case GROUNDVEHICLE:
			{
				AppearancePtr myActor = mover->getAppearance();
				myActor->resetPaintScheme(moverSpec->highlightColor1,
					moverSpec->highlightColor2, moverSpec->baseColor);
			}
			break;
			case ELEMENTAL:
				break;
			}
			//-----------------------------------------------------------------
			// Set object Unique ID to be Part Number
			// parts[i].object->setIdNumber(i);
			if (MPlayer)
			{
				MPlayer->addToMoverRoster(mover);
				MPlayer->addToPlayerMoverRoster(moverSpec->commanderID, mover);
				if (moverSpec->commanderID == MPlayer->commanderID)
					MPlayer->addToLocalMovers(mover);
			}
			if (moverSpec->exists)
				mover->setExists(true);
			else
				mover->setExists(false);
			//---------------------------------------------------------------------------------
			// If we're not playing multiplayer, make sure all home commander
			// movers have their localMoverId set to 0, so the iface can at
			// least check if a mover is player controlled...
			if (!MPlayer &&
				(Commander::home->getId() == moverSpec->commanderID))
				mover->setLocalMoverId(0);
			ObjectManager->modifyMoverLists(mover, MOVERLIST_ADD);
			Team::teams[moverSpec->teamID]->addToRoster(mover);
			missionInterface->addMover(mover);
			for (size_t i = 0; i < ObjectManager->getNumMovers(); i++)
			{
				MoverPtr curMover = ObjectManager->getMover(i);
				if (!curMover->isDisabled())
				{
					if (curMover->sensorSystem)
						curMover->sensorSystem->scanMover(mover);
					if (curMover->getPilot())
						curMover->getPilot()->triggerAlarm(
							PILOT_ALARM_NEW_MOVER, mover->getWatchID());
				}
			}
			if (moverSpec->numComponents > 0)
			{
				if (mover->getObjectClass() != BATTLEMECH)
					STOP(("LogisticsMech was not a MECH!!"));
				((BattleMechPtr)mover)
					->resetComponents(
						moverSpec->numComponents, moverSpec->components);
			}
			return (mover->getHandle());
		}
	}
	else
		return -1;
	return (0);
}
//---------------------------------------------------------------------------
int32_t Mission::addMover(MoverInitData* moveSpec, LogisticsMech* mechData)
{
	// OK, whole new ballgame here.
	// First, convert Heidi data to MoverInitData.
	// Then, create Mech using this data.
	// Then, Replace components to match logisticsMech Components.
	// Then, it should be good to go.
	MoverInitData mData;
	memset(&mData, 0, sizeof(MoverInitData));
	strncpy(mData.pilotFileName, mechData->getPilot()->getFileName(), 49);
	strcpy(mData.brainFileName, "pbrain");
	strncpy(mData.csvFileName, mechData->getFileName(), 50);
	mData.rosterIndex		  = 255;
	mData.objNumber			  = mechData->getFitID();
	mData.controlType		  = moveSpec->controlType;
	mData.controlDataType	 = moveSpec->controlDataType;
	mData.variant			  = moveSpec->variant;
	mData.position.x		  = moveSpec->position.x;
	mData.position.y		  = moveSpec->position.y;
	mData.position.z		  = moveSpec->position.z;
	mData.rotation			  = moveSpec->rotation;
	mData.teamID			  = moveSpec->teamID;
	mData.commanderID		  = moveSpec->commanderID;
	mData.baseColor			  = moveSpec->baseColor;
	mData.highlightColor1	 = moveSpec->highlightColor1;
	mData.highlightColor2	 = moveSpec->highlightColor2;
	mData.gestureID			  = 2;
	mData.active			  = true;
	mData.exists			  = true;
	mData.icon				  = 0;
	mData.capturable		  = false;
	mData.overrideLoadedPilot = moveSpec->overrideLoadedPilot;
	mData.gunnerySkill		  = moveSpec->gunnerySkill;
	mData.pilotingSkill		  = moveSpec->pilotingSkill;
	memcpy(mData.specialtySkills, moveSpec->specialtySkills,
		sizeof(bool) * NUM_SPECIALTY_SKILLS);
	int32_t moverHandle = addMover(&mData);
	// Now take the moverHandle and change the component data to match Heidi's
	// passed logisticsMech.
	MoverPtr mMech = (MoverPtr)ObjectManager->get(moverHandle);
	if (mMech->getObjectClass() != BATTLEMECH)
		STOP(("LogisticsMech was not a MECH!!"));
	strcpy(((BattleMech*)mMech)->variantName, mechData->getName());
	int32_t totalComponents = mechData->getComponentCount();
	int32_t* componentList  = nullptr;
	if (totalComponents)
	{
		componentList =
			(int32_t*)systemHeap->Malloc(sizeof(int32_t) * totalComponents);
		int32_t otherCount = totalComponents;
		mechData->getComponents(otherCount, componentList);
		if (otherCount != totalComponents)
			STOP(("Heidi's getComponentCount does not agree with count "
				  "returned from getComponents"));
	}
	((BattleMechPtr)mMech)->resetComponents(totalComponents, componentList);
	return moverHandle;
}
//---------------------------------------------------------------------------
void DEBUGWINS_removeGameObject(GameObjectPtr obj);
int32_t Mission::removeMover(MoverPtr mover)
{
	DEBUGWINS_removeGameObject((GameObjectPtr)mover);
	missionInterface->removeMover(mover);
	if (MPlayer)
	{
		MPlayer->removeFromLocalMovers(mover);
		MPlayer->removeFromMoverRoster(mover);
	}
	ObjectManager->freeMover(mover);
	return (0);
}
//---------------------------------------------------------------------------
void Mission::tradeMover(MoverPtr mover, int32_t newTeamID,
	int32_t newCommanderID, PSTR pilotFileName, PSTR brainFileName)
{
	missionInterface->removeMover(mover);
	if (MPlayer)
	{
		MPlayer->removeFromLocalMovers(mover);
		MPlayer->removeFromPlayerMoverRoster(mover);
	}
	if (newCommanderID > -1)
	{
		char realPilotName[256];
		strcpy(realPilotName, &(pilotFileName[MPlayer ? 4 : 3]));
		LogisticsPilot* lPilot =
			LogisticsData::instance->getPilot(realPilotName);
		mover->loadPilot(pilotFileName, brainFileName, lPilot);
	}
	ObjectManager->tradeMover(mover, newTeamID, newCommanderID);
	if (mover->sensorSystem)
		mover->sensorSystem->broken = false;
	if (MPlayer && (newCommanderID > -1))
	{
		MPlayer->addToPlayerMoverRoster(newCommanderID, mover);
		if (newCommanderID == MPlayer->commanderID)
			MPlayer->addToLocalMovers(mover);
	}
	if (newCommanderID > -1)
	{
		missionInterface->addMover(mover);
	}
}
//----------------------------------------------------------------------------
bool Mission::calcComplexDropZones(
	PSTR missionName, char dropZoneCID[MAX_MC_PLAYERS])
{
	for (size_t p = 0; p < MAX_MC_PLAYERS; p++)
		dropZoneCID[p] = -1;
	FullPathFileName missionFileName;
	missionFileName.init(missionPath, missionName, ".fit");
	FitIniFile* missionFile = new FitIniFile;
	gosASSERT(missionFile != nullptr);
	int32_t result = missionFile->open(missionFileName);
	if (result != NO_ERROR)
		STOP(("Unable to open Mission File %s", missionFileName));
	//------------------------------------------------------------
	// First, let's see how many teams and commanders there are...
	/*int32_t maxTeamID = -1;
	int32_t maxCommanderID = -1;
	for (size_t i = 0; i < MAX_MC_PLAYERS; i++) {
		if (MPlayer->playerInfo[i].team > maxTeamID)
			maxTeamID = MPlayer->playerInfo[i].team;
		if (MPlayer->playerInfo[i].commanderID > maxCommanderID)
			maxCommanderID = MPlayer->playerInfo[i].commanderID;
	}
	*/
	int32_t dropZoneSetup[MAX_MC_PLAYERS] = {-1, -1, -1, -1, -1, -1, -1, -1};
	result								  = missionFile->seekBlock("Parts");
	gosASSERT(result == NO_ERROR);
	result = missionFile->readIdULong("NumParts", numParts);
	gosASSERT(result == NO_ERROR);
	if (numParts)
		for (size_t i = 1; i < int32_t(numParts + 1); i++)
		{
			char partName[12];
			sprintf(partName, "Part%d", i);
			//------------------------------------------------------------------
			// Find the object to load
			result = missionFile->seekBlock(partName);
			gosASSERT(result == NO_ERROR);
			char teamID = -1;
			result		= missionFile->readIdChar("TeamId", teamID);
			gosASSERT(result == NO_ERROR);
			if ((teamID < 0) || (teamID >= MAX_TEAMS))
				STOP(("Mission.calcComplexDropZones: bad teamID"));
			char commanderID = -1;
			result = missionFile->readIdChar("CommanderId", commanderID);
			if (result != NO_ERROR)
			{
				int32_t cID;
				result = missionFile->readIdLong("CommanderId", cID);
				gosASSERT(result == NO_ERROR);
				commanderID = (char)cID;
			}
			if ((commanderID < 0) || (commanderID >= MAX_MC_PLAYERS))
				STOP(("Mission.calcComplexDropZones: bad commanderID"));
			dropZoneSetup[commanderID] = teamID;
		}
	//------------------------------------------------------------------------------------
	// First, let's confirm that we have the correct player/team setup for this
	// mission...
	int32_t teamSize[MAX_TEAMS][2];
	for (size_t i = 0; i < MAX_TEAMS; i++)
	{
		teamSize[i][0] = 0;
		teamSize[i][1] = 0;
	}
	/*	for (i = 0; i < MAX_MC_PLAYERS; i++)
			if (dropZoneSetup[i] > -1)
				teamSize[dropZoneSetup[i]][0]++;
		for (i = 0; i < MAX_MC_PLAYERS; i++)
			if (MPlayer->playerInfo[i].commanderID > -1)
				teamSize[MPlayer->playerInfo[i].team][1]++;
		for (i = 0; i < MAX_MC_PLAYERS; i++)
			if (teamSize[i][0] != teamSize[i][1])
				return(false);
	*/
	//---------------------------------------------------------
	// We know everything's good, so hand out the drop zones...
	for (i = 0; i < MAX_MC_PLAYERS; i++)
	{
		int32_t index =
			RandomNumber(teamSize[MPlayer->playerInfo[i].team][0]--);
		for (size_t j = 0; j < MAX_MC_PLAYERS; j++)
			if (dropZoneSetup[j] == MPlayer->playerInfo[i].team)
			{
				if (index == 0)
				{
					dropZoneSetup[j] = -1;
					dropZoneCID[j]   = i;
					break;
				}
				index--;
			}
	}
	missionFile->close();
	delete missionFile;
	missionFile = nullptr;
	return (true);
}
//----------------------------------------------------------------------------
bool IsGateDisabled(int32_t objectWID)
{
	//--------------------------------------------------------------
	// Actually looks at the object and its parent, if it has one...
	if (objectWID < 1)
		return (false);
	GameObjectPtr obj = ObjectManager->getByWatchID(objectWID);
	if (obj && obj->isDisabled())
		return (true);
	if (obj->isBuilding())
	{
		GameObjectPtr parent = ((BuildingPtr)obj)->getParent();
		if (parent)
			return (parent->isDisabled());
	}
	return (false);
}
//----------------------------------------------------------------------------
bool IsGateOpen(int32_t objectWID)
{
	//--------------------------------------------------------------
	// Actually looks at the object and its parent, if it has one...
	if (!objectWID)
		return (false);
	GameObjectPtr obj = ObjectManager->getByWatchID(objectWID);
	if (!obj)
		return (false);
	if (obj->getObjectClass() == GATE)
		return (((Gate*)obj)->opened);
	return (false);
}
//----------------------------------------------------------------------------
void Mission::init(PSTR missionName, int32_t loadType, int32_t dropZoneID,
	Stuff::Vector3D* dropZoneList, char commandersToLoad[8][3],
	int32_t numMoversPerCommander)
{
	neverEndingStory  = false;
	invulnerableON	= false;
	terminationResult = mis_PLAYING;
	// Start finding the Leaks
	// systemHeap->startHeapMallocLog();
	// systemHeap->dumpRecordLog();
	loadProgress = 0.0f;
	loadProgress = 1.0f;
	if ((loadType == MISSION_LOAD_SP_QUICKSTART) ||
		(loadType == MISSION_LOAD_SP_LOGISTICS))
	{
		char teamRelationsForSP[MAX_TEAMS][MAX_TEAMS] = {
			{0, 2, 1, 2, 2, 2, 2, 2}, {2, 0, 2, 2, 2, 2, 2, 2},
			{1, 2, 0, 2, 2, 2, 2, 2}, {2, 2, 2, 0, 2, 2, 2, 2},
			{2, 2, 2, 2, 0, 2, 2, 2}, {2, 2, 2, 2, 2, 0, 2, 2},
			{2, 2, 2, 2, 2, 2, 0, 2}, {2, 2, 2, 2, 2, 2, 2, 0}};
		for (size_t i = 0; i < MAX_TEAMS; i++)
			for (size_t j = 0; j < MAX_TEAMS; j++)
			{
				Team::relations[i][j] = teamRelationsForSP[i][j];
				TeamRelations[i][j]   = teamRelationsForSP[i][j];
			}
	}
	else
	{
		char teamRelationsForMP[MAX_TEAMS][MAX_TEAMS] = {
			{0, 2, 2, 2, 2, 2, 2, 2}, {2, 0, 2, 2, 2, 2, 2, 2},
			{2, 2, 0, 2, 2, 2, 2, 2}, {2, 2, 2, 0, 2, 2, 2, 2},
			{2, 2, 2, 2, 0, 2, 2, 2}, {2, 2, 2, 2, 2, 0, 2, 2},
			{2, 2, 2, 2, 2, 2, 0, 2}, {2, 2, 2, 2, 2, 2, 2, 0}};
		for (size_t i = 0; i < MAX_TEAMS; i++)
			for (size_t j = 0; j < MAX_TEAMS; j++)
			{
				Team::relations[i][j] = teamRelationsForMP[i][j];
				TeamRelations[i][j]   = teamRelationsForMP[i][j];
			}
	}
	//-------------------------------------------
	// Always reset turn at scenario start
	turn					  = 0;
	terminationCounterStarted = 0;
#ifdef LAB_ONLY
	x = GetCycles();
#endif
	//-----------------------
	// Init the ABL system...
	initABL();
#ifdef LAB_ONLY
	x1			  = GetCycles();
	MCTimeABLLoad = x1 - x;
#endif
	initBareMinimum();
	loadProgress = 4.0f;
	initTGLForMission();
	//--------------------------------------------------------------
	// Start the Mission Heap
	missionHeap = new UserHeap;
	gosASSERT(missionHeap != nullptr);
	missionHeap->init(missionHeapSize, "MISSION");
	//--------------------------
	// Load Game System stuff...
	FullPathFileName fullGameSystemName;
	fullGameSystemName.init(missionPath, "gamesys", ".fit");
	FitIniFile* gameSystemFile = new FitIniFile;
	gosASSERT(gameSystemFile != nullptr);
	int32_t result = gameSystemFile->open(fullGameSystemName);
	gosASSERT(result == NO_ERROR);
	result = gameSystemFile->seekBlock("WeaponRanges");
	gosASSERT(result == NO_ERROR);
	float span[2];
	result = gameSystemFile->readIdFloatArray("Short", span, 2);
	gosASSERT(result == NO_ERROR);
	WeaponRanges[WEAPON_RANGE_SHORT][0] = span[0];
	WeaponRanges[WEAPON_RANGE_SHORT][1] = span[1];
	result = gameSystemFile->readIdFloatArray("Medium", span, 2);
	gosASSERT(result == NO_ERROR);
	WeaponRanges[WEAPON_RANGE_MEDIUM][0] = span[0];
	WeaponRanges[WEAPON_RANGE_MEDIUM][1] = span[1];
	result = gameSystemFile->readIdFloatArray("Long", span, 2);
	gosASSERT(result == NO_ERROR);
	WeaponRanges[WEAPON_RANGE_LONG][0] = span[0];
	WeaponRanges[WEAPON_RANGE_LONG][1] = span[1];
	result							   = gameSystemFile->readIdFloatArray(
		"OptimalRangePoints", OptimalRangePoints, 5);
	gosASSERT(result == NO_ERROR);
	for (size_t i = 0; i < 5; i++)
		for (size_t j = 0; j < 3; j++)
		{
			OptimalRangePointInRange[i][j] = false;
			if (OptimalRangePoints[i] > WeaponRanges[j][0])
				if (OptimalRangePoints[i] <= WeaponRanges[j][1])
					OptimalRangePointInRange[i][j] = true;
		}
	result = gameSystemFile->seekBlock("General");
	gosASSERT(result == NO_ERROR);
	result = gameSystemFile->readIdFloat("MaxVisualRange", maxVisualRange);
	gosASSERT(result == NO_ERROR);
	MaxVisualRadius = maxVisualRange * 1.4142;
	result = gameSystemFile->readIdFloat("FireVisualRange", fireVisualRange);
	gosASSERT(result == NO_ERROR);
	result =
		gameSystemFile->readIdULong("MaxTreeLOSBlock", MaxTreeLOSCellBlock);
	if (result != NO_ERROR)
		MaxTreeLOSCellBlock = 5;
	result = gameSystemFile->readIdFloatArray(
		"WeaponRange", WeaponRange, NUM_FIRERANGES);
	gosASSERT(result == NO_ERROR);
	result =
		gameSystemFile->readIdFloat("DefaultAttackRange", DefaultAttackRange);
	if (result != NO_ERROR)
		DefaultAttackRange = 75.0;
	result = gameSystemFile->readIdFloat("BaseSensorRange", baseSensorRange);
	gosASSERT(result == NO_ERROR);
	result = gameSystemFile->readIdLongArray(
		"VisualRangeTable", visualRangeTable, 256);
	gosASSERT(result == NO_ERROR);
	result = gameSystemFile->readIdFloat(
		"BaseHeadShotElevation", BaseHeadShotElevation);
	if (result != NO_ERROR)
		BaseHeadShotElevation = 1.0f;
	int32_t forestMoveCost;
	result = gameSystemFile->readIdLong("ForestMoveCost", forestMoveCost);
	gosASSERT(result == NO_ERROR);
	result = gameSystemFile->readIdFloat(
		"MaxUnitExtractDistance", MaxExtractUnitDistance);
	if (result != NO_ERROR)
		MaxExtractUnitDistance = 1280.0f; // Ten Tiles away
	//----------------------------------------------------------------------
	// Now that we have some base values, load the master component table...
	if (!MasterComponent::masterList)
	{
		FullPathFileName compFileName;
		compFileName.init(objectPath, "compbas", ".csv");
#ifdef _DEBUG
		int32_t loadErr =
#endif
			MasterComponent::loadMasterList(compFileName, 255, baseSensorRange);
		gosASSERT(loadErr == NO_ERROR);
	}
	result = gameSystemFile->readIdUChar("GodMode", godMode);
	if (result != NO_ERROR)
		godMode = 0;
	uint8_t revealTacMap;
	result = gameSystemFile->readIdUChar("RevealTacMap", revealTacMap);
	if (result != NO_ERROR)
		revealTacMap = 0;
	result = gameSystemFile->readIdUChar("FootPrints", footPrints);
	if (result != NO_ERROR)
		footPrints = 1;
	result = gameSystemFile->readIdLong("BonusTonnageDivisor", tonnageDivisor);
	gosASSERT(result == NO_ERROR);
	result =
		gameSystemFile->readIdLong("BonusPointsPerTon", resourcesPerTonDivided);
	gosASSERT(result == NO_ERROR);
#ifndef FINAL
	result = gameSystemFile->readIdFloat("CheatHitDamage", CheatHitDamage);
	if (result != NO_ERROR)
		CheatHitDamage = 5.0f;
#endif
	//---------------------------------------
	// Read in difficulty here if it exits.
	InitDifficultySettings(gameSystemFile);
	result = Mover::loadGameSystem(gameSystemFile, maxVisualRange);
	gosASSERT(result == NO_ERROR);
	// result = loadMultiplayerGameSystem(gameSystemFile);
	// gosASSERT(result == NO_ERROR);
	result = BattleMech::loadGameSystem(gameSystemFile);
	gosASSERT(result == NO_ERROR);
	//--------------------------------------------------------------------
	result = GroundVehicle::loadGameSystem(gameSystemFile);
	gosASSERT(result == NO_ERROR);
#ifdef USE_ELEMENTALS
	result = loadElementalGameSystem(gameSystemFile);
	gosASSERT(result == NO_ERROR);
#endif
	result = gameSystemFile->seekBlock("Mine");
	gosASSERT(result == NO_ERROR);
	result = gameSystemFile->readIdFloat("BaseDamage", MineDamage);
	gosASSERT(result == NO_ERROR);
	result = gameSystemFile->readIdFloat("SplashDamage", MineSplashDamage);
	gosASSERT(result == NO_ERROR);
	result = gameSystemFile->readIdFloat("SplashRange", MineSplashRange);
	gosASSERT(result == NO_ERROR);
	result = gameSystemFile->readIdLong("Explosion", MineExplosion);
	gosASSERT(result == NO_ERROR);
	result = gameSystemFile->readIdLong("MineLayThrottle", MineLayThrottle);
	if (result != NO_ERROR)
		MineLayThrottle = 50;
	result = gameSystemFile->readIdLong("MineSweepThrottle", MineSweepThrottle);
	if (result != NO_ERROR)
		MineSweepThrottle = 50;
	result = gameSystemFile->readIdFloat("MineWaitTime", MineWaitTime);
	gosASSERT(result == NO_ERROR);
	result = gameSystemFile->readIdFloat("StrikeWaitTime", StrikeWaitTime);
	gosASSERT(result == NO_ERROR);
	result =
		gameSystemFile->readIdFloat("StrikeTimeToImpact", StrikeTimeToImpact);
	gosASSERT(result == NO_ERROR);
	result = gameSystemFile->seekBlock("Smoke");
	gosASSERT(result == NO_ERROR);
	result = gameSystemFile->readIdLong("MaxSmokeSpheres", totalSmokeSpheres);
	gosASSERT(result == NO_ERROR);
	result =
		gameSystemFile->readIdLong("TotalSmokeShapeSize", totalSmokeShapeSize);
	gosASSERT(result == NO_ERROR);
	result = gameSystemFile->seekBlock("Fire");
	gosASSERT(result == NO_ERROR);
	result = gameSystemFile->readIdLong("MaxFiresBurning", maxFiresBurning);
	gosASSERT(result == NO_ERROR);
	result = gameSystemFile->readIdFloat("MaxFireBurnTime", maxFireBurnTime);
	gosASSERT(result == NO_ERROR);
	memset(missionFileName, 0, 80);
	strncpy(missionFileName, missionName, 79);
	FullPathFileName missionFileName;
	missionFileName.init(missionPath, missionName, ".fit");
	duration	= 60;
	missionFile = new FitIniFile;
	gosASSERT(missionFile != nullptr);
	result = missionFile->open(missionFileName);
	if (result != NO_ERROR)
		STOP(("Unable to open Mission File %s", missionFileName));
	if (!dropZoneList)
	{
		result = missionFile->seekBlock("Multiplayer");
		if (result == NO_ERROR)
		{
#if 0
				result = missionFile->readIdULong("TeamId", MultiPlayTeamId);
				gosASSERT(result == NO_ERROR);
				result = missionFile->readIdULong("CommanderId", MultiPlayCommanderId);
				gosASSERT(result == NO_ERROR);
				char sessionName[128];
				result = missionFile->readIdString("SessionName", sessionName, 127);
				gosASSERT(result == NO_ERROR);
				char playerName[128];
				result = missionFile->readIdString("PlayerName", playerName, 127);
				gosASSERT(result == NO_ERROR);
				bool isServer = false;
				result = missionFile->readIdBoolean("Server", isServer);
				gosASSERT(result == NO_ERROR);
				uint32_t numPlayers;
				result = missionFile->readIdULong("NumPlayers", numPlayers);
				gosASSERT(result == NO_ERROR);
				gosASSERT(MPlayer == nullptr);
				MPlayer = new MultiPlayer;
				Assert(MPlayer != nullptr, 0, " Unable to create MultiPlayer object ");
				MPlayer->setup();
				MPlayer->commanderID = MultiPlayCommanderId;
				//-------------------------------------------
				// If I'm the server, then create the game...
				if(isServer)
				{
					if(MPlayer->hostGame(sessionName, playerName, numPlayers))
					{
						//---------------------------------------------
						//game hosted, so now wait for all check-ins...
						MPlayer->serverCID = MultiPlayCommanderId;//(MultiPlayCommanderId == ServerPlayerNum); //(gos_NetInformation(gos_AmITheServer) == 0);
					}
				}
				else
				{
					MPlayer->joinGame(nullptr, sessionName, playerName);
					//MPlayer->numFitPlayers = numPlayers;
				}
#endif
		}
	}
#ifdef LAB_ONLY
	x					 = GetCycles();
	MCTimeMiscToTeamLoad = x - x1;
#endif
	//-----------------------------------
	// Find the SKY Number and save it.
	// If no number, i.e. an old mission file,
	// simply make it 1 until its written out
	// in the magical editor (tm)
	result = missionFile->seekBlock("TheSky");
	if (result != NO_ERROR)
		theSkyNumber = DEFAULT_SKY;
	else
	{
		result = missionFile->readIdLong("SkyNumber", theSkyNumber);
		if (result != NO_ERROR)
			theSkyNumber = DEFAULT_SKY;
		if ((theSkyNumber < 1) || (theSkyNumber > 21))
			theSkyNumber = DEFAULT_SKY;
	}
	//-----------------------------------------
	// Begin Setting up Teams and Commanders...
	loadProgress = 10.0f;
	result		 = missionFile->seekBlock("Teams");
	Assert(result == NO_ERROR, result, " Could not find Teams Block ");
	for (i = 0; i < Team::numTeams; i++)
		if (Team::teams[i])
		{
			delete Team::teams[i];
			Team::teams[i] = nullptr;
		}
	Team::numTeams = 0;
	for (i = 0; i < Commander::numCommanders; i++)
		if (Commander::commanders[i])
		{
			delete Commander::commanders[i];
			Commander::commanders[i] = nullptr;
		}
	Commander::numCommanders = 0;
	//------------------------------------------------------------
	// First, let's see how many teams and commanders there are...
	int32_t maxTeamID	  = -1;
	int32_t maxCommanderID = -1;
	if (loadType == MISSION_LOAD_MP_LOGISTICS)
	{
		for (size_t i = 0; i < MAX_MC_PLAYERS; i++)
		{
			if (MPlayer->playerInfo[i].team > maxTeamID)
				maxTeamID = MPlayer->playerInfo[i].team;
			if (MPlayer->playerInfo[i].commanderID > maxCommanderID)
				maxCommanderID = MPlayer->playerInfo[i].commanderID;
		}
	}
	else
	{
		result = missionFile->seekBlock("Parts");
		gosASSERT(result == NO_ERROR);
		result = missionFile->readIdULong("NumParts", numParts);
		gosASSERT(result == NO_ERROR);
		if (numParts)
			for (i = 1; i < int32_t(numParts + 1); i++)
			{
				char partName[12];
				sprintf(partName, "Part%d", i);
				//------------------------------------------------------------------
				// Find the object to load
				result = missionFile->seekBlock(partName);
				gosASSERT(result == NO_ERROR);
				char teamID = -1;
				result		= missionFile->readIdChar("TeamId", teamID);
				gosASSERT(result == NO_ERROR);
				char commanderID = -1;
				result = missionFile->readIdChar("CommanderId", commanderID);
				if (result != NO_ERROR)
				{
					int32_t cID;
					result = missionFile->readIdLong("CommanderId", cID);
					gosASSERT(result == NO_ERROR);
					commanderID = (char)cID;
				}
				if (MPlayer && dropZoneList)
				{
					//-------------------------------------------------------------
					// Since dropZoneList is not nullptr, we know this was not
					// started from the command-line...
					int32_t origCommanderID = commanderID;
					commanderID = commandersToLoad[origCommanderID][0];
					teamID		= commandersToLoad[origCommanderID][1];
				}
				if (commanderID > maxCommanderID)
					maxCommanderID = commanderID;
				if (teamID > maxTeamID)
					maxTeamID = teamID;
			}
	}
	//----------------------------------------------
	// Now, init the teams and commanders we need...
	for (i = 0; i <= maxTeamID; i++)
	{
		Team::teams[i] = new Team;
		Team::teams[i]->init(i);
	}
	for (i = 0; i <= maxCommanderID; i++)
	{
		Commander::commanders[i] = new Commander;
		Commander::commanders[i]->setId(i);
	}
	if (MPlayer)
	{
		Team::home		= Team::teams[MultiPlayTeamId];
		Commander::home = Commander::commanders[MultiPlayCommanderId];
		for (size_t i = 0; i <= maxCommanderID; i++)
			if (MPlayer->playerInfo[i].commanderID > -1)
				Commander::commanders[MPlayer->playerInfo[i].commanderID]
					->setTeam(Team::teams[MPlayer->playerInfo[i].team]);
	}
	else
	{
		Team::home		= Team::teams[0];
		Commander::home = Commander::commanders[0];
		for (size_t i = 0; i <= maxCommanderID; i++)
		{
			if (commandersToLoad[i][0] > -1)
				Commander::commanders[commandersToLoad[i][0]]->setTeam(
					Team::teams[commandersToLoad[i][1]]);
		}
		Commander::commanders[0]->setTeam(Team::home);
	}
	//-----------------------------
	// Init Trigger Area Manager...
	if (Mover::triggerAreaMgr)
	{
		delete Mover::triggerAreaMgr;
		Mover::triggerAreaMgr = nullptr;
	}
	Mover::triggerAreaMgr = new TriggerAreaManager;
	Assert(Mover::triggerAreaMgr != nullptr, 0,
		" Mossion.init: unable to init triggerAreaMgr ");
	//-----------------------------------
	// Setup the Sensor System Manager...
	SensorManager = new SensorSystemManager;
	Assert(
		SensorManager != nullptr, 0, " Unable to init sensor system manager ");
	result = SensorManager->init(true);
	Assert(
		result == NO_ERROR, result, " could not start Sensor System Manager ");
	result = missionFile->seekBlock("DropZone0");
	if (result == NO_ERROR) // lets not enforce drop zones for now
	{
		missionFile->readIdFloat("PositionX", dropZone.x);
		missionFile->readIdFloat("PositionY", dropZone.y);
	}
	else
	{
		dropZone.x = -1.f;
		dropZone.y = -1.f;
	}
#ifdef LAB_ONLY
	x1			   = GetCycles();
	MCTimeTeamLoad = x1 - x;
#endif
	//-----------------------------------------------------------------
	// Load the names of the scenario tunes.
	// result = missionFile->seekBlock("Music");
	result = missionFile->seekBlock("MissionSettings");
	gosASSERT(result == NO_ERROR);
	result = missionFile->readIdUChar("scenarioTuneNum", missionTuneNum);
	gosASSERT(result == NO_ERROR);
	int32_t numRPoints;
	result = missionFile->readIdLong("ResourcePoints", numRPoints);
	if (MaxResourcePoints > -1)
		numRPoints = MaxResourcePoints;
	if (MPlayer)
	{
		numRPoints = MPlayer->missionSettings.resourcePoints;
		for (size_t i = 0; i < MAX_MC_PLAYERS; i++)
		{
			MPlayer->playerInfo[i].resourcePoints =
				MPlayer->missionSettings.resourcePoints;
			MPlayer->playerInfo[i].resourcePointsGained = 0;
			MPlayer->playerInfo[i].resourcePointsAtStart =
				MPlayer->missionSettings.resourcePoints;
		}
	}
	LogisticsData::instance->setResourcePoints(numRPoints);
	if (MPlayer)
	{
		result = missionFile->readIdULong(
			"NumRandomRPbuildings", MPlayer->numRandomResourceBuildings);
	}
	craterManager =
		(CraterManagerPtr)missionHeap->Malloc(sizeof(CraterManager));
	gosASSERT(craterManager != nullptr);
	result = craterManager->init(1000, 20479, "feet");
	gosASSERT(result == NO_ERROR);
	//-----------------------------------------------------------------
	// Start the object system next.
	ObjectManager = new GameObjectManager;
	if (!ObjectManager)
		Fatal(0, " Mission.init: unable to create ObjectManager ");
	ObjectManager->init("object2", 716799, 3072000);
	gosASSERT(result == NO_ERROR);
	//-----------------------------------------------------------------
	// Start the collision detection system. -- Doesn't need objects?
	ObjectManager->initCollisionSystem(missionFile);
	//------------------------------------------------------------
	// Start the Terrain System
	FullPathFileName terrainFileName;
	terrainFileName.init(missionPath, missionName, ".pak");
	PacketFile pakFile;
	result = pakFile.open(terrainFileName);
	gosASSERT(result == NO_ERROR);
	land = new Terrain;
	land->getColorMapName(missionFile);
	gosASSERT(land != nullptr);
	loadProgress = 15.0f;
#ifdef LAB_ONLY
	x				 = GetCycles();
	MCTimeObjectLoad = x - x1;
#endif
	int32_t terrainInitResult =
		land->init(&pakFile, 0, GameVisibleVertices, loadProgress, 20.0);
	if (terrainInitResult != NO_ERROR)
	{
		STOP(("Could not load terrain.  Probably size was wrong!"));
	}
	loadProgress = 35.0f;
#ifdef LAB_ONLY
	x1				  = GetCycles();
	MCTimeTerrainLoad = x1 - x;
#endif
	land->load(missionFile);
	loadProgress = 36.0f;
	//	land->recalcWater();		//Should have already been done in the
	//editor
	//----------------------------------------------------
	// Start GameMap for Movement System
	Assert(SimpleMovePathRange > 20, SimpleMovePathRange,
		" Simple MovePath Range too small ");
	MOVE_init(SimpleMovePathRange);
	if (pakFile.seekPacket(4) == NO_ERROR)
	{
		if (pakFile.getPacketSize() != 0)
		{
			MOVE_readData(&pakFile, 4);
			if (GlobalMoveMap[0]->badLoad)
				Fatal(0,
					" Mission.init: old version of move data (re-save map) ");
			GameMap->placeMoversCallback			 = PlaceMovers;
			GlobalMoveMap[0]->isGateDisabledCallback = IsGateDisabled;
			GlobalMoveMap[1]->isGateDisabledCallback = IsGateDisabled;
			GlobalMoveMap[2]->isGateDisabledCallback = IsGateDisabled;
			GlobalMoveMap[0]->isGateOpenCallback	 = IsGateOpen;
			GlobalMoveMap[1]->isGateOpenCallback	 = IsGateOpen;
			GlobalMoveMap[2]->isGateOpenCallback	 = IsGateOpen;
		}
		else
			STOP(("Mission has not movement Data.  QuickSaved Map?"));
	}
	PathFindMap[SECTOR_PATHMAP]->blockedDoorCallback = GetBlockedDoorCells;
	PathFindMap[SECTOR_PATHMAP]->placeStationaryMoversCallback =
		PlaceStationaryMovers;
	PathFindMap[SIMPLE_PATHMAP]->placeStationaryMoversCallback =
		PlaceStationaryMovers;
	PathFindMap[SECTOR_PATHMAP]->forestCost = forestMoveCost;
	PathFindMap[SIMPLE_PATHMAP]->forestCost = forestMoveCost;
	PathManager								= new MovePathManager;
	loadProgress							= 40.0f;
#ifdef LAB_ONLY
	x			   = GetCycles();
	MCTimeMoveLoad = x - x1;
#endif
	//----------------------
	// Load ABL Libraries...
	int32_t numErrors, numLinesProcessed;
	FullPathFileName libraryFileName;
	libraryFileName.init(missionPath, "orders", ".abx");
	ABLModulePtr library =
		ABLi_loadLibrary(libraryFileName, &numErrors, &numLinesProcessed);
	gosASSERT(library != nullptr);
	FullPathFileName libraryFileName1;
	libraryFileName1.init(missionPath, "miscfunc", ".abx");
	library =
		ABLi_loadLibrary(libraryFileName1, &numErrors, &numLinesProcessed);
	gosASSERT(library != nullptr);
	FullPathFileName libraryFileName2;
	libraryFileName2.init(missionPath, "corebrain", ".abx");
	library =
		ABLi_loadLibrary(libraryFileName2, &numErrors, &numLinesProcessed);
	gosASSERT(library != nullptr);
	//---------------------------
	// Load the mission script...
	//-----------------------------------------------------------------
	// We now read in the mission Script File Name
	result = missionFile->seekBlock("Script");
	gosASSERT(result == NO_ERROR);
	result = missionFile->readIdString("ScenarioScript", missionScriptName, 79);
	gosASSERT(result == NO_ERROR);
	FullPathFileName brainFileName;
	brainFileName.init(missionPath, missionScriptName, ".abl");
	missionScriptHandle =
		ABLi_preProcess(brainFileName, &numErrors, &numLinesProcessed);
	gosASSERT(missionScriptHandle >= 0);
	missionBrain = new ABLModule;
	gosASSERT(missionBrain != nullptr);
#ifdef _DEBUG
	int32_t brainErr =
#endif
		missionBrain->init(missionScriptHandle);
	gosASSERT(brainErr == NO_ERROR);
	missionBrain->setName("Mission");
	// MissionBrain->setStep(TRUE);
	missionBrainParams = new ABLParam;
	gosASSERT(missionBrainParams != nullptr);
	missionBrainCallback = missionBrain->findFunction("handlemessage", TRUE);
#ifdef LAB_ONLY
	x1					 = GetCycles();
	MCTimeMissionABLLoad = x1 - x;
#endif
	loadProgress = 41.0f;
	//-------------------------------------------
	// Load all MechWarriors for this mission...
	MechWarrior::setup();
	result = missionFile->seekBlock("Warriors");
	gosASSERT(result == NO_ERROR);
	uint32_t numWarriors;
	result = missionFile->readIdULong("NumWarriors", numWarriors);
	gosASSERT(result == NO_ERROR);
	bool loadBrainParameters = (result == NO_ERROR);
	if (numWarriors)
	{
		for (size_t i = 1; i <= numWarriors; i++)
		{
			char warriorName[12];
			sprintf(warriorName, "Warrior%d", i);
			//-------------------------
			// Find the warrior to load
			result = missionFile->seekBlock(warriorName);
			Assert(
				result == NO_ERROR, i, " Could not find Warrior Number Block ");
			char warriorFile[100];
			result = missionFile->readIdString("Profile", warriorFile, 99);
			Assert(result == NO_ERROR, 0,
				" Could not find Warrior Profile in Warrior Number Block ");
			MechWarriorPtr pilot = MechWarrior::newWarrior();
			if (!pilot)
				STOP(("Too many pilots in this mission!"));
			//--------------------------------------
			// Load the mechwarrior into the mech...
			FullPathFileName pilotFullFileName;
			pilotFullFileName.init(warriorPath, warriorFile, ".fit");
			FitIniFile* pilotFile = new FitIniFile;
			gosASSERT(pilotFile != nullptr);
			int32_t result = pilotFile->open(pilotFullFileName);
			gosASSERT(result == NO_ERROR);
			result = pilot->init(pilotFile);
			gosASSERT(result == NO_ERROR);
			pilotFile->close();
			delete pilotFile;
			pilotFile = nullptr;
			//----------------------------
			// Read in the Brain module...
			char moduleName[128];
			result = missionFile->readIdString("Brain", moduleName, 127);
			gosASSERT(result == NO_ERROR);
			//------------------------------------------------------------
			// For now, all mplayer brains are pbrain. Need to change when
			// we allow ai brains in mplayer...
			int32_t numErrors, numLinesProcessed;
			FullPathFileName brainFileName;
			if (MPlayer)
			{
				pilot->setBrainName("pbrain");
				brainFileName.init(warriorPath, "pbrain", ".abl");
			}
			else
			{
				pilot->setBrainName(moduleName);
				brainFileName.init(warriorPath, moduleName, ".abl");
			}
			int32_t moduleHandle =
				ABLi_preProcess(brainFileName, &numErrors, &numLinesProcessed);
			gosASSERT(moduleHandle >= 0);
#ifdef _DEBUG
			int32_t error =
#endif
				pilot->setBrain(moduleHandle);
			gosASSERT(error == 0);
		}
	}
	if (loadBrainParameters)
	{
		//---------------------------------------------------------------
		// Load the brain parameter file and load 'em for each warrior...
		for (uint32_t i = 1; i <= numWarriors; i++)
		{
			result = MechWarrior::warriorList[i]->loadBrainParameters(
				missionFile, i);
			// Assert(result == NO_ERROR, result, " Could not load Warrior Brain
			// Parameters ");
		}
	}
#ifdef LAB_ONLY
	x				  = GetCycles();
	MCTimeWarriorLoad = x - x1;
#endif
	loadProgress = 43.0f;
	//-----------------------------------------------------------------
	// All systems are GO if we reach this point.  Now we need to
	// parse the scenario file for the Objects we need for this scenario
	// We then create each object and place it in the world at the
	// position we read in with the frame we read in.
	result = missionFile->seekBlock("Parts");
	gosASSERT(result == NO_ERROR);
	result = missionFile->readIdULong("NumParts", numParts);
	gosASSERT(result == NO_ERROR);
	//--------------------------------------------------------------------------------
	// IMPORTANT NOTE: mission parts should always start with Part 1.
	// Part 0 is reserved as a "nullptr" id for routines that reference the
	// mission parts. AI routines, Brain keywords, etc. use PART ID 0 as an
	// "object not found" error code. DO NOT USE PART 0!!!!!!! Start with
	// Part 1...
#define MAX_SQUADS 256
#define MAX_ALTERNATIVES 15
#define USE_ALTERNATES
	loadProgress							= 43.5f;
	int32_t numMoversLoaded[MAX_MC_PLAYERS] = {0, 0, 0, 0, 0, 0, 0, 0};
	int32_t numDropZonePositions			= 0;
	if (numParts)
	{
		//-----------------------------------------------------
		// Since we leave part 0 unused, malloc numParts + 1...
		parts = (PartPtr)missionHeap->Malloc(sizeof(Part) * (numParts + 1));
		gosASSERT(parts != nullptr);
		memset(parts, 0, sizeof(Part) * (numParts + 1));
#ifdef USE_ALTERNATES
		//------------------------------------------------------------
		// Before we actually read in the parts, do some prep work for
		// determining squad alternatives...
		int32_t numSquads = 0;
		int32_t squadMap[MAX_SQUADS];
		for (size_t i = 0; i < MAX_SQUADS; i++)
			squadMap[i] = -1;
		int32_t maxAlternatives[MAX_SQUADS];
		int32_t randomAlternative[MAX_SQUADS];
		for (size_t s = 0; s < MAX_SQUADS; s++)
		{
			maxAlternatives[s]   = 0;
			randomAlternative[s] = -1;
		}
		int32_t bigAlternatives = 0;
		for (i = 1; i < int32_t(numParts + 1); i++)
		{
			char partName[12];
			sprintf(partName, "Part%d", i);
			result = missionFile->seekBlock(partName);
			gosASSERT(result == NO_ERROR);
			uint32_t squadNum;
			result			   = missionFile->readIdULong("squadNum", squadNum);
			int32_t squadIndex = 0;
			for (squadIndex = 0; squadIndex < numSquads; squadIndex++)
				if (squadMap[squadIndex] == squadNum)
					break;
			if (squadIndex == numSquads)
				squadMap[numSquads++] = squadNum;
			int32_t alternatives[MAX_ALTERNATIVES];
			result = missionFile->readIdLongArray(
				"IndicesOfAlternatives", alternatives, MAX_ALTERNATIVES);
			int32_t numAlternatives = 0;
			for (numAlternatives = 0; numAlternatives < MAX_ALTERNATIVES;
				 numAlternatives++)
				if (alternatives[numAlternatives] == -1)
					break;
			if (maxAlternatives[squadIndex] < numAlternatives)
				maxAlternatives[squadIndex] = numAlternatives;
			if (numAlternatives > bigAlternatives)
				bigAlternatives = numAlternatives;
		}
		if (MPlayer && (bigAlternatives > 0))
		{
			PAUSE(("Mission.init: multiplayer map has random squads"));
			bigAlternatives = 0;
		}
		int32_t alternateChoice = RandomNumber(bigAlternatives + 1);
		for (s = 0; s < numSquads; s++)
		{
			randomAlternative[s] = alternateChoice;
			if (GameDifficulty >= 2)
			{
				randomAlternative[s]--;
				if (randomAlternative[s] < 1)
					randomAlternative[s] = 1;
			}
		}
		//--------------------------------------------------------
		// This block is optional, and is used for testing only...
		result = missionFile->seekBlock("Squads");
		if (result == NO_ERROR)
			for (size_t i = 0; i < numSquads; i++)
			{
				char s[128];
				sprintf(s, "Squad%d", i);
				uint32_t alternate = -1;
				result			   = missionFile->readIdULong(s, alternate);
				if (result == NO_ERROR)
					randomAlternative[i] = alternate;
			}
#else
		int32_t i;
#endif
		for (i = 1; i < int32_t(numParts + 1); i++)
		{
			char partName[12];
			sprintf(partName, "Part%d", i);
			//------------------------------------------------------------------
			// Find the object to load
			result = missionFile->seekBlock(partName);
			gosASSERT(result == NO_ERROR);
#ifdef USE_ALTERNATES
			//----------------------------------------------------------------------
			// If we have alternatives, choose which one we're taking before we
			// read anything else in...
			bool usingAlternate = false;
			uint32_t realPilot  = 0;
			uint32_t squadNum;
			result			   = missionFile->readIdULong("squadNum", squadNum);
			parts[i].squadId   = squadNum;
			int32_t squadIndex = 0;
			for (squadIndex = 0; squadIndex < numSquads; squadIndex++)
				if (squadMap[squadIndex] == squadNum)
					break;
			int32_t alternatives[MAX_ALTERNATIVES];
			result = missionFile->readIdLongArray(
				"IndicesOfAlternatives", alternatives, MAX_ALTERNATIVES);
			gosASSERT(result == NO_ERROR);
			if (maxAlternatives[squadIndex])
			{
				int32_t partId = i;
				if (randomAlternative[squadIndex] > 0)
					partId = alternatives[randomAlternative[squadIndex] - 1];
				if (partId == -1)
					continue;
				Assert(partId > 0, partId, " Mission.init: Bad Alternate ");
				// MUST save off ORIGINAL Pilot.  WE don't load the alternate
				// pilots!!!!!
				usingAlternate = true;
				result		   = missionFile->readIdULong("Pilot", realPilot);
				gosASSERT(result == NO_ERROR, );
				sprintf(partName, "Part%d", partId);
				result = missionFile->seekBlock(partName);
				gosASSERT(result == NO_ERROR);
			}
#endif
			//------------------------------------------------------------------
			// Find out what kind of object this is.
			result =
				missionFile->readIdULong("ObjectNumber", parts[i].objNumber);
			gosASSERT(result == NO_ERROR);
			//-------------------------------------------------
			// Read in the data needed to control the object...
			result =
				missionFile->readIdULong("ControlType", parts[i].controlType);
			gosASSERT(result == NO_ERROR);
			result = missionFile->readIdULong(
				"ControlDataType", parts[i].controlDataType);
			gosASSERT(result == NO_ERROR);
			result = missionFile->readIdString(
				"ObjectProfile", parts[i].profileName, 9);
			gosASSERT(result == NO_ERROR);
			result =
				missionFile->readIdULong("VariantNumber", parts[i].variantNum);
			if (result != NO_ERROR)
				parts[i].variantNum = 0; // FOR NOW!!!!!!!!!!!!!!!!
			// MAKE a REAL error when Heidi fixes editor.
			//-fs 12/7/99
			if (usingAlternate)
			{
				parts[i].pilot = realPilot;
			}
			else
			{
				result = missionFile->readIdULong("Pilot", parts[i].pilot);
				gosASSERT(result == NO_ERROR, );
			}
			//------------------------------------------------------------------
			// Read the object's position, initial velocity and rotation.
			result = missionFile->readIdFloat("PositionX", parts[i].position.x);
			gosASSERT(result == NO_ERROR);
			result = missionFile->readIdFloat("PositionY", parts[i].position.y);
			gosASSERT(result == NO_ERROR);
			parts[i].position.z = -1.0;
			result = missionFile->readIdFloat("Rotation", parts[i].rotation);
			gosASSERT(result == NO_ERROR);
			result = missionFile->readIdChar("TeamId", parts[i].teamId);
			gosASSERT(result == NO_ERROR);
			//--------------------------------------------------------------------------
			// Hack for singleplayer, until editor spits this out properly for
			// allies...
			if (!MPlayer && (parts[i].teamId == 2))
				parts[i].teamId = 0;
			if (dropZoneList && (dropZoneID == parts[i].teamId))
				dropZoneList[numDropZonePositions++] = parts[i].position;
			char cmdId = 0;
			result	 = missionFile->readIdChar("CommanderId", cmdId);
			if (result != NO_ERROR)
			{
				result = missionFile->readIdLong(
					"CommanderId", parts[i].commanderID);
				gosASSERT(result == NO_ERROR);
			}
			else
			{
				parts[i].commanderID = cmdId;
			}
			if (loadType == MISSION_LOAD_MP_QUICKSTART)
			{
				int32_t origCommanderID = parts[i].commanderID;
				parts[i].commanderID	= commandersToLoad[origCommanderID][0];
				parts[i].teamId			= commandersToLoad[origCommanderID][1];
				if (commandersToLoad[origCommanderID][0] > -1)
				{
					if (numMoversLoaded[commandersToLoad[origCommanderID][0]] ==
						numMoversPerCommander)
					{
						parts[i].commanderID = -1;
						parts[i].teamId		 = -1;
					}
					else
						numMoversLoaded[commandersToLoad[origCommanderID][0]]++;
				}
			}
			parts[i].gestureId = 2; // this has never changed
			result = missionFile->readIdULong("BaseColor", parts[i].baseColor);
			if (result != NO_ERROR || MPlayer)
				parts[i].baseColor = prefs.baseColor;
			result = missionFile->readIdULong(
				"HighlightColor1", parts[i].highlightColor1);
			if (result != NO_ERROR || MPlayer)
				parts[i].highlightColor1 = prefs.highlightColor;
			result = missionFile->readIdULong(
				"HighlightColor2", parts[i].highlightColor2);
			if (result != NO_ERROR || MPlayer)
				parts[i].highlightColor2 = prefs.highlightColor;
			parts[i].velocity = 0;
			result = missionFile->readIdLong("Active", parts[i].active);
			gosASSERT(result == NO_ERROR);
			result = missionFile->readIdLong("Exists", parts[i].exists);
			gosASSERT(result == NO_ERROR);
			float fDamage = 0.0f;
			result		  = missionFile->readIdFloat("Damage", fDamage);
			if (result == NO_ERROR)
			{
				if (fDamage >= 1.0)
				{
					parts[i].destroyed = true;
				};
			}
			result = missionFile->readIdChar("MyIcon", parts[i].myIcon);
			gosASSERT(result == NO_ERROR);
			result = missionFile->readIdChar("MyIcon", parts[i].myIcon);
			gosASSERT(result == NO_ERROR);
			result =
				missionFile->readIdBoolean("Captureable", parts[i].captureable);
			if (result != NO_ERROR)
				parts[i].captureable = FALSE;
			float increment = 5.0f / (numParts + 1);
			loadProgress += increment;
		}
	}
	loadProgress = 48.5f;
	//--------------------------------------------------------------------------
	// Now that the parts data has been loaded, let's prep the ObjectManager for
	// the real things. First, count the number of objects we need...
	int32_t numMechs	= 0;
	int32_t numVehicles = 0;
	for (i = 1; i < (numParts + 1); i++)
	{
		ObjectTypePtr objType =
			ObjectManager->loadObjectType(parts[i].objNumber);
		if (!objType)
			objType = ObjectManager->getObjectType(parts[i].objNumber);
		if (objType)
			switch (objType->getObjectTypeClass())
			{
			case BATTLEMECH_TYPE:
				numMechs++;
				break;
			case VEHICLE_TYPE:
				numVehicles++;
				break;
			}
		float increment = 6.5f / (numParts + 1);
		loadProgress += increment;
	}
	switch (loadType)
	{
	case MISSION_LOAD_SP_QUICKSTART:
	case MISSION_LOAD_SP_LOGISTICS:
		break;
	case MISSION_LOAD_MP_QUICKSTART:
	case MISSION_LOAD_MP_LOGISTICS:
		numMechs	= 64;
		numVehicles = 64;
		break;
	}
	loadProgress = 55.0f;
	pakFile.seekPacket(1);
	ObjectManager->countTerrainObjects(&pakFile,
		(numMechs + MAX_TEAMS * MAX_REINFORCEMENTS_PER_TEAM) +
			(numVehicles +
				MAX_TEAMS *
					MAX_REINFORCEMENTS_PER_TEAM) /* + ObjectManager->maxElementals*/
			+ 1);
	loadProgress = 58.0f;
	ObjectManager->setNumObjects(
		numMechs, numVehicles, 0, -1, -1, -1, 100, 50, 0, 130, -1);
	//-------------------------
	// Load the mech objects...
	int32_t curMech	= 0;
	int32_t curVehicle = 0;
	for (size_t t = 0; t < 8; t++)
		for (size_t i = 1; i < (numParts + 1); i++)
		{
			bool loadEm = true;
			if (loadType == MISSION_LOAD_MP_LOGISTICS)
				loadEm = false;
			if (loadType == MISSION_LOAD_SP_LOGISTICS)
				if (parts[i].commanderID == 0 && !parts[i].destroyed)
					loadEm = false;
			if (loadType == MISSION_LOAD_MP_QUICKSTART)
				if (parts[i].commanderID == -1)
					loadEm = false;
			if (loadEm)
			{
				ObjectTypePtr objType =
					ObjectManager->getObjectType(parts[i].objNumber);
				if (objType)
					switch (objType->getObjectTypeClass())
					{
					case BATTLEMECH_TYPE:
						if (parts[i].teamId == t)
						{
							BattleMechPtr mech =
								ObjectManager->getMech(curMech++);
							createPartObject(i, mech);
							float increment = 10.0f / (numParts + 1);
							loadProgress += increment;
						}
						break;
					case VEHICLE_TYPE:
						if (parts[i].teamId == t)
						{
							GroundVehiclePtr vehicle =
								ObjectManager->getVehicle(curVehicle++);
							createPartObject(i, vehicle);
							float increment = 10.0f / (numParts + 1);
							loadProgress += increment;
						}
						break;
					}
			}
			else
			{
				MechWarrior::freeWarrior(
					MechWarrior::warriorList[parts[i].pilot]);
			}
		}
#ifdef LAB_ONLY
	x1					 = GetCycles();
	MCTimeMoverPartsLoad = x1 - x;
#endif
	loadProgress = 68.0f;
	ObjectManager->loadTerrainObjects(&pakFile, loadProgress, 30);
	loadProgress = 98.0f;
	ObjectManager->buildMoverLists();
	if (MPlayer)
		MPlayer->initSpecialBuildings(commandersToLoad);
	//----------------------------------------------
	// Read in the Mission Time Limit.
	{
		m_timeLimit = -1.0f /*seconds  IF -1.0 mission never times out!!! -fs*/;
		int32_t result = 0;
		result		   = missionFile->seekBlock("MissionSettings");
		if (NO_ERROR == result)
		{
			float tmpFloat = 0.0;
			result		   = missionFile->readIdFloat("TimeLimit", tmpFloat);
			if (NO_ERROR == result)
			{
				m_timeLimit = tmpFloat;
			}
		}
		if (MPlayer)
			m_timeLimit = MPlayer->missionSettings.timeLimit;
	}
	if (NumDisableAtStart)
	{
		for (size_t i = 0; i < NumDisableAtStart; i++)
			ObjectManager->getByWatchID(parts[DisableAtStart[i]].objectWID)
				->setDebugFlag(OBJECT_DFLAG_DISABLE, true);
	}
	//----------------------------------------------
	// Read in the Objectives.  Safe to have none.
	for (i = 0; i <= maxTeamID; i++)
		Team::teams[i]->loadObjectives(missionFile);
		/*	numObjectives = 0; // this refers to the number of *old* objectives
			objectives = 0;

			warning1 = FALSE;
			warning2 = FALSE;

			Team::home->objectives.Clear();
			Team::home->objectives.Read(missionFile);

			Team::home->numPrimaryObjectives = 0;
			CObjectives::EIterator it = Team::home->objectives.Begin();
			while (!it.IsDone())
			{
				if (1 == (*it)->Priority())
				{
					Team::home->numPrimaryObjectives += 1;
				}
				it++;
			}
			ReadNavMarkers(missionFile, Team::home->objectives);
		*/
#ifdef LAB_ONLY
	x					= GetCycles();
	MCTimeObjectiveLoad = x - x1;
#endif
	//----------------------------
	// Read in Commander Groups...
	for (size_t curCommanderId = 0; curCommanderId < MAX_MC_PLAYERS;
		 curCommanderId++)
	{
		int32_t numGroups = 0;
		char headingStr[128];
		if (commandersToLoad[curCommanderId][0] == -1)
			continue;
		sprintf(headingStr, "Commander%dGroup:%d", curCommanderId, numGroups);
		result = missionFile->seekBlock(headingStr);
		while (result == NO_ERROR)
		{
			//---------------------------
			// Read in the Group Mates...
			bool pointSet = FALSE;
			int32_t groupMates[MAX_MOVERGROUP_COUNT_START];
			result = missionFile->readIdLongArray(
				"Mates", groupMates, MAX_MOVERGROUP_COUNT_START);
			Assert(result == NO_ERROR, result,
				" could not find Mates in Group in Scenario File ");
			for (size_t curMate = 0; curMate < MAX_MOVERGROUP_COUNT_START;
				 curMate++)
			{
				if ((groupMates[curMate] > 0) &&
					parts[groupMates[curMate]].objectWID)
				{
					ObjectManager->setPartId(
						ObjectManager->getByWatchID(
							parts[groupMates[curMate]].objectWID),
						commandersToLoad[curCommanderId][0], numGroups,
						curMate);
					Commander::commanders[commandersToLoad[curCommanderId][0]]
						->getGroup(numGroups)
						->add((MoverPtr)ObjectManager->getByWatchID(
							parts[groupMates[curMate]].objectWID));
					if (!pointSet)
					{
						Commander::commanders[commandersToLoad[curCommanderId]
															  [0]]
							->getGroup(numGroups)
							->selectPoint(true);
						pointSet = true;
					}
				}
			}
			numGroups++;
			sprintf(
				headingStr, "Commander%dGroup:%d", curCommanderId, numGroups);
			result = missionFile->seekBlock(headingStr);
		}
	}
	//-----------------------------------------------------------------------
	// Now that the parts are loaded, let's build the roster for each team...
	for (i = 0; i < Team::numTeams; i++)
		Team::teams[i]->buildRoster();
	//---------------------------------------------------------------------------------
	// If we're not playing multiplayer, make sure all home commander movers
	// have their localMoverId set to 0, so the iface can at least check if a
	// mover is player controlled...
	if (!MPlayer)
		Commander::home->setLocalMoverId(0);
#ifdef LAB_ONLY
	x1					= GetCycles();
	MCTimeCommanderLoad = x1 - x;
#endif
	//-----------------------------------------------------
	// This tracks time since scenario started in seconds.
	LastTimeGetTime  = 0xffffffff;
	scenarioTime	 = 0.0;
	MissionStartTime = 0;
	runningTime		 = 0.0;
	actualTime		 = 0.0;
	gameSystemFile->close();
	delete gameSystemFile;
	gameSystemFile = nullptr;
	//----------------------------
	// Create and load the Weather
	weather = new Weather;
	weather->init(missionFile);
	//---------------------------------------------------------------
	// Start the Camera and Lists
	eye = new GameCamera;
	eye->init();
	gosASSERT(eye != nullptr);
	result = eye->init(missionFile);
	gosASSERT(result == NO_ERROR);
	eye->inMovieMode = false;
	loadProgress	 = 99.0;
#ifdef LAB_ONLY
	x			   = GetCycles();
	MCTimeMiscLoad = x - x1;
#endif
	//----------------------------------------------------------------------------
	// Start the Mission GUI
	missionInterface = new MissionInterfaceManager;
	gosASSERT(missionInterface != nullptr);
	missionInterface->initTacMap(&pakFile, 2);
	FullPathFileName missionScreenName;
	missionScreenName.init(artPath, "missionScrn", ".fit");
	FitIniFile missionLoader;
	result = missionLoader.open(missionScreenName);
	gosASSERT(result == NO_ERROR);
	missionInterface->init(&missionLoader);
	missionInterface->initMechs();
	missionLoader.close();
	//----------------------------------------------------------------------------
	userInput->setMouseCursor(mState_NORMAL);
	loadProgress = 100.0;
	// MechWarrior::initGoalManager(200);
	if (tempSpecialAreaFootPrints)
	{
		systemHeap->Free(tempSpecialAreaFootPrints);
		tempSpecialAreaFootPrints = nullptr;
		tempNumSpecialAreas		  = 0;
	}
	Mover::initOptimalCells(32);
	//--------------------------------------------------
	// Close all walls and open gates and landbridges...
	//	GameObjectPtr wallObjects[MAX_WALL_OBJECTS];
	//	int32_t numWalls = ObjectManager->getSpecificObjects(BUILDING,
	//BUILDING_SUBTYPE_WALL, wallObjects, MAX_WALL_OBJECTS); 	for (i = 0; i <
	//numWalls; i++)
	//		((BuildingPtr)wallObjects[i])->closeSubAreas();
	//	int32_t numLandBridges = ObjectManager->getSpecificObjects(BUILDING,
	//BUILDING_SUBTYPE_LANDBRIDGE, wallObjects, MAX_WALL_OBJECTS); 	for (i = 0;
	//i < numLandBridges; i++)
	//		((BuildingPtr)wallObjects[i])->openSubAreas();
	//	for (i = 0; i < ObjectManager->getNumGates(); i++) {
	//		GatePtr gate = ObjectManager->getGate(i);
	//		gate->openSubAreas();
	//	}
	if (CombatLog)
		MechWarrior::logPilots(CombatLog);
#ifdef LAB_ONLY
	x1			  = GetCycles();
	MCTimeGUILoad = x1 - x;
#endif
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
#endif
	missionFile->close();
	delete missionFile;
	missionFile = nullptr;
}
//----------------------------------------------------------------------------------
// Sets mission active.  Assumes that whatever we needed to do to the mission
// data set for logistis has been done and player is ready to play!
void DEBUGWINS_setGameObject(int32_t debugObj, GameObjectPtr obj);
extern int32_t GameObjectWindowList[3];
extern int32_t NumGameObjectsToDisplay;
void Mission::start(void)
{
	active = true;
	for (size_t i = 0; i < NumGameObjectsToDisplay; i++)
		DEBUGWINS_setGameObject(
			-1, ObjectManager->getByWatchID(
					parts[GameObjectWindowList[i]].objectWID));
}
//----------------------------------------------------------------------------------
void Mission::initTGLForMission()
{
	//---------------------------------------------------------
	// End the Tiny Geometry Layer Heap for Logistics
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
	loadProgress += 1.0f;
	//------------------------------------------------------
	// Start the Tiny Geometry Layer Heap for the Mission.
	if (!TG_Shape::tglHeap)
	{
		//---------------------------------------------------------
		uint32_t tglHeapSize = 40 * 1024 * 1024;
		TG_Shape::tglHeap	= new UserHeap;
		TG_Shape::tglHeap->init(tglHeapSize, "TinyGeom");
		// Start up the TGL RAM pools.
		colorPool = new TG_VertexPool;
		colorPool->init(30000);
		vertexPool = new TG_GOSVertexPool;
		vertexPool->init(30000);
		facePool = new TG_DWORDPool;
		facePool->init(40000);
		shadowPool = new TG_ShadowPool;
		shadowPool->init(30000);
		trianglePool = new TG_TrianglePool;
		trianglePool->init(20000);
	}
	loadProgress += 4.0f;
	// Stupid hack for now.  Should really get from prefs!!
	// Needed cause Heidi resets in logistics.
	useFog	 = true;
	useShadows = prefs.useShadows;
}
//----------------------------------------------------------------------------------
void Mission::destroy(bool initLogistics)
{
	//---------------------------------------------------------------
	// Shutdown the Mission Interface
	if (missionInterface)
	{
		missionInterface->destroy();
		delete missionInterface;
		missionInterface = nullptr;
	}
	delete missionBrain;
	missionBrain = nullptr;
	delete missionBrainParams;
	missionBrainParams = nullptr;
	// Team::home->objectives.Clear();
	//---------------------------------------------------------------
	// End the Camera and Lists
	if (eye)
	{
		eye->destroy();
		delete eye;
		eye = nullptr;
	}
	if (PathManager)
	{
		PathManager->destroy();
		delete PathManager;
		PathManager = nullptr;
	}
	MOVE_cleanup();
	MechWarrior::shutdown();
	delete weather;
	weather = nullptr;
	//------------------------------------------------------------
	// End the Terrain System
	if (land)
	{
		land->destroy();
		delete land;
		land = nullptr;
	}
	//--------------------------------------------------------------
	// Shut down the object system
	// CANNOT just delete heaps do to our ASSHOLE MUNGA/ADEPT
	// CODE which cannot be shutdown and restarted more then once.
	//
	// I wish I could just do demos and call that game experience!!!!
	//
	if (ObjectManager)
	{
		ObjectManager->destroy();
		delete ObjectManager;
		ObjectManager = nullptr;
	}
	if (Mover::sortList)
	{
		delete Mover::sortList;
		Mover::sortList = nullptr;
	}
	if (SensorSystem::sortList)
	{
		delete SensorSystem::sortList;
		SensorSystem::sortList = nullptr;
	}
	if (GVAppearanceType::SensorTriangleShape)
	{
		delete GVAppearanceType::SensorTriangleShape;
		GVAppearanceType::SensorTriangleShape = nullptr;
	}
	if (GVAppearanceType::SensorCircleShape)
	{
		delete GVAppearanceType::SensorCircleShape;
		GVAppearanceType::SensorCircleShape = nullptr;
	}
	if (Mech3DAppearanceType::SensorSquareShape)
	{
		delete Mech3DAppearanceType::SensorSquareShape;
		Mech3DAppearanceType::SensorSquareShape = nullptr;
	}
	if (MasterComponent::masterList)
	{
		MasterComponent::freeMasterList();
	}
	//-----------------------------------
	// Get rid of the Effects manager
	if (weaponEffects)
		weaponEffects->destroy();
	delete weaponEffects;
	weaponEffects = nullptr;
	if (craterManager)
	{
		craterManager->destroy();
		missionHeap->Free(craterManager);
		craterManager = nullptr;
	}
	//-----------------------------------
	// Sensors now stored in the missionHeap
	// Which goes away several lines below this
	// if (SensorManager)
	//{
	//	SensorManager->destroy();
	//	delete SensorManager;
	//	SensorManager = nullptr;
	//}
	int32_t numt = Team::numTeams;
	for (size_t i = 0; i < numt; i++)
		if (Team::teams[i])
		{
			delete Team::teams[i];
			Team::teams[i] = nullptr;
		}
	Team::numTeams = 0;
	int32_t numC   = Commander::numCommanders;
	for (i = 0; i < numC; i++)
		if (Commander::commanders[i])
		{
			delete Commander::commanders[i];
			Commander::commanders[i] = nullptr;
		}
	Commander::numCommanders = 0;
	if (Mover::triggerAreaMgr)
	{
		delete Mover::triggerAreaMgr;
		Mover::triggerAreaMgr = nullptr;
	}
	if (MoverGroup::goalMap)
	{
		systemHeap->Free(MoverGroup::goalMap);
		MoverGroup::goalMap = nullptr;
	}
	if (openList)
	{
		delete openList;
		openList = nullptr;
	}
	if (Team::sortList)
	{
		delete Team::sortList;
		Team::sortList = nullptr;
	}
	closeABL();
	//------------------------------------------------------------
	// End the Mission Heap
	if (missionHeap)
	{
		missionHeap->destroy();
		delete missionHeap;
		missionHeap = nullptr;
	}
	if (appearanceTypeList)
	{
		appearanceTypeList->destroy();
		delete appearanceTypeList;
		appearanceTypeList = nullptr;
	}
	if (initLogistics)
		initTGLForLogistics();
	else
	{
		//---------------------------------------------------------
		// End the Tiny Geometry Layer Heap for Logistics
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
	}
	//	userInput->mouseOff();
	active = FALSE;
	scenarioResult =
		mis_PLAYING; // We've completed the mission.  Set it back to playing!
	GeneralAlarm = false;
	EllipseElement::removeTextureHandle();
	// The flush below will remove this one.
	Mover::holdFireIconHandle = 0;
	mcTextureManager->flush();
	mcTextureManager->freeVertices();
	soundSystem->purgeSoundSystem();
	missionFileName[0] = 0;
	if (CObjective::s_markerFont)
	{
		delete CObjective::s_markerFont;
		CObjective::s_markerFont = nullptr;
	}
	// Heading back to logistics now.  Change screen back to 800x600
	//	if (renderer == 3)
	//		gos_SetScreenMode(800,600,16,0,0,0,true,false,0,false,0,renderer);
	//	else
	//		gos_SetScreenMode(800,600,16,0,0,0,0,false,0,false,0,renderer);
	// Start finding the Leaks
	// systemHeap->dumpRecordLog();
}
//----------------------------------------------------------------------------------
void Mission::handleMultiplayMessage(int32_t code, int32_t param1)
{
	if (missionBrainCallback)
	{
		CurMultiplayCode  = code;
		CurMultiplayParam = param1;
		missionBrain->execute(nullptr, missionBrainCallback);
		CurMultiplayCode  = 0;
		CurMultiplayParam = 0;
	}
}
//----------------------------------------------------------------------------------
void Mission::startObjectiveTimers(void)
{
	for (size_t i = 0; i < (int32_t)numObjectives; i++)
	{
		if (objectives[i].timeLeft > 0.0)
		{
			setObjectiveTimer(i, objectives[i].timeLeft);
		}
	}
}
//----------------------------------------------------------------------------------
int32_t Mission::setObjectiveTimer(int32_t objectiveNum, float timeLeft)
{
	gosASSERT((objectiveNum >= 0) && objectiveNum < (int32_t)numObjectives);
	int32_t timerNumber = OBJECTIVE_1_TIMER + objectiveNum;
	//------------
	// Add Timer.
	TimerPtr timer = timerManager->getTimer(timerNumber);
	gosASSERT(timer != nullptr);
	timer->setTimer(timeLeft);
	return (NO_ERROR);
}
//----------------------------------------------------------------------------------
float Mission::checkObjectiveTimer(int32_t objectiveNum)
{
	gosASSERT((objectiveNum >= 0) || objectiveNum < (int32_t)numObjectives);
	int32_t timerNumber = OBJECTIVE_1_TIMER + objectiveNum;
	uint32_t timeLeft   = 0;
	TimerPtr timer		= timerManager->getTimer(timerNumber);
	gosASSERT(timer != nullptr);
	timeLeft = timer->getCurrentTime();
	return (timeLeft);
}
//----------------------------------------------------------------------------------
int32_t Mission::setObjectiveStatus(
	int32_t objectiveNum, ObjectiveStatus status)
{
	gosASSERT((objectiveNum >= 0) || objectiveNum < (int32_t)numObjectives);
	if (objectives)
		objectives[objectiveNum].status = status;
	return (NO_ERROR);
}
//----------------------------------------------------------------------------------
bool Mission::checkObjectiveSuccess(void)
{
	// NOW uses NEW(tm) objectives.
	int32_t count = 0;
	bool result   = false;
	if (Team::home->objectives.Count())
	{
		for (CObjectives::EIterator iter = Team::home->objectives.Begin();
			 !iter.IsDone(); iter++, count++)
		{
			// Must iterate through ALL objectives and check.
			result |= (*iter)->StatusChangedSuccess();
		}
	}
	return result;
}
//----------------------------------------------------------------------------------
bool Mission::checkObjectiveFailed(void)
{
	// NOW uses NEW(tm) objectives.
	int32_t count = 0;
	bool result   = false;
	if (Team::home->objectives.Count())
	{
		for (CObjectives::EIterator iter = Team::home->objectives.Begin();
			 !iter.IsDone(); iter++, count++)
		{
			// Must iterate through ALL objectives and check.
			result |= (*iter)->StatusChangedFailed();
		}
	}
	return result;
}
//----------------------------------------------------------------------------------
ObjectiveStatus Mission::checkObjectiveStatus(int32_t objectiveNum)
{
	// NOW uses NEW(tm) objectives.
	int32_t count = 0;
	if (Team::home->objectives.Count())
	{
		for (CObjectives::EIterator iter = Team::home->objectives.Begin();
			 !iter.IsDone(); iter++, count++)
		{
			if (count == objectiveNum)
			{
				return (*iter)->Status(Team::home->objectives);
			}
		}
	}
	return OS_UNDETERMINED;
}
//----------------------------------------------------------------------------------
int32_t Mission::setObjectiveType(int32_t objectiveNum, ObjectiveType type)
{
	gosASSERT((objectiveNum >= 0) || objectiveNum < (int32_t)numObjectives);
	if (objectives)
		objectives[objectiveNum].type = type;
	return (NO_ERROR);
}
//----------------------------------------------------------------------------------
ObjectiveType Mission::checkObjectiveType(int32_t objectiveNum)
{
	gosASSERT((objectiveNum >= 0) || objectiveNum < (int32_t)numObjectives);
	ObjectiveType result = PrimaryGoal;
	if (objectives)
		result = objectives[objectiveNum].type;
	return (result);
}
//----------------------------------------------------------------------------------
void Mission::setObjectivePos(
	int32_t objectiveNum, float realX, float realY, float realZ)
{
	gosASSERT((objectiveNum >= 0) || objectiveNum < (int32_t)numObjectives);
	if (objectives)
	{
		objectives[objectiveNum].position.x = realX;
		objectives[objectiveNum].position.y = realY;
		objectives[objectiveNum].position.z = realZ;
	}
}
//---------------------------------------------------------------------------