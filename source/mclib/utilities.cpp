#define UTILITIES_CPP
/*************************************************************************************************\
Utilities.cpp			: Implementation of the Utilities component.
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//
\*************************************************************************************************/
#include "utilities.h"

#include "txmmgr.h"
#include "IniFile.h"
#include <mclib.h>

//#pragma warning(disable:4514)

void drawRect(const RECT& area, uint32_t color)
{
	if (color & 0xff000000)
	{
		RECT clientArea = area;
		if (clientArea.left < 0)
			clientArea.left = 0;
		if (clientArea.right < 0)
			clientArea.right = 0;
		if (clientArea.top < 0)
			clientArea.top = 0;
		if (clientArea.bottom < 0)
			clientArea.bottom = 0;
		if (clientArea.left >= Environment.screenWidth)
			clientArea.left = Environment.screenWidth;
		if (clientArea.right >= Environment.screenWidth)
			clientArea.right = Environment.screenWidth;
		if (clientArea.top >= Environment.screenHeight)
			clientArea.top = Environment.screenHeight;
		if (clientArea.bottom >= Environment.screenHeight)
			clientArea.bottom = Environment.screenHeight;
		gos_VERTEX v[4];
		memset(v, 0, sizeof(v));
		v[0].rhw = v[1].rhw = v[2].rhw = v[3].rhw = 1.0;
		v[0].argb = v[1].argb = v[2].argb = v[3].argb = color;
		v[0].x										  = (float)clientArea.left;
		v[0].y										  = (float)clientArea.top;
		v[1].x										  = (float)clientArea.left;
		v[1].y = (float)clientArea.bottom;
		v[2].x = (float)clientArea.right;
		v[2].y = (float)clientArea.bottom;
		v[3].x = (float)clientArea.right;
		v[3].y = (float)clientArea.top;
		gos_SetRenderState(gos_State_Texture, 0);
		gos_SetRenderState(gos_State_ZWrite, 0);
		gos_SetRenderState(gos_State_ZCompare, 0);
		gos_SetRenderState(gos_State_AlphaMode, gos_Alpha_AlphaInvAlpha);
		gos_DrawQuads(v, 4);
	}
}

void drawEmptyRect(const RECT& area, uint32_t leftTopBorderColor,
	uint32_t rightBottomBorderColor)
{
	gos_VERTEX v[4];
	memset(v, 0, sizeof(v));
	v[0].rhw = v[1].rhw = v[2].rhw = v[3].rhw = 1.0;
	RECT clientArea							  = area;
	if ((clientArea.left < 0 && clientArea.right < 0) ||
		(clientArea.right > Environment.screenWidth &&
			clientArea.left > Environment.screenWidth) ||
		(clientArea.top < 0 && clientArea.bottom < 0) ||
		(clientArea.top > Environment.screenHeight &&
			clientArea.bottom > Environment.screenHeight))
		return;
	if (clientArea.left < 0)
		clientArea.left = 0;
	if (clientArea.right < 0)
		clientArea.right = 0;
	if (clientArea.top < 0)
		clientArea.top = 0;
	if (clientArea.bottom < 0)
		clientArea.bottom = 0;
	if (clientArea.left >= Environment.screenWidth - 1)
		clientArea.left = Environment.screenWidth - 1;
	if (clientArea.right >= Environment.screenWidth - 1)
		clientArea.right = Environment.screenWidth - 1;
	if (clientArea.top >= Environment.screenHeight - 1)
		clientArea.top = Environment.screenHeight - 1;
	if (clientArea.bottom >= Environment.screenHeight - 1)
		clientArea.bottom = Environment.screenHeight - 1;
	gos_SetRenderState(gos_State_Texture, 0);
	gos_SetRenderState(gos_State_ZWrite, 0);
	gos_SetRenderState(gos_State_ZCompare, 0);
	gos_SetRenderState(gos_State_AlphaMode, gos_Alpha_AlphaInvAlpha);
	v[0].x	= (float)(clientArea.left);
	v[0].y	= (float)(clientArea.top);
	v[0].argb = leftTopBorderColor;
	v[1].x	= (float)(clientArea.right);
	v[1].y	= (float)(clientArea.top);
	v[1].argb = leftTopBorderColor;
	v[2]	  = v[0];
	v[3].x	= (float)(clientArea.left);
	v[3].y	= (float)(clientArea.bottom);
	v[3].argb = leftTopBorderColor;
	gos_DrawLines(v, 4);
	v[0].x	= (float)(clientArea.right);
	v[0].y	= (float)(clientArea.top);
	v[0].argb = rightBottomBorderColor;
	v[1].x	= (float)(clientArea.right);
	v[1].y	= (float)(clientArea.bottom);
	v[1].argb = rightBottomBorderColor;
	v[3]	  = v[1];
	v[3].x += 1;
	v[2].x	= (float)(clientArea.left);
	v[2].y	= (float)(clientArea.bottom) + .1 / 256.f;
	v[2].argb = rightBottomBorderColor;
	gos_DrawLines(v, 4);
}

// STATIC STUFF

StaticInfo::~StaticInfo()
{
	if (mcTextureManager)
	{
		uint32_t gosID = mcTextureManager->get_gosTextureHandle(textureHandle);
		mcTextureManager->removeTexture(gosID);
	}
}

void StaticInfo::render()
{
	uint32_t gosID = mcTextureManager->get_gosTextureHandle(textureHandle);
	gos_SetRenderState(gos_State_Texture, gosID);
	gos_SetRenderState(gos_State_Filter, gos_FilterNone);
	gos_SetRenderState(gos_State_AlphaMode, gos_Alpha_AlphaInvAlpha);
	gos_SetRenderState(gos_State_Clipping, 1);
	gos_DrawQuads(location, 4);
}

void StaticInfo::showGUIWindow(bool bShow)
{
	int32_t mask = bShow ? 0xff000000 : 0x00ffffff;
	for (size_t i = 0; i < 4; i++)
	{
		if (bShow)
			location[i].argb |= mask;
		else
			location[i].argb &= mask;
	}
}

bool StaticInfo::isInside(int32_t mouseX, int32_t mouseY)
{
	if ((location[0].x) <= mouseX && location[3].x >= mouseX &&
		location[0].y <= mouseY && location[1].y >= mouseY)
		return true;
	return false;
}

// Fills the buffer with the bitmap data.
// Used by new mouse cursor draw routines.
void StaticInfo::getData(puint8_t buffer)
{
	if ((vHeight > 32) || (uWidth > 32))
	{
		memset(buffer, 0, sizeof(uint32_t) * 32 * 32);
		return;
	}
	uint32_t gosID = mcTextureManager->get_gosTextureHandle(textureHandle);
	if (gosID)
	{
		TEXTUREPTR textureData;
		gos_LockTexture(gosID, 0, 0, &textureData);
		uint32_t* bufMem = (uint32_t*)buffer;
		uint32_t localU  = location[0].u * textureData.Width;
		uint32_t localV  = location[0].v * textureData.Width;
		// Make sure we don't fall off of the texture!
		if ((localU > textureData.Width) ||
			((localU + uWidth) > textureData.Width) ||
			(localV > textureData.Width) ||
			((localV + vHeight) > textureData.Width))
		{
			memset(buffer, 0, sizeof(uint32_t) * 32 * 32);
			gos_UnLockTexture(gosID);
			return;
		}
		for (size_t y = 0; y < vHeight; y++)
		{
			uint32_t* textureMemory =
				textureData.pTexture +
				(localU + ((localV + y) * textureData.Width));
			for (size_t x = 0; x < uWidth; x++)
			{
				*bufMem = *textureMemory;
				bufMem++;
				textureMemory++;
			}
		}
		gos_UnLockTexture(gosID);
	}
}

void StaticInfo::init(FitIniFile& file, PSTR blockName, int32_t hiResOffsetX,
	int32_t hiResOffsetY, uint32_t neverFlush)
{
	memset(location, 0, sizeof(location));
	char fileName[256];
	textureHandle = 0;
	textureWidth  = 0;
	if (NO_ERROR != file.seekBlock(blockName))
	{
		char errBuffer[256];
		sprintf(errBuffer, "couldn't find static block %s", blockName);
		Assert(0, 0, errBuffer);
		return;
	}
	int32_t x, y, width, height;
	file.readIdLong("XLocation", x);
	file.readIdLong("YLocation", y);
	x += hiResOffsetX;
	y += hiResOffsetY;
	file.readIdLong("Width", width);
	file.readIdLong("Height", height);
	file.readIdString("FileName", fileName, 32);
	if (!textureHandle)
	{
		FullPathFileName fullPath;
		_strlwr(fileName);
		fullPath.init(artPath, fileName, ".tga");
		int32_t ID = mcTextureManager->loadTexture(
			fullPath, gos_Texture_Alpha, 0, 0, 0x2);
		textureHandle  = ID;
		uint32_t gosID = mcTextureManager->get_gosTextureHandle(ID);
		TEXTUREPTR textureData;
		gos_LockTexture(gosID, 0, 0, &textureData);
		textureWidth = textureData.Width;
		gos_UnLockTexture(gosID);
	}
	bool bRotated = 0;
	file.readIdLong("UNormal", u);
	file.readIdLong("VNormal", v);
	file.readIdLong("UWidth", uWidth);
	file.readIdLong("VHeight", vHeight);
	file.readIdBoolean("texturesRotated", bRotated);
	for (size_t k = 0; k < 4; k++)
	{
		location[k].argb = 0xffffffff;
		location[k].frgb = 0;
		location[k].x	= x;
		location[k].y	= y;
		location[k].z	= 0.f;
		location[k].rhw  = .5;
		location[k].u =
			(float)u / (float)textureWidth + (.1f / (float)textureWidth);
		location[k].v =
			(float)v / (float)textureWidth + (.1f / (float)textureWidth);
	}
	location[3].x = location[2].x = x + width;
	location[2].y = location[1].y = y + height;
	location[2].u				  = location[3].u =
		((float)(u + uWidth)) / ((float)textureWidth) +
		(.1f / (float)textureWidth);
	location[1].v = location[2].v =
		((float)(v + vHeight)) / ((float)textureWidth) +
		(.1f / (float)textureWidth);
	if (bRotated)
	{
		location[0].u =
			(u + uWidth) / (float)textureWidth + (.1f / (float)textureWidth);
		;
		location[1].u = u / (float)textureWidth + (.1f / (float)textureWidth);
		;
		location[2].u = u / (float)textureWidth + (.1f / (float)textureWidth);
		location[3].u =
			(u + uWidth) / (float)textureWidth + (.1f / (float)textureWidth);
		location[0].v = v / (float)textureWidth + (.1f / (float)textureWidth);
		;
		location[1].v = v / (float)textureWidth + (.1f / (float)textureWidth);
		;
		location[2].v =
			(v + vHeight) / (float)textureWidth + (.1f / (float)textureWidth);
		;
		location[3].v =
			(v + vHeight) / (float)textureWidth + (.1f / (float)textureWidth);
		;
	}
}

void StaticInfo::setLocation(float newX, float newY)
{
	float height  = location[1].y - location[0].y;
	float width   = location[3].x - location[0].x;
	location[0].x = location[1].x = newX;
	location[2].x = location[3].x = newX + width;
	location[0].y = location[3].y = newY;
	location[1].y = location[2].y = newY + height;
}

void StaticInfo::move(float deltaX, float deltaY)
{
	for (size_t i = 0; i < 4; i++)
	{
		location[i].x += deltaX;
		location[i].y += deltaY;
	}
}

void StaticInfo::setColor(int32_t newColor)
{
	for (size_t i = 0; i < 4; i++)
		location[i].argb = newColor;
}

void StaticInfo::setNewUVs(float uLeft, float vTop, float uRight, float vBottom)
{
	location[0].u = location[1].u = uLeft;
	location[2].u = location[3].u = uRight;
	location[0].v = location[3].v = vTop;
	location[1].v = location[2].v = vBottom;
}

void drawShadowText(int32_t colorTop, int32_t colorShadow, HGOSFONT3D font,
	int32_t left, int32_t top, bool proportional, PCSTR text, bool bold,
	float scale)
{
	drawShadowText(colorTop, colorShadow, font, left, top, proportional, text,
		bold, scale, -1, 1);
}

void drawShadowText(int32_t colorTop, int32_t colorShadow, HGOSFONT3D font,
	int32_t left, int32_t top, bool proportional, PCSTR text, bool bold,
	float scale, int32_t xOffset, int32_t yOffset)
{
	gos_TextSetAttributes(
		font, colorShadow, scale, false, proportional, bold, false, 0);
	gos_TextSetRegion(0, 0, Environment.screenWidth, Environment.screenHeight);
	gos_TextSetPosition(left + xOffset, top + yOffset);
	gos_TextDraw(text);
	gos_TextSetAttributes(
		font, colorTop, scale, false, proportional, bold, false, 0);
	gos_TextSetPosition(left, top);
	gos_TextDraw(text);
}

void drawShadowText(int32_t colorTop, int32_t colorShadow, HGOSFONT3D font,
	int32_t left, int32_t top, int32_t right, int32_t bottom, bool proportional,
	PCSTR text, bool bold, float scale, int32_t xOffset, int32_t yOffset)
{
	gos_TextSetAttributes(
		font, colorShadow, scale, true, proportional, bold, false, 2);
	gos_TextSetRegion(
		left + xOffset, top + yOffset, right + xOffset, bottom + yOffset);
	gos_TextSetPosition(left + xOffset, top + yOffset);
	gos_TextDraw(text);
	gos_TextSetAttributes(
		font, colorTop, scale, true, proportional, bold, false, 2);
	gos_TextSetRegion(left, top, right, bottom);
	gos_TextSetPosition(left, top);
	gos_TextDraw(text);
}

int32_t interpolateColor(int32_t color1, int32_t color2, float percent)
{
	int32_t color = 0xffffffff;
	if (percent > 1.0)
		color = color2;
	else if (percent < 0.0)
		color = color1;
	else
	{
		int32_t alphaMin = (color1 >> 24) & 0xff;
		int32_t alphaMax = (color2 >> 24) & 0xff;
		int32_t newAlpha =
			(float)alphaMin + ((float)(alphaMax - alphaMin)) * percent;
		int32_t redMin   = (color1 & 0x00ff0000) >> 16;
		int32_t redMax   = (color2 & 0x00ff0000) >> 16;
		int32_t newRed   = (float)redMin + ((float)(redMax - redMin)) * percent;
		int32_t greenMin = (color1 & 0x0000ff00) >> 8;
		int32_t greenMax = (color2 & 0x0000ff00) >> 8;
		int32_t newGreen =
			(float)greenMin + ((float)(greenMax - greenMin)) * percent;
		int32_t blueMin = color1 & 0x000000ff;
		int32_t blueMax = color2 & 0x000000ff;
		int32_t newBlue =
			(float)blueMin + ((float)(blueMax - blueMin)) * percent;
		color = newBlue + (newGreen << 8) + (newRed << 16) + (newAlpha << 24);
	}
	return color;
}
