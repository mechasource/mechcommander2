/*************************************************************************************************\
MPDirectTcpip.h			: Interface for the MPDirectTcpip component.
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//
\*************************************************************************************************/

//*************************************************************************************************

#pragma once

#ifndef MPDIRECTTCPIP_H
#define MPDIRECTTCPIP_H

#ifndef LOGISTICSSCREEN_H
#include <mechgui/logisticsscreen.h>
#endif

#include <mechgui/asystem.h>
#include <mechgui/alistbox.h>
#include "attributemeter.h"
#include "simplecamera.h"

#ifndef AANIM_H
#include <mechgui/aanim.h>
#endif

class aButton;


class aStyle7TextListItem : public aTextListItem
{
public:
	aStyle7TextListItem() { hasAnimation = false; normalColor = 0xff808080; }
	virtual int32_t	init( FitIniFile* file, PCSTR blockName );
	virtual void		render();

protected:
	bool hasAnimation;
	aAnimGroup animGroup;
	int normalColor;
};

class MPDirectTcpip : public LogisticsScreen
{
public:
	
	MPDirectTcpip();
	virtual ~MPDirectTcpip();
	
	void init(FitIniFile* file);
	bool isDone();
	virtual void		begin();
	virtual void		end();
 	virtual void render( int xOffset, int yOffset );
	virtual void render();
	virtual void update();
	virtual int			handleMessage( ULONG, ULONG );

	bool				bDone;


private:
	int indexOfButtonWithID(int id);

	aComboBox				ipAddressComboBox;
};



//*************************************************************************************************
#endif  // end of file ( MPDirectTcpip.h )
