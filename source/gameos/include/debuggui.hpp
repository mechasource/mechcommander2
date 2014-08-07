//===========================================================================//
// File:	 DebugGUI.hpp													 //
// Contents: Debugger user interface										 //
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#pragma once

void __stdcall UpdateDebugMouse(void);
void __stdcall UpdateDebugWindow(void);
void __stdcall InitTextDisplay(void);
void __stdcall DrawText( ULONG Color, char* String );
void __stdcall DrawSquare( int TopX, int TopY, int Width, int Height, ULONG Color );
void __stdcall DrawChr( char Chr );


//
// Debugger window variables
//
extern int DbTopX,DbTopY;
extern int DbMaxX,DbMaxY,DbMinX;
extern ULONG DebugDisplay;
extern ULONG TopStatistics;

//
// Debugger window constants
//
#define DbSizeX 489
#define DbSizeY 342


extern int CurrentX,CurrentY;		// Current pixel position
extern int DbChrX,DbChrY;			// Current character x,y
extern int StartX,StartY;			// Current start of line




