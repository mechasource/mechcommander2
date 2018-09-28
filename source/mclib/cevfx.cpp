//---------------------------------------------------------------------------
//
// cevfx.cpp - This file contains the code for the VFX Shape Element
//
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#include "stdinc.h"

#include "cevfx.h"
#include "scale.h"
#include "vport.h"
#include "vfx.h"
#include "heap.h"
#include "tgl.h"
#include "camera.h"
#include "clip.h"

extern void AG_shape_translate_transform(PANE* globalPane, PVOID shapeTable, int32_t frameNum,
	int32_t hotX, int32_t hotY, PVOID tempBuffer, int32_t reverse, int32_t scaleUp);
extern void AG_shape_transform(PANE* globalPane, PVOID shapeTable, int32_t frameNum, int32_t hotX,
	int32_t hotY, PVOID tempBuffer, int32_t reverse, int32_t scaleUp);
extern void AG_shape_lookaside(puint8_t table);
extern void AG_shape_draw(
	PANE* pane, PVOID shape_table, int32_t shape_number, int32_t hotX, int32_t hotY);
extern void AG_shape_translate_draw(
	PANE* pane, PVOID shape_table, int32_t shape_number, int32_t hotX, int32_t hotY);
// void memclear(PVOID Dest,size_t Length);
//---------------------------------------------------------------------------
// Static Globals
puint8_t shapeBuffer16  = nullptr;
puint8_t shapeBuffer32  = nullptr;
puint8_t shapeBuffer64  = nullptr;
puint8_t shapeBuffer128 = nullptr;
puint8_t shapeBuffer256 = nullptr;

//---------------------------------------------------------------------------
VFXElement::VFXElement(puint8_t _shape, int32_t _x, int32_t _y, int32_t frame, bool rev,
	puint8_t fTable, bool noScale, bool upScale)
	: Element(-_y)
{
	shapeTable  = _shape;
	x			= _x;
	y			= _y;
	frameNum	= frame;
	reverse		= rev;
	fadeTable   = fTable;
	noScaleDraw = noScale;
	scaleUp		= upScale;
	//-------------------------------------
	// Integrity Check here.
	int32_t result = VFX_shape_count(shapeTable);
	if (result <= frameNum)
	{
		frameNum = result - 1;
	}
#ifdef _DEBUG
	result		 = VFX_shape_bounds(shapeTable, frameNum);
	int32_t xMax = result >> 16;
	int32_t yMax = result & 0x0000ffff;
#define MAX_X 360
#define MAX_Y 360
	if ((yMax * xMax) >= (MAX_Y * MAX_X))
	{
		return;
	}
#endif
}

//---------------------------------------------------------------------------
VFXElement::VFXElement(puint8_t _shape, float _x, float _y, int32_t frame, bool rev,
	puint8_t fTable, bool noScale, bool upScale)
	: Element(-_y)
{
	shapeTable  = _shape;
	x			= _x;
	y			= _y;
	frameNum	= frame;
	reverse		= rev;
	fadeTable   = fTable;
	noScaleDraw = noScale;
	scaleUp		= upScale;
	//-------------------------------------
	// Integrity Check here.
#define MAX_X 360
#define MAX_Y 360
	int32_t result = VFX_shape_count(shapeTable);
	if (result <= frameNum)
	{
		frameNum = result - 1;
	}
#ifdef _DEBUG
	result		 = VFX_shape_bounds(shapeTable, frameNum);
	int32_t xMax = result >> 16;
	int32_t yMax = result & 0x0000ffff;
	if ((yMax * xMax) >= (MAX_Y * MAX_X))
	{
		return;
	}
#endif
}

extern puint8_t tempBuffer;
//---------------------------------------------------------------------------
void VFXElement::draw(void)
{
	if (!noScaleDraw)
		scaleDraw(shapeTable, frameNum, x, y, reverse, fadeTable, scaleUp);
	else
	{
		//----------------------------------------------------------------
		// Check if shape is actually valid.
		if ((*(pint32_t)shapeTable != *(pint32_t) "1.10"))
			return;
		if (!reverse)
		{
			if (!fadeTable)
				AG_shape_draw(globalPane, shapeTable, frameNum, x, y);
			else
			{
				AG_shape_lookaside(fadeTable);
				AG_shape_translate_draw(globalPane, shapeTable, frameNum, x, y);
			}
		}
		else // 45Pixel mechs dealt with here.
		{
			if (!tempBuffer)
				tempBuffer = (puint8_t)systemHeap->Malloc(MAX_X * MAX_Y);
			if (fadeTable)
			{
				AG_shape_lookaside(fadeTable);
				AG_shape_translate_transform(
					globalPane, shapeTable, frameNum, x, y, tempBuffer, reverse, TRUE);
			}
			else
			{
				AG_shape_transform(
					globalPane, shapeTable, frameNum, x, y, tempBuffer, reverse, TRUE);
			}
		}
	}
}

int32_t mechRGBLookup[56] = {
	(0x00) + (0x00 << 8) + (0x00 << 16) + (0x00 << 24), // 0
	(0x00) + (0x00 << 8) + (0x00 << 16) + (0x00 << 24), // 1
	(0x00) + (0x00 << 8) + (0x00 << 16) + (0x00 << 24), // 2
	(0x00) + (0x00 << 8) + (0x00 << 16) + (0x00 << 24), // 3
	(0x00) + (0x00 << 8) + (0x00 << 16) + (0x00 << 24), // 4
	(0x00) + (0x00 << 8) + (0x00 << 16) + (0x00 << 24), // 5
	(0x00) + (0x00 << 8) + (0x00 << 16) + (0x00 << 24), // 6
	(0x00) + (0x00 << 8) + (0x00 << 16) + (0x00 << 24), // 7
	(0x00) + (0x00 << 8) + (0x00 << 16) + (0x00 << 24), // 8
	(0x00) + (0x00 << 8) + (0x00 << 16) + (0x00 << 24), // 9
	(0x00) + (0x00 << 8) + (0x00 << 16) + (0x00 << 24), // 10
	(0x00) + (0x00 << 8) + (0x00 << 16) + (0x00 << 24), // 11
	(0x00) + (0x00 << 8) + (0x00 << 16) + (0x00 << 24), // 12
	(0x00) + (0x00 << 8) + (0x00 << 16) + (0x00 << 24), // 13
	(0x00) + (0x00 << 8) + (0x00 << 16) + (0x00 << 24), // 14
	(0x00) + (0x00 << 8) + (0x00 << 16) + (0x00 << 24), // 15

	(0x00) + (0x10 << 8) + (0x00 << 16) + (0xff << 24), // 16
	(0x00) + (0x20 << 8) + (0x00 << 16) + (0xff << 24), // 17
	(0x00) + (0x30 << 8) + (0x00 << 16) + (0xff << 24), // 18
	(0x00) + (0x40 << 8) + (0x00 << 16) + (0xff << 24), // 19
	(0x00) + (0x50 << 8) + (0x00 << 16) + (0xff << 24), // 20
	(0x00) + (0x60 << 8) + (0x00 << 16) + (0xff << 24), // 21
	(0x00) + (0x70 << 8) + (0x00 << 16) + (0xff << 24), // 22
	(0x00) + (0x80 << 8) + (0x00 << 16) + (0xff << 24), // 23
	(0x00) + (0x90 << 8) + (0x00 << 16) + (0xff << 24), // 24
	(0x00) + (0xA0 << 8) + (0x00 << 16) + (0xff << 24), // 25
	(0x00) + (0xB0 << 8) + (0x00 << 16) + (0xff << 24), // 26
	(0x00) + (0xC0 << 8) + (0x00 << 16) + (0xff << 24), // 27
	(0x00) + (0xD0 << 8) + (0x00 << 16) + (0xff << 24), // 28
	(0x00) + (0xE0 << 8) + (0x00 << 16) + (0xff << 24), // 29
	(0x00) + (0xF0 << 8) + (0x00 << 16) + (0xff << 24), // 30
	(0x00) + (0xFF << 8) + (0x00 << 16) + (0xff << 24), // 31

	(0x00) + (0x00 << 8) + (0x00 << 16) + (0x00 << 24), // 32
	(0x00) + (0x00 << 8) + (0x00 << 16) + (0x00 << 24), // 33
	(0x00) + (0x00 << 8) + (0x00 << 16) + (0x00 << 24), // 34
	(0x00) + (0x00 << 8) + (0x00 << 16) + (0x00 << 24), // 35
	(0x00) + (0x00 << 8) + (0x00 << 16) + (0x00 << 24), // 36
	(0x00) + (0x00 << 8) + (0x00 << 16) + (0x00 << 24), // 37
	(0x00) + (0x00 << 8) + (0x00 << 16) + (0x00 << 24), // 38
	(0x00) + (0x00 << 8) + (0x00 << 16) + (0x00 << 24), // 39

	(0x20) + (0x00 << 8) + (0x00 << 16) + (0xff << 24), // 40
	(0x40) + (0x00 << 8) + (0x00 << 16) + (0xff << 24), // 41
	(0x60) + (0x00 << 8) + (0x00 << 16) + (0xff << 24), // 42
	(0x80) + (0x00 << 8) + (0x00 << 16) + (0xff << 24), // 43
	(0xA0) + (0x00 << 8) + (0x00 << 16) + (0xff << 24), // 44
	(0xC0) + (0x00 << 8) + (0x00 << 16) + (0xff << 24), // 45
	(0xE0) + (0x00 << 8) + (0x00 << 16) + (0xff << 24), // 46
	(0xFF) + (0x00 << 8) + (0x00 << 16) + (0xff << 24), // 47

	(0x00) + (0x00 << 8) + (0x20 << 16) + (0xff << 24), // 48
	(0x00) + (0x00 << 8) + (0x40 << 16) + (0xff << 24), // 49
	(0x00) + (0x00 << 8) + (0x60 << 16) + (0xff << 24), // 50
	(0x00) + (0x00 << 8) + (0x80 << 16) + (0xff << 24), // 51
	(0x00) + (0x00 << 8) + (0xA0 << 16) + (0xff << 24), // 52
	(0x00) + (0x00 << 8) + (0xC0 << 16) + (0xff << 24), // 53
	(0x00) + (0x00 << 8) + (0xE0 << 16) + (0xff << 24), // 54
	(0x00) + (0x00 << 8) + (0xFF << 16) + (0xff << 24), // 55
};

int32_t mechRGBLookup2[56] = {
	(0x00) + (0x00 << 8) + (0x00 << 16) + (0x00 << 24), // 0
	(0x00) + (0x00 << 8) + (0x00 << 16) + (0x00 << 24), // 1
	(0x00) + (0x00 << 8) + (0x00 << 16) + (0x00 << 24), // 2
	(0x00) + (0x00 << 8) + (0x00 << 16) + (0x00 << 24), // 3
	(0x00) + (0x00 << 8) + (0x00 << 16) + (0x00 << 24), // 4
	(0x00) + (0x00 << 8) + (0x00 << 16) + (0x00 << 24), // 5
	(0x00) + (0x00 << 8) + (0x00 << 16) + (0x00 << 24), // 6
	(0x00) + (0x00 << 8) + (0x00 << 16) + (0x00 << 24), // 7
	(0x00) + (0x00 << 8) + (0x00 << 16) + (0x00 << 24), // 8
	(0x00) + (0x00 << 8) + (0x00 << 16) + (0x00 << 24), // 9
	(0x00) + (0x00 << 8) + (0x00 << 16) + (0x00 << 24), // 10
	(0x00) + (0x00 << 8) + (0x00 << 16) + (0x00 << 24), // 11
	(0x00) + (0x00 << 8) + (0x00 << 16) + (0x00 << 24), // 12
	(0x00) + (0x00 << 8) + (0x00 << 16) + (0x00 << 24), // 13
	(0x00) + (0x00 << 8) + (0x00 << 16) + (0x00 << 24), // 14
	(0x00) + (0x00 << 8) + (0x00 << 16) + (0x00 << 24), // 15

	(104) + (104 << 8) + (104 << 16) + (0xff << 24), // 16
	(112) + (112 << 8) + (112 << 16) + (0xff << 24), // 17
	(120) + (120 << 8) + (120 << 16) + (0xff << 24), // 18
	(128) + (128 << 8) + (128 << 16) + (0xff << 24), // 19
	(136) + (136 << 8) + (136 << 16) + (0xff << 24), // 20
	(144) + (144 << 8) + (144 << 16) + (0xff << 24), // 21
	(156) + (156 << 8) + (156 << 16) + (0xff << 24), // 22
	(168) + (168 << 8) + (168 << 16) + (0xff << 24), // 23
	(180) + (180 << 8) + (180 << 16) + (0xff << 24), // 24
	(196) + (196 << 8) + (196 << 16) + (0xff << 24), // 25
	(208) + (208 << 8) + (208 << 16) + (0xff << 24), // 26
	(224) + (224 << 8) + (224 << 16) + (0xff << 24), // 27
	(236) + (236 << 8) + (236 << 16) + (0xff << 24), // 28
	(248) + (248 << 8) + (248 << 16) + (0xff << 24), // 29
	(255) + (255 << 8) + (255 << 16) + (0xff << 24), // 30
	(255) + (255 << 8) + (255 << 16) + (0xff << 24), // 31

	(0x00) + (0x00 << 8) + (0x00 << 16) + (0x00 << 24), // 32
	(0x00) + (0x00 << 8) + (0x00 << 16) + (0x00 << 24), // 33
	(0x00) + (0x00 << 8) + (0x00 << 16) + (0x00 << 24), // 34
	(0x00) + (0x00 << 8) + (0x00 << 16) + (0x00 << 24), // 35
	(0x00) + (0x00 << 8) + (0x00 << 16) + (0x00 << 24), // 36
	(0x00) + (0x00 << 8) + (0x00 << 16) + (0x00 << 24), // 37
	(0x00) + (0x00 << 8) + (0x00 << 16) + (0x00 << 24), // 38
	(0x00) + (0x00 << 8) + (0x00 << 16) + (0x00 << 24), // 39

	(28) + (36 << 8) + (48 << 16) + (0xff << 24),	// 40
	(60) + (68 << 8) + (76 << 16) + (0xff << 24),	// 41
	(96) + (100 << 8) + (104 << 16) + (0xff << 24),  // 42
	(132) + (132 << 8) + (136 << 16) + (0xff << 24), // 43
	(148) + (152 << 8) + (156 << 16) + (0xff << 24), // 44
	(168) + (172 << 8) + (176 << 16) + (0xff << 24), // 45
	(184) + (192 << 8) + (196 << 16) + (0xff << 24), // 46
	(204) + (212 << 8) + (220 << 16) + (0xff << 24), // 47

	(44) + (52 << 8) + (60 << 16) + (0xff << 24),   // 48
	(44) + (52 << 8) + (88 << 16) + (0xff << 24),   // 49
	(44) + (52 << 8) + (116 << 16) + (0xff << 24),  // 50
	(48) + (56 << 8) + (148 << 16) + (0xff << 24),  // 51
	(48) + (64 << 8) + (172 << 16) + (0xff << 24),  // 52
	(52) + (72 << 8) + (196 << 16) + (0xff << 24),  // 53
	(56) + (80 << 8) + (220 << 16) + (0xff << 24),  // 54
	(92) + (116 << 8) + (255 << 16) + (0xff << 24), // 55
};

int32_t mechCmdr1PaletteLookup[256];

//---------------------------------------------------------------------------
void VFXShapeElement::init(puint8_t _shape, int32_t _x, int32_t _y, int32_t frame, bool rev,
	uint32_t* fTable, float _z, float tZ)
{
	gosASSERT(_shape != nullptr);
	int32_t i = 0;
	while (shapeTable[i] != nullptr)
		i++;
	gosASSERT(i < MAX_ELEMENT_SHAPES);
	shapeTable[i] = _shape;
	frameNum[i]   = frame;
	reverse[i]	= rev;
	if (!i)
	{
		x		  = _x;
		y		  = _y;
		fadeTable = fTable;
		z		  = _z;
		topZ	  = tZ;
	}
}

//---------------------------------------------------------------------------
int32_t VFXShapeElement::getTextureHandle(int32_t height)
{
	uint32_t textureResult = 0;
	if (height == -1)
	{
		//-----------------------------------------------
		// We need to take the shape bounds and find the
		// smallest texture buffer that will hold it.
		// Create an empty Keyed buffer for now.
		int32_t textureSize = VFX_shape_bounds(shapeTable[0], frameNum[0]);
		int32_t xRes		= textureSize >> 16;
		int32_t yRes		= textureSize & 0x0000ffff;
		if ((xRes < 16) && (yRes < 16))
		{
			textureResult =
				gos_NewEmptyTexture(gos_Texture_Keyed, "ShapeTable", 16, gosHint_DisableMipmap);
		}
		else if ((xRes < 32) && (yRes < 32))
		{
			textureResult =
				gos_NewEmptyTexture(gos_Texture_Keyed, "ShapeTable", 32, gosHint_DisableMipmap);
		}
		else if ((xRes < 64) && (yRes < 64))
		{
			textureResult =
				gos_NewEmptyTexture(gos_Texture_Keyed, "ShapeTable", 64, gosHint_DisableMipmap);
		}
		else if ((xRes < 128) && (yRes < 128))
		{
			textureResult =
				gos_NewEmptyTexture(gos_Texture_Keyed, "ShapeTable", 128, gosHint_DisableMipmap);
		}
		else if ((xRes < 256) && (yRes < 256))
		{
			textureResult =
				gos_NewEmptyTexture(gos_Texture_Keyed, "ShapeTable", 256, gosHint_DisableMipmap);
		}
	}
	else
	{
		textureResult =
			gos_NewEmptyTexture(gos_Texture_Keyed, "ShapeTable", height, gosHint_DisableMipmap);
	}
	textureMemoryHandle = textureResult;
	actualHeight		= height;
	//-------------------------------------------------------------------
	// Try setting up the texture buffer here instead of right before
	// we force D3D to draw the damned thing.  Maybe helps LockTexture?
	drawShape();
	return textureResult;
}

//-------------------------------------------------------------------------------------------
void VFXShapeElement::drawShape(void)
{
	//-------------------------------------------------------------------------------------------
	// -Create a buffer to hold the shapes.
	// -Draw all of the shapes to the buffer in order received.
	// -Get Pointer to Locked texture RAM.
	// -Copy 8Bit buffer to Texture Buffer using fTable passed in or default if
	// fTable is nullptr -Unlock Texture RAM! draw in Draw Function now.  Not
	// here.  Give texture a chance to settle in before drawing.
	PANE gPane;
	WINDOW gWindow;
	gPane.window = &gWindow;
	gPane.x0 = gPane.y0 = 0;
	gWindow.stencil = gWindow.shadow = nullptr;
	int32_t textureWidth			 = 0;
	int32_t maxFrames				 = VFX_shape_count(shapeTable[0]);
	if (frameNum[0] >= maxFrames)
		frameNum[0] = maxFrames - 1;
	int32_t textureSize = VFX_shape_bounds(shapeTable[0], frameNum[0]);
	int32_t xRes		= textureSize >> 16;
	int32_t yRes		= textureSize & 0x0000ffff;
	if (actualHeight == -1)
	{
		if ((xRes < 16) && (yRes < 16))
		{
			if (!shapeBuffer16)
				shapeBuffer16 = (puint8_t)systemHeap->Malloc(16 * 16);
			gWindow.buffer = shapeBuffer16;
			memset(gWindow.buffer, 0, 16 * 16);
			gPane.x1 = gPane.y1 = 16;
			gWindow.x_max = gWindow.y_max = 15;
			textureWidth				  = 16;
		}
		else if ((xRes < 32) && (yRes < 32))
		{
			if (!shapeBuffer32)
				shapeBuffer32 = (puint8_t)systemHeap->Malloc(32 * 32);
			gWindow.buffer = shapeBuffer32;
			memset(gWindow.buffer, 0, 32 * 32);
			gPane.x1 = gPane.y1 = 32;
			gWindow.x_max = gWindow.y_max = 31;
			textureWidth				  = 32;
		}
		else if ((xRes < 64) && (yRes < 64))
		{
			if (!shapeBuffer64)
				shapeBuffer64 = (puint8_t)systemHeap->Malloc(64 * 64);
			gWindow.buffer = shapeBuffer64;
			memset(gWindow.buffer, 0, 64 * 64);
			gPane.x1 = gPane.y1 = 64;
			gWindow.x_max = gWindow.y_max = 63;
			textureWidth				  = 64;
		}
		else if ((xRes < 128) && (yRes < 128))
		{
			if (!shapeBuffer128)
				shapeBuffer128 = (puint8_t)systemHeap->Malloc(128 * 128);
			gWindow.buffer = shapeBuffer128;
			memset(gWindow.buffer, 0, 128 * 128);
			gPane.x1 = gPane.y1 = 128;
			gWindow.x_max = gWindow.y_max = 127;
			textureWidth				  = 128;
		}
		else if ((xRes < 256) && (yRes < 256))
		{
			if (!shapeBuffer256)
				shapeBuffer256 = (puint8_t)systemHeap->Malloc(256 * 256);
			gWindow.buffer = shapeBuffer256;
			memset(gWindow.buffer, 0, 256 * 256);
			gPane.x1 = gPane.y1 = 256;
			gWindow.x_max = gWindow.y_max = 255;
			textureWidth				  = 256;
		}
	}
	else
	{
		switch (actualHeight)
		{
		case 16:
			if (!shapeBuffer16)
				shapeBuffer16 = (puint8_t)systemHeap->Malloc(16 * 16);
			gWindow.buffer = shapeBuffer16;
			break;
		case 32:
			if (!shapeBuffer32)
				shapeBuffer32 = (puint8_t)systemHeap->Malloc(32 * 32);
			gWindow.buffer = shapeBuffer32;
			break;
		case 64:
			if (!shapeBuffer64)
				shapeBuffer64 = (puint8_t)systemHeap->Malloc(64 * 64);
			gWindow.buffer = shapeBuffer64;
			break;
		case 128:
			if (!shapeBuffer128)
				shapeBuffer128 = (puint8_t)systemHeap->Malloc(128 * 128);
			gWindow.buffer = shapeBuffer128;
			break;
		case 256:
			if (!shapeBuffer256)
				shapeBuffer256 = (puint8_t)systemHeap->Malloc(256 * 256);
			gWindow.buffer = shapeBuffer256;
			break;
		}
		memset(gWindow.buffer, 0, actualHeight * actualHeight);
		gPane.x1 = gPane.y1 = actualHeight;
		gWindow.x_max = gWindow.y_max = actualHeight - 1;
		textureWidth				  = actualHeight;
	}
	//------------------------------------------
	// Draw the shapes in order to the buffer
	int32_t i  = 0;
	xHS		   = 0.0;
	yHS		   = 0.0;
	float uxHS = 0.0, uyHS = 0.0;
	textureFactor = float(textureWidth) * Camera::globalScaleFactor;
	if (!tempBuffer)
		tempBuffer = (puint8_t)systemHeap->Malloc(MAX_X * MAX_Y);
	while (shapeTable[i] && (i < MAX_ELEMENT_SHAPES))
	{
		int32_t oldHSX, oldHSY;
		int32_t HS = VFX_shape_origin(shapeTable[i], frameNum[i]);
		oldHSX	 = HS >> 16;
		oldHSY	 = HS & 0x0000ffff;
		uxHS	   = float(textureWidth) * oldHSX / xRes;
		uyHS	   = float(textureWidth) * oldHSY / yRes;
		xHS		   = uxHS * Camera::globalScaleFactor;
		yHS		   = uyHS * Camera::globalScaleFactor;
		AG_shape_transform(
			&gPane, shapeTable[i], frameNum[i], uxHS, uyHS, tempBuffer, reverse[i], TRUE);
		i++;
	}
	//-------------------
	// Lock the texture.
	TEXTUREPTR textureData;
	gos_LockTexture(textureMemoryHandle, 0, 0, &textureData);
	//-------------------------------------------------------
	// Copy 8 bit data to 32 bit texture surface using table
	// Could easily assemblify and much improve speed!
	uint32_t* textureMemory = textureData.pTexture;
	puint8_t bufferMemory   = gWindow.buffer;
	if (fadeTable)
	{
		for (i = 0; i < textureWidth; i++)
		{
			for (size_t j = 0; j < textureWidth; j++)
			{
				*textureMemory = fadeTable[*bufferMemory];
				bufferMemory++;
				textureMemory++;
			}
		}
	}
	else
	{
		// memclear(textureMemory,textureWidth * textureWidth);	//Nothing to
		// draw.  Wipe it.
		memset(textureMemory, 0,
			textureWidth * textureWidth); // Nothing to draw.  Wipe it.
	}
	//------------------------
	// Unlock the texture
	gos_UnLockTexture(textureMemoryHandle);
}

//---------------------------------------------------------------------------
void VFXShapeElement::setTextureHandle(uint32_t handle, int32_t height)
{
	textureMemoryHandle = handle;
	actualHeight		= height;
	//-------------------------------------------------------------------
	// Try setting up the texture buffer here instead of right before
	// we force D3D to draw the damned thing.  Maybe helps LockTexture?
	drawShape();
}

//---------------------------------------------------------------------------
void VFXShapeElement::draw(void)
{
	//-------------------------------------
	// Clip and Draw Top Triangle First
	gos_VERTEX gVertex[3];
	gVertex[0].x	= x - xHS;
	gVertex[0].y	= y - yHS;
	gVertex[0].z	= z; // topZ;
	gVertex[0].rhw  = 0.5;
	gVertex[0].u	= 0.0;
	gVertex[0].v	= 0.0;
	gVertex[0].argb = lightRGB;
	gVertex[0].frgb = fogRGB;
	gVertex[1].x	= gVertex[0].x + textureFactor;
	gVertex[1].y	= gVertex[0].y;
	gVertex[1].z	= z; // topZ;
	gVertex[1].rhw  = 0.5;
	gVertex[1].u	= 1.0;
	gVertex[1].v	= 0.0;
	gVertex[1].argb = lightRGB;
	gVertex[1].frgb = fogRGB;
	gVertex[2].x	= gVertex[0].x + textureFactor;
	gVertex[2].y	= gVertex[0].y + textureFactor;
	gVertex[2].z	= topZ; // z;
	gVertex[2].rhw  = 0.5;
	gVertex[2].u	= 1.0;
	gVertex[2].v	= 1.0;
	gVertex[2].argb = lightRGB;
	gVertex[2].frgb = fogRGB;
	gos_SetRenderState(gos_State_Filter, gos_FilterNone);
	gos_SetRenderState(gos_State_AlphaMode, gos_Alpha_OneZero);
	gos_SetRenderState(gos_State_Specular, false);
	gos_SetRenderState(gos_State_AlphaTest, TRUE);
	gos_SetRenderState(gos_State_Texture, textureMemoryHandle);
	if ((gVertex[0].z >= 0.0f) && (gVertex[0].z < 1.0f) && (gVertex[1].z >= 0.0f) &&
		(gVertex[1].z < 1.0f) && (gVertex[2].z >= 0.0f) && (gVertex[2].z < 1.0f))
	{
		//-----------------------------------------------------------------------------
		// Reject Any triangle which has vertices off screeen in software for
		// now. Do real cliping in geometry layer for software and hardware that
		// needs it!
		gos_DrawTriangles(gVertex, 3);
	}
	//----------------------------------------
	// clip and draw bottom triangle
	gVertex[1].x	= gVertex[2].x;
	gVertex[1].y	= gVertex[2].y;
	gVertex[1].z	= gVertex[2].z;
	gVertex[1].rhw  = gVertex[2].rhw;
	gVertex[1].u	= gVertex[2].u;
	gVertex[1].v	= gVertex[2].v;
	gVertex[1].argb = gVertex[2].argb;
	gVertex[1].frgb = gVertex[2].frgb;
	gVertex[2].x	= gVertex[0].x;
	gVertex[2].y	= gVertex[0].y + textureFactor;
	gVertex[2].z	= topZ; // z;
	gVertex[2].rhw  = 0.5;
	gVertex[2].u	= 0.0;
	gVertex[2].v	= 1.0;
	gVertex[2].argb = lightRGB;
	gVertex[2].frgb = fogRGB;
	if ((gVertex[0].z >= 0.0f) && (gVertex[0].z < 1.0f) && (gVertex[1].z >= 0.0f) &&
		(gVertex[1].z < 1.0f) && (gVertex[2].z >= 0.0f) && (gVertex[2].z < 1.0f))
	{
		//-----------------------------------------------------------------------------
		// Reject Any triangle which has vertices off screeen in software for
		// now. Do real cliping in geometry layer for software and hardware that
		// needs it!
		gos_DrawTriangles(gVertex, 3);
	}
	//-------------------------------------------
	// Reset shapeTable Data after draw!
	// Everything else is reset during INIT call!
	shapeTable[0] = shapeTable[1] = shapeTable[2] = shapeTable[3] = nullptr;
}

//---------------------------------------------------------------------------
// class TextureElement
void TextureElement::init(uint32_t textureHandle, int32_t _x, int32_t _y, int32_t hsx, int32_t hsy,
	float tW, float _z, float tZ)
{
	x					= _x;
	y					= _y;
	z					= _z;
	topZ				= tZ;
	xHS					= hsx;
	yHS					= hsy;
	tWidth				= tW;
	textureMemoryHandle = textureHandle;
}

//---------------------------------------------------------------------------
void TextureElement::draw(void)
{
	//-------------------------------------
	// Nothing else to do with this bUt
	// -gosDraw!
	float textureFactor = float(tWidth) * Camera::globalScaleFactor;
	xHS *= Camera::globalScaleFactor;
	yHS *= Camera::globalScaleFactor;
	//----------------------------
	// Clip and draw top triangle
	gos_VERTEX gVertex[3];
	gVertex[0].x	= x - xHS;
	gVertex[0].y	= y - yHS;
	gVertex[0].z	= topZ;
	gVertex[0].rhw  = 0.5;
	gVertex[0].u	= 0.0;
	gVertex[0].v	= 0.0;
	gVertex[0].argb = lightRGB;
	gVertex[0].frgb = fogRGB;
	gVertex[1].x	= gVertex[0].x + textureFactor;
	gVertex[1].y	= gVertex[0].y;
	gVertex[1].z	= topZ;
	gVertex[1].rhw  = 0.5;
	gVertex[1].u	= 1.0;
	gVertex[1].v	= 0.0;
	gVertex[1].argb = lightRGB;
	gVertex[1].frgb = fogRGB;
	gVertex[2].x	= gVertex[0].x + textureFactor;
	gVertex[2].y	= gVertex[0].y + textureFactor;
	gVertex[2].z	= z;
	gVertex[2].rhw  = 0.5;
	gVertex[2].u	= 1.0;
	gVertex[2].v	= 1.0;
	gVertex[2].argb = lightRGB;
	gVertex[2].frgb = fogRGB;
	//--------------------------------
	// Set States for Software Renderer
	gos_SetRenderState(gos_State_Texture, textureMemoryHandle);
	if (Environment.Renderer == 3)
	{
		gos_SetRenderState(gos_State_AlphaMode, gos_Alpha_OneZero);
		gos_SetRenderState(gos_State_ShadeMode, gos_ShadeGouraud);
		gos_SetRenderState(gos_State_MonoEnable, 1);
		gos_SetRenderState(gos_State_Perspective, 0);
		gos_SetRenderState(gos_State_Clipping, 0);
		gos_SetRenderState(gos_State_AlphaTest, 0);
		gos_SetRenderState(gos_State_Specular, 0);
		gos_SetRenderState(gos_State_Dither, 0);
		gos_SetRenderState(gos_State_TextureMapBlend, gos_BlendModulate);
		gos_SetRenderState(gos_State_Filter, gos_FilterNone);
		gos_SetRenderState(gos_State_TextureAddress, gos_TextureClamp);
	}
	//--------------------------------
	// Set States for Hardware Renderer
	else
	{
		gos_SetRenderState(gos_State_AlphaMode, gos_Alpha_OneZero);
		gos_SetRenderState(gos_State_ShadeMode, gos_ShadeGouraud);
		gos_SetRenderState(gos_State_MonoEnable, 0);
		gos_SetRenderState(gos_State_Perspective, 1);
		gos_SetRenderState(gos_State_Clipping, 2);
		gos_SetRenderState(gos_State_AlphaTest, 0);
		gos_SetRenderState(gos_State_Specular, 0);
		gos_SetRenderState(gos_State_Dither, 1);
		gos_SetRenderState(gos_State_TextureMapBlend, gos_BlendModulate);
		gos_SetRenderState(gos_State_Filter, gos_FilterBiLinear);
		gos_SetRenderState(gos_State_TextureAddress, gos_TextureClamp);
	}
	if ((gVertex[0].z >= 0.0f) && (gVertex[0].z < 1.0f) && (gVertex[1].z >= 0.0f) &&
		(gVertex[1].z < 1.0f) && (gVertex[2].z >= 0.0f) && (gVertex[2].z < 1.0f))
	{
		//-----------------------------------------------------------------------------
		// Reject Any triangle which has vertices off screeen in software for
		// now. Do real cliping in geometry layer for software and hardware that
		// needs it!
		gos_DrawTriangles(gVertex, 3);
	}
	//---------------------------------------
	// Clip and draw Bottom Triangle
	gVertex[1].x	= gVertex[2].x;
	gVertex[1].y	= gVertex[2].y;
	gVertex[1].z	= gVertex[2].z;
	gVertex[1].rhw  = gVertex[2].rhw;
	gVertex[1].u	= gVertex[2].u;
	gVertex[1].v	= gVertex[2].v;
	gVertex[1].argb = gVertex[2].argb;
	gVertex[1].frgb = gVertex[2].frgb;
	gVertex[2].x	= gVertex[0].x;
	gVertex[2].y	= gVertex[0].y + textureFactor;
	gVertex[2].z	= z;
	gVertex[2].rhw  = 0.5;
	gVertex[2].u	= 0.0;
	gVertex[2].v	= 1.0;
	gVertex[2].argb = lightRGB;
	gVertex[2].frgb = fogRGB;
	if ((gVertex[0].z >= 0.0f) && (gVertex[0].z < 1.0f) && (gVertex[1].z >= 0.0f) &&
		(gVertex[1].z < 1.0f) && (gVertex[2].z >= 0.0f) && (gVertex[2].z < 1.0f))
	{
		//-----------------------------------------------------------------------------
		// Reject Any triangle which has vertices off screeen in software for
		// now. Do real cliping in geometry layer for software and hardware that
		// needs it!
		gos_DrawTriangles(gVertex, 3);
	}
}

//---------------------------------------------------------------------------
// class PolygonQuadElement
void PolygonQuadElement::init(gos_VERTEX* v)
{
	vertices[0] = v[0];
	vertices[1] = v[1];
	vertices[2] = v[2];
	vertices[3] = v[3];
	//------------------------------------------
	// Calc Average Depth here.
	depth = -(v[0].y + v[1].y + v[2].y + v[3].y) / 4.0;
}

//---------------------------------------------------------------------------
void PolygonQuadElement::draw(void)
{
	//-------------------------------------
	// nothing else to do with this bUt
	// -gosDraw!
	//------------------------
	// Draw em!
	//--------------------------------
	// Set States for Software Renderer
	if (Environment.Renderer == 3)
	{
		gos_SetRenderState(gos_State_AlphaMode, gos_Alpha_OneZero);
		gos_SetRenderState(gos_State_ShadeMode, gos_ShadeGouraud);
		gos_SetRenderState(gos_State_MonoEnable, 1);
		gos_SetRenderState(gos_State_Perspective, 0);
		gos_SetRenderState(gos_State_Clipping, 0);
		gos_SetRenderState(gos_State_AlphaTest, 0);
		gos_SetRenderState(gos_State_Specular, 0);
		gos_SetRenderState(gos_State_Dither, 0);
		gos_SetRenderState(gos_State_TextureMapBlend, gos_BlendModulate);
		gos_SetRenderState(gos_State_Filter, gos_FilterNone);
		//		gos_SetRenderState( gos_State_TextureAddress, gos_TextureClamp
		//);
		gos_SetRenderState(gos_State_Texture, 0);
	}
	//--------------------------------
	// Set States for Hardware Renderer
	else
	{
		gos_SetRenderState(gos_State_AlphaMode, gos_Alpha_AlphaInvAlpha);
		gos_SetRenderState(gos_State_ShadeMode, gos_ShadeGouraud);
		gos_SetRenderState(gos_State_MonoEnable, 0);
		gos_SetRenderState(gos_State_Perspective, 1);
		gos_SetRenderState(gos_State_Clipping, 2);
		gos_SetRenderState(gos_State_AlphaTest, 1);
		gos_SetRenderState(gos_State_Specular, 0);
		gos_SetRenderState(gos_State_Dither, 1);
		gos_SetRenderState(gos_State_TextureMapBlend, gos_BlendModulate);
		gos_SetRenderState(gos_State_Filter, gos_FilterBiLinear);
		//		gos_SetRenderState( gos_State_TextureAddress, gos_TextureClamp
		//);
		gos_SetRenderState(gos_State_Texture, 0);
		gos_SetRenderState(gos_State_ZCompare, 1);
		gos_SetRenderState(gos_State_ZWrite, 1);
	}
	//----------------------------------------
	// Clip and draw top triangle
	gos_VERTEX gVertex[3];
	memcpy(gVertex, vertices, sizeof(gos_VERTEX) * 3);
	if ((gVertex[0].z >= 0.0f) && (gVertex[0].z < 1.0f) && (gVertex[1].z >= 0.0f) &&
		(gVertex[1].z < 1.0f) && (gVertex[2].z >= 0.0f) && (gVertex[2].z < 1.0f))
	{
		//-----------------------------------------------------------------------------
		// Reject Any triangle which has vertices off screeen in software for
		// now. Do real cliping in geometry layer for software and hardware that
		// needs it!
		gos_DrawTriangles(gVertex, 3);
	}
	//---------------------------------------
	// Clip and draw Bottom Triangle
	gVertex[1].x	= gVertex[2].x;
	gVertex[1].y	= gVertex[2].y;
	gVertex[1].z	= gVertex[2].z;
	gVertex[1].rhw  = gVertex[2].rhw;
	gVertex[1].u	= gVertex[2].u;
	gVertex[1].v	= gVertex[2].v;
	gVertex[1].argb = gVertex[2].argb;
	gVertex[1].frgb = gVertex[2].frgb;
	gVertex[2].x	= vertices[3].x;
	gVertex[2].y	= vertices[3].y;
	gVertex[2].z	= vertices[3].z;
	gVertex[2].rhw  = vertices[3].rhw;
	gVertex[2].u	= vertices[3].u;
	gVertex[2].v	= vertices[3].v;
	gVertex[2].argb = vertices[3].argb;
	gVertex[2].frgb = vertices[3].frgb;
	if ((gVertex[0].z >= 0.0f) && (gVertex[0].z < 1.0f) && (gVertex[1].z >= 0.0f) &&
		(gVertex[1].z < 1.0f) && (gVertex[2].z >= 0.0f) && (gVertex[2].z < 1.0f))
	{
		//-----------------------------------------------------------------------------
		// Reject Any triangle which has vertices off screeen in software for
		// now. Do real cliping in geometry layer for software and hardware that
		// needs it!
		gos_DrawTriangles(gVertex, 3);
	}
}

//---------------------------------------------------------------------------
// class PolygonTriElement
void PolygonTriElement::init(gos_VERTEX* v)
{
	vertices[0] = v[0];
	vertices[1] = v[1];
	vertices[2] = v[2];
	//------------------------------------------
	// Calc Average Depth here.
	depth = -(v[0].y + v[1].y + v[2].y) / 3.0;
}

//---------------------------------------------------------------------------
void PolygonTriElement::draw(void)
{
	//-------------------------------------
	// nothing else to do with this bUt
	// -gosDraw!
	//------------------------
	// Draw em!
	//--------------------------------
	// Set States for Software Renderer
	if (Environment.Renderer == 3)
	{
		gos_SetRenderState(gos_State_AlphaMode, gos_Alpha_OneZero);
		gos_SetRenderState(gos_State_ShadeMode, gos_ShadeGouraud);
		gos_SetRenderState(gos_State_MonoEnable, 1);
		gos_SetRenderState(gos_State_Perspective, 0);
		gos_SetRenderState(gos_State_Clipping, 0);
		gos_SetRenderState(gos_State_AlphaTest, 0);
		gos_SetRenderState(gos_State_Specular, 0);
		gos_SetRenderState(gos_State_Dither, 0);
		gos_SetRenderState(gos_State_TextureMapBlend, gos_BlendModulate);
		gos_SetRenderState(gos_State_Filter, gos_FilterNone);
		//		gos_SetRenderState( gos_State_TextureAddress, gos_TextureClamp
		//);
		gos_SetRenderState(gos_State_Texture, 0);
	}
	//--------------------------------
	// Set States for Hardware Renderer
	else
	{
		gos_SetRenderState(gos_State_AlphaMode, gos_Alpha_OneZero);
		gos_SetRenderState(gos_State_ShadeMode, gos_ShadeGouraud);
		gos_SetRenderState(gos_State_MonoEnable, 0);
		gos_SetRenderState(gos_State_Perspective, 1);
		gos_SetRenderState(gos_State_Clipping, 2);
		gos_SetRenderState(gos_State_AlphaTest, 0);
		gos_SetRenderState(gos_State_Specular, 0);
		gos_SetRenderState(gos_State_Dither, 1);
		gos_SetRenderState(gos_State_TextureMapBlend, gos_BlendModulate);
		gos_SetRenderState(gos_State_Filter, gos_FilterBiLinear);
		//		gos_SetRenderState( gos_State_TextureAddress, gos_TextureClamp
		//);
		gos_SetRenderState(gos_State_Texture, 0);
	}
	//----------------------------------------
	// Clip and draw top triangle
	gos_VERTEX gVertex[3];
	memcpy(gVertex, vertices, sizeof(gos_VERTEX) * 3);
	if ((gVertex[0].z >= 0.0f) && (gVertex[0].z < 1.0f) && (gVertex[1].z >= 0.0f) &&
		(gVertex[1].z < 1.0f) && (gVertex[2].z >= 0.0f) && (gVertex[2].z < 1.0f))
	{
		//-----------------------------------------------------------------------------
		// Reject Any triangle which has vertices off screeen in software for
		// now. Do real cliping in geometry layer for software and hardware that
		// needs it!
		gos_DrawTriangles(gVertex, 3);
	}
}

//---------------------------------------------------------------------------
// class TexturedPolygonQuadElement : public PolygonQuadElement
void TexturedPolygonQuadElement::init(gos_VERTEX* v, uint32_t tHandle, bool writez, bool compz)
{
	PolygonQuadElement::init(v);
	textureHandle = tHandle;
	zWrite		  = writez;
	zComp		  = compz;
}

//---------------------------------------------------------------------------
void TexturedPolygonQuadElement::draw(void)
{
	//-------------------------------------
	// nothing else to do with this bUt
	// -gosDraw!
	//------------------------
	// Draw em!
	//--------------------------------
	// Set States for Software Renderer
	if (Environment.Renderer == 3)
	{
		gos_SetRenderState(gos_State_AlphaMode, gos_Alpha_OneZero);
		gos_SetRenderState(gos_State_ShadeMode, gos_ShadeGouraud);
		gos_SetRenderState(gos_State_MonoEnable, 1);
		gos_SetRenderState(gos_State_Perspective, 0);
		gos_SetRenderState(gos_State_Clipping, 0);
		gos_SetRenderState(gos_State_AlphaTest, 0);
		gos_SetRenderState(gos_State_Specular, 0);
		gos_SetRenderState(gos_State_Dither, 0);
		gos_SetRenderState(gos_State_TextureMapBlend, gos_BlendModulate);
		gos_SetRenderState(gos_State_Filter, gos_FilterNone);
		gos_SetRenderState(gos_State_TextureAddress, gos_TextureClamp);
		gos_SetRenderState(gos_State_ZCompare, zComp);
		gos_SetRenderState(gos_State_ZWrite, zWrite);
	}
	//--------------------------------
	// Set States for Hardware Renderer
	else
	{
		gos_SetRenderState(gos_State_AlphaMode, gos_Alpha_AlphaInvAlpha);
		gos_SetRenderState(gos_State_ShadeMode, gos_ShadeGouraud);
		gos_SetRenderState(gos_State_MonoEnable, 0);
		gos_SetRenderState(gos_State_Perspective, 1);
		gos_SetRenderState(gos_State_Clipping, 2);
		gos_SetRenderState(gos_State_AlphaTest, 1);
		gos_SetRenderState(gos_State_Specular, 0);
		gos_SetRenderState(gos_State_Dither, 1);
		gos_SetRenderState(gos_State_TextureMapBlend, gos_BlendModulate);
		gos_SetRenderState(gos_State_Filter, gos_FilterBiLinear);
		gos_SetRenderState(gos_State_TextureAddress, gos_TextureClamp);
		gos_SetRenderState(gos_State_ZCompare, zComp);
		gos_SetRenderState(gos_State_ZWrite, zWrite);
	}
	if (textureHandle == 0xffffffff)
	{
		gos_SetRenderState(gos_State_Texture, 0);
		gos_SetRenderState(gos_State_Filter, gos_FilterNone);
	}
	else
		gos_SetRenderState(gos_State_Texture, textureHandle);
	//----------------------------------------
	// Clip and draw top triangle
	gos_VERTEX gVertex[3];
	memcpy(gVertex, vertices, sizeof(gos_VERTEX) * 3);
	if ((gVertex[0].z >= 0.0f) && (gVertex[0].z < 1.0f) && (gVertex[1].z >= 0.0f) &&
		(gVertex[1].z < 1.0f) && (gVertex[2].z >= 0.0f) && (gVertex[2].z < 1.0f))
	{
		//-----------------------------------------------------------------------------
		// Reject Any triangle which has vertices off screeen in software for
		// now. Do real cliping in geometry layer for software and hardware that
		// needs it!
		gos_DrawTriangles(gVertex, 3);
	}
	//---------------------------------------
	// Clip and draw Bottom Triangle
	gVertex[1].x	= gVertex[2].x;
	gVertex[1].y	= gVertex[2].y;
	gVertex[1].z	= gVertex[2].z;
	gVertex[1].rhw  = gVertex[2].rhw;
	gVertex[1].u	= gVertex[2].u;
	gVertex[1].v	= gVertex[2].v;
	gVertex[1].argb = gVertex[2].argb;
	gVertex[1].frgb = gVertex[2].frgb;
	gVertex[2].x	= vertices[3].x;
	gVertex[2].y	= vertices[3].y;
	gVertex[2].z	= vertices[3].z;
	gVertex[2].rhw  = vertices[3].rhw;
	gVertex[2].u	= vertices[3].u;
	gVertex[2].v	= vertices[3].v;
	gVertex[2].argb = vertices[3].argb;
	gVertex[2].frgb = vertices[3].frgb;
	if ((gVertex[0].z >= 0.0f) && (gVertex[0].z < 1.0f) && (gVertex[1].z >= 0.0f) &&
		(gVertex[1].z < 1.0f) && (gVertex[2].z >= 0.0f) && (gVertex[2].z < 1.0f))
	{
		//-----------------------------------------------------------------------------
		// Reject Any triangle which has vertices off screeen in software for
		// now. Do real cliping in geometry layer for software and hardware that
		// needs it!
		gos_DrawTriangles(gVertex, 3);
	}
}

//---------------------------------------------------------------------------
// class TexturedPolygonTriElement : public PolygonTriElement
void TexturedPolygonTriElement::init(gos_VERTEX* v, uint32_t tHandle)
{
	PolygonTriElement::init(v);
	textureHandle = tHandle;
}

//---------------------------------------------------------------------------
void TexturedPolygonTriElement::draw(void)
{
	//-------------------------------------
	// nothing else to do with this bUt
	// -gosDraw!
	//------------------------
	// Draw em!
	//--------------------------------
	// Set States for Software Renderer
	if (Environment.Renderer == 3)
	{
		gos_SetRenderState(gos_State_AlphaMode, gos_Alpha_OneZero);
		gos_SetRenderState(gos_State_ShadeMode, gos_ShadeGouraud);
		gos_SetRenderState(gos_State_MonoEnable, 1);
		gos_SetRenderState(gos_State_Perspective, 0);
		gos_SetRenderState(gos_State_Clipping, 0);
		gos_SetRenderState(gos_State_AlphaTest, 0);
		gos_SetRenderState(gos_State_Specular, 0);
		gos_SetRenderState(gos_State_Dither, 0);
		gos_SetRenderState(gos_State_TextureMapBlend, gos_BlendModulate);
		gos_SetRenderState(gos_State_Filter, gos_FilterNone);
		gos_SetRenderState(gos_State_TextureAddress, gos_TextureClamp);
	}
	//--------------------------------
	// Set States for Hardware Renderer
	else
	{
		gos_SetRenderState(gos_State_AlphaMode, gos_Alpha_OneZero);
		gos_SetRenderState(gos_State_ShadeMode, gos_ShadeGouraud);
		gos_SetRenderState(gos_State_MonoEnable, 0);
		gos_SetRenderState(gos_State_Perspective, 1);
		gos_SetRenderState(gos_State_Clipping, 2);
		gos_SetRenderState(gos_State_AlphaTest, 0);
		gos_SetRenderState(gos_State_Specular, 0);
		gos_SetRenderState(gos_State_Dither, 1);
		gos_SetRenderState(gos_State_TextureMapBlend, gos_BlendModulate);
		gos_SetRenderState(gos_State_Filter, gos_FilterBiLinear);
		gos_SetRenderState(gos_State_TextureAddress, gos_TextureClamp);
	}
	if (textureHandle == 0xffffffff)
	{
		gos_SetRenderState(gos_State_Texture, 0);
		gos_SetRenderState(gos_State_Filter, gos_FilterNone);
	}
	else
		gos_SetRenderState(gos_State_Texture, textureHandle);
	//----------------------------------------
	// Clip and draw top triangle
	gos_VERTEX gVertex[3];
	memcpy(gVertex, vertices, sizeof(gos_VERTEX) * 3);
	if ((gVertex[0].z >= 0.0f) && (gVertex[0].z < 1.0f) && (gVertex[1].z >= 0.0f) &&
		(gVertex[1].z < 1.0f) && (gVertex[2].z >= 0.0f) && (gVertex[2].z < 1.0f))
	{
		//-----------------------------------------------------------------------------
		// Reject Any triangle which has vertices off screeen in software for
		// now. Do real cliping in geometry layer for software and hardware that
		// needs it!
		gos_DrawTriangles(gVertex, 3);
	}
}

//---------------------------------------------------------------------------
