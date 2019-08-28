/*************************************************************************************************\
LogisticsScreen.h			: Interface for the LogisticsScreen component.
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//
\*************************************************************************************************/

#pragma once

#ifndef LOGISTICSSCREEN_H
#define LOGISTICSSCREEN_H

#include "mechgui/asystem.h"


class FitIniFile;

namespace mechgui
{
class aObject;
class aRect;
class aText;
class aAnimButton;
class aButton;
class aEdit;
class aAnimObject;
} // namespace mechgui

namespace mechgui
{

/**************************************************************************************************
CLASS DESCRIPTION
LogisticsScreen:
**************************************************************************************************/
class LogisticsScreen : public aObject
{
	enum Status
	{
		RUNNING = 0,
		NEXT = 1,
		PREVIOUS = 2,
		DONE = 3,
		PAUSED = 4,
		UP,
		DOWN,
		YES,
		NO,
		MAINMENU,
		RESTART,
		MULTIPLAYERRESTART,
		SKIPONENEXT,
		SKIPONEPREVIOUS,
		FADEDOWN,
		FADEUP,
		READYTOLOAD,
		GOTOSPLASH
	};

public:
	LogisticsScreen(void);
	virtual ~LogisticsScreen(void);
	LogisticsScreen(const LogisticsScreen& src);
	LogisticsScreen& operator=(const LogisticsScreen& src);

protected:
	void init(FitIniFile& file, const std::wstring_view& staticName, const std::wstring_view& textName, const std::wstring_view& rectName, const std::wstring_view& buttonName,
		const std::wstring_view& editName = "Edit", const std::wstring_view& animObjectName = "AnimObject", uint32_t neverFlush = 0);

	virtual void update(void);
	virtual void render(void);

	virtual void begin(void);
	virtual void end(void) {}

	virtual void render(int32_t xOffset, int32_t yOffset);

	int32_t getStatus(void);

	aButton* getButton(int32_t who);
	aRect* getRect(int32_t who);

	virtual void moveTo(int32_t xPos, int32_t yPos);
	virtual void move(int32_t xPos, int32_t yPos);

	bool inside(int32_t x, int32_t y);

	void beginFadeIn(float fNewTime)
	{
		fadeInTime = fNewTime;
		fadeOutTime = fadeTime = 0.f;
	}
	void beginFadeOut(float fNewTime)
	{
		fadeInTime = 0.f;
		fadeOutTime = fNewTime;
		fadeTime = 0.f;
	}

	void clear(void); // remove everything

protected:
	aObject* statics;
	aRect* rects;
	int32_t rectCount;
	int32_t staticCount;

	aText* textObjects;
	int32_t textCount;

	aAnimButton* buttons;
	int32_t buttonCount;

	aEdit* edits;
	int32_t editCount;

	aAnimObject* animObjects;
	int32_t animObjectsCount;

	float fadeInTime;
	float fadeOutTime;
	float fadeTime;

protected:
	int32_t status;
	int32_t fadeOutMaxcolour;
	int32_t helpTextArrayID;

private:
	void copyData(const LogisticsScreen&);
	void destroy(void);
};
} // namespace mechgui

#endif // end of file ( LogisticsScreen.h )
