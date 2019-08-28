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
 mfcplatform.cpp - GameOS entry point functions

 MechCommander 2 source code

 2014-07-24 Jerker Back, created

*******************************************************************************/

#include "gameos.hpp"
#include "platform.hpp"
#include "mfcplatform.hpp"

extern "C" void WINAPI
InitGameOS(HINSTANCE hinstance, HWND hwnd, PSTR commandline)
{
	Platform = Platform_MFC;
	RunFromOtherApp(hinstance, hwnd, commandline);
}

#if CONSIDERED_OBSOLETE
PVOID __cdecl
operator new(size_t nSize, HGOSHEAP HeapBlock)
{
	return gos_Malloc(nSize, HeapBlock);
}

PVOID __cdecl
operator new(size_t nSize)
{
	return gos_Malloc(nSize, (HGOSHEAP)-1);
}

void __cdecl
operator delete(PVOID block)
{
	gos_Free(block);
}
#endif
