//---------------------------------------------------------------------------
//
// cepane.cpp - This file contains the code for the VFX Pane Element
//
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#include "stdafx.h"

#ifndef CEPANE_H
#include "cepane.h"
#endif

#ifndef VPORT_H
#include "vport.h"
#endif

#ifndef VFX_H
#include "vfx.h"
#endif

//---------------------------------------------------------------------------
// Static Globals
#define SPRITE_MAX_X 320
#define SPRITE_MAX_Y 240

#define SPRITE_MIDDLE_X 160
#define SPRITE_MIDDLE_Y 120

//---------------------------------------------------------------------------
PaneElement::PaneElement(PANE* _shapePane, int32_t _x, int32_t _y,
	int32_t _midx, int32_t _midy, int32_t _SizeX, int32_t _SizeY)
	: Element(-_y)
{
	shapePane = _shapePane;
	x		  = _x;
	y		  = _y;
	midx	  = _midx;
	midy	  = _midy;
	SizeX	 = _SizeX;
	SizeY	 = _SizeY;
}

extern int32_t DrawTransparent(PANE* pane, WINDOW* texture, int32_t X,
	int32_t Y, int32_t Width, int32_t Height);

//---------------------------------------------------------------------------
void PaneElement::draw(void)
{
	DrawTransparent(
		globalPane, shapePane->window, x - midx, y - midy, SizeX, SizeY);
}

//---------------------------------------------------------------------------

extern void AG_shape_draw(PANE* pane, PVOIDshape_table, int32_t shape_number,
	int32_t hotX, int32_t hotY);
extern void AG_shape_translate_draw(PANE* pane, PVOIDshape_table,
	int32_t shape_number, int32_t hotX, int32_t hotY);
extern void AG_shape_lookaside(puint8_t table);
//---------------------------------------------------------------------------
// Static Globals

//---------------------------------------------------------------------------
DeltaElement::DeltaElement(puint8_t _shape, int32_t _x, int32_t _y,
	int32_t frame, bool rev, puint8_t fTable, bool noScale, bool upScale)
	: Element(-_y)
{
	shapeTable  = _shape;
	x			= _x;
	y			= _y;
	frameNum	= frame;
	reverse		= rev;
	fadeTable   = fTable;
	noScaleDraw = noScale;
	scaleUp		= upScale;
	//-------------------------------------
	// Integrity Check here.
#ifdef _DEBUG
	int32_t result = VFX_shape_count(shapeTable);
	if (result <= frameNum)
	{
		frameNum = result - 1;
	}
	result		 = VFX_shape_bounds(shapeTable, frameNum);
	int32_t xMax = result >> 16;
	int32_t yMax = result & 0x0000ffff;
#define MAX_X 360
#define MAX_Y 360
	if ((yMax * xMax) >= (MAX_Y * MAX_X))
	{
		return;
	}
#endif
}

//---------------------------------------------------------------------------
void DeltaElement::draw(void)
{
	{
		//----------------------------------------------------------------
		// Check if shape is actually valid.
		if ((*(pint32_t)shapeTable != *(pint32_t) "1.10"))
			return;
		//----------------------------------------------------
		// DEBUG TEMP Until all are correct delta shapes!!!!!!
		int32_t result = VFX_shape_count(shapeTable);
		if (result <= (frameNum + 1))
			frameNum = result - 2;
		if (!fadeTable)
		{
			AG_shape_draw(globalPane, shapeTable, 0, x, y); // KeyFrame First!!
			AG_shape_draw(globalPane, shapeTable, frameNum + 1, x, y);
		}
		else
		{
			AG_shape_lookaside(fadeTable);
			AG_shape_translate_draw(
				globalPane, shapeTable, 0, x, y); // KeyFrame First!!
			AG_shape_translate_draw(globalPane, shapeTable, frameNum + 1, x, y);
		}
	}
}
