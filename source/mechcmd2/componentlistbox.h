/*************************************************************************************************\
ComponentListBox.h			: Interface for the ComponentListBox component.
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//
\*************************************************************************************************/

#pragma once

#ifndef COMPONENTLISTBOX_H
#define COMPONENTLISTBOX_H

//#include "alistbox.h"
//#include "aanim.h"
//#include "elist.h"

class LogisticsVariant;
class LogisticsComponent;
class LogisticsVehicle;


#define COMP_ANIMATION_COUNT 5


//*************************************************************************************************

/**************************************************************************************************
CLASS DESCRIPTION
ComponentListBox:
**************************************************************************************************/
// complex list box items, icons and all
class ComponentListItem : public aListItem
{
public:

	ComponentListItem( LogisticsComponent* pComp );
	virtual ~ComponentListItem();

	static int init( FitIniFile& file );

	LogisticsComponent* getComponent() { return pComponent; }

	virtual void render();

	void	update();

private:

	friend class ComponentIconListBox;

	static ComponentListItem* s_templateItem;

	aText		name;
	aText		costText;
	aText		heatText;
	aObject		costIcon;
	aObject		heatIcon;
	aObject		icon;
	aRect		iconOutline;
	aRect		outline;
	aText		disabledText;

	aAnimGroup	animations[COMP_ANIMATION_COUNT];

	aAnimGroup*	pChildAnims[6];

	LogisticsComponent* pComponent;

	static void assignAnimation( FitIniFile& file,
		int whichChild, char animNames[COMP_ANIMATION_COUNT][32], aObject* pObject );

	void doAdd();
	void setComponent();
	void startDrag();



};

class ComponentIconListBox : public aListBox
{

public:
	ComponentIconListBox();
	virtual ~ComponentIconListBox();
	void setType( int Type, int orThisType, int orThis);
	virtual void update();

	LogisticsComponent* getComponent();
	int selectFirstAvailableComponent();

	static ComponentIconListBox* s_instance;

private:

	ComponentIconListBox( const ComponentIconListBox& src );
	ComponentIconListBox& operator=( const ComponentIconListBox& src );

	EList<ComponentListItem*, ComponentListItem*>	masterComponentList;

	int type;

	void addSortedItem( ComponentListItem* pItem );

};


//*************************************************************************************************
#endif  // end of file ( ComponentListBox.h )
