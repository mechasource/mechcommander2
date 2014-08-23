#ifndef EDITORMESSAGES_H
#define EDITORMESSAGES_H
/*************************************************************************************************\
EditorMessages.h			: Interface for the EditorMessages component.
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//
\*************************************************************************************************/

#include "..\MCLib\MCLibresource.h"

#define PAINT_OVERLAY_DIRT		IDS_DIRTROAD
#define PAINT_OVERLAY_PAVED		IDS_PAVED_ROAD
#define PAINT_OVERLAY_ROUGH		IDS_ROUGH
#define PAINT_TERRAIN_GRASS		IDS_GRASS
#define PAINT_TERRAIN_BLUEWATER		IDS_BLUEWATER
#define PAINT_TERRAIN_GREENWATER		IDS_GREENWATER
#define PAINT_TERRAIN_MUD		IDS_MUD
#define PAINT_TERRAIN_MOSS		IDS_MOSS
#define PAINT_TERRAIN_DIRT		IDS_DIRT
#define PAINT_TERRAIN_ASH		IDS_ASH
#define PAINT_TERRAIN_MOUNTAIN		IDS_MOUNTAIN
#define PAINT_TERRAIN_TUNDRA		IDS_TUNDRA
#define PAINT_TERRAIN_FORESTFLOOR		IDS_FORESTFLOOR
#define PAINT_TERRAIN_CONCRETE		IDS_CONCRETE
#define PAINT_TERRAIN_CLIFF		IDS_CLIFF

// CURSORS!
#define NORMAL_CURSOR		0
#define PAINT_CURSOR		1
#define SELECT_AREA_CURSOR		2
#define ERASER_CURSOR		3
#define TERRAIN_CURSORS		4 // don't use next 12, those are for the terrains
#define OVERLAY_CURSORS		17 // don't use next 10, reserving those for overlays
#define FLATTEN_CURSOR		28
#define DRAG_SMOOTH			29
#define DRAG_NORMAL			30
#define DRAG_HAND			31
#define MOVE_BUILDING		32
#define DAMAGE				33
#define REPAIR				34


//*************************************************************************************************
#endif  // end of file ( EditorMessages.h )
