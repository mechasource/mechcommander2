
#pragma once

#ifndef MECHLISTBOX_H
#define MECHLISTBOX_H
/*************************************************************************************************\
MechListBox.h			: Interface for the MechListBox component.
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//
\*************************************************************************************************/

#ifndef ALISTBOX_H
#include <mechgui/alistbox.h>
#endif

#ifndef AANIM_H
#include <mechgui/aanim.h>
#endif

class FitIniFile;
class LogisticsMech;


//*************************************************************************************************

/**************************************************************************************************
CLASS DESCRIPTION
MechListBox:
**************************************************************************************************/

#define ANIMATION_COUNT 4
class MechListBoxItem : public aListItem
{
public:

	virtual ~MechListBoxItem(void);
	MechListBoxItem(LogisticsMech* pMech, int32_t count);

	static void init(FitIniFile& file);

	virtual void update(void);
	virtual void render(void);

	LogisticsMech* getMech()
	{
		return pMech;
	}
	void	resetMech()
	{
		pMech = nullptr;
	}
	virtual bool		pointInside(int32_t xPos, int32_t yPos) const;

	bool		bOrange;
	static bool	bAddCalledThisFrame;




private:

	static MechListBoxItem* s_templateItem;

	aAnimGroup		animations[2][ANIMATION_COUNT];

	aObject		mechIcon;
	aText		chassisName;
	aText		variantName;
	aText		countText;
	aText		weightText;
	aObject		weightIcon;
	aText		costText;
	aObject		costIcon;
	aRect		outline;
	aRect		line;

	int32_t		animationIDs[9];

	bool		drawCBills;
	bool		bIncludeForceGroup;

	LogisticsMech* pMech;

	int32_t mechCount;

	MechListBoxItem& operator=(const MechListBoxItem& src);

	friend class MechListBox;

	void setMech(void);
	void doAdd(void);
	void startDrag(void);
	static void assignAnimation(FitIniFile& file, int32_t& curAnimation);


	bool bDim;

	float animTime;


};


class MechListBox: public aListBox
{
public:

	MechListBox(bool deleteIfNoInventory, bool bIncludeForceGroup);


	void	setScrollBarOrange(void);
	void	setScrollBarGreen(void);

	virtual ~MechListBox(void);

	static int32_t		init(void);
	void	drawCBills(bool bDraw);

	virtual void update(void);

	LogisticsMech* getCurrentMech(void);

	static void initIcon(LogisticsMech* pMec, aObject& icon);

	virtual int32_t		AddItem(aListItem* itemString);

	void	dimItem(LogisticsMech* pMech, bool bDim);
	void	undimAll(void);
	void	disableItemsThatCostMoreThanRP(void);
	void	disableItemsThatCanNotGoInFG(void);

	void	setOrange(bool bOrange);




private:

	static bool	s_DrawCBills;

	MechListBox(const MechListBox& src);
	MechListBox& operator=(const MechListBox& src);

	friend class MechListBoxItem;

	bool		bDeleteIfNoInventory;
	bool		bIncludeForceGroup;

	bool		bOrange;




};


//*************************************************************************************************
#endif  // end of file ( MechListBox.h )
