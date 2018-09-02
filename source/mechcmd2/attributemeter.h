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

#define MAX_ARMOR_RANGE 432.f
#define MAX_SPEED_RANGE 35.f
#define MAX_JUMP_RANGE 125.f

/**************************************************************************************************
CLASS DESCRIPTION
AttributeMeter:
**************************************************************************************************/
class AttributeMeter
{
private:
	RECT		outsideRect;
	int32_t		unitHeight;
	int32_t		unitWidth;
	int32_t		unitCount;
	int32_t		skipWidth;
	int32_t		rectColor;
	int32_t		colorMin;
	int32_t		colorMax;
	int32_t		addedColorMin;
	int32_t		addedColorMax;
	float		percent;
	float		addedPercent;
	int32_t		numBars;
	int32_t		helpID;
	bool		bShow;

public:
	AttributeMeter(void) : 
		unitHeight(0), unitWidth(0), unitCount(0), skipWidth(0), rectColor(0),
		colorMin(0xff005392), colorMax(0xffcdeaff), 
		addedColorMin(0), addedColorMax(0),
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
	void setColorMin(int32_t newColor) { colorMin = newColor; }
	void setColorMax(int32_t newColor) { colorMax = newColor; }
	void setAddedColorMin(int32_t newColor) { addedColorMin = newColor; }
	void setAddedColorMax(int32_t newColor) { addedColorMax = newColor; }
	void showGUIWindow(bool show) { bShow = show; }
	bool pointInside(int32_t mouseX, int32_t mouseY) const;

};

//*************************************************************************************************
#endif // end of file ( AttributeMeter.h )
