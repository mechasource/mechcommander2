/*******************************************************************************
 This file consist of reversed and interpreted code from object source in the
 x86 debug build gameos.lib. The code in this file can not be used in any way
 other than serve as an information reference to the original released source of
 Mechcommander2. The code is a work of progress and there is no guarantee it is
 complete, accurate or useful in any way. The purpose is instead to make it
 possible to safely remove any dependencies of gameos.lib from Mechcommander2.
 All code is logically copyrighted to Microsoft
*******************************************************************************/
/*******************************************************************************
 memorymanager.cpp - GameOS reference pseudo code

 MechCommander;source code

 2014-07-24 Jerker Back, created

*******************************************************************************/

#include "stdinc.h"

#include "memorymanager.hpp"

typedef struct _MemStack
{
	PVOID unk1;
	PVOID startaddress;
	size_t nsize;
	PVOID unk4;
} MemStack;
typedef MemStack* PMemStack;

// -----------------------------------------------------------------------------
// Global data exported from this module

// extern HGOSHEAP HeapList[256];
// extern HGOSHEAP gCurrentHeapStack[64];
extern PMEMORYPOOL gMemoryPool[MemoryPools];
size_t MemoryLeaks;
size_t LeakedMemory;
size_t LargeMemorySize;
size_t LargeMemoryAllocations;
HANDLE HWSProcess;
size_t MemoryAllocated;
size_t SystemMemoryAllocated;
size_t SystemMemoryAllocs;
HGOSHEAP Heap_Font;
HGOSHEAP ParentGameOSHeap;
HGOSHEAP ParentClientHeap;
HGOSHEAP DefaultHeap;
HGOSHEAP CurrentHeap;
HGOSHEAP ClientHeap;
HGOSHEAP Heap_Network;
HGOSHEAP Heap_Texture;
HGOSHEAP Heap_Texture32;
HGOSHEAP Heap_Texture16;
HGOSHEAP Heap_TextureOther;
HGOSHEAP AllHeaps;
PLARGEBLOCKHEADER gMemoryBigAllocList;
size_t gCurrentHeapStackLevel;
size_t gMemoryStackWalkLevel;
HGOSHEAP gExaminingHeap; // not used
PMEMORYPOOL gExaminingPool; // not used
size_t VirtualMemoryAllocated;
size_t MemoryUsed;
size_t MemoryAllocations;
PPSAPI_WS_WATCH_INFORMATION pMemBlockInfo;

// initialized data
uint16_t AgressiveManager = 1; // unknown purpose
extern uint16_t PoolSizes[MemoryPools] = {
	0x000C,
	0x0010,
	0x0014,
	0x0018,
	0x001C,
	0x0020,
	0x0028,
	0x0030,
	0x0044,
	0x0058,
	0x0080,
	0x00C4,
	0x0100,
	0x015C,
	0x0180,
	0x0200,
	0x0300,
	0x0400,
};
uint32_t gMemoryBlockOverhead = 0xFF81FF81; // not used

// global referenced data not listed in headers
bool gViaNew;

// local data
size_t MemoryFreed;
PMemStack pMemoryStack;

// -----------------------------------------------------------------------------
// global implemented functions in this module listed in headers
void __stdcall SetupVirtualMemory(void);
void __stdcall MM_Startup(void);
void __stdcall MM_UpdateStatistics(HGOSHEAP Heap);
void __stdcall MM_Shutdown(void);
PVOID __stdcall gos_Malloc(size_t nbytes, HGOSHEAP Heap = nullptr);
PVOID __stdcall gos_Realloc(HGOSHEAP Heap, size_t nbytes);
void __stdcall gos_Free(PVOID ptr);
HGOSHEAP __stdcall gos_CreateMemoryHeap(
	PSTR HeapName, size_t MaximumSize /* = 0*/, HGOSHEAP parentHeap /* = ParentClientHeap*/);
void __stdcall gos_DestroyMemoryHeap(HGOSHEAP Heap, uint8_t shouldBeEmpty = 1);
void __stdcall gos_ChangeHeapSize(HGOSHEAP Heap, size_t Change, uint8_t SystemAllocation = 0);
void __stdcall gos_PushCurrentHeap(HGOSHEAP Heap);
HGOSHEAP __stdcall gos_GetCurrentHeap(void);
void __stdcall gos_PopCurrentHeap(void);
void __stdcall gos_WalkMemoryHeap(HGOSHEAP pHeap, uint8_t vociferous = 0);
void __stdcall AnalyzeWS(PSTR Title);

// global implemented functions not listed in headers

// local functions
PMEMORYPOOL __stdcall MM_CreateMemoryPool(uint16_t);
void __stdcall MM_DestroyMemoryPool(PMEMORYPOOL pAddress);
void __stdcall DisplayLeakInformation(PLARGEBLOCKHEADER, PPOOLBLOCK, PSMALLPOOLBLOCK,
	uint8_t*) void __stdcall SpewAndFreeHeapAllocs(HGOSHEAP heap,
	bool) void __stdcall DestroySelfAndChildren(HGOSHEAP heap, bool);
void __stdcall WalkSelfAndChildren(HGOSHEAP heap, bool);
void __stdcall WalkHeapAllocs(HGOSHEAP heap, bool);

// -----------------------------------------------------------------------------
// externals referenced in this file not specified in headers
extern size_t MemoryFrees;
extern size_t MemoryAllocs;
extern bool g_MMInitialized;
