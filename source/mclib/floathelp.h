//-----------------------------------------------------------------------------------------
//
// Floating Help class.  Used for tool tips, building/mech/vehicle IDs.
// Basically draws itself AFTER everything else since gos_Font"3D" has no Z
// Depth!
//
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#ifndef FLOATHELP_H
#define FLOATHELP_H
//--------------------------
// Include Files

//#include "dbasegui.h"
////#include "gameos.hpp"
//#include "stuff/stuff.h"

extern HGOSFONT3D gosFontHandle;
extern float gosFontScale;

#define MAX_FLOAT_HELPS 100 // Used for sensor information, too!
//-----------------------------------------------------------------------------------------
// Classes
class FloatHelp
{
protected:
	wchar_t text[2048]; // Last person to set this displays the font.
	Stuff::Vector4D screenPos; // x,y are left and top.  z,w are width and height.
	uint32_t foregroundcolour; // colour in aRGB Format.
	uint32_t backgroundcolour; // colour in aRGB Format.
	float scale; // Scale.  1.0f is normal.
	bool proportional; // if false, spacing is equal for each letter.
	bool bold; // if true, draws bold.
	bool italic; // if true, draws italic.
	bool wordWrap; // if true, wraps word.

	static uint32_t currentFloatHelp; // How many of them are we using at present
	static FloatHelp* floatHelps; // POinters to all of them.

public:
	FloatHelp(int32_t maxHelps)
	{
		init(maxHelps);
		text[0] = 0;
		screenPos.x = screenPos.y = screenPos.z = screenPos.w = 0.0f;
		foregroundcolour = SD_WHITE;
		backgroundcolour = SD_BLACK;
		scale = 1.0f;
		proportional = true;
		bold = italic = wordWrap = false;
	}

	~FloatHelp(void)
	{
		destroy(void);
	}

	void init(int32_t maxHelps);

	void destroy(void);

	static void resetAll(void);

	static void renderAll(void);

	static void setFloatHelp(std::wstring_view txt, Stuff::Vector4D screenPos, uint32_t fClr, uint32_t bClr,
		float scl, bool proportional, bool bold, bool italic, bool wordWrap);

	static void getTextStringLength(std::wstring_view text, uint32_t fcolour, float scl, bool proportional,
		bool bold, bool italic, bool wordWrap, uint32_t& width, uint32_t& height);

protected:
	void setHelpText(std::wstring_view txt)
	{
		if (strlen(txt) < 2048)
		{
			strcpy(text, txt);
		}
		else
		{
			text[0] = 0;
		}
	}

	void setScreenPos(Stuff::Vector4D pos)
	{
		screenPos = pos;
	}
	void setForegroundcolour(uint32_t clr)
	{
		foregroundcolour = clr;
	}
	void setBackgroundcolour(uint32_t clr)
	{
		backgroundcolour = clr;
	}
	void setScale(float scl)
	{
		scale = scl;
	}
	void setProportional(bool flag)
	{
		proportional = flag;
	}
	void setBold(bool flag)
	{
		bold = flag;
	}
	void setItalic(bool flag)
	{
		italic = flag;
	}
	void setWordWrap(bool flag)
	{
		wordWrap = flag;
	}
	void reset(void)
	{
		// Call every frame.  Otherwise floating help keeps floating!
		text[0] = 0;
	}

	void render(void)
	{
		if (text[0])
		{
			// must use global scale, incase of True Type fonts.
			gos_TextSetAttributes(
				gosFontHandle, foregroundcolour, gosFontScale, wordWrap, proportional, bold, italic);
			// gos_TextDrawBackground ((int32_t)screenPos.x,
			// (int32_t)screenPos.y, (int32_t)(screenPos.x+screenPos.z),
			// (int32_t)(screenPos.y+screenPos.w), SD_BLACK);
			gos_TextSetPosition((int32_t)screenPos.x, (int32_t)screenPos.y);
			gos_TextDraw(text);
		}
	}
};

// typedef FloatHelp* FloatHelpPtr;

//-----------------------------------------------------------------------------------------
#endif