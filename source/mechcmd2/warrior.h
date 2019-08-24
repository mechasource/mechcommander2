//===========================================================================//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

//***************************************************************************
//
// WARRIOR.H
//
//***************************************************************************

#pragma once

#ifndef WARRIOR_H
#define WARRIOR_H

//#include "mclib.h"
//#include "move.h"
//#include "dmovemgr.h"
//#include "dwarrior.h"
//#include "dgameobj.h"
//#include "dmover.h"
//#include "dmech.h"
//#include "mechclass.h"
//#include "dgvehicl.h"
//#include "dteam.h"
//#include "dcomndr.h"
//#include "tacordr.h"
//#include "dradio.h"
//#include "mech3d.h"
//#include "dgoal.h"
//#include "dgroup.h"
//#include "dgamelog.h"

extern float scenarioTime;

class BldgAppearance;

//***************************************************************************

#define MAX_WARRIORS 120

#define MOVEGOAL_NONE 0xFFFFFFFF
#define MOVEGOAL_LOCATION 0x00000000

#define CLAN_SEEN_FLAG 2
#define IS_SEEN_FLAG 1

#define NUM_WEAPON_TYPE_AFFINITIES 2

#define NUM_OFFSET_RANGES 5

#define WARRIOR_DEBUGGING_ENABLED TRUE
#define WARRIOR_DEBUG_FLAG_NONE 0
#define WARRIOR_DEBUG_FLAG_COMBAT 1

#define MOVEPATH_ERR_NONE 0
#define MOVEPATH_ERR_NO_GOAL -1
#define MOVEPATH_ERR_GOALOBJ_DEAD -2
#define MOVEPATH_ERR_NO_VALID_GOAL -3
#define MOVEPATH_ERR_ALREADY_THERE -4
#define MOVEPATH_ERR_LR_NOT_ENABLED -5
#define MOVEPATH_ERR_LR_START_BLOCKED -6
#define MOVEPATH_ERR_LR_NO_PATH -7
#define MOVEPATH_ERR_LR_NO_SR_PATH -8
#define MOVEPATH_ERR_EARLY_STOP -9
#define MOVEPATH_ERR_TIME_OUT -10
#define MOVEPATH_ERR_LR_DOOR_CLOSED -11
#define MOVEPATH_ERR_LR_DOOR_BLOCKED -12
#define MOVEPATH_ERR_ESCAPING_TILE -13
#define MOVEPATH_ERR_TACORDER_CLEARED -14

#define MAX_QUEUED_TACORDERS 2000
#define MAX_QUEUED_TACORDERS_PER_WARRIOR 16

enum class OrderType : uint8_t
{
	current,
	issued,
	order_pending,
	numberoftypes
};

enum class SituationType : uint8_t
{
	normal,
	engage,
	withdraw,
	retreat,
	rout,
	berserk,
	suicidal,
	numberoftypes
};

enum class MechType : uint8_t
{
	none,
	hollander,
	commando,
	cougar,
	firestarter,
	puma,
	raven,
	uller,
	centurion,
	hunchback,
	hunchbackiic,
	catapult,
	jagermech,
	loki,
	madcat,
	thor,
	vulture,
	atlas,
	awesome,
	masakari,
	numberoftypes
};

enum class WeaponClass : uint8_t
{
	none,
	ballistic,
	energy,
	missile,
	numberoftypes
};

enum class MechWarriorRole : uint8_t
{
	point,
	wingman,
	rearguard,
	scout,
	numberofroles
};

enum class Skill : uint8_t
{
	piloting,
	sensors,
	gunnery,
	numberofskills
};

enum class PersonalityTrait : uint8_t
{
	professionalism,
	decorum,
	aggressiveness,
	courage,
	numberoftraits
};

enum class WarriorRank : uint8_t
{
	green,
	regular,
	veteran,
	elite,
	ace,
	numberofranks
};

enum class SpecialtySkill : uint8_t
{
	LIGHT_MECH_SPECIALIST,
	LASER_SPECIALIST,
	LIGHT_AC_SPECIALIST,
	MEDIUM_AC_SPECIALIST,
	SRM_SPECIALIST,
	SMALL_ARMS_SPECIALIST,
	SENSOR_SPECIALIST,
	TOUGHNESS_SPECIALIST,
	MEDIUM_MECH_SPECIALIST,
	PULSE_LASER_SPECIALIST,
	ER_LASER_SPECIALIST,
	LRM_SPECIALIST,
	SCOUT,
	LONG_JUMP,
	HEAVY_MECH_SPECIALIST,
	PPC_SPECIALIST,
	HEAVY_AC_SPECIALIST,
	SHORT_RANGE_SPECIALIST,
	MEDIUM_RANGE_SPECIALIST,
	LONG_RANGE_SPECIALIST,
	ASSAULT_MECH_SPECIALIST,
	GAUSS_SPECIALIST,
	SHARPSHOOTER,
	NUM_SPECIALTY_SKILLS
};

enum SpecialtySkillType
{
	WEAPON_SPECIALTY = 0,
	CHASSIS_SPECIALTY = 1,
	RANGE_SPECIALTY = 2,
	OTHER_SPECIALTY = 3

};

#define FIRST_REGULAR_SPECIALTY 0
#define FIRST_VETERAN_SPECIALTY 8
#define FIRST_ELITE_SPECIALTY 14
#define FIRST_ACE_SPECIALTY 20

enum class 
{
	CV_CURRENT,
	CV_FIELDED,
	CV_NEW,
	NUM_CV_TYPES
} CombatValueType;

enum class CombatStats
{
	COMBAT_STAT_CAREER,
	COMBAT_STAT_MISSION,
	NUM_COMBAT_STATS
} CombatStats;

enum class WarriorStatus
{
	WARRIOR_STATUS_NORMAL,
	WARRIOR_STATUS_WITHDRAWING,
	WARRIOR_STATUS_WITHDRAWN,
	WARRIOR_STATUS_EJECTED,
	WARRIOR_STATUS_DEAD,
	WARRIOR_STATUS_MIA,
	WARRIOR_STATUS_CAPTURED,
	WARRIOR_STATUS_BASECAMP,
	NUM_WARRIOR_STATUSES
} WarriorStatus;

enum class MoveStateType
{
	MOVESTATE_STAND,
	MOVESTATE_FORWARD,
	MOVESTATE_REVERSE,
	MOVESTATE_PIVOT_FORWARD,
	MOVESTATE_PIVOT_REVERSE,
	MOVESTATE_PIVOT_TARGET,
	NUM_MOVESTATES
} MoveStateType;

//---------------------------------------------------------------------------

#define MAX_ALARM_TRIGGERS 15

enum class 
{
	PILOT_ALARM_TARGET_OF_WEAPONFIRE,
	PILOT_ALARM_HIT_BY_WEAPONFIRE,
	PILOT_ALARM_DAMAGE_TAKEN_RATE,
	PILOT_ALARM_DEATH_OF_MATE,
	PILOT_ALARM_FRIENDLY_VEHICLE_CRIPPLED,
	PILOT_ALARM_FRIENDLY_VEHICLE_DESTROYED,
	PILOT_ALARM_VEHICLE_INCAPACITATED,
	PILOT_ALARM_VEHICLE_DESTROYED,
	PILOT_ALARM_VEHICLE_WITHDRAWN,
	PILOT_ALARM_ATTACK_ORDER,
	PILOT_ALARM_COLLISION,
	PILOT_ALARM_GUARD_RADIUS_BREACH,
	PILOT_ALARM_KILLED_TARGET,
	PILOT_ALARM_MATE_FIRED_WEAPON,
	PILOT_ALARM_PLAYER_ORDER,
	PILOT_ALARM_NO_MOVEPATH,
	PILOT_ALARM_GATE_CLOSING,
	PILOT_ALARM_FIRED_WEAPON,
	PILOT_ALARM_NEW_MOVER,
	NUM_PILOT_ALARMS
} PilotAlarmType;

typedef struct
{
	char code;
	uint8_t numTriggers;
	int32_t trigger[MAX_ALARM_TRIGGERS];
} PilotAlarm;

//---------------------------------------------------------------------------

typedef struct
{
	float lastUnderFire; // time of last under fire message
	bool weaponsIneffective; // true if already informed player
	bool weaponsOut; // true if already informed player
	float lastContact; // time of last contact message
	RadioMessageType lastMessageType; // e.g. RADIO_WEAPONS_OUT
	uint32_t lastMessage; // PacketNumber of message
	float lastMessageTime; // time of last message (of any type)
} RadioLog;

//---------------------------------------------------------------------------

#define MAX_TARGET_PRIORITIES 20

enum class 
{
	TARGET_PRIORITY_NONE,
	TARGET_PRIORITY_GAMEOBJECT,
	TARGET_PRIORITY_MOVER,
	TARGET_PRIORITY_BUILDING,
	TARGET_PRIORITY_CURTARGET,
	TARGET_PRIORITY_TURRET,
	TARGET_PRIORITY_TURRET_CONTROL,
	TARGET_PRIORITY_GATE,
	TARGET_PRIORITY_GATE_CONTROL,
	TARGET_PRIORITY_SALVAGE,
	TARGET_PRIORITY_MECHBAY,
	TARGET_PRIORITY_LOCATION,
	TARGET_PRIORITY_EVENT_TARGETED,
	TARGET_PRIORITY_EVENT_HIT,
	TARGET_PRIORITY_EVENT_DAMAGED,
	TARGET_PRIORITY_EVENT_MATE_DIED,
	TARGET_PRIORITY_EVENT_FRIENDLY_DISABLED,
	TARGET_PRIORITY_EVENT_FRIENDLY_DESTROYED,
	TARGET_PRIORITY_EVENT_FRIENDLY_WITHDRAWS,
	TARGET_PRIORITY_SKIP,
	NUM_TARGET_PRIORITIES
} TargetPriorityType;

typedef struct _TargetPriority
{
	int32_t type;
	int32_t params[3];
} TargetPriority;

typedef TargetPriority* TargetPriorityPtr;

class TargetPriorityList
{

public:
	int32_t size;
	TargetPriority list[MAX_TARGET_PRIORITIES];

public:
	PVOID operator new(size_t ourSize);

	void operator delete(PVOID us);

	void init(void);

	TargetPriorityList(void) { init(void); }

	void destroy(void);

	~TargetPriorityList(void) { destroy(void); }

	int32_t insert(int32_t index, TargetPriorityPtr priority);

	void remove(int32_t index);

	int32_t calcAction(std::unique_ptr<MechWarrior> pilot, GameObjectPtr target);

	int32_t calcTarget(
		std::unique_ptr<MechWarrior> pilot, Stuff::Vector3D location, int32_t contactCriteria, int32_t& action);
};

//---------------------------------------------------------------------------

//---------------------------------------------------------------------------

enum class 
{
	ORDERSTATE_GENERAL,
	ORDERSTATE_PLAYER,
	ORDERSTATE_ALARM,
	NUM_ORDERSTATES
} OrderStateType;

typedef struct _AttackerRec
{
	uint32_t WID;
	float lastTime;
} AttackerRec;

typedef AttackerRec* AttackerRecPtr;

#define MAX_GLOBAL_PATH 50

enum class 
{
	MOVEPATH_UNDEFINED,
	MOVEPATH_SIMPLE,
	MOVEPATH_COMPLEX
} MovePathType;

typedef struct _MoveOrders
{
	//------------------
	// order parameters
	float time;
	char origin;
	int32_t speedType; // best, max, slow, moderate
	float speedVelocity; // based upon speedType (m/s)
	char speedState; // based upon speedVelocity (walk, etc.)
	char speedThrottle; // if speedState is walk, else ignored
	uint32_t goalType; // is there no goal, a location or object?
	GameObjectWatchID goalObjectWID; // if our goal is an object...
	Stuff::Vector3D goalObjectPosition; // object's position at time of path calc
	Stuff::Vector3D goalLocation; // goal location, if any
	//----------------------
	// pathfinding guts/data
	float nextUpdate;
	bool newGoal;
	Stuff::Vector3D wayPath[MAX_WAYPTS]; // if this move order has a way path...
	char numWayPts; // how many way points?
	char curWayPt; // current goal waypoint
	char curWayDir; // waypath direction: 1 = forward, -1 = backward
	char pathType; // "quick" path or global/int32_t-range?
	Stuff::Vector3D originalGlobalGoal[2]; // 0 = original final goal, 1 = actual final goal
	Stuff::Vector3D globalGoalLocation;
	GlobalPathStep globalPath[MAX_GLOBAL_PATH];
	char numGlobalSteps;
	char curGlobalStep;
	MovePathPtr path[2]; // 0 = current path, 1 = next path
	float timeOfLastStep; // last time a step was reached for this order
	int32_t moveState; // forward, reverse, paused, etc.
	int32_t moveStateGoal;
	bool twisting;
	float yieldTime;
	int32_t yieldState; // have we just yielded, been for a while, etc.
	float waitForPointTime;
	bool run;

	void init(void);

} MoveOrders;

typedef struct _SaveableMoveOrders
{
	//------------------
	// order parameters
	float time;
	char origin;
	int32_t speedType; // best, max, slow, moderate
	float speedVelocity; // based upon speedType (m/s)
	char speedState; // based upon speedVelocity (walk, etc.)
	char speedThrottle; // if speedState is walk, else ignored
	uint32_t goalType; // is there no goal, a location or object?
	GameObjectWatchID goalObjectWID; // if our goal is an object...
	Stuff::Vector3D goalObjectPosition; // object's position at time of path calc
	Stuff::Vector3D goalLocation; // goal location, if any
	//----------------------
	// pathfinding guts/data
	float nextUpdate;
	bool newGoal;
	Stuff::Vector3D wayPath[MAX_WAYPTS]; // if this move order has a way path...
	char numWayPts; // how many way points?
	char curWayPt; // current goal waypoint
	char curWayDir; // waypath direction: 1 = forward, -1 = backward
	char pathType; // "quick" path or global/int32_t-range?
	Stuff::Vector3D originalGlobalGoal[2]; // 0 = original final goal, 1 = actual final goal
	Stuff::Vector3D globalGoalLocation;
	GlobalPathStep globalPath[MAX_GLOBAL_PATH];
	char numGlobalSteps;
	char curGlobalStep;
	MovePath path[2]; // 0 = current path, 1 = next path
	float timeOfLastStep; // last time a step was reached for this order
	int32_t moveState; // forward, reverse, paused, etc.
	int32_t moveStateGoal;
	bool twisting;
	float yieldTime;
	int32_t yieldState; // have we just yielded, been for a while, etc.
	float waitForPointTime;
	bool run;

	void copy(MoveOrders& orders)
	{
		time = orders.time;
		origin = orders.origin;
		speedType = orders.speedType;
		speedVelocity = orders.speedVelocity;
		speedState = orders.speedState;
		goalType = orders.goalType;
		goalObjectWID = orders.goalObjectWID;
		goalObjectPosition = orders.goalObjectPosition;
		goalLocation = orders.goalLocation;
		nextUpdate = orders.nextUpdate;
		newGoal = orders.newGoal;
		memcpy(wayPath, orders.wayPath, sizeof(Stuff::Vector3D) * MAX_WAYPTS);
		numWayPts = orders.numWayPts;
		curWayPt = orders.curWayPt;
		pathType = orders.pathType;
		originalGlobalGoal[0] = orders.originalGlobalGoal[0];
		originalGlobalGoal[1] = orders.originalGlobalGoal[1];
		globalGoalLocation = orders.globalGoalLocation;
		memcpy(globalPath, orders.globalPath, sizeof(GlobalPathStep) * MAX_GLOBAL_PATH);
		numGlobalSteps = orders.numGlobalSteps;
		curGlobalStep = orders.curGlobalStep;
		path[0] = *(orders.path[0]);
		path[1] = *(orders.path[1]);
		timeOfLastStep = orders.timeOfLastStep;
		moveState = orders.moveState;
		moveStateGoal = orders.moveStateGoal;
		twisting = orders.twisting;
		yieldTime = orders.yieldTime;
		yieldState = orders.yieldState;
		waitForPointTime = orders.waitForPointTime;
		run = orders.run;
	}

	void copyTo(MoveOrders& orders)
	{
		orders.time = time;
		orders.origin = origin;
		orders.speedType = speedType;
		orders.speedVelocity = speedVelocity;
		orders.speedState = speedState;
		orders.goalType = goalType;
		orders.goalObjectWID = goalObjectWID;
		orders.goalObjectPosition = goalObjectPosition;
		orders.goalLocation = goalLocation;
		orders.nextUpdate = nextUpdate;
		orders.newGoal = newGoal;
		memcpy(orders.wayPath, wayPath, sizeof(Stuff::Vector3D) * MAX_WAYPTS);
		orders.numWayPts = numWayPts;
		orders.curWayPt = curWayPt;
		orders.pathType = pathType;
		orders.originalGlobalGoal[0] = originalGlobalGoal[0];
		orders.originalGlobalGoal[1] = originalGlobalGoal[1];
		orders.globalGoalLocation = globalGoalLocation;
		memcpy(orders.globalPath, globalPath, sizeof(GlobalPathStep) * MAX_GLOBAL_PATH);
		orders.numGlobalSteps = numGlobalSteps;
		orders.curGlobalStep = curGlobalStep;
		for (size_t i = 0; i < 2; i++)
		{
			// orders.path[i] = new MovePath;
			if (!orders.path[i])
				Fatal(0, " No RAM for warrior path ");
			*(orders.path[i]) = path[i];
		}
		orders.timeOfLastStep = timeOfLastStep;
		orders.moveState = moveState;
		orders.moveStateGoal = moveStateGoal;
		orders.twisting = twisting;
		orders.yieldTime = yieldTime;
		orders.yieldState = yieldState;
		orders.waitForPointTime = waitForPointTime;
		orders.run = run;
	}

} SaveableMoveOrders;

typedef struct _AttackOrders
{
	float time;
	char origin;
	int32_t type; // to kill, to disable, etc.
	GameObjectWatchID targetWID; // current object targeted for attack, if attacking object
	Stuff::Vector3D targetpoint; // target location targeted for attack, if attacking point
	int32_t aimLocation;
	bool pursue; // does this attack entail a move order too?
	int32_t tactic;
	float targetTime;

	void init(void);

} AttackOrders;

typedef struct _SituationOrders
{
	float time;
	// general orders
	int32_t mode; // NORMAL, WITHDRAW, RETREAT, ETC.
	float defFormation;
	float curFormation;
	// fire orders
	bool openFire;
	bool uponFireOnly; // openFire set to TRUE upon getting attacked
	float fireRange;
	float fireOdds;
	// guard orders
	GameObjectWatchID guardObjectWID;

	void init(void);

} SituationOrders;

#if 0
typedef struct _RoleOrders
{
	float time;
	MechWarriorRole role;
} RoleOrders;
#endif

//------------------------------------------------------------------------------------------

#define NUM_MEMORY_CELLS 60

typedef union _MemoryCell {
	float real;
	int32_t integer;
} MemoryCell;

//------------------------------------------------------------------------------------------

#define MAX_ATTACKERS 50

//------------------------------------------------------------------------------------------

typedef struct _QueuedTacOrder
{
	int32_t id;
	Stuff::Vector3D point;
	uint32_t packedData[2];
	uint8_t tactic;
	BldgAppearance* marker;
	uint8_t moveMode; // So save/load can recreate this lovely pointer!
} QueuedTacOrder;

typedef QueuedTacOrder* QueuedTacOrderPtr;

//------------------------------------------------------------------------------------------

#define NUM_PILOT_DEBUG_STRINGS 5
#define MAXLEN_PILOT_DEBUG_STRING 80
#define MAXLEN_PILOT_NAME 80
#define MAXLEN_PILOT_CALLSIGN 20
#define MAXLEN_PILOT_PHOTO 20
#define MAXLEN_PILOT_VIDEO 20
#define MAXLEN_PILOT_AUDIO 20
#define MAXLEN_PILOT_BRAIN 40

enum class 
{
	GOAL_ACTION_NONE,
	GOAL_ACTION_UNDECIDED,
	GOAL_ACTION_MOVE,
	GOAL_ACTION_ATTACK,
	GOAL_ACTION_CAPTURE,
	GOAL_ACTION_GUARD,
	NUM_GOAL_ACTIONS
} GoalActionType;

typedef struct _MechWarriorData
{
	bool used;
	char name[MAXLEN_PILOT_NAME];
	char callsign[MAXLEN_PILOT_CALLSIGN];
	char videoStr[MAXLEN_PILOT_VIDEO];
	char audioStr[MAXLEN_PILOT_AUDIO];
	char brainStr[MAXLEN_PILOT_BRAIN];
	int32_t index;
	int32_t paintScheme;
	int32_t photoIndex;

	char rank;
	char skills[Skill::numberofskills]; // Current
	char professionalism;
	char professionalismModifier;
	char decorum;
	char decorumModifier;
	char aggressiveness;
	char courage;
	char baseCourage;

	float wounds;
	float health;
	int32_t status;
	bool escapesThruEjection;
	RadioLog radioLog;
	bool notMineYet; // Pilot must EARNED through logistics!!!

	int32_t teamId;
	GameObjectWatchID vehicleWID; // Must point to a Mover

	int32_t numSkillUses[Skill::numberofskills][NUM_COMBAT_STATS];
	int32_t numSkillSuccesses[Skill::numberofskills][NUM_COMBAT_STATS];
	int32_t numMechKills[NUM_VEHICLE_CLASSES][NUM_COMBAT_STATS];
	int32_t numPhysicalAttacks[NUM_PHYSICAL_ATTACKS][NUM_COMBAT_STATS];
	float skillRank[Skill::numberofskills];
	float skillPoints[Skill::numberofskills];
	char originalSkills[Skill::numberofskills];
	char startingSkills[Skill::numberofskills];
	bool specialtySkills[NUM_SPECIALTY_SKILLS];
	GameObjectWatchID killed[MAX_MOVERS / 3];
	int32_t numKilled;

	int32_t descID; // Used by Logistics to Desc.
	int32_t nameIndex; // Used by Logistics to Desc.

	float timeOfLastOrders; // when I ask for orders
	AttackerRec attackers[MAX_ATTACKERS]; // should never have more than 12 attackers...
	int32_t numAttackers;
	float attackRadius;

	MemoryCell memory[NUM_MEMORY_CELLS];
	char debugStrings[NUM_PILOT_DEBUG_STRINGS][MAXLEN_PILOT_DEBUG_STRING];

	float brainUpdate;
	float combatUpdate;
	float movementUpdate;
	int32_t weaponsStatus[MAX_WEAPONS_PER_MOVER];
	int32_t weaponsStatusResult;

	bool useGoalPlan;
	int32_t mainGoalAction;
	GameObjectWatchID mainGoalObjectWID;
	Stuff::Vector3D mainGoalLocation;
	float mainGoalControlRadius;
	int16_t lastGoalPathSize;
	int16_t lastGoalPath[MAX_GLOBAL_PATH];

	bool newTacOrderReceived[NUM_ORDERSTATES];
	TacticalOrder tacOrder[NUM_ORDERSTATES];
	TacticalOrder lastTacOrder;
	TacticalOrder curTacOrder;
	PilotAlarm alarm[NUM_PILOT_ALARMS];
	PilotAlarm alarmHistory[NUM_PILOT_ALARMS]; // used by brain update in ABL
	int32_t alarmPriority;
	bool curPlayerOrderFromQueue;
	bool tacOrderQueueLocked;
	bool tacOrderQueueExecuting;
	bool tacOrderQueueLooping;
	char numTacOrdersQueued; // Currently, only player orders may be queued...
	QueuedTacOrder tacOrderQueue[MAX_QUEUED_TACORDERS_PER_WARRIOR];
	int32_t tacOrderQueueIndex;
	int32_t nextTacOrderId;
	int32_t lastTacOrderId;

	int32_t coreScanTargetWID;
	int32_t coreAttackTargetWID;
	int32_t coreMoveTargetWID;
	TargetPriorityList targetPriorityList;
	int32_t brainState;
	bool willHelp;

	SaveableMoveOrders moveOrders;
	AttackOrders attackOrders;
	SituationOrders situationOrders;
	GameObjectWatchID lastTargetWID;
	float lastTargetTime;
	bool lastTargetObliterate;
	bool lastTargetFriendly;
	bool lastTargetConserveAmmo;
	bool keepMoving; // If true, he won't find his "optimal" spot and stand
		// there.

	char orderState;

	uint32_t debugFlags;

	uint8_t oldPilot;

	int32_t warriorBrainHandle;
} MechWarriorData;

typedef struct _StaticMechWarriorData
{
	int32_t numWarriors;
	int32_t numWarriorsInCombat;
	bool brainsEnabled[MAX_TEAMS];
	float minSkill;
	float maxSkill;
	int32_t increaseCap;
	float maxVisualRadius;
	int32_t curEventID;
	int32_t curEventTrigger;
} StaticMechWarriorData;

class MechWarrior
{

protected:
	// Misc.
	bool used;
	char name[MAXLEN_PILOT_NAME];
	char callsign[MAXLEN_PILOT_CALLSIGN];
	char videoStr[MAXLEN_PILOT_VIDEO];
	char audioStr[MAXLEN_PILOT_AUDIO];
	char brainStr[MAXLEN_PILOT_BRAIN];
	int32_t index;
	int32_t paintScheme;
	int32_t photoIndex;

	// Skills and Traits
	char rank;
	char professionalism;
	char professionalismModifier;
	char decorum;
	char decorumModifier;
	char aggressiveness;
	char courage;
	char baseCourage;

	// Status
	float wounds;
	float health;
	int32_t status;
	bool escapesThruEjection;
	RadioLog radioLog;
	bool notMineYet; // Pilot must EARNED through logistics!!!

	// Unit
	int32_t teamId;
	GameObjectWatchID vehicleWID; // Must point to a Mover

public:
	// Combat Stats and History
	int32_t numSkillUses[Skill::numberofskills][NUM_COMBAT_STATS];
	int32_t numSkillSuccesses[Skill::numberofskills][NUM_COMBAT_STATS];
	int32_t numMechKills[NUM_VEHICLE_CLASSES][NUM_COMBAT_STATS];
	int32_t numPhysicalAttacks[NUM_PHYSICAL_ATTACKS][NUM_COMBAT_STATS];
	char skills[Skill::numberofskills]; // Current
	float skillRank[Skill::numberofskills];
	float skillPoints[Skill::numberofskills];
	char originalSkills[Skill::numberofskills]; // How I arrived in MechCommander
	char startingSkills[Skill::numberofskills]; // How I arrived in the mission
	bool specialtySkills[NUM_SPECIALTY_SKILLS];
	GameObjectWatchID killed[MAX_MOVERS / 3];
	int32_t numKilled;

	int32_t descID; // Used by Logistics to Desc.
	int32_t nameIndex; // Used by Logistics to Desc.
	static SpecialtySkillType skillTypes[NUM_SPECIALTY_SKILLS];

protected:
	// AI
	float timeOfLastOrders; // when I ask for orders
	AttackerRec attackers[MAX_ATTACKERS]; // should never have more than 12 attackers...
	int32_t numAttackers;
	float attackRadius;
	static SortListPtr sortList;

	// ABL Brain and Memory
	MemoryCell memory[NUM_MEMORY_CELLS];
	const std::unique_ptr<ABLModule>& brain;
	int32_t warriorBrainHandle;
	const std::unique_ptr<SymTableNode>& brainAlarmCallback[NUM_PILOT_ALARMS];
	char debugStrings[NUM_PILOT_DEBUG_STRINGS][MAXLEN_PILOT_DEBUG_STRING];

	// Orders
	float brainUpdate;
	float combatUpdate;
	float movementUpdate;
	int32_t weaponsStatus[MAX_WEAPONS_PER_MOVER];
	int32_t weaponsStatusResult;

	bool useGoalPlan;
	int32_t mainGoalAction;
	GameObjectWatchID mainGoalObjectWID;
	Stuff::Vector3D mainGoalLocation;
	float mainGoalControlRadius;
	int16_t lastGoalPathSize;
	int16_t lastGoalPath[MAX_GLOBAL_PATH];

	bool newTacOrderReceived[NUM_ORDERSTATES];
	TacticalOrder tacOrder[NUM_ORDERSTATES];
	TacticalOrder lastTacOrder;
	TacticalOrder curTacOrder;
	PilotAlarm alarm[NUM_PILOT_ALARMS];
	PilotAlarm alarmHistory[NUM_PILOT_ALARMS]; // used by brain update in ABL
	int32_t alarmPriority;
	bool curPlayerOrderFromQueue;
	bool tacOrderQueueLocked;
	bool tacOrderQueueExecuting;
	bool tacOrderQueueLooping;
	char numTacOrdersQueued; // Currently, only player orders may be queued...
	QueuedTacOrderPtr tacOrderQueue;
	int32_t tacOrderQueueIndex;
	int32_t nextTacOrderId;
	int32_t lastTacOrderId;

	int32_t coreScanTargetWID;
	int32_t coreAttackTargetWID;
	int32_t coreMoveTargetWID;
	TargetPriorityList targetPriorityList;
	int32_t brainState;
	bool willHelp;

	MoveOrders moveOrders;
	AttackOrders attackOrders;
	SituationOrders situationOrders;
	// RoleOrders roleOrders;
	GameObjectWatchID lastTargetWID;
	float lastTargetTime;
	bool lastTargetObliterate;
	bool lastTargetFriendly;
	bool lastTargetConserveAmmo;
	bool keepMoving; // If true, he won't find his "optimal" spot and stand
		// there.

	char orderState;

	std::unique_ptr<PathQueueRec> movePathRequest;

	uint32_t debugFlags;

	RadioPtr radio;
	bool isPlayingMsg; // Always false unless I'm playing a message!
	uint8_t oldPilot;

	// std::unique_ptr<MechWarrior> next;

public:
	static int32_t numWarriors;
	static int32_t numWarriorsInCombat;
	static bool brainsEnabled[MAX_TEAMS];
	static float minSkill;
	static float maxSkill;
	static int32_t increaseCap;
	static float maxVisualRadius;
	static int32_t curEventID;
	static int32_t curEventTrigger;

	static MechWarrior* warriorList[MAX_WARRIORS];
	static GoalManager* goalManager;

	static BldgAppearance* wayPointMarkers[3];

public:
	PVOID operator new(size_t ourSize);
	void operator delete(PVOID us);

	static void setup(void);

	static void shutdown(void);

	static MechWarrior* newWarrior(void);

	static void freeWarrior(MechWarrior* warrior);

	static bool warriorInUse(const std::wstring_view& warriorName);

	void init(bool create);

	void init(MechWarriorData data);

	MechWarrior(void) { init(true); }

	void destroy(void);

	~MechWarrior(void) { destroy(void); }

	int32_t init(FitIniFile* warriorFile);

	void clear(void);

	void update(void);

	void updateMissionSkills(void);

	const std::wstring_view& getName(void) { return (name); }

	int32_t getPhoto(void) { return (photoIndex); }

	const std::wstring_view& getCallsign(void) { return (callsign); }

	void setIndex(int32_t i) { index = i; }

	int32_t getIndex(void) { return (index); }

	int32_t getPaintScheme(void) { return paintScheme; }

	void setPaintScheme(int32_t newPaintScheme) { paintScheme = newPaintScheme; }

	RadioPtr getRadio(void) { return radio; }

	void radioMessage(int32_t message, bool propogateIfMultiplayer = false);

	void cancelRadioMessage(int32_t message, bool propogateIfMultiplayer = false);

	uint32_t getLastMessage(void) { return radioLog.lastMessage; }

	char getRank(void) { return (rank); }

	uint32_t getSkill(uint32_t skillId) { return (skills[skillId]); }

	void setSkill(uint32_t skillId, int32_t skillValue)
	{
		if (skillId > 0 && skillId < Skill::numberofskills)
			skills[skillId] = skillValue;
	}

	int32_t checkSkill(int32_t skillId, float factor = 1.0);

	int32_t getNumSkillUse(int32_t skillStat, int32_t skillId)
	{
		return (numSkillUses[skillId][skillStat]);
	}

	int32_t getNumSkillSuccesses(int32_t skillStat, int32_t skillId)
	{
		return (numSkillSuccesses[skillId][skillStat]);
	}

	void incNumSkillUses(int32_t skillStat, int32_t skillId) { numSkillUses[skillId][skillStat]++; }

	void incNumSkillSuccesses(int32_t skillStat, int32_t skillId)
	{
		numSkillSuccesses[skillId][skillStat]++;
	}

	int32_t getProfessionalism(void) { return (professionalism); }

	int32_t getProfessionalismModifier(void) { return (professionalismModifier); }

	int32_t getDecorum(void) { return (decorum); }

	int32_t getDecorumModifier(void) { return (decorumModifier); }

	int32_t getAggressiveness(bool current = true);

	int32_t getCourage(void) { return (courage); }

	void setCourage(uint8_t _courage) { courage = _courage; }

	int32_t getTeamworkModifier(void) { return (professionalismModifier + decorumModifier); }

	float getWounds(void) { return (wounds); }

	bool alive(void) { return (wounds < 6.0); }

	bool active(void) { return (status == WARRIOR_STATUS_NORMAL); }

	void setStatus(int32_t _status) { status = _status; }

	int32_t getStatus(void) { return (status); }

	bool hasEjected(void)
	{
		return (status == WARRIOR_STATUS_EJECTED || status == WARRIOR_STATUS_MIA || status == WARRIOR_STATUS_CAPTURED);
	}

	void setEscapesThruEjection(bool escapes) { escapesThruEjection = escapes; }

	bool getEscapesThruEjection(void) { return (escapesThruEjection); }

	void setKeepMoving(bool set) { keepMoving = set; }

	bool getKeepMoving(void) { return (keepMoving); }

	bool getNotMineYet(void) { return notMineYet; }

	void setNotMineYet(bool result) { notMineYet = result; }

	bool getSpecialtySkill(int32_t whichSkill) { return (specialtySkills[whichSkill]); }

	//---------------------------------
	// Specialty Skills
	bool isLightMechSpecialist(void) // DONE
	{
		return (specialtySkills[0] == 1);
	}

	bool isMediumMechSpecialist(void) // DONE
	{
		return (specialtySkills[8] == 1);
	}

	bool isLaserSpecialist(void) // DONE
	{
		return (specialtySkills[1] == 1);
	}

	bool isLightACSpecialist(void) // DONE
	{
		return (specialtySkills[2] == 1);
	}

	bool isMediumACSpecialist(void) // DONE
	{
		return (specialtySkills[3] == 1);
	}

	bool isSRMSpecialist(void) // DONE
	{
		return (specialtySkills[4] == 1);
	}

	bool isSmallArmsSpecialist(void) // DONE
	{
		return (specialtySkills[5] == 1);
	}

	bool isToughnessSpecialist(void) { return (specialtySkills[7] == 1); }

	bool isHevayMechSpecialist(void) // DONE
	{
		return (specialtySkills[14] == 1);
	}

	bool isPulseLaserSpecialist(void) // DONE
	{
		return (specialtySkills[9] == 1);
	}

	bool isERLaserSpecialist(void) // DONE
	{
		return (specialtySkills[10] == 1);
	}

	bool isLRMSpecialist(void) // DONE
	{
		return (specialtySkills[11] == 1);
	}

	bool isPPCSpecialist(void) // DONE
	{
		return (specialtySkills[15] == 1);
	}

	bool isScout(void) { return (specialtySkills[12] == 1); }

	bool isLongJump(void) { return (specialtySkills[13] == 1); }

	bool isAssaultMechSpecialist(void) // DONE
	{
		return (specialtySkills[20] == 1);
	}

	bool isGaussSpecialist(void) // DONE
	{
		return (specialtySkills[21] == 1);
	}

	bool isHeavyACSpecialist(void) // DONE
	{
		return (specialtySkills[16] == 1);
	}

	bool isShortRangeSpecialist(void) // DONE
	{
		return (specialtySkills[17] == 1);
	}

	bool isMediumRangeSpecialist(void) // DONE
	{
		return (specialtySkills[18] == 1);
	}

	bool isLongRangeSpecialist(void) // DONE
	{
		return (specialtySkills[19] == 1);
	}

	/* CUT~!!!!!!!!!
	bool isDeadeye (void)
	{
	return (specialtySkills[23] == 1);
	}
	*/

	bool isSharpShooter(void) { return (specialtySkills[22] == 1); }

	bool isSensorProfileSpecialist(void) { return (specialtySkills[6] == 1); }

	//--------------------------
	// TacOrderQueue routines...

	bool enableTacOrderQueue(void);

	int32_t addQueuedTacOrder(TacticalOrder tacOrder);

	int32_t removeQueuedTacOrder(TacticalOrderPtr tacOrder);

	int32_t getNextQueuedTacOrder(TacticalOrderPtr tacOrder);

	int32_t peekQueuedTacOrder(int32_t index, TacticalOrderPtr tacOrder);

	void clearTacOrderQueue(void);

	void setUseGoalPlan(bool set) { useGoalPlan = set; }

	bool getUseGoalPlan(void) { return (useGoalPlan); }

	void setMainGoal(int32_t action, GameObjectPtr obj, Stuff::Vector3D* location, float range);

	int32_t getMainGoal(GameObjectPtr& obj, Stuff::Vector3D& location, float& range);

	int32_t getNumTacOrdersQueued(void) { return (numTacOrdersQueued); }

	void drawWaypointPath(void);
	void updateDrawWaypointPath(void);

	void executeTacOrderQueue(void);

	void setExecutingQueue(bool setting) { tacOrderQueueExecuting = setting; }

	void setTacOrderQueueLooping(bool setting) { tacOrderQueueLooping = setting; }

	bool getExecutingTacOrderQueue(void) { return (tacOrderQueueExecuting); }

	bool getTacOrderQueueLooping(void) { return (tacOrderQueueLooping); }

	void lockTacOrderQueue(void);

	void unlockTacOrderQueue(void);

	bool getTacOrderQueueLocked(void) { return (tacOrderQueueLocked); }

	int32_t getTacOrderQueue(QueuedTacOrderPtr list);

	bool getCurPlayerOrderFromQueue(void) { return (curPlayerOrderFromQueue); }

	int32_t getNextTacOrderId(void) { return (nextTacOrderId); }

	void setNextTacOrderId(int32_t newId) { nextTacOrderId = newId; }

	int32_t getLastTacOrderId(void) { return (lastTacOrderId); }

	void setLastTacOrderId(int32_t newId) { lastTacOrderId = newId; }

	void updateClientOrderQueue(int32_t curTacOrderId);

	//------------------
	// Group routines...

	MoverGroupPtr getGroup(void);

	std::unique_ptr<Mover> getPoint(void);

	TeamPtr getTeam(void);

	CommanderPtr getCommander(void);

	const std::wstring_view& getAudioString(void) { return audioStr; }

	const std::wstring_view& getVideoString(void) { return videoStr; }

	const std::wstring_view& getBrainString(void) { return brainStr; }

	int32_t getPilotPhoto(void) { return photoIndex; }

	bool onHomeTeam(void);

	bool underHomeCommand(void);

	std::unique_ptr<Mover> getVehicle(void);

	GameObjectPtr calcTurretThreats(float threatRange, int32_t minThreat);

	int32_t getVehicleStatus(void);

	int32_t getWeaponsStatus(int32_t* list)
	{
		memcpy(list, weaponsStatus, MAX_WEAPONS_PER_MOVER * sizeof(int32_t));
		return (weaponsStatusResult);
	}

	// void setUnit (MoverGroupPtr _group, int32_t _unitMateId);

	void setTeam(TeamPtr team);

	void setVehicle(GameObjectPtr _vehicle);

	void setWounds(float _wounds) { wounds = _wounds; }

	int32_t setBrain(int32_t brainHandle);

	void setBrainName(const std::wstring_view& brainName);

	const std::unique_ptr<ABLModule>& getBrain(void) { return (brain); }

	int32_t runBrain(void);

	int32_t loadBrainParameters(FitIniFile* brainFile, int32_t warriorId);

	bool injure(float numWounds, bool checkEject = true);

	void eject(void);

	float getTimeOfLastOrders(void) { return (timeOfLastOrders); }

	float getAttackRadius(void) { return (attackRadius); }

	void setAttackRadius(float radius) { attackRadius = radius; }

	void setIntegerMemory(int32_t index, int32_t val) { memory[index].integer = val; }

	void setRealMemory(int32_t index, float val) { memory[index].real = val; }

	int32_t getIntegerMemory(int32_t index) { return (memory[index].integer); }

	float getRealMemory(int32_t index) { return (memory[index].real); }

	void updateAttackerStatus(uint32_t attackerWID, float time);

	AttackerRecPtr getAttackerInfo(uint32_t attackerWID);

	int32_t getAttackers(uint32_t* attackerList, float seconds);

	int32_t scanOwnVehicle(void);

	void lobotomy(void); // Whacks the ABL brain and just blindly follows last order.

	//-----------
	// Tac Orders

	TacticalOrderPtr getCurTacOrder(void) { return (&curTacOrder); }

	TacticalOrderPtr getLastTacOrder(void) { return (&lastTacOrder); }

	TacticalOrderPtr getTacOrder(int32_t _orderState) { return (&tacOrder[_orderState]); }

	int32_t getOrderState(void) { return (orderState); }

	void setOrderState(int32_t state) { orderState = state; }

	//------------
	// Move Orders

	MoveOrders* getMoveOrders(void) { return (&moveOrders); }

	void setMoveOrigin(int32_t origin) { moveOrders.origin = origin; }

	int32_t getMoveOrigin(void) { return (moveOrders.origin); }

	void setMoveSpeedType(int32_t type);

	int32_t getMoveSpeedType(void) { return (moveOrders.speedType); }

	void setMoveSpeedVelocity(float speed);

	float getMoveSpeedVelocity(void) { return (moveOrders.speedVelocity); }

	int32_t getMoveSpeedState(void) { return (moveOrders.speedState); }

	int32_t getMoveSpeedThrottle(void) { return (moveOrders.speedThrottle); }

	int32_t setMoveGoal(uint32_t type, Stuff::Vector3D* location, GameObjectPtr obj = nullptr);

	uint32_t getMoveGoal(Stuff::Vector3D* location = nullptr, GameObjectPtr* obj = nullptr);

	bool getMoveGlobalGoal(Stuff::Vector3D& location)
	{
		if (moveOrders.pathType != MOVEPATH_UNDEFINED)
		{
			location = moveOrders.originalGlobalGoal[1];
			return (true);
		}
		return (false);
	}

	void clearMoveGoal(void)
	{
		moveOrders.origin = ORDER_ORIGIN_COMMANDER;
		moveOrders.goalType = MOVEGOAL_NONE;
		moveOrders.goalObjectWID = 0;
		moveOrders.goalLocation.x = -999999.0;
		moveOrders.goalLocation.y = -999999.0;
		moveOrders.goalLocation.z = -999999.0;
	}

	void clearMovePath(int32_t pathNum); /* {
										  if (moveOrders.path[pathNum])
										  moveOrders.path[pathNum]->clear(void);
										  }*/

	bool hasMoveGoal(void) { return (moveOrders.goalType != MOVEGOAL_NONE); }

	bool getMoveNewGoal(void) { return (moveOrders.newGoal); }

	void setMoveNewGoal(bool state) { moveOrders.newGoal = state; }

	MovePathPtr getMovePath(int32_t which) { return (moveOrders.path[which]); }

	MovePathPtr getMovePath(void);

	void setMoveGoalObjectPos(Stuff::Vector3D pos) { moveOrders.goalObjectPosition = pos; }

	Stuff::Vector3D getMoveGoalObjectPos(void) { return (moveOrders.goalObjectPosition); }

	void setMoveRun(bool run) { moveOrders.run = run; }

	bool getMoveRun(void) { return (moveOrders.run); }

	void setMovePathRequest(std::unique_ptr<PathQueueRec> rec) { movePathRequest = rec; }

	std::unique_ptr<PathQueueRec> getMovePathRequest(void) { return (movePathRequest); }

	void pausePath(void);

	void resumePath(void);

	void rethinkPath(uint32_t strategy);

	void setMoveState(int32_t state) { moveOrders.moveState = state; }

	int32_t getMoveState(void) { return (moveOrders.moveState); }

	void setMoveTimeOfLastStep(float t) { moveOrders.timeOfLastStep = t; }

	int32_t getMoveTimeOfLastStep(void) { return (moveOrders.timeOfLastStep); }

	void setMoveStateGoal(int32_t state) { moveOrders.moveStateGoal = state; }

	int32_t getMoveStateGoal(void) { return (moveOrders.moveStateGoal); }

	void setMoveTwisting(bool set) { moveOrders.twisting = set; }

	bool getMoveTwisting(void) { return (moveOrders.twisting); }

	void setMoveYieldTime(float time) { moveOrders.yieldTime = time; }

	void setMoveYieldState(int32_t state) { moveOrders.yieldState = state; }

	float getMoveYieldTime(void) { return (moveOrders.yieldTime); }

	int32_t getMoveYieldState(void) { return (moveOrders.yieldState); }

	bool isYielding(void) { return (moveOrders.yieldTime > -1.0); }

	void setMoveWaitForPointTime(float time) { moveOrders.waitForPointTime = time; }

	float getMoveWaitForPointTime(void) { return (moveOrders.waitForPointTime); }

	bool isWaitingForPoint(void) { return (moveOrders.waitForPointTime > -1.0); }

	float getMoveDistanceLeft(void);

	bool isJumping(Stuff::Vector3D* jumpGoal = nullptr);

	void setWillHelp(bool setting) { willHelp = setting; }

	bool getWillHelp(void) { return (willHelp); }

	int32_t getMovePathType(void) { return (moveOrders.pathType); }

	void setMoveWayPath(WayPathPtr wayPath, bool patrol = false);

	void addMoveWayPoint(Stuff::Vector3D wayPt, bool patrol = false);

	void setMoveGlobalPath(GlobalPathStepPtr path, int32_t numSteps);

	int32_t getMoveNumGlobalSteps(void) { return (moveOrders.numGlobalSteps); }

	int32_t getMoveCurGlobalStep(void) { return (moveOrders.curGlobalStep); }

	int32_t getMovePathGlobalStep(void) { return (moveOrders.path[0]->globalStep); }

	void requestMovePath(int32_t selectionindex, uint32_t moveparams, int32_t source);

	int32_t calcMovePath(int32_t selectionindex, uint32_t moveparams = MOVEPARAM_NONE);

	int32_t calcMoveSpeedState(void)
	{
		//-------------------------------------------------
		// Assumes we want to go as fast as orders allow...
		return (moveOrders.speedState);
	}

	int32_t calcMoveSpeedThrottle(int32_t speedState, int32_t speedThrottle)
	{
		//----------------------------------------------------------
		// Always Assume 100 percent in MC2 for now --fs
		return (100);
	}

	bool getNextWayPoint(Stuff::Vector3D& nextPoint, bool incWayPoint);

	//--------------
	// Attack Orders

	void setAttackOrigin(int32_t origin) { attackOrders.origin = origin; }

	int32_t getAttackOrigin(void) { return (attackOrders.origin); }

	void setAttackType(int32_t type) { attackOrders.type = type; }

	int32_t getAttackType(void) { return (attackOrders.type); }

	void changeAttackRange(int32_t newRange);

	int32_t setAttackTarget(GameObjectPtr object);

	GameObjectPtr getLastTarget(void);

	void setLastTarget(GameObjectPtr target, bool obliterate = false, bool conserveAmmo = 0);

	float getLastTargetTime(void) { return (lastTargetTime); }

	GameObjectPtr getAttackTarget(void);

	GameObjectPtr getCurrentTarget(void) { return (getLastTarget()); }

	void setCurrentTarget(GameObjectPtr target);

	void setAttackPursuit(bool pursue) { attackOrders.pursue = pursue; }

	bool getAttackPursuit(void) { return (attackOrders.pursue); }

	void setAttackAimLocation(int32_t location) { attackOrders.aimLocation = location; }

	int32_t getAttackAimLocation(void) { return (attackOrders.aimLocation); }

	void setAttackTargetPoint(Stuff::Vector3D location) { attackOrders.targetpoint = location; }

	Stuff::Vector3D getAttackTargetPoint(void) { return (attackOrders.targetpoint); }

	float getAttackTargetTime(void) { return (attackOrders.targetTime); }

	GameObjectPtr getAttackTargetPosition(Stuff::Vector3D& pos);

	void clearAttackOrders(void);

	void clearMoveOrders(void);

	//-----------------
	// Situation Orders

	int32_t getSituationMode(void) { return (situationOrders.mode); }

	void setSituationMode(int32_t mode) { situationOrders.mode = mode; }

	float getSituationDefFormation(void) { return (situationOrders.defFormation); }

	void setSituationDefFormation(float distance) { situationOrders.defFormation = distance; }

	float getSituationCurFormation(void) { return (situationOrders.curFormation); }

	void setSituationCurFormation(float distance) { situationOrders.curFormation = distance; }

	bool getSituationOpenFire(void) { return (situationOrders.openFire); }

	void setSituationOpenFire(bool openFire) { situationOrders.openFire = openFire; }

	float getSituationFireRange(void) { return (situationOrders.fireRange); }

	void setSituationFireRange(float range) { situationOrders.fireRange = range; }

	float getSituationFireOdds(void) { return (situationOrders.fireOdds); }

	void setSituationFireOdds(float odds) { situationOrders.fireOdds = odds; }

	GameObjectPtr getSituationGuardObject(void);

	void setSituationGuardObject(GameObjectWatchID objWID);

	uint32_t getSituationGuardObjectPartId(void);

	void setMessagePlaying() { isPlayingMsg = true; }

	void clearMessagePlaying() { isPlayingMsg = false; }

	bool getMessagePlaying() { return isPlayingMsg; }

#if 0
	//-------------
	// Role Orders

	int32_t getRole(void)
	{
		return(roleOrders.role);
	}

	void setRole(int32_t role)
	{
		roleOrders.role = (MechWarriorRole)role;
	}
#endif

	//--------------------
	// Command/Observation
	int32_t executeTacticalOrder(TacticalOrderPtr order = nullptr);

	int32_t calcWeaponsStatus(
		GameObjectPtr target, int32_t* weaponList, _In_ DirectX::XMFLOAT3& targetpoint);

	void printWeaponsStatus(const std::wstring_view& s);

	int32_t combatDecisionTree(void);

	Stuff::Vector3D calcWithdrawGoal(float withdrawRange = 1000.0);

	bool movingOverBlownBridge(void);

	bool movementDecisionTree(void);

	void collisionAlert(GameObjectPtr obstacle, float distance, float timeToImpact);

	int32_t triggerAlarm(int32_t alarmCode, uint32_t triggerId = 0);

	int32_t handleAlarm(int32_t alarmCode, uint32_t triggerId = 0);

	int32_t getAlarmTriggers(int32_t alarmCode, uint32_t* triggerList);

	int32_t getAlarmTriggersHistory(int32_t alarmCode, uint32_t* triggerList);

	void clearAlarm(int32_t alarmCode);

	void clearAlarms(void);

	void clearAlarmHistory(int32_t alarmCode);

	void clearAlarmsHistory(void);

	void clearCurTacOrder(bool updateTacOrder = true);

	void setCurTacOrder(TacticalOrder tacOrder);

	void setGeneralTacOrder(TacticalOrder tacOrder);

	void setPlayerTacOrder(TacticalOrder tacOrder, bool fromQueue = false);

	void setAlarmTacOrder(TacticalOrder tacOrder, int32_t priority);

	int32_t checkCommandDecision(void);

	int32_t checkAlarms(void);

	void updateActions(void);

	int32_t mainDecisionTree(void);

	void setDebugFlags(uint32_t flags) { debugFlags = flags; }

	void setDebugFlag(uint32_t flag, bool on);

	bool getDebugFlag(uint32_t flag);

	void debugPrint(const std::wstring_view& s, bool debugMode = false);

	void debugOrders(void);

	void initTargetPriorities(void);

	int32_t setTargetPriority(
		int32_t index, int32_t type, int32_t param1, int32_t param2, int32_t param3);

	int32_t insertTargetPriority(
		int32_t index, int32_t type, int32_t param1, int32_t param2, int32_t param3);

	int32_t setBrainState(int32_t newState);

	int32_t getBrainState(void);

	int32_t getEventHistory(int32_t alarmCode, int32_t* paramList);

	int32_t getNextEventHistory(int32_t* paramList);

	//--------------
	// CORE COMMANDS

	int32_t getCoreScanTargetWID(void) { return (coreScanTargetWID); }

	int32_t coreMoveTo(Stuff::Vector3D location, uint32_t params);

	int32_t coreMoveToObject(GameObjectPtr object, uint32_t params);

	int32_t coreEject(void);

	int32_t corePower(bool powerUp);

	int32_t coreAttack(GameObjectPtr target, uint32_t params);

	int32_t coreCapture(GameObjectPtr object, uint32_t params);

	int32_t coreScan(GameObjectPtr object, uint32_t params);

	int32_t coreControl(GameObjectPtr object, uint32_t params);

	// int32_t coreWithdraw (Stuff::Vector3D location, uint32_t params);

	int32_t coreSetState(int32_t stateID, bool thinkAgain);

	//----------------
	// Core Tac Orders

	int32_t orderWait(
		bool unitOrder, int32_t origin, int32_t seconds = 1969000.0, bool clearLastTarget = false);

	int32_t orderStop(bool unitOrder, bool setTacOrder);

	int32_t orderMoveToPoint(bool unitOrder, bool setTacOrder, int32_t origin,
		Stuff::Vector3D location, int32_t selectionindex = -1,
		uint32_t params = TACORDER_PARAM_NONE);

	int32_t orderFormation(bool unitOrder, bool setTacOrder, int32_t origin);

	int32_t orderMoveToObject(bool unitOrder, bool setTacOrder, int32_t origin,
		GameObjectPtr target, int32_t fromArea, int32_t selectionindex = -1,
		uint32_t params = TACORDER_PARAM_FACE_OBJECT);

	int32_t orderJumpToPoint(bool unitOrder, bool setTacOrder, int32_t origin,
		Stuff::Vector3D location, int32_t selectionindex = -1);

	int32_t orderJumpToObject(bool unitOrder, bool setTacOrder, int32_t origin,
		GameObjectPtr target, int32_t selectionindex = -1);

	int32_t orderTraversePath(bool unitOrder, bool setTacOrder, int32_t origin, WayPathPtr wayPath,
		uint32_t params = TACORDER_PARAM_NONE);

	int32_t orderPatrolPath(bool unitOrder, bool setTacOrder, int32_t origin, WayPathPtr wayPath);

	int32_t orderPowerUp(bool unitOrder, int32_t origin);

	int32_t orderPowerDown(bool unitOrder, int32_t origin);

	int32_t orderUseSpeed(float speed);

	int32_t orderUsePattern(int32_t pattern);

	int32_t orderOrbitPoint(Stuff::Vector3D location);

	int32_t orderOrbitObject(GameObjectPtr target);

	int32_t orderUseOrbitRange(int32_t type, float range);

	int32_t orderAttackObject(bool unitOrder, int32_t origin, GameObjectPtr target, int32_t type,
		int32_t method, int32_t range, int32_t aimLocation = -1, int32_t fromArea = -1,
		uint32_t params = TACORDER_PARAM_NONE);

	int32_t orderAttackPoint(bool unitOrder, int32_t origin, Stuff::Vector3D location, int32_t type,
		int32_t method, int32_t range, uint32_t params = TACORDER_PARAM_NONE);

	int32_t orderWithdraw(bool unitOrder, int32_t origin, Stuff::Vector3D location);

	int32_t orderEject(bool unitOrder, bool setTacOrder, int32_t origin);

	// int32_t orderUseFireOdds (int32_t odds);

	int32_t orderRefit(int32_t origin, GameObjectPtr target, uint32_t params = TACORDER_PARAM_NONE);

	int32_t orderRecover(
		int32_t origin, GameObjectPtr target, uint32_t params = TACORDER_PARAM_NONE);

	int32_t orderGetFixed(
		int32_t origin, GameObjectPtr target, uint32_t params = TACORDER_PARAM_NONE);

	int32_t orderLoadIntoCarrier(
		int32_t origin, GameObjectPtr target, uint32_t params = TACORDER_PARAM_NONE);

	int32_t orderDeployElementals(int32_t origin, uint32_t params = TACORDER_PARAM_NONE);

	int32_t orderCapture(int32_t origin, GameObjectPtr target, int32_t fromArea = -1,
		uint32_t params = TACORDER_PARAM_NONE);

	//--------------
	// Combat Events

	int32_t handleTargetOfWeaponFire(void);

	int32_t handleHitByWeaponFire(void);

	int32_t handleDamageTakenRate(void);

	int32_t handleFriendlyVehicleCrippled(void);

	int32_t handleFriendlyVehicleDestruction(void);

	int32_t handleOwnVehicleIncapacitation(uint32_t cause);

	int32_t handleOwnVehicleDestruction(uint32_t cause);

	int32_t handleOwnVehicleWithdrawn(void);

	int32_t handleAttackOrder(void);

	int32_t handleCollisionAlert(void);

	int32_t handleCollision(void);

	int32_t handleKilledTarget(void);

	int32_t handleUnitMateFiredWeapon(void);

	int32_t handleUnitMateDeath(void);

	int32_t handlePlayerOrder(void);

	int32_t handleNoMovePath(void);

	int32_t handleGateClosing(void);

	int32_t handleFiredWeapon(void);

	int32_t handleNewMover(void);

	//---------------
	// Status Windows
	int32_t openStatusWindow(int32_t x, int32_t y, int32_t w, int32_t h);

	int32_t closeStatusWindow(void);

	int32_t missionLog(std::unique_ptr<File> file, int32_t unitLevel);

	void calcRank(void);

	bool isCloseToFirstTacOrder(Stuff::Vector3D& pos);

	BldgAppearance* getWayPointMarker(const Stuff::Vector3D& pos, const std::wstring_view& name);

	void setDebugString(int32_t stringNum, const std::wstring_view& s)
	{
		if ((stringNum > -1) && (stringNum < NUM_PILOT_DEBUG_STRINGS))
			if (s == nullptr)
				debugStrings[stringNum][0] = nullptr;
			else
				strncpy(debugStrings[stringNum], s, MAXLEN_PILOT_DEBUG_STRING - 1);
	}

	const std::wstring_view& getDebugString(int32_t stringNum)
	{
		if ((stringNum > -1) && (stringNum < NUM_PILOT_DEBUG_STRINGS))
			return (debugStrings[stringNum]);
		return (nullptr);
	}

	int32_t calcTacOrder(int32_t goalAction, int32_t goalWID, Stuff::Vector3D goalLocation,
		float controlRange, int32_t aggressiveness, int32_t searchDepth, float turretRange,
		int32_t turretThreat, TacticalOrder& tacOrder);

	static void initGoalManager(int32_t poolSize);

	static void logPilots(GameLogPtr log);

	static bool anyPlayerInCombat(void);

	void copyToData(MechWarriorData& data);
	void copyFromData(MechWarriorData& data);

	void save(PacketFilePtr file, int32_t packetNum);
	void load(PacketFilePtr file, int32_t packetNum);

	int32_t static Save(PacketFilePtr file, int32_t packetNum);
	int32_t static Load(PacketFilePtr file, int32_t packetNum);
};

//---------------------------------------------------------------------------

extern MovePathManagerPtr PathManager;

void
SeedRandomNumbers(void);
int32_t
RandomNumber(int32_t range);
bool
RollDice(int32_t percent);
int32_t
SignedRandomNumber(int32_t range);
int32_t
GetMissionTurn(void);

//***************************************************************************

#endif
