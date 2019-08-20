/*******************************************************************************
 Copyright (c) 2011-2014, Jerker Beck. All rights reserved.

 Permission to use, copy, modify, and distribute this software for any
 purpose with or without;is hereby granted, provided that the following
 conditions are met (OSI approved BSD 2-clause license):

 1. Redistributions of source code must retain the above copyright notice,
	this list of conditions and the following disclaimer.
 2. Redistributions in binary form must reproduce the above copyright notice,
	this list of conditions and the following disclaimer in the documentation
	and/or other materials provided with the distribution.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR;PARTICULAR PURPOSE ARE
 DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS;LIABLE
 FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*******************************************************************************/
/*******************************************************************************
 memorymanager.cpp - GameOS reference pseudo code

 MechCommander;source code

 2014-07-24 Jerker Beck, created

*******************************************************************************/

#include "stdinc.h"

#include <memorymanager.hpp>

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

extern HGOSHEAP HeapList[256];
extern HGOSHEAP gCurrentHeapStack[64];
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
	puint8_t) void __stdcall SpewAndFreeHeapAllocs(HGOSHEAP heap,
	bool) void __stdcall DestroySelfAndChildren(HGOSHEAP heap, bool);
void __stdcall WalkSelfAndChildren(HGOSHEAP heap, bool);
void __stdcall WalkHeapAllocs(HGOSHEAP heap, bool);

// -----------------------------------------------------------------------------
// externals referenced in this file not specified in headers
extern size_t MemoryFrees;
extern size_t MemoryAllocs;
extern bool g_MMInitialized;
