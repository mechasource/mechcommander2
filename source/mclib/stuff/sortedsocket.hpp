//===========================================================================//
// File:	srtskt.hh                                                        //
// Contents: Interface definition for sorted socket class                    //
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#pragma once

#include <stuff/safesocket.hpp>

namespace Stuff {

	//~~~~~~~~~~~~~~~~~~~~~~~~~~~ SortedSocket ~~~~~~~~~~~~~~~~~~~~~~~~~~~

	class SortedSocket:
		public SafeSocket
	{
	public:
		~SortedSocket(void);

		void
			AddValuePlug(
				Plug *plug,
				PCVOID value
			)
         	{AddValueImplementation(plug, value);}

		Plug*
			FindPlug(PCVOID value)
				{return FindImplementation(value);}

	protected:
		explicit SortedSocket(
			bool has_unique_entries = true
		);
		SortedSocket(
			Node *node,
			bool has_unique_entries = true
		);

		bool
			HasUniqueEntries()
				{return hasUniqueEntries;}

		virtual void
			AddValueImplementation(
				Plug *plug,
				PCVOID value
			);

		virtual Plug*
			FindImplementation(PCVOID value);

	private:
		bool
      	hasUniqueEntries;
	};

	//~~~~~~~~~~~~~~~~~~~~~~~~~~~ SortedIterator ~~~~~~~~~~~~~~~~~~~~~~~~~~~

	class SortedIterator:
		public SafeIterator
	{
	public:
		~SortedIterator(void);

		virtual Plug*
			FindImplementation(PCVOID value);

		virtual PVOID
			GetValueImplementation(void);

	protected:
		explicit SortedIterator(SortedSocket *sortedSocket);
	};

}
