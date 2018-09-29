/*************************************************************************************************\
MissionSelectionScreen.h : Header file for mission selection
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//
\*************************************************************************************************/

#pragma once

#ifndef MISSIONSELECTION_H
#define MISSIONSELECTION_H

//#include <mechgui/logisticsscreen.h>
//#include <mechgui/alistbox.h>
//#include "mc2movie.h"

class FitIniFile;

#define MAX_MISSIONS_IN_GROUP 6

class MissionSelectionScreen : public LogisticsScreen
{

public:
	MissionSelectionScreen(void);
	virtual ~MissionSelectionScreen(void);
	virtual void render(int32_t xOffset, int32_t yOffset);
	virtual void begin(void);
	virtual void end(void);
	virtual void update(void);
	void init(FitIniFile* file);
	virtual int32_t handleMessage(uint32_t, uint32_t);

private:
	LogisticsScreen operationScreen;

	MC2MoviePtr bMovie;
	bool playedLogisticsTune;

	// HGOSVIDEO			video;
	// uint32_t		videoTexture;

	PCSTR missionNames[MAX_MISSIONS_IN_GROUP];
	int32_t missionCount;

	void setMission(int32_t whichOne);

	aListBox missionDescriptionListBox;

	void updateListBox(void);

	int32_t pressedButton;

	bool bStop;
};

#endif