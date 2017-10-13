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
 userinterface.cpp - gameos reference pseudo code

 MechCommander 2 source code

 2014-07-24 jerker_back, created

 $LastChangedBy$

================================================================================
 RcsID = $Id$ */

#include "stdafx.h"


// -----------------------------------------------------------------------------
// Global data exported from this module

// global referenced data not listed in headers

// local data

// -----------------------------------------------------------------------------
// global implemented functions in this module listed in headers

// global implemented functions not listed in headers

// local functions

// -----------------------------------------------------------------------------
// externals referenced in this file not specified in headers

GetListSize(void*, int, void** const);
SortGameList(void*, int, void** const);
Select(void*, int, void** const);
GetSelection(void*, int, void** const);
GetGameData(void*, int, void** const);
StartPingServer(void*, int, void** const);
StartPingClient(void*, int, void** const);
ShutdownPing(void*, int, void** const);
GetPing(void*, int, void** const);
UpdatePlayerList(void*, int, void** const);
UpdateGUNPlayerList(void*, int, void** const);
UpdateGUNGameList(void*, int, void** const);
UpdateGameList(void*, int, void** const);
UpdateModemList(void*, int, void** const);
UpdateSerialPortList(void*, int, void** const);
CreateSession(void*, int, void** const);
JoinSelected(void*, int, void** const);
HostModemGame(void*, int, void** const);
JoinModemGame(void*, int, void** const);
Disconnect(void*, int, void** const);
ConnectTCP(void*, int, void** const);
ConnectZoneMatch(void*, int, void** const);
ConnectIPX(void*, int, void** const);
ConnectSerialPort(void*, int, void** const);
IsConnectionAvailable(void*, int, void** const);
UnRegisterData(void);
gos_NetEndGame(void);
gos_NetStartGame(char*, bool);
gos_NetDoneStartGame(void);
