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

#ifndef MCLIB_H
#include <mclib.h>
#endif

#ifndef DAPRTYPE_H
#include "daprtype.h"
#endif

//---------------------------------------------------------------------------------
class Shape
{
	public:

		PUCHAR			frameList;		//This is binary image of VFX file
		PUCHAR			stupidHeader;
		ULONG		lastTurnUsed;
		Shape				*next;
		AppearanceTypePtr	owner;

	public:

		void init (void) 
		{
			frameList = stupidHeader = NULL;
			lastTurnUsed = 0;
			next = NULL;
			owner = NULL;
		}

		Shape (void) 
		{
			init();
		}

		int32_t init (PUCHAR fileBlock, AppearanceTypePtr, int32_t shapeSize);

		void destroy (void);

		~Shape (void) 
		{
			destroy();
		}
};

typedef Shape *ShapePtr;
//---------------------------------------------------------------------------------
#endif
