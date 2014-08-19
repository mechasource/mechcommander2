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

extern void AG_ellipse_draw(PANE *pane, LONG xc, LONG yc, LONG width, LONG height, LONG color);
extern void AG_ellipse_fill(PANE *pane, LONG xc, LONG yc, LONG width, LONG height, LONG color);
extern void AG_ellipse_fillXor(PANE *pane, LONG xc, LONG yc, LONG width, LONG height, LONG color);
extern void AG_ellipse_fillOr(PANE *pane, LONG xc, LONG yc, LONG width, LONG height, LONG color);
extern void memclear(void *Dest,int Length);
//------------------------------------------------------------------------
// Class BitFlag
long BitFlag::init (ULONG numRows, ULONG numColumns, ULONG initialValue)
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
		
	long result = flagHeap->createHeap(totalRAM);
	if (result != NO_ERR)
		return(result);
		
	result = flagHeap->commitHeap();
	if (result != NO_ERR)
		return(result);

	resetAll(initialValue);
		
	return(NO_ERR);
}	

//------------------------------------------------------------------------
void BitFlag::resetAll (ULONG initialValue)
{
	//------------------------------------------
	// Set memory to initial Flag Value
	if (initialValue == 0)
	{
		memclear(flagHeap->getHeapPtr(),totalRAM);
		maskValue = 1;
	}
	else
	{
		//-----------------------------------------
		// Not zero, must jump through hoops.
		maskValue = 1;
		memset(flagHeap->getHeapPtr(),0xff,totalRAM);
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
void BitFlag::setFlag (ULONG r, ULONG c)
{
	if ((r < 0) || (r >= rows) || (c < 0) || (c > columns))
		return;
		
	//-------------------------------
	// find out where we are setting.
	ULONG location = (c>>3) + (r*colWidth);
	uint8_t shiftValue = (c % divValue);

	//----------------------------------------
	// find the location we care about.
	PUCHAR bitResult = flagHeap->getHeapPtr();
	bitResult += location;

	//----------------------------------------
	// Shift bits to correct place.
	uint8_t bits = 1;
	bits <<= shiftValue;
	*bitResult |= bits;
}	
		
//------------------------------------------------------------------------
void BitFlag::setGroup (ULONG r, ULONG c, ULONG length)
{
	if (length)
	{
		if ((r < 0) || (r >= rows) || (c < 0) || (c > columns) || ((r*c+length) >= (rows*columns)))
			return;
		
		//-------------------------------
		// find out where we are setting.
		ULONG location = (c>>3) + (r*colWidth);
		uint8_t shiftValue = (c % divValue);
	
		//----------------------------------------
		// find the location we care about.
		PUCHAR bitResult = flagHeap->getHeapPtr();
		bitResult += location;
	
		//--------------------------------------------------
		// We are only setting bits in the current location
		if ((8 - shiftValue) >= (long)length)
		{
			uint8_t startVal = 1;
			ULONG repeatVal = length;
			for (long i=0;i<long(repeatVal-1);i++)
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
				ULONG repeatVal = (8-shiftValue);
				for (long i=0;i<long(repeatVal-1);i++)
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
				for (long i=0;i<long(length-1);i++)
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
uint8_t BitFlag::getFlag (ULONG r, ULONG c)
{
	if ((r < 0) || (r >= rows) || (c < 0) || (c > columns))
		return 0;
		
	//------------------------------------
	// Find out where we are getting from
	ULONG location = (c>>3) + (r*colWidth);
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
long ByteFlag::init (ULONG numRows, ULONG numColumns, ULONG initialValue)
{
	rows = numRows;
	columns = numColumns;
	
	totalFlags = numRows * numColumns;
	totalRAM = totalFlags;

	flagHeap = new HeapManager;
	
	gosASSERT(flagHeap != NULL);
		
	long result = flagHeap->createHeap(totalRAM);
	gosASSERT(result == NO_ERR);
		
	result = flagHeap->commitHeap();
	gosASSERT(result == NO_ERR);

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
		
	return(NO_ERR);
}	

//------------------------------------------------------------------------
void ByteFlag::initTGA (PSTR tgaFileName)
{
	File tgaFile;
#ifdef _DEBUG
	long result = 
#endif
		tgaFile.open(tgaFileName);

	gosASSERT(result == NO_ERR);

	MemoryPtr tgaBuffer = (MemoryPtr)malloc(tgaFile.fileSize());
	if(tgaBuffer == NULL)
		return;

	tgaFile.read(tgaBuffer,tgaFile.fileSize());

	//---------------------------------------
	// Parse out TGAHeader.
	TGAFileHeader *header = (TGAFileHeader *)tgaBuffer;

	long height = header->height;
	long width = header->width;

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
			MemoryPtr image = tgaBuffer + sizeof(TGAFileHeader);
			if (header->color_map)
				image += header->cm_length * (header->cm_entry_size>>3);

			MemoryPtr ourRAM = memDump();
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
void ByteFlag::setCircle (ULONG x, ULONG y, ULONG radius, uint8_t value)
{
	if (radius)
		AG_ellipse_fillOr(flagPane,x,y,radius,radius,value);
}	

//------------------------------------------------------------------------
void ByteFlag::clearCircle (ULONG x, ULONG y, ULONG radius, uint8_t value)
{
	if (radius)
		AG_ellipse_fillXor(flagPane,x,y,radius,radius,value);
}	

//------------------------------------------------------------------------
void ByteFlag::resetAll (ULONG initialValue)
{
	//------------------------------------------
	// Set memory to initial Flag Value
	if (initialValue == 0)
	{
		memclear(flagHeap->getHeapPtr(),totalRAM);
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
		
//------------------------------------------------------------------------
// This sets location to bits
void ByteFlag::setFlag (ULONG r, ULONG c)
{
	if ((r < 0) || (r >= rows) || (c < 0) || (c > columns))
		return;
		
	//-------------------------------
	// find out where we are setting.
	ULONG location = c + (r*columns);

	//----------------------------------------
	// find the location we care about.
	PUCHAR bitResult = flagHeap->getHeapPtr();
	bitResult += location;

	//--------------------------------------------
	// Set is Zero because we use this to DRAW!!!
	*bitResult = 0xff;
}	
		
//------------------------------------------------------------------------
void ByteFlag::setGroup (ULONG r, ULONG c, ULONG length)
{
	if (length)
	{
		if ((r < 0) || (r >= rows) || (c < 0) || (c > columns) || ((r*c+length) >= (rows*columns)))
			return;
		
		//-------------------------------
		// find out where we are setting.
		ULONG location = c + (r*columns);
	
		//----------------------------------------
		// find the location we care about.
		PUCHAR bitResult = flagHeap->getHeapPtr();
		bitResult += location;
	
		//--------------------------------------------------
		// We are only setting bits in the current location
		memset(bitResult,0xff,length);
	}
}

//------------------------------------------------------------------------
uint8_t ByteFlag::getFlag (ULONG r, ULONG c)
{
	if ((r < 0) || (r >= rows) || (c < 0) || (c > columns))
		return 0;
		
	//------------------------------------
	// Find out where we are getting from
	ULONG location = c + (r*columns);

	//-------------------------------------
	// get Location value
	uint8_t result = *(flagHeap->getHeapPtr() + location);
	
	//-------------------
	// Just return bits.
	return (result);
}

//----------------------------------------------------------------------------------
