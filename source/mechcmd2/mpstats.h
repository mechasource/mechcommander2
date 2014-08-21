/*************************************************************************************************\
MPStats.h			: Interface for the MPStats component.
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//
\*************************************************************************************************/

#pragma once

#ifndef MPSTATS_H
#define MPSTATS_H

//#include "logisticsscreen.h"
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
		virtual void render( int x, int y );

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
		virtual void render( int x, int y );

		void init( );
		void setData( const _MC2Player*, ULONG laurelColor, bool bShowScore );
		int32_t	overlayColor;

};




class MPStats: public LogisticsScreen
{
	public:

		MPStats();
		virtual ~MPStats();

		virtual void render(int xOffset, int yOffset);
		virtual void update();

		virtual void begin();
		virtual void end();

		virtual int handleMessage( ULONG what, ULONG who );

		int init();

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
