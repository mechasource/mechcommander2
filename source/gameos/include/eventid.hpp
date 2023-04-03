//===========================================================================//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#pragma once

#ifndef _EVENTID_HPP_
#define _EVENTID_HPP_

class GosEventIdMgr
{
	struct EventInfo
	{
		GosLogRef::EventType m_type;
		PSTR m_pName;
		PSTR m_pFileName;
		uint32_t m_nLineNo;
		uint32_t m_id;
	};

	static EventInfo* pEventInfo;
	static uint32_t NextEntry;
	static size_t ListSize;
	static int32_t ListSpace;

public:
	static void Resize(void);
	static uint32_t IdToEntry(uint32_t id)
	{
		return id & 0x000FFFFF;
	}
	static uint32_t EntryToId(uint32_t entry, int32_t type)
	{
		return entry | (type << 20);
	}
	static uint32_t AssignId(GosLogRef::EventType type, PSTR name, PSTR filename, int32_t lineno)
	{
		if (!ListSpace)
			Resize(void);
		EventInfo* pInfo = pEventInfo + NextEntry;
		ListSpace--;
		pInfo->m_type = type;
		pInfo->m_pName = name;
		pInfo->m_pFileName = filename;
		pInfo->m_nLineNo = lineno;
		pInfo->m_id = EntryToId(NextEntry, (int32_t)type);
		NextEntry++;
		return pInfo->m_id;
	}
	static PSTR EventName(uint32_t id)
	{
		return pEventInfo[IdToEntry(id)].m_pName;
	}
	static void Cleanup(void);
};

#endif
