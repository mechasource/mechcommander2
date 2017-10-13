#pragma once
//===========================================================================//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//
// Session.h: interface for the Session class.
//
//////////////////////////////////////////////////////////////////////
#include <dplay.h>
#include "flinkedlist.hpp"

#define MAXNAMELEN 64

class FIDPSession: public ListItem
{
protected:
	DPSESSIONDESC2 SessionDescriptor;
	char Name[MAXNAMELEN];
	char Password[MAXNAMELEN];

public:
	int32_t ID;

public:
	FIDPSession(int32_t id = 0);
	FIDPSession(DPSESSIONDESC2 sdesc, int32_t id);
	FIDPSession(FIDPSession& session);

	virtual ~FIDPSession(void);

	void Initialize(DPSESSIONDESC2 sdesc);


	DPSESSIONDESC2* GetSessionDescriptor()
	{
		return &SessionDescriptor;
	}

	inline void SetNumPlayers(int32_t nplayers)
	{
		SessionDescriptor.dwCurrentPlayers = nplayers;
	}

	inline void SetMaxPlayers(int32_t maxplayers)
	{
		SessionDescriptor.dwMaxPlayers = maxplayers;
	}

	void SetName(PSTR name);

	void SetPassword(PSTR password);

	inline int32_t GetNumPlayers()
	{
		return SessionDescriptor.dwCurrentPlayers;
	}

	inline int32_t GetMaxPlayers()
	{
		return SessionDescriptor.dwMaxPlayers;
	}

	inline PCSTR GetName()
	{
		return SessionDescriptor.lpszSessionNameA;
	}

	inline PCSTR GetPassword()
	{
		return SessionDescriptor.lpszPasswordA;
	}

	inline LPGUID GetGuidInstance()
	{
		return &SessionDescriptor.guidInstance;
	}


	inline BOOL IsPasswordRequired()
	{
		return SessionDescriptor.dwFlags & DPSESSION_PASSWORDREQUIRED;
	}

	inline BOOL IsLocked()
	{
		return SessionDescriptor.dwFlags & DPSESSION_NEWPLAYERSDISABLED;
	}

	inline void Lock()
	{
		if (!IsLocked())
		{
			SessionDescriptor.dwFlags |= DPSESSION_NEWPLAYERSDISABLED;
			PropagateSettings(void);
		}
	}

	void PropagateSettings(void);


};









