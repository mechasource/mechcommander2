#define MPPREFS_CPP
/*************************************************************************************************\
mpprefs.cpp			: Implementation of the mpprefs component.ef
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//
\*************************************************************************************************/

#include "stdinc.h"
#include "mpprefs.h"
#include "mclib.h"
#include "mechbayscreen.h"
#include "prefs.h"
#include "multplyr.h"
#include "gamesound.h"
#include "chatwindow.h"

#define MP_PREFS_BASE 200
#define MP_PREFS_STRIPE 201
#define FIRST_COLOR_RECT 3
#define LAST_COLOR_RECT 34
#define BASE_RECT 35
#define STRIPE_RECT 36

MPPrefs* MPPrefs::s_instance = nullptr;

MPPrefs::MPPrefs()
{
	helpTextArrayID = 0;
	s_instance = this;
}

//-------------------------------------------------------------------------------------------------

MPPrefs::~MPPrefs()
{
	comboBox[0].destroy();
	comboBox[1].destroy();
	comboBox[2].destroy();
}

int32_t
MPPrefs::init(FitIniFile& file)
{
	LogisticsScreen::init(file, "Static", "Text", "Rect", "Button");
	for (size_t i = 0; i < buttonCount; i++)
	{
		if (buttons[i].getID() != MP_PREFS_BASE && buttons[i].getID() != MP_PREFS_STRIPE)
			buttons[i].setMessageOnRelease();
		int32_t id = buttons[i].getID();
		if (id != MB_MSG_NEXT && id != MB_MSG_PREV && id != MB_MSG_MAINMENU)
		{
			buttons[i].setPressFX(LOG_VIDEOBUTTONS);
			buttons[i].setHighlightFX(LOG_DIGITALHIGHLIGHT);
			buttons[i].setDisabledFX(LOG_WRONGBUTTON);
		}
	}
	file.seekBlock("ComboBoxes");
	int32_t count = 0;
	file.readIdLong("ComboBoxCount", count);
	wchar_t blockname[256];
	std::wstring_view headers[3] = {"PlayerNameComboBox", "UnitNameComboBox", "UnitInsigniaComboBox"};
	for (i = 0; i < count; i++)
	{
		sprintf(blockname, "ComboBox%ld", i);
		file.seekBlock(blockname);
		file.readIdString("filename", blockname, 255);
		int32_t tmpX;
		int32_t tmpY;
		file.readIdLong("XLocation", tmpX);
		file.readIdLong("YLocation", tmpY);
		FitIniFile tmpFile;
		FullPathFileName path;
		path.init(artPath, blockname, ".fit");
		if (NO_ERROR != tmpFile.open(path))
		{
			wchar_t error[256];
			sprintf(error, "couldn't open file %s", path);
			Assert(0, 0, error);
			return -1;
		}
		comboBox[i].moveTo(tmpX, tmpY);
		comboBox[i].init(&tmpFile, headers[i]);
		comboBox[i].ListBox().setOrange(true);
		comboBox[0].EditBox().limitEntry(24);
		comboBox[1].EditBox().limitEntry(24);
		if (i == 2)
		{
			insigniaBmp.init(&tmpFile, "Static0");
			// insigniaBmp.move( comboBox[i].globalX(), comboBox[i].globalY() );
			comboBox[i].addChild(&insigniaBmp);
		}
		comboBox[i].EditBox().allowIME(0);
	}
	comboBox[2].EditBox().setReadOnly(true);
	camera.init(rects[2].left(), rects[2].top(), rects[2].right(), rects[2].bottom());
	status = NEXT;
	return 0;
}

void MPPrefs::begin()
{
	status = RUNNING;
	comboBox[0].ListBox().removeAllItems(true);
	comboBox[0].SelectItem(-1);
	for (size_t i = 0; i < 10; i++)
	{
		if (strlen(prefs.playerName[i]))
		{
			comboBox[0].AddItem(prefs.playerName[i], 0xffffffff);
		}
		else
			break;
	}
	comboBox[0].SelectItem(0);
	comboBox[1].ListBox().removeAllItems(true);
	comboBox[1].SelectItem(-1);
	for (i = 0; i < 10; i++)
	{
		if (strlen(prefs.unitName[i]))
		{
			comboBox[1].AddItem(prefs.unitName[i], 0xffffffff);
		}
		else
			break;
	}
	comboBox[1].SelectItem(0);
	comboBox[2].ListBox().removeAllItems(true);
	comboBox[2].SelectItem(-1);
	// need to add items to the save game list
	wchar_t findString[512];
	wchar_t path[256];
	sprintf(path, "data\\multiplayer\\insignia\\");
	sprintf(findString, "%s*.tga", path);
	WIN32_FIND_DATA findResult;
	HANDLE searchHandle = FindFirstFile(findString, &findResult);
	do
	{
		// 24 or 32 bit files
		if ((findResult.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0)
		{
			aBmpListItem* pItem = new aBmpListItem;
			sprintf(path, "data\\multiplayer\\insignia\\%s", findResult.cFileName);
			if (!pItem->setBmp(findResult.cFileName))
			{
				delete pItem;
			}
			else
			{
				int32_t index = comboBox[2].AddItem(pItem);
				if (strstr(findResult.cFileName, prefs.insigniaFile))
				{
					comboBox[2].SelectItem(index);
					insigniaBmp.setTexture(path);
					insigniaBmp.setUVs(0, 0, 32, 32);
					statics[12].setTexture(path);
					statics[12].setUVs(0, 0, 32, 32);
				}
			}
		}
	} while (FindNextFile(searchHandle, &findResult) != 0);
	FindClose(searchHandle);
	if (comboBox[2].GetSelectedItem() == -1)
	{
		comboBox[2].SelectItem(0);
	}
	MC2Player* player = MPlayer->getPlayerInfo(MPlayer->commanderid);
	int32_t mySeniority = player->teamSeniority;
	bool bMostSenior = true;
	int32_t playerCount;
	const MC2Player* players = MPlayer->getPlayers(playerCount);
	for (i = 0; i < playerCount; i++)
	{
		if (players[i].teamSeniority > mySeniority && players[i].team == player->team)
		{
			bMostSenior = false;
		}
	}
	if (!bMostSenior)
	{
		getButton(MP_PREFS_BASE)->press(0);
		getButton(MP_PREFS_BASE)->disable(true);
		getButton(MP_PREFS_STRIPE)->press(true);
	}
	else
	{
		getButton(MP_PREFS_BASE)->disable(false);
		getButton(MP_PREFS_BASE)->press(true);
		getButton(MP_PREFS_STRIPE)->press(0);
	}
	camera.setMech("Bushwacker", MPlayer->colors[player->basecolour[BASECOLOR_SELF]],
		MPlayer->colors[player->stripecolour], MPlayer->colors[player->stripecolour]);
	camera.zoomIn(1.5);
}
void MPPrefs::end()
{
	camera.setMech(nullptr);
	status = NEXT;
}

void MPPrefs::update()
{
	MC2Player* player = MPlayer->getPlayerInfo(MPlayer->commanderid);
	int32_t mySeniority = player->teamSeniority;
	bool bMostSenior = true;
	int32_t playerCount;
	const MC2Player* players = MPlayer->getPlayers(playerCount);
	for (size_t i = 0; i < playerCount; i++)
	{
		if (players[i].teamSeniority > mySeniority && players[i].team == player->team)
		{
			bMostSenior = false;
		}
	}
	if (!bMostSenior)
	{
		getButton(MP_PREFS_BASE)->disable(true);
	}
	else
		getButton(MP_PREFS_BASE)->disable(false);
	if (!ChatWindow::instance()->pointInside(userInput->getMouseX(), userInput->getMouseY()))
		LogisticsScreen::update();
	camera.update();
	int32_t oldSel = comboBox[2].GetSelectedItem();
	bool bExpanded = 0;
	for (i = 0; i < 3; i++)
	{
		if (comboBox[i].ListBox().isShowing())
		{
			bExpanded = true;
		}
		comboBox[i].update();
	}
	int32_t newSel = comboBox[2].GetSelectedItem();
	if (newSel != oldSel && newSel != -1)
	{
		aBmpListItem* pItem = (aBmpListItem*)(comboBox[2].ListBox().GetItem(newSel));
		std::wstring_view pName = pItem->getBmp();
		MC2Player* pInfo = MPlayer->getPlayerInfo(MPlayer->commanderid);
		strcpy(pInfo->insigniaFile, pName);
		MPlayer->sendPlayerUpdate(0, 5, -1);
		FullPathFileName path;
		path.init("data\\multiplayer\\insignia\\", pName, ".tga");
		insigniaBmp.setTexture(path);
		insigniaBmp.setUVs(0, 0, 32, 32);
		statics[12].setTexture(path);
		statics[12].setUVs(0, 0, 32, 32);
		File file;
		file.open(path);
		int32_t size = file.getLength();
		uint8_t* pData = new uint8_t[size];
		file.read(pData, size);
		MPlayer->sendPlayerInsignia((std::wstring_view)pName, pData, size);
		MPlayer->insigniaList[MPlayer->commanderid] = 1;
		delete pData;
	}
	if (userInput->isLeftClick() && !bExpanded)
	{
		for (size_t j = FIRST_COLOR_RECT; j < LAST_COLOR_RECT + 1; j++)
		{
			if (rects[j].pointInside(userInput->getMouseX(), userInput->getMouseY()))
			{
				// OK, figure out if this is remotely viable....
				// x out colors that are already taken
				if (getButton(MP_PREFS_BASE)->isPressed())
				{
					setcolour(rects[j].getcolour());
				}
				else
					setHighlightcolour(rects[j].getcolour());
				break;
			}
		}
	}
	if (MPlayer && ChatWindow::instance())
		ChatWindow::instance()->update();
}

void MPPrefs::setcolour(uint32_t color)
{
	int32_t playerCount;
	const MC2Player* players = MPlayer->getPlayers(playerCount);
	for (size_t i = 0; i < playerCount; i++)
	{
		if (MPlayer->colors[players[i].basecolour[BASECOLOR_SELF]] == color && i != MPlayer->commanderid)
		{
			soundSystem->playDigitalSample(LOG_WRONGBUTTON);
			return;
		}
	}
	// GD:MPlayer->setPlayerBasecolour( MPlayer->commanderid, getcolourIndex(
	// color ) );
	MC2Player* player = MPlayer->getPlayerInfo(MPlayer->commanderid);
	player->basecolour[BASECOLOR_PREFERENCE] = getcolourIndex(color);
	MPlayer->sendPlayerUpdate(0, 6, -1);
	// GD:camera.setMech( "Bushwacker", color,
	// MPlayer->colors[player->stripecolour], MPlayer->colors[player->stripecolour]
	// );  GD:camera.zoomIn( 1.5 );
}

void MPPrefs::setHighlightcolour(uint32_t color)
{
	MC2Player* player = MPlayer->getPlayerInfo(MPlayer->commanderid);
	player->stripecolour = getcolourIndex(color);
	MPlayer->sendPlayerUpdate(0, 6, -1);
	camera.setMech(
		"Bushwacker", MPlayer->colors[player->basecolour[BASECOLOR_PREFERENCE]], color, color);
	camera.zoomIn(1.5);
}

wchar_t
MPPrefs::getcolourIndex(uint32_t color)
{
	for (size_t i = 0; i < MAX_COLORS; i++)
	{
		if (MPlayer->colors[i] == color)
			return i;
	}
	return 0;
}

void MPPrefs::updateBasecolours(const MC2Player* players, int32_t playerCount, bool bDrawRect)
{
	if (getButton(MP_PREFS_BASE)->isPressed())
	{
		for (size_t i = 0; i < playerCount; i++)
		{
			if (players[i].commanderid == MPlayer->commanderid)
			{
				for (size_t j = FIRST_COLOR_RECT; j < LAST_COLOR_RECT + 1; j++)
				{
					if (MPlayer->colors[players[i].basecolour[BASECOLOR_PREFERENCE]] == rects[j].getcolour())
					{
						RECT rect = {rects[j].globalX() - 1, rects[j].globalY() - 1,
							rects[j].right(), rects[j].bottom()};
						if (bDrawRect)
							drawEmptyRect(rect, 0xffffffff, 0xffffffff);
						break;
					}
				}
			}
			else
			{
				for (size_t j = FIRST_COLOR_RECT; j < LAST_COLOR_RECT + 1; j++)
				{
					if (MPlayer->colors[players[i].basecolour[BASECOLOR_SELF]] == rects[j].getcolour() && bDrawRect)
					{
						statics[21].moveTo(rects[j].globalX(), rects[j].globalY());
						statics[21].render();
						break;
					}
				}
			}
		}
	}
	MC2Player* pInfo = MPlayer->getPlayerInfo(MPlayer->commanderid);
	rects[BASE_RECT].setcolour(MPlayer->colors[pInfo->basecolour[BASECOLOR_PREFERENCE]]);
}

void MPPrefs::updateStripecolours(const MC2Player* players, int32_t playerCount, bool bDrawRect)
{
	if (getButton(MP_PREFS_STRIPE)->isPressed())
	{
		for (size_t i = 0; i < playerCount; i++)
		{
			if (players[i].commanderid == MPlayer->commanderid)
			{
				for (size_t j = FIRST_COLOR_RECT; j < LAST_COLOR_RECT + 1; j++)
				{
					if (MPlayer->colors[players[i].stripecolour] == rects[j].getcolour())
					{
						RECT rect = {rects[j].globalX() - 1, rects[j].globalY() - 1,
							rects[j].right(), rects[j].bottom()};
						if (bDrawRect)
							drawEmptyRect(rect, 0xffffffff, 0xffffffff);
						break;
					}
				}
			}
		}
	}
	MC2Player* pInfo = MPlayer->getPlayerInfo(MPlayer->commanderid);
	rects[STRIPE_RECT].setcolour(MPlayer->colors[pInfo->stripecolour]);
}

void MPPrefs ::render(int32_t OffsetX, int32_t OffsetY)
{
	statics[21].showGUIWindow(0);
	LogisticsScreen::render(OffsetX, OffsetY);
	statics[21].showGUIWindow(1);
	aObject* pObject = nullptr;
	if (OffsetX == 0 && OffsetY == 0)
	{
		camera.render();
		for (size_t i = 0; i < 3; i++)
		{
			if (!comboBox[i].ListBox().isShowing())
				comboBox[i].render();
			else
				pObject = &comboBox[i];
		}
		// x out colors that are already taken
		bool bRect = (pObject == &comboBox[2] || pObject == &comboBox[1]) ? 0 : 1;
		int32_t playerCount;
		const MC2Player* players = MPlayer->getPlayers(playerCount);
		updateBasecolours(players, playerCount, bRect);
		updateStripecolours(players, playerCount, bRect);
		textObjects[helpTextArrayID].render();
	}
	if (MPlayer && ChatWindow::instance())
		ChatWindow::instance()->render(OffsetX, OffsetY);
	if (pObject)
	{
		pObject->render();
		if (pObject == &comboBox[2] && !ChatWindow::instance()->isExpanded())
		{
			statics[16].render();
			statics[17].render();
		}
	}
}

int32_t
MPPrefs::handleMessage(uint32_t message, uint32_t who)
{
	switch (who)
	{
	case MB_MSG_NEXT:
		status = DOWN;
		saveSettings();
		break;
	case MB_MSG_PREV:
		status = DOWN;
		cancelSettings();
		break;
	case MB_MSG_MAINMENU:
		status = MAINMENU;
		break;
	case MP_PREFS_BASE:
	case MP_PREFS_STRIPE:
	{
		getButton(MP_PREFS_BASE)->press(0);
		getButton(MP_PREFS_STRIPE)->press(0);
		getButton(who)->press(1);
	}
	break;
	}
	return 0;
}

void MPPrefs::saveSettings()
{
	// check and see if name has changed
	std::wstring_view txt;
	comboBox[0].EditBox().getEntry(txt);
	if (txt != prefs.playerName[0])
	{
		prefs.setNewName(txt);
	}
	MC2Player* pInfo = MPlayer->getPlayerInfo(MPlayer->commanderid);
	strcpy(pInfo->name, txt);
	// check and see if name has changed
	comboBox[1].EditBox().getEntry(txt);
	if (txt != prefs.unitName[0])
	{
		prefs.setNewUnit(txt);
	}
	if (txt)
		strcpy(pInfo->unitName, txt);
	// colors should already be updated by now
	// update insignia
	int32_t index = comboBox[2].GetSelectedItem();
	if (index != -1)
	{
		aBmpListItem* pItem = (aBmpListItem*)(comboBox[2].ListBox().GetItem(index));
		if (pItem)
		{
			std::wstring_view pName = pItem->getBmp();
			strcpy(prefs.insigniaFile, pName);
			strcpy(pInfo->insigniaFile, pName);
		}
	}
	//	if ( MPlayer->isHost() )
	{
		prefs.basecolour = MPlayer->colors[pInfo->basecolour[BASECOLOR_PREFERENCE]];
		prefs.highlightcolour = MPlayer->colors[pInfo->stripecolour];
	}
	MPlayer->sendPlayerUpdate(0, 5, -1);
	prefs.save();
}

void MPPrefs::cancelSettings()
{
	MC2Player* pInfo = MPlayer->getPlayerInfo(MPlayer->commanderid);
	strcpy(pInfo->name, prefs.playerName[0]);
	strcpy(pInfo->unitName, prefs.unitName[0]);
	strcpy(pInfo->insigniaFile, prefs.insigniaFile);
	MPlayer->sendPlayerUpdate(0, 5, -1);
	prefs.save();
}

void MPPrefs::initcolours()
{
	for (size_t j = FIRST_COLOR_RECT; j < LAST_COLOR_RECT + 1; j++)
	{
		if (MPlayer)
			MPlayer->colors[j - FIRST_COLOR_RECT] = rects[j].getcolour();
	}
}

int32_t
aBmpListItem::setBmp(std::wstring_view pFileName)
{
	if (strlen(pFileName) >= MAXLEN_INSIGNIA_FILE)
		return 0;
	FullPathFileName path;
	path.init("data\\multiplayer\\insignia\\", pFileName, ".tga");
	TGAFileHeader header;
	File file;
	if (NO_ERROR == file.open(path))
	{
		file.read((uint8_t*)&header, sizeof(header));
		if (header.width != 32 || header.height != 32 || header.pixel_depth < 24)
			return 0;
	}
	else
		return 0;
	file.close();
	bmp.moveTo(2, 2);
	bmp.resize(32, 32);
	bmp.setTexture(path);
	bmp.setUVs(0, 0, 32, 32);
	bmp.setcolour(0xffffffff);
	resize(64, 36);
	addChild(&bmp);
	fileName = pFileName;
	return true;
}

void MPPrefs::setMechcolours(uint32_t base, uint32_t highlight)
{
	if (status == RUNNING)
	{
		camera.setMech("Bushwacker", base, highlight, highlight);
		camera.zoomIn(1.5);
	}
}

// end of file ( mpprefs.cpp )
