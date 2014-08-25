
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

#include <mechgui/asystem.h>
#include <mechgui/alistbox.h>
#include <mechgui/aanim.h>
#include "abl.h"

class LogisticsScreen;
class MainMenu;

class MissionBegin
{
	public:

		//Tutorial - SAVE with savegame, please!!
		static bool 				FirstTimePurchase;
		static bool					FirstTimeMechLab;

		MissionBegin(void);
		~MissionBegin(void);

		void init(void);

		PCSTR update(void);
		void render(void);

		void begin(void);
		void end(void);
		bool isDone(){ return bDone; }
		bool readyToLoad() { return bReadyToLoad; }

		void beginSplash( PCSTR playerName = 0 );

		void beginMPlayer(void);
		void beginSPlayer(void);
		void beginZone(void);
		void setToMissionBriefing(void);

		int32_t getCurrentScreenId(void);	//Returns screen ID as a function of curScreenX and curScreenY
		bool startAnimation (int32_t bId, bool isButton, float scrollTime, int32_t nFlashes);

		bool isInCalloutAnimation()
		{
			return animationRunning;
		}

		void beginAtConnectionScreen(void);

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

		void						setUpMultiplayerLogisticsScreens(void);

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
