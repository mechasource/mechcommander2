/*************************************************************************************************\
MPStats.h			: Interface for the MPStats component.
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//
\*************************************************************************************************/

#pragma once

#ifndef MPSTATS_H
#define MPSTATS_H

//#include "mechgui/logisticsscreen.h"
//#include "Multplyr.h"

struct _MC2Player;

/**************************************************************************************************
CLASS DESCRIPTION
MPStats:
**************************************************************************************************/
class MPStatsEntry : public LogisticsScreen
{
public:
	MPStatsEntry(void);
	virtual ~MPStatsEntry(void);

	void init();
	void setData(const _MC2Player*, bool bShowScore);
	virtual void render(int32_t x, int32_t y);

	int32_t getPlayerHeadingX(void);
	int32_t getRankingX(void);
	int32_t getKillsX(void);
	int32_t getLossesX(void);
	int32_t overlaycolour;
};

class MPStatsResultsEntry : public LogisticsScreen
{
public:
	MPStatsResultsEntry(void);
	virtual ~MPStatsResultsEntry(void);
	virtual void render(int32_t x, int32_t y);

	void init();
	void setData(const _MC2Player*, uint32_t laurelcolour, bool bShowScore);
	int32_t overlaycolour;
};

class MPStats : public LogisticsScreen
{
public:
	MPStats(void);
	virtual ~MPStats(void);

	virtual void render(int32_t xOffset, int32_t yOffset);
	virtual void update(void);

	virtual void begin(void);
	virtual void end(void);

	virtual int32_t handleMessage(uint32_t what, uint32_t who);

	int32_t init(void);

	void setHostLeftDlg(const std::wstring_view& hostName);

private:
	MPStats(const MPStats& src);

	MPStatsResultsEntry entries[MAX_MC_PLAYERS];

	bool bShowMainMenu;
	bool bSavingStats;
	bool bHostLeftDlg;

	// HELPER FUNCTIONS
};

#endif // end of file ( MPStats.h )
