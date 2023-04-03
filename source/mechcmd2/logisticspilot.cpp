//===========================================================================//
// LogisticsPilot.cpp		: Implementation of the LogisticsPilot component.//
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#define LOGISTICSPILOT_CPP

#include "stdinc.h"
#include "mclib.h"
#include "logisticspilot.h"
#include "logisticserrors.h"
#include "logisticsdata.h"
#include "warrior.h"
#include "resource.h"
#include "mechicon.h"
#include "objmgr.h"

wchar_t LogisticsPilot::skillTexts[NUM_SPECIALTY_SKILLS][255] = {0};
extern std::wstring_view SpecialtySkillsTable[NUM_SPECIALTY_SKILLS];

LogisticsPilot::LogisticsPilot()
{
	bIsUsed = 0;
	bJustDied = 0;
	mechKills = vehicleKills = infantryKills = 0;
	missionsCompleted = 0;
	memset(missionsPlayed, 0, sizeof(uint8_t) * MAX_MISSIONS);
	bDead = 0;
	bAvailable = 0;
	newPiloting = 0;
	newGunnery = 0;
	memset(specialtySkills, 0, sizeof(bool) * NUM_SPECIALTY_SKILLS);
	if (!strlen(skillTexts[0]))
	{
		wchar_t tmp[256];
		for (size_t i = 0; i < NUM_SPECIALTY_SKILLS; i++)
		{
			cLoadString(IDS_SPECIALTY + i, tmp, 255);
			strcpy(skillTexts[i], tmp);
		}
	}
	memset(medals, 0, sizeof(bool) * MAX_MEDAL);
}

LogisticsPilot::~LogisticsPilot() { }

int32_t
LogisticsPilot::init(std::wstring_view pilotFileName)
{
	fileName = pilotFileName;
	wchar_t path[256];
	strcpy(path, warriorPath);
	strcat(path, fileName);
	strcat(path, ".fit");
	FitIniFile pilotFile;
	if (NO_ERROR != pilotFile.open(path))
	{
		wchar_t errorString[256];
		sprintf(errorString, "Couldn't open file %s", fileName);
		Assert(0, 0, errorString);
		return -1;
	}
	// if we got this far we have a file, make a pilot
	int32_t result = pilotFile.seekBlock("General");
	gosASSERT(result == 0);
	int32_t tmp;
	result = pilotFile.readIdLong("descIndex", tmp);
	gosASSERT(result == NO_ERROR);
	cLoadString(tmp, path, 256);
	name = path;
	result = pilotFile.readIdLong("PictureIndex", photoIndex);
	if (result != NO_ERROR)
		photoIndex = 0;
	result = pilotFile.readIdLong("Rank", rank);
	gosASSERT(result == 0);
	//	result = pilotFile.readIdULong( "ID", id );
	//	gosASSERT( result == 0 );
	result = pilotFile.readIdLong("FlavorText", flavorTextID);
	result = pilotFile.readIdString("pilotAudio", path, 256);
	gosASSERT(result == 0);
	audio = path;
	result = pilotFile.readIdString("pilotVideo", path, 256);
	gosASSERT(result == 0);
	video = path;
	result = pilotFile.readIdString("Picture", path, 256);
	iconFile = artPath;
	iconFile += path;
	pilotFile.seekBlock("Skills");
	wchar_t tPilot, tGunnery;
	result = pilotFile.readIdChar("Piloting", tPilot);
	gosASSERT(result == NO_ERROR);
	pilotFile.readIdChar("Gunnery", tGunnery);
	piloting = tPilot;
	gunnery = tGunnery;
	result = pilotFile.seekBlock("SpecialtySkills");
	if (result == NO_ERROR)
	{
		for (size_t i = 0; i < NUM_SPECIALTY_SKILLS; i++)
		{
			wchar_t tmpChar;
			result = pilotFile.readIdChar(SpecialtySkillsTable[i], tmpChar);
			if (result == NO_ERROR)
				specialtySkills[i] = (tmpChar == 1);
		}
	}
	rank = turnAverageIntoRank((gunnery + piloting) / 2.f);
	return 0;
}

std::wstring_view
LogisticsPilot::getSkillText(int32_t skillID)
{
	gosASSERT(skillID <= Skill::numberofskills);
	if (skillTexts[skillID])
	{
		return skillTexts[skillID];
	}
	//	wchar_t tmp[256];
	//	cLoadString( IDS_SKILL0 + skillID, tmp, 256 );
	//	skillTexts[skillID] = tmp;
	return skillTexts[skillID];
}

int32_t
LogisticsPilot::getNumberMissions(void) const
{
	return missionsCompleted;
}

int32_t
LogisticsPilot::save(FitIniFile& file, int32_t which)
{
	wchar_t tmp[256];
	sprintf(tmp, "Pilot%ld", which);
	file.writeBlock(tmp);
	file.writeIdString("filename", fileName);
	file.writeIdLong("Rank", rank);
	file.writeIdFloat("Gunnery", gunnery);
	file.writeIdFloat("Piloting", piloting);
	file.writeIdLong("Kills", mechKills);
	file.writeIdLong("VehicleKills", vehicleKills);
	file.writeIdLong("InfantryKills", infantryKills);
	file.writeIdLong("MissionsCompleted", missionsCompleted);
	file.writeIdUCHARArray("MissionsPlayed", missionsPlayed, MAX_MISSIONS);
	file.writeIdBoolean("Dead", bDead);
	wchar_t buffer[64];
	int32_t i;
	for (i = 0; i < MAX_MEDAL; i++)
	{
		sprintf(buffer, "Medal%ld", i);
		file.writeIdBoolean(buffer, medals[i]);
	}
	for (i = 0; i < NUM_SPECIALTY_SKILLS; i++)
	{
		sprintf(buffer, "SpecialtySkill%ld", i);
		file.writeIdBoolean(buffer, specialtySkills[i]);
	}
	return 0;
}

int32_t
LogisticsPilot::load(FitIniFile& file)
{
	wchar_t tmp[256];
	file.readIdString("filename", tmp, 255);
	fileName = tmp;
	file.readIdLong("Rank", rank);
	file.readIdFloat("Gunnery", gunnery);
	file.readIdFloat("Piloting", piloting);
	file.readIdLong("Kills", mechKills);
	file.readIdLong("VehicleKills", vehicleKills);
	file.readIdLong("InfantryKills", infantryKills);
	file.readIdLong("MissionsCompleted", missionsCompleted);
	int32_t result = file.readIdUCHARArray("MissionsPlayed", missionsPlayed, MAX_MISSIONS);
	if (result != NO_ERROR)
		memset(missionsPlayed, 0, sizeof(uint8_t) * MAX_MISSIONS);
	file.readIdBoolean("Dead", bDead);
	wchar_t buffer[64];
	int32_t i;
	for (i = 0; i < MAX_MEDAL; i++)
	{
		sprintf(buffer, "Medal%ld", i);
		file.readIdBoolean(buffer, medals[i]);
	}
	for (i = 0; i < NUM_SPECIALTY_SKILLS; i++)
	{
		sprintf(buffer, "SpecialtySkill%ld", i);
		file.readIdBoolean(buffer, specialtySkills[i]);
	}
	rank = turnAverageIntoRank((gunnery + piloting) / 2.f);
	return 0;
}

void LogisticsPilot::clearIcons()
{
	// clear out the old ones
	for (EList<ForceGroupIcon*, ForceGroupIcon*>::EIterator iter = killedIcons.Begin();
		 !iter.IsDone(); iter++)
	{
		delete (*iter);
	}
	killedIcons.Clear();
}

int32_t
LogisticsPilot::update(MechWarrior* pWarrior)
{
#ifndef VIEWER
	// clear out the old ones
	for (EList<ForceGroupIcon*, ForceGroupIcon*>::EIterator iter = killedIcons.Begin();
		 !iter.IsDone(); iter++)
	{
		delete (*iter);
	}
	killedIcons.Clear();
	missionsCompleted++;
	int32_t missionJustPlayed = LogisticsData::instance->getCurrentMissionId();
	if ((missionJustPlayed < 0) || (missionJustPlayed > MAX_MISSIONS))
		STOP(("Logistics thinks last mission played was %d", missionJustPlayed));
	missionsPlayed[missionJustPlayed] = 1;
	if (pWarrior->getStatus() == WARRIOR_STATUS_DEAD)
	{
		bDead = true;
		bJustDied = true;
	}
	setUsed(1);
	pWarrior->updateMissionSkills();
	newGunnery = pWarrior->skillRank[Skill::gunnery] - gunnery;
	newPiloting = pWarrior->skillRank[Skill::piloting] - piloting;
	gunnery = pWarrior->skillRank[Skill::gunnery];
	piloting = pWarrior->skillRank[Skill::piloting];
	// make sure no more than 4 points per mission
	if (newGunnery > 4.f)
	{
		gunnery -= newGunnery;
		newGunnery = 4.f;
		gunnery += newGunnery;
	}
	if (newPiloting > 4.f)
	{
		piloting -= newPiloting;
		newPiloting = 4.f;
		piloting += newPiloting;
	}
	infantryKills += pWarrior->numMechKills[VEHICLE_CLASS_ELEMENTAL][COMBAT_STAT_MISSION];
	mechKills += pWarrior->numMechKills[VEHICLE_CLASS_LIGHTMECH][COMBAT_STAT_MISSION];
	mechKills += pWarrior->numMechKills[VEHICLE_CLASS_MEDIUMMECH][COMBAT_STAT_MISSION];
	mechKills += pWarrior->numMechKills[VEHICLE_CLASS_HEAVYMECH][COMBAT_STAT_MISSION];
	mechKills += pWarrior->numMechKills[VEHICLE_CLASS_ASSAULTMECH][COMBAT_STAT_MISSION];
	vehicleKills += pWarrior->numMechKills[VEHICLE_CLASS_GROUND][COMBAT_STAT_MISSION];
	rank = pWarrior->getRank();
	int32_t deadMechCount = 0;
	for (size_t i = 0; i < pWarrior->numKilled; i++)
	{
		GameObject* pDead = ObjectManager->getByWatchID(pWarrior->killed[i]);
		if (pDead->getObjectClass() == BATTLEMECH)
		{
			MechIcon* pIcon = new MechIcon();
			pIcon->swapResolutions(0);
			killedIcons.Append(pIcon);
			pIcon->init((Mover*)pDead);
			pIcon->update();
			if (((std::unique_ptr<Mover>)pDead)->getMoveType() != MOVETYPE_AIR)
				deadMechCount++;
		}
		else if (pDead->getObjectClass() == GROUNDVEHICLE)
		{
			VehicleIcon* pIcon = new VehicleIcon();
			pIcon->swapResolutions(0);
			killedIcons.Append(pIcon);
			pIcon->init((Mover*)pDead);
			pIcon->update();
		}
	}
	memset(medalsLastMission, 0, sizeof(bool) * MAX_MEDAL);
	if (deadMechCount >= 7)
	{
		medalsLastMission[UNCOMMON_VALOR] = true;
		medals[UNCOMMON_VALOR] = true;
	}
	if ((deadMechCount >= 3) && (deadMechCount < 7))
	{
		medalsLastMission[VALOR] = true;
		medals[VALOR] = true;
	}
	if (pWarrior->getWounds())
	{
		if (!medals[PURPLE_HEART])
			medalsLastMission[PURPLE_HEART] = true;
		medals[PURPLE_HEART] = true;
	}
	// Check for the campaign ribbons and medals.
	// NOTE: NONE of these should be awarded UNLESS we are playing the shipping
	// campaign.
	int32_t anySteinerPlayed = missionsPlayed[0] + missionsPlayed[1] + missionsPlayed[2] + missionsPlayed[3] + missionsPlayed[4] + missionsPlayed[5] + missionsPlayed[6] + missionsPlayed[7] + missionsPlayed[8];
	int32_t anyLiaoPlayed = missionsPlayed[9] + missionsPlayed[10] + missionsPlayed[11] + missionsPlayed[12] + missionsPlayed[13] + missionsPlayed[14] + missionsPlayed[15] + missionsPlayed[16];
	int32_t anyDavionPlayed = missionsPlayed[17] + missionsPlayed[18] + missionsPlayed[19] + missionsPlayed[20] + missionsPlayed[21] + missionsPlayed[22] + missionsPlayed[23];
	bool allSteinerPlayed = (anySteinerPlayed == 9);
	bool allLiaoPlayed = (anyLiaoPlayed == 8);
	bool allDavionPlayed = (anyDavionPlayed == 7);
	if (anySteinerPlayed)
	{
		if (!medals[CAMPAIGN_RIBBON1])
			medalsLastMission[CAMPAIGN_RIBBON1] = true;
		medals[CAMPAIGN_RIBBON1] = true;
	}
	if (anyLiaoPlayed)
	{
		if (!medals[CAMPAIGN_RIBBON2])
			medalsLastMission[CAMPAIGN_RIBBON2] = true;
		medals[CAMPAIGN_RIBBON2] = true;
	}
	if (anyDavionPlayed)
	{
		if (!medals[CAMPAIGN_RIBBON3])
			medalsLastMission[CAMPAIGN_RIBBON3] = true;
		medals[CAMPAIGN_RIBBON3] = true;
	}
	if (allSteinerPlayed)
	{
		if (!medals[CAMPAIGN_FULLRIBBON1])
			medalsLastMission[CAMPAIGN_FULLRIBBON1] = true;
		medals[CAMPAIGN_FULLRIBBON1] = true;
	}
	if (allLiaoPlayed)
	{
		if (!medals[CAMPAIGN_FULLRIBBON2])
			medalsLastMission[CAMPAIGN_FULLRIBBON2] = true;
		medals[CAMPAIGN_FULLRIBBON2] = true;
	}
	if (allDavionPlayed)
	{
		if (!medals[CAMPAIGN_FULLRIBBON3])
			medalsLastMission[CAMPAIGN_FULLRIBBON3] = true;
		medals[CAMPAIGN_FULLRIBBON3] = true;
	}
	if (missionsPlayed[8])
	{
		if (!medals[STEINER_MEDAL])
			medalsLastMission[STEINER_MEDAL] = true;
		medals[STEINER_MEDAL] = true;
	}
	if (missionsPlayed[12])
	{
		if (!medals[LIAO_MEDAL])
			medalsLastMission[LIAO_MEDAL] = true;
		medals[LIAO_MEDAL] = true;
	}
	if (missionsPlayed[21])
	{
		if (!medals[DAVION_MEDAL])
			medalsLastMission[DAVION_MEDAL] = true;
		medals[DAVION_MEDAL] = true;
	}
	for (i = 0; i < NUM_SPECIALTY_SKILLS; i++)
	{
		if (pWarrior->specialtySkills[i])
		{
			specialtySkills[i] = true;
		}
	}
#endif
	return 0;
}

bool LogisticsPilot::promotePilot()
{
	float oldGunnery = gunnery - newGunnery;
	float oldPiloting = piloting - newPiloting;
	float oldAvg = (oldGunnery + oldPiloting) / 2.f;
	float newAvg = (gunnery + piloting) / 2.f;
	int32_t oldRank = turnAverageIntoRank(oldAvg);
	int32_t newRank = turnAverageIntoRank(newAvg);
	if (rank != newRank)
	{
		// go ahead and set that rank
		rank = newRank;
	}
	if (oldRank != newRank)
	{
		gosASSERT(newRank > oldRank); // bad to demote
		if (rank > WarriorRank::green)
			return true;
		return false;
	}
	// temporary for testing change to false
	return false;
}

int32_t
LogisticsPilot::turnAverageIntoRank(float avg)
{
	if (avg > 79)
		return WarriorRank::ace;
	else if (avg > 70)
		return WarriorRank::elite;
	else if (avg > 60)
		return WarriorRank::veteran;
	else if (avg > 50)
		return WarriorRank::regular;
	return WarriorRank::green;
}

int32_t
LogisticsPilot::getSpecialtySkillCount(void) const
{
	int32_t count = 0;
	for (size_t i = 0; i < NUM_SPECIALTY_SKILLS; i++)
	{
		if (specialtySkills[i])
			count++;
	}
	return count;
}
int32_t
LogisticsPilot::getSpecialtySkills(std::wstring_view* array, int32_t& count)
{
	int32_t max = count;
	count = 0;
	for (size_t i = 0; i < NUM_SPECIALTY_SKILLS; i++)
	{
		if (count >= max)
		{
			return NEED_BIGGER_ARRAY;
		}
		if (specialtySkills[i])
		{
			array[count] = skillTexts[i];
			count++;
		}
	}
	return 0;
}

int32_t
LogisticsPilot::getSpecialtySkills(int32_t* array, int32_t& count)
{
	int32_t max = count;
	count = 0;
	for (size_t i = 0; i < NUM_SPECIALTY_SKILLS; i++)
	{
		if (i >= max)
		{
			return NEED_BIGGER_ARRAY;
		}
		if (specialtySkills[i])
		{
			array[count] = i;
			count++;
		}
	}
	return 0;
}

void LogisticsPilot::setSpecialtySkill(int32_t skill, bool set)
{
	if (skill >= NUM_SPECIALTY_SKILLS)
	{
		gosASSERT(0);
		return;
	}
	specialtySkills[skill] = set;
}

// end of file ( LogisticsPilot.cpp )
