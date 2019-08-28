//===========================================================================//
// AttributeMeter.cpp	: Implementation of the AttributeMeter component.    //
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

// #define ATTRIBUTEMETER_CPP

#include "stdinc.h"

//#include "utilities.h"
#include "attributemeter.h"
#include "file.h"
#include "inifile.h"
#include "mathfunc.h"
#include "userinput.h"

//#include "mclib.h"

extern uint32_t helpTextID;
extern uint32_t helpTextHeaderID;

bool
AttributeMeter::pointInside(int32_t mouseX, int32_t mouseY) const
{
	if ((m_outsiderect.left) <= mouseX && m_outsiderect.right >= mouseX && m_outsiderect.top <= mouseY && m_outsiderect.bottom >= mouseY)
		return true;
	return false;
}

void
AttributeMeter::update()
{
	int32_t mouseX = userInput->getMouseX();
	int32_t mouseY = userInput->getMouseY();
	if (pointInside(mouseX, mouseY))
		::helpTextID = m_helpid;
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
	if (!m_doshow)
		return;
	float barCount = m_percent * m_numbars;
	uint32_t nBarCount = static_cast<uint32_t>(barCount);
	float remainder = (m_addedpercent * m_numbars) + (barCount - nBarCount);
	uint32_t nAddedCount = static_cast<uint32_t>(remainder);
	RECT tmpOutside = m_outsiderect;
	tmpOutside.left += xOffset;
	tmpOutside.right += xOffset;
	tmpOutside.top += yOffset;
	tmpOutside.bottom += yOffset;
	// drawRect( tmpOutside, 0xff000000 );
	drawEmptyRect(tmpOutside, m_rectcolour, m_rectcolour);
	RECT tmpRect;
	tmpRect.left = m_outsiderect.left + 2 * m_skipwidth + xOffset;
	tmpRect.right = tmpRect.left + m_unitwidth;
	tmpRect.top = m_outsiderect.top + 2 * m_skipwidth + yOffset;
	tmpRect.bottom = m_outsiderect.bottom - 2 * m_skipwidth + yOffset;
	uint32_t color = m_mincolour;
	tmpRect.bottom += m_skipwidth;
	for (i = 0; i < nBarCount; i++)
	{
		drawRect(tmpRect, color);
		color = interpolatecolour(m_mincolour, m_maxcolour, static_cast<float>(i / m_numbars));
		tmpRect.left += m_unitwidth + m_skipwidth;
		tmpRect.right = tmpRect.left + m_unitwidth;
	}
	for (i = 0u; i < nAddedCount; i++)
	{
		drawRect(tmpRect, color);
		color = interpolatecolour(m_addedmincolour, m_addedmaxcolour, static_cast<float>(i / m_numbars));
		tmpRect.left += m_unitwidth + m_skipwidth;
		tmpRect.right = tmpRect.left + m_unitwidth;
	}
	tmpRect.bottom -= m_skipwidth;
	tmpRect.right -= 1;
	for (i = nBarCount + nAddedCount; i < m_numbars; i++)
	{
		drawEmptyRect(tmpRect, m_rectcolour, m_rectcolour);
		tmpRect.left += m_unitwidth + m_skipwidth;
		tmpRect.right = tmpRect.left + m_unitwidth - 1;
	}
}

void
AttributeMeter::init(FitIniFile* file, const std::wstring_view& headerName)
{
	if (NO_ERROR != file->seekBlock(headerName))
	{
#if CONSIDERED_OBSOLETE
		wchar_t errorTxt[256];
		sprintf(errorTxt, "couldn't find block %s in file %s", headerName, file->getFilename());
		ASSERT(0, 0, errorTxt);
#endif
		return;
	}
	if (NO_ERROR == file->readIdLong("left", m_outsiderect.left))
	{
		file->readIdLong("right", m_outsiderect.right);
		file->readIdLong("top", m_outsiderect.top);
		file->readIdLong("bottom", m_outsiderect.bottom);
	}
	else
	{
		file->readIdLong("XLocation", m_outsiderect.left);
		file->readIdLong("YLocation", m_outsiderect.top);
		long32_t tmp;
		file->readIdLong("width", tmp);
		m_outsiderect.right = m_outsiderect.left + tmp;
		file->readIdLong("height", tmp);
		m_outsiderect.bottom = m_outsiderect.top + tmp;
	}
	file->readIdLong("Unitwidth", m_unitwidth);
	file->readIdLong("Skip", m_skipwidth);
	file->readIdUInt("NumberUnits", m_numbars);
	file->readIdUInt("colour", m_rectcolour);
	file->readIdUInt("HelpDesc", m_helpid);
	uint32_t tmp;
	if (NO_ERROR == file->readIdUInt("colourMin", tmp))
	{
		m_mincolour = tmp;
	}
	if (NO_ERROR == file->readIdUInt("colourMax", tmp))
	{
		m_maxcolour = tmp;
	}
}

// end of file ( AttributeMeter.cpp )
