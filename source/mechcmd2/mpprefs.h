/*************************************************************************************************\
MPPrefs.h			: Interface for the MPPrefs component.
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//
\*************************************************************************************************/

#pragma once

#ifndef MPPREFS_H
#define MPPREFS_H

//#include "mechgui/logisticsscreen.h"
//#include "mechgui/alistbox.h"
//#include "simplecamera.h"

class FitIniFile;
struct _MC2Player;

/**************************************************************************************************
CLASS DESCRIPTION
MPPrefs:
**************************************************************************************************/
class MPPrefs : public LogisticsScreen
{
public:
	MPPrefs(void);
	virtual ~MPPrefs(void);

	static MPPrefs* instance()
	{
		return s_instance;
	}
	int32_t init(FitIniFile& file);
	virtual void update(void);
	virtual void render(int32_t OffsetX, int32_t OffsetY);
	virtual int32_t handleMessage(uint32_t message, uint32_t who);
	virtual void begin(void);
	virtual void end(void);
	void initcolours(void);

	void saveSettings(void);
	void cancelSettings(void);

	void setMechcolours(uint32_t base,
		uint32_t highlight); // called by MPlayer when it resets a color

private:
	MPPrefs(const MPPrefs& src);
	MPPrefs& operator=(const MPPrefs& PPrefs);

	aComboBox comboBox[3];
	SimpleCamera camera;
	aObject insigniaBmp; // the one inside the combo box...

	// HELPERS
	void updateStripecolours(const _MC2Player* players, int32_t playerCount, bool bDrawRect);
	void updateBasecolours(const _MC2Player* players, int32_t playerCount, bool bDrawRect);
	wchar_t getcolourIndex(uint32_t color);
	void setcolour(uint32_t color);
	void setHighlightcolour(uint32_t color);

	static MPPrefs* s_instance;
};

class aBmpListItem : public aListItem
{
public:
	int32_t setBmp(std::wstring_view pFileName);
	std::wstring_view getBmp()
	{
		return fileName;
	}

private:
	aObject bmp;

	std::wstring_view fileName;
};

#endif // end of file ( MPPrefs.h )
