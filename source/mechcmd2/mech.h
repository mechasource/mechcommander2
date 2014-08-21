//***************************************************************************
//
//	mech.h - This file contains the BattleMech Class header
//
//	MechCommander 2
//
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#pragma once

#ifndef MECH_H
#define MECH_H

//#include <mclib.h>
//#include "mover.h"
//#include "dmech.h"
//#include "gameobj.h"
//#include "objtype.h"
//#include "dobjnum.h"
//#include "cmponent.h"
//#include "warrior.h"
//#include "djet.h"
//#include "mechclass.h"

extern float metersPerWorldUnit;

//***************************************************************************

#define	PILOTCHECK_SITUATION_NONE		0
#define PILOTCHECK_SITUATION_JUMPING	1
#define	PILOTCHECK_SITUATION_COLLISION	2

#define	MECH_SPRITESPEED_FLAG_NONE		0
#define	MECH_SPRITESPEED_FLAG_GO_SLOW	1

#define	NUM_MECH_HIT_ARCS				4
#define MECH_HIT_ARC_FRONT				0
#define	MECH_HIT_ARC_REAR				1
#define	MECH_HIT_ARC_LEFT				2
#define	MECH_HIT_ARC_RIGHT				3

#define	NUM_MECH_HIT_SECTIONS			3
#define	MECH_HIT_SECTION_TOP			0
#define	MECH_HIT_SECTION_MIDDLE			1
#define	MECH_HIT_SECTION_BOTTOM			2

#define	HIT_TYPE_WEAK					0
#define	HIT_TYPE_MODERATE				1
#define	HIT_TYPE_HEAVY					2

#define	NUM_CRITICAL_SPACES_HEAD		6
#define	NUM_CRITICAL_SPACES_CTORSO		12
#define	NUM_CRITICAL_SPACES_LTORSO		12
#define	NUM_CRITICAL_SPACES_RTORSO		12
#define	NUM_CRITICAL_SPACES_LARM		12
#define	NUM_CRITICAL_SPACES_RARM		12
#define	NUM_CRITICAL_SPACES_LLEG		6
#define	NUM_CRITICAL_SPACES_RLEG		6

#define	NUM_ACTUATORS					4
#define	ACTUATOR_LSHOULDER				0
#define	ACTUATOR_RSHOULDER				1
#define	ACTUATOR_LHIP					2
#define	ACTUATOR_RHIP					3

#define	NUM_MECH_GESTURES				25
#define	MECH_GESTURE_PARK				0
#define	MECH_GESTURE_STAND_TO_PARK		1
#define	MECH_GESTURE_STAND				2
#define	MECH_GESTURE_STAND_TO_WALK		3
#define	MECH_GESTURE_WALK				4
#define	MECH_GESTURE_WALK_TO_STAND		5
#define	MECH_GESTURE_WALK_TO_RUN		6
#define	MECH_GESTURE_RUN				7
#define	MECH_GESTURE_RUN_TO_WALK		8
#define MECH_GESTURE_REVERSE			9
#define MECH_GESTURE_STAND_TO_REVERSE	10
#define MECH_GESTURE_LIMPING			11	//Limp
#define MECH_GESTURE_WALK_HIT_B			12	//WalkToHitBehindToWalk
#define MECH_GESTURE_WALK_HIT_F			13	//WalkToHitFrontToWalk
#define MECH_GESTURE_WALK_FALL_B		14	//WalkToFallBackward 
#define MECH_GESTURE_WALK_FALL_F		15	//WalkToFallForward 
#define MECH_GESTURE_RUN_FALL_B			16	//RunToFallBackward 
#define MECH_GESTURE_RUN_FALL_F			17	//RunToFallForward 
#define MECH_GESTURE_STAND_FALL_B		18	//StandToFallBackward 
#define MECH_GESTURE_STAND_FALL_F		19	//StandToFallForward 
#define MECH_GESTURE_JUMP				20	//Jump
#define MECH_GESTURE_ROLLOVER			21	//Rollover from FallBack to FallForward 
#define MECH_GESTURE_GETUP				22	//Get up from FallForward 
#define MECH_GESTURE_FALLEN_F			23	//Fallen Forward (Single Frame, mech on ground)
#define MECH_GESTURE_FALLEN_B			24	//Fallen Backward (Single Frame, mech on ground)

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

#define	MAGIC_ROTATE_FORCE_NUMBER		(360.0 / (17.0))
#define MAX_MECH_SMOKES 4

//------------------------------------------------------------------------------------------
// Enums 
//------------------------------------------------------------------------------------------

typedef enum {
	MECH_BODY_LOCATION_ANY = -1,
	MECH_BODY_LOCATION_HEAD,
	MECH_BODY_LOCATION_CTORSO,
	MECH_BODY_LOCATION_LTORSO,
	MECH_BODY_LOCATION_RTORSO,
	MECH_BODY_LOCATION_LARM,
	MECH_BODY_LOCATION_RARM,
	MECH_BODY_LOCATION_LLEG,
	MECH_BODY_LOCATION_RLEG,
	NUM_MECH_BODY_LOCATIONS
} MechBodyLocationType;

typedef enum {
	MECH_ARMOR_LOCATION_ANY = -1,
	MECH_ARMOR_LOCATION_HEAD,
	MECH_ARMOR_LOCATION_CTORSO,
	MECH_ARMOR_LOCATION_LTORSO,
	MECH_ARMOR_LOCATION_RTORSO,
	MECH_ARMOR_LOCATION_LARM,
	MECH_ARMOR_LOCATION_RARM,
	MECH_ARMOR_LOCATION_LLEG,
	MECH_ARMOR_LOCATION_RLEG,
	MECH_ARMOR_LOCATION_RCTORSO,
	MECH_ARMOR_LOCATION_RLTORSO,
	MECH_ARMOR_LOCATION_RRTORSO,
	NUM_MECH_ARMOR_LOCATIONS
} MechArmorLocationType;

typedef enum {
	LEG_STATUS_NORMAL,
	LEG_STATUS_HURTING,
	LEG_STATUS_IMPAIRED_LEFT,
	LEG_STATUS_IMPAIRED_RIGHT,
	LEG_STATUS_DESTROYED,
	NUM_LEG_STATUSES
} LegStatusType;

typedef enum {
	TORSO_STATUS_NORMAL,
	TORSO_STATUS_IMPAIRED,
	NUM_TORSO_STATUSES
} TorsoStatusType;

typedef enum {
	MECH_PILOT_CHECK_CONDITION_NO_LEG,
	MECH_PILOT_CHECK_CONDITION_NO_HIP,
	NUM_MECH_PILOT_CHECK_CONDITIONS
} MechPilotCheckCondition;

typedef enum {
	MECH_STATUSCHUNK_BODYSTATE_NORMAL,
	MECH_STATUSCHUNK_BODYSTATE_STANDING,
	MECH_STATUSCHUNK_BODYSTATE_PARKED,
	MECH_STATUSCHUNK_BODYSTATE_FALLEN_BACKWARD,
	MECH_STATUSCHUNK_BODYSTATE_FALLEN_FORWARD,
	MECH_STATUSCHUNK_BODYSTATE_DESTROYED
} MechStatusChunkBodyState;

//---------------------------------------------------------------------------
// classes and structs
//---------------------------------------------------------------------------

class BattleMechType : public ObjectType {

	//-------------
	// Data Members

	public:

		bool				isEndoSteel;								// has endosteel frame?
		uint8_t		maxInternalStructure[NUM_BODY_LOCATIONS];

		PSTR				anim;
		uint8_t		moveAnimSpeed[3];

		//ULONG		controlType;								// Who controls us, Player, AI or NET?
		//ULONG		controlDataType;							// which data set do we use.
		MoverDynamics		dynamics;									// what dynamics do we use.

//----------------------------------------------------------------------------------------
// NO MORE HOTSPOT DATA HERE.  ALL RESIDES IN APPEARANCE NOW!  Including footprint type!

		ULONG		rightArmDebrisId;							//Id of the shot off right arm.
		ULONG		leftArmDebrisId;							//Id of the shot off left arm.
		ULONG		destroyedDebrisId;							//Id of the destroyed mech piece.
		int32_t				moveType;

		int32_t				crashAvoidSelf;
		int32_t				crashAvoidPath;
		int32_t				crashBlockSelf;
		int32_t				crashBlockPath;
		float				crashYieldTime;

		float				explDmg;
		float				explRad;
		
		float				LOSFactor;
		float				destructDamage;

	//----------------
	//Member Functions

	public:

		void init (void);

		BattleMechType (void)
		{
			ObjectType::init();
			init();
		}
		
		virtual int32_t init (FilePtr objFile, ULONG fileSize);
		
		virtual GameObjectPtr createInstance (void);
		
		virtual void destroy (void);

		~BattleMechType (void) {
			destroy();
		}

		virtual bool handleCollision (GameObjectPtr collidee, GameObjectPtr collider);

		virtual bool handleDestruction (GameObjectPtr collidee, GameObjectPtr collider);
};

//---------------------------------------------------------------------------
typedef struct _MechData : public MoverData
{
	char				chassisClass;				   
	int32_t				chassisBR;					   

	int32_t				variantID;					   
	char				variantName[64];			   

	char				legStatus;					   
	char				torsoStatus;				   

	uint8_t		actuator[NUM_ACTUATORS];	   
	uint8_t		gyro;						   

	bool				sensorOK;

	uint8_t		numJumpJets;
	float				lastJumpTime;
	bool				inJump;
	Stuff::Vector3D		jumpGoal;
	float				centerTorsoBlowTime;
	bool				hitFromBehindThisFrame;
	bool				hitFromFrontThisFrame;

	bool				leftArmBlownThisFrame;
	bool				rightArmBlownThisFrame;

	float				torsoRotation;				   
	float				rightArmRotation;			   
	float				leftArmRotation;			   

	bool				fallen;						    
	bool				mechRemoved;				   

	float				pivotTurnRate;

	bool				playedJumpSFX;				   
	bool				playedCriticalHit;			   

	float				maxWeaponDamage;			   

	char				longName[MAXLEN_MECH_LONGNAME];
	int32_t				pilotNum;		

	bool				captureable;
	bool				notMineYet;		

	int32_t				descID;			

	float				damageThisFrame;
	bool				sentCrippledMsg;

	float				rotateValues[6];

	int32_t				ItemLocationToInvLocation[MAX_MOVER_INVENTORY_ITEMS];

	float				damageAfterDisabled;
											
	int32_t				numArmorComponents;
	int32_t				cBills;

	MechAppearanceData	apData;
} MechData;

class BattleMech : public Mover {

	//------------
	//Data Members
	
	public:

		char				chassisClass;					// none, light, medium, heavy, assault
		int32_t				chassisBR;						// chassis Battle Rating

		int32_t				variantID;						// need to store this for logistics
		char				variantName[64];				// user defined


		//------------------
		// Additional status
		char				legStatus;						// normal, hurting, impaired, disabled
		char				torsoStatus;					// normal, impaired

		//--------------------------------------
		// Additional Critical Component Indices
		uint8_t		actuator[NUM_ACTUATORS];		// actuator inventory index
		uint8_t		gyro;							// gyro inventory index

		//------------------
		// Additional Engine
		uint8_t		numJumpJets;
		float				lastJumpTime;
		bool				inJump;
		Stuff::Vector3D		jumpGoal;
		float				centerTorsoBlowTime;
		bool				hitFromBehindThisFrame;
		bool				hitFromFrontThisFrame;
		
		//-----------------------
		// Additional Update Info
		bool				leftArmBlownThisFrame;
		bool				rightArmBlownThisFrame;

		float				torsoRotation;					// Current Rotation of Torso in Degrees
		float				rightArmRotation;				// Current Rotation of Right Arm in Degrees
		float				leftArmRotation;				// Current Rotation of Left Arm in Degrees
		
		bool				fallen;							
		bool				mechRemoved;					// Have we removed ourselves from the interface.

		float				pivotTurnRate;
		
		bool				playedJumpSFX;					//Have I played the Jump SOund EFfect for this jump?
		bool				playedCriticalHit;				//Have I played a critical hit explosion this frame?
		
		float				maxWeaponDamage;				// max total damage possible
		
		char				longName[MAXLEN_MECH_LONGNAME];		//Used by logistics (and the interface) to get int32_t name.
		int32_t				pilotNum;						//Used by logistics to restore pilot on return.

		bool				captureable;
		bool				notMineYet;						//Used by logsitics to drop this mech if mission lost.

		int32_t				descID;							//Used by Logistics to Desc.		
//		int32_t				variant;						// Now we have three longs storing a bytes worth of data well done SJ
		
		float				damageThisFrame;				// How much damage have we taken this frame.
		bool				sentCrippledMsg;				// Have I already announced that I'm gimped?
		bool				playedHeloDestruct;

		float				rotateValues[6];
		
		int32_t				ItemLocationToInvLocation[MAX_MOVER_INVENTORY_ITEMS];
		
		float				damageAfterDisabled;			// Stores damaged inflicted after disabled.
															// Once we reach the threshold, BLOW the mech for real!!
															
		int32_t				numArmorComponents;
		int32_t				cBills;

	//----------------
	//Member Functions

	public:

		void operator = (BattleMech copy);
		
		virtual void init (bool create);
		
		BattleMech (void)
		{
			init(true);
		}
			
		virtual void init (bool create, ObjectTypePtr _type);

		virtual int32_t init (FitIniFile* mechFile);

		virtual int32_t init (ULONG variantNum);

		virtual int32_t init (FilePtr mechFile);

		virtual void destroy (void);

		virtual void clear (void);

		virtual int32_t write (FilePtr mechFile);

		virtual float getStatusRating (void);

		virtual void setControl (ControlType ctrlType);

		virtual bool crashAvoidanceSystem (void);

		virtual void updateAIControl (void);

		virtual void updatePlayerControl (void);

		virtual void updateNetworkControl (void);

		virtual void updateDynamics (void);

		virtual int32_t update (void);
		
		virtual void render (void);

		virtual void renderShadows (void);
	
		virtual bool isCrippled (void) {
			return((legStatus == LEG_STATUS_IMPAIRED_RIGHT) ||(legStatus == LEG_STATUS_IMPAIRED_LEFT) || (legStatus == LEG_STATUS_DESTROYED));
		}

		virtual int32_t calcHitLocation (GameObjectPtr attacker, int32_t weaponIndex, int32_t attackSource, int32_t attackType);

		virtual int32_t handleWeaponHit (WeaponShotInfoPtr shotInfo, bool addMultiplayChunk = false);

		virtual float weaponLocked (int32_t weaponIndex, Stuff::Vector3D targetPosition);

		virtual float getWeaponHeat (int32_t weaponIndex) {
			return(MasterComponent::masterList[inventory[weaponIndex].masterID].getWeaponHeat());
		}

		virtual void pilotingCheck (ULONG situation = 0, float modifier = 0.0);

		virtual void mineCheck (void);

		bool updateJump (void);

		bool pivotTo (void);

		void updateMoveStateGoal (void);

		bool updateMovePath (float& newRotate, char& newThrottleSetting, int32_t& newGestureStateGoal, int32_t& newMoveState, int32_t& minThrottle, int32_t& maxThrottle, float& facingRotate);

		bool netUpdateMovePath (float& newRotate, char& newThrottleSetting, int32_t& newGestureStateGoal, int32_t& newMoveState, int32_t& minThrottle, int32_t& maxThrottle);

		void setNextMovePath (char& newThrottleSetting, int32_t& newGestureStateGoal);

		void setControlSettings (float& newRotate, char& newThrottleSetting, int32_t& newGestureStateGoal, int32_t& minThrottle, int32_t& maxThrottle, float& facingRotate);

		void updateTorso (float newRotate);

		virtual int32_t getCBills (void) {
			return(cBills);
		}

		virtual void updateMovement (void);

		virtual void netUpdateMovement (void);

		virtual int32_t calcCV (bool calcMax = false);

		int32_t getResourcePointValue (void);

		virtual bool isWeaponReady (int32_t weaponIndex);

		virtual float calcAttackChance (GameObjectPtr target, int32_t aimLocation, float targetTime, int32_t weaponIndex, float modifiers, int32_t* range, Stuff::Vector3D* targetPoint = NULL);

		virtual float getTotalEffectiveness(void);

		virtual bool hitInventoryItem (int32_t itemIndex, bool setupOnly = false);

		virtual void destroyBodyLocation (int32_t location);

		virtual void handleCriticalHit (int32_t bodyLocation, int32_t criticalSpace);

		virtual int32_t updateCriticalHitChunks (int32_t which);

		virtual int32_t buildStatusChunk (void);

		virtual int32_t handleStatusChunk (int32_t updateAge, ULONG chunk);

		virtual int32_t buildMoveChunk (void);

		virtual int32_t handleMoveChunk (ULONG chunk);

		virtual void calcCriticalHit (int32_t hitLocation);

		virtual bool injureBodyLocation (int32_t bodyLocation, float damage);

		virtual int32_t handleWeaponFire (int32_t weaponIndex,
									   GameObjectPtr target,
									   Stuff::Vector3D* targetPoint,
									   bool hit,
									   float entryAngle,
									   int32_t numMissiles,
									   int32_t hitLocation);

		virtual int32_t fireWeapon (GameObjectPtr target, float targetTime, int32_t weaponIndex, int32_t attackType, int32_t aimLocation, Stuff::Vector3D* targetPoint, float &damageDone);

		virtual float relFacingTo (Stuff::Vector3D goal, int32_t bodyLocation = -1);

		virtual float relViewFacingTo (Stuff::Vector3D goal) {
			return(relFacingTo(goal, MECH_BODY_LOCATION_CTORSO));
		}

		virtual bool canPowerUp (void);

		virtual Stuff::Vector3D getPositionFromHS (int32_t weaponType);

		virtual int32_t getBodyState (void);

		virtual float getDestructLevel (void)
		{
			return 5000.0f;		//ALWAYS just keep shooting at a mech.  This would be nearly impossible to calculate.
		}

		//-----------------------------------
		// Additional Mech-specific functions

		void createJumpFX (void);
		
		void endJumpFX (void);
		
		Stuff::Vector3D getJumpPosition (void);

		int32_t getActuatorMax (int32_t actuatorId) {
			return(MasterComponent::masterList[inventory[actuator[actuatorId]].masterID].getHealth());
		}
		
		int32_t getActuatorHealth (int32_t actuatorId) {
			return(inventory[actuator[actuatorId]].health);
		}

		int32_t calcLegStatus (void);

		int32_t calcTorsoStatus (void);

		bool isTorso (int32_t bodyLocation) {
			return((bodyLocation >= MECH_BODY_LOCATION_CTORSO) && (bodyLocation <= MECH_BODY_LOCATION_RTORSO));
		}

		int32_t transferHitLocation (int32_t hitLocation);

		bool inTransitionGestureState (void) {
			return(appearance->getInTransition());
		}

		int32_t setGestureStateGoal (int32_t state) {
			return(appearance->setGestureGoal(state));
		}

		int32_t getGestureFramesLeft (void) 
		{
			return(appearance->getNumFramesInGesture(appearance->getCurrentGestureGoal()) - appearance->getFrameNumber());
		}

		bool isStanding (void) 
		{
			return(appearance->getOldGestureGoal() == MECH_STATE_STANDING);
		}

		bool isWalking (int32_t* throttle = NULL) 
		{
			if (throttle)
				*throttle = control.settings.mech.throttle;

			return(appearance->getOldGestureGoal() == MECH_STATE_WALKING);
		}

		bool isRunning (void) 
		{
			return(appearance->getOldGestureGoal() == MECH_STATE_RUNNING);
		}

		virtual bool isJumping (Stuff::Vector3D* jumpGoal = NULL);

		bool isReversing (void) 
		{
			return(appearance->getOldGestureGoal() == MECH_STATE_REVERSE);
		}

		bool isFallen (void) 
		{
			return((appearance->getOldGestureGoal() == MECH_STATE_FALLEN_BACKWARD) ||
				   (appearance->getOldGestureGoal() == MECH_STATE_FALLEN_FORWARD));
		}

		bool canRotateTorso (void) {
			return(body[MECH_BODY_LOCATION_CTORSO].damageState == IS_DAMAGE_NONE);
		}

		bool canRun (void) {
			return(legStatus == LEG_STATUS_NORMAL);
		}

		bool canWalk (void) {
			return(legStatus < LEG_STATUS_IMPAIRED_LEFT);
		}

		bool canLimp (void) {
			return((legStatus == LEG_STATUS_IMPAIRED_LEFT) || (legStatus == LEG_STATUS_IMPAIRED_RIGHT));
		}

		bool canMove (void) {
			return(legStatus != LEG_STATUS_DESTROYED);
		}

		virtual int32_t getSpeedState (void);

		virtual bool canJump (void) {
			return(numJumpJets > 0);
		}

		virtual float getJumpRange (int32_t* numOffsets = NULL, int32_t* jumpCost = NULL);

		virtual bool handleFall (bool forward);

		virtual bool handleEjection (void);

		float calcMaxSpeed (void);

		float calcSlowSpeed (void);

		float calcModerateSpeed (void);

		int32_t calcSpriteSpeed (float speed, ULONG flags, int32_t& state, int32_t& throttle);

		void forceTorsoAlign (void) {
			torsoRotation = rightArmRotation = leftArmRotation = 0.0;
		}

		~BattleMech (void) {
			destroy();
		}

		virtual bool isCaptureable(int32_t capturingTeamID);
		
		virtual void handleStaticCollision (void);

		virtual float calcMaxTargetDamage(void);

		bool isWeaponWorking (int32_t weaponIndex);
		
		void damageLoadedComponents (void);

		virtual PSTR getIfaceName(void) {
			return(longName);
		}

		static int32_t loadGameSystem (FitIniFilePtr mechFile);
		
		virtual bool isMech (void)
		{
			return true;
		}
		
		virtual bool isVehicle (void)
		{
			return false;
		}
		
		virtual bool isGuardTower (void)
		{
			return false;
		}
		
		void resetComponents (int32_t totalComponents, int32_t* componentList);
		
		virtual float getLOSFactor (void)
		{
			return (((BattleMechType *)getObjectType())->LOSFactor);
		}
		
		virtual void startShutDown (void);

		virtual void Save (PacketFilePtr file, int32_t packetNum);

		void CopyTo (MechData *data);

		void Load (MechData *data);

 		static void initOptimalCells (int32_t numIncrements);

		virtual void repairAll (void);
};

#endif

//******************************************************************************************

