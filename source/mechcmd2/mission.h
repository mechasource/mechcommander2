//******************************************************************************************
//	mission.h - This file contains the mission class header
//		Missions are what scenarios were in MechCommander 1.
//
//	MechCommander 2
//
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#pragma once

#ifndef MISSION_H
#define MISSION_H

//#include <mclib.h>
//#include "warrior.h"
//#include "missiongui.h"
//#include "objective.h"
//#include "logisticsmech.h"
//#include "multplyr.h"

//----------------------------------------------------------------------------------
// Macro Definitions
enum class player_mission : uint32_t
{
	mis_PLAYING,
	mis_PLAYER_LOST_BIG,
	mis_PLAYER_LOST_SMALL,
	mis_PLAYER_DRAW,
	mis_PLAYER_WIN_SMALL,
	mis_PLAYER_WIN_BIG
};

extern uint32_t scenarioResult;

//----------------------------------------------------------------------------------
// These are placeholders for the scenario specific objects.  This allows us to
// create objects which the scenario can manipulate using other scenario data.
// For example, a part may be assigned to a certain area.  This would inform
// the part of where its supposed to be for AI AND allow us to remove the part
// if all player controlled objects leave that area.  What the folks downstairs
// would call an interest manager except that its simple and it works!
struct Part
{
	GameObjectWatchID objectWID; // Pointer to my physical incarnation
	uint32_t objNumber;			 // What kind of object am I?
	uint32_t baseColor;			 // Base color of mech -- Overrides the RED in RGB
	uint32_t highlightColor1;	// First Highlight Color -- Overrides the GREEN in RGB
	uint32_t highlightColor2;	// Second Highlight Color -- Overrides the BLUE in RGB
	int32_t active;				 // Am I currently awake?
	int32_t exists;				 // Am I currently in Existance?
	bool destroyed;				 // Have I been destroyed for this scenario?
	Stuff::Vector3D position;	// Where do I start?  (relative to area)
	float velocity;				 // How fast am I going?
	float rotation;				 // Which direction am I facing?
	uint32_t gestureId;			 // What gesture do I start in?
	char alignment;				 // Who do I fight for?
	char teamId;				 // Which team am I on?
	int32_t commanderID;		 // Used when setting up multiplayer
	char squadId;				 // Which team am I on?
	char myIcon;				 // If set, start with Icon on Screen for this part.
	uint32_t controlType;		 // How am I controlled?
	uint32_t controlDataType;	// What data do you need to control me?
	char profileName[9];		 // Name of Object Profile file.
	uint32_t pilot;				 // Name of Pilot File.
	bool captureable;			 // Is this a capturable "enemy" mech?
	uint32_t variantNum;		 // Variant number of the Part.

	void Save(FitIniFilePtr file, int32_t partNum);
	void Load(FitIniFilePtr file, int32_t partNum);
};

typedef Part* PartPtr;

//----------------------------------------------------------------------------------
// This is the struct definition for the Scenario Objectives.
// These can be displayed in a window.  For now they are drawn with just text.
// There are a whole series of ABL commands to address these objectives.
typedef uint32_t ObjectiveType;
#define InvisibleGoal -1
#define PrimaryGoal 0
#define SecondaryGoal 1
#define OtherGoal 2
#define BonusGoal 3
#define InvalidGoal 9999

typedef uint32_t ObjectiveStatus;
#define Incomplete 0
#define Success 1
#define Failed 2
#define InvalidStatus 9999

#define SCENARIO_TIMER_1 7
#define SCENARIO_TIMER_2 (SCENARIO_TIMER_1 + 1)
#define SCENARIO_TIMER_3 (SCENARIO_TIMER_1 + 2)
#define SCENARIO_TIMER_4 (SCENARIO_TIMER_1 + 3)
#define SCENARIO_TIMER_5 (SCENARIO_TIMER_1 + 4)
#define SCENARIO_TIMER_6 (SCENARIO_TIMER_1 + 5)
#define SCENARIO_TIMER_7 (SCENARIO_TIMER_1 + 6)
#define SCENARIO_TIMER_8 (SCENARIO_TIMER_1 + 7)

#define OBJECTIVE_1_TIMER 24
#define OBJECTIVE_2_TIMER (OBJECTIVE_1_TIMER + 1)
#define OBJECTIVE_3_TIMER (OBJECTIVE_1_TIMER + 2)
#define OBJECTIVE_4_TIMER (OBJECTIVE_1_TIMER + 3)
#define OBJECTIVE_5_TIMER (OBJECTIVE_1_TIMER + 4)
#define OBJECTIVE_6_TIMER (OBJECTIVE_1_TIMER + 5)
#define OBJECTIVE_7_TIMER (OBJECTIVE_1_TIMER + 6)
#define OBJECTIVE_8_TIMER (OBJECTIVE_1_TIMER + 7)

#define MAX_OBJECTIVES 50

typedef enum
{
	MISSION_LOAD_SP_QUICKSTART,
	MISSION_LOAD_SP_LOGISTICS,
	MISSION_LOAD_MP_QUICKSTART,
	MISSION_LOAD_MP_LOGISTICS
} MissionLoadType;

struct Objective
{
	char name[80];
	ObjectiveType type;
	float timeLeft;
	ObjectiveStatus status;
	Stuff::Vector3D position;
	int32_t points;
	float radius;
};

typedef Objective* ObjectivePtr;

//----------------------------------------------------------------------------------
// To operate, simply call init with the filename of the mission.
// that's all she wrote.  update and render return the mission completion
// status.
class Mission
{

	// Data Members
	//-------------
protected:
	FitIniFilePtr missionFile;

	int32_t operationId; // aka operation id
	int32_t missionId;   // aka mission id

	char missionFileName[80];
	char missionScriptName[80];
	ABLModulePtr missionBrain;
	ABLParamPtr missionParams;
	SymTableNodePtr missionBrainCallback;

	uint32_t numParts;
	PartPtr parts;

	bool active;

	static double missionTerminationTime;

public:
	static bool terminationCounterStarted;
	static uint32_t terminationResult;

	uint32_t numObjectives;
	ObjectivePtr objectives;
	float m_timeLimit;
	// CObjectives						missionObjectives;
	// int32_t						numPrimaryObjectives;
	uint32_t duration;
	bool warning1;
	bool warning2;

	float actualTime;
	float runningTime;

	int32_t numSmallStrikes;
	int32_t numLargeStrikes;
	int32_t numSensorStrikes;
	int32_t numCameraStrikes;

	uint8_t missionTuneNum;

	int32_t missionScriptHandle;
	ABLParam* missionBrainParams;

	MissionInterfaceManagerPtr missionInterface;

	Stuff::Vector3D dropZone;

	int32_t theSkyNumber;

	static bool statisticsInitialized;

	// Member Functions
	//-----------------
protected:
public:
	void init(void)
	{
		missionFile = nullptr;
		operationId = missionId = -1;
		missionBrain			= nullptr;
		missionParams			= nullptr;
		missionBrainCallback	= nullptr;
		numParts				= 0;
		parts					= nullptr;
		numObjectives			= 0;
		objectives				= nullptr;
		duration				= 0;
		active					= FALSE;
		numSmallStrikes			= 0;
		numLargeStrikes			= 0;
		numSensorStrikes		= 0;
		numCameraStrikes		= 0;
		missionTuneNum			= 0;
		missionScriptHandle		= -1;
		missionInterface		= nullptr;
		missionFileName[0]		= 0;
		missionBrainParams		= nullptr;
	}

	Mission(void) { init(void); }

	bool calcComplexDropZones(PSTR missionName, char dropZoneList[MAX_MC_PLAYERS]);

	void init(PSTR missionName, int32_t loadType, int32_t dropZoneID, Stuff::Vector3D* dropZoneList,
		char commandersToLoad[8][3], int32_t numMoversPerCommander);

	static void initBareMinimum(void);

	static void initTGLForMission(void);
	static void initTGLForLogistics(void);

	void start(void);

	Stuff::Vector3D getDropZone() const { return dropZone; }

	int32_t update(void);
	int32_t render(void);

	void destroy(bool initLogistics = true);

	~Mission(void) { destroy(false); }

	int32_t getStatus(void);

	int32_t getNumParts(void) { return (numParts); }

	PartPtr getPart(int32_t partNumber) { return (&parts[partNumber]); }

	int32_t getPartTeamId(int32_t partNumber) { return (parts[partNumber].teamId); }

	GameObjectPtr getPartObject(int32_t partNumber)
	{
		if ((partNumber <= 0) || ((uint32_t)partNumber > numParts))
			return nullptr;
		if (!ObjectManager)
			return (nullptr);
		return ((GameObjectPtr)ObjectManager->getByWatchID(parts[partNumber].objectWID));
	}

	int32_t addMover(MoverInitData* moveSpec);
	int32_t addMover(MoverInitData* moveSpec, LogisticsMech* mechData);
	int32_t addMover(MoverInitData* moveSpec, CompressedMech* mechData);

	int32_t removeMover(MoverPtr mover);

	void tradeMover(MoverPtr mover, int32_t newTeamID, int32_t newCommanderID, PSTR pilotFileName,
		PSTR brainFileName);

	void createPartObject(int32_t objectId, MoverPtr mover);

	void createMissionObject(int32_t partId); // Moves object from holding area to real world.

	ABLModulePtr getBrain(void) { return (missionBrain); }

	void handleMultiplayMessage(int32_t code, int32_t param1);

	//-----------------------------------------------------
	// Objective Routines
	void startObjectiveTimers(void);

	int32_t setObjectiveTimer(int32_t objectiveNum, float timeLeft);
	float checkObjectiveTimer(int32_t objectiveNum);

	int32_t setObjectiveStatus(int32_t objectiveNum, ObjectiveStatus status);
	ObjectiveStatus checkObjectiveStatus(int32_t objectiveNum);

	int32_t setObjectiveType(int32_t objectiveNum, ObjectiveType type);
	ObjectiveType checkObjectiveType(int32_t objectiveNum);

	void setObjectivePos(int32_t objectiveNum, float realX, float realY, float realZ);

	void setupBonus(void);

	// Checks if any objective has succeeded or failed since we last checked.
	bool checkObjectiveSuccess(void);
	bool checkObjectiveFailed(void);
	//-----------------------------------------------------

	int32_t GetOperationID(void) { return operationId; }

	int32_t GetMissionID(void) { return missionId; }

	PSTR getMissionName(void) { return missionScriptName; }

	int32_t getMissionTuneId(void) { return missionTuneNum; }

	PCSTR getMissionFileName() { return missionFileName; }

	static void initializeStatistics(void);

	void load(PCSTR filename);
	void save(PCSTR filename);

	bool isActive(void) { return active; }
};

//----------------------------------------------------------------------------------

#define MAX_GLOBAL_MISSION_VALUES 50

extern float worldUnitsPerMeter;
extern float metersPerWorldUnit;
extern float maxVisualRange;
extern float fireVisualRange;
extern bool CantTouchThis;

extern float MineDamage;
extern float MineSplashDamage;
extern float MineSplashRange;
extern int32_t MineExplosion;

extern float globalMissionValues[];

extern uint8_t godMode;

extern Mission* mission;
extern uint32_t scenarioResult;

extern UserHeapPtr missionHeap;
//----------------------------------------------------------------------------------
#endif
