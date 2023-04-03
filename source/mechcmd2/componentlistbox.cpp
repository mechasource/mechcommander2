/*************************************************************************************************\
ComponentListBox.cpp			: Implementation of the ComponentListBox
component.
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//
\*************************************************************************************************/

#define COMPONENTLISTBOX_CPP

#include "stdinc.h"
#include "componentlistbox.h"
#include "logisticsvariant.h"
#include "logisticscomponent.h"
#include "resource.h"
#include "malloc.h"
#include "mclib.h"
#include "logisticsdata.h"
#include "mechlabscreen.h"
#include "gamesound.h"
#include "logisticserrors.h"

ComponentListItem* ComponentListItem::s_templateItem = nullptr;

ComponentIconListBox* ComponentIconListBox::s_instance = nullptr;

///////////////////////////////////////////////////////

ComponentListItem::ComponentListItem(LogisticsComponent* pComp)
{
	if (s_templateItem)
		*this = *s_templateItem;
	pComponent = pComp;
	if (pComp)
	{
		name.setText(pComponent->getName());
		wchar_t numericText[32];
		sprintf(numericText, "%ld", pComponent->getCost());
		costText.setText(numericText);
		sprintf(numericText, "%.0lf", pComponent->getHeat());
		heatText.setText(numericText);
		int32_t sizeX = pComponent->getComponentwidth();
		int32_t sizeY = pComponent->getComponentheight();
		std::wstring_view pFile = pComponent->getIconFileName();
		FullPathFileName path;
		path.init(artPath, pFile, "tga");
		icon.setTexture(path);
		icon.resize(
			sizeX * LogisticsComponent::XICON_FACTOR, sizeY * LogisticsComponent::YICON_FACTOR);
		icon.setUVs(0.f, 0.f, sizeX * 48.f, sizeY * 32.f);
		// figure out the difference between this thing's size and
		// the template objects
		int32_t deltaY = icon.height() - s_templateItem->icon.height();
		int32_t deltaX = icon.width() - s_templateItem->icon.width();
		// resize outlines and icon outlines
		outline.resize(outline.width(), outline.height() + deltaY);
		iconOutline.resize(iconOutline.width() + deltaX, iconOutline.height() + deltaY);
		disabledText.resize(outline.width(), outline.height());
		disabledText.moveTo(0, 0);
		disabledText.alignment = 3;
		aObject::init(outline.left(), outline.top(), outline.width(), outline.height());
		addChild(&icon);
		addChild(&name);
		addChild(&costText);
		addChild(&heatText);
		addChild(&costIcon);
		addChild(&heatIcon);
		addChild(&disabledText);
		if (s_templateItem)
		{
			for (size_t i = 0; i < 6; i++)
			{
				for (size_t j = 0; j < COMP_ANIMATION_COUNT; j++)
				{
					if (s_templateItem->pChildAnims[i] == &s_templateItem->animations[j])
					{
						pChildAnims[i] = &animations[j];
					}
				}
			}
		}
	}
}

ComponentListItem::~ComponentListItem() { }

int32_t
ComponentListItem::init(FitIniFile& file)
{
	if (!s_templateItem)
	{
		s_templateItem = new ComponentListItem(nullptr);
		wchar_t animName[COMP_ANIMATION_COUNT][32];
		for (size_t i = 0; i < COMP_ANIMATION_COUNT; i++)
		{
			sprintf(animName[i], "Animation%ld", i);
			s_templateItem->animations[i].init(&file, animName[i]);
		}
		s_templateItem->icon.init(&file, "ComponentIcon");
		assignAnimation(file, 0, animName, &s_templateItem->icon);
		s_templateItem->iconOutline.init(&file, "ComponentIconOutlineRect");
		s_templateItem->outline.init(&file, "ComponentEntryBox");
		s_templateItem->name.init(&file, "ComponentNameText");
		assignAnimation(file, 1, animName, &s_templateItem->name);
		s_templateItem->costText.init(&file, "ComponentCostText");
		assignAnimation(file, 2, animName, &s_templateItem->costText);
		s_templateItem->heatText.init(&file, "ComponentHeatText");
		assignAnimation(file, 3, animName, &s_templateItem->heatText);
		s_templateItem->costIcon.init(&file, "ComponentCostIcon");
		assignAnimation(file, 4, animName, &s_templateItem->costIcon);
		s_templateItem->heatIcon.init(&file, "ComponentHeatIcon");
		assignAnimation(file, 5, animName, &s_templateItem->heatIcon);
		s_templateItem->disabledText.init(&file, "ComponentDisabledText");
	}
	return 0;
}

void ComponentListItem::assignAnimation(
	FitIniFile& file, int32_t whichChild, wchar_t animNames[COMP_ANIMATION_COUNT][32], aObject* object)
{
	s_templateItem->pChildAnims[whichChild] = 0;
	wchar_t tmpAnimName[32];
	file.readIdString("Animation", tmpAnimName, 31);
	for (size_t i = 0; i < COMP_ANIMATION_COUNT; i++)
	{
		if (_stricmp(animNames[i], tmpAnimName) == 0)
		{
			s_templateItem->pChildAnims[whichChild] = &s_templateItem->animations[i];
			break;
		}
	}
	s_templateItem->addChild(object);
}

void ComponentListItem::render()
{
	aListItem::render();
	// draw rects, these aren't children
	outline.moveTo(globalX(), globalY());
	outline.setcolour(animations[0].getCurrentcolour(animations[0].getState()));
	outline.render();
	iconOutline.moveTo(globalX() + s_templateItem->iconOutline.left(),
		globalY() + s_templateItem->iconOutline.top());
	iconOutline.setcolour(animations[0].getCurrentcolour(animations[0].getState()));
	iconOutline.render();
	for (size_t i = 0; i < 6; i++)
	{
		if (pChildAnims[i])
		{
			int32_t color = pChildAnims[i]->getCurrentcolour(pChildAnims[i]->getState());
			child(i)->setcolour(color);
		}
	}
	if (MechLabScreen::instance()->canAddComponent(pComponent))
	{
		icon.setcolour(0xff404040);
	}
	else
		icon.setcolour(0xffffffff);
}

void ComponentListItem::update()
{
	int32_t bCanAdd = MechLabScreen::instance()->canAddComponent(pComponent);
	for (size_t i = 0; i < COMP_ANIMATION_COUNT; i++)
	{
		animations[i].update();
	}
	bool isInside = pointInside(userInput->getMouseX(), userInput->getMouseY());
	if (state == aListItem::SELECTED)
	{
		if ((userInput->isLeftClick() && isInside) || (animations[0].getState() != aAnimGroup::PRESSED) && ComponentIconListBox::s_instance->pointInside(userInput->getMouseX(), userInput->getMouseY()))
		{
			setComponent();
			if (bCanAdd)
			{
				soundSystem->playDigitalSample(LOG_WRONGBUTTON);
			}
			::helpTextID = IDS_HELP_COMP0 + pComponent->getID();
		}
		if (!bCanAdd)
		{
			if (animations[0].getState() != aAnimGroup::PRESSED)
			{
				for (size_t i = 0; i < COMP_ANIMATION_COUNT; i++)
				{
					animations[i].setState(aAnimGroup::PRESSED);
				}
			}
			if (isInside && ComponentIconListBox::s_instance->pointInside(userInput->getMouseX(), userInput->getMouseY()))
			{
				::helpTextID = IDS_HELP_COMP0 + pComponent->getID();
			}
			disabledText.setText("");
		}
		else
		{
			for (size_t i = 0; i < COMP_ANIMATION_COUNT; i++)
				animations[i].setState(aAnimGroup::DISABLED);
			if (COMPONENT_TOO_HOT == bCanAdd)
			{
				disabledText.setText(IDS_MC_COMPONENT_TOO_HOT);
			}
			else if (NO_MORE_ARMOR == bCanAdd)
			{
				disabledText.setText(IDS_MC_COMPONENT_TOO_MUCH_ARMOR);
			}
			else
				disabledText.setText("");
		}
		if (!bCanAdd)
		{
			if (userInput->isLeftDrag() && isInside && pointInside(userInput->getMouseDragX(), userInput->getMouseDragY()) && ComponentIconListBox::s_instance->pointInside(userInput->getMouseX(), userInput->getMouseY()))
				startDrag();
		}
	}
	else if (isInside && !bCanAdd && ComponentIconListBox::s_instance->pointInside(userInput->getMouseX(), userInput->getMouseY()))
	{
		if (animations[0].getState() != aAnimGroup::HIGHLIGHT)
		{
			for (size_t i = 0; i < COMP_ANIMATION_COUNT; i++)
			{
				animations[i].setState(aAnimGroup::HIGHLIGHT);
			}
			soundSystem->playDigitalSample(LOG_HIGHLIGHTBUTTONS);
		}
		state = aListItem::HIGHLITE;
		::helpTextID = IDS_HELP_COMP0 + pComponent->getID();
	}
	else if (!bCanAdd)
	{
		state = aListItem::ENABLED;
		for (size_t i = 0; i < COMP_ANIMATION_COUNT; i++)
			animations[i].setState(aAnimGroup::NORMAL);
		disabledText.setText("");
	}
	else
	{
		state = DISABLED;
		if (isInside && ComponentIconListBox::s_instance->pointInside(userInput->getMouseX(), userInput->getMouseY()))
			::helpTextID = IDS_HELP_COMP0 + pComponent->getID();
		if ((userInput->isLeftClick() && isInside && ComponentIconListBox::s_instance->pointInside(userInput->getMouseX(), userInput->getMouseY())))
		{
			setComponent();
			soundSystem->playDigitalSample(LOG_WRONGBUTTON);
		}
		for (size_t i = 0; i < COMP_ANIMATION_COUNT; i++)
			animations[i].setState(aAnimGroup::DISABLED);
		if (COMPONENT_TOO_HOT == bCanAdd)
		{
			disabledText.setText(IDS_MC_COMPONENT_TOO_HOT);
		}
		else if (NO_MORE_ARMOR == bCanAdd)
		{
			disabledText.setText(IDS_MC_COMPONENT_TOO_MUCH_ARMOR);
		}
		else
			disabledText.setText("");
	}
	if (userInput->isLeftDoubleClick() && isInside && ComponentIconListBox::s_instance->pointInside(userInput->getMouseX(), userInput->getMouseY()))
		doAdd();
	aObject::update();
}

void ComponentListItem::doAdd()
{
	int32_t x = -1;
	int32_t y = -1;
	MechLabScreen::instance()->addComponent(pComponent, x, y);
}

void ComponentListItem::setComponent()
{
	MechLabScreen::instance()->setComponent(pComponent, 1);
}

void ComponentListItem::startDrag()
{
	MechLabScreen::instance()->beginDrag(pComponent);
}

void ComponentIconListBox::setType(int32_t newType, int32_t otherNewType, int32_t orThis)
{
	if (newType == type && itemCount)
		return;
	type = newType;
	scrollBar->setGreen();
	removeAllItems(0);
	itemSelected = -1;
	if (!masterComponentList.Count())
	{
		int32_t count = 256;
		LogisticsComponent* pComp[256];
		LogisticsData::instance->getAllComponents(pComp, count);
		// for ( int32_t j = 0; j < 2048; j++ )
		//{
		for (size_t i = 0; i < count; i++)
		{
			ComponentListItem* pItem = new ComponentListItem(pComp[i]);
			masterComponentList.Append(pItem);
		}
		//	for ( EList< ComponentListItem*, ComponentListItem* >::EIterator
		// iter = masterComponentList.Begin(); 	!iter.IsDone(); iter++ )
		//	{
		///		delete *iter;
		//		turn++;
		//	}
		//	masterComponentList.Clear();
		//	}
	}
	for (EList<ComponentListItem*, ComponentListItem*>::EIterator iter =
			 masterComponentList.Begin();
		 !iter.IsDone(); iter++)
	{
		if ((*iter)->getComponent()->isAvailable())
		{
			if ((*iter)->getComponent()->getType() == type || (*iter)->getComponent()->getType() == otherNewType || (*iter)->getComponent()->getType() == orThis)
			{
				if ((*iter)->globalX() > globalX())
				{
					(*iter)->move(-globalX(), 0);
				}
				addSortedItem((*iter));
			}
		}
	}
	selectFirstAvailableComponent();
}

void ComponentIconListBox::update()
{
	aListBox::update();
	if (itemSelected != -1)
	{
		if (items[itemSelected]->getState() == aListItem::DISABLED || MechLabScreen::instance()->canAddComponent(((ComponentListItem*)items[itemSelected])->getComponent()))
		{
			selectFirstAvailableComponent();
		}
	}
}

int32_t
ComponentIconListBox::selectFirstAvailableComponent()
{
	bool bFound = 0;
	for (size_t i = 0; i < itemCount; i++)
	{
		if (items[i]->getState() != aListItem::DISABLED && !MechLabScreen::instance()->canAddComponent(((ComponentListItem*)items[i])->getComponent()))
		{
			SelectItem(i);
			((ComponentListItem*)items[i])->setComponent();
			bFound = true;
			return i;
		}
	}
	if (!bFound)
		SelectItem(-1);
	return -1;
}

ComponentIconListBox::ComponentIconListBox()
{
	skipAmount = 5;
	type = -1;
	s_instance = this;
}

ComponentIconListBox::~ComponentIconListBox()
{
	masterComponentList.Clear();
	s_instance = nullptr;
	delete ComponentListItem::s_templateItem;
	ComponentListItem::s_templateItem = nullptr;
}

LogisticsComponent*
ComponentIconListBox::getComponent()
{
	if (itemSelected != -1)
	{
		return ((ComponentListItem*)items[itemSelected])->pComponent;
	}
	return nullptr;
}

void ComponentIconListBox::addSortedItem(ComponentListItem* pItem)
{
	int32_t size =
		pItem->getComponent()->getComponentheight() * pItem->getComponent()->getComponentwidth();
	for (size_t i = 0; i < itemCount; i++)
	{
		LogisticsComponent* pTmp = ((ComponentListItem*)items[i])->getComponent();
		int32_t tmpSize = pTmp->getComponentheight() * pTmp->getComponentwidth();
		if (size > tmpSize)
		{
			InsertItem(pItem, i);
			return;
		}
		else if (size == tmpSize && _stricmp(pItem->getComponent()->getName(), pTmp->getName()) < 0)
		{
			InsertItem(pItem, i);
			return;
		}
	}
	aListBox::AddItem(pItem);
}

// end of file ( ComponentListBox.cpp )
