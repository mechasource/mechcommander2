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
 directdraw.cpp  - gameos

 MechCommander 2 source code

 2011-08-04 jerker_back, created
 
 $LastChangedBy$
 
================================================================================
 RcsID = $Id$ */

#include "stdafx.h"


InternalFunctionPause(char const *,...)                                                             UNDEF 00002050 00000004 R . . . . T .
ErrorNumberToMessage(int)                                                                           UNDEF 00002054 00000004 R . . . . T .
long (*_DirectDrawCreateEx)(_GUID *,void * *,_GUID const &,IUnknown *)                              UNDEF 00002058 00000004 R . . . . . .
InternalFunctionSpew(char const *,char const *,...)                                                 UNDEF 0000205C 00000004 R . . . . T .
long (*_DirectDrawEnumerate)(int (*)(_GUID *,char *,char *,void *),void *)                          UNDEF 00002060 00000004 R . . . . . .
long (*_DirectDrawEnumerateEx)(int (*)(_GUID *,char *,char *,void *,HMONITOR__ *),void *,ulong)     UNDEF 00002064 00000004 R . . . . . .
GetSurfaceCaps(_DDSCAPS2 *)                                                                         UNDEF 00002068 00000004 R . . . . T .
GetCreatePaletteFlags(ulong)                                                                        UNDEF 0000206C 00000004 R . . . . T .
GetEnumDisplayModeFlags(ulong)                                                                      UNDEF 00002070 00000004 R . . . . T .
GetSurfaceDescription(_DDSURFACEDESC2 *)                                                            UNDEF 00002074 00000004 R . . . . T .
GetSetCooperativeLevelFlags(ulong)                                                                  UNDEF 00002078 00000004 R . . . . T .
GetSetColorKeyFlags(ulong)                                                                          UNDEF 0000207C 00000004 R . . . . T .
GetLockFlags(ulong)                                                                                 UNDEF 00002080 00000004 R . . . . T .
GetRectangle(char *,tagRECT *)                                                                      UNDEF 00002084 00000004 R . . . . T .
__security_check_cookie(x)                                                                          UNDEF 0000208C 00000004 R . . . . T .
GetBltInformation(IDirectDrawSurface7 *,tagRECT *,IDirectDrawSurface7 *,tagRECT *,ulong,_DDBLTFX *) UNDEF 00002090 00000004 R . . . . T .
GetBltFastInformation(IDirectDrawSurface7 *,ulong,ulong,IDirectDrawSurface7 *,tagRECT *,ulong)      UNDEF 00002094 00000004 R . . . . T .
GetFlipFlags(ulong)                                                                                 UNDEF 00002098 00000004 R . . . . T .
GetSurfaceDescriptionOld(_DDSURFACEDESC *)                                                          UNDEF 0000209C 00000004 R . . . . T .
GetGosPixelFormat(_DDPIXELFORMAT *)                                                                 UNDEF 000020A0 00000004 R . . . . T .

wDirectDrawCreateEx(_GUID *,void * *,_GUID const &,IUnknown *)                                                        .text 00000D5C 00000082 R . . . B T .
wDirectDrawEnumerate(int (*)(_GUID *,char *,char *,void *),int (*)(_GUID *,char *,char *,void *,HMONITOR__ *),void *) .text 00000DEC 0000008A R . . . B T .
wSetHWnd(IDirectDrawClipper *,ulong,HWND__ *)                                                                         .text 00000E7C 00000072 R . . . B T .
wGetAvailableVidMem(IDirectDraw7 *,_DDSCAPS2 *,ulong *,ulong *)                                                       .text 00000EFC 000000A0 R . . . B T .
wGetFourCCCodes(IDirectDraw7 *,ulong *,ulong *)                                                                       .text 00000F9C 0000006A R . . . B T .
wGetDeviceIdentifier(IDirectDraw7 *,tagDDDEVICEIDENTIFIER2 *,ulong)                                                   .text 0000100C 0000006A R . . . B T .
wGetCaps(IDirectDraw7 *,_DDCAPS_DX7 *,_DDCAPS_DX7 *)                                                                  .text 0000107C 0000006A R . . . B T .
wCreatePalette(IDirectDraw7 *,ulong,tagPALETTEENTRY *,IDirectDrawPalette * *,IUnknown *)                              .text 000010EC 00000096 R . . . B T .
wEnumDisplayModes(IDirectDraw7 *,ulong,_DDSURFACEDESC2 *,void *,long (*)(_DDSURFACEDESC2 *,void *))                   .text 0000118C 0000007E R . . . B T .
wGetDisplayMode(IDirectDraw7 *,_DDSURFACEDESC2 *)                                                                     .text 0000120C 0000005E R . . . B T .
wSetDisplayMode(IDirectDraw7 *,ulong,ulong,ulong,ulong,ulong)                                                         .text 0000126C 0000008E R . . . B T .
wRestoreDisplayMode(IDirectDraw7 *)                                                                                   .text 000012FC 0000005A R . . . B T .
wCreateClipper(IDirectDraw7 *,ulong,IDirectDrawClipper * *,IUnknown *)                                                .text 0000135C 0000007E R . . . B T .
wCreateSurface(IDirectDraw7 *,_DDSURFACEDESC2 *,IDirectDrawSurface7 * *,IUnknown *)                                   .text 000013DC 0000009A R . . . B T .
wSetCooperativeLevel(IDirectDraw7 *,HWND__ *,ulong)                                                                   .text 0000147C 0000007E R . . . B T .
wIsLost(IDirectDrawSurface7 *)                                                                                        .text 000014FC 0000009C R . . . B T .
wGetDC(IDirectDrawSurface7 *,HDC__ * *)                                                                               .text 0000159C 0000007E R . . . B T .
wReleaseDC(IDirectDrawSurface7 *,HDC__ *)                                                                             .text 0000161C 0000006E R . . . B T .
wSetColorKey(IDirectDrawSurface7 *,ulong,_DDCOLORKEY *)                                                               .text 0000168C 00000098 R . . . B T .
wLock(IDirectDrawSurface7 *,tagRECT *,_DDSURFACEDESC2 *,ulong,void *)                                                 .text 0000172C 00000086 R . . . B T .
wUnlock(IDirectDrawSurface7 *,tagRECT *)                                                                              .text 000017BC 000000B3 R . . . B T .
wLock(IDirectDrawSurface *,tagRECT *,_DDSURFACEDESC *,ulong,void *)                                                   .text 0000187C 00000086 R . . . B T .
wUnlock(IDirectDrawSurface *,tagRECT *)                                                                               .text 0000190C 000000B3 R . . . B T .
wSetPalette(IDirectDrawSurface7 *,IDirectDrawPalette *)                                                               .text 000019CC 0000006E R . . . B T .
wRestore(IDirectDrawSurface7 *)                                                                                       .text 00001A3C 00000062 R . . . B T .
wBlt(IDirectDrawSurface7 *,tagRECT *,IDirectDrawSurface7 *,tagRECT *,ulong,_DDBLTFX *)                                .text 00001AAC 000000B3 R . . . B T .
wBltFast(IDirectDrawSurface7 *,ulong,ulong,IDirectDrawSurface7 *,tagRECT *,ulong)                                     .text 00001B6C 000000AA R . . . B T .
wFlip(IDirectDrawSurface7 *,IDirectDrawSurface7 *,ulong)                                                              .text 00001C1C 0000008F R . . . B T .
wGetSurfaceDesc(IDirectDrawSurface7 *,_DDSURFACEDESC2 *)                                                              .text 00001CAC 00000082 R . . . B T .
wGetSurfaceDesc(IDirectDrawSurface *,_DDSURFACEDESC *)                                                                .text 00001D3C 00000082 R . . . B T .
wGetPixelFormat(IDirectDrawSurface7 *,_DDPIXELFORMAT *)                                                               .text 00001DCC 00000082 R . . . B T .
wGetAttachedSurface(IDirectDrawSurface7 *,_DDSCAPS2 *,IDirectDrawSurface7 * *)                                        .text 00001E5C 00000082 R . . . B T .
wAddAttachedSurface(IDirectDrawSurface7 *,IDirectDrawSurface7 *)                                                      .text 00001EEC 0000006E R . . . B T .
wDeleteAttachedSurface(IDirectDrawSurface7 *,ulong,IDirectDrawSurface7 *)                                             .text 00001F5C 00000072 R . . . B T .
wSetClipper(IDirectDrawSurface7 *,IDirectDrawClipper *)                                                               .text 00001FDC 0000006E R . . . B T .