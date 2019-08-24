
#pragma once

#ifndef MISSIONRESULTS_H
#define MISSIONRESULTS_H
/*************************************************************************************************\
MissionResults.h			: Interface for the MissionResults component.
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//
\*************************************************************************************************/


/**************************************************************************************************
CLASS DESCRIPTION
MissionResults:
**************************************************************************************************/

#include "mechgui/asystem.h"
#include "mechgui/alistbox.h"
#include "mpstats.h"

class SalvageMechScreen;
class MechIcon;
class BattleMech;
class PilotReviewScreen;

class MissionResults
{
public:
	MissionResults(void);
	~MissionResults(void);

	void init(void);

	void update(void);
	void render(void);

	void begin(void);
	void end(void);
	bool isDone() { return bDone; }

	void setHostLeftDlg(const std::wstring_view& pName);

	// Tutorial - Used to play Voice Over, first time in!!
	// THIS must be added to savegame!!
	//
	static bool FirstTimeResults;

private:
	SalvageMechScreen* pSalvageScreen;
	PilotReviewScreen* pPilotScreen;
	MPStats mpStats;
	bool bDone;
	bool bPilotStarted;

	friend class Logistics;
};

#endif // end of file ( MissionResults.h )
