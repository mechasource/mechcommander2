//---------------------------------------------------------------------------
//
//	gameobj.h -- File contains the Basic Game Object definition
//
//	MechCommander 2
//
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#pragma once

#ifndef GAMEOBJ_H
#define GAMEOBJ_H

//---------------------------------------------------------------------------
// Include Files

//#include <mclib.h>
//#include "dobjtype.h"
//#include "dappear.h"
//#include "dcontact.h"
//#include "dteam.h"
//#include "dwarrior.h"
//#include "mechclass.h"
//#include "dgameobj.h"
//#include "dcarnage.h"
//#include "move.h"
//#include <stuff/stuff.hpp>

extern float metersPerWorldUnit;
extern ObjectTypeManagerPtr objectTypeManager;

//---------------------------------------------------------------------------

#define	CELLS_PER_TILE					3

#define	GAMEOBJECT_FLOATS				4
#define	GAMEOBJECT_LONG					2
#define	GAMEOBJECT_BYTES				1

#define	RELPOS_FLAG_ABS					1
#define	RELPOS_FLAG_PASSABLE_START		2
#define	RELPOS_FLAG_PASSABLE_GOAL		4

typedef enum {
	ATTACKSOURCE_WEAPONFIRE,
	ATTACKSOURCE_COLLISION,
	ATTACKSOURCE_DFA,
	ATTACKSOURCE_MINE,
	ATTACKSOURCE_ARTILLERY,
	NUM_ATTACKSOURCES
} AttackSourceType;

typedef struct _WeaponShotInfo {
	GameObjectWatchID	attackerWID;
	int32_t				masterId;			// attack weapon master ID
	float				damage;				// damage caused by this shot
	int32_t				hitLocation;		// hit location on target
	float				entryAngle;			// angle from which target was hit

	void init (GameObjectWatchID _attackerWID, int32_t _masterId, float _damage, int32_t _hitLocation, float _entryAngle);
	
	void setDamage (float _damage);

	void setEntryAngle (float _entryAngle);

	void operator = (_WeaponShotInfo copy) {
		init(copy.attackerWID, copy.masterId, copy.damage, copy.hitLocation, copy.entryAngle);
	}
	
} WeaponShotInfo;

//---------------------------------------------------------------------------

typedef struct _SalvageItem {
	uint8_t		itemID;			// id from MasterComponentList;
	uint8_t		numItems;		// how many are there?
	uint8_t		numSalvagers;	// how many are salvagers are going for this item?
} SalavageItem;

//------------------------------------------------------------------------------------------

class WeaponFireChunk {

	public:
		
		char				targetType;
		int32_t				targetId;
		int32_t				targetCell[2];
		char				specialType;
		int32_t				specialId;
		uint8_t		weaponIndex;
		bool				hit;
		char				entryAngle;
		char				numMissiles;
		char				hitLocation;

		uint32_t		data;

	public:

		PVOID operator new (size_t mySize);

		void operator delete (PVOID us);
		
		void init (void) {
			targetType = 0;
			targetId = 0;
			targetCell[0] = 0;
			targetCell[1] = 0;
			specialType = -1;
			specialId = -1;
			weaponIndex = 0;
			hit = false;
			entryAngle = 0;
			numMissiles = 0;
			hitLocation = -1;
			data = 0;
		}

		void destroy (void) {
		}

		WeaponFireChunk (void) {
			init(void);
		}

		~WeaponFireChunk (void) {
			destroy(void);
		}

		void buildMoverTarget (GameObjectPtr target,
							   int32_t weaponIndex,
							   bool hit,
							   float entryAngle,
							   int32_t numMissiles,
							   int32_t hitLocation);

		void buildTerrainTarget (GameObjectPtr target,
								 int32_t _weaponIndex,
								 bool _hit,
								 int32_t _numMissiles);

		void buildCameraDroneTarget (GameObjectPtr target,
									 int32_t _weaponIndex,
									 bool _hit,
									 float _entryAngle,
									 int32_t _numMissiles);

		void buildLocationTarget (Stuff::Vector3D location,
								  int32_t weaponIndex,
								  bool hit,
								  int32_t numMissiles);

		void pack (GameObjectPtr attacker);

		void unpack (GameObjectPtr attacker);

		bool equalTo (WeaponFireChunkPtr chunk);
};

//------------------------------------------------------------------------------------------

class WeaponHitChunk {

	public:

		char				targetType;
		int32_t				targetId;
		int32_t				targetCell[2];
		char				specialType;
		int32_t				specialId;
		char				cause;
		float				damage;
		char				hitLocation;
		char				entryAngle;
		bool				refit;

		uint32_t		data;

	public:

		PVOID operator new (size_t mySize);

		void operator delete (PVOID us);
		
		void init (void) {
			targetType = 0;
			targetId = 0;
			targetCell[0] = 0;
			targetCell[1] = 0;
			specialType = -1;
			specialId = -1;
			cause = 0;
			damage = 0.0;
			hitLocation = -1;
			entryAngle = 0;
			refit = false;
			data = 0;
		}

		void destroy (void) {
		}

		WeaponHitChunk (void) {
			init(void);
		}

		~WeaponHitChunk (void) {
			destroy(void);
		}

		void buildMoverTarget (GameObjectPtr target,
							   int32_t cause,
							   float damage,
							   int32_t hitLocation,
							   float entryAngle,
							   bool isRefit);

		void buildTerrainTarget (GameObjectPtr target,
								 float damage);

		void buildCameraDroneTarget (GameObjectPtr target,
									 float _damage,
									 float _entryAngle);

		void build (GameObjectPtr target, WeaponShotInfoPtr shotInfo, bool isRefit = false);

		void pack (void);

		void unpack (void);

		bool equalTo (WeaponHitChunkPtr chunk);

		bool valid (int32_t from);
};
 
//------------------------------------------------------------------------------------------
typedef struct _GameObjectData
{
	int32_t						objectTypeNum;
	ObjectClass					objectClass;		
	GameObjectHandle			handle;				
	int32_t						partId;				
	uint32_t				watchID;			

	GameObjectTypeHandle		typeHandle;
	Stuff::Vector3D				position;			
	uint16_t				cellPositionRow;	
	uint16_t				cellPositionCol;
	int32_t						d_vertexNum;		
	uint32_t				flags;				
	uint16_t				debugFlags;			
	uint8_t				status;				

	float						tonnage;			
	float			   			rotation;			
	char						appearanceTypeID[256];
	GameObjectWatchID			collisionFreeFromWID;
	float						collisionFreeTime;
	Stuff::Vector4D				screenPos;			
	int32_t						windowsVisible;		
	float						explRadius;			
	float						explDamage;			
	int16_t						maxCV;
	int16_t						curCV;
	int16_t						threatRating;
	float						lastFrameTime;		
	uint8_t				blipFrame;
	uint8_t				numAttackers;

	int32_t						drawFlags;			
} GameObjectData;

class GameObject {

	public:

		ObjectClass					objectClass;		//What kind of object is this.
		GameObjectHandle			handle;				//Used to reference into master obj table
		int32_t						partId;				//What is my unique part number.
		uint32_t				watchID;			//Used to reference in the game engine

		GameObjectTypeHandle		typeHandle;			//Who made me?
		Stuff::Vector3D				position;			//Where am I?
		uint16_t				cellPositionRow;	//Cell RC position
		uint16_t				cellPositionCol;
		int32_t						d_vertexNum;		//Physical Vertex in mapData array that I'm lower right from
		uint32_t				flags;				//See GAMEOBJECT_FLAGS_ defines
		uint16_t				debugFlags;			// use ONLY for debugging purposes...
		uint8_t				status;				//Am I normal, disabled, destroyed, etc..?
	
		float						tonnage;			//How hefty am I?
		float			   			rotation;			//everything's base facing
		AppearancePtr				appearance;
		GameObjectWatchID			collisionFreeFromWID;	//Index into GameObject Table
		float						collisionFreeTime;
		Stuff::Vector4D				screenPos;			//Actual Screen position
		int32_t						windowsVisible;		//Which Windows can see me.
		float						explRadius;			//How big is my explosion.
		float						explDamage;			//How much damage does it do?
		int16_t						maxCV;
		int16_t						curCV;
		int16_t						threatRating;
		float						lastFrameTime;		//Time elapsed since last frame was drawn.  (Replaces HEAT.  No net gain in size!)
		uint8_t				blipFrame;
		uint8_t				numAttackers;

		int32_t						drawFlags;			// bars, text, brackets, and highlight colors

		static uint32_t		spanMask;			//Used to preserve tile's LOS
		static float				blockCaptureRange;
		static bool					initialize;

	public:

		PVOID operator new(size_t ourSize);
		void operator delete(PVOID us);
		virtual void set(GameObject copy);

		GameObject (void)
		{
			init(true);
		}
		virtual ~GameObject (void)
		{
			destroy(void);
		}
		virtual void init (bool create);
		virtual void destroy(void);
		
		ObjectClass getObjectClass (void) {
			return(objectClass);
		}

		virtual int32_t update (void) {
			return(NO_ERROR);
		}

		virtual void render (void) {
		}
		
		virtual void renderShadows (void) {
		}

		virtual void updateDebugWindow (GameDebugWindow* /*debugWindow*/)
		{
		}

		virtual AppearancePtr getAppearance (void) {
			return(appearance);
		}
		
		virtual bool underPlayerControl (void) {
			return (false);
		}
		
		virtual int32_t getGroupId (void) {
			return(-1);
		}

		virtual void setPartId (int32_t newPartId) {
			partId = newPartId;
		}
		
		int32_t getPartId (void) {
			return(partId);
		}

		virtual void setHandle (GameObjectHandle newHandle) {
			handle = newHandle;
		}

		GameObjectHandle getHandle (void) {
			return(handle);
		}

		uint32_t getWatchID (bool assign = true);

		virtual PSTR getName (void) {
			return(nullptr);
		}

		virtual float getStatusRating (void) {
			return(0.0);
		}

		virtual float getDestructLevel (void)
		{
			return 5000.0f;		//If we somehow miss a object class, KEEP SHOOTING!!!!
		}

		void getCellPosition (int32_t& cellRow, int32_t& cellCol) {
			cellRow = cellPositionRow;
			cellCol = cellPositionCol;
		}

		virtual void handleStaticCollision (void) {
		}
		
		virtual void getBlockAndVertexNumber (int32_t &blockNum, int32_t &vertexNum);

		virtual int32_t getTypeHandle (void) {
			return(typeHandle);
		}

		virtual ObjectTypePtr getObjectType (void);
		
		virtual void init (bool create, ObjectTypePtr _type);

		virtual int32_t init (FitIniFile* /*objProfile*/)
		{
			return(NO_ERROR);
		}

		bool isMover (void)
		{
			return((objectClass == BATTLEMECH) || (objectClass == GROUNDVEHICLE) || (objectClass == ELEMENTAL) || (objectClass == MOVER));
		}

		bool isMech (void) {
			return((objectClass == BATTLEMECH));
		}

		virtual int32_t calcHitLocation (GameObjectPtr /*attacker*/, int32_t /*weaponIndex*/, int32_t /*attackSource*/, int32_t /*attackType*/)
		{
			return(-1);
		}

		virtual int32_t handleWeaponHit (WeaponShotInfoPtr /*shotInfo*/, bool addMultiplayChunk = false)
		{
			addMultiplayChunk;
			return(NO_ERROR);
		}

		virtual void setFireHandle (GameObjectHandle /*handle*/) 
		{
		}

		virtual void killFire (void) {
		}

		virtual float getAppearRadius (void)
		{
			return 0.0f;
		}
		
		virtual int32_t getTeamId (void) {
			return(-1);
		}

		virtual int32_t getVertexNum (void)
		{
			return d_vertexNum;
		}
		
		virtual int32_t setTeamId (int32_t /*_teamId*/, bool /*setup*/)
		{
			return(NO_ERROR);
		}

		virtual TeamPtr getTeam (void)
		{
			return(nullptr);
		}

		virtual int32_t setTeam (TeamPtr /*_team*/)
		{
			return(NO_ERROR);
		}

		virtual bool isFriendly (TeamPtr /*team*/)
		{
			return(false);
		}

		virtual bool isEnemy (TeamPtr /*team*/)
		{
			return(false);
		}

		virtual bool isNeutral (TeamPtr /*team*/)
		{
			return(true);
		}

		virtual Stuff::Vector3D getPosition (void)
		{
			return(position);
		}

		virtual Stuff::Vector3D getLOSPosition (void)
		{
			return(position);
		}

		virtual Stuff::Vector3D relativePosition (float angle, float distance, uint32_t flags);
		
		virtual Stuff::Vector3D getPositionFromHS (int32_t /*weaponType*/) 
		{
			//-----------------------------------------
			// No hot spots with regular game objects.
			// just return position.
			return(position);
		}
		
		virtual void setPosition (const Stuff::Vector3D& newPosition, bool calcPositions = true);
		
		virtual void setTerrainPosition (const Stuff::Vector3D& /*position*/, 
			const Stuff::Vector2DOf<int32_t>& /*numbers*/)
		{
		}

		virtual Stuff::Vector3D getVelocity (void) 
		{
			Stuff::Vector3D result;
			result.Zero(void);
			return(result);
		}
		
		virtual Stuff::Vector4D getScreenPos (int32_t /*whichOne*/)
		{
			return(screenPos);
		}
		
		virtual void setVelocity (Stuff::Vector3D& /*newVelocity*/)
		{
		}

		virtual float getSpeed (void)
		{
			return(0.0);
		}

		virtual int32_t getMoveLevel (void)
		{
			return(0);
		}

		virtual float getRotation (void) 
		{
			return(rotation);
		}

		virtual void setRotation (float rot) 
		{
			rotation = rot;
		}

		virtual void rotate (float /*angle*/)
		{
		}

		virtual void rotate (float /*yaw*/, float /*pitch*/)
		{
		}
		
		virtual Stuff::Vector3D getRotationVector (void) 
		{
			Stuff::Vector3D rotationVec;
			rotationVec.x = 0.0f;
			rotationVec.y = -1.0f;
			rotationVec.z = 0.0f;
			Rotate(rotationVec, -rotation);
			return(rotationVec);
		}
		
		virtual bool calcAdjacentAreaCell (int32_t /*moveLevel*/, int32_t /*areaID*/, int32_t& /*adjRow*/, int32_t& /*adjCol*/)
		{
			return(false);
		}

		uint8_t getStatus (void) 
		{
			return(status);
		}

		//NEVER call this with forceStatus UNLESS you are recovering a mech!!!
		void setStatus (int32_t newStatus, bool forceStatus = false) 
		{
			if (((status != OBJECT_STATUS_DESTROYED) && (status != OBJECT_STATUS_DISABLED)) || forceStatus)
				status = (uint8_t)newStatus;

			if (newStatus == OBJECT_STATUS_DESTROYED)
				status = (uint8_t)newStatus;
		}

		virtual bool isCrippled (void)
		{
			return(false);
		}

		virtual bool isDisabled (void)
		{
			return((status == OBJECT_STATUS_DISABLED) || (status == OBJECT_STATUS_DESTROYED));
		}

		virtual bool isDestroyed (void)
		{
			return(status == OBJECT_STATUS_DESTROYED);
		}

		virtual float getDamage (void)
		{
			return(0.0);
		}

		virtual void setDamage (float /*newDamage*/)
		{
		}

		virtual float getDamageLevel (void)
		{
			return(0.0);
		}

		virtual int32_t getContacts (int32_t* /*contactList*/, int32_t /*contactCriteria*/, int32_t /*sortType*/)
		{
			return(0);
		}

		bool getTangible (void)
		{
			return((flags & OBJECT_FLAG_TANGIBLE) != 0);
		}
		
		void setTangible (bool set)
		{
			if (set)
				flags |= OBJECT_FLAG_TANGIBLE;
			else
				flags &= (OBJECT_FLAG_TANGIBLE ^ 0xFFFFFFFF);
		}
		
		virtual void setCommanderId (int32_t /*_commanderId*/)
		{
		}

		virtual MechWarriorPtr getPilot (void)
		{
			return(nullptr);
		}

		virtual int32_t getCommanderId (void)
		{
			return(-1);
		}


		virtual int32_t write (FilePtr /*objFile*/)
		{
			return NO_ERROR;
		}
		
		virtual float distanceFrom (Stuff::Vector3D goal);

		virtual int32_t cellDistanceFrom (Stuff::Vector3D goal);

		virtual int32_t cellDistanceFrom (GameObjectPtr obj);

		virtual void calcLineOfSightNodes (void)
		{
		}

		virtual int32_t getLineOfSightNodes (int32_t eyeCellRow, int32_t eyeCellCol, int32_t* cells);

		virtual bool lineOfSight (int32_t cellRow, int32_t cellCol, bool checkVisibleBits = true);

		virtual bool lineOfSight (Stuff::Vector3D point, bool checkVisibleBits = true);

		virtual bool lineOfSight (GameObjectPtr target, float startExtRad = 0.0f, bool checkVisibleBits = true);
	
		virtual float relFacingTo (Stuff::Vector3D goal, int32_t bodyLocation = -1);

		virtual float relViewFacingTo (Stuff::Vector3D goal) 
		{
			return(GameObject::relFacingTo(goal));
		}

		virtual int32_t openStatusWindow (int32_t /*x*/, int32_t /*y*/, int32_t /*w*/, int32_t /*h*/) 
		{
			return(NO_ERROR);
		}

		virtual int32_t closeStatusWindow (void) 
		{
			return(NO_ERROR);
		}

		virtual int32_t getMoveState (void)
		{
			return 0;
		}

		virtual void orderWithdraw (void) {
			//Does nothing until this is a mover.
		}
		
		virtual bool isWithdrawing (void) {
			return(false);
		}
		
		virtual float getExtentRadius (void);

		virtual void setExtentRadius (float newRadius);
		
		virtual bool isBuilding(void) {
			return(false);
		}

		virtual bool isTerrainObject (void) {
			return(false);
		}

		virtual bool inTransport(void) {
			return(false);
		}

		virtual bool isCaptureable (int32_t /*capturingTeamID*/)
		{
			return(false);
		}

		virtual bool canBeCaptured (void)
		{
			return false;
		}

		virtual bool isPrison(void) {
			return(false);
		}

		virtual bool isPowerSource(void)
		{
			return false;
		}
		
		virtual bool isSpecialBuilding(void)
		{
			return false;
		}
		
 		virtual bool isLit (void)
		{
			return false;
		}

		virtual void setPowerSupply (GameObjectPtr /*power*/)
		{
		
		}
		
		//----------------------
		// DEBUG FLAGS functions

		virtual void setDebugFlag (uint16_t flag, bool set) {
			if (set)
				debugFlags |= flag;
			else
				debugFlags &= (flag ^ 0xFFFF);
		}

		virtual bool getDebugFlag (uint16_t flag) {
			return((debugFlags & flag) != 0);
		}

		//---------------
		// FLAG functions

		virtual void setFlag (uint32_t flag, bool set) {
			if (set)
				flags |= flag;
			else
				flags &= (flag ^ 0xFFFFFFFF);
		}

		virtual bool getFlag (uint32_t flag) {
			return((flags & flag) != 0);
		}

		virtual void initFlags (void) {
			flags = OBJECT_FLAG_USEME | OBJECT_FLAG_AWAKE;
		}

		virtual void setSelected (bool set) {
			if (set)
				flags |= OBJECT_FLAG_SELECTED;
			else
				flags &= (OBJECT_FLAG_SELECTED ^ 0xFFFFFFFF);

			setDrawBars( set );
			setDrawBrackets( set );
		}

		virtual bool getSelected(void) {
			return ((flags & OBJECT_FLAG_SELECTED) != 0);
		}
		
		virtual bool isSelected (void) {
			return (getSelected());
		}

		virtual bool isSelectable()
		{
			return true;
		}

		virtual void setTargeted (bool set) {
			if (set)
				flags |= OBJECT_FLAG_TARGETED;
			else
				flags &= (OBJECT_FLAG_TARGETED ^ 0xFFFFFFFF);

			setDrawBars( set );
			setDrawText( set );
			setDrawColored( set );

			if ( isSelected() )
				setSelected( true ); // reset flags

			setDrawColored( set );

		}

		virtual bool getTargeted(void) {
			return ((flags & OBJECT_FLAG_TARGETED) != 0);
		}

		virtual void setDrawNormal()
		{
			drawFlags = DRAW_NORMAL;
		}

		virtual void setDrawText( bool set )
		{
			if ( set )
				drawFlags |= DRAW_TEXT;
			else
				drawFlags &=( DRAW_TEXT ^ 0xffffffff );
		}

		virtual int32_t getDrawText()
		{
			return drawFlags & DRAW_TEXT;
		}

		virtual void setDrawBars( bool set )
		{
			if ( set )
				drawFlags |= DRAW_BARS;
			else
				drawFlags &=( DRAW_BARS ^ 0xffffffff );
		}

		virtual int32_t getDrawBars()
		{
			return drawFlags & DRAW_BARS;
		}

		virtual void setDrawBrackets( bool set )
		{
			if ( set )
				drawFlags |= DRAW_BRACKETS;
			else
				drawFlags &=( DRAW_BRACKETS ^ 0xffffffff );
		}

		virtual int32_t getDrawBrackets()
		{
			return drawFlags & DRAW_BRACKETS;
		}

		virtual void setDrawColored( bool set )
		{
			if ( set )
				drawFlags |= DRAW_COLORED;
			else
				drawFlags &=( DRAW_COLORED ^ 0xffffffff );
		}

		virtual int32_t getDrawColored()
		{
			return drawFlags & DRAW_COLORED;
		}
		
		virtual void setObscured (bool set) {
			if (set)
				flags |= OBJECT_FLAG_OBSCURED;
			else
				flags &= (OBJECT_FLAG_OBSCURED ^ 0xFFFFFFFF);
		}

		virtual bool getObscured (void) {
			return ((flags & OBJECT_FLAG_OBSCURED) != 0);
		}

		virtual bool isTargeted (void) {
			return(getTargeted());
		}

		virtual void setExists (bool set) {
			if (set)
				flags |= OBJECT_FLAG_EXISTS;
			else
				flags &= (OBJECT_FLAG_EXISTS ^ 0xFFFFFFFF);
		}

		virtual bool getExists(void) {
			return ((flags & OBJECT_FLAG_EXISTS) != 0);
		}

		virtual void setAwake (bool set) {
			if (set)
				flags |= OBJECT_FLAG_AWAKE;
			else
				flags &= (OBJECT_FLAG_AWAKE ^ 0xFFFFFFFF);
		}

		virtual bool getAwake (void) {
			return ((flags & OBJECT_FLAG_AWAKE) != 0);
		}

		virtual bool getExistsAndAwake (void) {
			return(getExists() && getAwake());
		}

		virtual void setUseMe (bool set) {
			if (set)
				flags |= OBJECT_FLAG_USEME;
			else
				flags &= (OBJECT_FLAG_USEME ^ 0xFFFFFFFF);
		}

		virtual bool getUseMe (void) {
			return ((flags & OBJECT_FLAG_USEME) != 0);
		}

		virtual void setCaptured (bool set) {
			if (set)
				flags |= OBJECT_FLAG_CAPTURED;
			else
				flags &= (OBJECT_FLAG_CAPTURED ^ 0xFFFFFFFF);
		}

		virtual bool getCaptured (void) {
			return ((flags & OBJECT_FLAG_CAPTURED) != 0);
		}

		virtual void clearCaptured(void) {
			setCaptured(false);
		}

		virtual bool isCaptured (void) {
			return(getCaptured());
		}
		
		virtual void setTonnage (float _tonnage) {
			tonnage = _tonnage;
		}

		virtual float getTonnage (void) {
			return(tonnage);
		}

		virtual void setCollisionFreeFromWID (GameObjectWatchID objWID) {
			collisionFreeFromWID = objWID;
		}

		virtual GameObjectWatchID getCollisionFreeFromWID (void)
		{
			return(collisionFreeFromWID);
		}

#ifdef USE_COLLISION
		virtual GameObjectHandle getCollisionFreeFromObject (void)
		{
			return(...);
		}
#endif

		virtual void setCollisionFreeTime (float time)
		{
			collisionFreeTime = time;
		}

		virtual float getCollisionFreeTime (void)
		{
			return(collisionFreeTime);
		}

		virtual void damageObject (float /*dmgAmount*/)
		{
			//damage += dmgAmount;
		}
		
		virtual void setExplDmg (float newDmg)
		{
			explDamage = newDmg;
		}
		
		virtual void setExplRad (float newRad)
		{
			explRadius = newRad;
		}
		
		virtual float getExplDmg (void) 
		{
			return(explDamage);
		}

		virtual void setSensorRange (float /*range*/)
		{
		}

		virtual bool hasActiveProbe (void)
		{
			return(false);
		}

		virtual float getEcmRange (void)
		{
			return(0.0);
		}

		virtual bool hasNullSignature (void)
		{
			return(false);
		}

		virtual void setSalvage (SalvageItemPtr /*newSalvage*/)
		{
		}
				
		virtual SalvageItemPtr getSalvage (void)
		{
			return(nullptr);
		}
		
		virtual int32_t getWindowsVisible (void)
		{
			return(windowsVisible);
		}
		
		virtual int32_t getCaptureBlocker (GameObjectPtr capturingMover, GameObjectPtr* blockerList = nullptr);

		virtual int32_t kill (void);

		virtual bool isMarine(void)
		{
			return(false);
		}

		virtual float getRefitPoints(void)
		{
			return(0.0);
		}

		virtual bool burnRefitPoints(float /*pointsToBurn*/)
		{
			return(false);
		}

		virtual float getRecoverPoints(void)
		{
			return(0.0);
		}

		virtual bool burnRecoverPoints(float /*pointsToBurn*/)
		{
			return(false);
		}

		virtual int32_t getCurCV (void) {
			return(curCV);
		}

		virtual int32_t getMaxCV (void) {
			return(maxCV);
		}
		
		virtual void setCurCV (int32_t newCV)
		{
			curCV = (int16_t)newCV;
		}

		virtual int32_t getThreatRating (void) {
			return(threatRating);
		}

		virtual void setThreatRating (int16_t rating) {
			threatRating = rating;
		}

		virtual void incrementAttackers(void) {
			numAttackers++;
		}

		virtual void decrementAttackers(void) {
			Assert(numAttackers > 0, numAttackers, " GameObject.decrementAttackers: neg ");
			numAttackers--;
		}

		virtual int32_t getNumAttackers(void) {
			return(numAttackers);
		}

		virtual bool onScreen (void);

		virtual MechClass getMechClass(void);

#if 0

		virtual void setSalvage (SalvageItemPtr newSalvage)
		{
			salvage = newSalvage;
		}
		
		virtual SalvageItemPtr getSalvage (void)
		{
			return salvage;
		}
	
#endif
		virtual bool isFriendly (GameObjectPtr obj);

		virtual bool isEnemy (GameObjectPtr obj);

		virtual bool isNeutral (GameObjectPtr obj);

		virtual bool isLinked (void)
		{
			return false;
		}

		virtual GameObjectPtr getParent (void)
		{
			return nullptr;
		}

		virtual void setParentId (uint32_t /*pId*/)
		{
			//Do Nothing.  Most of the time, this is OK!
		}

		virtual SensorSystem* getSensorSystem(){ return nullptr; }

		static void setInitialize (bool setting) {
			initialize = setting;
		}

		virtual int32_t getDescription(){ return -1; }
		
		virtual bool isOnGUI (void)
		{
			return false;
		}
		
		virtual void setOnGUI (bool /*onGui*/)
		{
		}
		
		virtual float getLOSFactor (void)
		{
			return 1.0f;
		}
		
		virtual bool isLookoutTower (void)
		{
			return false;
		}
		
		virtual void Save (PacketFilePtr file, int32_t packetNum);

		void Load (GameObjectData *data);

		void CopyTo (GameObjectData *data);

		virtual void repairAll (void)
		{
		}
};

//---------------------------------------------------------------------------

#endif










