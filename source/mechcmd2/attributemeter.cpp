//===========================================================================//
// AttributeMeter.cpp	: Implementation of the AttributeMeter component.    //
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

// #define ATTRIBUTEMETER_CPP

#include "stdinc.h"
#include "utilities.h"
#include "attributemeter.h"
#include "file.h"
#include "inifile.h"
//#include <mclib.h>

extern int32_t helpTextID;
extern int32_t helpTextHeaderID;

bool AttributeMeter::pointInside(int32_t mouseX, int32_t mouseY) const
{
	if ((outsideRect.left) <= mouseX && outsideRect.right >= mouseX &&
		outsideRect.top <= mouseY && outsideRect.bottom >= mouseY)
		return true;
	return false;
}

void AttributeMeter::update()
{
	int32_t mouseX = userInput->getMouseX();
	int32_t mouseY = userInput->getMouseY();
	if (pointInside(mouseX, mouseY))
		::helpTextID = helpID;
}
void AttributeMeter::render() { render(0, 0); }

void AttributeMeter::render(int32_t xOffset, int32_t yOffset)
{
	int32_t i;
	if (!bShow)
		return;
	float barCount	= percent * numBars;
	int32_t nBarCount = (int32_t)barCount;
	float remainder   = addedPercent * numBars + (barCount - float(nBarCount));
	int32_t nAddedCount = (int32_t)remainder;
	RECT tmpOutside		= outsideRect;
	tmpOutside.left += xOffset;
	tmpOutside.right += xOffset;
	tmpOutside.top += yOffset;
	tmpOutside.bottom += yOffset;
	// drawRect( tmpOutside, 0xff000000 );
	drawEmptyRect(tmpOutside, rectColor, rectColor);
	RECT tmpRect;
	tmpRect.left   = outsideRect.left + 2 * skipWidth + xOffset;
	tmpRect.right  = tmpRect.left + unitWidth;
	tmpRect.top	= outsideRect.top + 2 * skipWidth + yOffset;
	tmpRect.bottom = outsideRect.bottom - 2 * skipWidth + yOffset;
	int32_t color  = colorMin;
	tmpRect.bottom += skipWidth;
	for (i = 0; i < nBarCount; i++)
	{
		drawRect(tmpRect, color);
		color =
			interpolateColor(colorMin, colorMax, ((float)i) / (float)numBars);
		tmpRect.left += unitWidth + skipWidth;
		tmpRect.right = tmpRect.left + unitWidth;
	}
	for (i = 0; i < nAddedCount; i++)
	{
		drawRect(tmpRect, color);
		color = interpolateColor(
			addedColorMin, addedColorMax, ((float)i) / (float)numBars);
		tmpRect.left += unitWidth + skipWidth;
		tmpRect.right = tmpRect.left + unitWidth;
	}
	tmpRect.bottom -= skipWidth;
	tmpRect.right -= 1;
	for (i = nBarCount + nAddedCount; i < numBars; i++)
	{
		drawEmptyRect(tmpRect, rectColor, rectColor);
		tmpRect.left += unitWidth + skipWidth;
		tmpRect.right = tmpRect.left + unitWidth - 1;
	}
}

void AttributeMeter::init(FitIniFile* file, PCSTR headerName)
{
	if (NO_ERROR != file->seekBlock(headerName))
	{
		char errorTxt[256];
		sprintf(errorTxt, "couldn't find block %s in file %s", headerName,
			file->getFilename());
		Assert(0, 0, errorTxt);
		return;
	}
	if (NO_ERROR == file->readIdLong("left", outsideRect.left))
	{
		file->readIdLong("right", outsideRect.right);
		file->readIdLong("top", outsideRect.top);
		file->readIdLong("bottom", outsideRect.bottom);
	}
	else
	{
		file->readIdLong("XLocation", outsideRect.left);
		file->readIdLong("YLocation", outsideRect.top);
		int32_t tmp;
		file->readIdLong("Width", tmp);
		outsideRect.right = outsideRect.left + tmp;
		file->readIdLong("Height", tmp);
		outsideRect.bottom = outsideRect.top + tmp;
	}
	file->readIdLong("UnitWidth", unitWidth);
	file->readIdLong("Skip", skipWidth);
	file->readIdLong("NumberUnits", numBars);
	file->readIdLong("Color", rectColor);
	file->readIdLong("HelpDesc", helpID);
	int32_t tmp;
	if (NO_ERROR == file->readIdLong("ColorMin", tmp))
	{
		colorMin = tmp;
	}
	if (NO_ERROR == file->readIdLong("ColorMax", tmp))
	{
		colorMax = tmp;
	}
}

//*************************************************************************************************
// end of file ( AttributeMeter.cpp )
