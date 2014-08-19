#ifndef LOGISTICSDATA_H
#define LOGISTICSDATA_H
/*************************************************************************************************\
LogisticsData.h			: Interface for the LogisticsData component.  This thing holds the inventory
							lists and the component lists
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//
\*************************************************************************************************/

#include "stuff\stuff.hpp"
#include <EList.h>

#include "LogisticsMech.h"
#include "LogisticsComponent.h"
#include "LogisticsPilot.h"

class FitIniFile;
class LogisticsMissionInfo;
class PacketFile;

class LogisticsData
{

public:

	struct Building;

	LogisticsData();
	~LogisticsData();
	void			init();

	bool			rpJustAdded;

	static LogisticsData*		instance;

	LogisticsComponent* getComponent( int componentID );
	LogisticsPilot*		getPilot( int pilotID );
	LogisticsPilot*		getPilot( PCSTR pilotName );
	int					getPilotCount();
	int					getPilots( LogisticsPilot**, long& count );
	PCSTR			getBestPilot( long mechWeight );
	bool				gotPilotsLeft();

	void				setPilotUnused( PCSTR pPilot );

	int					getVariantsInInventory( LogisticsVariant* pVar, bool bIncludeForceGroup );
	int					getPlayerVariantNames( PCSTR*, int& count );
	int					getChassisVariants( const LogisticsChassis* pChassis, LogisticsVariant** pVar, int& count );
	
	LogisticsVariant*	getVariant( int ID );
	LogisticsVariant*	getVariant( PCSTR mechName );
	LogisticsVariant*	getVariant( PCSTR pCSVFileNaem, int VariantNum );
	int					removeVariant( PCSTR varName );


	LogisticsMech*		getMech( int ID );
	LogisticsMech*		getMech( PCSTR MechName, PCSTR pilotName );
	LogisticsMech*		getMechWithoutForceGroup( LogisticsMech* clone );

	void				addMechToInventory( LogisticsVariant* pVar, LogisticsPilot* pPilot, int ForceGrup,
											bool bSubtractPts = 0);




	int createInstanceID( LogisticsVariant* pVar );
	LogisticsPilot*		getFirstAvailablePilot();
	void				getForceGroup( EList<LogisticsMech*, LogisticsMech*>& newList );
	void				getInventory( EList<LogisticsMech*, LogisticsMech*>& newList );


	void				removeMechsInForceGroup(); // takes mechs that are in the force group out of the inventory
	void				addMechToInventory( LogisticsVariant* pVar,
											int addToForceGroup, LogisticsPilot*,
											ULONG color1 = 0xffffffff, 
											ULONG color2 = 0xffffffff,
											ULONG color3 = 0xffffffff);
	void				removeMechFromInventory( PCSTR mechName, PCSTR pilotName );

	int					addMechToForceGroup( LogisticsMech* pMech, int slot );
	int					removeMechFromForceGroup( LogisticsMech* pMech, bool bRemovePilot );
	int					removeMechFromForceGroup( int slot );

	bool				canAddMechToForceGroup( LogisticsMech* pMech );


	long				save( FitIniFile& file );
	long				load( FitIniFile& file );

	void				setResourcePoints(long rp) {resourcePoints = rp; rpJustAdded = 0;}
	int					getResourcePoints() { return resourcePoints; }
	void				addResourcePoints( int amount ){ resourcePoints+= amount; rpJustAdded = true;}
	void				decrementResourcePoints( int amount ){ resourcePoints -= amount; }

	int					getCBills();
	void				addCBills( int amount );
	void				decrementCBills( int amount );

	int					comparePilots( LogisticsPilot* p1, LogisticsPilot* p2, long weight );
	int					getCurrentDropWeight() const;
	int					getMaxDropWeight() const;

	long				loadMech( FitIniFile& file, int& count );

	void				setMissionCompleted();
	long				updateAvailability();

	const EString&		getCurrentMission() const;
	const EString&		getLastMission() const;
	int					setCurrentMission( const EString& missionName );
	void				setSingleMission( PCSTR pName );
	bool				isSingleMission();
	long				getCurrentMissionTune();
	PCSTR 		getCurrentABLScript() const;
	long 				getCurrentMissionId();

	void				clearInventory();

	int					getPurchasableMechs( LogisticsVariant**, int& count );
	int					canPurchaseMech( LogisticsVariant* pVar );
	int					purchaseMech( LogisticsVariant* pVar );
	int					sellMech( LogisticsMech* pVar );
	int					getEncyclopediaMechs( const LogisticsVariant**, int& count );
	int					getHelicopters( const LogisticsVariant**, int& count );
	
	int					getVehicles( const LogisticsVehicle**, int& count );
	LogisticsVehicle*	getVehicle( PCSTR pName );


	int					setMechToModify( LogisticsMech* pVariant );
	LogisticsMech*		getMechToModify( ){ return currentlyModifiedMech; }
	int					acceptMechModifications( PCSTR pNewVariantName );
	int					cancelMechModfications();
	bool				canReplaceVariant( PCSTR name );
	bool				canDeleteVariant( PCSTR name );

	void				setCurrentMissionNum (long cMission);
	long				getCurrentMissionNum (void);

	int					getAvailableComponents( LogisticsComponent** pComps, int& maxCount );
	int					getAllComponents( LogisticsComponent** pComps, int& maxCount );
	PCSTR			getCurrentOperationFileName();
	PCSTR			getCurrentVideoFileName();
	PCSTR			getCurrentMissionDescription();
	int					getAvailableMissions( PCSTR* missionNames, long& count );
	int					getCurrentMissions( PCSTR* missionNames, long& count );
	bool				getMissionAvailable( PCSTR missionName );
	PCSTR			getCurrentMissionFriendlyName( );
	PCSTR			getMissionFriendlyName( PCSTR missionName );
	long				getMaxTeams() const;
	long				getMaxPlayers() const;

	int					acceptMechModificationsUseOldVariant( PCSTR name );

	const EString&		getCampaignName() const;




	int					setCurrentMission( PCSTR missionName );

	void				startNewCampaign( PCSTR fileName = "campaign");
	void				startMultiPlayer();
	void				setPurchaseFile( PCSTR fileName );

	Building*			getBuilding( int nameID );
	int					getBuildings( Building** bdgs, int& count );

	bool				campaignOver();
	PCSTR			getCurrentBigVideo() const;
	PCSTR			getFinalVideo() const;

	bool				newMechsAvailable() { return bNewMechs; }
	bool				newPilotsAvailable() { return bNewPilots; }
	bool				newWeaponsAvailable() { return bNewWeapons; }

	void				setNewPilotsAcknowledged(){ bNewPilots = 0; }
	void				setNewWeaponsAcknowledged(){ bNewWeapons = 0; }
	void				setNewMechsAcknowledged(){ bNewMechs = 0; }

	void				addNewBonusPurchaseFile( PCSTR pFileName ); 
	void				appendAvailability(PCSTR pFileName, bool* availableArray );

	bool				skipLogistics();
	bool				showChooseMission();
	bool				skipPilotReview();
	bool				skipSalvageScreen();
	bool				skipPurchasing();

	bool				canHaveSalavageCraft();
	bool				canHaveRepairTruck();
	bool				canHaveScoutCopter();
	bool				canHaveArtilleryPiece();
	bool				canHaveAirStrike();
	bool				canHaveSensorStrike();
	bool				canHaveMineLayer();

	bool				getVideoShown();
	void				setVideoShown();




	struct Building
	{
		long nameID;
		long weight;
		long encycloID;
		long componentIDs[4];
		char fileName[64];
		float scale;
	};




private:

	bool		bNewMechs;
	bool		bNewPilots;
	bool		bNewWeapons;

	typedef EList< LogisticsComponent, const LogisticsComponent& >	COMPONENT_LIST;
	typedef EList< LogisticsPilot, const LogisticsPilot& >			PILOT_LIST;
	typedef EList< LogisticsVariant*, LogisticsVariant* >			VARIANT_LIST;
	typedef EList< LogisticsMech*, LogisticsMech* >					MECH_LIST;
	typedef EList< LogisticsVehicle*, LogisticsVehicle* >			VEHICLE_LIST;
	typedef EList< Building, const Building& >						BUILDING_LIST;
	
	VARIANT_LIST									variants;
	MECH_LIST										inventory;
	COMPONENT_LIST									components;
	PILOT_LIST										pilots;
	VEHICLE_LIST									vehicles;
	BUILDING_LIST									buildings;
	
	LogisticsMissionInfo*							missionInfo;
	

	LogisticsMech*									currentlyModifiedMech;
	LogisticsVariant*								oldVariant;
	
	
	int												resourcePoints; // C-Bills for buying mechs

	long	loadVariant( FitIniFile& file );


	

	// HELPERS
	void RegisterFunctions();
	void UnRegisterFunctions();

	void initComponents();
	void initPilots();
	void initVariants();

	void addVehicle( long fitID, PacketFile& objectFile, float scale );
	int  addBuilding( long fitID, PacketFile& objectFile, float scale );
	void removeDeadWeight();
	void clearVariants();




	

};


//*************************************************************************************************
#endif  // end of file ( LogisticsData.h )
