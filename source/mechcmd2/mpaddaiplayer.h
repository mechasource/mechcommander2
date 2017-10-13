/*************************************************************************************************\
MPAddAIPlayer.h			: Interface for the MPAddAIPlayer component.
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//
\*************************************************************************************************/

//*************************************************************************************************

#pragma once

#ifndef MPADDAIPLAYER_H
#define MPADDAIPLAYER_H

#ifndef LOGISTICSSCREEN_H
#include <mechgui/logisticsscreen.h>
#endif

#include <mechgui/asystem.h>
#include <mechgui/alistbox.h>
#include "attributemeter.h"
#include "simplecamera.h"
#include "MPParameterScreen.h" /*only for the definition of CFocusManager*/

#ifndef AANIM_H
#include <mechgui/aanim.h>
#endif

class aStyle4TextListItem : public aTextListItem
{
  public:
	aStyle4TextListItem()
	{
		hasAnimation = false;
		normalColor  = 0xff808080;
	}
	virtual int32_t init(FitIniFile* file, PCSTR blockName);
	virtual void render(void);

  protected:
	bool hasAnimation;
	aAnimGroup animGroup;
	int32_t normalColor;
};

class MPAddAIPlayer : public LogisticsScreen
{
  public:
	MPAddAIPlayer(void);
	virtual ~MPAddAIPlayer(void);

	void init(FitIniFile* file);
	bool isDone(void);
	virtual void begin(void);
	virtual void end(void);
	virtual void render(int32_t xOffset, int32_t yOffset);
	virtual void render(void);
	virtual void update(void);
	virtual int32_t handleMessage(uint32_t, uint32_t);

  private:
	aEdit nameEntry;
	aRect nameEntryOutline;
	aDropList experienceDropList;
	aDropList factionDropList;
	aDropList mechSelectionDropLists[4][3];

	int32_t indexOfButtonWithID(int32_t id);

	CFocusManager focusManager;
};

//*************************************************************************************************
#endif // end of file ( MPAddAIPlayer.h )
