/*************************************************************************************************\
ControlGui.h			: Interface for the ControlGui component.  This thing
holds the tac map and everything else on the left hand side of the screen.
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//
\*************************************************************************************************/

#pragma once

#ifndef CONTROLGUI_H
#define CONTROLGUI_H

//#include "gametacmap.h"
//#include "forcegroupbar.h"
//#include "mover.h"
//#include "utilities.h"
//#include "pausewindow.h"
//#include "mechgui/afont.h"
//#include "mechgui/aedit.h"
//#include "mc2movie.h"
//#include "mpstats.h"

class CObjective;
class InfoWindow;
class PauseWindow;

#define LAST_VEHICLE (MAX_VEHICLE - LARGE_AIRSTRIKE)

/**************************************************************************************************
CLASS DESCRIPTION
ControlGui:
**************************************************************************************************/

struct ButtonData
{
	uint32_t ID;
	int32_t helpTextHeader;
	int32_t helpTextID;
	int32_t textID;
	int32_t textcolours[4];
	aFont textFont;
	wchar_t fileName[32];
	int32_t stateCoords[4][2];
	int32_t texturewidth;
	int32_t textureheight;
	int32_t filewidth;
	int32_t fileheight;
	uint32_t textureHandle;
	bool textureRotated;
};

class ControlButton
{
public:
	gos_VERTEX location[4];
	int32_t ID;

	ButtonData* data;
	int32_t state;

	void render(void);
	void press(bool);
	void toggle(void);
	void disable(bool);
	bool isEnabled(void);
	void makeAmbiguous(bool bAmbiguous);
	void hide(bool);
	void move(float deltaX, float deltaY);
	void setcolour(uint32_t newcolour);
	static void makeUVs(gos_VERTEX* vertices, int32_t State, ButtonData& data);

	static void initButtons(FitIniFile& file, int32_t buttonCount, ControlButton* buttons,
		ButtonData* buttonData, std::wstring_view str, aFont* font = 0);

	enum States
	{
		ENABLED = 0,
		PRESSED,
		DISABLED,
		AMBIGUOUS,
		HIDDEN
	};
};

class ControlGui
{

public:
	static ControlGui* instance;

	static int32_t hiResOffsetX;
	static int32_t hiResOffsetY;

	ControlGui(void);
	~ControlGui(void);
	bool inRegion(int32_t mouseX, int32_t mouseY, bool bPaused);
	void render(bool bPaused);
	void update(bool bPaused, bool bLOS);
	void initTacMapBuildings(uint8_t* data, int32_t size)
	{
		tacMap.initBuildings(data, size);
	}
	void initTacMap(uint8_t* data, int32_t size)
	{
		tacMap.init(data, size);
	}
	void initMechs(void);
	void unPressAllVehicleButtons(void);
	void disableAllVehicleButtons(void);
	void addMover(std::unique_ptr<Mover> mover);
	void removeMover(std::unique_ptr<Mover> mover);
	int32_t updateChat(void);

	void beginPause(void);
	void endPause(void);

	bool resultsDone(void);

	void startObjectives(bool bStart);
	bool objectivesStarted()
	{
		return renderObjectives;
	}

	void setInfoWndMover(Mover* pMover);
	void setVehicleCommand(bool);
	bool getVehicleCommand();

	void playMovie(std::wstring_view fileName);
	bool isMoviePlaying(void);

	bool playPilotVideo(MechWarrior* pPilot, wchar_t movieCode);
	void endPilotVideo(void);
	bool isSelectingInfoObject(void);

	bool isOverTacMap(void);

	bool isChatting()
	{
		return bChatting;
	}

	// TUTORIAL
	bool animateTacMap(int32_t buttonId, float timeToScroll, int32_t numFlashes);
	bool pushButton(int32_t buttonId);
	bool flashRPTotal(int32_t numFlashes);

	std::wstring_view getVehicleName(int32_t& ID);
	std::wstring_view getVehicleNameFromID(int32_t ID);
	void swapResolutions(int32_t newResolution);

	GameTacMap tacMap;
	ForceGroupBar forceGroupBar;

	bool mouseInVehicleStopButton;

	enum class __controlgui_const
	{
		OBJECTVE_MOVE_COUNT = 2,
		RESULTS_MOVE_COUNT = 3,
		MAX_CHAT_COUNT = 5,
		RPTOTAL_CALLOUT = 21185,
	};

	enum Commands
	{
		DEFAULT_RANGE = 0,
		SHORT_RANGE,
		MED_RANGE,
		LONG_RANGE,
		JUMP_COMMAND,
		RUN_COMMAND,
		GUARD_COMMAND,
		FIRE_FROM_CURRENT_POS,
		STOP_COMMAND,
		INFO_COMMAND,
		OBJECTIVES_COMMAND,
		SAVE_COMMAND,
		TACMAP_TAB,
		INFO_TAB,
		VEHICLE_TAB,
		LAYMINES,
		REPAIR,
		SALVAGE,
		GUARDTOWER,
		CUR_RANGE, // fire from right where I AM
		LAST_COMMAND

	};

	enum VehicleCommands
	{
		LARGE_AIRSTRIKE = 100,
		GUARD_TOWER,
		SENSOR_PROBE,
		REPAIR_VEHICLE,
		PEGASUS_SCOUT,
		MINELAYER,
		RECOVERY_TEAM,
		STOP_VEHICLE,
		MAX_VEHICLE
	};

	bool isDefaultSpeed(void);
	void toggleDefaultSpeed();
	void toggleJump();
	bool getJump();
	bool getWalk();
	bool getRun();
	void toggleGuard();
	bool getGuard();
	void setDefaultSpeed(void);
	void toggleHoldPosition(void);

	void setRange(FireRangeType Range);
	void doStop(void);
	void toggleFireFromCurrentPos(void);
	bool getFireFromCurrentPos()
	{
		return fireFromCurrentPos;
	}
	void setFireFromCurrentPos(bool bset)
	{
		fireFromCurrentPos = bset;
	}
	bool isAddingVehicle()
	{
		return addingVehicle;
	}
	bool isAddingAirstrike()
	{
		return addingArtillery;
	}
	bool isAddingSalvage()
	{
		return addingSalvage;
	}
	bool isButtonPressed(int32_t ID)
	{
		return getButton(ID)->state & ControlButton::PRESSED;
	}
	bool getMines(void);
	bool getSalvage(void);
	bool getRepair(void);
	bool getGuardTower(void);
	void switchTabs(int32_t direction);
	void renderObjective(CObjective* pObjective, int32_t xPos, int32_t yPos, bool bDrawTotal);
	void renderMissionStatus(bool bRender)
	{
		renderStatusInfo = bRender;
	}

	FireRangeType getCurrentRange(void);
	void pressInfoButton()
	{
		handleClick(INFO_COMMAND);
	}
	bool infoButtonPressed()
	{
		return getButton(INFO_COMMAND)->state & ControlButton::PRESSED;
	}

	void showServerMissing(void);

	void pressAirstrikeButton()
	{
		for (size_t i = 0; i < LAST_VEHICLE; i++)
		{
			if ((vehicleButtons[i].ID == LARGE_AIRSTRIKE) && !(vehicleButtons[i].state & ControlButton::PRESSED))
			{
				handleVehicleClick(LARGE_AIRSTRIKE);
			}
		}
	}
	void pressLargeAirstrikeButton()
	{
		handleVehicleClick(LARGE_AIRSTRIKE);
	}
	void pressSensorStrikeButton()
	{
		for (size_t i = 0; i < LAST_VEHICLE; i++)
		{
			if ((vehicleButtons[i].ID == SENSOR_PROBE) && !(vehicleButtons[i].state & ControlButton::PRESSED))
			{
				handleVehicleClick(SENSOR_PROBE);
			}
		}
	}

	void setRolloverHelpText(uint32_t textID);

	void setChatText(std::wstring_view playerName, std::wstring_view message, uint32_t backgroundcolour, uint32_t textcolour);
	void toggleChat(bool setTeamOnly);
	void eatChatKey(void);
	void cancelInfo(void);

	ControlButton* getButton(int32_t ID);

	struct RectInfo
	{
		RECT rect;
		int32_t color;
	};

private:
	struct ChatInfo
	{
		wchar_t playerName[32];
		wchar_t message[128];
		uint32_t backgroundcolour;
		uint32_t time;
		uint32_t messageLength; // number of lines
		uint32_t chatTextcolour;
	};

	ChatInfo chatInfos[MAX_CHAT_COUNT]; // max five lines -- could change

	RectInfo* rectInfos;
	int32_t rectCount;

	// static	ButtonFile		vehicleFileData[LAST_VEHICLE];
	static uint32_t RUN;
	static uint32_t WALK;
	static uint32_t GUARD;
	static uint32_t JUMP;

	ControlButton* buttons;
	ControlButton* vehicleButtons;
	static ButtonData* buttonData;
	static ButtonData* vehicleData;
	static std::wstring_view vehicleNames[5];
	static int32_t vehicleIDs[5];
	static std::wstring_view vehiclePilots[5];

	static int32_t vehicleCosts[LAST_VEHICLE];

	InfoWindow* infoWnd;
	PauseWindow* pauseWnd;

	StaticInfo* staticInfos;
	int32_t staticCount;

	StaticInfo* objectiveInfos; // 2nd to last one is check, last is x
	int32_t objectiveInfoCount;

	StaticInfo* missionStatusInfos;
	int32_t missionStatusInfoCount;
	RectInfo missionStatusRect;

	bool renderStatusInfo;
	float resultsTime;
	bool renderObjectives;
	float objectiveTime;

	float tabFlashTime;
	static int32_t OBJECTIVESTOP;
	static int32_t OBJECTIVESLEFT;
	static int32_t OBJECTIVESSKIP;
	static int32_t OBJECTIVESTOTALRIGHT;
	static int32_t OBJEECTIVESHEADERSKIP;
	static int32_t OBJECTIVESHEADERTOP;
	static int32_t OBJECTIVEBOXX;
	static int32_t OBJECTIVEBOXSKIP;
	static int32_t OBJECTIVECHECKSKIP;
	static int32_t OBJECTIVEHEADERLEFT;
	static int32_t HELPAREA_LEFT;
	static int32_t HELPAREA_BOTTOM;
	static int32_t RPLEFT;
	static int32_t RPTOP;

	static MoveInfo objectiveMoveInfo[OBJECTVE_MOVE_COUNT];
	static MoveInfo missionResultsMoveInfo[RESULTS_MOVE_COUNT];

	StaticInfo* videoInfos;
	int32_t videoInfoCount;
	RECT videoRect;
	RECT videoTextRect;
	MC2MoviePtr bMovie;

	StaticInfo* timerInfos;
	int32_t timerInfoCount;
	RectInfo timerRect;

	// TUTORIAL!!
	RectInfo rpCallout;
	int32_t rpNumFlashes;
	float rpFlashTime;
	int32_t buttonToPress;

	uint32_t curOrder;
	bool fireFromCurrentPos;
	bool addingVehicle;
	bool addingArtillery;
	bool addingSalvage;
	bool wasLayingMines;

	bool moviePlaying;
	bool twoMinWarningPlayed;
	bool thirtySecondWarningPlayed;
	bool bChatting;

	int32_t idToUnPress;
	aFont guiFont;
	aFont helpFont;
	aFont vehicleFont;
	aFont timerFont;
	aFont missionResultsFont;

	aText chatEdit;
	aEdit playerNameEdit;
	aEdit personalEdit;

	void handleClick(int32_t ID);
	void updateVehicleTab(int32_t mouseX, int32_t mouseY, bool bLOS);
	void renderVehicleTab(void);
	void RenderObjectives(void);
	void renderResults(void);
	void handleVehicleClick(int32_t ID);
	void renderHelpText(void);
	void renderInfoTab(void);
	void renderChatText(void);

	void initStatics(FitIniFile& file);
	void initRects(FitIniFile& file);

	void renderPlayerStatus(float delta);

	MPStatsEntry mpStats[9];

	bool chatIsTeamOnly;
	bool bServerWarningShown;

public:
	RectInfo* getRect(int32_t id)
	{
		if ((id >= 0) && (id < rectCount))
		{
			return &(rectInfos[id]);
		}
		if (id == RPTOTAL_CALLOUT)
		{
			return &rpCallout;
		}
		return nullptr;
	}
};

#endif // end of file ( ControlGui.h )
