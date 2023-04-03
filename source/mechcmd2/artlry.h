//***************************************************************************
//
//	artillery.h -- File contains the artillery strike Object Class
//
//	MechCommander 2
//
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#pragma once

#ifndef ARTLRY_H
#define ARTLRY_H

//---------------------------------------------------------------------------

//#include "dartlry.h"
//#include "gameobj.h"
//#include "objtype.h"
//#include "contact.h"
//#include "gosfx/gosfxheaders.h"

//***************************************************************************

#define NO_RAM_FOR_ARTY 0xDEFD0000
#define NO_RAM_FOR_ARTY_SHAPES 0xDEFD0001

#define NO_RAM_FOR_ARTILLERY 0xDEFD0002
#define NO_APPEARANCE_TYPE_FOR_ARTY 0xDEFD0003
#define NO_APPEARANCE_FOR_ARTY 0xDEFD0004
#define APPEARANCE_NOT_VFX_APPEAR 0xDCDC0009

#define CLAN_SEEN_FLAG 2
#define IS_SEEN_FLAG 1

#define MAX_ARTILLERY_EXPLOSIONS 32

//***************************************************************************

class ArtilleryChunk
{

public:
	wchar_t commanderId;
	wchar_t striketype;
	int32_t cellRC[2];
	wchar_t secondsToImpact;

	uint32_t data;

public:
	PVOID operator new(size_t mySize);

	void operator delete(PVOID us);

	void init(void)
	{
		commanderId = -1;
		striketype = -1;
		cellRC[0] = -1;
		cellRC[1] = -1;
		secondsToImpact = -1;
		data = 0;
	}

	void destroy(void) { }

	ArtilleryChunk(void)
	{
		init(void);
	}

	~ArtilleryChunk(void)
	{
		destroy(void);
	}

	void build(int32_t commanderId, int32_t striketype, Stuff::Vector3D location, int32_t seconds);

	void pack(void);

	void unpack(void);

	bool equalTo(ArtilleryChunkPtr chunk);
};

//***************************************************************************

class ArtilleryType : public ObjectType
{

public:
	uint8_t* frameList; // Pointer to JMiles shape file binary
	uint32_t frameCount; // Number of frames in shape file
	uint32_t startFrame; // Frame in List to start with.
	float frameRate; // Speed at which frames playback

	float nominalTimeToImpact;
	float nominalTimeToLaunch;

	float nominalDamage;
	float nominalMajorRange;
	float nominalMajorHits;
	float nominalMinorRange;
	float nominalMinorHits;

	float nominalSensorTime;
	float nominalSensorRange;

	float fontScale;
	float fontXOffset;
	float fontYOffset;
	uint32_t fontcolour;

	int32_t numExplosions;
	float* explosionOffsetX;
	float* explosionOffsetY;
	float* explosionDelay;

	int32_t numExplosionsPerExplosion;
	int32_t explosionRandomX;
	int32_t explosionRandomY;

	int32_t minArtilleryHeadRange;

public:
	void init(void);

	ArtilleryType(void)
	{
		init(void);
	}

	virtual int32_t init(std::unique_ptr<File> objFile, uint32_t fileSize);

	int32_t init(FitIniFilePtr objFile);

	~ArtilleryType(void)
	{
		destroy(void);
	}

	virtual void destroy(void);

	virtual GameObjectPtr createInstance(void);

	virtual bool handleCollision(GameObjectPtr collidee, GameObjectPtr collider);

	virtual bool handleDestruction(GameObjectPtr collidee, GameObjectPtr collider);
};

//***************************************************************************

typedef union _ArtilleryInfo
{
	struct
	{
		float timeToImpact; // Time until strike goes off.
		float timeToLaunch; // Time until strike is launched (and unmoveable).
		float sensorRange; // If I am a sensor round, how big a range.
		float timeToBlind; // How int32_t do I return sensor data.
		int16_t sensorSystemIndex;
		float contactUpdate;
		wchar_t timeString[5];
		uint32_t explosionOffFlags;
	} strike;
} ArtilleryInfo;

typedef struct _ArtilleryData : public GameObjectData
{
	wchar_t artilleryType;
	wchar_t teamId;
	wchar_t commanderId;
	ArtilleryInfo info;
	int32_t effectId;

	bool bombRunStarted;
	bool inView;
	Stuff::Vector3D iFacePosition;

} ArtilleryData;

class Artillery : public GameObject
{
	//-------------
	// Data Members

public:
	wchar_t artilleryType;
	wchar_t teamId;
	wchar_t commanderId;
	ArtilleryInfo info;
	int32_t effectId;

	gosFX::Effect* hitEffect;
	gosFX::Effect* rightContrail;
	gosFX::Effect* leftContrail;

	Appearance* bomber;
	bool bombRunStarted;

	bool inView;

	Stuff::Vector3D iFacePosition;

	// Member Functions
	//-----------------
public:
	virtual void init(bool create);

	virtual void init(bool create, int32_t _artilleryType);

	Artillery(void)
		: GameObject()
	{
		init(true);
	}

	~Artillery(void)
	{
		destroy(void);
	}

	bool recalcBounds(CameraPtr myEye);

	void setArtilleryData(float impactTime)
	{
		info.strike.timeToImpact = impactTime;
		info.strike.timeToLaunch = impactTime - 10;
	}

	virtual void setSensorRange(float range);

	void setSensorData(TeamPtr team, float sensorTime = -1.0, float range = -1.0);

	void setJustCreated(void);

	float getTimeToImpact(void)
	{
		return (info.strike.timeToImpact);
	}

	float getTimeToLaunch(void)
	{
		return (info.strike.timeToLaunch);
	}

	bool launched(void)
	{
		return (info.strike.timeToLaunch <= 0);
	}

	bool impacted(void)
	{
		return (info.strike.timeToImpact <= 0);
	}

	void drawSelectBox(uint8_t color);

	bool isStrike(void)
	{
		static bool lookup[NUM_ARTILLERY_TYPES] = {true, true, false};
		return (lookup[artilleryType]);
	}

	bool isSensor(void)
	{
		static bool lookup[NUM_ARTILLERY_TYPES] = {false, false, true};
		return (lookup[artilleryType]);
	}

	virtual void destroy(void);

	virtual int32_t update(void);

	virtual void render(void);

	virtual void init(bool create, ObjectTypePtr _type);

	virtual int32_t kill(void)
	{
		return (NO_ERROR);
	}

	virtual void setCommanderId(int32_t _commanderId);
	virtual int32_t getCommanderId(void)
	{
		return commanderId;
	}

	virtual int32_t setTeamId(int32_t _teamId, bool setup);

	virtual int32_t getTeamId(void)
	{
		return (teamId);
	}

	virtual bool isFriendly(TeamPtr team);

	virtual bool isEnemy(TeamPtr team);

	virtual bool isNeutral(TeamPtr team);

	virtual TeamPtr getTeam(void);

	virtual int32_t handleWeaponHit(WeaponShotInfoPtr shotInfo, bool addMultiplayChunk = false);

	virtual void handleStaticCollision(void);

	virtual void Save(PacketFilePtr file, int32_t packetNum);

	void Load(ArtilleryData* data);

	void CopyTo(ArtilleryData* data);
};

//---------------------------------------------------------------------------
extern void
CallArtillery(int32_t commanderId, int32_t striketype, Stuff::Vector3D strikeloc,
	int32_t secondsToImpact, bool randomOffset);

extern void
IfaceCallStrike(int32_t strikeID, Stuff::Vector3D* strikeloc,
	GameObjectPtr strikeTarget, bool playerStrike = true, bool clanStrike = false,
	float timeToImpact = -1.00);

//---------------------------------------------------------------------------
#endif
