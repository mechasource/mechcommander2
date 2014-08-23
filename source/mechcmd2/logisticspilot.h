
#pragma once

#ifndef LOGISTICSPILOT_H
#define LOGISTICSPILOT_H

//===========================================================================//
//LogisticsPilot.h			: Interface for the LogisticsPilot component.    //
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
#include "estring.h"
#include "elist.h"
#include "warrior.h"

#define MAX_MISSIONS		50

enum Medals
{

	CAMPAIGN_RIBBON1 = 0,
	CAMPAIGN_RIBBON2 = 1,
	CAMPAIGN_RIBBON3 = 2,
	CAMPAIGN_FULLRIBBON1 = 3,
	CAMPAIGN_FULLRIBBON2 = 4,
	CAMPAIGN_FULLRIBBON3 = 5,
	PURPLE_HEART = 6,
	VALOR = 7,
	UNCOMMON_VALOR = 8,
	MISSION_SPEC1 = 9,
	MISSION_SPEC2 = 10,
	MISSION_SPEC3 = 11,
	MISSION_SPEC4 = 12,
	LIAO_MEDAL = 13,
	DAVION_MEDAL = 14,
	STEINER_MEDAL = 15,
	MAX_MEDAL

};


class LogisticsPilot
{
	public:

		int32_t init( PSTR fileName );
		LogisticsPilot();
		~LogisticsPilot();


		const EString&  getName() const { return name; }
		const EString&	 getAudio() const { return audio; }
		const EString& getVideo() const { return video; }
		int32_t			getRank() const { return rank; }
		const EString& getIconFile() const { return iconFile; }
		float			getGunnery() const { return gunnery; }
		float			getPiloting() const { return piloting; }
		int32_t			getMechKills() const { return mechKills; }
		int32_t			getVehicleKills() const { return vehicleKills; }
		int32_t			getInfantryKills() const { return infantryKills; }
		int32_t			getID() const { return id; }
		int32_t			getFlavorTextID() const { return flavorTextID; }
		int32_t			getNumberMissions() const;
		bool		getMissionPlayed (int32_t missionNum)
		{
			if ((missionNum >= 0) && (missionNum < MAX_MISSIONS))
				return (missionsPlayed[missionNum] == 1);

			return false;
		}

		bool		isUsed() const{ return bIsUsed; }
		void		setUsed( bool bUsed ){ bIsUsed = bUsed; }
		int32_t			getNewGunnery() const { return newGunnery; }
		int32_t			getNewPiloting() const { return newPiloting; }
		
		void		setDead();
		bool		isAvailable(){ return (bAvailable&&!bDead); } // depends on purchasing file .... maybe should put dead checks and stuff
		void		setAvailable( bool available ) { bAvailable = available; }
		bool		isDead() const { return bDead; }
		bool		justDied() const { return bJustDied; }
		void		setJustDied( bool bdied){ bJustDied = bdied; }
		int32_t		getPhotoIndex() const { return photoIndex; }

		bool		promotePilot();
		int32_t			turnAverageIntoRank( float avg);



		static		PCSTR getSkillText( int32_t skillID );
		const EString&	getFileName(){ return fileName; }

		int32_t		save( FitIniFile& file, int32_t count );
		int32_t		load( FitIniFile& file );

		int32_t		update( MechWarrior* pWarrior );

		int32_t			getSpecialtySkillCount() const;
		int32_t			getSpecialtySkills( PCSTR* array, int32_t& count );
		int32_t			getSpecialtySkills( pint32_t array, int32_t& count );
		bool		*getSpecialtySkills() { return specialtySkills;}
		void		setSpecialtySkill( int32_t skill, bool set );

		const bool*			getNewMedalArray() const{ return medalsLastMission;}
		const bool*			getMedalArray() const{ return medals;}


		void		clearIcons();

		
		uint32_t	id;
		EList< ForceGroupIcon*, ForceGroupIcon* >	killedIcons;



	private:
		EString		name;
		EString		audio;
		EString		video;
		int32_t			rank;
		EString		iconFile; // or file name?
		float			gunnery;
		float			piloting;
		int32_t			mechKills;
		int32_t			vehicleKills;
		int32_t			infantryKills;
		int32_t			missionsCompleted;
		uint8_t	missionsPlayed[MAX_MISSIONS];
		int32_t			flavorTextID;
		bool			bIsUsed;
		bool			bDead;
		bool			bAvailable;
		bool			bJustDied;
		int32_t			photoIndex;
		float			newPiloting;
		float			newGunnery;
		bool			specialtySkills[NUM_SPECIALTY_SKILLS];

		static char skillTexts[][255];
		EString		fileName;

		bool		medals[MAX_MEDAL];
		bool		medalsLastMission[MAX_MEDAL];


		// last second hack
		friend class LogisticsData;


};


//*************************************************************************************************
#endif  // end of file ( LogisticsPilot.h )
