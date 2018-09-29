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

//#include <elist.h>
//#include <estring.h>

#define MAX_PLAYER_NAME_LENGTH 64

//*************************************************************************************************
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

	int32_t getAvailableMissions(PCSTR* missions, int32_t& numberOfEm);
	int32_t getCurrentMissions(PCSTR* missions, int32_t& numberOfEm);

	bool getMissionAvailable(PCSTR missionName);

	bool isMissionComplete(PCSTR missionName);
	bool isSingleMission(void) const;

	int32_t setNextMission(PCSTR missionName);
	void setMissionComplete();

	int32_t getCurrentLogisticsTuneId(void);

	int32_t getCurrentMissionId(void);

	const std::wstring& getCurrentPurchaseFile(void) const;
	const std::wstring& getCurrentMission(void) const { return currentMissionName; }
	const std::wstring& getLastMission(void) const { return lastMissionName; }

	int32_t getCurrentDropWeight(void) const;
	PCSTR getCurrentVideo(void) const;
	PCSTR getCurrentOperationFile(void) const;
	PCSTR getCurrentMissionDescription(void) const;
	PCSTR getCurrentMissionFriendlyName(void) const;
	PCSTR getMissionFriendlyName(PCSTR missionName) const;
	PCSTR getCurrentABLScriptName(void) const;

	int32_t getCurrentRP(void) const;
	int32_t getCBills(void) const { return CBills; }
	void incrementCBills(int32_t amount) { CBills += amount; }
	void decrementCBills(int32_t amount) { CBills -= amount; }

	const std::wstring& getCampaignName() const { return campaignName; }
	const std::wstring& getCampaignDisplayName(void) const { return campaignDisplayName; }

	bool campaignOver(void) const;

	PCSTR getCurrentBigVideo(void) const;
	PCSTR getFinalVideo(void) const;

	void setMultiplayer(void);
	void setPurchaseFile(PCSTR fileName);

	//		int32_t				getMaxTeams(void) const { return maxTeams; }
	//		int32_t				getMaxPlayers(void) const { return maxPlayers; }

	int32_t getAdditionalPurachaseFiles(PCSTR* list, int32_t& maxCount);
	void addBonusPurchaseFile(PCSTR fileName); // extra bonus

	bool skipLogistics(void);
	bool showChooseMission(void);

	bool skipPilotReview(void);
	bool skipSalvageScreen(void);
	bool skipPurchasing(void);

	void setSingleMission(PCSTR pName);

	bool canHaveSalavageCraft(void);
	bool canHaveRepairTruck(void);
	bool canHaveScoutCopter(void);
	bool canHaveArtilleryPiece(void);
	bool canHaveAirStrike(void);
	bool canHaveSensorStrike(void);
	bool canHaveMineLayer(void);
	bool getVideoShown(void);
	void setVideoShown(void);

	int32_t getCurrentMissionNumber() { return currentMission; }

	void setCurrentMissionNumber(int32_t cMission) { currentMission = cMission; }

private:
	class MissionInfo
	{
	public:
		~MissionInfo(void);
		std::wstring missionDescriptiveName;
		std::wstring fileName;
		std::wstring description;
		std::wstring purchaseFileName;
		std::wstring videoName;
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
	typedef EList<PSTR, PSTR> FILE_LIST;

	class MissionGroup
	{
	public:
		int32_t numberToBeCompleted;
		std::wstring videoFileName;
		std::wstring operationFileName;
		std::wstring bigVideoName;
		std::wstring ablBrainName;
		int32_t logisticsTuneId;
		MISSION_LIST infos;
		bool bigVideoShown;
		bool videoShown;
	};

	// DATA

	std::wstring campaignName;
	std::wstring campaignDisplayName;
	std::wstring playerName;
	std::wstring currentMissionName;
	std::wstring finalVideoName;
	int32_t currentStage;
	int32_t currentMission;

	int32_t lastStage;
	int32_t lastMission;
	std::wstring lastMissionName;

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

//*************************************************************************************************
#endif // end of file ( LogisticsMissionInfo.h )
