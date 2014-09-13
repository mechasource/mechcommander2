//===========================================================================//
// File:	sfeskt.cc                                                        //
// Contents: Implementation details for safe socket class                    //
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#include "stdafx.h"
//#include "stuffheaders.hpp"

#include <gameos.hpp>
#include <stuff/safesocket.hpp>

using namespace Stuff;

//
//###########################################################################
// SafeSocket
//###########################################################################
//
SafeSocket::SafeSocket(Node* node):
	Socket(node)
{
	iteratorHead = nullptr;
}

//
//###########################################################################
// ~SafeSocket
//###########################################################################
//
SafeSocket::~SafeSocket()
{
	Check_Object(this);
	Verify(iteratorHead == nullptr);
}

//
//###########################################################################
// TestInstance
//###########################################################################
//
void
SafeSocket::TestInstance()
{
	Socket::TestInstance();
	if(iteratorHead != nullptr)
	{
		Check_Signature(iteratorHead);
	}
}

//
//###########################################################################
// SendIteratorMemo
//###########################################################################
//
void
SafeSocket::SendIteratorMemo(
	IteratorMemo memo,
	PVOID content
)
{
	Check_Object(this);
	SafeIterator* iterator;
	for(
		iterator = iteratorHead;
		iterator != nullptr;
		iterator = iterator->nextIterator
	)
	{
		Check_Object(iterator);
		iterator->ReceiveMemo(memo, content);
	}
}

//
//###########################################################################
// SafeIterator
//###########################################################################
//
SafeIterator::SafeIterator(SafeSocket* safeSocket):
	SocketIterator(safeSocket)
{
	//
	// Link iterator into sockets set of iterators
	//
	Check_Object(safeSocket);
	if((nextIterator = safeSocket->iteratorHead) != nullptr)
	{
		Check_Object(nextIterator);
		nextIterator->prevIterator = this;
	}
	prevIterator = nullptr;
	safeSocket->iteratorHead = this;
}

//
//###########################################################################
// ~SafeIterator
//###########################################################################
//
SafeIterator::~SafeIterator()
{
	Check_Object(this);
	SafeSocket* safeSocket = Cast_Object(SafeSocket*, socket);
	//
	// Remove iterator from sockets set of iterators
	//
	Check_Object(safeSocket);
	if(safeSocket->iteratorHead == this)
	{
		safeSocket->iteratorHead = nextIterator;
	}
	if(prevIterator != nullptr)
	{
		Check_Object(prevIterator);
		prevIterator->nextIterator = nextIterator;
	}
	if(nextIterator != nullptr)
	{
		Check_Object(nextIterator);
		nextIterator->prevIterator = prevIterator;
	}
}

//
//###########################################################################
// TestInstance
//###########################################################################
//
void
SafeIterator::TestInstance(void) const
{
	SocketIterator::TestInstance();
	if(prevIterator != nullptr)
	{
		Check_Signature(prevIterator);
	}
	if(nextIterator != nullptr)
	{
		Check_Signature(nextIterator);
	}
}

//
//###########################################################################
// ReceiveMemo
//###########################################################################
//
void
SafeIterator::ReceiveMemo(
	IteratorMemo,
	PVOID
)
{
	Check_Object(this);
	STOP(("SafeIterator::ReceiveMemo - Should never reach here"));
}

