/*************************************************************************************************\
LogisticsMissionInfo.h			: Interface for the LogisticsMissionInfo
component.
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//
\*************************************************************************************************/

#pragma once

#ifndef LOGISTICSMISSIONINFO_H
#define LOGISTICSMISSIONINFO_H

//#include "elist.h"
//#include "estring.h"

#define MAX_PLAYER_NAME_LENGTH 64

class FitIniFile;
/**************************************************************************************************
CLASS DESCRIPTION
LogisticsMissionInfo:
**************************************************************************************************/
class LogisticsMissionInfo
{
public:
	LogisticsMissionInfo(void);
	~LogisticsMissionInfo(void);

	int32_t init(FitIniFile& file); // init campaign
	void save(FitIniFile& file);
	int32_t load(FitIniFile& file); // init previously saved stuff

	int32_t getAvailableMissions(std::wstring_view* missions, int32_t& numberOfEm);
	int32_t getCurrentMissions(std::wstring_view* missions, int32_t& numberOfEm);

	bool getMissionAvailable(std::wstring_view missionName);

	bool isMissionComplete(std::wstring_view missionName);
	bool isSingleMission(void) const;

	int32_t setNextMission(std::wstring_view missionName);
	void setMissionComplete();

	int32_t getCurrentLogisticsTuneId(void);

	int32_t getCurrentMissionId(void);

	std::wstring_view getCurrentPurchaseFile(void) const;
	std::wstring_view getCurrentMission(void) const
	{
		return currentMissionName;
	}
	std::wstring_view getLastMission(void) const
	{
		return lastMissionName;
	}

	int32_t getCurrentDropWeight(void) const;
	std::wstring_view getCurrentVideo(void) const;
	std::wstring_view getCurrentOperationFile(void) const;
	std::wstring_view getCurrentMissionDescription(void) const;
	std::wstring_view getCurrentMissionFriendlyName(void) const;
	std::wstring_view getMissionFriendlyName(std::wstring_view missionName) const;
	std::wstring_view getCurrentABLScriptName(void) const;

	int32_t getCurrentRP(void) const;
	int32_t getCBills(void) const
	{
		return CBills;
	}
	void incrementCBills(int32_t amount)
	{
		CBills += amount;
	}
	void decrementCBills(int32_t amount)
	{
		CBills -= amount;
	}

	std::wstring_view getCampaignName(void) const
	{
		return campaignName;
	}
	std::wstring_view getCampaignDisplayName(void) const
	{
		return campaignDisplayName;
	}

	bool campaignOver(void) const;

	std::wstring_view getCurrentBigVideo(void) const;
	std::wstring_view getFinalVideo(void) const;

	void setMultiplayer(void);
	void setPurchaseFile(std::wstring_view fileName);

	//		int32_t				getMaxTeams(void) const { return maxTeams; }
	//		int32_t				getMaxPlayers(void) const { return maxPlayers; }

	int32_t getAdditionalPurachaseFiles(std::wstring_view* list, int32_t& maxCount);
	void addBonusPurchaseFile(std::wstring_view fileName); // extra bonus

	bool skipLogistics(void);
	bool showChooseMission(void);

	bool skipPilotReview(void);
	bool skipSalvageScreen(void);
	bool skipPurchasing(void);

	void setSingleMission(std::wstring_view pName);

	bool canHaveSalavageCraft(void);
	bool canHaveRepairTruck(void);
	bool canHaveScoutCopter(void);
	bool canHaveArtilleryPiece(void);
	bool canHaveAirStrike(void);
	bool canHaveSensorStrike(void);
	bool canHaveMineLayer(void);
	bool getVideoShown(void);
	void setVideoShown(void);

	int32_t getCurrentMissionNumber()
	{
		return currentMission;
	}

	void setCurrentMissionNumber(int32_t cMission)
	{
		currentMission = cMission;
	}

private:
	class MissionInfo
	{
	public:
		~MissionInfo(void);
		std::wstring_view missionDescriptiveName;
		std::wstring_view fileName;
		std::wstring_view description;
		std::wstring_view purchaseFileName;
		std::wstring_view videoName;
		bool mandatory;
		bool completePrevious;
		bool completed;
		int32_t resourcePoints;
		int32_t additionalCBills;
		int32_t dropWeight;
		bool playLogistics;
		bool playSalvage;
		bool playPilotPromotion;
		bool playPurchasing;
		bool playMissionSelection;
		bool hidden;
		bool enableSalavageCraft;
		bool enableRepairTruck;
		bool enableScoutCopter;
		bool enableArtilleryPiece;
		bool enableAirStrike;
		bool enableSensorStrike;
		bool enableMineLayer;
	};

	typedef EList<MissionInfo*, MissionInfo*> MISSION_LIST;
	typedef EList<std::wstring_view, std::wstring_view> FILE_LIST;

	class MissionGroup
	{
	public:
		int32_t numberToBeCompleted;
		std::wstring_view videoFileName;
		std::wstring_view operationFileName;
		std::wstring_view bigVideoName;
		std::wstring_view ablBrainName;
		int32_t logisticsTuneId;
		MISSION_LIST infos;
		bool bigVideoShown;
		bool videoShown;
	};

	// DATA

	std::wstring_view campaignName;
	std::wstring_view campaignDisplayName;
	std::wstring_view playerName;
	std::wstring_view currentMissionName;
	std::wstring_view finalVideoName;
	int32_t currentStage;
	int32_t currentMission;

	int32_t lastStage;
	int32_t lastMission;
	std::wstring_view lastMissionName;

	MissionGroup* groups;
	int32_t groupCount;

	int32_t CBills;

	void clear(void);
	bool bMultiplayer;

	FILE_LIST additionalPurchaseFiles;

	LogisticsMissionInfo::MissionInfo* getPreviousMission(void);
	void readMissionInfo(FitIniFile& file, LogisticsMissionInfo::MissionInfo* pInfo);

	// HELPER FUNCTIONS
};

#endif // end of file ( LogisticsMissionInfo.h )
