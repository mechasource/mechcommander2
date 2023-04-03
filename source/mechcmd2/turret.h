//***************************************************************************
//
//	turret.h -- File contains the Turret Object Class
//
//	MechCommander 2
//
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#pragma once

#ifndef TURRET_H
#define TURRET_H

//---------------------------------------------------------------------------

//#include "dturret.h"
//#include "objmgr.h"
//#include "terrobj.h"

//---------------------------------------------------------------------------
// Macro Definitions
#define NO_RAM_FOR_BUILDING 0xDCDC0006
#define NO_APPEARANCE_TYPE_FOR_BLD 0xDCDC0007
#define NO_APPEARANCE_FOR_BLD 0xDCDC0008
#define APPEARANCE_NOT_VFX_APPEAR 0xDCDC0009

#define MAX_TURRET_WEAPONFIRE_CHUNKS 32

#define MAX_TURRET_WEAPONS 4
//---------------------------------------------------------------------------

class TurretType : public ObjectType
{

protected:
	float damageLevel;
	// uint32_t	dmgLevelClosed;

public:
	uint32_t blownEffectId;
	uint32_t normalEffectId;
	uint32_t damageEffectId;

	float baseTonnage;

	float explDmg;
	float explRad;

	float littleExtent;
	float LOSFactor;

	float engageRadius;
	float turretYawRate;
	int32_t weaponMasterId[MAX_TURRET_WEAPONS];
	int32_t pilotSkill;
	float punch;

	int32_t turretTypeName;
	int32_t buildingDescriptionID;

public:
	void init(void)
	{
		ObjectType::init(void);
		objectTypeClass = TURRET_TYPE;
		objectClass = TURRET;
		damageLevel = 0.0;
		blownEffectId = 0xFFFFFFFF;
		normalEffectId = 0xFFFFFFFF;
		damageEffectId = 0xFFFFFFFF;
		explDmg = explRad = 0.0;
		baseTonnage = 0.0;
		weaponMasterId[0] = weaponMasterId[1] = weaponMasterId[2] = weaponMasterId[3] = -1;
		pilotSkill = 0;
		punch = 0.0;
		turretYawRate = 0.0;
		turretTypeName = 0;
		LOSFactor = 1.0f;
	}

	TurretType(void)
	{
		init(void);
	}

	virtual int32_t init(std::unique_ptr<File> objFile, uint32_t fileSize);

	int32_t init(FitIniFilePtr objFile);

	~TurretType(void)
	{
		destroy(void);
	}

	float getDamageLevel(void)
	{
		return (damageLevel);
	}

	virtual void destroy(void);

	virtual GameObjectPtr createInstance(void);

	virtual bool handleCollision(GameObjectPtr collidee, GameObjectPtr collider);

	virtual bool handleDestruction(GameObjectPtr collidee, GameObjectPtr collider);
};

//---------------------------------------------------------------------------
typedef struct _TurretData : public TerrainObjectData
{
	wchar_t teamId;
	float turretRotation;
	bool didReveal;
	GameObjectWatchID targetWID;
	float readyTime[MAX_TURRET_WEAPONS];
	float lastFireTime[MAX_TURRET_WEAPONS];
	float minRange; // current min attack range
	float maxRange; // current max attack range
	int32_t numFunctionalWeapons; // takes into account damage, etc.

	float idleWait;
	Stuff::Vector3D idlePosition;
	Stuff::Vector3D oldPosition;
	uint32_t parentId;
	GameObjectWatchID parent;
	int32_t currentWeaponNode;
} TurretData;

class Turret : public TerrainObject
{

public:
	wchar_t teamId;
	float turretRotation;
	bool didReveal;
	GameObjectWatchID targetWID;
	float readyTime[MAX_TURRET_WEAPONS];
	float lastFireTime[MAX_TURRET_WEAPONS];
	float minRange; // current min attack range
	float maxRange; // current max attack range
	int32_t numFunctionalWeapons; // takes into account damage, etc.

	int32_t netRosterIndex;
	int32_t numWeaponFireChunks[2];
	uint32_t weaponFireChunks[2][MAX_TURRET_WEAPONFIRE_CHUNKS];

	TG_LightPtr pointLight;
	uint32_t lightId;
	float idleWait;
	Stuff::Vector3D idlePosition;
	Stuff::Vector3D oldPosition;
	uint32_t parentId;
	GameObjectWatchID parent;
	int32_t currentWeaponNode;

	static bool turretsEnabled[MAX_TEAMS];

public:
	void init(bool create);

	Turret(void)
		: TerrainObject()
	{
		init(true);
	}

	~Turret(void)
	{
		destroy(void);
	}

	virtual void updateDebugWindow(GameDebugWindow* debugWindow);

	virtual int32_t setTeamId(int32_t _teamId, bool setup);

	virtual int32_t getTeamId(void)
	{
		return (teamId);
	}

	virtual TeamPtr getTeam(void);

	virtual bool isFriendly(TeamPtr team);

	virtual bool isEnemy(TeamPtr team);

	virtual bool isNeutral(TeamPtr team);

	virtual void destroy(void);

	virtual int32_t update(void);

	virtual void render(void);

	virtual void init(bool create, ObjectTypePtr _type);

	virtual int32_t handleWeaponHit(WeaponShotInfoPtr shotInfo, bool addMultiplayChunk = false);

	int32_t getNetRosterIndex(void)
	{
		return (netRosterIndex);
	}

	void setNetRosterIndex(int32_t index)
	{
		netRosterIndex = index;
	}

	void lightOnFire(float timeToBurn);

	virtual int32_t kill(void)
	{
		// Do nothing for now.  Later, Buildings may do something.
		return NO_ERROR;
	}

	virtual bool isBuilding(void)
	{
		return (true);
	}

	virtual void getBlockAndVertexNumber(int32_t& blockNum, int32_t& vertexNum)
	{
		blockNum = blockNumber;
		vertexNum = vertexNumber;
	}

	bool isWeaponReady(int32_t weaponId);

	bool isWeaponMissile(int32_t weaponId);

	bool isWeaponStreak(int32_t weaponId);

	float calcAttackChance(GameObjectPtr target, int32_t* range, int32_t weaponId);

	void recordWeaponFireTime(int32_t weaponId);

	void startWeaponRecycle(int32_t weaponId);

	int32_t getNumWeaponFireChunks(int32_t which)
	{
		return (numWeaponFireChunks[which]);
	}

	int32_t clearWeaponFireChunks(int32_t which);

	int32_t addWeaponFireChunk(int32_t which, WeaponFireChunkPtr chunk);

	int32_t addWeaponFireChunks(int32_t which, uint32_t* packedChunkBuffer, int32_t numChunks);

	int32_t grabWeaponFireChunks(int32_t which, uint32_t* packedChunkBuffer);

	virtual int32_t updateWeaponFireChunks(int32_t which);

	void fireWeapon(GameObjectPtr target, int32_t weaponId);

	virtual Stuff::Vector3D getPositionFromHS(int32_t weaponNum);

	virtual float relFacingTo(Stuff::Vector3D goal, int32_t bodyLocation = -1);

	int32_t handleWeaponFire(int32_t weaponIndex, GameObjectPtr target,
		Stuff::Vector3D* targetpoint, bool hit, float entryAngle, int32_t numMissiles,
		int32_t hitLocation);

	virtual void printFireWeaponDebugInfo(GameObjectPtr target, Stuff::Vector3D* targetpoint,
		int32_t chance, int32_t roll, WeaponShotInfo* shotInfo);

	virtual void printHandleWeaponHitDebugInfo(WeaponShotInfo* shotInfo);

	float getLittleExtent(void)
	{
		return (((TurretTypePtr)getObjectType())->littleExtent);
	}

	virtual bool isLinked(void);

	virtual GameObjectPtr getParent(void);

	virtual void setParentId(uint32_t pId);

	virtual int32_t getDescription()
	{
		return ((TurretType*)getObjectType())->buildingDescriptionID;
	}

	int32_t updateAnimations(void);

	virtual Stuff::Vector3D getLOSPosition(void);

	virtual float getDestructLevel(void)
	{
		return ((TurretTypePtr)getObjectType())->getDamageLevel() - damage;
	}

	virtual void setDamage(float newDamage);

	virtual int32_t calcFireRanges(void);

	virtual void Save(PacketFilePtr file, int32_t packetNum);

	void CopyTo(TurretData* data);

	void Load(TurretData* data);

	virtual void renderShadows(void);
};

//***************************************************************************

#endif
