#pragma once
//===========================================================================//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//
//////////////////////////////////////////////////////////////////////
//
// NetworkMessages.hpp: interface for all network messages in Linkup Library
//
#include "directx.hpp"
#include "networking.hpp"
#include "Flinkedlist.hpp"
#include "Net.hpp"

#pragma pack(1)

class FIMessageHeader
{
protected:
	uint16_t flags;

#ifdef _ARMOR
public:
	uint16_t packetNumber;
#endif

	enum
	{
		TypeBits = 0,
		MulticastBit = TypeBits + 11,
		GuaranteedBit
	};

	enum
	{
		TypeMask = 0x3FF, // 1st 10 bits
		MulticastFlag = 1 << MulticastBit,
		GuaranteedFlag = 1 << GuaranteedBit
	};

public:
	FIMessageHeader() { flags = 0; }

	void Init() { flags = 0; }

	// Accessors
	inline BOOLEAN IsMulticast()
	{
		if (flags & MulticastFlag)
			return true;
		else
			return false;
	}

	inline BOOLEAN IsGuaranteed()
	{
		if (flags & GuaranteedFlag)
			return true;
		else
			return false;
	}

	// This message construct limits the application to
	// 1024 unique types of messages.
	inline int32_t GetType() { return flags & TypeMask; }

	// Information setters
	inline void SetMulticast() { flags |= MulticastFlag; }

	inline void SetUnicast() { flags &= ~MulticastFlag; }

	inline void SetGuaranteed() { flags |= GuaranteedFlag; }

	inline void SetNonGuaranteed() { flags &= ~GuaranteedFlag; }

	inline void SetType(uint32_t new_type)
	{
		gosASSERT(new_type < 0x3FF);
		flags &= ~TypeMask; // clear the type
		flags |= (new_type);
	}
};

class MessageTagger
{
public:
	BOOLEAN sendCounts[MAXPLAYERS];

	inline void Clear()
	{
		int32_t i;
		for (i = 0; i < MAXPLAYERS; i++)
		{
			sendCounts[i] = 255;
		}
	}
};

class FIGuaranteedMessageHeader : public FIMessageHeader, public MessageTagger
{
public:
	// Constructor just calls Clear
	FIGuaranteedMessageHeader() :
		FIMessageHeader(), MessageTagger()
	{
		SetGuaranteed(void);
		Clear(void);
	}

	inline void Init()
	{
		Clear(void);
		FIMessageHeader::Init(void);
		SetGuaranteed(void);
	}
};

//#pragma warning (disable : 4200)
class FIGenericGuaranteedMessage : public FIGuaranteedMessageHeader
{
private:
	// Keep the constructor private because we don't want
	// anyone to call it when there is an undefined size
	// for the class.
	FIGenericGuaranteedMessage() :
		FIGuaranteedMessageHeader() {}

public:
	BOOLEAN buffer[0];
};

class FIGenericMessage : public FIMessageHeader
{
private:
	// Keep the constructor private because we don't want
	// anyone to call it when there is an undefined size
	// for the class.
	FIGenericMessage() :
		FIMessageHeader() {}

public:
	BOOLEAN buffer[0];
};

class FIVerifyCluster : public FIMessageHeader
{
protected:
	FIVerifyCluster() :
		FIMessageHeader() {}

public:
	BOOLEAN n_messages;
	MessageTagger message[0];

	void Init()
	{
		FIMessageHeader::Init(void);
		SetType(FIDP_MSG_VERIFY_CLUSTER);
		n_messages = 0;
	}
};

class FIPlayerIDMessage : public FIGuaranteedMessageHeader
{
public:
	uint32_t playerID[MAXPLAYERS];
	BOOLEAN serverIndex;

	FIPlayerIDMessage() :
		FIGuaranteedMessageHeader() { SetType(FIDP_MSG_PLAYERID); }

	int32_t GetPlayerNumber(uint32_t player_id)
	{
		int32_t i;
		for (i = 0; i < MAXPLAYERS; i++)
		{
			if (playerID[i] == player_id)
			{
				return i;
			}
		}
		return -1;
	}

	uint32_t GetServerID() { return playerID[serverIndex]; }
};

class FIPlayersInGroupMessage : public FIGuaranteedMessageHeader
{
public:
	uint32_t groupID;
	uint32_t playerID[MAXPLAYERS];

	FIPlayersInGroupMessage() :
		FIGuaranteedMessageHeader() { SetType(FIDP_MSG_PLAYERS_IN_GROUP); }
};

class FIServerIDMessage : public FIGuaranteedMessageHeader
{
public:
	uint32_t serverID;

	FIServerIDMessage(uint32_t server_id) :
		FIGuaranteedMessageHeader()
	{
		serverID = server_id;
		SetType(FIDP_MSG_SERVERID);
	}
};

//#pragma warning (default : 4200)

#pragma pack()

class FIDPMessage : public NetworkMessageContainer, public ListItem
{
public:
	uint32_t time;
	uint32_t bufferSize;

	// originalTime is the time at which this message was originally
	// sent.  It differs from "time" which is the time which this message
	// was last resent.
	uint32_t originalTime;

	// isResend is set to true when this message is sent for the second time.
	BOOL isResend;

	// numTimesSent is the number of times this message has been sent.
	int32_t numTimesSent;

	// Constructor and destructor
	FIDPMessage(DPID player_id, uint32_t buf_size = MAXMESSAGESIZE);

	virtual ~FIDPMessage(void);

	// Member functions
	//

	void Clear(void);

	inline DPID SenderID() { return senderId; }

	inline DPID ReceiverID() { return receiverId; }

	inline uint32_t MessageSize() { return messageSize; }

	inline PCVOID GetBuffer() { return buffer; }

	inline uint32_t Time() { return time; }

	inline void SetTime(uint32_t new_time)
	{
		time = new_time;
		if (!isResend)
			originalTime = time;
	}

	inline void SetSenderID(DPID id) { senderId = id; }

	inline void SetReceiverID(DPID id) { receiverId = id; }

	uint32_t SetMessageBuffer(LPVOID data, uint32_t nbytes);

	HRESULT ReceiveMessage(void);
};
