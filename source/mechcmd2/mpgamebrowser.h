/*************************************************************************************************\
MPGameBrowser.h			: Interface for the MPGameBrowser component.
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//
\*************************************************************************************************/

//*************************************************************************************************

#pragma once

#ifndef MPGAMEBROWSER_H
#define MPGAMEBROWSER_H

//#include <mechgui/logisticsscreen.h>
//#include <mechgui/asystem.h>
//#include <mechgui/alistbox.h>
//#include "attributemeter.h"
//#include "simplecamera.h"
//#include "multplyr.h"
//#include <mechgui/aanim.h>
//#include "mphostgame.h"

class aButton;
//struct _MC2Session;


class aStyle3TextListItem : public aTextListItem
{
public:
	aStyle3TextListItem() { hasAnimation = false; normalColor = 0xff808080; }
	virtual int32_t	init( FitIniFile* file, PCSTR blockName );
	virtual void		render();

protected:
	bool hasAnimation;
	aAnimGroup animGroup;
	int normalColor;
};

class aGameListItem : public aListItem
{
public:
	aGameListItem();
	virtual int32_t	init( FitIniFile* file, PCSTR blockName );
	virtual void update();
	void setSessionInfo( _MC2Session* pSessions );

	PCSTR		getSessionName();

	PCSTR		getText( int which );

	const MC2Session* getSession (void) { return(&session); }


	aGameListItem& operator=( const aGameListItem& );

protected:
	MC2Session session;
	aObject allTechGraphic;
	aStyle3TextListItem gameName;
	aStyle3TextListItem numPlayers;
	aStyle3TextListItem mapName;
	aTextListItem latency;
	aRect allTechRect;
	aRect gameNameRect;
	aRect numPlayersRect;
	aRect mapNameRect;
	aRect latencyRect;
	aObject pingIcon;
};

class MPGameBrowser : public LogisticsScreen
{
public:
	
	MPGameBrowser();
	virtual ~MPGameBrowser();
	
	void init(FitIniFile* file);
	bool isDone();
	virtual void		begin();
	virtual void		end();
 	virtual void render( int xOffset, int yOffset );
	virtual void render();
	virtual void update();
	virtual int			handleMessage( ULONG, ULONG );


private:
	int indexOfButtonWithID(int id);

	aListBox				gameList;
	aGameListItem			items[256];
	aGameListItem			templateItem;

	MPHostGame				hostDlg;

	bool					bHosting;
	bool					bShowErrorDlg;

	int						sortOrder;

	int						bSortUpward;
	int32_t					oldScrollPos;
};



//*************************************************************************************************
#endif  // end of file ( MPGameBrowser.h )
