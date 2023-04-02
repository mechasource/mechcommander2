#pragma once
//
//============================================================
//
// Contents:  Network header file for external clients
//
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

//
// Different connection types
//
#define NP_NOT 0
#define NP_TCP 1
#define NP_IPX 2
#define NP_SERIAL 3
#define NP_MODEM 4
#define NP_ZONEMATCH 5

//
// Functionality: Creates the networking object.
//
void __stdcall gos_InitializeNetworking(void);

//
// Close any game in progress, clean up networking
//
void __stdcall gos_ShutdownNetwork(void);

//
// Returns true if connection supported
//
BOOLEAN __stdcall gos_CheckConnectionAvailable(int32_t Connection);

//
// Open a TCPIP connection
//
// You can pass the address of the server or nullptr for the sub net.
//
//
BOOLEAN __stdcall gos_ConnectTCP(PSTR IPAddress, uint16_t port);

//
// Open a TCPIP and ZoneMatch connection
//
// You can pass the address of the server or nullptr for the sub net.
//
//
BOOLEAN __stdcall gos_ConnectZoneMatch(PSTR IPAddress, uint16_t port);

//
// Open an IPX connection
//
// BOOLEAN __stdcall gos_ConnectIPX(void);

//
// Open a Serial connection
//
// You pass the number of the COM port to use
//
// BOOLEAN __stdcall gos_ConnectSerialPort( uint32_t SerialPort );

//
// Open a Modem connection
//
// You pass a phone number and number of modem to use. If 0 is passed for the
// phone number Answer mode will be entered
//
// BOOLEAN __stdcall gos_ConnectModem( PSTR PhoneNumber, uint32_t Modem );

//
// Create the game specified
//
// 0=OK
// 1=Bad game name
// 2=Bad player name
// 3=Failed
// 4=Waiting for answer
//
int32_t __stdcall gos_CreateGame(PSTR GameName, PSTR PlayerName, int32_t MaxPlayers,
	PSTR GamePassword = 0, BOOLEAN Secure = 0, PSTR UserPassword = 0, uint32_t flags = 0);

//
// Join the game specified
//
// 0=OK
// 1=Bad game name  (or waiting for game on modem/serial)
// 2=Bad player name
// 3=Failed to connect
// 4=No dial tone
// 5=Invalid password
// 6=Too many players in the game
//
int32_t __stdcall gos_JoinGame(
	PSTR GameName, PSTR PlayerName, PSTR GamePassword = 0, PSTR UserPassword = 0);

//
// Functionality: If connected to a network protocol, gos_Disconnect resets the
// connection
//
void __stdcall gos_Disconnect(void);

//
// Functionality: If browser services are available, advertise the indicated
// key/value pair
//
BOOLEAN __stdcall gos_NetSetAdvertItem(uint32_t player, PSTR Name, PSTR value);

void
NGStatsSetPlayerId(PSTR name, PSTR passwd);

extern GUID NGStatsPlayerId;

GUID __stdcall gos_GenerateUniqueGUID(void);

BOOLEAN __stdcall CheckForZoneMatch(void);

// GUN Status functions
int32_t __stdcall GetGUNStatus(void);
int32_t __stdcall GetGUNRegStatus(void);
int32_t __stdcall GetGUNNetStatus(void);
int32_t __stdcall GetGUNLastError(void);
PSTR __stdcall GetGUNErrorMessage(void);
void __stdcall GetGUNDownloadStats(int32_t* tableSize, int32_t* progress);

#if CONSIDERED_OBSOLETE
// -----------------MD5 Hashing stuff

/* UINT4 defines a four byte word */
// Alphas really have 8 bytes for longs, which this code
// assumes only 4.  The original line is commented in case
// something is terribly terribly wrong with the fix.
// typedef uint32_t long UINT4;

typedef struct
{
	uint32_t state[4]; /* state (ABCD) */
	uint32_t count[2]; /* number of bits, modulo 2^64 (lsb first) */
	BOOLEAN buffer[64]; /* input buffer */
} MD5_CTX;

// do not remove, used for run time checks against
// source code in network games.
void __stdcall MD5SecurityCheckStart(void);
void __stdcall MD5SecurityCheckStop(void);

void __stdcall MD5Init(MD5_CTX*);
void __stdcall MD5Update(MD5_CTX*, BOOLEAN*, uint32_t);
void __stdcall MD5Final(BOOLEAN[16], MD5_CTX*);
#endif

namespace Browse
{
class GameList;
};

//========================================================================
// network server browsers (GUN, GameSpy, TCP/IP LAN, IPX LAN)
//========================================================================
class ServerBrowser
{
	friend class ServerBrowserMonitor;

public:
	//
	// called by GameOS
	//
	static BOOLEAN __stdcall InitializeAll(void);
	static BOOLEAN __stdcall SynchronizeAll(void);
	static BOOLEAN __stdcall DisconnectAll(void);
	static BOOLEAN __stdcall ReleaseAll(void);
	static BOOLEAN __stdcall JoinGame(PSTR szGameName, PVOID* ppDplay);
	static BOOLEAN __stdcall Update(int32_t browserHandle = -1);
	static BOOLEAN __stdcall RefreshList(void);
	static BOOLEAN __stdcall CancelAllActivity(void);
	static BOOLEAN __stdcall ServerInfo(PSTR gameID);
	static BOOLEAN __stdcall StillBusy(void);
	static int32_t __stdcall GetBrowserCount(void);

	static void __stdcall CullStaleGames(void);
	static void __stdcall DestroyGameList(void);

	static BOOLEAN IPX; // create server with IPX

	static Browse::GameList* __stdcall GetGameList(void);

	//
	// updating status
	//
	enum GUN_NET_STATUS
	{
		GNS_NONE = 0x0000,
		GNS_DOWNLOADING_PLAYERS = 0x0002,
		GNS_DOWNLOADING_SERVERS = 0x0004
	};

	ServerBrowser() :
		m_bDelete(false) { ; }

protected:
	//
	// server browsers should implement these members, and they will
	// be called by GameOS at various times
	//

	// general use
	virtual BOOLEAN __stdcall Initialize(void) = 0;
	virtual BOOLEAN __stdcall Disconnect(void) = 0;
	virtual BOOLEAN __stdcall Release(void) = 0; // disconnect, unregister, and free
	virtual int32_t __stdcall GetStatus(void) = 0;
	virtual PSTR __stdcall GetDescription(void) = 0;
	virtual BOOLEAN __stdcall Synchronize(void) = 0; // called every frame
	// for every active
	// browser or server
	virtual BOOLEAN __stdcall PrepareRefresh(void) = 0;
	virtual BOOLEAN __stdcall Refresh(PSTR gameID) = 0;
	virtual BOOLEAN __stdcall RefreshServerInfo(PSTR gameID) = 0;
	virtual BOOLEAN __stdcall StartUpdate(void) = 0;
	virtual BOOLEAN __stdcall CancelActivity(void) = 0;
	virtual BOOLEAN __stdcall GetNetStatus(void) = 0;

	// for browsers
	virtual BOOLEAN __stdcall PrepareJoinGame(PSTR szGameName, PVOID* ppDPlay) = 0;

	//
	// server browsers can call these members
	//
	BOOLEAN __stdcall Register(void);
	BOOLEAN __stdcall Unregister(void);

public:
	//
	// the browser handle is a unique string to
	// differentiate one server browser from
	// another
	//
	PSTR __stdcall GetBrowserHandle(void) { return m_szHandle; }

protected:
	BOOLEAN m_bDelete;

private:
	static ServerBrowser* m_pHead;
	static Browse::GameList* m_pGameList;
	ServerBrowser* m_pNext;
	ServerBrowser* m_pPrev;
	static BOOLEAN s_bCancelActivityPending; // true if we have not yet called
		// cancelactivity for browsers during
		// current update cycle.
	int32_t m_handle;
	char m_szHandle[10];
};

extern BOOLEAN __stdcall InitGUNServerBrowser(void);
extern BOOLEAN __stdcall InitGUNServerAdvertiser(void);
extern int32_t __stdcall InitTCPIPServerBrowser(PSTR szIPAddress = 0);
extern BOOLEAN __stdcall InitIPXServerBrowser(void);
extern BOOLEAN __stdcall InitLANServerAdvertiser(void);

//========================================================================
// network server advertisers (GUN, GameSpy)
//========================================================================
class ServerAdvertiser
{
	friend class ServerBrowserMonitor;

public:
	//
	// called by GameOS
	//
	static BOOLEAN __stdcall InitializeAll(void);
	static BOOLEAN __stdcall SynchronizeAll(void);
	static BOOLEAN __stdcall CreateGameAll(PSTR szGameName, PSTR szPlayerName, int32_t MaxPlayers,
		PSTR szPassword, const GUID& guidInstance, uint32_t flags);
	static BOOLEAN __stdcall AddPlayerAll(uint32_t dwItemID, PSTR szPlayerName, BOOLEAN bBot = false);
	static BOOLEAN __stdcall DisconnectAll(void);
	static BOOLEAN __stdcall RemovePlayerAll(uint32_t dwItemID, PSTR szPlayerName, BOOLEAN bBot);
	static BOOLEAN __stdcall ReleaseAll(void);
	static BOOLEAN __stdcall AdvertiseItem(uint32_t dwPlayer, PSTR szName, PSTR szValue);
	static uint32_t __stdcall GetFlagsAllAdvertisers(void);
	static BOOLEAN __stdcall SetFlagsAllAdvertisers(uint32_t flags);

	static BOOLEAN __stdcall GetAdvertiseOK(void);

	ServerAdvertiser() :
		m_bDelete(false) { ; }

protected:
	// general use
	virtual BOOLEAN __stdcall Initialize(void) = 0;
	virtual BOOLEAN __stdcall Disconnect(void) = 0;
	virtual BOOLEAN __stdcall Release(void) = 0; // disconnect, unregister, and free
	virtual PSTR __stdcall GetDescription(void) = 0;
	virtual BOOLEAN __stdcall Synchronize(void) = 0; // called every frame
	// for every active
	// browser or server

	// for servers
	virtual BOOLEAN __stdcall CreateGame(PSTR szGameName, PSTR szPlayerName, int32_t MaxPlayers,
		PSTR szPassword, const GUID& guidInstance, uint32_t flags) = 0;
	virtual BOOLEAN __stdcall CreatePlayer(
		uint32_t dwItemID, PSTR szPlayerName, BOOLEAN bBot = false) = 0;
	virtual BOOLEAN __stdcall RemovePlayer(
		uint32_t dwItemID, PSTR szPlayerName, BOOLEAN bBot = false) = 0;
	virtual BOOLEAN __stdcall SetItemValue(uint32_t dwPlayer, PSTR szName, PSTR szValue) = 0;
	virtual BOOLEAN __stdcall SetFlags(uint32_t flags) = 0;

	//
	// server browsers can call these members
	//
	BOOLEAN __stdcall Register(void);
	BOOLEAN __stdcall Unregister(void);

	BOOLEAN m_bDelete;

private:
	static ServerAdvertiser* m_pHead;
	static BOOLEAN s_bAdvertiseOK;
	static uint32_t m_dwFlags;
	ServerAdvertiser* m_pNext;
	ServerAdvertiser* m_pPrev;
};
