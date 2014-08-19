//******************************************************************************************
//	bitflag.h - This file contains the definitions for the bitflag class
//
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#ifndef BITFLAG_H
#define BITFLAG_H
//----------------------------------------------------------------------------------
// Include Files
#ifndef DBITFLAG_H
#include "dbitflag.h"
#endif

#ifndef HEAP_H
#include "heap.h"
#endif

#ifndef DSTD_H
#include "dstd.h"
#endif

#ifndef VFX_H
#include "vfx.h"
#endif

//----------------------------------------------------------------------------------
// Macro Definitions
#define BITS_PER_BYTE	8

#define NO_RAM_FOR_FLAG_HEAP	0xFFFA0000
#define NUM_BITS_NOT_SUPPORTED	0xFFFA0001

//----------------------------------------------------------------------------------
// Class Definitions
class BitFlag
{
	//Data Members
	//-------------
	protected:
	
		HeapManagerPtr		flagHeap;
		uint8_t		numBitsPerFlag;
		ULONG		rows;
		ULONG		columns;
		uint8_t		maskValue;
		ULONG		divValue;
		ULONG		colWidth;
		ULONG		totalFlags;
		ULONG		totalRAM;
		
	//Member Functions
	//-----------------
	public:
	
		void init (void)
		{
			flagHeap = NULL;
			numBitsPerFlag = 0;
			rows = columns = 0;
			maskValue = 0;
			divValue = 1;
			colWidth = 1;
		}
		
		BitFlag (void)
		{
			init();
		}
		
		long init (ULONG numRows, ULONG numColumns, ULONG initialValue = 0);
		void destroy (void);
		
		~BitFlag (void)
		{
			destroy();
		}
	
		void resetAll (ULONG bits);
			
		void setFlag (ULONG r, ULONG c);
		void clearFlag (ULONG r, ULONG c);
		
		void setGroup (ULONG r, ULONG c, ULONG length);
		void clearGroup (ULONG r, ULONG c, ULONG length);
		
		uint8_t getFlag (ULONG r, ULONG c);
};

//----------------------------------------------------------------------------------
class ByteFlag
{
	//Data Members
	//-------------
	protected:
	
		HeapManagerPtr		flagHeap;
		ULONG		rows;
		ULONG		columns;
		ULONG		totalFlags;
		ULONG		totalRAM;
		
		PANE				*flagPane;
		WINDOW				*flagWindow;
		
	//Member Functions
	//-----------------
	public:
	
		void init (void)
		{
			flagHeap = NULL;
			rows = columns = 0;
			
			flagPane = NULL;
			flagWindow = NULL;
		}
		
		ByteFlag (void)
		{
			init();
		}
		
		long init (ULONG numRows, ULONG numColumns, ULONG initialValue = 0);
		void initTGA (PSTR tgaFileName);

		void destroy (void);
		
		~ByteFlag (void)
		{
			destroy();
		}
	
		MemoryPtr memDump (void)
		{
			return (flagHeap->getHeapPtr());
		}
	
		long getWidth (void)
		{
			return rows;
		}

		long getHeight (void)
		{
			return columns;
		}

		void resetAll (ULONG byte);
			
		void setFlag (ULONG r, ULONG c);
		void clearFlag (ULONG r, ULONG c);
		
		void setGroup (ULONG r, ULONG c, ULONG length);
		void clearGroup (ULONG r, ULONG c, ULONG length);
		
		uint8_t getFlag (ULONG r, ULONG c);
		
		void setCircle (ULONG x, ULONG y, ULONG radius, uint8_t value);
		void clearCircle (ULONG x, ULONG y, ULONG radius, uint8_t value);
};

//----------------------------------------------------------------------------------
#endif
