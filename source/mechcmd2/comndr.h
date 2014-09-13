//***************************************************************************
//
//	comndr.h - This file contains the Commander Class header
//
//	MechCommander 2
//
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#pragma once

#ifndef COMNDR_H
#define COMNDR_H

//---------------------------------------------------------------------------

//#include "dcomndr.h"
//#include "gameobj.h"
//#include "dmover.h"
//#include "group.h"
//#include "dteam.h"
//#include "tacordr.h"
//#include "unitdesg.h"

//---------------------------------------------------------------------------
typedef struct _CommanderData
{
	int32_t					id;
	int32_t					teamId;
	MoverGroupData			groups[MAX_MOVERGROUPS];

} CommanderData;

typedef struct _StaticCommanderData
{
	int32_t	numCommanders;
	int32_t	homeCommanderId;

} StaticCommanderData;

class Commander
{

public:

	int32_t					id;
	TeamPtr					team;
	MoverGroupPtr			groups[MAX_MOVERGROUPS];

	static int32_t				numCommanders;
	static CommanderPtr		commanders[MAX_COMMANDERS];
	static CommanderPtr		home;

public:

	PVOID operator new(size_t ourSize);
	void operator delete(PVOID us);

	virtual void init(void);

	Commander(void)
	{
		init(void);
	}

	virtual void destroy(void);

	~Commander(void)
	{
		destroy(void);
	}

	virtual int32_t getId(void)
	{
		return(id);
	}

	virtual void setId(int32_t _id)
	{
		id = _id;
		commanders[id] = this;
	}

	virtual TeamPtr getTeam(void)
	{
		return(team);
	}

	virtual void setTeam(TeamPtr _team)
	{
		team = _team;
	}

	virtual MoverGroupPtr getGroup(int32_t groupNumber)
	{
		return(groups[groupNumber]);
	}

	int32_t setGroup(int32_t id, int32_t numMates, MoverPtr* moverList, int32_t point);

	void setLocalMoverId(int32_t localMoverId);

	void eject(void);

	void addToGUI(bool visible = true);

	void setHomeCommander(void)
	{
		home = this;
	}

	static CommanderPtr getCommander(int32_t _id)
	{
		return(commanders[_id]);
	}

	static int32_t Save(PacketFilePtr file, int32_t packetNum);
	static int32_t Load(PacketFilePtr file, int32_t packetNum);
};

//***************************************************************************

#endif
