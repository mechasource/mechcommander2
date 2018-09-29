//***************************************************************************
//
//	Dteam.h -- File contains the Team Definition
//
//	MechCommander 2
//
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#pragma once

#ifndef DTEAM_H
#define DTEAM_H

//---------------------------------------------------------------------------


enum __dteam_const : uint32_t
{
	MAX_TEAMS						= 8,
	MAX_MOVERS_PER_TEAM				= 120,
	MAX_CONTACTS_PER_TEAM			= MAX_MOVERS,
	NUM_CONTACT_UPDATES_PER_PASS	= 3,
};

typedef enum Relation : uint32_t
{
	RELATION_FRIENDLY,
	RELATION_NEUTRAL,
	RELATION_ENEMY,
	NUM_RELATIONS
} Relation;

//---------------------------------------------------------------------------

class Team;
typedef Team* TeamPtr;

struct _SystemTracker;
typedef _SystemTracker* SystemTrackerPtr;

#endif

//***************************************************************************
