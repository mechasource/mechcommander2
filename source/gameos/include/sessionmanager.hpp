//===========================================================================//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//
// SessionManager.h: interface for the SessionManager class.
//
//============================================================
//
// Contents:  Low level networking
//
// Owner   :  AndyG
//
// All Rights reserved worldwide (c) Microsoft
// This unpublished sourcecode is PROPRIETARY and CONFIDENTIAL               
//============================================================

#pragma once

#include "networkingglobals.hpp"
#include "flinkedlist.hpp"
#include "session.hpp"
#include "netplayer.hpp"
#include "networkmessages.hpp"
#include "network.hpp"

typedef void(__stdcall *LPVOIDCALLBACK)(void);

//
// Current session
//
extern FIDPSession *currentSession;

//
// Current connection
//
extern int currentConnectionType;
extern bool Connected;

//
// Modem/Serial has dialed
//
extern bool Connected;

//
// Modem variables
//
extern char modemNames[10][64];
extern ULONG NumModems;
extern bool modemChecked;
extern bool hasModem;
bool __stdcall FindModems(void);
bool __stdcall gos_ConnectModem( PSTR phone_number, PSTR modem_name );

//
// COM port variables
//
bool __stdcall gos_ConnectComPort(ULONG com_port);


//
// IPX variables
//
extern bool hasIPX;
bool __stdcall gos_ConnectIPX(void);

//
// TCPIP variables
//
extern bool hasTCP;
bool __stdcall gos_ConnectTCP( PSTR ip_address, USHORT port );

/////  AddressInfo is used in connections initialized entirely by
// user defined data, such as a TCP connection created by calling
// the CreateCompoundAddress function and using it as the connection.
typedef struct _DPAddressInfo
{
	ULONG	type;
	ULONG	size;
	PVOID	data;
} DPAddressInfo;


//////////////////////////////////////////////////////////////////////
// SessionManager class.  The SessionManager deals with all DirectPlay
// specific functionality.  It allows the user to call a few simple functions
// to get a network game going and send/receive messages.  
//
//	----------   Using the SessionManager: ---------------
//  The user must create one instance (and only one) of the SessionManager,
// passing it the Application's GUID.
//
//  Once the SessionManager is created, the user must choose a connection type
// and call Connect() with the appropriate connection ID.  If using connections
// from the GetConnections() method, the user can simply call Connect().  
// Otherwise, one of the ConnectThruXXX functions must be called.
//
//	Once connected, the user can either Host or Join a session.  To host a session,
// simply create a temporary session object and pass it into the HostSession() function.
// To join a session, the user must get a GUID from the list of sessions returned by
// the GetSessions() function, then call JoinSession() with that GUID.
//
//  Now, a player has been created for the user, and the session is setup to send
// and receive messages.  Messages are constantly being retrieved from the 
// ReceiveThread, so the user can process them anytime he wishes by calling
// ProcessSystemMessages() and ProcessApplicationMessages() respectively.  The user
// must additionally call UpdateGuaranteedMessages() at a high frequency in order
// to re-send lost messages.
//
//	Sending Messages:
// To send a message, the user must create an application specific message
// descended from either FIMessageHeader or FIGuaranteedMessageHeader.  The
// constructor should call SetType() and set the message type to a constant
// added to the enumerated message type list.  Once the message is created and
// all parameters are set, it can be sent by calling the appropriate send function. 
// 
// 
// call
//
//////////////////////////////////////////////////////////////////////

class SessionManager
{
public:

	FLinkedList<FIDPSession>	listOfSessions;
	FLinkedList<FIDPPlayer>		listOfPlayers;
	FLinkedList<FIDPMessage>	listOfEmptyMessages;
	FLinkedList<FIDPMessage>	listOfSystemMessages;
	FLinkedList<FIDPMessage>	listOfApplicationMessages;
	FLinkedList<FIDPMessage>	guaranteedMessagesToProcess;
	FLinkedList<FIDPMessage>	guaranteedMessagesToSend;
	FLinkedList<FIDPMessage>	serverMessageHoldList;


	// The needIDList is used when a player is added to the session,
	// but we don't know what playerNumber to assign it.  When a 
	// playerID message comes through, we can go through the needIDList
	// and give these players their numbers. 
	FLinkedList<FIDPPlayer> needIDList;

	// rtPlayerListIterator is used to iterate the player list from the
	// receiveThread.
	FListIterator<FIDPPlayer> *rtPlayerListIterator;

	// this list is filled in with players who have dropped out.
	DPID cancelledPlayerID[MAXPLAYERS];

	FIServerIDMessage *serverIDMessage;

	HANDLE hPlayerEvent;
	HANDLE hKillReceiveEvent;
	HANDLE playerThread;
	DPID thisPlayerID;
	DPID serverID;
	FIDPPlayer *thisPlayer;
	ULONG threadID;

	BOOL launchedFromLobby; // TRUE if this app was launched through a DPlay lobby.
	BOOL lobbyChecked; // TRUE if we have initialized launchedFromLobby.
	BOOL inReceiveThread; // TRUE while in ReceiveThread, FALSE while in another thread.
	BOOL DisabledCallerID;
	BOOL CallerIDChanged[6];


	int32_t enterGameTime;


	// nextSessionID is the ID of the next session added.  When enumerating all
	// sessions from scratch, this number is set back to zero.
	int nextSessionID;

	// enableAutoDialValue is the value of the EnableAutodial key in the
	// registry.  
	ULONG enableAutodialValue;

	HANDLE ReceiveMutex;
	HANDLE EmptyMessageListMutex;
	CRITICAL_SECTION ProcessingMessageList;
	CRITICAL_SECTION AddingMessageList;

	// bSessionHost is set to TRUE when this application hosts
	// a session.
	BOOL bSessionHost;

	// canSendGuaranteedMessages is set to true when this player
	// gets a list of player ids from the server.
	BOOL canSendGuaranteedMessages;




	int NetworkProtocolFlags;

	uint8_t messageBuffer[1024];


	DPID newPlayerNumbers[MAXPLAYERS];


	// nextFileSendID is a number used as a file handle for files
	// being send across the network.
	int nextFileSendID;

#ifdef _ARMOR
	ULONG lastBandwidthRecordingTime;
	ULONG bytesSent;
	ULONG lastRecordedBytesSent;
	ULONG packetNumber;
	ULONG lastPacketNumber;
	ULONG packetsInBandwidthCheck;
#endif


	FIVerifyCluster *verifyClusterBuffer;
	FIVerifyCluster *verifyMessageCluster[MAXPLAYERS];
	DPID receiveThreadPlayerIDs[MAXPLAYERS];

	int serverOrder[MAXPLAYERS];
	ULONG pingUpdateTime;
	ULONG pingUpdateFrequency;


	HRESULT	ResetDirectPlayInterface(void);

	HRESULT CreatePlayer(PSTR player_name);


	inline HRESULT SetSessionDescriptor(FIDPSession *ss)
	{
		return wSetSessionDesc(dplay4, ss -> GetSessionDescriptor(void),0);
	}


	void SendVerifyMessage(int player_number, FIDPMessage *msg, int send_count);


	void EnumeratePlayers(FIDPSession *session);

	void AddPlayerOrGroup(
		ULONG dwPlayerType,
		DPID dpId,
		DPID dpIdParent,
		LPDPNAME dpnName,
		ULONG dwFlags);

	// DeletePlayerOrGroup is called when a player or group leaves the
	// game -- after calling AppCallback.  It actually deletes this player
	// or group from the linked list.
	void DeletePlayerOrGroup(
		ULONG dwPlayerType,
		DPID dpId);


	// PlayerOrGroupLeaving is called when a player or group leaves the
	// game -- before calling AppCallback.
	void PlayerOrGroupLeaving(	ULONG dwPlayerType,
		DPID dpId);

	void HandleApplicationMessage(FIDPMessage *msg);

	void UpdatePlayerGuaranteedMessages
		(FIDPPlayer *player, ULONG time);

	// ProcessGuaranteedMessages is called by ProcessApplicationMessages
	// once all messages have been initially handled.  Guaranteed messages
	// are processed in 2 steps.  The first puts them in the proper hold 
	// buffers.  The second gets messages ready to be processed from the
	// hold buffers.
	void ProcessGuaranteedMessages(void);

	// The following 2 functions are called to add and remove messages from
	// the empty message queue.  They use mutexes so the 2 threads don't
	// both access the queue at the same time.
	void AddMessageToEmptyQueue(FIDPMessage *msg);

	FIDPMessage *GetMessageFromEmptyQueue(void);




public:
	// Returns the session in the listOfSessions with a matching guid
	// or NULL if there is no match.
	FIDPSession *FindMatchingSession(LPGUID pguid);
private:


	// Functions called by the Callbacks.  These functions are
	// essentially the callbacks for enumerating connections,
	// sessions, players, and groups.  However, since the callback
	// itself cannot be a member function of a class, the callback
	// bounces the message into the class using the lpContext member.
	BOOL FAR PASCAL AddConnection(
		LPCGUID     lpguidSP,
		PVOID		lpConnection,
		ULONG		dwConnectionSize,
		LPCDPNAME   lpName,
		ULONG 		dwFlags,
		PVOID 		lpContext);

	BOOL FAR PASCAL AddSession(
		LPCDPSESSIONDESC2 lpThisSD,
		PULONG lpdwTimeOut,
		ULONG dwFlags,
		PVOID lpContext
		);

	BOOL FAR PASCAL NewPlayerEnumeration(DPID dp_id,
		ULONG dwPlayerType,
		LPCDPNAME lpName,
		ULONG dwFlags);

	// RTProcessApplicationMessage is called by the receive thread when
	// a new application message arrives.
	void RTProcessApplicationMessage(FIDPMessage *message_info);

	// RTHandleNewGuaranteedMessage is called to handle an incoming guaranteed
	// message in the receive thread.
	void RTHandleNewGuaranteedMessage(FIDPMessage *message_info, FIDPPlayer *sending_player);

	// returns a DPID from a player number
	DPID RTGetIDFromPlayerNumber(int pn);

	FIDPPlayer *RTGetPlayer(DPID id);

	void ClearSessionList(void);


	void HandlePreSystemMessage(FIDPMessage *msg);
	void HandlePostSystemMessage(FIDPMessage *msg);

	void SetupMessageSendCounts(
		FIGuaranteedMessageHeader *message,
		FLinkedList<FIDPPlayer> *player_list);

	// SendPreIDGuaranteedMessages is called once when the new player
	// gets a list of player numbers and the server ID.
	void SendPreIDGuaranteedMessages(void);

	// GivePlayerAnID assigns the first available playerNumber to 
	// this player.  If there is no gap in assigned numbers, this
	// player gets the next one in line.
	void GivePlayerAnID(FIDPPlayer *player);


public:

	SessionManager(GUID app_guid);

	virtual ~SessionManager(void);

	virtual void destroy (void);


	friend BOOL FAR PASCAL DPSessionMgrSessionsCallback(
		LPCDPSESSIONDESC2	lpSessionDesc,
		PULONG				lpdwTimeOut,
		ULONG				dwFlags,
		PVOID				lpContext);

	friend BOOL FAR PASCAL EnumPlayersCallback (DPID dpId,
		ULONG dwPlayerType,
		LPCDPNAME lpName,
		ULONG dwFlags,
		PVOID lpContext);

	friend BOOL FAR PASCAL EnumGroupsCallback (DPID dpId,
		ULONG dwPlayerType,
		LPCDPNAME lpName,
		ULONG dwFlags,
		PVOID lpContext);

	friend ULONG WINAPI
		SessionManagerReceiveThread(PVOID lpThreadParameter);


	// ReceiveThread is called by the SessionManagerReceiveThread
	// callback to handle thread events.
	int ReceiveThread(void);



	// InitializeConnection is called by one of the following functions
	// to allow the user to bypass the crappy windows dialogs.
	int32_t InitializeConnection(
		DPCOMPOUNDADDRESSELEMENT *compound_address, int n_items);

	int32_t Dial(void);
	void CancelDialing(void);


	// Call this function at program start up to find out if
	// we were launched from a lobby.
	BOOL WasLaunchedFromLobby(void);

	// If we were launched from a lobby, SetupLobbyConnection creates
	// a protocol, session, and player to start the game.
	ULONG SetupLobbyConnection(LPVOIDCALLBACK create_callback,LPVOIDCALLBACK destroy_callback);


	// returns the name of the modem at index or NULL if index
	// is greater than the # of modems.
	PSTR GetModemName(int32_t index);

	// Host or join a session
	//
	// This must be done before attempting to Send or Receive
	// any messages
	// A return value of DP_OK means it worked
	//	A return value of -1 means the guid passed to JoinSession is
	// invalid.  Any other return value is a directplay error code.
	HRESULT HostSession(FIDPSession& new_session, PSTR player_name);

	HRESULT JoinSession(LPGUID p_guid, PSTR player_name);

	// LockSession can only be done by the host.  If successful, it
	// returns 1, else it returns 0
	int LockSession(void);

	// LeaveSession kills the current player and removes him from the session.
	int LeaveSession(void);


	void CreateGroup 
		(	LPDPID id, 
		PSTR name, 
		PVOID data = NULL, 
		ULONG size = 0, 
		ULONG flags = 0);

	void SetGroupData 
		(	DPID id, 
		PVOID data, 
		ULONG size, 
		ULONG flags = 0);

	// If player_id is 0 or not filled in, the local player is
	// added.  If the group doesn't exist, the player isn't added.
	// returns 1 if successful, 0 if not.
	int AddPlayerToGroup(DPID group_id, DPID player_id=0);

	int RemovePlayerWithID(DPID player_id);

	int RemovePlayerFromGame(FIDPPlayer *p);

	// Returns 1 if successful, 0 if not.
	int RemovePlayerFromGroup(DPID group_id, DPID player_id=0);

	void GetPlayerListForGroup(
		DPID id, 
		FLinkedList<FIDPPlayer> *player_list);



	// 3. Send a message
	//
	// Messages can be broadcasted to all players,
	// sent to a group, or
	// sent to individual players.

	void SendMessageToGroupGuaranteed(
		DPID group_id, 
		FIGuaranteedMessageHeader *message, 
		ULONG size);

	void SendMessageToPlayerGuaranteed(
		DPID player_id, 
		FIGuaranteedMessageHeader *message, 
		ULONG size,
		BOOL set_send_count = TRUE);

	void SendMessageToServerGuaranteed(
		FIGuaranteedMessageHeader *message, 
		ULONG size);

	void BroadcastMessage(FIMessageHeader *message, ULONG size);

	void SendMessageToServer(
		FIMessageHeader *message, 
		ULONG size);

	HRESULT SendMessage(
		DPID group_or_player_id,
		FIMessageHeader *message, 
		ULONG size);

	HRESULT SendGOSMessage(NetworkMessageContainer& message_info);

	void SendMessageFromInfo(FIDPMessage *message);

	int SendVerifies(void);


	// Accessor functions to update lists of network specifics
	FLinkedList<FIDPSession> *GetSessions(bool refresh=true);
	FLinkedList<FIDPPlayer> *GetPlayers(FIDPSession *session=NULL);


	inline FIDPSession *GetCurrentSession(void)
	{
		return currentSession;
	}

	inline FIDPPlayer *GetLocalPlayer(void)
	{
		return thisPlayer;
	}

	DPID GetServerID(void)
	{
		return serverID;
	}


	// FindSession returns the session with the corresponding id or NULL
	// if no match is found.
	FIDPSession *FindSession(int session_id);

	// IsLocked returns the status of the session with the given session_id.
	// If the session_id is -1, it returns the status of the current session.
	BOOL IsLocked(int session_id = -1);

	FIDPPlayer *GetPlayerNumber(int32_t i);

	FIDPPlayer *GetPlayer(ULONG id);

	inline BOOL IsLocalHost(void)
	{
		return bSessionHost;
	}


	// ProcessMessages should be called to get all new messages and
	// send guaranteed and file messages.
	void ProcessMessages(void);

	//ProcessSystemMessages processes all of the system messages in the
	//receive buffer.  If the application needs to deal with them
	//individually, the sys_callback function is called for each one.
	int ProcessSystemMessages(void);

	int ProcessApplicationMessages(void);

	// UpdateGuaranteedMessages should be sent by the application???
	void UpdateGuaranteedMessages(void);



};


extern SessionManager *globalSessionManager;

void __stdcall InitNetworking(void);

