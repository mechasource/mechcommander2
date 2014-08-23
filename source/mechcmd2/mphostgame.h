/*************************************************************************************************\
MPHostGame.h			: Interface for the MPHostGame component.
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//
\*************************************************************************************************/

//*************************************************************************************************

#pragma once

#ifndef MPHOSTGAME_H
#define MPHOSTGAME_H

//#include "logisticsdialog.h"
//#include <mechgui/asystem.h>
//#include <mechgui/alistbox.h>
//#include "attributemeter.h"
//#include "simplecamera.h"
//#include <mechgui/aanim.h>

class aStyle5TextListItem : public aTextListItem
{
public:
	aStyle5TextListItem() { hasAnimation = false; normalColor = 0xff808080; }
	virtual int32_t	init( FitIniFile* file, PCSTR blockName );
	virtual void		render();

protected:
	bool hasAnimation;
	aAnimGroup animGroup;
	int32_t normalColor;
};

class MPHostGame : public LogisticsDialog
{
public:
	
	MPHostGame();
	virtual ~MPHostGame();
	
	void init();
	bool isDone();
	virtual void		begin();
	virtual void		end();
 	virtual void render( int32_t xOffset, int32_t yOffset );
	virtual void render();
	virtual void update();
	virtual int32_t			handleMessage( uint32_t, uint32_t );


private:
//	aEdit		nameEntry;
//	aRect		nameEntryOutline;
	bool					bShowDlg;

	int32_t indexOfButtonWithID(int32_t id);
};



//*************************************************************************************************
#endif  // end of file ( MPHostGame.h )
