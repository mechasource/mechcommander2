//===========================================================================//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

//***************************************************************************
//
//								TACORDR.CPP
//
//***************************************************************************
#include "stdinc.h"

#ifndef MCLIB
#include "mclib.h"
#endif

#ifndef TACORDR_H
#include "tacordr.h"
#endif

#ifndef MOVE_H
#include "move.h"
#endif

#ifndef OBJMGR_H
#include "objmgr.h"
#endif

#ifndef MECH_H
#include "mech.h"
#endif

#ifndef GVEHICL_H
#include "gvehicl.h"
#endif

#ifndef WARRIOR_H
#include "warrior.h"
#endif

#ifndef BLDNG_H
#include "bldng.h"
#endif

#ifndef GROUP_H
#include "group.h"
#endif

#ifndef UNITDESG_H
#include "unitdesg.h"
#endif

#ifndef MULTPLYR_H
#include "multplyr.h"
#endif

#include "gamesound.h"
#ifndef SOUNDS_H
#include "sounds.h"
#endif

#ifndef TEAM_H
#include "team.h"
#endif

#ifndef MISSION_H
#include "mission.h"
#endif

#ifndef COMNDR_H
#include "comndr.h"
#endif

#include "LogisticsData.h"

#include <stdio.h>

//***************************************************************************

#define TACORDERCHUNK_UNITORDER_BITS 1
#define TACORDERCHUNK_ORIGIN_BITS 2
#define TACORDERCHUNK_CODE_BITS 5
#define TACORDERCHUNK_GROUP_BITS 16
#define TACORDERCHUNK_ATTACKTYPE_BITS 2
#define TACORDERCHUNK_ATTACKMETHOD_BITS 2
#define TACORDERCHUNK_ATTACKRANGE_BITS 3
#define TACORDERCHUNK_AIMLOCATION_BITS 4
#define TACORDERCHUNK_PURSUE_BITS 1
#define TACORDERCHUNK_OBLITERATE_BITS 1
#define TACORDERCHUNK_CELLPOS_BITS 10
#define TACORDERCHUNK_FACEOBJECT_BITS 1
#define TACORDERCHUNK_WAIT_BITS 1
#define TACORDERCHUNK_MODE_BITS 1
#define TACORDERCHUNK_RUN_BITS 1
#define TACORDERCHUNK_TARGETTYPE_BITS 2
#define TACORDERCHUNK_MOVERINDEX_BITS 7
#define TACORDERCHUNK_TERRAINPARTID_BITS 20
#define TACORDERCHUNK_TERRAINBLOCK_BITS 8
#define TACORDERCHUNK_TERRAINVERTEX_BITS 9
#define TACORDERCHUNK_TERRAINITEM_BITS 3
#define TACORDERCHUNK_TRAIN_BITS 8
#define TACORDERCHUNK_TRAINCAR_BITS 8

#define TACORDERCHUNK_UNITORDER_MASK 0x00000001
#define TACORDERCHUNK_ORIGIN_MASK 0x00000003
#define TACORDERCHUNK_CODE_MASK 0x0000001F
#define TACORDERCHUNK_GROUP_MASK 0x0000FFFF
#define TACORDERCHUNK_ATTACKTYPE_MASK 0x00000003
#define TACORDERCHUNK_ATTACKMETHOD_MASK 0x00000003
#define TACORDERCHUNK_ATTACKRANGE_MASK 0x00000007
#define TACORDERCHUNK_AIMLOCATION_MASK 0x0000000F
#define TACORDERCHUNK_PURSUE_MASK 0x00000001
#define TACORDERCHUNK_OBLITERATE_MASK 0x00000001
#define TACORDERCHUNK_CELLPOS_MASK 0x000003FF
#define TACORDERCHUNK_FACEOBJECT_MASK 0x00000001
#define TACORDERCHUNK_WAIT_MASK 0x00000001
#define TACORDERCHUNK_MODE_MASK 0x00000001
#define TACORDERCHUNK_RUN_MASK 0x00000001
#define TACORDERCHUNK_TARGETTYPE_MASK 0x00000003
#define TACORDERCHUNK_MOVERINDEX_MASK 0x0000007F
#define TACORDERCHUNK_TERRAINPARTID_MASK 0x000FFFFF
#define TACORDERCHUNK_TERRAINBLOCK_MASK 0x000000FF
#define TACORDERCHUNK_TERRAINVERTEX_MASK 0x000001FF
#define TACORDERCHUNK_TERRAINITEM_MASK 0x00000007
#define TACORDERCHUNK_TRAIN_MASK 0x000000FF
#define TACORDERCHUNK_TRAINCAR_MASK 0x000000FF

extern TeamPtr homeTeam;
extern UserHeapPtr systemHeap;
extern float scenarioTime;
extern float FireArc;
extern int32_t LastMoveCalcErr;

extern wchar_t OverlayIsBridge[NUM_OVERLAY_TYPES];

int32_t statusCode;

//***************************************************************************

int32_t AttitudeEffectOnMovePath[AttitudeType::count][3] = {
	{10, 10, 2}, // Cautious
	{8, 8, 4}, // Conservative
	{6, 6, 6}, // Normal
	{4, 4, 8}, // Aggressive
	{2, 2, 10}, // Berserker
	{1, 1, 10} // Suicidal
};

//***************************************************************************
// TACTICAL ORDER class
//***************************************************************************

PVOID
TacticalOrder::operator new(size_t mySize)
{
	PVOID result = systemHeap->Malloc(mySize);
	return (result);
}

//---------------------------------------------------------------------------

void TacticalOrder::operator delete(PVOID us)
{
	systemHeap->Free(us);
}

//---------------------------------------------------------------------------

void TacticalOrder::init(void)
{
	id = 0;
	time = -1.0;
	delayedTime = -1.0;
	lastTime = -1.0;
	unitOrder = false;
	origin = OrderOriginType::commander;
	code = TacticalOrderCode::none;
	moveparams.wayPath.numPoints = 0;
	moveparams.wayPath.curPoint = 0;
	moveparams.wayPath.points[0] = 0.0;
	moveparams.wayPath.points[1] = 0.0;
	moveparams.wayPath.points[2] = 0.0;
	moveparams.wayPath.mode[0] = TravelModeType::slow;
	moveparams.fromArea = -1;
	moveparams.faceObject = true;
	moveparams.wait = false;
	moveparams.mode = SpecialMoveMode::normal;
	moveparams.escapeTile = false;
	moveparams.jump = false;
	moveparams.keepMoving = true;
	attackParams.type = AttackType::destroy;
	attackParams.method = AttackMethod::ranged;
	attackParams.range = FireRangeType::optimal;
	attackParams.tactic = TacticType::none;
	attackParams.aimLocation = -1;
	attackParams.pursue = true;
	attackParams.obliterate = false;
	attackParams.targetpoint.Zero();
	targetWID = 0;
	targetObjectClass = BASEOBJECT;
	selectionindex = -1;
	stage = 1;
	pointLocalMoverId = 0x0F;
	groupFlags = 0;
	subOrder = false;
	statusCode = 0;
}

//---------------------------------------------------------------------------

void TacticalOrder::init(OrderOriginType _origin, TacticalOrderCode _code, bool _unitOrder)
{
	init();
	time = scenarioTime;
	unitOrder = _unitOrder;
	origin = _origin;
	code = _code;
}

//---------------------------------------------------------------------------

void TacticalOrder::initWayPath(LocationNodePtr path)
{
	int32_t numPoints = 0;
	while (path)
	{
		if (numPoints == MAX_WAYPTS)
			Fatal(0, " Way Path Too Long ");
		else
		{
			moveparams.wayPath.points[numPoints * 3] = path->location.x;
			moveparams.wayPath.points[numPoints * 3 + 1] = path->location.y;
			moveparams.wayPath.points[numPoints * 3 + 2] = path->location.z;
			if (path->run)
				moveparams.wayPath.mode[numPoints] = TravelModeType::fast;
			else
				moveparams.wayPath.mode[numPoints] = TravelModeType::slow;
		}
		numPoints++;
		path = path->next;
	}
	moveparams.wayPath.numPoints = numPoints;
}

//---------------------------------------------------------------------------

Stuff::Vector3D
TacticalOrder::getWayPoint(int32_t index)
{
	Stuff::Vector3D wayPoint(moveparams.wayPath.points[index * 3],
		moveparams.wayPath.points[index * 3 + 1], moveparams.wayPath.points[index * 3 + 2]);
	return (wayPoint);
}

//---------------------------------------------------------------------------

void TacticalOrder::setWayPoint(int32_t index, Stuff::Vector3D wayPoint)
{
	moveparams.wayPath.points[index * 3] = wayPoint.x;
	moveparams.wayPath.points[index * 3 + 1] = wayPoint.y;
	moveparams.wayPath.points[index * 3 + 2] = wayPoint.z;
}

//---------------------------------------------------------------------------

void TacticalOrder::addWayPoint(Stuff::Vector3D wayPoint, int32_t travelMode)
{
	if (moveparams.wayPath.numPoints == MAX_WAYPTS)
		Fatal(MAX_WAYPTS, " tacticalOrder.addWayPoint: too many! ");
	moveparams.wayPath.points[moveparams.wayPath.numPoints * 3] = wayPoint.x;
	moveparams.wayPath.points[moveparams.wayPath.numPoints * 3 + 1] = wayPoint.y;
	moveparams.wayPath.points[moveparams.wayPath.numPoints * 3 + 2] = wayPoint.z;
	moveparams.wayPath.mode[moveparams.wayPath.numPoints] = (uint8_t)travelMode;
	moveparams.wayPath.numPoints++;
}

//---------------------------------------------------------------------------

GameObjectPtr
TacticalOrder::getRamTarget(void)
{
	if (code == TacticalOrderCode::attackobject)
		if (attackParams.method == AttackMethod::ramming)
			return (ObjectManager->getByWatchID(targetWID));
	return (nullptr);
}

//---------------------------------------------------------------------------

GameObjectPtr
TacticalOrder::getJumpTarget(void)
{
	if (code == TacticalOrderCode::jumptopoint)
		return (ObjectManager->get(targetWID));
	return (nullptr);
}

//---------------------------------------------------------------------------

bool TacticalOrder::isGroupOrder(void)
{
	return (unitOrder);
}

//---------------------------------------------------------------------------

bool TacticalOrder::isCombatOrder(void)
{
	return ((code == TacticalOrderCode::attackobject) || (code == TacticalOrderCode::attackpoint));
}

//---------------------------------------------------------------------------

bool TacticalOrder::isMoveOrder(void)
{
	return ((code == TacticalOrderCode::movetopoint) || (code == TacticalOrderCode::movetoobject));
}

//---------------------------------------------------------------------------

bool TacticalOrder::isWayPathOrder(void)
{
	return ((code == TacticalOrderCode::traversepath) || (code == TacticalOrderCode::patrolpath));
}

//---------------------------------------------------------------------------

bool TacticalOrder::isJumpOrder(void)
{
	return ((code == TacticalOrderCode::jumptopoint) || (code == TacticalOrderCode::jumptoobject));
}

//---------------------------------------------------------------------------

void TacticalOrder::setId(std::unique_ptr<MechWarrior> pilot)
{
	Assert(id == 0, id, " TacticalOrder.setId: id != 0 ");
	int32_t nextId = pilot->getNextTacOrderId();
	id = nextId;
	if (nextId == 255)
		pilot->setNextTacOrderId(1);
	else
		pilot->setNextTacOrderId(nextId + 1);
}

//---------------------------------------------------------------------------

int32_t
TacticalOrder::getParamData(float* timeOfOrder, int32_t* paramList)
{
	if (timeOfOrder)
		*timeOfOrder = time;
	GameObjectPtr target = ObjectManager->getByWatchID(targetWID);
	paramList[0] = code;
	paramList[1] = origin;
	paramList[2] = unitOrder ? 1 : 0;
	paramList[3] = target ? target->getPartId() : 0;
	paramList[4] = targetObjectClass;
	//	UnitPtr targetUnit = nullptr;
	//	if (obj && obj->isMover())
	//		targetUnit = ((std::unique_ptr<Mover>)obj)->getUnit();
	paramList[5] = subOrder ? 1 : 0;
	paramList[6] = selectionindex;
	switch (code)
	{
	case TacticalOrderCode::movetopoint:
	case TacticalOrderCode::guard:
		paramList[7] = (moveparams.wayPath.mode[0] == TravelModeType::fast) ? 1 : 0;
		paramList[8] = moveparams.wait;
		paramList[9] = moveparams.wayPath.points[0];
		paramList[10] = moveparams.wayPath.points[1];
		paramList[11] = moveparams.wayPath.points[2];
		break;
	case TacticalOrderCode::movetoobject:
		paramList[7] = (moveparams.wayPath.mode[0] == TravelModeType::fast) ? 1 : 0;
		paramList[8] = moveparams.wait;
		paramList[9] = moveparams.faceObject;
		break;
	case TacticalOrderCode::attackobject:
		paramList[7] = (moveparams.wayPath.mode[0] == TravelModeType::fast) ? 1 : 0;
		paramList[8] = attackParams.type;
		paramList[9] = attackParams.method;
		paramList[10] = attackParams.range;
		paramList[11] = attackParams.aimLocation;
		paramList[12] = attackParams.pursue ? 1 : 0;
		paramList[13] = attackParams.obliterate ? 1 : 0;
		break;
	case TacticalOrderCode::attackpoint:
		break;
	}
	return (code);
}

//---------------------------------------------------------------------------

int32_t
TacticalOrder::pack(MoverGroupPtr group, std::unique_ptr<Mover> point)
{
	int32_t tempData = 0;
	//--------------------
	// First data uint32_t...
	tempData |= (int32_t)(attackParams.aimLocation + 2);
	tempData <<= TACORDERCHUNK_ATTACKMETHOD_BITS;
	tempData |= (int32_t)attackParams.method;
	tempData <<= TACORDERCHUNK_ATTACKTYPE_BITS;
	tempData |= (int32_t)attackParams.type;
	tempData <<= TACORDERCHUNK_GROUP_BITS;
	tempData |= groupFlags;
	tempData <<= TACORDERCHUNK_UNITORDER_BITS;
	tempData |= (unitOrder ? 1 : 0);
	tempData <<= TACORDERCHUNK_ORIGIN_BITS;
	tempData |= (int32_t)origin;
	tempData <<= TACORDERCHUNK_CODE_BITS;
	//------------------------
	// MAJOR HACK FOR GUARD...
	if (code == TacticalOrderCode::guard)
	{
		// GameObjectPtr curTarget = ObjectManager->getByWatchID(targetWID);
		if (targetWID)
			tempData |= (uint32_t)TacticalOrderCode::guard;
		else
			tempData |= (uint32_t)TACORDERCHUNK_CODE_MASK;
	}
	else
		tempData |= (int32_t)code;
	data[0] = tempData;
	//---------------------
	// Second data uint32_t...
	tempData = 0;
	bool saveTarget = false;
	int32_t targetType = 0;
	bool saveLocation = false;
	GameObjectPtr curTarget = ObjectManager->getByWatchID(targetWID);
	switch (code)
	{
	case TacticalOrderCode::movetopoint:
	case TacticalOrderCode::jumptopoint:
		saveLocation = true;
		break;
	case TacticalOrderCode::guard:
		if (curTarget)
		{
			saveTarget = true;
			if (curTarget->isMover())
				targetType = 0;
			else
				targetType = 1;
		}
		else
			saveLocation = true;
		break;
	case TacticalOrderCode::attackobject:
	case TacticalOrderCode::movetoobject:
	case TacticalOrderCode::jumptoobject:
	case TacticalOrderCode::capture:
	case TacticalOrderCode::refit:
	case TacticalOrderCode::recover:
	case TacticalOrderCode::getfixed:
		saveTarget = true;
		if (curTarget->isMover())
			targetType = 0;
		else
			targetType = 1;
		break;
	case TacticalOrderCode::attackpoint:
		saveTarget = true;
		targetType = 3;
		break;
	}
	if (saveTarget)
	{
		switch (targetType)
		{
		case 0:
			//------
			// Mover
			tempData |= ((std::unique_ptr<Mover>)curTarget)->getNetRosterIndex();
			tempData <<= TACORDERCHUNK_TARGETTYPE_BITS;
			tempData |= 0;
			break;
		case 1:
		{
			// Terrain
			tempData |= (curTarget->getPartId() - MIN_TERRAIN_PART_ID);
			tempData <<= TACORDERCHUNK_TARGETTYPE_BITS;
			tempData |= 1;
		}
		break;
		case 3:
			// Location
			int32_t worldCell[2] = {0, 0};
			land->worldToCell(attackParams.targetpoint, worldCell[0], worldCell[1]);
			tempData |= worldCell[0];
			tempData <<= TACORDERCHUNK_CELLPOS_BITS;
			tempData |= worldCell[1];
			// tempData <<= TACORDERCHUNK_TARGETTYPE_BITS;
			// tempData |= 3; // not really nec, but we'll store it anyway...
			break;
		}
		tempData <<= TACORDERCHUNK_RUN_BITS;
	}
	else if (saveLocation)
	{
		//--------------
		// Move Location
		int32_t worldCell[2] = {0, 0};
		land->worldToCell(getWayPoint(0), worldCell[0], worldCell[1]);
		tempData |= worldCell[0];
		tempData <<= TACORDERCHUNK_CELLPOS_BITS;
		tempData |= worldCell[1];
		tempData <<= TACORDERCHUNK_RUN_BITS;
	}
	tempData |= ((moveparams.wayPath.mode[0] == TravelModeType::fast) ? 1 : 0);
	tempData <<= TACORDERCHUNK_MODE_BITS;
	tempData |= (int32_t)moveparams.mode;
	tempData <<= TACORDERCHUNK_WAIT_BITS;
	tempData |= (moveparams.wait ? 1 : 0);
	tempData <<= TACORDERCHUNK_FACEOBJECT_BITS;
	tempData |= (moveparams.faceObject ? 1 : 0);
	tempData <<= TACORDERCHUNK_OBLITERATE_BITS;
	tempData |= (attackParams.obliterate ? 1 : 0);
	tempData <<= TACORDERCHUNK_PURSUE_BITS;
	tempData |= (attackParams.pursue ? 1 : 0);
	tempData <<= TACORDERCHUNK_ATTACKRANGE_BITS;
	tempData |= (int32_t)(attackParams.range + 4);
	data[1] = tempData;
	return (NO_ERROR);
}

//---------------------------------------------------------------------------

int32_t
TacticalOrder::unpack(void)
{
	int32_t tempData = data[0];
	code = (TacticalOrderCode)(tempData & TACORDERCHUNK_CODE_MASK);
	tempData >>= TACORDERCHUNK_CODE_BITS;
	//------------------------
	// MAJOR HACK FOR GUARD...
	bool guardLocation = false;
	if (code == TACORDERCHUNK_CODE_MASK)
	{
		code = TacticalOrderCode::guard;
		guardLocation = true;
	}
	origin = (OrderOriginType)(tempData & TACORDERCHUNK_ORIGIN_MASK);
	tempData >>= TACORDERCHUNK_ORIGIN_BITS;
	unitOrder = ((tempData & TACORDERCHUNK_UNITORDER_MASK) ? true : false);
	tempData >>= TACORDERCHUNK_UNITORDER_BITS;
	init(origin, code, unitOrder);
	// time = scenarioTime;
	// delayedTime = -1.0;
	// lastTime = -1.0;
	// selectionindex = -1;
	groupFlags = (tempData & TACORDERCHUNK_GROUP_MASK);
	tempData >>= TACORDERCHUNK_GROUP_BITS;
	attackParams.type = (AttackType)(tempData & TACORDERCHUNK_ATTACKTYPE_MASK);
	tempData >>= TACORDERCHUNK_ATTACKTYPE_BITS;
	attackParams.method = (AttackMethod)(tempData & TACORDERCHUNK_ATTACKMETHOD_MASK);
	tempData >>= TACORDERCHUNK_ATTACKMETHOD_BITS;
	attackParams.aimLocation = (tempData & TACORDERCHUNK_AIMLOCATION_MASK) - 2;
	tempData = data[1];
	attackParams.range = (FireRangeType)((tempData & TACORDERCHUNK_ATTACKRANGE_MASK) - 4);
	tempData >>= TACORDERCHUNK_ATTACKRANGE_BITS;
	attackParams.pursue = ((tempData & TACORDERCHUNK_PURSUE_MASK) ? true : false);
	tempData >>= TACORDERCHUNK_PURSUE_BITS;
	attackParams.obliterate = ((tempData & TACORDERCHUNK_OBLITERATE_MASK) ? true : false);
	tempData >>= TACORDERCHUNK_OBLITERATE_BITS;
	moveparams.faceObject = ((tempData & TACORDERCHUNK_FACEOBJECT_MASK) ? true : false);
	tempData >>= TACORDERCHUNK_FACEOBJECT_BITS;
	moveparams.wait = ((tempData & TACORDERCHUNK_WAIT_MASK) ? true : false);
	tempData >>= TACORDERCHUNK_WAIT_BITS;
	moveparams.mode = (SpecialMoveMode)(tempData & TACORDERCHUNK_MODE_MASK);
	tempData >>= TACORDERCHUNK_MODE_BITS;
	moveparams.wayPath.mode[0] =
		((tempData & TACORDERCHUNK_RUN_MASK) ? TravelModeType::fast : TravelModeType::slow);
	tempData >>= TACORDERCHUNK_RUN_BITS;
	switch (code)
	{
	case TacticalOrderCode::movetopoint:
	case TacticalOrderCode::jumptopoint:
	{
		int32_t worldCell[2] = {0, 0};
		worldCell[1] = (tempData & TACORDERCHUNK_CELLPOS_MASK);
		tempData >>= TACORDERCHUNK_CELLPOS_BITS;
		worldCell[0] = (tempData & TACORDERCHUNK_CELLPOS_MASK);
		Stuff::Vector3D worldPos;
		land->cellToWorld(worldCell[0], worldCell[1], worldPos);
		setWayPoint(0, worldPos);
		moveparams.wayPath.numPoints = 1;
	}
	break;
	case TacticalOrderCode::guard:
		if (guardLocation)
		{
			int32_t worldCell[2] = {0, 0};
			worldCell[1] = (tempData & TACORDERCHUNK_CELLPOS_MASK);
			tempData >>= TACORDERCHUNK_CELLPOS_BITS;
			worldCell[0] = (tempData & TACORDERCHUNK_CELLPOS_MASK);
			Stuff::Vector3D worldPos;
			land->cellToWorld(worldCell[0], worldCell[1], worldPos);
			setWayPoint(0, worldPos);
			moveparams.wayPath.numPoints = 1;
		}
		else
		{
			int32_t targetType = (tempData & TACORDERCHUNK_TARGETTYPE_MASK);
			tempData >>= TACORDERCHUNK_TARGETTYPE_BITS;
			if (targetType == 0)
			{
				// Mover Index
				int32_t netMoverIndex = (tempData & TACORDERCHUNK_MOVERINDEX_MASK);
				if (MPlayer)
				{
					targetWID = 0;
					if (MPlayer->moverRoster[netMoverIndex])
						targetWID = MPlayer->moverRoster[netMoverIndex]->getWatchID();
				}
			}
			else if (targetType == 1)
			{
				// Terrain
				uint32_t targetPartId =
					(tempData & TACORDERCHUNK_TERRAINPARTID_MASK) + MIN_TERRAIN_PART_ID;
				tempData >>= TACORDERCHUNK_TERRAINPARTID_BITS;
				GameObjectPtr obj = ObjectManager->findByPartId(targetPartId);
				if (!obj)
					STOP(("TacticalOrder.unpack: nullptr Terrain Target"));
				targetWID = obj->getWatchID();
			}
			else
				Fatal(targetType, " TacticalOrder.unpack: Bad targetType ");
		}
		break;
	case TacticalOrderCode::attackobject:
	case TacticalOrderCode::movetoobject:
	case TacticalOrderCode::jumptoobject:
	case TacticalOrderCode::capture:
	case TacticalOrderCode::refit:
	case TacticalOrderCode::recover:
	case TacticalOrderCode::getfixed:
	{
		int32_t targetType = (tempData & TACORDERCHUNK_TARGETTYPE_MASK);
		tempData >>= TACORDERCHUNK_TARGETTYPE_BITS;
		if (targetType == 0)
		{
			// Mover Index
			int32_t netMoverIndex = (tempData & TACORDERCHUNK_MOVERINDEX_MASK);
			if (MPlayer)
			{
				targetWID = 0;
				if (MPlayer->moverRoster[netMoverIndex])
					targetWID = MPlayer->moverRoster[netMoverIndex]->getWatchID();
			}
		}
		else if (targetType == 1)
		{
			// Terrain
			uint32_t targetPartId =
				(tempData & TACORDERCHUNK_TERRAINPARTID_MASK) + MIN_TERRAIN_PART_ID;
			tempData >>= TACORDERCHUNK_TERRAINPARTID_BITS;
			GameObjectPtr obj = ObjectManager->findByPartId(targetPartId);
			if (!obj)
				STOP(("TacticalOrder.unpack: nullptr Terrain Target"));
			targetWID = obj->getWatchID();
		}
		else
			Fatal(0, " Bad targetType ");
	}
	break;
	case TacticalOrderCode::attackpoint:
	{
		int32_t worldCell[2] = {0, 0};
		worldCell[1] = (tempData & TACORDERCHUNK_CELLPOS_MASK);
		tempData >>= TACORDERCHUNK_CELLPOS_BITS;
		worldCell[0] = (tempData & TACORDERCHUNK_CELLPOS_MASK);
		land->cellToWorld(worldCell[0], worldCell[1], attackParams.targetpoint);
		attackParams.targetpoint.z = land->getTerrainElevation(attackParams.targetpoint);
	}
	break;
	}
	if (targetWID)
	{
		targetObjectClass = ObjectManager->getByWatchID(targetWID)->getObjectClass();
	}
	return (NO_ERROR);
}

//---------------------------------------------------------------------------

void TacticalOrder::setGroupFlag(int32_t localMoverId, bool set)
{
	uint32_t mask = (1 << localMoverId);
	if (set)
		groupFlags |= mask;
	else
		groupFlags &= (mask ^ 0xFFFFFFFF);
}

//---------------------------------------------------------------------------

int32_t
TacticalOrder::getGroup(
	int32_t commanderid, std::unique_ptr<Mover>* moverList, std::unique_ptr<Mover>* point, int32_t sortType)
{
	if (!MPlayer)
		return (0);
	int32_t numMovers = 0;
	int32_t gFlags = groupFlags;
	for (size_t i = 0; i < MAX_LOCAL_MOVERS; i++)
	{
		if (gFlags & 1)
			moverList[numMovers++] = MPlayer->playerMoverRoster[commanderid][i];
		gFlags >>= 1;
	}
	*point = nullptr;
	if (pointLocalMoverId != 0x0F)
		*point = MPlayer->playerMoverRoster[commanderid][pointLocalMoverId];
	return (numMovers);
}

//---------------------------------------------------------------------------

int32_t TacticalOrder::execute(std::unique_ptr<MechWarrior> warrior, int32_t& message)
{
	int32_t result = TACORDER_FAILURE;
	message = -1;
	if (warrior->getVehicle()->refitBuddyWID) // am I in a refitting operation?
		// if is, point my and my refit
	{
		// buddy to nullptr, taking us out of the operation
		GameObjectPtr refitBuddy =
			ObjectManager->getByWatchID(warrior->getVehicle()->refitBuddyWID);
		std::unique_ptr<Mover> vehicle = warrior->getVehicle();
		// turn off refitting (may not be on, but make sure)
		if (vehicle->getObjectClass() == GROUNDVEHICLE)
			((GroundVehiclePtr)vehicle)->refitting = false;
		if (refitBuddy)
		{
			if (refitBuddy->getObjectClass() == GROUNDVEHICLE)
				((GroundVehiclePtr)refitBuddy)->refitting = false;
			if (refitBuddy->isMover())
				((std::unique_ptr<Mover>)refitBuddy)->refitBuddyWID = 0;
			else if (refitBuddy->getObjectClass() == BUILDING)
				((BuildingPtr)refitBuddy)->refitBuddyWID = 0;
		}
		warrior->getVehicle()->refitBuddyWID = 0;
	}
	if (warrior->getVehicle()->recoverBuddyWID)
	{
		GameObjectPtr recoverBuddy =
			ObjectManager->getByWatchID(warrior->getVehicle()->recoverBuddyWID);
		std::unique_ptr<Mover> vehicle = warrior->getVehicle();
		// turn off recovering (may not be on, but make sure)
		if (vehicle->getObjectClass() == GROUNDVEHICLE)
			((GroundVehiclePtr)vehicle)->recovering = false;
		if (recoverBuddy)
		{
			if (recoverBuddy->getObjectClass() == GROUNDVEHICLE)
				((GroundVehiclePtr)recoverBuddy)->recovering = false;
			if (recoverBuddy->isMover())
				((std::unique_ptr<Mover>)recoverBuddy)->recoverBuddyWID = 0;
			//			else if (recoverBuddy->getObjectClass() == BUILDING)
			//				((BuildingPtr)recoverBuddy)->recoverBuddyWID = 0;
		}
		warrior->getVehicle()->recoverBuddyWID = 0;
	}
	//-------------------------------------------------------------------------
	// NOTE: If not a lance order, formationPos will equal -1. Otherwise,
	// will equal position in formation. And, if the lance point, will equal 0.
	GameObjectPtr target = ObjectManager->getByWatchID(targetWID);
	switch (code)
	{
	case TacticalOrderCode::movetopoint:
	{
		if (delayedTime != -1.0)
		{
			if (delayedTime > scenarioTime)
				return (TACORDER_FAILURE);
			delayedTime = -1.0;
		}
		Stuff::Vector3D location;
		location.x = moveparams.wayPath.points[0];
		location.y = moveparams.wayPath.points[1];
		location.z = moveparams.wayPath.points[2];
		uint32_t params = TACORDER_PARAM_NONE;
		if (moveparams.wayPath.mode[0] == TravelModeType::fast)
			params |= TACORDER_PARAM_RUN;
		if (moveparams.wait)
			params |= TACORDER_PARAM_WAIT;
		if (moveparams.mode == SpecialMoveMode::minelaying)
			params |= TACORDER_PARAM_LAY_MINES;
		if (moveparams.escapeTile)
			params |= TACORDER_PARAM_ESCAPE_TILE;
		if (moveparams.jump)
			params |= TACORDER_PARAM_JUMP;
		result =
			warrior->orderMoveToPoint(unitOrder, true, origin, location, selectionindex, params);
		message = -1;
	}
	break;
	case TacticalOrderCode::movetoobject:
	{
		uint32_t params = TACORDER_PARAM_NONE;
		if (moveparams.wayPath.mode[0] == TravelModeType::fast)
			params |= TACORDER_PARAM_RUN;
		if (moveparams.faceObject)
			params |= TACORDER_PARAM_FACE_OBJECT;
		if (moveparams.mode == SpecialMoveMode::minelaying)
			params |= TACORDER_PARAM_LAY_MINES;
		warrior->orderMoveToObject(
			unitOrder, true, origin, target, moveparams.fromArea, selectionindex, params);
		message = -1;
	}
	break;
	case TacticalOrderCode::jumptopoint:
	{
		//-----------------------------------------------------
		// If we got into here, we know the jump can be done...
		if (delayedTime != -1.0)
		{
			if (delayedTime > scenarioTime)
				return (TACORDER_FAILURE);
			delayedTime = -1.0;
		}
		Stuff::Vector3D location;
		location.x = moveparams.wayPath.points[0];
		location.y = moveparams.wayPath.points[1];
		location.z = moveparams.wayPath.points[2];
		result = warrior->orderJumpToPoint(unitOrder, true, origin, location);
	}
	break;
	case TacticalOrderCode::jumptoobject:
		//-----------------------------------------------------
		// If we got into here, we know the jump can be done...
		if (delayedTime != -1.0)
		{
			if (delayedTime > scenarioTime)
				return (TACORDER_FAILURE);
			delayedTime = -1.0;
		}
		if (!target)
			return (TACORDER_SUCCESS);
		result = warrior->orderJumpToObject(unitOrder, true, origin, target);
		break;
	/*
			case TacticalOrderCode::traversepath: {
				uint32_t params = TACORDER_PARAM_NONE;
				if (moveparams.mode == SpecialMoveMode::minelaying)
					params |= TACORDER_PARAM_LAY_MINES;
				warrior->orderTraversePath( unitOrder, TRUE, origin,
	   &moveparams.wayPath, params); message = RADIO_MOVETO;
				}
				break;
			case TacticalOrderCode::patrolpath:
				//-------------
				// NEW BRAIN...
				warrior->orderPatrolPath( unitOrder, TRUE, origin,
	   &moveparams.wayPath); message = RADIO_MOVETO; break;
	*/
	case TacticalOrderCode::escort:
		break;
	case TacticalOrderCode::follow:
		break;
	case TacticalOrderCode::guard:
		attackParams.type = AttackType::destroy;
		attackParams.method = AttackMethod::ranged;
		attackParams.pursue = TRUE;
		attackParams.range = FireRangeType::optimal;
		message = RADIO_GUARD;
		break;
	case TacticalOrderCode::stop:
		warrior->orderStop(unitOrder, true);
		message = RADIO_ACK;
		break;
	case TacticalOrderCode::powerup:
		if (delayedTime > -1.0)
		{
			if (scenarioTime < delayedTime)
				return (TACORDER_FAILURE);
			delayedTime = -1.0;
		}
		warrior->orderPowerUp(unitOrder, origin);
		break;
	case TacticalOrderCode::powerdown:
		if (delayedTime > -1.0)
		{
			if (scenarioTime < delayedTime)
				return (TACORDER_FAILURE);
			delayedTime = -1.0;
		}
		warrior->orderPowerDown(unitOrder, origin);
		break;
	case TacticalOrderCode::waypointsdone:
		break;
	case TacticalOrderCode::eject:
		warrior->orderEject(unitOrder, true, origin);
		break;
	case TacticalOrderCode::attackobject:
		message = RADIO_ILLEGAL_ORDER;
		result = -1;
		if (target)
		{
			if (target->inTransport())
			{
#ifdef USE_ELEMENTALS
				target = ((ElementalPtr)target)->transport;
				targetHandle = target->getHandle();
#endif
			}
			warrior->getVehicle()->calcFireRanges();
			targetObjectClass = target->getObjectClass();
			//----------------------------------------------------------------
			// If the target is already disabled, I guess that means we simply
			// want to obliterate the sucker...
			attackParams.obliterate = target->isDisabled();
			if (!target->isDestroyed())
			{
				uint32_t params = TACORDER_PARAM_NONE;
				if (attackParams.obliterate)
					params |= TACORDER_PARAM_OBLITERATE;
				if (attackParams.pursue)
					params |= TACORDER_PARAM_PURSUE;
				if (attackParams.tactic != TacticType::none)
					params |= (TACORDER_PARAM_TACTIC_FLANK_RIGHT << (attackParams.tactic - 1));
				// 07-05-00 need to keep run info -- HKG
				if (moveparams.wayPath.mode[0] == TravelModeType::fast)
					params |= TACORDER_PARAM_RUN;
				result = warrior->orderAttackObject(unitOrder, origin, target, attackParams.type,
					attackParams.method, attackParams.range, attackParams.aimLocation,
					moveparams.fromArea, params);
				switch (result)
				{
				case TACORDER_SUCCESS:
					break;
				case TACORDER_FAILURE:
					message = (RollDice(50) ? RADIO_TARGET_ACK : RADIO_ACK);
					break;
				}
			}
		}
		else
		{
			//--------------------------------
			// Targeting a terrain location...
			warrior->setLastTarget(nullptr);
			warrior->getVehicle()->calcFireRanges();
			//-------------------------------------------------------------------
			// If we want to refuse the order if the range is beyond our
			// personal attack range, uncomment the following if/then...
			// if (warrior->getVehicle()->getFireRange(attackParams.range) <=
			// warrior->getVehicle()->getFireRange(FireRangeType::longest)) {
			uint32_t params = TACORDER_PARAM_NONE;
			if (attackParams.obliterate)
				params |= TACORDER_PARAM_OBLITERATE;
			if (attackParams.pursue)
				params |= TACORDER_PARAM_PURSUE;
			if (attackParams.tactic != TacticType::none)
				params |= (TACORDER_PARAM_TACTIC_FLANK_RIGHT << (attackParams.tactic - 1));
			result = warrior->orderAttackPoint(unitOrder, origin, attackParams.targetpoint,
				attackParams.type, attackParams.method, attackParams.range, params);
			switch (result)
			{
			case TACORDER_SUCCESS:
				break;
			case TACORDER_FAILURE:
				//-------------------------------
				// Now, look for specific case...
				message = (RollDice(50) ? RADIO_TARGET_ACK : RADIO_ACK);
				break;
			}
			//}
		}
		break;
	case TacticalOrderCode::attackpoint:
	{
		message = RADIO_ILLEGAL_ORDER;
		result = -1;
		//--------------------------------
		// Targeting a terrain location...
		warrior->setLastTarget(nullptr);
		warrior->getVehicle()->calcFireRanges();
		//-------------------------------------------------------------------
		// If we want to refuse the order if the range is beyond our personal
		// attack range, uncomment the following if/then...
		// if (warrior->getVehicle()->getFireRange(attackParams.range) <=
		// warrior->getVehicle()->getFireRange(FireRangeType::longest)) {
		uint32_t params = TACORDER_PARAM_NONE;
		if (attackParams.obliterate)
			params |= TACORDER_PARAM_OBLITERATE;
		if (attackParams.pursue)
			params |= TACORDER_PARAM_PURSUE;
		if (attackParams.tactic != TacticType::none)
			params |= (TACORDER_PARAM_TACTIC_FLANK_RIGHT << (attackParams.tactic - 1));
		result = warrior->orderAttackPoint(unitOrder, origin, attackParams.targetpoint,
			attackParams.type, attackParams.method, attackParams.range, params);
		switch (result)
		{
		case TACORDER_SUCCESS:
			break;
		case TACORDER_FAILURE:
			//-------------------------------
			// Now, look for specific case...
			message = (RollDice(50) ? RADIO_TARGET_ACK : RADIO_ACK);
			break;
		}
		//}
	}
	break;
	case TacticalOrderCode::holdfire:
		warrior->orderWait(unitOrder, origin, 0.0, TRUE);
		break;
	case TacticalOrderCode::withdraw:
	{
		Stuff::Vector3D location;
		location.x = moveparams.wayPath.points[0];
		location.y = moveparams.wayPath.points[1];
		location.z = moveparams.wayPath.points[2];
		warrior->orderWithdraw(unitOrder, origin, location);
	}
	break;
	case TacticalOrderCode::scramble:
	{
		Stuff::Vector3D location;
		location.x = moveparams.wayPath.points[0];
		location.y = moveparams.wayPath.points[1];
		location.z = moveparams.wayPath.points[2];
		// warrior->orderScramble(OrderType::current, unitOrder, origin, location,
		// moveparams.radius);
	}
	break;
	case TacticalOrderCode::refit:
		result = TACORDER_SUCCESS;
		if (target && (target->getObjectClass() == BATTLEMECH) && ((std::unique_ptr<Mover>)target)->needsRefit())
		{
			if ((warrior->getVehicle()->getObjectClass() == GROUNDVEHICLE) && (((GroundVehiclePtr)warrior->getVehicle())->getRefitPoints() > 0.0) && (warrior->getVehicle()->refitBuddyWID == 0))
			{
				uint32_t params = TACORDER_PARAM_FACE_OBJECT;
				if (moveparams.wayPath.mode[0] == TravelModeType::fast)
					params |= TACORDER_PARAM_RUN;
				//					result = warrior->orderMoveToObject(unitOrder,
				// false, origin, target, moveparams.fromArea, selectionindex,
				// params);
				int32_t cellRow, cellCol;
				Stuff::Vector3D destination;
				land->worldToCell(target->getPosition(), cellRow, cellCol);
				//					cellRow++;
				//					cellCol++;
				land->cellToWorld(cellRow, cellCol, destination);
				result = warrior->orderMoveToPoint(
					unitOrder, false, origin, destination, selectionindex, params);
				time = 0;
				if (result == TACORDER_FAILURE)
				{
					((std::unique_ptr<Mover>)target)->refitBuddyWID = warrior->getVehicle()->getWatchID();
					warrior->getVehicle()->refitBuddyWID = target->getWatchID();
				}
			}
			if (result != TACORDER_FAILURE)
				stage = -1; // order failed
		}
		break;
	case TacticalOrderCode::getfixed:
		result = TACORDER_SUCCESS;
		if (target && (target->getObjectClass() == BUILDING))
		{
			BuildingPtr repairBuilding = (BuildingPtr)target;
			if ((repairBuilding->getRefitPoints() > 0.0) && (repairBuilding->refitBuddyWID == 0) && ((warrior->getVehicle()->getObjectClass() == BATTLEMECH && repairBuilding->getFlag(OBJECT_FLAG_MECHBAY)) || (warrior->getVehicle()->getObjectClass() == GROUNDVEHICLE && !repairBuilding->getFlag(OBJECT_FLAG_MECHBAY))))
			{
				uint32_t params = TACORDER_PARAM_FACE_OBJECT;
				if (moveparams.wayPath.mode[0] == TravelModeType::fast)
					params |= TACORDER_PARAM_RUN;
				result = warrior->orderMoveToPoint(unitOrder, false, origin,
					repairBuilding->getPosition(), selectionindex, params);
				time = 0;
				if (result == TACORDER_FAILURE)
				{
					repairBuilding->refitBuddyWID = warrior->getVehicle()->getWatchID();
					warrior->getVehicle()->refitBuddyWID = target->getWatchID();
				}
			}
			if (result != TACORDER_FAILURE)
				stage = -1;
		}
		break;
	case TacticalOrderCode::loadintocarrier:
		result = TACORDER_SUCCESS;
#ifdef USE_ELEMENTALS
		if ((warrior->getVehicle()->getObjectClass() == ELEMENTAL) && target && (target->getObjectClass() == GROUNDVEHICLE) && ((GroundVehiclePtr)target)->elementalCarrier)
			result = ((GameObjectPtr)target)
						 ->getPilot()
						 ->orderMoveToObject(
							 false, false, origin, warrior->getVehicle(), moveparams.fromArea);
#endif
		if (result != TACORDER_FAILURE)
			stage = -1;
		break;
	case TacticalOrderCode::deployelementals:
	{
		uint32_t params = TACORDER_PARAM_NONE;
		if (moveparams.wayPath.mode[0] == TravelModeType::fast)
			params |= TACORDER_PARAM_RUN;
		if (moveparams.wait)
			params |= TACORDER_PARAM_WAIT;
		result = TACORDER_SUCCESS;
		if (result == TACORDER_SUCCESS)
			stage = -1;
	}
	break;
	case TacticalOrderCode::capture:
		result = TACORDER_SUCCESS;
		if (target && (target->isCaptureable(warrior->getTeam()->getId())) && !warrior->getVehicle()->isFriendly(target->getTeam()))
		{
			if (!target->getCaptureBlocker(warrior->getVehicle()))
			{
				bool prison = false;
				if (target->isBuilding())
				{
					if ((target->getObjectClass() == BUILDING) && ((BuildingPtr)target)->isPrison())
						prison = true;
				}
				// if this isn't a prison, go for it. if it is, only a ground
				// vehicle with seats for pilots can capture it NOTHING IS EVER
				// A PRISON NOW!!
				result = TACORDER_FAILURE;
			}
		}
		if (result == TACORDER_FAILURE)
		{
			// What if we're already there??
			float distanceToGoal = warrior->getVehicle()->distanceFrom(target->getPosition());
			result = TACORDER_FAILURE;
			GameObjectPtr blockerList[MAX_MOVERS];
			int32_t numBlockers = target->getCaptureBlocker(warrior->getVehicle(), blockerList);
			if (target->getTeam() == warrior->getTeam())
			{
				//--------------------------------
				// our team already captured it...
				result = TACORDER_SUCCESS;
			}
			else if (numBlockers > 0)
			{
				statusCode = 1;
				result = TACORDER_SUCCESS;
				warrior->radioMessage(RADIO_CANNOT_CAPTURE, true);
			}
			else if (distanceToGoal < 30.0 && target->isCaptureable(warrior->getTeam()->getId()))
			{
				switch (target->getObjectClass())
				{
				case BUILDING:
					if (MPlayer)
					{
						if (MPlayer->isServer())
							MPlayer->addCaptureBuildingChunk((BuildingPtr)target,
								((BuildingPtr)target)->getCommanderId(),
								warrior->getVehicle()->getCommanderId());
						if (target->getObjectType()->getObjTypeNum() == GENERIC_HQ_BUILDING_OBJNUM)
							if (MPlayer->missionSettings.missionType == MISSION_TYPE_CAPTURE_BASE)
								target->setFlag(OBJECT_FLAG_CAPTURABLE, false);
					}
					target->setCommanderId(warrior->getVehicle()->getCommanderId());
					target->setTeamId(warrior->getTeam()->getId(), false);
					result = TACORDER_SUCCESS;
					warrior->radioMessage(RADIO_CAPTURED_BUILDING, TRUE);
					break;
				case GROUNDVEHICLE:
				{
					int32_t cellRow, cellCol;
					target->getCellPosition(cellRow, cellCol);
					target->setTeamId(warrior->getTeam()->getId(), false);
					result = TACORDER_SUCCESS;
					warrior->radioMessage(RADIO_CAPTURED_VEHICLE, true);
				}
				break;
				case BATTLEMECH:
				{
					int32_t cellRow, cellCol;
					target->getCellPosition(cellRow, cellCol);
				}
				break;
				}
			}
			if (result == TACORDER_FAILURE)
			{
				uint32_t params = TACORDER_PARAM_NONE;
				if (moveparams.wayPath.mode[0] == TravelModeType::fast)
					params |= TACORDER_PARAM_RUN;
				if (moveparams.faceObject)
					params |= TACORDER_PARAM_FACE_OBJECT;
				message = RADIO_ACK;
				result = warrior->orderMoveToObject(
					false, false, origin, target, moveparams.fromArea, 0, params);
			}
		}
		else
			message = RADIO_CANNOT_CAPTURE;
		if (result == TACORDER_SUCCESS)
			stage = -1;
		break;
	case TacticalOrderCode::recover:
		result = TACORDER_SUCCESS;
		if (target && (target->getObjectClass() == BATTLEMECH))
		{
			if ((warrior->getVehicle()->getObjectClass() == GROUNDVEHICLE) && (((GroundVehiclePtr)warrior->getVehicle())->getRecoverPoints() > 0.0) && (warrior->getVehicle()->recoverBuddyWID == 0))
			{
				uint32_t params = TACORDER_PARAM_FACE_OBJECT;
				if (moveparams.wayPath.mode[0] == TravelModeType::fast)
					params |= TACORDER_PARAM_RUN;
				result = warrior->orderMoveToObject(
					unitOrder, false, origin, target, moveparams.fromArea, selectionindex, params);
				time = 0;
				if (result == TACORDER_FAILURE)
				{
					((std::unique_ptr<Mover>)target)->recoverBuddyWID = warrior->getVehicle()->getWatchID();
					warrior->getVehicle()->recoverBuddyWID = target->getWatchID();
				}
			}
			if (result != TACORDER_FAILURE)
				stage = -1; // order failed
		}
		break;
	}
	if (origin == OrderOriginType::player)
		warrior->triggerAlarm(PILOT_ALARM_PLAYER_ORDER, code);
	else
		message = -1;
	if (code != TacticalOrderCode::withdraw)
	{
		if (warrior->getVehicle()->getStatus() != OBJECT_STATUS_DESTROYED)
		{
			//-------------------------------------------------------------------
			// Any non-withdraw order forces us out of withdraw mode, if in
			// it...
			((std::unique_ptr<Mover>)warrior->getVehicle())->withdrawing = false;
		}
	}
	return (result);
}

//---------------------------------------------------------------------------
extern Mission* mission;

int32_t
TacticalOrder::status(std::unique_ptr<MechWarrior> warrior)
{
	int32_t result = TACORDER_SUCCESS;
	GameObjectPtr target = ObjectManager->getByWatchID(targetWID);
	switch (code)
	{
	case TacticalOrderCode::wait:
		result = (scenarioTime > delayedTime) ? TACORDER_SUCCESS : TACORDER_FAILURE;
		break;
	case TacticalOrderCode::movetopoint:
	{
		int32_t formation = -1;
		int32_t formationPos = -1;
		if ((formation > -1) && (formationPos > 0))
			result = TACORDER_FAILURE;
		else
		{
			if (delayedTime > -1.0)
			{
				if (scenarioTime < delayedTime)
					return (TACORDER_FAILURE);
				delayedTime = -1.0;
				int32_t message;
				execute(warrior, message);
			}
			Stuff::Vector3D location;
			location.x = moveparams.wayPath.points[0];
			location.y = moveparams.wayPath.points[1];
			location.z = moveparams.wayPath.points[2];
			// float distanceToGoal =
			// warrior->getVehicle()->distanceFrom(location);
			int32_t cellDistanceToGoal = warrior->getVehicle()->cellDistanceFrom(location);
			if (cellDistanceToGoal < 1 /*< Mover::marginOfError[1]*/)
			{
				if (moveparams.wait)
				{
					code = TacticalOrderCode::wait;
					result = TACORDER_FAILURE;
				}
			}
			else
				result = TACORDER_FAILURE;
		}
	}
	break;
	case TacticalOrderCode::movetoobject:
		if (target)
		{
			Stuff::Vector3D location;
			location.x = moveparams.wayPath.points[0];
			location.y = moveparams.wayPath.points[1];
			location.z = moveparams.wayPath.points[2];
			// float distanceToGoal =
			// warrior->getVehicle()->distanceFrom(location);
			int32_t cellDistanceToGoal = warrior->getVehicle()->cellDistanceFrom(location);
			if (cellDistanceToGoal > 0 /*Mover::marginOfError[1]*/)
				result = TACORDER_FAILURE;
			//	else if (moveparams.faceObject) {
			//		float relFacing =
			// warrior->getVehicle()->relViewFacingTo(target->getPosition());
			//		float fireArc = warrior->getVehicle()->getFireArc();
			//		if ((relFacing < -fireArc) || (relFacing > fireArc))
			//			result = TACORDER_FAILURE;
			//	}
		}
		break;
	case TacticalOrderCode::jumptopoint:
		if (stage != 3)
			result = TACORDER_FAILURE;
		break;
	case TacticalOrderCode::jumptoobject:
	{
#ifdef USE_JUMPING
		BattleMechPtr mech = (BattleMechPtr)warrior->getVehicle();
		if (!mech->inJump || ((MechActor*)mech->appearance)->inJump)
			result = TACORDER_FAILURE;
#endif
	}
	break;
#if 0
		case TacticalOrderCode::traversepath:
			if(stage == 2)
				result = TACORDER_SUCCESS;
			else
				result = TACORDER_FAILURE;
			break;
		case TacticalOrderCode::patrolpath:
			result = TACORDER_FAILURE;
			break;
#endif
	case TacticalOrderCode::escort:
	case TacticalOrderCode::follow:
		break;
	case TacticalOrderCode::guard:
		result = TACORDER_FAILURE;
		break;
	case TacticalOrderCode::stop:
		result = TACORDER_SUCCESS;
		break;
	case TacticalOrderCode::powerup:
		if (delayedTime > -1.0)
		{
			if (scenarioTime < delayedTime)
				return (TACORDER_FAILURE);
			delayedTime = -1.0;
			int32_t message;
			execute(warrior, message);
		}
		if (warrior->getVehicleStatus() != OBJECT_STATUS_NORMAL)
			result = TACORDER_FAILURE;
		break;
	case TacticalOrderCode::powerdown:
		if (delayedTime > -1.0)
		{
			if (scenarioTime < delayedTime)
				return (TACORDER_FAILURE);
			delayedTime = -1.0;
			int32_t message;
			execute(warrior, message);
		}
		//-----------------------------------------------------------------
		// For now, they're always powering down until given a new order...
		result = TACORDER_FAILURE;
		break;
	case TacticalOrderCode::waypointsdone:
		break;
	case TacticalOrderCode::eject:
		break;
	case TacticalOrderCode::attackobject:
		if (target)
		{
#ifdef USE_ELEMENTALS
			if ((target->getObjectClass() == ELEMENTAL) && ((ElementalPtr)target)->transport)
			{
				target = ((ElementalPtr)target)->transport;
				targetHandle = target->getHandle();
			}
#endif
			if (target->isDestroyed())
				result = TACORDER_SUCCESS;
			else if (target->isDisabled())
			{
				if (attackParams.obliterate)
					result = TACORDER_FAILURE;
				else
					result = TACORDER_SUCCESS;
			}
			else
				result = TACORDER_FAILURE;
		}
		break;
	case TacticalOrderCode::attackpoint:
		result = TACORDER_FAILURE;
		break;
	case TacticalOrderCode::holdfire:
		result = TACORDER_FAILURE;
		break;
	case TacticalOrderCode::withdraw:
		result = TACORDER_FAILURE;
		break;
	case TacticalOrderCode::scramble:
	{
		result = TACORDER_FAILURE;
		int32_t formation = -1;
		int32_t formationPos = -1;
		if ((formation > -1) && (formationPos > 0))
			result = TACORDER_FAILURE;
		else
		{
			Stuff::Vector3D location;
			location.x = moveparams.wayPath.points[0];
			location.y = moveparams.wayPath.points[1];
			location.z = moveparams.wayPath.points[2];
			float distanceToGoal = warrior->getVehicle()->distanceFrom(location);
			if (distanceToGoal < 10.0)
			{
				if (moveparams.wait)
				{
					//-------------------
					// Calc a new goal...
				}
			}
		}
	}
	break;
	case TacticalOrderCode::refit:
		// has refitter or refitee gotten distracted?
		if (!target || (warrior->getVehicle()->refitBuddyWID == 0) || (((std::unique_ptr<Mover>)target)->refitBuddyWID == 0))
			result = TACORDER_SUCCESS;
		else
		{
			// refitter and refitee *should* be pointing at each other...
			//
			if (warrior->getVehicle()->refitBuddyWID != target->getWatchID())
				warrior->getVehicle()->refitBuddyWID = target->getWatchID();
			if (((std::unique_ptr<Mover>)target)->refitBuddyWID != warrior->getVehicle()->getWatchID())
				((std::unique_ptr<Mover>)target)->refitBuddyWID = warrior->getVehicle()->getWatchID();
			result = TACORDER_FAILURE;
			switch (stage)
			{
			case 1: // move
				if (warrior->getMovePath()->numStepsWhenNotPaused == 0 && warrior->getMovePathRequest() == nullptr)
				{
					if (warrior->getVehicle()->distanceFrom(target->getPosition()) < Mover::refitRange)
					{
						stage++;
					}
					else
					{
						int32_t cellRow, cellCol;
						Stuff::Vector3D destination;
						land->worldToCell(target->getPosition(), cellRow, cellCol);
						cellRow++;
						cellCol++;
						land->cellToWorld(cellRow, cellCol, destination);
						uint32_t params = TACORDER_PARAM_FACE_OBJECT;
						result = warrior->orderMoveToPoint(
							unitOrder, false, origin, destination, selectionindex, params);
					}
				}
				break;
			case 2: // power-down (or other cool animation...)
				if (time == 0)
				{
					target->getPilot()->orderPowerDown(unitOrder, OrderOriginType::self);
					time = scenarioTime;
				}
				else if (scenarioTime > time + 3)
				{
					stage++;
					time = scenarioTime;
				}
				break;
			case 3: // do the deed
				((GroundVehiclePtr)warrior->getVehicle())->refitting = true;
				if (scenarioTime > time + Mover::refitTime)
				{
					float pointsUsed = 0;
					float pointsAvailable = warrior->getVehicle()->getRefitPoints();
					int32_t result = ((std::unique_ptr<Mover>)target)->refit(pointsAvailable, pointsUsed);
					warrior->getVehicle()->burnRefitPoints(pointsUsed);
					if (warrior->getVehicle()->getRefitPoints() <= 0)
					{
						// I died trying to repair someone.  Power the someone
						// back up.
						target->getPilot()->orderPowerUp(unitOrder, OrderOriginType::self);
						((std::unique_ptr<Mover>)target)->refitBuddyWID = 0;
						warrior->getVehicle()->refitBuddyWID = 0;
					}
					if (MPlayer)
					{
						if (pointsAvailable > 255.0f)
							pointsAvailable = 255.0f;
						WeaponShotInfo refitInfo;
						refitInfo.init(nullptr, -5, pointsUsed, GROUNDVEHICLE_LOCATION_TURRET, 0.0);
						MPlayer->addWeaponHitChunk(
							(GameObjectPtr)warrior->getVehicle(), &refitInfo);
						refitInfo.init(nullptr, -5, pointsAvailable, 0, 0.0);
						MPlayer->addWeaponHitChunk(target, &refitInfo, TRUE);
					}
					stage += result;
					if (result == 0)
						time = scenarioTime;
					if (!warrior->getVehicle()->getRefitPoints())
						warrior->getVehicle()->disable(POWER_USED_UP);
				}
				break;
			case 4: // done. power-up (or whatever) and report
				((GroundVehiclePtr)warrior->getVehicle())->refitting = false;
				target->getPilot()->orderPowerUp(unitOrder, OrderOriginType::self);
				result = TACORDER_SUCCESS;
				((std::unique_ptr<Mover>)target)->refitBuddyWID = 0;
				warrior->getVehicle()->refitBuddyWID = 0;
				break;
			case -1: // execute failed
				result = TACORDER_SUCCESS;
				break;
			}
		}
		break;
	case TacticalOrderCode::getfixed:
		if (!target || (warrior->getVehicle()->refitBuddyWID == 0) || (((BuildingPtr)target)->refitBuddyWID == 0))
			result = TACORDER_SUCCESS;
		else
		{
			// refitter and refitee *should* be pointing at each other...
			Assert(warrior->getVehicle()->refitBuddyWID == target->getWatchID() && ((BuildingPtr)target)->refitBuddyWID == warrior->getVehicle()->getWatchID(),
				0, "Refitee and refitter aren't pointing at each other.");
			result = TACORDER_FAILURE;
			switch (stage)
			{
			case 1: // move
				if (warrior->getMovePath()->numStepsWhenNotPaused == 0 && warrior->getMovePathRequest() == nullptr)
					stage++;
				break;
			case 2: // power-down (or other cool animation...)
				if (time == 0)
				{
					warrior->orderPowerDown(unitOrder, OrderOriginType::self);
					time = scenarioTime;
				}
				else if (scenarioTime > time + 3)
				{
					stage++;
					time = scenarioTime;
					soundSystem->playDigitalSample(REPAIRBAY_FX, target->getPosition());
				}
				break;
			case 3: // do the deed
				if (scenarioTime > time + Mover::refitTime)
				{
					float pointsUsed = 0;
					float pointsAvailable = target->getRefitPoints();
					int32_t result = warrior->getVehicle()->refit(pointsAvailable, pointsUsed);
					target->burnRefitPoints(pointsUsed);
					if (MPlayer)
					{
						if (pointsAvailable > 255.0f)
							pointsAvailable = 255.0f;
						WeaponShotInfo refitInfo;
						refitInfo.init(nullptr, -5, pointsUsed, -1, 0.0);
						MPlayer->addWeaponHitChunk(target, &refitInfo);
						refitInfo.init(nullptr, -5, pointsAvailable, 0, 0.0);
						MPlayer->addWeaponHitChunk(warrior->getVehicle(), &refitInfo, true);
					}
					stage += result;
					if (result == 0)
					{
						time = scenarioTime;
					}
					else // power-up (or whatever) and walk off
					{
						warrior->getVehicle()->startUp();
						{
							BuildingPtr repairBuilding = (BuildingPtr)target;
							int32_t tileRow, tileCol;
							Stuff::Vector3D destination;
							land->worldToTile(repairBuilding->getPosition(), tileRow, tileCol);
							tileRow++;
							tileCol++;
							land->tileCellToWorld(tileRow, tileCol, 2, 2, destination);
							warrior->orderMoveToPoint(unitOrder, false, OrderOriginType::self,
								destination, selectionindex, 0);
						}
					}
				}
				break;
			case 4: // walking off...
				if (warrior->getMovePath()->numStepsWhenNotPaused == 0 && warrior->getMovePathRequest() == nullptr)
					stage++;
				break;
			case 5: // done. report
				result = TACORDER_SUCCESS;
				((BuildingPtr)target)->refitBuddyWID = 0;
				warrior->getVehicle()->refitBuddyWID = 0;
				break;
			case -1: // execute failed
				result = TACORDER_SUCCESS;
				break;
			}
		}
		break;
	case TacticalOrderCode::loadintocarrier:
	{
		result = TACORDER_FAILURE;
#ifdef USE_ELEMENTALS
		MoverGroupPtr myGroup = warrior->getGroup();
		if (!myGroup)
			break;
		switch (stage)
		{
		case 1:
			if (warrior->getVehicle()->getPosition().distance_from(target->getPosition()) < 200)
			{
				warrior->clearMoveOrders();
				if (myGroup)
					for (size_t i = 0; i < myGroup->getNumMovers(); i++)
						myGroup->getMover(i)->getPilot()->clearMoveOrders(); // stop!
				target->getPilot()->clearMoveOrders(); // stop!
				time = scenarioTime;
				stage++; // not really stage here. see below
			}
			break;
		case 2:
			if ((scenarioTime > time + 3) && myGroup)
			{
				for (size_t i = 0; i < myGroup->getNumMovers(); i++)
					myGroup->getMover(i)->getPilot()->orderMoveToObject(
						false, false, OrderOriginType::self, target, moveparams.fromArea);
				stage++;
			}
			break;
		case 3:
			if (scenarioTime > time + 5)
			{
				for (size_t i = 0; i < myGroup->getNumMovers(); i++)
				{
					((ElementalPtr)myGroup->getMover(i))->transport = (std::unique_ptr<Mover>)target;
					for (size_t j = 0; j < MAX_TOADS; j++)
					{
						if (((GroundVehiclePtr)target)->toads[j] == 0)
						{
							((GroundVehiclePtr)target)->toads[j] =
								myGroup->getMover(i /*-1*/)->getHandle();
							break;
						}
					}
				}
				result = TACORDER_SUCCESS;
			}
			break;
		case -1: // execute failed
			result = TACORDER_SUCCESS;
			break;
		}
#endif
	}
	break;
	case TacticalOrderCode::deployelementals:
#ifdef USE_ELEMENTALS
		if (stage != -1)
		{
			result = TACORDER_FAILURE;
			if (scenarioTime > time + 5)
			{
				int16_t deployCount = 5;
				int32_t i;
				for (i = 0; i < MAX_TOADS; i++)
				{
					ElementalPtr toad =
						(ElementalPtr)(BaseObjectPtr)((GroundVehiclePtr)warrior->getVehicle())
							->toads[i];
					if (toad)
					{
						vector_2d offset;
						Stuff::Vector3D targetLocation;
						float angle;
						deployCount--;
						offset.x = 50;
						offset.y = 50;
						angle = deployCount * 72;
						offset.rotate(angle);
						targetLocation = warrior->getVehicle()->getPosition();
						targetLocation.x += offset.x;
						targetLocation.y += offset.y;
						toad->setPosition(targetLocation);
						toad->transport = nullptr;
						((GroundVehiclePtr)warrior->getVehicle())->toads[i] = nullptr;
						if (deployCount == 0)
						{
							time = scenarioTime; // come back for another load
							break;
						}
					}
				}
				if (i == MAX_TOADS)
					result = TACORDER_SUCCESS;
			}
		}
#endif
		break;
	case TacticalOrderCode::capture:
		if (stage != -1)
		{
			float distanceToGoal = warrior->getVehicle()->distanceFrom(target->getPosition());
			result = TACORDER_FAILURE;
			GameObjectPtr blockerList[MAX_MOVERS];
			int32_t numBlockers = target->getCaptureBlocker(warrior->getVehicle(), blockerList);
			if (target->getTeam() == warrior->getTeam())
			{
				//--------------------------------
				// our team already captured it...
				result = TACORDER_SUCCESS;
			}
			else if (numBlockers > 0)
			{
				// is someone blocking the capture now?
				// if (warrior->getTeam() == Team::home) {
				statusCode = 1;
				result = TACORDER_SUCCESS;
				warrior->radioMessage(RADIO_CANNOT_CAPTURE, true);
				/*
					}
				else
				{
					//-----------------------------------------------------
					// Why is it blocked, and can we do something about it?
					if (MPlayer)
						Fatal(0, " MULTIPLAYER! handle this case:
				tacorder.status--capture "); if ((origin == OrderOriginType::self)
				&& (numBlockers > 3)) { TacticalOrder alarmTacOrder;
						alarmTacOrder.init(OrderOriginType::player,
				TacticalOrderCode::attackobject); alarmTacOrder.playerSubOrder =
				true; alarmTacOrder.targetWID = target->getWatchID();
						alarmTacOrder.attackParams.type = AttackType::destroy;
						alarmTacOrder.attackParams.method = AttackMethod::ranged;
						alarmTacOrder.attackParams.aimLocation = -1;
						alarmTacOrder.attackParams.range = FireRangeType::optimal;
						alarmTacOrder.attackParams.pursue = true;
						alarmTacOrder.attackParams.obliterate = false;
						alarmTacOrder.moveparams.wayPath.mode[0] =
				TravelModeType::fast; warrior->setAlarmTacOrder(alarmTacOrder, 255);
					}
					else {
						TacticalOrder alarmTacOrder;
						alarmTacOrder.init(OrderOriginType::player,
				TacticalOrderCode::attackobject); alarmTacOrder.playerSubOrder =
				true; alarmTacOrder.targetWID = blockerList[0]->getWatchID();
						alarmTacOrder.attackParams.type = AttackType::destroy;
						alarmTacOrder.attackParams.method = AttackMethod::ranged;
						alarmTacOrder.attackParams.aimLocation = -1;
						alarmTacOrder.attackParams.range = FireRangeType::optimal;
						alarmTacOrder.attackParams.pursue = true;
						alarmTacOrder.attackParams.obliterate = false;
						alarmTacOrder.moveparams.wayPath.mode[0] =
				TravelModeType::fast; warrior->setAlarmTacOrder(alarmTacOrder, 255);
					}
				}
				*/
			}
			else if (distanceToGoal < 30.0 && target->isCaptureable(warrior->getTeam()->getId()))
			{
				switch (target->getObjectClass())
				{
				case BUILDING:
					if (MPlayer)
					{
						if (MPlayer->isServer())
							MPlayer->addCaptureBuildingChunk((BuildingPtr)target,
								((BuildingPtr)target)->getCommanderId(),
								warrior->getVehicle()->getCommanderId());
						if (target->getObjectType()->getObjTypeNum() == GENERIC_HQ_BUILDING_OBJNUM)
							if (MPlayer->missionSettings.missionType == MISSION_TYPE_CAPTURE_BASE)
								target->setFlag(OBJECT_FLAG_CAPTURABLE, false);
					}
					target->setCommanderId(warrior->getVehicle()->getCommanderId());
					target->setTeamId(warrior->getTeam()->getId(), false);
					result = TACORDER_SUCCESS;
					warrior->radioMessage(RADIO_CAPTURED_BUILDING, TRUE);
					break;
				case GROUNDVEHICLE:
				{
					int32_t cellRow, cellCol;
					target->getCellPosition(cellRow, cellCol);
					target->setTeamId(warrior->getTeam()->getId(), false);
					result = TACORDER_SUCCESS;
					warrior->radioMessage(RADIO_CAPTURED_VEHICLE, true);
				}
				break;
				case BATTLEMECH:
				{
					int32_t cellRow, cellCol;
					target->getCellPosition(cellRow, cellCol);
				}
				break;
				}
			}
		}
		break;
	case TacticalOrderCode::recover:
		// has recoverer gotten distracted?
		STOP(("RECOVER TACORDER NO LONGER VALID"));
#if 0
			if(!target || (warrior->getVehicle()->recoverBuddyWID == 0) || (((std::unique_ptr<Mover>)target)->recoverBuddyWID == 0))
				result = TACORDER_SUCCESS;
			else
			{
				// refitter and refitee *should* be pointing at each other...
				//Assert(warrior->getVehicle()->recoverBuddyWID == target->getWatchID() &&
				//		((std::unique_ptr<Mover>) target)->recoverBuddyWID == warrior->getVehicle()->getWatchID(), 0, "Recoveree and recoverer aren't pointing at each other.");
				result = TACORDER_FAILURE;
				switch(stage)
				{
					case 1:	// move
						if(warrior->getVehicle()->distanceFrom(target->getPosition()) < Mover::recoverRange)
							stage++;
						break;
					case 2: // power-down (or other cool animation...)
						if(target->getObjectClass() == BATTLEMECH)
						{
							((BattleMechPtr)target)->leftArmBlownThisFrame = false;
							((BattleMechPtr)target)->rightArmBlownThisFrame = false;
							((std::unique_ptr<Mover>)target)->getPilot()->clearAlarms();
						}
						if(target->isDisabled())
							target->setStatus(OBJECT_STATUS_SHUTDOWN);
						mission->tradeMover((Mover*)target, Team::home->getId(), Commander::home->getId(),
											(std::wstring_view)LogisticsData::instance->getBestPilot((target)->tonnage), "pbrain");
						((std::unique_ptr<Mover>)target)->recoverBuddyWID = warrior->getVehicle()->getWatchID();
						warrior->getVehicle()->recoverBuddyWID = target->getWatchID();
						stage++;
						break;
					case 3:
						if(time <= 0.0)
						{
							target->getPilot()->orderPowerDown(unitOrder, OrderOriginType::self);
							time = scenarioTime;
						}
						else if(scenarioTime > time + 3.0)
						{
							stage++;
							time = scenarioTime;
						}
						break;
					case 4:	// do the deed
						((GroundVehiclePtr)warrior->getVehicle())->refitting = true;
						if(scenarioTime > time + Mover::refitTime)
						{
							float pointsUsed = 0;
							int32_t result = ((std::unique_ptr<Mover>)target)->recover(warrior->getVehicle()->getRecoverPoints(), pointsUsed);
							warrior->getVehicle()->burnRecoverPoints(pointsUsed);
							if(MPlayer)
							{
								STOP((" Need to implement recover for multiplayer "));
								WeaponShotInfo recoverInfo;
								recoverInfo.init(nullptr,
												 -5,
												 pointsUsed,
												 GROUNDVEHICLE_LOCATION_TURRET,
												 0.0);
								MPlayer->addWeaponHitChunk((GameObjectPtr)warrior->getVehicle(), &recoverInfo);
								recoverInfo.init(nullptr,
												 -5,
												 pointsUsed,
												 0,
												 0.0);
								MPlayer->addWeaponHitChunk(target, &recoverInfo, TRUE);
							}
							stage += result;
							if(result == 0)
								time = scenarioTime;
						}
						break;
					case 5: // done. power-up (or whatever) and report
						((GroundVehiclePtr)warrior->getVehicle())->recovering = false;
						target->getPilot()->orderPowerUp(unitOrder, OrderOriginType::self);
						result = TACORDER_SUCCESS;
						((std::unique_ptr<Mover>)target)->recoverBuddyWID = 0;
						warrior->getVehicle()->recoverBuddyWID = 0;
						warrior->getVehicle()->setSelected(0);
						warrior->getVehicle()->disable(POWER_USED_UP) ;
						break;
					case -1:	// execute failed
						result = TACORDER_SUCCESS;
						break;
				}
			}
#endif
		break;
	}
	return (result);
}

//---------------------------------------------------------------------------

GameObjectPtr
TacticalOrder::getTarget(void)
{
	return (ObjectManager->getByWatchID(targetWID));
}

//---------------------------------------------------------------------------

bool TacticalOrder::equals(TacticalOrder* tacOrder)
{
	if (code != tacOrder->code)
		return (false);
	if (targetWID != tacOrder->targetWID)
		return (false);
	if (stage != tacOrder->stage)
		return (false);
	if (unitOrder != tacOrder->unitOrder)
		return (false);
	if (origin != tacOrder->origin)
		return (false);
	if (groupFlags != tacOrder->groupFlags)
		return (false);
	switch (code)
	{
	case TacticalOrderCode::wait:
		break;
	case TacticalOrderCode::movetopoint:
	{
		if (moveparams.faceObject != tacOrder->moveparams.faceObject)
			return (false);
		if (moveparams.wait != tacOrder->moveparams.wait)
			return (false);
		if (moveparams.mode != tacOrder->moveparams.mode)
			return (false);
		if (moveparams.escapeTile != tacOrder->moveparams.escapeTile)
			return (false);
		if (moveparams.wayPath.numPoints != tacOrder->moveparams.wayPath.numPoints)
			return (false);
		int32_t numCoords = moveparams.wayPath.numPoints * 3;
		for (size_t i = 0; i < numCoords; i++)
		{
			if (moveparams.wayPath.points[i] != tacOrder->moveparams.wayPath.points[i])
				return (false);
			if (moveparams.wayPath.mode[i] != tacOrder->moveparams.wayPath.mode[i])
				return (false);
		}
	}
	break;
	case TacticalOrderCode::movetoobject:
		break;
	case TacticalOrderCode::jumptopoint:
		break;
	case TacticalOrderCode::jumptoobject:
		break;
	case TacticalOrderCode::traversepath:
		break;
	case TacticalOrderCode::patrolpath:
		break;
	case TacticalOrderCode::escort:
		break;
	case TacticalOrderCode::follow:
		break;
	case TacticalOrderCode::guard:
		break;
	case TacticalOrderCode::stop:
		break;
	case TacticalOrderCode::powerup:
		break;
	case TacticalOrderCode::powerdown:
		break;
	case TacticalOrderCode::waypointsdone:
		break;
	case TacticalOrderCode::eject:
		break;
	case TacticalOrderCode::attackobject:
		if (attackParams.method != tacOrder->attackParams.method)
			return (false);
		if (attackParams.range != tacOrder->attackParams.range)
			return (false);
		if (attackParams.aimLocation != tacOrder->attackParams.aimLocation)
			return (false);
		if (attackParams.pursue != tacOrder->attackParams.pursue)
			return (false);
		if (attackParams.obliterate != tacOrder->attackParams.obliterate)
			return (false);
		if (attackParams.tactic != tacOrder->attackParams.tactic)
			return (false);
		break;
	case TacticalOrderCode::attackpoint:
		if (attackParams.method != tacOrder->attackParams.method)
			return (false);
		if (attackParams.range != tacOrder->attackParams.range)
			return (false);
		if (attackParams.aimLocation != tacOrder->attackParams.aimLocation)
			return (false);
		if (attackParams.pursue != tacOrder->attackParams.pursue)
			return (false);
		if (attackParams.obliterate != tacOrder->attackParams.obliterate)
			return (false);
		if (attackParams.targetpoint != tacOrder->attackParams.targetpoint)
			return (false);
		if (attackParams.tactic != tacOrder->attackParams.tactic)
			return (false);
		break;
	case TacticalOrderCode::holdfire:
		break;
	case TacticalOrderCode::withdraw:
		break;
	case TacticalOrderCode::scramble:
		break;
	case TacticalOrderCode::capture:
		break;
	case TacticalOrderCode::refit:
		break;
	case TacticalOrderCode::getfixed:
		break;
	case TacticalOrderCode::loadintocarrier:
		break;
	case TacticalOrderCode::deployelementals:
		break;
	}
	return (true);
}

//---------------------------------------------------------------------------

void TacticalOrder::destroy(void)
{
}

//***************************************************************************

void TacticalOrder::debugString(std::unique_ptr<MechWarrior> pilot, std::wstring_view s)
{
	GameObjectPtr target = ObjectManager->getByWatchID(targetWID);
	switch (code)
	{
	case TacticalOrderCode::none:
		strcpy(s, "none");
		break;
	case TacticalOrderCode::wait:
		strcpy(s, "wait");
		break;
	case TacticalOrderCode::movetopoint:
	{
		Stuff::Vector3D curPos;
		curPos.x = moveparams.wayPath.points[0];
		curPos.y = moveparams.wayPath.points[1];
		curPos.z = 0.0;
		int32_t cell[2];
		land->worldToCell(curPos, cell[0], cell[1]);
		sprintf(s, "move to pt (%.0f, %.0f) [%d, %d]", moveparams.wayPath.points[0],
			moveparams.wayPath.points[1], cell[0], cell[1]);
	}
	break;
	case TacticalOrderCode::movetoobject:
		sprintf(s, "move to obj (%d)", target ? target->getPartId() : 0);
		break;
	case TacticalOrderCode::jumptopoint:
	{
		Stuff::Vector3D curPos;
		curPos.x = moveparams.wayPath.points[0];
		curPos.y = moveparams.wayPath.points[1];
		curPos.z = 0.0;
		int32_t cell[2];
		land->worldToCell(curPos, cell[0], cell[1]);
		sprintf(s, "jump to pt (%.0f, %.0f)", moveparams.wayPath.points[0],
			moveparams.wayPath.points[1], cell[0], cell[1]);
	}
	break;
	case TacticalOrderCode::jumptoobject:
		sprintf(s, "jump to obj (%d)", target ? target->getPartId() : 0);
		break;
	case TacticalOrderCode::traversepath:
		strcpy(s, "traverse path");
		break;
	case TacticalOrderCode::patrolpath:
		strcpy(s, "patrol path");
		break;
	case TacticalOrderCode::escort:
		strcpy(s, "escort");
		break;
	case TacticalOrderCode::follow:
		strcpy(s, "follow");
		break;
	case TacticalOrderCode::guard:
		strcpy(s, "guard");
		break;
	case TacticalOrderCode::stop:
		strcpy(s, "stop");
		break;
	case TacticalOrderCode::powerup:
		strcpy(s, "power up");
		break;
	case TacticalOrderCode::powerdown:
		strcpy(s, "power down");
		break;
	case TacticalOrderCode::waypointsdone:
		strcpy(s, "waypoints done");
		break;
	case TacticalOrderCode::eject:
		strcpy(s, "eject");
		break;
	case TacticalOrderCode::attackobject:
	{
		std::wstring_view tacticCode[] = {"  ", "RT", "LT", "RR", "SF", "TT", "JS"};
		sprintf(s, "attack obj (%d) %s", target ? target->getPartId() : 0,
			tacticCode[attackParams.tactic]);
	}
	break;
	case TacticalOrderCode::attackpoint:
	{
		std::wstring_view tacticCode[] = {"  ", "RT", "LT", "RR", "SF", "TT", "JS"};
		sprintf(s, "attack pt (%.0f, %.0f) %s", attackParams.targetpoint.x,
			attackParams.targetpoint.y, tacticCode[attackParams.tactic]);
	}
	break;
	case TacticalOrderCode::holdfire:
		strcpy(s, "hold fire");
		break;
	case TacticalOrderCode::withdraw:
		strcpy(s, "withdraw");
		break;
	case TacticalOrderCode::scramble:
		strcpy(s, "scramble");
		break;
	case TacticalOrderCode::capture:
	{
		Stuff::Vector3D goalPos;
		GameObjectPtr obj;
		int32_t goalType = pilot->getMoveGoal(&goalPos, &obj);
		if ((goalType > 0) && obj)
		{
			sprintf(s, "capture obj (%d) [%d]", target ? target->getPartId() : 0, obj->getPartId());
		}
		else
			sprintf(s, "capture obj (%d) [GOAL?]", target ? target->getPartId() : 0);
	}
	break;
	case TacticalOrderCode::refit:
		sprintf(s, "refit obj (%d)", target ? target->getPartId() : 0);
		break;
	case TacticalOrderCode::getfixed:
		sprintf(s, "get fixed (%d)", target ? target->getPartId() : 0);
		break;
	case TacticalOrderCode::loadintocarrier:
		strcpy(s, "load carrier");
		break;
	case TacticalOrderCode::deployelementals:
		strcpy(s, "deploy elementals");
		break;
	}
}

//***************************************************************************
