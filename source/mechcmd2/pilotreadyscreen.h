/*************************************************************************************************\
PilotReadyScreen.h : Header file for pilot selection
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//
\*************************************************************************************************/

#pragma once

#ifndef PILOTREADYSCREEN_H
#define PILOTREADYSCREEN_H

//#include "logisticsscreen.h"
//#include "attributemeter.h"
//#include "aanim.h"
//#include "logisticspilotlistbox.h"
//#include "logisticsmechdisplay.h"

class LogisticsPilot;
class LogisticsMechIcon;

class PilotReadyScreen : public LogisticsScreen
{

public:

	static PilotReadyScreen* instance(){ return s_instance; }
	PilotReadyScreen();
	virtual ~PilotReadyScreen();

	void init(FitIniFile* file);
	virtual void render(int xOffset, int yOffset);
	virtual void update();
	virtual int			handleMessage( ULONG, ULONG );
	
	void beginDrag( LogisticsPilot* pPilot );

	virtual void begin();
	virtual void end();

	void setPilot( LogisticsPilot* pPilot );
	void setMech( LogisticsMech* pMech );
	void addSelectedPilot();
	

private:

	LogisticsPilot*		pCurPilot;
	LogisticsPilot*		pDragPilot;
	LogisticsMechIcon*	pIcons;
	aObject				dragIcon;
	bool				dragLeft;
	int32_t				forceGroupCount;

	aObject				specialtySkillIcons[4];
	RECT			skillLocations[4];
	aObject				skillIcons[4];
	aObject				medalIcons[16];
	RECT			medalLocations[16];

	AttributeMeter		attributeMeters[2];
	LogisticsPilotListBox	pilotListBox;
	LogisticsMechDisplay	mechDisplay;
	bool					mechSelected;

	aObject				rankIcons[5];

	static	PilotReadyScreen*	s_instance;


	void removeSelectedPilot();
	
	void putBackPilot( LogisticsPilot* pPilot );
	void endDrag( LogisticsMechIcon* pIcon  );
	
	PilotReadyScreen( const PilotReadyScreen& );
	PilotReadyScreen& operator=( const PilotReadyScreen& );

	float				launchFadeTime;



};

#endif