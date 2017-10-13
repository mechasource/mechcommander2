//---------------------------------------------------------------------------
//
// cepane.h - This file contains the class declarations for the VFX Pane Element
//
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#ifndef CEPANE_H
#define CEPANE_H
//---------------------------------------------------------------------------
// Include files

#ifndef CELEMENT_H
#include "celement.h"
#endif

#ifndef VFX_H
#include "vfx.h"
#endif

//---------------------------------------------------------------------------
class PaneElement : public Element
{
  public:
	PANE* shapePane;
	int32_t x;
	int32_t y;
	int32_t midx, midy, SizeX, SizeY;

	PaneElement(void)
	{
		shapePane = nullptr;
		x = y = 0;
	}

	PaneElement(PANE* _shapePane, int32_t _x, int32_t _y, int32_t _midx,
		int32_t _midy, int32_t _SizeX, int32_t _SizeY);

	virtual void draw(void);
};

//---------------------------------------------------------------------------
class DeltaElement : public Element
{
  public:
	puint8_t shapeTable;
	int32_t frameNum;
	int32_t x, y;
	bool reverse;
	puint8_t fadeTable;
	bool noScaleDraw;
	bool scaleUp;

	DeltaElement(void)
	{
		shapeTable = nullptr;
		frameNum   = 0;
		x = y		= 0;
		reverse		= FALSE;
		fadeTable   = nullptr;
		noScaleDraw = FALSE;
		scaleUp		= FALSE;
	}

	DeltaElement(puint8_t _shape, int32_t _x, int32_t _y, int32_t frame,
		bool rev, puint8_t fTable = nullptr, bool noScale = FALSE,
		bool scaleUp = FALSE);

	virtual void draw(void);
};

//---------------------------------------------------------------------------
#endif
