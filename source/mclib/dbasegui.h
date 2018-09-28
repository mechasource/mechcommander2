//===========================================================================//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

//--------------------------------------------------------------------------------------
//
// MechCommander 2
//
// This header contains the base classes for the GUI
//
// GUI is now single update driven. An event comes in, the manager decides who
// its for and passes the event down. Everything still know how to draw etc.
//
// All drawing is done through gos_drawQuad and drawTriangle
//
// Basic cycle is
//		call gObject->update with this frame's events.
//			This will check the events to see if any pertain to me.
//			Draw anything to the texture plane that needs drawing.
//			Call any code which the events might have trigged.
//		call gObject->render with this frame's events.
//			This draws the object to the screen.
//			called in order of depth.
//
//--------------------------------------------------------------------------------------

#pragma once

#ifndef DBASEGUI_H
#define DBASEGUI_H
//--------------------------------------------------------------------------------------
// Include Files
//--------------------------------------------------------------------------------------

enum gtgui_const : int32_t
{
	gtINVALID	= -1,
	gtBACKGROUND = 0,
	gtBUTTON	 = 1,
	gtTEXT		 = 2,
	gtHOTTEXT	= 3,
	gtEDIT		 = 4,
	gtCHECKBOX   = 5,
	gtSPINNER	= 6,
	gtLISTBOX	= 7,
	gtTABCTRL	= 8,
	gtDRAGOBJECT = 9,
	gtMENU		 = 10,
	gtDIALOG	 = 11,
};

enum amsg_const : int32_t
{
	aMSG_NOTHING	 = -1,
	aMSG_MOUSE_OVER  = 0,
	aMSG_BUTTON_DOWN = 1,
	aMSG_BUTTON_UP   = 2,
	aMSG_BUTTON_GRAY = 3,
	aMSG_MOUSE_MOVE  = 4,
	aMSG_SELCHANGE   = 5,
	aMSG_CANCEL		 = 6,
	aMSG_OK			 = 7,
};

enum abtn_const : int32_t
{
	aBTN_NEWCAMPAIGN = 0,
	aBTN_LOADMISSION = 1,
	aBTN_SAVEMISSION = 2,
	aBTN_MULTIPLAYER = 3,
	aBTN_PREFERENCES = 4,
	aBTN_OPENINGCIN  = 5,
	aBTN_QUIT		 = 6,
	aBTN_RETURNGAME  = 7,
};

//	Status bar interface colors
enum statusbar_iface_colors_const : uint32_t
{
	SD_YELLOW = ((0x00) + (0xff << 8) + (0xff << 16) + (0xff << 24)),
	SD_RED	= ((0x00) + (0x00 << 8) + (0xff << 16) + (0xff << 24)),
	SD_GREEN  = ((0x00) + (0xff << 8) + (0x00 << 16) + (0xff << 24)),
	SD_BLUE   = ((0xff) + (0x00 << 8) + (0x00 << 16) + (0xff << 24)),
	SD_ORANGE = ((0x00) + (0x7f << 8) + (0xff << 16) + (0xff << 24)),
	SD_WHITE  = ((0xff) + (0xff << 8) + (0xff << 16) + (0xff << 24)),
	SD_GRAY   = ((0x3f) + (0x3f << 8) + (0x3f << 16) + (0xff << 24)),
	SD_BLACK  = ((0x00) + (0x00 << 8) + (0x00 << 16) + (0xff << 24)),

	SX_WHITE  = ((0xff) + (0xff << 8) + (0xff << 16) + (0xbf << 24)),
	SX_CYAN   = ((0xff) + (0xff << 8) + (0x00 << 16) + (0xbf << 24)),
	SX_YELLOW = ((0x00) + (0xff << 8) + (0xff << 16) + (0xbf << 24)),
	SX_RED	= ((0x00) + (0x00 << 8) + (0xff << 16) + (0xbf << 24)),
	SX_GREEN  = ((0x00) + (0xff << 8) + (0x00 << 16) + (0xbf << 24)),
	SX_BLUE   = ((0xff) + (0x00 << 8) + (0x00 << 16) + (0xbf << 24)),
	SX_ORANGE = ((0x00) + (0x7f << 8) + (0xff << 16) + (0xbf << 24)),
	SX_GRAY   = ((0x3f) + (0x3f << 8) + (0x3f << 16) + (0xbf << 24)),
	SX_BLACK  = ((0x00) + (0x00 << 8) + (0x00 << 16) + (0xbf << 24)),

	SB_YELLOW = ((0x00) + (0xff << 8) + (0xff << 16) + (0x6f << 24)),
	SB_RED	= ((0x00) + (0x00 << 8) + (0xff << 16) + (0x6f << 24)),
	SB_GREEN  = ((0x00) + (0xff << 8) + (0x00 << 16) + (0x6f << 24)),
	SB_BLUE   = ((0xff) + (0x00 << 8) + (0x00 << 16) + (0x6f << 24)),
	SB_ORANGE = ((0x00) + (0x7f << 8) + (0xff << 16) + (0x6f << 24)),
	SB_WHITE  = ((0xff) + (0xff << 8) + (0xff << 16) + (0x6f << 24)),
	SB_GRAY   = ((0x3f) + (0x3f << 8) + (0x3f << 16) + (0x6f << 24)),
	SB_BLACK  = ((0x00) + (0x00 << 8) + (0x00 << 16) + (0x6f << 24)),

	XP_YELLOW = ((0x00) + (0xff << 8) + (0xff << 16) + (0x3f << 24)),
	XP_RED	= ((0x00) + (0x00 << 8) + (0xff << 16) + (0x3f << 24)),
	XP_GREEN  = ((0x00) + (0xff << 8) + (0x00 << 16) + (0x3f << 24)),
	XP_BLUE   = ((0xff) + (0x00 << 8) + (0x00 << 16) + (0x3f << 24)),
	XP_ORANGE = ((0x00) + (0x7f << 8) + (0xff << 16) + (0x3f << 24)),
	XP_WHITE  = ((0xff) + (0xff << 8) + (0xff << 16) + (0x3f << 24)),
	XP_GRAY   = ((0x3f) + (0x3f << 8) + (0x3f << 16) + (0x3f << 24)),
	XP_BLACK  = ((0x00) + (0x00 << 8) + (0x00 << 16) + (0x3f << 24)),
};

//--------------------------------------------------------------------------------------
#endif