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
	aStyle7TextListItem()
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

class MPDirectTcpip : public LogisticsScreen
{
  public:
	MPDirectTcpip(void);
	virtual ~MPDirectTcpip(void);

	void init(FitIniFile* file);
	bool isDone(void);
	virtual void begin(void);
	virtual void end(void);
	virtual void render(int32_t xOffset, int32_t yOffset);
	virtual void render(void);
	virtual void update(void);
	virtual int32_t handleMessage(uint32_t, uint32_t);

	bool bDone;

  private:
	int32_t indexOfButtonWithID(int32_t id);

	aComboBox ipAddressComboBox;
};

//*************************************************************************************************
#endif // end of file ( MPDirectTcpip.h )
