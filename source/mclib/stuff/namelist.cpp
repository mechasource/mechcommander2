//===========================================================================//
// File:     namelist.cpp                                                    //
// Title:    Definition of NameList classes.                                 //
// Purpose:  Maintains an unsorted list of strings with (PVOID) to          //
//           anything the client needs to associate with the string.         //
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#include "stdinc.h"
#include <stuff/namelist.hpp>

using namespace Stuff;

//#############################################################################
//##############    ObjectNameList    #########################################
//#############################################################################

ObjectNameList::ObjectNameList(void)
{
	// Check_Pointer(this);
	firstEntry = lastEntry = nullptr;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
ObjectNameList::~ObjectNameList(void)
{
	// Check_Object(this);
	Entry* next;
	while (firstEntry)
	{
		Check_Pointer(firstEntry);
		next = firstEntry->nextEntry;
		Unregister_Pointer(firstEntry);
		delete firstEntry;
		firstEntry = next;
	}
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
PCSTR ObjectNameList::AddEntry(PCSTR name, PVOID data)
{
	// Check_Object(this);
	Check_Pointer(name);
	Entry* entry;
	entry = Cast_Pointer(Entry*, new char[sizeof(Entry) + strlen(name) + 1]);
	Register_Object(entry);
	entry->dataReference = data;
	entry->nextEntry	 = nullptr;
	if (firstEntry)
	{
		Check_Pointer(lastEntry);
		lastEntry->nextEntry = entry;
	}
	else
	{
		firstEntry = entry;
	}
	lastEntry = entry;
	entry->SetName(name);
	return entry->GetName();
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
PVOID ObjectNameList::FindObject(PCSTR name)
{
	// Check_Object(this);
	Check_Pointer(name);
	Entry* entry;
	for (entry = firstEntry; entry; entry = entry->nextEntry)
	{
		Check_Pointer(entry);
		if (!strcmp(entry->GetName(), name))
		{
			break;
		}
	}
	return (entry) ? entry->dataReference : nullptr;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
void ObjectNameList::DeleteEntry(PCSTR name)
{
	// Check_Object(this);
	Check_Pointer(name);
	Entry *cur, *prev, *entry;
	//----------------------------------------------------
	// ***** DANGEROUS!!! see notice in namelist.hh *****
	//----------------------------------------------------
	entry = Cast_Pointer(Entry*, const_cast<PSTR>(name - sizeof(Entry)));
	prev  = nullptr;
	for (cur = firstEntry; cur && cur != entry; cur = cur->nextEntry)
	{
		Check_Pointer(cur);
		prev = cur;
	}
	_ASSERT(cur && cur == entry);
	if (!prev)
	{
		firstEntry = entry->nextEntry;
	}
	else
	{
		prev->nextEntry = entry->nextEntry;
	}
	if (entry == lastEntry)
	{
		lastEntry = prev;
	}
	Unregister_Pointer(entry);
	delete entry;
	return;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
size_t ObjectNameList::GetEntryCount(void) const
{
	// Check_Object(this);
	Entry* entry = firstEntry;
	size_t count = 0;
	while (entry)
	{
		Check_Pointer(entry);
		count++;
		entry = entry->nextEntry;
	}
	return count;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
int32_t ObjectNameList::BuildSubList(const ObjectNameList& source_list, PCSTR prefix)
{
	// Check_Object(this);
	Check_Object(&source_list);
	Check_Pointer(prefix);
	const size_t length = strlen(prefix);
	Entry* entry		= source_list.firstEntry;
	PCSTR name;
	int32_t count = 0;
	//------------------------------------------------------
	// add entries to SubList whose name begins with prefix
	//------------------------------------------------------
	while (entry)
	{
		Check_Pointer(entry);
		name = entry->GetName();
		Check_Pointer(name);
		if (!strncmp(name, prefix, length))
		{
			count++;
			AddEntry(name, entry->dataReference);
		}
		entry = entry->nextEntry;
	}
	//-------------------------------------------
	// return number of entries added to SubList
	//-------------------------------------------
	return count;
}

//#############################################################################
//##############    ObjectNameList::Entry    ##################################
//#############################################################################

void ObjectNameList::Entry::SetName(PCSTR name)
{
	// Check_Pointer(this);
	Check_Pointer(name);
	// suppress warning about unsafe strcpy as a strcpy_s implementation isn't
	// straightforward
	SUPPRESS_WARNING(4996)
	strcpy(Cast_Pointer(PSTR, this) + sizeof(ObjectNameList::Entry), name);
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
bool ObjectNameList::Entry::IsName(PCSTR name) const
{
	// Check_Object(this);
	// do not check name here
	if (name)
	{
		Check_Pointer(name);
		return strcmp(GetName(), name) == 0;
	}
	else
	{
		return GetName() == name;
	}
}

//#############################################################################
//##############    NameList    ###############################################
//#############################################################################

NameList::NameList(void) {}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
NameList::~NameList(void) {}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
PCSTR NameList::FindName(PVOID data)
{
	// Check_Object(this);
	Entry* entry;
	for (entry = firstEntry; entry; entry = entry->nextEntry)
	{
		Check_Pointer(entry);
		if (entry->dataReference == data)
		{
			break;
		}
	}
	return (entry) ? entry->GetName() : nullptr;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
NameList::Entry* NameList::FindEntry(PCSTR name)
{
	// Check_Object(this);
	Check_Pointer(name);
	Entry* entry;
	for (entry = firstEntry; entry; entry = entry->nextEntry)
	{
		Check_Pointer(entry);
		if (!_stricmp(entry->GetName(), name))
		{
			break;
		}
	}
	return entry;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
size_t NameList::FindEntryIndex(PCSTR name)
{
	// Check_Object(this);
	Check_Pointer(name);
	Entry* entry;
	size_t result = static_cast<size_t>(-1);
	for (entry = firstEntry; entry; entry = entry->nextEntry)
	{
		++result;
		Check_Pointer(entry);
		if (!strcmp(entry->GetName(), name))
		{
			return result;
		}
	}
	return static_cast<size_t>(-1);
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
NameList::Entry* NameList::FindEntry(PVOID data)
{
	// Check_Object(this);
	Entry* entry;
	for (entry = firstEntry; entry; entry = entry->nextEntry)
	{
		Check_Pointer(entry);
		if (entry->dataReference == data)
		{
			break;
		}
	}
	return entry;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
void NameList::DeleteEntry(PCSTR name)
{
	// Check_Object(this);
	Check_Pointer(name);
	Entry *prev = nullptr, *entry;
	for (entry = firstEntry; entry; entry = entry->nextEntry)
	{
		Check_Pointer(entry);
		if (!strcmp(entry->GetName(), name))
		{
			break;
		}
		prev = entry;
	}
	if (entry)
	{
		if (!prev)
		{
			firstEntry = entry->nextEntry;
		}
		else
		{
			prev->nextEntry = entry->nextEntry;
		}
		if (entry == lastEntry)
		{
			lastEntry = prev;
		}
		Unregister_Pointer(entry);
		delete entry;
	}
	return;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
std::ostream& MemoryStreamIO::Write(std::ostream& stream, const NameList* names)
{
	Check_Object(names);
	//
	//------------------------------
	// Write out the number of names
	//------------------------------
	//
	size_t i					 = 0;
	const NameList::Entry* entry = names->GetFirstEntry();
	while (entry)
	{
		Check_Object(entry);
		++i;
		entry = entry->GetNextEntry();
	}
	*stream << i;
	//
	//--------------------
	// Write out the names
	//--------------------
	//
	entry = names->GetFirstEntry();
	while (entry)
	{
		Check_Object(entry);
		PCSTR name = entry->GetName();
		Check_Pointer(name);
		stream.write(name, strlen(name) + 1);
		entry = entry->GetNextEntry();
	}
	return *stream;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
std::iostream& MemoryStreamIO::Read(std::iostream& stream, NameList* names)
{
	//
	//---------------------------------------------------------------------
	// Read in the number of strings to add, then loop through and add them
	//---------------------------------------------------------------------
	//
	int32_t count;
	*stream >> count;
	while (count-- > 0)
	{
		PCSTR name = Cast_Pointer(PCSTR, stream->GetPointer());
		names->AddEntry(name, nullptr);
		stream->AdvancePointer(strlen(name) + 1);
	}
	return *stream;
}

//#############################################################################
//##############    AlphaNameList    ##########################################
//#############################################################################

AlphaNameList::AlphaNameList(void) {}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
AlphaNameList::~AlphaNameList(void) {}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
PCSTR AlphaNameList::AddEntry(PCSTR name, PVOID data)
{
	// Check_Object(this);
	Check_Pointer(name);
	Entry *entry, *next = firstEntry, *prev = nullptr;
	entry = Cast_Pointer(Entry*, new char[sizeof(Entry) + strlen(name) + 1]);
	Register_Object(entry);
	//-----------------------------------
	// find location to insert new entry
	//-----------------------------------
	while (next)
	{
		Check_Pointer(next);
		if (strcmp(name, next->GetName()) < 0)
		{
			break;
		}
		prev = next;
		next = next->nextEntry;
	}
	//---------------------------------------------
	// insert new entry after prev and before next
	//---------------------------------------------
	if (!next)
	{
		lastEntry = entry;
	}
	if (!prev)
	{
		firstEntry = entry;
	}
	else
	{
		prev->nextEntry = entry;
	}
	entry->dataReference = data;
	entry->nextEntry	 = next;
	entry->SetName(name);
	return entry->GetName();
}

//#############################################################################
//#############################################################################
