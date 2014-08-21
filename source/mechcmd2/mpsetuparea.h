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

//#include "logisticsscreen.h"
//#include "asystem.h"
//#include "alistbox.h"
//#include "attributemeter.h"
//#include "simplecamera.h"
//#include "aanim.h"

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
	virtual int			handleMessage( ULONG message, ULONG who );
	virtual void		move( float offsetX, float offsetY );
	void				setColor0(int color);
	int				getColor0() const { return color0; }
	void				setColor1(int color);
	int				getColor1() const { return color1; }

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
	int		color0;
	int		color1;
	int		activeTab;
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
	int normalColor;
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
 	virtual void render( int xOffset, int yOffset );
	virtual void render();
	virtual void update();
	virtual int			handleMessage( ULONG, ULONG );

	void updateMPSetup(); // put into inventory

	bool				bDone;

	PVOID*	ppConnectionScreen;
	PVOID	pLocalBrowserScreen;
	PVOID	pDirectTcpipScreen;
	PVOID	pModem2ModemScreen;

private:

	int indexOfButtonWithID(int id);

	aComboBox				playerNameComboBox;
	aComboBox				unitNameComboBox;
	aDropList				insigniaDropList;

	aColorPicker		colorPicker;
	int		baseColor;
	int		stripeColor;

	SimpleCamera		mechCamera;
	bool bPaintSchemeInitialized;
};



//*************************************************************************************************
#endif  // end of file ( MPSetupArea.h )
