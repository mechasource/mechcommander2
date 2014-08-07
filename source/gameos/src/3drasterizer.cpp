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
 3drasterizer.cpp  - gameos

 MechCommander 2 source code

 2014-07-24 jerker_back, created
 
 $LastChangedBy$
 
================================================================================
 RcsID = $Id$ */

#include "stdafx.h"

CheckVertices2(gos_VERTEX_2UV *,ulong)                                      .text   00000A28 00000646 R . . . B T .
CheckVertices3(gos_VERTEX_3UV *,ulong)                                      .text   00001078 00000740 R . . . B T .
CheckVertices(gos_VERTEX *,ulong,bool)                                      .text   000017B8 00000671 R . . . B T .
GetMipmapColor(int)                                                         .text   00001E38 00000044 R . . . B T .
DebugTriangle_2UV(gos_VERTEX_2UV *,gos_VERTEX_2UV *,gos_VERTEX_2UV *)       .text   00001E88 00000018 R . . . B T .
DebugTriangle_3UV(gos_VERTEX_3UV *,gos_VERTEX_3UV *,gos_VERTEX_3UV *)       .text   00001EA8 00000018 R . . . B T .
DebugTriangle(gos_VERTEX *,gos_VERTEX *,gos_VERTEX *)                       .text   00001EC8 0000111C R . . . B T .
Destroy3D(void)                                                             .text   00003018 00000049 R . . . B T .
ReInit3D(void)                                                              .text   00003068 0000001D R . . . B T .
gos_SetViewport(ulong,ulong,ulong,ulong,float,float)                        .text   00003088 0000014B R . . . B T .
gos_SetupViewport(bool,float,bool,ulong,float,float,float,float,bool,ulong) .text   000031D8 0000044A R . . . B T .
gos_GetViewport(float *,float *,float *,float *)                            .text   00003628 00000053 R . . . B T .
InitRenderer(void)                                                          .text   00003688 000000EF R . . . B T .
__unwindfunclet$?CheckVertices@@YGXPAUgos_VERTEX@@K_N@Z$0                   .text$x 00003780 0000000E R . . . . . .
__unwindfunclet$?CheckVertices@@YGXPAUgos_VERTEX@@K_N@Z$1                   .text$x 0000378E 00000008 R . . . . . .
__ehhandler$?CheckVertices@@YGXPAUgos_VERTEX@@K_N@Z                         .text$x 00003796 0000001B R . . . . . .
__unwindfunclet$?gos_SetViewport@@YGXKKKKMM@Z$0                             .text$x 000037B1 0000000E R . . . . . .
__unwindfunclet$?gos_SetViewport@@YGXKKKKMM@Z$1                             .text$x 000037BF 00000008 R . . . . . .
__ehhandler$?gos_SetViewport@@YGXKKKKMM@Z                                   .text$x 000037C7 0000001B R . . . . . .
GosLogRef::operator ulong(void)                                             .text   00003858 00000011 R . . . B . .
GosLogFuncScope::GosLogFuncScope(ulong)                                     .text   0000386C 00000031 R . . . B . .
GosEventLog::Log(ulong)                                                     .text   000038A0 00000060 R . . . B . .
GosLogFuncScope::~GosLogFuncScope(void)                                     .text   00003900 00000029 R . . . B . .
fabs(float)                                                                 .text   00003934 00000011 R . . . B T .
fabsf(x)                                                                    .text   00003948 00000021 R . . . B T .




InternalFunctionSpew(char const *,char const *,...)                           UNDEF 00003980 00000004 R . . . . T .
gos_GetMachineInformation(MachineInfo,int,int,int,int)                        UNDEF 00003988 00000004 R . . . . . .
InternalFunctionPause(char const *,...)                                       UNDEF 00003998 00000004 R . . . . T .
GosLogRef::GosLogRef(GosLogRef::EventType,char *,char *,int)                  UNDEF 000039A8 00000004 R . . . . . .
___CxxFrameHandler3                                                           UNDEF 000039B0 00000004 R . . . . T .
__security_check_cookie(x)                                                    UNDEF 000039B4 00000004 R . . . . . .
GetMipmapUsed(ulong,gos_VERTEX *,gos_VERTEX *,gos_VERTEX *)                   UNDEF 000039D4 00000004 R . . . . T .
GetAsyncKeyState(x)                                                           UNDEF 000039DC 00000004 R . . . . T .
_memcpy                                                                       UNDEF 000039E4 00000004 R . . . . T .
wDrawPrimitive(IDirect3DDevice7 *,_D3DPRIMITIVETYPE,ulong,void *,ulong,ulong) UNDEF 00003A14 00000004 R . . . . T .
FlushRenderStates(void)                                                       UNDEF 00003A20 00000004 R . . . . T .
gos_SetRenderState(gos_RenderState,int)                                       UNDEF 00003A28 00000004 R . . . . . .
fabs                                                                         UNDEF 00003A2C 00000004 R . . . . . .
End3DScene(void)                                                              UNDEF 00003A30 00000004 R . . . . T .
gos_Free(void *)                                                              UNDEF 00003A34 00000004 R . . . . T .
wSetViewport(IDirect3DDevice7 *,_D3DVIEWPORT7 *)                              UNDEF 00003A3C 00000004 R . . . . T .
ErrorHandler(x,x)                                                             UNDEF 00003A44 00000004 R . . . . . .
DoRenderState(gos_RenderState,int)                                            UNDEF 00003A48 00000004 R . . . . . .
memset                                                                       UNDEF 00003A4C 00000004 R . . . . T .
InitRenderStates(void)                                                        UNDEF 00003A50 00000004 R . . . . T .
wBeginScene(IDirect3DDevice7 *)                                               UNDEF 00003A58 00000004 R . . . . T .
RestoreAreas(void)                                                            UNDEF 00003A5C 00000004 R . . . . T .
EnterWindowMode(void)                                                         UNDEF 00003A60 00000004 R . . . . T .
ErrorNumberToMessage(int)                                                     UNDEF 00003A64 00000004 R . . . . T .
wClear(IDirect3DDevice7 *,ulong,_D3DRECT *,ulong,ulong,float,ulong)           UNDEF 00003A68 00000004 R . . . . T .
fabs
HRESULT __stdcall wClear(struct IDirect3DDevice7* d3dDevice7, ULONG dwCount, LPD3DRECT lpRects, ULONG dwFlags, ULONG dwColor, D3DVALUE dvZ, ULONG dwStencil );

extern ULONG gShowVertexData;
 ULONG __stdcall gos_GetMachineInformation(MachineInfo mi, int Param1, int Param2, int Param3, int Param4);
 struct gosEnvironment Environment;
 extern ULONG RenderStates[gos_MaxState];			// Current User settings
 unsigned long DisableUVAssert;
 int InternalFunctionPause(PCSTR Message, ...);
 extern bool InDebugger;						// During debugger rendering
 unsigned long gForceAlpha;
 public: __thiscall GosLogRef::GosLogRef(enum  GosLogRef::EventType, char *, char *, int);
 uintptr_t __security_cookie;
 ULONG __CxxFrameHandler3;
 __fastcall __security_check_cookie(x);
 private: static unsigned long GosLogFuncScope::Current;
 private: static unsigned long * GosEventLog::pLogBase;
 private: static unsigned long GosEventLog::LogMod;
 private: static unsigned long GosEventLog::LogOffset;
 int DBMouseY;
 int DBMouseX;
 unsigned long MipColorTexture;
 int __stdcall GetMipmapUsed(unsigned __int32, struct gos_VERTEX *, struct gos_VERTEX *, struct gos_VERTEX *);
 extern ULONG WhichImage;					// Original / Special mode image?
 SHORT __stdcall GetAsyncKeyState(int vKey);
 int ZoomMode;
 void *__cdecl memcpy(void *Dst, const void *Src, size_t Size);
 int ExMouseY;
 int ExMouseX;
 unsigned long PolygonsInArea;
 int AreaB;
