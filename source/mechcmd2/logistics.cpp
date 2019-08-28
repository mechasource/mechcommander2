//******************************************************************************************
// logistics.cpp - This file contains the logistics class header
//
//	MechCommander 2
//
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//
#include "stdinc.h"

//----------------------------------------------------------------------------------
// Include Files
#include "logistics.h"
#include "mission.h"
#include "objmgr.h"
#include "team.h"
#include "comndr.h"
#include "missiongui.h"
#include "mech.h"
#include "multplyr.h"
#include "gamesound.h"
#include "prefs.h"
#include "gamecam.h"
#include "sounds.h"
#include "missionbegin.h"
#include "mechicon.h"
#include "missionresults.h"
#include "paths.h"
#include "resource.h"

extern CPrefs prefs;
extern bool quitGame;
extern bool justStartMission;
extern int32_t renderer;
extern bool useUnlimitedAmmo;
extern float loadProgress;
extern bool aborted;

void
DEBUGWINS_print(const std::wstring_view& s, int32_t window = 0);

//----------------------------------------------------------------------------------
// class Logistics
Logistics* logistics = nullptr;

//----------------------------------------------------------------------------------
void
Logistics::destroy(void)
{
	delete missionBegin;
	missionBegin = nullptr;
	delete missionResults;
	missionResults = nullptr;
}

//----------------------------------------------------------------------------------
void
Logistics::start(int32_t startMode)
{
	bMissionLoaded = 0;
	userInput->setMouseCursor(mState_LOGISTICS);
	//	userInput->mouseOn();
	uint32_t localRenderer = prefs.renderer;
	if (prefs.renderer != 0 && prefs.renderer != 3)
		localRenderer = 0;
	bool localFullScreen = prefs.fullScreen;
	bool localWindow = !prefs.fullScreen;
	if (Environment.fullScreen && prefs.fullScreen)
		localFullScreen = false;
	switch (startMode)
	{
	case log_RESULTS: // pull out results later...
		active = true;
		// Heading back to logistics now.  Change screen back to 800x600
		if (prefs.renderer == 3)
			gos_SetScreenMode(
				800, 600, 16, 0, 0, 0, true, localFullScreen, 0, localWindow, 0, localRenderer);
		else
			gos_SetScreenMode(800, 600, 16, prefs.renderer, 0, 0, 0, localFullScreen, 0,
				localWindow, 0, localRenderer);
		lastMissionResult = scenarioResult;
		if (!missionResults)
		{
			LogisticsData::instance->init();
			missionResults = new MissionResults;
			missionResults->init();
		}
		if (scenarioResult == mis_PLAYER_WIN_BIG || MPlayer)
		{
			LogisticsData::instance->removeMechsInForceGroup();
			if (!MPlayer)
				LogisticsData::instance->setMissionCompleted();
			// if mission is over, play video then quit when  done
			if (LogisticsData::instance->campaignOver() && !MPlayer && !LogisticsData::instance->isSingleMission())
			{
				mission->destroy();
				playFullScreenVideo(LogisticsData::instance->getFinalVideo());
				setLogisticsState(log_SPLASH);
				missionBegin->beginSplash();
				userInput->mouseOn();
				wchar_t tmp[256];
				cLoadString(IDS_FINAL_MISSION, tmp, 255);
				FullPathFileName path;
				path.init(savePath, tmp, ".fit");
				FitIniFile file;
				file.create(path);
				LogisticsData::instance->save(file);
				active = true;
			}
			else if (LogisticsData::instance->isSingleMission() && !MPlayer)
			{
				missionBegin->beginSplash();
				logisticsState = log_SPLASH;
			}
			else
			{
				missionResults->begin();
				logisticsState = log_RESULTS;
			}
		}
		else if (!justStartMission)
		{
			missionResults->bDone = true;
			logisticsState = log_RESULTS;
		}
		else // end the game
			quitGame = true;
		break;
	case log_SPLASH:
	{
		if (aborted)
		{
			if (prefs.renderer == 3)
				gos_SetScreenMode(
					800, 600, 16, 0, 0, 0, true, localFullScreen, 0, localWindow, 0, localRenderer);
			else
				gos_SetScreenMode(800, 600, 16, prefs.renderer, 0, 0, 0, localFullScreen, 0,
					localWindow, 0, localRenderer);
			if (missionBegin)
				missionBegin->beginSplash();
			if (MPlayer)
			{
				delete MPlayer;
				MPlayer = nullptr;
			}
		}
		initializeLogData();
		bool bTestScript = false;
		if (bTestScript)
		{
			FitIniFile loadFile;
			loadFile.open("data\\missions\\save.fit");
			LogisticsData::instance->load(loadFile);
			FitIniFile saveFile;
			saveFile.open("data\\missions\\save.fit", CREATE);
			LogisticsData::instance->save(saveFile);
		}
		active = TRUE;
		setLogisticsState(log_SPLASH);
		if (!missionBegin)
		{
			missionBegin = new MissionBegin;
			missionBegin->beginSplash();
			missionBegin->init();
		}
	}
	break;
	case log_ZONE:
	{
		logisticsData.init();
		if (!missionBegin)
		{
			missionBegin = new MissionBegin;
			missionBegin->init();
		}
		missionBegin->beginZone();
		active = TRUE;
		setLogisticsState(log_SPLASH);
		userInput->mouseOn();
		userInput->setMouseCursor(mState_NORMAL);
	}
	break;
	}
}

//----------------------------------------------------------------------------------
void
Logistics::stop(void)
{
	switch (prevState)
	{
	case log_SPLASH:
		break;
	case log_RESULTS:
		break;
	}
	active = FALSE;
}

//----------------------------------------------------------------------------------
int32_t
Logistics::update(void)
{
	// MUST do this every frame.  The movie movies will play wrong, otherwise!!
	if (bMovie)
	{
		userInput->mouseOff();
		if (userInput->getKeyDown(KEY_SPACE) || userInput->getKeyDown(KEY_ESCAPE) || userInput->getKeyDown(KEY_LMOUSE))
		{
			bMovie->stop();
		}
		bool result = bMovie->update();
		if (result)
		{
			if (LogisticsData::instance->campaignOver() && (strnicmp(bMovie->getMovieName(), "credits", 7) == 0))
			{
				missionBegin->beginSplash();
			}
			// Check if we are cinema5.  If we are, spool up the credits and
			// play those.
			// Otherwise, same as before Mr. Sulu.  Stay with him...
			if (strnicmp(bMovie->getMovieName(), "cinema5", 7) == 0)
			{
				// OLD Movie's Over.
				// Whack it.
				delete bMovie;
				bMovie = nullptr;
				FullPathFileName path;
				path.init(moviePath, "credits", ".bik");
				RECT movieRect;
				movieRect.top = 0;
				movieRect.left = 0;
				movieRect.right = Environment.screenwidth;
				movieRect.bottom = 600;
				bMovie = new MC2Movie;
				bMovie->init(path, movieRect, true);
				soundSystem->playDigitalMusic(33);
			}
			else
			{
				// Movie's Over.
				// Whack it.
				delete bMovie;
				bMovie = nullptr;
				soundSystem->playDigitalMusic(LogisticsData::instance->getCurrentMissionTune());
				userInput->mouseOn();
			}
		}
		return logisticsState;
	}
	// Don't keep setting the mouse cursor to logistics cursor during mission!!
	if (!mission->isActive())
		userInput->setMouseCursor(mState_LOGISTICS);
	// Stop doing this every frame.  ONLY turn it on when we need to!!
	if (logisticsState == log_RESULTS)
	{
		if (missionResults)
			missionResults->update();
		if (missionResults->isDone())
		{
			missionResults->end();
			if (!justStartMission && !MPlayer)
			{
				logisticsState = log_SPLASH;
				if (!missionBegin)
				{
					missionBegin = new MissionBegin();
					missionBegin->init();
				}
				missionBegin->begin();
				if (lastMissionResult == mis_PLAYER_LOST_BIG && !MPlayer)
				{
					if (LogisticsData::instance->skipLogistics())
					{
						if (LogisticsData::instance->showChooseMission())
							missionBegin->begin();
						else
							missionBegin->beginSplash();
					}
					else
						missionBegin->setToMissionBriefing();
				}
				else
				{
					const std::wstring_view& videoName = LogisticsData::instance->getCurrentBigVideo();
					if (videoName)
					{
						playFullScreenVideo(videoName);
					}
				}
			}
			else if (MPlayer)
			{
				logisticsState = log_SPLASH;
				if (!missionBegin)
				{
					missionBegin = new MissionBegin();
					missionBegin->init();
				}
				if (MPlayer->hostLeft || MPlayer->commanderid < 0)
				{
					missionBegin->beginSplash(nullptr);
					MPlayer->closeSession();
				}
				else
					missionBegin->restartMPlayer(nullptr);
			}
			else
				quitGame = true;
		}
	}
	// Used to start mission from command line, bypassing logistics
	else if (logisticsState == log_STARTMISSIONFROMCMDLINE)
	{
	}
	else if (logisticsState != log_DONE)
	{
		if (missionBegin)
		{
			const std::wstring_view& pVid = missionBegin->update();
			if (pVid && (0 != strcmp("", pVid)))
			{
				playFullScreenVideo(pVid);
				return logisticsState;
			}
			if (missionBegin->readyToLoad() && !bMissionLoaded)
			{
				missionBegin->end(); // Called to clean up ABL!!
				lastMissionResult = 0;
				if (!beginMission(0, 0, 0))
				{
					missionBegin->beginAtConnectionScreen();
					return (logisticsState);
				}
				bMissionLoaded = true;
			}
			if (missionBegin->isDone())
			{
				logisticsState = log_DONE;
			}
		}
	}
	return (logisticsState);
}

//----------------------------------------------------------------------------------
void
Logistics::render(void)
{
	if (bMovie)
	{
		bMovie->render();
	}
	else if (active)
	{
		if (logisticsState == log_RESULTS)
			missionResults->render();
		else if (logisticsState == log_SPLASH && missionBegin)
			missionBegin->render();
	}
}

//-----------------------------------------------------------------------------

extern uint32_t MultiPlayTeamId;
extern uint32_t MultiPlayCommanderId;

int32_t _stdcall Logistics::beginMission(PVOID, int32_t, PVOID[])
{
	if (MPlayer)
		MPlayer->setMode(MULTIPLAYER_MODE_LOADING);
	wchar_t commandersToLoad[MAX_MC_PLAYERS][3] = {
		{0, 0, 0}, {1, 1, 1}, {2, 2, 2}, {3, 3, 3}, {4, 4, 4}, {5, 5, 5}, {6, 6, 6}, {7, 7, 7}};
	int32_t missionLoadType = LogisticsData::instance->skipLogistics() ? MISSION_LOAD_SP_QUICKSTART
																	   : MISSION_LOAD_SP_LOGISTICS;
	if (MPlayer)
	{
		//---------------------------
		// Calc valid commanderIDs...
		int32_t curCommanderID = 0;
		for (size_t CID = 0; CID < MAX_MC_PLAYERS; CID++)
		{
			MPlayer->availableCIDs[CID] = true;
			if (MPlayer->playerInfo[CID].player && (MPlayer->playerInfo[CID].commanderid > -1))
			{
				if (CID != curCommanderID)
				{
					int32_t oldCommanderID = CID;
					MPlayer->playerInfo[CID].commanderid = curCommanderID;
					memcpy(&MPlayer->playerInfo[curCommanderID], &MPlayer->playerInfo[CID],
						sizeof(MC2Player));
					MPlayer->playerInfo[CID].player = nullptr;
					MPlayer->playerInfo[CID].commanderid = -1;
					for (size_t j = 0; j < MAX_MC_PLAYERS; j++)
						if (MPlayer->playerList[j].player == MPlayer->playerInfo[curCommanderID].player)
							MPlayer->playerList[j].commanderid = curCommanderID;
					if (oldCommanderID == MPlayer->commanderid)
						MPlayer->commanderid = curCommanderID;
				}
				MPlayer->availableCIDs[curCommanderID] = false;
				curCommanderID++;
				;
			}
		}
		//----------------------
		// Calc valid teamIDs...
		int32_t curTeamID = 0;
		for (size_t teamID = 0; teamID < MAX_MC_PLAYERS; teamID++)
		{
			bool teamFound = false;
			for (size_t i = 0; i < MAX_MC_PLAYERS; i++)
				if (MPlayer->playerInfo[i].player && (MPlayer->playerInfo[i].team == teamID))
				{
					MPlayer->playerInfo[i].team = curTeamID;
					teamFound = true;
				}
			if (teamFound)
				curTeamID++;
		}
		if (MPlayer->isHost())
		{
			// Determine drop zone order here...
			wchar_t dropZoneList[8];
			wchar_t hqs[MAX_TEAMS];
			if (MPlayer->missionSettings.missionType == MISSION_TYPE_OTHER)
			{
				bool goodToLoad = mission->calcComplexDropZones(
					(const std::wstring_view&)(const std::wstring_view&)LogisticsData::instance->getCurrentMission(), dropZoneList);
				if (!goodToLoad)
					STOP(("Logisitics.beginMission: teams do not match up for "
						  "complex mission"));
				for (size_t i = 0; i < MAX_TEAMS; i++)
					hqs[i] = i;
			}
			else
				MPlayer->calcDropZones(dropZoneList, hqs);
			if (MPlayer->missionSettings.quickStart)
				for (size_t i = 0; i < MAX_MC_PLAYERS; i++)
				{
					MPlayer->commandersToLoad[i][0] =
						(int32_t)dropZoneList[i]; // MPlayer->playerInfo[i].commanderid;
					MPlayer->commandersToLoad[i][1] = (int32_t)(dropZoneList[i] > -1)
						? MPlayer->playerInfo[dropZoneList[i]].team
						: 0;
					MPlayer->commandersToLoad[i][2] = hqs[i];
				}
			else
				for (size_t i = 0; i < MAX_MC_PLAYERS; i++)
				{
					MPlayer->commandersToLoad[i][0] = dropZoneList[i]; //-1;
					MPlayer->commandersToLoad[i][1] = (dropZoneList[i] > -1)
						? MPlayer->playerInfo[dropZoneList[i]].team
						: 0; //-1;
					MPlayer->commandersToLoad[i][2] = hqs[i];
				}
			MPlayer->sendMissionSetup(0, 0, nullptr);
		}
		if (!MPlayer->waitTillStartLoading())
		{
			// SERVER DROPPED
			return (0);
		}
		if (MPlayer->commandersToLoad[0][0] < -1)
			PAUSE(("Logistics.beginMission: bad commandersToLoad"));
		for (size_t i = 0; i < MAX_MC_PLAYERS; i++)
		{
			commandersToLoad[i][0] = MPlayer->commandersToLoad[i][0];
			commandersToLoad[i][1] = MPlayer->commandersToLoad[i][1];
			commandersToLoad[i][2] = MPlayer->commandersToLoad[i][2];
		}
		if (MPlayer->missionSettings.quickStart)
		{
			MultiPlayTeamId = MPlayer->playerInfo[MPlayer->commanderid].team;
			if (MultiPlayTeamId < 0)
				STOP(("Bad commanderid"));
			MultiPlayCommanderId = MPlayer->commanderid;
			if (MultiPlayCommanderId < 0)
				STOP(("Bad commanderid"));
			missionLoadType = MISSION_LOAD_MP_QUICKSTART;
		}
		else
		{
			MultiPlayTeamId = MPlayer->playerInfo[MPlayer->commanderid].team;
			MultiPlayCommanderId = MPlayer->commanderid;
			missionLoadType = MISSION_LOAD_MP_LOGISTICS;
		}
		int32_t maxTeam = -1;
		for (i = 0; i < MAX_MC_PLAYERS; i++)
			if (MPlayer->playerInfo[i].team > maxTeam)
				maxTeam = MPlayer->playerInfo[i].team;
		MPlayer->numTeams = maxTeam + 1;
	}
	else if (missionLoadType == MISSION_LOAD_SP_LOGISTICS)
	{
		commandersToLoad[0][0] = -1;
		commandersToLoad[0][1] = -1;
		commandersToLoad[0][2] = -1;
	}
	else
	{
		commandersToLoad[0][0] = 0;
		commandersToLoad[0][1] = 0;
		commandersToLoad[0][2] = -1;
	}
	if (mission)
		mission->destroy();
	int32_t numPlayers = 1;
	if (MPlayer)
		MPlayer->getPlayers(numPlayers);
	int32_t numMoversPerCommander[MAX_MC_PLAYERS] = {12, 12, 12, 9, 7, 6, 5, 4};
	Stuff::Vector3D dropZoneList[255]; // ubsurdly large, but sometimes we overrun this.
	int32_t dropZoneID = 0;
	if (MPlayer)
	{
		// dropZoneID = MPlayer->commanderid;
		for (size_t i = 0; i < MAX_MC_PLAYERS; i++)
			if (commandersToLoad[i][0] == MPlayer->commanderid)
			{
				dropZoneID = i;
				break;
			}
		useUnlimitedAmmo = MPlayer->missionSettings.unlimitedAmmo;
	}
	mission->init((const std::wstring_view&)(const std::wstring_view&)LogisticsData::instance->getCurrentMission(), missionLoadType,
		dropZoneID, dropZoneList, commandersToLoad, numMoversPerCommander[numPlayers - 1]);
	LogisticsData::instance->rpJustAdded = 0;
	if (MPlayer)
	{
		if (missionLoadType == MISSION_LOAD_MP_LOGISTICS)
		{
			EList<LogisticsMech*, LogisticsMech*> list;
			LogisticsData::instance->getForceGroup(list);
			int32_t dropZoneIndex = 0;
			int32_t numMechs = 0;
			for (EList<LogisticsMech*, LogisticsMech*>::EIterator iter = list.Begin();
				 !iter.IsDone(); iter++)
			{
				numMechs++;
				if (!(*iter)->getPilot())
					continue;
				CompressedMech mechData;
				mechData.lastMech = (list.Count() == numMechs);
				mechData.objNumber = (*iter)->getFitID();
				mechData.commanderid = MPlayer->commanderid;
				mechData.basecolour = MPlayer->colors[MPlayer->playerInfo[MPlayer->commanderid]
														 .basecolour[BASECOLOR_TEAM]];
				mechData.highlightcolour1 =
					MPlayer->colors[MPlayer->playerInfo[MPlayer->commanderid].stripecolour];
				mechData.highlightcolour2 =
					MPlayer->colors[MPlayer->playerInfo[MPlayer->commanderid].stripecolour];
				strcpy(mechData.pilotFile, (*iter)->getPilot()->getFileName());
				strcpy(mechData.mechFile, (*iter)->getFileName());
				strcpy(mechData.variantName, (*iter)->getName());
				mechData.variantNum = (*iter)->getVariant()->getFileID();
				mechData.cBills = (*iter)->getVariant()->getCost();
				mechData.pos[0] = dropZoneList[dropZoneIndex].x;
				mechData.pos[1] = dropZoneList[dropZoneIndex++].y;
				mechData.designerMech = (*iter)->getVariant()->isDesignerMech();
				mechData.numComponents = (*iter)->getComponentCount();
				if (mechData.numComponents)
				{
					int32_t* componentList =
						(int32_t*)systemHeap->Malloc(sizeof(int32_t) * mechData.numComponents);
					int32_t otherCount = mechData.numComponents;
					(*iter)->getComponents(otherCount, componentList);
					if (otherCount != mechData.numComponents)
						STOP(("Heidi's getComponentCount does not agree with "
							  "count returned from getComponents"));
					for (size_t i = 0; i < mechData.numComponents; i++)
						mechData.components[i] = (uint8_t)componentList[i];
				}
				MPlayer->sendMissionSetup(0, 1, &mechData);
			}
			if (!MPlayer->waitTillMechDataReceived())
			{
				// SERVER DROPPED
				mission->destroy();
				return (0);
			}
			ObjectManager->numMechs = 0;
			ObjectManager->numVehicles = 0;
			for (size_t i = 0; i < MAX_MC_PLAYERS; i++)
			{
				if (MPlayer->mechDataReceived[i])
				{
					for (size_t j = 0; j < 12; j++)
					{
						if (MPlayer->mechData[i][j].objNumber > -1)
						{
							MoverInitData data;
							memset(&data, 0, sizeof(MoverInitData));
							data.objNumber = MPlayer->mechData[i][j].objNumber;
							data.rosterIndex = 255;
							data.controlType = 2;
							data.controlDataType = 1;
							data.position.x = MPlayer->mechData[i][j].pos[0];
							data.position.y = MPlayer->mechData[i][j].pos[1];
							data.position.z = 0.0;
							data.rotation = 0;
							data.teamID =
								MPlayer->playerInfo[MPlayer->mechData[i][j].commanderid].team;
							data.commanderid = MPlayer->mechData[i][j].commanderid;
							data.basecolour = MPlayer->mechData[i][j].basecolour;
							data.highlightcolour1 = MPlayer->mechData[i][j].highlightcolour1;
							data.highlightcolour2 = MPlayer->mechData[i][j].highlightcolour2;
							data.gestureID = 2;
							data.active = 1;
							data.exists = 1;
							data.capturable = 0;
							data.icon = 0;
							strcpy(data.pilotFileName, MPlayer->mechData[i][j].pilotFile);
							strcpy(data.brainFileName, "pbrain");
							strcpy(data.csvFileName, MPlayer->mechData[i][j].mechFile);
							data.numComponents = MPlayer->mechData[i][j].numComponents;
							for (size_t k = 0; k < MPlayer->mechData[i][j].numComponents; k++)
								data.components[k] = MPlayer->mechData[i][j].components[k];
							int32_t moverHandle = mission->addMover(&data);
							if (moverHandle < 1)
								STOP(("Logistics.beginMission: unable to "
									  "addMover"));
							std::unique_ptr<Mover> mover = (std::unique_ptr<Mover>)ObjectManager->get(moverHandle);
							if (!mover)
								STOP(("Logistics.beginMission: nullptr mover"));
							if (mover->getObjectClass() != BATTLEMECH)
								STOP(("Logistics.beginMission: not a mech"));
							((BattleMech*)mover)->cBills = MPlayer->mechData[i][j].cBills;
							strcpy(((BattleMech*)mover)->variantName,
								MPlayer->mechData[i][j].variantName);
							data.variant = MPlayer->mechData[i][j].variantNum;
						}
					}
				}
			}
		}
		else // gotta update pilot availability
		{
			int32_t count = 256;
			LogisticsPilot* pilots[256];
			LogisticsData::instance->getPilots(pilots, count);
			for (size_t i = 0; i < count; i++)
			{
				pilots[i]->setUsed(0);
			}
			Team* pTeam = Team::home;
			if (pTeam)
			{
				for (i = pTeam->getRosterSize() - 1; i > -1; i--)
				{
					Mover* pMover = (Mover*)pTeam->getMover(i);
					if (pMover && pMover->getCommander()->getId() == Commander::home->getId())
					{
						LogisticsPilot* pPilot =
							LogisticsData::instance->getPilot(pMover->getPilot()->getName());
						if (pPilot)
							pPilot->setUsed(true);
					}
				}
			}
		}
		CompressedMech mechData;
		mechData.commanderid = MPlayer->commanderid;
		MPlayer->sendMissionSetup(0, 2, &mechData);
		if (!MPlayer->waitTillMissionLoaded())
		{
			// SERVER DROPPED
			mission->destroy();
			return (0);
		}
	}
	else if (missionLoadType == MISSION_LOAD_SP_LOGISTICS)
	{
		EList<LogisticsMech*, LogisticsMech*> list;
		LogisticsData::instance->getForceGroup(list);
		float rotation = list.Count() ? 360.f / list.Count() : 0.f;
		MoverInitData data;
		memset(&data, 0, sizeof(data));
		data.rosterIndex = 255;
		data.controlType = 2;
		data.controlDataType = 1;
		data.position.Zero(); // need to get the drop zone location
		data.rotation = 0;
		data.teamID = Team::home->getId();
		data.commanderid = Team::home->getId();
		data.active = 1;
		data.exists = 1;
		data.capturable = 0;
		data.basecolour = prefs.basecolour;
		data.highlightcolour1 = prefs.highlightcolour;
		data.highlightcolour2 = prefs.highlightcolour;
		strcpy(data.pilotFileName, "pmw00031");
		strcpy(data.brainFileName, "pbrain");
		Stuff::Vector3D vector;
		vector.x = 0.f;
		vector.y = 1.f;
		vector.z = 0.f;
		Stuff::Vector3D scaled;
		for (EList<LogisticsMech*, LogisticsMech*>::EIterator iter = list.Begin(); !iter.IsDone();
			 iter++)
		{
			scaled = vector;
			scaled *= 128.;
			data.position.Add(dropZoneList[0], scaled);
			// data.position = dropZoneList[dropZoneIndex++];
			data.objNumber = (*iter)->getFitID();
			strcpy(data.csvFileName, (*iter)->getFileName());
			if (!(*iter)->getPilot())
				continue;
			strcpy(data.pilotFileName, (*iter)->getPilot()->getFileName());
			data.overrideLoadedPilot = true;
			data.gunnerySkill = (*iter)->getPilot()->getGunnery();
			data.pilotingSkill = (*iter)->getPilot()->getPiloting();
			memcpy(data.specialtySkills, (*iter)->getPilot()->getSpecialtySkills(),
				sizeof(bool) * NUM_SPECIALTY_SKILLS);
			if (!(*iter)->getVariant()->isDesignerMech())
			{
				data.variant = 0;
				mission->addMover(&data, (*iter));
			}
			else
			{
				data.variant = (*iter)->getVariant()->getFileID();
				mission->addMover(&data);
			}
			Rotate(vector, rotation);
		}
	}
	/*	if (MPlayer) {
			if (MPlayer->isHost())
				MPlayer->serverCID = MPlayer->commanderid;
			else {
				for (size_t i = 0; i < MAX_MC_PLAYERS; i++)
					if (MPlayer->serverPlayer && (MPlayer->playerList[i].player
	   == MPlayer->serverPlayer)) MPlayer->serverCID =
	   MPlayer->playerList[i].commanderid; if (MPlayer->serverCID == -1)
					STOP(("Logistics.beginMission: bad serverCID"));
			}
		}
	*/
	mission->missionInterface->initMechs();
	eye->activate();
	eye->update();
	mission->start();
	mission->update(); // force this, so we don't render first
	if (MPlayer)
	{
		CompressedMech mechData;
		mechData.commanderid = MPlayer->commanderid;
		MPlayer->sendMissionSetup(0, 4, &mechData);
		if (!MPlayer->waitTillMissionSetup())
		{
			// SERVER DROPPED
			mission->destroy();
			return (0);
		}
		MPlayer->setMode(MULTIPLAYER_MODE_MISSION);
	}
	if (MPlayer && MPlayer->hostLeft)
	{
		mission->destroy();
		return (0);
	}
	return (1);
}

int32_t
Logistics::DoBeginMission()
{
#ifndef TEST_SHELL
//---------------------------
// Networking startup code...
#if 0
	/*MPlayer->launchedFromLobby =*/ gos_NetCheckLobby();
	Environment.NetworkGame = TRUE;
	if(!MPlayer->launchedFromLobby)
	{
		if(!gos_NetStartGame(0))
		{
			//gos_TerminateApplication();
			return;
		}
	}
#endif
	if (!MPlayer)
	{
		logistics->setLogisticsState(log_DONE);
		soundSystem->playBettySample(BETTY_NEW_CAMPAIGN);
	}
#endif // TEST_SHELL
	return 0;
}

void
Logistics::initializeLogData()
{
	LogisticsData::instance->removeMechsInForceGroup();
	LogisticsData::instance->init();
	Team* pTeam = Team::home;
	if (pTeam)
	{
		for (size_t i = pTeam->getRosterSize() - 1; i > -1; i--)
		{
			Mover* pMover = (Mover*)pTeam->getMover(i);
			LogisticsPilot* pPilot =
				LogisticsData::instance->getPilot(pMover->getPilot()->getName());
			uint32_t base, highlight1, highlight2;
			((Mech3DAppearance*)pMover->getAppearance())
				->getPaintScheme(highlight1, highlight2, base);
			if (pMover->getObjectType()->getObjectTypeClass() == BATTLEMECH_TYPE)
			{
				LogisticsVariant* pVar =
					LogisticsData::instance->getVariant(((BattleMech*)pMover)->variantName);
				LogisticsData::instance->addMechToInventory(
					pVar, 1, pPilot, base, highlight1, highlight2);
			}
		}
	}
}

void
Logistics::playFullScreenVideo(const std::wstring_view& fileName)
{
	if (!fileName || !fileName[0])
		return;
	FullPathFileName path;
	path.init(moviePath, fileName, ".bik");
	RECT movieRect;
	movieRect.top = 100;
	movieRect.left = 0;
	movieRect.right = Environment.screenwidth;
	movieRect.bottom = 500;
	bMovie = new MC2Movie;
	bMovie->init(path, movieRect, true);
	soundSystem->stopDigitalMusic();
}

void
Logistics::setResultsHostLeftDlg(const std::wstring_view& pName)
{
	if (missionResults && logisticsState == log_RESULTS)
	{
		missionResults->setHostLeftDlg(pName);
	}
}
//----------------------------------------------------------------------------------
