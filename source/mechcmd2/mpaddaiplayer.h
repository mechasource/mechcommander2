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
#include "logisticsscreen.h"
#endif

#include "asystem.h"
#include "alistbox.h"
#include "attributemeter.h"
#include "simplecamera.h"
#include "MPParameterScreen.h"/*only for the definition of CFocusManager*/

#ifndef AANIM_H
#include "aanim.h"
#endif


class aStyle4TextListItem : public aTextListItem
{
public:
	aStyle4TextListItem() { hasAnimation = false; normalColor = 0xff808080; }
	virtual int32_t	init( FitIniFile* file, PCSTR blockName );
	virtual void		render();

protected:
	bool hasAnimation;
	aAnimGroup animGroup;
	int normalColor;
};

class MPAddAIPlayer : public LogisticsScreen
{
public:
	
	MPAddAIPlayer();
	virtual ~MPAddAIPlayer();
	
	void init(FitIniFile* file);
	bool isDone();
	virtual void		begin();
	virtual void		end();
 	virtual void render( int xOffset, int yOffset );
	virtual void render();
	virtual void update();
	virtual int			handleMessage( ULONG, ULONG );

private:
	aEdit		nameEntry;
	aRect		nameEntryOutline;
	aDropList				experienceDropList;
	aDropList				factionDropList;
	aDropList				mechSelectionDropLists[4][3];

	int indexOfButtonWithID(int id);

	CFocusManager focusManager;
};



//*************************************************************************************************
#endif  // end of file ( MPAddAIPlayer.h )
