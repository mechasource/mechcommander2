/*************************************************************************************************\
MPStats.h			: Interface for the MPStats component.
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//
\*************************************************************************************************/

#pragma once

#ifndef MPSTATS_H
#define MPSTATS_H

//#include <mechgui/logisticsscreen.h>
//#include "Multplyr.h"

//*************************************************************************************************
struct _MC2Player;

/**************************************************************************************************
CLASS DESCRIPTION
MPStats:
**************************************************************************************************/
class MPStatsEntry : public LogisticsScreen
{
	public:
		MPStatsEntry();
		virtual ~MPStatsEntry();

		void init( );
		void setData( const _MC2Player*, bool bShowScore );
		virtual void render( int32_t x, int32_t y );

		int32_t	getPlayerHeadingX();
		int32_t	getRankingX();
		int32_t	getKillsX();
		int32_t	getLossesX();
		int32_t	overlayColor;

		

};

class MPStatsResultsEntry : public LogisticsScreen
{
	public:
		MPStatsResultsEntry();
		virtual ~MPStatsResultsEntry();
		virtual void render( int32_t x, int32_t y );

		void init( );
		void setData( const _MC2Player*, uint32_t laurelColor, bool bShowScore );
		int32_t	overlayColor;

};




class MPStats: public LogisticsScreen
{
	public:

		MPStats();
		virtual ~MPStats();

		virtual void render(int32_t xOffset, int32_t yOffset);
		virtual void update();

		virtual void begin();
		virtual void end();

		virtual int32_t handleMessage( uint32_t what, uint32_t who );

		int32_t init();

		void		setHostLeftDlg( PCSTR hostName );

	private:

		MPStats( const MPStats& src );

		MPStatsResultsEntry		entries[MAX_MC_PLAYERS];

		bool				bShowMainMenu;
		bool				bSavingStats;
		bool				bHostLeftDlg;

		// HELPER FUNCTIONS

};


//*************************************************************************************************
#endif  // end of file ( MPStats.h )
