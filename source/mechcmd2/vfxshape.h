//---------------------------------------------------------------------------
//
// vfxshape.h - This file contains the header for the old shape format
//
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#pragma once

#ifndef VFXSHAPE_H
#define VFXSHAPE_H
//---------------------------------------------------------------------------
// Include files

// #include "mclib.h"

#ifndef DAPRTYPE_H
#include <daprtype.h>
#endif

//---------------------------------------------------------------------------------
class Shape
{
public:
	uint8_t* frameList; // This is binary image of VFX file
	uint8_t* stupidHeader;
	uint32_t lastTurnUsed;
	Shape* next;
	AppearanceTypePtr owner;

public:
	void init(void)
	{
		frameList = stupidHeader = nullptr;
		lastTurnUsed = 0;
		next = nullptr;
		owner = nullptr;
	}

	Shape(void) { init(void); }

	int32_t init(uint8_t* fileBlock, AppearanceTypePtr, int32_t shapeSize);

	void destroy(void);

	~Shape(void) { destroy(void); }
};

typedef Shape* ShapePtr;
//---------------------------------------------------------------------------------
#endif
