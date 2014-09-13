/*************************************************************************************************\
OptionsArea.h			: Interface for the OptionsArea component.
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//
\*************************************************************************************************/

//*************************************************************************************************

#pragma once

#ifndef OPTIONSAREA_H
#define OPTIONSAREA_H

//#include <mechgui/logisticsscreen.h>
//#include "simplecamera.h"
//#include <mechgui/alistbox.h>
//#include "attributemeter.h"

class aButton;

class CPrefs;


class OptionsXScreen : public LogisticsScreen
{
public:

	OptionsXScreen(void);
	virtual ~OptionsXScreen(void);

	void init(FitIniFile* file);
	bool isDone(void);
	virtual void render(void);
	virtual void update(void);
	virtual int32_t			handleMessage(uint32_t, uint32_t);

	void updateOptions(void); // put into inventory

	bool				bDone;


private:

	int32_t indexOfButtonWithID(int32_t id);

	LogisticsScreen*		tabAreas[4];
	int32_t						curTab;
	bool			bShowWarning;


};

class ScrollX : public aObject
{
public:

	ScrollX(void);
	int32_t	init(aButton* pLeft, aButton* pRight, aButton* pTab);
	virtual void	update(void);

	virtual int32_t		handleMessage(uint32_t message, uint32_t fromWho);

	void			SetScrollMax(float newMax);
	void			SetScrollPos(float newPos);
	float			GetScrollMax(void)
	{
		return scrollMax;
	};
	float			GetScrollPos(void)
	{
		return scrollPos;
	};
	int32_t			SetSrollInc(int32_t newInc)
	{
		scrollInc = (float)newInc;    // amount you move for one arrow click
	}
	int32_t			SetScrollPage(int32_t newInc)
	{
		pageInc = (float)newInc;   // amount you move if you click on the bar itself
	}
	void			ScrollUp(void);
	void			ScrollPageUp(void);
	void			ScrollDown(void);
	void			ScrollPageDown(void);
	void			SetScroll(int32_t newScrollPos);
	void			Enable(bool enable);

private:

	void ResizeAreas(void);


	aButton*		buttons[3];
	float			scrollMax;
	float			scrollPos;
	float			scrollInc;
	float			pageInc;

	int32_t			lastX;

};

class OptionsGraphics : public LogisticsScreen
{

public:
	void init(int32_t xOffset, int32_t yOffset);
	virtual void render(void);
	virtual void update(void);
	virtual void begin(void);
	virtual void end(void);
	void reset(const CPrefs& newPrefs);

	virtual int32_t		handleMessage(uint32_t message, uint32_t fromWho);

private:
	aDropList		resolutionList;
	aDropList		cardList;
	bool			bExpanded;

};

class OptionsAudio : public LogisticsScreen
{
public:
	void init(int32_t xOffset, int32_t yOffset);
	virtual void render(void);
	virtual void update(void);
	virtual void begin(void);
	virtual void end(void);
	void reset(const CPrefs& newPrefs);

	virtual int32_t		handleMessage(uint32_t message, uint32_t fromWho);

private:

	ScrollX		scrollBars[5];

};

class OptionsHotKeys : public LogisticsScreen
{

public:
	void init(int32_t xOffset, int32_t yOffset);
	virtual void render(void);
	virtual void update(void);
	virtual void begin(void);
	virtual void end(void);
	void reset(bool bUseOld);

	virtual int32_t		handleMessage(uint32_t message, uint32_t fromWho);

private:

	static void makeKeyString(int32_t hotKey, PSTR buffer);
	static int32_t makeInputKeyString(int32_t& hotKey, PSTR buffer);


	aListBox		hotKeyList;
	bool			bShowDlg;
	int32_t			curHotKey;
};

class OptionsGamePlay : public LogisticsScreen
{
public:
	void init(int32_t xOffset, int32_t yOffset);
	virtual void render(void);
	virtual void update(void);
	virtual void begin(void);
	virtual void end(void);
	void reset(const CPrefs& newPrefs);

	virtual int32_t		handleMessage(uint32_t message, uint32_t fromWho);

	void resetCamera(void);

private:

	SimpleCamera	camera;
};

class HotKeyListItem : public aListItem
{
public:

	static void init(void);
	virtual void render(void);
	virtual void update(void);

	void setDescription(PCSTR pText);
	void setKey(PCSTR pText);

	HotKeyListItem();
	~HotKeyListItem(void);

	void	setHotKey(int32_t lNew)
	{
		hotKey = lNew;
	}
	void	setCommand(int32_t lCommand)
	{
		command = lCommand;
	}

	int32_t	getCommand(void) const
	{
		return command;
	}
	int32_t	getHotKey(void) const
	{
		return hotKey;
	}

private:

	aText		description;
	aText		text;
	aAnimGroup	animations[3];
	aRect		rects[2];
	int32_t		hotKey;
	int32_t		command;

	static		HotKeyListItem* s_item;

};





//*************************************************************************************************
#endif  // end of file ( OptionsArea.h )
