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

//#include "logisticsscreen.h"
//#include "simplecamera.h"
//#include "alistbox.h"
//#include "attributemeter.h"

class aButton;

class CPrefs;


class OptionsXScreen : public LogisticsScreen
{
public:
	
	OptionsXScreen();
	virtual ~OptionsXScreen();
	
	void init(FitIniFile* file);
	bool isDone();
	virtual void render();
	virtual void update();
	virtual int			handleMessage( ULONG, ULONG );

	void updateOptions(); // put into inventory

	bool				bDone;


private:

	int indexOfButtonWithID(int id);

	LogisticsScreen*		tabAreas[4];
	int						curTab;
	bool			bShowWarning;


};

class ScrollX : public aObject
{
public:

	ScrollX();
	int32_t	init(aButton* pLeft, aButton* pRight, aButton* pTab );
	virtual void	update();

	virtual int		handleMessage( ULONG message, ULONG fromWho );

	void			SetScrollMax(float newMax);
	void			SetScrollPos(float newPos);
	float			GetScrollMax(void){return scrollMax;};
	float			GetScrollPos(void){return scrollPos;};
	int32_t			SetSrollInc( int32_t newInc ){ scrollInc = (float)newInc; } // amount you move for one arrow click
	int32_t			SetScrollPage(int32_t newInc){ pageInc = (float)newInc;} // amount you move if you click on the bar itself
	void			ScrollUp(void);
	void			ScrollPageUp(void);
	void			ScrollDown(void);
	void			ScrollPageDown(void);
	void			SetScroll( int32_t newScrollPos );	
	void			Enable( bool enable );

private:

	void ResizeAreas();

	 
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
	virtual void render();
	virtual void update();
	virtual void begin();
	virtual void end();
	void reset( const CPrefs& newPrefs);

	virtual int		handleMessage( ULONG message, ULONG fromWho );

private:
	aDropList		resolutionList;
	aDropList		cardList;
	bool			bExpanded;

};

class OptionsAudio : public LogisticsScreen
{
	public:
		void init(int32_t xOffset, int32_t yOffset);
		virtual void render();
		virtual void update();
		virtual void begin();
		virtual void end();
		void reset(const CPrefs& newPrefs);

		virtual int		handleMessage( ULONG message, ULONG fromWho );

	private:

		ScrollX		scrollBars[5];

};

class OptionsHotKeys : public LogisticsScreen
{

public:
		void init(int32_t xOffset, int32_t yOffset);
		virtual void render();
		virtual void update();
		virtual void begin();
		virtual void end();
		void reset(bool bUseOld);

		virtual int		handleMessage( ULONG message, ULONG fromWho );

private:

	static void makeKeyString( int32_t hotKey, PSTR buffer );
	static int makeInputKeyString( int32_t& hotKey, PSTR buffer );


	aListBox		hotKeyList;
	bool			bShowDlg;
	int32_t			curHotKey;
};

class OptionsGamePlay : public LogisticsScreen
{
public:
	void init(int32_t xOffset, int32_t yOffset);
	virtual void render();
	virtual void update();
	virtual void begin();
	virtual void end();
	void reset(const CPrefs& newPrefs);

	virtual int		handleMessage( ULONG message, ULONG fromWho );

	void resetCamera();

	private:

	SimpleCamera	camera;
};

class HotKeyListItem : public aListItem
{
public:

	static void init();
	virtual void render();
	virtual void update();
	
	void setDescription( PCSTR pText );
	void setKey( PCSTR pText );

	HotKeyListItem( );
	~HotKeyListItem();

	void	setHotKey( int32_t lNew ){ hotKey = lNew; }
	void	setCommand( int32_t lCommand ) { command = lCommand; }

	int32_t	getCommand() const { return command; }
	int32_t	getHotKey() const { return hotKey; }

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
