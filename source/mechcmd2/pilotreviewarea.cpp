#define SALVAGEMECHAREA_CPP
/*************************************************************************************************\
SalvageMechArea.cpp			: Implementation of the SalvageMechArea component.
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//
\*************************************************************************************************/
#include "stdinc.h"

#include "PilotReviewArea.h"
#include "IniFile.h"
#include "objMgr.h"
#include "LogisticsPilot.h"
#include "LogisticsData.h"
#include "resource.h"
#include "mech.h"
#include <malloc.h>
#include "MechIcon.h"
#include "windows.h"
#include "gameSound.h"

aText* DeadPilotListItem::s_nameText = nullptr;
aText* DeadPilotListItem::s_rankText = nullptr;
aText* DeadPilotListItem::s_missionText = nullptr;
aText* DeadPilotListItem::s_killsText = nullptr;
aRect* DeadPilotListItem::s_area = nullptr;
aRect* DeadPilotListItem::s_liveIconRect = nullptr;
aRect* DeadPilotListItem::s_deadIconRect = nullptr;
int32_t DeadPilotListItem::s_itemCount = 0;

aText* ActivePilotListItem::s_nameText = nullptr;
aText* ActivePilotListItem::s_missionText = nullptr;
aText* ActivePilotListItem::s_killsText = nullptr;

aText* ActivePilotListItem::s_rankText = nullptr;
aText* ActivePilotListItem::s_gunneryText = nullptr;
aText* ActivePilotListItem::s_pilotingText = nullptr;

int32_t ActivePilotListItem::s_itemCount = 0;
aRect* ActivePilotListItem::s_outline[5];
aObject* ActivePilotListItem::s_icons[8];
aRect* ActivePilotListItem::s_area = nullptr;
aRect* ActivePilotListItem::s_iconRect = nullptr;
AttributeMeter* ActivePilotListItem::s_attributeMeters[2] = {0, 0};
aRect* ActivePilotListItem::s_killIconRect = nullptr;
PilotPromotionArea* ActivePilotListItem::s_pilotPromotionArea = nullptr;
aText* ActivePilotListItem::s_medalAwardedText = 0;
aText* ActivePilotListItem::s_medalText = 0;
aText* ActivePilotListItem::s_promotionText = 0;
aObject* ActivePilotListItem::s_medals[MAX_MEDAL] = {0};

aAnimation* ActivePilotListItem::s_skillAnim = nullptr;
aAnimation* ActivePilotListItem::s_medalAwardedAnim = nullptr;
aAnimation* ActivePilotListItem::s_pilotPromotedAnim = nullptr;
int32_t ActivePilotListItem::s_totalwidth = 0;

PilotPromotionArea* PilotReviewScreen::s_curPromotion = nullptr;
;

aButton* SpecialtyListItem::s_radioButton = 0;
aObject* SpecialtyListItem::s_skillIcons[4] = {0, 0, 0, 0};
aAnimation* SpecialtyListItem::s_highlightAnim = 0;
aAnimation* SpecialtyListItem::s_normalAnim = 0;
aAnimation* SpecialtyListItem::s_pressedAnim = 0;
aText* SpecialtyListItem::s_description = 0;
aRect* SpecialtyListItem::s_area = 0;
aRect* SpecialtyListItem::s_outline = 0;
int32_t SpecialtyListItem::s_itemCount = 0; // hack, we really don't want to delete these each time

PilotReviewScreen* PilotReviewScreen::instance = nullptr;

extern float frameLength;

#define RP_TEXTID 3
#define HELP_TEXTID 2
#define NAME_TEXTID 0
#define WEIGHT_TEXTID 2
#define ARMOR_TEXTID 3
#define SPEED_TEXTID 4
#define JUMP_TEXTID 5
#define APPEARTIME .5f
#define CHECK_BUTTON 200

PilotReviewScreen::PilotReviewScreen()
{
	statics = 0;
	rects = 0;
	staticCount = rectCount = buttonCount = textCount = 0;
	buttons = 0;
	textObjects = 0;
	bDone = 0;
	instance = this;
	// this MUST be initialized
	LogisticsData::instance->init();
	fadeOutMaxcolour = 0x7f000000;
}

PilotReviewScreen::~PilotReviewScreen()
{
	pilotListBox.removeAllItems(true);
	int32_t count = LogisticsData::instance->getPilotCount();
	LogisticsPilot** pPilots = (LogisticsPilot**)_alloca(count * sizeof(LogisticsPilot*));
	LogisticsData::instance->getPilots(pPilots, count);
	bool bDeadTextAdded = 0;
	bool bFirst = 0;
	for (size_t i = 0; i < count; i++)
	{
		pPilots[i]->clearIcons();
		pPilots[i]->setJustDied(0);
	}
}

void PilotReviewScreen::init(FitIniFile* file)
{
	LogisticsScreen::init(
		*file, "PilotReviewStatic", "PilotReviewText", "PilotReviewRect", "PilotReviewButton");
	file->seekBlock("PilotReviewMisc");
	int32_t tmp;
	file->readIdLong("PilotReviewSmallSkip", tmp);
	pilotListBox.setSpaceBetweenItems(tmp);
	int32_t color, font;
	file->readIdLong("color", color);
	file->readIdLong("font", font);
	pilotListBox.init(rects[0].left(), rects[0].top(), rects[0].width(), rects[0].height());
	DeadPilotListItem::init(file);
	ActivePilotListItem::init(file);
	ActivePilotListItem::s_totalwidth = pilotListBox.width() - 70;
	int32_t count = LogisticsData::instance->getPilotCount();
	LogisticsPilot** pPilots = (LogisticsPilot**)_alloca(count * sizeof(LogisticsPilot*));
	LogisticsData::instance->getPilots(pPilots, count);
	bool bDeadTextAdded = 0;
	bool bFirst = 0;
	for (size_t i = 0; i < count; i++)
	{
		if (pPilots[i]->justDied() && pPilots[i]->isUsed())
		{
			if (!bDeadTextAdded)
			{
				aTextListItem* pItem = new aTextListItem(font);
				pItem->setText(IDS_KILLED_IN_ACTION);
				pItem->setcolour(color);
				pilotListBox.AddItem(pItem);
				bDeadTextAdded = true;
			}
			DeadPilotListItem* pItem = new DeadPilotListItem(pPilots[i]);
			if (!bFirst)
			{
				pItem->begin();
				pItem->showGUIWindow(0);
			}
			bFirst = 1;
			pilotListBox.AddItem(pItem);
		}
	}
	if (bDeadTextAdded)
	{
		aTextListItem* pItem = new aTextListItem(font);
		pItem->setText("");
		pItem->setcolour(color);
		pilotListBox.AddItem(pItem);
	}
	bool bLiveTextAdded = 0;
	bool bJustPromoted = 0;
	for (i = 0; i < count; i++)
	{
		if (!pPilots[i]->isDead() && pPilots[i]->isUsed())
		{
			if (!bLiveTextAdded)
			{
				aTextListItem* pItem = new aTextListItem(font);
				pItem->setText(IDS_ACTIVE_DUTY);
				pItem->setcolour(color);
				int32_t oldheight = pItem->height();
				pItem->resize(pItem->width(), oldheight + 20);
				if (bDeadTextAdded)
					pItem->showGUIWindow(0);
				pilotListBox.AddItem(pItem);
				bLiveTextAdded = true;
			}
			ActivePilotListItem* pItem = new ActivePilotListItem(pPilots[i]);
			pilotListBox.AddItem(pItem);
			if (pPilots[i]->promotePilot())
				bJustPromoted = true;
		}
	}
	entryAnim.initWithBlockName(file, "PilotAreaBeginAnimation");
	entryAnim.begin();
	exitAnim.initWithBlockName(file, "PilotReviewFadeOutAnimation");
	beginFadeIn(0);
	gosASSERT(rects); // need this for the list box
	if (bJustPromoted) // disable done button if promotion is going to happen
		buttons[0].disable(true);
}

void PilotReviewScreen::render()
{
	int32_t xOffset = 0;
	int32_t yOffset = 0;
	if (!entryAnim.isDone())
	{
		xOffset = entryAnim.getXDelta();
		yOffset = entryAnim.getYDelta();
	}
	else if (bDone)
	{
		xOffset = exitAnim.getXDelta();
		yOffset = exitAnim.getYDelta();
	}
	pilotListBox.move(xOffset, yOffset);
	pilotListBox.render();
	pilotListBox.move(-xOffset, -yOffset);
	LogisticsScreen::render(xOffset, yOffset);
	if (s_curPromotion)
		s_curPromotion->render();
}

int32_t
PilotReviewScreen::handleMessage(uint32_t message, uint32_t who)
{
	if (who == aMSG_DONE && entryAnim.isDone())
	{
		bDone = true;
		exitAnim.begin();
		beginFadeOut(1.0);
		soundSystem->stopBettySample(); // don't want to carry droning on to
			// next screen
		return 1;
	}
	return 0;
}

bool PilotReviewScreen::isDone()
{
	return bDone && exitAnim.isDone();
}
void PilotReviewScreen::update()
{
	if (s_curPromotion && !bDone)
	{
		s_curPromotion->update();
		if (s_curPromotion->isDone())
			s_curPromotion = 0;
		return;
	}
	if (entryAnim.isDone())
		pilotListBox.update();
	for (size_t i = 0; i < buttonCount; i++)
	{
		buttons[i].update();
		if (buttons[i].pointInside(userInput->getMouseX(), userInput->getMouseY()) && userInput->isLeftClick() && buttons[i].isEnabled())
		{
			handleMessage(aMSG_DONE, aMSG_DONE);
		}
	}
	if (pilotListBox.isDone())
		buttons[0].disable(false);
	entryAnim.update();
	exitAnim.update();
}

void PilotReviewScreen::updatePilots()
{
}

//////////////////////////////////////////////

DeadPilotListItem::~DeadPilotListItem()
{
	removeAllChildren(false); // don't delete
	if (liveIcon)
		delete liveIcon;
	if (deadIcon)
		delete deadIcon;
	deadIcon = liveIcon = nullptr;
	s_itemCount--;
	if (s_itemCount < 1)
	{
		if (s_area)
			delete s_area;
		s_area = nullptr;
		if (s_missionText)
			delete s_missionText;
		s_missionText = nullptr;
		if (s_killsText)
			delete s_killsText;
		s_killsText = nullptr;
		if (s_rankText)
			delete s_rankText;
		s_rankText = nullptr;
		if (s_nameText)
			delete s_nameText;
		s_nameText = nullptr;
		if (s_liveIconRect)
			delete s_liveIconRect;
		s_liveIconRect = nullptr;
		if (s_deadIconRect)
			delete s_deadIconRect;
		s_deadIconRect = nullptr;
	}
}

bool DeadPilotListItem::isDone()
{
	return currentTime > APPEARTIME ? 1 : 0;
}
void DeadPilotListItem::init(FitIniFile* file)
{
	s_area = new aRect;
	s_missionText = new aText;
	s_killsText = new aText;
	s_rankText = new aText;
	s_nameText = new aText;
	s_liveIconRect = new aRect;
	s_deadIconRect = new aRect;
	s_area->init(file, "PilotReviewKIABox");
	s_liveIconRect->init(file, "KIABoxPilotIcon");
	s_deadIconRect->init(file, "KIABoxDeadIcon");
	s_missionText->init(file, "KIAMissionsText");
	s_rankText->init(file, "KIARankText");
	s_nameText->init(file, "KIAPilotNameText");
	s_killsText->init(file, "KIAKillsText");
}

DeadPilotListItem::DeadPilotListItem(LogisticsPilot* pPilot)
{
	wchar_t tmpText[256];
	wchar_t realText[256];
	aObject::init(s_area->left(), s_area->top(), s_area->width(), s_area->height());
	killsText = *s_killsText;
	nameText = *s_nameText;
	rankText = *s_rankText;
	missionText = *s_missionText;
	int32_t missions = pPilot->getNumberMissions();
	cLoadString(IDS_PILOT_MISSIONS, tmpText, 255);
	sprintf(realText, tmpText, missions);
	missionText.setText(realText);
	int32_t numberOfKills =
		pPilot->getInfantryKills() + pPilot->getMechKills() + pPilot->getVehicleKills();
	cLoadString(IDS_PILOT_KILLS, tmpText, 255);
	sprintf(realText, tmpText, numberOfKills);
	killsText.setText(realText);
	std::wstring_view name = pPilot->getName();
	cLoadString(IDS_PILOT_NAME, tmpText, 255);
	sprintf(realText, tmpText, name);
	nameText.setText(realText);
	int32_t rank = pPilot->getRank();
	wchar_t rankStr[32];
	// ACE Not Contiguous with other ranks.  Added too late.
	if (rank != 4)
		cLoadString(rank + IDS_GREEN, rankStr, 31);
	else
		cLoadString(IDS_ACE, rankStr, 31);
	cLoadString(IDS_PILOT_RANK, tmpText, 255);
	sprintf(realText, tmpText, rankStr);
	rankText.setText(realText);
	addChild(&killsText);
	addChild(&nameText);
	addChild(&rankText);
	addChild(&missionText);
	liveIcon = new PilotIcon();
	liveIcon->setTextureIndex(pPilot->getPhotoIndex());
	deadIcon = new PilotIcon();
	deadIcon->setTextureIndex(PilotIcon::DEAD_PILOT_INDEX);
	showWindow = 0;
}

void DeadPilotListItem::update()
{
	PilotListItem::update();
}

void DeadPilotListItem::render()
{
	if (!showWindow)
		return;
	RECT tmp;
	tmp.left = globalX();
	tmp.right = globalX() + width();
	tmp.top = globalY();
	tmp.bottom = globalY() + height();
	aObject::render();
	drawEmptyRect(tmp, s_area->getcolour(), s_area->getcolour());
	liveIcon->render(globalX() + s_liveIconRect->left(), globalY() + s_liveIconRect->top(),
		globalX() + s_liveIconRect->left() + s_liveIconRect->width(),
		globalY() + s_liveIconRect->top() + s_liveIconRect->height());
	tmp.left = globalX() + s_liveIconRect->left();
	tmp.top = globalY() + s_liveIconRect->top();
	tmp.right = globalX() + s_liveIconRect->left() + s_liveIconRect->width();
	tmp.bottom = globalY() + s_liveIconRect->top() + s_liveIconRect->height();
	drawEmptyRect(tmp, s_liveIconRect->getcolour(), s_liveIconRect->getcolour());
	deadIcon->render(globalX() + s_deadIconRect->left(), globalY() + s_deadIconRect->top(),
		globalX() + s_deadIconRect->left() + s_deadIconRect->width(),
		globalY() + s_deadIconRect->top() + s_deadIconRect->height());
	tmp.left = globalX() + s_deadIconRect->left();
	tmp.top = globalY() + s_deadIconRect->top();
	tmp.right = globalX() + s_deadIconRect->left() + s_deadIconRect->width();
	tmp.bottom = globalY() + s_deadIconRect->top() + s_deadIconRect->height();
	drawEmptyRect(tmp, s_deadIconRect->getcolour(), s_deadIconRect->getcolour());
}

int32_t
PilotListBox::AddItem(aListItem* add)
{
	bDone = 0;
	DeadPilotListItem* pItem = dynamic_cast<DeadPilotListItem*>(add);
	if (pItem)
	{
		if (itemCount >= MAX_LIST_ITEMS)
			return TOO_MANY_ITEMS;
		float lastX = x() + add->x();
		float lastY = y() + skipAmount;
		if (itemCount > 1)
		{
			lastX = items[itemCount - 2]->x();
			lastY = items[itemCount - 2]->bottom() + skipAmount;
		}
		else if (itemCount)
		{
			lastY = items[itemCount - 1]->bottom() + skipAmount;
		}
		if ((itemCount && !(itemCount % 2))) // even ones go somewhere else
		{
			lastX = items[itemCount - 1]->x() + add->width() + skipAmount;
			lastY = items[itemCount - 1]->y();
		}
		items[itemCount++] = add;
		add->moveTo(lastX, lastY);
		if (add->right() > right()) // can't have it hanging over the edge
		{
			float shrink = right() - add->right();
			add->resize(add->width() + shrink, add->height());
		}
		if (scrollBar && !(itemCount % 2))
		{
			int32_t itemsTotalheight = 0;
			if (items)
				itemsTotalheight = items[itemCount - 1]->bottom() - items[0]->top();
			if (itemsTotalheight > scrollBar->height())
				scrollBar->SetScrollMax(itemsTotalheight - scrollBar->height());
			else
				scrollBar->SetScrollMax(0);
		}
	}
	else
	{
		aListBox::AddItem(add);
	}
	return NO_ERROR;
}

void PilotListBox::update()
{
	aObject::update();
	timeSinceStart += frameLength;
	if (timeSinceStart < 1.0)
		return;
	if (this->curItem >= itemCount)
	{
		for (size_t i = 0; i < itemCount; i++)
		{
			GetItem(i)->showGUIWindow(true);
			GetItem(i)->update();
		}
		bDone = true;
		return;
	} // done
	for (size_t i = 0; i < itemCount; i++)
	{
		bDone = 0;
		aListItem* item = GetItem(i);
		if (item)
		{
			if (i < curItem)
			{
				item->showGUIWindow(true);
			}
			else if (i == curItem)
			{
				PilotListItem* pilotItem = dynamic_cast<PilotListItem*>(item);
				if (pilotItem)
				{
					if (pilotItem->isDone())
					{
						pilotItem = nullptr;
						while (!pilotItem)
						{
							curItem++;
							scrollTime = 0.f;
							oldScroll = scrollBar->GetScrollPos();
							if (curItem <= itemCount)
							{
								item = GetItem(curItem);
								pilotItem = dynamic_cast<PilotListItem*>(item);
							}
							else
							{
								curItem++;
								break;
							}
						}
						if (pilotItem)
						{
							pilotItem->begin();
						}
					}
					else
						pilotItem->update();
				}
				else
					curItem++;
				if (item)
				{
					float itemTop = item->globalY();
					if (itemTop > bottom() - 3 * height() / 4 && !newScroll)
					{
						// need to scroll
						oldScroll = scrollBar->GetScrollPos();
						newScroll = oldScroll + itemTop - bottom() + 3 * height() / 4;
						if (newScroll < oldScroll)
							newScroll = 0;
					}
					else if (!newScroll)
						items[curItem]->showGUIWindow(1);
				}
			}
			else
			{
				item->showGUIWindow(false);
			}
		}
	}
	if (newScroll)
	{
		scrollTime += frameLength;
		int32_t delta = 140.f * scrollTime;
		if (delta + oldScroll < newScroll && delta + oldScroll < scrollBar->GetScrollMax())
			scrollBar->SetScroll(oldScroll + delta);
		else
		{
			scrollBar->SetScroll(newScroll);
			if (items[curItem])
				items[curItem]->showGUIWindow(1);
			newScroll = 0;
			scrollTime = 0;
		}
	}
}

PilotListBox::PilotListBox()
{
	timeSinceStart = 0;
	curItem = 0;
	scrollTime = 0.f;
	oldScroll = 0.f;
	newScroll = 0.f;
	topSkip = 2.f;
	bDone = 0;
}
//////////////////////////////////

void ActivePilotListItem::render()
{
	if (!showWindow)
		return;
	float timeOffset = 0;
	if (pilot->killedIcons.Count())
		timeOffset += 1.5 + .5 * pilot->killedIcons.Count();
	if (currentTime - timeOffset > 0)
	{
		medalAwardedText.setcolour(s_medalAwardedAnim->getcolour(currentTime - timeOffset));
		if (currentTime - timeOffset > 1.5)
		{
			float tmpTime = currentTime - timeOffset - 1.5;
			for (size_t i = 0; i < medalCount; i++)
			{
				if (tmpTime > 0.f)
				{
					if (!medalIcons[i]->isShowing()) // play sound when it shows up
					{
						soundSystem->playDigitalSample(LOG_PILOTMEDAL);
					}
					medalIcons[i]->showGUIWindow(true);
					medalTexts[i]->showGUIWindow(true);
				}
				tmpTime -= .5f;
			}
		}
	}
	else
	{
		medalAwardedText.setcolour(0);
		for (size_t i = 0; i < medalCount; i++)
		{
			medalIcons[i]->showGUIWindow(0);
			medalTexts[i]->showGUIWindow(0);
		}
	}
	if (medalCount)
		timeOffset += (1.5 + .5 * medalCount);
	if (currentTime - timeOffset > 0)
	{
		if (promotionText.getcolour() == 0 && currentTime - timeOffset < .3 && pilot->promotePilot())
		{
			soundSystem->playDigitalSample(LOG_PROMOTED);
		}
		promotionText.setcolour(s_pilotPromotedAnim->getcolour(currentTime - timeOffset));
	}
	else
	{
		promotionText.setcolour(0);
	}
	bool bCanBeDone = currentTime > flashTime();
	aObject::render();
	RECT tmp;
	tmp.left = s_area->left();
	tmp.top = globalY() + s_area->top();
	tmp.right = tmp.left + s_area->width();
	tmp.bottom = tmp.top + s_area->height();
	drawEmptyRect(tmp, s_area->getcolour(), s_area->getcolour());
	// now do same thing with line....
	tmp.right = tmp.left + s_totalwidth;
	tmp.top -= 12;
	tmp.bottom = tmp.top + 2;
	drawRect(tmp, 0xff002f55);
	pilotIcon->render(globalX() + s_iconRect->left(), globalY() + s_iconRect->top(),
		globalX() + s_iconRect->right(), globalY() + s_iconRect->bottom());
	int32_t color = s_skillAnim->getcolour(currentTime);
	for (size_t i = 0; i < 2; i++)
	{
		attributeMeters[i].setAddedcolourMax(color);
		attributeMeters[i].setAddedcolourMin(color);
		attributeMeters[i].render(globalX(), globalY());
	}
	for (i = 0; i < 5; i++)
	{
		tmp = s_outline[i]->getGUI_RECT();
		tmp.left += globalX();
		tmp.top += globalY();
		tmp.right += globalX();
		tmp.bottom += globalY();
		drawEmptyRect(tmp, s_outline[i]->getcolour(), s_outline[i]->getcolour());
	}
	int32_t x = globalX() + s_killIconRect->left();
	int32_t y = globalY() + s_killIconRect->top();
	int32_t counter = 0;
	int32_t oldPossible = 2.0 * (currentTime - frameLength - 1.5);
	int32_t numPossible = 2.0 * (currentTime - 1.5);
	if (currentTime - 1.5 < 0)
		numPossible = -1;
	if (currentTime - 1.5 - frameLength < 0)
		oldPossible = -1;
	if (numPossible > 0 && promotionShown && bCanBeDone) // finish when all of the dead guys are rendered
		bDone = true;
	// when adding a new icon, play sound
	if (oldPossible != numPossible && numPossible < pilot->killedIcons.Count())
	{
		soundSystem->playDigitalSample(
			LOG_KILLMARKER, Stuff::Vector3D(-9999.0f, -9999.0, -9999.0f), true);
	}
	for (EList<ForceGroupIcon*, ForceGroupIcon*>::EIterator iter = pilot->killedIcons.Begin();
		 !iter.IsDone(); iter++)
	{
		if (counter > numPossible)
			break;
		if (counter && !(counter % 8))
		{
			y += s_killIconRect->height() + 1;
			x = globalX() + s_killIconRect->left();
		}
		(*iter)->renderUnitIcon(x, y, x + s_killIconRect->width(), y + s_killIconRect->height());
		x += s_killIconRect->width() + 1;
		counter++;
	}
}

void ActivePilotListItem::update()
{
	if (!isShowing()) // do nothing until active
		return;
	if (showingPromotion) // if in promotion area, do that...
	{
		s_pilotPromotionArea->update();
		PilotReviewScreen::s_curPromotion = s_pilotPromotionArea;
		if (s_pilotPromotionArea->isDone())
		{
			promotionShown = true;
			PilotReviewScreen::s_curPromotion = nullptr;
			showingPromotion = false;
		}
	}
	else if (!showingPromotion && !promotionShown && pilot->promotePilot())
	{
		if (currentTime > flashTime() + .5f) // wait five seconds before bringing in the doors
		{
			s_pilotPromotionArea->setPilot(pilot, pilotIcon);
			showingPromotion = true;
			promotionShown = false;
			showGUIWindow(true);
		}
	}
	else if (!pilot->promotePilot())
	{
		promotionShown = true;
		showingPromotion = false;
	}
	PilotListItem::update();
}

float ActivePilotListItem::flashTime()
{
	float flashTime = 1.5f * (float)pilot->promotePilot();
	flashTime += medalCount ? 2.0f : 0.f;
	flashTime += .5 * medalCount;
	flashTime += 1.5 + .5 * pilot->killedIcons.Count();
	return flashTime;
}
ActivePilotListItem::~ActivePilotListItem()
{
	delete pilotIcon;
	pilotIcon = nullptr;
	for (size_t i = 0; i < MAX_MEDAL; i++)
	{
		if (medalIcons[i])
		{
			delete medalIcons[i];
			medalIcons[i] = 0;
		}
		if (medalTexts[i])
		{
			delete medalTexts[i];
			medalTexts[i] = nullptr;
		}
	}
	s_itemCount--;
	if (s_itemCount < 1)
	{
		if (s_area)
			delete s_area;
		s_area = 0;
		if (s_missionText)
			delete s_missionText;
		s_missionText = 0;
		if (s_killsText)
			delete s_killsText;
		s_killsText = 0;
		if (s_rankText)
			delete s_rankText;
		s_rankText = 0;
		if (s_nameText)
			delete s_nameText;
		s_nameText = 0;
		if (s_iconRect)
			delete s_iconRect;
		s_iconRect = 0;
		if (s_gunneryText)
			delete s_gunneryText;
		s_gunneryText = 0;
		if (s_pilotingText)
			delete s_pilotingText;
		s_pilotingText = 0;
		if (s_killIconRect)
			delete s_killIconRect;
		s_killIconRect = 0;
		for (i = 0; i < 5; i++)
		{
			if (s_outline[i])
				delete s_outline[i];
			s_outline[i] = 0;
		}
		for (i = 0; i < 7; i++)
		{
			if (s_icons[i])
				delete s_icons[i];
			s_icons[i] = 0;
		}
		for (i = 0; i < MAX_MEDAL; i++)
		{
			if (s_medals[i])
				delete s_medals[i];
			s_medals[i] = 0;
		}
		if (s_promotionText)
			delete s_promotionText;
		s_promotionText = 0;
		if (s_medalText)
			delete s_medalText;
		s_medalText = 0;
		if (s_medalAwardedText)
			delete s_medalAwardedText;
		s_medalAwardedText = 0;
		if (s_pilotPromotedAnim)
			delete s_pilotPromotedAnim;
		s_pilotPromotedAnim = 0;
		if (s_medalAwardedAnim)
			delete s_medalAwardedAnim;
		s_medalAwardedAnim = 0;
		if (s_skillAnim)
			delete s_skillAnim;
		s_skillAnim = 0;
		if (s_pilotPromotionArea)
			delete s_pilotPromotionArea;
		s_pilotPromotionArea = nullptr;
	}
}

void ActivePilotListItem::init(FitIniFile* file)
{
	if (s_area)
		return;
	s_area = new aRect;
	s_missionText = new aText;
	s_killsText = new aText;
	s_rankText = new aText;
	s_nameText = new aText;
	s_iconRect = new aRect;
	s_area = new aRect;
	s_rankText = new aText;
	s_gunneryText = new aText;
	s_pilotingText = new aText;
	s_killIconRect = new aRect;
	s_pilotPromotionArea = new PilotPromotionArea;
	for (size_t i = 0; i < MAX_MEDAL; i++)
		s_medals[i] = new aObject;
	s_promotionText = new aText;
	s_medalText = new aText;
	s_medalAwardedText = new aText;
	s_pilotPromotedAnim = new aAnimation;
	s_skillAnim = new aAnimation;
	s_medalAwardedAnim = new aAnimation;
	s_nameText->init(file, "ActivePilotNameText");
	s_missionText->init(file, "ActiveMissionsText");
	s_rankText->init(file, "ActiveRankText");
	s_gunneryText->init(file, "ActiveGunneryText");
	s_pilotingText->init(file, "ActivePilotingText");
	s_killsText->init(file, "ActiveKillsNameText");
	s_pilotPromotionArea->init(*file);
	s_promotionText->init(file, "PromotionHeader");
	s_medalText->init(file, "MedalDescriptionText");
	s_medalAwardedText->init(file, "MedalAwardedHeader");
	wchar_t name[64];
	// last three are missing, remove this
	for (i = 0; i < MAX_MEDAL; i++)
	{
		sprintf(name, "MedalIcon%ld", i);
		s_medals[i]->init(file, name);
	}
	wchar_t buffer[256];
	for (i = 0; i < 5; i++)
	{
		s_outline[i] = new aRect;
		sprintf(buffer, "ActiveDutyBoxRect%ld", i);
		s_outline[i]->init(file, buffer);
	}
	for (i = 0; i < 2; i++)
	{
		s_icons[i] = new aObject;
		sprintf(buffer, "ActiveDutyBoxSkill%ld", i);
		s_icons[i]->init(file, buffer);
		sprintf(buffer, "ActiveDutyBoxSkillMeter%ld", i);
		s_attributeMeters[i] = new AttributeMeter();
		s_attributeMeters[i]->init(file, buffer);
		s_attributeMeters[i]->setAddedcolourMax(0xffffffff);
		s_attributeMeters[i]->setAddedcolourMin(0xffffffff);
	}
	for (i = 0; i < 5; i++)
	{
		s_icons[i + 3] = new aObject;
		sprintf(buffer, "ActiveDutyBoxRankIcon%ld", i);
		s_icons[i + 3]->init(file, buffer);
	}
	s_area->init(file, "ActiveDutyBox");
	s_iconRect->init(file, "ActiveDutyBoxPilotIconRect");
	s_killIconRect->init(file, "KillIcons");
	s_skillAnim->initWithBlockName(file, "SkillMeterAnimation");
	s_pilotPromotedAnim->initWithBlockName(file, "PilotPromotedHeaderAnimation");
	s_medalAwardedAnim->initWithBlockName(file, "MedalAwardedAnimation");
}

bool ActivePilotListItem::isDone()
{
	return bDone;
}

ActivePilotListItem::ActivePilotListItem(LogisticsPilot* pPilot)
{
	wchar_t tmpText[256];
	wchar_t realText[256];
	memset(medalIcons, 0, MAX_MEDAL * sizeof(aObject*));
	memset(medalTexts, 0, MAX_MEDAL * sizeof(aText*));
	promotionShown = 0;
	showingPromotion = 0;
	currentTime = 0;
	const bool* pMedals = pPilot->getNewMedalArray();
	medalCount = 0;
	int32_t offset = 0;
	int32_t killCount = pPilot->killedIcons.Count();
	if (killCount > 32)
		offset = (killCount - 32) / 8 + 1;
	offset *= s_killIconRect->height();
	for (size_t i = 0; i < MAX_MEDAL; i++)
	{
		if (pMedals[i])
		{
			medalIcons[medalCount] = new aObject;
			medalTexts[medalCount] = new aText;
			*medalIcons[medalCount] = *s_medals[i];
			*medalTexts[medalCount] = *s_medalText;
			medalIcons[medalCount]->move(0, medalCount * medalIcons[medalCount]->height() + offset);
			medalTexts[medalCount]->move(0, medalCount * medalIcons[medalCount]->height() + offset);
			medalTexts[medalCount]->setText(IDS_MEDAL + i);
			medalCount++;
		}
	}
	pilot = pPilot;
	showingPromotion = 0;
	int32_t top = s_area->top();
	killsText = *s_killsText;
	nameText = *s_nameText;
	rankText = *s_rankText;
	missionText = *s_missionText;
	pilotingText = *s_pilotingText;
	gunneryText = *s_gunneryText;
	promotionText = *s_promotionText;
	medalAwardedText = *s_medalAwardedText;
	if (medalCount)
		promotionText.moveTo(promotionText.left(), medalIcons[medalCount - 1]->bottom() + 15);
	if (medalCount > 1)
		medalAwardedText.setText(IDS_ONE_MEDAL_AWARD2);
	else
		medalAwardedText.setText(IDS_ONE_MEDAL_AWARD);
	// Gotta promote the pilot before we decide what rank he/she is!
	bool pilotPromoted = pilot->promotePilot();
	wchar_t text[256];
	cLoadString(IDS_PILOT_PROMOTED, text, 255);
	wchar_t finalText[256];
	wchar_t tmpRank[64];
	// ACE not contiguous with other ranks.  Added too late!
	if (pilot->getRank() != 4)
		cLoadString(IDS_GREEN + pilot->getRank(), tmpRank, 63);
	else
		cLoadString(IDS_ACE, tmpRank, 63);
	CharUpper(tmpRank);
	sprintf(finalText, text, tmpRank);
	promotionText.setText(finalText);
	int32_t bottom = promotionText.top() + promotionText.font.height() + 34;
	if (!pilotPromoted)
	{
		if (medalCount)
			bottom = medalIcons[medalCount - 1]->bottom() + 34;
		else
			bottom = promotionText.top() + 34;
	}
	aObject::init(30, top, s_area->width(), bottom);
	int32_t missions = pPilot->getNumberMissions();
	cLoadString(IDS_PILOT_MISSIONS, tmpText, 255);
	sprintf(realText, tmpText, missions);
	missionText.setText(realText);
	cLoadString(IDS_KILLS_NO_COUNT, tmpText, 255);
	killsText.setText(tmpText);
	std::wstring_view name = pPilot->getName();
	cLoadString(IDS_PILOT_NAME, tmpText, 255);
	sprintf(realText, tmpText, name);
	nameText.setText(realText);
	int32_t rank = pPilot->getRank();
	wchar_t rankStr[32];
	// ACE not Contiguous with other Ranks.  Added too late.
	if (rank != 4)
		cLoadString(rank + IDS_GREEN, rankStr, 31);
	else
		cLoadString(IDS_ACE, rankStr, 31);
	rankText.setText(rankStr);
	wchar_t buffer[32];
	sprintf(buffer, "%ld", (int32_t)pPilot->getGunnery());
	gunneryText.setText(buffer);
	sprintf(buffer, "%ld", (int32_t)pPilot->getPiloting());
	pilotingText.setText(buffer);
	for (i = 0; i < 2; i++)
	{
		icons[i] = *s_icons[i];
		addChild(&icons[i]);
		attributeMeters[i] = *s_attributeMeters[i];
	}
	attributeMeters[0].setValue(pPilot->getGunnery() / 100.f - pPilot->getNewGunnery() / 100.f);
	attributeMeters[1].setValue(pPilot->getPiloting() / 100.f - pPilot->getNewPiloting() / 100.f);
	attributeMeters[0].setAddedValue(pPilot->getNewGunnery() / 100.f);
	attributeMeters[1].setAddedValue(pPilot->getNewPiloting() / 100.f);
	icons[3] = *s_icons[3 + pPilot->getRank()];
	addChild(&icons[3]);
	addChild(&killsText);
	addChild(&nameText);
	addChild(&rankText);
	addChild(&missionText);
	addChild(&gunneryText);
	addChild(&pilotingText);
	if (medalCount)
	{
		addChild(&medalAwardedText);
		for (size_t i = 0; i < medalCount; i++)
		{
			addChild(medalIcons[i]);
			addChild(medalTexts[i]);
		}
	}
	if (pPilot->promotePilot())
	{
		addChild(&promotionText);
	}
	pilotIcon = new PilotIcon();
	pilotIcon->setTextureIndex(pPilot->getPhotoIndex());
	s_itemCount++;
	showWindow = 0;
}

void PilotListItem::begin()
{
	currentTime = 0.f;
}
void PilotListItem::update()
{
	currentTime += frameLength;
}

void PilotPromotionArea::init(FitIniFile& file)
{
	areaLeft.init(
		file, "PromoteGadgetLeftStatic", "PromoteGadgetLeftText", "PromoteGadgetLeftRect", nullptr);
	areaRight.init(file, "PromoteGadgetRightStatic", "PromoteGadgetRightText",
		"PromoteGadgetRightRect", "PromoteGadgetRightButton");
	file.seekBlock("PromoteGadgetLeftEnterAnimation");
	leftInfo.init(&file, "");
	file.seekBlock("PromoteGadgetRightEnterAnimation");
	rightInfo.init(&file, "");
	file.seekBlock("PromoteGadgetLeftExitAnimation");
	leftExitInfo.init(&file, "");
	file.seekBlock("PromoteGadgetRightExitAnimation");
	rightExitInfo.init(&file, "");
	SpecialtyListItem::init(&file);
	wchar_t buffer[32];
	for (size_t i = 0; i < 2; i++)
	{
		sprintf(buffer, "PromoteGadgetLeftSkillMeter%ld", i);
		attributeMeters[i].init(&file, buffer);
		attributeMeters[i].setAddedcolourMax(0xffffffff);
		attributeMeters[i].setAddedcolourMin(0xffffffff);
	}
	file.seekBlock("SkillSelectionAnimation");
	selSkillAnim.init(&file, "");
	pilot = nullptr;
}

void PilotPromotionArea::render()
{
	int32_t xOffset = leftInfo.getXDelta();
	int32_t yOffset = leftInfo.getYDelta();
	if (bDone)
	{
		xOffset = leftExitInfo.getXDelta();
		yOffset = leftExitInfo.getYDelta();
	}
	areaLeft.render(xOffset, yOffset);
	pilotIcon->render(areaLeft.rects[3].left() + xOffset + 1, areaLeft.rects[3].top() + yOffset + 1,
		areaLeft.rects[3].right() + xOffset - 1, areaLeft.rects[3].bottom() + yOffset - 1);
	for (size_t i = 0; i < 2; i++)
		attributeMeters[i].render(xOffset, yOffset);
	xOffset = rightInfo.getXDelta();
	yOffset = rightInfo.getYDelta();
	if (bDone)
	{
		xOffset = rightExitInfo.getXDelta();
		yOffset = rightExitInfo.getYDelta();
	}
	skillListBox.move(xOffset, yOffset);
	skillListBox.setcolour(0xff000000, 0);
	skillListBox.render();
	skillListBox.move(-xOffset, -yOffset);
	areaRight.render(xOffset, yOffset);
}

void PilotPromotionArea::update()
{
	leftInfo.update();
	rightInfo.update();
	rightExitInfo.update();
	leftExitInfo.update();
	areaLeft.update();
	areaRight.update();
	for (size_t i = 0; i < areaRight.buttonCount; i++)
	{
		if (areaRight.buttons[i].isPressed())
		{
			handleMessage(aMSG_DONE, aMSG_DONE);
			areaRight.buttons[i].press(0);
		}
	}
	if (!bDone)
		skillListBox.update();
	int32_t index = skillListBox.GetCheckedItem();
	if (index != -1 && (index != lastCheck || bDone))
	{
		aListItem* pItem = skillListBox.GetItem(index);
		SpecialtyListItem* pTmp = dynamic_cast<SpecialtyListItem*>(pItem);
		if (pTmp)
		{
			int32_t skillID = pTmp->getID();
			if (bDone)
			{
				pilot->setSpecialtySkill(skillID, 1);
				pTmp->setCheck(0);
			}
			int32_t specSkills = pilot->getSpecialtySkillCount();
			if (!bDone)
			{
				if (8 + specSkills < areaLeft.textCount)
					areaLeft.textObjects[8 + specSkills].setText(IDS_SPECIALTY + skillID);
				selSkillAnim.begin();
			}
		}
		lastCheck = index;
		areaRight.buttons[0].disable(false);
	}
	else if (index == -1)
	{
		areaRight.buttons[0].disable(true);
	}
	selSkillAnim.update();
	int32_t specSkills = pilot->getSpecialtySkillCount();
	if (8 + specSkills < areaLeft.textCount)
		areaLeft.textObjects[8 + specSkills].setcolour(selSkillAnim.getcolour());
}

void PilotPromotionArea::setPilot(LogisticsPilot* pPilot, PilotIcon* pIcon)
{
	PilotReviewScreen::instance->beginFadeOut(1.0f);
	lastCheck = -1;
	if (!areaLeft.staticCount) // haven't been initialized
		return;
	pilot = pPilot;
	bDone = 0;
	wchar_t tmpText[255];
	wchar_t realText[255];
	int32_t missions = pPilot->getNumberMissions();
	cLoadString(IDS_PILOT_MISSIONS, tmpText, 255);
	sprintf(realText, tmpText, missions);
	areaLeft.textObjects[2].setText(realText);
	std::wstring_view name = pPilot->getName();
	cLoadString(IDS_PILOT_NAME, tmpText, 255);
	sprintf(realText, tmpText, name);
	areaLeft.textObjects[1].setText(realText);
	int32_t rank = pPilot->getRank();
	for (size_t i = 12; i < 17; i++)
	{
		if (i - 12 == rank)
			areaLeft.statics[i].showGUIWindow(true);
		else
			areaLeft.statics[i].showGUIWindow(false);
	}
	wchar_t rankStr[32];
	// ACE not Contiguous with other Ranks.  Added too late.
	if (rank != 4)
		cLoadString(rank + IDS_GREEN, rankStr, 31);
	else
		cLoadString(IDS_ACE, rankStr, 31);
	areaLeft.textObjects[3].setText(rankStr);
	wchar_t buffer[32];
	sprintf(buffer, "%ld", (int32_t)pPilot->getGunnery());
	areaLeft.textObjects[4].setText(buffer);
	sprintf(buffer, "%ld", (int32_t)pPilot->getPiloting());
	areaLeft.textObjects[5].setText(buffer);
	int32_t specSkills = pPilot->getSpecialtySkillCount();
	if (specSkills)
	{
		std::wstring_view tmp[NUM_SPECIALTY_SKILLS];
		specSkills = 10;
		pPilot->getSpecialtySkills(tmp, specSkills);
		for (size_t i = 0; i < specSkills; i++)
		{
			if (8 + i < 12)
				areaLeft.textObjects[8 + i].setText(tmp[i]);
		}
	}
	for (size_t j = specSkills; j < 4; j++)
	{
		if (8 + j < 12)
			areaLeft.textObjects[8 + j].setText("");
	}
	int32_t skillArray[NUM_SPECIALTY_SKILLS];
	specSkills = NUM_SPECIALTY_SKILLS;
	pPilot->getSpecialtySkills(skillArray, specSkills);
	attributeMeters[0].setValue(pPilot->getGunnery() / 100.f - pPilot->getNewGunnery() / 100.f);
	attributeMeters[1].setValue(pPilot->getPiloting() / 100.f - pPilot->getNewPiloting() / 100.f);
	attributeMeters[0].setAddedValue(pPilot->getNewGunnery() / 100.f);
	attributeMeters[1].setAddedValue(pPilot->getNewPiloting() / 100.f);
	pilot = pPilot;
	leftInfo.begin();
	rightInfo.begin();
	bDone = 0;
	skillListBox.init(areaRight.rects[2].left(), areaRight.rects[2].top(),
		areaRight.rects[2].width(), areaRight.rects[2].height());
	skillListBox.setSpaceBetweenItems(5);
	skillListBox.setScrollPos(0);
	skillListBox.removeAllItems(true);
	int32_t maxSkill = 0;
	if (rank > WarriorRank::elite)
	{
		maxSkill = NUM_SPECIALTY_SKILLS;
		soundSystem->playBettySample(BETTY_PROMOACE);
	}
	else if (rank > WarriorRank::veteran)
	{
		maxSkill = FIRST_ACE_SPECIALTY;
		soundSystem->playBettySample(BETTY_PROMOELI);
	}
	else if (rank > WarriorRank::regular)
	{
		maxSkill = FIRST_ELITE_SPECIALTY;
		soundSystem->playBettySample(BETTY_PROMOVET);
	}
	else
	{
		maxSkill = FIRST_VETERAN_SPECIALTY;
		soundSystem->playBettySample(BETTY_PROMOREG);
	}
	for (i = 0; i < maxSkill; i++)
	{
		SpecialtyListItem* pItem = new SpecialtyListItem(i);
		aTextListItem* tmpItem = nullptr;
		if (i == FIRST_REGULAR_SPECIALTY)
		{
			tmpItem = new aTextListItem(IDS_PROMOTION_LISTBOX_FONT);
			tmpItem->setText(IDS_REGULAR_SKILLS);
			tmpItem->setcolour(0xffffffff);
		}
		else if (i == FIRST_VETERAN_SPECIALTY)
		{
			tmpItem = new aTextListItem(IDS_PROMOTION_LISTBOX_FONT);
			tmpItem->setText(IDS_VETERAN_SKILLS);
			tmpItem->setcolour(0xffffffff);
		}
		else if (i == FIRST_ELITE_SPECIALTY)
		{
			tmpItem = new aTextListItem(IDS_PROMOTION_LISTBOX_FONT);
			tmpItem->setText(IDS_ELITE_SKILLS3);
			tmpItem->setcolour(0xffffffff);
		}
		else if (i == FIRST_ACE_SPECIALTY)
		{
			tmpItem = new aTextListItem(IDS_PROMOTION_LISTBOX_FONT);
			tmpItem->setText(IDS_ACE_SKILLS);
			tmpItem->setcolour(0xffffffff);
		}
		if (tmpItem)
		{
			tmpItem->resize(tmpItem->width(), pItem->height() / 2);
			skillListBox.AddItem(tmpItem);
		}
		bool bAdd = 1;
		for (size_t j = 0; j < specSkills; j++)
		{
			if (skillArray[j] == i)
			{
				delete pItem;
				bAdd = 0;
				break;
			}
		}
		if (bAdd)
			skillListBox.AddItem(pItem);
	}
	pilotIcon = pIcon;
}

int32_t
PilotPromotionArea::handleMessage(uint32_t msg, uint32_t who)
{
	if (who == aMSG_DONE)
	{
		rightExitInfo.begin();
		leftExitInfo.begin();
		bDone = true;
		PilotReviewScreen::instance->beginFadeIn(1.0f);
	}
	return 0;
}

bool PilotPromotionArea::isDone()
{
	if (bDone && rightExitInfo.isDone() && leftExitInfo.isDone())
		return true;
	return false;
}

PilotPromotionArea::~PilotPromotionArea()
{
	SpecialtyListItem::deleteStatics();
}

void SpecialtyListItem::render()
{
	aObject::render();
}
void SpecialtyListItem::update()
{
	normalAnim.update();
	uint32_t color = normalAnim.getcolour();
	if (PilotReviewScreen::s_curPromotion)
	{
		aListBox* skillListBox = PilotReviewScreen::s_curPromotion->getSkillListBox();
		if (skillListBox && skillListBox->pointInside(userInput->getMouseX(), userInput->getMouseY()))
			aObject::update();
		if (userInput->isLeftClick() && pointInside(userInput->getMouseX(), userInput->getMouseY()) && (skillListBox && skillListBox->pointInside(userInput->getMouseX(), userInput->getMouseY())))
		{
			state = SELECTED;
			if (!radioButton.isPressed())
				radioButton.press(true);
			pressedAnim.begin();
		}
	}
	// set the texts and colors appropriately
	if (state == SELECTED)
	{
		if (!radioButton.isPressed())
			radioButton.press(true);
		if (!pressedAnim.isAnimating())
			pressedAnim.begin();
		pressedAnim.update();
		color = pressedAnim.getcolour();
	}
	else if (state == HIGHLITE)
	{
		if (!highlightAnim.isAnimating())
			highlightAnim.begin();
		highlightAnim.update();
		color = highlightAnim.getcolour();
	}
	setcolour(color, true);
	// make sure the background stays black
	setcolour(0xff000000, 0);
}

void SpecialtyListItem::deleteStatics()
{
	if (s_itemCount < 1)
	{
		if (s_radioButton)
			delete s_radioButton;
		s_radioButton = 0;
		if (s_highlightAnim)
			delete s_highlightAnim;
		s_highlightAnim = 0;
		if (s_normalAnim)
			delete s_normalAnim;
		s_normalAnim = 0;
		if (s_pressedAnim)
			delete s_pressedAnim;
		s_pressedAnim = 0;
		if (s_description)
			delete s_description;
		s_description = 0;
		if (s_area)
			delete s_area;
		s_area = 0;
		if (s_outline)
			delete s_outline;
		s_outline = 0;
		for (size_t i = 0; i < 4; i++)
		{
			if (s_skillIcons[i])
				delete s_skillIcons[i];
			s_skillIcons[i] = 0;
		}
	}
}

SpecialtyListItem::~SpecialtyListItem()
{
	s_itemCount--;
}

void SpecialtyListItem::init(FitIniFile* file)
{
	s_radioButton = new aButton;
	s_highlightAnim = new aAnimation;
	s_normalAnim = new aAnimation;
	s_pressedAnim = new aAnimation;
	s_description = new aText;
	s_area = new aRect;
	s_outline = new aRect;
	s_radioButton->init(*file, "PromoteGadgetRightRadioButton");
	wchar_t buffer[64];
	for (size_t i = 0; i < 4; i++)
	{
		s_skillIcons[i] = new aObject;
		sprintf(buffer, "SkillTypeIcon%ld", i);
		s_skillIcons[i]->init(file, buffer);
	}
	s_description->init(file, "SkillDescriptionText");
	file->seekBlock("PromoteGadgetRightSkillEntryBox");
	s_highlightAnim->init(file, "Highlight");
	s_normalAnim->init(file, "Normal");
	s_pressedAnim->init(file, "Pressed");
	s_outline->init(file, "PromoteGadgetRightSkillEntryOutline");
	s_area->init(file, "PromoteGadgetRightSkillEntryBox");
}

int32_t
SpecialtyListItem::handleMessage(uint32_t message, uint32_t who)
{
	return 0;
}

bool SpecialtyListItem::isChecked()
{
	return radioButton.isPressed();
}

int32_t
SpecialtyListItem::getID()
{
	return ID;
}

SpecialtyListItem::SpecialtyListItem(int32_t newID)
{
	s_itemCount++;
	ID = newID;
	aObject::init(s_area->left(), s_area->top(), s_area->right(), s_area->bottom());
	normalAnim = *s_normalAnim;
	pressedAnim = *s_pressedAnim;
	highlightAnim = *s_highlightAnim;
	description = *s_description;
	int32_t iconType = MechWarrior::skillTypes[newID];
	icon = *s_skillIcons[iconType];
	radioButton = *s_radioButton;
	outline = *s_outline;
	outline.bOutline = true;
	addChild(&description);
	addChild(&icon);
	addChild(&radioButton);
	addChild(&outline);
	description.setText(IDS_PROMOTION_SKILL + newID);
	normalAnim.begin();
}

void SpecialtyListItem::setCheck(bool press)
{
	radioButton.press(press);
	state = press ? SELECTED : ENABLED;
}
