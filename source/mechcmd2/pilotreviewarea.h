/*************************************************************************************************\
PilotReviewARea.h			: Interface for the PilotReviewARea component.
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//
\*************************************************************************************************/

//*************************************************************************************************

#pragma once

#ifndef PILOTREVIEWAREA_H
#define PILOTREVIEWAREA_H

//#include <mechgui/logisticsscreen.h>
//#include <mechgui/alistbox.h>
//#include "attributemeter.h"
//#include <mechgui/aanim.h>
//#include "logisticspilot.h"

class aButton;
class PilotIcon;
class PilotPromotionArea;

/**************************************************************************************************
CLASS DESCRIPTION
PilotReviewARea:
**************************************************************************************************/

// holds the dead and active pilots
class PilotListBox : public aListBox
{
public:
	PilotListBox(void);
	virtual int32_t AddItem(aListItem* item);
	virtual void update(void);

	bool isDone() { return bDone; }

private:
	float timeSinceStart;
	int32_t curItem;
	int32_t oldScroll;
	int32_t newScroll;
	float scrollTime;
	bool bDone;
};

// this screen shows up after the salvage screen
class PilotReviewScreen : public LogisticsScreen
{
public:
	PilotReviewScreen(void);
	virtual ~PilotReviewScreen(void);

	void init(FitIniFile* file);
	bool isDone(void);
	virtual void render(void);
	virtual void update(void);
	virtual int32_t handleMessage(uint32_t, uint32_t);

	void updatePilots(void); // put into inventory, save??

	bool bDone;

	static PilotPromotionArea* s_curPromotion;
	static PilotReviewScreen* instance;

private:
	PilotListBox pilotListBox;
	aAnimation entryAnim;
	aAnimation exitAnim;
};

// base class for dead, and active pilot list box items
class PilotListItem : public aListItem
{
public:
	PilotListItem()
	{
		currentTime = -1.f;
		bDone		= 0;
	}

	virtual void begin(void);  // for animation purposes...
	virtual bool isDone() = 0; // implement this yourself
	virtual void update(void);

protected:
	float currentTime;
	bool bDone;
};

// list box item for dead pilots
class DeadPilotListItem : public PilotListItem
{
public:
	virtual void render(void);
	virtual void update(void);
	virtual bool isDone(void);

	virtual ~DeadPilotListItem(void);

	static void init(FitIniFile* file);

	LogisticsPilot* getPilot() { return pPilot; }

	DeadPilotListItem(LogisticsPilot* pUnit);

private:
	friend class PilotReviewScreen;

	LogisticsPilot* pPilot;

	static aText* s_nameText;
	static aText* s_rankText;
	static aText* s_missionText;
	static aText* s_killsText;
	static aRect* s_area;
	static aRect* s_liveIconRect;
	static aRect* s_deadIconRect;
	static int32_t s_itemCount;

	aText nameText;
	aText rankText;
	aText missionText;
	aText killsText;

	PilotIcon* liveIcon;
	PilotIcon* deadIcon;
};

// pilots that weren't killed
class ActivePilotListItem : public PilotListItem
{
public:
	virtual void render(void);
	virtual void update(void);
	virtual bool isDone(void);

	virtual ~ActivePilotListItem(void);

	static void init(FitIniFile* file);
	ActivePilotListItem(LogisticsPilot* pUnit);
	static int32_t s_totalWidth;

	float flashTime(void);

protected:
	AttributeMeter attributeMeters[2];
	static AttributeMeter* s_attributeMeters[2];

	static aObject* s_icons[8];
	static aText* s_nameText;
	static aText* s_missionText;
	static aText* s_killsText;
	static int32_t s_itemCount;
	static aRect* s_outline[5];

	static aText* s_rankText;
	static aText* s_gunneryText;
	static aText* s_pilotingText;
	static aRect* s_area;
	static aRect* s_iconRect;
	static aRect* s_killIconRect;
	static PilotPromotionArea* s_pilotPromotionArea;
	static aObject* s_medals[MAX_MEDAL];
	static aText* s_promotionText;
	static aText* s_medalText;
	static aText* s_medalAwardedText;
	static aAnimation* s_skillAnim;
	static aAnimation* s_medalAwardedAnim;
	static aAnimation* s_pilotPromotedAnim;

	aObject icons[4];
	aText nameText;
	aText missionText;
	aText killsText;
	int32_t itemCount;

	aText rankText;
	aText gunneryText;
	aText pilotingText;
	aRect area;

	aObject* medalIcons[MAX_MEDAL];
	aText* medalTexts[MAX_MEDAL];
	aText promotionText;
	aText medalAwardedText;

	PilotIcon* pilotIcon;
	LogisticsPilot* pilot;

	bool showingPromotion;
	bool promotionShown;

	int32_t medalCount;
};

class PilotPromotionArea : public LogisticsScreen
{
public:
	bool isDone(void);
	void init(FitIniFile& file);
	virtual void render(void);
	virtual void update(void);
	void setPilot(LogisticsPilot* pPilot, PilotIcon* pIcon);
	virtual int32_t handleMessage(uint32_t, uint32_t);
	virtual ~PilotPromotionArea(void);

	aListBox* getSkillListBox() { return &skillListBox; }

private:
	AttributeMeter attributeMeters[2];

	LogisticsScreen areaLeft;
	LogisticsScreen areaRight;
	LogisticsPilot* pilot;
	aAnimation leftInfo;
	aAnimation rightInfo;
	aAnimation leftExitInfo;
	aAnimation rightExitInfo;
	bool bDone;

	aListBox skillListBox;
	PilotIcon* pilotIcon;
	aAnimation selSkillAnim;
	int32_t lastCheck;
};

// specialty skills each show up in here
class SpecialtyListItem : public aListItem
{
public:
	virtual void render(void);
	virtual void update(void);

	virtual ~SpecialtyListItem(void);

	static void init(FitIniFile* file);
	virtual int32_t handleMessage(uint32_t message, uint32_t who);

	virtual bool isChecked(void);
	virtual void setCheck(bool);
	int32_t getID(void);
	SpecialtyListItem(int32_t ID);

private:
	static aButton* s_radioButton;
	static aObject* s_skillIcons[4];
	static aAnimation* s_highlightAnim;
	static aAnimation* s_normalAnim;
	static aAnimation* s_pressedAnim;
	static aText* s_description;
	static aRect* s_area;
	static aRect* s_outline;
	static int32_t s_itemCount;

	static void deleteStatics(void);

	aAnimation pressedAnim;
	aAnimation highlightAnim;
	aAnimation normalAnim;
	aObject icon;
	aText description;
	aButton radioButton;
	aRect outline;

	int32_t ID;

	friend class PilotPromotionArea;
};

//*************************************************************************************************
#endif // end of file ( PilotReview.h )
