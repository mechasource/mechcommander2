#define MECHLISTBOX_CPP
/*************************************************************************************************\
MechListBox.cpp			: Implementation of the MechListBox component.
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//
\*************************************************************************************************/
#include "stdinc.h"

#include "MechListBox.h"
#include "LogisticsMech.h"
#include "paths.h"
#include "iniFile.h"
#include "err.h"
#include "userInput.h"
#include "MechBayScreen.h"
#include "LogisticsData.h"
#include "MechPurchaseScreen.h"
#include "gameSound.h"

MechListBoxItem* MechListBoxItem::s_templateItem = nullptr;

bool MechListBox::s_DrawCBills = true;
bool MechListBoxItem::bAddCalledThisFrame = 0;

MechListBox::MechListBox(bool bDel, bool bInclude)
{
	bDeleteIfNoInventory = bDel;
	bIncludeForceGroup = bInclude;
	bOrange = 0;
	skipAmount = 5;
}

//-------------------------------------------------------------------------------------------------

MechListBox::~MechListBox()
{
	removeAllItems(true);
	delete MechListBoxItem::s_templateItem;
	MechListBoxItem::s_templateItem = nullptr;
}

void MechListBox::setScrollBarOrange()
{
	scrollBar->setOrange();
}
void MechListBox::setScrollBarGreen()
{
	scrollBar->setGreen();
}

void MechListBox::drawCBills(bool bDraw)
{
	s_DrawCBills = bDraw;
}

void MechListBox::update()
{
	aListBox::update();
	MechListBoxItem::bAddCalledThisFrame = false;
	if (bDeleteIfNoInventory)
	{
		for (size_t i = 0; i < itemCount; i++)
		{
			if (((MechListBoxItem*)items[i])->mechCount == 0)
			{
				RemoveItem(items[i], true);
				i--;
				disableItemsThatCanNotGoInFG();
				// find better thing to select if necessary
				if (itemSelected >= itemCount || itemSelected == -1 || items[itemSelected]->getState() == aListItem::DISABLED
					//	|| !LogisticsData::instance->canAddMechToForceGroup(
					//((MechListBoxItem*)items[itemSelected])->getMech()  )
				)
				{
					if (itemCount)
					{
						for (size_t j = 0; j < itemCount; j++)
							if (items[j]->getState() != aListItem::DISABLED)
							{
								SelectItem(j);
								break;
							}
					}
					else
						itemSelected = -1;
				}
			}
		}
	}
}

LogisticsMech*
MechListBox::getCurrentMech()
{
	if (itemSelected != -1)
	{
		return ((MechListBoxItem*)items[itemSelected])->pMech;
	}
	return 0;
}

int32_t
MechListBox::init()
{
	if (MechListBoxItem::s_templateItem)
		return 0;
	wchar_t path[256];
	strcpy(path, artPath);
	strcat(path, "mcl_gn_availablemechentry.fit");
	FitIniFile file;
	if (NO_ERROR != file.open(path))
	{
		wchar_t errorStr[256];
		sprintf(errorStr, "couldn't open file %s", path);
		Assert(0, 0, errorStr);
		return -1;
	}
	MechListBoxItem::init(file);
	return 0;
}

//-------------------------------------------------------------------------------------------------
bool MechListBoxItem::pointInside(int32_t xPos, int32_t yPos) const
{
	int32_t minX = location[0].x + outline.globalX();
	int32_t minY = location[0].y + outline.globalY();
	int32_t maxX = location[0].x + outline.globalX() + outline.width();
	int32_t maxY = location[0].y + outline.globalY() + outline.height();
	if (minX < xPos && xPos < maxX && minY < yPos && yPos < maxY)
		return true;
	return 0;
}
MechListBoxItem::MechListBoxItem(LogisticsMech* pRefMech, int32_t count)
{
	bIncludeForceGroup = 0;
	bOrange = 0;
	if (s_templateItem)
	{
		*this = *s_templateItem;
	}
	animTime = 0.f;
	pMech = pRefMech;
	if (!pMech)
		return;
	aObject::init(0, outline.top(), outline.width(), outline.height());
	setcolour(0, 0);
	chassisName.setText(pMech->getChassisName());
	wchar_t text[32];
	sprintf(text, "%ld", pMech->getCost());
	costText.setText(text);
	mechCount =
		LogisticsData::instance->getVariantsInInventory(pRefMech->getVariant(), bIncludeForceGroup);
	sprintf(text, "%ld", mechCount);
	countText.setText(text);
	MechListBox::initIcon(pRefMech, mechIcon);
	variantName.setText(pMech->getName());
	sprintf(text, "%.0lf", pMech->getMaxWeight());
	weightText.setText(text);
	addChild(&weightIcon);
	addChild(&mechIcon);
	addChild(&costIcon);
	addChild(&chassisName);
	addChild(&weightText);
	addChild(&countText);
	addChild(&variantName);
	addChild(&costText);
	//	addChild( &line );
	//	addChild( &outline );
	bDim = 0;
}

MechListBoxItem::~MechListBoxItem()
{
	removeAllChildren(false);
}

void MechListBoxItem::init(FitIniFile& file)
{
	if (!s_templateItem)
	{
		s_templateItem = new MechListBoxItem(nullptr, 0);
		file.seekBlock("MainBox");
		int32_t width, height;
		file.readIdLong("width", width);
		file.readIdLong("height", height);
		((aObject*)s_templateItem)->init(0, 0, width, height);
		memset(s_templateItem->animationIDs, 0, sizeof(int32_t) * 9);
		// rects
		s_templateItem->line.init(&file, "Rect1");
		s_templateItem->outline.init(&file, "Rect0");
		int32_t curAnim = 0;
		// statics
		s_templateItem->weightIcon.init(&file, "Static0");
		assignAnimation(file, curAnim);
		s_templateItem->mechIcon.init(&file, "Static1");
		assignAnimation(file, curAnim);
		s_templateItem->costIcon.init(&file, "Static2");
		assignAnimation(file, curAnim);
		// texts
		s_templateItem->chassisName.init(&file, "Text0");
		assignAnimation(file, curAnim);
		s_templateItem->weightText.init(&file, "Text1");
		assignAnimation(file, curAnim);
		s_templateItem->countText.init(&file, "Text2");
		assignAnimation(file, curAnim);
		s_templateItem->variantName.init(&file, "Text3");
		assignAnimation(file, curAnim);
		s_templateItem->costText.init(&file, "Text4");
		assignAnimation(file, curAnim);
		wchar_t blockname[64];
		for (size_t i = 0; i < 4; i++)
		{
			sprintf(blockname, "OrangeAnimation%ld", i);
			s_templateItem->animations[1][i].init(&file, blockname);
			sprintf(blockname, "Animation%ld", i);
			s_templateItem->animations[0][i].init(&file, blockname);
		}
	}
}

void MechListBoxItem::assignAnimation(FitIniFile& file, int32_t& curAnim)
{
	wchar_t tmpStr[64];
	s_templateItem->animationIDs[curAnim] = -1;
	if (NO_ERROR == file.readIdString("Animation", tmpStr, 63))
	{
		for (size_t j = 0; j < strlen(tmpStr); j++)
		{
			if (isdigit(tmpStr[j]))
			{
				tmpStr[j + 1] = 0;
				s_templateItem->animationIDs[curAnim] = atoi(&tmpStr[j]);
			}
		}
	}
	curAnim++;
}
MechListBoxItem&
MechListBoxItem::operator=(const MechListBoxItem& src)
{
	if (&src != this)
	{
		chassisName = src.chassisName;
		costIcon = src.costIcon;
		costText = src.costText;
		line = src.line;
		mechIcon = src.mechIcon;
		outline = src.outline;
		variantName = src.variantName;
		weightIcon = src.weightIcon;
		weightText = src.weightText;
		countText = src.countText;
		for (size_t i = 0; i < ANIMATION_COUNT; i++)
		{
			animations[0][i] = src.animations[0][i];
			animations[1][i] = src.animations[1][i];
		}
		for (i = 0; i < 9; i++)
		{
			animationIDs[i] = src.animationIDs[i];
		}
	}
	return *this;
}

void MechListBoxItem::update()
{
	wchar_t text[32];
	int32_t oldMechCount = mechCount;
	if (!pMech)
	{
		mechCount = 0;
		return;
	}
	mechCount =
		LogisticsData::instance->getVariantsInInventory(pMech->getVariant(), bIncludeForceGroup);
	if (oldMechCount != mechCount)
	{
		animTime = .0001f;
	}
	sprintf(text, "%ld", mechCount);
	countText.setText(text);
	if (animTime)
	{
		if (animTime < .25f || (animTime > .5f && animTime <= .75f))
		{
			countText.setcolour(0);
		}
		else
			countText.setcolour(0xffffffff);
		animTime += frameLength;
		if (animTime > 1.0f)
			animTime = 0.f;
	}
	bool isInside = pointInside(userInput->getMouseX(), userInput->getMouseY());
	for (size_t i = 0; i < ANIMATION_COUNT; i++)
		animations[bOrange][i].update();
	if (state == aListItem::SELECTED)
	{
		for (size_t i = 0; i < ANIMATION_COUNT; i++)
			animations[bOrange][i].setState(aAnimGroup::PRESSED);
		//	if ( userInput->isLeftClick() && isInside )
		//		setMech();
		if (userInput->isLeftDrag() && pointInside(userInput->getMouseDragX(), userInput->getMouseDragY()))
			startDrag();
	}
	else if (state == aListItem::HIGHLITE)
	{
		for (size_t i = 0; i < ANIMATION_COUNT; i++)
			animations[bOrange][i].setState(aAnimGroup::HIGHLIGHT);
	}
	else if (state == aListItem::DISABLED && isShowing())
	{
		if (userInput->isLeftClick() && isInside)
		{
			soundSystem->playDigitalSample(LOG_WRONGBUTTON);
			setMech(); // need to call explicitly
		}
		for (size_t i = 0; i < ANIMATION_COUNT; i++)
			animations[bOrange][i].setState(aAnimGroup::DISABLED);
	}
	else
	{
		for (size_t i = 0; i < ANIMATION_COUNT; i++)
			animations[bOrange][i].setState(aAnimGroup::NORMAL);
	}
	if (userInput->isLeftDoubleClick() && isInside && state != aListItem::DISABLED && isShowing())
		doAdd();
	aObject::update();
}

void MechListBoxItem::render()
{
	if (!MechListBox::s_DrawCBills)
	{
		costText.showGUIWindow(0);
		costIcon.showGUIWindow(0);
	}
	else
	{
		costText.showGUIWindow(1);
		costIcon.showGUIWindow(1);
	}
	for (size_t i = 0; i < this->pNumberOfChildren; i++)
	{
		int32_t index = animationIDs[i];
		if (index != -1)
		{
			if (pChildren[i]->isShowing())
			{
				if (!animTime || pChildren[i] != &countText)
				{
					int32_t color = animations[bOrange][index].getCurrentcolour(
						animations[bOrange][index].getState());
					pChildren[i]->setcolour(color);
				}
			}
		}
		pChildren[i]->render();
	}
	if (bDim)
	{
		mechIcon.setcolour(0xa0000000);
		mechIcon.render();
	}
	outline.setcolour(animations[bOrange][2].getCurrentcolour(animations[bOrange][2].getState()));
	outline.render(location[0].x, location[0].y);
	line.setcolour(animations[bOrange][2].getCurrentcolour(animations[bOrange][2].getState()));
	line.render(location[0].x, location[0].y);
}

void MechListBoxItem::setMech()
{
	MechBayScreen::instance()->setMech(pMech);
	MechPurchaseScreen::instance()->setMech(pMech, true);
}

void MechListBoxItem::startDrag()
{
	if (state != DISABLED)
	{
		MechBayScreen::instance()->beginDrag(pMech);
		MechPurchaseScreen::instance()->beginDrag(pMech);
	}
}

void MechListBoxItem::doAdd()
{
	if (!bAddCalledThisFrame) // only select one, sometimes we auto scroll,
		// don't want to be selecting each time
	{
		MechBayScreen::instance()->handleMessage(ID, MB_MSG_ADD);
		MechPurchaseScreen::instance()->handleMessage(ID, MB_MSG_ADD);
		bAddCalledThisFrame = true;
	}
}

void MechListBox::initIcon(LogisticsMech* pMech, aObject& mechIcon)
{
	mechIcon = (MechListBoxItem::s_templateItem->mechIcon);
	int32_t index = pMech->getIconIndex();
	int32_t xIndex = index % 10;
	int32_t yIndex = index / 10;
	float fX = xIndex;
	float fY = yIndex;
	float width = mechIcon.width();
	float height = mechIcon.height();
	float u = (fX * width);
	float v = (fY * height);
	fX += 1.f;
	fY += 1.f;
	float u2 = (fX * width);
	float v2 = (fY * height);
	mechIcon.setFilewidth(256.f);
	mechIcon.setUVs(u, v, u2, v2);
}

int32_t
MechListBox::AddItem(aListItem* itemString)
{
	itemString->setID(ID);
	MechListBoxItem* pItem = dynamic_cast<MechListBoxItem*>(itemString);
	std::wstring_view addedName;
	wchar_t tmp[256];
	cLoadString(pItem->getMech()->getChassisName(), tmp, 255);
	addedName = tmp;
	if (pItem)
	{
		pItem->bOrange = bOrange;
		pItem->bIncludeForceGroup = bIncludeForceGroup;
		if (!bDeleteIfNoInventory)
		{
			pItem->countText.setcolour(0);
			pItem->countText.showGUIWindow(0);
		}
		std::wstring_view chassisName;
		for (size_t i = 0; i < itemCount; i++)
		{
			int32_t ID = ((MechListBoxItem*)items[i])->pMech->getChassisName();
			wchar_t tmpChassisName[256];
			cLoadString(ID, tmpChassisName, 255);
			chassisName = tmpChassisName;
			if (((MechListBoxItem*)items[i])->pMech->getMaxWeight() < pItem->pMech->getMaxWeight())
			{
				return InsertItem(itemString, i);
				break;
			}
			else if (((MechListBoxItem*)items[i])->pMech->getMaxWeight() == pItem->pMech->getMaxWeight() && chassisName.Compare(addedName) > 0)
			{
				return InsertItem(itemString, i);
			}
			else if (((MechListBoxItem*)items[i])->pMech->getMaxWeight() == pItem->pMech->getMaxWeight() && chassisName.Compare(addedName) == 0 && ((MechListBoxItem*)itemString)->pMech->getName().Find("Prime") != -1)
			{
				return InsertItem(itemString, i);
			}
			else if (((MechListBoxItem*)items[i])->pMech->getMaxWeight() == pItem->pMech->getMaxWeight() && chassisName.Compare(addedName) == 0 && (((MechListBoxItem*)items[i])->pMech->getName().Find("Prime") == -1) && ((MechListBoxItem*)items[i])->pMech->getName().Compare(pItem->pMech->getName()) > 0)
			{
				return InsertItem(itemString, i);
			}
		}
	}
	return aListBox::AddItem(itemString);
}

void MechListBox::dimItem(LogisticsMech* pMech, bool bDim)
{
	for (size_t i = 0; i < itemCount; i++)
	{
		if (((MechListBoxItem*)items[i])->pMech == pMech)
		{
			((MechListBoxItem*)items[i])->bDim = bDim;
		}
	}
}

void MechListBox::undimAll()
{
	for (size_t i = 0; i < itemCount; i++)
	{
		((MechListBoxItem*)items[i])->bDim = 0;
	}
}

void MechListBox::disableItemsThatCostMoreThanRP()
{
	bool bDisabledSel = 0;
	for (size_t i = 0; i < itemCount; i++)
	{
		if (((MechListBoxItem*)items[i])->pMech->getCost() > LogisticsData::instance->getCBills())
		{
			items[i]->setState(aListItem::DISABLED);
			if (itemSelected == i)
				bDisabledSel = true;
		}
		else
		{
			if (items[i]->getState() == aListItem::DISABLED)
				items[i]->setState(aListItem::ENABLED);
		}
	}
	if (bDisabledSel)
	{
		for (i = 0; i < itemCount; i++)
		{
			if (items[i]->getState() != aListItem::DISABLED)
			{
				SelectItem(i);
				bDisabledSel = 0;
				break;
			}
		}
		if (bDisabledSel)
			SelectItem(-1);
	}
}

void MechListBox::disableItemsThatCanNotGoInFG()
{
	bool bDisabledSel = 0;
	for (size_t i = 0; i < itemCount; i++)
	{
		if (!LogisticsData::instance->canAddMechToForceGroup(((MechListBoxItem*)items[i])->pMech))
		{
			if (itemSelected == i)
				bDisabledSel = true;
			items[i]->setState(aListItem::DISABLED);
		}
		else
		{
			if (items[i]->getState() == aListItem::DISABLED)
				items[i]->setState(aListItem::ENABLED);
		}
	}
	if (bDisabledSel)
	{
		for (i = 0; i < itemCount; i++)
		{
			if (items[i]->getState() != aListItem::DISABLED)
			{
				SelectItem(i);
				bDisabledSel = 0;
				break;
			}
		}
		if (bDisabledSel)
			SelectItem(-1);
	}
}

void MechListBox::setOrange(bool bNewOrange)
{
	bOrange = bNewOrange ? 1 : 0;
	for (size_t i = 0; i < itemCount; i++)
	{
		((MechListBoxItem*)items[i])->bOrange = bOrange;
	}
	if (bNewOrange)
		scrollBar->setOrange();
	else
		scrollBar->setGreen();
}

// end of file ( MechListBox.cpp )
