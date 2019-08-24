//***************************************************************************
//
//	Debugging.cpp -- File contains the Game Debugging code
//
//	MechCommander 2
//
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//
#include "stdinc.h"
// #include <string.h>

#ifndef DEBUGGING_H
#include "debugging.h"
#endif

#ifndef HEAP_H
#include "heap.h"
#endif

extern UserHeapPtr systemHeap;

HGOSFONT3D GameDebugWindow::font = nullptr;
int32_t GameDebugWindow::fontHeight = 0;

//***************************************************************************
//	GAME DEBUG WINDOW class
//***************************************************************************

PVOID
GameDebugWindow::operator new(size_t ourSize)
{
	PVOID result = systemHeap->Malloc(ourSize);
	return (result);
}

//---------------------------------------------------------------------------

void
GameDebugWindow::operator delete(PVOID us)
{
	systemHeap->Free(us);
}

//---------------------------------------------------------------------------

void
GameDebugWindow::setFont(const std::wstring_view& fontFile)
{
	if (font)
	{
		gos_DeleteFont(font);
		font = nullptr;
	}
	if (fontFile)
	{
		font = gos_LoadFont(fontFile);
		gos_TextSetAttributes(font, 0xffffffff, 1.0, true, true, false, false);
	}
	uint32_t height, width;
	gos_TextStringLength(&width, &height, "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ");
	fontHeight = height;
}

//---------------------------------------------------------------------------

void
GameDebugWindow::print(const std::wstring_view& s)
{
	if (numLines < MAX_DEBUG_WINDOW_LINES)
		strcpy(textBuffer[numLines++], s);
	else
	{
		numLines++;
		strcpy(textBuffer[linePos++], s);
		if (linePos == MAX_DEBUG_WINDOW_LINES)
			linePos = 0;
	}
}

//---------------------------------------------------------------------------

void
GameDebugWindow::render(void)
{
	if (!display)
		return;
	int32_t i;
	gos_TextSetAttributes(font, 0xffffffff, 1.0, true, true, false, false);
	gos_TextSetRegion(0, 0, Environment.screenWidth, Environment.screenHeight);
	int32_t curY = pos[1] + 5;
	for (i = linePos; i < MAX_DEBUG_WINDOW_LINES; i++)
	{
		gos_TextSetPosition(pos[0] + 5, curY);
		curY += fontHeight;
		gos_TextDraw(textBuffer[i]);
	}
	for (i = 0; i < linePos; i++)
	{
		gos_TextSetPosition(pos[0] + 5, curY);
		curY += fontHeight;
		gos_TextDraw(textBuffer[i]);
	}
}

//***************************************************************************
