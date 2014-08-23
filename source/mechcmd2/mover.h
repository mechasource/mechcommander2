//***************************************************************************
//
//	mover.h - This file contains the Mover Class header
//
//	MechCommander 2
//
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#pragma once

#ifndef MOVER_H
#define MOVER_H

//#include <mclib.h>
//#include "contact.h"
//#include "dmover.h"
//#include "gameobj.h"
//#include "objtype.h"
//#include "move.h"
//#include "dobjnum.h"
//#include "cmponent.h"
//#include "warrior.h"
//#include "dgroup.h"
//#include "trigger.h"
//#include "dteam.h"
//#include "comndr.h"

extern float metersPerWorldUnit;

//***************************************************************************

#define	DEBUG_CHUNKS

#define YIELD_WAITING		0
#define YIELDED				1
#define COLLIDER_YIELDED	2
#define COLLIDER_BEHIND		3
#define NO_ACTION			4
#define ON_THE_MOVE			5

#define	MAX_LOCK_RANGE		10

#define	MAX_WEAPONFIRE_CHUNKS			128
#define	MAX_CRITICALHIT_CHUNKS			128
#define	MAX_RADIO_CHUNKS				7

#define	CHUNK_SEND						0
#define	CHUNK_RECEIVE					1

#define	NUM_WEAPONFIRE_MODIFIERS		30

#define	MAX_ANTI_MISSILE_SYSTEMS		16			// Way more than we should need!

#define	ARMOR_TYPE_STANDARD				0
#define	ARMOR_TYPE_FERROFIBROUS			1

#define	CHASSIS_TYPE_STANDARD			0
#define	CHASSIS_TYPE_ENDOSTEEL			1

#define	ENGINE_TYPE_COMBUSTION			0
#define	ENGINE_TYPE_FUSION_STANDARD		1
#define	ENGINE_TYPE_FUSION_XL			2

#define	HEATSINK_TYPE_SINGLE			1
#define	HEATSINK_TYPE_DOUBLE			2

#define	IS_DAMAGE_NONE					0
#define	IS_DAMAGE_PARTIAL				1
#define	IS_DAMAGE_DESTROYED				2

#define	UNLIMITED_SHOTS					9999

#define NO_APPEARANCE_FOR_MECH			0xFFFA0001
#define	NO_RAM_FOR_DYNAMICS_TYPE		0xFFFA0002
#define	INVALID_DYNAMICS_TYPE			0xFFFA0003
#define	NO_RAM_FOR_CONTROL				0xFFFA0004
#define	INVALID_CONTROL_TYPE			0xFFFA0005
#define	NO_RAM_FOR_CONTROL_DATA			0xFFFA0006
#define	INVALID_CONTROL_DATA_TYPE		0xFFFA0007
#define	NO_RAM_FOR_DYNAMICS				0xFFFA0008
#define	NO_APPEARANCE_TYPE_FOR_MECH		0xFFFA0009
#define	APPEARANCE_NOT_SPRITE_TREE		0xFFFA000A
#define NO_RAM_FOR_FOOT_HS				0xFFFA000B
#define NO_RAM_FOR_WEAPON_HS			0xFFFA000C
#define FOOT_HS_WRONG_SIZE				0xFFFA000D
#define BAD_HS_PACKET					0xFFFA000E
#define NO_RAM_FOR_XLAT_HS				0xFFFA000F

//------------------------------------------------------------------------------
// Enums
typedef enum {
	WEAPONSORT_ATTACKCHANCE,
	NUM_WEAPONSORT_TYPES
} WeaponSortType;

typedef enum {
	SPEED_STATE_STATIONARY,
	SPEED_STATE_MOVING,
	SPEED_STATE_MOVING_FAST,
	NUM_MOVE_STATES
} SpeedState;

enum DeathCause
{
	DEBUGGER_DEATH = 0,
	ENGINE_DEATH = 1,
	PILOT_DEATH = 2,
	EJECTION_DEATH = 3,
	UNDETERMINED_DEATH = 4,
	POWER_USED_UP = 5
};

//---------------------------------------------------------------------------

class MoveChunk;
typedef MoveChunk* MoveChunkPtr;

class MoveChunk {

	public:

		int32_t				stepPos[4][2];		// 0 = curPos, 1 thru 3 = next steps
		int32_t				stepRelPos[3];		// delta for steps 1 thru 3
		int32_t				numSteps;
		bool				run;
		bool				moving;
		uint32_t		data;
		static int32_t			err;

	public:

		PVOID operator new (size_t mySize);

		void operator delete (PVOID us);
		
		void init (void) {
			stepPos[0][0] = 0xFFFFFFFF;
			stepPos[0][1] = 0xFFFFFFFF;
			numSteps = 0;
			run = false;
			moving = false;
			data = 0;
		}

		void destroy (void) {
		}

		MoveChunk (void) {
			init();
		}

		~MoveChunk (void) {
			destroy();
		}

		void build (MoverPtr mover, MovePath* path1, MovePath* path2 = NULL);

		void build (MoverPtr mover, Stuff::Vector3D jumpGoal);

		void pack (MoverPtr mover);

		void unpack (MoverPtr mover);

		bool equalTo (MoverPtr mover, MoveChunkPtr chunk);
};

//---------------------------------------------------------------------------
#define MAX_YAW		64

typedef enum {
	DYNAMICS_BASE,
	DYNAMICS_MECH,
	DYNAMICS_GROUNDVEHICLE,
	DYNAMICS_ELEMENTAL
} DynamicsType;

typedef union {
	struct {
		int32_t	yawRate;			//Degrees per sec
	} elemental;
	struct {
		int32_t	yawRate;			//Degrees per sec
		int32_t	turretYawRate;		//Degrees per sec
		int32_t	pivotRate;			//Degrees per sec
		float	accel;				//Meters per sec per sec
		float	speed;				//Meters per sec
		int32_t	turretYaw;			//Degrees
	} groundVehicle;
	struct {
		int32_t	torsoYawRate;		//Degrees per sec
		int32_t	torsoYaw;			//Degrees
	} mech;
} DynamicsLimits;

class MoverDynamics {

	public:

		DynamicsType			type;
		DynamicsLimits			max;

	public:

		void operator = (MoverDynamics copy) {
			type = copy.type;
			max = copy.max;
			//cur = copy.cur;
		}
		
		virtual void init (void);

		MoverDynamics (void) {
			init();
		}
			
		virtual void destroy (void) {
		}

		~MoverDynamics (void) {
			destroy();
		}

		void init (DynamicsType newType);

		virtual void init (CSVFilePtr dynamicsFile);

		virtual void init (FitIniFilePtr dynamicsFile);
													
		void setType (DynamicsType newType) {
			type = newType;
		}

		DynamicsType getType (void) {
			return(type);
		}

		int32_t brake (void);
};

//---------------------------------------------------------------------------

typedef enum {
	CONTROL_BASE,
	CONTROL_PLAYER,
	CONTROL_AI,
	CONTROL_NET
} ControlType;

typedef enum {
	CONTROL_DATA_BASE,
	CONTROL_DATA_MECH,
	CONTROL_DATA_GROUNDVEHICLE,
	CONTROL_DATA_ELEMENTAL
} ControlDataType;

typedef union {
	struct {
		float	rotate;
	} elemental;
	struct {
		char	throttle;			
		float	rotate;
		float	rotateTurret;
		char	gestureGoal;
		bool	pivot;
		bool	isWalking;
	} groundVehicle;
	struct {
		char	throttle;			
		float	rotate;				//aka mechYaw
		float	facingRotate;		//Direction mech is FACING, NOT MOVING!!!!!
		float	rotateTorso;
		float	rotateLeftArm;		//aka leftArmYaw
		float	rotateRightArm;		//aka rightArmYaw
		char	gestureGoal;
		bool	blowLeftArm;
		bool	blowRightArm;
		bool	pivot;
	} mech;
} ControlSettings;

class MoverControl {

	public:

		ControlType			type;
		ControlDataType		dataType;
		ControlSettings		settings;

	public:

		void operator = (MoverControl copy) {
			type = copy.type;
			dataType = copy.dataType;
			settings = copy.settings;
		}

		virtual void init (void) {
			type = CONTROL_BASE;
			dataType = CONTROL_DATA_BASE;
		}

		MoverControl (void) {
			init();
		}
			
		virtual void destroy (void) {
		}

		~MoverControl (void) {
			destroy();
		}

		void init (ControlType newType, ControlDataType newDataType) {
			setType(newType);
			setDataType(newDataType);
		}

		virtual int32_t init (FitIniFilePtr controlFile);

		void setType (ControlType newType) {
			type = newType;
		}

		ControlType getType (void) {
			return(type);
		}

		void setDataType (ControlDataType newDataType) {
			dataType = newDataType;
			reset();
		}

		ControlDataType getDataType (void) {
			return(dataType);
		}

		void reset (void);

		void brake (void);

		void update (MoverPtr mover);
};

//---------------------------------------------------------------------------

#define	MAX_RANGE_RATINGS		51

typedef struct _InventoryItem* InventoryItemPtr;

typedef struct _RangeRating {
	float				accuracy;
	float				adjAccuracy;
} RangeRating;

typedef struct _RangeRating* RangeRatingPtr;

typedef struct _InventoryItem {
	//------------------
	// general item info
	uint8_t		masterID;		// master component ID
	uint8_t		health;			// number of points left before destroyed
	bool				disabled;		// TRUE if effectively destroyed

	//----------------
	// weapon specific
	uint8_t		facing;			// weapon facing in torso: 0 = forward, 1 = rear -- NO WEAPONS fire rear.  This is the weapon Node ID now!!!!
	int16_t				startAmount;	// ammo's mission-start level
	int16_t				amount;			// generally for ammo, and weapon's total ammo
	int16_t				ammoIndex;		// used by ammo to reference ammo pools
	float				readyTime;		// next time weapon will be ready
	uint8_t		bodyLocation;	// where is the weapon located
	int16_t				effectiveness;	// weapon max effectiveness
} InventoryItem;

//------------------------------------------------------------------------------------------

#define	MAX_AMMO_TYPES			10

typedef struct _AmmoTally {
	int32_t				masterId;		// ammo type's Master Component Id
	int32_t				curAmount;		// current amount of this ammo type in inventory
	int32_t				maxAmount;		// starting amount of this ammo type in inventory
} AmmoTally;

typedef AmmoTally* AmmoTallyPtr;

//------------------------------------------------------------------------------------------

typedef struct _CriticalSpace* CriticalSpacePtr;

typedef struct _CriticalSpace {

	uint8_t		inventoryID;	// indexes into mech's inventory
	bool				hit;			// TRUE, if this space has been hit

	void operator = (struct _CriticalSpace copy) {
		inventoryID = copy.inventoryID;
		hit = copy.hit;
	}

} CriticalSpace;

//------------------------------------------------------------------------------------------

#define	MAX_CRITSPACES_PER_BODYLOCATION		12

class BodyLocation {

	public:

		bool				CASE;
		int32_t				totalSpaces;
		CriticalSpace		criticalSpaces[MAX_CRITSPACES_PER_BODYLOCATION];
		float				curInternalStructure;
		uint8_t		hotSpotNumber;
		uint8_t		maxInternalStructure;
		uint8_t		damageState;

	public:

		void operator = (BodyLocation copy) {
			CASE = copy.CASE;
			totalSpaces = copy.totalSpaces;
			for (int32_t i = 0; i < MAX_CRITSPACES_PER_BODYLOCATION; i++)
				criticalSpaces[i] = copy.criticalSpaces[i];
			curInternalStructure = copy.curInternalStructure;
			hotSpotNumber = copy.hotSpotNumber;
			maxInternalStructure = copy.maxInternalStructure;
			damageState = copy.damageState;
		}
};

typedef BodyLocation* BodyLocationPtr;

//---------------------------------------------------------------------------

class ArmorLocation {

	public:

		float			curArmor;
		uint8_t	maxArmor;
};

typedef ArmorLocation* ArmorLocationPtr;

//---------------------------------------------------------------------------

#define	STATUSCHUNK_BODYSTATE_BITS		3
#define	STATUSCHUNK_TARGETTYPE_BITS		3
#define	STATUSCHUNK_MOVERINDEX_BITS		7
#define	STATUSCHUNK_TERRAINPARTID_BITS	20
#define	STATUSCHUNK_TERRAINBLOCK_BITS	8
#define	STATUSCHUNK_TERRAINVERTEX_BITS	9
#define	STATUSCHUNK_TERRAINITEM_BITS	3
#define	STATUSCHUNK_TRAIN_BITS			8
#define	STATUSCHUNK_TRAINCAR_BITS		8
#define	STATUSCHUNK_CELLPOS_BITS		10
#define	STATUSCHUNK_EJECTORDER_BITS		1
#define	STATUSCHUNK_JUMPORDER_BITS		1

#define	STATUSCHUNK_BODYSTATE_MASK		0x00000007
#define	STATUSCHUNK_TARGETTYPE_MASK		0x00000007
#define	STATUSCHUNK_MOVERINDEX_MASK		0x0000007F
#define	STATUSCHUNK_TERRAINPARTID_MASK	0x000FFFFF
#define	STATUSCHUNK_TERRAINBLOCK_MASK	0x000000FF
#define	STATUSCHUNK_TERRAINVERTEX_MASK	0x000001FF
#define	STATUSCHUNK_TERRAINITEM_MASK	0x00000007
#define	STATUSCHUNK_TRAIN_MASK			0x000000FF
#define	STATUSCHUNK_TRAINCAR_MASK		0x000000FF
#define	STATUSCHUNK_CELLPOS_MASK		0x000003FF
#define	STATUSCHUNK_EJECTORDER_MASK		0x00000001
#define	STATUSCHUNK_JUMPORDER_MASK		0x00000001

typedef enum {
	STATUSCHUNK_TARGET_NONE,
	STATUSCHUNK_TARGET_MOVER,
	STATUSCHUNK_TARGET_TERRAIN,
	STATUSCHUNK_TARGET_SPECIAL,
	STATUSCHUNK_TARGET_LOCATION
} StatusChunkTarget;

class StatusChunk;
typedef StatusChunk* StatusChunkPtr;

class StatusChunk {

	public:

		uint32_t		bodyState;
		char				targetType;
		int32_t				targetId;
		int32_t				targetBlockOrTrainNumber;
		int32_t				targetVertexOrCarNumber;
		char				targetItemNumber;
		int16_t				targetCellRC[2];
		bool				ejectOrderGiven;
		bool				jumpOrder;

		uint32_t		data;

	public:

		PVOID operator new (size_t mySize);

		void operator delete (PVOID us);
		
		void init (void) {
			bodyState = 0;
			targetType = 0;
			targetId = 0;
			targetBlockOrTrainNumber = 0;
			targetVertexOrCarNumber = 0;
			targetItemNumber = 0;
			targetCellRC[0] = -1;
			targetCellRC[1] = -1;
			ejectOrderGiven = false;
			jumpOrder = false;

			data = 0;
		}

		void destroy (void) {
		}

		StatusChunk (void) {
			init ();
		}

		~StatusChunk (void) {
			destroy();
		}

		virtual void build (MoverPtr mover);

		virtual void pack (MoverPtr mover);

		virtual void unpack (MoverPtr mover);

		bool equalTo (StatusChunkPtr chunk);
};


//---------------------------------------------------------------------------

#define	MAXLEN_MOVER_NAME			65
#define	MAX_MOVER_BODY_LOCATIONS	8
#define	MAX_MOVER_ARMOR_LOCATIONS	11
#define	MAX_MOVER_INVENTORY_ITEMS	72
#define	MAXLEN_NET_PLAYER_NAME		256
#define	MAXLEN_MECH_LONGNAME	35

#define	MAX_ATTACK_CELLRANGE		30
#define	MAX_ATTACK_INCREMENTS		32
#define	RANGED_CELLS_DIM			(MAX_ATTACK_CELLRANGE * 2 + 1) * (MAX_ATTACK_CELLRANGE * 2 + 1)

typedef struct _MoverData : public GameObjectData
{
	bool				killed;
	bool				lost;
	Stuff::Vector3D		positionNormal;						
	Stuff::Vector3D		velocity;							
	char				name[MAXLEN_MOVER_NAME];			
	uint8_t		chassis;							
	bool				startDisabled;
	float				creationTime;

	int32_t				moveType;
	int32_t				moveLevel;
	bool				followRoads;

	int32_t				lastMapCell[2];

	float				damageRateTally;					
	float				damageRateCheckTime;				
	float				pilotCheckDamageTally;				

	BodyLocation		body[MAX_MOVER_BODY_LOCATIONS];		
	char				numBodyLocations;					
	int32_t				fieldedCV;

	int32_t				attackRange;						

	ArmorLocation		armor[MAX_MOVER_ARMOR_LOCATIONS];	
	char				numArmorLocations;
	char				longName[MAXLEN_MECH_LONGNAME];		

	InventoryItem		inventory[MAX_MOVER_INVENTORY_ITEMS];
	uint8_t		numOther;
	uint8_t		numWeapons;
	uint8_t		numAmmos;
	AmmoTally			ammoTypeTotal[MAX_AMMO_TYPES];	
	char				numAmmoTypes;					
	int32_t				pilotHandle;

	uint8_t		cockpit;										
	uint8_t		engine;											
	uint8_t		lifeSupport;									
	uint8_t		sensor;											
	uint8_t		ecm;											
	uint8_t		probe;											
	uint8_t		jumpJets;										
	uint8_t		nullSignature;									
	float				maxWeaponEffectiveness;							
	float				weaponEffectiveness;							

	float				minRange;										
	float				maxRange;										
	float				optimalRange;									
	int32_t				numFunctionalWeapons;							

	char				numAntiMissileSystems;							
	uint8_t		antiMissileSystem[MAX_ANTI_MISSILE_SYSTEMS];	

	float				engineBlowTime;
	float				maxMoveSpeed;
	bool				shutDownThisFrame;
	bool				startUpThisFrame;
	bool				disableThisFrame;

	char				teamId;
	char				groupId;
	int32_t				squadId;
	int32_t				selectionIndex;					
	int32_t				teamRosterIndex;				
	char				commanderId;
	int32_t				unitGroup;						
														
	int32_t				iconPictureIndex;				
	bool				suppressionFire;				

	int32_t				pilotCheckModifier;
	int32_t				prevPilotCheckModifier;
	int32_t				prevPilotCheckDelta;
	float				prevPilotCheckUpdate;
	bool				failedPilotingCheck;			
	float				lastWeaponEffectivenessCalc;	
	float				lastOptimalRangeCalc;			
	GameObjectWatchID	challengerWID;

	char				lastGesture;

	MoverControl		control;						
	MoverDynamics		dynamics;						

	int32_t				numWeaponHitsHandled;
	float				timeLeft;						
	bool				exploding;
	bool				withdrawing;

	float				yieldTimeLeft;					
	Stuff::Vector3D		lastValidPosition;				
	char				pivotDirection;					
	float				lastHustleTime;					

	bool				salvageVehicle;

	float				markDistanceMoved;				

	GameObjectWatchID	refitBuddyWID;
	GameObjectWatchID	recoverBuddyWID;

	int32_t				crashAvoidSelf;
	int32_t				crashAvoidPath;
	int32_t				crashBlockSelf;
	int32_t				crashBlockPath;
	float				crashYieldTime;
	int32_t				pathLockLength;
	int32_t				pathLockList[MAX_LOCK_RANGE][2];
	Stuff::Vector3D		moveCenter;
	float				moveRadius;

	int32_t				overlayWeightClass;

	float				timeToClearSelection;

	float				timeSinceMoving;

	float 				timeSinceFiredLast;				

	GameObjectWatchID	lastMovingTargetWID;

	bool 				mechSalvage;					

	Stuff::Vector3D		teleportPosition;				
	int32_t				debugPage;

	bool				pathLocks;						
	bool				isOnGui;						

	int32_t				conStat;						
	float				fadeTime;						
	uint8_t				alphaValue;						
	int32_t				causeOfDeath;

	int32_t				lowestWeaponNodeID;

	uint32_t				psRed;
	uint32_t				psBlue;
	uint32_t				psGreen;

} MoverData;


typedef enum {
	MOVETYPE_GROUND,
	MOVETYPE_AIR,
	NUM_MOVETYPES
} MoveType;

class LogisticsPilot;

class Mover : public GameObject {

	//------------
	//Data Members
	
	public:

		Stuff::Vector3D		positionNormal;						// normal to terrain at current position
		Stuff::Vector3D		velocity;							//How fast am I going?
		char				name[MAXLEN_MOVER_NAME];		// Name of this particular mover
		uint8_t		chassis;							// type of mover's chassis
		bool				startDisabled;
		float				creationTime;

		bool				killed;							// used to record when the kill score has been awarded
		bool				lost;							// used to record when the loss score has been awarded
		int32_t				moveType;
		int32_t				moveLevel;
		bool				followRoads;

		int32_t				lastMapCell[2];
		
		float				damageRateTally;					// damage points taken since last check
		float				damageRateCheckTime;				// time (in game time) of next damage check
		float				pilotCheckDamageTally;				// damage points taken since last pilot check

		BodyLocation		body[MAX_MOVER_BODY_LOCATIONS];		// body parts of this mech
		char				numBodyLocations;					// should be set based upon mover type
		int32_t				fieldedCV;

		int32_t				attackRange;						// attack range

		bool				salvaged;

		// Armor
		ArmorLocation		armor[MAX_MOVER_ARMOR_LOCATIONS];	// armor locations of this mover
		char				numArmorLocations;
		char				longName[MAXLEN_MECH_LONGNAME];		//Used by logistics (and the interface) to get int32_t name.

		// Inventory
		InventoryItem		inventory[MAX_MOVER_INVENTORY_ITEMS];
		uint8_t		numOther;
		uint8_t		numWeapons;
		uint8_t		numAmmos;
		AmmoTally			ammoTypeTotal[MAX_AMMO_TYPES];	// tracks total ammo per ammo type
		char				numAmmoTypes;					// number of different ammo types
		MechWarriorPtr		pilot;
		int32_t				pilotHandle;
		SensorSystemPtr		sensorSystem;
		ContactInfoPtr		contactInfo;

		// Critical Component Indices
		uint8_t		cockpit;										// cockpit inventory index
		uint8_t		engine;											// engine inventory index
		uint8_t		lifeSupport;									// life support inventory index
		uint8_t		sensor;											// sensor inventory index
		uint8_t		ecm;											// ecm inventory index
		uint8_t		probe;											// probe inventory index
		uint8_t		jumpJets;										// jump jets inventory index
		uint8_t		nullSignature;									// null signature inventory index
		float				maxWeaponEffectiveness;							// max total damage possible
		float				weaponEffectiveness;							// basically, total damage possible
		
		float				minRange;										// current min attack range
		float				maxRange;										// current max attack range
		float				optimalRange;									// current optimum attack range
		int32_t				numFunctionalWeapons;							// takes into account damage, etc.

		char				numAntiMissileSystems;							// number of anti-missile systems
		uint8_t		antiMissileSystem[MAX_ANTI_MISSILE_SYSTEMS];	// anti-missile system list

		// Engine
		float				engineBlowTime;
		float				maxMoveSpeed;
		bool				shutDownThisFrame;
		bool				startUpThisFrame;
		bool				disableThisFrame;

		// Team
		//MoverGroupPtr		group;							// what group am I a member of?
		char				teamId;
		char				groupId;
		int32_t				squadId;
		int32_t				selectionIndex;					// > 0 when in selected group
		int32_t				teamRosterIndex;				// where am I in my team's roster?
		char				commanderId;
		int32_t				unitGroup;						// the thing the user sets by hitting ctrl and a number
															// this is a field since they can belong to more than one
		int32_t				iconPictureIndex;				// the little picture that shows arms and stuff falling off
		bool				suppressionFire;				// is this guy permanently shooting at ground
		char				prevTeamId;
		char				prevCommanderId;
		

		// Update Info
		int32_t				pilotCheckModifier;
		int32_t				prevPilotCheckModifier;
		int32_t				prevPilotCheckDelta;
		float				prevPilotCheckUpdate;
		bool				failedPilotingCheck;			// Passed or failed this frame...
//		BaseObjectPtr		collisionFreeFrom;
//		float				collisionFreeTime;
		float				lastWeaponEffectivenessCalc;	// time of last calc
		float				lastOptimalRangeCalc;			// time of last calc
		GameObjectWatchID	challengerWID;

		char				lastGesture;

//		AppearancePtr		appearance;						// pointer to the Actor which is the appearance.
		MoverControl		control;						// control settings for this mover
		MoverDynamics		dynamics;						// dynamics settings for this mover
		
		// Network
		//uint32_t				netPlayerId;
		char				netPlayerName[MAXLEN_NET_PLAYER_NAME];	// netPlayerName is the player who owns this mover
		int32_t				localMoverId;					// if >= 0, is locally controlled
		int32_t				netRosterIndex;					// used for mover id in net packets
		StatusChunk			statusChunk;					// last status chunk built/received
		bool				newMoveChunk;					// set if last movechunk not yet processed
		MoveChunk			moveChunk;						// last move chunk built/received
		int32_t				numWeaponFireChunks[2];
		uint32_t		weaponFireChunks[2][MAX_WEAPONFIRE_CHUNKS];
		int32_t				numCriticalHitChunks[2];
		uint8_t		criticalHitChunks[2][MAX_CRITICALHIT_CHUNKS];
		int32_t				numRadioChunks[2];
		uint8_t		radioChunks[2][MAX_RADIO_CHUNKS];
		bool				ejectOrderGiven;
															// Still awaiting final destruct orders from update.
		int32_t				numWeaponHitsHandled;
		float				timeLeft;						// How int32_t before we return FALSE to update.
		bool				exploding;
		bool				withdrawing;
	
		float				yieldTimeLeft;					// How much time do I have left to wait
		Stuff::Vector3D		lastValidPosition;				// Last valid move path point I've been to
		char				pivotDirection;					// Used in pivotTo(): -1 = not pivoting
		float				lastHustleTime;					// last time we had to hustle (on bridge, etc.)

		static int32_t			numMovers;
		static SortListPtr	sortList;

		bool				salvageVehicle;
		
		float				markDistanceMoved;				//Used to track distance object has moved since last mark of terrain visible.

		GameObjectWatchID	refitBuddyWID;
		GameObjectWatchID	recoverBuddyWID;

		int32_t				crashAvoidSelf;
		int32_t				crashAvoidPath;
		int32_t				crashBlockSelf;
		int32_t				crashBlockPath;
		float				crashYieldTime;
		int32_t				pathLockLength;
		int32_t				pathLockList[MAX_LOCK_RANGE][2];
		Stuff::Vector3D		moveCenter;
		float				moveRadius;

		int32_t				overlayWeightClass;

		float				timeToClearSelection;

		float				timeSinceMoving;

		float 				timeSinceFiredLast;				//used to track when they stop revealing

		//GameObjectPtr		lastMovingTarget;
		GameObjectWatchID	lastMovingTargetWID;

		bool 				mechSalvage;					//As this guy dies, ONLY check once if he's salvagable!

		Stuff::Vector3D		teleportPosition;				//debug feature :)
		int32_t				debugPage;

		static float		newThreatMultiplier;
		static float		marginOfError[2];
		static float		refitRange;
		static float		refitTime;
		static float		refitCostArray[NUM_COSTS][NUM_FIXERS];
		static float		refitAmount;
		static float		recoverRange;
		static float		recoverTime;
		static float		recoverCost;
		static float		recoverAmount;
		static bool			inRecoverUpdate;
		static char			optimalCells[MAX_ATTACK_CELLRANGE][MAX_ATTACK_INCREMENTS][2];
		static int32_t			numOptimalIncrements;
		static int16_t		rangedCellsIndices[MAX_ATTACK_CELLRANGE][2];
		static char			rangedCells[RANGED_CELLS_DIM][2];
		static int32_t			IndirectFireWeapons[20];
		static int32_t			AreaEffectWeapons[20];
		static uint32_t holdFireIconHandle;
		
		static TriggerAreaManager* triggerAreaMgr;


		bool				pathLocks;						//For movers which can be stepped on.  They do NOT lock!
		bool				isOnGui;						//For movers which start out on player team but not on gui.  Like raven in 0103

		int32_t				conStat;						//Contact status stored for this frame, for this machine
		float				fadeTime;						//Time between fade from LOS to non-LOS
		uint8_t				alphaValue;						//Current Fade value;
		int32_t				causeOfDeath;

		int32_t				lowestWeaponNodeID;
		float				lowestWeaponNodeZ;

	//----------------
	//Member Functions

	public:

		virtual void set (Mover copy);

		virtual void init (bool create);

		Mover (void) {
			init(true);
		}
			
		virtual void destroy (void);

		virtual void init (bool create, ObjectTypePtr objType) {
			GameObject::init(create, objType);
		}

		virtual int32_t init (FitIniFile* objProfile) {
			return(NO_ERROR);
		}

		virtual int32_t init (uint32_t variantNum) {
			return(NO_ERROR);
		}

		virtual void release (void);

		//virtual int32_t update (void);

		//virtual void render (void);

		virtual void updateDebugWindow (GameDebugWindow* debugWindow);

		virtual PSTR getName (void) {
			return(name);
		}

		virtual void setName (PSTR s) {
			strncpy(name, s, MAXLEN_MOVER_NAME);
		}

		virtual void setControl (ControlType ctrlType) {
		}

		virtual void updateAIControl (void) {
		}

		virtual void updateNetworkControl (void) {
		}

		virtual void updatePlayerControl (void) {
		}

		virtual void updateDynamics (void) {
		}

		virtual void setPartId (int32_t newPartId);

		virtual void setPosition (Stuff::Vector3D& newPosition);
		
		virtual void setTeleportPosition (Stuff::Vector3D& newPos);

		ContactInfoPtr getContactInfo (void) {
			return(contactInfo);
		}

		virtual void tradeRefresh (void);

		virtual void setMoveType (int32_t type);

		virtual int32_t getMoveType (void) {
			return(moveType);
		}

		virtual void setMoveLevel (int32_t level) {
			moveLevel = level;
		}

		virtual int32_t getMoveLevel (void) {
			return(moveLevel);
		}

		virtual bool getFollowRoads (void) {
			return(followRoads);
		}

		virtual void setFollowRoads (bool setting) {
			followRoads = setting;
		}

		virtual Stuff::Vector3D getVelocity (void) {
			return(velocity);
		}

		virtual void setVelocity (Stuff::Vector3D& newVelocity) {
			velocity = newVelocity;
		}

		virtual float getSpeed (void) {
			return(velocity.GetLength());
		}

		virtual bool hasWeaponNode (void);

		Stuff::Vector3D getLOSPosition (void);

		virtual void rotate (float angle);

		virtual void rotate (float angle, float facingAngle);
		
		virtual void setAwake (bool state);

		virtual bool isMarine(void) 
		{
			return(pathLocks == false);
		}

		void drawWaypointPath();

		void updateDrawWaypointPath();
		
//		virtual frame_of_ref getFrame (void)
//		{
//			return(frame);
//		}

		virtual int32_t getSpeedState (void) {
			return(SPEED_STATE_STATIONARY);
		}

		virtual float getTerrainAngle (void);

		virtual float getVelocityTilt (void);

//		virtual void setFrame (frame_of_ref &newFrame)
//		{
//			frame = newFrame;
//		}
		
		virtual Stuff::Vector3D relativePosition (float angle, float radius, uint32_t flags);

		int32_t calcLineOfSightView (int32_t range);
		
		virtual void setSensorRange (float range);
		
		virtual Stuff::Vector3D getRotationVector (void) 
		{
			Stuff::Vector3D rotationVec;
			rotationVec.x = 0.0f;
			rotationVec.y = -1.0f;
			rotationVec.z = 0.0f;
			Rotate(rotationVec, -rotation);
			return(rotationVec);
		}

		virtual bool hasActiveProbe (void);

		virtual float getEcmRange (void);

		virtual bool hasNullSignature (void);

		virtual void setSelected (bool set, int32_t newSelectionIndex = 0) {
			GameObject::setSelected(set);
			selectionIndex = newSelectionIndex;
		}

		virtual int32_t handleTacticalOrder (TacticalOrder tacOrder, int32_t priority = 1, bool queuePlayerOrder = false);

		virtual AppearancePtr getAppearance (void) {
			return(appearance);
		}

//		virtual float getAppearRadius (void)
//		{
//			return appearance->getRadius();
//		}

		virtual void reduceAntiMissileAmmo (int32_t numAntiMissiles);

		virtual void pilotingCheck (uint32_t situation = 0, float modifier = 0.0);

		virtual void forcePilotingCheck (void) {
			if (pilotCheckModifier < 0)
				pilotCheckModifier = 0;
		}

		virtual bool canFireWeapons (void) {
			return(true);
		}

		virtual void updateDamageTakenRate (void);

		virtual int32_t checkShortRangeCollision (void) {
			return(NO_ACTION);
		}

		virtual void setOverlayWeightClass (int32_t overlayClass) {
			overlayWeightClass = overlayClass;
		}

		virtual int32_t getOverlayWeightClass (void) {
			return(overlayWeightClass);
		}

		virtual void getStopInfo (float &stopTime, float &stopDistance) {}

		virtual bool getAdjacentCellPathLocked (int32_t level, int32_t cellRow, int32_t cellCol, int32_t dir);

		virtual void updatePathLock (bool set);

		virtual bool getPathRangeLock (int32_t range, bool* reachedEnd = NULL);

		virtual int32_t setPathRangeLock (bool set, int32_t range = 0);

		virtual bool getPathRangeBlocked (int32_t range, bool* reachedEnd = NULL);

		virtual bool crashAvoidanceSystem (void) {
			return(false);
		}

		void setLastHustleTime (float t) {
			lastHustleTime = t;
		}

		float getLastHustleTime (void) {
			return(lastHustleTime);
		}

		virtual void updateHustleTime (void);

		virtual void mineCheck (void) {
			//----------------------------
			// Does nothing, be default...
		}

		virtual void updateMovement (void) {}

		virtual int32_t bounceToAdjCell (void);

#ifdef USE_MOVERCONTROLS
		uint32_t getControlClass (void) {
			return(control->getControlClass());
		}
#endif

		virtual void setSquadId (char newSquadId) {
			squadId = newSquadId;
		}

		virtual char getSquadId (void) {
			return(squadId);
		}

		virtual int32_t setTeamId (int32_t _teamId, bool setup);
		
		virtual int32_t getTeamId (void) {
			return(teamId);
		}

		virtual TeamPtr getTeam (void);

		virtual bool isFriendly (TeamPtr team);

		virtual bool isEnemy (TeamPtr team);

		virtual bool isNeutral (TeamPtr team);

		virtual int32_t setGroupId (int32_t _groupId, bool setup);

		virtual int32_t getGroupId (void) {
			return(groupId);
		}

		virtual MoverGroupPtr getGroup (void);

		virtual void setPilotHandle (int32_t _pilotHandle);

		virtual void loadPilot (PSTR pilotFileName, PSTR brainFileName, LogisticsPilot *lPilot);

		virtual void setCommanderId (int32_t _commanderId);

		virtual int32_t getCommanderId (void) {
			return(commanderId);
		}

		virtual CommanderPtr getCommander (void);
		
		virtual MechWarriorPtr getPilot (void) {
			return(pilot);
		}

		MoverPtr getPoint (void);

		void setTeamRosterIndex (int32_t index) {
			teamRosterIndex = index;
		}

		int32_t getTeamRosterIndex (void) {
			return(teamRosterIndex);
		}

		virtual int32_t getContacts (int32_t* contactList, int32_t contactCriteria, int32_t sortType);

		int32_t getContactStatus (int32_t scanningTeamID, bool includingAllies);

//		void setNetPlayerId (uint32_t playerId) {
//			netPlayerId = playerId;
//		}

//		uint32_t getNetPlayerId (void) {
//			return(netPlayerId);
//		}

		void setNetPlayerName(PCSTR name) {
			if (netPlayerName)
				strncpy(netPlayerName,name,255);
		}

		PCSTR getNetPlayerName(void) {
			return(netPlayerName);
		}

		void setLocalMoverId (int32_t id) {
			localMoverId = id;
		}

		int32_t getLocalMoverId (void) {
			return(localMoverId);
		}

		virtual int32_t getCBills (void) {
			return(0);
		}

		void setNetRosterIndex (int32_t index) {
			netRosterIndex = index;
		}

		int32_t getNetRosterIndex (void) {
			return(netRosterIndex);
		}

		int32_t getNumWeaponFireChunks (int32_t which) {
			return(numWeaponFireChunks[which]);
		}

		int32_t clearWeaponFireChunks (int32_t which);

		int32_t addWeaponFireChunk (int32_t which, WeaponFireChunkPtr chunk);

		int32_t addWeaponFireChunks (int32_t which, uint32_t* packedChunkBuffer, int32_t numChunks);

		int32_t grabWeaponFireChunks (int32_t which, uint32_t* packedChunkBuffer, int32_t maxChunks);

		virtual int32_t updateWeaponFireChunks (int32_t which);

		int32_t getNumCriticalHitChunks (int32_t which) {
			return(numCriticalHitChunks[which]);
		}

		int32_t clearCriticalHitChunks (int32_t which);

		int32_t addCriticalHitChunk (int32_t which, int32_t bodyLocation, int32_t criticalSpace);

		int32_t addCriticalHitChunks (int32_t which, puint8_t packedChunkBuffer, int32_t numChunks);

		int32_t grabCriticalHitChunks (int32_t which, puint8_t packedChunkBuffer);

		virtual int32_t updateCriticalHitChunks (int32_t which);

		int32_t getNumRadioChunks (int32_t which) {
			return(numRadioChunks[which]);
		}

		int32_t clearRadioChunks (int32_t which);

		int32_t addRadioChunk (int32_t which, uint8_t msg);

		int32_t addRadioChunks (int32_t which, puint8_t packedChunkBuffer, int32_t numChunks);

		int32_t grabRadioChunks (int32_t which, puint8_t packedChunkBuffer);

		virtual int32_t updateRadioChunks (int32_t which);

		virtual StatusChunkPtr getStatusChunk (void) {
			return(&statusChunk);
		}

		virtual int32_t buildStatusChunk (void) {
			return(NO_ERROR);
		}

		virtual int32_t handleStatusChunk (int32_t updateAge, uint32_t chunk) {
			return(NO_ERROR);
		}

		virtual MoveChunkPtr getMoveChunk (void) {
			return(&moveChunk);
		}

		virtual int32_t buildMoveChunk (void) {
			return(NO_ERROR);
		}

		virtual int32_t handleMoveChunk (uint32_t chunk) {
			return(NO_ERROR);
		}

		void setMoveChunk (MovePathPtr path, MoveChunkPtr chunk);

		void playMessage (RadioMessageType messageId, bool propogateIfMultiplayer = false);

		virtual int32_t calcCV (bool calcMax = false) {
			return(0);
		}

		void setFieldedCV (int32_t CV) {
			fieldedCV = CV;
		}

		uint32_t getFieldedCV (void) {
			return(fieldedCV);
		}

		virtual void setThreatRating (int16_t rating);

		virtual int32_t getThreatRating (void);

//		virtual void getDamageClass (int32_t& damageClass, bool& shutDown);

		virtual bool refit (float pointsAvailable, float& pointsUsed, bool ammoOnly = false);

		float calcRecoverPrice (void);

		virtual bool recover (void);

		int32_t getInventoryDamage (int32_t itemIndex);

		int32_t getInventoryMax (int32_t itemIndex) {
			return(MasterComponent::masterList[inventory[itemIndex].masterID].getHealth());
		}

		virtual int32_t getBodyState (void) {
			return(-1);
		}

		int32_t getSensorMax (void) {
			if (sensor > -1)
				return(MasterComponent::masterList[inventory[sensor].masterID].getHealth());
			else
				return(0);
		}

		int32_t getSensorHealth (void) {
			if (sensor > -1)
				return(inventory[sensor].health);
			else
				return(0);
		}

		float getVisualRange (void);

		float getLastWeaponEffectivenessCalc (void) {
			return(lastWeaponEffectivenessCalc);
		}

		float getLastOptimalRangeCalc (void) {
			return(lastOptimalRangeCalc);
		}

		virtual float getTotalEffectiveness(void)
		{
			return (0.0);
		}
		
		//ContactWatchPtr addContactWatch (GameObjectPtr who, ContactRecPtr contact) {
		//	return(contactWatches.add(who, contact));
		//}

		//void removeContactWatch (ContactWatchPtr watch) {
		//	contactWatches.remove(watch);
		//}

		void setChallenger (GameObjectPtr challenger);
		
		GameObjectPtr getChallenger (void);

/*		void addContact (ContactRecPtr contact) {
			sensorSystem.addContact(contact);
		}
*/
		int32_t scanContact (int32_t contactType, uint32_t contactHandle);

		int32_t analyzeContact (int32_t contactType, uint32_t contactHandle);

		int32_t scanBattlefield (int32_t quadrant, int32_t contactType, int32_t potentialContactType);

		virtual Stuff::Vector3D calcOffsetMoveGoal (Stuff::Vector3D target);

		virtual int32_t calcGlobalPath (GlobalPathStep* globalPath, GameObjectPtr obj, Stuff::Vector3D* location, bool useClosedAreas);

		virtual int32_t calcMoveGoal (GameObjectPtr target,
						  Stuff::Vector3D moveCenter,
						  float moveRadius,
						  Stuff::Vector3D moveGoal,
						  int32_t selectionIndex,
						  Stuff::Vector3D& newGoal,
						  int32_t numValidAreas,
						  pint16_t validAreas,
						  uint32_t moveParams);
								   

		virtual int32_t calcMovePath (MovePathPtr path,
								   int32_t pathType,
								   Stuff::Vector3D start,
								   Stuff::Vector3D goal,
								   int32_t* goalCell,
								   uint32_t moveParams = MOVEPARAM_NONE);

		virtual int32_t calcEscapePath (MovePathPtr path,
									 Stuff::Vector3D start,
									 Stuff::Vector3D goal,
									 int32_t* goalCell,
									 uint32_t moveParams,
									 Stuff::Vector3D& escapeGoal);

		virtual int32_t calcMovePath (MovePathPtr path,
								   Stuff::Vector3D start,
								   int32_t thruArea[2],
								   int32_t goalDoor,
								   Stuff::Vector3D finalGoal,
								   Stuff::Vector3D* goalWorldPos,
								   int32_t* goalCell,
								   uint32_t moveParams = MOVEPARAM_NONE);

		virtual float weaponLocked (int32_t weaponIndex, Stuff::Vector3D targetPosition);

		virtual bool weaponInRange (int32_t weaponIndex, float metersToTarget, float buffer);

		virtual int32_t getWeaponsReady (int32_t* list, int32_t listSize);

		virtual int32_t getWeaponsLocked (int32_t* list, int32_t listSize);

		virtual int32_t getWeaponsInRange (int32_t* list, int32_t listSize, float orderFireRange);

		virtual int32_t getWeaponShots (int32_t weaponIndex);
		
		virtual bool getWeaponIndirectFire (int32_t weaponIndex);

		virtual bool getWeaponAreaEffect (int32_t weaponIndex);
		
		virtual float getWeaponAmmoLevel (int32_t weaponIndex);

		bool	getWeaponIsEnergy( int32_t weaponIndx );

		virtual void calcWeaponEffectiveness (bool setMax);

		virtual void calcAmmoTotals (void);

		virtual int32_t calcFireRanges (void);

		virtual float getOrderedFireRange (int32_t* attackRange = NULL);

		virtual float getMinFireRange (void);

		virtual float getMaxFireRange (void);

		virtual float getOptimalFireRange (void);

		virtual bool isWeaponIndex (int32_t itemIndex);

		virtual bool isWeaponReady (int32_t weaponIndex);

		virtual bool isWeaponMissile (int32_t weaponIndex);

		virtual void startWeaponRecycle (int32_t weaponIndex);

		virtual int32_t tallyAmmo (int32_t ammoMasterId);

		virtual int32_t reduceAmmo (int32_t ammoMasterId, int32_t amount);

		virtual int32_t getNumAmmoTypes (void) {
			return(numAmmoTypes);
		}

		bool hasNonAreaWeapon (void);
	
		virtual int32_t getAmmoType (int32_t ammoTypeIndex) {
			return(ammoTypeTotal[ammoTypeIndex].masterId);
		}

		virtual int32_t getAmmoTypeTotal (int32_t ammoTypeIndex) {
			//--------------------------------------------------
			// This assumes 0 <= ammoTypeIndex < numAmmoTypes...
			return(ammoTypeTotal[ammoTypeIndex].curAmount);
		}

		virtual int32_t getAmmoTypeStart (int32_t ammoTypeIndex) {
			//--------------------------------------------------
			// This assumes 0 <= ammoTypeIndex < numAmmoTypes...
			return(ammoTypeTotal[ammoTypeIndex].maxAmount);
		}

		virtual void deductWeaponShot (int32_t weaponIndex, int32_t ammoAmount = 1);

		virtual bool needsRefit(void);
		
		virtual int32_t sortWeapons (int32_t* weaponList, int32_t* valueList, int32_t listSize, int32_t sortType, bool skillCheck);

		virtual float calcAttackChance (GameObjectPtr target, int32_t aimLocation, float targetTime, int32_t weaponIndex, float modifiers, int32_t* range, Stuff::Vector3D* targetPoint = NULL);

		virtual float calcAttackModifier (GameObjectPtr target, int32_t weaponIndex, bool skillCheck) {
			return(0.0);
		}

		MasterComponentPtr getMasterComponent (int32_t itemIndex) {
			return(&MasterComponent::masterList[inventory[itemIndex].masterID]);
		}

		virtual bool hitInventoryItem (int32_t itemIndex, bool setupOnly = FALSE) {
			return(false);
		}

		bool isInventoryDisabled (int32_t itemIndex) {
			return(inventory[itemIndex].disabled);
		}

		virtual void disable (uint32_t cause);

		virtual void shutDown (void);
		
		virtual void startUp (void);

		virtual void destroyBodyLocation (int32_t location) {}

		virtual void calcCriticalHit (int32_t hitLocation) {}

		virtual bool injureBodyLocation (int32_t bodyLocation, float damage) {
			return(false);
		}

		virtual void ammoExplosion (int32_t ammoIndex);

		virtual int32_t fireWeapon (GameObjectPtr target, float targetTime, int32_t weaponIndex, int32_t attackType, int32_t aimLocation, Stuff::Vector3D* targetPoint, float &damageDone) {
			return(NO_ERROR);
		}

		virtual int32_t handleWeaponFire (int32_t weaponIndex,
									   GameObjectPtr target,
									   Stuff::Vector3D* targetPoint,
									   bool hit,
									   float entryAngle,
									   int32_t numMissiles,
									   int32_t hitLocation) {
			return(NO_ERROR);
		}

		virtual void printFireWeaponDebugInfo (GameObjectPtr target, Stuff::Vector3D* targetPoint, int32_t chance, int32_t aimLocation, int32_t roll, WeaponShotInfo* shotInfo);

		virtual void printHandleWeaponHitDebugInfo (WeaponShotInfo* shotInfo);

		virtual float relFacingDelta (Stuff::Vector3D goalPos, Stuff::Vector3D targetPos);

		virtual float relFacingTo (Stuff::Vector3D goal, int32_t bodyLocation = -1);

		virtual float relViewFacingTo (Stuff::Vector3D goal) {
			return(GameObject::relFacingTo(goal));
		}

		bool canMoveHere (Stuff::Vector3D worldPos);

		void setLastValidPosition (Stuff::Vector3D pt) {
			lastValidPosition = pt;
		}

		Stuff::Vector3D getLastValidPosition (void) {
			return(lastValidPosition);
		}

		virtual bool canPowerUp (void) {
			return(true);
		}
		
		virtual bool canMove (void) {
			return(true);
		}
		
		virtual bool canJump (void) {
			return(false);
		}

		virtual float getJumpRange (int32_t* numOffsets = NULL, int32_t* jumpCost = NULL) {
			if (numOffsets)
				*numOffsets = 8;
			if (jumpCost)
				*jumpCost = 0;
			return(0.0);
		}

		virtual bool isJumping (Stuff::Vector3D* jumpGoal = NULL) {
			return(false);
		}

		virtual bool isMineSweeper (void) {
			return(false);
		}

		virtual bool isMineLayer (void) {
			return(false);
		}

		virtual bool isLayingMines (void) {
			return(false);
		}

		virtual float getFireArc (void);

		virtual float calcMaxSpeed (void) {
			return(0.0);
		}

		virtual float calcSlowSpeed (void) {
			return(0.0);
		}

		virtual float calcModerateSpeed (void) {
			return(0.0);
		}

		virtual int32_t calcSpriteSpeed (float speed, uint32_t flags, int32_t& state, int32_t& throttle) {
			state = 0;
			throttle = 100;
			return(-1);
		}

		virtual Stuff::Vector3D getPositionFromHS (int32_t weaponType) 
		{
			Stuff::Vector3D pos;
			pos.Zero();
			return(pos);
		}

		virtual float getGestureStopDistance (void) {
			return 0.0;
		}
		
		virtual bool handleEjection (void);

		virtual bool isWithdrawing (void);
		
		virtual bool underPlayerControl (void) {
			return (true);
		}
		
		~Mover (void) {
			destroy();
		}

		bool enemyRevealed (void);

		virtual PSTR getIfaceName(void) {
			return ("No Name");
		}

		void drawSensorTextHelp (float screenX, float screenY, int32_t resID, uint32_t color, bool drawBOLD);

		virtual SensorSystem* getSensorSystem(){ return sensorSystem; }

		
		static int32_t loadGameSystem (FitIniFilePtr mechFile, float visualRange);

		bool isCloseToFirstTacOrder( Stuff::Vector3D& pos );

		void removeFromUnitGroup( int32_t id );
		void addToUnitGroup( int32_t id );
		bool isInUnitGroup( int32_t id );
		virtual bool isRefit(){ return 0; }
		virtual bool isRecover() { return 0; }

		int32_t getIconPictureIndex(){ return iconPictureIndex; }

		bool isWeaponWorking(int32_t index );

		virtual void startShutDown (void)
		{
		
		}
		
		virtual bool isMech (void)
		{
			return false;
		}
		
		virtual bool isVehicle (void)
		{
			return false;
		}
		
		virtual bool isGuardTower (void)
		{
			return false;
		}

		virtual bool isOnGUI (void)
		{
			return isOnGui;
		}
		
		virtual void setOnGUI (bool onGui)
		{
			isOnGui = onGui;
		}
		
		virtual void Save (PacketFilePtr file, int32_t packetNum);

		void CopyTo (MoverData *data);

		void Load (MoverData *data);

		static void initOptimalCells (int32_t numIncrements);
};

//---------------------------------------------------------------------------

typedef struct _MoverInitData {
	char			pilotFileName[50];
	bool			overrideLoadedPilot;
	float			gunnerySkill;
	float			pilotingSkill;
	bool			specialtySkills[NUM_SPECIALTY_SKILLS];
	char			brainFileName[50];
	char			csvFileName[50];
	int32_t			objNumber;
	int32_t			rosterIndex;
	uint32_t	controlType;
	uint32_t	controlDataType;
	uint32_t	variant;
	Stuff::Vector3D	position;
	int32_t			rotation;
	char			teamID;
	char			commanderID;
	uint32_t			baseColor;
	uint32_t			highlightColor1;
	uint32_t			highlightColor2;
	char			gestureID;
	bool			active;
	bool			exists;
	char			icon;
	bool			capturable;
	int32_t			numComponents;
	int32_t			components[50];
} MoverInitData;

//---------------------------------------------------------------------------

extern MoverPtr getMoverFromHandle (int32_t partId);

//******************************************************************************************

#endif
