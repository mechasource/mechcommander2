//===========================================================================//
// File:	vchain.cc                                                        //
// Contents: Implementation details of SortedChain class                     //
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#include "stdinc.h"
//#include "stuffheaders.hpp"

#include <gameos.hpp>
#include <stuff/node.hpp>
#include <stuff/sortedchain.hpp>

using namespace Stuff;

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ SortedChainLink
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

//
//###########################################################################
// SortedChainLink
//###########################################################################
//
SortedChainLink::SortedChainLink(SortedChain* vchain, Plug* plug)
	: Link(vchain, plug)
{
	next = nullptr;
	prev = nullptr;
}

//
//###########################################################################
// ~SortedChainLink
//###########################################################################
//
SortedChainLink::~SortedChainLink(void)
{
	// Check_Object(this);
	SortedChain* vchain = Cast_Object(SortedChain*, socket);
	//
	//-------------------------------------------
	// Notify iterators that deletion is occuring
	//-------------------------------------------
	//
	vchain->SendIteratorMemo(PlugRemoved, this);
	//
	//---------------------------
	// Remove from existing links
	//---------------------------
	//
	if (prev != nullptr)
	{
		Check_Object(prev);
		prev->next = next;
	}
	else
	{
		Check_Object(vchain);
		vchain->head = next;
	}
	if (next != nullptr)
	{
		Check_Object(next);
		next->prev = prev;
	}
	else
	{
		Check_Object(vchain);
		vchain->tail = prev;
	}
	prev = next = nullptr;
	//
	//------------------------------------------
	// Remove this link from any plug references
	//------------------------------------------
	//
	ReleaseFromPlug();
	//
	//-------------------------------------------------------------
	// Tell the node to release this link.  Note that this link
	// is not referenced by the plug or the chain at this point in
	// time.
	//-------------------------------------------------------------
	//
	if (vchain->GetReleaseNode() != nullptr)
	{
		Check_Object(vchain->GetReleaseNode());
		vchain->GetReleaseNode()->ReleaseLinkHandler(vchain, plug);
	}
}

//
//###########################################################################
// TestInstance
//###########################################################################
//
void SortedChainLink::TestInstance(void)
{
	Link::TestInstance();
	if (next != nullptr)
	{
		Check_Signature(next);
	}
	if (prev != nullptr)
	{
		Check_Signature(prev);
	}
}

//
//###########################################################################
// SetupSortedChainLinks
//###########################################################################
//
void SortedChainLink::SetupSortedChainLinks(
	SortedChainLink* next, SortedChainLink* prev)
{
	// Check_Object(this);
	this->next = next;
	this->prev = prev;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ SortedChain
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

//
//###########################################################################
// SortedChain
//###########################################################################
//
SortedChain::SortedChain(Node* node, bool has_unique_entries)
	: SortedSocket(node, has_unique_entries)
{
	head = nullptr;
	tail = nullptr;
}

//
//###########################################################################
// ~SortedChain
//###########################################################################
//
SortedChain::~SortedChain(void)
{
	// Check_Object(this);
	SetReleaseNode(nullptr);
	SortedChainLink* link = head;
	while (link)
	{
		Check_Object(link);
		SortedChainLink* next = link->next;
		Unregister_Object(link);
		delete link;
		link = next;
	}
}

//
//###########################################################################
// TestInstance
//###########################################################################
//
void SortedChain::TestInstance(void)
{
	SortedSocket::TestInstance();
	if (head != nullptr)
	{
		Check_Object(head);
	}
	if (tail != nullptr)
	{
		Check_Object(tail);
	}
}

//
//###########################################################################
// MakeSortedChainLink
//###########################################################################
//
SortedChainLink* SortedChain::MakeSortedChainLink(Plug*, PCVOID)
{
	// Check_Object(this);
	STOP(("SortedChain::MakeSortedChainLink - Should never reach here"));
	return nullptr;
}

//
//###########################################################################
// CompareSortedChainLinks
//###########################################################################
//
int32_t SortedChain::CompareSortedChainLinks(SortedChainLink*, SortedChainLink*)
{
	// Check_Object(this);
	STOP(("SortedChain::CompareSortedChainLinks - Should never reach here"));
	return 0;
}

//
//###########################################################################
// CompareValueToSortedChainLink
//###########################################################################
//
int32_t SortedChain::CompareValueToSortedChainLink(PCVOID, SortedChainLink*)
{
	// Check_Object(this);
	STOP(("SortedChain::CompareValueToSortedChainLink - Should never reach "
		  "here"));
	return 0;
}

//
//###########################################################################
// AddImplementation
//###########################################################################
//
void SortedChain::AddImplementation(Plug* plug)
{
	// Check_Object(this);
	(void)plug;
	STOP(("Must use AddValue call"));
	// AddValueImplementation(plug, nullptr);
}

//
//###########################################################################
// AddValueImplementation
//###########################################################################
//
void SortedChain::AddValueImplementation(Plug* plug, PCVOID value)
{
	// Check_Object(this);
	SortedChainLink* link;
	//
	//-------------------------------------------------------------
	// Verify that value has not been added
	//-------------------------------------------------------------
	//
	Verify(
		HasUniqueEntries() ? (SearchForValue(value) == nullptr) : (bool)true);
	//
	//-------------------------------------------------------------
	// Make new vchain link
	//-------------------------------------------------------------
	//
	link = MakeSortedChainLink(plug, value);
	Register_Object(link);
	//
	//-------------------------------------------------------------
	// Find insertion point for the new link
	//-------------------------------------------------------------
	//
	if (head == nullptr)
	{
		link->SetupSortedChainLinks(nullptr, nullptr);
		head = link;
		tail = link;
	}
	else
	{
		SortedChainLink* greater_link;
		Check_Object(head);
		Check_Object(tail);
		for (greater_link = head; greater_link != nullptr;
			 greater_link = greater_link->next)
		{
			Check_Object(greater_link);
			if (CompareValueToSortedChainLink(value, greater_link) < 0)
				break;
		}
		if (greater_link == nullptr)
		{
			//
			// Add after tail
			//
			link->SetupSortedChainLinks(nullptr, tail);
			tail->next = link;
			tail	   = link;
		}
		else if (greater_link == head)
		{
			//
			// Add before head
			//
			link->SetupSortedChainLinks(head, nullptr);
			head->prev = link;
			head	   = link;
		}
		else
		{
			//
			// Add before greater_link
			//
			link->SetupSortedChainLinks(greater_link, greater_link->prev);
			greater_link->prev->next = link;
			greater_link->prev		 = link;
		}
	}
	SendIteratorMemo(PlugAdded, link);
}

//
//###########################################################################
// FindImplementation
//###########################################################################
//
Plug* SortedChain::FindImplementation(PCVOID value)
{
	// Check_Object(this);
	SortedChainLink* link;
	if ((link = SearchForValue(value)) != nullptr)
	{
		Check_Object(link);
		return link->GetPlug();
	}
	return nullptr;
}

//
//#############################################################################
// IsEmpty
//#############################################################################
//
bool SortedChain::IsEmpty(void)
{
	// Check_Object(this);
	return (head == nullptr);
}

//
//###########################################################################
// SearchForValue
//###########################################################################
//
SortedChainLink* SortedChain::SearchForValue(PCVOID value)
{
	// Check_Object(this);
	SortedChainLink* link;
	int32_t ret;
	for (link = head; link != nullptr; link = link->next)
	{
		Check_Object(link);
		if ((ret = CompareValueToSortedChainLink(value, link)) == 0)
			break;
		if (ret < 0)
			return (nullptr);
	}
	return link;
}

//
//###########################################################################
// SortedChainIterator
//###########################################################################
//
SortedChainIterator::SortedChainIterator(SortedChain* vchain)
	: SortedIterator(vchain)
{
	Check_Object(vchain);
	currentLink = vchain->head;
}

//
//###########################################################################
// SortedChainIterator
//###########################################################################
//
SortedChainIterator::SortedChainIterator(const SortedChainIterator* iterator)
	: SortedIterator(Cast_Object(SortedChain*, iterator->socket))
{
	Check_Object(iterator);
	currentLink = iterator->currentLink;
}

Iterator* SortedChainIterator::MakeClone(void)
{
	// Check_Object(this);
	return new SortedChainIterator(*this);
}

//
//###########################################################################
//###########################################################################
//
SortedChainIterator::~SortedChainIterator(void)
{
	// Check_Object(this);
}

//
//###########################################################################
// TestInstance
//###########################################################################
//
void SortedChainIterator::TestInstance(void) const
{
	SortedIterator::TestInstance();
	if (currentLink != nullptr)
	{
		Check_Object(currentLink);
	}
}

//
//###########################################################################
// First
//###########################################################################
//
void SortedChainIterator::First(void)
{
	// Check_Object(this);
	currentLink = Cast_Object(SortedChain*, socket)->head;
}

//
//###########################################################################
// Last
//###########################################################################
//
void SortedChainIterator::Last(void)
{
	// Check_Object(this);
	currentLink = Cast_Object(SortedChain*, socket)->tail;
}

//
//###########################################################################
// Next
//###########################################################################
//
void SortedChainIterator::Next(void)
{
	// Check_Object(this);
	Check_Object(currentLink);
	currentLink = currentLink->next;
}

//
//###########################################################################
// Previous
//###########################################################################
//
void SortedChainIterator::Previous(void)
{
	// Check_Object(this);
	Check_Object(currentLink);
	currentLink = currentLink->prev;
}

//
//###########################################################################
// ReadAndNextImplementation
//###########################################################################
//
PVOID SortedChainIterator::ReadAndNextImplementation(void)
{
	// Check_Object(this);
	if (currentLink != nullptr)
	{
		Plug* plug;
		Check_Object(currentLink);
		plug		= currentLink->GetPlug();
		currentLink = currentLink->next;
		return plug;
	}
	return nullptr;
}

//
//###########################################################################
// ReadAndPreviousImplementation
//###########################################################################
//
PVOID SortedChainIterator::ReadAndPreviousImplementation(void)
{
	// Check_Object(this);
	if (currentLink != nullptr)
	{
		Plug* plug;
		Check_Object(currentLink);
		plug		= currentLink->plug;
		currentLink = currentLink->prev;
		return plug;
	}
	return nullptr;
}

//
//###########################################################################
// GetCurrentImplementation
//###########################################################################
//
PVOID SortedChainIterator::GetCurrentImplementation(void)
{
	// Check_Object(this);
	if (currentLink != nullptr)
	{
		Check_Object(currentLink);
		return currentLink->GetPlug();
	}
	return nullptr;
}

//
//###########################################################################
// GetSize
//###########################################################################
//
CollectionSize SortedChainIterator::GetSize(void)
{
	// Check_Object(this);
	SortedChainLink* link;
	CollectionSize count;
	count = 0;
	for (link = Cast_Object(SortedChain*, socket)->head; link != nullptr;
		 link = link->next)
	{
		Check_Object(link);
		count++;
	}
	return count;
}

//
//###########################################################################
// GetNthImplementation
//###########################################################################
//
PVOID SortedChainIterator::GetNthImplementation(CollectionSize index)
{
	// Check_Object(this);
	SortedChainLink* link;
	CollectionSize count;
	count = 0;
	for (link = Cast_Object(SortedChain*, socket)->head; link != nullptr;
		 link = link->next)
	{
		Check_Object(link);
		if (count == index)
		{
			currentLink = link;
			return currentLink->GetPlug();
		}
		count++;
	}
	return nullptr;
}

//
//###########################################################################
// Remove
//###########################################################################
//
void SortedChainIterator::Remove(void)
{
	// Check_Object(this);
	Check_Object(currentLink);
	Unregister_Object(currentLink);
	delete currentLink;
}

//
//###########################################################################
// FindImplementation
//###########################################################################
//
Plug* SortedChainIterator::FindImplementation(PCVOID value)
{
	// Check_Object(this);
	SortedChainLink* link;
	if ((link = Cast_Object(SortedChain*, socket)->SearchForValue(value)) !=
		nullptr)
	{
		Check_Object(link);
		return (currentLink = link)->GetPlug();
	}
	return nullptr;
}

//
//###########################################################################
// ReceiveMemo
//###########################################################################
//
void SortedChainIterator::ReceiveMemo(IteratorMemo memo, PVOID content)
{
	// Check_Object(this);
	if (memo == PlugRemoved)
	{
		if (content == currentLink)
		{
			Next();
		}
	}
}
