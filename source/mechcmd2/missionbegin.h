
#pragma once

#ifndef MISSIONNBEGIN_H
#define MISSIONNBEGIN_H
/*************************************************************************************************\
MissionBegin.h			: Interface for the MissionBegin component.
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//
\*************************************************************************************************/

//*************************************************************************************************

/**************************************************************************************************
CLASS DESCRIPTION
MissionBegin:
**************************************************************************************************/

#include "asystem.h"
#include "alistbox.h"
#include "aanim.h"
#include "abl.h"

class LogisticsScreen;
class MainMenu;

class MissionBegin
{
	public:

		//Tutorial - SAVE with savegame, please!!
		static bool 				FirstTimePurchase;
		static bool					FirstTimeMechLab;

		MissionBegin();
		~MissionBegin();

		void init();

		PCSTR update();
		void render();

		void begin();
		void end();
		bool isDone(){ return bDone; }
		bool readyToLoad() { return bReadyToLoad; }

		void beginSplash( PCSTR playerName = 0 );

		void beginMPlayer();
		void beginSPlayer();
		void beginZone();
		void setToMissionBriefing();

		int32_t getCurrentScreenId();	//Returns screen ID as a function of curScreenX and curScreenY
		bool startAnimation (int32_t bId, bool isButton, float scrollTime, int32_t nFlashes);

		bool isInCalloutAnimation()
		{
			return animationRunning;
		}

		void beginAtConnectionScreen();

		void restartMPlayer( PCSTR playerName);

	private:

		bool						bDone;
		bool						bReadyToLoad;

		ABLModulePtr				logisticsBrain;
		int32_t						logisticsScriptHandle;

		aAnimation					leftAnim;
		aAnimation					rightAnim;
		aAnimation					upAnim;
		aAnimation					downAnim;
	
		bool						bMultiplayer;

		LogisticsScreen*			multiplayerScreens[5/*dim screen X*/][3/*dim screen Y*/];
		LogisticsScreen*			singlePlayerScreens[5/*dim screen X*/][3/*dim screen Y*/];
		LogisticsScreen*			screens[5/*dim screen X*/][3/*dim screen Y*/];
		LogisticsScreen*			placeHolderScreen;

		int32_t						curScreenX;
		int32_t						curScreenY;

		MainMenu*					mainMenu;
		bool						bSplash;

		bool						animJustBegun;

		void						setUpMultiplayerLogisticsScreens();

		//-----------------------------------------------
		// Tutorial Data
		bool						animationRunning;
		float						timeLeftToScroll;
		int32_t						targetButtonId;
		bool						targetIsButton;
		int32_t						buttonNumFlashes;
		float						buttonFlashTime;
};





//*************************************************************************************************
#endif  // end of file ( MissionBegin.h )
