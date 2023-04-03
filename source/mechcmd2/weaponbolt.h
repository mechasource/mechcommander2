//---------------------------------------------------------------------------
//
// weaponBolt.h -- File contains the NEW and IMPROVED Weapon class for MC2
//					ALL weapons go through this class now.
//
//	MechCommander 2
//
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#pragma once

#ifndef WEAPONBOLT_H
#define WEAPONBOLT_H
//---------------------------------------------------------------------------

//#include "dweaponbolt.h"
//#include "gameobj.h"
//#include "objtype.h"
//#include "objmgr.h"
//#include "stuff/stuff.h"
//#include "gosfx/gosfxheaders.h"

//---------------------------------------------------------------------------
// Macro Definitions
#define ALL_COLORS 4

//---------------------------------------------------------------------------
class WeaponBoltType : public ObjectType
{
	// Data Members
	//-------------
public:
	int32_t fireSoundFX;
	int32_t hitEffectObjNum;
	int32_t missEffectObjNum;
	int32_t lightEffectObjNum;
	int32_t fireEffect;
	int32_t trailEffect;

	uint32_t frontRGB;
	uint32_t backRGB;
	uint32_t middleRGB;
	uint32_t midEdgeRGB;

	float projLength;
	float bulgeLength;
	float bulgewidth;
	float velocity;

	uint8_t boltAlpha;
	uint8_t edgeAlpha;

	bool lightSource;
	float maxRadius;
	float outRadius;
	uint32_t lightRGB;
	float intensity;

	bool isBeam;
	float beamDuration;
	float beamWiggle;
	wchar_t* textureName;
	float uvAnimRate;
	float uvRepeat;
	float unitLength;
	bool mipTexture;
	bool arcEffect;
	float archeight;
	float afterHitTime;
	float areaEffectDmg;
	float areaEffectRad;

	// Member Functions
	//-----------------
public:
	void init(void)
	{
		ObjectType::init(void);
		objectTypeClass = WEAPONBOLT_TYPE;
		objectClass = WEAPONBOLT;
		fireSoundFX = hitEffectObjNum = missEffectObjNum = lightEffectObjNum = fireEffect =
			trailEffect = 0;
		frontRGB = middleRGB = backRGB = midEdgeRGB = 0;
		projLength = bulgeLength = bulgewidth = velocity = 0.0;
		boltAlpha = edgeAlpha = 0xff;
		lightSource = false;
		maxRadius = outRadius = 0.0f;
		lightRGB = 0x00000000;
		intensity = 1.0;
		isBeam = false;
		beamDuration = 0.0f;
		beamWiggle = 0.0f;
		mipTexture = false;
		textureName = nullptr;
		arcEffect = false;
		archeight = -1.0f;
		afterHitTime = 0.0f;
		areaEffectDmg = areaEffectRad = 0.0f;
	}

	WeaponBoltType(void)
	{
		init(void);
	}

	virtual int32_t init(std::unique_ptr<File> objFile, uint32_t fileSize);
	int32_t init(FitIniFilePtr objFile);

	~WeaponBoltType(void)
	{
		destroy(void);
	}

	virtual void destroy(void);

	virtual GameObjectPtr createInstance(void);

	//--------------------------------------------------
	// This functions creates lights for the below bolts
	TG_LightPtr getLight(void);

	virtual bool handleCollision(GameObjectPtr collidee, GameObjectPtr collider);
	virtual bool handleDestruction(GameObjectPtr collidee, GameObjectPtr collider);
};

//---------------------------------------------------------------------------
typedef struct _WeaponBoltData : public GameObjectData
{
	GameObjectWatchID ownerWID;
	int32_t hotSpotNumber;

	GameObjectWatchID targetWID;
	int32_t targetHotSpot;
	Stuff::Vector3D targetposition;
	float distanceToTarget;
	float halfDistanceToTarget;

	WeaponShotInfo weaponShot;

	Stuff::Vector3D laserPosition;
	Stuff::Vector3D laserVertices[4];
	Stuff::Vector3D laserSide[4];

	int32_t effectId;
	bool hitTarget;

	float timeLeft;
	Stuff::Vector3D hsPos;

	float hitLeft;
	uint32_t mcTextureHandle;
	uint32_t texturehandle;
	float startUV;
	float goalheight;
} WeaponBoltData;

class WeaponBolt : public GameObject
{
	// Data Members
	//-------------
protected:
	GameObjectWatchID ownerWID;
	int32_t hotSpotNumber;

	GameObjectWatchID targetWID;
	int32_t targetHotSpot;
	Stuff::Vector3D* targetposition;
	float distanceToTarget;
	float halfDistanceToTarget;

	WeaponShotInfo weaponShot;

	Stuff::Vector3D laserPosition;
	Stuff::Vector3D laserVertices[4];
	Stuff::Vector3D laserSide[4];

	TG_LightPtr pointLight;
	uint32_t lightId;

	int32_t effectId;
	bool hitTarget;

	// NEW  GOS FX
	gosFX::Effect* gosEffect;
	gosFX::Effect* muzzleEffect;
	gosFX::Effect* hitEffect;
	gosFX::Effect* missEffect;
	gosFX::Effect* waterMissEffect;

	float timeLeft;
	Stuff::Vector3D hsPos;

	float hitLeft;
	uint32_t mcTextureHandle;
	uint32_t texturehandle;
	float startUV;
	float goalheight;

	// Member Functions
	//-----------------
public:
	virtual void init(bool create) { }

	WeaponBolt(void)
		: GameObject()
	{
		init(true);
		hotSpotNumber = 0;
		targetHotSpot = 0;
		targetposition = nullptr;
		distanceToTarget = 0.0;
		ownerWID = targetWID = 0;
		pointLight = nullptr;
		lightId = 0xffffffff;
		gosEffect = nullptr;
		muzzleEffect = nullptr;
		hitEffect = nullptr;
		missEffect = nullptr;
		waterMissEffect = nullptr;
		effectId = -1;
		hitTarget = false;
		timeLeft = 0.0;
		hsPos.Zero(void);
		hitLeft = 0.0f;
		startUV = 0.0f;
		mcTextureHandle = 0;
		texturehandle = 0xffffffff;
		weaponShot.damage = 0.0f;
		goalheight = 0.0f;
	}

	~WeaponBolt(void)
	{
		destroy(void);
	}

	virtual void destroy(void);

	virtual int32_t update(void);
	virtual void render(void);

	virtual void init(bool create, ObjectTypePtr _type);

	virtual void init(int32_t fxId)
	{
		effectId = fxId;
	}

	virtual void setExists(bool set)
	{
		GameObject::setExists(set);
		// If we are setting this effect to not existing, clean up its RAM!!
		if (!set)
		{
			destroy(void);
		}
	}

	bool isVisible(void);

	void setOwner(GameObjectPtr who);
	void setTarget(GameObjectPtr who);
	void setTargetPosition(Stuff::Vector3D pos);

	void connect(GameObjectPtr source, GameObjectPtr dest, WeaponShotInfo* shotInfo = nullptr,
		int32_t sourceHS = 0, int32_t targetHS = 0)
	{
		ownerWID = source->getWatchID(void);
		targetWID = dest->getWatchID(void);
		hotSpotNumber = sourceHS;
		targetHotSpot = targetHS;
		if (dest->isMech())
		{
			targetHotSpot = 0;
			if (RollDice(50))
				targetHotSpot = 1;
		}
		else // Vehicles need to know to use the hitnode for this hotspot and
			// NOT the weaponNode!
		{
			targetHotSpot = -1;
		}
		if (shotInfo)
			weaponShot = *shotInfo;
		// If this is an AreaEffect weapon, NO target WID, hit the target's
		// CURRENT LOCATION!!!
		if (((WeaponBoltTypePtr)getObjectType())->hitEffectObjNum && (((WeaponBoltTypePtr)getObjectType())->areaEffectDmg > 0.0f) && (((WeaponBoltTypePtr)getObjectType())->areaEffectRad > 0.0f))
		{
			Stuff::Vector3D targetLoc;
			targetLoc = ObjectManager->getByWatchID(targetWID)->getPosition(void);
			setTargetPosition(targetLoc);
			targetWID = 0;
		}
		// Reset Node recycle time to either BASE or beam duration.
		GameObjectPtr myOwner = ObjectManager->getByWatchID(ownerWID);
		if (((WeaponBoltTypePtr)getObjectType())->isBeam)
			myOwner->appearance->setWeaponNodeRecycle(
				sourceHS, ((WeaponBoltTypePtr)getObjectType())->beamDuration);
		else
			myOwner->appearance->setWeaponNodeRecycle(sourceHS, BASE_NODE_RECYCLE_TIME);
	}

	void connect(GameObjectPtr source, Stuff::Vector3D targetLoc,
		WeaponShotInfo* shotInfo = nullptr, int32_t sourceHS = 0)
	{
		ownerWID = source->getWatchID(void);
		targetWID = 0;
		hotSpotNumber = sourceHS;
		setTargetPosition(targetLoc);
		if (shotInfo)
			weaponShot = *shotInfo;
		// Reset Node recycle time to either BASE or beam duration.
		GameObjectPtr myOwner = ObjectManager->getByWatchID(ownerWID);
		if (((WeaponBoltTypePtr)getObjectType())->isBeam)
			myOwner->appearance->setWeaponNodeRecycle(
				sourceHS, ((WeaponBoltTypePtr)getObjectType())->beamDuration);
		else
			myOwner->appearance->setWeaponNodeRecycle(sourceHS, BASE_NODE_RECYCLE_TIME);
	}

	virtual void Save(PacketFilePtr file, int32_t packetNum);

	void Load(WeaponBoltData* data);

	void CopyTo(WeaponBoltData* data);

	void finishNow(void);
};

//---------------------------------------------------------------------------
#endif
