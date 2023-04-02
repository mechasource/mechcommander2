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
 userinterface.cpp - GameOS reference pseudo code

 MechCommander 2 source code

 2014-07-24 Jerker Back, created

*******************************************************************************/

#include "stdinc.h"

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
gos_NetStartGame(char*, BOOLEAN);
gos_NetDoneStartGame(void);
