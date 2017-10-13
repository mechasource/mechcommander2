#define MPDIRECTTCPIP_CPP
/*************************************************************************************************\
MPDirectTcpip.cpp			: Implementation of the MPDirectTcpip component.
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//
\*************************************************************************************************/
#include "stdafx.h"

#include "MPDirectTcpip.h"
#include "prefs.h"
#include "IniFile.h"
#include "../MCLib/UserInput.h"
#include "..\resource.h"

#include "gamesound.h"
#define CHECK_BUTTON 200

static int32_t connectionType = 0;

static cint32_t FIRST_BUTTON_ID  = 1000010;
static cint32_t OK_BUTTON_ID	 = 1000001;
static cint32_t CANCEL_BUTTON_ID = 1000002;

MPDirectTcpip::MPDirectTcpip()
{
	bDone  = 0;
	status = RUNNING;
}

MPDirectTcpip::~MPDirectTcpip() {}

int32_t MPDirectTcpip::indexOfButtonWithID(int32_t id)
{
	int32_t i;
	for (i = 0; i < buttonCount; i++)
	{
		buttons[i].setMessageOnRelease();
		if (buttons[i].getID() == id)
		{
			return i;
		}
	}
	return -1;
}

void MPDirectTcpip::init(FitIniFile* file)
{
	LogisticsScreen::init(*file, "Static", "Text", "Rect", "Button");
	if (buttonCount)
	{
		for (size_t i = 0; i < buttonCount; i++)
		{
			if (buttons[i].getID() == 0)
			{
				buttons[i].setID(FIRST_BUTTON_ID + i);
			}
		}
	}
	{
		char path[256];
		strcpy(path, artPath);
		strcat(path, "mcl_mp_tcpip_combobox0.fit");
		FitIniFile PNfile;
		if (NO_ERROR != PNfile.open(path))
		{
			char error[256];
			sprintf(error, "couldn't open file %s", path);
			Assert(0, 0, error);
			return;
		}
		ipAddressComboBox.init(&PNfile, "TCIPNumberComboBox");
		aStyle7TextListItem* pTmp2;
		int32_t i;
		for (i = 0; i < 15; i += 1)
		{
			pTmp2 = new aStyle7TextListItem;
			EString tmpStr;
			tmpStr.Format("Text0");
			pTmp2->init(&PNfile, tmpStr.Data());
			ipAddressComboBox.AddItem(pTmp2);
		}
		ipAddressComboBox.SelectItem(0);
	}
}

void MPDirectTcpip::begin() { status = RUNNING; }

void MPDirectTcpip::end() {}

void MPDirectTcpip::render(int32_t xOffset, int32_t yOffset)
{
	if ((0 == xOffset) && (0 == yOffset))
	{
		ipAddressComboBox.render();
	}
	LogisticsScreen::render(xOffset, yOffset);
}

void MPDirectTcpip::render() { render(0, 0); }

int32_t MPDirectTcpip::handleMessage(uint32_t message, uint32_t who)
{
	if (RUNNING == status)
	{
		switch (who)
		{
		case 57 /*MB_MSG_MAINMENU*/:
		{
			getButton(57 /*MB_MSG_MAINMENU*/)->press(0);
			status = MAINMENU;
		}
		break;
		case 51 /*MB_MSG_PREV*/:
		{
			getButton(51 /*MB_MSG_PREV*/)->press(0);
			status = PREVIOUS;
		}
		break;
		case 50 /*MB_MSG_NEXT*/:
		{
			getButton(50 /*MB_MSG_NEXT*/)->press(0);
			// status = NEXT;
		}
		break;
		case FIRST_BUTTON_ID + 1:
		{
			// join game
			getButton(FIRST_BUTTON_ID + 1)->press(0);
			status = NEXT;
			return 1;
		}
		break;
		case FIRST_BUTTON_ID + 2:
		{
			// host game
			getButton(FIRST_BUTTON_ID + 2)->press(0);
			status = SKIPONENEXT;
			return 1;
		}
		break;
		}
	}
	return 0;
}

bool MPDirectTcpip::isDone() { return bDone; }

void MPDirectTcpip::update()
{
	LogisticsScreen::update();
	ipAddressComboBox.update();
	helpTextID		 = 0;
	helpTextHeaderID = 0;
	/*
	for ( int32_t i = 0; i < buttonCount; i++ )
	{
		buttons[i].update();
		if ( buttons[i].pointInside( userInput->getMouseX(),
	userInput->getMouseY() )
			&& userInput->isLeftClick() )
		{
			handleMessage( buttons[i].getID(), buttons[i].getID() );
		}

	}
	*/
}

int32_t aStyle7TextListItem::init(FitIniFile* file, PCSTR blockName)
{
	file->seekBlock(blockName);
	int32_t x = 0;
	int32_t y = 0;
	file->readIdLong("XLocation", x);
	file->readIdLong("YLocation", y);
	int32_t fontResID = 0;
	file->readIdLong("Font", fontResID);
	int32_t textID = 0;
	file->readIdLong("TextID", textID);
	aTextListItem::init(fontResID);
	setText(textID);
	int32_t color = 0xff808080;
	file->readIdLong("Color", color);
	normalColor = color;
	setColor(color);
	char tmpStr[64];
	strcpy(tmpStr, "");
	file->readIdString("Animation", tmpStr, 63);
	if (0 == strcmp("", tmpStr))
	{
		hasAnimation = false;
	}
	else
	{
		hasAnimation = true;
		animGroup.init(file, tmpStr);
	}
	moveTo(x, y);
	return 0;
}

void aStyle7TextListItem::render()
{
	float color;
	if (aListItem::SELECTED == getState())
	{
		color = 0.33 * ((uint32_t)normalColor) + 0.67 * ((uint32_t)0xffffffff);
	}
	else if (aListItem::HIGHLITE == getState())
	{
		color = 0.67 * ((uint32_t)normalColor) + 0.33 * ((uint32_t)0xffffffff);
	}
	else
	{
		color = normalColor;
	}
	aTextListItem::setColor((uint32_t)color);
	aTextListItem::render();
}

//////////////////////////////////////////////

//*************************************************************************************************
// end of file ( MPDirectTcpip.cpp )
