//===========================================================================//
// File:	 DebugGUI.hpp													 //
// Contents: Debugger user interface										 //
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#pragma once

#ifndef _DEBUGGUI_HPP_
#define _DEBUGGUI_HPP_

void __stdcall UpdateDebugMouse(void);
void __stdcall UpdateDebugWindow(void);
void __stdcall InitTextDisplay(void);
void __stdcall DrawText(uint32_t Color, PSTR String);
void __stdcall DrawSquare(
	int32_t TopX, int32_t TopY, int32_t Width, int32_t Height, uint32_t Color);
void __stdcall DrawChr(char Chr);

//
// Debugger window variables
//
extern int32_t DbTopX, DbTopY;
extern int32_t DbMaxX, DbMaxY, DbMinX;
extern uint32_t DebugDisplay;
extern uint32_t TopStatistics;

//
// Debugger window constants
//
#define DbSizeX 489
#define DbSizeY 342

extern int32_t CurrentX, CurrentY; // Current pixel position
extern int32_t DbChrX, DbChrY; // Current character x,y
extern int32_t StartX, StartY; // Current start of line

#endif
