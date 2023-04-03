/*************************************************************************************************\
PilotReadyScreen.cpp			: Implementation of the PilotReadyScreen
component.
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//
\*************************************************************************************************/
#include "stdinc.h"

#include "PilotReadyScreen.h"
#include "LogisticsMech.h"
#include "LogisticsData.h"
#include "LogisticsMechIcon.h"
#include "resource.h"
#include "MechBayScreen.h"
#include "warrior.h"
#include "Gamesound.h"
#include "multPlyr.h"
#include "chatwindow.h"

PilotReadyScreen* PilotReadyScreen::s_instance = nullptr;

#define PR_MSG_ADD 62
#define PR_MSG_REMOVE 63
#define PR_MSG_MAIN 64

PilotReadyScreen::PilotReadyScreen()
{
	pCurPilot = nullptr;
	pIcons = nullptr;
	status = LogisticsScreen::RUNNING;
	s_instance = this;
	forceGroupCount = 0;
	pDragPilot = 0;
	dragLeft = 0;
	status = PAUSED;
	helpTextArrayID = 13;
	mechSelected = 0;
}

PilotReadyScreen::~PilotReadyScreen()
{
	if (pIcons)
		delete[] pIcons;
	pIcons = nullptr;
	LogisticsScreen::clear();
}

void PilotReadyScreen::init(FitIniFile* file)
{
	mechDisplay.init();
	// init button, texts, statics, rects
	LogisticsScreen::init(
		*file, "PilotReadyStatic", "PilotReadyTextEntry", "PilotReadyRect", "PilotReadyButton");
	// initialize little icons
	FitIniFile iconFile;
	wchar_t path[256];
	strcpy(path, artPath);
	strcat(path, "mcl_gn_deploymentteams.fit");
	strlwr(path);
	if (NO_ERROR != iconFile.open(path))
	{
		gosASSERT("couldn't open the MCL_GN_DevploymentTeams file");
		return;
	}
	LogisticsMechIcon::init(iconFile);
	pIcons = new LogisticsMechIcon[ICON_COUNT];
	int32_t count = 0;
	int32_t x = 0;
	int32_t y = 0;
	for (size_t j = 0; j < ICON_COUNT_Y; j++)
	{
		for (size_t i = 0; i < ICON_COUNT_X; i++)
		{
			pIcons[count].setHelpID(IDS_HELP_DEPLOYTEAMMW);
			pIcons[count].setMech(0);
			pIcons[count].move(x, y);
			x += pIcons[count].width();
			x += 4;
			count++;
		}
		y += pIcons[i].height();
		y += 5;
		x = 0;
	}
	int32_t mechCount[256];
	memset(mechCount, 0, sizeof(int32_t) * 256);
	// initialize the attribute meeters
	attributeMeters[0].init(file, "AttributeMeter0");
	attributeMeters[1].init(file, "AttributeMeter1");
	for (size_t i = 0; i < buttonCount; i++)
		buttons[i].setMessageOnRelease();
	pilotListBox.init(rects[0].left(), rects[0].top(), rects[0].width(), rects[0].height());
	FullPathFileName tmpPath;
	tmpPath.init(artPath, "mcl_pr_availablepilotentry", ".fit");
	FitIniFile tmpFile;
	if (NO_ERROR != tmpFile.open(tmpPath))
	{
		Assert(0, 0, "couldn't open MCL_PR_AvailablePilotEntry.fit");
	}
	LogisticsPilotListBoxItem::init(&tmpFile);
	wchar_t blockname[32];
	for (i = 0; i < 4; i++)
	{
		sprintf(blockname, "PilotSpecialtySkillIcon%ld", i);
		specialtySkillIcons[i].init(file, blockname);
	}
	for (i = 0; i < 4; i++)
	{
		sprintf(blockname, "PilotSpecialtySkill%ld", i);
		file->seekBlock(blockname);
		file->readIdLong("left", skillLocations[i].left);
		file->readIdLong("top", skillLocations[i].top);
		file->readIdLong("bottom", skillLocations[i].bottom);
		file->readIdLong("right", skillLocations[i].right);
	}
	for (i = 0; i < 5; i++)
	{
		sprintf(blockname, "PilotReadyRankIcon%ld", i);
		rankIcons[i].init(file, blockname);
	}
	for (i = 0; i < 16; i++)
	{
		sprintf(blockname, "PilotMedal%ld", i);
		medalIcons[i].init(file, blockname);
	}
	for (i = 0; i < 16; i++)
	{
		sprintf(blockname, "PilotMedalPlacement%ld", i);
		file->seekBlock(blockname);
		file->readIdLong("left", medalLocations[i].left);
		file->readIdLong("right", medalLocations[i].right);
		file->readIdLong("top", medalLocations[i].top);
		file->readIdLong("bottom", medalLocations[i].bottom);
	}
}

void PilotReadyScreen::begin()
{
	getButton(MB_MSG_PREV)->disable(false);
	getButton(MB_MSG_MAINMENU)->disable(false);
	launchFadeTime = 0;
	// initialize both the inventory and icon lists
	EList<LogisticsMech*, LogisticsMech*> mechList;
	LogisticsData::instance->getInventory(mechList);
	// reset force group
	forceGroupCount = 0;
	int32_t maxUnits = 12;
	if (MPlayer)
	{
		int32_t playerCount;
		MPlayer->getPlayers(playerCount);
		if (playerCount)
			maxUnits = MAX_MULTIPLAYER_MECHS_IN_LOGISTICS / playerCount;
		if (maxUnits > 12)
			maxUnits = 12;
	}
	for (size_t i = 0; i < 12; i++)
	{
		pIcons[i].setMech(0);
		pIcons[i].select(0);
		if (i >= maxUnits)
			pIcons[i].disable(1);
		else
			pIcons[i].disable(0);
	}
	if (getButton(MB_MSG_MAINMENU))
		getButton(MB_MSG_MAINMENU)->setPressFX(-1);
	int32_t bHasPilot = -1;
	// update pilot use here... things can get screwed up.
	pilotListBox.removeAllItems(true);
	int32_t count = 256;
	LogisticsPilot* pilots[256];
	LogisticsData::instance->getPilots(pilots, count);
	for (i = 0; i < count; i++)
	{
		pilots[i]->setUsed(0);
	}
	for (EList<LogisticsMech*, LogisticsMech*>::EIterator iter = mechList.Begin(); !iter.IsDone();
		 iter++)
	{
		if ((*iter)->getForceGroup())
		{
			int32_t FG = (*iter)->getForceGroup();
			pIcons[FG - 1].setMech((*iter));
			if ((*iter)->getPilot())
			{
				(*iter)->getPilot()->setUsed(true);
				pIcons[FG - 1].setPilot((*iter)->getPilot());
				bHasPilot = FG - 1;
			}
			forceGroupCount++;
		}
	}
	for (i = 0; i < count; i++)
	{
		if (!pilots[i]->isUsed() && pilots[i]->isAvailable())
		{
			pilotListBox.AddItem(new LogisticsPilotListBoxItem(pilots[i]));
		}
	}
	status = RUNNING;
	if (bHasPilot != -1)
	{
		pIcons[bHasPilot].select(true);
		setPilot(pIcons[bHasPilot].getPilot());
	}
	else
	{
		LogisticsPilotListBoxItem* pItem = (LogisticsPilotListBoxItem*)pilotListBox.GetItem(0);
		if (pItem)
		{
			pilotListBox.SelectItem(0);
			setPilot(pItem->getPilot());
		}
		pIcons[0].select(true);
	}
}

void PilotReadyScreen::end()
{
	mechDisplay.setMech(nullptr);
}

void PilotReadyScreen::render(int32_t xOffset, int32_t yOffset)
{
	pilotListBox.move(xOffset, yOffset);
	pilotListBox.render();
	pilotListBox.move(-xOffset, -yOffset);
	if (!xOffset && !yOffset)
	{
		if (!MPlayer && !LogisticsData::instance->isSingleMission() && LogisticsData::instance->newPilotsAvailable())
		{
			soundSystem->playBettySample(BETTY_NEW_PILOTS);
			LogisticsData::instance->setNewPilotsAcknowledged();
		}
	}
	for (size_t i = 0; i < 2; i++)
	{
		attributeMeters[i].render(xOffset, yOffset);
	}
	if (pCurPilot)
		rankIcons[pCurPilot->getRank()].render(xOffset, yOffset);
	for (i = 0; i < 4; i++)
		skillIcons[i].render(xOffset, yOffset);
	for (i = 0; i < MAX_MEDAL; i++)
		medalIcons[i].render(xOffset, yOffset);
	LogisticsScreen::render(xOffset, yOffset);
	for (i = 0; i < ICON_COUNT; i++)
	{
		pIcons[i].render(xOffset, yOffset);
	}
	if (mechSelected)
	{
		// hack, cover up pilot stuff.
		RECT rect = {77 + xOffset, 317 + yOffset, 720 + xOffset, 515 + yOffset};
		drawRect(rect, 0xff000000);
		mechDisplay.render(xOffset, yOffset);
		// hack, cover up list box overrruns.
		statics[27].render(xOffset, yOffset);
		statics[28].render(xOffset, yOffset);
	}
	if (launchFadeTime)
	{
		launchFadeTime += frameLength;
		int32_t color = interpolatecolour(0x00000000, 0x7f000000, launchFadeTime / .5f);
		RECT rect = {0, 0, Environment.screenwidth, Environment.screenheight};
		drawRect(rect, color);
	}
	if (MPlayer && ChatWindow::instance())
		ChatWindow::instance()->render(xOffset, yOffset);
	if (pDragPilot)
		dragIcon.render();
}

void PilotReadyScreen::update()
{
	if (MPlayer)
	{
		if (MPlayer->playersReadyToLoad())
			status = NEXT;
	}
	if (getButton(MB_MSG_PREV)->isEnabled()) // this is disabled if the user has
		// already pressed launch )
	{
		// update current text
		wchar_t str[64];
		// RP
		sprintf(str, "%ld ", LogisticsData::instance->getCBills());
		textObjects[1].setText(str);
		for (size_t i = 0; i < 4; i++)
			skillIcons[i].update();
		// desel icons if necessary
		bool bAllFull = 1;
		int32_t newSel = -1;
		int32_t oldSel = -1;
		bool bHasMech = 0;
		int32_t newRightSel = -1;
		for (i = 0; i < ICON_COUNT; i++)
		{
			pIcons[i].update();
			if (pIcons[i].justSelected())
				newSel = i;
			else if (pIcons[i].isSelected())
				oldSel = i;
			bool bInside = pIcons[i].pointInside(userInput->getMouseX(), userInput->getMouseY());
			if (userInput->isLeftDoubleClick() && bInside && pIcons[i].getMech())
			{
				removeSelectedPilot();
			}
			if (userInput->isRightClick() && bInside && pIcons[i].getMech())
			{
				newRightSel = i;
			}
			else if (userInput->isLeftDrag() && bInside && pIcons[i].pointInside(userInput->getMouseDragX(), userInput->getMouseDragY()) && pIcons[i].getMech())
			{
				beginDrag(pIcons[i].getMech()->getPilot());
				pIcons[i].dimPilot(true);
				pIcons[i].setPilot(nullptr);
			}
			if (pIcons[i].getMech() && !pIcons[i].getPilot())
				bAllFull = 0;
			if (pIcons[i].getMech())
				bHasMech = true;
		}
		if (newSel != -1)
		{
			if (oldSel != -1)
				pIcons[oldSel].select(0);
			if (pIcons[newSel].getPilot())
			{
				setPilot(pIcons[newSel].getPilot());
			}
			else
				setMech(pIcons[newSel].getMech());
		}
		if (newRightSel != -1)
		{
			setMech(pIcons[newRightSel].getMech());
		}
		int32_t curSel = newSel == -1 ? oldSel : newSel;
		if (!MPlayer || !ChatWindow::instance()->pointInside(userInput->getMouseX(), userInput->getMouseY()))
			LogisticsScreen::update();
		if (mechSelected && (!MPlayer || !ChatWindow::instance()->pointInside(userInput->getMouseX(), userInput->getMouseY())))
		{
			if (userInput->getMouseY() > 317)
			{
				helpid = 0;
				textObjects[helpTextArrayID].setText("");
			}
			mechDisplay.update();
		}
		if (!bAllFull) // disable the next button until all have pilots
		{
			buttons[1].disable(1);
			if (curSel == -1 || pIcons[curSel].getPilot())
				buttons[2].disable(1);
			else
				buttons[2].disable(0);
		}
		else
		{
			if (bHasMech)
				buttons[1].disable(0);
			else
				buttons[1].disable(1);
			buttons[2].disable(1);
		}
		if ((newSel == -1 && oldSel == -1) || !pIcons[curSel].getPilot())
		{
			// disable the remove button
			buttons[3].disable(1);
		}
		else
			buttons[3].disable(0);
		for (i = 0; i < MAX_MEDAL; i++) // so we get rollovers
			medalIcons[i].update();
		// update drag and drop
		if (pDragPilot)
		{
			dragIcon.moveTo(userInput->getMouseX() - dragIcon.width() / 2,
				userInput->getMouseY() - dragIcon.height() / 2);
			LogisticsMechIcon* pSelIcon = 0;
			for (size_t i = 0; i < ICON_COUNT; i++)
			{
				if (pIcons[i].pointInside(userInput->getMouseX(), userInput->getMouseY()) && pIcons[i].getMech())
				{
					pSelIcon = &pIcons[i];
					break;
				}
			}
			if (userInput->leftMouseReleased())
			{
				endDrag(pSelIcon);
			}
		}
		pilotListBox.update();
	}
	if (MPlayer && ChatWindow::instance())
		ChatWindow::instance()->update();
}

int32_t
PilotReadyScreen::handleMessage(uint32_t message, uint32_t who)
{
	switch (who)
	{
	case MB_MSG_NEXT:
		if (MPlayer)
		{
			MPlayer->sendMissionSetup(0, 7, nullptr);
			getButton(MB_MSG_PREV)->disable(true);
			getButton(MB_MSG_NEXT)->disable(true);
			getButton(MB_MSG_MAINMENU)->disable(true);
			MC2Player* pInfo = MPlayer->getPlayerInfo(MPlayer->commanderid);
			if (pInfo)
			{
				std::wstring_view name = pInfo->name;
				MPlayer->sendPlayerActionChat(nullptr, name, IDS_MP_PLAYER_READY);
			}
			pilotListBox.SelectItem(-1);
			launchFadeTime = .0001f;
		}
		else
			status = NEXT;
		break;
	case MB_MSG_PREV:
		status = PREVIOUS;
		break;
	case PR_MSG_ADD:
		addSelectedPilot();
		break;
	case PR_MSG_REMOVE:
		removeSelectedPilot();
		break;
	case MB_MSG_MAINMENU:
		status = MAINMENU;
		break;
	}
	return 0;
}

void PilotReadyScreen::addSelectedPilot()
{
	aListItem* pItem = nullptr;
	bool bFound = 0;
	int32_t index = pilotListBox.GetSelectedItem();
	if (index != -1)
		pItem = pilotListBox.GetItem(index);
	if (pItem)
	{
		LogisticsPilot* pPilot = ((LogisticsPilotListBoxItem*)pItem)->getPilot();
		if (pPilot)
		{
			for (size_t i = 0; i < ICON_COUNT; i++)
			{
				if (pIcons[i].isSelected())
				{
					if (!pIcons[i].getPilot() && pIcons[i].getMech())
					{
						pIcons[i].setPilot(pPilot);
						pIcons[i].select(0);
						i++;
						if (i >= ICON_COUNT || !pIcons[i].getMech())
							i = 0;
						pIcons[i].select(true);
						bFound = true;
						break;
					}
				}
			}
			if (!bFound)
			{
				// if we got here, there was nothing selected
				for (i = 0; i < ICON_COUNT; i++)
				{
					if (pIcons[i].getMech() && !pIcons[i].getPilot())
					{
						pIcons[i].setPilot(pPilot);
						bFound = true;
						break;
					}
				}
			}
		}
	}
	if (index != -1)
		pItem = pilotListBox.GetItem(index + 1);
	if (pItem && bFound)
	{
		LogisticsPilot* pPilot = ((LogisticsPilotListBoxItem*)pItem)->getPilot();
		if (pPilot)
		{
			setPilot(pPilot);
		}
	}
}

void PilotReadyScreen::removeSelectedPilot()
{
	for (size_t i = 0; i < ICON_COUNT; i++)
	{
		if (pIcons[i].isSelected())
		{
			if (pIcons[i].getPilot())
			{
				// add the pilot back into the list
				int32_t oldSel = pilotListBox.GetSelectedItem();
				LogisticsPilotListBoxItem* pNewItem =
					new LogisticsPilotListBoxItem(pIcons[i].getPilot());
				int32_t newIndex = pilotListBox.AddItem(pNewItem);
				pIcons[i].setPilot(nullptr);
				pIcons[i].select(0);
				i++;
				if (i >= ICON_COUNT || !pIcons[i].getMech())
					i = 0;
				pIcons[i].select(true);
				if (pIcons[i].getPilot())
				{
					if (newIndex <= oldSel)
					{
						pilotListBox.SelectItem(oldSel + 1);
					}
					setPilot(pIcons[i].getPilot());
				}
				else
				{
					pilotListBox.SelectItem(newIndex);
					int32_t index = pilotListBox.GetSelectedItem();
					if (index != -1)
					{
						LogisticsPilotListBoxItem* pItem =
							(LogisticsPilotListBoxItem*)pilotListBox.GetItem(index);
						if (pItem)
						{
							LogisticsPilot* pPilot =
								((LogisticsPilotListBoxItem*)pItem)->getPilot();
							if (pPilot)
							{
								setPilot(pPilot);
							}
						}
					}
				}
			}
			return;
		}
	}
}

void PilotReadyScreen::beginDrag(LogisticsPilot* pPilot)
{
	if (!pDragPilot)
		pDragPilot = pPilot;
	if (pPilot)
		LogisticsPilotListBox::makeUVs(pPilot, dragIcon);
	launchFadeTime = 0.f;
}

void PilotReadyScreen::endDrag(LogisticsMechIcon* pIcon)
{
	if (pIcon)
	{
		LogisticsPilot* pOldPilot = pIcon->getPilot();
		if (pOldPilot)
			putBackPilot(pOldPilot);
		pIcon->setPilot(pDragPilot);
		pilotListBox.removePilot(pDragPilot);
	}
	else
	{
		putBackPilot(pDragPilot);
	}
	pDragPilot = 0;
}

void PilotReadyScreen::putBackPilot(LogisticsPilot* pPilot)
{
	for (size_t i = 0; i < ICON_COUNT; i++)
	{
		if (pIcons[i].getPilot() == pPilot)
		{
			pIcons[i].setPilot(0);
			pIcons[i].dimPilot(false);
		}
	}
	LogisticsPilotListBoxItem* pItem = new LogisticsPilotListBoxItem(pPilot);
	pilotListBox.AddItem(pItem);
}

void PilotReadyScreen::setPilot(LogisticsPilot* pPilot)
{
	mechSelected = 0;
	if (pPilot != pCurPilot)
	{
		pCurPilot = pPilot;
		// set up the big bitmap
		if (!pCurPilot)
		{
			statics[staticCount - 1].setcolour(0);
			return;
		}
		wchar_t fileName[256];
		strcpy(fileName, artPath);
		strcat(fileName, "MCL_PR_");
		strcat(fileName, pPilot->getName());
		strcat(fileName, ".tga");
		std::wstring_view str = fileName;
		str.MakeLower();
		statics[67].setTexture(str);
		statics[67].setcolour(0xffffffff);
		statics[67].setUVs(0, 0, 92, 128);
	}
	// update pilot specific stuff
	if (pCurPilot)
	{
		wchar_t str[256];
		// ACE not continguous with other ranks.  Added too late!
		if (pCurPilot->getRank() != 4)
			textObjects[4].setText(IDS_GREEN + pCurPilot->getRank());
		else
			textObjects[4].setText(IDS_ACE);
		sprintf(str, "%ld", (int32_t)pCurPilot->getGunnery());
		textObjects[5].setText(str);
		attributeMeters[0].setValue(((float)pCurPilot->getGunnery()) / 80.f);
		wchar_t number[64];
		sprintf(number, "%ld", (int32_t)pCurPilot->getPiloting());
		textObjects[6].setText(number);
		attributeMeters[1].setValue(pCurPilot->getPiloting() / 80.f);
		sprintf(number, "%ld", pCurPilot->getMechKills());
		textObjects[7].setText(number);
		std::wstring_view name = pCurPilot->getName();
		name.MakeUpper();
		textObjects[8].setText(name);
		int32_t count = 32;
		std::wstring_view specialtySkills[32];
		int32_t skillIDs[32];
		pCurPilot->getSpecialtySkills(specialtySkills, count);
		count = 32;
		pCurPilot->getSpecialtySkills(skillIDs, count);
		for (size_t i = 0; i < 4; i++)
		{
			if (i < count)
			{
				textObjects[9 + i].setText(specialtySkills[i]);
				textObjects[9 + i].resize(
					textObjects[9 + i].width(), textObjects[9 + i].font.height());
				textObjects[9 + i].setHelpID(skillIDs[i] + IDS_SPECIALTY_HELP1);
				skillIcons[i] = specialtySkillIcons[MechWarrior::skillTypes[skillIDs[i]]];
				skillIcons[i].resize(skillLocations[i].right - skillLocations[i].left,
					skillLocations[i].bottom - skillLocations[i].top);
				skillIcons[i].moveTo(skillLocations[i].left, skillLocations[i].top);
				skillIcons[i].setcolour(0xffffffff);
				skillIcons[i].setHelpID(skillIDs[i] + IDS_SPECIALTY_HELP1);
			}
			else
			{
				textObjects[9 + i].setText("");
				skillIcons[i].setcolour(0);
				skillIcons[i].setHelpID(0);
			}
		}
		for (; i < 4; i++)
			skillIcons[i].setcolour(0);
		const bool* medals = pCurPilot->getMedalArray();
		int32_t medalCount = 0;
		for (i = 0; i < MAX_MEDAL; i++)
		{
			if (medals[i])
			{
				medalIcons[i].moveTo(
					medalLocations[medalCount].left, medalLocations[medalCount].top);
				medalIcons[i].resize(
					medalLocations[medalCount].right - medalLocations[medalCount].left,
					medalLocations[medalCount].bottom - medalLocations[medalCount].top);
				medalIcons[i].setcolour(0xffffffff);
				medalCount++;
				medalIcons[i].showGUIWindow(true);
				medalIcons[i].setHelpID(IDS_HELP_MEDAL1 + i);
			}
			else
			{
				medalIcons[i].showGUIWindow(0);
			}
		}
	}
	else
	{
		for (size_t i = 4; i < 13; i++)
			textObjects[i].setText("");
		statics[staticCount - 1].setcolour(0);
	}
}

void PilotReadyScreen::setMech(LogisticsMech* pMech)
{
	mechSelected = true;
	mechDisplay.setMech(pMech, 0);
}