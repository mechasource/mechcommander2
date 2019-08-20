//******************************************************************************************
//
//	group.h - This file contains the MoverGroup Class header
//
//	MechCommander 2
//
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#pragma once

#ifndef GROUP_H
#define GROUP_H

//--------------
// Include Files

//#include <mclib.h>
//#include "gameobj.h"
//#include "dmover.h"
//#include "dgroup.h"
//#include "unitdesg.h"
//#include "tacordr.h"

//---------------------------------------------------------------------------

#define GOALFLAG_OPEN 1
#define GOALFLAG_CLOSED 2
#define GOALFLAG_AVAILABLE 4
#define GOALFLAG_NO_NEIGHBORS 8
#define GOALFLAG_MOVER_HERE 16

typedef struct _GoalMapNode
{
	int16_t cost; // normal cost to travel here, based upon terrain
	// int32_t				parent;								// where we came
	// from (parent cell)
	uint8_t flags; // CLOSED, OPEN, STEP flags
	int32_t g; // known cost from START to this node
	// int32_t				hPrime;								// estimated cost from this
	// node to GOAL  int32_t				fPrime;
	// // = g + hPrime

	void setFlag(uint8_t flag) { flags |= flag; }

	void clearFlag(uint8_t flag) { flags &= (flag ^ 0xFFFFFFFF); }
} GoalMapNode;

typedef struct _MoverGroupData
{
	int32_t id;
	int32_t numMovers;
	GameObjectWatchID moverWIDs[MAX_MOVERGROUP_COUNT];
	GameObjectWatchID pointWID;
	bool disbandOnNoPoint;

} MoverGroupData;

class MoverGroup
{

public:
	int32_t id;
	int32_t numMovers;
	GameObjectWatchID moverWIDs[MAX_MOVERGROUP_COUNT];
	GameObjectWatchID pointWID;
	bool disbandOnNoPoint;

	static SortList sortList;
	static GoalMapNode* goalMap;

public:
	PVOID operator new(size_t ourSize);
	void operator delete(PVOID us);

	virtual void init(void)
	{
		id = -1;
		numMovers = 0;
		pointWID = 0;
		disbandOnNoPoint = false;
		goalMap = nullptr;
	}

	void init(MoverGroupData& data);

	MoverGroup(void) { init(void); }

	virtual void destroy(void);

	~MoverGroup(void) { destroy(void); }

	virtual int32_t getId(void) { return (id); }

	virtual void setId(int32_t _id) { id = _id; }

	// int32_t calcRosterSize (BOOL checkUnits = TRUE);

	// void addToRoster (TeamPtr team, int32_t* rosterIndex);

	int32_t getNumMovers(void) { return (numMovers); }

	void setNumMovers(int32_t num) { numMovers = num; }

	virtual bool add(MoverPtr mover);

	virtual bool remove(MoverPtr mover);

	virtual bool isMember(MoverPtr mover);

	virtual int32_t disband(void);

	virtual int32_t setPoint(MoverPtr mover);

	virtual MoverPtr getPoint(void);

	virtual void setDisbandOnNoPoint(bool setting) { disbandOnNoPoint = setting; }

	virtual bool getDisbandOnNoPoint(void) { return (disbandOnNoPoint); }

	MoverPtr getMover(int32_t i);

	MoverPtr selectPoint(bool excludePoint);

	virtual int32_t getMovers(MoverPtr* moverList);

	MechWarriorPtr getPointPilot(void);

	void statusCount(int32_t* statusTally);

	void addToGUI(bool visible = true);

	int32_t calcMoveGoals(Stuff::Vector3D goal, Stuff::Vector3D* goalList);

	int32_t calcJumpGoals(
		Stuff::Vector3D goal, Stuff::Vector3D* goalList, GameObjectPtr DFATarget = nullptr);

	//----------------
	// Tactical Orders

	virtual int32_t handleTacticalOrder(TacticalOrder tacOrder, int32_t priority,
		Stuff::Vector3D* jumpGoalList = nullptr, bool queueGroupOrder = false);

	int32_t orderMoveToPoint(
		bool setTacOrder, int32_t origin, Stuff::Vector3D location, uint32_t params);

	int32_t orderMoveToObject(
		bool setTacOrder, int32_t origin, GameObjectPtr target, int32_t fromArea, uint32_t params);

	int32_t orderTraversePath(int32_t origin, WayPathPtr wayPath, uint32_t params);

	int32_t orderPatrolPath(int32_t origin, WayPathPtr wayPath);

	int32_t orderPowerDown(int32_t origin);

	int32_t orderPowerUp(int32_t origin);

	int32_t orderAttackObject(int32_t origin, GameObjectPtr target, int32_t attackType,
		int32_t attackMethod, int32_t attackRange, int32_t aimLocation, int32_t fromArea,
		uint32_t params);

	int32_t orderWithdraw(int32_t origin, Stuff::Vector3D location);

	int32_t orderEject(int32_t origin);

	//--------------
	// Combat Events
	void triggerAlarm(int32_t alarmCode, uint32_t triggerId);

	int32_t handleMateCrippled(uint32_t mateWID);

	int32_t handleMateDisabled(uint32_t mateWID);

	int32_t handleMateDestroyed(uint32_t mateWID);

	int32_t handleMateEjected(uint32_t mateWID);

	void handleMateFiredWeapon(uint32_t mateWID);

	static void sortMovers(int32_t numMoversInGroup, MoverPtr* moverList, Stuff::Vector3D dest);

	static int32_t calcMoveGoals(
		Stuff::Vector3D goal, int32_t numMovers, Stuff::Vector3D* goalList);

	static int32_t calcJumpGoals(Stuff::Vector3D goal, int32_t numMovers, Stuff::Vector3D* goalList,
		GameObjectPtr DFATarget);

	//----------------
	// Save Load
	void copyTo(MoverGroupData& data);
};

//***************************************************************************

#endif
