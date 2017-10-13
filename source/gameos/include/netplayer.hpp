//===========================================================================//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//
// DPPlayer.h: interface for the FIDPPlayer class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(NETPLAYER_HPP)
#define NETPLAYER_HPP

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#ifndef FLINKEDLIST_HPP
#include "flinkedlist.hpp"
#endif

#ifndef NETWORKMESSAGES_HPP
#include "networkmessages.hpp"
#endif


class FIDPPlayer: public ListItem
{
protected:
	char				shortName[128];
	char				longName[256];
	uint32_t		playerID;
	uint32_t		playerFlags;

	// The awaitingVerificationList holds a list of messages
	// that were sent to this player and are awaiting
	// verification.
	FLinkedList<FIDPMessage>		verifyList;

	// The incomingHoldBuffer holds a list of guaranteed messages
	// that came in out of order.  They will be processed as soon
	// as the item before them is processed.
	FIDPMessage* incomingHoldBuffer[256];

	// the playerCriticalSection protects data from being accessed by
	// the receive thread and the main thread at once.a
	CRITICAL_SECTION playerCriticalSection;



public:
	// sendCount is used for guaranteed messages.  It
	// is always between 0 and 255 and it represents
	// the next message sent to this player
	uint8_t				sendCount;

	// The incomingHoldIndex is the next expected sendCount for
	// incoming messages from this player.
	uint8_t incomingHoldIndex;

	uint8_t nextIncomingSendCount;

	int32_t nItemsInHoldBuffer;

	// player_number is used for guaranteed messages.  It represents
	// which 4 bits to use for this player's messages.
	int32_t					playerNumber;

	// isActive is set to true when the player is created, and is set
	// to FALSE when the player is kicked out.
	BOOL				isActive;


	// messageResendTime is the time to wait before re-sending a message.  If
	// a message is re-sent, this value is bumped up a bit.  If the average
	// latency is well below the resend time, it is dropped.
	uint32_t		messageResendTime;

	// physicalMemory is the amount of RAM on this player's machine.
	uint32_t		physicalMemory;




	// Public functions
	FIDPPlayer(void);
	FIDPPlayer(DPID& id,
			   LPCDPNAME name,
			   uint32_t flags);
	virtual ~FIDPPlayer(void);


	// SetMessageSendCount is called each time a new message is
	// prepared to send to this player.
	inline void SetMessageSendCount(FIGuaranteedMessageHeader* msg)
	{
		sendCount++;
		if (sendCount == 255)
			sendCount = 0;
		msg -> sendCounts[playerNumber] = sendCount;
	}

	void JoinGroup(DPID group);

	void LeaveGroup(DPID group);

	inline void  SetShortName(PSTR name)
	{
		if (name != nullptr)
		{
			strncpy(shortName, name, 127);
		}
		else
		{
			strcpy(shortName, "");
		}
	}

	inline void SetLongName(PSTR name)
	{
		if (name != nullptr)
		{
			strncpy(longName, name, 255);
		}
		else
		{
			strcpy(longName, "");
		}
	}

	inline PCSTR GetShortName()
	{
		return shortName;
	}

	inline PCSTR GetLongName()
	{
		return longName;
	}

	inline DPID PlayerID()
	{
		return playerID;
	}

	inline FLinkedList<FIDPMessage>* GetVerifyList()
	{
		return &verifyList;
	}


	void AddToVerifyList(FIDPMessage* msg);


	// HandleIncomingMessage places the message into the incomingHoldBuffer.
	// if its new and returns TRUE.  If the message is old, it returns FALSE.
	BOOL HandleIncomingMessage(FIDPMessage* message_info, int32_t send_count);

	void SetNextIncomingSendCount(void);

	FIDPMessage* RemoveFromVerifyList(int32_t send_count);

	BOOL IsVerifyListFull(void);

	int32_t VerifyCountDifference(void);

	FIDPMessage* NextMessageToProcess(void);


	BOOL IsInGroup(DPID group_id);



};

#endif // !defined(DPPLAYER_H)
