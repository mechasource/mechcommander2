//---------------------------------------------------------------------------
//
//	ObjType.h -- File contains the Basic Game Object Type definition
//
//	MechCommander 2
//
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#pragma once

#ifndef OBJTYPE_H
#define OBJTYPE_H

//---------------------------------------------------------------------------

//#include <mclib.h>
//#include "dobjtype.h"
//#include "dgameobj.h"
//#include <stuff/stuff.hpp>

#define MAX_NAME		25

//---------------------------------------------------------------------------
// Classes

class ObjectType {

	protected:
	
		ObjectTypeNumber		objTypeNum;				//What exactly am I?
		int32_t					numUsers;				//How many people love me?
		int32_t					objectTypeClass;		//What type am I?
		ObjectClass				objectClass;			//What object class am i?
		ObjectTypeNumber		destroyedObject;		//What I turn into when I die.
		ObjectTypeNumber		explosionObject;		//How I blow up
		bool					potentialContact;		//Can I can be a contact?
		char					*appearName;			//Base Name of appearance Files.
		float					extentRadius;			//Smallest sphere which will hold me.
		bool					keepMe;					//Do not EVER cache this objType out.
		int32_t					iconNumber;				//my index into the big strip o' icons
		int32_t					teamId;					//DEfault for this type
		uint8_t			subType;				//if building, what type of building? etc.

	public:

		PVOID operator new (size_t ourSize);
		void operator delete (PVOID us);
			
		void init (void) {
			objectClass = INVALID;
			objectTypeClass = -1;			//This is an invalid_object
			destroyedObject = -1;
			explosionObject = -1;
			potentialContact = false;

			extentRadius = 0;				//Nothing can hit me if this is zero.
			
			keepMe = false;
			
			iconNumber = -1;				//defaults to no icon

			appearName = NULL;
			subType = 0;
		}
		
		ObjectType (void) {
			init(void);
		}
		
		virtual int32_t init (FilePtr objFile, uint32_t fileSize);

		int32_t init (FitIniFilePtr objFile);
		
		virtual ~ObjectType (void) {
			destroy(void);
		}
		
		virtual void destroy (void);
		
		virtual GameObjectPtr createInstance (void);
		
		void addUser (void) {
			numUsers++;
		}
		
		void removeUser (void) {
			numUsers--;
		}
		
		void noMoreUsers (void) {
			numUsers = 0;
		}

		bool inUse (void) {
			return (numUsers > 0);
		}

		bool lovable (void) {
			return keepMe;
		}
		
		void makeLovable (void) {
			keepMe = true;
		}
		
		ObjectTypeNumber whatAmI (void) {
			return(objTypeNum);
		}

		PSTR  getAppearanceTypeName (void) 
		{
			return(appearName);
		}
			
		bool getPotentialContact (void) {
			return(potentialContact);
		}

		int32_t getObjectTypeClass (void) {
			return(objectTypeClass);
		}

		ObjectClass getObjectClass (void) {
			return(objectClass);
		}

		ObjectTypeNumber getDestroyedObject (void) {
			return(destroyedObject);
		}
		
		ObjectTypeNumber getExplosionObject (void) {
			return(explosionObject);
		}
		
		float getExtentRadius (void) {
			return(extentRadius);
		}

		void setExtentRadius (float newRadius) {
			extentRadius = newRadius;
		}
		
		ObjectTypeNumber getObjTypeNum (void) {
			return(objTypeNum);
		}

		void setObjTypeNum (ObjectTypeNumber objTNum) {
			objTypeNum = objTNum;
		}

		void setIconNumber(int32_t newNumber) {
			iconNumber = newNumber;
		}

		int32_t getIconNumber(void) {
			return iconNumber;
		}
						
		int32_t getTeamId (void) {
			return teamId;
		}

		void setSubType (uint8_t type) {
			subType = type;
		}

		uint8_t getSubType (void) {
			return(subType);
		}

		virtual bool handleCollision (GameObjectPtr collidee, GameObjectPtr collider);
		
		virtual bool handleDestruction (GameObjectPtr collidee, GameObjectPtr collider);

		virtual float getBurnTime (void) {
			return(0.0);
		}

		void createExplosion (Stuff::Vector3D &position, float dmg = 0.0, float rad = 0.0);
};

//---------------------------------------------------------------------------

class ObjectTypeManager {

	public:

		int32_t					numObjectTypes;
		ObjectTypePtr*			table;

		static UserHeapPtr		objectTypeCache;
		static UserHeapPtr		objectCache;
		static PacketFilePtr	objectFile;

		//--------------------------------------------------------
		// Following is done to maintain compatibility with MC1...
		static int32_t				bridgeTypeHandle;
		static int32_t				forestTypeHandle;
		static int32_t				wallHeavyTypeHandle;
		static int32_t				wallMediumTypeHandle;
		static int32_t				wallLightTypeHandle;
			
	public:

		void init (void) {
		}
			
		ObjectTypeManager (void) {
			init(void);
		}

		int32_t init (PSTR objectFileName, int32_t objectTypeCacheSize, int32_t objectCacheSize, int32_t maxObjectTypes = 1024);
			
		void destroy (void);
							
		~ObjectTypeManager (void) {
			destroy(void);
		}

		void remove (int32_t objTypeNum);

		void remove (ObjectTypePtr ptr);
			
		ObjectTypePtr load (ObjectTypeNumber objTypeNum, bool noCacheOut = true, bool forceLoad = false);

		ObjectTypePtr get (ObjectTypeNumber objTypeNum, bool loadIt = true);

		GameObjectPtr create (ObjectTypeNumber objTypeNum);
};

//---------------------------------------------------------------------------
#endif
