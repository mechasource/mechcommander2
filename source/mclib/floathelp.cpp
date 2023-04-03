//-----------------------------------------------------------------------------------------
//
// Floating Help class.  Used for tool tips, building/mech/vehicle IDs.
// Basically draws itself AFTER everything else since gos_Font"3D" has no Z
// Depth!
//
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//
#include "stdinc.h"

//--------------------------
// Include Files
#ifndef FLOATHELP_H
#include "floathelp.h"
#endif

#ifndef HEAP_H
#include "heap.h"
#endif

//-----------------------------------------------------------------------------------------
uint32_t FloatHelp::currentFloatHelp = 0; // How many of them are we using at present
FloatHelp* FloatHelp::floatHelps = nullptr; // POinters to all of them.

//-----------------------------------------------------------------------------------------
// class FloatHelp
void FloatHelp::init(int32_t maxHelps)
{
	floatHelps = (FloatHelp*)systemHeap->Malloc(sizeof(FloatHelp) * MAX_FLOAT_HELPS);
	gosASSERT(floatHelps != nullptr);
	FloatHelp* tmp = floatHelps;
	for (size_t i = 0; i < MAX_FLOAT_HELPS; i++, tmp++)
	{
		tmp->text[0] = 0;
		tmp->screenPos.x = tmp->screenPos.y = tmp->screenPos.z = tmp->screenPos.w = 0.0f;
		tmp->foregroundcolour = SD_WHITE;
		tmp->backgroundcolour = SD_BLACK;
		tmp->scale = 1.0f;
		tmp->proportional = true;
		tmp->bold = tmp->italic = tmp->wordWrap = false;
	}
	currentFloatHelp = 0;
}

//-----------------------------------------------------------------------------------------
void FloatHelp::destroy(void)
{
	systemHeap->Free(floatHelps);
	floatHelps = nullptr;
	currentFloatHelp = 0;
}

//-----------------------------------------------------------------------------------------
void FloatHelp::resetAll(void)
{
	currentFloatHelp = 0;
	for (size_t i = 0; i < MAX_FLOAT_HELPS; i++)
		floatHelps[i].reset();
}

//-----------------------------------------------------------------------------------------
void FloatHelp::renderAll(void)
{
	for (size_t i = 0; i < MAX_FLOAT_HELPS; i++)
		floatHelps[i].render();
}

//-----------------------------------------------------------------------------------------
void FloatHelp::setFloatHelp(std::wstring_view txt, Stuff::Vector4D screenPos, uint32_t fClr, uint32_t bClr,
	float scl, bool proportional, bool bold, bool italic, bool wordWrap)
{
	if (currentFloatHelp < MAX_FLOAT_HELPS)
	{
		floatHelps[currentFloatHelp].setHelpText(txt);
		floatHelps[currentFloatHelp].setScreenPos(screenPos);
		floatHelps[currentFloatHelp].setForegroundcolour(fClr);
		floatHelps[currentFloatHelp].setBackgroundcolour(bClr);
		floatHelps[currentFloatHelp].setScale(scl);
		floatHelps[currentFloatHelp].setProportional(proportional);
		floatHelps[currentFloatHelp].setBold(bold);
		floatHelps[currentFloatHelp].setItalic(italic);
		floatHelps[currentFloatHelp].setWordWrap(wordWrap);
		currentFloatHelp++;
	}
}

//-----------------------------------------------------------------------------------------
void FloatHelp::getTextStringLength(std::wstring_view txt, uint32_t fcolour, float scl, bool proportional,
	bool bold, bool italic, bool wordWrap, uint32_t& width, uint32_t& height)
{
	// must use globalFloat Scale because of true type fonts
	gos_TextSetAttributes(
		gosFontHandle, fcolour, gosFontScale, wordWrap, proportional, bold, italic);
	uint32_t gheight = 0, gwidth = 0;
	if (txt[0])
	{
		gos_TextStringLength(&gwidth, &gheight, txt);
	}
	width = gwidth;
	height = gheight;
}

//-----------------------------------------------------------------------------------------
