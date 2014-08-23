/*************************************************************************************************\
MPSetupArea.h			: Interface for the MPSetupArea component.
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//
\*************************************************************************************************/

//*************************************************************************************************

#pragma once

#ifndef MPSETUP_H
#define MPSETUP_H

//#include <mechgui/logisticsscreen.h>
//#include <mechgui/asystem.h>
//#include <mechgui/alistbox.h>
//#include "attributemeter.h"
//#include "simplecamera.h"
//#include <mechgui/aanim.h>

class aButton;


class aColorPicker : public aObject
{
public:

	aColorPicker();

	virtual int32_t		init(int32_t xPos, int32_t yPos, int32_t w, int32_t h);
	void				init( FitIniFile* file, PCSTR blockName );

	virtual void		destroy();
	virtual void		render();
	virtual void		update();
	virtual int32_t			handleMessage( uint32_t message, uint32_t who );
	virtual void		move( float offsetX, float offsetY );
	void				setColor0(int32_t color);
	int32_t				getColor0() const { return color0; }
	void				setColor1(int32_t color);
	int32_t				getColor1() const { return color1; }

protected:
	aRect		mainRect;

	aText		tab0text;
	aRect		tab0ColorOutlineRect;
	aRect		tab0ColorRect;
	aButton		tab0Button;

	aText		tab1text;
	aRect		tab1ColorOutlineRect;
	aRect		tab1ColorRect;
	aButton		tab1Button;

	aObject		colorPlaneStatic;
	aRect		intensityGradientRect;
	mcScrollBar		intesitySliderScrollBar;
	aObject		colorPlaneCursorStatic;
	int32_t		color0;
	int32_t		color1;
	int32_t		activeTab;
};

class aStyle1TextListItem : public aTextListItem
{
public:
	aStyle1TextListItem() { hasAnimation = false; normalColor = 0xff808080; }
	virtual int32_t	init( FitIniFile* file, PCSTR blockName );
	virtual void		render();

protected:
	bool hasAnimation;
	aAnimGroup animGroup;
	int32_t normalColor;
};

class aInsigniaListItem : public aListItem
{
public:
	aInsigniaListItem() { ; }
	virtual int32_t	init( FitIniFile* file, PCSTR blockName );
	virtual void update();

protected:
	aObject graphic;
	aStyle1TextListItem text;
};

class MPSetupXScreen : public LogisticsScreen
{
public:
	
	MPSetupXScreen();
	virtual ~MPSetupXScreen();
	
	void init(FitIniFile* file);
	bool isDone();
	virtual void		begin();
	virtual void		end();
 	virtual void render( int32_t xOffset, int32_t yOffset );
	virtual void render();
	virtual void update();
	virtual int32_t			handleMessage( uint32_t, uint32_t );

	void updateMPSetup(); // put into inventory

	bool				bDone;

	PVOID*	ppConnectionScreen;
	PVOID	pLocalBrowserScreen;
	PVOID	pDirectTcpipScreen;
	PVOID	pModem2ModemScreen;

private:

	int32_t indexOfButtonWithID(int32_t id);

	aComboBox				playerNameComboBox;
	aComboBox				unitNameComboBox;
	aDropList				insigniaDropList;

	aColorPicker		colorPicker;
	int32_t		baseColor;
	int32_t		stripeColor;

	SimpleCamera		mechCamera;
	bool bPaintSchemeInitialized;
};



//*************************************************************************************************
#endif  // end of file ( MPSetupArea.h )
