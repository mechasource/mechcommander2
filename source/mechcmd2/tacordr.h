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
	best,           // notused
	maximum,        // notused
	slow,           // notused
	moderate,
	count           // notused
};

enum class MovePatternType : uint8_t
{
	direct,         // notused
	zigzagnarrow,   // notused
	zigzagmedium,   // notused
	zigzagwide,     // notused
	randomnarrow,   // notused
	randommedium,   // notused
	randomwide,     // notused
	count           // notused
};

enum class AttitudeType : uint8_t
{
	cautious,       // notused
	conservative,   // notused
	normal,         // notused
	aggressive,     // notused
	berserker,      // notused
	suicidal,       // notused
	count
};

//#define	FIRERANGE_OPTIMUM	-1

enum class FireRangeType : int8_t
{
	none = -5,
	normal = -4,
	ramming,
	longest,
	optimal,
	shortest,
	medium,
	extended,
	current,
	count
};

enum class AttackType : uint8_t
{
	none,
	destroy,
	disable,
	conservingammo,
	count
};

enum class AttackMethod : uint8_t
{
	ranged,
	dfa,
	ramming,
	count
};

enum class FireOddsType : uint8_t
{
	lucky,      // notused
	bad,        // notused
	medium,     // notused
	good,       // notused
	great,      // notused
	count
};

enum class TacticType : uint8_t
{
	none,
	flankleft,
	flankright,
	flankrear,
	stopandfire,
	turret,
	joust,
	count
};

enum class OrderOriginType : uint8_t
{
	player,
	commander,
	self
};

enum class TacticalOrderCode : uint8_t
{
	none,
	wait,
	movetopoint,
	movetoobject,
	jumptopoint,
	jumptoobject,
	traversepath,
	patrolpath,
	escort,
	follow,
	guard,
	stop,
	powerup,
	powerdown,
	waypointsdone,
	eject,
	attackobject,
	attackpoint,
	holdfire,
	withdraw,
	scramble,
	capture,
	refit,
	getfixed,
	loadintocarrier,
	deployelementals,
	recover,
	count // IF THIS CHANGES, ADD GUARD_POINT AS SEP. ORDER (see pack/unpack hack)
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
	invalid = UCHAR_MAX // not used
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
	wchar_t stage;
	wchar_t statusCode;
	Stuff::Vector3D lastMoveGoal;

	// for network use only
	wchar_t pointLocalMoverId;
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
	TacticalOrder(void)
	{
		init(void);
	}
	~TacticalOrder(void)
	{
		destroy(void);
	}
	void setId(int32_t newId)
	{
		id = newId;
	}
	void setId(std::unique_ptr<MechWarrior> pilot);
	int32_t getId(void)
	{
		return (id);
	}
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
	void setStage(int32_t newStage)
	{
		stage = (wchar_t)newStage;
	}
	GameObjectPtr getTarget(void);
	bool equals(TacticalOrder* tacOrder);
	wchar_t getStatusCode(void)
	{
		return (statusCode);
	}
	void debugString(std::unique_ptr<MechWarrior> pilot, std::wstring_view s);
};

//***************************************************************************

#endif
