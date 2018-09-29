
#pragma once

#ifndef LOGISTICSPILOT_H
#define LOGISTICSPILOT_H

//===========================================================================//
// LogisticsPilot.h			: Interface for the LogisticsPilot component.    //
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

class FitIniFile;
class MechWarrior;
class ForceGroupIcon;
/**************************************************************************************************
CLASS DESCRIPTION
LogisticsPilot:
**************************************************************************************************/
#include <estring.h>
#include <elist.h>
#include "warrior.h"

#define MAX_MISSIONS 50

enum Medals
{

	CAMPAIGN_RIBBON1	 = 0,
	CAMPAIGN_RIBBON2	 = 1,
	CAMPAIGN_RIBBON3	 = 2,
	CAMPAIGN_FULLRIBBON1 = 3,
	CAMPAIGN_FULLRIBBON2 = 4,
	CAMPAIGN_FULLRIBBON3 = 5,
	PURPLE_HEART		 = 6,
	VALOR				 = 7,
	UNCOMMON_VALOR		 = 8,
	MISSION_SPEC1		 = 9,
	MISSION_SPEC2		 = 10,
	MISSION_SPEC3		 = 11,
	MISSION_SPEC4		 = 12,
	LIAO_MEDAL			 = 13,
	DAVION_MEDAL		 = 14,
	STEINER_MEDAL		 = 15,
	MAX_MEDAL

};

class LogisticsPilot
{
public:
	int32_t init(PSTR fileName);
	LogisticsPilot(void);
	~LogisticsPilot(void);

	const std::wstring& getName(void) const { return name; }
	const std::wstring& getAudio(void) const { return audio; }
	const std::wstring& getVideo(void) const { return video; }
	int32_t getRank(void) const { return rank; }
	const std::wstring& getIconFile(void) const { return iconFile; }
	float getGunnery(void) const { return gunnery; }
	float getPiloting(void) const { return piloting; }
	int32_t getMechKills(void) const { return mechKills; }
	int32_t getVehicleKills(void) const { return vehicleKills; }
	int32_t getInfantryKills(void) const { return infantryKills; }
	int32_t getID(void) const { return id; }
	int32_t getFlavorTextID(void) const { return flavorTextID; }
	int32_t getNumberMissions(void) const;
	bool getMissionPlayed(int32_t missionNum)
	{
		if ((missionNum >= 0) && (missionNum < MAX_MISSIONS))
			return (missionsPlayed[missionNum] == 1);
		return false;
	}

	bool isUsed(void) const { return bIsUsed; }
	void setUsed(bool bUsed) { bIsUsed = bUsed; }
	int32_t getNewGunnery(void) const { return newGunnery; }
	int32_t getNewPiloting(void) const { return newPiloting; }

	void setDead(void);
	bool isAvailable()
	{
		return (bAvailable && !bDead); // depends on purchasing file .... maybe
									   // should put dead checks and stuff
	}
	void setAvailable(bool available) { bAvailable = available; }
	bool isDead(void) const { return bDead; }
	bool justDied(void) const { return bJustDied; }
	void setJustDied(bool bdied) { bJustDied = bdied; }
	int32_t getPhotoIndex(void) const { return photoIndex; }

	bool promotePilot(void);
	int32_t turnAverageIntoRank(float avg);

	static PCSTR getSkillText(int32_t skillID);
	const std::wstring& getFileName() { return fileName; }

	int32_t save(FitIniFile& file, int32_t count);
	int32_t load(FitIniFile& file);

	int32_t update(MechWarrior* pWarrior);

	int32_t getSpecialtySkillCount(void) const;
	int32_t getSpecialtySkills(PCSTR* array, int32_t& count);
	int32_t getSpecialtySkills(pint32_t array, int32_t& count);
	bool* getSpecialtySkills() { return specialtySkills; }
	void setSpecialtySkill(int32_t skill, bool set);

	const bool* getNewMedalArray(void) const { return medalsLastMission; }
	const bool* getMedalArray(void) const { return medals; }

	void clearIcons(void);

	uint32_t id;
	EList<ForceGroupIcon*, ForceGroupIcon*> killedIcons;

private:
	std::wstring name;
	std::wstring audio;
	std::wstring video;
	int32_t rank;
	std::wstring iconFile; // or file name?
	float gunnery;
	float piloting;
	int32_t mechKills;
	int32_t vehicleKills;
	int32_t infantryKills;
	int32_t missionsCompleted;
	uint8_t missionsPlayed[MAX_MISSIONS];
	int32_t flavorTextID;
	bool bIsUsed;
	bool bDead;
	bool bAvailable;
	bool bJustDied;
	int32_t photoIndex;
	float newPiloting;
	float newGunnery;
	bool specialtySkills[NUM_SPECIALTY_SKILLS];

	static char skillTexts[][255];
	std::wstring fileName;

	bool medals[MAX_MEDAL];
	bool medalsLastMission[MAX_MEDAL];

	// last second hack
	friend class LogisticsData;
};

//*************************************************************************************************
#endif // end of file ( LogisticsPilot.h )
