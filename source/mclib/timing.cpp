//******************************************************************************************
//	timing.cpp - This file contains the declarations for the timing variables
//
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//
#include "stdinc.h"
//#include <windows.h>
//#include <winbase.h>
//#include <math.h>

//----------------------------------------------------------------------------------
// Include Files
#ifndef TIMING_H
#include "timing.h"
#endif

//----------------------------------------------------------------------------------
int32_t turn = 0;
float frameLength = 0.05f;
float scenarioTime = 0.0;
uint32_t LastTimeGetTime = 0;
bool dynamicFrameTiming = TRUE;

std::wstring_view monthName[12] = {
	"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};

//----------------------------------------------------------------------------------
uint32_t MCTiming_GetTimeZoneInforation(PVOIDtimeData)
{
	// Get Time Zone information for this machine to calculate
	// Astronomy correctly.
	uint32_t daylightSavingsInfo = GetTimeZoneInformation((TIME_ZONE_INFORMATION*)timeData);
	return daylightSavingsInfo;
}

//----------------------------------------------------------------------------------
uint32_t
MCTiming_GetTimeZoneInformationSize(void)
{
	return sizeof(TIME_ZONE_INFORMATION);
}

//----------------------------------------------------------------------------------
void MC_SYSTEMTIME::copyFromSystemTime(PVOIDsystemTime)
{
	SYSTEMTIME* sysTime = (SYSTEMTIME*)systemTime;
	dwYear = sysTime->wYear;
	dwMonth = sysTime->wMonth;
	dwDayOfWeek = sysTime->wDayOfWeek;
	dwDay = sysTime->wDay;
	dwHour = sysTime->wHour;
	dwMinute = sysTime->wMinute;
	dwSecond = sysTime->wSecond;
	dwMilliseconds = sysTime->wMilliseconds;
}

//----------------------------------------------------------------------------------
void MCTiming_GetUTCSystemTimeFromInformation(
	uint32_t daylightInfo, PVOIDtimeData, MC_SYSTEMTIME* systemTime)
{
	TIME_ZONE_INFORMATION* tzInfo = (TIME_ZONE_INFORMATION*)timeData;
	SYSTEMTIME sysTime;
	GetSystemTime(&sysTime);
	int32_t bias = tzInfo->Bias;
	if (daylightInfo == TIME_ZONE_ID_STANDARD)
	{
		bias += tzInfo->StandardBias;
	}
	else if (daylightInfo == TIME_ZONE_ID_DAYLIGHT)
	{
		bias += tzInfo->DaylightBias;
	}
	else // Assume Standard
	{
		bias += tzInfo->StandardBias;
	}
	systemTime->copyFromSystemTime(&(sysTime));
}

//----------------------------------------------------------------------------------
