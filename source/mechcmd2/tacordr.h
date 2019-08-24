//******************************************************************************************
//
//	tacordr.h - This file contains the Tactical Orders header
//
//	MechCommander 2
//
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#pragma once

#ifndef TACORDR_H
#define TACORDR_H

//--------------

//#include "gameobj.h"
//#include "dgroup.h"
//#include "dwarrior.h"
//#include "dmover.h"
//#include "dtacordr.h"

//***************************************************************************

#define TACORDER_FAILURE 0
#define TACORDER_SUCCESS 1

enum class MoveSpeedType : uint8_t
{
	MOVE_SPEED_BEST,
	MOVE_SPEED_MAXIMUM,
	MOVE_SPEED_SLOW,
	MOVE_SPEED_MODERATE,
	NUM_MOVE_SPEEDS
};

enum class MovePatternType : uint8_t
{
	MOVE_PATTERN_DIRECT,
	MOVE_PATTERN_ZIGZAG_NARROW,
	MOVE_PATTERN_ZIGZAG_MEDIUM,
	MOVE_PATTERN_ZIGZAG_WIDE,
	MOVE_PATTERN_RANDOM_NARROW,
	MOVE_PATTERN_RANDOM_MEDIUM,
	MOVE_PATTERN_RANDOM_WIDE,
	NUM_MOVE_PATTERNS
};

enum class AttitudeType : uint8_t
{
	ATTITUDE_CAUTIOUS,
	ATTITUDE_CONSERVATIVE,
	ATTITUDE_NORMAL,
	ATTITUDE_AGGRESSIVE,
	ATTITUDE_BERSERKER,
	ATTITUDE_SUICIDAL,
	NUM_ATTITUDES
};

//#define	FIRERANGE_OPTIMUM	-1

enum class FireRangeType : uint8_t
{
	FIRERANGE_DEFAULT = -4,
	FIRERANGE_RAMMING,
	FIRERANGE_LONGEST,
	FIRERANGE_OPTIMAL,
	FIRERANGE_SHORT,
	FIRERANGE_MEDIUM,
	FIRERANGE_LONG,
	FIRERANGE_CURRENT,
	NUM_FIRERANGES
};

enum class AttackType : uint8_t
{
	ATTACK_NONE,
	ATTACK_TO_DESTROY,
	ATTACK_TO_DISABLE,
	ATTACK_CONSERVING_AMMO,
	NUM_ATTACK_TYPES
};

enum class AttackMethod : uint8_t
{
	ATTACKMETHOD_RANGED,
	ATTACKMETHOD_DFA,
	ATTACKMETHOD_RAMMING,
	NUM_ATTACKMETHODS
};

enum class FireOddsType : uint8_t
{
	FIREODDS_LUCKY,
	FIREODDS_BAD,
	FIREODDS_MEDIUM,
	FIREODDS_GOOD,
	FIREODDS_GREAT,
	NUM_FIREODDS
};

enum class TacticType : uint8_t
{
	TACTIC_NONE,
	TACTIC_FLANK_LEFT,
	TACTIC_FLANK_RIGHT,
	TACTIC_FLANK_REAR,
	TACTIC_STOP_AND_FIRE,
	TACTIC_TURRET,
	TACTIC_JOUST,
	NUM_TACTICS
};

enum class OrderOriginType : uint8_t
{
	ORDER_ORIGIN_PLAYER,
	ORDER_ORIGIN_COMMANDER,
	ORDER_ORIGIN_SELF
};

enum class TacticalOrderCode : uint8_t
{
	TACTICAL_ORDER_NONE,
	TACTICAL_ORDER_WAIT,
	TACTICAL_ORDER_MOVETO_POINT,
	TACTICAL_ORDER_MOVETO_OBJECT,
	TACTICAL_ORDER_JUMPTO_POINT,
	TACTICAL_ORDER_JUMPTO_OBJECT,
	TACTICAL_ORDER_TRAVERSE_PATH,
	TACTICAL_ORDER_PATROL_PATH,
	TACTICAL_ORDER_ESCORT,
	TACTICAL_ORDER_FOLLOW,
	TACTICAL_ORDER_GUARD,
	TACTICAL_ORDER_STOP,
	TACTICAL_ORDER_POWERUP,
	TACTICAL_ORDER_POWERDOWN,
	TACTICAL_ORDER_WAYPOINTS_DONE,
	TACTICAL_ORDER_EJECT,
	TACTICAL_ORDER_ATTACK_OBJECT,
	TACTICAL_ORDER_ATTACK_POINT,
	TACTICAL_ORDER_HOLD_FIRE,
	TACTICAL_ORDER_WITHDRAW,
	TACTICAL_ORDER_SCRAMBLE,
	TACTICAL_ORDER_CAPTURE,
	TACTICAL_ORDER_REFIT,
	TACTICAL_ORDER_GETFIXED,
	TACTICAL_ORDER_LOAD_INTO_CARRIER,
	TACTICAL_ORDER_DEPLOY_ELEMENTALS,
	TACTICAL_ORDER_RECOVER,
	NUM_TACTICAL_ORDERS // IF THIS CHANGES, ADD GUARD_POINT AS SEP. ORDER (see
	// pack/unpack hack)
};

//***************************************************************************

struct LocationNode
{
	Stuff::Vector3D location;
	bool run;
	LocationNodePtr next;
};

enum class TravelModeType : uint8_t
{
	slow,
	fast,
	jump,
	numberoftypes,
	invalid = UCHAR_MAX	// not used
};

enum class SpecialMoveMode : uint8_t
{
	normal,
	minelaying
};

typedef struct _WayPath
{
	int32_t numPoints;
	int32_t curPoint;
	float points[3 * MAX_WAYPTS];
	uint8_t mode[MAX_WAYPTS];
} WayPath;

typedef WayPath* WayPathPtr;

typedef struct _TacOrderMoveParams
{
	WayPath wayPath;
	bool faceObject;
	bool wait;
	SpecialMoveMode mode;
	bool escapeTile;
	bool jump;
	int32_t fromArea;
	bool keepMoving;
} TacOrderMoveParams;

typedef struct _TacOrderAttackParams
{
	AttackType type; // NO LONGER USED! 1/7/00
	AttackMethod method;
	FireRangeType range;
	TacticType tactic;
	int32_t aimLocation;
	bool pursue;
	bool obliterate;
	Stuff::Vector3D targetpoint;
} TacOrderAttackParams;

class TacticalOrder
{

public:
	int32_t id;
	float time;
	float delayedTime;
	float lastTime;
	bool unitOrder; // TRUE if unit order, else individual order
	bool subOrder;
	OrderOriginType origin;
	TacticalOrderCode code;
	TacOrderMoveParams moveparams;
	TacOrderAttackParams attackParams;
	GameObjectWatchID targetWID;
	int32_t targetObjectClass;
	int32_t selectionindex;
	char stage;
	char statusCode;
	Stuff::Vector3D lastMoveGoal;

	// for network use only
	char pointLocalMoverId;
	uint32_t groupFlags;
	uint32_t data[2];

public:
	PVOID operator new(size_t ourSize);
	void operator delete(PVOID us);

	void operator=(TacticalOrder copy)
	{
		time = copy.time;
		delayedTime = copy.delayedTime;
		lastTime = copy.lastTime;
		unitOrder = copy.unitOrder;
		origin = copy.origin;
		code = copy.code;
		moveparams = copy.moveparams;
		attackParams = copy.attackParams;
		targetWID = copy.targetWID;
		targetObjectClass = copy.targetObjectClass;
		selectionindex = copy.selectionindex;
		stage = copy.stage;
		subOrder = copy.subOrder;
		lastMoveGoal.x = -99999.0;
		statusCode = 0;
		pointLocalMoverId = copy.pointLocalMoverId;
		groupFlags = copy.groupFlags;
		data[0] = copy.data[0];
		data[1] = copy.data[1];
	}

	void init(void);
	void init(OrderOriginType _origin, TacticalOrderCode _code, bool _unitOrder = false);
	void initWayPath(LocationNodePtr path);
	void initAttackWayPath(LocationNodePtr path);
	void destroy(void);
	TacticalOrder(void) { init(void); }
	~TacticalOrder(void) { destroy(void); }
	void setId(int32_t newId) { id = newId; }
	void setId(std::unique_ptr<MechWarrior> pilot);
	int32_t getId(void) { return (id); }
	int32_t execute(std::unique_ptr<MechWarrior> warrior, int32_t& message);
	int32_t status(std::unique_ptr<MechWarrior> warrior);
	Stuff::Vector3D getWayPoint(int32_t index);
	void setWayPoint(int32_t index, Stuff::Vector3D wayPoint);
	void addWayPoint(Stuff::Vector3D wayPoint, int32_t travelMode);
	GameObjectPtr getRamTarget(void);
	GameObjectPtr getJumpTarget(void);
	bool isGroupOrder(void);
	bool isCombatOrder(void);
	bool isMoveOrder(void);
	bool isJumpOrder(void);
	bool isWayPathOrder(void);
	int32_t getParamData(float* time, int32_t* paramList);
	int32_t pack(MoverGroupPtr unit, std::unique_ptr<Mover> point);
	int32_t unpack(void);
	void setGroupFlag(int32_t localMoverId, bool set);
	int32_t getGroup(int32_t commanderid, std::unique_ptr<Mover>* moverList, std::unique_ptr<Mover>* point, int32_t sortType = 0);
	void setStage(int32_t newStage) { stage = (char)newStage; }
	GameObjectPtr getTarget(void);
	bool equals(TacticalOrder* tacOrder);
	char getStatusCode(void) { return (statusCode); }
	void debugString(std::unique_ptr<MechWarrior> pilot, const std::wstring_view& s);
};

//***************************************************************************

#endif
