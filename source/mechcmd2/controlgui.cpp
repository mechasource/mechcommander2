/*************************************************************************************************\
controlGui.cpp			: Implementation of the controlGui component.
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//
\*************************************************************************************************/
#include "stdinc.h"

#include "controlgui.h"
#include "Team.h"
#include "gamesound.h"
#include "comndr.h"

#ifndef MULTPLYR_H
#include "multplyr.h"
#endif

#ifndef GAMETACMAP_H
#include "gametacmap.h"
#endif

#ifndef LOGISTICSDATA_H
#include "LogisticsData.h"
#endif

#ifndef PAUSEWINDOW_H
#include "pausewindow.h"
#endif

#include "Objective.h"
#include "mission.h"
#include "infoWindow.h"

#include "txmmgr.h"

#include "resource.h"
#include "malloc.h"

#include "chatwindow.h"

ControlGui* ControlGui::instance = 0;

extern uint32_t helpTextHeaderID;
extern uint32_t helpTextID;

extern bool neverEndingStory;

ButtonData* ControlGui::buttonData = nullptr;
ButtonData* ControlGui::vehicleData = nullptr;

int32_t ControlGui::hiResOffsetX = 0;
int32_t ControlGui::hiResOffsetY = 0;

int32_t ControlGui::OBJECTIVESTOP = 0;
int32_t ControlGui::OBJECTIVESLEFT = 0;
int32_t ControlGui::OBJECTIVESSKIP = 0;
int32_t ControlGui::OBJECTIVESTOTALRIGHT = 0;
int32_t ControlGui::OBJEECTIVESHEADERSKIP = 0;
int32_t ControlGui::OBJECTIVESHEADERTOP = 0;
int32_t ControlGui::OBJECTIVEBOXX = 0;
int32_t ControlGui::OBJECTIVEBOXSKIP = 0;
int32_t ControlGui::OBJECTIVECHECKSKIP = 0;
int32_t ControlGui::OBJECTIVEHEADERLEFT = 0;
int32_t ControlGui::HELPAREA_LEFT = 0;
int32_t ControlGui::HELPAREA_BOTTOM = 0;
int32_t ControlGui::RPLEFT = 0;
int32_t ControlGui::RPTOP = 0;

extern bool useLeftRightMouseProfile;
extern bool drawGUIOn; // Used to shut off GUI for Screen Shots and Movie Mode
extern bool paintingMyVtol;

#define TEXT_SKIP 5.f * Environment.screenheight / 600.f
#define CHAT_DISPLAY_TIME 30

int32_t __cdecl sortStats(PCVOID pPlayer1, PCVOID pPlayer2);

int32_t ControlGui::vehicleCosts[LAST_VEHICLE] = {7000, 8000, 2000, 6000, 2000, 4000, 10000, 0};

std::wstring_view ControlGui::vehicleNames[5] = {
	"repairTruck", "scout", "minelayer", "pv20600",
	"guardTower" // needs to be recovery

};

int32_t ControlGui::vehicleIDs[5] = {182, 393, 120, 147, 415};

std::wstring_view ControlGui::vehiclePilots[5] = {
	"PMWRepair", "PMWScout", "PMWMinelayer", "PMWRecover", "PMWGuardTower"};

uint32_t ControlGui::WALK = 0;
uint32_t ControlGui::RUN = 0x01;
uint32_t ControlGui::GUARD = 0x02;
uint32_t ControlGui::JUMP = 0x04;

MoveInfo ControlGui::objectiveMoveInfo[OBJECTVE_MOVE_COUNT] = {0.0f, -640, 0.0f, 0};

MoveInfo ControlGui::missionResultsMoveInfo[RESULTS_MOVE_COUNT] = {

	0.0f, -65, 0.16f, 0, 0.24f, -10};

ControlGui::ControlGui()
{
	gosASSERT(!instance);
	instance = this;
	idToUnPress = -1;
	renderObjectives = 0;
	addingVehicle = 0;
	addingArtillery = 0;
	addingSalvage = 0;
	wasLayingMines = 0;
	bMovie = nullptr;
	moviePlaying = 0;
	helpTextHeaderID = 0;
	helpTextID = 0;
	rectInfos = nullptr;
	rectCount = nullptr;
	staticInfos = 0;
	staticCount = 0;
	buttons = 0;
	buttonData = 0;
	vehicleButtons = 0;
	vehicleData = 0;
	videoInfos = 0;
	videoInfoCount = 0;
	timerInfos = 0;
	timerInfoCount = 0;
	curOrder = 0;
	fireFromCurrentPos = 0;
	pauseWnd = new PauseWindow;
	renderStatusInfo = 0;
	missionStatusInfos = 0;
	objectiveTime = 0.0f;
	infoWnd = 0;
	bChatting = 0;
	if (!infoWnd)
	{
		infoWnd = new InfoWindow;
	}
	objectiveInfos = 0;
	swapResolutions(Environment.screenwidth);
	resultsTime = 0;
	twoMinWarningPlayed = false;
	thirtySecondWarningPlayed = false;
	tabFlashTime = 0;
	memset(chatInfos, 0, MAX_CHAT_COUNT * sizeof(ChatInfo));
	chatEdit.text.SetBufferSize(512);
	playerNameEdit.setBufferSize(16); // pre - alloc
	buttonToPress = -1;
	rpNumFlashes = 0;
	rpFlashTime = 0.0f;
	chatIsTeamOnly = 0;
	bServerWarningShown = 0;
}

ControlGui::~ControlGui()
{
	if (staticInfos)
		delete[] staticInfos;
	staticInfos = 0;
	if (videoInfos)
		delete[] videoInfos;
	if (objectiveInfos)
		delete[] objectiveInfos;
	if (timerInfos)
		delete[] timerInfos;
	timerInfos = 0;
	if (rectInfos)
		delete[] rectInfos;
	rectInfos = 0;
	if (buttonData)
		delete[] buttonData;
	buttonData = 0;
	if (buttons)
		delete[] buttons;
	buttons = 0;
	if (infoWnd)
		delete infoWnd;
	infoWnd = 0;
	if (pauseWnd)
		delete pauseWnd;
	pauseWnd = 0;
	if (vehicleButtons)
		delete[] vehicleButtons;
	vehicleButtons = 0;
	if (vehicleData)
		delete[] vehicleData;
	vehicleData = 0;
	if (missionStatusInfos)
		delete[] missionStatusInfos;
	missionStatusInfos = 0;
	instance = nullptr;
	if (bMovie)
	{
		delete bMovie;
		bMovie = nullptr;
	}
}

bool ControlGui::pushButton(int32_t buttonId)
{
	buttonToPress = buttonId;
	// I KNOW its hard-coded, OK?
	if ((buttonId >= 12) && (buttonId <= 14))
	{
		// Unpress TacMap if we didn't want to press that.
		if ((buttonId != 12) && getButton(12)->state & ControlButton::PRESSED)
			getButton(12)->press(false);
		// Unpress Info Button if we didn't want to press that.
		if ((buttonId != 13) && getButton(13)->state & ControlButton::PRESSED)
			getButton(13)->press(false);
		// Unpress VehicleTab if we didn't want to press that.
		if ((buttonId != 14) && getButton(14)->state & ControlButton::PRESSED)
			getButton(14)->press(false);
	}
	return true;
}

bool ControlGui::flashRPTotal(int32_t numFlashes)
{
	rpNumFlashes = numFlashes;
	rpFlashTime = 0.0f;
	return true;
}

bool ControlGui::animateTacMap(int32_t buttonId, float timeToScroll, int32_t numFlashes)
{
	return tacMap.animate(abs(buttonId), numFlashes);
}

void ControlGui::render(bool bPaused)
{
	if (drawGUIOn)
	{
		gos_SetRenderState(gos_State_AlphaMode, gos_Alpha_AlphaInvAlpha);
		gos_SetRenderState(gos_State_Specular, FALSE);
		gos_SetRenderState(gos_State_AlphaTest, TRUE);
		gos_SetRenderState(gos_State_Filter, gos_FilterNone);
		gos_SetRenderState(gos_State_ZWrite, 0);
		gos_SetRenderState(gos_State_ZCompare, 0);
		for (size_t i = 0; i < rectCount; i++)
		{
			drawRect(rectInfos[i].rect, rectInfos[i].color);
		}
		if (getButton(TACMAP_TAB)->state & ControlButton::PRESSED)
			tacMap.render();
		else if (getButton(VEHICLE_TAB)->state & ControlButton::PRESSED)
			renderVehicleTab();
		else
			renderInfoTab();
		for (i = 0; i < staticCount; i++)
		{
			staticInfos[i].render();
		}
		forceGroupBar.render();
		renderHelpText();
		gos_SetRenderState(gos_State_AlphaMode, gos_Alpha_OneZero);
		for (i = 0; i < LAST_COMMAND; ++i)
			buttons[i].render();
		// want to draw the tab on top last
		for (i = TACMAP_TAB; i < TACMAP_TAB + 3; i++)
		{
			if ((getButton(i)->state) & ControlButton::PRESSED)
			{
				buttons[i].render();
			}
		}
		getButton(LAYMINES)->render();
		RenderObjectives();
	}
	if (moviePlaying && bMovie)
	{
		for (size_t i = 0; i < videoInfoCount; i++)
			videoInfos[i].render();
		bMovie->render();
	}
	if (drawGUIOn)
	{
		// draw the mission timer if we have one
		if (mission->m_timeLimit > 0)
		{
			float fTime = (float)mission->m_timeLimit - scenarioTime;
			int32_t time = (int32_t)fTime;
			if (time <= 120 && !twoMinWarningPlayed)
			{
				twoMinWarningPlayed = true;
				soundSystem->playBettySample(BETTY_TWO_MIN_LEFT);
			}
			else if (time <= 30 && !thirtySecondWarningPlayed)
			{
				thirtySecondWarningPlayed = true;
				soundSystem->playBettySample(BETTY_THIRTY_LEFT);
			}
			else if (time < 0)
			{
				time = 0;
			}
			wchar_t buffer[32];
			uint32_t minutes = time / 60;
			uint32_t seconds = time % 60;
			sprintf(buffer, "%03ld : %02ld", minutes, seconds);
			uint32_t color = timerRect.color;
			if (minutes < 5)
			{
				color = 0xffc61c00;
				if (minutes < 2)
				{
					if (seconds % 2)
					{
						color = 0;
					}
				}
			}
			int32_t width = timerFont.width(buffer);
			drawShadowText(color, 0x00000000, timerFont.getTempHandle(),
				timerRect.rect.left - width, timerRect.rect.top, true, buffer, 0,
				guiFont.getSize());
			for (size_t i = 0; i < timerInfoCount; i++)
				timerInfos[i].render();
		}
		if (bPaused)
			pauseWnd->render();
		if ((!MPlayer && (Team::home->objectives.Count() != 0) && (Team::home->objectives.Status() != OS_UNDETERMINED)) || mission->terminationCounterStarted)
		{
			if (!resultsTime)
				resultsTime = .001f;
			renderStatusInfo = 1;
		}
		renderResults();
		renderChatText();
		// l XLocation = 80
		// l YLocation = 505
		// l width = 41
		// l height = 34
		// l colour = 0xff000000
		// b outline = FALSE
		// flash the rp tab if we just got kudos
		if (LogisticsData::instance->rpJustAdded)
		{
			tabFlashTime += frameLength;
			if (tabFlashTime > .5)
			{
				tabFlashTime = 0;
			}
			else if (tabFlashTime > .25)
			{
				float scaleX = Environment.screenwidth <= 1024 ? Environment.screenwidth : 1024;
				float scaleY = Environment.screenheight <= 768 ? Environment.screenheight : 768;
				RECT rect = {51 * scaleX / 640.f + ControlGui::hiResOffsetX,
					317 * scaleY / 480.f + ControlGui::hiResOffsetY,
					74 * scaleX / 640.f + ControlGui::hiResOffsetX,
					337 * scaleY / 480.f + ControlGui::hiResOffsetY};
				drawRect(rect, 0xff000000);
			}
		}
	}
}

void ControlGui::startObjectives(bool bStart)
{
	if (!renderObjectives)
		objectiveTime = 0.f;
	else if (renderStatusInfo) // reset timer, if we are in results land
		objectiveTime = objectiveMoveInfo[OBJECTVE_MOVE_COUNT - 1].time;
	renderObjectives = bStart;
	if (MPlayer)
	{
		MPlayer->calcPlayerRanks();
		bool scoreShown[MAX_MC_PLAYERS]; // keep track of whose shown the score
		memset(scoreShown, 0, sizeof(bool) * MAX_MC_PLAYERS);
		int32_t playerCount = 0;
		const MC2Player* players = MPlayer->getPlayers(playerCount);
		const MC2Player* sorted[MAX_MC_PLAYERS];
		for (size_t j = 0; j < playerCount; j++)
		{
			sorted[j] = &players[j];
		}
		qsort(sorted, playerCount, sizeof(MC2Player*), sortStats);
		mpStats[0].setData(nullptr, 1); // this is the header...
		for (size_t i = 1; i < MAX_MC_PLAYERS + 1; i++)
		{
			if (i - 1 < playerCount)
			{
				mpStats[i].setData(sorted[i - 1], !scoreShown[sorted[i - 1]->team]);
				scoreShown[sorted[i - 1]->team] = true;
				mpStats[i].showGUIWindow(true);
			}
			else
				mpStats[i].showGUIWindow(0);
		}
	}
};

void ControlGui::renderResults()
{
	if (renderStatusInfo)
	{
		if (infoWnd) // kill info wnd stuff
			infoWnd->setUnit(nullptr);
		resultsTime += frameLength;
		float t0, t1, p0, p1;
		t1 = p1 = t0 = p0 = 0.f;
		float delta = 0.f;
		// figure out where we are in animation
		for (size_t j = 0; j < RESULTS_MOVE_COUNT - 1; j++)
		{
			if (missionResultsMoveInfo[j].time <= resultsTime && missionResultsMoveInfo[j + 1].time > resultsTime)
			{
				t0 = missionResultsMoveInfo[j].time;
				t1 = missionResultsMoveInfo[j + 1].time;
				p0 =
					(missionResultsMoveInfo[j].position) * ((float)Environment.screenwidth) / 640.f;
				p1 = (missionResultsMoveInfo[j + 1].position) * ((float)Environment.screenwidth) / 640.f;
				break;
			}
		}
		// if not done yet
		if (t1)
		{
			float dT = resultsTime - t0;
			float currentPosition = p0 + dT * ((p1 - p0) / (t1 - t0));
			delta = currentPosition - missionStatusInfos[0].location[0].y;
		}
		else
		{
			delta = missionResultsMoveInfo[OBJECTVE_MOVE_COUNT - 1].position - missionStatusInfos[0].location[0].y;
			// grey down the screen
			RECT rect = {0, 0, Environment.screenwidth, Environment.screenheight};
			drawEmptyRect(rect, 0x44000000);
			if (resultsTime < missionResultsMoveInfo[OBJECTVE_MOVE_COUNT - 1].time + .25)
				startObjectives(1);
		}
		for (size_t i = 0; i < missionStatusInfoCount; i++)
		{
			missionStatusInfos[i].move(0, delta);
			missionStatusInfos[i].render();
		}
		missionStatusRect.rect.top += delta;
		missionStatusRect.rect.bottom += delta;
		wchar_t text[256];
		if (Team::home->objectives.Status() == OS_SUCCESSFUL)
			cLoadString(IDS_MISSION_SUCCESS, text, 256);
		else
			cLoadString(IDS_MISSION_FAILED, text, 256);
		if (MPlayer)
		{
			if (MPlayer->playerInfo[MPlayer->commanderid].winner)
				cLoadString(IDS_MISSION_SUCCESS, text, 256);
			else
				cLoadString(IDS_MISSION_FAILED, text, 256);
		}
		gos_TextDrawBackground(missionStatusRect.rect.left, missionStatusRect.rect.top,
			missionStatusRect.rect.right, missionStatusRect.rect.bottom, 0xff000000);
		missionResultsFont.render(text, missionStatusRect.rect.left, missionStatusRect.rect.top,
			missionStatusRect.rect.right - missionStatusRect.rect.left,
			missionStatusRect.rect.bottom - missionStatusRect.rect.top, missionStatusRect.color, 0,
			3);
		float fadeTime = objectiveTime - objectiveMoveInfo[OBJECTVE_MOVE_COUNT - 1].time - 3.0f - Team::home->objectives.Count();
		if (!neverEndingStory && (fadeTime > 0.f))
		{
			int32_t color = interpolatecolour(0x00000000, 0xff000000, fadeTime / 1.0f);
			RECT rect = {0, 0, Environment.screenwidth, Environment.screenheight};
			drawRect(rect, color);
		}
	}
}

bool playedObjectiveClick[100];
void ControlGui::RenderObjectives()
{
	// make sure we need to do it.
	if (renderObjectives || renderStatusInfo)
	{
		if (renderStatusInfo)
		{
			if (resultsTime < .24f) // don't start objectives until mission results is done
			{
				memset(playedObjectiveClick, 0, sizeof(bool) * 100);
				return;
			}
		}
		objectiveTime += frameLength;
		float t0, t1, p0, p1;
		t1 = p1 = t0 = p0 = 0.f;
		float delta = 0.f;
		// figure out where we are in animation
		for (size_t j = 0; j < OBJECTVE_MOVE_COUNT - 1; j++)
		{
			if (objectiveMoveInfo[j].time <= objectiveTime && objectiveMoveInfo[j + 1].time > objectiveTime)
			{
				t0 = objectiveMoveInfo[j].time;
				t1 = objectiveMoveInfo[j + 1].time;
				p0 = (objectiveMoveInfo[j].position) * ((float)Environment.screenwidth) / 640.f;
				p1 = (objectiveMoveInfo[j + 1].position) * ((float)Environment.screenwidth) / 640.f;
				break;
			}
		}
		// if not done yet
		if (t1)
		{
			float dT = objectiveTime - t0;
			float currentPosition = p0 + dT * ((p1 - p0) / (t1 - t0));
			delta = currentPosition - objectiveInfos[0].location[0].x;
		}
		else
		{
			delta = objectiveMoveInfo[OBJECTVE_MOVE_COUNT - 1].position - objectiveInfos[0].location[0].x;
		}
		// draw little edge pieces
		for (size_t i = 0; i < objectiveInfoCount - 3; i++)
		{
			objectiveInfos[i].move(delta, 0);
			objectiveInfos[i].render();
		}
		if (delta)
		{
			OBJECTIVEHEADERLEFT += delta;
			OBJECTIVESLEFT += delta;
		}
		if (MPlayer && MPlayer->missionSettings.missionType != MISSION_TYPE_OTHER)
		{
			renderPlayerStatus(delta);
			return;
		}
		// draw "Primary"
		wchar_t buffer[256];
		cLoadString(IDS_PRIMARY, buffer, 245);
		drawShadowText(0xffffffff, 0xff000000, guiFont.getTempHandle(), OBJECTIVEHEADERLEFT,
			OBJECTIVESTOP, 1, buffer, 0, guiFont.getSize());
		// draw primary objectives
		uint32_t pos = OBJECTIVESTOP;
		pos += OBJECTIVESSKIP;
		float objectiveCount = -objectiveMoveInfo[OBJECTVE_MOVE_COUNT - 1].time;
		int32_t sum = 0;
		int32_t objectiveNum = 0;
		for (EList<CObjective*, CObjective*>::EIterator iter = Team::home->objectives.Begin();
			 !iter.IsDone(); iter++)
		{
			if (((*iter)->IsActive()) && (!(*iter)->IsHiddenTrigger()) && (1 == (*iter)->Priority()))
			{
				bool bTotal =
					renderStatusInfo && objectiveCount < objectiveTime; // drawing one per second
				renderObjective((*iter), OBJECTIVESLEFT, pos, bTotal);
				if ((*iter)->Status(Team::home->objectives) == OS_SUCCESSFUL && bTotal)
				{
					sum += (*iter)->ResourcePoints();
					if (!playedObjectiveClick[objectiveNum])
					{
						playedObjectiveClick[objectiveNum] = true;
						soundSystem->playDigitalSample(LOG_KILLMARKER);
					}
				}
				pos += OBJECTIVESSKIP;
				objectiveCount += 1.f;
				objectiveNum++;
			}
		}
		pos -= OBJECTIVESSKIP;
		// draw "Secondary"
		cLoadString(IDS_SECONDARY, buffer, 245);
		pos += OBJEECTIVESHEADERSKIP;
		drawShadowText(0xffffffff, 0xff000000, guiFont.getTempHandle(), OBJECTIVEHEADERLEFT, pos, 1,
			buffer, 0, guiFont.getSize());
		// draw secondary objectives
		for (iter = Team::home->objectives.Begin(); !iter.IsDone(); iter++)
		{
			if (((*iter)->IsActive()) && (!(*iter)->IsHiddenTrigger()) && (!(1 == (*iter)->Priority())))
			{
				pos += OBJECTIVESSKIP;
				bool bTotal =
					renderStatusInfo && objectiveCount < objectiveTime; // drawing one per second
				renderObjective((*iter), OBJECTIVESLEFT, pos, bTotal);
				if ((*iter)->Status(Team::home->objectives) == OS_SUCCESSFUL && bTotal)
				{
					sum += (*iter)->ResourcePoints();
					if (!playedObjectiveClick[objectiveNum])
					{
						playedObjectiveClick[objectiveNum] = true;
						soundSystem->playDigitalSample(LOG_KILLMARKER);
					}
				}
				objectiveCount += 1.f;
				objectiveNum++;
			}
		}
		if (renderStatusInfo) // draw the total
		{
			pos += OBJECTIVESSKIP;
			wchar_t total[256];
			wchar_t total2[256];
			cLoadString(IDS_TOTAL, total2, 255);
			sprintf(total, total2, sum);
			uint32_t width = guiFont.width(total);
			drawShadowText(0xffffffff, 0xff000000, guiFont.getTempHandle(),
				OBJECTIVESTOTALRIGHT - width, pos, 0, total, 0, guiFont.getSize());
		}
	}
}

void ControlGui::renderPlayerStatus(float xDelta)
{
	bool scoreShown[MAX_MC_PLAYERS]; // keep track of whose shown the score
	memset(scoreShown, 0, sizeof(bool) * MAX_MC_PLAYERS);
	MPlayer->calcPlayerRanks();
	int32_t playerCount = 0;
	const MC2Player* players = MPlayer->getPlayers(playerCount);
	const MC2Player* sorted[MAX_MC_PLAYERS];
	for (size_t j = 0; j < playerCount; j++)
	{
		sorted[j] = &players[j];
	}
	qsort(sorted, playerCount, sizeof(MC2Player*), sortStats);
	mpStats[0].setData(nullptr, 1); // this is the header...
	for (size_t i = 1; i < MAX_MC_PLAYERS + 1; i++)
	{
		if (i - 1 < playerCount)
		{
			mpStats[i].setData(sorted[i - 1], !scoreShown[sorted[i - 1]->team]);
			scoreShown[sorted[i - 1]->team] = true;
			mpStats[i].showGUIWindow(true);
		}
		else
			mpStats[i].showGUIWindow(0);
	}
	RECT rect = {mpStats[0].left() - 5 + xDelta, mpStats[0].top() - 5,
		mpStats[0].right() + 5 + xDelta, mpStats[0].bottom() + 5};
	for (i = 1; i < 9; i++)
	{
		if (mpStats[i].isShowing())
			rect.bottom += mpStats[0].height() + 4;
	}
	drawRect(rect, 0x7f000000);
	for (i = 0; i < 9; i++)
	{
		mpStats[i].render(xDelta, ((i) * (mpStats[0].height() + 4)));
	}
	/*	int32_t xLocs[4] = {	mpStats[0].getPlayerHeadingX(),
							mpStats[0].getRankingX(),
							mpStats[0].getKillsX(),
							mpStats[0].getLossesX()
						};
		int32_t textIDs[4] = { IDS_MP_STATS_PLAYER_NAME,
							IDS_MP_STATS_SCORE,
							IDS_MP_STATS_KILLS,
							IDS_MP_STATS_LOSSES
							};

		wchar_t text[256];
		for ( i = 0; i < 4; i++ )
		{
			cLoadString( textIDs[i], text, 255 );

			drawShadowText( 0xffffffff, 0xff000000,
	   missionResultsFont.getTempHandle(), xLocs[i] + OBJECTIVEHEADERLEFT,
	   OBJECTIVESTOP, 1, text, 0, 1 );
		}*/
}

void ControlGui::renderObjective(
	CObjective* pObjective, int32_t xPos, int32_t yPos, bool bDrawTotal)
{
	// draw description
	int32_t color = pObjective->Status(Team::home->objectives) == OS_FAILED
		? 0xffc61c00
		: (pObjective->Status(Team::home->objectives) == OS_SUCCESSFUL ? 0xff41c700 : 0xffffc600);
	gosASSERT(pObjective->IsActive());
	if (pObjective->ActivateOnFlag() && (20.0 /*seconds*/ /*arbitrary*/ > (mission->actualTime - pObjective->ActivationTime())))
	{
		/* flash the objective text */
		int32_t tempI = (int32_t)(2.0 /*Hz*/ /*flash frequency*/ * (mission->actualTime - pObjective->ActivationTime()));
		int32_t parity = (tempI % 2);
		if (0 == parity)
		{
			color = 0xffffffff;
		}
	}
	drawShadowText(color, 0xff000000, guiFont.getTempHandle(), xPos, yPos, 1,
		pObjective->LocalizedDescription(), 0, guiFont.getSize());
	if (bDrawTotal)
	{
		/*these used to be declared as uint32_t, but mixing signed and uint32_t
		values in arithmetic expressions can be bad news if any intermediate
		result is negative */
		int32_t width, height;
		int32_t descwidth;
		int32_t dotwidth;
		wchar_t total[64];
		int32_t amount = pObjective->Status(Team::home->objectives) == OS_SUCCESSFUL
			? pObjective->ResourcePoints()
			: 0;
		sprintf(total, "%ld", amount);
		width = guiFont.width(total);
		// draw total
		drawShadowText(0xffffffff, 0xff000000, guiFont.getTempHandle(),
			OBJECTIVESTOTALRIGHT - width, yPos, 0, total, 0, guiFont.getSize());
		// draw .............
		uint32_t tmpULong1, tmpULong2;
		gos_TextStringLength(&tmpULong1, &tmpULong2, pObjective->LocalizedDescription());
		descwidth = (int32_t)tmpULong1;
		height = (int32_t)tmpULong2;
		gos_TextStringLength(&tmpULong1, &tmpULong2, "....................");
		dotwidth = (int32_t)tmpULong1;
		height = (int32_t)tmpULong2;
		gosASSERT(0 < dotwidth);
		float dotRealwidth = (((float)dotwidth) / 20.f);
		float totalLength = (OBJECTIVESTOTALRIGHT - width - xPos - descwidth);
		int32_t numberOfDots = ((float)totalLength) / dotRealwidth;
		if (3 > numberOfDots)
		{
			numberOfDots = 3;
		}
		std::wstring_view dots = (std::wstring_view)_alloca(sizeof(wchar_t) * (numberOfDots + 1));
		for (size_t i = 0; i < numberOfDots - 2; i++)
			dots[i] = '.';
		dots[i] = 0;
		float dotFinalLength = dotRealwidth * (float)(numberOfDots - 2);
		drawShadowText(color, 0xff000000, guiFont.getTempHandle(),
			OBJECTIVESTOTALRIGHT - width - dotFinalLength, yPos, 0, dots, 0, guiFont.getSize());
	}
	// draw little boxes and check marks
	float height = objectiveInfos[objectiveInfoCount - 3].location[2].y - objectiveInfos[objectiveInfoCount - 3].location[0].y;
	objectiveInfos[objectiveInfoCount - 3].location[0].y = yPos + OBJECTIVEBOXSKIP;
	objectiveInfos[objectiveInfoCount - 3].location[3].y = yPos + OBJECTIVEBOXSKIP;
	objectiveInfos[objectiveInfoCount - 3].location[1].y = yPos + OBJECTIVEBOXSKIP + height;
	objectiveInfos[objectiveInfoCount - 3].location[2].y = yPos + OBJECTIVEBOXSKIP + height;
	int32_t x = objectiveInfos[objectiveInfoCount - 3].location[0].x;
	objectiveInfos[objectiveInfoCount - 3].setLocation(x, yPos + OBJECTIVEBOXSKIP);
	objectiveInfos[objectiveInfoCount - 3].render();
	if (pObjective->Status(Team::home->objectives) == OS_FAILED)
	{
		float height = objectiveInfos[objectiveInfoCount - 1].location[2].y - objectiveInfos[objectiveInfoCount - 1].location[0].y;
		objectiveInfos[objectiveInfoCount - 1].location[0].y = yPos + OBJECTIVECHECKSKIP;
		objectiveInfos[objectiveInfoCount - 1].location[3].y = yPos + OBJECTIVECHECKSKIP;
		objectiveInfos[objectiveInfoCount - 1].location[1].y = yPos + OBJECTIVECHECKSKIP + height;
		objectiveInfos[objectiveInfoCount - 1].location[2].y = yPos + OBJECTIVECHECKSKIP + height;
		int32_t x = objectiveInfos[objectiveInfoCount - 1].location[0].x;
		objectiveInfos[objectiveInfoCount - 1].setLocation(x, yPos + OBJECTIVECHECKSKIP);
		objectiveInfos[objectiveInfoCount - 1].render();
	}
	else if (pObjective->Status(Team::home->objectives) == OS_SUCCESSFUL)
	{
		float height = objectiveInfos[objectiveInfoCount - 2].location[2].y - objectiveInfos[objectiveInfoCount - 2].location[0].y;
		objectiveInfos[objectiveInfoCount - 2].location[0].y = yPos + OBJECTIVECHECKSKIP;
		objectiveInfos[objectiveInfoCount - 2].location[3].y = yPos + OBJECTIVECHECKSKIP;
		objectiveInfos[objectiveInfoCount - 2].location[1].y = yPos + OBJECTIVECHECKSKIP + height;
		objectiveInfos[objectiveInfoCount - 2].location[2].y = yPos + OBJECTIVECHECKSKIP + height;
		int32_t x = objectiveInfos[objectiveInfoCount - 2].location[0].x;
		objectiveInfos[objectiveInfoCount - 2].setLocation(x, yPos + OBJECTIVECHECKSKIP);
		objectiveInfos[objectiveInfoCount - 2].render();
	}
}

void ControlGui::update(bool bPaused, bool bLOS)
{
	mouseInVehicleStopButton = 0;
	bool bMouseInButton = 0;
	showServerMissing();
	if (bPaused)
		pauseWnd->update();
	if (moviePlaying && bMovie)
	{
		bool result = bMovie->update();
		if (result)
		{
			moviePlaying = false;
			delete bMovie;
			bMovie = nullptr;
		}
	}
	// Just update the movie.  DO not update anything else if in movieMode!!
	if (eye && eye->inMovieMode)
		return;
	if (idToUnPress != -1)
	{
		getButton(idToUnPress)->press(false);
	}
	float mouseX = userInput->getMouseX();
	float mouseY = userInput->getMouseY();
	// also going to initialize buttons here
	for (size_t i = LAST_COMMAND - 1; i > -1; i--)
	{
		if (buttons[i].location[0].x <= mouseX && mouseX <= buttons[i].location[2].x && mouseY >= buttons[i].location[0].y && mouseY <= buttons[i].location[1].y)
		{
			if (buttons[i].state != ControlButton::HIDDEN)
			{
				helpTextHeaderID = buttonData[i].helpTextHeader;
				helpTextID = buttonData[i].helpTextID;
				bMouseInButton = true;
				int32_t lastX = mouseX - userInput->getMouseXDelta();
				int32_t lastY = mouseY - userInput->getMouseYDelta();
				if (buttons[i].location[0].x > lastX || lastX > buttons[i].location[2].x || lastY < buttons[i].location[0].y || lastY > buttons[i].location[1].y)
				{
					soundSystem->playDigitalSample(LOG_HIGHLIGHTBUTTONS);
				}
			}
			if (userInput->leftMouseReleased() && !userInput->isLeftDrag() && !userInput->wasLeftDrag())
			{
				{
					handleClick(buttons[i].ID);
				}
			}
		}
	}
	if ((buttonToPress != -1) && getButton(buttonToPress))
	{
		getButton(buttonToPress)->press(true);
		buttonToPress = -1;
	}
	// update all the attack ranges
	Team* pTeam = Team::home;
	int32_t bMover = 0;
	bool bJump = 1;
	// if no mover is selected, disable range buttons
	for (i = DEFAULT_RANGE; i < STOP_COMMAND; i++)
	{
		buttons[i].disable(false);
	}
	bool bMineLayer = 1;
	bool bSalvage = 1;
	bool bRepair = 1;
	bool bGuardTower = 1;
	bool bPressGuardTower = 1;
	Mover* pSelectedMover = 0;
	int32_t holdPositionCount = 0;
	for (i = 0; i < pTeam->getRosterSize(); ++i)
	{
		Mover* pMover = (Mover*)pTeam->getMover(i);
		if (pMover->isSelected() && pMover->getCommander()->getId() == Commander::home->getId())
		{
			//	if ( !isSelectingebject() )
			{
				bMineLayer &= ((pMover->isMineLayer()) ? 1 : 0);
				bSalvage &= (pMover->isRecover() ? 1 : 0);
				bRepair &= (pMover->isRefit() && pMover->getRefitPoints() ? 1 : 0);
				bGuardTower &= (pMover->maxMoveSpeed ? 0 : 1);
				if (bGuardTower)
					bPressGuardTower &= pMover->suppressionFire;
				if (pMover->getPilot()->getNumTacOrdersQueued())
					getButton(STOP_COMMAND)->disable(false);
				switch (pMover->attackRange)
				{
				case FireRangeType::shortest:
					getButton(SHORT_RANGE)->press(true);
					break;
				case FireRangeType::extended:
					getButton(LONG_RANGE)->press(true);
					break;
				case FireRangeType::medium:
					getButton(MED_RANGE)->press(true);
					break;
				case FireRangeType::optimal:
					getButton(DEFAULT_RANGE)->press(true);
					break;
				case FireRangeType::current:
					holdPositionCount++;
					break;
				}
				bJump &= pMover->canJump();
				pSelectedMover = pMover;
				bMover++;
			}
		}
	}
	if (!holdPositionCount)
	{
		getButton(CUR_RANGE)->press(0);
	}
	else if (holdPositionCount != bMover)
	{
		getButton(CUR_RANGE)->makeAmbiguous(true);
	}
	else
		getButton(CUR_RANGE)->press(1);
	int32_t rangeCount = 0;
	for (i = 0; i < 4; i++)
	{
		if (getButton(DEFAULT_RANGE + i)->state & ControlButton::PRESSED)
		{
			rangeCount++;
		}
	}
	if (rangeCount > 1)
	{
		for (i = 0; i < 4; i++)
		{
			if (getButton(DEFAULT_RANGE + i)->state & ControlButton::PRESSED)
			{
				getButton(DEFAULT_RANGE + i)->makeAmbiguous(true);
			}
		}
	}
	// if no mover is selected, disable range buttons
	if (!bMover)
	{
		for (i = DEFAULT_RANGE; i < STOP_COMMAND; i++)
			buttons[i].disable(true);
	}
	else
	{
		getButton(STOP_COMMAND)->disable(false);
	}
	if (bMover && bMineLayer)
	{
		if (!bMouseInButton)
			setRolloverHelpText(IDS_MINELAYER_HELP);
		getButton(LAYMINES)->disable(0);
		getButton(SALVAGE)->hide(1);
		getButton(REPAIR)->hide(1);
		getButton(GUARDTOWER)->hide(1);
	}
	else if (bMover && bSalvage)
	{
		if (!bMouseInButton)
			setRolloverHelpText(IDS_RECOVER_HELP);
		getButton(SALVAGE)->disable(0);
		getButton(SALVAGE)->press(1);
		getButton(LAYMINES)->hide(1);
		getButton(REPAIR)->hide(1);
		getButton(GUARDTOWER)->hide(1);
		wasLayingMines = 0;
	}
	else if (bMover && bRepair)
	{
		if (!bMouseInButton)
			setRolloverHelpText(IDS_REPAIR_HELP);
		getButton(REPAIR)->disable(0);
		getButton(REPAIR)->press(1);
		getButton(LAYMINES)->hide(1);
		getButton(SALVAGE)->hide(1);
		getButton(GUARDTOWER)->hide(1);
		wasLayingMines = 0;
	}
	else if (bMover && bGuardTower)
	{
		getButton(REPAIR)->hide(1);
		getButton(LAYMINES)->hide(1);
		getButton(SALVAGE)->hide(1);
		getButton(GUARDTOWER)->hide(0);
		if (bPressGuardTower)
			getButton(GUARDTOWER)->press(1);
		else
			getButton(GUARDTOWER)->press(0);
	}
	else
	{
		getButton(LAYMINES)->hide(1);
		getButton(REPAIR)->hide(1);
		getButton(SALVAGE)->hide(1);
		getButton(GUARDTOWER)->hide(1);
		wasLayingMines = 0;
	}
	if (bMover && bJump)
		getButton(JUMP_COMMAND)->disable(false);
	else
		getButton(JUMP_COMMAND)->disable(true);
	// upate all the orders
	if (curOrder != WALK && bMover && !isSelectingInfoObject())
	{
		if (curOrder == JUMP)
			getButton(JUMP_COMMAND)->press(true);
		else if (curOrder == GUARD)
			getButton(GUARD_COMMAND)->press(true);
		else if (curOrder == RUN)
			getButton(RUN_COMMAND)->press(true);
	}
	if (fireFromCurrentPos && bMover)
		getButton(FIRE_FROM_CURRENT_POS)->press(true);
	else
		getButton(FIRE_FROM_CURRENT_POS)->press(false);
	bool bMouseInsideTacArea = 0;
	if (rectInfos[0].rect.left <= userInput->getMouseX() && rectInfos[0].rect.right >= userInput->getMouseX() && rectInfos[0].rect.top <= userInput->getMouseY() && rectInfos[0].rect.bottom >= userInput->getMouseY())
	{
		bMouseInsideTacArea = true;
	}
	if (getButton(TACMAP_TAB)->state & ControlButton::PRESSED)
	{
		tacMap.update();
		if (bMouseInsideTacArea)
		{
			helpTextHeaderID = IDS_TACMAP;
			helpTextID = IDS_TACMAP_DESC;
		}
	}
	else if (getButton(INFO_TAB)->state & ControlButton::PRESSED)
	{
		infoWnd->update();
		if (bMouseInsideTacArea)
		{
			helpTextHeaderID = IDS_INFOTAB;
			helpTextID = IDS_INFOTAB_DESC;
		}
	}
	else if (bMouseInsideTacArea)
	{
		helpTextHeaderID = IDS_VEHICLE_TAB;
		helpTextID = IDS_VEHICLE_TAB_DESC;
	}
	updateVehicleTab(mouseX, mouseY, bLOS);
	if (renderObjectives)
		getButton(OBJECTIVES_COMMAND)->press(true);
	else
		getButton(OBJECTIVES_COMMAND)->press(false);
	forceGroupBar.update();
	getButton(DEFAULT_RANGE)->hide(true);
	getButton(SHORT_RANGE)->hide(true);
	getButton(MED_RANGE)->hide(true);
	getButton(LONG_RANGE)->hide(true);
}

bool ControlGui::isOverTacMap()
{
	bool bMouseInsideTacArea = 0;
	if (rectInfos[0].rect.left <= userInput->getMouseX() && rectInfos[0].rect.right >= userInput->getMouseX() && rectInfos[0].rect.top <= userInput->getMouseY() && rectInfos[0].rect.bottom >= userInput->getMouseY())
	{
		bMouseInsideTacArea = true;
	}
	if (getButton(TACMAP_TAB)->state & ControlButton::PRESSED)
		return true;
	return false;
}

bool ControlGui::inRegion(int32_t mouseX, int32_t mouseY, bool bPaused)
{
	for (size_t i = 0; i < LAST_COMMAND; i++)
	{
		if ((buttons[i].location[0].x) <= mouseX && buttons[i].location[3].x >= mouseX && buttons[i].location[0].y <= mouseY && buttons[i].location[1].y >= mouseY)
			return true;
	}
	for (i = 0; i < staticCount; i++)
	{
		if (staticInfos[i].isInside(mouseX, mouseY))
			return true;
	}
	for (i = 0; i < rectCount; i++)
	{
		if ((rectInfos[i].rect.left) <= mouseX && rectInfos[i].rect.right >= mouseX && rectInfos[i].rect.top <= mouseY && rectInfos[i].rect.bottom >= mouseY)
			return true;
	}
	if (bPaused && pauseWnd->inRect(mouseX, mouseY))
		return true;
	return false;
}

void ControlGui::addMover(std::unique_ptr<Mover> mover)
{
	if (mover->getCommanderId() == Commander::home->getId())
	{
		if (turn > 3)
			mover->removeFromUnitGroup(1);
		if (mover->getObjectType()->getObjectTypeClass() == BATTLEMECH_TYPE)
			forceGroupBar.addMech(mover);
		else // if ( mover->getSpeed() > 0 ) // don't add guard towers
			forceGroupBar.addVehicle(mover);
	}
}

void ControlGui::removeMover(std::unique_ptr<Mover> mover)
{
	if (mover->getCommanderId() == Commander::home->getId())
		forceGroupBar.removeMover(mover);
}

void ControlGui::initMechs()
{
	forceGroupBar.removeAll();
	Team* pTeam = Team::home;
	if (!pTeam)
		return;
	for (size_t i = 0; i < pTeam->getRosterSize(); ++i)
	{
		Mover* pMover = pTeam->getMover(i);
		if (pMover && pMover->getExists() && pMover->isOnGUI())
		{
			if (pMover->getCommander()->getId() == Commander::home->getId())
			{
				pMover->setSelected(1);
				addMover(pMover);
			}
		}
	}
	if (MPlayer)
	{
		mpStats[0].init();
		for (size_t i = 0; i < 8; i++)
		{
			mpStats[i + 1] = mpStats[i];
		}
	}
}

bool ControlGui::getRun()
{
	return curOrder == RUN;
}
void ControlGui::toggleJump()
{
	curOrder ^= JUMP;
	curOrder &= JUMP;
	getButton(JUMP_COMMAND)->press(curOrder & JUMP ? true : false);
	getButton(RUN_COMMAND)->press(curOrder & RUN ? true : false);
	getButton(GUARD_COMMAND)->press(curOrder & GUARD ? true : false);
}
bool ControlGui::getJump()
{
	return curOrder == JUMP;
}
void ControlGui::toggleDefaultSpeed()
{
	curOrder ^= RUN;
	curOrder &= (RUN | GUARD);
	getButton(RUN_COMMAND)->press(curOrder & RUN ? true : false);
	getButton(GUARD_COMMAND)->press(curOrder & GUARD ? true : false);
	getButton(JUMP_COMMAND)->press(curOrder & JUMP ? true : false);
}

void ControlGui::setDefaultSpeed()
{
	curOrder = 0;
	getButton(JUMP_COMMAND)->press(0);
	getButton(GUARD_COMMAND)->press(0);
	getButton(RUN_COMMAND)->press(0);
	getButton(REPAIR)->press(0);
	getButton(SALVAGE)->press(0);
	getButton(INFO_COMMAND)->press(0);
}
bool ControlGui::isDefaultSpeed()
{
	return getWalk();
}
bool ControlGui::getWalk()
{
	return curOrder == WALK;
}
void ControlGui::toggleGuard()
{
	curOrder ^= GUARD;
	curOrder &= (GUARD | RUN); // can run and guard
	getButton(GUARD_COMMAND)->press(curOrder & GUARD ? true : false);
	getButton(JUMP_COMMAND)->press(curOrder & JUMP ? true : false);
	getButton(RUN_COMMAND)->press(curOrder & RUN ? true : false);
}
bool ControlGui::getGuard()
{
	return curOrder == GUARD;
}

void ControlGui::setRange(FireRangeType Range)
{
	// apply to all selected mechs
	Team* pTeam = Team::home;
	for (size_t i = 0; i < pTeam->getRosterSize(); ++i)
	{
		Mover* pMover = pTeam->getMover(i);
		if (pMover->isSelected() && pMover->getCommander()->getId() == Commander::home->getId())
		{
			pMover->attackRange = Range;
			// only want to apply to future order
			//			pMover->getPilot()->changeAttackRange( Range );
		}
	}
}

FireRangeType ControlGui::getCurrentRange(void)
{
	FireRangeType range = FireRangeType::none;
	Team* pTeam = Team::home;
	for (auto i = 0; i < pTeam->getRosterSize(); ++i)
	{
		Mover* pMover = pTeam->getMover(i);
		if (pMover->isSelected() && pMover->getCommander()->getId() == Commander::home->getId())
		{
			if (range != FireRangeType::none && range != pMover->attackRange)
			{
				return FireRangeType::optimal;
			}
			else
				range = pMover->attackRange;
		}
	}
	return range;
}

void ControlButton::render()
{
	if (state != HIDDEN)
	{
		uint32_t gosID = mcTextureManager->get_gosTextureHandle(data->textureHandle);
		gos_SetRenderState(gos_State_Texture, gosID);
		gos_SetRenderState(gos_State_AlphaMode, gos_Alpha_AlphaInvAlpha);
		gos_SetRenderState(gos_State_Filter, gos_FilterNone);
		gos_SetRenderState(gos_State_AlphaTest, true);
		gos_SetRenderState(gos_State_TextureAddress, gos_TextureClamp);
		gos_DrawQuads(location, 4);
		if (data->textID)
		{
			wchar_t buffer[256];
			cLoadString(data->textID, buffer, 256);
			uint32_t height = data->textFont.height();
			data->textFont.render(buffer, location[0].x,
				(location[0].y + location[2].y) / 2 - height / 2 + 1, location[2].x - location[0].x,
				location[2].y - location[0].y, data->textcolours[state], 0, 2);
		}
	}
}

void ControlButton::press(bool bPress)
{
	if (!isEnabled())
		return;
	state = bPress ? PRESSED : ENABLED;
	ControlButton::makeUVs(location, state, *data);
}

void ControlButton::makeAmbiguous(bool bAmbiguous)
{
	state = bAmbiguous ? AMBIGUOUS : ENABLED;
	ControlButton::makeUVs(location, state, *data);
}

void ControlButton::disable(bool bDisable)
{
	state = (bDisable ? DISABLED : (state == PRESSED ? PRESSED : ENABLED));
	ControlButton::makeUVs(location, state, *data);
}

void ControlButton::hide(bool bHide)
{
	if (!bHide && state != HIDDEN)
		return;
	state = bHide ? HIDDEN : ENABLED;
	if (state != HIDDEN)
		ControlButton::makeUVs(location, state, *data);
}

bool ControlButton::isEnabled()
{
	return state == ENABLED || state == PRESSED || state == AMBIGUOUS;
}

void ControlButton::move(float deltaX, float deltaY)
{
	for (size_t i = 0; i < 4; i++)
	{
		location[i].x += deltaX;
		location[i].y += deltaY;
	}
}

ControlButton*
ControlGui::getButton(int32_t ID)
{
	for (size_t i = 0; i < LAST_COMMAND; i++)
	{
		if (buttons[i].ID == ID)
			return &buttons[i];
	}
	for (i = 0; i < LAST_VEHICLE; i++)
	{
		if (vehicleButtons[i].ID == ID)
			return &vehicleButtons[i];
	}
	return nullptr;
}

void ControlGui::handleClick(int32_t ID)
{
	if (getButton(ID)->state & ControlButton::HIDDEN)
		return;
	if (!getButton(ID)->isEnabled())
	{
		// need to play sound here
		soundSystem->playDigitalSample(INVALID_GUI);
		return;
	}
	int32_t i;
	int32_t sound = LOG_SELECT;
	switch (ID)
	{
	case SAVE_COMMAND:
		mission->missionInterface->togglePause();
		break;
	case SHORT_RANGE:
		setRange(FireRangeType::shortest);
		idToUnPress = ID;
		getButton(ID)->press(true);
		break;
	case MED_RANGE:
		setRange(FireRangeType::medium);
		idToUnPress = ID;
		getButton(ID)->press(true);
		break;
	case LONG_RANGE:
		setRange(FireRangeType::extended);
		idToUnPress = ID;
		getButton(ID)->press(true);
		break;
	case CUR_RANGE:
		toggleHoldPosition();
		break;
	case DEFAULT_RANGE:
		setRange(FireRangeType::optimal);
		idToUnPress = ID;
		getButton(ID)->press(true);
		break;
	case JUMP_COMMAND:
		toggleJump();
		break;
	case RUN_COMMAND:
		toggleDefaultSpeed();
		break;
	case GUARD_COMMAND:
		toggleGuard();
		break;
	case STOP_COMMAND:
		idToUnPress = ID;
		getButton(ID)->press(true);
		doStop();
		break;
	case FIRE_FROM_CURRENT_POS:
		toggleFireFromCurrentPos();
		break;
	case TACMAP_TAB:
	case INFO_TAB:
	case VEHICLE_TAB:
		for (i = TACMAP_TAB; i < TACMAP_TAB + 3; i++)
		{
			getButton(i)->press(false);
		}
		getButton(ID)->press(true);
		if (ID == VEHICLE_TAB)
			LogisticsData::instance->rpJustAdded = 0;
		break;
	case OBJECTIVES_COMMAND:
		if (!renderObjectives)
		{
			startObjectives(1);
			getButton(ID)->press(true);
		}
		else
			renderObjectives = 0;
		sound = LOG_CLICKONBUTTON;
		break;
	case LAYMINES:
	case REPAIR:
	case SALVAGE:
		getButton(ID)->toggle();
		break;
	case GUARDTOWER:
	{
		getButton(ID)->toggle();
		bool bPress = getButton(ID)->state == ControlButton::PRESSED ? 1 : 0;
		for (size_t i = 0; i < Team::home->getRosterSize(); i++)
		{
			Mover* pMover = (Mover*)Team::home->getMover(i);
			if (pMover->isSelected() && pMover->isGuardTower() && pMover->getCommander()->getId() == Commander::home->getId())
			{
				pMover->suppressionFire = bPress;
				if (!bPress && MPlayer && !MPlayer->isServer())
				{
					// We send a TAC_ORDER_WAIT which will clear the suppression
					// fire message.
					TacticalOrder tacOrder;
					tacOrder.init(OrderOriginType::player, TacticalOrderCode::wait);
					MPlayer->sendPlayerOrder(&tacOrder, false, 1, &pMover);
				}
			}
		}
	}
	break;
	case INFO_COMMAND:
		for (i = JUMP_COMMAND; i < INFO_COMMAND; i++)
		{
			getButton(i)->press(false);
		}
		getButton(ID)->toggle();
		break;
	default:
		sound = -1;
		break;
	}
	soundSystem->playDigitalSample(sound);
}

void ControlGui::doStop()
{
	TacticalOrder tacOrder;
	tacOrder.init(OrderOriginType::player, TacticalOrderCode::stop);
	soundSystem->playDigitalSample(BUTTON5);
	Team* pTeam = Team::home;
	for (size_t i = 0; i < pTeam->getRosterSize(); i++)
	{
		Mover* pMover = (Mover*)pTeam->getMover(i);
		if (pMover->isSelected() && pMover->getCommander()->getId() == Commander::home->getId())
		{
			tacOrder.attackParams.range = (FireRangeType)pMover->attackRange;
			tacOrder.pack(nullptr, nullptr);
			//---------------------------------------------------------------------
			// Helper function--perhaps this should just be a part of the mover
			// and group handleTacticalOrder() functions?
			if (MPlayer && !MPlayer->isServer())
				MPlayer->sendPlayerOrder(&tacOrder, false, 1, &pMover);
			else
				pMover->handleTacticalOrder(tacOrder);
		}
	}
	if (getMines())
		getButton(LAYMINES)->press(0);
	if (addingArtillery || addingVehicle || addingSalvage)
	{
		handleVehicleClick(STOP_VEHICLE);
		LogisticsData::instance->rpJustAdded = 0;
	}
	if (getFireFromCurrentPos())
		toggleFireFromCurrentPos();
	setDefaultSpeed();
}

void ControlGui::toggleFireFromCurrentPos()
{
	fireFromCurrentPos = !fireFromCurrentPos;
}

void ControlButton::makeUVs(gos_VERTEX* vertices, int32_t State, ButtonData& data)
{
	float left = data.stateCoords[State][0];
	float top = data.stateCoords[State][1];
	if (left == -1 || top == -1)
	{
		SPEW((0, "makeUVs given an Invalid state\n"));
	}
	float width = data.texturewidth;
	float height = data.textureheight;
	float right = left + width;
	float bottom = top + height;
	if (data.filewidth && data.fileheight) // will crash if 0
	{
		if (data.textureRotated)
		{
			vertices[0].u = right / (float)data.filewidth + (.1f / (float)data.filewidth);
			;
			vertices[1].u = left / (float)data.filewidth + (.1f / (float)data.filewidth);
			;
			vertices[2].u = left / (float)data.filewidth + (.1f / (float)data.filewidth);
			vertices[3].u = right / (float)data.filewidth + (.1f / (float)data.filewidth);
			vertices[0].v = top / (float)data.fileheight + (.1f / (float)data.filewidth);
			;
			vertices[1].v = top / (float)data.fileheight + (.1f / (float)data.filewidth);
			;
			vertices[2].v = bottom / (float)data.fileheight + (.1f / (float)data.fileheight);
			;
			vertices[3].v = bottom / (float)data.fileheight + (.1f / (float)data.fileheight);
			;
		}
		else
		{
			{
				vertices[0].u = vertices[1].u =
					left / (float)data.filewidth + (.1f / (float)data.filewidth);
				;
				vertices[2].u = vertices[3].u =
					right / (float)data.filewidth + (.1f / (float)data.filewidth);
				vertices[0].v = vertices[3].v =
					top / (float)data.fileheight + (.1f / (float)data.filewidth);
				;
				vertices[1].v = vertices[2].v =
					bottom / (float)data.fileheight + (.1f / (float)data.fileheight);
			}
		}
	}
}

void ControlGui::updateVehicleTab(int32_t mouseX, int32_t mouseY, bool bLOS)
{
	for (size_t i = LAST_VEHICLE - 1; i > -1; i--)
	{
		if (getButton(VEHICLE_TAB)->state & ControlButton::PRESSED)
		{
			if (vehicleButtons[i].state & ControlButton::PRESSED)
			{
				setRolloverHelpText(IDS_AIRSTRIKE_HELP_NO_LOS + 2 * (i));
				if (i == 0 && bLOS)
				{
					setRolloverHelpText(IDS_AIRSTRIKE_HELP);
				}
			}
			if (vehicleButtons[i].location[0].x <= mouseX && mouseX <= vehicleButtons[i].location[2].x && mouseY >= vehicleButtons[i].location[0].y && mouseY <= vehicleButtons[i].location[1].y)
			{
				helpTextHeaderID = vehicleData[i].helpTextHeader;
				helpTextID = vehicleData[i].helpTextID;
				if (vehicleButtons[i].ID == STOP_VEHICLE)
				{
					mouseInVehicleStopButton = true;
				}
				if (userInput->leftMouseReleased() && !userInput->isLeftDrag() && !userInput->wasLeftDrag() && !(vehicleButtons[i].state & ControlButton::PRESSED))
				{
					{
						handleVehicleClick(vehicleButtons[i].ID);
					}
				}
				int32_t lastX = mouseX - userInput->getMouseXDelta();
				int32_t lastY = mouseY - userInput->getMouseYDelta();
				if (vehicleButtons[i].location[0].x >= lastX || lastX >= vehicleButtons[i].location[2].x || lastY <= vehicleButtons[i].location[0].y || lastY >= vehicleButtons[i].location[1].y)
					soundSystem->playDigitalSample(LOG_HIGHLIGHTBUTTONS);
			}
		}
		if (vehicleButtons[i].ID != STOP_VEHICLE)
		{
			if (vehicleCosts[i] > LogisticsData::instance->getResourcePoints() && (!addingVehicle && !(vehicleButtons[i].state & ControlButton::PRESSED)))
			{
				vehicleButtons[i].disable(true);
			}
			else if ((addingArtillery || addingVehicle || addingSalvage) && !(vehicleButtons[i].state & ControlButton::PRESSED))
				vehicleButtons[i].disable(true);
			else
				vehicleButtons[i].disable(false);
		}
		else
			vehicleButtons[i].disable(false);
	}
	int32_t maxUnits = MAX_ICONS;
	if (MPlayer)
	{
		int32_t playerCount;
		MPlayer->getPlayers(playerCount);
		if (playerCount)
			maxUnits = (MAX_MULTIPLAYER_MECHS_IN_LOGISTICS / playerCount) + 4;
		else
			maxUnits = 0;
		if (maxUnits > MAX_ICONS)
			maxUnits = MAX_ICONS;
	}
	if (forceGroupBar.getIconCount() >= maxUnits)
	{
		getButton(REPAIR_VEHICLE)->disable(true);
		getButton(PEGASUS_SCOUT)->disable(true);
		getButton(RECOVERY_TEAM)->disable(true);
		getButton(MINELAYER)->disable(true);
		getButton(GUARD_TOWER)->disable(true);
	}
	if (LogisticsData::instance && !LogisticsData::instance->gotPilotsLeft())
		getButton(RECOVERY_TEAM)->disable(true);
	if (LogisticsData::instance) // turn off things that aren't allowed in
		// mplayer
	{
		bool disableAll = (MPlayer && MPlayer->allUnitsDestroyed[MPlayer->commanderid]);
		if (disableAll)
		{
			getButton(LARGE_AIRSTRIKE)->disable(true);
			getButton(GUARD_TOWER)->disable(true);
			getButton(SENSOR_PROBE)->disable(true);
			getButton(REPAIR_VEHICLE)->disable(true);
			getButton(PEGASUS_SCOUT)->disable(true);
			getButton(MINELAYER)->disable(true);
			getButton(RECOVERY_TEAM)->disable(true);
		}
		else
		{
			if (!LogisticsData::instance->canHaveAirStrike())
				getButton(LARGE_AIRSTRIKE)->disable(true);
			if (!LogisticsData::instance->canHaveArtilleryPiece())
				getButton(GUARD_TOWER)->disable(true);
			if (!LogisticsData::instance->canHaveSensorStrike())
				getButton(SENSOR_PROBE)->disable(true);
			if (!LogisticsData::instance->canHaveRepairTruck())
				getButton(REPAIR_VEHICLE)->disable(true);
			if (!LogisticsData::instance->canHaveScoutCopter())
				getButton(PEGASUS_SCOUT)->disable(true);
			if (!LogisticsData::instance->canHaveMineLayer())
				getButton(MINELAYER)->disable(true);
			if (!LogisticsData::instance->canHaveSalavageCraft())
				getButton(RECOVERY_TEAM)->disable(true);
		}
	}
}
void ControlGui::renderVehicleTab()
{
	int32_t cost = -1;
	wchar_t buffer[256];
	LogisticsData::instance->rpJustAdded = 0;
	for (size_t i = 0; i < LAST_VEHICLE; i++)
	{
		vehicleButtons[i].render();
		int32_t color = vehicleButtons[i].isEnabled() ? 0xff005392 : 0xff7f7f7f;
		if (vehicleButtons[i].state & ControlButton::PRESSED)
		{
			ControlButton::makeUVs(
				vehicleButtons[i].location, ControlButton::ENABLED, *vehicleButtons[i].data);
			cost = vehicleCosts[i];
			RECT rect = {vehicleButtons[i].location[0].x, vehicleButtons[i].location[0].y,
				vehicleButtons[i].location[2].x, vehicleButtons[i].location[2].y};
			vehicleButtons[i].render();
			drawEmptyRect(rect, 0xffffffff, 0xffffffff);
			color = 0xffffffff;
		}
		if (vehicleCosts[i])
		{
			// need to draw cost under the button
			int32_t left = float2long(vehicleButtons[i].location[0].x);
			int32_t top = float2long(vehicleButtons[i].location[2].y + TEXT_SKIP);
			int32_t right = float2long(vehicleButtons[i].location[2].x);
			int32_t bottom = float2long(vehicleButtons[i].location[2].y + 30 + TEXT_SKIP);
			sprintf(buffer, "%ld", vehicleCosts[i]);
			vehicleFont.render(buffer, left, top, right - left, bottom - top, color, 0, 2);
		}
	}
	int32_t left = RPLEFT;
	int32_t top = RPTOP;
	// draw in RP
	wchar_t originalStr[256];
	cLoadString(IDS_RP, originalStr, 256);
	sprintf(buffer, originalStr, (int32_t)LogisticsData::instance->getResourcePoints());
	// We are there.  Start flashing.
	/// TUTORIAL!!!
	if (rpNumFlashes)
	{
		rpFlashTime += frameLength;
		if (rpFlashTime > .5f)
		{
			rpFlashTime = 0.0f;
			rpNumFlashes--;
		}
		else if (rpFlashTime > .25f)
		{
			vehicleFont.render(buffer, left, top, 0, 0, 0xff5c96c2, 0, 0);
		}
	}
	else
	{
		vehicleFont.render(buffer, left, top, 0, 0, 0xff5c96c2, 0, 0);
	}
}

void ControlGui::handleVehicleClick(int32_t ID)
{
	if (getButton(ID)->state & ControlButton::HIDDEN)
		return;
	if (getButton(ID)->state & ControlButton::DISABLED) // ignore disabled button
	{
		// need to play sound here
		soundSystem->playDigitalSample(INVALID_GUI);
		return;
	}
	if ((ID == STOP_VEHICLE) && paintingMyVtol)
	{
		// You cannot stop it once the VTOL is in flight, no matter what the
		// button looks like.
		soundSystem->playDigitalSample(INVALID_GUI);
		return;
	}
	int32_t oldID = -1;
	for (size_t i = 0; i < LAST_VEHICLE; i++)
	{
		if (vehicleButtons[i].state & ControlButton::PRESSED)
			oldID = vehicleButtons[i].ID;
		if (vehicleButtons[i].ID != ID)
			vehicleButtons[i].disable(1);
		else
		{
			vehicleButtons[i].press(1);
			if (ID != STOP_VEHICLE)
			{
				int32_t cost = vehicleCosts[i];
				LogisticsData::instance->decrementResourcePoints(cost);
				if (MPlayer)
				{
					Stuff::Vector3D pos;
					MPlayer->sendReinforcement(-cost, 0, "noname", MPlayer->commanderid, pos, 6);
				}
			}
		}
	}
	soundSystem->playDigitalSample(LOG_SELECT);
	switch (ID)
	{
	case LARGE_AIRSTRIKE:
	case SENSOR_PROBE:
		addingArtillery = true;
		getButton(INFO_COMMAND)->press(0);
		break;
	case REPAIR_VEHICLE:
	case PEGASUS_SCOUT:
	case MINELAYER:
	case GUARD_TOWER:
		addingVehicle = true;
		getButton(INFO_COMMAND)->press(0);
		break;
	case RECOVERY_TEAM:
		addingSalvage = true;
		getButton(INFO_COMMAND)->press(0);
		break;
	case STOP_VEHICLE:
		addingArtillery = addingVehicle = addingSalvage = false;
		for (i = 0; i < LAST_VEHICLE - 1; i++)
		{
			if (vehicleCosts[i] <= LogisticsData::instance->getResourcePoints())
			{
				vehicleButtons[i].disable(0);
				vehicleButtons[i].press(0);
			}
		}
		if (oldID != -1)
		{
			int32_t cost = vehicleCosts[oldID - LARGE_AIRSTRIKE];
			LogisticsData::instance->addResourcePoints(cost);
			if (MPlayer)
			{
				MPlayer->playerInfo[MPlayer->commanderid].resourcePoints += cost;
				MPlayer->playerInfo[MPlayer->commanderid].resourcePointsGained += cost;
				if (!MPlayer->isHost())
				{
					MPlayer->sendPlayerUpdate(0, 5, MPlayer->commanderid);
				}
			}
		}
		idToUnPress = ID;
		break;
	default:
		NODEFAULT;
		break;
	}
}

void ControlGui::unPressAllVehicleButtons()
{
	for (size_t i = LARGE_AIRSTRIKE; i < MAX_VEHICLE; i++)
	{
		getButton(i)->press(0);
	}
	addingArtillery = 0;
	addingVehicle = 0;
	addingSalvage = 0;
}

void ControlGui::disableAllVehicleButtons()
{
	for (size_t i = LARGE_AIRSTRIKE; i < MAX_VEHICLE - 1; i++)
	{
		getButton(i)->disable(1);
	}
	//	addingArtillery = 0;
	//	addingVehicle = 0;
}

std::wstring_view
ControlGui::getVehicleName(int32_t& ID)
{
	for (size_t i = GUARD_TOWER; i <= RECOVERY_TEAM; i++)
	{
		if (getButton(i)->state & ControlButton::PRESSED)
		{
			if (i == GUARD_TOWER)
			{
				ID = vehicleIDs[4];
				return vehiclePilots[4];
			}
			ID = vehicleIDs[i - REPAIR_VEHICLE];
			return vehiclePilots[i - REPAIR_VEHICLE];
		}
	}
	//	wchar_t s[1024];
	//	sprintf(s, "getvehiclename: ID = %d", ID);
	//	PAUSE((s));
	return 0;
}

std::wstring_view
ControlGui::getVehicleNameFromID(int32_t ID)
{
	for (size_t i = 0; i < 5; i++)
		if (vehicleIDs[i] == ID)
			return (vehiclePilots[i]);
	return (nullptr);
}

bool ControlGui::getMines()
{
	if (getButton(LAYMINES)->state & ControlButton::PRESSED)
		return 1;
	return 0;
}

bool ControlGui::getSalvage()
{
	if (getButton(SALVAGE)->state & ControlButton::PRESSED)
		return 1;
	return 0;
}
bool ControlGui::getRepair()
{
	if (getButton(REPAIR)->state & ControlButton::PRESSED)
		return 1;
	return 0;
}

bool ControlGui::getGuardTower()
{
	if (getButton(GUARDTOWER)->state & ControlButton::PRESSED)
		return 1;
	return 0;
}

void ControlGui::renderHelpText()
{
	if (helpTextID)
	{
		wchar_t buffer[1024];
		cLoadString(helpTextID, buffer, 1024);
		uint32_t width, height;
		width = helpFont.width(buffer);
		height = helpFont.height(buffer, width);
		gos_TextStringLength(&width, &height, buffer);
		gos_TextSetRegion(HELPAREA_LEFT, HELPAREA_BOTTOM - 3 * height, Environment.screenwidth,
			Environment.screenheight);
		drawShadowText(0xffffffff, 0xff000000, helpFont.getTempHandle(), HELPAREA_LEFT,
			HELPAREA_BOTTOM - height, true, buffer, false, helpFont.getSize());
		if (helpTextHeaderID)
		{
			cLoadString(helpTextHeaderID, buffer, 1024);
			int32_t yPos = HELPAREA_BOTTOM - height;
			drawShadowText(0xffffffff, 0xff000000, helpFont.getTempHandle(), HELPAREA_LEFT,
				yPos - height, 1, buffer, true, helpFont.getSize());
		}
	}
}

void ControlGui::renderInfoTab()
{
	if (getButton(INFO_TAB)->state & ControlButton::PRESSED)
	{
		infoWnd->render();
	}
}

bool ControlGui::isSelectingInfoObject()
{
	if (getButton(INFO_COMMAND)->state & ControlButton::PRESSED)
		return true;
	return false;
}

void ControlGui::setInfoWndMover(Mover* pMover)
{
	infoWnd->setUnit(pMover);
	if (isSelectingInfoObject())
	{
		handleClick(INFO_TAB); // fake click
		// unpress the info button
		gosEnum_KeyIndex key;
		bool bC, bA, bS;
		MissionInterfaceManager::instance()->getHotKey(INFO_COMMAND_INDEX, key, bS, bC, bA);
		if (gos_GetKeyStatus(key) != KEY_HELD)
			getButton(INFO_COMMAND)->press(0);
	}
}

void ControlGui::setVehicleCommand(bool bSet)
{
	if ((getButton(REPAIR)->isEnabled()))
		getButton(REPAIR)->press(bSet);
	else if ((getButton(SALVAGE)->isEnabled()))
		getButton(SALVAGE)->press(bSet);
	else if ((getButton(LAYMINES)->isEnabled()))
		getButton(LAYMINES)->press(bSet);
	else
	{
		Team* pTeam = Team::home;
		if (pTeam)
		{
			for (size_t i = 0; i < pTeam->getRosterSize(); ++i)
			{
				Mover* pMover = (Mover*)pTeam->getMover(i);
				if (pMover->getCommander()->getId() != Commander::home->getId())
					continue;
				bool bGuardTower = (pMover->maxMoveSpeed ? 0 : 1);
				if (bGuardTower)
				{
					pMover->suppressionFire = bSet;
					if (!bSet && MPlayer && !MPlayer->isServer())
					{
						// We send a TAC_ORDER_WAIT which will clear the
						// suppression fire message.
						// Otherwise, this is handled when we doGuardTower if
						// pressed.
						TacticalOrder tacOrder;
						tacOrder.init(OrderOriginType::player, TacticalOrderCode::wait);
						MPlayer->sendPlayerOrder(&tacOrder, false, 1, &pMover);
					}
				}
			}
		}
	}
}
bool ControlGui::getVehicleCommand()
{
	if ((getButton(REPAIR)->state & ControlButton::PRESSED) || (getButton(SALVAGE)->state & ControlButton::PRESSED) || (getButton(LAYMINES)->state & ControlButton::PRESSED) || (getButton(GUARDTOWER)->state & ControlButton::PRESSED))
		return true;
	return false;
}

void ControlButton::initButtons(FitIniFile& buttonFile, int32_t buttonCount, ControlButton* Buttons,
	ButtonData* Data, std::wstring_view str, aFont* font)
{
	wchar_t path[256];
	for (size_t i = 0; i < buttonCount; ++i)
	{
		sprintf(path, "%s%ld", str, i);
		Data[i].textureHandle = 0;
		int32_t result = buttonFile.seekBlock(path);
		if (result != NO_ERROR)
		{
			wchar_t errorStr[256];
			sprintf(errorStr, "couldn't find button %s", path);
			Assert(0, 0, errorStr);
			continue;
		}
		buttonFile.readIdLong("ID", Buttons[i].ID);
		buttonFile.readIdString("filename", Data[i].fileName, 32);
		buttonFile.readIdLong("HelpCaption", Data[i].helpTextHeader);
		buttonFile.readIdLong("HelpDesc", Data[i].helpTextID);
		buttonFile.readIdLong("TextID", Data[i].textID);
		buttonFile.readIdLong("TextNormal", Data[i].textcolours[0]);
		buttonFile.readIdLong("TextPressed", Data[i].textcolours[1]);
		buttonFile.readIdLong("TextDisabled", Data[i].textcolours[2]);
		buttonFile.readIdLong("TextHighlight", Data[i].textcolours[3]);
		if (font)
			Data[i].textFont = *font;
		int32_t x, y, width, height;
		buttonFile.readIdLong("XLocation", x);
		buttonFile.readIdLong("YLocation", y);
		x += ControlGui::hiResOffsetX;
		y += ControlGui::hiResOffsetY;
		buttonFile.readIdLong("width", width);
		buttonFile.readIdLong("height", height);
		buttonFile.readIdLong("HelpCaption", Data[i].helpTextHeader);
		buttonFile.readIdLong("HelpDesc", Data[i].helpTextID);
		buttonFile.readIdBoolean("texturesRotated", Data[i].textureRotated);
		Buttons[i].data = &Data[i];
		Buttons[i].location[0].x = Buttons[i].location[1].x = x;
		Buttons[i].location[0].y = Buttons[i].location[3].y = y;
		Buttons[i].location[2].x = Buttons[i].location[3].x = x + width;
		Buttons[i].location[1].y = Buttons[i].location[2].y = y + height;
		for (size_t j = 0; j < 4; j++)
		{
			Buttons[i].location[j].argb = 0xffffffff;
			Buttons[i].location[j].frgb = 0;
			Buttons[i].location[j].rhw = .5;
			Buttons[i].location[j].u = 0.f;
			Buttons[i].location[j].v = 0.f;
			Buttons[i].location[j].z = 0.f;
		}
		if (0 == Buttons[i].data->textureHandle)
		{
			wchar_t file[256];
			strcpy(file, artPath);
			strcat(file, Buttons[i].data->fileName);
			strcat(file, ".tga");
			_strlwr(file);
			int32_t ID = mcTextureManager->loadTexture(file, gos_Texture_Alpha, 0, 0, 0x2);
			int32_t gosID = mcTextureManager->get_gosTextureHandle(ID);
			TEXTUREPTR textureData;
			gos_LockTexture(gosID, 0, 0, &textureData);
			gos_UnLockTexture(gosID);
			Buttons[i].data->textureHandle = ID;
			Buttons[i].data->filewidth = textureData.width;
			Buttons[i].data->fileheight = Buttons[i].data->filewidth;
		}
		if (NO_ERROR != buttonFile.readIdLong("UNormal", Buttons[i].data->stateCoords[0][0]))
			Buttons[i].data->stateCoords[0][0] = -1.f;
		if (NO_ERROR != buttonFile.readIdLong("VNormal", Buttons[i].data->stateCoords[0][1]))
			Buttons[i].data->stateCoords[0][1] = -1.f;
		if (NO_ERROR != buttonFile.readIdLong("UPressed", Buttons[i].data->stateCoords[1][0]))
			Buttons[i].data->stateCoords[1][0] = -1.f;
		if (NO_ERROR != buttonFile.readIdLong("VPressed", Buttons[i].data->stateCoords[1][1]))
			Buttons[i].data->stateCoords[1][1] = -1.f;
		if (NO_ERROR != buttonFile.readIdLong("UDisabled", Buttons[i].data->stateCoords[2][0]))
			Buttons[i].data->stateCoords[2][0] = -1.f;
		if (NO_ERROR != buttonFile.readIdLong("VDisabled", Buttons[i].data->stateCoords[2][1]))
			Buttons[i].data->stateCoords[2][1] = -1.f;
		if (NO_ERROR != buttonFile.readIdLong("UAmbiguous", Buttons[i].data->stateCoords[3][0]))
		{
			if (NO_ERROR != buttonFile.readIdLong("UHighlight", Buttons[i].data->stateCoords[3][0]))
				Buttons[i].data->stateCoords[3][0] = -1.f;
		}
		if (NO_ERROR != buttonFile.readIdLong("VAmbiguous", Buttons[i].data->stateCoords[3][1]))
		{
			if (NO_ERROR != buttonFile.readIdLong("VHighlight", Buttons[i].data->stateCoords[3][1]))
				Buttons[i].data->stateCoords[3][1] = -1.f;
		}
		buttonFile.readIdLong("Uwidth", Buttons[i].data->texturewidth);
		buttonFile.readIdLong("Vheight", Buttons[i].data->textureheight);
		Buttons[i].disable(0);
		Buttons[i].press(0);
	}
}

void ControlGui::initStatics(FitIniFile& file)
{
	if (infoWnd)
		infoWnd->setUnit(0);
	if (staticInfos)
		delete[] staticInfos;
	staticInfos = nullptr;
	if (objectiveInfos)
		delete[] objectiveInfos;
	objectiveInfos = 0;
	if (videoInfos)
		delete[] videoInfos;
	videoInfos = 0;
	if (timerInfos)
		delete[] timerInfos;
	timerInfos = 0;
	if (missionStatusInfos)
		delete[] missionStatusInfos;
	missionStatusInfos = 0;
	staticCount = 0;
	file.seekBlock("Statics");
	file.readIdLong("staticCount", staticCount);
	if (!staticCount)
		return;
	staticInfos = new StaticInfo[staticCount];
	for (size_t i = 0; i < staticCount; i++)
	{
		wchar_t buffer[32];
		sprintf(buffer, "Static%ld", i);
		staticInfos[i].init(file, buffer, hiResOffsetX, hiResOffsetY);
	}
	if (staticCount > 20)
		staticInfos[20].showGUIWindow(0);
	if (NO_ERROR != file.seekBlock("HelpText"))
	{
		Assert(0, 0, "couldn't find the help text block in the layout file");
	}
	file.readIdLong("HelpTextLeft", HELPAREA_LEFT);
	file.readIdLong("HelpTextBottom", HELPAREA_BOTTOM);
	HELPAREA_LEFT += hiResOffsetX;
	HELPAREA_BOTTOM += hiResOffsetY;
	if (NO_ERROR != file.seekBlock("ResourcePoints"))
	{
		Assert(0, 0, "couldn't find the help text block in the layout file");
	}
	file.readIdLong("RPTextLeft", RPLEFT);
	file.readIdLong("RPTextTop", RPTOP);
	RPLEFT += hiResOffsetX;
	RPTOP += hiResOffsetY;
	// TUTORIAL!!
	rpCallout.rect.left = RPLEFT;
	rpCallout.rect.right = RPLEFT;
	rpCallout.rect.top = RPTOP;
	rpCallout.rect.bottom = RPTOP;
	if (NO_ERROR != file.seekBlock("Objectives"))
	{
		Assert(0, 0, "couldn't find the objective block in the button layout file");
	}
	file.readIdLong("ObjStaticCount", objectiveInfoCount);
	file.readIdLong("ObjectiveLeft", OBJECTIVESLEFT);
	file.readIdLong("ObjectiveSkip", OBJECTIVESSKIP);
	file.readIdLong("SecondObjSkip", OBJEECTIVESHEADERSKIP);
	file.readIdLong("totalRight", OBJECTIVESTOTALRIGHT);
	file.seekBlock("PrimaryObjHeader");
	file.readIdLong("HeaderLeft", OBJECTIVEHEADERLEFT);
	file.readIdLong("HeaderTop", OBJECTIVESTOP);
	objectiveInfos = new StaticInfo[objectiveInfoCount + 3];
	wchar_t blockname[64];
	for (i = 0; i < objectiveInfoCount; i++)
	{
		sprintf(blockname, "ObjStatic%ld", i);
		objectiveInfos[i].init(file, blockname);
	}
	objectiveInfos[i++].init(file, "PrimaryObjBox");
	file.readIdLong("objBoxSkip", OBJECTIVEBOXSKIP);
	objectiveInfos[i++].init(file, "PrimaryObjCheck");
	file.readIdLong("objBoxSkip", OBJECTIVECHECKSKIP);
	objectiveInfos[i++].init(file, "PrimaryObjX");
	objectiveInfoCount = i;
	if (NO_ERROR != file.seekBlock("VideoWindow"))
	{
		Assert(0, 0, "couldn't find th videoWindow block in the button layout file");
	}
	file.readIdLong("VideoStaticCount", videoInfoCount);
	file.readIdLong("left", videoRect.left);
	file.readIdLong("right", videoRect.right);
	file.readIdLong("top", videoRect.top);
	file.readIdLong("bottom", videoRect.bottom);
	videoRect.left += hiResOffsetX;
	videoRect.right += hiResOffsetX;
	file.seekBlock("VideoTextBox");
	file.readIdLong("left", videoTextRect.left);
	file.readIdLong("right", videoTextRect.right);
	file.readIdLong("top", videoTextRect.top);
	file.readIdLong("bottom", videoTextRect.bottom);
	videoTextRect.left += hiResOffsetX;
	videoTextRect.right += hiResOffsetX;
	if (videoInfoCount)
	{
		videoInfos = new StaticInfo[videoInfoCount];
		for (i = 0; i < videoInfoCount; i++)
		{
			sprintf(blockname, "VideoStatic%ld", i);
			videoInfos[i].init(file, blockname);
		}
	}
	file.seekBlock("MissionTimer");
	file.readIdLong("left", timerRect.rect.left);
	file.readIdLong("top", timerRect.rect.top);
	file.readIdLong("color", timerRect.color);
	file.readIdLong("TimerStaticCount", timerInfoCount);
	timerRect.rect.left += hiResOffsetX;
	timerRect.rect.right += hiResOffsetX;
	timerRect.rect.top += hiResOffsetY;
	timerRect.rect.bottom += hiResOffsetY;
	if (timerInfoCount)
	{
		timerInfos = new StaticInfo[timerInfoCount];
		for (i = 0; i < timerInfoCount; i++)
		{
			sprintf(blockname, "TimerStatic%ld", i);
			timerInfos[i].init(file, blockname, hiResOffsetX, hiResOffsetY);
		}
	}
	file.seekBlock("MissionResults");
	file.readIdLong("ResultsStaticCount", missionStatusInfoCount);
	if (missionStatusInfoCount)
	{
		missionStatusInfos = new StaticInfo[missionStatusInfoCount];
		for (i = 0; i < missionStatusInfoCount; i++)
		{
			sprintf(blockname, "ResultsStatic%ld", i);
			missionStatusInfos[i].init(file, blockname);
		}
	}
	file.seekBlock("ResultsTextBox");
	file.readIdLong("left", missionStatusRect.rect.left);
	file.readIdLong("right", missionStatusRect.rect.right);
	file.readIdLong("top", missionStatusRect.rect.top);
	file.readIdLong("bottom", missionStatusRect.rect.bottom);
	file.readIdLong("ResultsTextcolour", missionStatusRect.color);
	file.seekBlock("ChatPlayerNameRect");
	int32_t left, top, width, height;
	file.readIdLong("xlocation", left);
	file.readIdLong("yLocation", top);
	file.readIdLong("width", width);
	file.readIdLong("height", height);
	((aObject*)&playerNameEdit)->init(left + 2, top, width, height);
	playerNameEdit.setFont(IDS_CHAT_PLAYERNAME_FONT);
	timerFont.init(IDS_OUTLINE_CHAT_FONT);
	int32_t fontheight = playerNameEdit.getFontObject()->height();
	playerNameEdit.resize(playerNameEdit.width(), fontheight);
	playerNameEdit.setTextcolour(0xffffffff);
	file.seekBlock("ChatTextRect");
	file.readIdLong("xlocation", left);
	file.readIdLong("yLocation", top);
	file.readIdLong("width", width);
	file.readIdLong("height", height);
	((aObject*)&chatEdit)->init(left, top, width, height);
	chatEdit.font.init(IDS_OUTLINE_CHAT_FONT);
	chatEdit.setcolour(0xffffffff);
	chatEdit.resize(chatEdit.width(), fontheight);
	((aObject*)&personalEdit)->init(left, top, width, height);
	personalEdit.setFont(IDS_OUTLINE_CHAT_FONT);
	personalEdit.setTextcolour(0xffffffff);
	personalEdit.resize(personalEdit.width(), fontheight);
	personalEdit.setcolour(0); // make tranparent
}

void ControlGui::initRects(FitIniFile& file)
{
	if (rectInfos)
		delete rectInfos;
	rectInfos = 0;
	rectCount = 0;
	if (NO_ERROR != file.seekBlock("Rects"))
		return;
	file.readIdLong("rectCount", rectCount);
	if (rectCount)
	{
		rectInfos = new RectInfo[rectCount];
		wchar_t buffer[32];
		for (size_t i = 0; i < rectCount; i++)
		{
			sprintf(buffer, "Rect%ld", i);
			if (NO_ERROR != file.seekBlock(buffer))
			{
				Assert(0, i, "couldn't find the rect block");
				return;
			}
			file.readIdLong("left", rectInfos[i].rect.left);
			file.readIdLong("top", rectInfos[i].rect.top);
			file.readIdLong("right", rectInfos[i].rect.right);
			file.readIdLong("bottom", rectInfos[i].rect.bottom);
			file.readIdLong("color", rectInfos[i].color);
			rectInfos[i].rect.left += hiResOffsetX;
			rectInfos[i].rect.top += hiResOffsetY;
			rectInfos[i].rect.right += hiResOffsetX;
			rectInfos[i].rect.bottom += hiResOffsetY;
		}
		tacMap.setPos(rectInfos[0].rect);
	}
}

void ControlGui::swapResolutions(int32_t resolution)
{
	FitIniFile buttonFile;
	wchar_t path[256];
	strcpy(path, artPath);
	wchar_t fileName[32];
	if (resolution == 1600)
		strcpy(fileName, "buttonlayout1600.fit");
	else if (resolution == 1280)
		strcpy(fileName, "buttonlayout1280.fit");
	else if (resolution == 1024)
		strcpy(fileName, "buttonlayout1024.fit");
	else if (resolution == 800)
		strcpy(fileName, "buttonlayout800.fit");
	else if (resolution == 640)
		strcpy(fileName, "buttonlayout640.fit");
	else
		strcpy(fileName, "buttonlayout1024.fit");
	strcat(path, fileName);
	if (NO_ERROR != buttonFile.open(path))
	{
		wchar_t error[256];
		sprintf(error, "couldn't find the file %s", path);
		Assert(0, 0, error);
	}
	//------------------------------------
	// Get the Resolution Centering Values
	int32_t result = buttonFile.seekBlock("HiresOffsets");
	if (result == NO_ERROR)
	{
		result = buttonFile.readIdLong("xOffset", hiResOffsetX);
		if (result != NO_ERROR)
			hiResOffsetX = 0;
		result = buttonFile.readIdLong("yOffset", hiResOffsetY);
		if (result != NO_ERROR)
			hiResOffsetY = 0;
	}
	else
	{
		hiResOffsetX = hiResOffsetY = 0;
	}
	infoWnd->init(buttonFile);
	if (NO_ERROR != buttonFile.seekBlock("Fonts"))
		Assert(0, 0, "couldn't find the font block");
	int32_t fontID;
	buttonFile.readIdLong("HelpFont", fontID);
	helpFont.init(fontID);
	buttonFile.readIdLong("ObjectiveFont", fontID);
	guiFont.init(fontID);
	buttonFile.readIdLong("vehicleFont", fontID);
	vehicleFont.init(fontID);
	buttonFile.readIdLong("MissionResultsFont", fontID);
	missionResultsFont.init(fontID);
	result = buttonFile.seekBlock("ControlButtons");
	if (result != NO_ERROR)
	{
		Assert(0, 0, "Control button block not found");
	}
	int32_t buttonCount;
	buttonFile.readIdLong("ButtonCount", buttonCount);
	Assert(buttonCount == 20, buttonCount, "wrong number of control buttons");
	// clean up old texture memeory
	if (buttonData)
	{
		for (size_t i = 0; i < buttonCount; i++)
		{
			uint32_t gosID = mcTextureManager->get_gosTextureHandle(buttonData[i].textureHandle);
			mcTextureManager->removeTexture(gosID);
		}
	}
	if (vehicleData)
	{
		for (size_t i = 0; i < LAST_VEHICLE; i++)
		{
			uint32_t gosID = mcTextureManager->get_gosTextureHandle(vehicleData[i].textureHandle);
			mcTextureManager->removeTexture(gosID);
		}
	}
	if (!buttons)
		buttons = new ControlButton[buttonCount];
	if (!buttonData)
		buttonData = new ButtonData[buttonCount];
	memset(buttonData, 0, buttonCount * sizeof(ButtonData));
	ControlButton::initButtons(buttonFile, buttonCount, buttons, buttonData, "Button");
	// getButton( REPAIR )->setcolour( 0 );
	getButton(SALVAGE)->setcolour(0);
	result = buttonFile.seekBlock("VehicleButtons");
	if (result != NO_ERROR)
	{
		Assert(0, 0, "No Vehicle Buttons");
	}
	buttonFile.readIdLong("ButtonCount", buttonCount);
	Assert(buttonCount == 8, buttonCount, "wrong number of vehicle buttons");
	if (!vehicleButtons)
		vehicleButtons = new ControlButton[buttonCount];
	if (!vehicleData)
		vehicleData = new ButtonData[buttonCount];
	memset(vehicleData, 0, buttonCount * sizeof(ButtonData));
	ControlButton::initButtons(buttonFile, buttonCount, vehicleButtons, vehicleData, "Vehicle");
	getButton(TACMAP_TAB)->press(true);
	initStatics(buttonFile);
	initRects(buttonFile);
	forceGroupBar.init(buttonFile, &staticInfos[20]);
	staticInfos[20].showGUIWindow(0);
	pauseWnd->init(buttonFile);
}

void ControlGui::beginPause()
{
	getButton(SAVE_COMMAND)->press(1);
	pauseWnd->begin(renderObjectives);
}
void ControlGui::endPause()
{
	getButton(SAVE_COMMAND)->press(0);
	pauseWnd->end();
}

void ControlGui::switchTabs(int32_t direction)
{
	int32_t ID = TACMAP_TAB;
	for (size_t i = TACMAP_TAB; i < TACMAP_TAB + 3; i++)
	{
		if (getButton(i)->state & ControlButton::PRESSED)
		{
			ID = i;
			getButton(i)->press(false);
		}
	}
	if (ID == INFO_TAB)
	{
		if (direction > 0)
			ID = TACMAP_TAB;
		else
			ID = VEHICLE_TAB;
	}
	else if (ID == VEHICLE_TAB)
	{
		if (direction > 0)
			ID = INFO_TAB;
		else
			ID = TACMAP_TAB;
	}
	else
	{
		if (direction > 0)
			ID = VEHICLE_TAB;
		else
			ID = INFO_TAB;
	}
	getButton(ID)->press(true);
}

void ControlGui::playMovie(std::wstring_view fileName)
{
	if (moviePlaying)
		return;
	RECT vRect;
	vRect.left = videoRect.left;
	vRect.right = videoRect.right;
	vRect.top = videoRect.top;
	vRect.bottom = videoRect.bottom;
	// Chop everything but the actual name.
	// Assume extension is BIK.
	// Assume the movie is in data\movies.
	wchar_t realName[1024];
	_splitpath(fileName, nullptr, nullptr, realName, nullptr);
	FullPathFileName movieName;
	movieName.init(moviePath, realName, ".bik");
	bMovie = new MC2Movie;
	bMovie->init((std::wstring_view)movieName, vRect, true);
	// Maybe not enough frames to run.  Do not flash the border!
	// Do ONE update to make sure the damned sound starts at least!!
	bMovie->update();
	if (bMovie->isPlaying())
	{
		moviePlaying = true;
	}
	else
	{
		// If we couldn't play it cause of the frame Rate, TOSS it!!
		moviePlaying = false;
		delete bMovie;
		bMovie = nullptr;
	}
}

void ControlButton::toggle()
{
	if (state & DISABLED)
		return;
	else if (state & HIDDEN)
		return;
	if (state & PRESSED)
		press(0);
	else
		press(1);
}

bool ControlGui::resultsDone()
{
	if ((renderStatusInfo && objectiveTime > Team::home->objectives.Count() + 4))
	{
		// need to empty out force group bar now...
		forceGroupBar.removeAll();
		return true;
	}
	return false;
}

void ControlButton::setcolour(uint32_t newcolour)
{
	for (size_t i = 0; i < 4; i++)
		location[i].argb = newcolour;
}

void ControlGui::setRolloverHelpText(uint32_t textID)
{
	if (helpTextID && textID != 0)
		return;
	if (useLeftRightMouseProfile)
		textID++;
	helpTextHeaderID = 0;
	helpTextID = textID;
}

void ControlGui::toggleHoldPosition()
{
	if (getButton(CUR_RANGE)->state == ControlButton::ENABLED)
	{
		setRange(FireRangeType::current);
	}
	else
	{
		setRange(FireRangeType::optimal);
	}
	soundSystem->playDigitalSample(LOG_SELECT);
	if (MPlayer && !MPlayer->isServer())
		MPlayer->sendHoldPosition();
	else
	{
		for (size_t i = 0; i < Team::home->getRosterSize(); i++)
		{
			Mover* pMover = Team::home->getMover(i);
			if (pMover->isSelected() && pMover->getCommander()->getId() == Commander::home->getId())
			{
				if (pMover->getPilot()->getCurTacOrder()->code != TacticalOrderCode::none)
				{
					pMover->getPilot()->getCurTacOrder()->attackParams.range =
						(FireRangeType)pMover->attackRange;
					if (pMover->attackRange == FireRangeType::current)
						pMover->getPilot()->getCurTacOrder()->attackParams.pursue = false;
					else if (getFireFromCurrentPos())
						pMover->getPilot()->getCurTacOrder()->attackParams.pursue = false;
					else
						pMover->getPilot()->getCurTacOrder()->attackParams.pursue = true;
					pMover->getPilot()->getCurTacOrder()->pack(nullptr, nullptr);
					pMover->handleTacticalOrder(*pMover->getPilot()->getCurTacOrder());
				}
			}
		}
	}
}

int32_t
ControlGui::updateChat()
{
	if (bChatting && MPlayer)
	{
		personalEdit.update();
		if (!personalEdit.hasFocus())
		{
			//			toggleChat(0);
		}
		return 1;
	}
	return 0;
}

void ControlGui::toggleChat(bool isTeamOnly)
{
	bChatting ^= 1;
	if (!bChatting && MPlayer) // over -- broadcast messages
	{
		std::wstring_view tmp;
		personalEdit.getEntry(tmp);
		if (tmp.Length())
		{
			wchar_t team = chatIsTeamOnly ? MPlayer->getPlayerInfo(MPlayer->commanderid)->team : -1;
			MPlayer->sendChat(0, team, (std::wstring_view)(std::wstring_view)tmp);
		}
		personalEdit.setFocus(0);
	}
	else if (MPlayer)
	{
		personalEdit.limitEntry(127);
		personalEdit.setFocus(true);
		personalEdit.setEntry("");
	}
	chatIsTeamOnly = isTeamOnly;
}

void ControlGui::eatChatKey()
{
	if (bChatting)
		personalEdit.setFocus(true);
	else
		personalEdit.setFocus(false);
}

void ControlGui::setChatText(std::wstring_view playerName, std::wstring_view message, uint32_t color, uint32_t chatcolour)
{
	for (size_t i = 0; i < MAX_CHAT_COUNT - 1; i++)
	{
		memcpy(&chatInfos[i], &chatInfos[i + 1], sizeof(ChatInfo));
	}
	i = MAX_CHAT_COUNT - 1;
	gosASSERT(strlen(message) < 128);
	gosASSERT(strlen(playerName) < 32);
	chatInfos[i].backgroundcolour = color;
	strcpy(chatInfos[i].message, message);
	strcpy(chatInfos[i].playerName, playerName);
	chatInfos[i].time = scenarioTime;
	chatInfos[i].chatTextcolour = chatcolour;
	int32_t totalheight = chatEdit.font.height(message, chatEdit.width());
	int32_t lineheight = chatEdit.font.height("A", chatEdit.width());
	chatInfos[i].messageLength = totalheight / lineheight;
	if (chatInfos[i].messageLength > 1)
		chatInfos[i].messageLength--;
	if (MPlayer && ChatWindow::instance())
	{
		ChatWindow::instance()->update();
	}
}

void ControlGui::renderChatText()
{
	int32_t curLine = 0;
	if (bChatting && MPlayer)
	{
		playerNameEdit.setcolour(0xff007f00);
		if (MPlayer)
		{
			MC2Player* pInfo = MPlayer->getPlayerInfo(MPlayer->commanderid);
			int32_t color = MPlayer->colors[pInfo->basecolour[BASECOLOR_TEAM]];
			int32_t textcolour = 0xff000000;
			if (((color & 0xff) + ((color & 0xff00) >> 8) + ((color & 0xff0000) >> 16)) / 3 < 85)
				textcolour = 0xffffffff;
			playerNameEdit.setcolour(color);
			playerNameEdit.setTextcolour(textcolour);
			int32_t width = playerNameEdit.getFontObject()->width(pInfo->name) + playerNameEdit.getFontObject()->width(" ");
			playerNameEdit.resize(width + 8, playerNameEdit.height());
			playerNameEdit.setEntry(pInfo->name);
			personalEdit.move(playerNameEdit.right() - personalEdit.left(), 0);
		}
		else
			playerNameEdit.setEntry("Heidi");
		playerNameEdit.render();
		personalEdit.render();
		curLine++;
	}
	int32_t lineCount = 0;
	int32_t curTime = scenarioTime;
	for (size_t i = MAX_CHAT_COUNT - 1; i > -1; i--)
	{
		if (chatInfos[i].messageLength && curTime - chatInfos[i].time < CHAT_DISPLAY_TIME)
		{
			lineCount += chatInfos[i].messageLength;
		}
		if (lineCount > MAX_CHAT_COUNT || curTime - chatInfos[i].time > CHAT_DISPLAY_TIME)
			break;
	}
	i++;
	float height = chatEdit.height();
	for (; i < MAX_CHAT_COUNT; i++)
	{
		if (i < 0)
			continue;
		if (lineCount > MAX_CHAT_COUNT)
		{
			// for now ignore
			//	int32_t numToMove = lineCount - MAX_CHAT_COUNT;
			//	chatEdit.move( 0, -numToMove * (height + 1 ));
			//	chatEdit.render();
			//	chatEdit.move( 0, numToMove * (height+1 ));
			//	curLine += MAX_CHAT_COUNT - numToMove;
			lineCount -= chatInfos[i].messageLength;
		}
		else
		{
			if (chatInfos[i].messageLength)
			{
				int32_t color = chatInfos[i].backgroundcolour;
				int32_t textcolour = 0xff000000;
				if (((color & 0xff) + ((color & 0xff00) >> 8) + ((color & 0xff0000) >> 16)) / 3 < 85)
					textcolour = 0xffffffff;
				chatEdit.resize(
					chatEdit.width(), (chatEdit.height() + 2) * chatInfos[i].messageLength);
				playerNameEdit.setcolour(color);
				playerNameEdit.setTextcolour(textcolour);
				playerNameEdit.move(0, curLine * (height + 1));
				int32_t width = playerNameEdit.getFontObject()->width(chatInfos[i].playerName) + playerNameEdit.getFontObject()->width(" ");
				playerNameEdit.resize(width + 8, playerNameEdit.height());
				playerNameEdit.setEntry(chatInfos[i].playerName);
				chatEdit.setText(chatInfos[i].message);
				chatEdit.setcolour(chatInfos[i].chatTextcolour);
				chatEdit.move(playerNameEdit.right() - chatEdit.left() + 4, curLine * (height + 1));
				int32_t oldheight = chatEdit.font.height();
				chatEdit.resize(chatEdit.width(), chatInfos->messageLength * oldheight);
				playerNameEdit.render();
				chatEdit.render();
				chatEdit.resize(chatEdit.width(), oldheight);
				chatEdit.move(0, -curLine * (height + 1));
				playerNameEdit.move(0, -curLine * (height + 1));
			}
			curLine += chatInfos[i].messageLength;
		}
	}
	chatEdit.resize(chatEdit.width(), height);
}

bool ControlGui::isMoviePlaying()
{
	if (moviePlaying)
		return true;
	else if (forceGroupBar.isPlayingVideo())
		return true;
	return false;
}

bool ControlGui::playPilotVideo(MechWarrior* pPilot, wchar_t movieCode)
{
	// stupid name format = "Worm 1 filter.avi"
	// CHANGED to "worm1"  for CD 8.3 reasons.
	// -fs
	wchar_t fileName[512];
	strcpy(fileName, moviePath);
	wchar_t realPilotName[8];
	strncpy(realPilotName, pPilot->getName(), 7);
	strcat(fileName, realPilotName); // swap in pilot name when videos are done
	wchar_t tmp[3];
	tmp[0] = movieCode;
	tmp[1] = nullptr;
	tmp[2] = nullptr;
	strcat(fileName, tmp);
	return forceGroupBar.setPilotVideo(fileName, pPilot);
}

void ControlGui::endPilotVideo()
{
	forceGroupBar.setPilotVideo(nullptr, nullptr);
}

void ControlGui::cancelInfo()
{
	gosEnum_KeyIndex key;
	bool bC, bA, bS;
	MissionInterfaceManager::instance()->getHotKey(INFO_COMMAND_INDEX, key, bS, bC, bA);
	if (gos_GetKeyStatus(key) != KEY_HELD)
		getButton(INFO_COMMAND)->press(0);
}

void ControlGui::showServerMissing()
{
	if (!MPlayer)
		return;
	if (MPlayer->isServerMissing())
	{
		// send a chat message
		if (!bServerWarningShown)
		{
			wchar_t text[256];
			cLoadString(IDS_SERVER_MIGRATING, text, 255);
			setChatText("", text, 0, 0xffffffff);
			bServerWarningShown = true;
			// pause the game
			MissionInterfaceManager::instance()->togglePauseWithoutMenu();
		}
	}
	else
	{
		if (bServerWarningShown)
			MissionInterfaceManager::instance()->togglePauseWithoutMenu();
		bServerWarningShown = 0;
	}
}

// end of file ( controlGui.cpp )
