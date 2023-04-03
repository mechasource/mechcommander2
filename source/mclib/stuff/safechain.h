//===========================================================================//
// File:	schain.hh                                                        //
// Contents: Interface definition for safe chains                            //
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#pragma once

#ifndef _SAFECHAIN_HPP_
#define _SAFECHAIN_HPP_

#include "stuff/link.h"
#include "stuff/memoryblock.h"
#include "stuff/safesocket.h"

namespace Stuff
{

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ SafeChainLink ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

enum
{
	SafeChainLink_MemoryBlock_Allocation = 100
};

class SafeChainLink : public Link
{
public:
	friend class SafeChain;
	friend class SafeChainIterator;

	static void InitializeClass(size_t block_count = SafeChainLink_MemoryBlock_Allocation,
		size_t block_delta = SafeChainLink_MemoryBlock_Allocation);
	static void __stdcall TerminateClass(void);

public:
	~SafeChainLink(void);
	void TestInstance(void);

private:
	SafeChainLink(SafeChain* chain, Plug* plug, SafeChainLink* nextSafeChainLink,
		SafeChainLink* prevSafeChainLink);

	SafeChainLink* nextSafeChainLink;
	SafeChainLink* prevSafeChainLink;

private:
	// static MemoryBlock* AllocatedMemory;

	PVOID operator new(size_t)
	{
		return AllocatedMemory->New();
	}
	void operator delete(PVOID where)
	{
		AllocatedMemory->Delete(where);
	}
};

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ SafeChain ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

class SafeChain : public SafeSocket
{
	friend class SafeChainLink;
	friend class SafeChainIterator;

public:
	//
	//-----------------------------------------------------------------------
	//-----------------------------------------------------------------------
	// Public interface
	//-----------------------------------------------------------------------
	//-----------------------------------------------------------------------
	//
	explicit SafeChain(Node* node);
	~SafeChain(void);

	void TestInstance(void);
	static void TestClass(void);
	static void ProfileClass(void);

	//
	//-----------------------------------------------------------------------
	// IsEmpty - Returns true if the socket contains no plugs.
	//-----------------------------------------------------------------------
	//
	bool IsEmpty(void);

protected:
	//
	//-----------------------------------------------------------------------
	//-----------------------------------------------------------------------
	// Protected interface
	//-----------------------------------------------------------------------
	//-----------------------------------------------------------------------
	//
	void AddImplementation(Plug* plug);

private:
	//
	//-----------------------------------------------------------------------
	// Private utilities
	//-----------------------------------------------------------------------
	//
	SafeChainLink* InsertSafeChainLink(Plug* plug, SafeChainLink* link);

	//
	//-----------------------------------------------------------------------
	// Private data
	//-----------------------------------------------------------------------
	//
	SafeChainLink* head;
	SafeChainLink* tail;
};

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ SafeChainOf ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

template <class T>
class SafeChainOf : public SafeChain
{
public:
	//
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	// Public interface
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	//
	explicit SafeChainOf(Node* node);
	~SafeChainOf(void);

	//
	//--------------------------------------------------------------------
	// Socket methods (see Socket for full listing)
	//--------------------------------------------------------------------
	//
	void Add(T plug)
	{
		AddImplementation(Cast_Pointer(Plug*, plug));
	}
	void Remove(T plug)
	{
		RemovePlug(Cast_Pointer(Plug*, plug));
	}
};

//~~~~~~~~~~~~~~~~~~~~~~~~~~~ SafeChainOf templates ~~~~~~~~~~~~~~~~~~~~~~~~~~~

template <class T>
SafeChainOf<T>::SafeChainOf(Node* node)
	: SafeChain(node)
{
}

template <class T>
SafeChainOf<T>::~SafeChainOf()
{
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ SafeChainIterator ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

class SafeChainIterator : public SafeIterator
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
	SafeChainIterator(SafeChain* chain, bool move_next_on_remove);
	SafeChainIterator(const SafeChainIterator& iterator);
	~SafeChainIterator(void);

	void TestInstance(void) const;

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
	PVOID
	ReadAndNextImplementation(void);
	PVOID
	ReadAndPreviousImplementation(void);
	PVOID
	GetCurrentImplementation(void);
	PVOID
	GetNthImplementation(CollectionSize index);
	void InsertImplementation(Plug* plug);

private:
	//
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	// Private interface
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	//
	void ReceiveMemo(IteratorMemo memo, PVOID content);

	//
	//--------------------------------------------------------------------
	// Private data
	//--------------------------------------------------------------------
	//
	SafeChainLink* currentLink;
	bool moveNextOnRemove;
};

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~ SafeChainIteratorOf ~~~~~~~~~~~~~~~~~~~~~~~~~~~~

template <class T>
class SafeChainIteratorOf : public SafeChainIterator
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
	SafeChainIteratorOf(SafeChainOf<T>* chain, bool move_next_on_remove = true);
	SafeChainIteratorOf(const SafeChainIteratorOf<T>& iterator);
	Iterator* MakeClone(void);

	~SafeChainIteratorOf(void);

	//
	//--------------------------------------------------------------------
	// Iterator methods (see Iterator for full listing)
	//--------------------------------------------------------------------
	//
	T ReadAndNext()
	{
		return (T)ReadAndNextImplementation(void);
	}
	T ReadAndPrevious()
	{
		return (T)ReadAndPreviousImplementation(void);
	}
	T GetCurrent()
	{
		return (T)GetCurrentImplementation(void);
	}
	T GetNth(CollectionSize index)
	{
		return (T)GetNthImplementation(index);
	}
	void Insert(T plug)
	{
		InsertImplementation(Cast_Object(Plug*, plug));
	}
};

//~~~~~~~~~~~~~~~~~~~~~~~ SafeChainIteratorOf templates ~~~~~~~~~~~~~~~~~~~~~~~

template <class T>
SafeChainIteratorOf<T>::SafeChainIteratorOf(SafeChainOf<T>* chain, bool move_next_on_remove)
	: SafeChainIterator(chain, move_next_on_remove)
{
}

template <class T>
SafeChainIteratorOf<T>::SafeChainIteratorOf(const SafeChainIteratorOf<T>& iterator)
	: SafeChainIterator(iterator)
{
}

template <class T>
Iterator*
SafeChainIteratorOf<T>::MakeClone()
{
	return new SafeChainIteratorOf<T>(*this);
}

template <class T>
SafeChainIteratorOf<T>::~SafeChainIteratorOf()
{
}
} // namespace Stuff

#endif
