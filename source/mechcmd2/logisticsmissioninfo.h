/*************************************************************************************************\
LogisticsMissionInfo.h			: Interface for the LogisticsMissionInfo component.
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
		
		
		int32_t init( FitIniFile& file ); // init campaign
		void save( FitIniFile& file );
		int32_t load( FitIniFile& file );// init previously saved stuff


		int32_t getAvailableMissions( PCSTR* missions, int32_t& numberOfEm );
		int32_t getCurrentMissions( PCSTR* missions, int32_t& numberOfEm );

		bool getMissionAvailable( PCSTR missionName );

		bool isMissionComplete( PCSTR missionName );
		bool isSingleMission() const;

		int32_t setNextMission( PCSTR missionName );
		void setMissionComplete( );

		int32_t getCurrentLogisticsTuneId();

		int32_t getCurrentMissionId();
		
		const EString& getCurrentPurchaseFile() const;
		const EString& getCurrentMission() const { return currentMissionName; }
		const EString& getLastMission() const { return lastMissionName; }

		int32_t				getCurrentDropWeight() const;
		PCSTR			getCurrentVideo() const;
		PCSTR			getCurrentOperationFile() const;
		PCSTR			getCurrentMissionDescription() const;
		PCSTR			getCurrentMissionFriendlyName() const;
		PCSTR			getMissionFriendlyName( PCSTR missionName ) const;
		PCSTR			getCurrentABLScriptName() const;

		int32_t					getCurrentRP() const;
		int32_t					getCBills() const { return CBills; }
		void				incrementCBills(int32_t amount) { CBills += amount; }
		void				decrementCBills(int32_t amount) { CBills -= amount; }

		const EString& 		getCampaignName( ) const { return campaignName; }
		const EString&		getCampaignDisplayName() const { return campaignDisplayName; }

		bool				campaignOver() const;

		PCSTR			getCurrentBigVideo() const;
		PCSTR			getFinalVideo() const;

		void				setMultiplayer();
		void				setPurchaseFile( PCSTR fileName );

//		int32_t				getMaxTeams() const { return maxTeams; }
//		int32_t				getMaxPlayers() const { return maxPlayers; }

		int32_t					getAdditionalPurachaseFiles( PCSTR* list, int32_t& maxCount );
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

		int32_t				getCurrentMissionNumber()
		{
			return currentMission;
		}

		void				setCurrentMissionNumber(int32_t cMission)
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
		typedef EList< PSTR, PSTR > FILE_LIST;


		class MissionGroup
		{
		public:
			int32_t numberToBeCompleted;
			EString videoFileName;
			EString operationFileName;
			EString bigVideoName;
			EString ablBrainName;
			int32_t logisticsTuneId;
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
		int32_t					currentStage;
		int32_t					currentMission;

		int32_t					lastStage;
		int32_t					lastMission;
		EString					lastMissionName;

		MissionGroup*			groups;
		int32_t					groupCount;

		int32_t					CBills;

		void					clear();
		bool					bMultiplayer;

		FILE_LIST				additionalPurchaseFiles;

		LogisticsMissionInfo::MissionInfo* getPreviousMission();
		void readMissionInfo( FitIniFile& file, LogisticsMissionInfo::MissionInfo* pInfo );




		// HELPER FUNCTIONS

};


//*************************************************************************************************
#endif  // end of file ( LogisticsMissionInfo.h )
