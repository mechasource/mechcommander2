
#pragma once

#ifndef MAINMENU_H
#define MAINMENU_H
/*************************************************************************************************\
MainMenu.h			: Interface for the MainMenu component.
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//
\*************************************************************************************************/

#ifndef LOGISTICSSCREEN_H
#include "mechgui/logisticsscreen.h"
#endif

#ifndef AANIM_H
#include "mechgui/aanim.h"
#endif

#ifndef MPLOADMAP_H
#include "MPLoadMap.h"
#endif

#ifndef MC2movie_H
#include "mc2movie.h"
#endif

class OptionsScreenWrapper;
class Mechlopedia;

/**************************************************************************************************
CLASS DESCRIPTION
MainMenu:
**************************************************************************************************/
class SplashIntro : public LogisticsScreen
{
public:
	SplashIntro() {}
	virtual ~SplashIntro() {}

	void init(void);
};

class MainMenu : public LogisticsScreen
{
public:
	MainMenu(void);
	virtual ~MainMenu(void);

	int32_t init(FitIniFile& file);

	virtual void begin(void);
	virtual void end(void);
	virtual void update(void);
	virtual void render(void);

	void setHostLeftDlg(const std::wstring_view& playerName);

	void setDrawBackground(bool bDrawBackground);
	void skipIntro(void);

	virtual int32_t handleMessage(uint32_t, uint32_t);

	static bool bDrawMechlopedia;

private:
	MainMenu& operator=(const MainMenu& ainMenu);
	MainMenu(const MainMenu& src);

	bool bDrawBackground;

	LogisticsScreen background;

	bool promptToQuit;
	bool bOptions;
	bool bSave;
	bool bLoad;
	bool bLoadSingle;
	bool bLoadCampaign;
	bool promptToDisconnect;
	bool bLegal;

	int32_t tuneId; // What music should I play here!
	bool musicStarted; // Should I restart the tune?

	int32_t endResult;

	aAnimation beginAnim;
	aAnimation endAnim;

	OptionsScreenWrapper* optionsScreenWrapper;
	Mechlopedia* mechlopedia;
	SplashIntro intro;
	bool introOver;
	MPLoadMap singleLoadDlg;
	bool bHostLeftDlg;

	MC2MoviePtr introMovie;
};

#endif // end of file ( MainMenu.h )
