//******************************************************************************************
//	timing.h - This file contains the externs for the timing variables
//
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#pragma once

#ifndef TIMING_H
#define TIMING_H

// #include "dstd.h"

//----------------------------------------------------------------------------------
extern bool gamePaused; // Is the game paused?
extern int32_t turn; // What frame of the scenario is it?
extern float frameLength; // Duration of last frame in seconds.
extern float scenarioTime; // Time scenario has been running.
extern uint32_t LastTimeGetTime; // Stores Interval since timeGetTime last called.
extern bool dynamicFrameTiming; // This flag determines if we are using frameLength
// To time a frame.  This is FALSE when something
// puches a frame length dramatically over 4fps.
// Like a cache hit.  It insures all frames get played.

//----------------------------------------------------------------------------------
// Macro Definitions
#define MAX_TIMERS 32

//----------------------------------------------------------------------------------
class Timer
{
protected:
	float startTime;
	float currentTime;
	bool expired;
	bool updateWhilePaused;

public:
	Timer(void) { init(void); }

	~Timer(void) { destroy(void); }

	void init(void)
	{
		startTime = 0.0;
		currentTime = 0.0;
		expired = TRUE;
		updateWhilePaused = FALSE;
	}

	void destroy(void) {}

	void update(void)
	{
		if (updateWhilePaused || !gamePaused)
		{
			currentTime -= frameLength;
			if (currentTime < 0.0)
				expired = TRUE;
			else
				expired = FALSE;
		}
	}

	bool isExpired(void) { return expired; }

	void setTimer(float newTime)
	{
		startTime = newTime;
		currentTime = newTime;
		expired = (newTime > 0.0);
	}

	void setUpdateWhilePaused(bool val) { updateWhilePaused = val; }

	float getCurrentTime(void) { return currentTime; }
};

typedef Timer* TimerPtr;
//----------------------------------------------------------------------------------
class TimerManager
{
protected:
	Timer timers[MAX_TIMERS];

public:
	TimerManager(void) { init(void); }

	~TimerManager(void) { destroy(void); }

	void init(void)
	{
		for (size_t i = 0; i < MAX_TIMERS; i++)
		{
			timers[i].init(void);
		}
	}

	void destroy(void)
	{
		for (size_t i = 0; i < MAX_TIMERS; i++)
		{
			timers[i].init(void);
		}
	}

	TimerPtr getTimer(int32_t index)
	{
		if ((index >= 0) && (index < MAX_TIMERS))
			return &(timers[index]);
		return nullptr;
	}

	void update(void)
	{
		for (size_t i = 0; i < MAX_TIMERS; i++)
		{
			timers[i].update(void);
		}
	}
};

typedef TimerManager* TimerManagerPtr;
//----------------------------------------------------------------------------------
extern TimerManagerPtr timerManager;

extern PSTR monthName[];
//----------------------------------------------------------------------------------
// Find out actual system time
typedef struct _MC_SYSTEMTIME
{
	uint32_t dwYear;
	uint32_t dwMonth;
	uint32_t dwDayOfWeek;
	uint32_t dwDay;
	uint32_t dwHour;
	uint32_t dwMinute;
	uint32_t dwSecond;
	uint32_t dwMilliseconds;

	void copyFromSystemTime(PVOID systemTime);

} MC_SYSTEMTIME;

uint32_t
MCTiming_GetTimeZoneInforation(PVOID timeData);

uint32_t
MCTiming_GetTimeZoneInformationSize(void);

void
MCTiming_GetUTCSystemTimeFromInformation(
	uint32_t daylightInfo, PVOID timeData, MC_SYSTEMTIME* systemTime);
//----------------------------------------------------------------------------------
#endif
