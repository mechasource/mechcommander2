/*************************************************************************************************\
MPPrefs.h			: Interface for the MPPrefs component.
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//
\*************************************************************************************************/

#pragma once

#ifndef MPPREFS_H
#define MPPREFS_H

//#include <mechgui/logisticsscreen.h>
//#include <mechgui/alistbox.h>
//#include "simplecamera.h"

class FitIniFile;
struct _MC2Player;

//*************************************************************************************************

/**************************************************************************************************
CLASS DESCRIPTION
MPPrefs:
**************************************************************************************************/
class MPPrefs: public LogisticsScreen
{
	public:

		MPPrefs();
		virtual ~MPPrefs();
		
		static MPPrefs* instance(){ return s_instance; }
		int32_t init( FitIniFile& file );
		virtual void update();
		virtual void render( int32_t OffsetX, int32_t OffsetY );
		virtual int32_t	handleMessage( uint32_t message, uint32_t who );
		virtual void begin();
		virtual void end();
		void			initColors();

		void	saveSettings();
		void	cancelSettings();

		void setMechColors( uint32_t base, uint32_t highlight ); // called by MPlayer when it resets a color

	private:
		MPPrefs( const MPPrefs& src );
		MPPrefs& operator=( const MPPrefs& PPrefs );

		aComboBox		comboBox[3];
		SimpleCamera	camera;
		aObject			insigniaBmp; // the one inside the combo box...

		// HELPERS
		void	updateStripeColors(const _MC2Player* players, int32_t playerCount, bool bDrawRect );
		void	updateBaseColors( const _MC2Player* players, int32_t playerCount, bool bDrawRect);
		char	getColorIndex( uint32_t color );
		void	setColor( uint32_t color );
		void	setHighlightColor( uint32_t color );

		static MPPrefs* s_instance;


};

class aBmpListItem : public aListItem
{
public:

	int32_t setBmp( PCSTR pFileName );
	PCSTR getBmp(){ return fileName; }
	
private:

	aObject		bmp;

	EString		fileName;
};


//*************************************************************************************************
#endif  // end of file ( MPPrefs.h )
