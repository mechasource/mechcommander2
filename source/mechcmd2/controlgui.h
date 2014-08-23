/*************************************************************************************************\
ControlGui.h			: Interface for the ControlGui component.  This thing holds the tac map
and everything else on the left hand side of the screen.
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//
\*************************************************************************************************/

#pragma once

#ifndef CONTROLGUI_H
#define CONTROLGUI_H

#include "gametacmap.h"
#include "forcegroupbar.h"
#include "mover.h"
#include "utilities.h"
#include "pausewindow.h"
#include <mechgui/afont.h>
#include <mechgui/aedit.h>
#include "mc2movie.h"
#include "mpstats.h"

class CObjective;
class InfoWindow;
class PauseWindow;

#define LAST_VEHICLE			(MAX_VEHICLE - LARGE_AIRSTRIKE)

/**************************************************************************************************
CLASS DESCRIPTION
ControlGui:
**************************************************************************************************/

struct	ButtonData {
	uint32_t		ID;
	int32_t			helpTextHeader;
	int32_t			helpTextID;
	int32_t			textID;
	int32_t			textColors[4];
	aFont			textFont;
	char			fileName[32];
	int32_t			stateCoords[4][2];	
	int32_t			textureWidth;
	int32_t			textureHeight;
	int				fileWidth;
	int				fileHeight;
	ULONG	textureHandle;
	bool			textureRotated;
};

class ControlButton
{
public:
	gos_VERTEX		location[4];
	int32_t ID;

	ButtonData*			data;
	int					state;

	void render();
	void press( bool );
	void toggle();
	void disable( bool );
	bool isEnabled();
	void makeAmbiguous( bool bAmbiguous );
	void hide( bool );
	void move( float deltaX, float deltaY );
	void setColor( ULONG newColor );
	static void makeUVs( gos_VERTEX* vertices, int State, ButtonData& data );

	static void initButtons( FitIniFile& file, int32_t buttonCount, 
		ControlButton* buttons, ButtonData* buttonData, PCSTR str, aFont* font = 0 );


	enum States {
		ENABLED = 0,
		PRESSED,
		DISABLED,
		AMBIGUOUS,
		HIDDEN
	};
};

class ControlGui
{

public:

	static ControlGui*	instance;

	static int32_t			hiResOffsetX;
	static int32_t			hiResOffsetY;

	ControlGui();
	~ControlGui();
	bool inRegion( int mouseX, int mouseY, bool bPaused );
	void render( bool bPaused );
	void update( bool bPaused, bool bLOS );
	void initTacMapBuildings( puint8_t data, int size ){ tacMap.initBuildings( data, size ); }
	void initTacMap( puint8_t data, int size ){ tacMap.init( data, size ); }
	void initMechs();
	void unPressAllVehicleButtons();
	void disableAllVehicleButtons();
	void addMover (MoverPtr mover);
	void removeMover (MoverPtr mover);
	int  updateChat();

	void beginPause();
	void endPause();

	bool resultsDone();

	void startObjectives( bool bStart );
	bool objectivesStarted() { return renderObjectives; }

	void setInfoWndMover( Mover* pMover );
	void setVehicleCommand( bool );
	bool getVehicleCommand( );

	void playMovie( PCSTR fileName );
	bool isMoviePlaying();

	bool playPilotVideo( MechWarrior* pPilot, char movieCode );
	void endPilotVideo();
	bool isSelectingInfoObject();

	bool isOverTacMap();

	bool isChatting() { return bChatting; }

	//TUTORIAL
	bool animateTacMap (int32_t buttonId,float timeToScroll,int32_t numFlashes);
	bool pushButton (int32_t buttonId);
	bool flashRPTotal (int32_t numFlashes);

	PCSTR getVehicleName( int32_t& ID );
	PCSTR getVehicleNameFromID (int32_t ID);
	void swapResolutions( int newResolution );

	GameTacMap						tacMap;
	ForceGroupBar					forceGroupBar;

	bool				mouseInVehicleStopButton;



	typedef enum __controlgui_const  {
		OBJECTVE_MOVE_COUNT			= 2,
		RESULTS_MOVE_COUNT			= 3,
		MAX_CHAT_COUNT				= 5,
		RPTOTAL_CALLOUT				= 21185,
	};

	enum Commands
	{
		DEFAULT_RANGE = 0,
		SHORT_RANGE ,
		MED_RANGE,
		LONG_RANGE,
		JUMP_COMMAND,
		RUN_COMMAND,
		GUARD_COMMAND,
		FIRE_FROM_CURRENT_POS,
		STOP_COMMAND,
		INFO_COMMAND,
		OBJECTIVES_COMMAND,
		SAVE_COMMAND,
		TACMAP_TAB,
		INFO_TAB,
		VEHICLE_TAB,
		LAYMINES,
		REPAIR,
		SALVAGE,
		GUARDTOWER,
		CUR_RANGE, // fire from right where I AM
		LAST_COMMAND

	};

	enum VehicleCommands
	{
		LARGE_AIRSTRIKE = 100,
		GUARD_TOWER,
		SENSOR_PROBE,
		REPAIR_VEHICLE,
		PEGASUS_SCOUT,
		MINELAYER,
		RECOVERY_TEAM,	
		STOP_VEHICLE,
		MAX_VEHICLE
	};

	bool	isDefaultSpeed();
	void	toggleDefaultSpeed( );
	void	toggleJump( );
	bool	getJump( );
	bool	getWalk( );
	bool	getRun( );
	void	toggleGuard( );
	bool	getGuard( );
	void	setDefaultSpeed();
	void	toggleHoldPosition();

	void	setRange( int Range );
	void	doStop();
	void	toggleFireFromCurrentPos();
	bool	getFireFromCurrentPos(){ return fireFromCurrentPos; }
	void    setFireFromCurrentPos( bool bset ) { fireFromCurrentPos = bset; }
	bool	isAddingVehicle(){ return addingVehicle; }
	bool	isAddingAirstrike() { return addingArtillery; }
	bool	isAddingSalvage() { return addingSalvage; }
	bool	isButtonPressed( int ID ) { return getButton( ID )->state & ControlButton::PRESSED; }
	bool	getMines();
	bool	getSalvage();
	bool	getRepair();
	bool	getGuardTower();
	void	switchTabs( int direction );
	void	renderObjective( CObjective* pObjective, int32_t xPos, int32_t yPos, bool bDrawTotal );
	void	renderMissionStatus( bool bRender){ renderStatusInfo = bRender; }		

	int		getCurrentRange();
	void	pressInfoButton( ){ handleClick( INFO_COMMAND ); }
	bool	infoButtonPressed() { return getButton( INFO_COMMAND )->state & ControlButton::PRESSED; }

	void	showServerMissing();

	void	pressAirstrikeButton() 
	{ 
		for ( int i = 0; i < LAST_VEHICLE; i++ )
		{
			if (( vehicleButtons[i].ID == LARGE_AIRSTRIKE) &&
				!( vehicleButtons[i].state & ControlButton::PRESSED ))
			{
				handleVehicleClick( LARGE_AIRSTRIKE ); 
			}
		}
	}		
	void	pressLargeAirstrikeButton() { handleVehicleClick( LARGE_AIRSTRIKE ); }		
	void	pressSensorStrikeButton() 
	{ 
		for ( int i = 0; i < LAST_VEHICLE; i++ )
		{
			if (( vehicleButtons[i].ID == SENSOR_PROBE) &&
				!( vehicleButtons[i].state & ControlButton::PRESSED ))
			{
				handleVehicleClick( SENSOR_PROBE ); 
			}
		}
	}		

	void	setRolloverHelpText( ULONG textID );


	void	setChatText( PCSTR playerName, PCSTR message, ULONG backgroundColor, 
		ULONG textColor );
	void	toggleChat( bool setTeamOnly );
	void eatChatKey();
	void	cancelInfo();

	ControlButton*		getButton( int ID );

	struct RectInfo
	{
		RECT rect;
		int32_t color;
	};

private:

	struct ChatInfo
	{
		char playerName[32];
		char message[128];
		ULONG backgroundColor;
		ULONG time;
		ULONG messageLength; // number of lines
		ULONG chatTextColor;
	};

	ChatInfo		chatInfos[MAX_CHAT_COUNT]; // max five lines -- could change

	RectInfo*		rectInfos;
	int32_t			rectCount;

	//static	ButtonFile		vehicleFileData[LAST_VEHICLE];
	static ULONG RUN;
	static ULONG WALK;
	static ULONG GUARD;
	static ULONG JUMP;

	ControlButton*		buttons;
	ControlButton*		vehicleButtons;
	static ButtonData*	buttonData;
	static ButtonData*	vehicleData;
	static PCSTR	vehicleNames[5];
	static int32_t			vehicleIDs[5];
	static PCSTR	vehiclePilots[5];


	static int32_t			vehicleCosts[LAST_VEHICLE];

	InfoWindow*			infoWnd;
	PauseWindow*		pauseWnd;

	StaticInfo*			staticInfos;
	int32_t				staticCount;

	StaticInfo*			objectiveInfos; //2nd to last one is check, last is x
	int32_t				objectiveInfoCount;

	StaticInfo*			missionStatusInfos;
	int32_t				missionStatusInfoCount;
	RectInfo			missionStatusRect;

	bool				renderStatusInfo;
	float				resultsTime;
	bool				renderObjectives;
	float				objectiveTime;

	float				tabFlashTime;
	static int32_t OBJECTIVESTOP;
	static int32_t OBJECTIVESLEFT;
	static int32_t OBJECTIVESSKIP;
	static int32_t OBJECTIVESTOTALRIGHT;
	static int32_t OBJEECTIVESHEADERSKIP;
	static int32_t OBJECTIVESHEADERTOP;
	static int32_t OBJECTIVEBOXX;
	static int32_t OBJECTIVEBOXSKIP;
	static int32_t OBJECTIVECHECKSKIP;
	static int32_t OBJECTIVEHEADERLEFT;
	static int32_t HELPAREA_LEFT;
	static int32_t HELPAREA_BOTTOM;
	static int32_t RPLEFT;
	static int32_t RPTOP;

	static MoveInfo objectiveMoveInfo[OBJECTVE_MOVE_COUNT];
	static MoveInfo missionResultsMoveInfo[RESULTS_MOVE_COUNT];

	StaticInfo*			videoInfos;
	int32_t				videoInfoCount;
	RECT			videoRect;
	RECT			videoTextRect;
	MC2MoviePtr			bMovie;

	StaticInfo*			timerInfos;
	int32_t				timerInfoCount;
	RectInfo			timerRect;

	//TUTORIAL!!
	RectInfo			rpCallout;
	int32_t				rpNumFlashes;
	float				rpFlashTime;
	int32_t				buttonToPress;

	ULONG		curOrder;
	bool				fireFromCurrentPos;
	bool				addingVehicle;
	bool				addingArtillery;
	bool				addingSalvage;
	bool				wasLayingMines;

	bool				moviePlaying;
	bool				twoMinWarningPlayed;
	bool				thirtySecondWarningPlayed;
	bool				bChatting;

	int32_t					idToUnPress;
	aFont					guiFont;
	aFont					helpFont;
	aFont					vehicleFont;
	aFont					timerFont;
	aFont					missionResultsFont;

	aText					chatEdit;
	aEdit					playerNameEdit;
	aEdit					personalEdit;

	void handleClick( int ID );
	void updateVehicleTab( int mouseX, int mouseY, bool bLOS );
	void renderVehicleTab();
	void RenderObjectives();
	void renderResults();
	void handleVehicleClick( int ID );
	void renderHelpText();
	void renderInfoTab();
	void renderChatText();

	void initStatics( FitIniFile& file );
	void initRects( FitIniFile& file );

	void renderPlayerStatus( float delta);

	MPStatsEntry			mpStats[9];

	bool					chatIsTeamOnly;
	bool					bServerWarningShown;

public:

	RectInfo *getRect (int32_t id)
	{
		if ((id >= 0) && (id < rectCount))
		{
			return &(rectInfos[id]);
		}

		if (id == RPTOTAL_CALLOUT)
		{
			return &rpCallout;
		}

		return NULL;
	}

};


//*************************************************************************************************
#endif  // end of file ( ControlGui.h )
