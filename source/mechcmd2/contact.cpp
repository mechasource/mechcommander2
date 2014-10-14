//***************************************************************************
//
//	contact.cpp - This file contains the Contact and Sensor Classes code
//
//	MechCommander 2
//
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#include "stdafx.h"

#ifndef MCLIB_H
#include <mclib.h>
#endif

#ifndef WARRIOR_H
#include "warrior.h"
#endif

#ifndef CONTACT_H
#include "contact.h"
#endif

#ifndef MOVER_H
#include "mover.h"
#endif

//#ifndef GVEHICL_H
//#include "gvehicl.h"
//#endif

#ifndef TEAM_H
#include "team.h"
#endif

#ifndef UNITDESG_H
#include "unitdesg.h"
#endif

#ifndef SOUNDSYS_H
#include "soundsys.h"
#endif

#include <gameos.hpp>

//***************************************************************************

SensorSystemManagerPtr		SensorManager = nullptr;

int32_t						SensorSystem::numSensors = 0;
SortListPtr					SensorSystem::sortList = nullptr;
float						SensorSystem::scanFrequency = 0.5;

bool						TeamSensorSystem::homeTeamInContact = false;
bool						SensorSystemManager::enemyInLOS = true;

extern float scenarioTime;
extern UserHeapPtr missionHeap;

#define	VISUAL_CONTACT_FLAG	0x8000

//***************************************************************************
// CONTACT INFO class
//***************************************************************************

PVOID ContactInfo::operator new(size_t ourSize)
{
	PVOID result;
	result = missionHeap->Malloc(ourSize);
	return(result);
}

//---------------------------------------------------------------------------

void ContactInfo::operator delete(PVOID us)
{
	missionHeap->Free(us);
}

//***************************************************************************
// SENSOR SYSTEM routines
//***************************************************************************

PVOID SensorSystem::operator new(size_t mySize)
{
	PVOID result = missionHeap->Malloc(mySize);
	return(result);
}

//---------------------------------------------------------------------------

void SensorSystem::operator delete(PVOID us)
{
	missionHeap->Free(us);
}

//---------------------------------------------------------------------------

void SensorSystem::init(void)
{
	//id = 0			// this should be set by the sensor system manager only!
	master = nullptr;
	masterIndex = -1;
	owner = nullptr;
	range = -1.0;
	skill = -1;
	broken = false;
	notShutdown = true;
	ecmEffect = 1.0;
	//ALWAYS true UNLESS this is a sensor probe or a sensor tower!!
	hasLOSCapability = true;
	//----------------------------------------------------
	// Don't update any sensors on the very first frame...
	nextScanUpdate = 0.5;
	lastScanUpdate = 0.0;
	numContacts = 0;
	id = numSensors++;
	numExclusives = 0;
	totalContacts = 0;
	if(!sortList)
	{
		sortList = new SortList;
		if(!sortList)
			Fatal(0, " Unable to create Contact::sortList ");
		sortList->init(MAX_CONTACTS_PER_SENSOR);
	}
}

//---------------------------------------------------------------------------

void SensorSystem::destroy(void)
{
	numSensors--;
	if(numSensors == 0)
	{
		delete sortList;
		sortList = nullptr;
	}
}

//---------------------------------------------------------------------------

void SensorSystem::setMaster(TeamSensorSystemPtr newMaster)
{
	master = newMaster;
	broken = false;
}

//---------------------------------------------------------------------------

void SensorSystem::setOwner(GameObjectPtr newOwner)
{
	owner = newOwner;
}

//---------------------------------------------------------------------------

void SensorSystem::disable(void)
{
	clearContacts();
}

//---------------------------------------------------------------------------

void SensorSystem::setShutdown(bool setting)
{
	if(setting)
	{
		if(notShutdown)
		{
			int32_t i = 0;
			while(i < numContacts)
			{
				MoverPtr mover = (MoverPtr)ObjectManager->get(contacts[i] & 0x7FFF);
				if(contacts[i] & VISUAL_CONTACT_FLAG)
				{
					master->removeContact(this, mover);
					i++;
				}
				else
					removeContact(i);
			}
		}
//		clearContacts();
		notShutdown = false;
	}
	else
	{
		if(!notShutdown)
			for(size_t i = 0; i < numContacts; i++)
			{
				MoverPtr mover = (MoverPtr)ObjectManager->get(contacts[i] & 0x7FFF);
				if(contacts[i] & VISUAL_CONTACT_FLAG)
					master->addContact(this, mover, i, CONTACT_VISUAL);
				else
					master->addContact(this, mover, i, getSensorQuality());
			}
		notShutdown = true;
	}
}

//---------------------------------------------------------------------------

bool SensorSystem::enabled(void)
{
	if(master && (masterIndex > -1))
	{
		if(!owner->getExistsAndAwake())
			return(false);
		if(owner->isDisabled())
			return(false);
		if(owner->isMover())
		{
			MoverPtr mover = (MoverPtr)owner;
			return((mover->sensor != 255) && !mover->inventory[mover->sensor].disabled);
		}
		return(true);
	}
	return(false);
}

//---------------------------------------------------------------------------
void SensorSystem::setRange(float newRange)
{
	range = newRange;
}

//---------------------------------------------------------------------------
float SensorSystem::getRange(void)
{
	float result = range;
	/*		NO More recon specialist
	if (owner && owner->isMover())
	{
		MoverPtr mover = (MoverPtr)owner;
		if (mover->pilot && mover->pilot->isReconSpecialist())
			result += range * 0.2f;
	}
	*/
	return result;
}

//---------------------------------------------------------------------------
// moved to header
//void SensorSystem::setSkill (int32_t newSkill) {
//
//	skill = newSkill;
//}

//---------------------------------------------------------------------------

float SensorSystem::getEffectiveRange(void)
{
	return(range * ecmEffect);
}

//---------------------------------------------------------------------------

int32_t SensorSystem::calcContactStatus(MoverPtr mover)
{
	if(!owner->getTeam())
		return(CONTACT_NONE);
	if(mover->getFlag(OBJECT_FLAG_REMOVED))
	{
		return(CONTACT_NONE);
	}
	//----------------------------------------------------------
	//If object we are looking for is at the edge, NO CONTACT!!
	if(!Terrain::IsGameSelectTerrainPosition(mover->getPosition()))
		return CONTACT_NONE;
	//-------------------------------------------------------------
	// Should be properly set when active probes are implemented...
	int32_t newContactStatus = CONTACT_NONE;
	if(!notShutdown || (range == 0.0) && !broken)
	{
		if(owner->lineOfSight(mover) && !mover->isDisabled())
			newContactStatus = CONTACT_VISUAL;
		return(newContactStatus);
	}
	if((masterIndex == -1) || (range < 0.0))
	{
		return(CONTACT_NONE);
	}
	if(owner->isMover())
	{
		MoverPtr mover = (MoverPtr)owner;
		if((mover->sensor == 255) || mover->inventory[mover->sensor].disabled || broken)
		{
			return(CONTACT_NONE);
		}
	}
	if(mover->getFlag(OBJECT_FLAG_SENSOR) && !mover->isDisabled())
	{
		bool moverNotContact = mover->hasNullSignature() || (mover->getEcmRange() != 0.0f);
		bool moverInvisible = (mover->getStatus() == OBJECT_STATUS_SHUTDOWN);
		if(!moverInvisible && !moverNotContact)
		{
			float distanceToMover = owner->distanceFrom(mover->getPosition());
			float sensorRange = getEffectiveRange();
			if(distanceToMover <= sensorRange)
			{
				//-------------------------------------------
				//No need to check shutdown and probe AGAIN!
				newContactStatus = getSensorQuality();
				//---------------------------------------
				// If ecm affecting me, my skill drops...
				// CUT, per Mitch 2/10/00
				if((ecmEffect < 1.0) && (newContactStatus > CONTACT_SENSOR_QUALITY_1))
					newContactStatus--;
				//---------------------------------------------------
				// We now we are within sensor range, check visual.
				float startRadius = 0.0f;
				if(!owner->isMover())
					startRadius = owner->getAppearRadius();
				if(hasLOSCapability && owner->lineOfSight(mover, startRadius))
					newContactStatus = CONTACT_VISUAL;
			}
			else	//Still need to check if visual!!! ECM and Lookout Towers!!
			{
				float startRadius = 0.0f;
				if(!owner->isMover())
					startRadius = owner->getAppearRadius();
				if(hasLOSCapability && owner->lineOfSight(mover, startRadius))
					newContactStatus = CONTACT_VISUAL;
			}
		}
		else
		{
			//Target is shutdown, can ONLY be visual cause this platform has no probe.
			float startRadius = 0.0f;
			if(!owner->isMover())
				startRadius = owner->getAppearRadius();
			if(hasLOSCapability && owner->lineOfSight(mover, startRadius))
				newContactStatus = CONTACT_VISUAL;
		}
	}
	//Let us know that we can see something, sensor or otherwise!!
	if(mover->getTeam() && (owner->getTeam() == Team::home) &&	mover->getTeam()->isEnemy(Team::home) &&
			(newContactStatus != CONTACT_NONE))
	{
		SensorSystemManager::enemyInLOS = true;
	}
	return(newContactStatus);
}

//---------------------------------------------------------------------------

int32_t SensorSystem::getSensorQuality(void)
{
	if(owner && owner->getPilot())
	{
		if(owner->getPilot()->isSensorProfileSpecialist())
		{
			if(owner->getPilot()->getRank() < 4)
			{
				return(owner->getPilot()->getRank() + CONTACT_SENSOR_QUALITY_1);
			}
			else if(owner->getPilot()->getRank() == 4)
			{
				return(CONTACT_SENSOR_QUALITY_4);
			}
		}
	}
	return(CONTACT_SENSOR_QUALITY_1);
}

//---------------------------------------------------------------------------

bool SensorSystem::isContact(MoverPtr mover)
{
	if(mover->getFlag(OBJECT_FLAG_REMOVED))
		return(false);
	if(notShutdown)
		return(mover->getContactInfo()->getSensor(id) < 255);
	for(size_t i = 0; i < numContacts; i++)
		if((contacts[i] & 0x7FFF) == mover->getHandle())
			return(true);
	return(false);
}

//---------------------------------------------------------------------------

void SensorSystem::addContact(MoverPtr mover, bool visual)
{
	Assert(!isContact(mover), 0, " SensorSystem.addContact: already contact ");
	if(numContacts < MAX_CONTACTS_PER_SENSOR)
	{
		contacts[numContacts] = mover->getHandle();
		if(visual)
			contacts[numContacts] |= VISUAL_CONTACT_FLAG;
		if(notShutdown)
			master->addContact(this, mover, numContacts, visual ? CONTACT_VISUAL :  getSensorQuality());
		numContacts++;
	}
}

//---------------------------------------------------------------------------

void SensorSystem::modifyContact(MoverPtr mover, bool visual)
{
	int32_t contactNum = mover->getContactInfo()->getSensor(id);
	if(contactNum < MAX_CONTACTS_PER_SENSOR)
	{
		if(visual)
			contacts[contactNum] =  mover->getHandle() | 0x8000;
		else
			contacts[contactNum] =  mover->getHandle();
	}
	if(notShutdown)
		master->modifyContact(this, mover, visual ? CONTACT_VISUAL :  getSensorQuality());
}

//---------------------------------------------------------------------------

void SensorSystem::removeContact(int32_t contactIndex)
{
	//-----------------------------------------------
	// This assumes the contactIndex is legitimate...
	MoverPtr contact = (MoverPtr)ObjectManager->get(contacts[contactIndex] & 0x7FFF);
	Assert(contact != nullptr, contacts[contactIndex] & 0x7FFF, " SensorSystem.removeContact: bad contact ");
	numContacts--;
	if((numContacts > 0) && (contactIndex != numContacts))
	{
		//-----------------------------------------------
		// Fill vacated slot with contact in last slot...
		contacts[contactIndex] = contacts[numContacts];
		MoverPtr contact = (MoverPtr)ObjectManager->get(contacts[numContacts] & 0x7FFF);
		contact->getContactInfo()->sensors[id] = contactIndex;
	}
	if(notShutdown)
		master->removeContact(this, contact);
}

//---------------------------------------------------------------------------

void SensorSystem::removeContact(MoverPtr contact)
{
	int32_t contactIndex = contact->getContactInfo()->getSensor(id);
	if(contactIndex < 255)
		removeContact(contactIndex);
}

//---------------------------------------------------------------------------

void SensorSystem::clearContacts(void)
{
	while(numContacts)
		removeContact((int32_t)0);
}

//---------------------------------------------------------------------------

void SensorSystem::updateContacts(void)
{
	if((masterIndex == -1) || (range < 0.0))
		return;
	if(!enabled())
	{
		clearContacts();
		return;
	}
	//---------------------------------------------------------------------
	// If we've already scanned this frame, don't bother updating contacts.
	// Otherwise, update contacts...
	if(scenarioTime == lastScanUpdate)
		return;
	int32_t i = 0;
	while(i < numContacts)
	{
		MoverPtr contact = (MoverPtr)ObjectManager->get(contacts[i] & 0x7FFF);
		int32_t contactStatus = calcContactStatus(contact);
		if(contactStatus == CONTACT_NONE)
			removeContact(i);
		else
		{
			contacts[i] =  contact->getHandle();
			if(contactStatus == CONTACT_VISUAL)
				contacts[i] |= 0x8000;
			modifyContact(contact, contactStatus == CONTACT_VISUAL);
			/*			if (teamContactStatus < contactStatus) {
							//--------------------------------------------------
							// Better sensor info, so update the team sensors...
							contactInfo->contactStatus[owner->getTeamId()] = contactStatus;
							contactInfo->teamSpotter[owner->getTeamId()] = (uint8_t)owner->getHandle();
						}
			*/			i++;
		}
	}
}

//---------------------------------------------------------------------------
#define CONTACT_TYPE1	0
#define CONTACT_TYPE2	1
#define CONTACT_TYPE3	2
#define CONTACT_TYPE4	3

void SensorSystem::updateScan(bool forceUpdate)
{
	if(!forceUpdate)
		if((masterIndex == -1) || (range < 0.0) || (turn < 2))
			return;
	if(!enabled())
	{
		clearContacts();
		return;
	}
	if(!owner->getTeam())
		return;
	if(1/*(nextScanUpdate < scenarioTime) || forceUpdate*/)
	{
		int32_t currentScan = -1;
		if((currentScan = scanBattlefield()) > -1)		//No returns size of largest contact.
		{
			if(owner->isMover() &&
					(owner->getTeam() == Team::home) &&
					(currentScan > SoundSystem::largestSensorContact))
			{
				SoundSystem::largestSensorContact = currentScan;
			}
		}
		lastScanUpdate = scenarioTime;
		if(!forceUpdate)
			nextScanUpdate += scanFrequency;
	}
}

//---------------------------------------------------------------------------
__inline void getLargest(int32_t& currentLargest, MoverPtr mover, int32_t contactStatus)
{
	int32_t thisMoverSize = -1;
	switch(contactStatus)
	{
		case	CONTACT_SENSOR_QUALITY_1:
		case	CONTACT_SENSOR_QUALITY_2:
			thisMoverSize = 0;
			break;
		case	CONTACT_SENSOR_QUALITY_3:
		case	CONTACT_SENSOR_QUALITY_4:
		case	CONTACT_VISUAL:
			float tonnage = mover->getTonnage();
			if(tonnage < 35.0f)
				thisMoverSize = 1;
			else if(tonnage < 55.0f)
				thisMoverSize = 2;
			else if(tonnage < 75.0f)
				thisMoverSize = 3;
			else
				thisMoverSize = 4;
			break;
	}
	if(thisMoverSize > currentLargest)
		currentLargest = thisMoverSize;
}

//---------------------------------------------------------------------------
int32_t SensorSystem::scanBattlefield(void)
{
	//NOW returns size of largest contact!
	int32_t currentLargest = -1;
	if(!owner)
		Fatal(0, " Sensor has no owner ");
	if(!master)
		Fatal(0, " Sensor has no master ");
	if((masterIndex == -1) || (range < 0.0))
		return(0);
	int32_t numNewContacts = 0;
	int32_t numMovers = ObjectManager->getNumMovers();
	for(size_t i = 0; i < numMovers; i++)
	{
		MoverPtr mover = (MoverPtr)ObjectManager->getMover(i);
		if(mover->getExists() && (mover->getTeamId() != owner->getTeamId()))
		{
			int32_t contactStatus = calcContactStatus(mover);
			if(isContact(mover))
			{
				if(contactStatus == CONTACT_NONE)
					removeContact(mover);
				else
				{
					modifyContact(mover, contactStatus == CONTACT_VISUAL ? true : false);
					getLargest(currentLargest, mover, contactStatus);
				}
			}
			else
			{
				if(contactStatus != CONTACT_NONE)
				{
					addContact(mover, contactStatus == CONTACT_VISUAL ? true : false);
					getLargest(currentLargest, mover, contactStatus);
					numNewContacts++;
				}
			}
		}
	}
	totalContacts += numNewContacts;
	return(currentLargest);
}

//---------------------------------------------------------------------------

int32_t SensorSystem::scanMover(Mover* mover)
{
	//---------------------------------------------------------------------
	// For now, I DO NOT return the largest contact. This should be
	// okay, since it'll just get caught during the next normal update next
	// frame.
	if(!enabled())
		return(0);
	if(mover->getExists() && (mover->getTeamId() != owner->getTeamId()))
	{
		int32_t contactStatus = calcContactStatus(mover);
		if(isContact(mover))
		{
			if(contactStatus == CONTACT_NONE)
				removeContact(mover);
			else
			{
				modifyContact(mover, contactStatus == CONTACT_VISUAL ? true : false);
				//getLargest(currentLargest,mover,contactStatus);
			}
		}
		else
		{
			if(contactStatus != CONTACT_NONE)
			{
				addContact(mover, contactStatus == CONTACT_VISUAL ? true : false);
				//getLargest(currentLargest,mover,contactStatus);
				totalContacts++;
			}
		}
	}
	return(0);
}

//---------------------------------------------------------------------------

int32_t SensorSystem::getTeamContacts(int32_t* contactList, int32_t contactCriteria, int32_t sortType)
{
	Assert(master != nullptr, 0, " SensorSystem.getTeamContacts: null master ");
	return(master->getContacts(owner, contactList, contactCriteria, sortType));
}

//***************************************************************************
// TEAM SENSOR SYSTEM class
//***************************************************************************

PVOID TeamSensorSystem::operator new(size_t mySize)
{
	PVOID result = missionHeap->Malloc(mySize);
	return(result);
}

//---------------------------------------------------------------------------

void TeamSensorSystem::operator delete(PVOID us)
{
	missionHeap->Free(us);
}

//---------------------------------------------------------------------------

void TeamSensorSystem::init(void)
{
	teamId = -1;
	nextContactId = 0;
	numContactUpdatesPerPass = NUM_CONTACT_UPDATES_PER_PASS;
	curContactUpdate = 0;
	numContacts = 0;
	numEnemyContacts = 0;
	numSensors = 0;
	ecms = nullptr;
	jammers = nullptr;
}

//---------------------------------------------------------------------------
void TeamSensorSystem::destroy(void)
{
	init();
}

//---------------------------------------------------------------------------

void TeamSensorSystem::setTeam(TeamPtr newTeam)
{
	teamId = newTeam->getId();
	if(Team::teams[teamId]->rosterSize < NUM_CONTACT_UPDATES_PER_PASS)
		numContactUpdatesPerPass = Team::teams[teamId]->rosterSize;
	else
		numContactUpdatesPerPass = NUM_CONTACT_UPDATES_PER_PASS;
}

//---------------------------------------------------------------------------

void TeamSensorSystem::addSensor(SensorSystemPtr sensor)
{
	if(numSensors == MAX_SENSORS_PER_TEAM)
		Fatal(0, " TeamSensorSystem.addSensor: too many sensors ");
	sensor->setMasterIndex(numSensors);
	sensors[numSensors++] = sensor;
}

//---------------------------------------------------------------------------

void TeamSensorSystem::removeSensor(SensorSystemPtr sensor)
{
	sensor->clearContacts();
	int32_t index = sensor->getMasterIndex();
	sensor->setMasterIndex(-1);
	sensors[index] = nullptr;
	if(index < (numSensors - 1))
	{
		sensors[index] = sensors[numSensors - 1];
		sensors[index]->setMasterIndex(index);
		sensors[numSensors - 1] = nullptr;
	}
	numSensors--;
}

//---------------------------------------------------------------------------

void TeamSensorSystem::update(void)
{
	if(numSensors > 0)
	{
#if 0
		//DEBUGGING
		for(size_t k = 0; k < numContacts; k++)
		{
			MoverPtr mover = (MoverPtr)ObjectManager->get(contacts[k]);
			Assert(mover->getContactInfo()->teams[teamId] == k, 0, " Bad teams/contact link ");
		}
#endif
		if(Team::teams[teamId] == Team::home)
			SoundSystem::largestSensorContact = -1;
		//---------------------------------
		// First, update actual scanning...
		for(size_t i = 0; i < numSensors; i++)
			sensors[i]->updateScan();
		if(Team::teams[teamId]->rosterSize < NUM_CONTACT_UPDATES_PER_PASS)
			numContactUpdatesPerPass = Team::teams[teamId]->rosterSize;
		else
			numContactUpdatesPerPass = NUM_CONTACT_UPDATES_PER_PASS;
		//--------------------------------
		// Now, update current contacts...
		for(i = 0; i < numContactUpdatesPerPass; i++)
		{
			if(curContactUpdate >= numSensors)
				curContactUpdate = 0;
			sensors[curContactUpdate]->updateContacts();
			curContactUpdate++;
		}
	}
}

//---------------------------------------------------------------------------

int32_t TeamSensorSystem::getVisualContacts(MoverPtr* moverList)
{
	int32_t numVisualContacts = 0;
	for(size_t i = 0; i < numContacts; i++)
	{
		MoverPtr contact = (MoverPtr)ObjectManager->get(contacts[i]);
		if(!contact->getFlag(OBJECT_FLAG_REMOVED))
			if(contact->getContactStatus(teamId, true) == CONTACT_VISUAL)
				moverList[numVisualContacts++] = contact;
	}
	return(numVisualContacts);
}

//---------------------------------------------------------------------------

int32_t TeamSensorSystem::getSensorContacts(MoverPtr* moverList)
{
	static bool isSensor[NUM_CONTACT_STATUSES] = {false, true, true, true, true, false};
	int32_t numSensorContacts = 0;
	for(size_t i = 0; i < numContacts; i++)
	{
		MoverPtr contact = (MoverPtr)ObjectManager->get(contacts[i]);
		if(!contact->getFlag(OBJECT_FLAG_REMOVED))
			if(isSensor[contact->getContactStatus(teamId, true)])
				moverList[numSensorContacts++] = contact;
	}
	return(numSensorContacts);
}

//---------------------------------------------------------------------------

bool TeamSensorSystem::meetsCriteria(GameObjectPtr looker, MoverPtr mover, int32_t contactCriteria)
{
	bool isSensor[NUM_CONTACT_STATUSES] = {false, true, true, true, true, false};
	int32_t status = mover->getContactStatus(teamId, true);
	if(mover->getFlag(OBJECT_FLAG_REMOVED))
		return(false);
	if(status == CONTACT_NONE)
		return(false);
	if(contactCriteria & CONTACT_CRITERIA_VISUAL_OR_SENSOR)
		if((status != CONTACT_VISUAL) && !isSensor[status])
			return(false);
	if(contactCriteria & CONTACT_CRITERIA_VISUAL)
		if(status != CONTACT_VISUAL)
			return(false);
	if(contactCriteria & CONTACT_CRITERIA_SENSOR)
		if(!isSensor[status])
			return(false);
	if(contactCriteria & CONTACT_CRITERIA_NOT_CHALLENGED)
	{
		if(mover->getChallenger() != nullptr)
			return(false);
	}
	if(contactCriteria & CONTACT_CRITERIA_NOT_DISABLED)
	{
		if(mover->isDisabled())
			return(false);
	}
	if(contactCriteria & CONTACT_CRITERIA_ENEMY)
	{
		if((teamId == -1) || !mover->getTeam())
			return(false);
		if(!Team::teams[teamId]->isEnemy(mover->getTeam()))
			return(false);
	}
	if(contactCriteria & CONTACT_CRITERIA_ARMED)
	{
		if(mover->numFunctionalWeapons == 0)
			return(false);
	}
	return(true);
}

//---------------------------------------------------------------------------

bool TeamSensorSystem::hasSensorContact(int32_t teamID)
{
	for(size_t i = 0; i < numContacts; i++)
	{
		MoverPtr mover = (MoverPtr)ObjectManager->get(contacts[i]);
		if(!mover->getFlag(OBJECT_FLAG_REMOVED))
		{
			static bool isSensor[NUM_CONTACT_STATUSES] = {false, true, true, true, true, false};
			if(isSensor[mover->getContactStatus(teamID, true)])
				return(true);
		}
	}
	return(false);
}

//---------------------------------------------------------------------------

int32_t TeamSensorSystem::getContacts(GameObjectPtr looker, int32_t* contactList, int32_t contactCriteria, int32_t sortType)
{
	if((sortType != CONTACT_SORT_NONE) && !looker)
		return(0);
	static float sortValues[MAX_CONTACTS_PER_SENSOR];
	float CV = 0;
	int32_t numValidContacts = 0;
	int32_t handleList[MAX_CONTACTS_PER_SENSOR];
	for(size_t i = 0; i < numContacts; i++)
	{
		MoverPtr mover = (MoverPtr)ObjectManager->get(contacts[i]);
		if(!meetsCriteria(looker, mover, contactCriteria))
			continue;
		handleList[numValidContacts] = mover->getHandle();
		switch(sortType)
		{
			case CONTACT_SORT_NONE:
				sortValues[numValidContacts] = 0.0;
				break;
			case CONTACT_SORT_CV:
				CV = (float)mover->getCurCV();
				sortValues[numValidContacts] = CV;
				break;
			case CONTACT_SORT_DISTANCE:
				sortValues[numValidContacts] = looker->distanceFrom(mover->getPosition());
				break;
		}
		numValidContacts++;
	}
	if((numValidContacts > 0) && (sortType != CONTACT_SORT_NONE))
	{
		//---------------------------------------------------------
		// BIG ASSUMPTION HERE: That a mech will not have more than
		// MAX_CONTACTS_PER_SENSOR contacts.
		if(!SensorSystem::sortList)
		{
			SensorSystem::sortList = new SortList;
			if(!SensorSystem::sortList)
				Fatal(0, " Unable to create Contact sortList ");
			SensorSystem::sortList->init(MAX_CONTACTS_PER_SENSOR);
		}
		bool descendSort = true;
		if(sortType == CONTACT_SORT_DISTANCE)
			descendSort = false;
		SensorSystem::sortList->clear(descendSort);
		for(size_t contact = 0; contact < numValidContacts; contact++)
		{
			SensorSystem::sortList->setId(contact, handleList[contact]);
			SensorSystem::sortList->setValue(contact, sortValues[contact]);
		}
		SensorSystem::sortList->sort(descendSort);
		for(contact = 0; contact < numValidContacts; contact++)
			contactList[contact] = SensorSystem::sortList->getId(contact);
	}
	else if(contactList)
		for(size_t contact = 0; contact < numValidContacts; contact++)
			contactList[contact] = handleList[contact];
	return(numValidContacts);
}

//---------------------------------------------------------------------------

int32_t TeamSensorSystem::getContactStatus(MoverPtr mover, bool includingAllies)
{
	if(mover->getFlag(OBJECT_FLAG_REMOVED))
		return(CONTACT_NONE);
	return(mover->getContactInfo()->getContactStatus(Team::teams[teamId]->getId(), true));
}

//---------------------------------------------------------------------------

void TeamSensorSystem::scanBattlefield(void)
{
	if(numSensors)
		for(size_t i = 0; i < numSensors; i++)
			sensors[i]->updateScan(true);
}

//---------------------------------------------------------------------------

void TeamSensorSystem::scanMover(Mover* mover)
{
	if(numSensors)
		for(size_t i = 0; i < numSensors; i++)
			sensors[i]->scanMover(mover);
}

//---------------------------------------------------------------------------

void TeamSensorSystem::incNumEnemyContacts(void)
{
	numEnemyContacts++;
	if((Team::teams[teamId] == Team::home) && numEnemyContacts)
		homeTeamInContact = true;
}

//---------------------------------------------------------------------------

void TeamSensorSystem::decNumEnemyContacts(void)
{
	numEnemyContacts--;
	if(!numEnemyContacts && (Team::teams[teamId] == Team::home))
		homeTeamInContact = false;
	if(numEnemyContacts < 0)
		Fatal(0, " Negative Team Contact Count ");
}

//---------------------------------------------------------------------------

void TeamSensorSystem::addContact(SensorSystemPtr sensor, MoverPtr contact, int32_t contactIndex, int32_t contactStatus)
{
	Assert(numContacts < MAX_CONTACTS_PER_TEAM, numContacts, " TeamSensorSystem.addContact: max team contacts ");
	ContactInfoPtr contactInfo = contact->getContactInfo();
	contactInfo->sensors[sensor->id] = contactIndex;
	contactInfo->contactCount[teamId]++;
	if(contactInfo->contactStatus[teamId] < contactStatus)
	{
		contactInfo->contactStatus[teamId] = contactStatus;
		contactInfo->teamSpotter[teamId] = sensor->owner->getHandle();
	}
	if(contactInfo->contactCount[teamId] == 1)
	{
		contacts[numContacts] = contact->getHandle();
		contactInfo->teams[teamId] = numContacts;
		numContacts++;
		sensor->numExclusives++;
	}
}

//---------------------------------------------------------------------------

SensorSystemPtr TeamSensorSystem::findBestSpotter(MoverPtr contact, int32_t* status)
{
	ContactInfoPtr contactInfo = contact->getContactInfo();
	if(!contactInfo)
	{
		char s[256];
		sprintf(s, "TeamSensorSystem.findBestSpotter: nullptr contactInfo for objClass %d partID %d team %d", contact->getObjectClass(), contact->getPartId(), contact->getTeamId());
		STOP((s));
	}
	SensorSystemPtr bestSensor = nullptr;
	int32_t bestStatus = CONTACT_NONE;
	for(size_t i = 0; i < MAX_SENSORS; i++)
		if(contactInfo->sensors[i] != 255)
		{
			SensorSystemPtr sensor = SensorManager->getSensor(i);
			if(sensor && sensor->owner && (teamId == sensor->owner->getTeamId()))
			{
				int32_t status = sensor->calcContactStatus(contact);
				if(status >= bestStatus)
				{
					bestSensor = sensor;
					bestStatus = status;
				}
			}
		}
	if(status)
		*status = bestStatus;
	return(bestSensor);
}

//---------------------------------------------------------------------------

void TeamSensorSystem::modifyContact(SensorSystemPtr sensor, MoverPtr contact, int32_t contactStatus)
{
	ContactInfoPtr contactInfo = contact->getContactInfo();
	if(contactInfo->teamSpotter[teamId] == sensor->owner->getHandle())
	{
		int32_t curStatus = contactInfo->contactStatus[teamId];
		if(contactStatus > curStatus)
			contactInfo->contactStatus[teamId] = contactStatus;
		else if(contactStatus < curStatus)
		{
			int32_t bestStatus;
			SensorSystemPtr bestSensor = findBestSpotter(contact, &bestStatus);
			contactInfo->contactStatus[teamId] = bestStatus;
			contactInfo->teamSpotter[teamId] = bestSensor->owner->getHandle();
		}
	}
	else
	{
		int32_t curStatus = contactInfo->contactStatus[teamId];
		if(contactStatus > curStatus)
		{
			contactInfo->contactStatus[teamId] = contactStatus;
			contactInfo->teamSpotter[teamId] = sensor->owner->getHandle();
		}
	}
}

//---------------------------------------------------------------------------

void TeamSensorSystem::removeContact(SensorSystemPtr sensor, MoverPtr contact)
{
	ContactInfoPtr contactInfo = contact->getContactInfo();
	contactInfo->sensors[sensor->id] = 255;
	contactInfo->contactCount[teamId]--;
	Assert(contactInfo->contactCount[teamId] != 255, 0, "FUDGE");
	if(contactInfo->contactCount[teamId] == 0)
	{
		int32_t contactIndex = contactInfo->teams[teamId];
		Assert(contactIndex < 0xFFFF, contactIndex, " 0xFFFF ");
		MoverPtr removedContact = (MoverPtr)ObjectManager->get(contacts[contactIndex]);
		Assert(removedContact == contact, contactIndex, " TeamSensorSystem.removeContact: bad contact ");
		contactInfo->teams[teamId] = 0xFFFF;
		contactInfo->contactStatus[teamId] = CONTACT_NONE;
		contactInfo->teamSpotter[teamId] = 0;
		numContacts--;
		if((numContacts > 0) && (contactIndex != numContacts))
		{
			//-----------------------------------------------
			// Fill vacated slot with contact in last slot...
			contacts[contactIndex] = contacts[numContacts];
			MoverPtr mover = (MoverPtr)ObjectManager->get(contacts[numContacts]);
			mover->getContactInfo()->teams[teamId] = contactIndex;
		}
	}
	else if(contactInfo->teamSpotter[teamId] == sensor->owner->getHandle())
	{
		if(sensor->owner->getObjectClass() == BATTLEMECH)
			Assert(sensor != nullptr, 0, " dumb ");
		int32_t bestStatus;
		SensorSystemPtr bestSensor = findBestSpotter(contact, &bestStatus);
//		Assert(bestSensor != nullptr, 0, " hit ");
		if(bestSensor)
		{
			contact->contactInfo->contactStatus[teamId] = bestStatus;
			contact->contactInfo->teamSpotter[teamId] = bestSensor->owner->getHandle();
		}
	}
}

//---------------------------------------------------------------------------
#if 0
void TeamSensorSystem::updateContactList(void)
{
	numAllContacts = 0;
	for(size_t i = 0; i < ObjectManager->getNumMovers(); i++)
	{
		MoverPtr mover = ObjectManager->getMover(i);
		if(mover)
		{
			int32_t bestStatus = CONTACT_NONE;
			for(size_t i = 0; i < Team::numTeams; i++)
			{
				if(Team::teams[teamId]->isFriendly(Team::teams[i]))
					if(mover->contactInfo->contactStatus[i] > bestStatus)
						bestStatus = mover->contactInfo->contactStatus[i];
			}
			mover->contactInfo->allContactStatus[teamId] = bestStatus;
			if(bestStatus != CONTACT_NONE)
			{
				allContacts[numAllContacts++] = mover->getHandle();
			}
		}
	}
}
#endif
//---------------------------------------------------------------------------

void TeamSensorSystem::updateEcmEffects(void)
{
	for(size_t i = 0; i < numSensors; i++)
		sensors[i]->ecmEffect = SensorManager->getEcmEffect(sensors[i]->owner);
}

//***************************************************************************
// SENSOR SYSTEM MANAGER class
//***************************************************************************

PVOID SensorSystemManager::operator new(size_t mySize)
{
	PVOID result = missionHeap->Malloc(mySize);
	return(result);
}

//---------------------------------------------------------------------------

void SensorSystemManager::operator delete(PVOID us)
{
	missionHeap->Free(us);
}

//---------------------------------------------------------------------------

int32_t SensorSystemManager::init(bool debug)
{
	if(MAX_SENSORS < 2)
		Fatal(0, " Way too few sensors in Sensor System Manager! ");
	sensorPool = (SensorSystemPtr*)missionHeap->Malloc(MAX_SENSORS * sizeof(SensorSystemPtr));
	gosASSERT(sensorPool != nullptr);
	for(size_t i = 0; i < MAX_SENSORS; i++)
		sensorPool[i] = new SensorSystem;
	//-----------------------------------------------------
	// This assumes we have at least 2 sensors in the pool
	// when initializing the pool...
	sensorPool[0]->id = 0;
	sensorPool[0]->prev = nullptr;
	sensorPool[0]->next = sensorPool[1];
	for(i = 1; i < (MAX_SENSORS - 1); i++)
	{
		sensorPool[i]->id = i;
		sensorPool[i]->prev = sensorPool[i - 1];
		sensorPool[i]->next = sensorPool[i + 1];
	}
	sensorPool[MAX_SENSORS - 1]->id = MAX_SENSORS - 1;
	sensorPool[MAX_SENSORS - 1]->prev = sensorPool[MAX_SENSORS - 2];
	sensorPool[MAX_SENSORS - 1]->next = nullptr;
	//------------------------------
	// All start on the free list...
	freeList = sensorPool[0];
	freeSensors = MAX_SENSORS;
	for(i = 0; i < MAX_TEAMS; i++)
		teamSensors[i] = nullptr;
	Assert(!debug || (Team::numTeams > 0), 0, " SensorSystemManager.init: 0 teams ");
	for(i = 0; i < Team::numTeams; i++)
	{
		teamSensors[i] = new TeamSensorSystem;
		teamSensors[i]->teamId = i;
	}
	SensorSystem::numSensors = 0;
	return(NO_ERROR);
}

int32_t SensorSystemManager::checkIntegrity(void)
{
	//See if every pointer in sensorPool is still OK.
	// SOMETHING is trashing memory here.  Damned if I know what.
	int32_t result = 0;
	for(size_t i = 0; i < MAX_SENSORS; i++)
	{
		SensorSystem* sensor = getSensor(i);
		if(sensor->owner)
			result = 1;
		else
			result = 0;
	}
	return result;
}

//---------------------------------------------------------------------------

SensorSystemPtr SensorSystemManager::newSensor(void)
{
	if(!freeSensors)
		Fatal(0, " No More Free Sensors ");
	freeSensors--;
	//---------------------------------------
	// Grab the first free sensor in line...
	SensorSystemPtr sensor = freeList;
	//------------------------------------------
	// Cut the new sensor from the free list...
	freeList = freeList->next;
	if(freeList)
		freeList->prev = nullptr;
	//----------------------------------------------------
	// New system has no next. Already has no previous...
	sensor->next = nullptr;
	return(sensor);
}

//---------------------------------------------------------------------------

void SensorSystemManager::freeSensor(SensorSystemPtr sensor)
{
	freeSensors++;
	sensor->prev = nullptr;
	sensor->next = freeList;
	freeList->prev = sensor;
	freeList = sensor;
}

//---------------------------------------------------------------------------

void SensorSystemManager::destroy(void)
{
	if(sensorPool)
	{
		for(size_t i = 0; i < MAX_SENSORS; i++)
		{
			delete sensorPool[i];
			sensorPool[i] = nullptr;
		}
		missionHeap->Free(sensorPool);
		sensorPool = nullptr;
	}
	freeSensors = 0;
	freeList = nullptr;
	for(size_t i = 0; i < Team::numTeams; i++)
	{
		delete teamSensors[i];
		teamSensors[i] = nullptr;
	}
	if(SensorSystem::sortList)
	{
		delete SensorSystem::sortList;
		SensorSystem::sortList = nullptr;
	}
}

//---------------------------------------------------------------------------

void SensorSystemManager::addTeamSensor(int32_t teamId, SensorSystemPtr sensor)
{
	if((teamId > -1) && (teamId < Team::numTeams))
	{
		teamSensors[teamId]->addSensor(sensor);
		sensor->setMaster(teamSensors[teamId]);
	}
}

//---------------------------------------------------------------------------

void SensorSystemManager::removeTeamSensor(int32_t teamId, SensorSystemPtr sensor)
{
	if(teamId == -1)
		return;
	teamSensors[teamId]->removeSensor(sensor);
	sensor->setMaster(nullptr);
}

//---------------------------------------------------------------------------

void SensorSystemManager::update(void)
{
	updateEcmEffects();
	updateSensors();
//	updateTeamContactLists();
}

//---------------------------------------------------------------------------

void SensorSystemManager::updateEcmEffects(void)
{
	for(size_t i = 0; i < Team::numTeams; i++)
		teamSensors[i]->updateEcmEffects();
}

//---------------------------------------------------------------------------

void SensorSystemManager::updateSensors(void)
{
	/*
	for (size_t i = 0; i < Team::numTeams; i++)
		teamSensors[i]->update();
	*/
	//Let's try one team per frame update!!
	if(Team::teams[teamToUpdate] == Team::home)
		enemyInLOS = false;		//Resets every scan for HOME team!!!!
	teamSensors[teamToUpdate]->update();
	teamToUpdate++;
	if(teamToUpdate == Team::numTeams)
		teamToUpdate = 0;
}

//---------------------------------------------------------------------------
#if 0
void SensorSystemManager::updateTeamContactLists(void)
{
	for(size_t i = 0; i < Team::numTeams; i++)
	{
		teamSensors[i]->updateContactList();
	}
}
#endif
//---------------------------------------------------------------------------

void SensorSystemManager::addEcm(GameObjectPtr owner, float range)
{
	if(numEcms == MAX_ECMS)
		Fatal(0, " SensorSystemManager.addEcm: too many ecms ");
	ecms[numEcms].owner = owner;
	ecms[numEcms].range = range;
	numEcms++;
}

//---------------------------------------------------------------------------

float SensorSystemManager::getEcmEffect(GameObjectPtr victim)
{
	/*		ECM stealths the mech carrying it.  Period.
	for (size_t i = 0; i < numEcms; i++)
		if (!victim->isFriendly(ecms[i].owner))
			if (ecms[i].owner->distanceFrom(victim->getPosition()) <= ecms[i].range)
				return(0.5);
	*/
	return(1.0);
}

//***************************************************************************




