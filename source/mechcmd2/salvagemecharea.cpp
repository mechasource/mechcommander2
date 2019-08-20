#define SALVAGEMECHAREA_CPP
/*************************************************************************************************\
SalvageMechArea.cpp			: Implementation of the SalvageMechArea component.
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//
\*************************************************************************************************/
#include "stdinc.h"

#include "SalvageMechArea.h"
#include "IniFile.h"
#include "objMgr.h"
#include "Mech.h"
#include "LogisticsVariant.h"
#include "MechIcon.h"
#include "LogisticsData.h"
#include "..\resource.h"
#include "gamesound.h"
#include <malloc.h>

aAnimButton* SalvageListItem::templateCheckButton = nullptr;

SalvageMechArea* SalvageMechArea::instance = nullptr;

aText* SalvageListItem::variantNameText = nullptr;
aText* SalvageListItem::mechNameText = nullptr;
aText* SalvageListItem::weightText = nullptr;
aText* SalvageListItem::costText = nullptr;
aObject* SalvageListItem::cBillsIcon = nullptr;
aObject* SalvageListItem::weightIcon = nullptr;
RECT SalvageListItem::iconRect;
RECT SalvageListItem::rect;

aAnimation* SalvageListItem::s_pressedAnim = nullptr;
aAnimation* SalvageListItem::s_highlightAnim = nullptr;
aAnimation* SalvageListItem::s_normalAnim = nullptr;

#define RP_TEXTID 3
#define NAME_TEXTID 0
#define WEIGHT_TEXTID 2
#define ARMOR_TEXTID 3
#define SPEED_TEXTID 4
#define JUMP_TEXTID 5
#define RANGE_TEXTID 6

#define MECH_RECT 6

#define CHECK_BUTTON 200

SalvageMechScreen::SalvageMechScreen()
{
	bDone = 0;
	// this MUST be initialized
	LogisticsData::instance->init();
	helpTextArrayID = 2;
	countDownTime = .5;
	curCount = 0.0;
	previousAmount = 0;
	oldCBillsAmount = 0;
	salvageListBox.setSpaceBetweenItems(5);
	salvageListBox.setTopSkip(2);
}

SalvageMechScreen::~SalvageMechScreen()
{
	delete SalvageListItem::s_pressedAnim;
	SalvageListItem::s_pressedAnim = nullptr;
	delete SalvageListItem::s_highlightAnim;
	SalvageListItem::s_highlightAnim = nullptr;
	delete SalvageListItem::s_normalAnim;
	SalvageListItem::s_normalAnim = nullptr;
	delete SalvageListItem::templateCheckButton;
	SalvageListItem::templateCheckButton = nullptr;
	delete SalvageListItem::weightText;
	SalvageListItem::weightText = nullptr;
	delete SalvageListItem::costText;
	SalvageListItem::costText = nullptr;
	delete SalvageListItem::variantNameText;
	SalvageListItem::variantNameText = nullptr;
	delete SalvageListItem::mechNameText;
	SalvageListItem::mechNameText = nullptr;
	delete SalvageListItem::weightIcon;
	SalvageListItem::weightIcon = nullptr;
	delete SalvageListItem::cBillsIcon;
	SalvageListItem::cBillsIcon = nullptr;
}

int32_t __cdecl sortMechs(PCVOID pW1, PCVOID pW2)
{
	BattleMech* p1 = *(BattleMech**)pW1;
	BattleMech* p2 = *(BattleMech**)pW2;
	LogisticsVariant* pV1 = LogisticsData::instance->getVariant(p1->variantName);
	LogisticsVariant* pV2 = LogisticsData::instance->getVariant(p2->variantName);
	if (pV1 && pV2)
	{
		int32_t cost1 = pV1->getCost();
		int32_t cost2 = pV2->getCost();
		if (cost1 < cost2)
			return -1;
		else if (cost2 < cost1)
			return 1;
	}
	return 0;
}

void
SalvageMechScreen::init(FitIniFile* file)
{
	LogisticsScreen::init(
		*file, "SalvageAreaStatic", "SalvageAreaText", "SalvageAreaRect", "SalvageAreaButton");
	SalvageListItem::init(file);
	for (size_t i = 0; i < buttonCount; i++)
	{
		buttons[i].setMessageOnRelease();
	}
	int32_t left, right, top, bottom;
	file->seekBlock("SalvageAreaRect3");
	file->readIdLong("Left", left);
	file->readIdLong("Right", right);
	file->readIdLong("Top", top);
	file->readIdLong("Bottom", bottom);
	salvageListBox.init(left, top, right - left, bottom - top);
	BattleMech** pSortedMechs =
		(BattleMech**)_alloca(ObjectManager->numMechs * sizeof(BattleMech*));
	int32_t count = 0;
	for (i = 0; i < ObjectManager->numMechs; i++)
	{
		BattleMech* pMech = ObjectManager->getMech(i);
		if (pMech->isDisabled() && !pMech->isDestroyed() && pMech->moveLevel != 2)
			pSortedMechs[count++] = pMech;
	}
	qsort(pSortedMechs, count, sizeof(BattleMech*), sortMechs);
	for (i = 0; i < count; i++)
	{
		BattleMech* pMech = pSortedMechs[i];
		if (pMech->isDisabled() && !pMech->isDestroyed() && pMech->moveLevel != 2) // don't put copters in the list
		{
			SalvageListItem* pItem = new SalvageListItem(pMech);
			salvageListBox.AddItem(pItem);
			pItem->setParent(this);
		}
	}
	selMechArea.init(file);
	salvageListBox.SelectItem(0);
	exitAnim.initWithBlockName(file, "SalvageAreaDoneAnimation");
	entryAnim.initWithBlockName(file, "SalvageAreaFadeInAnimation");
	entryAnim.begin();
	beginFadeIn(1.0);
}

void
SalvageMechScreen::render()
{
	int32_t xOffset = 0;
	int32_t yOffset = 0;
	if (bDone)
	{
		xOffset = exitAnim.getXDelta();
		yOffset = exitAnim.getYDelta();
	}
	salvageListBox.move(xOffset, yOffset);
	salvageListBox.render();
	salvageListBox.move(-xOffset, -yOffset);
	selMechArea.render(xOffset, yOffset);
	LogisticsScreen::render(xOffset, yOffset);
	// this animation draw a big white square and looks like crap
	//	if ( !entryAnim.isDone() )
	//	{
	//		uint32_t color = entryAnim.getColor();
	//		RECT rect = { 0, 0, Environment.screenWidth, Environment.screenHeight
	//}; 		drawRect( rect, color );
	//	}
}

int32_t
SalvageMechScreen::handleMessage(uint32_t message, uint32_t who)
{
	if (who == 101)
	{
		bDone = true;
		{
			soundSystem->stopBettySample(); // don't want to carry droning on to
				// next screen
			if (LogisticsData::instance->skipPilotReview())
			{
				beginFadeOut(.5);
			}
			else
			{
				exitAnim.begin();
				exitAnim.update();
				fadeTime = 0.0f;
				fadeInTime = 0.f;
			}
			return 1;
		}
	}
	return 0;
}

bool
SalvageMechScreen::isDone()
{
	return bDone && (exitAnim.isDone() || (fadeTime > fadeOutTime));
}
void
SalvageMechScreen::update()
{
	int32_t amount = LogisticsData::instance->getCBills();
	int32_t color = 0xff005392;
	if (amount != oldCBillsAmount)
	{
		previousAmount = oldCBillsAmount - amount;
		curCount = .00001f;
		oldCBillsAmount = amount;
		if (previousAmount < 0)
			soundSystem->playDigitalSample(WINDOW_OPEN);
		else
			soundSystem->playDigitalSample(WINDOW_CLOSE);
	}
	if (curCount && curCount + frameLength < countDownTime)
	{
		curCount += frameLength;
		float curAmount = previousAmount - (curCount / countDownTime * previousAmount);
		amount += curAmount;
		color = 0xffc8e100;
		if (curAmount > 0)
			color = 0xffa21600;
	}
	char cBillText[32];
	sprintf(cBillText, "%ld", amount);
	textObjects[RP_TEXTID].setText(cBillText);
	textObjects[RP_TEXTID].setColor(color);
	salvageListBox.update();
	for (size_t i = 0; i < buttonCount; i++)
	{
		buttons[i].update();
		/*		if ( buttons[i].pointInside( userInput->getMouseX(),
		   userInput->getMouseY() )
					&& userInput->isLeftClick() )
				{
					handleMessage( aMSG_DONE, aMSG_DONE );
				}*/
	}
	entryAnim.update();
	exitAnim.update();
	selMechArea.update();
	LogisticsScreen::update();
	if (exitAnim.isAnimating())
		fadeTime = 0.f;
}

void
SalvageMechScreen::updateSalvage()
{
	for (size_t i = 0; i < salvageListBox.GetItemCount(); i++)
	{
		SalvageListItem* item = (SalvageListItem*)salvageListBox.GetItem(i);
		if (item && item->isChecked())
		{
			LogisticsData::instance->addMechToInventory(
				item->getMech(), (LogisticsPilot*)nullptr, 0);
		}
	}
}

//////////////////////////////////////////////

SalvageListItem::~SalvageListItem()
{
	removeAllChildren(true);
}

void
SalvageListItem::init(FitIniFile* file)
{
	if (templateCheckButton)
		return; // already initialized
	templateCheckButton = new aAnimButton;
	mechNameText = new aText;
	variantNameText = new aText;
	weightText = new aText;
	costText = new aText;
	cBillsIcon = new aObject;
	weightIcon = new aObject;
	templateCheckButton->init(*file, "SalvageAreaCheckBoxButton");
	cBillsIcon->init(file, "CBillsIcon");
	weightIcon->init(file, "WeightIcon");
	mechNameText->init(file, "MechNameText");
	variantNameText->init(file, "VariantNameText");
	costText->init(file, "CBillsText");
	weightText->init(file, "WeightText");
	file->seekBlock("SalvageAreaMechEntryBox");
	file->readIdLong("XLocation", rect.left);
	file->readIdLong("YLocation", rect.top);
	int32_t width, height;
	file->readIdLong("Width", width);
	file->readIdLong("Height", height);
	rect.right = rect.left + width;
	rect.bottom = rect.top + height;
	s_normalAnim = new aAnimation;
	s_pressedAnim = new aAnimation;
	s_highlightAnim = new aAnimation;
	s_normalAnim->init(file, "Normal");
	s_pressedAnim->init(file, "Pressed");
	s_highlightAnim->init(file, "Highlight");
	file->seekBlock("MechEntryIcon");
	file->readIdLong("XLocation", iconRect.left);
	file->readIdLong("YLocation", iconRect.right);
	file->readIdLong("Width", width);
	file->readIdLong("Height", height);
	iconRect.right = iconRect.left + width;
	iconRect.bottom = iconRect.top + height;
}

SalvageListItem::SalvageListItem(BattleMech* pMech)
{
	gosASSERT(pMech);
	int32_t width = rect.right - rect.left;
	aObject::init(0, 2, width, rect.bottom - rect.top);
	icon = new MechIcon();
	icon->init(pMech);
	icon->update();
	((Mech3DAppearance*)pMech->getAppearance())->getPaintScheme(psRed, psGreen, psBlue);
	normalAnim = *s_normalAnim;
	pressedAnim = *s_pressedAnim;
	highlightAnim = *s_highlightAnim;
	pVariant = LogisticsData::instance->getVariant(pMech->variantName);
	if (!pVariant)
		return;
	costToSalvage = .8f * (float)pVariant->getCost();
	// add the chassis
	aText* pText = new aText();
	*pText = *mechNameText;
	int32_t nameID = pVariant->getChassisName();
	char tmp[64];
	cLoadString(nameID, tmp, 63);
	pText->setText(tmp);
	addChild(pText);
	// add the variant name
	pText = new aText();
	*pText = *variantNameText;
	pText->setText(pVariant->getName());
	addChild(pText);
	// add salvage
	char text[64];
	int32_t salvageAmount = costToSalvage;
	sprintf(text, "%ld", salvageAmount);
	pText = new aText();
	*pText = *costText;
	pText->setText(text);
	addChild(pText);
	// add tonnage
	int32_t tonnage = pMech->tonnage;
	sprintf(text, "%ld", tonnage);
	pText = new aText();
	*pText = *weightText;
	pText->setText(text);
	addChild(pText);
	// need to make the check box button
	aAnimButton* pButton = new aAnimButton;
	*pButton = *templateCheckButton;
	pButton->setID(CHECK_BUTTON);
	addChild(pButton);
	checkButton = pButton;
	// add the icons
	aObject* pObject = new aObject;
	*pObject = *cBillsIcon;
	addChild(pObject);
	pObject = new aObject;
	*pObject = *weightIcon;
	addChild(pObject);
	normalAnim.begin();
}

void
SalvageListItem::update()
{
	int32_t mouseX = userInput->getMouseX();
	int32_t mouseY = userInput->getMouseY();
	if (costToSalvage > LogisticsData::instance->getCBills() && !isChecked())
	{
		disable();
		checkButton->disable(1);
		if (pointInside(mouseX, mouseY) && showWindow == true)
		{
			if (userInput->isLeftClick())
			{
				SalvageMechArea::instance->setMech(pVariant, psBlue, psGreen, psRed);
				soundSystem->playDigitalSample(LOG_WRONGBUTTON);
			}
		}
	}
	else if (!isChecked() && getState() == DISABLED)
	{
		setState(ENABLED);
		checkButton->disable(0);
		if (pointInside(mouseX, mouseY) && showWindow == true)
		{
			if (userInput->isLeftClick())
			{
				SalvageMechArea::instance->setMech(pVariant, psBlue, psGreen, psRed);
			}
		}
	}
	else
	{
		if (pointInside(mouseX, mouseY) && showWindow == true)
		{
			if (userInput->isLeftClick())
			{
				SalvageMechArea::instance->setMech(pVariant, psBlue, psGreen, psRed);
				pressedAnim.begin();
			}
			// here's where you'd do the highlight thing
			else
			{
				if (state != aListItem::HIGHLITE)
				{
					highlightAnim.begin();
				}
				else
				{
					if (!highlightAnim.isAnimating())
						highlightAnim.begin();
					highlightAnim.update();
				}
			}
		}
		if (state == aListItem::SELECTED)
		{
			if (!pressedAnim.isAnimating())
			{
				pressedAnim.begin();
				SalvageMechArea::instance->setMech(pVariant, psBlue, psGreen, psRed);
			}
			pressedAnim.update();
		}
	}
	normalAnim.update();
	aObject::update();
}

void
SalvageListItem::render()
{
	aObject::render();
	icon->renderUnitIcon(iconRect.left + location[0].x + 3, iconRect.top + location[0].y + 5,
		iconRect.right + location[0].x, iconRect.bottom + location[0].y);
	int32_t color = state == HIGHLITE ? highlightAnim.getColor() : normalAnim.getColor();
	if (state == SELECTED)
		color = pressedAnim.getColor();
	if (state == DISABLED)
		color = 0xff373737;
	for (size_t i = 0; i < this->pNumberOfChildren; i++)
	{
		pChildren[i]->setColor(color, 1);
	}
	RECT tmp;
	tmp.left = location[0].x + templateCheckButton->width() + 3;
	tmp.right = tmp.left + rect.right - rect.left;
	tmp.top = location[0].y;
	tmp.bottom = location[2].y;
	drawEmptyRect(tmp, color, color);
}

bool
SalvageListItem::isChecked()
{
	return checkButton->isPressed();
}

int32_t
SalvageListItem::handleMessage(uint32_t message, uint32_t who)
{
	if (message == aMSG_LEFTMOUSEDOWN)
	{
		if (who == CHECK_BUTTON)
		{
			if (checkButton->isPressed())
				LogisticsData::instance->decrementCBills(costToSalvage);
			else
				LogisticsData::instance->addCBills(costToSalvage);
		}
		return 1;
	}
	return 0;
}

//////////////////////////////////

SalvageMechArea::SalvageMechArea()
{
	statics = 0;
	rects = 0;
	staticCount = rectCount = textCount = 0;
	textObjects = 0;
	gosASSERT(!instance);
	instance = this;
	unit = 0;
}

SalvageMechArea::~SalvageMechArea()
{
	instance = nullptr;
}

void
SalvageMechArea::init(FitIniFile* file)
{
	file->seekBlock("SalvageAreaRect8");
	int32_t left, right, top, bottom;
	file->readIdLong("left", left);
	file->readIdLong("top", top);
	file->readIdLong("right", right);
	file->readIdLong("top", top);
	file->readIdLong("bottom", bottom);
	loadoutListBox.init(left, top, right - left, bottom - top);
	file->seekBlock("SalvageAreaRect6");
	file->readIdLong("left", left);
	file->readIdLong("top", top);
	file->readIdLong("right", right);
	file->readIdLong("top", top);
	file->readIdLong("bottom", bottom);
	mechCamera.init(left, top, right, bottom);
	LogisticsScreen::init(*file, 0, "SalvageAreaMechText", 0, 0);
	char blockName[64];
	for (size_t i = 0; i < 3; i++)
	{
		sprintf(blockName, "AttributeMeter%ld", i);
		attributeMeters[i].init(file, blockName);
	}
}

void
SalvageMechArea::update()
{
	loadoutListBox.update();
	mechCamera.update();
}
void
SalvageMechArea::setMech(LogisticsVariant* pMech, int32_t red, int32_t green, int32_t blue)
{
	if (pMech == unit)
		return;
	loadoutListBox.setMech(pMech);
	if (pMech)
	{
		std::wstring fileName = pMech->getFileName();
		int32_t index = fileName.Find('.');
		fileName = fileName.Left(index);
		index = fileName.ReverseFind('\\');
		fileName = fileName.Right(fileName.Length() - index - 1);
		mechCamera.setMech(fileName, red, green, blue);
		textObjects[NAME_TEXTID].setText(pMech->getName());
		char text[256];
		sprintf(text, "%ld", pMech->getMaxWeight());
		textObjects[WEIGHT_TEXTID].setText(text);
		sprintf(text, "%ld", pMech->getArmor());
		textObjects[ARMOR_TEXTID].setText(text);
		sprintf(text, "%ld", pMech->getDisplaySpeed());
		textObjects[SPEED_TEXTID].setText(text);
		sprintf(text, "%ld", pMech->getJumpRange() * 25);
		textObjects[JUMP_TEXTID].setText(text);
		int32_t tmpColor;
		textObjects[RANGE_TEXTID].setText(pMech->getOptimalRangeString(tmpColor));
		textObjects[RANGE_TEXTID].setColor((tmpColor));
		attributeMeters[0].setValue(((float)pMech->getArmor()) / MAX_ARMOR_RANGE);
		attributeMeters[1].setValue(((float)pMech->getSpeed()) / MAX_SPEED_RANGE);
		attributeMeters[2].setValue(((float)pMech->getJumpRange() * 25.0f) / MAX_JUMP_RANGE);
	}
	else
	{
		textObjects[WEIGHT_TEXTID].setText("");
		textObjects[ARMOR_TEXTID].setText("");
		textObjects[SPEED_TEXTID].setText("");
		textObjects[JUMP_TEXTID].setText("");
		attributeMeters[0].setValue(0);
		attributeMeters[1].setValue(0);
		attributeMeters[2].setValue(0);
		mechCamera.setMech(0);
	}
	unit = pMech;
}

void
SalvageMechArea::render(int32_t xOffset, int32_t yOffset)
{
	loadoutListBox.move(xOffset, yOffset);
	loadoutListBox.render();
	loadoutListBox.move(-xOffset, -yOffset);
	if (this->unit)
	{
		for (size_t i = 0; i < textCount; i++)
		{
			textObjects[i].move(xOffset, yOffset);
			textObjects[i].render();
			textObjects[i].move(-xOffset, -yOffset);
		}
		for (i = 0; i < 3; i++)
		{
			attributeMeters[i].render(xOffset, yOffset);
		}
		if (!xOffset && !yOffset)
			mechCamera.render();
	}
}

//*************************************************************************************************
// end of file ( SalvageMechArea.cpp )
