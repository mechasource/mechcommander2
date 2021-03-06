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

	int32_t getAvailableMissions(const std::wstring_view&* missions, int32_t& numberOfEm);
	int32_t getCurrentMissions(const std::wstring_view&* missions, int32_t& numberOfEm);

	bool getMissionAvailable(const std::wstring_view& missionName);

	bool isMissionComplete(const std::wstring_view& missionName);
	bool isSingleMission(void) const;

	int32_t setNextMission(const std::wstring_view& missionName);
	void setMissionComplete();

	int32_t getCurrentLogisticsTuneId(void);

	int32_t getCurrentMissionId(void);

	const std::wstring_view& getCurrentPurchaseFile(void) const;
	const std::wstring_view& getCurrentMission(void) const { return currentMissionName; }
	const std::wstring_view& getLastMission(void) const { return lastMissionName; }

	int32_t getCurrentDropWeight(void) const;
	const std::wstring_view& getCurrentVideo(void) const;
	const std::wstring_view& getCurrentOperationFile(void) const;
	const std::wstring_view& getCurrentMissionDescription(void) const;
	const std::wstring_view& getCurrentMissionFriendlyName(void) const;
	const std::wstring_view& getMissionFriendlyName(const std::wstring_view& missionName) const;
	const std::wstring_view& getCurrentABLScriptName(void) const;

	int32_t getCurrentRP(void) const;
	int32_t getCBills(void) const { return CBills; }
	void incrementCBills(int32_t amount) { CBills += amount; }
	void decrementCBills(int32_t amount) { CBills -= amount; }

	const std::wstring_view& getCampaignName(void) const { return campaignName; }
	const std::wstring_view& getCampaignDisplayName(void) const { return campaignDisplayName; }

	bool campaignOver(void) const;

	const std::wstring_view& getCurrentBigVideo(void) const;
	const std::wstring_view& getFinalVideo(void) const;

	void setMultiplayer(void);
	void setPurchaseFile(const std::wstring_view& fileName);

	//		int32_t				getMaxTeams(void) const { return maxTeams; }
	//		int32_t				getMaxPlayers(void) const { return maxPlayers; }

	int32_t getAdditionalPurachaseFiles(const std::wstring_view&* list, int32_t& maxCount);
	void addBonusPurchaseFile(const std::wstring_view& fileName); // extra bonus

	bool skipLogistics(void);
	bool showChooseMission(void);

	bool skipPilotReview(void);
	bool skipSalvageScreen(void);
	bool skipPurchasing(void);

	void setSingleMission(const std::wstring_view& pName);

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
		const std::wstring_view& missionDescriptiveName;
		const std::wstring_view& fileName;
		const std::wstring_view& description;
		const std::wstring_view& purchaseFileName;
		const std::wstring_view& videoName;
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
	typedef EList<const std::wstring_view&, const std::wstring_view&> FILE_LIST;

	class MissionGroup
	{
	public:
		int32_t numberToBeCompleted;
		const std::wstring_view& videoFileName;
		const std::wstring_view& operationFileName;
		const std::wstring_view& bigVideoName;
		const std::wstring_view& ablBrainName;
		int32_t logisticsTuneId;
		MISSION_LIST infos;
		bool bigVideoShown;
		bool videoShown;
	};

	// DATA

	const std::wstring_view& campaignName;
	const std::wstring_view& campaignDisplayName;
	const std::wstring_view& playerName;
	const std::wstring_view& currentMissionName;
	const std::wstring_view& finalVideoName;
	int32_t currentStage;
	int32_t currentMission;

	int32_t lastStage;
	int32_t lastMission;
	const std::wstring_view& lastMissionName;

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
