//******************************************************************************************
//	bitflag.cpp - This file contains the bitflag class
//
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

//----------------------------------------------------------------------------------
// Include Files
#include "stdafx.h"

#ifndef BITFLAG_H
#include "bitflag.h"
#endif

#ifndef HEAP_H
#include "heap.h"
#endif

#ifndef FILE_H
#include "file.h"
#endif

#ifndef TGAINFO_H
#include "tgainfo.h"
#endif

#include <gameos.hpp>

extern void AG_ellipse_draw(PANE *pane, int32_t xc, int32_t yc, int32_t width, int32_t height, int32_t color);
extern void AG_ellipse_fill(PANE *pane, int32_t xc, int32_t yc, int32_t width, int32_t height, int32_t color);
extern void AG_ellipse_fillXor(PANE *pane, int32_t xc, int32_t yc, int32_t width, int32_t height, int32_t color);
extern void AG_ellipse_fillOr(PANE *pane, int32_t xc, int32_t yc, int32_t width, int32_t height, int32_t color);
//extern void memclear(PVOID Dest,size_t Length);
//------------------------------------------------------------------------
// Class BitFlag
int32_t BitFlag::init (uint32_t numRows, uint32_t numColumns, uint32_t initialValue)
{
	rows = numRows;
	columns = numColumns;
	
	divValue = BITS_PER_BYTE;
	totalFlags = numRows * numColumns;
	totalRAM = totalFlags / divValue;
	colWidth = columns / divValue;

	flagHeap = new HeapManager;
	
	if (!flagHeap)
		return(NO_RAM_FOR_FLAG_HEAP);
		
	int32_t result = flagHeap->createHeap(totalRAM);
	if (result != NO_ERROR)
		return(result);
		
	result = flagHeap->commitHeap();
	if (result != NO_ERROR)
		return(result);

	resetAll(initialValue);
		
	return(NO_ERROR);
}	

//------------------------------------------------------------------------
void BitFlag::resetAll (uint32_t initialValue)
{
	//------------------------------------------
	// Set memory to initial Flag Value
	if (initialValue == 0)
	{
		// memclear(flagHeap->getHeapPtr(),totalRAM);
		memset(flagHeap->getHeapPtr(), 0, totalRAM);
		maskValue = 1;
	}
	else
	{
		//-----------------------------------------
		// Not zero, must jump through hoops.
		maskValue = 1;
		memset(flagHeap->getHeapPtr(), 0xff, totalRAM);
	}
}

//------------------------------------------------------------------------
void BitFlag::destroy (void)
{
	delete flagHeap;
	flagHeap = NULL;
	
	init();
}	
		
//------------------------------------------------------------------------
// This sets location to bits
void BitFlag::setFlag (uint32_t r, uint32_t c)
{
	if ((r < 0) || (r >= rows) || (c < 0) || (c > columns))
		return;
		
	//-------------------------------
	// find out where we are setting.
	uint32_t location = (c>>3) + (r*colWidth);
	uint8_t shiftValue = (c % divValue);

	//----------------------------------------
	// find the location we care about.
	puint8_t bitResult = flagHeap->getHeapPtr();
	bitResult += location;

	//----------------------------------------
	// Shift bits to correct place.
	uint8_t bits = 1;
	bits <<= shiftValue;
	*bitResult |= bits;
}	
		
//------------------------------------------------------------------------
void BitFlag::setGroup (uint32_t r, uint32_t c, uint32_t length)
{
	if (length)
	{
		if ((r < 0) || (r >= rows) || (c < 0) || (c > columns) || ((r*c+length) >= (rows*columns)))
			return;
		
		//-------------------------------
		// find out where we are setting.
		uint32_t location = (c>>3) + (r*colWidth);
		uint8_t shiftValue = (c % divValue);
	
		//----------------------------------------
		// find the location we care about.
		puint8_t bitResult = flagHeap->getHeapPtr();
		bitResult += location;
	
		//--------------------------------------------------
		// We are only setting bits in the current location
		if ((8 - shiftValue) >= (int32_t)length)
		{
			uint8_t startVal = 1;
			uint32_t repeatVal = length;
			for (int32_t i=0;i<int32_t(repeatVal-1);i++)
			{
				startVal<<=1;
				startVal++;
			}
			
			//----------------------------------------
			// Shift bits to correct place.
			uint8_t bits = startVal;
			bits <<= shiftValue;
			*bitResult |= bits;
			length -= repeatVal;
		}
		else
		{
			//-------------------------------------------
			// set the beginning odd bits.
			// Remember Intel Architecture (MSB -> LSB)
			if (shiftValue)
			{
				uint8_t startVal = 1;
				uint32_t repeatVal = (8-shiftValue);
				for (int32_t i=0;i<int32_t(repeatVal-1);i++)
				{
					startVal<<=1;
					startVal++;
				}
			
				//----------------------------------------
				// Shift bits to correct place.
				uint8_t bits = startVal;
				bits <<= shiftValue;
				*bitResult |= bits;
				length -= repeatVal;
			}
		
			while (length >= 8)
			{
				bitResult++;
				*bitResult |= 0xff;
				length-=8;
			}
			
			if (length)
			{
				bitResult++;
				uint8_t startVal = 1;
				for (int32_t i=0;i<int32_t(length-1);i++)
				{
					startVal<<=1;
					startVal++;
				}
			
				//----------------------------------------
				// Shift bits to correct place.
				uint8_t bits = startVal;
				*bitResult |= bits;
			}
		}
	}
}

//------------------------------------------------------------------------
uint8_t BitFlag::getFlag (uint32_t r, uint32_t c)
{
	if ((r < 0) || (r >= rows) || (c < 0) || (c > columns))
		return 0;
		
	//------------------------------------
	// Find out where we are getting from
	uint32_t location = (c>>3) + (r*colWidth);
	uint8_t shiftValue = (c % divValue);

	//-------------------------------------
	// Create mask to remove unwanted bits
	uint8_t mask = maskValue;
	mask <<= shiftValue;
		
	//-------------------------------------
	// get Location value
	uint8_t result = *(flagHeap->getHeapPtr() + location);
	
	//-------------------------------------
	// mask then shift Bits
	result &= mask;
	result >>= shiftValue;
	
	return(result);
}

//----------------------------------------------------------------------------------

//------------------------------------------------------------------------
// Class ByteFlag
int32_t ByteFlag::init (uint32_t numRows, uint32_t numColumns, uint32_t initialValue)
{
	rows = numRows;
	columns = numColumns;
	
	totalFlags = numRows * numColumns;
	totalRAM = totalFlags;

	flagHeap = new HeapManager;
	
	gosASSERT(flagHeap != NULL);
		
	int32_t result = flagHeap->createHeap(totalRAM);
	gosASSERT(result == NO_ERROR);
		
	result = flagHeap->commitHeap();
	gosASSERT(result == NO_ERROR);

	resetAll(initialValue);

	flagPane = new PANE;
	flagWindow = new WINDOW;
	
	flagPane->x0 = 0;
	flagPane->y0 = 0;
	flagPane->x1 = numColumns;
	flagPane->y1 = numRows;
	flagPane->window = flagWindow;
	
	flagWindow->buffer = flagHeap->getHeapPtr();
	flagWindow->x_max = numColumns-1;
	flagWindow->y_max = numRows-1;
	flagWindow->stencil = NULL;
	flagWindow->shadow = NULL;
		
	return(NO_ERROR);
}	

//------------------------------------------------------------------------
void ByteFlag::initTGA (PSTR tgaFileName)
{
	File tgaFile;
#ifdef _DEBUG
	int32_t result = 
#endif
		tgaFile.open(tgaFileName);

	gosASSERT(result == NO_ERROR);

	puint8_t tgaBuffer = (puint8_t)malloc(tgaFile.fileSize());
	if(tgaBuffer == NULL)
		return;

	tgaFile.read(tgaBuffer,tgaFile.fileSize());

	//---------------------------------------
	// Parse out TGAHeader.
	TGAFileHeader *header = (TGAFileHeader *)tgaBuffer;

	int32_t height = header->height;
	int32_t width = header->width;

	init(width,height,0);

	gosASSERT(header->image_type != UNC_TRUE);
	gosASSERT(header->image_type != RLE_TRUE);
	gosASSERT(header->image_type != RLE_PAL);

	switch (header->image_type)
	{
		case UNC_PAL:
		{
			//------------------------------------------------
			// This is just a bitmap.  Copy it into ourRAM.
			puint8_t image = tgaBuffer + sizeof(TGAFileHeader);
			if (header->color_map)
				image += header->cm_length * (header->cm_entry_size>>3);

			puint8_t ourRAM = memDump();
			memcpy(ourRAM,image,tgaFile.fileSize() - sizeof(TGAFileHeader) - (header->cm_length * (header->cm_entry_size>>3)));

			//------------------------------------------------------------------------
			// Must check image_descriptor to see if we need to un upside down image.
			bool left = (header->image_descriptor & 16) != 0;
			bool top = (header->image_descriptor & 32) != 0;

			if (!top && !left)
			{
				//--------------------------------
				// Image is Upside down.
				flipTopToBottom(ourRAM,header->pixel_depth,width,height);
			}
			else if (!top && left)
			{
				flipTopToBottom(ourRAM,header->pixel_depth,width,height);
				//flipRightToLeft(ourRAM,header->pixel_depth,width,height);
			}
			else if (top && left)
			{
				//flipRightToLeft(ourRAM,header->pixel_depth,width,height);
			}
		}
		break;
	}

	free(tgaBuffer);
}	

//------------------------------------------------------------------------
void ByteFlag::setCircle (uint32_t x, uint32_t y, uint32_t radius, uint8_t value)
{
	if (radius)
		AG_ellipse_fillOr(flagPane,x,y,radius,radius,value);
}	

//------------------------------------------------------------------------
void ByteFlag::clearCircle (uint32_t x, uint32_t y, uint32_t radius, uint8_t value)
{
	if (radius)
		AG_ellipse_fillXor(flagPane,x,y,radius,radius,value);
}	

//------------------------------------------------------------------------
void ByteFlag::resetAll (uint32_t initialValue)
{
	//------------------------------------------
	// Set memory to initial Flag Value
	if (initialValue == 0)
	{
		//memclear(flagHeap->getHeapPtr(),totalRAM);
		memset(flagHeap->getHeapPtr(), 0, totalRAM);
	}
	else
	{
		memset(flagHeap->getHeapPtr(),0xff,totalRAM);
	}
}

//------------------------------------------------------------------------
void ByteFlag::destroy (void)
{
	delete flagHeap;
	flagHeap = NULL;

	delete flagPane;
	delete flagWindow;

	flagPane = NULL;
	flagWindow = NULL;
	
	init();
}	

puint8_t ByteFlag::memDump (void)
{
	return (flagHeap->getHeapPtr());
}

//------------------------------------------------------------------------
// This sets location to bits
void ByteFlag::setFlag (uint32_t r, uint32_t c)
{
	if ((r < 0) || (r >= rows) || (c < 0) || (c > columns))
		return;
		
	//-------------------------------
	// find out where we are setting.
	uint32_t location = c + (r*columns);

	//----------------------------------------
	// find the location we care about.
	puint8_t bitResult = flagHeap->getHeapPtr();
	bitResult += location;

	//--------------------------------------------
	// Set is Zero because we use this to DRAW!!!
	*bitResult = 0xff;
}	
		
//------------------------------------------------------------------------
void ByteFlag::setGroup (uint32_t r, uint32_t c, uint32_t length)
{
	if (length)
	{
		if ((r < 0) || (r >= rows) || (c < 0) || (c > columns) || ((r*c+length) >= (rows*columns)))
			return;
		
		//-------------------------------
		// find out where we are setting.
		uint32_t location = c + (r*columns);
	
		//----------------------------------------
		// find the location we care about.
		puint8_t bitResult = flagHeap->getHeapPtr();
		bitResult += location;
	
		//--------------------------------------------------
		// We are only setting bits in the current location
		memset(bitResult,0xff,length);
	}
}

//------------------------------------------------------------------------
uint8_t ByteFlag::getFlag (uint32_t r, uint32_t c)
{
	if ((r < 0) || (r >= rows) || (c < 0) || (c > columns))
		return 0;
		
	//------------------------------------
	// Find out where we are getting from
	uint32_t location = c + (r*columns);

	//-------------------------------------
	// get Location value
	uint8_t result = *(flagHeap->getHeapPtr() + location);
	
	//-------------------
	// Just return bits.
	return (result);
}

//----------------------------------------------------------------------------------
