//===========================================================================//
// File:	 MemoryManager.cpp												 //
// Contents: MemoryManager subsystem										 //
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#pragma once

#ifndef _MEMORYMANAGER_HPP_
#define _MEMORYMANAGER_HPP_

struct _MEMORYPOOL;
struct _HEAPHEADER;
struct _LARGEBLOCKHEADER;

//#define LAB_ONLY 1
//#define _DEBUG 1

//
// Single byte before allocations
//
// 0	 = Invalid (duplicate free detection)
// 1-252 = Heap number for a 'pool' allocated block
// 253   = Large block (not in pools)
// 254   = Block allocated before init.
// 255   = Unused
//
//
enum class _memorymanager_constants
{
	Magic_LargeBlock = 253, // Larger than 4k memory block (no in pools)
	Magic_BeforeInit = 254, // Allocated before inited  (was 0xabadcafe)

	// This is the largest 'pool' size - blocks larger than this use the system
	// allocator
	LargestMemoryBlock = 1024, // Largest memory block managed

	// This is the total number of pools being managed
	MemoryPools = 18,

	// Magic number placed after allocations in DEBUG/ARMOR
	MemoryEndMarker = 0x7fb1deaf, // Placed at the end of allocations in _ARMOR builds

};

#pragma pack(push, 1)
//
// Information block for all blocks in pools with < 256 bytes
//
typedef struct SMALLPOOLBLOCK
{
#if defined(LAB_ONLY)
	uint8_t Size; // Size of block (note pools are always <64K)
#endif
#ifdef _DEBUG
	DOUBLE dTimeStamp; // at what time was the block alloc'd
	uint32_t pContext[0]; // we'll allocate the right size based on
		// gMemoryStackWalkLevel
#endif
} SMALLPOOLBLOCK;
typedef SMALLPOOLBLOCK* PSMALLPOOLBLOCK;

//
// Information block for all blocks in pools with >= 256 bytes
//
typedef struct POOLBLOCK
{
#if defined(LAB_ONLY)
	uint16_t Size; // Size of block (note pools are always <64K)
#endif
#ifdef _DEBUG
	DOUBLE dTimeStamp; // at what time was the block alloc'd
	uint32_t pContext[0]; // we'll allocate the right size based on
		// gMemoryStackWalkLevel
#endif
} POOLBLOCK;
typedef POOLBLOCK* PPOOLBLOCK;

//
// Header for large blocks or blocks before memory manager inited.
//
typedef struct _LARGEBLOCK
{
	uint32_t Size; // Size of block (note can be any size)
	_LARGEBLOCKHEADER* pLast; // Pointer to previous large memory block
	_LARGEBLOCKHEADER* pNext; // Pointer to next large memory block
	uint8_t Heap;
#ifdef _DEBUG
	DOUBLE dTimeStamp; // at what time was the block alloc'd
	uint32_t pContext[0]; // we'll allocate the right size based on
		// gMemoryStackWalkLevel
#endif
} LARGEBLOCK;
typedef LARGEBLOCK* PLARGEBLOCK;

//
// Structure placed directly BEFORE each memory block
//
typedef struct _LARGEBLOCKHEADER
{
	LARGEBLOCK* pLargeBlockInfo;
	uint8_t LargeMagicNumber; // Magic_LargeBlock or Magic_BeforeInit
} LARGEBLOCKHEADER;
typedef LARGEBLOCKHEADER* PLARGEBLOCKHEADER;

//
// Structure used to hold information about 'pools' of memory blocks
//
typedef struct _MEMORYPOOL
{
	uint32_t HeapTag; // Tag so that memory is easily spotted in VM viewers
	_MEMORYPOOL* pLast; // Points to the last heap of the SAME block size
	_MEMORYPOOL* pNext; // Points to the next heap of the SAME block size
	POOLBLOCK* pInfoBlocks; // an array of blocks which describe particular
		// memory blocks (may point to a SMALLPOOLBLOCK
		// header for >256 bytes)
	puint8_t pMemoryPool; // Pointer to the base of the memory blocks (pointer
		// to header byte before allocation)
	uint16_t wBlockSize; // what is the size of the individual blocks?
	uint16_t wTotalBlocks; // Total blocks available
#if defined(LAB_ONLY)
	uint16_t wUserBytes; // the amount of memory in the pool that is actual user data
#endif
	uint16_t AllocCount[16]; // Number of blocks allocated in each 4K page (when
		// 0, block can be decommitted)
	uint16_t FreeBlockPtr; // Next available block in FreeBlockStack
	uint16_t FreeBlockStack[0]; // Free block offsets (from base of pool -
		// pointer to header byte before allocation)
} MEMORYPOOL;
typedef MEMORYPOOL* PMEMORYPOOL;

//
// Used to hold GameOS level heap information
//
typedef struct gos_Heap
{
	struct gos_Heap* pParent;
	struct gos_Heap* pNext;
	struct gos_Heap* pChild;
	uint32_t Magic;
	int32_t Instances;
	char Name[128];
#if defined(LAB_ONLY)
	uint32_t MaximumSize;
	int32_t BytesAllocated;
	int32_t TotalAllocatedLastLoop;
	int32_t BytesAllocatedThisLoop;
	int32_t BytesFreedThisLoop;
	int32_t AllocationsThisLoop;
	int32_t AllocationsLastLoop;
	int32_t TotalAllocations;
	int32_t PeakSize;
	uint8_t bExpanded; // this is used for collapsing tree in debugger
	uint8_t bExamined; // this is used for collapsing tree in debugger
	int32_t LargeAllocations;
	int32_t LargeAllocated;
	int32_t DXAllocations;
	int32_t DXAllocated;
#endif
	uint8_t HeapNumber; // Heap number in HeapList[]  (<<24)
} gos_Heap;
// typedef gos_Heap* Pgos_Heap;
typedef struct gos_Heap* HGOSHEAP;

#pragma pack(pop)

// extern HGOSHEAP AllHeaps;
extern uint16_t PoolSizes[MemoryPools];
// extern HGOSHEAP HeapList[256];
extern uint32_t LargeMemorySize;
extern uint32_t LargeMemoryAllocations;
extern uint32_t gMemoryBlockOverhead;
// extern HGOSHEAP Heap_Texture;
// extern HGOSHEAP Heap_Texture32;
// extern HGOSHEAP Heap_Texture16;
// extern HGOSHEAP Heap_TextureOther;
// extern HGOSHEAP Heap_Network;
// extern HGOSHEAP ParentGameOSHeap;
// extern HGOSHEAP DefaultHeap;
extern PMEMORYPOOL gMemoryPool[MemoryPools];

void __stdcall gos_ChangeHeapSize(HGOSHEAP Heap, size_t Change, uint8_t SystemAllocation = 0);
void __stdcall MM_CheckRegistered(void);
void __stdcall MM_Shutdown(void);
void __stdcall MM_UpdateStatistics(HGOSHEAP Heap);
void __stdcall MM_Startup(void);
void __stdcall SetupVirtualMemory(void);
void __stdcall AnalyzeWS(PSTR Title);

extern PPSAPI_WS_WATCH_INFORMATION pMemBlockInfo;
extern HANDLE HWSProcess;

#endif
