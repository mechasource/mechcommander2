/*************************************************************************************************\
MPParameterScreen.h			: Interface for the MPParameterScreen component.
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//
\*************************************************************************************************/

#pragma once

#ifndef MPPARAMETERSCREEN_H
#define MPPARAMETERSCREEN_H

//#include "mechgui/logisticsscreen.h"
//#include "mechgui/asystem.h"
//#include "mechgui/alistbox.h"
//#include "attributemeter.h"
//#include "simplecamera.h"
//#include "elist.h"
//#include "multplyr.h"
//#include "mploadmap.h"
//#include "mechgui/aanim.h"

struct _MC2Player;

class aStyle2TextListItem : public aTextListItem
{
public:
	aStyle2TextListItem()
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

class aPlayerParams : public aObject
{
public:
	aPlayerParams(void);
	~aPlayerParams(void);
	aPlayerParams& operator=(const aPlayerParams& src);

	virtual int32_t init(int32_t xPos, int32_t yPos, int32_t w, int32_t h);
	void init(FitIniFile* file, std::wstring_view blockname);

	virtual void destroy(void);
	virtual void render(void);
	virtual void update(void);
	virtual int32_t handleMessage(uint32_t message, uint32_t who);
	virtual void move(float offsetX, float offsetY);

	void setData(const _MC2Player* data);

	bool hasFocus();

	bool isSelected()
	{
		return bHasFocus;
	}

	int32_t getCommanderID(void) const
	{
		return commanderid;
	}

	void disableReadyButton(void);

protected:
	aAnimButton CBillsSpinnerUpButton;
	aAnimButton CBillsSpinnerDownButton;
	aAnimButton ReadyButton;

	aObject* statics;
	aRect* rects;
	int32_t rectCount;
	int32_t staticCount;

	aText* textObjects;
	int32_t textCount;

	aDropList teamNumberDropList;
	aDropList factionDropList;

	aStyle2TextListItem templateItem;

	std::wstring_view insigniaName;
	aEdit edit;

	bool bHasFocus;
	int32_t commanderid;

	friend class MPParameterScreen;
};

class CListOfDropListPointers : public EList<aDropList*, aDropList*>
{
};

class CFocusManager
{
public:
	enum control_species_data_type
	{
		CS_NONE,
		CS_DROPLIST
	};

	CFocusManager(void);
	void clear(void);
	PVOID registerDropList(aDropList& DropList);
	void unregisterDropList(aDropList& DropList);
	void update(void);
	bool somebodyHasTheFocus(void); /*this is distinct from the keyboard input focus*/
	aObject* pControlThatHasTheFocus(void);

private:
	control_species_data_type speciesOfTheControlWhichHasTheFocus;
	aDropList* pDropListThatHasTheFocus;
	CListOfDropListPointers listOfDropListPointers;
};

class MPParameterScreen : public LogisticsScreen
{
public:
	MPParameterScreen(void);
	virtual ~MPParameterScreen(void);

	void init(FitIniFile* file);
	virtual void begin(void);
	virtual void end(void);
	virtual void render(int32_t xOffset, int32_t yOffset);
	virtual void render(void);
	virtual void update(void);
	virtual int32_t handleMessage(uint32_t, uint32_t);

	static void resetCheckBoxes(void);

	void setHostLeftDlg(std::wstring_view playerName);

	static GUID getGUIDFromFile(std::wstring_view fileName);

	static void initializeMap(std::wstring_view fileName);

	static MPParameterScreen* s_instance;

private:
	int32_t indexOfButtonWithID(int32_t id);

	int32_t chatToSend;

	void setMission(std::wstring_view fileName, bool resetData = 1);
	void setMissionClientOnly(std::wstring_view pNewMapName);
	void checkVersionClientOnly(std::wstring_view pNewMapName);

	aPlayerParams playerParameters[MAX_MC_PLAYERS];
	int32_t playerCount;

	MPLoadMap mpLoadMap;
	LogisticsMapInfoDialog mapInfoDlg;
	bool bLoading;
	bool bShowNoMapDlg;
	bool bErrorDlg;
	bool bBootDlg;
	bool bDisconnectDlg;
	std::wstring_view mapName;
	int32_t bootPlayerID;

	bool bMapInfoDlg;

	float delayTime;

	bool bHostLeftDlg;
	bool bWaitingToStart;
};

#endif // end of file ( MPParameterScreen.h )
