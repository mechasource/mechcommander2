/*************************************************************************************************\
PilotReadyScreen.h : Header file for pilot selection
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//
\*************************************************************************************************/

#pragma once

#ifndef PILOTREADYSCREEN_H
#define PILOTREADYSCREEN_H

//#include "mechgui/logisticsscreen.h"
//#include "attributemeter.h"
//#include "mechgui/aanim.h"
//#include "logisticspilotlistbox.h"
//#include "logisticsmechdisplay.h"

class LogisticsPilot;
class LogisticsMechIcon;

class PilotReadyScreen : public LogisticsScreen
{

public:
	static PilotReadyScreen* instance() { return s_instance; }
	PilotReadyScreen(void);
	virtual ~PilotReadyScreen(void);

	void init(FitIniFile* file);
	virtual void render(int32_t xOffset, int32_t yOffset);
	virtual void update(void);
	virtual int32_t handleMessage(uint32_t, uint32_t);

	void beginDrag(LogisticsPilot* pPilot);

	virtual void begin(void);
	virtual void end(void);

	void setPilot(LogisticsPilot* pPilot);
	void setMech(LogisticsMech* pMech);
	void addSelectedPilot(void);

private:
	LogisticsPilot* pCurPilot;
	LogisticsPilot* pDragPilot;
	LogisticsMechIcon* pIcons;
	aObject dragIcon;
	bool dragLeft;
	int32_t forceGroupCount;

	aObject specialtySkillIcons[4];
	RECT skillLocations[4];
	aObject skillIcons[4];
	aObject medalIcons[16];
	RECT medalLocations[16];

	AttributeMeter attributeMeters[2];
	LogisticsPilotListBox pilotListBox;
	LogisticsMechDisplay mechDisplay;
	bool mechSelected;

	aObject rankIcons[5];

	static PilotReadyScreen* s_instance;

	void removeSelectedPilot(void);

	void putBackPilot(LogisticsPilot* pPilot);
	void endDrag(LogisticsMechIcon* pIcon);

	PilotReadyScreen(const PilotReadyScreen&);
	PilotReadyScreen& operator=(const PilotReadyScreen&);

	float launchFadeTime;
};

#endif