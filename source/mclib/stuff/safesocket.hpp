//===========================================================================//
// File:	sfeskt.hh                                                        //
// Contents: Interface definition for safe socket class                      //
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#pragma once

#ifndef _SAFESOCKET_HPP_
#define _SAFESOCKET_HPP_

#include <stuff/socket.hpp>

namespace Stuff {
	
	class SafeIterator;
	
	typedef int32_t IteratorMemo;

	enum
	{
		PlugAdded = 0,
		PlugRemoved,
		NextSafeSocketMemo
	};

	//~~~~~~~~~~~~~~~~~~~~~~~~~~~ SafeSocket ~~~~~~~~~~~~~~~~~~~~~~~~~~~

	class SafeSocket:
		public Socket
	{
		friend class SafeIterator;

	public:
		~SafeSocket();
		void
			TestInstance();

	protected:
		explicit SafeSocket(Node *node);

		void
			SendIteratorMemo(
				IteratorMemo memo,
				PVOID content
			);

	private:
		SafeIterator *iteratorHead;
	};

	//~~~~~~~~~~~~~~~~~~~~~~~~~~~ SafeIterator ~~~~~~~~~~~~~~~~~~~~~~~~~~~

	class SafeIterator:
		public SocketIterator
	{
		friend class SafeSocket;

	public:
		~SafeIterator();
		void TestInstance(void) const;

	protected:
		explicit SafeIterator(SafeSocket *safeSocket);

	private:
		virtual void
			ReceiveMemo(
				IteratorMemo memo,
				PVOID content
			);

		SafeIterator *nextIterator;
		SafeIterator *prevIterator;
	};

}
#endif
