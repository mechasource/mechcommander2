/*************************************************************************************************\
LogisticsData.h			: Interface for the LogisticsData component.  This
thing holds the inventory lists and the component lists
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//
\*************************************************************************************************/

#pragma once

#ifndef LOGISTICSDATA_H
#define LOGISTICSDATA_H

//#include <stuff/stuff.hpp>
//#include <elist.h>
//#include "logisticsmech.h"
//#include "logisticscomponent.h"
//#include "logisticspilot.h"

class FitIniFile;
class LogisticsMissionInfo;
class PacketFile;

class LogisticsData
{

  public:
	struct Building;

	LogisticsData(void);
	~LogisticsData(void);
	void init(void);

	bool rpJustAdded;

	static LogisticsData* instance;

	LogisticsComponent* getComponent(int32_t componentID);
	LogisticsPilot* getPilot(int32_t pilotID);
	LogisticsPilot* getPilot(PCSTR pilotName);
	int32_t getPilotCount(void);
	int32_t getPilots(LogisticsPilot**, int32_t& count);
	PCSTR getBestPilot(int32_t mechWeight);
	bool gotPilotsLeft(void);

	void setPilotUnused(PCSTR pPilot);

	int32_t getVariantsInInventory(
		LogisticsVariant* pVar, bool bIncludeForceGroup);
	int32_t getPlayerVariantNames(PCSTR*, int32_t& count);
	int32_t getChassisVariants(const LogisticsChassis* pChassis,
		LogisticsVariant** pVar, int32_t& count);

	LogisticsVariant* getVariant(int32_t ID);
	LogisticsVariant* getVariant(PCSTR mechName);
	LogisticsVariant* getVariant(PCSTR pCSVFileNaem, int32_t VariantNum);
	int32_t removeVariant(PCSTR varName);

	LogisticsMech* getMech(int32_t ID);
	LogisticsMech* getMech(PCSTR MechName, PCSTR pilotName);
	LogisticsMech* getMechWithoutForceGroup(LogisticsMech* clone);

	void addMechToInventory(LogisticsVariant* pVar, LogisticsPilot* pPilot,
		int32_t ForceGrup, bool bSubtractPts = 0);

	int32_t createInstanceID(LogisticsVariant* pVar);
	LogisticsPilot* getFirstAvailablePilot(void);
	void getForceGroup(EList<LogisticsMech*, LogisticsMech*>& newList);
	void getInventory(EList<LogisticsMech*, LogisticsMech*>& newList);

	void removeMechsInForceGroup(
		void); // takes mechs that are in the force group out of the inventory
	void addMechToInventory(LogisticsVariant* pVar, int32_t addToForceGroup,
		LogisticsPilot*, uint32_t color1 = 0xffffffff,
		uint32_t color2 = 0xffffffff, uint32_t color3 = 0xffffffff);
	void removeMechFromInventory(PCSTR mechName, PCSTR pilotName);

	int32_t addMechToForceGroup(LogisticsMech* pMech, int32_t slot);
	int32_t removeMechFromForceGroup(LogisticsMech* pMech, bool bRemovePilot);
	int32_t removeMechFromForceGroup(int32_t slot);

	bool canAddMechToForceGroup(LogisticsMech* pMech);

	int32_t save(FitIniFile& file);
	int32_t load(FitIniFile& file);

	void setResourcePoints(int32_t rp)
	{
		resourcePoints = rp;
		rpJustAdded	= 0;
	}
	int32_t getResourcePoints() { return resourcePoints; }
	void addResourcePoints(int32_t amount)
	{
		resourcePoints += amount;
		rpJustAdded = true;
	}
	void decrementResourcePoints(int32_t amount) { resourcePoints -= amount; }

	int32_t getCBills(void);
	void addCBills(int32_t amount);
	void decrementCBills(int32_t amount);

	int32_t comparePilots(
		LogisticsPilot* p1, LogisticsPilot* p2, int32_t weight);
	int32_t getCurrentDropWeight(void) const;
	int32_t getMaxDropWeight(void) const;

	int32_t loadMech(FitIniFile& file, int32_t& count);

	void setMissionCompleted(void);
	int32_t updateAvailability(void);

	const EString& getCurrentMission(void) const;
	const EString& getLastMission(void) const;
	int32_t setCurrentMission(const EString& missionName);
	void setSingleMission(PCSTR pName);
	bool isSingleMission(void);
	int32_t getCurrentMissionTune(void);
	PCSTR getCurrentABLScript(void) const;
	int32_t getCurrentMissionId(void);

	void clearInventory(void);

	int32_t getPurchasableMechs(LogisticsVariant**, int32_t& count);
	int32_t canPurchaseMech(LogisticsVariant* pVar);
	int32_t purchaseMech(LogisticsVariant* pVar);
	int32_t sellMech(LogisticsMech* pVar);
	int32_t getEncyclopediaMechs(const LogisticsVariant**, int32_t& count);
	int32_t getHelicopters(const LogisticsVariant**, int32_t& count);

	int32_t getVehicles(const LogisticsVehicle**, int32_t& count);
	LogisticsVehicle* getVehicle(PCSTR pName);

	int32_t setMechToModify(LogisticsMech* pVariant);
	LogisticsMech* getMechToModify() { return currentlyModifiedMech; }
	int32_t acceptMechModifications(PCSTR pNewVariantName);
	int32_t cancelMechModfications(void);
	bool canReplaceVariant(PCSTR name);
	bool canDeleteVariant(PCSTR name);

	void setCurrentMissionNum(int32_t cMission);
	int32_t getCurrentMissionNum(void);

	int32_t getAvailableComponents(
		LogisticsComponent** pComps, int32_t& maxCount);
	int32_t getAllComponents(LogisticsComponent** pComps, int32_t& maxCount);
	PCSTR getCurrentOperationFileName(void);
	PCSTR getCurrentVideoFileName(void);
	PCSTR getCurrentMissionDescription(void);
	int32_t getAvailableMissions(PCSTR* missionNames, int32_t& count);
	int32_t getCurrentMissions(PCSTR* missionNames, int32_t& count);
	bool getMissionAvailable(PCSTR missionName);
	PCSTR getCurrentMissionFriendlyName();
	PCSTR getMissionFriendlyName(PCSTR missionName);
	int32_t getMaxTeams(void) const;
	int32_t getMaxPlayers(void) const;

	int32_t acceptMechModificationsUseOldVariant(PCSTR name);

	const EString& getCampaignName(void) const;

	int32_t setCurrentMission(PCSTR missionName);

	void startNewCampaign(PCSTR fileName = "campaign");
	void startMultiPlayer(void);
	void setPurchaseFile(PCSTR fileName);

	Building* getBuilding(int32_t nameID);
	int32_t getBuildings(Building** bdgs, int32_t& count);

	bool campaignOver(void);
	PCSTR getCurrentBigVideo(void) const;
	PCSTR getFinalVideo(void) const;

	bool newMechsAvailable() { return bNewMechs; }
	bool newPilotsAvailable() { return bNewPilots; }
	bool newWeaponsAvailable() { return bNewWeapons; }

	void setNewPilotsAcknowledged() { bNewPilots = 0; }
	void setNewWeaponsAcknowledged() { bNewWeapons = 0; }
	void setNewMechsAcknowledged() { bNewMechs = 0; }

	void addNewBonusPurchaseFile(PCSTR pFileName);
	void appendAvailability(PCSTR pFileName, bool* availableArray);

	bool skipLogistics(void);
	bool showChooseMission(void);
	bool skipPilotReview(void);
	bool skipSalvageScreen(void);
	bool skipPurchasing(void);

	bool canHaveSalavageCraft(void);
	bool canHaveRepairTruck(void);
	bool canHaveScoutCopter(void);
	bool canHaveArtilleryPiece(void);
	bool canHaveAirStrike(void);
	bool canHaveSensorStrike(void);
	bool canHaveMineLayer(void);

	bool getVideoShown(void);
	void setVideoShown(void);

	struct Building
	{
		int32_t nameID;
		int32_t weight;
		int32_t encycloID;
		int32_t componentIDs[4];
		char fileName[64];
		float scale;
	};

  private:
	bool bNewMechs;
	bool bNewPilots;
	bool bNewWeapons;

	typedef EList<LogisticsComponent, const LogisticsComponent&> COMPONENT_LIST;
	typedef EList<LogisticsPilot, const LogisticsPilot&> PILOT_LIST;
	typedef EList<LogisticsVariant*, LogisticsVariant*> VARIANT_LIST;
	typedef EList<LogisticsMech*, LogisticsMech*> MECH_LIST;
	typedef EList<LogisticsVehicle*, LogisticsVehicle*> VEHICLE_LIST;
	typedef EList<Building, const Building&> BUILDING_LIST;

	VARIANT_LIST variants;
	MECH_LIST inventory;
	COMPONENT_LIST components;
	PILOT_LIST pilots;
	VEHICLE_LIST vehicles;
	BUILDING_LIST buildings;

	LogisticsMissionInfo* missionInfo;

	LogisticsMech* currentlyModifiedMech;
	LogisticsVariant* oldVariant;

	int32_t resourcePoints; // C-Bills for buying mechs

	int32_t loadVariant(FitIniFile& file);

	// HELPERS
	void RegisterFunctions(void);
	void UnRegisterFunctions(void);

	void initComponents(void);
	void initPilots(void);
	void initVariants(void);

	void addVehicle(int32_t fitID, PacketFile& objectFile, float scale);
	int32_t addBuilding(int32_t fitID, PacketFile& objectFile, float scale);
	void removeDeadWeight(void);
	void clearVariants(void);
};

//*************************************************************************************************
#endif // end of file ( LogisticsData.h )
