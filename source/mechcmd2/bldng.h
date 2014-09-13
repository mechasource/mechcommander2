//---------------------------------------------------------------------------
//
//	bldng.h -- File contains the Building Object Class
//
//	MechCommander 2
//
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#pragma once

#ifndef BLDNG_H
#define BLDNG_H

//#include "dbldng.h"
//#include "terrobj.h"
//#include "gameobj.h"
//#include "objtype.h"
//#include "dwarrior.h"

//---------------------------------------------------------------------------

#define NO_RAM_FOR_BUILDING				0xDCDC0006
#define NO_APPEARANCE_TYPE_FOR_BLD		0xDCDC0007
#define NO_APPEARANCE_FOR_BLD			0xDCDC0008
#define APPEARANCE_NOT_VFX_APPEAR		0xDCDC0009

#define	MAX_PRISONERS					4

typedef enum
{
	BUILDING_SUBTYPE_NONE,
	BUILDING_SUBTYPE_WALL,
	BUILDING_SUBTYPE_LANDBRIDGE,
	NUM_BUILDING_SUBTYPES
} BuildingSubType;

//---------------------------------------------------------------------------

class BuildingType : public ObjectType
{

public:

	float			damageLevel;
	float			sensorRange;
	int32_t			teamId;
	float			baseTonnage;
	float			explDmg;
	float			explRad;
	int32_t			buildingTypeName;
	int32_t			buildingDescriptionID;
	int32_t			startBR;
	int32_t			numMarines;
	int32_t			resourcePoints;
	bool			marksImpassableWhenDestroyed;

	bool			canRefit;
	bool			mechBay;							// otherwise it's a vehicle bay.
	bool			capturable;
	bool			powerSource;
	float			perimeterAlarmRange;
	float			perimeterAlarmTimer;
	float			lookoutTowerRange;

	uint32_t	activityEffectId;

public:

	void init(void);

	BuildingType(void)
	{
		ObjectType::init(void);
		init(void);
	}

	virtual int32_t init(FilePtr objFile, uint32_t fileSize);

	int32_t init(FitIniFilePtr objFile);

	~BuildingType(void)
	{
		destroy(void);
	}

	float getDamageLevel(void)
	{
		return(damageLevel);
	}

	virtual void destroy(void);

	virtual GameObjectPtr createInstance(void);


	virtual bool handleCollision(GameObjectPtr collidee, GameObjectPtr collider);

	virtual bool handleDestruction(GameObjectPtr collidee, GameObjectPtr collider);
};

//---------------------------------------------------------------------------
typedef struct _BuildingData : public TerrainObjectData
{
	char					teamId;
	uint8_t			baseTileId;
	char					commanderId;
	GameObjectWatchID		refitBuddyWID;
	uint32_t					parentId;
	GameObjectWatchID		parent;
	uint8_t			listID;
	float					captureTime;

	//PerimeterAlarms
	bool					moverInProximity;
	float					proximityTimer;
	int32_t					updatedTurn;
} BuildingData;

class Building : public TerrainObject
{
public:

	char					teamId;
	uint8_t			baseTileId;
	SensorSystemPtr			sensorSystem;
	char					commanderId;									//If capturable, who last captured it...
	GameObjectWatchID		refitBuddyWID;
	uint32_t					parentId;
	GameObjectWatchID		parent;
	uint8_t			listID;
	float					captureTime;
	float					scoreTime;

	//PerimeterAlarms
	bool					moverInProximity;
	float					proximityTimer;
	int32_t					updatedTurn;

public:

	virtual void init(bool create)
	{
		sensorSystem = nullptr;
		setFlag(OBJECT_FLAG_JUSTCREATED, true);
		appearance = nullptr;
		vertexNumber = 0;
		blockNumber = 0;
		baseTileId = 0;
		commanderId = -1;
		teamId = -1;
		refitBuddyWID = 0;
		parentId = 0xffffffff;
		parent = nullptr;
		powerSupply = nullptr;
		numSubAreas0 = 0;
		numSubAreas1 = 0;
		subAreas0 = nullptr;
		subAreas1 = nullptr;
		listID = 255;
		captureTime = 0.0;
		scoreTime = 0.0;
		moverInProximity = false;
		proximityTimer = 0.0f;
		updatedTurn = -1;
	}

	Building(void) : TerrainObject()
	{
		init(true);
	}

	~Building(void)
	{
		destroy(void);
	}

	virtual void destroy(void);

	virtual int32_t update(void);

	virtual void render(void);

	virtual void init(bool create, ObjectTypePtr objType);

	virtual void setSensorRange(float range);

	void setSensorData(TeamPtr team, float range = -1.0, bool setTeam = true);

	virtual int32_t setTeamId(int32_t _teamId, bool setup);

	virtual int32_t getTeamId(void)
	{
		return(teamId);
	}

	virtual int32_t getDescription()
	{
		return ((BuildingType*)getObjectType())->buildingDescriptionID;
	}

	virtual TeamPtr getTeam(void);

	virtual bool isFriendly(TeamPtr team);

	virtual bool isEnemy(TeamPtr team);

	virtual bool isNeutral(TeamPtr team);

	void lightOnFire(float timeToBurn);

	int32_t updateAnimations(void);

	virtual int32_t handleWeaponHit(WeaponShotInfoPtr shotInfo, bool addMultiplayChunk = false);

	virtual void setDamage(float newDamage);		//Damage encodes which groundtile to use, too.

	virtual int32_t kill(void)
	{
		return(NO_ERROR);
	}

	virtual PSTR getName(void);

	virtual Stuff::Vector3D getPositionFromHS(int32_t weaponType)
	{
		//-----------------------------------------
		// Hotspot for buildings is position plus
		// some Z based on OBB to make Effect visible.
		// If this doesn't work, replace with art defined site.
		Stuff::Vector3D hsPos = position;
		if(appearance)
		{
			hsPos = appearance->getHitNode(void);
			if(hsPos == position)
			{
				hsPos.z += appearance->getTopZ() * 0.5f;
			}
		}
		return(hsPos);
	}

	virtual Stuff::Vector3D getLOSPosition(void)
	{
		//-----------------------------------------
		// Hotspot for buildings is position plus
		// some Z based on OBB to make Effect visible.
		//
		// Use THIS position for LOS Calc!!!
		Stuff::Vector3D hsPos = position;
		if(appearance)
			hsPos.z += appearance->getTopZ() * 0.5f;
		BuildingTypePtr bldgType = ((BuildingTypePtr)getObjectType());
		if((bldgType->lookoutTowerRange > 0.0f) ||
				(bldgType->sensorRange > 0.0f))
		{
			hsPos.z = position.z + 75.0f;
		}
		return(hsPos);
	}

	virtual float getDestructLevel(void)
	{
		return (getDamageLevel() - damage);
	}

	virtual void setRefitBuddy(GameObjectWatchID objWID)
	{
		refitBuddyWID = objWID;
	}

	virtual void openFootPrint(void);

	virtual void closeFootPrint(void);

	bool isVisible(void);

	virtual bool isCaptureable(int32_t capturingTeamID);

	virtual void setCommanderId(int32_t _commanderId);

	virtual int32_t getCommanderId(void)
	{
		return(commanderId);
	}

	virtual float getDamageLevel(void);

	virtual void getBlockAndVertexNumber(int32_t& blockNum, int32_t& vertexNum)
	{
		blockNum = blockNumber;
		vertexNum = vertexNumber;
	}

	virtual bool isBuilding(void)
	{
		return(true);
	}

	virtual bool isTerrainObject(void)
	{
		return(true);
	}

	void createBuildingMarines(void);

	virtual bool isLinked(void);

	virtual GameObjectPtr getParent(void);

	virtual void setParentId(uint32_t pId);

	virtual SensorSystem* getSensorSystem()
	{
		return sensorSystem;
	}

	virtual float getAppearRadius(void)
	{
		return appearance->getRadius(void);
	}

	virtual bool canBeCaptured(void)
	{
		return ((BuildingTypePtr)getObjectType())->capturable;
	}

	virtual bool isSelectable()
	{
		return appearance->isSelectable(void);
	}

	virtual bool isPowerSource(void)
	{
		return ((BuildingTypePtr)getObjectType())->powerSource;
	}

	virtual bool isLit(void)
	{
		if(appearance)
			return appearance->getIsLit(void);
		return false;
	}

	virtual bool isSpecialBuilding(void)
	{
		BuildingTypePtr bldgType = ((BuildingTypePtr)getObjectType());
//			if ((bldgType->getObjTypeNum() == GENERIC_HQ_BUILDING_OBJNUM) ||
//				(bldgType->getObjTypeNum() == GENERIC_DESTRUCTIBLE_RESOURCE_BUILDING_OBJNUM) ||
//				(bldgType->getObjTypeNum() == GENERIC_INDESTRUCTIBLE_RESOURCE_BUILDING_OBJNUM))
//				return(true);
		if((bldgType->perimeterAlarmRange > 0.0f) &&
				(bldgType->perimeterAlarmTimer > 0.0f) ||
				(bldgType->lookoutTowerRange > 0.0f) ||
				(bldgType->sensorRange > 0.0f))
		{
			return true;
		}
		return false;
	}

	virtual bool isLookoutTower(void)
	{
		BuildingTypePtr bldgType = ((BuildingTypePtr)getObjectType());
		if(bldgType->lookoutTowerRange > 0.0f)
		{
			return true;
		}
		return false;
	}

	virtual float getRefitPoints(void)
	{
		return getDamageLevel() - getDamage(void);
	}

	virtual bool burnRefitPoints(float pointsToBurn);

	virtual void Save(PacketFilePtr file, int32_t packetNum);

	void Load(BuildingData* data);

	void CopyTo(BuildingData* data);

};

#endif

//***************************************************************************




