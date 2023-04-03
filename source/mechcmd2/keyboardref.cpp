#define KEYBOARDREF_CPP
/*************************************************************************************************\
KeyboardRef.cpp			: Implementation of the KeyboardRef component.
//===========================================================================//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//
\*************************************************************************************************/

#include "stdinc.h"
#include "keyboardref.h"
#include "mclib.h"
#include "resource.h"
#include "missiongui.h"
#include "utilities.h"

#define CTRL 0x10000000
#define SHIFT 0x01000000
#define ALT 0x00100000
#define WAYPT 0x20000000

KeyboardRef::KeyboardRef()
	: listItemTemplate(IDS_KEYBOARD_REF_FONT)
	, listItemTemplate2(IDS_KEYBOARD_REF_FONT)
{
}

KeyboardRef::~KeyboardRef()
{
	listBox.destroy();
}

//-------------------------------------------------------------------------------------------------

int32_t
KeyboardRef::init()
{
	// clear out old stuff first
	clear();
	FullPathFileName path;
	switch (Environment.screenwidth)
	{
	case 640:
		path.init(artPath, "mcui_keyref_640", ".fit");
		break;
	case 800:
		path.init(artPath, "mcui_keyref_800", ".fit");
		break;
	case 1024:
		path.init(artPath, "mcui_keyref_1024", ".fit");
		break;
	case 1280:
		path.init(artPath, "mcui_keyref_1280", ".fit");
		break;
	case 1600:
		path.init(artPath, "mcui_keyref_1600", ".fit");
		break;
	default:
		NODEFAULT;
		// gosASSERT( !"Invalid resolution" );
		return -1;
	}
	FitIniFile file;
	file.open(path);
	LogisticsScreen::init(file, "Static", "Text", "Rect", "Button");
	listBox.init(rects[2].left(), rects[2].top(), rects[2].width(), rects[2].height());
	file.close();
	path.init(artPath, "mcui_keyref_entry", ".fit");
	file.open(path);
	listItemTemplate.init(file, "Text0");
	listItemTemplate2.init(file, "Text1");
	buttons[0].setMessageOnRelease();
	return true;
}

void KeyboardRef::update()
{
	listBox.update();
	LogisticsScreen::update();
}

void KeyboardRef::render()
{
	RECT rect = {0, 0, Environment.screenwidth, Environment.screenheight};
	drawRect(rect, 0xff000000);
	listBox.render();
	LogisticsScreen::render();
}

void KeyboardRef::reseed(MissionInterfaceManager::Command* commands)
{
	listBox.removeAllItems(true);
	wchar_t shift[32];
	wchar_t control[32];
	wchar_t alt[32];
	wchar_t descText[128];
	wchar_t keysString[128];
	cLoadString(IDS_SHIFT, shift, 31);
	cLoadString(IDS_CONTROL, control, 31);
	cLoadString(IDS_ALT, alt, 31);
	// first count the number of hotTexts
	int32_t count = 0;
	for (size_t i = 0; i < MAX_COMMAND; i++)
	{
		if (commands[i].hotKeyDescriptionText != -1)
		{
			count++;
		}
	}
	int32_t curCount = 0;
	for (i = 0; i < MAX_COMMAND; i++)
	{
		if (commands[i].hotKeyDescriptionText != -1)
		{
			cLoadString(commands[i].hotKeyDescriptionText, descText, 127);
			int32_t key = commands[i].key;
			std::wstring_view pKey = gos_DescribeKey((key & 0x000fffff) << 8);
			strcpy(keysString, pKey);
			if (((key & SHIFT)))
			{
				strcat(keysString, " + ");
				strcat(keysString, shift);
			}
			if (((key & CTRL)))
			{
				strcat(keysString, " + ");
				strcat(keysString, control);
			}
			if (((key & ALT)))
			{
				strcat(keysString, " + ");
				strcat(keysString, alt);
			}
			aTextListItem* item = new aTextListItem(IDS_KEYBOARD_REF_FONT);
			*item = listItemTemplate;
			item->setText(keysString);
			item->setAlignment(1);
			listBox.AddItem(item);
			int32_t yVal = item->y();
			item = new aTextListItem(IDS_KEYBOARD_REF_FONT);
			*item = listItemTemplate2;
			int32_t xVal = listItemTemplate2.left();
			item->setText(descText);
			item->setAlignment(0);
			listBox.AddItem(item);
			item->moveTo(xVal + listBox.left(), yVal);
		}
	}
}

int32_t
KeyboardRef::handleMessage(uint32_t who, uint32_t)
{
	return MissionInterfaceManager::instance()->toggleHotKeys();
}

// end of file ( KeyboardRef.cpp )
