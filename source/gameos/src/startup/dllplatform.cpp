/*******************************************************************************
 This file consist of reversed and interpreted code from object source in the
 x86 debug build gameos.lib. The code in this file can not be used in any way
 other than serve as an information reference to the original released source of
 Mechcommander2. The code is a work of progress and there is no guarantee it is
 complete, accurate or useful in any way. The purpose is instead to make it
 possible to safely remove any dependencies of gameos.lib from Mechcommander2.
*******************************************************************************/
/*******************************************************************************
 dllplatform.cpp - gameos entry point functions

 MechCommander 2 source code

 2014-07-24 Jerker Beck, created

*******************************************************************************/

#include <gameos.hpp>
#include <platform.hpp>
#include <dllplatform.hpp>

extern "C" void WINAPI InitGameOS(HINSTANCE hInstance, HWND hWindow, PSTR CommandLine)
{
	Platform = Platform_DLL;
	RunFromOtherApp(hInstance, hWindow, CommandLine);
}

#if _CONSIDERED_OBSOLETE
PVOID __cdecl operator new(size_t size, HGOSHEAP /*Heap*/) { return malloc(Size); }

PVOID __cdecl operator new(size_t Size) { return malloc(Size); }

void __cdecl operator delete(PVOID Memory) { free(Memory); }
#endif
