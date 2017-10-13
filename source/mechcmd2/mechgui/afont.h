//===========================================================================//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#pragma once

#ifndef AFONT_H
#define AFONT_H

//#include <gameos.hpp>

namespace mechgui
{

// Error codes, local to this file...
typedef enum __afont_constants
{
	FONT_NOT_LOADED = -3,
};

class aFont
{
  public:
	aFont(void);
	~aFont(void);
	aFont(const aFont& src);
	aFont& operator=(const aFont& src);

	int32_t init(PCSTR fontName);
	int32_t init(int32_t resourceID);
	void destroy(void);
	void render(PCSTR text, int32_t XPos, int32_t YPos, int32_t areaWidth,
		int32_t areaHeight, uint32_t color, bool bBold, int32_t alignment);

	int32_t load(PCSTR fontName);
	uint32_t height(void) const;
	uint32_t width(PCSTR st) const;
	uint32_t height(PCSTR st, int32_t areaWidth) const;
	void getSize(uint32_t& width, uint32_t& height, PCSTR pText);

	static HGOSFONT3D loadFont(int32_t resourceID, int32_t& size);
	int32_t getSize() { return size; }

	HGOSFONT3D getTempHandle()
	{
		return gosFont; // don't you dare hang on to this
	}
	int32_t getFontID() { return resID; }

  private:
	HGOSFONT3D gosFont;
	int32_t resID;
	char fontName[64]; // so we can copy fonts
	int32_t size;
	void copyData(const aFont& src);
};

#endif
