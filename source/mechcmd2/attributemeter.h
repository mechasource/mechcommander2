//===========================================================================//
// AttributeMeter.h	: Implementation of the AttributeMeter component.        //
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#pragma once

#ifndef ATTRIBUTEMETER_H
#define ATTRIBUTEMETER_H

//#include "utilities.h"

class FitIniFile;

constexpr const float MAX_ARMOR_RANGE = 432.f;
constexpr const float MAX_SPEED_RANGE = 35.f;
constexpr const float MAX_JUMP_RANGE = 125.f;

/**************************************************************************************************
CLASS DESCRIPTION
AttributeMeter:
**************************************************************************************************/
class AttributeMeter
{
public:
	AttributeMeter(void) noexcept { }

	void update(void);
	void render(void);
	void render(int32_t xOffset, int32_t yOffset);

	void init(FitIniFile* file, const std::wstring_view& headerName);

	void setValue(float val) { m_percent = val < 0 ? 0 : val; }
	void setAddedValue(float val) { m_addedpercent = val < 0 ? 0 : val; }
	void setColorMin(uint32_t newColor) { m_mincolour = newColor; }
	void setColorMax(uint32_t newColor) { m_maxcolour = newColor; }
	void setAddedColorMin(uint32_t newColor) { m_addedmincolour = newColor; }
	void setAddedColorMax(uint32_t newColor) { m_addedmaxcolour = newColor; }
	void showGUIWindow(bool show) { m_doshow = show; }
	bool pointInside(int32_t mouseX, int32_t mouseY) const;

private:
	RECT m_outsiderect{};
	uint32_t m_unitheight = 0;
	uint32_t m_unitwidth = 0;
	uint32_t m_unitcount = 0;
	uint32_t m_skipwidth = 0;
	uint32_t m_rectcolour = 0;
	uint32_t m_mincolour = 0xff005392;
	uint32_t m_maxcolour = 0xffcdeaff;
	uint32_t m_addedmincolour = 0;
	uint32_t m_addedmaxcolour = 0;
	uint32_t m_numbars = 0;
	uint32_t m_helpid = 0;
	float m_percent = 0;
	float m_addedpercent = 0;
	bool m_doshow = true;
};

#endif // end of file ( AttributeMeter.h )
