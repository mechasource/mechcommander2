//---------------------------------------------------------------------------
//
// gate.h -- File contains the Gate Object Class
//
//	MechCommander 2
//
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#pragma once

#ifndef GATE_H
#define GATE_H
//---------------------------------------------------------------------------
// Include Files

//#include "dgate.h"
//#include "objtype.h"
//#include "terrobj.h"
//#include "dmover.h"
					
//---------------------------------------------------------------------------
// Macro Definitions

//---------------------------------------------------------------------------
class GateType : public ObjectType
{
	//Data Members
	//-------------
	protected:
		uint32_t	dmgLevel;

	public:
		uint32_t	blownEffectId;
		uint32_t	normalEffectId;
		uint32_t	damageEffectId;
	
		float			baseTonnage;
		
		int32_t			basePixelOffsetX;
		int32_t			basePixelOffsetY;
		
		float			explDmg;
		float			explRad;

		float			openRadius;
		
		float			littleExtent;
				
		int32_t			gateTypeName;

		bool			blocksLineOfFire;

		int32_t			buildingDescriptionID;

	//Member Functions
	//-----------------
	public:

		void init (void)
		{
			ObjectType::init();
			
			dmgLevel = 0;

			blownEffectId = 0xFFFFFFFF;
			normalEffectId = 0xFFFFFFFF;
			damageEffectId = 0xFFFFFFFF;
		
			explDmg = explRad = 0.0;
			baseTonnage = 0.0;
			
			gateTypeName = 0;
			
			objectClass = GATE;
		}
		
		GateType (void)
		{
			init();
		}
		
		virtual int32_t init (FilePtr objFile, uint32_t fileSize);
		int32_t init (FitIniFilePtr objFile);
		
		~GateType (void)
		{
			destroy();
		}
		
		int32_t getDamageLvl (void)
		{
			return dmgLevel;
		}
			
		virtual void destroy (void);
		
		virtual GameObjectPtr createInstance (void);
		
		virtual bool handleCollision (GameObjectPtr collidee, GameObjectPtr collider);
		virtual bool handleDestruction (GameObjectPtr collidee, GameObjectPtr collider);
};

//---------------------------------------------------------------------------
typedef struct _GateData : public TerrainObjectData
{
	char					teamId;

	bool					lockedOpen;
	bool					lockedClose;
	bool					reasonToOpen;
	bool					opened;
	bool					opening;
	bool					closed;
	bool					closing;
	bool					justDestroyed;

	bool					lastMarkedOpen;

	MoverPtr				closestObject;

	uint32_t					parentId;
	GameObjectWatchID		parent;
	int32_t					buildingDescriptionID;

	int32_t					updatedTurn;
} GateData;

class Gate : public TerrainObject
{
	//Data Members
	//-------------
		public:
			char					teamId;
			
			bool					lockedOpen;
			bool					lockedClose;
			bool					reasonToOpen;
			bool					opened;
			bool					opening;
			bool					closed;
			bool					closing;
			bool					justDestroyed;

			bool					lastMarkedOpen;

			MoverPtr				closestObject;
			
			uint32_t					parentId;
			GameObjectWatchID		parent;
			int32_t					buildingDescriptionID;

			int32_t					updatedTurn;

	//Member Functions
	//-----------------
		public:

		void init (bool create)
		{
		}

	   	Gate (void) : TerrainObject()
		{
			init (true);
			reasonToOpen = TRUE;
			
			lockedClose = FALSE;
			lockedOpen = FALSE;
			closed = TRUE;
			closing = opening = opened = FALSE;
			justDestroyed = FALSE;
			lastMarkedOpen = false;

			closestObject = NULL;
			
			parentId = 0xffffffff;
			parent = 0;

			updatedTurn = -1;
		}

		~Gate (void)
		{
			destroy();
		}

		virtual void destroy (void);
		
		virtual int32_t update (void);
		virtual void render (void);
		
		virtual void init (bool create, ObjectTypePtr _type);

		virtual int32_t handleWeaponHit (WeaponShotInfoPtr shotInfo, bool addMultiplayChunk = FALSE);

		virtual int32_t setTeamId (int32_t _teamId, bool setup);
		
		virtual int32_t getTeamId (void) {
			return(teamId);
		}

		TeamPtr getTeam (void); 

		void lightOnFire (float timeToBurn);
			
		virtual float getDestructLevel (void)
		{
			return ((GateTypePtr)getObjectType())->getDamageLvl() - damage;
		}

		virtual int32_t kill (void)
		{
			//Do nothing for now.  Later, Buildings may do something.
			return NO_ERROR;
		}

		bool isVisible (CameraPtr camera);

		void blowAnyOffendingObject (void);

		virtual bool isBuilding(void)
		{
			return (TRUE);
		}

		virtual void getBlockAndVertexNumber (int32_t &blockNum, int32_t &vertexNum)
		{
			blockNum = blockNumber;
			vertexNum = vertexNumber;
		}
		
		void openGate (void);
		
		void setLockedOpen()
		{
			lockedOpen = TRUE;
			lockedClose = FALSE;
		}
		
		void setLockedClose()
		{
			lockedOpen = FALSE;
			lockedClose = TRUE;
		}

		void releaseLocks()
		{
			lockedOpen = FALSE;
			lockedClose = FALSE;
		}

		float getLittleExtent (void);

		void destroyGate(void);
		
		virtual bool isLinked (void);

		virtual GameObjectPtr getParent (void);

		virtual void setParentId (uint32_t pId);

		virtual int32_t getDescription(){ return ((GateType*)getObjectType())->buildingDescriptionID; }
		
		virtual void setDamage (float newDamage);

		virtual void Save (PacketFilePtr file, int32_t packetNum);

		void CopyTo (GateData *data);

		void Load (GateData *data);
};

//---------------------------------------------------------------------------
#endif




