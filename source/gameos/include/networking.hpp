#pragma once
//===========================================================================//
// File:	 Networking.hpp													 //
// Contents: API for multiplayer games										 //
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#define MAXMESSAGESIZE 512
#define MAXPLAYERS 8

//////////////////////////////////////////////////////////////////////////////////
// ***************************	  NETWORK API	 ******************************
// //
//////////////////////////////////////////////////////////////////////////////////

class NetworkMessageContainer
{
public:
	// If receiving a message {receiverId} is accessed to see if the message was
	// sent to a group in which the player belongs, to all players, or directly
	// to this player.
	//
	// When sending a message, {receiverId} is the GOSNETWORKID of the intended
	// recipient. If this message is for all players, the {receiverId} should be
	// GOSALLPLAYERS.  If this message is for the server, {server} should be
	// true and {receiverId} will be ignored.
	uint32_t receiverId;

	// {senderId} is the GOSNETWORKID of the sender if this message is being
	// received, When sending a message, {senderId} is ignored.
	uint32_t senderId;

	// {type} is the messsage type.  This value should be between
	// GOSFIRSTAPPMESSAGETYPE and GOSLASTAPPMESSAGETYPE.  If not, the send will
	// result in an error.
	uint32_t type;

	// {data} is the information sent across the network.
	BOOLEAN* buffer;

	// Number of bytes to be sent or number of bytes received.
	uint32_t messageSize;

	// Sending: {guaranteed} is false by default.  If true, this message is
	// sent guaranteed.  Use this flag sparingly.
	//
	// Receiving: tells us if the message arrived guaranteed.
	BOOLEAN guaranteed;

	// Sending: {server} is false by default.
	// If {server} is true, the message goes to the server only.
	// {receiverID} is not used if {server} is true!
	//
	// Receiving: Not used.
	BOOLEAN server;

	// Sending: Not used.
	//
	// Receiving: User callback should look at {system} and interpret this as a
	// system message if {system} is true.  Otherwise, this is an application
	// message, and the callback should look at {type} for the specific type.
	BOOLEAN system;

	// Priority of zero is the default.  Higher number means a higher priority
	// message.
	uint32_t priority;

	NetworkMessageContainer(void);

	void Clear(void);
};

//
//
//
typedef struct _Messages
{
	_Messages* pNext;

	NetPacket Data;

} Messages;
