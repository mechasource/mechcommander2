#define MPADDAIPLAYER_CPP
/*************************************************************************************************\
MPAddAIPlayer.cpp			: Implementation of the MPAddAIPlayer component.
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//
\*************************************************************************************************/
#include "stdinc.h"

#include "MPAddAIPlayer.h"
#include "prefs.h"
#include "IniFile.h"
#include "userinput.h"
#include "resource.h"

#include "gamesound.h"
#define CHECK_BUTTON 200

static CFocusManager* g_focusManager = nullptr;

static const int32_t FIRST_BUTTON_ID = 1000010;
static const int32_t OK_BUTTON_ID = 1000001;
static const int32_t CANCEL_BUTTON_ID = 1000002;

MPAddAIPlayer::MPAddAIPlayer()
{
	status = RUNNING;
	g_focusManager = &focusManager;
}

MPAddAIPlayer::~MPAddAIPlayer()
{
	g_focusManager = nullptr;
}

int32_t
MPAddAIPlayer::indexOfButtonWithID(int32_t id)
{
	int32_t i;
	for (i = 0; i < buttonCount; i++)
	{
		if (buttons[i].getID() == id)
		{
			return i;
		}
	}
	return -1;
}

void MPAddAIPlayer::init(FitIniFile* file)
{
	if (g_focusManager)
	{
		g_focusManager->registerDropList(experienceDropList);
		g_focusManager->registerDropList(factionDropList);
		int32_t row;
		for (row = 0; row < 4; row += 1)
		{
			int32_t column;
			for (column = 0; column < 3; column += 1)
			{
				g_focusManager->registerDropList(mechSelectionDropLists[row][column]);
			}
		}
	}
	LogisticsScreen::init(*file, "Static", "Text", "Rect", "Button");
	if (buttonCount)
	{
		for (size_t i = 0; i < buttonCount; i++)
		{
			buttons[i].setMessageOnRelease();
			if (buttons[i].getID() == 0)
			{
				buttons[i].setID(FIRST_BUTTON_ID + i);
			}
		}
	}
	nameEntry.init(file, "EditBox0");
	nameEntryOutline.init(file, "EditBoxRect0");
	{
		wchar_t path[256];
		strcpy(path, artPath);
		strcat(path, "mcl_mp_addai_combobox1.fit");
		FitIniFile PNfile;
		if (NO_ERROR != PNfile.open(path))
		{
			wchar_t error[256];
			sprintf(error, "couldn't open file %s", path);
			Assert(0, 0, error);
			return;
		}
		experienceDropList.init(&PNfile, "ExperienceComboBox");
		PNfile.seekBlock("Texts");
		int32_t listItemCount = 0;
		PNfile.readIdLong("Textcount", listItemCount);
		aStyle4TextListItem* pTmp2;
		int32_t i;
		for (i = 0; i < listItemCount; i += 1)
		{
			pTmp2 = new aStyle4TextListItem;
			std::wstring_view tmpStr;
			tmpStr.Format("Text%d", i);
			pTmp2->init(&PNfile, tmpStr.Data());
			experienceDropList.AddItem(pTmp2);
		}
		experienceDropList.SelectItem(0);
	}
	{
		wchar_t path[256];
		strcpy(path, artPath);
		strcat(path, "mcl_mp_addai_combobox2.fit");
		FitIniFile PNfile;
		if (NO_ERROR != PNfile.open(path))
		{
			wchar_t error[256];
			sprintf(error, "couldn't open file %s", path);
			Assert(0, 0, error);
			return;
		}
		factionDropList.init(&PNfile, "FactionComboBox");
		PNfile.seekBlock("Texts");
		int32_t listItemCount = 0;
		PNfile.readIdLong("Textcount", listItemCount);
		aStyle4TextListItem* pTmp2;
		int32_t i;
		for (i = 0; i < listItemCount; i += 1)
		{
			pTmp2 = new aStyle4TextListItem;
			std::wstring_view tmpStr;
			tmpStr.Format("Text%d", i);
			pTmp2->init(&PNfile, tmpStr.Data());
			factionDropList.AddItem(pTmp2);
		}
		factionDropList.SelectItem(0);
	}
	int32_t row;
	for (row = 0; row < 4; row += 1)
	{
		int32_t column;
		for (column = 0; column < 3; column += 1)
		{
			{
				wchar_t path[256];
				strcpy(path, artPath);
				strcat(path, "mcl_mp_addai_combobox3.fit");
				FitIniFile PNfile;
				if (NO_ERROR != PNfile.open(path))
				{
					wchar_t error[256];
					sprintf(error, "couldn't open file %s", path);
					Assert(0, 0, error);
					return;
				}
				mechSelectionDropLists[row][column].init(&PNfile, "MechSelectionComboBox");
				PNfile.seekBlock("Texts");
				int32_t listItemCount = 0;
				PNfile.readIdLong("Textcount", listItemCount);
				aStyle4TextListItem* pTmp2;
				int32_t i;
				for (i = 0; i < listItemCount; i += 1)
				{
					pTmp2 = new aStyle4TextListItem;
					std::wstring_view tmpStr;
					tmpStr.Format("Text%d", i);
					pTmp2->init(&PNfile, tmpStr.Data());
					mechSelectionDropLists[row][column].AddItem(pTmp2);
				}
				mechSelectionDropLists[row][column].SelectItem(0);
				mechSelectionDropLists[row][column].moveTo(
					126 + column * (176 + 10), 317 + row * (32 + 5));
			}
		}
	}
}

void MPAddAIPlayer::begin()
{
	status = RUNNING;
}

void MPAddAIPlayer::end()
{
}

void MPAddAIPlayer::render(int32_t xOffset, int32_t yOffset)
{
	LogisticsScreen::render(xOffset, yOffset);
	if ((0 == xOffset) && (0 == yOffset))
	{
		nameEntry.render();
		nameEntryOutline.render();
		experienceDropList.render();
		factionDropList.render();
		int32_t row;
		for (row = 0; row < 4; row += 1)
		{
			int32_t column;
			for (column = 0; column < 3; column += 1)
			{
				mechSelectionDropLists[row][column].render();
			}
		}
		/*make sure that that the control that has the focus is not obscured*/
		aObject* pControlThatHasTheFocus = focusManager.pControlThatHasTheFocus();
		if (pControlThatHasTheFocus)
		{
			pControlThatHasTheFocus->render();
		}
	}
}

void MPAddAIPlayer::render()
{
	render(0, 0);
}

int32_t
MPAddAIPlayer::handleMessage(uint32_t message, uint32_t who)
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
			status = DOWN;
		}
		break;
		case 50 /*MB_MSG_NEXT*/:
		{
			getButton(50 /*MB_MSG_NEXT*/)->press(0);
			// status = NEXT;
			status = DOWN;
		}
		break;
		}
	}
	return 0;
}

void MPAddAIPlayer::update()
{
	focusManager.update();
	aObject* pControlThatHasTheFocus = focusManager.pControlThatHasTheFocus();
	if (pControlThatHasTheFocus)
	{
		pControlThatHasTheFocus->update();
	}
	else
	{
		LogisticsScreen::update();
		nameEntry.update();
		nameEntryOutline.update();
		experienceDropList.update();
		factionDropList.update();
		int32_t row;
		for (row = 0; row < 4; row += 1)
		{
			int32_t column;
			for (column = 0; column < 3; column += 1)
			{
				mechSelectionDropLists[row][column].update();
			}
		}
	}
	helpTextID = 0;
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

int32_t
aStyle4TextListItem::init(FitIniFile* file, std::wstring_view blockname)
{
	file->seekBlock(blockname);
	int32_t fontResID = 0;
	file->readIdLong("Font", fontResID);
	int32_t textID = 0;
	file->readIdLong("TextID", textID);
	aTextListItem::init(fontResID);
	setText(textID);
	int32_t color = 0xff808080;
	file->readIdLong("colour", color);
	normalcolour = color;
	setcolour(color);
	wchar_t tmpStr[64];
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
	return 0;
}

void aStyle4TextListItem::render()
{
	float color;
	if (aListItem::SELECTED == getState())
	{
		color = 0.33 * ((uint32_t)normalcolour) + 0.67 * ((uint32_t)0xffffffff);
	}
	else if (aListItem::HIGHLITE == getState())
	{
		color = 0.67 * ((uint32_t)normalcolour) + 0.33 * ((uint32_t)0xffffffff);
	}
	else
	{
		color = normalcolour;
	}
	aTextListItem::setcolour((uint32_t)color);
	aTextListItem::render();
}

//////////////////////////////////////////////

// end of file ( MPAddAIPlayer.cpp )
