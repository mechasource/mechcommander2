//===========================================================================//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#pragma once

#ifndef ASCROLL_H
#define ASCROLL_H

#include "mechgui/asystem.h"
#include "mechgui/abutton.h"

namespace mechgui
{

class mcScrollButton : public aAnimButton
{
public:
	virtual void render(void);

	uint32_t lightEdge;
	uint32_t darkEdge;
	uint32_t regularColor;
};

class aScrollBar : public aObject
{
public:
	aScrollBar(void);

	virtual int32_t init(int32_t xPos, int32_t yPos, int32_t w, int32_t h);
	virtual void destroy(void);
	virtual void update(void);
	virtual void render(void);

	virtual int32_t handleMessage(uint32_t message, uint32_t fromWho);

	void SetScrollMax(float newMax);
	void SetScrollPos(float newPos);
	float GetScrollMax(void) { return scrollMax; };
	float GetScrollPos(void) { return scrollPos; };
	int32_t SetSrollInc(int32_t newInc)
	{
		scrollInc = newInc; // amount you move for one arrow click
	}
	int32_t SetScrollPage(int32_t newInc)
	{
		pageInc = newInc; // amount you move if you click on the bar itself
	}
	void ScrollUp(void);
	void ScrollPageUp(void);
	void ScrollDown(void);
	void ScrollPageDown(void);
	void SetScroll(int32_t newScrollPos);
	void Enable(bool enable);

protected:
	float scrollMax;
	float scrollPos;
	aAnimButton topButton;
	aAnimButton bottomButton;
	mcScrollButton scrollTab;

	int32_t lastY;
	int32_t scrollInc;
	int32_t pageInc;
	uint32_t color;
	void ResizeAreas(void);
};

class mcScrollBar : public aScrollBar
{
public:
	int32_t init(int32_t xPos, int32_t yPos, int32_t w, int32_t h);
	virtual void resize(int32_t w, int32_t h);

	void setOrange(void);
	void setGreen(void);

private:
	aAnimation orangeInfo[4];
	aAnimation greenInfo[4];
};
} // namespace mechgui

#endif
