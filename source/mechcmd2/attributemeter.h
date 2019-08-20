//===========================================================================//
// AttributeMeter.h	: Implementation of the AttributeMeter component.        //
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#pragma once

#ifndef ATTRIBUTEMETER_H
#define ATTRIBUTEMETER_H

//#include "utilities.h"

//*************************************************************************************************
class FifIniFile;

constexpr const float MAX_ARMOR_RANGE = 432.f;
constexpr const float MAX_SPEED_RANGE = 35.f;
constexpr const float MAX_JUMP_RANGE = 125.f;

/**************************************************************************************************
CLASS DESCRIPTION
AttributeMeter:
**************************************************************************************************/
class AttributeMeter
{
private:
	RECT outsideRect;
	uint32_t unitHeight;
	long32_t unitWidth;
	uint32_t unitCount;
	long32_t skipWidth;
	uint32_t rectColor;
	uint32_t colorMin;
	uint32_t colorMax;
	uint32_t addedColorMin;
	uint32_t addedColorMax;
	float percent;
	float addedPercent;
	uint32_t numBars;
	uint32_t helpID;
	bool bShow;

public:
	AttributeMeter(void) :
		unitHeight(0), unitWidth(0), unitCount(0), skipWidth(0), rectColor(0),
		colorMin(0xff005392), colorMax(0xffcdeaff), addedColorMin(0), addedColorMax(0),
		percent(0), addedPercent(0), numBars(0), helpID(0), bShow(true)
	{
		outsideRect = {0};
	}

	void update(void);
	void render(void);
	void render(int32_t xOffset, int32_t yOffset);

	void init(FitIniFile* file, PCSTR headerName);

	void setValue(float val) { percent = val < 0 ? 0 : val; }
	void setAddedValue(float val) { addedPercent = val < 0 ? 0 : val; }
	void setColorMin(uint32_t newColor) { colorMin = newColor; }
	void setColorMax(uint32_t newColor) { colorMax = newColor; }
	void setAddedColorMin(uint32_t newColor) { addedColorMin = newColor; }
	void setAddedColorMax(uint32_t newColor) { addedColorMax = newColor; }
	void showGUIWindow(bool show) { bShow = show; }
	bool pointInside(int32_t mouseX, int32_t mouseY) const;
};

//*************************************************************************************************
#endif // end of file ( AttributeMeter.h )
