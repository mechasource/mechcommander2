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
void __stdcall DrawText(uint32_t colour, PSTR string);
void __stdcall DrawTextA(uint32_t colour, PSTR string)
void __stdcall DrawSquare(int32_t topx, int32_t topy, int32_t width, int32_t height, uint32_t colour);
void __stdcall DrawChr(char Chr);

//
// Debugger window variables
//
extern int32_t Dbtopx, Dbtopy;
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
