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

//------------------------------------------------------------------------------
// Macro Definitions

enum bitflag_const : uint32_t
{
	BITS_PER_BYTE		   = 8,
	NO_RAM_FOR_FLAG_HEAP   = 0xFFFA0000,
	NUM_BITS_NOT_SUPPORTED = 0xFFFA0001
};

//-------------------------------------------------------------------------------
// Class Definitions

// vfx.h
struct PANE;
struct WINDOW;
// heap.h
class HeapManager;

class BitFlag
{
protected:
	HeapManager* flagHeap;
	uint8_t numBitsPerFlag;
	uint32_t rows;
	uint32_t columns;
	uint8_t maskValue;
	uint32_t divValue;
	uint32_t colWidth;
	uint32_t totalFlags;
	uint32_t totalRAM;

public:
	BitFlag(void)
		: flagHeap(nullptr), numBitsPerFlag(0), rows(0), columns(0), maskValue(0), divValue(1),
		  colWidth(1)
	{
	}
	~BitFlag(void) { destroy(); }

	int32_t init(uint32_t numRows, uint32_t numColumns, uint32_t initialValue = 0);
	void destroy();

	void resetAll(uint32_t bits);

	void setFlag(uint32_t r, uint32_t c);
	void clearFlag(uint32_t r, uint32_t c);

	void setGroup(uint32_t r, uint32_t c, uint32_t length);
	void clearGroup(uint32_t r, uint32_t c, uint32_t length);

	uint8_t getFlag(uint32_t r, uint32_t c);
};

// -----------------------------------------------------------------------------
class ByteFlag
{
protected:
	HeapManager* flagHeap;
	uint32_t rows;
	uint32_t columns;
	uint32_t totalFlags;
	uint32_t totalRAM;
	PANE* flagPane;
	WINDOW* flagWindow;

public:
	ByteFlag(void) : flagHeap(nullptr), rows(0), columns(0), flagPane(nullptr), flagWindow(nullptr)
	{
	}
	~ByteFlag(void) { destroy(); }

	int32_t init(uint32_t numRows, uint32_t numColumns, uint32_t initialValue = 0);
	void initTGA(PSTR tgaFileName);

	void destroy(void);

	puint8_t memDump(void);

	uint32_t getWidth(void) { return rows; }
	uint32_t getHeight(void) { return columns; }

	void resetAll(uint32_t byte);

	void setFlag(uint32_t r, uint32_t c);
	void clearFlag(uint32_t r, uint32_t c);

	void setGroup(uint32_t r, uint32_t c, uint32_t length);
	void clearGroup(uint32_t r, uint32_t c, uint32_t length);

	uint8_t getFlag(uint32_t r, uint32_t c);

	void setCircle(uint32_t x, uint32_t y, uint32_t radius, uint8_t value);
	void clearCircle(uint32_t x, uint32_t y, uint32_t radius, uint8_t value);
};

#endif
