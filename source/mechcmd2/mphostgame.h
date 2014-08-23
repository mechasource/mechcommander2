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
	int normalColor;
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
 	virtual void render( int xOffset, int yOffset );
	virtual void render();
	virtual void update();
	virtual int			handleMessage( ULONG, ULONG );


private:
//	aEdit		nameEntry;
//	aRect		nameEntryOutline;
	bool					bShowDlg;

	int indexOfButtonWithID(int id);
};



//*************************************************************************************************
#endif  // end of file ( MPHostGame.h )
