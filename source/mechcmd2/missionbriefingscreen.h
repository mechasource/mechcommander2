//===========================================================================//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#pragma once

#ifndef MISSIONBRIEFINGSCREEN_H
#define MISSIONBRIEFINGSCREEN_H

#include <mechgui/logisticsscreen.h>
#include <mechgui/alistbox.h>
#include <mechgui/abutton.h>
#include "Mission.h"
#include "simplecamera.h"

#define MN_MSG_PLAY 80
#define MN_MSG_STOP 82
#define MN_MSG_PAUSE 81


//*************************************************************************************************

/**************************************************************************************************
CLASS DESCRIPTION
MissionBriefingScreen:
**************************************************************************************************/
class MissionBriefingScreen: public LogisticsScreen
{
	public:

	MissionBriefingScreen();
	virtual ~MissionBriefingScreen();

	virtual void render( int xOffset, int yOffset );
	virtual void begin();
	virtual void end();
	virtual void update();
	void	init( FitIniFile* file );
	virtual int			handleMessage( ULONG, ULONG );


	static int32_t	getMissionTGA( PCSTR missionName );



	private:

	aObject*		objectiveButtons[MAX_OBJECTIVES];
	aObject			dropZoneButton;
	EString			objectiveModels[MAX_OBJECTIVES];
	int32_t			modelTypes[MAX_OBJECTIVES];
	float			modelScales[MAX_OBJECTIVES];
	int32_t			modelColors[MAX_OBJECTIVES][3];
	aListBox		missionListBox; 

	int			addLBItem( PCSTR itemName, ULONG color, int ID);
	int			addItem( int ID, ULONG color, int LBid );
	void		addObjectiveButton( float fMakerX, float fMarkerY, int count, int priority,
											   float mapWidth, float mapHeight, bool display );
	void		setupDropZone( float fX, float fY, float mapWidth, float mapHeight );


	float		runTime;
	bool		bClicked;


	SimpleCamera	camera;





};


//*************************************************************************************************
#endif  // end of file ( MissionBriefingScreen.h )
