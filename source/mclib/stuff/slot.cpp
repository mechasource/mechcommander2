//===========================================================================//
// File:	slot.cc                                                          //
// Contents: Implementation details for the slot class                       //
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#include "stdafx.h"
//#include "stuffheaders.hpp"

#include <gameos.hpp>
#include <stuff/node.hpp>
#include <stuff/slot.hpp>

using namespace Stuff;


MemoryBlock* SlotLink::AllocatedMemory = nullptr;

//
//#############################################################################
//#############################################################################
//
void
	SlotLink::InitializeClass(
		size_t block_count,
		size_t block_delta
	)
{
	Verify(!AllocatedMemory);
	AllocatedMemory =
		new MemoryBlock(
			sizeof(SlotLink),
			block_count,
			block_delta,
			"Stuff::SlotLink",
			ConnectionEngineHeap
		);
}

//
//#############################################################################
//#############################################################################
//
void
	SlotLink::TerminateClass()
{
	delete AllocatedMemory;
	AllocatedMemory = nullptr;
}

//
//###########################################################################
// SlotLink
//###########################################################################
//
SlotLink::SlotLink(
	Slot *slot,
	Plug *plug
):
	Link(slot, plug)
{
}

//
//###########################################################################
// ~SlotLink
//###########################################################################
//
SlotLink::~SlotLink()
{
	Check_Object(this);
	Slot *slot = Cast_Object(Slot*, socket);
	
	//
	//-------------------------------------------------
	// Make sure the link is not referenced by the slot
	//-------------------------------------------------
	//
	Check_Object(slot);
	Verify(slot->slotLink == this);
	slot->slotLink = nullptr;

	//
	//------------------------------------------
	// Remove this link from any plug references
	//------------------------------------------
	//
	ReleaseFromPlug();

	//
	//-------------------------------------------------------------
	// Tell the node to release this link.  Note that this link
	// is not referenced by the plug or the slot at this point in
	// time.
	//-------------------------------------------------------------
	//
	if (slot->GetReleaseNode() != nullptr)
	{
		Check_Object(slot->GetReleaseNode());
		slot->GetReleaseNode()->ReleaseLinkHandler(slot, plug);
	}
}

//
//###########################################################################
// Slot
//###########################################################################
//
Slot::Slot(
	Node *node
):
	Socket(node)
{
	slotLink = nullptr;
}

//
//###########################################################################
// ~Slot
//###########################################################################
//
Slot::~Slot()
{
	Check_Object(this);
	SetReleaseNode(nullptr);
	if (slotLink != nullptr)
	{
		Unregister_Object(slotLink);
		delete slotLink;
	}
}

//
//###########################################################################
// TestInstance
//###########################################################################
//
void
	Slot::TestInstance()
{
	Socket::TestInstance();
	if (slotLink != nullptr)
	{
		Check_Object(slotLink);
	}
}

//
//###########################################################################
// Remove
//###########################################################################
//
void
	Slot::Remove()
{
	Check_Object(this);
	if (slotLink != nullptr)
	{
		Unregister_Object(slotLink);
		delete slotLink;
		slotLink = nullptr;
	}
}

//
//###########################################################################
// AddImplementation
//###########################################################################
//
void
	Slot::AddImplementation(
		Plug *plug
	)
{
	Check_Object(this);
	Verify(slotLink == nullptr);
	slotLink = new SlotLink(this, plug);
	Register_Object(slotLink);
}

//
//###########################################################################
// GetCurrentPlug
//###########################################################################
//
Plug*
	Slot::GetCurrentPlug()
{
	Check_Object(this);
	if (slotLink != nullptr)
	{
		Check_Object(slotLink);
		return slotLink->GetPlug();
	}
	return nullptr;
}

