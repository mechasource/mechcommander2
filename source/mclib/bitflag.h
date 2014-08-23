//******************************************************************************************
//	bitflag.h - This file contains the definitions for the bitflag class
//
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#pragma once

#ifndef BITFLAG_H
#define BITFLAG_H

//#include "dbitflag.h"
//#include "heap.h"
//#include "dstd.h"
//#include "vfx.h"

//----------------------------------------------------------------------------------
// Macro Definitions
#define BITS_PER_BYTE	8

#define NO_RAM_FOR_FLAG_HEAP	0xFFFA0000
#define NUM_BITS_NOT_SUPPORTED	0xFFFA0001

//----------------------------------------------------------------------------------
// Class Definitions

// vfx.h
struct PANE;
struct WINDOW;
// heap.h
class HeapManager;

class BitFlag
{
	//Data Members
	//-------------
	protected:
		HeapManager*	flagHeap;
		uint8_t			numBitsPerFlag;
		uint32_t		rows;
		uint32_t		columns;
		uint8_t			maskValue;
		uint32_t		divValue;
		uint32_t		colWidth;
		uint32_t		totalFlags;
		uint32_t		totalRAM;
		
	//Member Functions
	//-----------------
	public:
		BitFlag(void)
		{
			init();
		}
		~BitFlag(void)
		{
			destroy();
		}
		void init(void)
		{
			flagHeap = NULL;
			numBitsPerFlag = 0;
			rows = columns = 0;
			maskValue = 0;
			divValue = 1;
			colWidth = 1;
		}
		int32_t init(ULONG numRows, ULONG numColumns, ULONG initialValue = 0);
		void destroy(void);
	
		void resetAll(ULONG bits);
			
		void setFlag(ULONG r, ULONG c);
		void clearFlag(ULONG r, ULONG c);
		
		void setGroup(ULONG r, ULONG c, ULONG length);
		void clearGroup(ULONG r, ULONG c, ULONG length);
		
		uint8_t getFlag(ULONG r, ULONG c);
};

//----------------------------------------------------------------------------------
class ByteFlag
{
	//Data Members
	//-------------
	protected:

		HeapManager*	flagHeap;
		uint32_t		rows;
		uint32_t		columns;
		uint32_t		totalFlags;
		uint32_t		totalRAM;
	
		PANE*			flagPane;
		WINDOW*			flagWindow;
		
	//Member Functions
	//-----------------
	public:
	
		void init(void)
		{
			flagHeap = NULL;
			rows = columns = 0;
			flagPane = NULL;
			flagWindow = NULL;
		}
		
		ByteFlag(void)
		{
			init();
		}
		
		int32_t init(ULONG numRows, ULONG numColumns, ULONG initialValue = 0);
		void initTGA(PSTR tgaFileName);

		void destroy(void);
		
		~ByteFlag(void)
		{
			destroy();
		}
	
		PUCHAR memDump(void);
	
		uint32_t getWidth(void)
		{
			return rows;
		}

		uint32_t getHeight(void)
		{
			return columns;
		}

		void resetAll(ULONG byte);
			
		void setFlag(ULONG r, ULONG c);
		void clearFlag(ULONG r, ULONG c);
		
		void setGroup(ULONG r, ULONG c, ULONG length);
		void clearGroup(ULONG r, ULONG c, ULONG length);
		
		uint8_t getFlag(ULONG r, ULONG c);
		
		void setCircle(ULONG x, ULONG y, ULONG radius, uint8_t value);
		void clearCircle(ULONG x, ULONG y, ULONG radius, uint8_t value);
};

//----------------------------------------------------------------------------------
#endif
