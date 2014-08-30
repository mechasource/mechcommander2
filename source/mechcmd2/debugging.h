//***************************************************************************
//
//	Debugging.h -- File contains the MechCommander Debugging Definitions
//
//	MechCommander 2
//
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#pragma once

#ifndef DEBUGGING_H
#define DEBUGGING_H

//#include <stdio.h>
//#include <gameos.hpp>
//#include <GameOS\ToolOS.hpp>

//---------------------------------------------------------------------------

#define	MAX_DEBUG_WINDOW_LINES		6
#define	MAX_DEBUG_WINDOW_LINELEN	256

class GameDebugWindow {

	public:

		bool				display;
		int32_t				pos[2];
		char				textBuffer[MAX_DEBUG_WINDOW_LINES][MAX_DEBUG_WINDOW_LINELEN];
		int32_t				linePos;
		int32_t				numLines;

		static HGOSFONT3D	font;
		static int32_t			fontHeight;

	public:

		void init (void) {
			display = false;
			pos[0] = 0;
			pos[1] = 0;
			linePos = 0;
			numLines = 0;
			for (int32_t i = 0; i < MAX_DEBUG_WINDOW_LINES; i++)
				textBuffer[i][0] = nullptr;
		}
		
		GameDebugWindow (void) {
			init(void);
		}

		void setPos (int32_t x, int32_t y) {
			pos[0] = x;
			pos[1] = y;
		}
		
		void open (int32_t x = -1, int32_t y = -1) {
			if ((x > -1) && (y > -1))
				setPos(x, y);
			display = true;
		}

		void close (void) {
			display = false;
		}

		void toggle (void) {
			if (display)
				close(void);
			else
				open(void);
		}

		~GameDebugWindow (void) {
			destroy(void);
		}
		
		virtual void destroy (void) {
		}
		
		void print (PSTR s);

		void render (void);

		void clear (void) {
			numLines = 0;
			for (int32_t i = 0; i < MAX_DEBUG_WINDOW_LINES; i++)
				textBuffer[i][0] = nullptr;
		}

		static void setFont (PSTR fontFile);
};

//***************************************************************************

#endif




