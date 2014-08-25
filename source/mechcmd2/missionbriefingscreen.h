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

	MissionBriefingScreen(void);
	virtual ~MissionBriefingScreen(void);

	virtual void render( int32_t xOffset, int32_t yOffset );
	virtual void begin(void);
	virtual void end(void);
	virtual void update(void);
	void	init( FitIniFile* file );
	virtual int32_t			handleMessage( uint32_t, uint32_t );


	static int32_t	getMissionTGA( PCSTR missionName );



	private:

	aObject*		objectiveButtons[MAX_OBJECTIVES];
	aObject			dropZoneButton;
	EString			objectiveModels[MAX_OBJECTIVES];
	int32_t			modelTypes[MAX_OBJECTIVES];
	float			modelScales[MAX_OBJECTIVES];
	int32_t			modelColors[MAX_OBJECTIVES][3];
	aListBox		missionListBox; 

	int32_t			addLBItem( PCSTR itemName, uint32_t color, int32_t ID);
	int32_t			addItem( int32_t ID, uint32_t color, int32_t LBid );
	void		addObjectiveButton( float fMakerX, float fMarkerY, int32_t count, int32_t priority,
											   float mapWidth, float mapHeight, bool display );
	void		setupDropZone( float fX, float fY, float mapWidth, float mapHeight );


	float		runTime;
	bool		bClicked;


	SimpleCamera	camera;





};


//*************************************************************************************************
#endif  // end of file ( MissionBriefingScreen.h )
