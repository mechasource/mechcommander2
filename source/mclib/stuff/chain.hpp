//===========================================================================//
// File: chain.hh //
// Contents: Interface specification of Chains and their iterators //
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved. //
//===========================================================================//

#pragma once

#ifndef _CHAIN_HPP_
#define _CHAIN_HPP_

#include <stuff/link.hpp>
#include <stuff/memoryblock.hpp>
#include <stuff/socket.hpp>

namespace Stuff{

	class Chain;
	class ChainIterator;

	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ ChainLink ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	enum{
		ChainLink_MemoryBlock_Allocation = 1000
	};

	class ChainLink: public Link
	{
	public:
		friend class Chain;
		friend class ChainIterator;

		static void
			InitializeClass(
			size_t block_count = ChainLink_MemoryBlock_Allocation,
			size_t block_delta = ChainLink_MemoryBlock_Allocation
			);
		static void __stdcall TerminateClass(void);

	public:
		~ChainLink(void);
		void
			TestInstance()
		{}

	private:
		ChainLink(
			Chain *chain,
			Plug *plug,
			ChainLink *nextChainLink,
			ChainLink *prevChainLink
			);

		ChainLink
			*nextChainLink,
			*prevChainLink;

	private:
		static MemoryBlock* AllocatedMemory;
		PVOID operator new(size_t)
		{
			return AllocatedMemory->New();
		}
		void operator delete(PVOID where)
		{
			AllocatedMemory->Delete(where);
		}
	};

	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Chain ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	class Chain:
		public Socket
	{
		friend class ChainLink;
		friend class ChainIterator;

	public:
		//
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		// Public interface
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		//

		//
		//--------------------------------------------------------------------
		// Constructor, Destructor and testing
		//--------------------------------------------------------------------
		//
		explicit Chain(Node *node);
		~Chain(void);

		void
			TestInstance()
		{}
		static bool
			TestClass(void);
		static bool
			ProfileClass(void);

		//
		//-----------------------------------------------------------------------
		// IsEmpty - Returns true if the socket contains no plugs.
		//-----------------------------------------------------------------------
		//
		bool
			IsEmpty(void);

	protected:
		//
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		// Protected interface
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		//
		void
			AddImplementation(Plug *plug);

	private:
		//
		//--------------------------------------------------------------------
		// Private methods
		//--------------------------------------------------------------------
		//
		ChainLink*
			InsertChainLink(
			Plug *plug,
			ChainLink *current_link
			);

		//
		//--------------------------------------------------------------------
		// Private data
		//--------------------------------------------------------------------
		//
		ChainLink
			*head,
			*tail;
	};

	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ ChainOf ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	template <class T> class ChainOf:
	public Chain
	{
	public:
		//
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		// Public interface
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		//
		explicit ChainOf(Node *node);
		~ChainOf(void);

		//
		//--------------------------------------------------------------------
		// Socket methods (see Socket for full listing)
		//--------------------------------------------------------------------
		//
		void
			Add(T plug)
		{AddImplementation(Cast_Pointer(Plug*, plug));}
		void
			Remove(T plug)
		{RemovePlug(Cast_Pointer(Plug*, plug));}
	};

	//~~~~~~~~~~~~~~~~~~~~~~~~~~~ ChainOf templates ~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	template <class T>
	ChainOf<T>::ChainOf(Node* node):
	Chain(node)
	{
	}

	template <class T>
	ChainOf<T>::~ChainOf()
	{
	}

	//~~~~~~~~~~~~~~~~~~~~~~~~~~~ ChainIterator ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	class ChainIterator:
		public SocketIterator
	{
	public:
		//
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		// Public interface
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		//

		//
		//--------------------------------------------------------------------
		// Constructors, Destructor and testing
		//--------------------------------------------------------------------
		//
		explicit ChainIterator(Chain *chain) 
			: SocketIterator(chain)
		{
			Check_Object(chain); currentLink = chain->head;
		}
		ChainIterator(const ChainIterator &iterator)
			: SocketIterator(iterator.socket)
		{
			Check_Object(&iterator); currentLink = iterator.currentLink;
		}
		~ChainIterator() {}

		void TestInstance(void) const {}

		//
		//--------------------------------------------------------------------
		// Iterator methods (see Iterator for full listing)
		//--------------------------------------------------------------------
		//
		void First(void);
		void Last(void);
		void Next(void);
		void Previous(void);
		CollectionSize GetSize(void);
		void Remove(void);

	protected:
		//
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		// Protected interface
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		//
		PVOID ReadAndNextImplementation(void);
		PVOID ReadAndPreviousImplementation(void);
		PVOID GetCurrentImplementation(void);
		PVOID GetNthImplementation(CollectionSize index);
		void InsertImplementation(Plug*);

		//
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		// Protected data
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		//
		ChainLink* currentLink;
	};

	inline PVOID ChainIterator::ReadAndNextImplementation()
	{
		if (currentLink != nullptr)
		{
			Check_Object(currentLink);
			Plug *plug = currentLink->plug;
			currentLink = currentLink->nextChainLink;
			return plug;
		}
		return nullptr;
	}

	//~~~~~~~~~~~~~~~~~~~~~~~~~~ ChainIteratorOf ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	template <class T> class ChainIteratorOf:
	public ChainIterator
	{
	public:
		//
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		// Public interface
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		//

		//
		//--------------------------------------------------------------------
		// Constructors and Destructor
		//--------------------------------------------------------------------
		//
		ChainIteratorOf(ChainOf<T> *chain);
		ChainIteratorOf(const ChainIteratorOf<T> &iterator);
		Iterator* MakeClone(void);
		~ChainIteratorOf(void);

		//
		//--------------------------------------------------------------------
		// Iterator methods (see Iterator for full listing)
		//--------------------------------------------------------------------
		//
		T ReadAndNext(void) {return (T)ReadAndNextImplementation();}
		T ReadAndPrevious(void) {return (T)ReadAndPreviousImplementation();}
		T GetCurrent(void) {return (T)GetCurrentImplementation();}
		T GetNth(CollectionSize index) {return (T)GetNthImplementation(index);}
		void Insert(T plug){InsertImplementation(Cast_Object(Plug*,plug));}

		ChainIteratorOf<T>& Begin(void) {return (ChainIteratorOf<T>&)BeginImplementation();}
		ChainIteratorOf<T>& End(void) {return (ChainIteratorOf<T>&)EndImplementation();}
		ChainIteratorOf<T>& Forward(void) {return (ChainIteratorOf<T>&)ForwardImplementation();}
		ChainIteratorOf<T>& Backward(void) {return (ChainIteratorOf<T>&)BackwardImplementation();}
	};

	//~~~~~~~~~~~~~~~~~~~~~~~ ChainIteratorOf templates ~~~~~~~~~~~~~~~~~~~~~~~~

	template <class T>
	ChainIteratorOf<T>::ChainIteratorOf(ChainOf<T> *chain)
		: ChainIterator(chain)
	{
	}

	template <class T>
	ChainIteratorOf<T>::ChainIteratorOf(const ChainIteratorOf<T> &iterator)
		: ChainIterator(iterator)
	{
	}

	template <class T> Iterator* ChainIteratorOf<T>::MakeClone()
	{
		return new ChainIteratorOf<T>(*this);
	}

	template <class T>
	ChainIteratorOf<T>::~ChainIteratorOf()
	{
	}

}

#endif
