//---------------------------------------------------------------------------
//
// Heap.h -- This file contains the definition for the Base HEAP
//		 			Manager Class.  The Base HEAP manager creates,
//					manages and destroys block of memory using Win32
//					virtual memory calls.
//
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#pragma once

#ifndef HEAP_H
#define HEAP_H

//#include "dstd.h"
//#include "dheap.h"
//#include <memory.h>
//#include <gameos.hpp>

//---------------------------------------------------------------------------
// Macro Definitions

#define OUT_OF_MEMORY 0xBADD0001
#define ALLOC_ZERO 0xBADD0002
#define ALLOC_OVERFLOW 0xBADD0003
#define NULL_FREE_LIST 0xBADD0004
#define HEAP_CORRUPTED 0xBADD0005
#define BAD_FREED_PTR 0xBADD0006
#define ALLOC_TOO_BIG 0xBADD0007
#define COULDNT_COMMIT 0xBADD0008
#define COULDNT_CREATE 0xBADD0009

#ifdef _DEBUG
#define allocatedBlockSize 12
#else
#define allocatedBlockSize 8
#endif

#define BASE_HEAP 0
#define USER_HEAP 1

#define MAX_HEAPS 256
//---------------------------------------------------------------------------

struct HeapBlock;
typedef HeapBlock* HeapBlockPtr;
class HeapManager;
typedef HeapManager* HeapManagerPtr;
class UserHeap;
typedef UserHeap* UserHeapPtr;

extern UserHeapPtr systemHeap;

//---------------------------------------------------------------------------
// Additional Debug Information
#ifdef _DEBUG
#define NUMMEMRECORDS 128000

typedef struct _memRecord
{
	PVOID ptr;
	size_t size;
	uint32_t stack[12];
} memRecord;
#endif

typedef struct _GlobalHeapRec
{
	HeapManagerPtr thisHeap;
	size_t heapSize;
	uint32_t totalCoreLeft;
	uint32_t coreLeft;
} GlobalHeapRec;

//---------------------------------------------------------------------------
// Class Definitions
class HeapManager
{
	// Data Members
	//-------------
protected:
	puint8_t heap;
	bool memReserved;
	size_t totalSize;
	size_t committedSize;
	uint32_t whoMadeMe;

	//		BOOL	VMQuery (PVOID pvAddress, PVMQUERY pVMQ);
	//		PCSTR   GetMemStorageText (uint32_t dwStorage);
	//		PSTR	GetProtectText (uint32_t dwProtect, PSTR szBuf, BOOL
	// fShowFlags);
	//		void	ConstructRgnInfoLine (PVMQUERY pVMQ, PSTR szLine, int32_t
	// nMaxLen); 		void	ConstructBlkInfoLine (PVMQUERY pVMQ, PSTR
	// szLine, int32_t nMaxLen);

public:
	HeapManagerPtr nxt;

	// Member Functions
	//-----------------
public:
	HeapManager(void) { init(void); }
	virtual ~HeapManager(void) { destroy(void); }

	void destroy(void);
	void init(void);
	int32_t createHeap(uint32_t memSize);
	int32_t commitHeap(uint32_t commitSize = 0);
	int32_t decommitHeap(uint32_t decommitSize = 0);

	puint8_t getHeapPtr(void);
	operator puint8_t(void);

	void HeapManager::MemoryDump(void);

	virtual uint8_t heapType(void) { return BASE_HEAP; }

	uint32_t owner(void) { return whoMadeMe; }

	size_t tSize(void) { return committedSize; }
};

//---------------------------------------------------------------------------
typedef struct HeapBlock
{
	size_t blockSize;
	HeapBlockPtr upperBlock;
	HeapBlockPtr previous;
	HeapBlockPtr next;
} HeapBlock;

//---------------------------------------------------------------------------
class UserHeap : public HeapManager
{
	// Data Members
	//-------------
protected:
	HeapBlockPtr heapStart;
	HeapBlockPtr heapEnd;
	HeapBlockPtr firstNearBlock;
	size_t heapSize;
	bool mallocFatals;
	int32_t heapState;
	PSTR heapName;
	bool useGOSGuardPage;
	HGOSHEAP gosHeap;

#ifdef _DEBUG
	memRecord* recordArray;
	int32_t recordCount;
	bool logMallocs;
#endif

	// Member Functions
	//-----------------
protected:
	void relink(HeapBlockPtr newBlock);
	void unlink(HeapBlockPtr oldBlock);
	bool mergeWithLower(HeapBlockPtr block);

public:
	UserHeap(void);
	int32_t init(uint32_t memSize, PSTR heapId = nullptr, bool useGOS = false);

	~UserHeap(void);
	void destroy(void);

	uint32_t totalCoreLeft(void);
	uint32_t coreLeft(void);
	size_t size(void) { return heapSize; }

	PVOID Malloc(size_t memSize);
	int32_t Free(PVOID memBlock);

	PVOID calloc(size_t memSize);

	void walkHeap(bool printIt = false, bool skipAllocated = false);

	HRESULT getLastError(void);

	bool heapReady(void) { return (heapSize != 0); }

	void setMallocFatals(bool fatalFlag) { mallocFatals = fatalFlag; }

	virtual uint8_t heapType(void) { return USER_HEAP; }

	PSTR getHeapName(void) { return heapName; }

	bool pointerOnHeap(PVOID ptr);

#ifdef _DEBUG
	void startHeapMallocLog(void); // This function will start recoding each malloc and
	// free to insure that there are no leaks.

	void stopHeapMallocLog(void); // This can be used to suspend logging when mallocs
	// that are not desired to be logged are called.

	void dumpRecordLog(void);

#else
	void startHeapMallocLog(void) // DOES NOTHING IN RELEASE/PROFILE BUILDS!
	{
	}

	void stopHeapMallocLog(void) {}

	void dumpRecordLog(void) {}
#endif
};

//---------------------------------------------------------------------------
// This class tracks each heap which is created to provide more accurate
// memory map information about Honor Bound.
// NOTE this class just records heaps.  It does NO allocating of its own.
class HeapList
{
	// Data Members
	//-------------
protected:
	static GlobalHeapRec heapRecords[MAX_HEAPS];
	static bool heapInstrumented;

	// Member Functions
	//----------------
public:
	void init(void)
	{
		memset(heapRecords, 0, sizeof(GlobalHeapRec) * MAX_HEAPS);
		heapInstrumented = false;
	}

	HeapList(void) { init(void); }

	void destroy(void) { init(void); }

	~HeapList(void) { destroy(void); }

	void addHeap(HeapManagerPtr newHeap);
	void removeHeap(HeapManagerPtr oldHeap);

	void update(void); // Called every frame in Debug to monitor heaps!

	void dumpLog(void);

	static void initializeStatistics(void);
};

//---------------------------------------------------------------------------
typedef HeapList* HeapListPtr;
extern HeapListPtr globalHeapList;

//---------------------------------------------------------------------------
#endif

//---------------------------------------------------------------------------
//
// Edit Log
//
//---------------------------------------------------------------------------
