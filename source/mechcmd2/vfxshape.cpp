//---------------------------------------------------------------------------
//
// vfxshape.cpp - This file contains the header for the new shape engine
//
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

//---------------------------------------------------------------------------
// Include files
#include "stdinc.h"

// #include "mclib.h"

#ifndef VFXSHAPE_H
#include "vfxshape.h"
#endif

//#ifndef SPRTMGR_H
//#include "sprtmgr.h"
//#endif

#ifndef APPRTYPE_H
#include "apprtype.h"
#endif

//---------------------------------------------------------------------------
// Statics
#define HEADER_OFFSET 8

//****************************************************************************
//
// class Shape member functions
//
//****************************************************************************
void Shape::destroy(void)
{
	//----------------------------------------------------------------
	//-- First, tell our owner that we've gone away.
	//-- Owner simply NULLs the corresponding entry in its shapeList.
	if (owner)
	{
		owner->removeShape(this);
	}
	//----------------------------------------------------
	// The header points to the start of the memory block
	// all other pointer are internal to that block and do
	// NOT need to be deleted.  Please don't delete them
	// or the heap will fatal out!
	if (stupidHeader)
		spriteManager->freeShapeRAM(stupidHeader);
	else
		spriteManager->freeShapeRAM(frameList);
	frameList = nullptr;
}

#define STUPID_OFFSET 6
//----------------------------------------------------------------------------
int32_t
Shape::init(uint8_t* fileBlock, AppearanceTypePtr myOwner, int32_t shapeSize)
{
	//-----------------------------------------------------------------
	// Everything in the below comment is a LIE!!!
	//
	// The entire shape is now stored in a NewShape FSY shape file.
	// The entire shape file is loaded as the .FSY file and all info
	// is stored in the file format itself.  This solves all of the
	// crazy malloc issues previously associated with the sprites.
	//
	// Truth begins here.
	// This is simply the data holder for ALL shape types in the game EXCEPT
	// mechs.
	if ((*(int32_t*)fileBlock != *(int32_t*)"1.10"))
	{
		stupidHeader = fileBlock;
		frameList = fileBlock + STUPID_OFFSET; // You can talk to GDoren about this one!!!
	}
	else
	{
		stupidHeader = nullptr;
		frameList = fileBlock;
	}
	int32_t numFrames = VFX_shape_count(frameList);
	if (!numFrames)
	{
		return (-1); // There are no frames, this shape is nullptr!!
	}
	owner = myOwner;
	if (numFrames)
	{
		int32_t* testOffset = (int32_t*)(frameList + 8);
		if (*testOffset >= shapeSize)
		{
			frameList = nullptr;
			return (-3);
		}
		int32_t minOffset = 8 + (numFrames * 8); // The first shape MUST be this far in or WRONG
		if (minOffset != *testOffset)
		{
			frameList = nullptr;
			return (-4);
		}
	}
	return (NO_ERROR);
}

//----------------------------------------------------------------------------
