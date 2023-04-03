/*************************************************************************************************\
MPSetupArea.h			: Interface for the MPSetupArea component.
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//
\*************************************************************************************************/

#pragma once

#ifndef MPSETUP_H
#define MPSETUP_H

//#include "mechgui/logisticsscreen.h"
//#include "mechgui/asystem.h"
//#include "mechgui/alistbox.h"
//#include "attributemeter.h"
//#include "simplecamera.h"
//#include "mechgui/aanim.h"

class aButton;

class acolourPicker : public aObject
{
public:
	acolourPicker(void);

	virtual int32_t init(int32_t xPos, int32_t yPos, int32_t w, int32_t h);
	void init(FitIniFile* file, std::wstring_view blockname);

	virtual void destroy(void);
	virtual void render(void);
	virtual void update(void);
	virtual int32_t handleMessage(uint32_t message, uint32_t who);
	virtual void move(float offsetX, float offsetY);
	void setcolour0(int32_t color);
	int32_t getcolour0(void) const
	{
		return color0;
	}
	void setcolour1(int32_t color);
	int32_t getcolour1(void) const
	{
		return color1;
	}

protected:
	aRect mainRect;

	aText tab0text;
	aRect tab0colourOutlineRect;
	aRect tab0colourRect;
	aButton tab0Button;

	aText tab1text;
	aRect tab1colourOutlineRect;
	aRect tab1colourRect;
	aButton tab1Button;

	aObject colorPlaneStatic;
	aRect intensityGradientRect;
	mcScrollBar intesitySliderScrollBar;
	aObject colorPlaneCursorStatic;
	int32_t color0;
	int32_t color1;
	int32_t activeTab;
};

class aStyle1TextListItem : public aTextListItem
{
public:
	aStyle1TextListItem()
	{
		hasAnimation = false;
		normalcolour = 0xff808080;
	}
	virtual int32_t init(FitIniFile* file, std::wstring_view blockname);
	virtual void render(void);

protected:
	bool hasAnimation;
	aAnimGroup animGroup;
	int32_t normalcolour;
};

class aInsigniaListItem : public aListItem
{
public:
	aInsigniaListItem()
	{
		;
	}
	virtual int32_t init(FitIniFile* file, std::wstring_view blockname);
	virtual void update(void);

protected:
	aObject graphic;
	aStyle1TextListItem text;
};

class MPSetupXScreen : public LogisticsScreen
{
public:
	MPSetupXScreen(void);
	virtual ~MPSetupXScreen(void);

	void init(FitIniFile* file);
	bool isDone(void);
	virtual void begin(void);
	virtual void end(void);
	virtual void render(int32_t xOffset, int32_t yOffset);
	virtual void render(void);
	virtual void update(void);
	virtual int32_t handleMessage(uint32_t, uint32_t);

	void updateMPSetup(void); // put into inventory

	bool bDone;

	PVOID* ppConnectionScreen;
	PVOID pLocalBrowserScreen;
	PVOID pDirectTcpipScreen;
	PVOID pModem2ModemScreen;

private:
	int32_t indexOfButtonWithID(int32_t id);

	aComboBox playerNameComboBox;
	aComboBox unitNameComboBox;
	aDropList insigniaDropList;

	acolourPicker colorPicker;
	int32_t basecolour;
	int32_t stripecolour;

	SimpleCamera mechCamera;
	bool bPaintSchemeInitialized;
};

#endif // end of file ( MPSetupArea.h )
