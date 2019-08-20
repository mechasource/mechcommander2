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
#include "mathfunc.h"
#include "userinput.h"

//#include <mclib.h>

extern uint32_t helpTextID;
extern uint32_t helpTextHeaderID;

bool
AttributeMeter::pointInside(int32_t mouseX, int32_t mouseY) const
{
	if ((outsideRect.left) <= mouseX && outsideRect.right >= mouseX && outsideRect.top <= mouseY && outsideRect.bottom >= mouseY)
		return true;
	return false;
}

void
AttributeMeter::update()
{
	int32_t mouseX = userInput->getMouseX();
	int32_t mouseY = userInput->getMouseY();
	if (pointInside(mouseX, mouseY))
		::helpTextID = helpID;
}
void
AttributeMeter::render()
{
	render(0, 0);
}

void
AttributeMeter::render(int32_t xOffset, int32_t yOffset)
{
	uint32_t i;
	if (!bShow)
		return;
	float barCount = percent * numBars;
	uint32_t nBarCount = static_cast<uint32_t>(barCount);
	float remainder = (addedPercent * numBars) + (barCount - nBarCount);
	uint32_t nAddedCount = static_cast<uint32_t>(remainder);
	RECT tmpOutside = outsideRect;
	tmpOutside.left += xOffset;
	tmpOutside.right += xOffset;
	tmpOutside.top += yOffset;
	tmpOutside.bottom += yOffset;
	// drawRect( tmpOutside, 0xff000000 );
	drawEmptyRect(tmpOutside, rectColor, rectColor);
	RECT tmpRect;
	tmpRect.left = outsideRect.left + 2 * skipWidth + xOffset;
	tmpRect.right = tmpRect.left + unitWidth;
	tmpRect.top = outsideRect.top + 2 * skipWidth + yOffset;
	tmpRect.bottom = outsideRect.bottom - 2 * skipWidth + yOffset;
	uint32_t color = colorMin;
	tmpRect.bottom += skipWidth;
	for (i = 0; i < nBarCount; i++)
	{
		drawRect(tmpRect, color);
		color = interpolateColor(colorMin, colorMax, static_cast<float>(i / numBars));
		tmpRect.left += unitWidth + skipWidth;
		tmpRect.right = tmpRect.left + unitWidth;
	}
	for (i = 0u; i < nAddedCount; i++)
	{
		drawRect(tmpRect, color);
		color = interpolateColor(addedColorMin, addedColorMax, static_cast<float>(i / numBars));
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

void
AttributeMeter::init(FitIniFile* file, PCSTR headerName)
{
	if (NO_ERROR != file->seekBlock(headerName))
	{
#if _CONSIDERED_OBSOLETE
		char errorTxt[256];
		sprintf(errorTxt, "couldn't find block %s in file %s", headerName, file->getFilename());
		ASSERT(0, 0, errorTxt);
#endif
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
		long32_t tmp;
		file->readIdLong("Width", tmp);
		outsideRect.right = outsideRect.left + tmp;
		file->readIdLong("Height", tmp);
		outsideRect.bottom = outsideRect.top + tmp;
	}
	file->readIdLong("UnitWidth", unitWidth);
	file->readIdLong("Skip", skipWidth);
	file->readIdUInt("NumberUnits", numBars);
	file->readIdUInt("Color", rectColor);
	file->readIdUInt("HelpDesc", helpID);
	uint32_t tmp;
	if (NO_ERROR == file->readIdUInt("ColorMin", tmp))
	{
		colorMin = tmp;
	}
	if (NO_ERROR == file->readIdUInt("ColorMax", tmp))
	{
		colorMax = tmp;
	}
}

//*************************************************************************************************
// end of file ( AttributeMeter.cpp )
