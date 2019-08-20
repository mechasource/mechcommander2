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
// struct _MC2Session;

class aStyle3TextListItem : public aTextListItem
{
public:
	aStyle3TextListItem()
	{
		hasAnimation = false;
		normalColor = 0xff808080;
	}
	virtual int32_t init(FitIniFile* file, PCSTR blockName);
	virtual void render(void);

protected:
	bool hasAnimation;
	aAnimGroup animGroup;
	int32_t normalColor;
};

class aGameListItem : public aListItem
{
public:
	aGameListItem(void);
	virtual int32_t init(FitIniFile* file, PCSTR blockName);
	virtual void update(void);
	void setSessionInfo(_MC2Session* pSessions);

	PCSTR getSessionName(void);

	PCSTR getText(int32_t which);

	const MC2Session* getSession(void) { return (&session); }

	aGameListItem& operator=(const aGameListItem&);

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
	MPGameBrowser(void);
	virtual ~MPGameBrowser(void);

	void init(FitIniFile* file);
	bool isDone(void);
	virtual void begin(void);
	virtual void end(void);
	virtual void render(int32_t xOffset, int32_t yOffset);
	virtual void render(void);
	virtual void update(void);
	virtual int32_t handleMessage(uint32_t, uint32_t);

private:
	int32_t indexOfButtonWithID(int32_t id);

	aListBox gameList;
	aGameListItem items[256];
	aGameListItem templateItem;

	MPHostGame hostDlg;

	bool bHosting;
	bool bShowErrorDlg;

	int32_t sortOrder;

	int32_t bSortUpward;
	int32_t oldScrollPos;
};

//*************************************************************************************************
#endif // end of file ( MPGameBrowser.h )
