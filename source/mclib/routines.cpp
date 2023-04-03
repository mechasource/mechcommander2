//
//
// Faster versions of common routines
//
//
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#include "stdinc.h"

// #include <stdlib.h>
//#include "gameos.hpp"

// void memclear(PVOID Dest,size_t Length);
// void memfill(PVOID Dest,size_t Length);

extern enum { CPU_UNKNOWN,
	CPU_PENTIUM,
	CPU_MMX,
	CPU_KATMAI } Processor;

static int64_t fillnum = -1;

#if CONSIDERED_OBSOLETE
//
// Instead of using memset(x,0,x) - use this function
//
void memclear(PVOID Dest, size_t Len) {_asm {

	mov edi, Dest mov ecx, Len cmp Processor,
	CPU_MMX jnz mem1

		// memclear_mmx:
		mov ebx,
	ecx;
8 byte align edi when possible sub ecx, edi xor eax, eax sub ecx, ebx and ecx, 7 sub ebx,
	ecx jle doalign0 test ecx, ecx jz mmx0a mmx0 : mov byte ptr[edi], al inc edi dec ecx jnz mmx0 mmx0a : mov ecx, ebx and ebx, 7 shr ecx, 3 jz doalign0 pxor mm0, mm0 mmx1 : movq[edi], mm0 add edi, 8 dec ecx jnz mmx1 doalign0 : add ecx, ebx test ecx, ecx jz mmx0d mmx0c : mov byte ptr[edi], al inc edi dec ecx jnz mmx0c

																																																																									   mmx0d : emms jmp done

																																																																												   mem1 : mov ebx,
																																																																														  ecx;
uint32_t align edi when possible sub ecx, edi xor eax, eax sub ecx, ebx and ecx, 3 sub ebx,
	ecx jle doalign1 rep stosb mov ecx, ebx and ebx, 3 shr ecx,
	2 rep stosd doalign1 : add ecx, ebx rep stosb done:
}
}
;
#endif

#if CONSIDERED_OBSOLETE
//
// Instead of using memset(x,0xff,x) - use this function
//
void memfill(PVOID Dest, size_t Len) {_asm {

	mov edi, Dest mov ecx, Len cmp Processor,
	CPU_MMX jnz memf1

		// memfill_mmx:
		mov ebx,
	ecx;
8 byte align edi when possible sub ecx, edi mov eax, -1 sub ecx, ebx and ecx, 7 sub ebx,
	ecx jle doalign0 test ecx, ecx jz mmx0fa mmx0f : mov byte ptr[edi], al inc edi dec ecx jnz mmx0f mmx0fa : mov ecx, ebx and ebx, 7 shr ecx, 3 jz doalign0 movq mm0, fillnum mmx1 : movq[edi], mm0 add edi, 8 dec ecx jnz mmx1 doalign0 : add ecx, ebx test ecx, ecx jz mmx0fb mmx1f : mov byte ptr[edi], al inc edi dec ecx jnz mmx1f mmx0fb : emms jmp done

																																																																																					  memf1 : mov ebx,
																																																																																							  ecx;
uint32_t align edi when possible sub ecx, edi mov eax, -1 sub ecx, ebx and ecx, 3 sub ebx,
	ecx jle doalign1 rep stosb mov ecx, ebx and ebx, 3 shr ecx,
	2 rep stosd doalign1 : add ecx, ebx rep stosb done:
}
}
;
#endif

//---------------------------------------------------------------------------
// Random Number Functions
int32_t
RandomNumber(int32_t range)
{
	gosASSERT(RAND_MAX == (1 << 15) - 1); // This is always TRUE in VC
	return ((gos_rand() * range) >> 15); // Used to used mod (%) - which costs 40+ cycles (AG)
}

//---------------------------------------------------------------------------
bool RollDice(int32_t percent)
{
	return (((rand() * 100) >> 15) < percent); // Optimized the % out
}
