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

#ifndef DEBUGGING_H
#include "debugging.h"
#endif

#include <stdio.h>
#include <string.h>
//#include "gameos.hpp"
#include "toolos.hpp"

HGOSFONT3D GameDebugWindow::font = nullptr;
int32_t GameDebugWindow::fontheight = 0;

//***************************************************************************
//	GAME DEBUG WINDOW class
//***************************************************************************

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
	fontheight = height;
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
	int32_t curY = pos[1] + 5;
	for (size_t i = linePos; i < MAX_DEBUG_WINDOW_LINES; i++)
	{
		gos_TextSetPosition(pos[0] + 5, curY);
		curY += fontheight;
		gos_TextDraw(textBuffer[i]);
	}
	for (i = 0; i < linePos; i++)
	{
		gos_TextSetPosition(pos[0] + 5, curY);
		curY += fontheight;
		gos_TextDraw(textBuffer[i]);
	}
}

//***************************************************************************
