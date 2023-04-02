//
//============================================================
//
// Contents:  Network header file
//
//===========================================================================//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//
//

#pragma once

#include "network.hpp"
#include "directx.hpp"

// define
#define OUTBOUND_WINDOW

//
// Packet overhead of DirectPlay
//
enum class __net_header_const
{
	PACKET_OVERHEAD = 42,
	MAX_DP_OBJECTS = 8
};

//
//
// Used to store network packets
//
//
typedef struct _PacketBuffer
{
	struct _PacketBuffer* pNext;
	size_t Size; // Size of buffer
	size_t PacketSize;
	uint32_t FromID;
	uint32_t ToID;
	uint64_t TimeStamp;
#if CONSIDERED_OBSOLETE
	BOOLEAN Encrypted; // changed from BOOLEAN due to packing
#else
	uint32_t Encrypted;
#endif
	BOOLEAN Data[0]; // Data follows
} PacketBuffer;

//
// List of names, used for modems, serial ports, players, games etc...
//
typedef struct _ListOfNames
{
	struct _ListOfNames* pNext; // Pointer to next in chain
	uint32_t Data; // Data that is list dependent
	uint32_t PacketsReceived; // NUmber of packets received from a player
	uint64_t LastTime; // Time last packet received
	char Name[1]; // Name
} ListOfNames;
typedef ListOfNames* PListOfNames;

//
// List of games (and players in those games)
//
typedef struct _ListOfGames
{
	struct _ListOfGames* pNext; // Pointer to next in chain
	DPSESSIONDESC2 SessionDescriptor; // Session description
	ListOfNames* pPlayers; // Pointer to list of player names
	BOOLEAN bIPX; // whether the game is hosted with IPX or not (TCP/IP)
	char Name[1]; // Name
} ListOfGames;

//
// List of messages waiting for the application to get
//
typedef struct _Messages
{
	struct _Messages* pNext;
	NetPacket Data;
} Messages;

//
// All memory in networking must be allocated on this heap
//
// extern HGOSHEAP Heap_Network;

//
// Interfaces
//
extern IDirectPlayLobby3* dplobby3; // Lobby interface
extern IDirectPlay4* dplay4; // Direct Play interface

//
// Current joined or opened session description
//
extern DPSESSIONDESC2 CurrentSession;

//
// Last time that games and players were enumerated
//
extern double LastEnumTime;
extern BOOLEAN WaitingForEnum;

//
// Protocols
//
extern BOOLEAN hasTCP;
extern BOOLEAN hasIPX; // True when protcol is valid
extern BOOLEAN hasModem;
extern BOOLEAN hasSerial;
extern BOOLEAN hasZoneMatch;

//
// True when connection exists
//
extern int32_t Connected;

//
// Result from last enumeration on a thread
//
extern HRESULT EnumResult;

//
// True when a game created or joined
//
extern BOOLEAN InNetworkGame;

//
// True when created the game
//
extern BOOLEAN IAmTheServer;

//
// Current players ID
//
extern uint32_t MyDirectPlayID;

//
// string containing my address (when TCPIP)
//
extern PSTR MyIPAddress;

//
// Valid serial ports
//
extern ListOfNames* SerialPortNames;

//
// Valid modem names (maximum of 8)
//
extern ListOfNames* ModemNames;

//
// Points to a list of the current game names found on the network
//
extern ListOfGames* GameNames;

extern PSTR* ListOfPassworded;

//
// Points to a list of the current game names found on the network being
// enumerated
//
extern ListOfGames* EnumNames;

//
// List of messages on the recieve queue
//
extern Messages* pMessages;

//
// List of players in the current game
//
extern ListOfNames* CurrentPlayers;

//
// Buffer used while receiving messages
//
extern BOOLEAN* MessageBuffer;
extern uint32_t MessageBufferSize;

//
// Points to a list of messages that the Application will pull message from
//
extern Messages* pMessages;
//
// Points to the last message read by the Application (will be freed)
//
extern Messages* pLastMessage;

//
// Linked list of sent/recieved packets
//
typedef struct _PacketLogging
{
	_PacketLogging* pNext;
	double TimeStamp; // When packet was added to list
	uint32_t FrameNumber; // Frame number added
	char Type[11]; // Type of packet
	char Player[11]; // From or To player name
	uint32_t FromID; // ID of FROM player
	uint32_t ToID; // ID of TO player
	uint32_t Size; // Size of packet
	BOOLEAN Data[8]; // 1st 8 bytes

} PacketLogging;
//
// Pointer to latest packet
//
extern PacketLogging* pPacketLog;
//
// Exact time network stats were grabbed
//
extern double NetworkDebugTime;
//
// Pointer to log at start of frame
//
extern PacketLogging* pDebugPacketLog;

//
// Structure sent with all network packets
//
#pragma pack(push, 1)
#ifdef OUTBOUND_WINDOW
typedef struct _PacketHeader
{
	BOOLEAN Type; // User defined type (224-> system messages)
	uint16_t ThisPacketNumber;
	uint16_t LastPacketReceived;

} PacketHeader;
#else
typedef struct _PacketHeader
{
	BOOLEAN Type; // User defined type (224-> system messages)

} PacketHeader;
#endif
#pragma pack(pop)

//
// Routine's
//
void __stdcall CheckProtocols(void);
void __stdcall ReceivePackets(void);
void __stdcall GetCurrentPlayers(void);
void __stdcall AddGOSMessage(Messages* pMessage);
BOOL __stdcall EnumSessionsCallback(
	LPCDPSESSIONDESC2 lpSessionDesc, uint32_t* lpdwTimeOut, uint32_t flags, PVOID pcontext);
BOOL __stdcall EnumJoinSessionCallback(
	LPCDPSESSIONDESC2 lpSessionDesc, uint32_t* lpdwTimeOut, uint32_t flags, PVOID pcontext);
BOOL __stdcall EnumPlayersCallback(
	DPID dpId, uint32_t dwPlayerType, LPCDPNAME lpName, uint32_t flags, LPVOID pcontext);
BOOL __stdcall ModemCallback(
	REFGUID guidDataType, uint32_t dwDataSize, LPCVOID pdata, PVOID pcontext);
BOOL __stdcall TCPIPCallback(
	REFGUID guidDataType, uint32_t dwDataSize, LPCVOID pdata, PVOID pcontext);
void __stdcall WaitTillQueueEmpty(void);
void __stdcall AddPlayerToGame(ListOfNames** pListOfPlayers, PSTR Name, DPID dpId);
void __stdcall RemovePlayerFromGame(ListOfNames** pListOfPlayers, PSTR Name, DPID dpId);
PSTR __stdcall GetName10(uint32_t Id);
void __stdcall UpdateNetworkDebugInfo(void);
PSTR __stdcall DecodeIPAddress(DPLCONNECTION* pConnection);
uint16_t __stdcall DecodePORTAddress(DPLCONNECTION* pConnection);

//
// Receive packet thread variables
//
extern uint32_t __stdcall NetworkThread(PVOID);
extern uint32_t NetworkThreadID;
extern HANDLE HandleNetworkThread;
extern CRITICAL_SECTION NetworkCriticalSection;
extern HANDLE NetworkEvent;
extern HANDLE KillNetworkEvent;
extern uint32_t __stdcall NetworkThread(PVOID);

//
// Enumeration thread variables
//
extern uint32_t EnumThreadID;
extern HANDLE HandleEnumThread;
extern CRITICAL_SECTION EnumCriticalSection;
extern HANDLE EnumEvent;
extern HANDLE KillEnumEvent;
extern uint32_t __stdcall EnumThread(PVOID);

//
// Used to hold list of packets to pass data between threads
//
extern PacketBuffer* pEmptyList; // List of spare blocks
extern PacketBuffer* pPackets; // List of received packets
extern PacketBuffer* pLastPacket; // End of list of received packets

//
// Functions used initialize game list drivers
//

int32_t
InitLanGames(void);
int32_t
InitGUNGames(void);
void
GUNDestroyNetworking(void);
void
CheckForInternet(void);

// InternalJoinGame(void) needs this for joining GUN Games.
void
PushGameList(void);
BOOLEAN
GUNPrepareDPlay(PSTR GameName);

HRESULT
QuickEnum(BOOLEAN async); // quickly begin dplay enumeration of sessions.

#ifdef OUTBOUND_WINDOW
typedef struct tagPACKETQUEUE
{
	NetPacket* pPacket;
	struct tagPACKETQUEUE* pNext;
} PACKETQUEUE;

//========================================================================
// OutboundWindow
//
// The OutboundWindow class is an object that manages how many non
// guaranteed packets are allowed to be outstanding and not accounted for.
// This prevents us from filling the modem send queue qith messages.
//
// What we do is we stamp each outgoing message with a count, and the
// remote machine informs us of the last packet it received with every
// packet it sends to us. This way, we can know how many outstanding non
// guaranteed packets are outstanding at any time. If there are more than
// some predetermined number (the "window"), then any attempt to send a
// packet fails.
//
// A client will have a single OutboundWindow object, associated with the
// server. A server will have one for each client.
//
// If we haven't sent the last packet we've received after a certain
// amount of time, we manually send this in a packet that contains ONLY
// this.
//========================================================================
class OutboundWindow
{
public:
	static const int32_t m_WindowSize;
	static const double m_ResendTime;

	// we only have one constructor: you MUST supply the DPID when you create
	// one!!!
	OutboundWindow(DPID);

	static BOOLEAN __stdcall Synchronize(void); // call every frame
	static BOOLEAN __stdcall CleanAll(void);
	static OutboundWindow* Find(DPID dpid);
	BOOLEAN __stdcall Add(void);
	BOOLEAN __stdcall Remove(void);
	PVOID operator new(size_t size) { return gos_Malloc(size, Heap_Network); }
	void operator delete(PVOID ptr) { gos_Free(ptr); }
	void operator delete[](PVOID ptr) { gos_Free(ptr); }

public:
	double m_TimeLastPacketSent;
	uint16_t m_NextPacketNumberToSend;
	uint16_t m_LastPacketWeReceived;
	uint16_t m_LastPacketTheyReceived;
	PACKETQUEUE* m_PacketQueue;
	DPID m_dpid; // the dpid of the associated remote machine
	OutboundWindow* m_pNext;

	static OutboundWindow* m_pHead;

	// PacketLogging* pThis=(PacketLogging*)gos_Malloc( sizeof(PacketLogging),
	// Heap_Network );
};

#endif // OUTBOUND_WINDOW
