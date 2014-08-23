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

#define OUT_OF_MEMORY	0xBADD0001
#define ALLOC_ZERO		0xBADD0002
#define ALLOC_OVERFLOW	0xBADD0003
#define NULL_FREE_LIST	0xBADD0004
#define HEAP_CORRUPTED	0xBADD0005
#define BAD_FREED_PTR	0xBADD0006
#define ALLOC_TOO_BIG	0xBADD0007
#define COULDNT_COMMIT	0xBADD0008
#define COULDNT_CREATE	0xBADD0009

#ifdef _DEBUG
#define allocatedBlockSize 12
#else
#define allocatedBlockSize 8
#endif

#define BASE_HEAP		0
#define USER_HEAP		1

#define MAX_HEAPS		256
//---------------------------------------------------------------------------

struct HeapBlock;
typedef HeapBlock*		HeapBlockPtr;
class HeapManager;
typedef HeapManager*	HeapManagerPtr;
class UserHeap;
typedef UserHeap*		UserHeapPtr;

extern UserHeapPtr		systemHeap;

//---------------------------------------------------------------------------
// Additional Debug Information
#ifdef _DEBUG
#define NUMMEMRECORDS 128000

typedef struct _memRecord 
{
	PVOID			ptr;
	size_t			size;
	ULONG			stack[12];
} memRecord;
#endif

typedef struct _GlobalHeapRec
{
	HeapManagerPtr 	thisHeap;
	size_t	heapSize;
	ULONG   totalCoreLeft;
	ULONG	coreLeft;
} GlobalHeapRec;

//---------------------------------------------------------------------------
// Class Definitions
class HeapManager
{
	//Data Members
	//-------------
protected:

	PUCHAR			heap;
	bool			memReserved;
	size_t   		totalSize;
	size_t   		committedSize;
	ULONG			whoMadeMe;

	//		BOOL	VMQuery (PVOID pvAddress, PVMQUERY pVMQ);
	//		LPCTSTR GetMemStorageText (ULONG dwStorage);
	//		LPTSTR	GetProtectText (ULONG dwProtect, LPTSTR szBuf, BOOL fShowFlags);
	//		void	ConstructRgnInfoLine (PVMQUERY pVMQ, LPTSTR szLine, int nMaxLen);
	//		void	ConstructBlkInfoLine (PVMQUERY pVMQ, LPTSTR szLine, int nMaxLen);

public:
	HeapManagerPtr			nxt;

	//Member Functions
	//-----------------
public:

	HeapManager(void)
	{
		init();
	}
	virtual ~HeapManager(void)
	{
		destroy();
	}

	void destroy (void);
	void init (void);
	int32_t createHeap (ULONG memSize);
	int32_t commitHeap (ULONG commitSize = 0);
	int32_t decommitHeap (ULONG decommitSize = 0);

	PUCHAR getHeapPtr (void);
	operator PUCHAR (void);

	void HeapManager::MemoryDump();

	virtual uint8_t heapType (void)
	{
		return BASE_HEAP;
	}

	ULONG owner (void)
	{
		return whoMadeMe;
	}

	size_t tSize (void)
	{
		return committedSize;
	}

};

//---------------------------------------------------------------------------
typedef struct HeapBlock {
	size_t 			blockSize;
	HeapBlockPtr 	upperBlock;
	HeapBlockPtr	previous;
	HeapBlockPtr	next;
} HeapBlock;

//---------------------------------------------------------------------------
class UserHeap : public HeapManager
{
	//Data Members
	//-------------	
protected:
	HeapBlockPtr		heapStart;
	HeapBlockPtr		heapEnd;
	HeapBlockPtr		firstNearBlock;
	size_t				heapSize;
	bool				mallocFatals;
	int32_t				heapState;
	PSTR				heapName;
	bool				useGOSGuardPage;
	HGOSHEAP			gosHeap;

#ifdef _DEBUG
	memRecord*			recordArray;
	int32_t				recordCount;
	bool				logMallocs;
#endif

	//Member Functions
	//-----------------
protected:
	void	relink (HeapBlockPtr newBlock);
	void	unlink (HeapBlockPtr oldBlock);
	bool	mergeWithLower (HeapBlockPtr block);

public:

	UserHeap (void);
	int32_t init (ULONG memSize, PSTR heapId = NULL, bool useGOS = false);

	~UserHeap (void);
	void destroy (void);

	ULONG totalCoreLeft (void);
	ULONG coreLeft (void);
	size_t size (void) { return heapSize;}

	PVOID Malloc (size_t memSize);
	int32_t Free (PVOID memBlock);

	PVOID calloc (size_t memSize);

	void walkHeap (bool printIt = FALSE, bool skipAllocated = FALSE);

	HRESULT getLastError (void);

	bool heapReady (void)
	{
		return (heapSize != 0);
	}

	void setMallocFatals (bool fatalFlag)
	{
		mallocFatals = fatalFlag;
	}

	virtual uint8_t heapType (void)
	{
		return USER_HEAP;
	}

	PSTR getHeapName (void)
	{
		return heapName;
	}

	bool pointerOnHeap (PVOID ptr);

#ifdef _DEBUG
	void startHeapMallocLog (void);		//This function will start recoding each malloc and
	//free to insure that there are no leaks.

	void stopHeapMallocLog (void);		//This can be used to suspend logging when mallocs
	//that are not desired to be logged are called.

	void dumpRecordLog (void);

#else
	void startHeapMallocLog (void)		//DOES NOTHING IN RELEASE/PROFILE BUILDS!
	{
	}

	void stopHeapMallocLog (void)
	{
	}

	void dumpRecordLog (void)
	{
	}
#endif
};

//---------------------------------------------------------------------------
// This class tracks each heap which is created to provide more accurate
// memory map information about Honor Bound.
// NOTE this class just records heaps.  It does NO allocating of its own.
class HeapList
{
	//Data Members
	//-------------
protected:
	static GlobalHeapRec heapRecords[MAX_HEAPS];
	static bool heapInstrumented;

	//Member Functions
	//----------------
public:
	void init (void)
	{
		memset(heapRecords,0,sizeof(GlobalHeapRec)*MAX_HEAPS);

		heapInstrumented = false;
	}

	HeapList (void)
	{
		init();
	}

	void destroy (void)
	{
		init();
	}

	~HeapList (void)
	{
		destroy();
	}

	void addHeap (HeapManagerPtr newHeap);
	void removeHeap (HeapManagerPtr oldHeap);

	void update (void);			//Called every frame in Debug to monitor heaps!

	void dumpLog (void);

	static void initializeStatistics();

};

//---------------------------------------------------------------------------
typedef HeapList*	HeapListPtr;
extern HeapListPtr	globalHeapList;

//---------------------------------------------------------------------------
#endif

//---------------------------------------------------------------------------
//
// Edit Log
//
//---------------------------------------------------------------------------

