//===========================================================================//
// File:	slot.hh                                                          //
// Contents: Interface specification for slot class                          //
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#pragma once

#ifndef _SLOT_HPP_
#define _SLOT_HPP_

#include "stuff/link.h"
#include "stuff/memoryblock.h"
#include "stuff/socket.h"

namespace Stuff
{

class Slot;
class PLug;

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ SlotLink ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

enum
{
	SlotLink_MemoryBlock_Allocation = 100
};

class SlotLink : public Link
{
	friend class Slot;

public:
	static void InitializeClass(size_t block_count = SlotLink_MemoryBlock_Allocation,
		size_t block_delta = SlotLink_MemoryBlock_Allocation);
	static void __stdcall TerminateClass(void);

public:
	~SlotLink(void);

private:
	SlotLink(Slot* slot, Plug* plug);

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

//~~~~~~~~~~~~~~~~~~~~~~~~~~~ Slot ~~~~~~~~~~~~~~~~~~~~~~~~~~~

class Slot : public Socket
{
	friend class SlotLink;

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
	explicit Slot(Node* node);
	~Slot(void);

	void TestInstance(void);

	//
	//--------------------------------------------------------------------
	// Remove	- Remove the link
	//--------------------------------------------------------------------
	//
	void Remove(void);

protected:
	//
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	// Protected interface
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	//
	void AddImplementation(Plug* plug);
	Plug* GetCurrentPlug(void);

private:
	//
	//--------------------------------------------------------------------
	// Private data
	//--------------------------------------------------------------------
	//
	SlotLink* slotLink;
};

//~~~~~~~~~~~~~~~~~~~~~~~~~~~ SlotOf ~~~~~~~~~~~~~~~~~~~~~~~~~~~

template <class T>
class SlotOf : public Slot
{
public:
	//
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	// Public interface
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	//
	explicit SlotOf(Node* node);
	~SlotOf(void);

	//
	//--------------------------------------------------------------------
	// Socket methods (see Socket for full listing)
	//--------------------------------------------------------------------
	//
	void Add(T plug)
	{
		AddImplementation(Cast_Object(Plug*, plug));
	}

	T GetCurrent()
	{
		return (T)GetCurrentPlug(void);
	}
};

//~~~~~~~~~~~~~~~~~~~~~~~~~~~ SlotOf templates ~~~~~~~~~~~~~~~~~~~~~~~~~~~

template <class T>
SlotOf<T>::SlotOf(Node* node)
	: Slot(node)
{
}

template <class T>
SlotOf<T>::~SlotOf()
{
}
} // namespace Stuff
#endif
