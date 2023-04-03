#pragma once
//
//============================================================
//
// Contents:  Network header file for external clients
//
//===========================================================================//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

enum class GOSERRORCODE
{
	GOS_OK = 0,
	GOS_ERR_GENERIC,
	GOS_ERR_NOTINITIALIZED,
	GOS_ERR_NOCONNECTION,
	GOS_ERR_SESSIONNOTFOUND,
	GOS_ERR_NOLOCALPLAYER,
	GOS_ERR_NOTLOBBIED,
	GOS_SCRIPTNOTFOUND,
	GOS_DIALING
};

enum class
{
	NP_TCP = 1,
	NP_IPX,
	NP_SERIAL,
	NP_MODEM,
} ConnectionType;

enum class
{
	FIDP_MSG_STARTGAME = 224,
	FIDP_MSG_VERIFY,
	FIDP_MSG_VERIFY_CLUSTER,
	FIDP_MSG_PLAYERID,
	FIDP_MSG_PLAYERS_IN_GROUP,
	FIDP_MSG_PLAYERIDVERIFY,
	FIDP_MSG_SERVERID,
	FIDP_MSG_REMOVEPLAYER,
	FIDP_MSG_SYSTEM_INFO,
} MessageTypes;

//
// Routine typedef used to return game and player information
//
typedef void(__stdcall* NETCALLBACK)(PSTR string, uint32_t value);

extern BOOLEAN Connected;

//
// Functionality: Creates the networking object.
//
// Return value: If successful, return value is GOS_OK.
//
void __stdcall gos_InitializeNetworking(void);

//
// Close any game in progress, clean up networking
//
void __stdcall gos_ShutdownNetwork(void);

//
// Returns true if connection supported
//
// 1: TCPIP
// 2: IPX
// 3: Modem
// 4: Serial
//
BOOLEAN __stdcall CheckConnectionAvailable(int32_t Connection);

//
// Open a TCPIP connection
//
// You can pass the address of the server or nullptr for the sub net.
//
//
BOOLEAN __stdcall gos_ConnectTCP(PSTR ip_address, uint16_t port);

//
// Open an IPX connection
//
BOOLEAN __stdcall gos_ConnectIPX(void);

//
// Open a Serial connection
//
// You pass the number of the COM port to use
//
BOOLEAN __stdcall gos_ConnectComPort(uint32_t com_port);

//
// Open a Modem connection
//
// You pass a phone number and optional modem name to use
//
BOOLEAN __stdcall gos_ConnectModem(PSTR phone_number, PSTR modem_name);

//
// Enumerates all sessions available.
//
// Returns a game name and a session ID number. The ID number can be used to
// join the game.
//
// This list is updated once a frame.
//
// This API is only valid before a game is joined or created
//
void __stdcall gos_EnumerateSessions(NETCALLBACK callback);

// Functionality: Enumerates the players in the given session by calling the
// callback
//					for each player in the session.  The callback string is nullptr
// when
//					all players have been enumerated.  The value given in the
// callback
//					is the GOSNETWORKID of the player.  It can be used when sending
// messages 				    to the player.
// Return value: If successful, return value is GOS_OK.  If no session matching
// <session_name>
//					is found, returns GOS_ERR_SESSIONNOTFOUND.
//
void __stdcall gos_EnumeratePlayers(uint32_t session_id, NETCALLBACK callback);

// Functionality: Creates a new game using the given game name, player name and
// maximum players.
//
// Return value: If successful, return value is GOS_OK.
//
GOSERRORCODE __stdcall gos_CreateGame(PSTR game_name, PSTR player_name, int32_t max_players);

// Functionality: Joins the game with the given session ID.  The session ids are
// returned in the
//					callback listing all sessions.
//
// Return value: If successful, return value is GOS_OK.  If no session matching
// <session_id>
//					is found, returns GOS_ERR_SESSIONNOTFOUND.
//
GOSERRORCODE __stdcall gos_JoinGame(uint32_t session_id, PSTR player_name);

// Functionality: Calls the callback once for each modem installed on this
//					machine, passing the name of the modem.
//
// Return value: If successful, return value is GOS_OK.
//
GOSERRORCODE __stdcall gos_GetModemNames(NETCALLBACK callback);

// Functionality: If connected to the modem protocol, gos_Dial attempts to dial
// the number
//				  given in the connection parameters.  It returns GOS_OK when
// connected.
//
// Return value:  It returns GOS_DIALING while still attempting to connect.
//				  The user must continue calling Dial until it no longer returns
// GOS_DIALING.
GOSERRORCODE __stdcall gos_Dial(void);

// Functionality: If connected to a network protocol, gos_Disconnect resets the
//					connection.  This is useful to cancel dialing.
//
GOSERRORCODE __stdcall gos_Disconnect(void);

// Functionality: LockSession can only be done by the host.
//
// Return value: If successful, it returns GOS_OK, else it returns
// GOS_ERR_GENERIC
//
GOSERRORCODE
gos_LockSession(void);

// LeaveSession kills the current player and removes him from the session.
//
// AG - kill this one
//
GOSERRORCODE
gos_LeaveSession(void);

GOSERRORCODE
gos_RemovePlayer(uint32_t player_id);
