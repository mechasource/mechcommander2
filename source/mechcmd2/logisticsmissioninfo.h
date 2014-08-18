#ifndef LOGISTICSMISSIONINFO_H
#define LOGISTICSMISSIONINFO_H
/*************************************************************************************************\
LogisticsMissionInfo.h			: Interface for the LogisticsMissionInfo component.
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//
\*************************************************************************************************/

#ifndef ELIST_H
#include "elist.h"
#endif

#include "estring.h"

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

		LogisticsMissionInfo();
		~LogisticsMissionInfo();
		
		
		long init( FitIniFile& file ); // init campaign
		void save( FitIniFile& file );
		long load( FitIniFile& file );// init previously saved stuff


		long getAvailableMissions( PCSTR* missions, int& numberOfEm );
		long getCurrentMissions( PCSTR* missions, int& numberOfEm );

		bool getMissionAvailable( PCSTR missionName );

		bool isMissionComplete( PCSTR missionName );
		bool isSingleMission() const;

		long setNextMission( PCSTR missionName );
		void setMissionComplete( );

		long getCurrentLogisticsTuneId();

		long getCurrentMissionId();
		
		const EString& getCurrentPurchaseFile() const;
		const EString& getCurrentMission() const { return currentMissionName; }
		const EString& getLastMission() const { return lastMissionName; }

		long				getCurrentDropWeight() const;
		PCSTR			getCurrentVideo() const;
		PCSTR			getCurrentOperationFile() const;
		PCSTR			getCurrentMissionDescription() const;
		PCSTR			getCurrentMissionFriendlyName() const;
		PCSTR			getMissionFriendlyName( PCSTR missionName ) const;
		PCSTR			getCurrentABLScriptName() const;

		int					getCurrentRP() const;
		int					getCBills() const { return CBills; }
		void				incrementCBills(int amount) { CBills += amount; }
		void				decrementCBills(int amount) { CBills -= amount; }

		const EString& 		getCampaignName( ) const { return campaignName; }
		const EString&		getCampaignDisplayName() const { return campaignDisplayName; }

		bool				campaignOver() const;

		PCSTR			getCurrentBigVideo() const;
		PCSTR			getFinalVideo() const;

		void				setMultiplayer();
		void				setPurchaseFile( PCSTR fileName );

//		long				getMaxTeams() const { return maxTeams; }
//		long				getMaxPlayers() const { return maxPlayers; }

		int					getAdditionalPurachaseFiles( PCSTR* list, long& maxCount );
		void				addBonusPurchaseFile( PCSTR fileName ); // extra bonus

		bool				skipLogistics();
		bool				showChooseMission();

		bool				skipPilotReview();
		bool				skipSalvageScreen();
		bool				skipPurchasing();

		void				setSingleMission( PCSTR pName );

		bool				canHaveSalavageCraft();
		bool				canHaveRepairTruck();
		bool				canHaveScoutCopter();
		bool				canHaveArtilleryPiece();
		bool				canHaveAirStrike();
		bool				canHaveSensorStrike();
		bool				canHaveMineLayer();
		bool				getVideoShown();
		void				setVideoShown();

		long				getCurrentMissionNumber()
		{
			return currentMission;
		}

		void				setCurrentMissionNumber(long cMission)
		{
			currentMission = cMission;
		}


	
	private:

	
		class MissionInfo
		{
		public:
			~MissionInfo();
			EString missionDescriptiveName;
			EString fileName;
			EString description;
			EString purchaseFileName;
			EString videoName;
			bool mandatory;
			bool completePrevious;
			bool completed;
			long resourcePoints;
			long additionalCBills;
			long dropWeight;
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
		typedef EList< char*, char* > FILE_LIST;


		class MissionGroup
		{
		public:
			long numberToBeCompleted;
			EString videoFileName;
			EString operationFileName;
			EString bigVideoName;
			EString ablBrainName;
			long logisticsTuneId;
			MISSION_LIST infos;
			bool	bigVideoShown;
			bool	videoShown;
		};


		// DATA

		EString					campaignName;
		EString					campaignDisplayName;
		EString					playerName;
		EString					currentMissionName;
		EString					finalVideoName;
		long					currentStage;
		long					currentMission;

		long					lastStage;
		long					lastMission;
		EString					lastMissionName;

		MissionGroup*			groups;
		long					groupCount;

		long					CBills;

		void					clear();
		bool					bMultiplayer;

		FILE_LIST				additionalPurchaseFiles;

		LogisticsMissionInfo::MissionInfo* getPreviousMission();
		void readMissionInfo( FitIniFile& file, LogisticsMissionInfo::MissionInfo* pInfo );




		// HELPER FUNCTIONS

};


//*************************************************************************************************
#endif  // end of file ( LogisticsMissionInfo.h )
