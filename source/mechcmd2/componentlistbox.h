/*************************************************************************************************\
ComponentListBox.h			: Interface for the ComponentListBox component.
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//
\*************************************************************************************************/

#pragma once

#ifndef COMPONENTLISTBOX_H
#define COMPONENTLISTBOX_H

//#include "mechgui/alistbox.h"
//#include "mechgui/aanim.h"
//#include "elist.h"

class LogisticsVariant;
class LogisticsComponent;
class LogisticsVehicle;

#define COMP_ANIMATION_COUNT 5


/**************************************************************************************************
CLASS DESCRIPTION
ComponentListBox:
**************************************************************************************************/
// complex list box items, icons and all
class ComponentListItem : public aListItem
{
public:
	ComponentListItem(LogisticsComponent* pComp);
	virtual ~ComponentListItem(void);

	static int32_t init(FitIniFile& file);

	LogisticsComponent* getComponent() { return pComponent; }

	virtual void render(void);

	void update(void);

private:
	friend class ComponentIconListBox;

	static ComponentListItem* s_templateItem;

	aText name;
	aText costText;
	aText heatText;
	aObject costIcon;
	aObject heatIcon;
	aObject icon;
	aRect iconOutline;
	aRect outline;
	aText disabledText;

	aAnimGroup animations[COMP_ANIMATION_COUNT];

	aAnimGroup* pChildAnims[6];

	LogisticsComponent* pComponent;

	static void assignAnimation(FitIniFile& file, int32_t whichChild,
		char animNames[COMP_ANIMATION_COUNT][32], aObject* pObject);

	void doAdd(void);
	void setComponent(void);
	void startDrag(void);
};

class ComponentIconListBox : public aListBox
{

public:
	ComponentIconListBox(void);
	virtual ~ComponentIconListBox(void);
	void setType(int32_t Type, int32_t orThisType, int32_t orThis);
	virtual void update(void);

	LogisticsComponent* getComponent(void);
	int32_t selectFirstAvailableComponent(void);

	static ComponentIconListBox* s_instance;

private:
	ComponentIconListBox(const ComponentIconListBox& src);
	ComponentIconListBox& operator=(const ComponentIconListBox& src);

	EList<ComponentListItem*, ComponentListItem*> masterComponentList;

	int32_t type;

	void addSortedItem(ComponentListItem* pItem);
};

#endif // end of file ( ComponentListBox.h )
