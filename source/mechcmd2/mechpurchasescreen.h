
#pragma once

#ifndef MECHPURCHASESCREEN_H
#define MECHPURCHASESCREEN_H
/*************************************************************************************************\
MechPurchaseScreen.h			: Interface for the MechPurchaseScreen
component.
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//
\*************************************************************************************************/

#ifndef LOGISTICSSCREEN_H
#include "mechgui/logisticsscreen.h"
#endif

#include "MechListBox.h"
#include "LogisticsMech.h"
#include "elist.h"
#include "LogisticsMechDisplay.h"

class LogisticsMech;

/**************************************************************************************************
CLASS DESCRIPTION
MechPurchaseScreen:
**************************************************************************************************/
class MechPurchaseScreen : public LogisticsScreen
{
public:
	static MechPurchaseScreen* instance()
	{
		return s_instance;
	}

	MechPurchaseScreen(void);
	virtual ~MechPurchaseScreen(void);

	int32_t init(FitIniFile& file);

	virtual void begin(void);
	virtual void end(void);
	virtual void update(void);
	virtual void render(int32_t xOffset, int32_t yOffset);

	void setMech(LogisticsMech* pMech, bool bFromLB = 0);
	void beginDrag(LogisticsMech* pMech);
	virtual int32_t handleMessage(uint32_t, uint32_t);

private:
	MechPurchaseScreen(const MechPurchaseScreen& src);
	MechPurchaseScreen& operator=(const MechPurchaseScreen& echPurchaseScreen);

	MechListBox inventoryListBox;
	MechListBox variantListBox;
	LogisticsMechDisplay mechDisplay;

	LogisticsMech* pDragMech;
	aObject dragIcon;

	bool acceptPressed;

	static MechPurchaseScreen* s_instance;

	bool dragStartLeft;

	typedef EList<LogisticsMech, LogisticsMech> MECH_LIST;
	MECH_LIST prevInventory;

	float countDownTime;
	float curCount;
	float previousAmount;
	float oldCBillsAmount;

	float flashTime;

	// HELPER FUNCTIONS

	void addSelectedMech(void);
	void removeSelectedMech(void);
	void addMech(LogisticsMech* pMech);
	void removeMech(LogisticsMech* pMech);
	void endDrag(void);
	bool selectFirstBuyableMech(void);
};

#endif // end of file ( MechPurchaseScreen.h )
