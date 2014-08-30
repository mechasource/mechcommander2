//***************************************************************************
//
//	contact.h - This file contains the Contact Class header definitions
//
//	MechCommander 2 -- FASA Interactive Technologies
//
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#pragma once

#ifndef CONTACT_H
#define	CONTACT_H

//---------------------------------------------------------------------------

//#include <mclib.h>
//#include "dcontact.h"
//#include "dgameobj.h"
//#include "dmover.h"
//#include "dobjtype.h"
//#include "dwarrior.h"
//#include "dteam.h"
//#include "objmgr.h"

//***************************************************************************

class ContactInfo {

	public:

		uint8_t				contactStatus[MAX_TEAMS];
		//uint8_t				allContactStatus[MAX_TEAMS];
		uint8_t				contactCount[MAX_TEAMS];	//How many mechs/vehicles have me on sensors?
		uint8_t				sensors[MAX_SENSORS];		//index into sensor's contact list
		uint16_t				teams[MAX_TEAMS];			//index into team's contact list
		uint8_t				teamSpotter[MAX_TEAMS];

	public:

		PVOID operator new (size_t mySize);

		void operator delete (PVOID us);

		void destroy (void) {
		}

		void init (void) {
			int32_t i;
			for (i = 0; i < MAX_TEAMS; i++) {
				contactStatus[i] = CONTACT_NONE;
				//allContactStatus[i] = CONTACT_NONE;
				contactCount[i] = 0;
				teams[i] = 0;
				teamSpotter[i] = 0;
			}
			for (i = 0; i < MAX_SENSORS; i++)
				sensors[i] = 255;
		}

		ContactInfo (void) {
			init (void);
		}

		~ContactInfo (void) {
			destroy(void);
		}

		int32_t getContactStatus (int32_t teamId, bool includingAllies) {
			if (teamId == -1)
				return(CONTACT_NONE);
			return(contactStatus[teamId]);
		}

		void incContactCount (int32_t teamId) {
			contactCount[teamId]++;
		}

		void decContactCount (int32_t teamId) {
			contactCount[teamId]--;
		}

		int32_t getContactCount (int32_t teamId) {
			return(contactCount[teamId]);
		}

		void setSensor (int32_t sensor, int32_t contactIndex) {
			sensors[sensor] = contactIndex;
		}

		int32_t getSensor (int32_t sensor) {
			return(sensors[sensor]);
		}

		void setTeam (int32_t teamId, int32_t contactIndex) {
			teams[teamId] = contactIndex;
		}

		int32_t getTeam (int32_t teamId) {
			return(teams[teamId]);
		}
};

//---------------------------------------------------------------------------

typedef struct _ECMInfo {
	GameObjectPtr				owner;
	float						range;
} ECMInfo;

//---------------------------------------------------------------------------

class SensorSystem {

	public:

		int32_t					id;
		TeamSensorSystemPtr		master;
		int32_t					masterIndex;
		GameObjectPtr			owner;
		float					range;
		uint32_t					skill;
		bool					broken;
		bool					notShutdown;
		bool					hasLOSCapability;

		float					ecmEffect;

		float					nextScanUpdate;
		float					lastScanUpdate;

		uint16_t			contacts[MAX_CONTACTS_PER_SENSOR];
		int32_t					numContacts;
		int32_t					numExclusives;
		int32_t					totalContacts;

		SensorSystemPtr			prev;
		SensorSystemPtr			next;

		static int32_t				numSensors;
		static float			scanFrequency;
		static SortListPtr		sortList;

	public:

		PVOID operator new (size_t ourSize);
		
		void operator delete (PVOID us);
		
		void init (void);

		void destroy (void);

		void setMaster (TeamSensorSystemPtr newMaster);

		void setOwner (GameObjectPtr newOwner);

		void disable (void);

		bool enabled (void);

		void setShutdown (bool setting);

		void setMasterIndex (int32_t index) {
			masterIndex = index;
		}

		int32_t getMasterIndex (void) {
			return(masterIndex);
		}

		void setRange (float newRange);

		float getRange (void);

		float getEffectiveRange (void);

		void setSkill (uint32_t newSkill)
		{
			skill = newSkill;
		}

		int32_t getSkill (void) {
			return(skill);
		}

		int32_t getTotalContacts (void) {
			return(totalContacts);
		}

		void setNextScanUpdate (float when) {
			nextScanUpdate = when;
		}

		void setScanFrequency (float seconds) {
			scanFrequency = seconds;
		}

		SensorSystem (void) {
			init(void);
		}

		~SensorSystem (void) {
			destroy(void);
		}

		int32_t getSensorQuality (void);

		int32_t calcContactStatus (MoverPtr mover);

		bool isContact (MoverPtr mover);

		void addContact (MoverPtr mover, bool visual);

		void modifyContact (MoverPtr mover, bool visual);

		void removeContact (int32_t contactIndex);

		void removeContact (MoverPtr mover);

		void clearContacts (void);

		int32_t scanBattlefield (void);

		int32_t scanMover (Mover* mover);

		void updateContacts (void);

		void updateScan (bool forceUpdate = false);

		int32_t getTeamContacts (int32_t* contactList, int32_t contactCriteria, int32_t sortType);
		
		void setLOSCapability (bool flag)
		{
			hasLOSCapability = flag;
		}
};

//---------------------------------------------------------------------------


#define	MAX_SENSORS_PER_TEAM MAX_MOVERS

class TeamSensorSystem {

	public:

		int32_t				teamId;
		int32_t				nextContactId;
		int32_t				numContactUpdatesPerPass;
		int32_t				curContactUpdate;
		int32_t				contacts[MAX_MOVERS];
		//int32_t				allContacts[MAX_MOVERS];
		int32_t				numContacts;
		//int32_t				numAllContacts;
		int32_t				numEnemyContacts;
		SensorSystemPtr		sensors[MAX_SENSORS_PER_TEAM];
		int32_t				numSensors;
		SystemTrackerPtr	ecms;
		int32_t				numEcms;
		SystemTrackerPtr	jammers;
		int32_t				numJammers;

		static bool			homeTeamInContact;

	public:

		PVOID operator new (size_t ourSize);
		
		void operator delete (PVOID us);
		
		void init (void);

		void destroy (void);

		TeamSensorSystem (void) {
			init(void);
		}
		
		~TeamSensorSystem (void) {
			destroy(void);
		}

		void update (void);

		void setTeam (TeamPtr newTeam);

		void incNumEnemyContacts (void);

		void decNumEnemyContacts (void);

		void addContact (SensorSystemPtr sensor, MoverPtr contact, int32_t contactIndex, int32_t contactStatus);

		SensorSystemPtr findBestSpotter (MoverPtr contact, int32_t* status);

		void modifyContact (SensorSystemPtr sensor, MoverPtr contact, int32_t contactStatus);

		void removeContact (SensorSystemPtr sensor, MoverPtr contact);

		void addSensor (SensorSystemPtr sensor);

		void removeSensor (SensorSystemPtr sensor);

		int32_t getVisualContacts (MoverPtr* moverList);

		int32_t getSensorContacts (MoverPtr* moverList);

		bool hasSensorContact (int32_t teamID);

		int32_t getContacts (GameObjectPtr looker, int32_t* contactList, int32_t contactCriteria, int32_t sortType);

		int32_t getContactStatus (MoverPtr mover, bool includingAllies);

		bool meetsCriteria (GameObjectPtr looker, MoverPtr mover, int32_t contactCriteria);

		void scanBattlefield (void);

		void scanMover (Mover* mover);

		void addEcm (GameObjectPtr owner, float range);

		void updateEcmEffects (void);

		//void updateContactList (void);
};

//---------------------------------------------------------------------------

class SensorSystemManager {

	//-------------
	// Data Members
	
	protected:
		
		int32_t					freeSensors;			//How many sensors are currently free
		SensorSystemPtr*		sensorPool;				//Pool of ALL sensors
		SensorSystemPtr			freeList;				//List of available sensors
		TeamSensorSystemPtr		teamSensors[MAX_TEAMS];
		ECMInfo					ecms[MAX_ECMS];
		int32_t					numEcms;
		int32_t 					teamToUpdate;
		static float			updateFrequency;
		
	public:
		static bool				enemyInLOS;				//Flag is set every frame that I can see someone on sensors or visually.

	//-----------------
	// Member Functions
	
	public:

		PVOID operator new (size_t ourSize);
		void operator delete (PVOID us);
		
		SensorSystemManager (void) {
			init(void);
		}
		
		~SensorSystemManager (void) {
			destroy(void);
		}
		
		void destroy (void);
		
		void init (void) {
			freeSensors = 0;
			sensorPool = nullptr;
			freeList = nullptr;
			for (int32_t i = 0; i < MAX_TEAMS; i++)
				teamSensors[i] = nullptr;
			numEcms = 0;
			teamToUpdate = 0;
		}
		
		int32_t init (bool debug);

		TeamSensorSystemPtr getTeamSensor (int32_t teamId) {
			return(teamSensors[teamId]);
		}
		
		SensorSystemPtr newSensor (void);

		void freeSensor (SensorSystemPtr sensor);

		SensorSystemPtr getSensor (int32_t id) 
		{
			if ((id < 0) || (id >= MAX_SENSORS))
				STOP(("Tried to access Sensor outside of range.  Tried to access: %d",id));
				
			return(sensorPool[id]);
		}

		int32_t checkIntegrity (void);

		void addTeamSensor (int32_t teamId, SensorSystemPtr sensor);

		void removeTeamSensor (int32_t teamId, SensorSystemPtr sensor);

		void addEcm (GameObjectPtr owner, float range);

		float getEcmEffect (GameObjectPtr victim);

		void updateEcmEffects (void);

		void updateSensors (void);

		//void updateTeamContactLists (void);

		void update (void);
};

//---------------------------------------------------------------------------

extern SensorSystemManagerPtr		SensorManager;

//***************************************************************************

#endif
