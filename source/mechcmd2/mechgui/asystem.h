//===========================================================================//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#pragma once

#ifndef ASYSTEM_H
#define ASYSTEM_H

//#include <gameos.hpp>
//#include <estring.h>
//#include <mechgui/afont.h>
//#include "utilities.h"

// class aSystem;
// class aCallback;
// class aObject;
// class aTitleWindow;
// class InterfaceObject;

class FitIniFile;

namespace mechgui
{

typedef enum __asystem_constants
{
	MAX_CHILDREN			= 64,
	aMSG_LEFTMOUSEDOWN		= 1,
	aMSG_MIDDLEMOUSEDOWN	= 2,
	aMSG_RIGHTMOUSEDOWN		= 3,
	aMSG_LEFTMOUSEUP		= 4,
	aMSG_MIDDLEMOUSEUP		= 5,
	aMSG_RIGHTMOUSEUP		= 6,
	aMSG_MOUSEMOVE			= 7,
	aMSG_LEFTMOUSEDBLCLICK  = 16,
	aMSG_RIGHTMOUSEDBLCLICK = 17,
	aMSG_LEFTMOUSEHELD		= 18,
	aMSG_RIGHTMOUSEHELD		= 19,
	aMSG_SCROLLUP			= 101,
	aMSG_SCROLLDOWN			= 102,
	aMSG_PAGEUP				= 103,
	aMSG_PAGEDOWN			= 104,
	aMSG_TRACKTAB			= 106,
	aMSG_SCROLLTO			= 107, //	sent by parent
	aMSG_SCROLLPARENT		= 108, //	sent to parent
	aMSG_SELECTME			= 109,
	aMSG_DONE				= 110,
	aMSG_BUTTONCLICKED		= 111,
	aMSG_SELCHANGED			= 112,
};

#if _CONSIDERED_OBSOLETE
typedef enum WINDOW_ID
{
	UNDEFINEDWINDOW = -1,
	GENERIC			= 1,
	STATIC,
	SCROLLBAR,
	BUTTON,
	LISTBOX,
	COMBOBOX,
	NUMBER_OF_WINDOWTYPES
} WINDOW_ID;

// Error codes, local to this file...
#define DUPLICATE_INSTANCE -1
#define FAILED_TO_CREATE_WINDOW -2
#define DISPLAY_MISMATCH -3
#define INIT_FAILED -4
#define FAILED_TO_CREATE_SYSTEM -5
#define LOCK_FAILURE -6
#define UNLOCK_FAILURE -7
#define BLT_FAILURE -8
#define FAILED_TO_ALLOCATE_PORT -9
#define USER_INIT_FAILED -10
#endif

extern int32_t helpTextID;
extern int32_t helpTextHeaderID;

class aBaseObject
{
public:
	virtual void render(void) {}
	virtual void update(void) {}
};

// base class aObject definition
class aObject : public aBaseObject
{

public:
	aObject(void);
	virtual ~aObject(void);
	aObject(const aObject& src);
	aObject& operator=(const aObject& src);

	virtual int32_t init(int32_t xPos, int32_t yPos, int32_t w, int32_t h);
	virtual void destroy(void);

	float width(void) const;
	float height(void) const;
	float x(void) const;
	float y(void) const;

	virtual void moveTo(int32_t xPos, int32_t yPos);
	virtual void moveToNoRecurse(int32_t xPos, int32_t yPos);

	void resize(int32_t w, int32_t h);
	void addChild(aObject* c);
	void removeChild(aObject* c);
	void setParent(aObject* p);

	void setTexture(PCSTR fileName);
	void setTexture(uint32_t newHandle);
	void setUVs(float u1, float v1, float u2, float v2);
	void setColor(uint32_t color, bool bRecurse = 0); // color the vertices

	void init(FitIniFile* file, PCSTR block, uint32_t neverFlush = 0); // for statics

	aObject* getParent(void)
	{
		return pParent; // No Need for this to be virtual!!!!! Can now check if
						// object has been deleted without crashing!
	}

	int32_t numberOfChildren(void) const;
	int32_t globalX(void) const;
	int32_t globalY(void) const;
	int32_t globalLeft(void) const { return globalX(void); }
	int32_t globalTop(void) const { return globalY(void); }
	int32_t globalRight(void) const;
	int32_t globalBottom(void) const;

	virtual aObject* findObject(int32_t xPos, int32_t yPos);
	virtual int32_t handleMessage(uint32_t, uint32_t) { return 0; }
	virtual bool pointInside(int32_t xPos, int32_t yPos) const;
	bool rectIntersect(int32_t top, int32_t left, int32_t bottom, int32_t right) const;
	bool rectIntersect(const RECT& testRect) const;

	aObject* children(void);
	aObject* child(int32_t w);

	virtual void render(void);
	virtual void render(int32_t x, int32_t y);
	virtual void update(void);

	COLORREF getColor(void) { return location[0].argb; }

	void showGUIWindow(bool show) { showWindow = show; }
	bool isShowing(void) const { return showWindow; }

	void FillBox(int16_t left, int16_t top, int16_t bottom, int16_t right, char color);
	void SetBit(int32_t xpos, int32_t ypos, char value);
	void removeAllChildren(bool bDelete = 0);
	virtual void move(float offsetX, float offsetY);
	virtual void moveNoRecurse(float offsetX, float offsetY);

	void setFileWidth(float newWidth) { fileWidth = newWidth; }
	int32_t getID(void) const { return ID; }
	void setID(int32_t newID) { ID = newID; }

	void setHelpID(int32_t newID) { helpID = newID; }
	int32_t getHelpID(void) const { return helpID; }

	float left(void) { return x(void); }
	float top(void) { return y(void); }
	float right(void) { return x() + width(void); }

	float bottom(void) { return y() + height(void); }

protected:
	gos_VERTEX location[4];

	uint32_t textureHandle;
	float fileWidth;
	bool showWindow;

	aObject* pChildren[MAX_CHILDREN];
	size_t nNumberOfChildren;
	aObject* pParent;

	uint32_t ID;

	void copyData(const aObject& src);

	int32_t helpHeader;
	uint32_t helpID;
};

// class aRect : public aBaseObject
/* It may seem wasteful to derive from aObject instead of aBaseObject, but an
aRect needs to able to be a child of an aObject. Perhaps bounding box and
parent/child support should be part of aBaseObject. */
class aRect : public aObject
{
public:
	aRect(void);
	virtual ~aRect(void) {}

	virtual void render(void);
	virtual void render(int32_t x, int32_t y);

	void init(FitIniFile* file, PCSTR blockName);

	RECT getGUI_RECT(void);
	RECT getGlobalGUI_RECT(void);

	bool bOutline;
};

class aText : public aObject
{
public:
	aText(void);
	aText(const aText& src);
	aText& operator=(const aText& src);

	virtual ~aText(void);

	virtual void render(void);
	virtual void render(int32_t x, int32_t y);
	void init(FitIniFile* file, PCSTR header);

	void setText(const std::string& text);
	void setText(int32_t resID);

	std::string text;
	int32_t alignment; // left, right, ala GOS
	aFont font;

	virtual bool pointInside(int32_t xPos, int32_t yPos) const;

private:
	void CopyData(const aText& src);
};
} // namespace mechgui

#endif // ASYSTEM_H
