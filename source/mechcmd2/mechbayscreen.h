
#pragma once

#ifndef MECHBAYSCREEN_H
#define MECHBAYSCREEN_H

/*************************************************************************************************\
MechBayScreen.h : Header file for mech selection
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//
\*************************************************************************************************/

#include <mechgui/logisticsscreen.h>
#include "attributemeter.h"
#include "MechListBox.h"
#include "simplecomponentlistbox.h"

class LogisticsMech;
class LogisticsMechIcon;
class SimpleCamera;

#define ICON_COUNT 12
#define ICON_COUNT_X 4
#define ICON_COUNT_Y 3

#define MB_MSG_NEXT 50
#define MB_MSG_PREV 51
#define MB_MSG_ADD 52
#define MB_MSG_REMOVE 53
#define MB_MSG_BUY 54
#define MB_MSG_CHANGE_LOADOUT 55
#define MB_MSG_BUY_SELL 56
#define MB_MSG_MAINMENU 57

class MechBayScreen : public LogisticsScreen
{

  public:
	static MechBayScreen* instance() { return s_instance; }
	MechBayScreen(void);
	virtual ~MechBayScreen(void);

	void init(FitIniFile* file);
	virtual void render(int32_t xOffset, int32_t yOffset);
	virtual void update(void);
	virtual void begin(void);
	virtual void end(void);

	virtual int32_t handleMessage(uint32_t, uint32_t);
	void setMech(LogisticsMech* pMech, bool bCommandFromLB = true);
	void beginDrag(LogisticsMech* pMech);

  private:
	LogisticsMech* pCurMech;
	LogisticsMech* pDragMech;
	LogisticsMechIcon* pIcons;
	aObject dragIcon;
	bool dragLeft;
	int32_t forceGroupCount;

	AttributeMeter attributeMeters[3];
	MechListBox mechListBox;
	aListBox componentListBox;
	aObject dropWeightMeter;
	aAnimation addWeightAnim;
	int32_t addWeightAmount;
	aAnimation removeWeightAnim;
	int32_t removeWeightAmount;

	int32_t weightCenterX;
	int32_t weightCenterY;
	int32_t weightStartColor;
	int32_t weightEndColor;

	static MechBayScreen* s_instance;

	SimpleCamera* mechCamera;
	ComponentListBox loadoutListBox;

	void removeSelectedMech(void);
	void addSelectedMech(void);

	void drawWeightMeter(int32_t xOffset, int32_t yOffset);
	void reinitMechs(void);

	MechBayScreen(const MechBayScreen&);
	MechBayScreen& operator=(const MechBayScreen&);

	void unselectDeploymentTeam(void);
	bool selectFirstFGItem(void);
	bool selectFirstViableLBMech(void);
	LogisticsMech* getFGSelMech(void);
};

#endif