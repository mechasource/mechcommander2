//===========================================================================//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#pragma once


class GosEventIdMgr
{
	struct EventInfo
	{
		GosLogRef::EventType	m_type;
		PSTR 					m_pName;
		PSTR 					m_pFileName;
		int						m_nLineNo;
		ULONG					m_id;
	};


	static EventInfo			*pEventInfo;
	static ULONG				NextEntry;
	static int					ListSize;
	static int					ListSpace;

	public:
	static void					Resize();
	static ULONG				IdToEntry( ULONG id ) { return id & 0x000FFFFF; }
	static ULONG				EntryToId( ULONG entry, int type ) { return entry|(type<<20); }
	static ULONG				AssignId( GosLogRef::EventType type, PSTR name, PSTR filename, int lineno )
								{
									if( !ListSpace )
										Resize();
									EventInfo *pInfo = pEventInfo + NextEntry;
									ListSpace--;
									pInfo->m_type = type;
									pInfo->m_pName = name;
									pInfo->m_pFileName = filename;
									pInfo->m_nLineNo = lineno;
									pInfo->m_id = EntryToId( NextEntry, (int)type );
									NextEntry++;
									return pInfo->m_id;
								}
	static PSTR 				EventName( ULONG id ) { return pEventInfo[IdToEntry(id)].m_pName; }
	static void					Cleanup();
};
