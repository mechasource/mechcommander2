//---------------------------------------------------------------------------//
// scale.cpp - This file contains definitions of the scaleFactor for scaled
// draws
//
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//
#include "stdafx.h"
// #include <math.h>

//---------------------------------------------------------------------------
// Include files
#ifndef DSTD_H
#include "dstd.h"
#endif

#ifndef HEAP_H
#include "heap.h"
#endif

#ifndef VPORT_H
#include "vport.h"
#endif

#ifndef VFX_H
#include "vfx.h"
#endif

#ifndef CAMERA_H
#include "camera.h"
#endif

#include <gameos.hpp>

extern void AG_shape_translate_transform(PANE* globalPane, PVOIDshapeTable,
	int32_t frameNum, int32_t hotX, int32_t hotY, PVOIDtempBuffer,
	int32_t reverse, int32_t scaleUp);
extern void AG_shape_transform(PANE* globalPane, PVOIDshapeTable,
	int32_t frameNum, int32_t hotX, int32_t hotY, PVOIDtempBuffer,
	int32_t reverse, int32_t scaleUp);
extern void AG_shape_lookaside(puint8_t table);
extern void AG_shape_draw(PANE* pane, PVOIDshape_table, int32_t shape_number,
	int32_t hotX, int32_t hotY);
extern void AG_shape_translate_draw(PANE* pane, PVOIDshape_table,
	int32_t shape_number, int32_t hotX, int32_t hotY);

//---------------------------------------------------------------------------
#define MAX_X 360
#define MAX_Y 360

puint8_t tempBuffer = nullptr;
//---------------------------------------------------------------------------
//
int32_t scaleDraw(puint8_t shapeTable, uint32_t frameNum, int32_t sx,
	int32_t sy, bool reverse, puint8_t fadeTable, bool scaleUp)
{
	//--------------------------------------------------------------
	// Check GlobalPane and GlobalWindow for validity.
	if (Camera::globalScaleFactor == 1.0)
		scaleUp = 1;
	else
		scaleUp = 0;
	//----------------------------------------------------------------
	// Check if shape is actually valid.
	if ((*(pint32_t)shapeTable != *(pint32_t) "1.10"))
		return (TRUE);
	int32_t result = VFX_shape_count(shapeTable);
	if (result <= (int32_t)frameNum)
	{
		frameNum = result - 1;
	}
	result		 = VFX_shape_bounds(shapeTable, frameNum);
	int32_t xMax = result >> 16;
	int32_t yMax = result & 0x0000ffff;
	if ((xMax == 0) || (yMax == 0))
		return -1;
	if (abs(xMax - yMax) > 256)
	{
		return -1;
	}
	if ((xMax > 400) || (yMax > 400))
	{
		return -1;
	}
	if (!tempBuffer)
		tempBuffer = (puint8_t)systemHeap->Malloc(MAX_X * MAX_Y);
	gosASSERT((yMax * xMax) < (MAX_Y * MAX_X));
	if (fadeTable)
	{
		AG_shape_lookaside(fadeTable);
		if (scaleUp && !reverse)
			AG_shape_translate_draw(globalPane, shapeTable, frameNum, sx, sy);
		else
			AG_shape_translate_transform(globalPane, shapeTable, frameNum, sx,
				sy, tempBuffer, reverse, scaleUp);
	}
	else
	{
		if (scaleUp && !reverse)
			AG_shape_draw(globalPane, shapeTable, frameNum, sx, sy);
		else
			AG_shape_transform(globalPane, shapeTable, frameNum, sx, sy,
				tempBuffer, reverse, scaleUp);
	}
	return (result);
}

//---------------------------------------------------------------------------
