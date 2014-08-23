/*************************************************************************************************\
MPParameterScreen.h			: Interface for the MPParameterScreen component.
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//
\*************************************************************************************************/

//*************************************************************************************************

#pragma once

#ifndef MPPARAMETERSCREEN_H
#define MPPARAMETERSCREEN_H

//#include <mechgui/logisticsscreen.h>
//#include <mechgui/asystem.h>
//#include <mechgui/alistbox.h>
//#include "attributemeter.h"
//#include "simplecamera.h"
//#include "elist.h"
//#include "multplyr.h"
//#include "mploadmap.h"
//#include <mechgui/aanim.h>

struct _MC2Player;



class aStyle2TextListItem : public aTextListItem
{
public:
	aStyle2TextListItem() { hasAnimation = false; normalColor = 0xff808080; }
	virtual int32_t	init( FitIniFile* file, PCSTR blockName );
	virtual void		render();

protected:
	bool hasAnimation;
	aAnimGroup animGroup;
	int32_t normalColor;
	

};

class aPlayerParams : public aObject
{
public:
	aPlayerParams();
	~aPlayerParams();
	aPlayerParams& operator=( const aPlayerParams& src );

	virtual int32_t		init(int32_t xPos, int32_t yPos, int32_t w, int32_t h);
	void				init( FitIniFile* file, PCSTR blockName );

	virtual void		destroy();
	virtual void		render();
	virtual void		update();
	virtual int32_t			handleMessage( uint32_t message, uint32_t who );
	virtual void		move( float offsetX, float offsetY );

	void				setData(const _MC2Player* data);

	bool				hasFocus( );

	bool				isSelected() { return bHasFocus; }

	int32_t					getCommanderID( ) const { return commanderID; }

	void				disableReadyButton();



protected:
	aAnimButton		CBillsSpinnerUpButton;
	aAnimButton		CBillsSpinnerDownButton;
	aAnimButton		ReadyButton;

	aObject*			statics;
	aRect*				rects;
	int32_t				rectCount;
	int32_t				staticCount;

	aText*				textObjects;
	int32_t				textCount;

	aDropList				teamNumberDropList;
	aDropList				factionDropList;

	aStyle2TextListItem		templateItem;

	EString					insigniaName;
	aEdit					edit;

	bool					bHasFocus;
	int32_t					commanderID;

	friend class MPParameterScreen;
};

class CListOfDropListPointers : public EList<aDropList *, aDropList *> {};

class CFocusManager
{
public:
	enum control_species_data_type
	{
		CS_NONE,
		CS_DROPLIST
	};

	CFocusManager();
	void clear();
	PVOID registerDropList(aDropList &DropList);
	void unregisterDropList(aDropList &DropList);
	void update();
	bool somebodyHasTheFocus(); /*this is distinct from the keyboard input focus*/
	aObject* pControlThatHasTheFocus();

private:
	control_species_data_type speciesOfTheControlWhichHasTheFocus;
	aDropList *pDropListThatHasTheFocus;
	CListOfDropListPointers listOfDropListPointers;
};


class MPParameterScreen : public LogisticsScreen
{
public:
	
	MPParameterScreen();
	virtual ~MPParameterScreen();
	
	void				init(FitIniFile* file);
	virtual void		begin();
	virtual void		end();
 	virtual void		render( int32_t xOffset, int32_t yOffset );
	virtual void		render();
	virtual void		update();
	virtual int32_t			handleMessage( uint32_t, uint32_t );
	
	static void			resetCheckBoxes();

	void				setHostLeftDlg( PCSTR playerName);

	static GUID			getGUIDFromFile( PCSTR fileName );

	static void			initializeMap( PCSTR fileName );

	static MPParameterScreen* s_instance;


private:
	int32_t indexOfButtonWithID(int32_t id);
	
	int32_t					chatToSend;

	void				setMission( PCSTR fileName, bool resetData = 1 );
	void				setMissionClientOnly( PCSTR pNewMapName );
	void				checkVersionClientOnly( PCSTR pNewMapName );




	aPlayerParams	playerParameters[MAX_MC_PLAYERS];
	int32_t			playerCount;

	MPLoadMap		mpLoadMap;
	LogisticsMapInfoDialog mapInfoDlg;
	bool			bLoading;
	bool			bShowNoMapDlg;
	bool			bErrorDlg;
	bool			bBootDlg;
	bool			bDisconnectDlg;
	EString			mapName;
	int32_t			bootPlayerID;

	bool			bMapInfoDlg;

	float			delayTime;

	bool			bHostLeftDlg;
	bool			bWaitingToStart;

	

};



//*************************************************************************************************
#endif  // end of file ( MPParameterScreen.h )
