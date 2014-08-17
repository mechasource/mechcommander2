/*******************************************************************************
 Copyright (c) 2011-2014, Jerker Back. All rights reserved.

 Permission to use, copy, modify, and distribute this software for any
 purpose with or without fee is hereby granted, provided that the following 
 conditions are met (OSI approved BSD 2-clause license):

 1. Redistributions of source code must retain the above copyright notice, 
    this list of conditions and the following disclaimer.
 2. Redistributions in binary form must reproduce the above copyright notice, 
    this list of conditions and the following disclaimer in the documentation 
    and/or other materials provided with the distribution.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
 AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
 IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE 
 DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE 
 FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL 
 DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR 
 SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER 
 CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, 
 OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
 OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*******************************************************************************/
/*******************************************************************************
 mechtypes.h - MechCommander common types

 MechCommander 2 source code

 2014-08-16 jerker_back, created
 
 $LastChangedBy$
 
================================================================================
 RcsID = $Id$ */

#pragma once

#ifndef _MECHTYPES_H_
#define _MECHTYPES_H_

#include <stdint.h>

#pragma pack(push,8)

#ifdef __cplusplus
extern "C" {
#endif

// addition to C standard types
#ifndef _CONST_TYPES_DEFINED
#define _CONST_TYPES_DEFINED
typedef const signed char			cint8_t;
typedef const unsigned char			cuint8_t;
typedef const unsigned char*		pcuint8_t;
typedef const signed short			cint16_t;
typedef const signed short*			pcint16_t;
typedef const unsigned short		cuint16_t;
typedef const unsigned short*		pcuint16_t;
typedef const signed int			cint32_t;
typedef const unsigned int			cuint32_t;
typedef const unsigned int*			pcuint32_t;
typedef const signed long long		cint64_t;
typedef const signed long long*		pcint64_t;
typedef const unsigned long long	cuint64_t;
typedef const unsigned long long*	pcuint64_t;
#endif

#ifndef _POINTER_TYPES_DEFINED
#define _POINTER_TYPES_DEFINED
typedef signed char*				pint8_t;
typedef signed short*				pint16_t;
typedef signed int*					pint32_t;
typedef signed long long*			pint64_t;
typedef unsigned char*				puint8_t;
typedef unsigned short*				puint16_t;
typedef unsigned int*				puint32_t;
typedef unsigned long long*			puint64_t;
#endif

// addition to Windows types
typedef const void*					PCVOID;
typedef const unsigned char*		PCUCHAR;
typedef const unsigned short*		PCUSHORT;

#ifdef __cplusplus
}
#endif
#pragma pack(pop)
#endif
