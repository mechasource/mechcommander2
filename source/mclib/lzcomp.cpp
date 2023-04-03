//--------------------------------------------------------------------------
// LZ Compress Routine
//
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

//#include "stdinc.h"
#include "lz.h"

//#ifndef _MBCS
////#include "gameos.hpp"
//
//#ifndef HEAP_H
//#include "heap.h"
//#endif
//
//#else
//#include <_ASSERT.h>
//#include <malloc.h>
//#define gosASSERT _ASSERT
//#define gos_Malloc malloc
//#define gos_Free free
//#endif
//
//---------------------------------------------------------------------------
// Static Globals

#ifndef nullptr
#define nullptr 0
#endif

typedef uint8_t* uint8_t*;

//-----------------------------
// Used by Compressor Routine
uint8_t* LZCHashBuf = nullptr;
size_t InBufferUpperLimit = 0;
size_t InBufferPos = 0;
uint8_t* InBuffer = nullptr;
size_t OutBufferPos = 0;
uint8_t* OutBuffer = nullptr;
uint32_t PrefixCode = 0;
uint32_t FreeCode = 0;
uint32_t MaxCode = 0;
uint32_t NBits = 0;
uint32_t BitOffset = 0;
uint32_t codeToWrite = 0;
uint8_t K = 0;

enum class __lzcomp_const
{
	MaxMax = 4096,
	Clear = 256,
	EOF = 257,
	First_Free = 258,
};

typedef struct Hash
{
	uint32_t hashFirst;
	uint32_t hashNext;
	uint8_t hashChar;
} Hash;

static uint8_t tag_LZCHashBuf[sizeof(Hash) * MaxMax + 1024];
//-----------------------------

//-------------------------------------------------------------------------------
// LZ Compress Routine
// Takes a pointer to dest buffer, a pointer to source buffer and len of source.
// returns length of compressed image.
size_t __stdcall LZCompress(uint8_t* dest, uint8_t* src, size_t srcLen)
{
	size_t result = 0;
	if (!LZCHashBuf)
	{
		/* allocating LZCHashBuf off a gos heap causes problems for applications
		that need to reset gos or its heaps*/
		LZCHashBuf = (uint8_t*)&(tag_LZCHashBuf[0]);
	}
	// Initialize:
	uint32_t clearSize = sizeof(Hash) * 256;
	__asm
		{
		mov		eax, [dest]
		mov		[OutBufferPos], eax
		mov		[OutBuffer], eax
		mov		eax, [src]
		mov		[InBufferPos], eax
		mov		[InBuffer], eax
		add		eax, [srcLen]
		mov		[InBufferUpperLimit], eax

		mov		[BitOffset], 0 // zero out BitOffset

			// call	InitTable
		mov		[NBits], 9 // Starting with 9 bit codes
		mov		[MaxCode], 512 // 10 0000 0000 b
		mov		eax, -1 // Marked as unused
		mov		ecx, [clearSize] // Clear first 256 entries
		mov		edi, [LZCHashBuf] // Start at the start of buffer
		rep		stosb
		mov		[FreeCode], First_Free // Next code to use is first_free

		mov		eax, Clear // first byte in buffer or file is CLEAR

				// call	writeCode
		xor		edx, edx // make sure the DL is CLEAR
		mov		edi, [OutBufferPos] // obtain destination address
		mov		ecx, [BitOffset] // get bitposition
		jecxz	save1
		}
shift1:
	__asm
		{
		shl		ax, 1
		rcl		edx, 1
		loop	shift1
		or		al, [edi]
		}
save1:
	__asm
	{
		stosw
		mov		al, dl
		stosb

				// AdvanceBuffer

		mov		ecx, [NBits]                 ;
		get number of bits to advance
		mov		eax, [BitOffset]             ;
		get low word of OutBuffer
		add		eax, ecx
		mov		cl, al
		shr		al, 3
		add		[OutBufferPos], eax
		and		cl, 7
		movzx	ecx, cl
		mov		[BitOffset], ecx

			//-------------------------------------------------------------------------
			// This is the main compression loop
			//-------------------------------------------------------------------------

			// call	ReadChar              		//get byte from source
		mov		esi, [InBufferPos] // get address
		cmp		esi, [InBufferUpperLimit] // Check to see if we are done
		cmc // compliment carry
		jc		doneRC1

		lodsb // load byte
		mov		[InBufferPos], esi
		clc
	}
doneRC1:
Make_Into_Code:
	__asm
		{
		movzx	eax, al // turn wchar_t into code
		}
	//-------------------------------------------------------------------------
Set_AX_Prefix:
	__asm
		{
		mov		[PrefixCode], eax // into prefix code

								  // call	ReadChar              		//more...
		mov		esi, [InBufferPos] // get address
		cmp		esi, [InBufferUpperLimit] // Check to see if we are done
		cmc // compliment carry
		jc		doneRC2

		lodsb // load byte
		mov		[InBufferPos], esi
		clc
		}
doneRC2:
	__asm
		{
		jc		FoundEOF // No more input
		mov		[K], al // Save returned wchar_t
		mov		ebx, [PrefixCode] // check for this pair
			// call	LookUpCode            		//in the table

			// call	Index                      	//index into current table
			// address
		lea		esi, [ebx*8]
		add		esi, ebx // EBX * 9
		add		esi, [LZCHashBuf]

		xor		edi, edi // flag destination
		cmp		[esi].hashFirst, -1 // see if code is used
		je		int16_t exit1 // if == then CARRY = CLEAR, set it
		inc		edi // flag as used
		mov		ebx, [esi].hashFirst // get prefix code
		}
lookloop:
	__asm
		{
			// call	Index                      	//translate prefix or table to
			// index
		lea		esi, [ebx*8]
		add		esi, ebx // EBX * 9
		add		esi, [LZCHashBuf]

		cmp		[esi].hashChar, al // is the suffix the same? if yes
		jne		notSame // then we can compress this a
		clc // little more by taking this prefix
		mov		eax, ebx // code and getting a new suffix
		jmp		int16_t exit2 // in a moment
		}
notSame:
	__asm
		{
		cmp		[esi].hashNext, -1 // found a new pattern so exit
		je		exit1 // if == then CARRY = CLEAR, set it
		mov		ebx, [esi].hashNext // continue through chain to get to
		jmp		int16_t lookloop // end of chain
		}
exit1:
	__asm
		{
		stc
		}
exit2:
	__asm
		{
		jnc		Set_AX_Prefix // new prefix in EAX

				//------------------------------------------------------------------------

				// call	AddCode               		//Add to table
		mov		ebx, [FreeCode] // get the next available hash table code
		push	ebx
		or		edi, edi // is this first use of prefix?
		je		int16_t newprefix
		mov		[esi].hashNext, ebx // if not, then set next code dest
		jmp		int16_t addcode
		}
newprefix:
	__asm
		{
		mov		[esi].hashFirst, ebx // Mark first as used
		}
addcode:
	__asm
		{
		cmp		ebx, MaxMax // is this the last code?
		je		exitAC

							 // call	Index                     	//create new entry
		lea		esi, [ebx*8]
		add		esi, ebx // EBX * 9
		add		esi, [LZCHashBuf]

		mov		[esi].hashFirst, -1 // mark new entry as unused
		mov		[esi].hashNext, -1
		mov		[esi].hashChar, al // and save suffix
		inc		ebx // set up for next code available
		}
exitAC:
	__asm
	{
		mov	[FreeCode], ebx
		pop	ebx
	}
	__asm {push ebx // save new code
			   mov eax,
		[PrefixCode] // write the old prefix code

			// call	writeCode
			xor edx,
		edx // make sure the DL is CLEAR
			mov edi,
		[OutBufferPos] // obtain destination address
		mov ecx,
		[BitOffset] // get bitposition
		jecxz save5} shift5 : __asm {shl ax, 1 rcl edx, 1 loop shift5 or al, [edi]} save5 : __asm
	{
		stosw mov al,
			dl stosb

				// AdvanceBuffer

				mov ecx,
			[NBits];
		get number of bits to advance mov eax, [BitOffset];
		get low word of OutBuffer add eax, ecx mov cl, al shr al, 3 add[OutBufferPos], eax and cl,
			7 movzx ecx, cl mov[BitOffset],
			ecx

				pop ebx // back
					mov al,
			[K] cmp ebx,
			[MaxCode] // exceeded size?
			jb Make_Into_Code // no
				cmp[NBits],
			12 // less than 12 bit encoding?
			jb Another_Bit // give it one more

				//---------------------------------------------------------------
				// All codes up to 12 bits are used: clear table and restart
				//---------------------------------------------------------------

				mov eax,
			Clear // write a clear code.....out of bits

			// call writeCode
			xor edx,
			edx // make sure the DL is CLEAR
				mov edi,
			[OutBufferPos] // obtain destination address
			mov ecx,
			[BitOffset] // get bitposition
			jecxz save2
	}
shift2:
	__asm
		{
		shl		ax, 1
		rcl		edx, 1
		loop	shift2
		or		al, [edi]
		}
save2:
	__asm
	{
		stosw
		mov		al, dl
		stosb

				// AdvanceBuffer

		mov		ecx, [NBits]                 ;
		get number of bits to advance
		mov		eax, [BitOffset]             ;
		get low word of OutBuffer
		add		eax, ecx
		mov		cl, al
		shr		al, 3
		add		[OutBufferPos], eax
		and		cl, 7
		movzx	ecx, cl
		mov		[BitOffset], ecx

			// call	InitTable             		//Cleanup table
		mov		[NBits], 9 // Starting with 9 bit codes
		mov		[MaxCode], 512 // 10 0000 0000 b
		mov		eax, -1 // Marked as unused
		mov		ecx, clearSize // Clear first 256 entries
		mov		edi, [LZCHashBuf] // Start at the start of buffer
		rep		stosb
		mov		[FreeCode], First_Free // Next code to use is first_free


		mov		al, [K] // Last wchar_t
		jmp		Make_Into_Code
	}
	//---------------------------------------------------------------
	// Extended bit length by a bit, adjusting Max_Code accordingly
	//---------------------------------------------------------------
Another_Bit:
	__asm
		{
		inc		[NBits] // one more
		shl		[MaxCode], 1 // Double it's size
		jmp		Make_Into_Code
		}
	//----------------------------------------------------------------
	// No more input: flush what's left and perform housekeeping
	//----------------------------------------------------------------
FoundEOF:
	__asm
		{
		mov		eax, [PrefixCode] // write the last code

				// call	writeCode
		xor		edx, edx // make sure the DL is CLEAR
		mov		edi, [OutBufferPos] // obtain destination address
		mov		ecx, [BitOffset] // get bitposition
		jecxz	save3
		}
shift3:
	__asm
		{
		shl		ax, 1
		rcl		edx, 1
		loop	shift3
		or		al, [edi]
		}
save3:
	__asm
	{
		stosw
		mov		al, dl
		stosb

				// AdvanceBuffer

		mov		ecx, [NBits]                 ;
		get number of bits to advance
		mov		eax, [BitOffset]             ;
		get low word of OutBuffer
		add		eax, ecx
		mov		cl, al
		shr		al, 3
		add		[OutBufferPos], eax
		and		cl, 7
		movzx	ecx, cl
		mov		[BitOffset], ecx

		mov		eax, EOF // write EOF code

			// call	writeCode
		xor		edx, edx // make sure the DL is CLEAR
		mov		edi, [OutBufferPos] // obtain destination address
		mov		ecx, [BitOffset] // get bitposition
		jecxz	save4
	}
shift4:
	__asm
		{
		shl		ax, 1
		rcl		edx, 1
		loop	shift4
		or		al, [edi]
		}
save4:
	__asm
	{
		stosw
		mov		al, dl
		stosb

				// AdvanceBuffer

		mov		ecx, [NBits]                 ;
		get number of bits to advance
		mov		eax, [BitOffset]             ;
		get low word of OutBuffer
		add		eax, ecx
		mov		cl, al
		shr		al, 3
		add		[OutBufferPos], eax
		and		cl, 7
		movzx	ecx, cl
		mov		[BitOffset], ecx

		mov		eax, [BitOffset] // bits waiting to go?
		or		eax, eax
		je		CompressDone // no....just close things up and go back
		inc		[OutBufferPos]
	}
CompressDone:
	__asm
	{
		//	return number of bytes in compressed buffer
		mov	eax, [OutBufferPos]
		sub	eax, [OutBuffer]

		mov [result], eax
	}
	return (result);
}
