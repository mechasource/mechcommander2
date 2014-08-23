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
#define NP_NOT			0
#define NP_TCP			1
#define NP_IPX			2
#define NP_SERIAL		3
#define NP_MODEM		4
#define NP_ZONEMATCH	5


//
// Functionality: Creates the networking object.
//				
void __stdcall gos_InitializeNetworking();

//
// Close any game in progress, clean up networking
//
void __stdcall gos_ShutdownNetwork();




//
// Returns true if connection supported
//
bool __stdcall gos_CheckConnectionAvailable( int Connection );

//
// Open a TCPIP connection
//
// You can pass the address of the server or NULL for the sub net.
//
//
bool __stdcall gos_ConnectTCP( PSTR IPAddress, uint16_t port );


//
// Open a TCPIP and ZoneMatch connection
//
// You can pass the address of the server or NULL for the sub net.
//
//
bool __stdcall gos_ConnectZoneMatch( PSTR IPAddress, uint16_t port );


//
// Open an IPX connection
//
//bool __stdcall gos_ConnectIPX();

//
// Open a Serial connection
//
// You pass the number of the COM port to use
//
//bool __stdcall gos_ConnectSerialPort( ULONG SerialPort );

//
// Open a Modem connection
//
// You pass a phone number and number of modem to use. If 0 is passed for the phone number Answer mode will be entered
//
//bool __stdcall gos_ConnectModem( PSTR PhoneNumber, ULONG Modem );


//
// Create the game specified
//
// 0=OK
// 1=Bad game name
// 2=Bad player name
// 3=Failed
// 4=Waiting for answer
//
int __stdcall gos_CreateGame( PSTR GameName, PSTR PlayerName, int MaxPlayers, PSTR GamePassword=0, bool Secure=0, PSTR UserPassword=0, ULONG dwFlags=0);


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
int __stdcall gos_JoinGame( PSTR GameName, PSTR PlayerName, PSTR GamePassword=0 , PSTR UserPassword=0);





//
// Functionality: If connected to a network protocol, gos_Disconnect resets the connection
//
void __stdcall gos_Disconnect();


//
// Functionality: If browser services are available, advertise the indicated key/value pair
//
bool __stdcall gos_NetSetAdvertItem( ULONG player, PCSTR Name , PCSTR  Value);


void NGStatsSetPlayerId(PSTR name, PSTR passwd);

extern GUID NGStatsPlayerId;



GUID __stdcall gos_GenerateUniqueGUID();

bool __stdcall CheckForZoneMatch();


// GUN Status functions
int __stdcall GetGUNStatus( void );
int __stdcall GetGUNRegStatus( void );
int __stdcall GetGUNNetStatus( void );
int __stdcall GetGUNLastError( void );
PCSTR  __stdcall GetGUNErrorMessage( void );
void __stdcall GetGUNDownloadStats( int * tableSize, int * progress );





// -----------------MD5 Hashing stuff

/* UINT4 defines a four byte word */
// Alphas really have 8 bytes for longs, which this code
// assumes only 4.  The original line is commented in case
// something is terribly terribly wrong with the fix.
//typedef ULONG int UINT4;
typedef uint32_t UINT4;

typedef struct {
  UINT4 state[4];                                   /* state (ABCD) */
  UINT4 count[2];        /* number of bits, modulo 2^64 (lsb first) */
  uint8_t buffer[64];                         /* input buffer */
} MD5_CTX;



// do not remove, used for run time checks against 
// source code in network games. 
void __stdcall MD5SecurityCheckStart();
void __stdcall MD5SecurityCheckStop();


void __stdcall MD5Init (MD5_CTX *);
void __stdcall MD5Update  (MD5_CTX *, puint8_t, uint32_t);
void __stdcall MD5Final (uint8_t [16], MD5_CTX *);





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
	static bool __stdcall InitializeAll(void);
	static bool __stdcall SynchronizeAll(void);
	static bool __stdcall DisconnectAll(void);
	static bool __stdcall ReleaseAll(void);
	static bool __stdcall JoinGame(PCSTR szGameName, PVOID*ppDplay );
	static bool __stdcall Update(int browserHandle = -1 );
	static bool __stdcall RefreshList(void);
    static bool __stdcall CancelAllActivity(void);
	static bool __stdcall ServerInfo(PCSTR  gameID);
	static bool __stdcall StillBusy(void);
    static int  __stdcall GetBrowserCount( void );

    static void __stdcall CullStaleGames( void );
    static void __stdcall DestroyGameList( void );

	static bool IPX;			// create server with IPX

	static Browse::GameList * __stdcall GetGameList(void);

	//
	// updating status
	//
	enum GUN_NET_STATUS
	{
	    GNS_NONE                 = 0x0000,
	    GNS_DOWNLOADING_PLAYERS  = 0x0002,
	    GNS_DOWNLOADING_SERVERS  = 0x0004
	};

	ServerBrowser() : m_bDelete(false)
	{;}


protected:
	//
	// server browsers should implement these members, and they will
	// be called by GameOS at various times
	//

	// general use
	virtual	bool __stdcall Initialize(void) = 0;
	virtual	bool __stdcall Disconnect(void) = 0;
	virtual bool __stdcall Release(void) = 0;		// disconnect, unregister, and free
	virtual int __stdcall GetStatus(void) = 0;
	virtual PCSTR  __stdcall GetDescription(void) = 0;
	virtual bool __stdcall Synchronize(void) = 0;	// called every frame
													// for every active
													// browser or server
	virtual bool __stdcall PrepareRefresh(void) = 0;
	virtual bool __stdcall Refresh(PSTR gameID) = 0;
	virtual bool __stdcall RefreshServerInfo(PSTR gameID) = 0;
	virtual bool __stdcall StartUpdate(void) = 0;
	virtual bool __stdcall CancelActivity(void) = 0;
	virtual bool __stdcall GetNetStatus(void) = 0;


	// for browsers
    virtual bool __stdcall PrepareJoinGame(PCSTR  szGameName, PVOID*ppDPlay ) = 0;

	//
	// server browsers can call these members
	//
	bool __stdcall Register(void);
	bool __stdcall Unregister(void);

public:
	//
	// the browser handle is a unique string to
	// differentiate one server browser from
	// another
	//
	PCSTR  __stdcall GetBrowserHandle(void) { return m_szHandle; }

protected:
	bool						m_bDelete;

private:
	static ServerBrowser *		m_pHead;
	static Browse::GameList *	m_pGameList;
	ServerBrowser *				m_pNext;
	ServerBrowser *				m_pPrev;
	static bool					s_bCancelActivityPending; // true if we have not yet called cancelactivity for browsers during current update cycle.
	int							m_handle;
	char						m_szHandle[10];
};


extern bool __stdcall InitGUNServerBrowser(void);
extern bool __stdcall InitGUNServerAdvertiser(void);
extern int __stdcall InitTCPIPServerBrowser(PSTR szIPAddress=0);
extern bool __stdcall InitIPXServerBrowser(void);
extern bool __stdcall InitLANServerAdvertiser(void);


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
	static bool __stdcall InitializeAll(void);
	static bool __stdcall SynchronizeAll(void);
	static bool __stdcall CreateGameAll(PCSTR szGameName, PCSTR szPlayerName, int MaxPlayers, PCSTR szPassword, const GUID &guidInstance, ULONG dwFlags);
	static bool __stdcall AddPlayerAll(ULONG dwItemID, PCSTR  szPlayerName, bool bBot=false);
	static bool __stdcall DisconnectAll(void);
	static bool __stdcall RemovePlayerAll(ULONG dwItemID, PCSTR  szPlayerName, bool bBot );
	static bool __stdcall ReleaseAll(void);
	static bool __stdcall AdvertiseItem(ULONG dwPlayer, PCSTR  szName , PCSTR  szValue);
	static ULONG __stdcall GetFlagsAllAdvertisers(void);
	static bool __stdcall SetFlagsAllAdvertisers(ULONG dwFlags);
    
    static bool __stdcall GetAdvertiseOK(void);

	ServerAdvertiser() : m_bDelete(false)
	{;}

protected:
	// general use
	virtual	bool __stdcall Initialize(void) = 0;
	virtual	bool __stdcall Disconnect(void) = 0;
	virtual bool __stdcall Release(void) = 0;		// disconnect, unregister, and free
	virtual PCSTR  __stdcall GetDescription(void) = 0;
	virtual bool __stdcall Synchronize(void) = 0;	// called every frame
													// for every active
													// browser or server


	// for servers
	virtual bool __stdcall CreateGame(PCSTR szGameName, PCSTR szPlayerName, int MaxPlayers, PCSTR szPassword, const GUID &guidInstance, ULONG dwFlags) = 0;
	virtual bool __stdcall CreatePlayer(ULONG dwItemID, PCSTR szPlayerName, bool bBot=false) = 0;
	virtual bool __stdcall RemovePlayer(ULONG dwItemID, PCSTR szPlayerName, bool bBot=false) = 0;
	virtual bool __stdcall SetItemValue(ULONG dwPlayer, PCSTR  szName , PCSTR  szValue) = 0;
	virtual bool __stdcall SetFlags(ULONG dwFlags)=0;

	//
	// server browsers can call these members
	//
	bool __stdcall Register(void);
	bool __stdcall Unregister(void);

	bool						m_bDelete;

private:
	static ServerAdvertiser *	m_pHead;
	static bool					s_bAdvertiseOK;
	static ULONG				m_dwFlags;
	ServerAdvertiser *			m_pNext;
	ServerAdvertiser *			m_pPrev;
};

