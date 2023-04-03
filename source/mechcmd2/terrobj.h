//---------------------------------------------------------------------------
//
//	terrobj.h -- File contains the Terrain Object Definition
//
//	MechCommander 2
//
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#pragma once

#ifndef TERROBJ_H
#define TERROBJ_H

//#include "dterrobj.h"
//#include "objtype.h"
//#include "gameobj.h"
//#include "dcarnage.h"
//#include "gosfx/gosfxheaders.h"

//---------------------------------------------------------------------------
// Macro Definitions
//#define NO_RAM_FOR_BUILDING				0xDCDC0006
//#define NO_APPEARANCE_TYPE_FOR_BLD		0xDCDC0007
//#define NO_APPEARANCE_FOR_BLD			0xDCDC0008
//#define APPEARANCE_NOT_VFX_APPEAR		0xDCDC0009

//---------------------------------------------------------------------------

enum class
{
	TERROBJ_NONE,
	TERROBJ_TREE,
	TERROBJ_BRIDGE,
	TERROBJ_FOREST,
	TERROBJ_WALL_HEAVY,
	TERROBJ_WALL_MEDIUM,
	TERROBJ_WALL_LIGHT,
	NUM_TERROBJ_SUBTYPES
} TerrainObjectSubType;

class TerrainObjectType : public ObjectType
{

public:
	wchar_t subType;
	float damageLevel;
	int32_t collisionOffsetX;
	int32_t collisionOffsetY;
	bool setImpassable;
	int32_t xImpasse;
	int32_t yImpasse;
	float explDmg;
	float explRad;
	uint32_t fireTypeHandle;

public:
	void init(void);

	TerrainObjectType(void)
	{
		ObjectType::init(void);
		init(void);
	}

	virtual void initMiscTerrObj(int32_t objTypeNum);

	virtual int32_t init(std::unique_ptr<File> objFile, uint32_t fileSize);

	int32_t init(FitIniFilePtr objFile);

	~TerrainObjectType(void)
	{
		destroy(void);
	}

	float getDamageLevel(void)
	{
		return (damageLevel);
	}

	virtual void destroy(void);

	virtual GameObjectPtr createInstance(void);

	virtual float getBurnDmg(void)
	{
		return (0.0);
	}

	virtual float getBurnTime(void)
	{
		return (0.0);
	}

	virtual bool handleCollision(GameObjectPtr collidee, GameObjectPtr collider);

	virtual bool handleDestruction(GameObjectPtr collidee, GameObjectPtr collider);
};

//---------------------------------------------------------------------------
typedef struct _TerrainObjectData : public GameObjectData
{
	float damage;
	int32_t vertexNumber;
	int32_t blockNumber;
	float pitchAngle;
	float fallRate;
	GameObjectWatchID powerSupply;
	int16_t cellFootprint[4];
	Stuff::Vector3D vectorFootprint[4];
	int16_t numSubAreas0;
	int16_t numSubAreas1;
	int16_t subAreas0[MAX_SPECIAL_SUB_AREAS];
	int16_t subAreas1[MAX_SPECIAL_SUB_AREAS];
	uint8_t listID;

	uint8_t numCellsCovered;
	int16_t cellsCovered[81];
} TerrainObjectData;

#define MAXLEN_TERRAINOBJECT_NAME 35

class TerrainObject : public GameObject
{

public:
	float damage;
	int32_t vertexNumber;
	int32_t blockNumber;
	float pitchAngle;
	float fallRate;
	GameObjectWatchID powerSupply;
	int16_t cellFootprint[4];
	Stuff::Vector3D vectorFootprint[4];
	int16_t numSubAreas0;
	int16_t numSubAreas1;
	int16_t* subAreas0;
	int16_t* subAreas1;
	uint8_t listID;

	uint8_t numCellsCovered;
	int16_t* cellsCovered;
	gosFX::Effect* bldgDustPoofEffect;

	static int32_t cellArray[9];

public:
	virtual void init(bool create)
	{
		objectClass = TERRAINOBJECT;
		setFlag(OBJECT_FLAG_JUSTCREATED, true);
		appearance = nullptr;
		vertexNumber = 0;
		blockNumber = 0;
		damage = 0.0;
		pitchAngle = 0.0f;
		fallRate = 0.0f;
		powerSupply = nullptr;
		cellFootprint[0] = -1;
		cellFootprint[1] = -1;
		cellFootprint[2] = -1;
		cellFootprint[3] = -1;
		numSubAreas0 = 0;
		numSubAreas1 = 0;
		subAreas0 = nullptr;
		subAreas1 = nullptr;
		listID = 255;
		numCellsCovered = 0;
		cellsCovered = nullptr;
		bldgDustPoofEffect = nullptr;
	}

	TerrainObject(void)
		: GameObject()
	{
		init(true);
	}

	~TerrainObject(void)
	{
		destroy(void);
	}

	virtual void updateDebugWindow(GameDebugWindow* debugWindow);

	virtual std::wstring_view getName(void);

	virtual void killFire(void);

	void lightOnFire(float timeToBurn);

	virtual void destroy(void);

	virtual int32_t update(void);

	virtual void render(void);

	virtual void renderShadows(void);

	virtual void init(bool create, ObjectTypePtr objType);

	virtual int32_t handleWeaponHit(WeaponShotInfoPtr shotInfo, bool addMultiplayChunk = false);

	virtual void setTerrainPosition(
		const Stuff::Vector3D& position, const Stuff::Vector2DOf<int32_t>& numbers);

	virtual void setDamage(int32_t newDamage); // Damage encodes which groundtile to use, too.

	virtual void setRotation(float rot);

	virtual float getDamage(void)
	{
		return (damage);
	}

	virtual float getDamageLevel(void)
	{
		return ((TerrainObjectTypePtr)getObjectType())->getDamageLevel(void);
	}

	virtual float getDestructLevel(void)
	{
		return (getDamageLevel() - damage);
	}

	virtual float getStatusRating(void);

	int32_t getSubType(void)
	{
		return (((TerrainObjectTypePtr)getObjectType())->subType);
	}

	virtual int32_t kill(void)
	{
		// Do nothing for now.  Later, Buildings may do something.
		return NO_ERROR;
	}

	bool isVisible(void);

	virtual int32_t getLineOfSightNodes(int32_t eyeCellRow, int32_t eyeCellCol, int32_t* cells);

	virtual bool isTerrainObject(void)
	{
		return (true);
	}

	virtual void getBlockAndVertexNumber(int32_t& blockNum, int32_t& vertexNum)
	{
		blockNum = blockNumber;
		vertexNum = vertexNumber;
	}

	virtual void rotate(float yaw, float pitch);

	virtual float getAppearRadius(void)
	{
		return appearance->getRadius(void);
	}

	virtual void setPowerSupply(GameObjectPtr power)
	{
		powerSupply = power->getWatchID(void);
	}

	virtual void calcCellFootprint(Stuff::Vector3D& pos);

	virtual bool calcAdjacentAreaCell(
		int32_t moveLevel, int32_t areaID, int32_t& adjRow, int32_t& adjCol);

	void calcSubAreas(int32_t numCells, int16_t cells[MAX_GAME_OBJECT_CELLS][2]);

	void markMoveMap(bool passable);

	void openSubAreas(void);

	void closeSubAreas(void);

	void setSubAreasTeamId(int32_t id);

	virtual void Save(PacketFilePtr file, int32_t packetNum);

	void Load(TerrainObjectData* data);

	void CopyTo(TerrainObjectData* data);

	virtual Stuff::Vector3D getPositionFromHS(int32_t weaponType)
	{
		//-----------------------------------------
		// Hotspot for buildings is position plus
		// some Z based on OBB to make Effect visible.
		// If this doesn't work, replace with art defined site.
		Stuff::Vector3D hsPos = position;
		if (appearance)
			hsPos.z += appearance->getTopZ() * 0.5f;
		return (hsPos);
	}

	virtual Stuff::Vector3D getLOSPosition(void)
	{
		//-----------------------------------------
		// Hotspot for buildings is position plus
		// some Z based on OBB to make Effect visible.
		//
		// Use THIS position for LOS Calc!!!
		Stuff::Vector3D hsPos = position;
		if (appearance)
			hsPos.z += appearance->getTopZ() * 0.5f;
		return (hsPos);
	}
};

//---------------------------------------------------------------------------
#endif
