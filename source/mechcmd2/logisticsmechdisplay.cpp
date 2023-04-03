/*************************************************************************************************\
LogisticsMechDisplay.cpp			: Implementation of the LogisticsMechDisplay
component.
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//
\*************************************************************************************************/

#define LOGISTICSMECHDISPLAY_CPP

#include "stdinc.h"
#include "logisticsmechdisplay.h"
#include "logisticsmech.h"
#include "mclib.h"
#include "resource.h"
#include "prefs.h"

LogisticsMechDisplay::LogisticsMechDisplay()
{
	pCurMech = nullptr;
	helpTextArrayID = 7;
}

//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------

void LogisticsMechDisplay::update()
{
	componentListBox.update();
	LogisticsScreen::update();
	mechCamera.update();
}

//-------------------------------------------------------------------------------------------------
LogisticsMechDisplay::~LogisticsMechDisplay() { }

//-------------------------------------------------------------------------------------------------

void LogisticsMechDisplay::render(int32_t xOffset, int32_t yOffset)
{
	componentListBox.move(xOffset, yOffset);
	componentListBox.render();
	componentListBox.move(-xOffset, -yOffset);
	if (xOffset == 0 && yOffset == 0) // don't draw until we're done animating in or out
		mechCamera.render();
	for (size_t i = 0; i < 3; i++)
	{
		attributeMeters[i].render(xOffset, yOffset);
	}
	LogisticsScreen::render(xOffset, yOffset);
}

//-------------------------------------------------------------------------------------------------

int32_t
LogisticsMechDisplay::init()
{
	FitIniFile file;
	FullPathFileName path;
	path.init(artPath, "mcl_mechinfo", ".fit");
	if (NO_ERROR != file.open(path))
	{
		Assert(0, 0, "Couldn't open mcl_mechinfo.fit");
		return 0;
	}
	LogisticsScreen::init(file, "Static", "Text", "Rect", "Button");
	mechCamera.init(rects[1].left(), rects[2].top(), rects[2].left(), rects[2].bottom());
	wchar_t blockname[64];
	for (size_t i = 0; i < 3; i++)
	{
		sprintf(blockname, "AttributeMeter%ld", i);
		attributeMeters[i].init(&file, blockname);
	}
	componentListBox.init(rects[0].left(), rects[0].top(), rects[0].width(), rects[0].height());
	return 1;
}

//-------------------------------------------------------------------------------------------------

void LogisticsMechDisplay::setMech(LogisticsMech* pMech, bool bFromLB)
{
	if (pMech != pCurMech)
	{
		pCurMech = pMech;
		if (pCurMech)
		{
			textObjects[0].setText(pCurMech->getChassisName());
			textObjects[1].setText(pCurMech->getName());
			wchar_t text[64];
			wchar_t tmpStr[64];
			cLoadString(IDS_MB_MECH_WEIGHT, tmpStr, 63);
			sprintf(text, tmpStr, pCurMech->getMaxWeight(), (std::wstring_view)pCurMech->getMechClass());
			textObjects[3].setText(text);
			int32_t tmpcolour;
			textObjects[2].setText(pCurMech->getVariant()->getOptimalRangeString(tmpcolour));
			textObjects[2].setcolour(tmpcolour);
			sprintf(text, "%ld", pCurMech->getArmor());
			textObjects[4].setText(text);
			sprintf(text, "%ld", pCurMech->getDisplaySpeed());
			textObjects[5].setText(text);
			sprintf(text, "%ld", pCurMech->getJumpRange() * 25);
			textObjects[6].setText(text);
			attributeMeters[0].setValue(pCurMech->getArmor() / MAX_ARMOR_RANGE);
			attributeMeters[1].setValue(pCurMech->getSpeed() / MAX_SPEED_RANGE);
			attributeMeters[2].setValue(pCurMech->getJumpRange() * 25 / MAX_JUMP_RANGE);
			std::wstring_view fileName = pMech->getFileName();
			int32_t index = fileName.Find('.');
			fileName = fileName.Left(index);
			index = fileName.ReverseFind('\\');
			fileName = fileName.Right(fileName.Length() - index - 1);
			mechCamera.setMech(
				fileName, prefs.basecolour, prefs.highlightcolour, prefs.highlightcolour);
			mechCamera.setScale(pMech->getVariant()->getChassis()->getScale());
			componentListBox.setMech(pCurMech->getVariant());
		}
		else
		{
			for (size_t i = 0; i < 6; i++)
				textObjects[i].setText("");
			attributeMeters[0].setValue(0);
			attributeMeters[1].setValue(0);
			attributeMeters[2].setValue(0);
			componentListBox.setMech(0);
			mechCamera.setMech(nullptr);
		}
	}
}

// end of file ( LogisticsMechDisplay.cpp )
