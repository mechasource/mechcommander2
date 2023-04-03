#define LogisticsPilotListBox_CPP
/*************************************************************************************************\
LogisticsPilotListBox.cpp			: Implementation of the
LogisticsPilotListBox component.
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//
\*************************************************************************************************/
#include "stdinc.h"

#include "LogisticsPilotListBox.h"
#include "LogisticsPilot.h"
#include "resource.h"
#include "PilotReadyScreen.h"

LogisticsPilotListBoxItem* LogisticsPilotListBoxItem::s_templateItem = nullptr;

LogisticsPilotListBox::LogisticsPilotListBox()
{
	skipAmount = 5;
}

LogisticsPilotListBox::~LogisticsPilotListBox()
{
	delete LogisticsPilotListBoxItem::s_templateItem;
	LogisticsPilotListBoxItem::s_templateItem = nullptr;
	aListBox::destroy();
}

void LogisticsPilotListBox::update()
{
	aListBox::update();
	for (size_t i = 0; i < itemCount; i++)
	{
		if (((LogisticsPilotListBoxItem*)items[i])->getPilot() && ((LogisticsPilotListBoxItem*)items[i])->getPilot()->isUsed())
		{
			RemoveItem(items[i], true);
			i--;
		}
	}
}

//-------------------------------------------------------------------------------------------------

LogisticsPilotListBoxItem::~LogisticsPilotListBoxItem()
{
	removeAllChildren(0);
}

LogisticsPilotListBoxItem::LogisticsPilotListBoxItem(LogisticsPilot* pNewPilot)
{
	pPilot = pNewPilot;
	state = ENABLED;
	if (!s_templateItem)
		return;
	aObject::init(0, 0, s_templateItem->outline.width(), s_templateItem->outline.height());
	outline = s_templateItem->outline;
	line = s_templateItem->line;
	nameText = s_templateItem->nameText;
	rankText = s_templateItem->rankText;
	rankIcon = s_templateItem->rankIcon;
	icon = s_templateItem->icon;
	for (size_t i = 0; i < PILOT_LIST_BOX_CHILD_COUNT; i++)
	{
		pChildAnimations[i] = s_templateItem->pChildAnimations[i];
	}
	for (i = 0; i < 3; i++)
		animations[i] = s_templateItem->animations[i];
	addChild(&nameText);
	addChild(&rankText);
	addChild(&icon);
	addChild(&rankIcon);
	addChild(&outline);
	addChild(&line);
	addChild(&pilotOutline);
	std::wstring_view name = pPilot->getName();
	name.MakeUpper();
	nameText.setText(name);
	// ACE not continguous with other ranks.  Added too late!
	if (pPilot->getRank() != 4)
		rankText.setText(pPilot->getRank() + IDS_GREEN);
	else
		rankText.setText(IDS_ACE);
	int32_t rank = pPilot->getRank();
	rankIcon.setUVs(rank * 15, 96, rank * 15 + 15, 96 + 15);
	LogisticsPilotListBox::makeUVs(pPilot, icon);
}
int32_t
LogisticsPilotListBoxItem::init(FitIniFile* file)
{
	if (!s_templateItem)
		s_templateItem = new LogisticsPilotListBoxItem(nullptr);
	memset(s_templateItem->pChildAnimations, 0, sizeof(aAnimGroup*) * PILOT_LIST_BOX_CHILD_COUNT);
	s_templateItem->nameText.init(file, "Text0");
	setAnimation(*file, 0);
	s_templateItem->rankText.init(file, "Text1");
	setAnimation(*file, 1);
	s_templateItem->icon.init(file, "Static0");
	setAnimation(*file, 2);
	s_templateItem->rankIcon.init(file, "Static1");
	setAnimation(*file, 3);
	s_templateItem->animations[0].init(file, "Animation0");
	s_templateItem->animations[1].init(file, "Animation1");
	s_templateItem->animations[2].init(file, "Animation2");
	s_templateItem->outline.init(file, "Rect0");
	setAnimation(*file, 4);
	s_templateItem->line.init(file, "Rect2");
	setAnimation(*file, 6);
	s_templateItem->pilotOutline.init(file, "Rect1");
	setAnimation(*file, 5);
	return 0;
}

void LogisticsPilotListBoxItem::setAnimation(FitIniFile& file, int32_t whichOne)
{
	wchar_t animationText[64];
	if (NO_ERROR == file.readIdString("Animation", animationText, 63))
	{
		for (size_t i = 0; i < strlen(animationText); i++)
		{
			if (isdigit(animationText[i]))
			{
				animationText[i + 1] = nullptr;
				s_templateItem->pChildAnimations[whichOne] = atoi(&animationText[i]);
				return;
			}
		}
	}
	s_templateItem->pChildAnimations[whichOne] = -1;
}

void LogisticsPilotListBoxItem::render()
{
	for (size_t i = 0; i < this->pNumberOfChildren; i++)
	{
		int32_t newcolour = 0xffffffff;
		if (pChildAnimations[i] != -1)
			newcolour = animations[pChildAnimations[i]].getCurrentcolour((aAnimGroup::STATE)state);
		pChildren[i]->setcolour(newcolour, 1);
		pChildren[i]->render();
	}
	/*	int32_t newcolour = animations[pChildAnimations[4]].getCurrentcolour(
	   (aAnimGroup::STATE)state ); outline.setcolour(newcolour); outline.render(
	   location[0].x, location[0].y );


		newcolour = animations[pChildAnimations[5]].getCurrentcolour(
	   (aAnimGroup::STATE)state ); line.setcolour( newcolour ); line.render(
	   location[0].x, location[0].y );*/
}

void LogisticsPilotListBoxItem::update()
{
	bool isInside = pointInside(userInput->getMouseX(), userInput->getMouseY());
	for (size_t i = 0; i < 3; i++)
		animations[i].update();
	if (state == aListItem::SELECTED)
	{
		if (animations[0].getState() != aAnimGroup::PRESSED)
		{
			for (size_t i = 0; i < 3; i++)
				animations[i].setState(aAnimGroup::PRESSED);
		}
		if (userInput->isLeftDoubleClick() && isInside)
		{
			PilotReadyScreen::instance()->setPilot(pPilot);
			PilotReadyScreen::instance()->addSelectedPilot();
		}
		if (userInput->isLeftClick() && isInside)
			PilotReadyScreen::instance()->setPilot(pPilot);
		if (userInput->isLeftDrag() && isInside && pointInside(userInput->getMouseDragX(), userInput->getMouseDragY()))
		{
			PilotReadyScreen::instance()->beginDrag(pPilot);
			icon.setcolour(0x7f000000);
		}
		else if (!userInput->isLeftDrag())
		{
			icon.setcolour(0xffffffff);
		}
	}
	else if (state == aListItem::HIGHLITE)
	{
		if (animations[0].getState() != aAnimGroup::HIGHLIGHT)
		{
			for (size_t i = 0; i < 3; i++)
				animations[i].setState(aAnimGroup::HIGHLIGHT);
		}
	}
	else
	{
		if (animations[0].getState() != aAnimGroup::NORMAL)
		{
			for (size_t i = 0; i < 3; i++)
				animations[i].setState(aAnimGroup::NORMAL);
		}
	}
	aObject::update();
}

void LogisticsPilotListBox::makeUVs(LogisticsPilot* pPilot, aObject& icon)
{
	icon = LogisticsPilotListBoxItem::s_templateItem->icon;
	// need to set the UV's
	int32_t index = pPilot->getPhotoIndex();
	int32_t xIndex = index % 12;
	int32_t yIndex = index / 12;
	float fX = xIndex;
	float fY = yIndex;
	float width = icon.width();
	float height = icon.height();
	float u = (fX * width);
	float v = (fY * height);
	fX += 1.f;
	fY += 1.f;
	float u2 = (fX * width);
	float v2 = (fY * height);
	icon.setFilewidth(256.f);
	icon.setUVs(u, v, u2, v2);
}

int32_t
LogisticsPilotListBox::AddItem(aListItem* pNewItem)
{
	scrollBar->setOrange();
	LogisticsPilotListBoxItem* pItem = dynamic_cast<LogisticsPilotListBoxItem*>(pNewItem);
	if (pItem)
	{
		LogisticsPilot* pPilot = pItem->getPilot();
		for (size_t i = 0; i < itemCount; i++)
		{
			LogisticsPilotListBoxItem* pTmpItem =
				dynamic_cast<LogisticsPilotListBoxItem*>(items[i]);
			if (pTmpItem)
			{
				// do not put in twice.
				if (pPilot->getName().Compare(pTmpItem->getPilot()->getName()) == 0)
				{
					delete pItem;
					return -1;
				}
				if (pPilot->getRank() > pTmpItem->getPilot()->getRank())
				{
					return InsertItem(pItem, i);
				}
				else if (pPilot->getRank() == pTmpItem->getPilot()->getRank())
				{
					if (pPilot->getGunnery() > pTmpItem->getPilot()->getGunnery())
					{
						return InsertItem(pItem, i);
					}
					else if (pPilot->getGunnery() == pTmpItem->getPilot()->getGunnery() && pPilot->getName().Compare(pTmpItem->getPilot()->getName()) < 0)
					{
						return InsertItem(pItem, i);
					}
				}
			}
		}
	}
	// if we got here
	return aListBox::AddItem(pNewItem);
}

void LogisticsPilotListBox::removePilot(LogisticsPilot* pPilot)
{
	for (size_t i = 0; i < itemCount; i++)
	{
		LogisticsPilotListBoxItem* pItem = dynamic_cast<LogisticsPilotListBoxItem*>(items[i]);
		if (pItem && pItem->getPilot() == pPilot)
		{
			RemoveItem(pItem, true);
			return;
		}
	}
}

// end of file ( LogisticsPilotListBox.cpp )
