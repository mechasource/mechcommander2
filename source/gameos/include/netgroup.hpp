//===========================================================================//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//
// NetGroup.hpp: interface for the FIDPGroup class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(NETGROUP_HPP)
#define NETGROUP_HPP

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#ifndef FLINKED_LIST_HPP
#include "flinkedlist.hpp"
#endif

#ifndef NETWORKMESSAGES_HPP
#include "networkmessages.hpp"
#endif

class ListFriendlyDPID : public ListItem
{
public:
	DPID ID;

	ListFriendlyDPID(DPID id)
	{
		ID = id;
	}

	ListFriendlyDPID& operator=(DPID id)
	{
		ID = id;
		return *this;
	}
};

class FIDPGroup : public ListItem
{
protected:
	DPID groupID;
	DPID parentGroupID;
	char shortName[64];
	char longName[256];
	uint32_t groupFlags;

	LPVOID groupData;
	uint32_t dataSize;
	FLinkedList<ListFriendlyDPID> playerIDList;

public:
	// Constructor and destructor
	FIDPGroup(void);
	FIDPGroup(DPID id, DPID parent_id, LPCDPNAME name, uint32_t flags);

	virtual ~FIDPGroup(void);

	inline FLinkedList<ListFriendlyDPID>* GetPlayerList()
	{
		return &playerIDList;
	}

	BOOL AddPlayer(DPID& id);

	BOOL RemovePlayer(DPID& id);

	inline void SetShortName(PSTR name)
	{
		if (name != nullptr)
		{
			strncpy(shortName, name, 64);
		}
		else
		{
			strcpy(shortName, "");
		}
	}

	inline void SetLongName(PSTR name)
	{
		if (name != nullptr)
		{
			strncpy(longName, name, 255);
		}
		else
		{
			strcpy(longName, "");
		}
	}

	PSTR GetShortName()
	{
		return shortName;
	}

	PSTR GetLongName()
	{
		return longName;
	}

	void SetGroupData(LPVOID data, uint32_t size);

	inline LPVOID GetGroupData()
	{
		return groupData;
	}

	inline DPID ID()
	{
		return groupID;
	}
};

#endif // !defined(FIDPGROUP_H)
