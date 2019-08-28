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
 gameplatform.cpp - gameos entry point functions

 MechCommander 2 source code

 2014-07-24 Jerker Back, created

*******************************************************************************/

#include "gameos.hpp"
#include "platform.hpp"

extern "C" int WINAPI
WinMain(_In_ HINSTANCE hinstance, _In_opt_ HINSTANCE hPrevInstance,
	_In_ PSTR commandline, _In_ int nShowCmd)
{
	Platform = Platform_Game;
	RunFromWinMain(hinstance, hPrevInstance, commandline, nShowCmd);
	return 0;
}

#if CONSIDERED_OBSOLETE
extern bool gViaNew;

PVOID __cdecl
operator new(size_t nSize, HGOSHEAP HeapBlock)
{
	gViaNew = true;
	return gos_Malloc(nSize, HeapBlock);
}

PVOID __cdecl
operator new(size_t nSize)
{
	gViaNew = true;
	return gos_Malloc(nSize, (HGOSHEAP)-1);
}

void __cdecl
operator delete(PVOID block)
{
	gos_Free(block);
}

void __cdecl
operator delete[](PVOID block)
{
	operator delete(block);
}
#endif
