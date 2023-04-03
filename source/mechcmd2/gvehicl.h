//******************************************************************************************
//
//	gvehicl.h - This file contains the GroundVehicle Class header
//
//	MechCommander 2
//
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#pragma once

#ifndef GVEHICL_H
#define GVEHICL_H

//#include "mclib.h"
//#include "mover.h"
//#include "dgvehicl.h"
//#include "gameobj.h"
//#include "objtype.h"
//#include "dobjnum.h"
//#include "cmponent.h"
//#include "warrior.h"
//#include "mechclass.h"
//
//#ifdef USE_ELEMENTALS
//#include "delemntl.h"
//#endif

extern float metersPerWorldUnit;

//***************************************************************************

#define NUM_GROUNDVEHICLE_HIT_ARCS 4
#define GROUNDVEHICLE_HIT_ARC_FRONT 0
#define GROUNDVEHICLE_HIT_ARC_REAR 1
#define GROUNDVEHICLE_HIT_ARC_LEFT 2
#define GROUNDVEHICLE_HIT_ARC_RIGHT 3

#define NO_APPEARANCE_FOR_VEHICLE 0xFFFD0001
#define NO_APPEARANCE_TYPE_FOR_VEHICLE 0xFFFD0009
#define APPEARANCE_NOT_GV_TYPE 0xFFFD000A

#define NUM_CRITICAL_SPACES_FRONT 1
#define NUM_CRITICAL_SPACES_LEFT 1
#define NUM_CRITICAL_SPACES_RIGHT 1
#define NUM_CRITICAL_SPACES_REAR 1
#define NUM_CRITICAL_SPACES_TURRET 1

#define MAX_TOADS 10
#define MAX_SEATS 4

enum class
{
	GROUNDVEHICLE_LOCATION_ANY = -1,
	GROUNDVEHICLE_LOCATION_FRONT,
	GROUNDVEHICLE_LOCATION_LEFT,
	GROUNDVEHICLE_LOCATION_RIGHT,
	GROUNDVEHICLE_LOCATION_REAR,
	GROUNDVEHICLE_LOCATION_TURRET,
	NUM_GROUNDVEHICLE_LOCATIONS
} VehicleLocationType;

enum class
{
	GROUNDVEHICLE_CHASSIS_WHEELED,
	GROUNDVEHICLE_CHASSIS_TRACKED,
	GROUNDVEHICLE_CHASSIS_HOVER,
	NUM_GROUNDVEHICLE_CHASSIS
} GroundVehicleChassisType;

enum class
{
	GROUNDVEHICLE_CRITICAL_NO_EFFECT,
	GROUNDVEHICLE_CRITICAL_AMMO_POWER_HIT,
	GROUNDVEHICLE_CRITICAL_FUEL_TANK_HIT,
	GROUNDVEHICLE_CRITICAL_CREW_KILLED,
	GROUNDVEHICLE_CRITICAL_ENGINE_HIT,
	GROUNDVEHICLE_CRITICAL_MAIN_WEAPON_JAMMED,
	GROUNDVEHICLE_CRITICAL_CREW_STUNNED,
	GROUNDVEHICLE_CRITICAL_MOVE_DISABLED,
	GROUNDVEHICLE_CRITICAL_MOVEMENT_HINDERED,
	GROUNDVEHICLE_CRITICAL_HOVERCRAFT_MOVEMENT_HINDERED,
	GROUNDVEHICLE_CRITICAL_TURRET_JAMMED,
	NUM_GROUNDVEHICLE_CRITICAL_HIT_TYPES
} GroundVehicleCriticalHitType;

//******************************************************************************************

class GroundVehicleType : public ObjectType
{

	//-------------
	// Data Members

public:
	uint8_t chassis;
	uint8_t maxInternalStructure[NUM_GROUNDVEHICLE_LOCATIONS];

	MoverDynamics dynamics; // what dynamics do we use.

	int32_t crashAvoidSelf;
	int32_t crashAvoidPath;
	int32_t crashBlockSelf;
	int32_t crashBlockPath;
	float crashYieldTime;

	float explDmg;
	float explRad;

	// Special Vehicle Info
	int32_t refitPoints; // number of refit points remaining
	int32_t recoverPoints;
	int32_t resourcePoints;
	bool mineSweeper;
	int32_t mineLayer;
	bool aerospaceSpotter;
	bool hoverCraft;

	int32_t moveType;

	bool pathLocks;
	bool isSensorContact;

	float LOSFactor;

	//----------------
	// Member Functions

public:
	void init(void);

	GroundVehicleType(void)
	{
		ObjectType::init(void);
		init(void);
	}

	virtual int32_t init(std::unique_ptr<File> objFile, uint32_t fileSize);

	int32_t loadHotSpots(FitIniFilePtr vehicleFile);

	virtual GameObjectPtr createInstance(void);

	virtual void destroy(void);

	~GroundVehicleType(void)
	{
		destroy(void);
	}

	virtual bool handleCollision(GameObjectPtr collidee, GameObjectPtr collider);
	virtual bool handleDestruction(GameObjectPtr collidee, GameObjectPtr collider);
};

//---------------------------------------------------------------------------
typedef struct _GroundVehicleData : public MoverData
{
	float accel;
	float velocityMag;

	float suspension;
	bool movementEnabled;

	bool turretEnabled;
	float turretRotation;
	bool turretBlownThisFrame;

	bool captureable;
	bool deadByCrushed;

	bool canRefit;
	bool canRecover;
	bool refitting;
	bool recovering;
	bool mineSweeper;
	float sweepTime;
	int32_t mineLayer;
	bool aerospaceSpotter;
	int32_t cellColToMine;
	int32_t cellRowToMine;

	bool notMineYet;

	int32_t battleRating;

	int32_t descID;

	float timeInCurrent;

	Stuff::Vector3D dVel;
	Stuff::Vector3D dRot;
	Stuff::Vector3D dAcc;
	Stuff::Vector3D dRVel;
	Stuff::Vector3D dRacc;
	float dTime;

	bool sensorOK;
	bool isVehiclePilot;

} GroundVehicleData;

class GroundVehicle : public Mover
{

	//------------
	// Data Members

public:
	float accel; // Current acceleration of vehicle in meters per sec per sec
	float velocityMag; // Current Speed of vehicle.  Not a vector quantity!! in
		// M/s

	float suspension;
	bool movementEnabled;

	bool turretEnabled;
	float turretRotation; // Current Rotation of Turret in Degrees
	bool turretBlownThisFrame;

	bool captureable;
	bool deadByCrushed; // Were we stepped on to die?

	bool canRefit;
	bool canRecover;
	bool refitting; // actually working, playing refit animation
	bool recovering;
	bool mineSweeper;
	float sweepTime; // time since we last swept a mine
	int32_t mineLayer;
	bool aerospaceSpotter;
	int32_t cellColToMine;
	int32_t cellRowToMine;

	bool notMineYet;

	int32_t battleRating; // Override of BattleRating

	int32_t descID; // Used by Logistics to Desc.

	float timeInCurrent; // Used by MineLayers to lay mines better

	bool isVehiclePilot; // Is this a pilot running away?

	// Destruction Coolness
	Stuff::Vector3D dVel;
	Stuff::Vector3D dRot;
	Stuff::Vector3D dAcc;
	Stuff::Vector3D dRVel;
	Stuff::Vector3D dRacc;
	float dTime;

	//----------------
	// Member Functions

public:
	virtual void init(bool create);

	GroundVehicle(void)
	{
		init(true);
	}

	virtual void init(bool create, ObjectTypePtr objType);

	virtual int32_t init(FitIniFile* vehicleFile);

	virtual void destroy(void);

	virtual void setControl(ControlType ctrlType);

	virtual bool crashAvoidanceSystem(void);

	virtual void updateAIControl(void);

	virtual void updatePlayerControl(void);

	virtual void updateNetworkControl(void);

	virtual void updateDynamics(void);

	virtual int32_t update(void);

	virtual void render(void);

	virtual void renderShadows(void);

	virtual float getStatusRating(void);

	virtual int32_t calcHitLocation(
		GameObjectPtr attacker, int32_t weaponIndex, int32_t attackSource, int32_t attackType);

	virtual int32_t handleWeaponHit(WeaponShotInfoPtr shotInfo, bool addMultiplayChunk = false);

	virtual float weaponLocked(int32_t weaponIndex, Stuff::Vector3D targetposition);

	virtual void pilotingCheck(void);

	virtual void mineCheck(void);

	bool pivotTo(void);

	void updateMoveStateGoal(void);

	bool updateMovePath(float& newRotate, wchar_t& newThrottleSetting, int32_t& newMoveState,
		int32_t& minThrottle, int32_t& maxThrottle);

	bool netUpdateMovePath(float& newRotate, wchar_t& newThrottleSetting, int32_t& newMoveState,
		int32_t& minThrottle, int32_t& maxThrottle);

	void setNextMovePath(wchar_t& newThrottleSetting);

	void setControlSettings(
		float& newRotate, wchar_t& newThrottleSetting, int32_t& minThrottle, int32_t& maxThrottle);

	void updateTurret(float newRotate);

	virtual void updateMovement(void);

	virtual void netUpdateMovement(void);

	virtual int32_t calcCV(bool calcMax = false);

	virtual float calcAttackChance(GameObjectPtr target, int32_t aimLocation, float targetTime,
		int32_t weaponIndex, float modifiers, int32_t* range,
		Stuff::Vector3D* targetpoint = nullptr);

	virtual float getTotalEffectiveness(void);

	virtual bool hitInventoryItem(int32_t itemIndex, bool setupOnly = false);

	virtual void destroyBodyLocation(int32_t location);

	virtual int32_t buildStatusChunk(void);

	virtual int32_t handleStatusChunk(int32_t updateAge, uint32_t chunk);

	virtual int32_t buildMoveChunk(void);

	virtual int32_t handleMoveChunk(uint32_t chunk);

	void calcThrottleLimits(int32_t& minThrottle, int32_t& maxThrottle);

	virtual bool injureBodyLocation(int32_t bodyLocation, float damage);

	virtual int32_t handleWeaponFire(int32_t weaponIndex, GameObjectPtr target,
		Stuff::Vector3D* targetpoint, bool hit, float entryAngle, int32_t numMissiles,
		int32_t hitLocation);

	virtual int32_t fireWeapon(GameObjectPtr target, float targetTime, int32_t weaponIndex,
		int32_t attackType, int32_t aimLocation, Stuff::Vector3D* targetpoint, float& damageDone);

	virtual float relFacingTo(Stuff::Vector3D goal, int32_t bodyLocation = -1);

	virtual float relViewFacingTo(Stuff::Vector3D goal)
	{
		return (relFacingTo(goal, GROUNDVEHICLE_LOCATION_TURRET));
	}

	virtual Stuff::Vector3D getPositionFromHS(int32_t weaponType);

	virtual bool canMove(void)
	{
		return (movementEnabled && (dynamics.max.groundVehicle.speed != 0));
	}

	virtual int32_t getSpeedState(void);

	virtual void disable(uint32_t cause);

	virtual float getDestructLevel(void)
	{
		float maxArmorLocation = 0.0f;
		// Find MAX armor location.
		for (size_t curLocation = 0; curLocation < NUM_GROUNDVEHICLE_LOCATIONS; curLocation++)
		{
			if (armor[curLocation].curArmor > maxArmorLocation)
				maxArmorLocation = armor[curLocation].curArmor;
		}
		return maxArmorLocation + 1; // Always assume you need one point more
			// than the best armor to be sure.
	}

	//----------------------------------------------
	// Additional ground vehicle-specific routines...
	bool canRotateTurret(void)
	{
		return (body[GROUNDVEHICLE_LOCATION_TURRET].damageState == IS_DAMAGE_NONE);
	}

	virtual int32_t getThrottle(void)
	{
		return (control.settings.groundVehicle.throttle);
	}

	bool isMoving(void)
	{
		return (control.settings.groundVehicle.throttle != 0);
	}

	bool isReversing(void)
	{
		return (control.settings.groundVehicle.throttle < 0);
	}

	virtual bool isLayingMines(void)
	{
		return (pilot && (pilot->getCurTacOrder()->moveparams.mode == SpecialMoveMode::minelaying));
	}

	virtual bool isMineSweeper(void)
	{
		return (mineSweeper);
	}

	virtual bool isMineLayer(void)
	{
		return (mineLayer != 0);
	}

	~GroundVehicle(void)
	{
		destroy(void);
	}

#ifdef USE_SALVAGE
	virtual bool isCaptureable(int32_t capturingTeamID)
	{
		// return ((captureable || salvage != nullptr) && (getTeamId() !=
		// capturingTeamID) && !isDestroyed());
		return ((getFlag(OBJECT_FLAG_CAPTURABLE) || (salvage != nullptr)) && (getTeamId() != capturingTeamID) && !isDestroyed());
	}
#else
	virtual bool isCaptureable(int32_t capturingTeamID)
	{
		// return (captureable && (getTeamId() != capturingTeamID) &&
		// !isDestroyed());
		return (
			getFlag(OBJECT_FLAG_CAPTURABLE) && (getTeamId() != capturingTeamID) && !isDestroyed());
	}
#endif

	virtual void handleStaticCollision(void);

	virtual std::wstring_view getIfaceName(void)
	{
		return (longName);
	}

	void createVehiclePilot(void);

	float getRefitPoints(void)
	{
		if (canRefit)
			return (armor[GROUNDVEHICLE_LOCATION_TURRET].curArmor);
		else
			return (0.0);
	}

	float getTotalRefitPoints(void)
	{
		if (canRefit)
			return (armor[GROUNDVEHICLE_LOCATION_TURRET].maxArmor);
		else
			return (0.0);
	}

	bool burnRefitPoints(float pointsToBurn);

	float getRecoverPoints(void)
	{
		if (canRecover)
			return (armor[GROUNDVEHICLE_LOCATION_TURRET].curArmor);
		else
			return (0.0);
	}

	float getTotalRecoverPoints(void)
	{
		if (canRecover)
			return (armor[GROUNDVEHICLE_LOCATION_TURRET].maxArmor);
		else
			return (0.0);
	}

	bool burnRecoverPoints(float pointsToBurn)
	{
		if (canRecover && pointsToBurn <= armor[GROUNDVEHICLE_LOCATION_TURRET].curArmor)
		{
			armor[GROUNDVEHICLE_LOCATION_TURRET].curArmor -= pointsToBurn;
			return (true);
		}
		else
			return (false);
	}

	virtual bool isRefit()
	{
		return canRefit;
	}
	virtual bool isRecover()
	{
		return canRecover;
	}

	static int32_t loadGameSystem(FitIniFilePtr sysFile);

	int32_t updateAnimations(void);

	virtual bool isMech(void)
	{
		return false;
	}

	virtual bool isVehicle(void)
	{
		if (dynamics.max.groundVehicle.speed != 0.0)
			return true;
		return false;
	}

	virtual bool isGuardTower(void)
	{
		if (dynamics.max.groundVehicle.speed != 0.0)
			return false;
		return true;
	}

	virtual int32_t setTeamId(int32_t _teamId, bool setup);

	virtual float getLOSFactor(void)
	{
		return (((GroundVehicleType*)getObjectType())->LOSFactor);
	}

	virtual void Save(PacketFilePtr file, int32_t packetNum);

	void CopyTo(GroundVehicleData* data);

	void Load(GroundVehicleData* data);
};

//******************************************************************************************

#endif
