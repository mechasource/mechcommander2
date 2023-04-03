#define MPSTATS_CPP
/*************************************************************************************************\
MPStats.cpp			: Implementation of the MPStats component.
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//
\*************************************************************************************************/
#include "stdinc.h"

#include "mpstats.h"
#include "resource.h"
#include "MissionBriefingScreen.h"
#include "chatwindow.h"
#include "MechBayScreen.h"
#include "logisticsdialog.h"

#define MP_STATS_SAVE 400

MPStats::MPStats()
{
	helpTextArrayID = 8;
	status = -1;
	bHostLeftDlg = 0;
}

//-------------------------------------------------------------------------------------------------

MPStats::~MPStats() { }

int32_t
MPStats::init()
{
	FitIniFile file;
	FullPathFileName path;
	path.init(artPath, "mcl_mp_stats", ".fit");
	if (NO_ERROR != file.open(path))
	{
		wchar_t buffer2[512];
		sprintf(buffer2, "couldn't open file %s", (std::wstring_view)path);
		Assert(0, 0, buffer2);
		return false;
	}
	LogisticsScreen::init(file, "Static", "Text", "Rect", "Button", "Edit");
	entries[0].init();
	entries[0].moveTo(rects[1].globalX(), rects[1].globalY());
	entries[0].resize(rects[1].width(), rects[1].height());
	for (size_t i = 0; i < MAX_MC_PLAYERS - 1; i++)
	{
		entries[i + 1] = entries[0];
		entries[i + 1].move(0, (i + 1) * (entries[0].height() + 1));
	}
	for (i = 0; i < buttonCount; i++)
		buttons[i].setMessageOnRelease();
	bSavingStats = 0;
	bHostLeftDlg = 0;
	return true;
}

int32_t __cdecl sortStats(PCVOID pPlayer1, PCVOID pPlayer2)
{
	MC2Player* player1 = *(MC2Player**)pPlayer1;
	MC2Player* player2 = *(MC2Player**)pPlayer2;
	if (player1->rank > player2->rank)
		return 1;
	else if ((player1)->rank < (player2)->rank)
		return -1;
	// else if ( (player1)->kills > (player2)->kills )
	//	return 1;
	// else if ( (player1)->losses > (player2)->losses )
	//	return 1;
	return 0;
}

void MPStats::end()
{
	MPlayer->setInProgress(false);
	bSavingStats = 0;
}

void MPStats::begin()
{
	MPlayer->setMode(MULTIPLAYER_MODE_RESULTS);
	bSavingStats = 0;
	status = RUNNING;
	beginFadeIn(.5);
	for (size_t i = 0; i < MAX_MC_PLAYERS; i++)
		entries[i].showGUIWindow(0);
	statics[15].setcolour(0);
	// need to set up map name
	bool bRes = 0;
	wchar_t text2[256];
	wchar_t text[256];
	cLoadString(IDS_MP_LM_MAP_LIST_MAP_NAME, text, 255);
	sprintf(text2, text, MPlayer->missionSettings.name);
	textObjects[1].setText(text2);
	uint32_t type = MPlayer->missionSettings.missionType;
	cLoadString(IDS_MP_LM_MAP_LIST_TYPE, text, 255);
	wchar_t mType[128];
	cLoadString(IDS_MP_LM_TYPE0 + type, mType, 127);
	sprintf(text2, text, mType);
	textObjects[6].setText(text2);
	uint32_t numPlayers = MPlayer->missionSettings.maxPlayers;
	cLoadString(IDS_MP_LM_MAP_LIST_MAX_PLAYERS, text, 255);
	sprintf(text2, text, numPlayers);
	textObjects[7].setText(text2);
}

int32_t
MPStats::handleMessage(uint32_t what, uint32_t who)
{
	if (who == MP_STATS_SAVE)
	{
		LogisticsVariantDialog::instance()->beginTranscript();
		bSavingStats = true;
	}
	else if (who == MB_MSG_NEXT)
	{
		status = NEXT;
		end();
		beginFadeOut(.5f);
		statics[15].setTexture((uint32_t)0);
		statics[15].setcolour(0);
	}
	return 1;
}

void MPStats::render(int32_t xOffset, int32_t yOffset)
{
	LogisticsScreen::render(xOffset, yOffset);
	for (size_t i = 0; i < MAX_MC_PLAYERS; i++)
	{
		entries[i].render(0, 0);
	}
	ChatWindow::instance()->render(xOffset, yOffset);
	if (bSavingStats)
	{
		LogisticsVariantDialog::instance()->render();
	}
	if (bHostLeftDlg)
	{
		LogisticsOneButtonDialog::instance()->render();
	}
}

void MPStats::update()
{
	if (status == RUNNING && !statics[15].getcolour())
	{
		int32_t textureHandle = MissionBriefingScreen::getMissionTGA(MPlayer->missionSettings.map);
		statics[15].setTexture(textureHandle);
		statics[15].setUVs(0, 127, 127, 0);
		statics[15].setcolour(0xffffffff);
		int32_t playerCount = 0;
		const MC2Player* players = MPlayer->getPlayers(playerCount);
		const MC2Player* sorted[MAX_MC_PLAYERS];
		int32_t winnerCount = 0;
		for (size_t j = 0; j < playerCount; j++)
		{
			sorted[j] = &players[j];
			if (sorted[j]->rank == 1)
				winnerCount++;
		}
		bool scoreShown[MAX_MC_PLAYERS]; // keep track of whose shown the score
		memset(scoreShown, 0, sizeof(bool) * MAX_MC_PLAYERS);
		uint32_t winnercolour = 0xffFFCC00; // gold
		if (winnerCount > 1)
			winnercolour = 0xffA6A6A6;
		qsort(sorted, playerCount, sizeof(MC2Player*), sortStats);
		for (size_t i = 0; i < MAX_MC_PLAYERS; i++)
		{
			if (i < playerCount)
			{
				entries[i].setData(sorted[i], winnercolour, !scoreShown[sorted[i]->team]);
				scoreShown[sorted[i]->team] = true;
				entries[i].showGUIWindow(true);
			}
			else
				entries[i].showGUIWindow(0);
		}
	}
	if (bSavingStats)
	{
		LogisticsVariantDialog::instance()->update();
		if (LogisticsVariantDialog::instance()->getStatus() == YES)
		{
			std::wstring_view str = LogisticsVariantDialog::instance()->getFileName();
			FullPathFileName oldPath;
			FullPathFileName newPath;
			newPath.init("data\\multiplayer\\transcripts\\", str, ".txt");
			oldPath.init("data\\multiplayer\\transcripts\\", "transcript", ".txt");
			LogisticsVariantDialog::instance()->end();
			CopyFile(oldPath, newPath, 0);
			bSavingStats = 0;
		}
		else if (LogisticsVariantDialog::instance()->getStatus() == NO)
		{
			if (LogisticsVariantDialog::instance()->isDone())
				bSavingStats = 0;
		}
	}
	else if (bHostLeftDlg)
	{
		LogisticsOneButtonDialog::instance()->update();
		if (LogisticsOneButtonDialog::instance()->isDone())
		{
			LogisticsOneButtonDialog::instance()->end();
			bHostLeftDlg = 0;
		}
	}
	else if (!ChatWindow::instance()->pointInside(userInput->getMouseX(), userInput->getMouseY()))
		LogisticsScreen::update();
	if (ChatWindow::instance())
		ChatWindow::instance()->update();
}

void MPStats::setHostLeftDlg(std::wstring_view hostName)
{
	wchar_t leaveStr[256];
	wchar_t formatStr[256];
	cLoadString(IDS_PLAYER_LEFT, leaveStr, 255);
	sprintf(formatStr, leaveStr, hostName);
	LogisticsOneButtonDialog::instance()->setText(IDS_PLAYER_LEFT, IDS_DIALOG_OK, IDS_DIALOG_OK);
	LogisticsOneButtonDialog::instance()->setText(formatStr);
	LogisticsOneButtonDialog::instance()->begin();
	bHostLeftDlg = true;
}

MPStatsEntry::MPStatsEntry()
{
	overlaycolour = 0;
}

void MPStatsEntry::render(int32_t x, int32_t y)
{
	LogisticsScreen::render(x, y);
}

MPStatsEntry::~MPStatsEntry() { }

void MPStatsEntry::init()
{
	FitIniFile file;
	FullPathFileName path;
	path.init(artPath, "mcl_mp_scoreboard", ".fit");
	if (NO_ERROR != file.open(path))
	{
		wchar_t buffer2[512];
		sprintf(buffer2, "couldn't open file %s", (std::wstring_view)path);
		Assert(0, 0, buffer2);
		return;
	}
	LogisticsScreen::init(file, "Static", "Text", "Rect", "Button", "Edit");
	aObject::init(
		rects[2].left(), rects[2].top(), rects[5].right() - rects[0].left(), rects[2].height());
	overlaycolour = 0;
}

void MPStatsEntry::setData(const MC2Player* data, bool bShowScore)
{
	int32_t color = data ? MPlayer->colors[data->basecolour[BASECOLOR_TEAM]] : 0x00000000;
	int32_t color2 = data ? MPlayer->colors[data->stripecolour] : 0x00000000;
	int32_t scorecolour = 0xffffffff;
	if (data && MPlayer->allUnitsDestroyed[data->commanderid])
		scorecolour = 0xff7f7f7f;
	rects[0].setcolour(color2);
	rects[1].setcolour(color);
	int32_t textcolour = 0xff000000;
	if (((color & 0xff) + ((color & 0xff00) >> 8) + ((color & 0xff0000) >> 16)) / 3 < 85)
		textcolour = 0xffffffff;
	if (!data)
	{
		textObjects[0].setText(IDS_MP_STATS_PLAYER_NAME);
		textObjects[1].setText("");
		textObjects[2].setText(IDS_MP_STATS_SCORE);
		textObjects[3].setText(IDS_MP_STATS_KILLS);
		textObjects[4].setText(IDS_MP_STATS_LOSSES);
	}
	else
	{
		textObjects[1].setcolour(scorecolour);
		textObjects[2].setcolour(scorecolour);
		textObjects[3].setcolour(scorecolour);
		textObjects[4].setcolour(scorecolour);
		textObjects[0].setcolour(textcolour);
		textObjects[0].setText(data->name);
		wchar_t team[32];
		sprintf(team, "%ld", data->teamSelected + 1);
		textObjects[1].setText(team);
		// score needs to do here
		sprintf(team, "%ld", data->score);
		if (MPlayer->missionSettings.missionType == MISSION_TYPE_KING_OF_THE_HILL)
		{
			sprintf(team, "%ld:%.2ld", data->score / 60, data->score % 60);
		}
		if (bShowScore)
			textObjects[2].setText(team);
		else
			textObjects[2].setText("");
		sprintf(team, "%ld", data->kills);
		textObjects[3].setText(team);
		sprintf(team, "%ld", data->losses);
		textObjects[4].setText(team);
	}
}

int32_t
MPStatsEntry::getPlayerHeadingX()
{
	return location[0].x;
}
int32_t
MPStatsEntry::getRankingX()
{
	return textObjects[0].globalX();
}
int32_t
MPStatsEntry::getKillsX()
{
	return textObjects[1].globalX();
}
int32_t
MPStatsEntry::getLossesX()
{
	return textObjects[2].globalX();
}

MPStatsResultsEntry::MPStatsResultsEntry()
{
	overlaycolour = 0;
}

MPStatsResultsEntry::~MPStatsResultsEntry() { }

void MPStatsResultsEntry::render(int32_t x, int32_t y)
{
	LogisticsScreen::render(x, y);
	// if ( overlaycolour )
	//{
	//	RECT rect = { textObjects[5].left(), rects[0].top(), rects[5].right(),
	// rects[0].y() + rects[0].height() }; 	drawRect( rect, overlaycolour );
	//}
}

void MPStatsResultsEntry::init()
{
	FitIniFile file;
	FullPathFileName path;
	path.init(artPath, "mcl_mp_stats_entry", ".fit");
	if (NO_ERROR != file.open(path))
	{
		wchar_t buffer2[512];
		sprintf(buffer2, "couldn't open file %s", (std::wstring_view)path);
		Assert(0, 0, buffer2);
		return;
	}
	LogisticsScreen::init(file, "Static", "Text", "Rect", "Button", "Edit");
	aObject::init(0, 0, rects[0].width(), rects[0].height());
}

void MPStatsResultsEntry::setData(const MC2Player* data, uint32_t laurelcolour, bool bShowScore)
{
	rects[4].setcolour(MPlayer->colors[data->basecolour[BASECOLOR_TEAM]]);
	rects[2].setcolour(MPlayer->colors[data->stripecolour]);
	if (data && MPlayer->allUnitsDestroyed[data->commanderid])
		overlaycolour = 0x7f000000;
	else
		overlaycolour = 0;
	int32_t color = MPlayer->colors[data->basecolour[BASECOLOR_TEAM]];
	int32_t textcolour = 0xff000000;
	if (((color & 0xff) + ((color & 0xff00) >> 8) + ((color & 0xff0000) >> 16)) / 3 < 85)
		textcolour = 0xffffffff;
	textObjects[3].setText(data->name);
	textObjects[4].setText(data->unitName);
	textObjects[3].setcolour(textcolour);
	textObjects[4].setcolour(textcolour);
	wchar_t text[64];
	sprintf(text, "%ld", MPlayer->teamScore[data->team]);
	if (MPlayer->missionSettings.missionType == MISSION_TYPE_KING_OF_THE_HILL)
	{
		sprintf(text, "%ld:%.2ld", MPlayer->teamScore[data->team] / 60,
			MPlayer->teamScore[data->team] % 60);
	}
	if (bShowScore)
		textObjects[0].setText(text);
	else
		textObjects[0].setText("");
	sprintf(text, "%ld", data->kills);
	textObjects[1].setText(text);
	sprintf(text, "%ld", data->losses);
	textObjects[2].setText(text);
	sprintf(text, "%ld", data->teamSelected + 1);
	textObjects[5].setText(text);
	wchar_t path[256];
	strcpy(path, "data\\multiplayer\\insignia\\");
	strcat(path, data->insigniaFile);
	if (data->winner)
	{
		statics[1].setcolour(laurelcolour);
		rects[7].setcolour(laurelcolour);
	}
	else
	{
		statics[1].setcolour(0);
		rects[7].setcolour(0);
	}
	if (fileExists(path))
	{
		statics[0].setTexture(path);
		statics[0].setUVs(0, 0, 32, 32);
	}
	else
	{
		TGAFileHeader* pData = (TGAFileHeader*)MPlayer->insigniaList[data->commanderid];
		if (pData)
		{
			int32_t size = pData->pixel_depth / 8;
			int32_t ID = mcTextureManager->textureFromMemory(
				(uint32_t*)(pData + 1), gos_Texture_Solid, 0, pData->width, size);
			statics[0].setTexture(ID);
			statics[0].setUVs(0, 32, 32, 0);
		}
	}
}

// end of file ( MPStats.cpp )
