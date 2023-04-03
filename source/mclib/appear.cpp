//---------------------------------------------------------------------------
//
// Appear.cpp -- File contains the Basic Game Appearance operator overrides
//
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

//---------------------------------------------------------------------------
// Include Files
#include "stdinc.h"

#include "appear.h"

//#include "camera.h"
//#include "apprtype.h"
//#include "dbasegui.h"
//#include "celine.h"
//#include "cevfx.h"

//#ifndef FONT_H
//#include "Font.h"
//#endif

// extern bool useFog;

#if CONSIDERED_OBSOLETE
//---------------------------------------------------------------------------
// class Appearance
PVOID
Appearance::operator new(size_t mySize)
{
	PVOID result = nullptr;
	if (AppearanceTypeList::appearanceHeap && AppearanceTypeList::appearanceHeap->heapReady())
	{
		result = AppearanceTypeList::appearanceHeap->Malloc(mySize);
	}
	return (result);
}

//---------------------------------------------------------------------------
void Appearance::operator delete(PVOID us)
{
	int32_t result;
	if (AppearanceTypeList::appearanceHeap && AppearanceTypeList::appearanceHeap->heapReady())
	{
		result = AppearanceTypeList::appearanceHeap->Free(us);
	}
}
#endif

//---------------------------------------------------------------------------
void Appearance::drawTextHelp(std::wstring_view text, uint32_t color)
{
	uint32_t width, height;
	Stuff::Vector4D moveHere;

	moveHere = screenPos;
	gos_TextSetAttributes(gosFontHandle, 0, gosFontScale, false, true, false, false);
	gos_TextStringLength(&width, &height, text);
	moveHere.y = lowerRight.y + 10.0f;
	moveHere.x -= width / 2;
	moveHere.z = width;
	moveHere.w = height;
	globalFloatHelp->setFloatHelp(text, moveHere, color, SD_BLACK, 1.0f, true, false, false, false);
}

void Appearance::drawTextHelp(std::wstring_view text)
{
	drawTextHelp(text, SD_GREEN);
}

void Appearance::drawPilotName(std::wstring_view text, uint32_t color)
{
	uint32_t width, height;
	Stuff::Vector4D moveHere;
	moveHere = screenPos;
	gos_TextSetAttributes(gosFontHandle, 0, gosFontScale, false, true, false, false);
	gos_TextStringLength(&width, &height, text);
	moveHere.y = lowerRight.y + 10.0f + height;
	moveHere.x -= width / 2;
	moveHere.z = width;
	moveHere.w = height;
	globalFloatHelp->setFloatHelp(text, moveHere, color, SD_BLACK, 1.0f, true, false, false, false);
}

//---------------------------------------------------------------------------
void Appearance::drawSelectBox(uint32_t color)
{
	Stuff::Vector4D ul, br, pos1, pos2;
	float offsets;
	AppearanceType* appearType = getAppearanceType();
	if (appearType && appearType->typeBoundExists())
	{
		eye->projectZ(appearType->typeUpperLeft, ul);
		ul.z = HUD_DEPTH;
		eye->projectZ(appearType->typeLowerRight, br);
		br.z = HUD_DEPTH;
	}
	else
	{
		ul.x = upperLeft.x;
		ul.y = upperLeft.y;
		ul.z = HUD_DEPTH;
		br.x = lowerRight.x;
		br.y = lowerRight.y;
		br.z = HUD_DEPTH;
	}
	//-----------------------------------------------------
	// Must scale the magic numbers for select bracket
	offsets = 5.0f * eye->getScaleFactor();
	pos1.x = float(ul.x - offsets);
	pos1.y = float(ul.y - offsets);
	pos1.z = ul.z;
	pos2.x = float(ul.x - offsets);
	pos2.y = float(ul.y);
	pos2.z = ul.z;
	pos2.w = pos1.w = 1.0f;
	{
		LineElement newElement(pos1, pos2, color, nullptr, -1);
		newElement.draw();
	}
	pos1.x = float(ul.x - offsets);
	pos1.y = float(ul.y - offsets);
	pos1.z = ul.z;
	pos2.x = float(ul.x);
	pos2.y = float(ul.y - offsets);
	pos2.z = ul.z;
	pos2.w = pos1.w = 1.0f;
	{
		LineElement newElement(pos1, pos2, color, nullptr, -1);
		newElement.draw();
	}
	pos1.x = float(br.x + offsets);
	pos1.y = float(ul.y - offsets);
	pos1.z = ul.z;
	pos2.x = float(br.x + offsets);
	pos2.y = float(ul.y);
	pos2.z = ul.z;
	pos2.w = pos1.w = 1.0f;
	{
		LineElement newElement(pos1, pos2, color, nullptr, -1);
		newElement.draw();
	}
	pos1.x = float(br.x + offsets);
	pos1.y = float(ul.y - offsets);
	pos1.z = ul.z;
	pos2.x = float(br.x);
	pos2.y = float(ul.y - offsets);
	pos2.z = ul.z;
	pos2.w = pos1.w = 1.0f;
	{
		LineElement newElement(pos1, pos2, color, nullptr, -1);
		newElement.draw();
	}
	pos1.x = float(br.x + offsets);
	pos1.y = float(br.y + offsets);
	pos1.z = ul.z;
	pos2.x = float(br.x + offsets);
	pos2.y = float(br.y);
	pos2.z = ul.z;
	pos2.w = pos1.w = 1.0f;
	{
		LineElement newElement(pos1, pos2, color, nullptr, -1);
		newElement.draw();
	}
	pos1.x = float(br.x + offsets);
	pos1.y = float(br.y + offsets);
	pos1.z = ul.z;
	pos2.x = float(br.x);
	pos2.y = float(br.y + offsets);
	pos2.z = ul.z;
	pos2.w = pos1.w = 1.0f;
	{
		LineElement newElement(pos1, pos2, color, nullptr, -1);
		newElement.draw();
	}
	pos1.x = float(ul.x - offsets);
	pos1.y = float(br.y + offsets);
	pos1.z = ul.z;
	pos2.x = float(ul.x - offsets);
	pos2.y = float(br.y);
	pos2.z = ul.z;
	pos2.w = pos1.w = 1.0f;
	{
		LineElement newElement(pos1, pos2, color, nullptr, -1);
		newElement.draw();
	}
	pos1.x = float(ul.x - offsets);
	pos1.y = float(br.y + offsets);
	pos1.z = ul.z;
	pos2.x = float(ul.x);
	pos2.y = float(br.y + offsets);
	pos2.z = ul.z;
	pos2.w = pos1.w = 1.0f;
	{
		LineElement newElement(pos1, pos2, color, nullptr, -1);
		newElement.draw();
	}
}

//---------------------------------------------------------------------------
void Appearance::drawSelectBrackets(uint32_t color)
{
	float offsets = 5.0 * eye->getScaleFactor();
	Stuff::Vector4D pos1;
	Stuff::Vector4D pos2;
	Stuff::Vector4D ul, br;
	ul.x = upperLeft.x;
	ul.y = upperLeft.y;
	ul.z = HUD_DEPTH;
	br.x = lowerRight.x;
	br.y = lowerRight.y;
	br.z = HUD_DEPTH;
	pos1.x = ul.x;
	pos1.y = ul.y;
	pos1.z = ul.z;
	pos2.x = ul.x + offsets;
	pos2.y = ul.y;
	pos2.z = ul.z;
	pos2.w = pos1.w = 1.0f;
	{
		LineElement newElement(pos1, pos2, color, nullptr, -1);
		newElement.draw();
	}
	pos1.x = ul.x;
	pos1.y = ul.y;
	pos1.z = ul.z;
	pos2.x = ul.x;
	pos2.y = ul.y + offsets;
	pos2.z = ul.z;
	pos2.w = pos1.w = 1.0f;
	{
		LineElement newElement(pos1, pos2, color, nullptr, -1);
		newElement.draw();
	}
	pos1.x = br.x;
	pos1.y = ul.y;
	pos1.z = ul.z;
	pos2.x = br.x;
	pos2.y = ul.y + offsets;
	pos2.z = ul.z;
	pos2.w = pos1.w = 1.0f;
	{
		LineElement newElement(pos1, pos2, color, nullptr, -1);
		newElement.draw();
	}
	pos1.x = br.x;
	pos1.y = ul.y;
	pos1.z = ul.z;
	pos2.x = br.x - offsets;
	pos2.y = ul.y;
	pos2.z = ul.z;
	pos2.w = pos1.w = 1.0f;
	{
		LineElement newElement(pos1, pos2, color, nullptr, -1);
		newElement.draw();
	}
	pos1.x = ul.x;
	pos1.y = br.y;
	pos1.z = br.z;
	pos2.x = ul.x;
	pos2.y = br.y - offsets;
	pos2.z = br.z;
	pos2.w = pos1.w = 1.0f;
	{
		LineElement newElement(pos1, pos2, color, nullptr, -1);
		newElement.draw();
	}
	pos1.x = ul.x;
	pos1.y = br.y;
	pos1.z = br.z;
	pos2.x = ul.x + offsets;
	pos2.y = br.y;
	pos2.z = br.z;
	pos2.w = pos1.w = 1.0f;
	{
		LineElement newElement(pos1, pos2, color, nullptr, -1);
		newElement.draw();
	}
	pos1.x = br.x;
	pos1.y = br.y;
	pos1.z = br.z;
	pos2.x = br.x;
	pos2.y = br.y - offsets;
	pos2.z = ul.z;
	pos2.w = pos1.w = 1.0f;
	{
		LineElement newElement(pos1, pos2, color, nullptr, -1);
		newElement.draw();
	}
	pos1.x = br.x;
	pos1.y = br.y;
	pos1.z = br.z;
	pos2.x = br.x - offsets;
	pos2.y = br.y;
	pos2.z = ul.z;
	pos2.w = pos1.w = 1.0f;
	{
		LineElement newElement(pos1, pos2, color, nullptr, -1);
		newElement.draw();
	}
}

//-----------------------------------------------------------------------------
void DrawBox(float l, float t, float r, float b)
{
	Stuff::Vector4D p1, p2;
	p1.x = l;
	p1.y = t;
	p1.z = HUD_DEPTH;
	p1.w = 1.0;
	p2.x = r;
	p2.y = t;
	p2.z = HUD_DEPTH;
	p2.w = 1.0;
	{
		LineElement newElement(p1, p2, SD_BLACK, nullptr, -1);
		newElement.draw();
	}
	p1.x = r;
	p1.y = t;
	p2.x = r;
	p2.y = b;
	{
		LineElement newElement(p1, p2, SD_BLACK, nullptr, -1);
		newElement.draw();
	}
	p1.x = r;
	p1.y = b;
	p2.x = l;
	p2.y = b;
	{
		LineElement newElement(p1, p2, SD_BLACK, nullptr, -1);
		newElement.draw();
	}
	p1.x = l;
	p1.y = t;
	p2.x = l;
	p2.y = b;
	{
		LineElement newElement(p1, p2, SD_BLACK, nullptr, -1);
		newElement.draw();
	}
}

void Appearance::drawIcon(
	uint32_t bmpHandle, uint32_t bmpwidth, uint32_t bmpheight, uint32_t color, uint32_t where)
{
	// ignoring where for now
	float offset = 8.0 * eye->getScaleFactor();
	float trueheight = HEIGHT * eye->getScaleFactor();
	float Y = upperLeft.y - offset - trueheight - 2 * bmpheight;
	float X = (upperLeft.x + lowerRight.x) / 2.f - bmpwidth / 2.f;
	gos_VERTEX v[4];
	for (size_t i = 0; i < 4; i++)
	{
		v[i].argb = color;
		v[i].frgb = 0;
		v[i].z = 0.f;
		v[i].rhw = .5f;
		v[i].x = X;
		v[i].y = Y;
		v[i].u = .25; // might want to pass these in too....
		v[i].v = 0.f;
	}
	v[2].x = v[3].x = X + bmpwidth;
	v[1].y = v[2].y = Y + bmpheight;
	v[2].u = v[3].u = 9.f / 16.f;
	v[1].v = v[2].v = 5.f / 16.f;
	gos_VERTEX v1[3];
	v1[0] = v[0];
	v1[1] = v[2];
	v1[2] = v[3];
	mcTextureManager->addVertices(bmpHandle, v, MC2_ISHUDLMNT);
	mcTextureManager->addVertices(bmpHandle, v1, MC2_ISHUDLMNT);
}

//---------------------------------------------------------------------------
void Appearance::drawBars(void)
{
	//-----------------------------------------
	// Change to GOS DrawQuad code for HWare!!
	float offset = 8.0 * eye->getScaleFactor(); // Remember, EVEN numbers!!!
	float truewidth = WIDTH * eye->getScaleFactor() * 2;
	float trueheight = HEIGHT * eye->getScaleFactor();
	float topY = upperLeft.y - offset - trueheight;
	float leftX = floor((upperLeft.x + lowerRight.x) / 2.f - truewidth / 2);
	uint32_t color;
	if (barStatus > 1.0f)
		barStatus = 1.0f;
	if (!barcolour)
	{
		if (barStatus >= 0.5)
			color = SB_GREEN;
		else if (barStatus > 0.2)
			color = SB_YELLOW;
		else if (barStatus)
			color = SB_RED;
		else
			color = 0;
	}
	else
		color = barcolour;
	float barLength = truewidth * barStatus;
	gos_VERTEX vertices[4];
	vertices[0].x = leftX - 1.0;
	vertices[0].y = topY - 1.0;
	vertices[0].z = HUD_DEPTH;
	vertices[0].rhw = 0.5;
	vertices[0].u = 0.0;
	vertices[0].v = 0.0;
	vertices[0].argb = color | 0xff000000; // Factor out the alpha color!!
	vertices[0].frgb = 0x00000000;
	vertices[1].x = leftX + barLength + 1.0;
	vertices[1].y = topY - 1.0;
	vertices[1].z = HUD_DEPTH;
	vertices[1].rhw = 0.5;
	vertices[1].u = 0.0;
	vertices[1].v = 0.0;
	vertices[1].argb = color | 0xff000000; // Factor out the alpha color!!
	vertices[1].frgb = 0x00000000;
	vertices[2].x = leftX + barLength + 1.0;
	vertices[2].y = topY + trueheight + 1.0;
	vertices[2].z = HUD_DEPTH;
	vertices[2].rhw = 0.5;
	vertices[2].u = 0.0;
	vertices[2].v = 0.0;
	vertices[2].argb = color | 0xff000000; // Factor out the alpha color!!
	vertices[2].frgb = 0x00000000;
	vertices[3].x = leftX - 1.0;
	vertices[3].y = topY + trueheight + 1.0;
	vertices[3].z = HUD_DEPTH;
	vertices[3].rhw = 0.5;
	vertices[3].u = 0.0;
	vertices[3].v = 0.0;
	vertices[3].argb = color | 0xff000000; // Factor out the alpha color!!
	vertices[3].frgb = 0x00000000;
	PolygonQuadElement newElement;
	newElement.init(vertices);
	gos_SetRenderState(gos_State_Fog, 0);
	newElement.draw();
	DrawBox(vertices[0].x, vertices[0].y, (leftX + truewidth + 1.0), vertices[2].y);
	uint32_t fogcolour = eye->fogcolour;
	//-----------------------------------------------------
	// FOG time.  Set Render state to FOG on!
	if (useFog)
	{
		gos_SetRenderState(gos_State_Fog, (int32_t)&fogcolour);
	}
}

//---------------------------------------------------------------------------
