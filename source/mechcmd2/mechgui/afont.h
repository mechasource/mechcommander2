//===========================================================================//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#pragma once

#ifndef AFONT_H
#define AFONT_H

////#include "gameos.hpp"

namespace mechgui
{

// Error codes, local to this file...
enum __afont_constants
{
	FONT_NOT_LOADED = -3,
};

class aFont
{
private:
	HGOSFONT3D gosFont;
	int32_t resID;
	wchar_t fontName[64]; // so we can copy fonts
	int32_t size;
	void copyData(const aFont& src);

public:
	aFont(void)
	{
		gosFont = 0;
		fontName[0] = 0;
		resID = 0;
		size = 1;
	}

	~aFont(void);
	aFont(const aFont& src);
	aFont& operator=(const aFont& src);

	int32_t init(std::wstring_view fontName);
	int32_t init(int32_t resourceID);
	void destroy(void);
	void render(std::wstring_view text, int32_t XPos, int32_t YPos, int32_t areawidth, int32_t areaheight,
		uint32_t color, bool bBold, int32_t alignment);

	int32_t load(std::wstring_view fontName);
	uint32_t height(void) const;
	uint32_t width(std::wstring_view st) const;
	uint32_t height(std::wstring_view st, int32_t areawidth) const;
	void getSize(uint32_t& width, uint32_t& height, std::wstring_view pText);

	static HGOSFONT3D loadFont(int32_t resourceID, int32_t& size);
	int32_t getSize()
	{
		return size;
	}

	HGOSFONT3D getTempHandle()
	{
		return gosFont; // don't you dare hang on to this
	}
	int32_t getFontID()
	{
		return resID;
	}
};

#endif
