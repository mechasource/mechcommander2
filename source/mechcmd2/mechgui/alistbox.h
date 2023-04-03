//===========================================================================//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#pragma once

#ifndef ALISTBOX_H
#define ALISTBOX_H

#include "mechgui/ascroll.h"
#include "mechgui/aedit.h"
#include "mechgui/afont.h"

#include "estring.h"

namespace mechgui
{

#define MAX_LIST_ITEMS 128
#define TOO_MANY_ITEMS 0xEEEE0001
#define ITEM_OUT_OF_RANGE 0xEEEE0002

class aListItem : public aObject
{
public:
	enum State
	{
		ENABLED = 0,
		SELECTED,
		HIGHLITE,
		PRESSED_HIGHLITE,
		DISABLED,

	};

	virtual void update()
	{
		aObject::update(void);
	}
	virtual void render()
	{
		aObject::render(void);
	}

	void setState(int32_t newState)
	{
		state = (State)newState;
	}
	State getState()
	{
		return state;
	}

	void select()
	{
		state = SELECTED;
	}
	void disable()
	{
		state = DISABLED;
	}
	void highlite()
	{
		state = HIGHLITE;
	}
	void deselect()
	{
		state = state == DISABLED ? DISABLED : ENABLED;
	}

	virtual bool isChecked()
	{
		return false;
	}
	virtual void setCheck(bool) { }

protected:
	State state;
};

class aTextListItem : public aListItem
{
public:
	aTextListItem(HGOSFONT3D newFont); // need the font, so we can determine the size
	aTextListItem(const aFont& newFont);
	aTextListItem(int32_t fontResID);

	virtual ~aTextListItem(void);

	void setText(std::wstring_view text);
	void setText(int32_t resID);
	std::wstring_view getText(void) const;
	void sizeToText(void);

	void init(FitIniFile& file, std::wstring_view blockname = "Text0");

	virtual void render(void);
	void setAlignment(int32_t newA)
	{
		alignment = newA;
	}

	void forceToTop(bool bForce)
	{
		bForceToTop = bForce;
	}

protected:
	aTextListItem() { }
	void init(int32_t fontResID);

	aFont font;
	std::wstring_view text;
	int32_t alignment;

	bool bForceToTop;
};

class aAnimTextListItem : public aTextListItem
{

public:
	aAnimTextListItem(HGOSFONT3D newFont)
		: aTextListItem(newFont)
	{
	}
	aAnimTextListItem(const aFont& newFont)
		: aTextListItem(newFont)
	{
	}
	aAnimTextListItem(int32_t fontResID)
		: aTextListItem(fontResID)
	{
	}

	aAnimTextListItem(const aAnimTextListItem& src);
	aAnimTextListItem& operator=(const aAnimTextListItem& src);

	void init(FitIniFile& file, std::wstring_view blockname = "Text0");
	virtual void render(void);
	virtual void update(void);

protected:
	aAnimGroup animInfo;
	void CopyData(const aAnimTextListItem& src);
};

class aLocalizedListItem : public aAnimTextListItem
{
public:
	aLocalizedListItem(void);
	virtual int32_t init(FitIniFile* file, std::wstring_view blockname);
	virtual void render(void);

	void setHiddenText(std::wstring_view pText)
	{
		hiddenText = pText;
	}
	std::wstring_view getHiddenText(void) const
	{
		return hiddenText;
	}

protected:
	std::wstring_view hiddenText;
};

class aListBox : public aObject
{
public:
	aListBox(void);

	virtual int32_t init(int32_t xPos, int32_t yPos, int32_t w, int32_t h);
	void init(FitIniFile* file, std::wstring_view blockname);

	virtual void destroy(void);
	virtual void render(void);
	virtual void update(void);
	virtual int32_t handleMessage(uint32_t message, uint32_t who);
	virtual void resize(int32_t w, int32_t h);

	virtual int32_t AddItem(aListItem* itemString);
	virtual int32_t InsertItem(aListItem* itemString, int32_t where);
	virtual int32_t RemoveItem(aListItem* itemString, bool bDelete);
	int32_t ChangeItemString(int16_t itemNumber, std::wstring_view newString);
	int32_t GetSelectedItem(void)
	{
		return itemSelected;
	};
	int32_t GetCheckedItem(void) const;
	int32_t SelectItem(int32_t itemNumber);
	bool IsScrollActive(void)
	{
		return scrollActive;
	};
	int32_t ActivateScrollbar(void);

	aListItem* GetItem(int32_t itemNumber);
	int32_t GetItemCount()
	{
		return itemCount;
	}

	void removeAllItems(bool bDelete);

	void setSpaceBetweenItems(int32_t newSpace)
	{
		skipAmount = newSpace;
	}
	int32_t getSpaceBetweenItems()
	{
		return skipAmount;
	}

	void setSingleCheck(bool checkOnlyOne)
	{
		singleCheck = checkOnlyOne;
	}

	virtual void move(float offsetX, float offsetY);
	void setScrollPos(int32_t pos);

	int32_t getScrollBarwidth(void);

	void setOrange(bool bOrange);
	void enableAllItems(void);

	void setPressFX(int32_t newFX)
	{
		clickSFX = newFX;
	}
	void setHighlightFX(int32_t newFX)
	{
		highlightSFX = newFX;
	}
	void setDisabledFX(int32_t newFX)
	{
		disabledSFX = newFX;
	}

	void setTopSkip(int32_t newSkip)
	{
		topSkip = newSkip;
	}

	bool pointInScrollBar(int32_t mouseX, int32_t mouseY);
	float getScrollPos()
	{
		return scrollBar ? scrollBar->GetScrollPos() : 0;
	}

protected:
	int32_t itemCount;
	int32_t itemSelected;
	int32_t skipAmount;

	aListItem* items[MAX_LIST_ITEMS];
	mcScrollBar* scrollBar;
	int32_t itemheight;
	bool scrollActive;
	bool singleCheck;

	int32_t clickSFX;
	int32_t highlightSFX;
	int32_t disabledSFX;
	int32_t topSkip;

	void scroll(int32_t amount);
};

/* Note that items in an aDropList items do not have to be aTextListItems,
whereas in an aComboBox they do (well, selectable items do).*/
class aDropList : public aObject
{
public:
	aDropList(void);

	virtual int32_t init(FitIniFile* file, std::wstring_view blockname);
	virtual void destroy(void);
	void specialDestroy(void);
	virtual void render(void);
	virtual void update(void);
	virtual int32_t handleMessage(uint32_t message, uint32_t who);
	virtual bool pointInside(int32_t xPos, int32_t yPos) const;
	// virtual void	resize(int32_t w, int32_t h);

	virtual int32_t AddItem(aListItem* itemString);
	int32_t SelectItem(int32_t item);

	aListBox& ListBox()
	{
		return listBox;
	}
	bool IsExpanded()
	{
		return listBox.isShowing(void);
	}
	void IsExpanded(bool isExpanded);
	void disable(bool bDisable)
	{
		if (bDisable)
			IsExpanded(0);
		expandButton.showGUIWindow(!bDisable);
		rects[0].showGUIWindow(!bDisable);
		rects[1].showGUIWindow(!bDisable);
	}

	int32_t AddItem(uint32_t textID, uint32_t color);
	int32_t AddItem(std::wstring_view text, uint32_t color);
	int32_t GetSelectedItem(void) const
	{
		return selectionindex;
	}

	aDropList& operator=(const aDropList&);

protected:
	aRect* rects;
	int32_t rectCount;
	aAnimButton expandButton;
	aListBox listBox;
	float listBoxMaxheight;
	int32_t selectionindex;
	aAnimTextListItem templateItem;
	aDropList(const aDropList&);

	int32_t textLeft;
	int32_t textTop;
};

class aComboBox : public aObject
{
public:
	aComboBox(void);
	~aComboBox() {};

	virtual int32_t init(FitIniFile* file, std::wstring_view blockname);
	virtual void destroy(void);
	virtual void render(void);
	virtual void update(void);
	virtual int32_t handleMessage(uint32_t message, uint32_t who);
	virtual bool pointInside(int32_t xPos, int32_t yPos) const;
	// virtual void	resize(int32_t w, int32_t h);

	virtual int32_t AddItem(aListItem* itemString);
	int32_t SelectItem(int32_t item);

	void setReadOnly(bool bReadOnly)
	{
		entry.setReadOnly(bReadOnly);
	}
	int32_t AddItem(uint32_t textID, uint32_t color);
	int32_t AddItem(std::wstring_view text, uint32_t color);
	int32_t GetSelectedItem(void) const
	{
		return selectionindex;
	}

	aComboBox& operator=(const aComboBox&);

	aEdit& EditBox()
	{
		return entry;
	}
	aListBox& ListBox()
	{
		return listBox;
	}

	void setFocus(bool bFocus)
	{
		EditBox().setFocus(bFocus);
	}

protected:
	aEdit entry;
	aRect* rects;
	int32_t rectCount;
	aAnimButton expandButton;
	aListBox listBox;
	float listBoxMaxheight;
	int32_t selectionindex;

	aComboBox(const aComboBox&);
	aAnimTextListItem templateItem;
};
} // namespace mechgui

#endif
