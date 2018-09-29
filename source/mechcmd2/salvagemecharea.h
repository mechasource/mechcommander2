/*************************************************************************************************\
SalvageMechArea.h			: Interface for the SalvageMechArea component.
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//
\*************************************************************************************************/

//*************************************************************************************************

#pragma once

#ifndef SALVAGEMECHAREA_H
#define SALVAGEMECHAREA_H

//#include <mechgui/asystem.h>
//#include "simplecomponentlistbox.h"
//#include "attributemeter.h"
//#include <mechgui/logisticsscreen.h>
//#include "simplecamera.h"

class aButton;
class MechIcon;
class BattleMech;
class LogisticsVariant;

/**************************************************************************************************
CLASS DESCRIPTION
SalvageMechArea:
**************************************************************************************************/
class SalvageMechArea : public LogisticsScreen
{
public:
	static SalvageMechArea* instance;
	SalvageMechArea(void);
	virtual ~SalvageMechArea(void);

	void init(FitIniFile* file);
	void setMech(LogisticsVariant* pMech, int32_t red, int32_t green, int32_t blue);

	virtual void render(int32_t xOffset, int32_t yOffset);
	virtual void update(void);

protected:
	LogisticsVariant* unit;

	AttributeMeter attributeMeters[3];
	ComponentListBox loadoutListBox;
	SimpleCamera mechCamera;
};

class SalvageMechScreen : public LogisticsScreen
{
public:
	SalvageMechScreen(void);
	virtual ~SalvageMechScreen(void);

	void init(FitIniFile* file);
	bool isDone(void);
	bool donePressed() { return bDone; }
	virtual void render(void);
	virtual void update(void);
	virtual int32_t handleMessage(uint32_t, uint32_t);

	void updateSalvage(void); // put into inventory

	bool bDone;

private:
	aListBox salvageListBox;

	SalvageMechArea selMechArea;
	aAnimation exitAnim;
	aAnimation entryAnim;

	float countDownTime;
	float curCount;
	float previousAmount;
	float oldCBillsAmount;
};

class SalvageListItem : public aListItem
{
public:
	virtual void render(void);
	virtual void update(void);

	virtual ~SalvageListItem(void);

	static void init(FitIniFile* file);
	virtual int32_t handleMessage(uint32_t message, uint32_t who);

	bool isChecked(void);
	LogisticsVariant* getMech() { return pVariant; }

	SalvageListItem(BattleMech* pUnit);

private:
	static aAnimButton* templateCheckButton;
	static aText* mechNameText;
	static aText* variantNameText;
	static aText* weightText;
	static aText* costText;
	static aObject* cBillsIcon;
	static aObject* weightIcon;
	static RECT iconRect;
	static RECT rect;
	static aAnimation* s_pressedAnim;
	static aAnimation* s_highlightAnim;
	static aAnimation* s_normalAnim;

	aAnimation pressedAnim;
	aAnimation highlightAnim;
	aAnimation normalAnim;

	int32_t salvageAmount;
	float costToSalvage;
	MechIcon* icon;
	LogisticsVariant* pVariant;
	aAnimButton* checkButton;

	uint32_t psRed;
	uint32_t psGreen;
	uint32_t psBlue;

	friend class SalvageMechScreen;
};

//*************************************************************************************************
#endif // end of file ( SalvageMechArea.h )
