//***************************************************************************
//
//	Carnage.h -- Base class for any and all damage-related effects that
//				 dynamically appear and disappear with int16_t lifetimes.
//				 Things such as fires, explosions and debris.
//
//	MechCommander 2
//
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#pragma once

#ifndef CARNAGE_H
#define CARNAGE_H

//---------------------------------------------------------------------------

//#include "dcarnage.h"
//#include "gameobj.h"
//#include "objtype.h"
//#include <gosfx/gosfxheaders.hpp>

//***************************************************************************
typedef union _CarnageInfo
{
	struct
	{
		float			timeToBurn;
		float			radius;
		int32_t			reallyVisible;
	} fire;

	struct
	{
		float			timer;
		float			radius;
		float			chunkSize;
	} explosion;
} CarnageInfo;

//---------------------------------------------------------------------------
typedef struct _CarnageData : public GameObjectData
{
	CarnageEnumType		carnageType;
	GameObjectWatchID	ownerWID;
	CarnageInfo			info;

	int32_t				effectId;
} CarnageData;

class Carnage : public GameObject
{
	//-------------
	// Data Members

public:

	CarnageEnumType		carnageType;
	GameObjectWatchID	ownerWID;
	CarnageInfo			info;

	static float		maxFireBurnTime;

	//NEW  GOS FX
	int32_t				effectId;
	gosFX::Effect*		gosEffect;

	//Lighting Data
	TG_LightPtr			pointLight;
	uint32_t				lightId;

	float				intensity;
	float				inRadius;
	float 				outRadius;
	float				duration;

	//-----------------
	// member Functions

public:

	virtual void init(bool create);

	Carnage(void) : GameObject()
	{
		init(true);
	}

	~Carnage(void)
	{
		destroy(void);
	}

	virtual void init(bool create, ObjectTypePtr _type);

	virtual void init(CarnageEnumType _carnageType);

	virtual void init(int32_t fxId)
	{
		effectId = fxId;
	}

	virtual void destroy(void);

	virtual int32_t kill(void)
	{
		return(NO_ERROR);
	}

	virtual bool onScreen(void);

	virtual int32_t update(void);

	virtual void render(void);

	virtual void setOwner(GameObjectPtr myOwner)
	{
		if(myOwner)
		{
			ownerWID = myOwner->getWatchID(void);
			if(carnageType == CARNAGE_FIRE)
				myOwner->setFireHandle(getHandle());
		}
		else
			ownerWID = 0;
	}

	GameObjectPtr getOwner(void);

	bool isVisible(int32_t whichFire = 0);

	virtual float getExtentRadius(void)
	{
		if(carnageType == CARNAGE_FIRE)
			return(info.fire.radius);
		else if(carnageType == CARNAGE_EXPLOSION)
			return(info.explosion.radius);
		return(0.0);
	}

	virtual void setExtentRadius(float radius)
	{
		if(carnageType == CARNAGE_FIRE)
			info.fire.radius = radius;
		else if(carnageType == CARNAGE_EXPLOSION)
			info.explosion.radius = radius;
	}

	virtual void handleStaticCollision(void);

	void addTimeLeft(float seconds);

	void finishNow(void);

	virtual void Save(PacketFilePtr file, int32_t packetNum);

	void CopyTo(CarnageData* data);

	void Load(CarnageData* data);
};

//---------------------------------------------------------------------------
class FireType : public ObjectType
{
	//-------------
	// Data Members

public:

	float			damageLevel;
	uint32_t	soundEffectId;
	uint32_t	lightObjectId;

	uint32_t 	startLoopFrame;
	uint32_t	endLoopFrame;
	uint32_t	numLoops;

	float			maxExtent;		//How Good am I at setting off other fires
	float			timeToMax;		//How int32_t before I grow to MaxExtent size

	int32_t			totalFires;

	float*			fireOffsetX;
	float*			fireOffsetY;
	float*			fireDelay;

	int32_t*			fireRandomOffsetX;
	int32_t*			fireRandomOffsetY;
	int32_t*			fireRandomDelay;

	//-----------------
	// Member Functions

public:

	void init(void)
	{
		ObjectType::init(void);
		objectTypeClass = FIRE_TYPE;
		objectClass = FIRE;
		damageLevel = 0.0;
		soundEffectId = -1;
		timeToMax = 0.0;
		maxExtent = 0.0;
		totalFires = 1;
		fireOffsetX = nullptr;
		fireOffsetY = nullptr;
		fireDelay = nullptr;
		fireRandomOffsetX = nullptr;
		fireRandomOffsetY = nullptr;
		fireRandomDelay = nullptr;
	}

	FireType(void)
	{
		init(void);
	}

	virtual int32_t init(FilePtr objFile, uint32_t fileSize);

	int32_t init(FitIniFilePtr objFile);

	~FireType(void)
	{
		destroy(void);
	}

	virtual void destroy(void);

	virtual GameObjectPtr createInstance(void);

	virtual bool handleCollision(GameObjectPtr collidee, GameObjectPtr collider);

	virtual bool handleDestruction(GameObjectPtr collidee, GameObjectPtr collider);
};

//---------------------------------------------------------------------------

class ExplosionType : public ObjectType
{
public:

	float				damageLevel;
	uint32_t		soundEffectId;
	uint32_t		lightObjectId;
	int32_t				explRadius;
	float				chunkSize;

	float 				lightMinMaxRadius;
	float				lightMaxMaxRadius;
	float 				lightOutMinRadius;
	float 				lightOutMaxRadius;
	uint32_t 				lightRGB;
	float 				maxIntensity;
	float 				minIntensity;
	float				duration;
	float 				delayUntilCollidable;

public:

	void init(void)
	{
		ObjectType::init(void);
		objectTypeClass = EXPLOSION_TYPE;
		objectClass = EXPLOSION;
		damageLevel = 0.0;
		soundEffectId = -1;
		explRadius = 0;
		chunkSize = 0.0;
		delayUntilCollidable = 0.5f;
		lightMinMaxRadius = 0.0f;
		lightMaxMaxRadius = 0.0f;
		lightOutMinRadius = 0.0f;
		lightOutMaxRadius = 0.0f;
		lightRGB = 0x00000000;
		maxIntensity = 0.0f;
		minIntensity = 0.0f;
		duration = 0.0f;
	}

	ExplosionType(void)
	{
		init(void);
	}

	virtual int32_t init(FilePtr objFile, uint32_t fileSize);

	int32_t init(FitIniFilePtr objFile);

	~ExplosionType(void)
	{
		destroy(void);
	}

	virtual void destroy(void);

	virtual GameObjectPtr createInstance(void);

	virtual bool handleCollision(GameObjectPtr collidee, GameObjectPtr collider);

	virtual bool handleDestruction(GameObjectPtr collidee, GameObjectPtr collider);
};

#endif

//***************************************************************************




