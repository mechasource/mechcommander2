//===========================================================================//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

// stuff I need all over the place
#ifndef UTILITIES_H
#define UTILITIES_H

//#include "gameos.hpp"
//#include <string.h>

class FitIniFile;

class StaticInfo
{
public:
	StaticInfo() {}
	~StaticInfo(void);

	void init(FitIniFile& file, const std::wstring_view& blockName, int32_t hiResOffsetX = 0, int32_t hiResOffsetY = 0,
		uint32_t neverFlush = 0);
	void render(void);
	bool isInside(int32_t mouseX, int32_t mouseY);

	void setLocation(float newX, float newY);
	void move(float deltaX, float deltaY);

	void setNewUVs(float uLeft, float vTop, float uRight, float vBottom);

	float width() { return location[2].x - location[0].x; }
	float height() { return location[2].y - location[0].y; }

	void getData(uint8_t* buffer);

	void showGUIWindow(bool doshow);

	void setcolour(int32_t newcolour);

	uint32_t textureHandle;
	gos_VERTEX location[4];
	int32_t u, v, uwidth, vheight;

	uint32_t texturewidth; // textures are square
};

void
drawEmptyRect(const RECT& rect, uint32_t leftBordercolour = 0xffffffff,
	uint32_t rightBordercolour = 0xff000000);

void
drawRect(const RECT& rect, uint32_t color);

void
drawShadowText(int32_t colorTop, int32_t colorShadow, HGOSFONT3D font, int32_t left,
	int32_t top, bool proportional, const std::wstring_view& text, bool bBold, float scale);

void
drawShadowText(int32_t colorTop, int32_t colorShadow, HGOSFONT3D font, int32_t left,
	int32_t top, bool proportional, const std::wstring_view& text, bool bold, float scale, int32_t xOffset,
	int32_t yOffset);

void
drawShadowText(int32_t colorTop, int32_t colorShadow, HGOSFONT3D font, int32_t left,
	int32_t top, int32_t right, int32_t bottom, bool proportional, const std::wstring_view& text, bool bold,
	float scale, int32_t xOffset, int32_t yOffset);

uint32_t
interpolatecolour(uint32_t color1, uint32_t color2, float percent);

inline COLORREF
reverseRGB(COLORREF oldVal)
{
	return ((oldVal & 0xff000000) | ((oldVal & 0xff0000) >> 16) | (oldVal & 0xff00) | ((oldVal & 0xff) << 16));
}

//-------------------------------------------------------
// Replaces the WINDOWS version because it is a lie!
// There are cases where windows does NOT append a null.
// THANKS!
//
// Replace with GOS string Resource get when available
// Replaced.  Andy wants us to call everytime.  Will try and see if practical.
extern uint32_t gosResourceHandle;

#if CONSIDERED_OBSOLETE
inline size_t
cLoadString(uint32_t uID, // resource identifier
	const std::wstring_view& lpBuffer, // pointer to buffer for resource
	size_t nBufferMax, // size of buffer
	uint32_t handle = gosResourceHandle)
{
	memset(lpBuffer, 0, nBufferMax);
	const std::wstring_view& tmpBuffer = gos_GetResourceString(handle, uID);
	size_t stringLength = strlen(tmpBuffer);
	if (stringLength >= nBufferMax)
		STOP(("string too int32_t for buffer.  string Id %d, bufferLen %d, StringLen %d",
			uID, nBufferMax, stringLength));
	memcpy(lpBuffer, tmpBuffer, stringLength);
	return stringLength;
}

#else

inline const std::wstring_view&
cLoadString(uint32_t uID)
{
	return gos_GetResourceString(gosResourceHandle, uID);
}
#endif

#endif