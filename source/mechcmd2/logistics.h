//******************************************************************************************
// logistics.h - This file contains the logistics class header
//
//	MechCommander 2
//
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#pragma once

#ifndef LOGISTICS_H
#define LOGISTICS_H

//#include "mclib.h"
//#include "mc2movie.h"
//#include "logisticsdata.h"

//----------------------------------------------------------------------------------
// Macro Definitions
#define log_DONE 0
#define log_STARTMISSIONFROMCMDLINE 1
#define log_SPLASH 2
#define log_RESULTS 3
#define log_ZONE 4

class MissionResults;
class MissionBegin;

//----------------------------------------------------------------------------------
class Logistics
{
protected:
	bool active; // Am I currently in control?

	int32_t logisticsState;
	int32_t prevState; // Used to cleanup previous state

	LogisticsData logisticsData;

	MissionResults* missionResults;
	MissionBegin* missionBegin;

public:
	Logistics(void)
	{
		init(void);
	}

	~Logistics(void)
	{
		destroy(void);
	}

	void init(void)
	{
		active = FALSE;
		missionResults = 0;
		missionBegin = 0;
		logisticsState = log_SPLASH;
		bMovie = nullptr;
	}

	void destroy(void);

	void initSplashScreen(std::wstring_view screenFile, std::wstring_view artFile);
	void destroySplashScreen(void);

	void start(int32_t logState); // Actually Starts execution of logistics in
		// state Specified
	void stop(void); // Guess what this does!

	int32_t update(void);

	void render(void);

	void setResultsHostLeftDlg(std::wstring_view pName);

	void setLogisticsState(int32_t state)
	{
		prevState = logisticsState;
		logisticsState = state;
	}

	MissionBegin* getMissionBegin(void)
	{
		return missionBegin;
	}

	static int32_t _stdcall beginMission(PVOID, int32_t, PVOID[]);

	int32_t DoBeginMission(void);
	void playFullScreenVideo(std::wstring_view fileName);

	MC2MoviePtr bMovie;

private:
	void initializeLogData(void);
	bool bMissionLoaded;
	int32_t lastMissionResult;
};

extern Logistics* logistics;
//----------------------------------------------------------------------------------
#endif