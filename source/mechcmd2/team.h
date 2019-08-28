//***************************************************************************
//
//	team.h - This file contains the Team Class header
//
//	MechCommander 2
//
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#pragma once

#ifndef TEAM_H
#define TEAM_H

//---------------------------------------------------------------------------

//#include "mclib.h"
//#include "dteam.h"
//#include "dmover.h"
//#include "contact.h"
//#include "objective.h"

//***************************************************************************

typedef struct _SystemTracker
{
	GameObjectPtr owner;
	int32_t masterId;
	float effect;
	SystemTrackerPtr prev;
	SystemTrackerPtr next;
} SystemTracker;

//---------------------------------------------------------------------------
typedef struct _TeamData
{
	int32_t id;
	int32_t rosterSize;
	GameObjectWatchID roster[MAX_MOVERS_PER_TEAM];
} TeamData;

typedef struct _TeamStaticData
{
	int32_t numTeams;
	int32_t homeTeamId;
	wchar_t relations[MAX_TEAMS][MAX_TEAMS];
	bool noPain[MAX_TEAMS];
} TeamStaticData;

class Team
{

public:
	//-------------
	// general info
	int32_t id;
	int32_t rosterSize;
	GameObjectWatchID roster[MAX_MOVERS_PER_TEAM]; // list of mover WIDs

	//-------------
	// mission info
	CObjectives objectives;
	int32_t numPrimaryObjectives;

	//------------------
	// static class info
	static int32_t numTeams;
	static TeamPtr home;
	static TeamPtr teams[MAX_TEAMS];
	static SortListPtr sortList;
	static wchar_t relations[MAX_TEAMS][MAX_TEAMS];
	static bool noPain[MAX_TEAMS];

public:
	virtual void init(void);

	Team(void) { init(void); }

	virtual int32_t init(int32_t _id, FitIniFile* pMissionFile = 0);

	virtual int32_t loadObjectives(FitIniFile* pMissionFile = 0);

	int32_t getId(void) { return (id); }

	void buildRoster(void);

	void addToRoster(std::unique_ptr<Mover> mover);

	int32_t getRosterSize(void) { return (rosterSize); }

	std::unique_ptr<Mover> getMover(int32_t index);

	void removeFromRoster(std::unique_ptr<Mover> mover);

	virtual int32_t getRoster(GameObjectPtr* objList, bool existsOnly = false);

	void disableTargets(void);

	void destroyTargets(void);

	bool isTargeting(GameObjectWatchID targetWID, GameObjectWatchID exceptWID);

	bool isCapturing(GameObjectWatchID targetWID, GameObjectWatchID exceptWID);

	bool isContact(GameObjectPtr looker, std::unique_ptr<Mover> mover, int32_t contactCriteria);

	virtual int32_t getContacts(
		GameObjectPtr looker, int32_t* contactList, int32_t contactCriteria, int32_t sortType);

	bool hasSensorContact(int32_t teamID);

	void addToGUI(void);

	Stuff::Vector3D calcEscapeVector(std::unique_ptr<Mover> mover, float threatRange);

	void statusCount(int32_t* statusTally);

	void eject(void);

	virtual int32_t init(FitIniFile* unitFile) { return (NO_ERROR); }

	virtual void destroy(void);

	~Team(void) { destroy(void); }

	static bool lineOfSight(float startLocal, int32_t mCellRow, int32_t mCellCol, float endLocal,
		int32_t tCellRow, int32_t tCellCol, int32_t teamId, float targetRadius,
		float startRadius = 0.0f, bool checkVisibleBits = true);

	static bool lineOfSight(Stuff::Vector3D myPos, Stuff::Vector3D targetposition, int32_t teamId,
		float targetRadius, float startRadius = 0.0f, bool checkVisibleBits = true);

	//-------------------------------------------
	// Can anyone on my team see this position?
	// Used for cursors, artillery, indirect fire.
	bool teamLineOfSight(Stuff::Vector3D tPos, float extRad);

	void markRadiusSeen(Stuff::Vector3D& location, float radius);

	void markRadiusSeenToTeams(
		Stuff::Vector3D& location, float radius = -1, bool shrinkForNight = false);

	void markSeen(Stuff::Vector3D& location, float specialUnitExpand);

	void setRelation(TeamPtr team, wchar_t relation)
	{
		relations[id][team->getId()] = relation;
		relations[team->getId()][id] = relation;
	}

	int32_t getRelation(TeamPtr team)
	{
		if (!team)
			return RELATION_NEUTRAL;
		return (relations[id][team->getId()]);
	}

	static int32_t getRelation(int32_t teamID1, int32_t teamID2)
	{
		if (teamID1 == -1)
			return (0);
		if (teamID2 == -1)
			return (0);
		return (relations[teamID1][teamID2]);
	}

	static int32_t Save(PacketFilePtr file, int32_t packetNum);
	static int32_t Load(PacketFilePtr file, int32_t packetNum);

	bool isFriendly(TeamPtr team)
	{
		if (!team)
			return 0;
		return (relations[id][team->getId()] == RELATION_FRIENDLY);
	}

	bool isEnemy(TeamPtr team)
	{
		if (!team)
			return 0;
		return (relations[id][team->getId()] == RELATION_ENEMY);
	}

	bool isNeutral(TeamPtr team)
	{
		if (!team)
			return true;
		return (relations[id][team->getId()] == RELATION_NEUTRAL);
	}
};

//***************************************************************************

#endif
