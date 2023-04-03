//--------------------------------------------------------------------------------------
//
// MechCommander 2
//
// This header contains the mission classes for the GUI
//
// GUI is now single update driven.  An event comes in, the manager decides who
// its for and passes the event down.  Eveything still know how to draw etc.
//
// All drawing is done through gos_drawQuad and drawTriangle
//
// Basic cycle is
//		call GObject->update with this frame's events.
//			This will check the events to see if any pertain to me.
//			Draw anything to the texture plane that needs drawing.
//			Call any code which the events might have trigged.
//		call GObject->render with this frame's events.
//			This draws the object to the screen.
//			called in order of depth.
//
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//
#include "stdinc.h"

//--------------------------------------------------------------------------------------
// Include Files
#ifndef MISSIONGUI_H
#include "missiongui.h"
#endif

#ifndef OBJMGR_H
#include "objmgr.h"
#endif

#include "gamesound.h"
#ifndef SOUNDS_H
#include "sounds.h"
#endif

#ifndef MULTPLYR_H
#include "multplyr.h"
#endif

#ifndef TEAM_H
#include "team.h"
#endif

#ifndef GAMECAM_H
#include "gamecam.h"
#endif

#ifndef VERTEX_H
#include "Vertex.h"
#endif

#ifndef MECH_H
#include "Mech.h"
#endif

#ifndef ARTLRY_H
#include "artlry.h"
#endif

#ifndef LOGISTICSPILOT_H
#include "LogisticsPilot.h"
#endif

#ifndef LOGISTICSDATA_H
#include "LogisticsData.h"
#endif

#ifndef MISSION_H
#include "Mission.h"
#endif

#ifndef GROUP_H
#include "group.h"
#endif

#ifndef MOVEMGR_H
#include "movemgr.h"
#endif

#ifndef COMNDR_H
#include "comndr.h"
#endif

#ifndef PREFS_H
#include "Prefs.h"
#endif

#ifndef KEYBOARDREF_H
#include "KeyboardRef.h"
#endif

#include "resource.h"

#include "windows.h"

#include "gvehicl.h" // remove

static std::wstring_view terrainStr[NUM_TERRAIN_TYPES] = {
	"Blue Water", // MC_BLUEWATER_TYPE
	"Green Water", // MC_GREEN_WATER_TYPE
	"Mud", // MC_MUD_TYPE
	"Moss", // MC_MOSS_TYPE
	"Dirt", // MC_DIRT_TYPE
	"Ash", // MC_ASH_TYPE
	"Mountain", // MC_MOUNTAIN_TYPE
	"Tundra", // MC_TUNDRA_TYPE
	"Forest", // MC_FORESTFLOOR_TYPE
	"Grass", // MC_GRASS_TYPE
	"Concrete", // MC_CONCRETE_TYPE
	"Cliff", // MC_CLIFF_TYPE
	"Slimy", // MC_SLIMY_TYPE
	"None" // MC_NONE_TYPE
};

extern void
testKeyStuff();

extern uint32_t helpTextHeaderID;
extern uint32_t helpTextID;
extern CPrefs prefs;

extern bool invulnerableON;
extern bool MLRVertexLimitReached;

//--------------------------------------------------------------------------------------
// Globals
extern float frameLength;

extern void
DEBUGWINS_print(std::wstring_view s, int32_t window = 0);
extern void
DEBUGWINS_setGameObject(int32_t debugObj, GameObjectPtr obj);
extern void
DEBUGWINS_toggle(bool* windowsOpen);
extern void
DEBUGWINS_display(bool* windowsOpen);
extern void
DEBUGWINS_viewGameObject(int32_t debugObj);
extern MidLevelRenderer::MLRClipper* theClipper;

extern GameObjectPtr DebugGameObject[3];
#ifndef FINAL
extern float CheatHitDamage;
#endif

float MissionInterfaceManager::pauseWndVelocity = -50.f;

//--------------------------------------------------------------------------------------
// class MissionInterfaceManager : public InterfaceManager
//--------------------------------------------------------------------------------------

#define CTRL 0x10000000
#define SHIFT 0x01000000
#define ALT 0x00100000
#define WAYPT 0x20000000

#define ATTILA_FACTOR 1.0f
#define ATTILA_ROTATION_FACTOR 4.1f
#define ATTILA_THRESHOLD 0.01f

int32_t MissionInterfaceManager::mouseX = 0;
int32_t MissionInterfaceManager::mouseY = 0;
GameObject* MissionInterfaceManager::target = nullptr;
MissionInterfaceManager* MissionInterfaceManager::s_instance = nullptr;
gosEnum_KeyIndex MissionInterfaceManager::WAYPOINT_KEY = (gosEnum_KeyIndex)-1;

extern bool drawTerrainTiles;
extern bool drawTerrainOverlays;
extern bool renderObjects;
extern bool drawTerrainGrid;
extern bool drawLOSGrid;
extern bool useClouds;
extern bool useFog;
extern bool useWaterInterestTexture;
extern bool useShadows;
extern bool useFaceLighting;
extern bool useVertexLighting;
extern bool renderTGLShapes;
extern bool useLeftRightMouseProfile;
bool drawGUIOn = true; // Used to shut off GUI for Screen Shots and Movie Mode
bool toggledGUI = true; // Used to tell if I shut the GUI off or if its in movieMode
int32_t lightState = 0;
extern bool ShowMovers;
extern bool CullPathAreas;
extern bool ZeroHPrime;
extern bool CalcValidAreaTable;
extern bool EnemiesGoalPlan;
bool paintingMyVtol = false;

int32_t MissionInterfaceManager::OldKeys[MAX_COMMAND] = {-1};

MissionInterfaceManager::Command MissionInterfaceManager::commands[MAX_COMMAND] = {KEY_S,
	mState_SHRTRNG_ATTACK, mState_SHRTRNG_LOS, false, &MissionInterfaceManager::attackShort,
	&MissionInterfaceManager::defaultAttack, 43200, KEY_M, mState_MEDRNG_ATTACK, mState_MEDRNG_LOS,
	false, &MissionInterfaceManager::attackMedium, &MissionInterfaceManager::defaultAttack, 43201,
	KEY_L, mState_LONGRNG_ATTACK, mState_LONGRNG_LOS, false, &MissionInterfaceManager::attackLong,
	&MissionInterfaceManager::defaultAttack, 43202, KEY_A, mState_ENERGY_WEAPONS,
	mState_ENERGY_WEAPONS_LOS, false, &MissionInterfaceManager::energyWeapons,
	&MissionInterfaceManager::energyWeapons, 43204, KEY_D, mState_GENERIC_ATTACK, mState_ATTACK_LOS,
	false, &MissionInterfaceManager::defaultAttack, 0, -1, KEY_J, mState_JUMP1, mState_JUMP_LOS,
	false, &MissionInterfaceManager::jump, &MissionInterfaceManager::stopJump, 43205, KEY_J | WAYPT,
	mState_JUMPWAYPT, mState_JUMPWAYPT_LOS, false, &MissionInterfaceManager::jump,
	&MissionInterfaceManager::stopJump, -1, KEY_C, mState_CURPOS_ATTACK, mState_CURPOS_ATTACK_LOS,
	false, &MissionInterfaceManager::fireFromCurrentPos,
	&MissionInterfaceManager::stopFireFromCurrentPos, 43206, KEY_G, mState_GUARD, mState_GUARD,
	false, &MissionInterfaceManager::guard, &MissionInterfaceManager::stopGuard, 43207, KEY_W,
	mState_ENERGY_WEAPONS, mState_ENERGY_WEAPONS_LOS, false, &MissionInterfaceManager::conserveAmmo,
	0, -1, KEY_E, -1, -1, false, &MissionInterfaceManager::selectVisible, 0, 43209, KEY_F, -1, -1,
	false, &MissionInterfaceManager::forceShot, 0, 43210, KEY_HOME, -1, -1, true,
	&MissionInterfaceManager::cameraNormal, 0, -1, KEY_F2, -1, -1, true,
	&MissionInterfaceManager::cameraDefault, 0, -1, KEY_F3, -1, -1, true,
	&MissionInterfaceManager::cameraMaxIn, 0, -1, KEY_F4, -1, -1, true,
	&MissionInterfaceManager::cameraTight, 0, -1, KEY_F5, -1, -1, true,
	&MissionInterfaceManager::cameraFour, 0, -1, KEY_F2 | CTRL, -1, -1, true,
	&MissionInterfaceManager::cameraAssign0, 0, -1, KEY_F3 | CTRL, -1, -1, true,
	&MissionInterfaceManager::cameraAssign1, 0, -1, KEY_F4 | CTRL, -1, -1, true,
	&MissionInterfaceManager::cameraAssign2, 0, -1, KEY_F5 | CTRL, -1, -1, true,
	&MissionInterfaceManager::cameraAssign3, 0, -1, KEY_NUMPAD2, mState_DONT,
	mState_AIMED_ATTACK_LOS, false, &MissionInterfaceManager::aimLeg, 0, 43211, KEY_NUMPAD5,
	mState_DONT, mState_AIMED_ATTACK_LOS, false, &MissionInterfaceManager::aimArm, 0, 43212,
	KEY_NUMPAD8, mState_DONT, mState_AIMED_ATTACK_LOS, false, &MissionInterfaceManager::aimHead, 0,
	43213, KEY_BACK, -1, -1, false, &MissionInterfaceManager::removeCommand, 0, 43214,
	KEY_NEXT | 0x100, -1, -1, false, &MissionInterfaceManager::powerDown, 0, 43215,
	KEY_PRIOR | 0x100, -1, -1, false, &MissionInterfaceManager::powerUp, 0, 43216, KEY_END | 0x100,
	mState_EJECT, mState_EJECT, false, &MissionInterfaceManager::eject, 0, 43217, KEY_SPACE,
	mState_RUN, mState_RUN_LOS, false, &MissionInterfaceManager::changeSpeed,
	&MissionInterfaceManager::stopChangeSpeed, 43218, KEY_SPACE | WAYPT, mState_RUNWAYPT,
	mState_RUNWAYPT_LOS, false, &MissionInterfaceManager::changeSpeed,
	&MissionInterfaceManager::stopChangeSpeed, -1, KEY_UP | 0x100, -1, -1, false,
	&MissionInterfaceManager::scrollUp, 0, IDS_HOTKEY_TRACKU, KEY_DOWN | 0x100, -1, -1, false,
	&MissionInterfaceManager::scrollDown, 0, IDS_HOTKEY_TRACKD, KEY_LEFT | 0x100, -1, -1, false,
	&MissionInterfaceManager::scrollLeft, 0, IDS_HOTKEY_TRACKL, KEY_RIGHT | 0x100, -1, -1, false,
	&MissionInterfaceManager::scrollRight, 0, IDS_HOTKEY_TRACKR, KEY_SUBTRACT, -1, -1, false,
	&MissionInterfaceManager::zoomOut, 0, 43225, KEY_MINUS, -1, -1, false,
	&MissionInterfaceManager::zoomOut, 0, -1, KEY_ADD, -1, -1, false,
	&MissionInterfaceManager::zoomIn, 0, 43224, SHIFT | KEY_EQUALS, -1, -1, false,
	&MissionInterfaceManager::zoomIn, 0, -1, KEY_LEFT | SHIFT | 0x100, -1, -1, false,
	&MissionInterfaceManager::rotateLeft, 0, IDS_HOTKEY_ROTATEL, KEY_RIGHT | SHIFT | 0x100, -1, -1,
	false, &MissionInterfaceManager::rotateRight, 0, IDS_HOTKEY_ROTATER, KEY_UP | SHIFT | 0x100, -1,
	-1, false, &MissionInterfaceManager::tiltUp, 0, IDS_HOTKEY_TILTU, KEY_DOWN | SHIFT | 0x100, -1,
	-1, false, &MissionInterfaceManager::tiltDown, 0, IDS_HOTKEY_TILTD, KEY_HOME | 0x100, -1, -1,
	false, &MissionInterfaceManager::centerCamera, 0, -1, KEY_LEFT | CTRL, -1, -1, false,
	&MissionInterfaceManager::rotateLightLeft, 0, -1, KEY_RIGHT | CTRL, -1, -1, false,
	&MissionInterfaceManager::rotateLightRight, 0, -1, KEY_UP | CTRL, -1, -1, false,
	&MissionInterfaceManager::rotateLightUp, 0, -1, KEY_DOWN | CTRL, -1, -1, false,
	&MissionInterfaceManager::rotateLightDown, 0, -1, KEY_T | CTRL | ALT, -1, -1, true,
	&MissionInterfaceManager::drawTerrain, 0, -1, KEY_O | CTRL | ALT, -1, -1, true,
	&MissionInterfaceManager::drawOverlays, 0, -1, KEY_B | CTRL | ALT, -1, -1, true,
	&MissionInterfaceManager::drawBuildings, 0, -1, CTRL | ALT | KEY_G, -1, -1, true,
	&MissionInterfaceManager::showGrid, 0, -1, CTRL | ALT | KEY_Q, -1, -1, true,
	&MissionInterfaceManager::recalcLights, 0, -1, CTRL | ALT | KEY_C, -1, -1, true,
	&MissionInterfaceManager::drawClouds, 0, -1, CTRL | ALT | KEY_F, -1, -1, true,
	&MissionInterfaceManager::drawFog, 0, -1, CTRL | ALT | KEY_P, -1, -1, true,
	&MissionInterfaceManager::usePerspective, 0, -1, CTRL | ALT | KEY_S, -1, -1, true,
	&MissionInterfaceManager::drawTGLShapes, 0, -1, CTRL | ALT | KEY_V, -1, -1, true,
	&MissionInterfaceManager::drawWaterEffects, 0, -1, CTRL | ALT | KEY_W, -1, -1, true,
	&MissionInterfaceManager::recalcWater, 0, -1, CTRL | ALT | KEY_D, -1, -1, true,
	&MissionInterfaceManager::drawShadows, 0, -1, CTRL | ALT | KEY_L, -1, -1, true,
	&MissionInterfaceManager::changeLighting, 0, -1, CTRL | ALT | KEY_Z, -1, -1, true,
	&MissionInterfaceManager::toggleGUI, 0, -1, KEY_V, -1, -1, true,
	&MissionInterfaceManager::vehicleCommand, 0, 43222, KEY_F9, -1, -1, true,
	&MissionInterfaceManager::showObjectives, 0, 43226, KEY_ESCAPE, -1, -1, true,
	&MissionInterfaceManager::togglePause, 0, 43234, KEY_F1, -1, -1, true,
	&MissionInterfaceManager::toggleHotKeys, 0, -1, KEY_P, -1, -1, true,
	&MissionInterfaceManager::togglePause, 0, -1, KEY_TAB, -1, -1, true,
	&MissionInterfaceManager::switchTab, 0, -1, KEY_TAB | SHIFT, -1, -1, true,
	&MissionInterfaceManager::reverseSwitchTab, 0, -1, KEY_I, -1, -1, true,
	&MissionInterfaceManager::infoCommand, &MissionInterfaceManager::infoButtonReleased, 43219,
	KEY_N, -1, -1, true, &MissionInterfaceManager::gotoNextNavMarker, 0, -1, KEY_MULTIPLY,
	mState_UNCERTAIN_AIRSTRIKE, mState_AIRSTRIKE, true, &MissionInterfaceManager::sendAirstrike, 0,
	43220, KEY_DIVIDE, mState_SENSORSTRIKE, mState_SENSORSTRIKE, true,
	&MissionInterfaceManager::sendSensorStrike, 0, 43221, KEY_BACKSLASH, -1, -1, true,
	&MissionInterfaceManager::toggleCompass, 0, 43223, ALT | KEY_SLASH, -1, -1, false,
	&MissionInterfaceManager::quickDebugInfo, 0, -1, ALT | SHIFT | KEY_SLASH, -1, -1, false,
	&MissionInterfaceManager::setGameObjectWindow, 0, -1, ALT | CTRL | KEY_1, -1, -1, false,
	&MissionInterfaceManager::pageGameObjectWindow1, 0, -1, ALT | CTRL | KEY_2, -1, -1, false,
	&MissionInterfaceManager::pageGameObjectWindow2, 0, -1, ALT | CTRL | KEY_3, -1, -1, false,
	&MissionInterfaceManager::pageGameObjectWindow3, 0, -1, ALT | KEY_1, -1, -1, false,
	&MissionInterfaceManager::jumpToDebugGameObject1, 0, -1, ALT | KEY_2, -1, -1, false,
	&MissionInterfaceManager::jumpToDebugGameObject2, 0, -1, ALT | KEY_3, -1, -1, false,
	&MissionInterfaceManager::jumpToDebugGameObject3, 0, -1, ALT | KEY_T, -1, -1, false,
	&MissionInterfaceManager::teleport, 0, -1, ALT | KEY_W, -1, -1, false,
	&MissionInterfaceManager::toggleDebugWins, 0, -1, ALT | KEY_M, -1, -1, false,
	&MissionInterfaceManager::showMovers, 0, -1, ALT | KEY_C, -1, -1, false,
	&MissionInterfaceManager::cullPathAreas, 0, -1, ALT | KEY_Z, -1, -1, false,
	&MissionInterfaceManager::zeroHPrime, 0, -1, ALT | KEY_A, -1, -1, false,
	&MissionInterfaceManager::calcValidAreaTable, 0, -1, ALT | KEY_G, -1, -1, false,
	&MissionInterfaceManager::globalMapLog, 0, -1, ALT | KEY_B, -1, -1, false,
	&MissionInterfaceManager::brainDead, 0, -1, ALT | KEY_P, -1, -1, false,
	&MissionInterfaceManager::goalPlan, 0, -1, ALT | KEY_V, -1, -1, false,
	&MissionInterfaceManager::showVictim, 0, -1, ALT | CTRL | KEY_P, -1, -1, false,
	&MissionInterfaceManager::enemyGoalPlan, 0, -1, ALT | KEY_4, -1, -1, false,
	&MissionInterfaceManager::damageObject1, 0, -1, ALT | KEY_5, -1, -1, false,
	&MissionInterfaceManager::damageObject2, 0, -1, ALT | KEY_6, -1, -1, false,
	&MissionInterfaceManager::damageObject3, 0, -1, ALT | KEY_7, -1, -1, false,
	&MissionInterfaceManager::damageObject4, 0, -1, ALT | KEY_8, -1, -1, false,
	&MissionInterfaceManager::damageObject5, 0, -1, ALT | KEY_9, -1, -1, false,
	&MissionInterfaceManager::damageObject6, 0, -1, ALT | KEY_0, -1, -1, false,
	&MissionInterfaceManager::damageObject0, 0, -1,
	// ALT | KEY_8,	-1, -1,					false,
	// &MissionInterfaceManager::damageObject8, 0, -1,  ALT | KEY_9,	-1, -1,
	// false,		&MissionInterfaceManager::damageObject9, 0, -1,  ALT |
	// KEY_0,	-1, -1,					false,
	// &MissionInterfaceManager::damageObject0, 0, -1,
	KEY_H, -1, -1, true, &MissionInterfaceManager::toggleHoldPosition, 0, -1, KEY_RETURN, -1, -1,
	true, &MissionInterfaceManager::handleChatKey, 0, IDS_HOTKEY_CHAT, SHIFT | KEY_RETURN, -1, -1,
	true, &MissionInterfaceManager::handleTeamChatKey, 0, IDS_HOTKEY_CHAT_TEAM, KEY_E | SHIFT, -1,
	-1, false, &MissionInterfaceManager::addVisibleToSelection, 0, -1, KEY_EQUALS, -1, -1, false,
	&MissionInterfaceManager::zoomIn, 0, -1, ALT | KEY_PERIOD, -1, -1, false,
	&MissionInterfaceManager::rotateObjectLeft, 0, -1, ALT | KEY_COMMA, -1, -1, false,
	&MissionInterfaceManager::rotateObjectRight, 0, -1, KEY_PAUSE, -1, -1, true,
	&MissionInterfaceManager::togglePause, 0, -1

};

extern bool drawTerrainGrid;
extern int32_t turn; // What frame of the scenario is it?

extern wchar_t DebugStatusBarString[256];

void MissionInterfaceManager::init(void)
{
	realRotation = 0.0;
	dragStart.Zero();
	dragEnd.Zero();
	isDragging = FALSE;
	terrainLineChanged = 0;
	for (size_t i = 0; i < MAX_TEAMS; i++)
	{
		vTol[i] = 0;
		paintingVtol[i] = 0;
		vTolTime[i] = 0.0f;
		dustCloud[i] = nullptr;
		recoveryBeam[i] = nullptr;
		mechRecovered[i] = false;
		mechToRecover[i] = 0;
		vehicleID[i] = 0;
	}
	bPaused = 0;
	bPausedWithoutMenu = 0;
	resolution = Environment.screenwidth;
	s_instance = this;
	bEnergyWeapons = 0;
	zoomChoice = 2;
	memset(oldTargets, 0, sizeof(GameObject*) * MAX_ICONS);
	bDrawHotKeys = 0;
	hotKeyFont.init(IDS_HOT_KEY_FONT);
	hotKeyHeaderFont.init(IDS_HOT_KEY_HEADER_FONT);
	lastUpdateDoubleClick = 0;
	keyboardRef = new KeyboardRef;
	animationRunning = false;
	timeLeftToScroll = 0.0f;
	targetButtonId = -1;
	buttonNumFlashes = 0;
	guiFrozen = false;
	reinforcement = nullptr;
	bForcedShot = false;
	bAimedShot = false;
}

#define TACMAP_ID 0
#define FLASH_JUMPERS 31798
bool MissionInterfaceManager::startAnimation(
	int32_t buttonId, bool isButton, bool isPressed, float timeToScroll, int32_t numFlashes)
{
	if (animationRunning)
		return false;
	else
	{
		animationRunning = true;
		timeLeftToScroll = timeToScroll;
		targetButtonId = buttonId;
		buttonNumFlashes = numFlashes;
		targetIsButton = isButton;
		targetIsPressed = isPressed;
		buttonFlashTime = 0.0f;
		if (buttonId < 0) // It means flash TacMap Objective using the ABS of
			// this objective number
		{
			controlGui.animateTacMap(buttonId, timeToScroll, numFlashes);
			animationRunning = true;
			timeLeftToScroll = timeToScroll;
			targetButtonId = TACMAP_ID;
			buttonNumFlashes = numFlashes;
			targetIsButton = false;
			targetIsPressed = false;
			buttonFlashTime = 0.0f;
		}
		if (buttonId == RPTOTAL_CALLOUT) // RP total callout on Support Palette
		{
			controlGui.flashRPTotal(numFlashes);
			animationRunning = true;
			timeLeftToScroll = timeToScroll;
			targetButtonId = buttonId;
			buttonNumFlashes = numFlashes;
			targetIsButton = false;
			targetIsPressed = false;
			buttonFlashTime = 0.0f;
		}
		if (buttonId == FLASH_JUMPERS) // Just flash the mech Icons.  Do not do
			// anything else.
		{
			controlGui.forceGroupBar.flashJumpers(numFlashes);
			animationRunning = false;
		}
	}
	return true;
}

static wchar_t tutorialPlayerName[1024];
void MissionInterfaceManager::setTutorialText(std::wstring_view text)
{
	cLoadString(IDS_TUTORIAL, tutorialPlayerName, 1023);
	controlGui.setChatText(tutorialPlayerName, text, 0x00ffffff, 0);
}

void MissionInterfaceManager::update(void)
{
	if (Environment.screenwidth != resolution)
	{
		swapResolutions();
	}
	bForcedShot = false;
	bAimedShot = false;
	if (eye && eye->inMovieMode)
	{
		drawGUIOn = false;
		userInput->mouseOff();
		// Need to check for ESC key so user can end IMI IMMEDIATELY.
		// After that check, just return.  Do NOT update any of the UI!!!!
		if (userInput->getKeyDown(KEY_ESCAPE))
			eye->forceMovieToEnd();
		controlGui.update(isPaused() && !isPausedWithoutMenu(), false);
		return;
	}
	else
	{
		drawGUIOn = toggledGUI;
		userInput->mouseOn();
	}
	if (animationRunning)
	{
		if (targetIsButton)
		{
			// Move mouse to correct position.
			ControlButton* targetButton = controlGui.getButton(targetButtonId);
			if (!targetButton)
			{
				animationRunning = false;
				return;
			}
			userInput->setMouseCursor(mState_TUTORIALS);
			// Get button position.
			float buttonPosX = (targetButton->location[0].x + targetButton->location[1].x + targetButton->location[2].x + targetButton->location[3].x) * 0.25f;
			float buttonPosY = (targetButton->location[0].y + targetButton->location[1].y + targetButton->location[2].y + targetButton->location[3].y) * 0.25f;
			//-------------------
			// Mouse Checks Next
			float realMouseX = userInput->realMouseX();
			float realMouseY = userInput->realMouseY();
			if (timeLeftToScroll > 0.0f)
			{
				float xDistLeft = buttonPosX - realMouseX;
				float yDistLeft = buttonPosY - realMouseY;
				float xDistThisFrame = xDistLeft / timeLeftToScroll * frameLength;
				float yDistThisFrame = yDistLeft / timeLeftToScroll * frameLength;
				userInput->setMousePos(realMouseX + xDistThisFrame, realMouseY + yDistThisFrame);
				timeLeftToScroll -= frameLength;
			}
			else
			{
				userInput->setMousePos(buttonPosX, buttonPosY);
				// We are there.  Start flashing.
				if (buttonNumFlashes)
				{
					buttonFlashTime += frameLength;
					if (buttonFlashTime > .5f)
					{
						controlGui.getButton(targetButtonId)->setcolour(0xffffffff);
						buttonFlashTime = 0.0f;
						buttonNumFlashes--;
					}
					else if (buttonFlashTime > .25f)
					{
						controlGui.getButton(targetButtonId)->setcolour(0xff7f7f7f);
					}
				}
				else
				{
					// Flashing is done.  We now return you to your regularly
					// scheduled program.
					animationRunning = false;
					controlGui.getButton(targetButtonId)->setcolour(0xffffffff);
					if (targetIsPressed)
						controlGui.pushButton(targetButtonId);
				}
			}
		}
		else
		{
			ControlGui::RectInfo* targetButton = controlGui.getRect(targetButtonId);
			if (!targetButton)
			{
				animationRunning = false;
			}
			userInput->setMouseCursor(mState_TUTORIALS);
			// Get button position.
			float buttonPosX = (targetButton->rect.left + targetButton->rect.right) * 0.5f;
			float buttonPosY = (targetButton->rect.top + targetButton->rect.bottom) * 0.5f;
			//-------------------
			// Mouse Checks Next
			float realMouseX = userInput->realMouseX();
			float realMouseY = userInput->realMouseY();
			if (timeLeftToScroll > 0.0f)
			{
				float xDistLeft = buttonPosX - realMouseX;
				float yDistLeft = buttonPosY - realMouseY;
				float xDistThisFrame = xDistLeft / timeLeftToScroll * frameLength;
				float yDistThisFrame = yDistLeft / timeLeftToScroll * frameLength;
				userInput->setMousePos(realMouseX + xDistThisFrame, realMouseY + yDistThisFrame);
				timeLeftToScroll -= frameLength;
			}
			else
			{
				userInput->setMousePos(buttonPosX, buttonPosY);
				// We are there.  Start flashing.
				if (buttonNumFlashes)
				{
					buttonFlashTime += frameLength;
					if (buttonFlashTime > .5f)
					{
						targetButton->color = 0xff000000;
						buttonFlashTime = 0.0f;
						buttonNumFlashes--;
					}
					else if (buttonFlashTime > .25f)
					{
						targetButton->color = 0xffffffff;
					}
				}
				else
				{
					// Flashing is done.  We now return you to your regularly
					// scheduled program.
					animationRunning = false;
					targetButton->color = 0xff000000;
				}
			}
		}
		return; // Don't let anything else in the GUI run!!
	}
	// Tutorial has locked out the gui.  Move mouse to screen center and do
	// nothing else!!
	if (guiFrozen)
	{
		userInput->setMouseCursor(mState_TUTORIALS);
		userInput->setMousePos(Environment.screenwidth * 0.5f, Environment.screenheight * 0.5f);
		controlGui.update(isPaused() && !isPausedWithoutMenu(), false);
		return;
	}
	//---------------------------------------------------
	// Per Andy G.  One check per frame saves log file!
	bool shiftDn = userInput->shift();
	bool altDn = userInput->alt();
	bool ctrlDn = userInput->ctrl();
	// Cheats
	// Comment out here if FINAL
#ifndef FINAL
#define MC2_DAMAGE_HEAD 0
#define MC2_DAMAGE_LARM 1
#define MC2_DAMAGE_RARM 2
#define MC2_DAMAGE_LTORSO 3
#define MC2_DAMAGE_RTORSO 4
#define MC2_DAMAGE_CTORSO 5
#define MC2_DAMAGE_LLEG 6
#define MC2_DAMAGE_RLEG 7
#define MC2_DAMAGE_RLTORSO 8
#define MC2_DAMAGE_RRTORSO 9
#define MC2_DAMAGE_RCTORSO 10
	uint32_t CheatCommand = -1;
	if (userInput->getKeyDown(KEY_1) && shiftDn && ctrlDn && altDn)
		CheatCommand = MC2_DAMAGE_HEAD;
	if (userInput->getKeyDown(KEY_2) && shiftDn && ctrlDn && altDn)
		CheatCommand = MC2_DAMAGE_LARM;
	if (userInput->getKeyDown(KEY_3) && shiftDn && ctrlDn && altDn)
		CheatCommand = MC2_DAMAGE_RARM;
	if (userInput->getKeyDown(KEY_4) && shiftDn && ctrlDn && altDn)
		CheatCommand = MC2_DAMAGE_LTORSO;
	if (userInput->getKeyDown(KEY_5) && shiftDn && ctrlDn && altDn)
		CheatCommand = MC2_DAMAGE_RTORSO;
	if (userInput->getKeyDown(KEY_6) && shiftDn && ctrlDn && altDn)
		CheatCommand = MC2_DAMAGE_CTORSO;
	if (userInput->getKeyDown(KEY_7) && shiftDn && ctrlDn && altDn)
		CheatCommand = MC2_DAMAGE_LLEG;
	if (userInput->getKeyDown(KEY_8) && shiftDn && ctrlDn && altDn)
		CheatCommand = MC2_DAMAGE_RLEG;
	if (userInput->getKeyDown(KEY_9) && shiftDn && ctrlDn && altDn)
		CheatCommand = MC2_DAMAGE_RLTORSO;
	if (userInput->getKeyDown(KEY_0) && shiftDn && ctrlDn && altDn)
		CheatCommand = MC2_DAMAGE_RRTORSO;
	if (userInput->getKeyDown(KEY_MINUS) && shiftDn && ctrlDn && altDn)
		CheatCommand = MC2_DAMAGE_RCTORSO;
	WeaponShotInfo cheatHit;
	switch (CheatCommand)
	{
	case -1:
		break;
	case MC2_DAMAGE_HEAD:
		cheatHit.init(0, -2, CheatHitDamage, MECH_ARMOR_LOCATION_HEAD, 0.0f);
		break;
	case MC2_DAMAGE_LARM:
		cheatHit.init(0, -2, CheatHitDamage, MECH_ARMOR_LOCATION_LARM, 0.0f);
		break;
	case MC2_DAMAGE_RARM:
		cheatHit.init(0, -2, CheatHitDamage, MECH_ARMOR_LOCATION_RARM, 0.0f);
		break;
	case MC2_DAMAGE_LTORSO:
		cheatHit.init(0, -2, CheatHitDamage, MECH_ARMOR_LOCATION_LTORSO, 0.0f);
		break;
	case MC2_DAMAGE_RTORSO:
		cheatHit.init(0, -2, CheatHitDamage, MECH_ARMOR_LOCATION_RTORSO, 0.0f);
		break;
	case MC2_DAMAGE_CTORSO:
		cheatHit.init(0, -2, CheatHitDamage, MECH_ARMOR_LOCATION_CTORSO, 0.0f);
		break;
	case MC2_DAMAGE_LLEG:
		cheatHit.init(0, -2, CheatHitDamage, MECH_ARMOR_LOCATION_LLEG, 0.0f);
		break;
	case MC2_DAMAGE_RLEG:
		cheatHit.init(0, -2, CheatHitDamage, MECH_ARMOR_LOCATION_RLEG, 0.0f);
		break;
	case MC2_DAMAGE_RLTORSO:
		cheatHit.init(0, -2, CheatHitDamage, MECH_ARMOR_LOCATION_RLTORSO, 0.0f);
		break;
	case MC2_DAMAGE_RRTORSO:
		cheatHit.init(0, -2, CheatHitDamage, MECH_ARMOR_LOCATION_RRTORSO, 0.0f);
		break;
	case MC2_DAMAGE_RCTORSO:
		cheatHit.init(0, -2, CheatHitDamage, MECH_ARMOR_LOCATION_RCTORSO, 0.0f);
		break;
	}
	// Call handleWeaponHit for each selected Mech!
	if (CheatCommand != -1)
	{
		Team* pTeam = Team::home;
		for (size_t i = 0; i < pTeam->getRosterSize(); i++)
		{
			Mover* pMover = (Mover*)pTeam->getMover(i);
			if (pMover->isSelected() && pMover->isMech())
			{
				pMover->handleWeaponHit(&cheatHit);
			}
		}
	}
#endif
	if (bDrawHotKeys)
	{
		keyboardRef->update();
		return;
	}
	//-------------------
	// Mouse Checks Next
	mouseX = userInput->getMouseX();
	mouseY = userInput->getMouseY();
	bool bGui = false;
	// check and see if its in the control area
	if (controlGui.inRegion(mouseX, mouseY, isPaused() && !isPausedWithoutMenu()))
	{
		bGui = true;
		if (userInput->isLeftClick() && !userInput->isLeftDrag())
		{
			dragStart.x = 0.f;
			dragStart.y = 0.f;
		}
	}
	else
	{
		helpTextHeaderID = helpTextID = 0;
	}
	if (bGui)
		userInput->setMouseCursor(mState_NORMAL);
	// find out where the mouse is
	Stuff::Vector2DOf<int32_t> mouseXY;
	mouseXY.x = mouseX;
	mouseXY.y = mouseY;
	eye->inverseProject(mouseXY, wPos);
	// find out if this position is passable, has line of sight
	int32_t cellR, cellC;
	bool passable = 1;
	bool lineOfSight = 0;
	if (Terrain::IsGameSelectTerrainPosition(wPos))
	{
		land->worldToCell(wPos, cellR, cellC);
		if (Team::home) // May go nullptr during multiplayer when a player first
			// dies?
			lineOfSight = Team::home->teamLineOfSight(wPos, 0.0f);
	}
	// update buttons and stuff, even if not in region, it draws objectives and
	// stuff
	controlGui.update(isPaused() && !isPausedWithoutMenu(), lineOfSight);
	bool leftClicked =
		(!userInput->isLeftDrag() && !userInput->isRightDrag() && userInput->isLeftClick());
	bool rightClicked =
		(!userInput->isLeftDrag() && !userInput->wasRightDrag() && userInput->rightMouseReleased());
	bool bLeftDouble = userInput->isLeftDoubleClick();
	updateTarget(bGui);
	//------------------------------------
	// Attila (Strategic Commander) Next
	attilaXAxis = userInput->getAttilaXAxis();
	attilaYAxis = userInput->getAttilaYAxis();
	attilaRZAxis = userInput->getAttilaRAxis();
	//-------------------------------
	// Update the Debug Status Bar...
	int32_t row, col;
	land->worldToCell(wPos, row, col);
	sprintf(DebugStatusBarString,
		"TIME: %06d, MOUSE: [%d, %d] %d,%d,%d (%.2f, %.2f, %.2f), PATHMGR: "
		"%02d(%02d)",
		(int32_t)scenarioTime, row, col, GlobalMoveMap[0]->calcArea(row, col),
		GlobalMoveMap[1]->calcArea(row, col), GlobalMoveMap[2]->calcArea(row, col), wPos.x, wPos.y,
		wPos.z, PathManager->m_numpaths, PathManager->m_peakpaths);
	if (MPlayer)
	{
		wchar_t mpStr[256];
		if (MPlayer->isServer())
			sprintf(mpStr, ", MULTIPLAY: %s-(%d)SERVER {%d,%d}", MPlayer->getPlayerName(),
				MPlayer->commanderid, MPlayer->maxReceiveLoad, MPlayer->maxReceiveSize);
		else
			sprintf(mpStr, ", MULTIPLAY: %s-(%d)CLIENT {%d,%d}", MPlayer->getPlayerName(),
				MPlayer->commanderid, MPlayer->maxReceiveLoad, MPlayer->maxReceiveSize);
		strcat(DebugStatusBarString, mpStr);
	}
	if (EnemiesGoalPlan)
		strcat(DebugStatusBarString, " [ENEMIES GOALPLAN]");
	if (ShowMovers)
		strcat(DebugStatusBarString, " [SHOW MOVERS]");
	if (CullPathAreas)
		strcat(DebugStatusBarString, " [CULL PATH AREAS]");
	if (ZeroHPrime)
		strcat(DebugStatusBarString, " [ZERO HPRIME]");
	if (CalcValidAreaTable)
		strcat(DebugStatusBarString, " [CALC VALID AREA]");
	if (GlobalMap::logEnabled)
		strcat(DebugStatusBarString, " [GLOBAL LOG]");
	if (!MechWarrior::brainsEnabled[1])
		strcat(DebugStatusBarString, " [BRAINDEAD: 1]");
	if (bLeftDouble && target && target->isMover() && target->getTeam() == Team::home)
	{
		int32_t forceGroup = -1;
		for (size_t i = 0; i < 10; i++)
		{
			if (((Mover*)target)->isInUnitGroup(i))
			{
				forceGroup = i;
				break;
			}
		}
		if (forceGroup != -1)
			selectForceGroup(forceGroup, true);
		lastUpdateDoubleClick = true;
	}
	Team* pTeam = Team::home;
	int32_t moverCount = 0;
	int32_t nonMoverCount = 0;
	for (size_t i = 0; i < pTeam->getRosterSize(); i++)
	{
		Mover* pMover = (Mover*)pTeam->getMover(i);
		if (pMover->isSelected() && pMover->getCommander()->getId() == Commander::home->getId())
		{
			if (pMover->maxMoveSpeed)
			{
				passable &= pMover->canMoveHere(wPos);
				moverCount++;
			}
			else
				nonMoverCount++;
		}
	}
	if (useLeftRightMouseProfile) // using AOE control style
	{
		if (WAYPOINT_KEY == -1)
			WAYPOINT_KEY = KEY_LCONTROL;
		commandClicked = rightClicked;
		selectClicked = !bLeftDouble && !lastUpdateDoubleClick && userInput->leftMouseReleased() && !userInput->getKeyDown(KEY_T) && !isDragging;
		cameraClicked = gos_GetKeyStatus(KEY_LMENU) == KEY_HELD;
		if (moveCameraAround(lineOfSight, passable, ctrlDn, bGui, moverCount, nonMoverCount))
		{
			bool leftClicked =
				(!userInput->isLeftDrag() && !userInput->isRightDrag() && userInput->isLeftClick());
			bool rightClicked = (!userInput->isLeftDrag() && !userInput->wasRightDrag() && userInput->rightMouseReleased());
			// deal with the hot keys
			update(leftClicked, rightClicked, mouseX, mouseY, target, lineOfSight);
			return;
		}
		updateAOEStyle(
			shiftDn, altDn, ctrlDn, bGui, lineOfSight, passable, moverCount, nonMoverCount);
	}
	else // using mc1 style
	{
		commandClicked = leftClicked;
		selectClicked = leftClicked && !lastUpdateDoubleClick;
		cameraClicked = userInput->isRightDrag();
		if (WAYPOINT_KEY == -1)
			WAYPOINT_KEY = KEY_LCONTROL;
		if (moveCameraAround(lineOfSight, passable, ctrlDn, bGui, moverCount, nonMoverCount))
		{
			bool leftClicked =
				(!userInput->isLeftDrag() && !userInput->isRightDrag() && userInput->isLeftClick());
			bool rightClicked = (!userInput->isLeftDrag() && !userInput->isRightDrag() && userInput->isRightClick());
			// deal with the hot keys
			update(leftClicked, rightClicked, mouseX, mouseY, target, lineOfSight);
			return;
		}
		updateOldStyle(
			shiftDn, altDn, ctrlDn, bGui, lineOfSight, passable, moverCount, nonMoverCount);
	}
	for (i = 0; i < Team::home->getRosterSize(); i++)
	{
		Mover* pMover = (Mover*)Team::home->getMover(i);
		MechWarrior* pilot = pMover->getPilot();
		if (pilot && pMover->getCommander()->getId() == Commander::home->getId())
		{
			GameObject* pTmpTarget = pilot->getCurrentTarget();
			if (pTmpTarget && (i < MAX_ICONS)) // Must check this because old
				// test maps have more then 16
				// movers on them!!
			{
				pTmpTarget->setDrawBars(true);
				oldTargets[i] = pTmpTarget;
			}
		}
	}
	if (!bLeftDouble && !(lastUpdateDoubleClick && userInput->getMouseLeftButtonState() == MC2_MOUSE_DOWN)) // check for the hold )
		lastUpdateDoubleClick = false;
	updateRollovers();
}

void MissionInterfaceManager::updateVTol()
{
	// We're probably going to use this alot!
	int32_t commanderid = Commander::home->getId();
	paintingMyVtol = paintingVtol[commanderid];
	for (size_t vtolNum = 0; vtolNum < MAX_TEAMS; vtolNum++)
	{
		// update the vtol, if it needs it
		if (paintingVtol[vtolNum] && (!bPaused || MPlayer))
		{
			if (vehicleID[vtolNum] != 147) // We are the standard vtol.
			{
				if (!vehicleDropped[vtolNum] && (vTol[vtolNum]->currentFrame >= 145))
				{
					vehicleDropped[vtolNum] = true;
					soundSystem->playDigitalSample(VTOL_DROP, vPos[vtolNum]);
					// OK, if this is another persons vtol, I probably shouldn't
					// do this
					// Glenn, what should it do?
					// Just check my commanderid against the vtolNum and if they
					// match do it? Otherwise do nothing? Help Me, Spock! -fs
					if (MPlayer)
					{
						if (commanderid == vtolNum)
						{
							//--------------------------------------------------------
							// This will get the other machines to enable the
							// mover...
							MPlayer->sendReinforcement(vehicleID[vtolNum],
								MPlayer->reinforcements[commanderid][0], "noname", commanderid,
								vPos[vtolNum], 2);
						}
					}
					else
						addVehicle(vPos[vtolNum]);
				}
				else if (vTol[vtolNum]->currentFrame >= 300)
				{
					paintingVtol[vtolNum] = 0;
					delete vTol[vtolNum];
					vTol[vtolNum] = nullptr;
					if (dustCloud[vtolNum])
					{
						dustCloud[vtolNum]->Kill();
						delete dustCloud[vtolNum];
						dustCloud[vtolNum] = nullptr;
					}
					if (recoveryBeam[vtolNum])
					{
						recoveryBeam[vtolNum]->Kill();
						delete recoveryBeam[vtolNum];
						recoveryBeam[vtolNum] = nullptr;
					}
					// We should have set this when the helo was brought in.
					// Let it takeoff now that the vtol is out of the way.
					if (MPlayer)
					{
						if ((vtolNum < 0) || (vtolNum >= MAX_MC_PLAYERS))
							STOP(("updateVTOL: bad vtolNum"));
						if (MPlayer->reinforcements[vtolNum][0] < 0)
							STOP(("updateVTOL: bad reinforcement"));
						std::unique_ptr<Mover> mover = MPlayer->moverRoster[MPlayer->reinforcements[vtolNum][0]];
						if (mover)
						{
							TacticalOrder tacOrder;
							tacOrder.init(OrderOriginType::player, TacticalOrderCode::powerup, true);
							tacOrder.pack(nullptr, nullptr);
							if (!MPlayer->isServer())
								MPlayer->sendPlayerOrder(&tacOrder, false, 1, &mover);
							else
								mover->handleTacticalOrder(tacOrder);
						}
						MPlayer->reinforcements[vtolNum][0] = -1;
					}
					else if (reinforcement)
					{
						reinforcement->getPilot()->orderPowerUp(true, OrderOriginType::self);
						reinforcement = nullptr;
					}
					if (!MPlayer || vtolNum == MPlayer->commanderid)
						controlGui.unPressAllVehicleButtons();
				}
			}
			else // Update the KARNOV animation
			{
				if ((vTol[vtolNum]->getCurrentGestureId() == 1) && !vTol[vtolNum]->getInTransition())
				{
					vTol[vtolNum]->setGesture(0);
					// Needs to STAY false, probably in MPlayer too because
					// If the mech we are about to recover gets destroyed, we
					// need to abort!! Probably in Multiplayer now its possible
					// to recover even if destroyed!
					if (!MPlayer)
						mechRecovered[vtolNum] = false; // Need to know when
							// mech is done so Karnov
							// can fly away.
					vTolTime[vtolNum] = scenarioTime;
					// Start GOsFX - We are now hovering.
					if (recoveryBeam[vtolNum])
					{
						Stuff::LinearMatrix4D shapeOrigin;
						Stuff::LinearMatrix4D localToWorld;
						Stuff::LinearMatrix4D localResult;
						Stuff::Vector3D dustPos = vTol[vtolNum]->position;
						Stuff::Point3D wakePos;
						wakePos.x = -dustPos.x;
						wakePos.y = dustPos.z;
						wakePos.z = dustPos.y;
						shapeOrigin.BuildRotation(Stuff::EulerAngles(0.0f, 0.0f, 0.0f));
						shapeOrigin.BuildTranslation(wakePos);
						gosFX::Effect::ExecuteInfo info(
							(Stuff::Time)scenarioTime, &shapeOrigin, nullptr);
						recoveryBeam[vtolNum]->SetLoopOff();
						recoveryBeam[vtolNum]->SetExecuteOn();
						recoveryBeam[vtolNum]->Start(&info);
					}
					// Actually the Karnov Deploy Sound Effect!
					soundSystem->playDigitalSample(RADAR_HUM, vTol[vtolNum]->position, false);
				}
				else if ((vTol[vtolNum]->getCurrentGestureId() == 0) && !mechRecovered[vtolNum] && mechToRecover[vtolNum] && !mechToRecover[vtolNum]->isDestroyed())
				{
					if (scenarioTime > vTolTime[vtolNum] + Mover::recoverTime)
					{
						bool doIt = true;
						if (MPlayer)
						{
							if (commanderid == vtolNum)
							{
								//--------------------------------------------------------
								// This will get the other machines to enable
								// the mover...
								MPlayer->sendReinforcement(vehicleID[vtolNum],
									MPlayer->reinforcements[vtolNum][1],
									MPlayer->reinforcementPilot[vtolNum], vtolNum, vPos[vtolNum],
									5);
							}
							else
								doIt = false;
						}
						if (doIt && !mechToRecover[vtolNum]->isDestroyed())
						{
							// Seems like we don't want to do this on every
							// machine, do we?
							// -fs
							// STAY in recover until its done.  I know it
							// shouldn't be possible but trust me, its happening.
							while (((std::unique_ptr<Mover>)mechToRecover[vtolNum])->recover() == false)
								;
							if (mechToRecover[vtolNum]->isDisabled())
							{
								mechToRecover[vtolNum]->setStatus(OBJECT_STATUS_SHUTDOWN, true);
								mechToRecover[vtolNum]->getSensorSystem()->broken = false;
								((std::unique_ptr<Mover>)mechToRecover[vtolNum])->timeLeft = 1.0f;
								((std::unique_ptr<Mover>)mechToRecover[vtolNum])->exploding = false;
							}
							std::wstring_view newPilotName = nullptr;
							if (MPlayer)
								newPilotName = MPlayer->reinforcementPilot[vtolNum];
							else
								newPilotName = (std::wstring_view)LogisticsData::instance->getBestPilot(
									mechToRecover[vtolNum]->tonnage);
							mission->tradeMover(mechToRecover[vtolNum],
								Commander::commanders[vtolNum]->getTeam()->getId(), vtolNum,
								newPilotName, "pbrain");
							mechRecovered[vtolNum] = true;
							mechToRecover[vtolNum]->getPilot()->orderPowerUp(
								true, OrderOriginType::self);
							if (MPlayer)
								MPlayer->reinforcements[vtolNum][1] = -1;
						}
					}
				}
				else if ((vTol[vtolNum]->getCurrentGestureId() == 0) && (mechRecovered[vtolNum] || mechToRecover[vtolNum]->isDestroyed()) && (!vTol[vtolNum]->getInTransition()))
				{
					vTol[vtolNum]->setGesture(2); // Fly Away!!
				}
				else if ((vTol[vtolNum]->getCurrentGestureId() == 2) && (!vTol[vtolNum]->getInTransition()))
				{
					paintingVtol[vtolNum] = 0;
					delete vTol[vtolNum];
					vTol[vtolNum] = nullptr;
					if (dustCloud[vtolNum])
					{
						dustCloud[vtolNum]->Kill();
						delete dustCloud[vtolNum];
						dustCloud[vtolNum] = nullptr;
					}
					if (recoveryBeam[vtolNum])
					{
						recoveryBeam[vtolNum]->Kill();
						delete recoveryBeam[vtolNum];
						recoveryBeam[vtolNum] = nullptr;
					}
					// Check if mech was recovered.  If not, restore the
					// resource points!!!
					// This can happen if we call in a recovery vehicle and the
					// mech is destroyed BEFORE the recovery vehicle recovers
					// it!!
					if (!mechRecovered[vtolNum])
					{
						if (MPlayer)
						{
							if (MPlayer->isServer())
							{
								// MPlayer->playerInfo[vtolNum].resourcePoints
								// += 10000;
								Stuff::Vector3D pos;
								MPlayer->sendReinforcement(10000, 0, "noname", vtolNum, pos, 6);
							}
						}
						else
							LogisticsData::instance->setResourcePoints(
								LogisticsData::instance->getResourcePoints() + 10000);
					}
					if (!MPlayer || vtolNum == MPlayer->commanderid)
						controlGui.unPressAllVehicleButtons();
				}
			}
			if (dustCloud[vtolNum] && dustCloud[vtolNum]->IsExecuted())
			{
				Stuff::LinearMatrix4D shapeOrigin;
				Stuff::LinearMatrix4D localToWorld;
				Stuff::LinearMatrix4D localResult;
				Stuff::Vector3D dustPos = vTol[vtolNum]->position;
				Stuff::Point3D wakePos;
				wakePos.x = -dustPos.x;
				wakePos.y = dustPos.z;
				wakePos.z = dustPos.y;
				shapeOrigin.BuildRotation(Stuff::EulerAngles(0.0f, 0.0f, 0.0f));
				shapeOrigin.BuildTranslation(wakePos);
				Stuff::OBB boundingBox;
				gosFX::Effect::ExecuteInfo info(
					(Stuff::Time)scenarioTime, &shapeOrigin, &boundingBox);
				dustCloud[vtolNum]->Execute(&info);
			}
			if (recoveryBeam[vtolNum] && recoveryBeam[vtolNum]->IsExecuted())
			{
				Stuff::LinearMatrix4D shapeOrigin;
				Stuff::LinearMatrix4D localToWorld;
				Stuff::LinearMatrix4D localResult;
				Stuff::Vector3D dustPos = vTol[vtolNum]->position;
				Stuff::Point3D wakePos;
				wakePos.x = -dustPos.x;
				wakePos.y = dustPos.z;
				wakePos.z = dustPos.y;
				shapeOrigin.BuildRotation(Stuff::EulerAngles(0.0f, 0.0f, 0.0f));
				shapeOrigin.BuildTranslation(wakePos);
				Stuff::OBB boundingBox;
				gosFX::Effect::ExecuteInfo info(
					(Stuff::Time)scenarioTime, &shapeOrigin, &boundingBox);
				recoveryBeam[vtolNum]->Execute(&info);
			}
			if (vTol[vtolNum]) // Might have been deleted above!!
			{
				vTol[vtolNum]->recalcBounds();
				vTol[vtolNum]->update(); // Must update even if not on screen!
			}
		}
		else if (paintingVtol[vtolNum] && bPaused)
		{
			if (vTol[vtolNum]) // Might have been deleted above!!
			{
				int32_t cFrame = vTol[vtolNum]->currentFrame;
				vTol[vtolNum]->recalcBounds();
				vTol[vtolNum]->update(); // Must update even if not on screen!
				// If we are paused, do not animate.
				vTol[vtolNum]->currentFrame = cFrame;
			}
		}
	}
}

void MissionInterfaceManager::updateTarget(bool bGui)
{
	// unset anything that isn't targeted
	for (size_t i = 0; i < MAX_ICONS; i++)
	{
		if (oldTargets[i])
		{
			oldTargets[i]->setDrawBars(0);
			oldTargets[i] = 0;
		}
	}
	// if there was a target, unset it
	if (target)
		target->setTargeted(0);
	//----------------------------------------------------------------------------------------
	// Get Any object we are over.  Change cursor if appropriate.  Set selected
	// if necessary. Clear the target pointer if we have done all we need to do
	// here.  Otherwise, leave it set and issue an order below based on who is
	// in the myForce pointers.
	target = ObjectManager->findObjectByMouse(mouseX, mouseY);
	if (target)
	{
		if (bGui)
			target = 0;
		else if (target->isMover() && !ShowMovers && !(MPlayer && MPlayer->allUnitsDestroyed[MPlayer->commanderid]))
		{
			if ((target->getTeamId() != Team::home->getId()) && !target->isDisabled() && (((Mover*)target)->conStat < CONTACT_SENSOR_QUALITY_1))
				target = nullptr;
		}
		if (target)
		{
			int32_t descID = target->getDescription();
			if (descID != -1)
			{
				int32_t nameID = target->getAppearance()->getObjectNameId();
				helpTextHeaderID = nameID;
				helpTextID = descID;
			}
			if (target->isDestroyed())
			{
				target = 0;
			}
			else if (target->isDisabled() && !target->isMover())
				target = 0;
			else if (!target->isSelectable())
			{
				if (!target->isDisabled() && !target->isMover())
					target = nullptr;
			}
		}
	}
}

void MissionInterfaceManager::drawWayPointPaths()
{
	Team* pTeam = Team::home;
	for (size_t i = 0; i < pTeam->getRosterSize(); i++)
	{
		Mover* pMover = (Mover*)pTeam->getMover(i);
		if (pMover->isSelected() && pMover->getCommander()->getId() == Commander::home->getId())
		{
			pMover->updateDrawWaypointPath();
		}
	}
}

void MissionInterfaceManager::updateOldStyle(bool shiftDn, bool altDn, bool ctrlDn, bool bGui,
	bool lineOfSight, bool passable, int32_t moverCount, int32_t nonMoverCount)
{
	printDebugInfo();
	// We're probably going to use this alot!
	int32_t commanderid = Commander::home->getId();
	// Update the waypoint markers so that they are visible!
	if (userInput->getKeyDown(WAYPOINT_KEY) || controlGui.getMines())
	{
		drawWayPointPaths();
		// Can Never make a patrol path.  Causes movement wubbies!
		//		if ( makePatrolPath() )
		//			return;
	}
	int32_t mState = userInput->getMouseCursor();
	bool leftClicked = (!userInput->isLeftDrag() && !userInput->isRightDrag() && userInput->isLeftClick() && !lastUpdateDoubleClick);
	bool rightClicked =
		(!userInput->isLeftDrag() && !userInput->isRightDrag() && userInput->isRightClick());
	// deal with the hot keys
	if (update(leftClicked, rightClicked, mouseX, mouseY, target, lineOfSight))
		return;
	// special case for bottom of screen
	if (bGui) // don't target or anything if we are int32_t the control panel
	{
		// now check the selection versus the commands
		if (!canJump() && controlGui.getJump())
			controlGui.toggleJump();
		userInput->setMouseCursor(mState);
		if (controlGui.mouseInVehicleStopButton)
			return;
		if (!userInput->isLeftDrag() && !controlGui.isAddingAirstrike() && !controlGui.isAddingVehicle())
			return;
	}
	Team* pTeam = Team::home;
	if (target) // if there is a target, make the appropritate cursor
	{
		userInput->setMouseCursor(makeTargetCursor(lineOfSight, moverCount, nonMoverCount));
	}
	else // make a move cursor
	{
		userInput->setMouseCursor(
			makeNoTargetCursor(passable, lineOfSight, ctrlDn, bGui, moverCount, nonMoverCount));
	}
	if (userInput->leftMouseReleased() && !userInput->wasLeftDrag() && !bGui && !lastUpdateDoubleClick) // move on the mouse ups
	{
		if ((controlGui.isAddingVehicle() && !paintingVtol[commanderid] && canAddVehicle(wPos)) || (controlGui.isAddingSalvage() && !paintingVtol[commanderid] && canRecover(wPos)))
		{
			// Target has already been confirmed  as a mover(BattleMech) by
			// canRecover OR it doesn't matter because the canAddVehicle part of
			// the beginVtol function doesn't reference it! Again, Its OK
			// because we only use it for salvage craft. Need to pass this in so
			// that Multiplayer can pass it in.
			beginVtol(-1, commanderid, nullptr,
				(std::unique_ptr<Mover>)target); // In Single player, this should always be zero?
			return;
		}
		else if ((controlGui.isAddingVehicle() && !paintingVtol[commanderid] && !canAddVehicle(wPos)) || (controlGui.isAddingSalvage() && !paintingVtol[commanderid] && !canRecover(wPos)))
		{
			soundSystem->playDigitalSample(INVALID_GUI);
			return;
		}
		else if (controlGui.isAddingAirstrike() && !paintingVtol[commanderid]) // if we're painting
			// the vtol, carry on
		{
			addAirstrike();
			return;
		}
		else if (!target)
		{
			if (controlGui.isSelectingInfoObject())
			{
				soundSystem->playDigitalSample(INVALID_GUI);
				controlGui.cancelInfo();
			}
			else if (Terrain::IsGameSelectTerrainPosition(wPos))
			{
				if (controlGui.getGuardTower())
					doGuardTower();
				else if (passable || selectionIsHelicopters())
				{
					if (controlGui.getGuard())
					{
						doGuard(target);
					}
					else if (!bForcedShot)
						doMove(wPos);
				}
				else
				{
					userInput->setMouseCursor(mState_DONT);
				}
			}
			else
			{
				userInput->setMouseCursor(mState_DONT);
			}
		}
		else
		// We clicked on a target.  If mouse cursor is normal, we want to select
		// a friendly.  if the mouse cursor is some form of attack, attack the
		// target.
		{
			if (target->isMover() && (target->getTeamId() == Team::home->getId() || CONTACT_VISUAL == ((Mover*)target)->getContactStatus(Team::home->getId(), true) || target->isDisabled()))
			{
				controlGui.setInfoWndMover((Mover*)target);
			}
			if (controlGui.getSalvage())
				doSalvage();
			else if (controlGui.getGuard())
			{
				doGuard(target);
			}
			else if (target->getTeamId() == Team::home->getId() && !target->isDisabled())
			{
				//--------------------------------------------------------------------
				// User wants to select the guy we are on.  If he did
				// it with a shift, add him.  If not, whack all and just add
				// target!
				if (shiftDn && target->getCommanderId() == Commander::home->getId())
				{
					bool alreadyThere = false;
					//-------------------------------------------
					// First, check if he's already there!
					for (size_t i = 0; i < pTeam->getRosterSize(); i++)
					{
						Mover* pMover = (Mover*)pTeam->getMover(i);
						if (pMover == target && pMover->isSelected() && pMover->getCommander()->getId() == Commander::home->getId())
							alreadyThere = true;
					}
					if (!alreadyThere && !target->isDisabled())
						target->setSelected(true);
					else
						target->setSelected(false);
				}
				else if (controlGui.getRepair() && canRepair(target))
				{
					doRepair(target);
				}
				else if (controlGui.getSalvage() && canSalvage(target))
					doSalvage();
				else if (controlGui.getGuardTower())
					doGuardTower();
				else if (target->getTeam() && Team::home->isFriendly(target->getTeam()) && target->getFlag(OBJECT_FLAG_CANREFIT) && target->getFlag(OBJECT_FLAG_MECHBAY) && canRepairBay(target))
					doRepairBay(target);
				else if (controlGui.isSelectingInfoObject() && target->isMover())
					controlGui.setInfoWndMover((Mover*)target);
				else if (target->isMover() && !target->isDisabled() && target->getCommanderId() == Commander::home->getId())
				{
					for (size_t i = 0; i < pTeam->getRosterSize(); i++)
					{
						Mover* pMover = (Mover*)pTeam->getMover(i);
						if (pMover->getCommander()->getId() == Commander::home->getId())
						{
							pMover->setSelected(false);
						}
					}
					target->setSelected(true);
				}
				else
					soundSystem->playDigitalSample(INVALID_GUI);
			}
			else
			{
				if (controlGui.getGuard()) // trying to guard invalid thing,
					// cancel guard
					controlGui.toggleGuard();
				else if (userInput->getMouseCursor() == mState_INFO && target->isMover())
					controlGui.setInfoWndMover((Mover*)target);
				else if (!target->isDisabled() && !bForcedShot && !bAimedShot)
					doAttack();
			}
		}
	}
}
void MissionInterfaceManager::updateAOEStyle(bool shiftDn, bool altDn, bool ctrlDn, bool bGui,
	bool lineOfSight, bool passable, int32_t moverCount, int32_t nonMoverCount)
{
	printDebugInfo();
	// We're probably going to use this alot!
	int32_t commanderid = Commander::home->getId();
	// Update the waypoint markers so that they are visible!
	if (userInput->getKeyDown(WAYPOINT_KEY) || controlGui.getMines())
	{
		drawWayPointPaths();
		// Can Never make a patrol path.  Causes movement wubbies!
		//		if ( makePatrolPath() )
		//			return;
	}
	int32_t mState = userInput->getMouseCursor();
	bool leftClicked =
		(!userInput->isLeftDrag() && !userInput->isRightDrag() && userInput->isLeftClick());
	bool rightClicked =
		(!userInput->isLeftDrag() && !userInput->wasRightDrag() && userInput->rightMouseReleased());
	// deal with the hot keys
	if (update(leftClicked, rightClicked, mouseX, mouseY, target, lineOfSight))
		return;
	// special case for bottom of screen
	if (bGui) // don't target or anything if we are int32_t the control panel
	{
		// now check the selection versus the commands
		if (!canJump() && controlGui.getJump())
			controlGui.toggleJump();
		userInput->setMouseCursor(mState);
		if (controlGui.mouseInVehicleStopButton)
			return;
		if (!userInput->isLeftDrag() && !controlGui.isAddingAirstrike() && !controlGui.isAddingVehicle() && !controlGui.isAddingSalvage())
			return;
	}
	Team* pTeam = Team::home;
	if (target) // if there is a target, make the appropritate cursor
	{
		userInput->setMouseCursor(makeTargetCursor(lineOfSight, moverCount, nonMoverCount));
	}
	else // make a move cursor
	{
		userInput->setMouseCursor(
			makeNoTargetCursor(passable, lineOfSight, ctrlDn, bGui, moverCount, nonMoverCount));
	}
	if (userInput->rightMouseReleased() && !userInput->wasRightDrag() && !bGui) // move on the mouse ups
	{
		if ((controlGui.isAddingVehicle() && !paintingVtol[commanderid] && canAddVehicle(wPos)) || (controlGui.isAddingSalvage() && !paintingVtol[commanderid] && canRecover(wPos)))
		{
			// Target has already been confirmed  as a mover(BattleMech) by
			// canRecover OR it doesn't matter because the canAddVehicle part of
			// the beginVtol function doesn't reference it! Again, Its OK
			// because we only use it for salvage craft. Need to pass this in so
			// that Multiplayer can pass it in.
			beginVtol(-1, commanderid, nullptr,
				(std::unique_ptr<Mover>)target); // In Single player, this should always be zero?
			return;
		}
		else if ((controlGui.isAddingVehicle() && !paintingVtol[commanderid] && !canAddVehicle(wPos)) || (controlGui.isAddingSalvage() && !paintingVtol[commanderid] && !canRecover(wPos)))
		{
			soundSystem->playDigitalSample(INVALID_GUI);
			return;
		}
		else if (controlGui.isAddingAirstrike() && !paintingVtol[commanderid]) // if we're painting
			// the vtol, carry on
		{
			addAirstrike();
			return;
		}
		else if (!target)
		{
			if (controlGui.isSelectingInfoObject())
			{
				soundSystem->playDigitalSample(INVALID_GUI);
				controlGui.cancelInfo();
			}
			else if (Terrain::IsGameSelectTerrainPosition(wPos))
			{
				if (passable || selectionIsHelicopters())
				{
					if (controlGui.getGuardTower())
						doGuardTower();
					else if (controlGui.getGuard())
					{
						doGuard(nullptr);
					}
					else
						doMove(wPos);
				}
				else
				{
					userInput->setMouseCursor(mState_DONT);
				}
			}
			else
			{
				userInput->setMouseCursor(mState_DONT);
			}
		}
		else if (controlGui.getSalvage())
			doSalvage();
		else if (controlGui.getGuard())
			doGuard(target);
		else if (controlGui.getRepair() && canRepair(target))
			doRepair(target);
		else if (controlGui.getGuardTower())
			doGuardTower();
		else if (target->getTeam() && Team::home->isFriendly(target->getTeam()) && target->getFlag(OBJECT_FLAG_CANREFIT) && target->getFlag(OBJECT_FLAG_MECHBAY) && canRepairBay(target))
			doRepairBay(target);
		else if (controlGui.getSalvage() && canSalvage(target))
			doSalvage();
		else if (controlGui.isSelectingInfoObject() && target->isMover())
			controlGui.setInfoWndMover((Mover*)target);
		else if (target->getTeam() != Team::home && !target->isDisabled())
			doAttack();
		else if (!bForcedShot && !bAimedShot)
			soundSystem->playDigitalSample(INVALID_GUI);
	}
	else if (selectClicked && !bGui)
	{
		// We clicked on a target.  If mouse cursor is normal, we want to select
		// a friendly.  if the mouse cursor is some form of attack, attack the
		// target.
		if ((controlGui.isAddingVehicle() && !paintingVtol[commanderid] && canAddVehicle(wPos)) || (controlGui.isAddingSalvage() && !paintingVtol[commanderid] && canRecover(wPos)))
		{
			// Target has already been confirmed  as a mover(BattleMech) by
			// canRecover. OR it doesn't matter because the canAddVehicle part
			// of the beginVtol function doesn't reference it! Again, Its OK
			// because we only use it for salvage craft. Need to pass this in so
			// that Multiplayer can pass it in.
			beginVtol(-1, commanderid, nullptr,
				(std::unique_ptr<Mover>)target); // In Single player, this should always be zero?
			return;
		}
		else if ((controlGui.isAddingVehicle() && !paintingVtol[commanderid] && !canAddVehicle(wPos)) || (controlGui.isAddingSalvage() && !paintingVtol[commanderid] && !canRecover(wPos)))
		{
			soundSystem->playDigitalSample(INVALID_GUI);
			return;
		}
		else if (controlGui.isAddingAirstrike() && !paintingVtol[commanderid]) // if we're painting
			// the vtol, carry on
		{
			addAirstrike();
			return;
		}
		else if (target)
		{
			if (target->isMover() && (target->getTeamId() == Team::home->getId() || CONTACT_VISUAL == ((Mover*)target)->getContactStatus(Team::home->getId(), true) || target->isDisabled()))
			{
				controlGui.setInfoWndMover((Mover*)target);
			}
			if (target->getCommanderId() == Commander::home->getId() && !target->isDisabled())
			{
				//--------------------------------------------------------------------
				// User wants to select the guy we are on.  If he did
				// it with a shift, add him.  If not, whack all and just add
				// target!
				if (shiftDn)
				{
					bool alreadyThere = false;
					//-------------------------------------------
					// First, check if he's already there!
					for (size_t i = 0; i < pTeam->getRosterSize(); i++)
					{
						Mover* pMover = (Mover*)pTeam->getMover(i);
						if (pMover == target && pMover->isSelected() && pMover->getCommander()->getId() == Commander::home->getId())
							alreadyThere = true;
					}
					if (!alreadyThere && !target->isDisabled())
						target->setSelected(true);
					else
						target->setSelected(false);
				}
				else if (target->isMover())
				{
					for (size_t i = 0; i < pTeam->getRosterSize(); i++)
					{
						Mover* pMover = (Mover*)pTeam->getMover(i);
						if (pMover->getCommander()->getId() == Commander::home->getId())
						{
							pMover->setSelected(false);
						}
					}
					target->setSelected(true);
				}
			}
		}
		else
		{
			// tried to select nothing
			controlGui.cancelInfo();
			soundSystem->playDigitalSample(INVALID_GUI);
		}
	}
}

void MissionInterfaceManager::updateWaypoints(void)
{
	// Update the waypoint markers so that they are visible and must happen
	// AFTER camera update!! Or they wiggle something fierce.
	if (userInput->getKeyDown(WAYPOINT_KEY) || controlGui.getMines())
	{
		Team* pTeam = Team::home;
		for (size_t i = 0; i < pTeam->getRosterSize(); i++)
		{
			Mover* pMover = (Mover*)pTeam->getMover(i);
			if (pMover->isSelected() && pMover->getCommander()->getId() == Commander::home->getId())
			{
				pMover->updateDrawWaypointPath();
			}
		}
	}
}

int32_t
MissionInterfaceManager::update(bool leftClickedClick, bool rightClickedClick,
	int32_t MouseX, int32_t MouseY, GameObject* pTarget, bool bLOS)
{
	bool shiftDn = userInput->shift();
	bool ctrlDn = userInput->ctrl();
	bool altDn = userInput->alt();
	bool wayPtDn = userInput->getKeyDown(WAYPOINT_KEY);
	bool bRetVal = 0;
	int32_t i = 0;
	int32_t last = MAX_COMMAND;
	// if chatting, ignore keyboard input
	if (controlGui.updateChat())
	{
		i = 100;
		last = 102;
		bRetVal = 1;
	}
	if (gos_GetKeyStatus(WAYPOINT_KEY) == KEY_RELEASED)
	{
		Team* pTeam = Team::home;
		for (size_t i = 0; i < pTeam->getRosterSize(); i++)
		{
			Mover* pMover = (Mover*)pTeam->getMover(i);
			if (pMover->isSelected() && !pMover->getPilot()->getExecutingTacOrderQueue() && pMover->getCommander()->getId() == Commander::home->getId())
			{
				if (MPlayer && !MPlayer->isServer())
				{
					TacticalOrder tacOrder;
					tacOrder.init(OrderOriginType::player, TacticalOrderCode::waypointsdone);
					tacOrder.pack(nullptr, nullptr);
					MPlayer->sendPlayerOrder(&tacOrder, false, 1, &pMover);
				}
				else
				{
					pMover->getPilot()->setExecutingQueue(true);
					pMover->getPilot()->executeTacOrderQueue();
				}
			}
		}
		bRetVal = 1;
	}
	mouseX = MouseX;
	mouseY = mouseY;
	setTarget(pTarget);
	if (!bRetVal)
	{
		for (size_t j = KEY_0; j < KEY_9 + 1; j++)
		{
			if (userInput->getKeyDown((gosEnum_KeyIndex)j) && gos_GetKeyStatus((gosEnum_KeyIndex)j) != KEY_HELD)
			{
				if (ctrlDn)
					makeForceGroup(j - KEY_0);
				else if (shiftDn)
					selectForceGroup(j - KEY_0, 0);
				else
					selectForceGroup(j - KEY_0, 1);
			}
		}
	}
	for (; i < last; i++)
	{
		int32_t key = commands[i].key;
		if (userInput->getKeyDown(gosEnum_KeyIndex(key & 0x0000000ff)))
		{
			// check for shifts and stuff
			// must check way pt first, because it can be shift or ctrl
			if ((key & WAYPT))
			{
				if (!wayPtDn)
					continue;
			}
			else if (((key & SHIFT) ? true : false) != shiftDn)
				continue;
			else if (((key & CTRL) ? true : false) != ctrlDn)
				continue;
			else if (((key & ALT) ? true : false) != altDn)
				continue;
			// got this far, call the command
			if (commands[i].key != -1)
				userInput->setMouseCursor(bLOS ? commands[i].cursorLOS : commands[i].cursor);
			if (!commands[i].singleClick)
			{
				if ((this->*commands[i].function)())
					bRetVal = 1;
			}
			else if (gos_GetKeyStatus((gosEnum_KeyIndex)(key & 0x000fffff)) != KEY_HELD)
			{
				if (this->commands[i].function && (this->*commands[i].function)())
					bRetVal = 1;
				terrainLineChanged = turn;
			}
		}
		else if (gos_GetKeyStatus(gosEnum_KeyIndex(key & 0x000fffff)) == KEY_RELEASED && commands[i].releaseFunction)
		{
			if ((this->*commands[i].releaseFunction)())
				bRetVal = 1;
		}
	}
	return bRetVal;
}

void MissionInterfaceManager::doAttack()
{
	if (userInput->getKeyDown(WAYPOINT_KEY) || !target)
	{
		soundSystem->playDigitalSample(INVALID_GUI);
		return; // don't do if in waypoint mode
	}
	TacticalOrder tacOrder;
	bool bCapture = target->isCaptureable(Team::home->getId());
	bool bFireFromCurrentPos = controlGui.getCurrentRange() == FireRangeType::current ? 1 : 0;
	if (controlGui.getFireFromCurrentPos())
		bFireFromCurrentPos = true;
	if (!bCapture)
	{
		if (!Team::home->isFriendly(target->getTeam()))
		{
			tacOrder.init(OrderOriginType::player, TacticalOrderCode::attackobject);
			tacOrder.targetWID = target->getWatchID();
			tacOrder.attackParams.type =
				bEnergyWeapons ? AttackType::conservingammo : AttackType::destroy;
			tacOrder.attackParams.method = AttackMethod::ranged;
			tacOrder.attackParams.range = FireRangeType::optimal;
			tacOrder.attackParams.pursue = !bFireFromCurrentPos;
			tacOrder.moveparams.wayPath.mode[0] =
				controlGui.getWalk() ? TravelModeType::slow : TravelModeType::fast;
			target->getAppearance()->flashBuilding(1.3f, 0.2f, 0xffff0000);
		}
		else
		{
			// Do NOT Attack friendlies unless force Fire.
			// That TacOrder is issued elsewhere!!
			return;
		}
	}
	else
	{
		for (size_t i = 0; i < Team::home->getRosterSize(); i++)
		{
			Mover* pMover = (Mover*)Team::home->getMover(i);
			if (pMover->isSelected() && pMover->getCommander()->getId() == Commander::home->getId())
			{
				if ((pMover->getObjectClass() == BATTLEMECH) && (pMover->getMoveType() == MOVETYPE_GROUND))
				{
					if (target->getCaptureBlocker(pMover))
					{
						soundSystem->playDigitalSample(INVALID_GUI);
						return;
					}
				}
				else
				{
					// Can't capture player has at least one support thing
					// selected
					// At least until design tells me what they REALLY want!
					// -fs
					soundSystem->playDigitalSample(INVALID_GUI);
					return;
				}
			}
		}
		tacOrder.init(OrderOriginType::player, TacticalOrderCode::capture);
		tacOrder.targetWID = target->getWatchID();
		tacOrder.attackParams.type = AttackType::none;
		tacOrder.attackParams.method = AttackMethod::ramming;
		tacOrder.attackParams.pursue = true;
		tacOrder.moveparams.wayPath.mode[0] =
			controlGui.getWalk() ? TravelModeType::slow : TravelModeType::fast;
	}
	soundSystem->playDigitalSample(BUTTON5);
	Team* pTeam = Team::home;
	for (size_t i = 0; i < pTeam->getRosterSize(); i++)
	{
		Mover* pMover = (Mover*)pTeam->getMover(i);
		if (pMover->isSelected() && pMover->getCommander()->getId() == Commander::home->getId())
		{
			tacOrder.attackParams.range = (FireRangeType)pMover->attackRange;
			if (pMover->attackRange == FireRangeType::current)
				tacOrder.attackParams.pursue = false;
			else if (controlGui.getFireFromCurrentPos())
				tacOrder.attackParams.pursue = false;
			else
				tacOrder.attackParams.pursue = true;
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
	controlGui.setFireFromCurrentPos(0);
}

int32_t
MissionInterfaceManager::attackShort()
{
	controlGui.setRange(FireRangeType::shortest);
	if (commandClicked && target)
	{
		doAttack();
		return 1;
	}
	return 0;
}

int32_t
MissionInterfaceManager::attackMedium()
{
	controlGui.setRange(FireRangeType::medium);
	if (commandClicked && target)
	{
		doAttack();
		return 1;
	}
	return 0;
}

int32_t
MissionInterfaceManager::attackLong()
{
	controlGui.setRange(FireRangeType::extended);
	if (commandClicked && target)
	{
		doAttack();
		return 1;
	}
	return 0;
}

int32_t
MissionInterfaceManager::alphaStrike()
{
	return 0;
}
int32_t
MissionInterfaceManager::defaultAttack()
{
	controlGui.setRange(FireRangeType::optimal);
	return 0;
}

int32_t
MissionInterfaceManager::jump()
{
	if (!canJump())
	{
		soundSystem->playDigitalSample(INVALID_GUI);
		return 0;
	}
	if (!controlGui.getJump())
		controlGui.toggleJump();
	return 0;
}

int32_t
MissionInterfaceManager::stopJump()
{
	if (controlGui.getJump())
		controlGui.toggleJump();
	return 1;
}
void MissionInterfaceManager::doJump()
{
	bool passable = 0;
	if (!target)
	{
		if (Terrain::IsGameSelectTerrainPosition(wPos))
		{
			int32_t cellR, cellC;
			land->worldToCell(wPos, cellR, cellC);
			passable = GameMap->getPassable(cellR, cellC);
		}
		bool canJump = canJumpToWPos();
		if (canJump)
		{
			TacticalOrder tacOrder;
			tacOrder.init(OrderOriginType::player, TacticalOrderCode::jumptopoint, false);
			tacOrder.moveparams.wait = false;
			tacOrder.moveparams.wayPath.mode[0] = TravelModeType::jump;
			tacOrder.setWayPoint(0, wPos);
			tacOrder.pack(nullptr, nullptr);
			handleOrders(tacOrder);
			controlGui.setDefaultSpeed();
			soundSystem->playDigitalSample(BUTTON5);
			controlGui.setDefaultSpeed();
		}
		else
		{
			userInput->setMouseCursor(mState_DONT);
			soundSystem->playDigitalSample(INVALID_GUI);
		}
	}
	else
	{
		userInput->setMouseCursor(mState_DONT);
		soundSystem->playDigitalSample(INVALID_GUI);
	}
}

int32_t
MissionInterfaceManager::fireFromCurrentPos()
{
	if (!controlGui.getFireFromCurrentPos())
		controlGui.toggleFireFromCurrentPos();
	return forceShot();
}
int32_t
MissionInterfaceManager::stopFireFromCurrentPos()
{
	if (controlGui.getFireFromCurrentPos())
		controlGui.toggleFireFromCurrentPos();
	return 1;
}

void MissionInterfaceManager::doGuard(GameObject* who)
{
	if (who)
	{
		TacticalOrder tacOrder;
		tacOrder.init(OrderOriginType::player, TacticalOrderCode::guard);
		tacOrder.targetWID = who->getWatchID();
		tacOrder.pack(nullptr, nullptr);
		handleOrders(tacOrder);
		controlGui.setDefaultSpeed();
	}
	else
	{
		LocationNode path;
		path.location = wPos;
		path.run = true;
		path.next = nullptr;
		TacticalOrder tacOrder;
		tacOrder.init(OrderOriginType::player, TacticalOrderCode::guard);
		tacOrder.initWayPath(&path);
		tacOrder.moveparams.wayPath.mode[0] =
			controlGui.getWalk() ? TravelModeType::slow : TravelModeType::fast;
		tacOrder.pack(nullptr, nullptr);
		handleOrders(tacOrder);
		controlGui.setDefaultSpeed();
	}
	soundSystem->playDigitalSample(BUTTON5);
}
int32_t
MissionInterfaceManager::guard()
{
	if (!controlGui.getGuard())
		controlGui.toggleGuard();
	return 0;
}
int32_t
MissionInterfaceManager::stopGuard()
{
	if (controlGui.getGuard())
		controlGui.toggleGuard();
	return 0;
}
int32_t
MissionInterfaceManager::conserveAmmo()
{
	return 0;
}
int32_t
MissionInterfaceManager::selectVisible()
{
	Team* pTeam = Team::home;
	for (size_t i = 0; i < pTeam->getRosterSize(); i++)
	{
		Mover* pMover = (Mover*)pTeam->getMover(i);
		if (pMover->getCommander()->getId() == Commander::home->getId())
		{
			pMover->setSelected(false);
		}
	}
	return addVisibleToSelection();
}

int32_t
MissionInterfaceManager::addVisibleToSelection()
{
	//----------------------------------------------------------------
	// Grab each member of the homeTeam and see if they are onScreen
	dragStart.Zero();
	dragEnd = eye->getScreenRes();
	for (size_t i = 0; i < ObjectManager->getNumMovers(); i++)
	{
		if (ObjectManager->moverInRect(i, dragStart, dragEnd))
		{
			GameObjectPtr mover = ObjectManager->getMover(i);
			if (mover && (mover->getCommanderId() == Commander::home->getId()) && !mover->isDisabled())
			{
				mover->setSelected(true);
			}
		}
	}
	dragStart.Zero();
	dragEnd.Zero();
	return 1;
}
int32_t
MissionInterfaceManager::aimLeg()
{
	if (target)
	{
		BattleMech* pMech = dynamic_cast<BattleMech*>(target);
		if (pMech && pMech->getMoveType() == MOVETYPE_GROUND)
		{
			if (!anySelectedWithoutAreaEffect())
			{
				// Can't aim shot.  No weapon which is not area effect
				// available!
				userInput->setMouseCursor(mState_DONT);
				return 1;
			}
			else if (pMech->body[MECH_BODY_LOCATION_LLEG].damageState == IS_DAMAGE_DESTROYED && pMech->body[MECH_BODY_LOCATION_RLEG].damageState == IS_DAMAGE_DESTROYED)
			{
				userInput->setMouseCursor(mState_DONT);
				return 1;
			}
			else if (!commandClicked) // cursor already set above, however if in
				// doubt
				return 1;
			else
			{
				TacticalOrder tacOrder;
				tacOrder.init(OrderOriginType::player, TacticalOrderCode::attackobject);
				tacOrder.targetWID = target->getWatchID();
				tacOrder.attackParams.type = AttackType::destroy;
				tacOrder.attackParams.method = AttackMethod::ranged;
				tacOrder.attackParams.range = FireRangeType::optimal;
				tacOrder.attackParams.pursue = controlGui.getFireFromCurrentPos() ? false : true;
				tacOrder.moveparams.wayPath.mode[0] =
					controlGui.getWalk() ? TravelModeType::slow : TravelModeType::fast;
				bool bRight =
					pMech->body[MECH_BODY_LOCATION_LLEG].damageState == IS_DAMAGE_DESTROYED;
				tacOrder.attackParams.aimLocation =
					bRight ? MECH_BODY_LOCATION_RLEG : MECH_BODY_LOCATION_LLEG;
				tacOrder.pack(nullptr, nullptr);
				handleOrders(tacOrder);
				soundSystem->playDigitalSample(BUTTON5);
				bAimedShot = true;
				return 1;
			}
		}
	}
	else
	{
		userInput->setMouseCursor(mState_DONT);
		return 1;
	}
	return 0;
}

int32_t
MissionInterfaceManager::aimArm()
{
	BattleMech* pMech = dynamic_cast<BattleMech*>(target);
	if (pMech && pMech->getMoveType() == MOVETYPE_GROUND)
	{
		if (anySelectedWithoutAreaEffect() && commandClicked)
		{
			TacticalOrder tacOrder;
			tacOrder.init(OrderOriginType::player, TacticalOrderCode::attackobject);
			tacOrder.targetWID = target->getWatchID();
			tacOrder.attackParams.type = AttackType::destroy;
			tacOrder.attackParams.method = AttackMethod::ranged;
			tacOrder.attackParams.pursue = controlGui.getFireFromCurrentPos() ? false : true;
			tacOrder.attackParams.range = FireRangeType::optimal;
			tacOrder.moveparams.wayPath.mode[0] =
				controlGui.getWalk() ? TravelModeType::slow : TravelModeType::fast;
			bool bRight = pMech->body[MECH_BODY_LOCATION_LARM].damageState == IS_DAMAGE_DESTROYED;
			tacOrder.attackParams.aimLocation =
				bRight ? MECH_BODY_LOCATION_RARM : MECH_BODY_LOCATION_LARM;
			tacOrder.pack(nullptr, nullptr);
			handleOrders(tacOrder);
			soundSystem->playDigitalSample(BUTTON5);
			bAimedShot = true;
			return 1;
		}
		if (!anySelectedWithoutAreaEffect())
		{
			// Can't aim shot.  No weapon which is not area effect available!
			userInput->setMouseCursor(mState_DONT);
			return 1;
		}
		else if (pMech->body[MECH_BODY_LOCATION_LARM].damageState == IS_DAMAGE_DESTROYED && pMech->body[MECH_BODY_LOCATION_RARM].damageState == IS_DAMAGE_DESTROYED)
		{
			userInput->setMouseCursor(mState_DONT);
			return 1;
		}
		else // cursor already set above, however if in doubt
			return 1;
	}
	else
	{
		userInput->setMouseCursor(mState_DONT);
		return 1;
	}
}

int32_t
MissionInterfaceManager::aimHead()
{
	if (target)
	{
		BattleMech* pMech = dynamic_cast<BattleMech*>(target);
		if (pMech && pMech->getMoveType() == MOVETYPE_GROUND)
		{
			if (anySelectedWithoutAreaEffect() && commandClicked)
			{
				TacticalOrder tacOrder;
				tacOrder.init(OrderOriginType::player, TacticalOrderCode::attackobject);
				tacOrder.targetWID = target->getWatchID();
				tacOrder.attackParams.type = AttackType::destroy;
				tacOrder.attackParams.method = AttackMethod::ranged;
				tacOrder.attackParams.pursue = controlGui.getFireFromCurrentPos() ? false : true;
				tacOrder.attackParams.range = FireRangeType::optimal;
				tacOrder.moveparams.wayPath.mode[0] =
					controlGui.getWalk() ? TravelModeType::slow : TravelModeType::fast;
				tacOrder.attackParams.aimLocation = MECH_BODY_LOCATION_HEAD;
				tacOrder.pack(nullptr, nullptr);
				handleOrders(tacOrder);
				bAimedShot = true;
				soundSystem->playDigitalSample(BUTTON5);
			}
			else if (!anySelectedWithoutAreaEffect())
			{
				// Can't aim shot.  No weapon which is not area effect
				// available!
				userInput->setMouseCursor(mState_DONT);
				return 1;
			}
			else if (pMech->body[MECH_BODY_LOCATION_HEAD].damageState == IS_DAMAGE_DESTROYED)
			{
				userInput->setMouseCursor(mState_DONT);
				return 1;
			}
			else // cursor already set above, however if in doubt
				return 1;
		}
		else
		{
			userInput->setMouseCursor(mState_DONT);
			return 1;
		}
	}
	else
	{
		userInput->setMouseCursor(mState_DONT);
		return 1;
	}
	return 0;
}

int32_t
MissionInterfaceManager::removeCommand()
{
	controlGui.doStop();
	return 1;
}
int32_t
MissionInterfaceManager::powerUp()
{
	TacticalOrder tacOrder;
	tacOrder.init(OrderOriginType::player, TacticalOrderCode::powerup, true);
	tacOrder.pack(nullptr, nullptr);
	handleOrders(tacOrder);
	return 1;
}
int32_t
MissionInterfaceManager::powerDown()
{
	TacticalOrder tacOrder;
	tacOrder.init(OrderOriginType::player, TacticalOrderCode::powerdown, true);
	tacOrder.pack(nullptr, nullptr);
	handleOrders(tacOrder);
	return 1;
}
int32_t
MissionInterfaceManager::changeSpeed()
{
	if (controlGui.isDefaultSpeed())
		controlGui.toggleDefaultSpeed();
	return 0;
}

int32_t
MissionInterfaceManager::stopChangeSpeed()
{
	if (!controlGui.isDefaultSpeed())
		controlGui.toggleDefaultSpeed();
	return 1;
}
int32_t
MissionInterfaceManager::eject()
{
	if (target && target->isMover() && target->getCommanderId() == Commander::home->getId() && !invulnerableON)
	{
		if (commandClicked)
		{
			doEject(target);
		}
		userInput->setMouseCursor(mState_EJECT);
	}
	else if (!controlGui.forceGroupBar.inRegion(userInput->getMouseX(), userInput->getMouseY()))
		userInput->setMouseCursor(mState_XEJECT);
	return 1;
}

int32_t
MissionInterfaceManager::bigAirStrike()
{
	Stuff::Vector3D v = makeAirStrikeTarget(wPos);
	IfaceCallStrike(ARTILLERY_LARGE, &v, nullptr);
	//	if ( !isPaused() )
	soundSystem->playSupportSample(SUPPORT_AIRSTRIKE);
	return 1;
}
int32_t
MissionInterfaceManager::smlAirStrike()
{
	Stuff::Vector3D v = makeAirStrikeTarget(wPos);
	IfaceCallStrike(ARTILLERY_SMALL, &v, nullptr);
	//	if ( !isPaused() )
	soundSystem->playSupportSample(SUPPORT_AIRSTRIKE);
	return 1;
}
int32_t
MissionInterfaceManager::snsAirStrike()
{
	Stuff::Vector3D v = makeAirStrikeTarget(wPos);
	IfaceCallStrike(ARTILLERY_SENSOR, &v, nullptr);
	//	if ( !isPaused() )
	soundSystem->playSupportSample(SUPPORT_PROBE);
	return 1;
}

Stuff::Vector3D
MissionInterfaceManager::makeAirStrikeTarget(const Stuff::Vector3D& pos)
{
	Stuff::Vector3D newPos = pos;
	/*	bool 	lineOfSight = Team::home->teamLineOfSight(pos);
		if ( !lineOfSight )
		{
			// need to offset x, y, random amounts up to five tiles...
			int32_t randX = rand();
			int32_t randY = rand();
			int32_t offsetX = randX % (128*5);
			int32_t offsetY = randY % (128*5);
			if ( randX % 2 )
				offsetX = -offsetX;
			if ( randY % 2 )
				offsetY = -offsetY;

			newPos.x += offsetX;
			newPos.y += offsetY;
		}*/
	return newPos;
}

int32_t
MissionInterfaceManager::cameraAssign0()
{
	Camera::cameraZoom[0] = eye->cameraAltitude;
	Camera::cameraTilt[0] = eye->getProjectionAngle();
	return 1;
}

int32_t
MissionInterfaceManager::cameraAssign1()
{
	Camera::cameraZoom[1] = eye->cameraAltitude;
	Camera::cameraTilt[1] = eye->getProjectionAngle();
	return 1;
}

int32_t
MissionInterfaceManager::cameraAssign2()
{
	Camera::cameraZoom[2] = eye->cameraAltitude;
	Camera::cameraTilt[2] = eye->getProjectionAngle();
	return 1;
}

int32_t
MissionInterfaceManager::cameraAssign3()
{
	Camera::cameraZoom[3] = eye->cameraAltitude;
	Camera::cameraTilt[3] = eye->getProjectionAngle();
	return 1;
}

int32_t
MissionInterfaceManager::cameraNormal()
{
	if (eye)
		eye->allNormal();
	return 1;
}
int32_t
MissionInterfaceManager::cameraDefault()
{
	if (eye)
		eye->setCameraView(0);
	return 1;
}
int32_t
MissionInterfaceManager::cameraMaxIn()
{
	if (eye)
		eye->setCameraView(1);
	return 1;
}
int32_t
MissionInterfaceManager::cameraTight()
{
	if (eye)
		eye->setCameraView(2);
	return 1;
}
int32_t
MissionInterfaceManager::cameraFour()
{
	if (eye)
		eye->setCameraView(3);
	return 1;
}

bool MissionInterfaceManager::anySelectedWithoutAreaEffect(void)
{
	// Scan the list of selected movers and see if any have at least one
	// non-area effect weapon
	// Used for Called shots which cannot be called if weapon is area effect!!
	Team* pTeam = Team::home;
	for (size_t i = 0; i < pTeam->getRosterSize(); i++)
	{
		Mover* pMover = (Mover*)pTeam->getMover(i);
		if (pMover->isSelected() && pMover->hasNonAreaWeapon() && pMover->getCommander()->getId() == Commander::home->getId())
			return true;
	}
	return false;
}

int32_t
MissionInterfaceManager::handleOrders(TacticalOrder& order)
{
	Team* pTeam = Team::home;
	//--------------------------------------------------------
	// First, find out how many are jumping and calc a list of
	// jump locations...
	bool isMoveOrder = false;
	Stuff::Vector3D moveGoals[MAX_MOVERS];
	int32_t numMovers = 0;
	if (order.code == TacticalOrderCode::jumptopoint)
	{
		for (size_t i = 0; i < pTeam->getRosterSize(); i++)
		{
			Mover* pMover = pTeam->getMover(i);
			if (pMover->isSelected() && pMover->getCommander()->getId() == Commander::home->getId())
				numMovers++;
		}
		MoverGroup::calcJumpGoals(order.getWayPoint(0), numMovers, moveGoals, nullptr);
		isMoveOrder = true;
	}
	else if (order.code == TacticalOrderCode::movetopoint)
	{
		for (size_t i = 0; i < pTeam->getRosterSize(); i++)
		{
			Mover* pMover = pTeam->getMover(i);
			if (pMover->isSelected() && pMover->getCommander()->getId() == Commander::home->getId())
				numMovers++;
		}
		MoverGroup::calcMoveGoals(order.getWayPoint(0), numMovers, moveGoals);
		isMoveOrder = true;
	}
	numMovers = 0;
	for (size_t i = 0; i < pTeam->getRosterSize(); i++)
	{
		Mover* pMover = (Mover*)pTeam->getMover(i);
		if (pMover->isSelected() && pMover->getCommander()->getId() == Commander::home->getId())
		{
			//---------------------------------------------------------------------
			// Helper function--perhaps this should just be a part of the mover
			// and group handleTacticalOrder() functions?
			if (isMoveOrder)
			{
				order.setWayPoint(0, moveGoals[numMovers++]);
				order.pack(nullptr, nullptr);
			}
			if (MPlayer && !MPlayer->isServer())
			{
				MPlayer->sendPlayerOrder(
					&order, false, 1, &pMover, 0, nullptr, userInput->getKeyDown(WAYPOINT_KEY));
				// pMover->getPilot()->setExecutingQueue(false);
				// pMover->getPilot()->addQueuedTacOrder(order);
			}
			else
			{
				order.attackParams.range = (FireRangeType)pMover->attackRange;
				order.pack(nullptr, nullptr);
				if (userInput->getKeyDown(WAYPOINT_KEY)) // way point one
				{
					pMover->getPilot()->setExecutingQueue(FALSE);
					pMover->getPilot()->addQueuedTacOrder(order);
				}
				else if (controlGui.getMines())
				{
					pMover->getPilot()->addQueuedTacOrder(order);
					pMover->getPilot()->setExecutingQueue(TRUE);
				}
				else
					pMover->handleTacticalOrder(order);
			}
		}
	}
	return 1;
}

int32_t
MissionInterfaceManager::scrollUp()
{
	float frameFactor = frameLength / baseFrameLength;
	float scrollFactor = scrollInc / eye->getScaleFactor() * frameFactor;
	eye->moveUp(scrollFactor);
	return 1;
}

int32_t
MissionInterfaceManager::scrollDown()
{
	float frameFactor = frameLength / baseFrameLength;
	float scrollFactor = scrollInc / eye->getScaleFactor() * frameFactor;
	eye->moveDown(scrollFactor);
	return 1;
}
int32_t
MissionInterfaceManager::scrollLeft()
{
	float frameFactor = frameLength / baseFrameLength;
	float scrollFactor = scrollInc / eye->getScaleFactor() * frameFactor;
	eye->moveLeft(scrollFactor);
	return 1;
}
int32_t
MissionInterfaceManager::scrollRight()
{
	float frameFactor = frameLength / baseFrameLength;
	float scrollFactor = scrollInc / eye->getScaleFactor() * frameFactor;
	eye->moveRight(scrollFactor);
	return 1;
}
int32_t
MissionInterfaceManager::zoomOut()
{
	float frameFactor = frameLength / baseFrameLength;
	eye->ZoomOut(zoomInc * frameFactor * eye->getScaleFactor());
	return 1;
}
int32_t
MissionInterfaceManager::zoomIn()
{
	float frameFactor = frameLength / baseFrameLength;
	eye->ZoomIn(zoomInc * frameFactor * eye->getScaleFactor());
	return 1;
}
int32_t
MissionInterfaceManager::zoomChoiceOut()
{
	float frameFactor = frameLength / baseFrameLength;
	eye->ZoomOut(zoomInc * frameFactor * 5.0f * eye->getScaleFactor());
	return 1;
}
int32_t
MissionInterfaceManager::zoomChoiceIn()
{
	float frameFactor = frameLength / baseFrameLength;
	eye->ZoomIn(zoomInc * frameFactor * 5.0f * eye->getScaleFactor());
	return 1;
}
int32_t
MissionInterfaceManager::rotateLeft()
{
	float frameFactor = frameLength / baseFrameLength;
	realRotation += degPerSecRot * frameFactor;
	if (realRotation >= rotationInc)
	{
		eye->rotateLeft(rotationInc);
		realRotation = 0;
	}
	return 1;
}
int32_t
MissionInterfaceManager::rotateRight()
{
	float frameFactor = frameLength / baseFrameLength;
	realRotation -= degPerSecRot * frameFactor;
	if (realRotation <= -rotationInc)
	{
		eye->rotateRight(rotationInc);
		realRotation = 0;
	}
	return 1;
}
int32_t
MissionInterfaceManager::tiltUp()
{
	float frameFactor = frameLength / baseFrameLength;
	float scrollFactor = scrollInc / eye->getScaleFactor() * frameFactor;
	eye->tiltDown(scrollFactor * frameFactor * 30.0f);
	return 1;
}
int32_t
MissionInterfaceManager::tiltDown()
{
	float frameFactor = frameLength / baseFrameLength;
	float scrollFactor = scrollInc / eye->getScaleFactor() * frameFactor;
	eye->tiltUp(scrollFactor * frameFactor * 30.0f);
	return 1;
}
int32_t
MissionInterfaceManager::centerCamera()
{
	return 0;
}
int32_t
MissionInterfaceManager::rotateLightLeft()
{
#ifndef FINAL
	eye->rotateLightLeft(rotationInc);
#endif
	return 1;
}
int32_t
MissionInterfaceManager::rotateLightRight()
{
#ifndef FINAL
	eye->rotateLightRight(rotationInc);
#endif
	return 1;
}
int32_t
MissionInterfaceManager::rotateLightUp()
{
#ifndef FINAL
	eye->rotateLightUp(rotationInc);
#endif
	return 1;
}
int32_t
MissionInterfaceManager::rotateLightDown()
{
#ifndef FINAL
	eye->rotateLightDown(rotationInc);
#endif
	return 1;
}

int32_t
MissionInterfaceManager::drawTerrain()
{
#ifndef FINAL
	drawTerrainTiles ^= TRUE;
#endif
	return 1;
}
int32_t
MissionInterfaceManager::drawOverlays()
{
#ifndef FINAL
	drawTerrainOverlays ^= TRUE;
#endif
	return 1;
}
int32_t
MissionInterfaceManager::drawBuildings()
{
#ifndef FINAL
	renderObjects ^= TRUE;
#endif
	return 1;
}
int32_t
MissionInterfaceManager::showGrid()
{
#ifndef FINAL
	drawTerrainGrid = !drawTerrainGrid;
#endif
	return 1;
}
int32_t
MissionInterfaceManager::recalcLights()
{
#ifndef FINAL
	Terrain::recalcLight ^= TRUE;
	Terrain::recalcShadows = false;
#endif
	return 1;
}
int32_t
MissionInterfaceManager::drawClouds()
{
#ifndef FINAL
	useClouds ^= TRUE;
#endif
	return 1;
}
int32_t
MissionInterfaceManager::drawFog()
{
#ifndef FINAL
	useFog ^= TRUE;
#endif
	return 1;
}
int32_t
MissionInterfaceManager::usePerspective()
{
#ifndef FINAL
	eye->usePerspective ^= true;
#endif
	return 1;
}
int32_t
MissionInterfaceManager::toggleGUI()
{
#ifndef FINAL
	drawGUIOn ^= true;
	toggledGUI = drawGUIOn;
#endif
	return 1;
}
int32_t
MissionInterfaceManager::drawTGLShapes()
{
#ifndef FINAL
	renderTGLShapes ^= true;
	terrainLineChanged = turn;
#endif
	return 1;
}
int32_t
MissionInterfaceManager::drawWaterEffects()
{
#ifndef FINAL
	useWaterInterestTexture ^= true;
	terrainLineChanged = turn;
#endif
	return 1;
}
int32_t
MissionInterfaceManager::recalcWater()
{
#ifndef FINAL
	Terrain::mapData->recalcWater();
#endif
	return 1;
}
int32_t
MissionInterfaceManager::drawShadows()
{
#ifndef FINAL
	useShadows ^= TRUE;
#endif
	return 1;
}
int32_t
MissionInterfaceManager::changeLighting()
{
#ifndef FINAL
	drawLOSGrid ^= true;
#endif
	return 1;
}

void MissionInterfaceManager::init(FitIniFilePtr loader)
{
	int32_t result = loader->seekBlock("CameraData");
	gosASSERT(result == NO_ERROR);
	result = loader->readIdFloat("ScrollIncrement", scrollInc);
	gosASSERT(result == NO_ERROR);
	result = loader->readIdFloat("RotationIncrement", rotationInc);
	gosASSERT(result == NO_ERROR);
	result = loader->readIdFloat("ZoomIncrement", zoomInc);
	gosASSERT(result == NO_ERROR);
	result = loader->readIdFloat("ScrollLeft", screenScrollLeft);
	gosASSERT(result == NO_ERROR);
	result = loader->readIdFloat("ScrollRight", screenScrollRight);
	gosASSERT(result == NO_ERROR);
	result = loader->readIdFloat("ScrollUp", screenScrollUp);
	gosASSERT(result == NO_ERROR);
	result = loader->readIdFloat("ScrollDown", screenScrollDown);
	gosASSERT(result == NO_ERROR);
	result = loader->readIdFloat("BaseFrameLength", baseFrameLength);
	gosASSERT(result == NO_ERROR);
	result = loader->readIdFloat("RotationDegPerSec", degPerSecRot);
	gosASSERT(result == NO_ERROR);
	float missionDragThreshold;
	result = loader->readIdFloat("MouseDragThreshold", missionDragThreshold);
	gosASSERT(result == NO_ERROR);
	float newThreshold = missionDragThreshold / Environment.screenheight;
	userInput->setMouseDragThreshold(newThreshold);
	float missionDblClkThreshold;
	result = loader->readIdFloat("MouseDoubleClickThreshold", missionDblClkThreshold);
	gosASSERT(result == NO_ERROR);
	userInput->setMouseDoubleClickThreshold(missionDblClkThreshold);
	swapResolutions();
}

//--------------------------------------------------------------------------------------
void MissionInterfaceManager::destroy(void)
{
	hotKeyFont.destroy();
	hotKeyHeaderFont.destroy();
	delete keyboardRef;
	keyboardRef = nullptr;
}

//--------------------
// TO TEST LIGHTING MODEL
bool rotateLightRt = false;
bool rotateLightLf = false;
bool rotateLightUp = false;
bool rotateLightDn = false;

//--------------------------------------------------------------------------------------

void HandlePlayerOrder(std::unique_ptr<Mover> mover, TacticalOrderPtr tacOrder)
{
	//---------------------------------------------------------------------
	// Helper function--perhaps this should just be a part of the mover and
	// group handleTacticalOrder() functions?
	if (MPlayer && !MPlayer->isServer())
		MPlayer->sendPlayerOrder(tacOrder, false, 1, &mover);
	else
		mover->handleTacticalOrder(*tacOrder);
}

//--------------------------------------------------------------------------------------
void MissionInterfaceManager::drawVTOL(void)
{
	for (size_t vtolNum = 0; vtolNum < MAX_TEAMS; vtolNum++)
	{
		if (paintingVtol[vtolNum] && vTol[vtolNum])
		{
			vTol[vtolNum]->render();
			vTol[vtolNum]->renderShadows();
			if (dustCloud[vtolNum] && dustCloud[vtolNum]->IsExecuted())
			{
				gosFX::Effect::DrawInfo drawInfo;
				drawInfo.m_clipper = theClipper;
				Stuff::LinearMatrix4D shapeOrigin;
				Stuff::LinearMatrix4D localToWorld;
				Stuff::LinearMatrix4D localResult;
				Stuff::Vector3D dustPos = vTol[vtolNum]->position;
				Stuff::Point3D wakePos;
				wakePos.x = -dustPos.x;
				wakePos.y = dustPos.z;
				wakePos.z = dustPos.y;
				shapeOrigin.BuildRotation(Stuff::EulerAngles(0.0f, 0.0f, 0.0f));
				shapeOrigin.BuildTranslation(wakePos);
				drawInfo.m_parentToWorld = &shapeOrigin;
				if (!MLRVertexLimitReached)
					dustCloud[vtolNum]->Draw(&drawInfo);
			}
			if (recoveryBeam[vtolNum] && recoveryBeam[vtolNum]->IsExecuted())
			{
				gosFX::Effect::DrawInfo drawInfo;
				drawInfo.m_clipper = theClipper;
				Stuff::LinearMatrix4D shapeOrigin;
				Stuff::LinearMatrix4D localToWorld;
				Stuff::LinearMatrix4D localResult;
				Stuff::Vector3D dustPos = vTol[vtolNum]->position;
				Stuff::Point3D wakePos;
				wakePos.x = -dustPos.x;
				wakePos.y = dustPos.z;
				wakePos.z = dustPos.y;
				shapeOrigin.BuildRotation(Stuff::EulerAngles(0.0f, 0.0f, 0.0f));
				shapeOrigin.BuildTranslation(wakePos);
				drawInfo.m_parentToWorld = &shapeOrigin;
				if (!MLRVertexLimitReached)
					recoveryBeam[vtolNum]->Draw(&drawInfo);
			}
		}
	}
}

//--------------------------------------------------------------------------------------
void MissionInterfaceManager::render(void)
{
	if (drawGUIOn)
	{
		//-------------------------
		// Draw the dragSelect Box
		if (isDragging)
		{
			gos_VERTEX vertices[5];
			uint32_t color = SB_WHITE;
			gos_SetRenderState(gos_State_AlphaMode, gos_Alpha_AlphaInvAlpha);
			gos_SetRenderState(gos_State_Specular, FALSE);
			gos_SetRenderState(gos_State_AlphaTest, TRUE);
			gos_SetRenderState(gos_State_Texture, 0);
			gos_SetRenderState(gos_State_Filter, gos_FilterNone);
			Stuff::Vector4D screenPos;
			eye->projectZ(dragStart, screenPos);
			vertices[0].x = screenPos.x;
			vertices[0].y = screenPos.y;
			vertices[0].z = 0.0;
			vertices[0].rhw = 0.5;
			vertices[0].u = 0.0;
			vertices[0].v = 0.0;
			vertices[0].argb = color;
			vertices[1].x = dragEnd.x;
			vertices[1].y = screenPos.y;
			vertices[1].z = 0.0;
			vertices[1].rhw = 0.5;
			vertices[1].u = 0.0;
			vertices[1].v = 0.0;
			vertices[1].argb = color;
			vertices[2].x = dragEnd.x;
			vertices[2].y = dragEnd.y;
			vertices[2].z = 0.0;
			vertices[2].rhw = 0.5;
			vertices[2].u = 0.0;
			vertices[2].v = 0.0;
			vertices[2].argb = color;
			vertices[3].x = screenPos.x;
			vertices[3].y = dragEnd.y;
			vertices[3].z = 0.0;
			vertices[3].rhw = 0.5;
			vertices[3].u = 0.0;
			vertices[3].v = 0.0;
			vertices[3].argb = color;
			vertices[4].x = screenPos.x;
			vertices[4].y = screenPos.y;
			vertices[4].z = 0.0;
			vertices[4].rhw = 0.5;
			vertices[4].u = 0.0;
			vertices[4].v = 0.0;
			vertices[4].argb = color;
			gos_DrawQuads(vertices, 4);
			vertices[0].argb = SB_BLACK;
			vertices[1].argb = SB_BLACK;
			vertices[2].argb = SB_BLACK;
			vertices[3].argb = SB_BLACK;
			vertices[4].argb = SB_BLACK;
			gos_DrawLines(&vertices[0], 2);
			gos_DrawLines(&vertices[1], 2);
			gos_DrawLines(&vertices[2], 2);
			gos_DrawLines(&vertices[3], 2);
		}
		if (userInput->getKeyDown(WAYPOINT_KEY) || controlGui.getMines())
		{
			Team* pTeam = Team::home;
			for (size_t i = 0; i < pTeam->getRosterSize(); i++)
			{
				Mover* pMover = (Mover*)pTeam->getMover(i);
				if (pMover->isSelected() && pMover->getCommander()->getId() == Commander::home->getId())
				{
					pMover->drawWaypointPath();
				}
			}
		}
		if (bDrawHotKeys)
		{
			drawHotKeys();
			return;
		}
		if (turn > 3 /*&& !controlGui.inRegion(userInput->getMouseX(), userInput->getMouseY())*/)
		{
#ifdef DRAW_CURSOR_CROSSHAIRS
			Stuff::Vector4D cursorPos;
			eye->projectZ(wPos, cursorPos);
			uint32_t color = SB_WHITE;
			gos_SetRenderState(gos_State_Texture, 0);
			gos_SetRenderState(gos_State_Filter, gos_FilterNone);
			Stuff::Vector4D ul, br, pos1, pos2;
			pos1.x = cursorPos.x - 5;
			pos1.y = cursorPos.y;
			pos1.z = 0.000001f;
			pos2.x = cursorPos.x + 5;
			pos2.y = cursorPos.y;
			pos2.z = 0.000001f;
			pos2.w = pos1.w = 1.0f;
			{
				LineElement newElement(pos1, pos2, color, nullptr, -1);
				newElement.draw();
			}
			pos1.x = cursorPos.x;
			pos1.y = cursorPos.y - 5;
			pos1.z = 0.000001f;
			pos2.x = cursorPos.x;
			pos2.y = cursorPos.y + 5;
			pos2.z = 0.000001f;
			pos2.w = pos1.w = 1.0f;
			{
				LineElement newElement(pos1, pos2, color, nullptr, -1);
				newElement.draw();
			}
#endif
			if (eye->usePerspective)
			{
				// Depth cue the cursor.
				Stuff::Vector3D Distance;
				Stuff::Point3D objPosition;
				Stuff::Point3D eyePosition(eye->getCameraOrigin());
				objPosition.x = -wPos.x;
				objPosition.y = wPos.z;
				objPosition.z = wPos.y;
				Distance.Subtract(objPosition, eyePosition);
				float eyeDistance = Distance.GetApproximateLength();
				float scaleFactor = 0.0f;
				scaleFactor = eyeDistance / (Camera::MaxClipDistance * 2.0f);
				scaleFactor = 1.5f - scaleFactor;
				userInput->setMouseScale(scaleFactor);
			}
		}
	}
	controlGui.render(isPaused() && !isPausedWithoutMenu());
	/* 	if ( scenarioTime  < 7.0 )
		{
			int32_t color = 0xff000000;
			if ( (prefs.resolution == 0 && Environment.screenwidth == 640)
			  || (prefs.resolution == 1 && Environment.screenwidth == 800)
			  || (prefs.resolution == 2 && Environment.screenwidth == 1024)
			  || (prefs.resolution == 3 && Environment.screenwidth == 1280)
			  ||(prefs.resolution == 4 && Environment.screenwidth == 1600))
			{
				color = interpolatecolour( 0xff000000, 0x00000000,
	(scenarioTime-swapTime)/(7.0-swapTime) );
			}
			else
				swapTime = scenarioTime;

			RECT tmpRect = { 0,0, Environment.screenwidth,
	Environment.screenheight }; drawRect( tmpRect, color );

	}*/
}

void MissionInterfaceManager::initTacMap(PacketFile* file, int32_t packet)
{
	file->seekPacket(packet);
	int32_t size = file->getPacketSize();
	uint8_t* mem = new uint8_t[size];
	file->readPacket(packet, mem);
	controlGui.initTacMapBuildings(mem, size);
	delete mem;
	file->seekPacket(packet + 1);
	size = file->getPacketSize();
	mem = new uint8_t[size];
	file->readPacket(packet + 1, mem);
	controlGui.initTacMap(mem, size);
	delete mem;
	swapTime = 0.f;
	for (size_t i = 0; i < MAX_ICONS; i++)
	{
		oldTargets[i] = nullptr;
	}
	target = nullptr;
}

void MissionInterfaceManager::printDebugInfo()
{
	if (userInput->isLeftClick() || userInput->isRightClick())
	{
		int32_t row, col;
		land->worldToCell(wPos, row, col);
		wchar_t debugString[256];
		if (target)
			sprintf(debugString, "POS = %s(%c) %d,%d [%d, %d] (%.4f, %.4f, %.4f) OBJ = %s\n",
				terrainStr[GameMap->getTerrain(row, col)],
				GameMap->getPassable(row, col) ? 'T' : 'F', GlobalMoveMap[0]->calcArea(row, col),
				GlobalMoveMap[1]->calcArea(row, col), row, col, wPos.x, wPos.y, wPos.z,
				target->getName());
		else
			sprintf(debugString, "POS = %s(%c) %d,%d [%d, %d] (%.4f, %.4f, %.4f)\n",
				terrainStr[GameMap->getTerrain(row, col)],
				GameMap->getPassable(row, col) ? 'T' : 'F', GlobalMoveMap[0]->calcArea(row, col),
				GlobalMoveMap[1]->calcArea(row, col), row, col, wPos.x, wPos.y, wPos.z);
		DEBUGWINS_print(debugString);
	}
}

void MissionInterfaceManager::doMove(const Stuff::Vector3D& pos)
{
	/*if ( controlGui.getGuardTower()  )
	{
		doGuardTower();
		return;
	}*/
	if (controlGui.isAddingAirstrike() && controlGui.isButtonPressed(ControlGui::SENSOR_PROBE))
	{
		// tmp hack
		Stuff::Vector3D tmp = wPos;
		wPos = pos;
		addAirstrike();
		wPos = tmp;
		return;
	}
	LocationNode path;
	path.location = pos;
	path.run = true;
	path.next = nullptr;
	TacticalOrder tacOrder;
	if (controlGui.getJump())
	{
		doJump();
		return;
	}
	else
		tacOrder.init(OrderOriginType::player, TacticalOrderCode::movetopoint, false);
	tacOrder.initWayPath(&path);
	tacOrder.moveparams.wait = false;
	tacOrder.moveparams.wayPath.mode[0] =
		controlGui.getWalk() ? TravelModeType::slow : TravelModeType::fast;
	if (controlGui.getMines())
		tacOrder.moveparams.mode = SpecialMoveMode::minelaying;
	tacOrder.pack(nullptr, nullptr);
	handleOrders(tacOrder);
	soundSystem->playDigitalSample(BUTTON5);
	controlGui.setDefaultSpeed();
}

bool MissionInterfaceManager::makePatrolPath()
{
	Team* pTeam = Team::home;
	for (size_t i = 0; i < pTeam->getRosterSize(); i++)
	{
		Mover* pMover = (Mover*)pTeam->getMover(i);
		if (pMover->isSelected() && pMover->getPilot()->getNumTacOrdersQueued() && pMover->getCommander()->getId() == Commander::home->getId())
		{
			if (pMover->isCloseToFirstTacOrder(wPos))
			{
				userInput->setMouseCursor(mState_LINK);
				if (commandClicked)
				{
					pMover->getPilot()->setTacOrderQueueLooping(true);
				}
				return 1;
			}
			else
			{
				Stuff::Vector3D tmp;
				tmp.Subtract(wPos, pMover->getPosition());
				if (tmp.GetLength() < 25) // random amount
				{
					userInput->setMouseCursor(mState_LINK);
					if (commandClicked)
					{
						doMove(wPos);
						pMover->getPilot()->setTacOrderQueueLooping(true);
					}
					return 1;
				}
			}
		}
	}
	return 0;
}

int32_t
MissionInterfaceManager::forceShot()
{
	bForcedShot = true;
	userInput->setMouseCursor(mState_GENERIC_ATTACK);
	if (commandClicked)
	{
		TacticalOrder tacOrder;
		tacOrder.init(OrderOriginType::player,
			target ? TacticalOrderCode::attackobject : TacticalOrderCode::attackpoint);
		tacOrder.targetWID = target ? target->getWatchID() : 0;
		if (!target)
			tacOrder.attackParams.targetpoint = wPos;
		tacOrder.attackParams.type = bEnergyWeapons ? AttackType::conservingammo : AttackType::destroy;
		tacOrder.attackParams.method = AttackMethod::ranged;
		tacOrder.attackParams.range = (FireRangeType)FireRangeType::optimal;
		tacOrder.attackParams.pursue = controlGui.getFireFromCurrentPos() ? false : true;
		tacOrder.moveparams.wayPath.mode[0] =
			controlGui.getWalk() ? TravelModeType::slow : TravelModeType::fast;
		soundSystem->playDigitalSample(BUTTON5);
		if (target)
			target->getAppearance()->flashBuilding(1.3f, 0.2f, 0xffff0000);
		handleOrders(tacOrder);
		return 1;
	}
	return 0;
}

void MissionInterfaceManager::selectForceGroup(int32_t forceGroup, bool deselect)
{
	Team* pTeam = Team::home;
	bool bAllSelected = deselect ? false : true;
	if (deselect)
	{
		for (size_t i = 0; i < pTeam->getRosterSize(); i++)
		{
			Mover* pMover = (Mover*)pTeam->getMover(i);
			if (pMover && pMover->isOnGUI() && (pMover->getCommander()->getId() == Commander::home->getId()))
			{
				pMover->setSelected(0);
			}
		}
	}
	else // see if the whole force group is already seleced
	{
		for (size_t i = 0; i < pTeam->getRosterSize(); i++)
		{
			Mover* pMover = (Mover*)pTeam->getMover(i);
			if (pMover && pMover->isInUnitGroup(forceGroup) && !pMover->isDisabled() && !pMover->isSelected() && pMover->isOnGUI() && (pMover->getCommander()->getId() == Commander::home->getId()))
			{
				bAllSelected = false;
			}
		}
	}
	for (size_t i = 0; i < pTeam->getRosterSize(); i++)
	{
		Mover* pMover = (Mover*)pTeam->getMover(i);
		if (pMover && pMover->isInUnitGroup(forceGroup) && !pMover->isDisabled() && pMover->isOnGUI() && (pMover->getCommander()->getId() == Commander::home->getId()))
			pMover->setSelected(!bAllSelected);
	}
}
void MissionInterfaceManager::makeForceGroup(int32_t forceGroup)
{
	Team* pTeam = Team::home;
	for (size_t i = 0; i < pTeam->getRosterSize(); i++)
	{
		Mover* pMover = (Mover*)pTeam->getMover(i);
		if (pMover && (pMover->getCommander()->getId() == Commander::home->getId()))
		{
			if (pMover->isInUnitGroup(forceGroup))
				pMover->removeFromUnitGroup(forceGroup);
		}
	}
	for (i = 0; i < pTeam->getRosterSize(); i++)
	{
		Mover* pMover = (Mover*)pTeam->getMover(i);
		if (pMover && (pMover->getCommander()->getId() == Commander::home->getId()))
		{
			if (pMover->isSelected())
				pMover->addToUnitGroup(forceGroup);
		}
	}
}

bool MissionInterfaceManager::moveCameraAround(bool lineOfSight, bool passable, bool ctrl,
	bool bGui, int32_t moverCount, int32_t nonMoverCount)
{
	bool bRetVal = 0;
	bool middleClicked =
		(!userInput->isLeftDrag() && !userInput->isRightDrag() && userInput->isMiddleClick());
	if ((useLeftRightMouseProfile && ((userInput->isLeftClick() && userInput->getKeyDown(KEY_T)) || userInput->isLeftDoubleClick()) && target) || (!useLeftRightMouseProfile && userInput->isRightClick() && !userInput->isRightDrag() && target) && !bGui)
	{
		if (eye)
			((GameCamera*)eye)->setTarget(target);
	}
	if (!cameraClicked)
	{
		if (mouseX <= (screenScrollLeft))
			scrollLeft();
		if ((mouseX >= (Environment.screenwidth - screenScrollRight)))
			scrollRight();
		if ((mouseY <= (screenScrollUp) && mouseY >= -screenScrollUp))
			scrollUp();
		if ((mouseY >= (Environment.screenheight - screenScrollDown)))
			scrollDown();
	}
	if (attilaXAxis < -ATTILA_THRESHOLD)
	{
		float oldScrollInc = scrollInc;
		scrollInc *= fabs(attilaXAxis) * ATTILA_FACTOR;
		scrollLeft();
		scrollInc = oldScrollInc;
	}
	if (attilaXAxis > ATTILA_THRESHOLD)
	{
		float oldScrollInc = scrollInc;
		scrollInc *= fabs(attilaXAxis) * ATTILA_FACTOR;
		scrollRight();
		scrollInc = oldScrollInc;
	}
	if (attilaYAxis < -ATTILA_THRESHOLD)
	{
		float oldScrollInc = scrollInc;
		scrollInc *= fabs(attilaYAxis) * ATTILA_FACTOR;
		scrollUp();
		scrollInc = oldScrollInc;
	}
	if (attilaYAxis > ATTILA_THRESHOLD)
	{
		float oldScrollInc = scrollInc;
		scrollInc *= fabs(attilaYAxis) * ATTILA_FACTOR;
		scrollDown();
		scrollInc = oldScrollInc;
	}
	if (attilaRZAxis < -ATTILA_THRESHOLD)
	{
		eye->rotateLeft(rotationInc * fabs(attilaRZAxis) * frameLength * ATTILA_ROTATION_FACTOR);
	}
	if (attilaRZAxis > ATTILA_THRESHOLD)
	{
		eye->rotateRight(rotationInc * fabs(attilaRZAxis) * frameLength * ATTILA_ROTATION_FACTOR);
	}
	//------------------------------------------------
	// Right drag is camera rotation and tilt now.
	if (cameraClicked)
	{
		bRetVal = 1;
		int32_t mouseXDelta = userInput->getMouseXDelta();
		float actualRot = rotationInc * 0.1f * abs(mouseXDelta);
		if (mouseXDelta > 0)
		{
			eye->rotateRight(actualRot);
			bRetVal = 1;
		}
		else if (mouseXDelta < 0)
		{
			eye->rotateLeft(actualRot);
			bRetVal = 1;
		}
		int32_t mouseYDelta = userInput->getMouseYDelta();
		float actualTilt = rotationInc * 0.1f * abs(mouseYDelta);
		if (mouseYDelta > 0)
		{
			eye->tiltDown(actualTilt);
			bRetVal = 1;
		}
		else if (mouseYDelta < 0)
		{
			eye->tiltUp(actualTilt);
			bRetVal = 1;
		}
		userInput->setMouseCursor(mState_ROTATE_CAMERA);
		//--------------------------------------------------
		// Zoom Camera based on Mouse Wheel input.
		int32_t mouseWheelDelta = userInput->getMouseWheelDelta();
		if (mouseWheelDelta)
		{
			// Mouse wheel just picks zooms now.
			// float actualZoom = zoomInc * abs(mouseWheelDelta) * 0.0001f *
			// eye->getScaleFactor();
			if (mouseWheelDelta > 0)
			{
				zoomChoiceOut();
			}
			else
			{
				zoomChoiceIn();
			}
			if (target)
				userInput->setMouseCursor(makeTargetCursor(lineOfSight, moverCount, nonMoverCount));
			else
				userInput->setMouseCursor(makeNoTargetCursor(
					passable, lineOfSight, ctrl, bGui, moverCount, nonMoverCount));
			bRetVal = 1;
		}
		return bRetVal;
	}
	//--------------------------------------------------
	// Zoom Camera based on Mouse Wheel input.
	int32_t mouseWheelDelta = userInput->getMouseWheelDelta();
	if (mouseWheelDelta)
	{
		// Mouse wheel just picks zooms now.
		// float actualZoom = zoomInc * abs(mouseWheelDelta) * 0.0001f *
		// eye->getScaleFactor();
		if (mouseWheelDelta > 0)
		{
			zoomChoiceOut();
		}
		else
		{
			zoomChoiceIn();
		}
		if (target)
			userInput->setMouseCursor(makeTargetCursor(lineOfSight, moverCount, nonMoverCount));
		else
			userInput->setMouseCursor(
				makeNoTargetCursor(passable, lineOfSight, ctrl, bGui, moverCount, nonMoverCount));
		bRetVal = 1;
	}
	//-----------------------------------------------------------------
	// If middle mouse button is pressed, go to normal tilt, 50% zoom
	if (middleClicked)
	{
		cameraNormal();
	}
	doDrag(bGui);
	return bRetVal;
}

bool MissionInterfaceManager::canJump()
{
	bool canJump = true;
	int32_t i = 0;
	Team* pTeam = Team::home;
	while ((i < pTeam->getRosterSize()))
	{
		Mover* pMover = (Mover*)pTeam->getMover(i);
		if (pMover->isSelected() && pMover->getCommander()->getId() == Commander::home->getId())
		{
			if (!pMover->canJump())
			{
				canJump = false;
				break;
			}
		}
		i++;
	}
	return canJump;
}

bool MissionInterfaceManager::canJumpToWPos()
{
	bool passable = 0;
	if (Terrain::IsGameSelectTerrainPosition(wPos))
	{
		int32_t cellR, cellC;
		land->worldToCell(wPos, cellR, cellC);
		passable = GameMap->getPassable(cellR, cellC);
	}
	if (passable)
	{
		bool canJump = true;
		int32_t i = 0;
		Team* pTeam = Team::home;
		while ((i < pTeam->getRosterSize()))
		{
			Mover* pMover = (Mover*)pTeam->getMover(i);
			if (pMover->isSelected() && pMover->getCommander()->getId() == Commander::home->getId())
			{
				if (pMover->canJump())
				{
					TacticalOrder lastQueuedTacOrder;
					int32_t err = pMover->getPilot()->peekQueuedTacOrder(-1, &lastQueuedTacOrder);
					Stuff::Vector3D lastWayPoint;
					if (err)
						lastWayPoint = pMover->getPosition();
					else
						lastWayPoint = lastQueuedTacOrder.getWayPoint(0);
					Stuff::Vector3D distance;
					distance.Subtract(lastWayPoint, wPos);
					// JUMP distance is always JUST horizontal.  NO Vertical!!
					distance.z = 0.0f;
					float dist = distance.GetLength();
					if (dist > pMover->getJumpRange())
						canJump = false;
				}
				else
					canJump = false;
			}
			i++;
		}
		return canJump;
	}
	return 0;
}

void MissionInterfaceManager::doDrag(bool bGui)
{
	//---------------------------------------------------------------------------
	// Check if we wanted to select all visible.  If so, do it!
	//	if ( ((GetAsyncKeyState( VK_LBUTTON ))) && !bGui && !cameraClicked )
	if (((gos_GetKeyStatus(KEY_LMOUSE) == KEY_PRESSED)) && !bGui && !cameraClicked && !dragStart.x)
	{
		dragStart.x = wPos.x;
		dragStart.y = wPos.y;
		dragStart.z = wPos.z;
		isDragging = 0;
	}
	else if ((gos_GetKeyStatus(KEY_LMOUSE) == KEY_HELD) && !isDragging && (!(dragStart.x == 0.f && dragStart.y == 0.f) && (dragStart.x != wPos.x && dragStart.y != wPos.y)))
	{
		isDragging = TRUE;
		dragEnd.x = mouseX;
		dragEnd.y = mouseY;
	}
	else if (isDragging)
	{
		dragEnd.x = mouseX;
		dragEnd.y = mouseY;
		if ((gos_GetKeyStatus(KEY_LMOUSE) == KEY_RELEASED || gos_GetKeyStatus(KEY_LMOUSE) == KEY_FREE) && isDragging)
		{
			//----------------------------------------------
			// We should select everything in the drag box.
			// AND deselect anything outside of it.
			GameObject* objsInRect[MAX_ICONS];
			int32_t count = 0;
			memset(objsInRect, 0, sizeof(GameObject*) * 16);
			for (size_t i = 0; i < ObjectManager->getNumMovers(); i++)
			{
				Stuff::Vector3D screenStart;
				Stuff::Vector4D screenPos;
				eye->projectZ(dragStart, screenPos);
				screenStart.x = screenPos.x;
				screenStart.y = screenPos.y;
				GameObjectPtr mover = ObjectManager->getMover(i);
				if (ObjectManager->moverInRect(i, screenStart, dragEnd) && mover->getCommanderId() == Commander::home->getId() && count < MAX_ICONS && !mover->isDisabled())
				{
					objsInRect[count] = mover;
					count++;
				}
			}
			if (count)
			{
				// empty the info window of any previously selected guys
				controlGui.setInfoWndMover(nullptr);
				if (!userInput->shift())
				{
					for (i = 0; i < ObjectManager->getNumMovers(); i++)
					{
						ObjectManager->getMover(i)->setSelected(0);
					}
				}
				for (i = 0; i < count; i++)
				{
					objsInRect[i]->setSelected(true);
				}
				if (count == 1)
					(controlGui).setInfoWndMover((Mover*)objsInRect[0]);
			}
			dragStart.Zero();
			dragEnd.Zero();
			isDragging = FALSE;
		}
	}
	else if (gos_GetKeyStatus(KEY_LMOUSE) == KEY_RELEASED)
	{
		dragStart.Zero();
	}
}

bool MissionInterfaceManager::canAddVehicle(const Stuff::Vector3D& pos)
{
	// We're probably going to use this alot!
	int32_t commanderid = Commander::home->getId();
	if (paintingVtol[commanderid])
		return 0;
	int32_t tileI, tileJ;
	land->worldToTile(pos, tileJ, tileI);
	if (tileJ > -1 && tileJ < land->realVerticesMapSide && tileI > -1 && tileI < land->realVerticesMapSide)
	{
		if (Team::home->teamLineOfSight(pos, 0.0f))
		{
			if (GameMap->getPassable(pos) && (!land->getWater((Stuff::Vector3D)pos)))
				return true;
			else
				return false;
		}
	}
	return false;
}

bool MissionInterfaceManager::canRecover(const Stuff::Vector3D& pos)
{
	// We're probably going to use this alot!
	int32_t commanderid = Commander::home->getId();
	if (paintingVtol[commanderid])
		return 0;
	int32_t tileI, tileJ;
	land->worldToTile(pos, tileJ, tileI);
	if (tileJ > -1 && tileJ < land->realVerticesMapSide && tileI > -1 && tileI < land->realVerticesMapSide)
	{
		if (Team::home->teamLineOfSight(pos, 0.0f))
		{
			if (target && (target->getObjectClass() == BATTLEMECH) && target->isDisabled() && !target->isDestroyed())
				return true;
			else
				return false;
		}
	}
	return false;
}

int32_t
MissionInterfaceManager::makeTargetCursor(
	bool lineOfSight, int32_t moverCount, int32_t nonMoverCount)
{
	// We're probably going to use this alot!
	int32_t commanderid = Commander::home->getId();
	int32_t currentCursor = makeRangeCursor(lineOfSight);
	if (!moverCount && !nonMoverCount)
		currentCursor = mState_NORMAL;
	if ((userInput->getKeyDown(WAYPOINT_KEY)))
	{
		currentCursor = mState_DONT;
		return currentCursor;
	}
	if (controlGui.isSelectingInfoObject())
	{
		if (target->isMover())
		{
			currentCursor = mState_INFO;
		}
		else
			currentCursor = mState_DONT;
		return currentCursor;
	}
	else if (controlGui.isAddingVehicle() && !paintingVtol[commanderid])
	{
		currentCursor = mState_DONT;
		return currentCursor;
	}
	else if (controlGui.isAddingSalvage() && !paintingVtol[commanderid])
	{
		if (canSalvage(target))
		{
			currentCursor = mState_SALVAGE;
			target->setTargeted(true);
		}
		else
			currentCursor = mState_XSALVAGE;
		return currentCursor;
	}
	else if (controlGui.isAddingAirstrike() && !paintingVtol[commanderid])
	{
		if (controlGui.getButton(ControlGui::SENSOR_PROBE)->state == ControlButton::PRESSED)
		{
			currentCursor = lineOfSight ? mState_SENSORSTRIKE : mState_UNCERTAIN_AIRSTRIKE;
		}
		else
		{
			currentCursor = lineOfSight ? mState_AIRSTRIKE : mState_UNCERTAIN_AIRSTRIKE;
		}
		return currentCursor;
	}
	else if (controlGui.getSalvage())
	{
		if (canSalvage(target))
		{
			currentCursor = mState_SALVAGE;
			target->setTargeted(true);
		}
		else
			currentCursor = mState_XSALVAGE;
		return currentCursor;
	}
	else if (controlGui.getRepair())
	{
		if (canRepair(target))
			currentCursor = mState_REPAIR;
		else
			currentCursor = mState_XREPAIR;
		if (target)
			target->setTargeted(true);
		return currentCursor;
	}
	else if (controlGui.getMines())
	{
		currentCursor = mState_DONT;
		return currentCursor;
	}
	else if (controlGui.getGuardTower())
	{
		currentCursor = lineOfSight ? mState_GUARDTOWER_LOS : mState_GUARDTOWER;
		return currentCursor;
	}
	else if (target->isCaptureable(Team::home->getId()) && moverCount)
	{
		for (size_t i = 0; i < Team::home->getRosterSize(); i++)
		{
			Mover* pMvr = Team::home->getMover(i);
			if (pMvr->isSelected() && pMvr->getCommander()->getId() == Commander::home->getId())
			{
				if ((pMvr->getObjectClass() == BATTLEMECH) && (pMvr->getMoveType() == MOVETYPE_GROUND))
				{
					if (target->getCaptureBlocker(pMvr))
					{
						currentCursor = mState_XCAPTURE;
						break;
					}
					else
					{
						currentCursor = mState_CAPTURE;
					}
				}
				else
				{
					currentCursor = mState_XCAPTURE;
				}
			}
		}
	}
	switch (target->getObjectClass())
	{
	case BATTLEMECH:
	case GROUNDVEHICLE:
	case ELEMENTAL:
	{
		//---------------------------------------------------------------------------------------
		// if target is enemy battleMech, select it, change cursor and check
		// below for leftClick
		if (target->getTeam() && !target->getTeam()->isFriendly(Team::home) && !target->getTeam()->isNeutral(Team::home))
		{
			target->setTargeted(true);
			if (target->isDisabled())
			{
				currentCursor = mState_NORMAL;
				//				target = nullptr;
			}
		}
		else
		{
			//-------------------------------------------------------------------
			// Otherwise we over a friendly.  Assume we intend to select them if
			// we click.  If we shift-click, just add them.
			target->setTargeted(true); // For friendlies, let us know we need to
				// draw their names!
			currentCursor = mState_NORMAL;
			controlGui.setRolloverHelpText(IDS_UNIT_SELECT_HELP);
		}
	}
	break;
	case BUILDING:
	case TREEBUILDING:
	case TURRET:
	case GATE:
	case TREE:
	case TERRAINOBJECT:
	{
		if (target->getObjectType()->getSubType() == BUILDING_SUBTYPE_LANDBRIDGE)
		{
			// Check if forcing fire or fire from current.  If so, shoot away.
			if (gos_GetKeyStatus((gosEnum_KeyIndex)commands[FORCE_FIRE_KEY].key) == KEY_HELD || controlGui.getFireFromCurrentPos())
			{
				target->setTargeted(true);
			}
			else
			{
				target = nullptr;
				// find out if this position is passable.  Landbridges kinda
				// are, or they'd be useless.
				int32_t cellR, cellC;
				bool passable = 1;
				bool lineOfSight = 0;
				if (Terrain::IsGameSelectTerrainPosition(wPos))
				{
					land->worldToCell(wPos, cellR, cellC);
					passable = GameMap->getPassable(cellR, cellC);
				}
				return makeNoTargetCursor(passable, lineOfSight, 0, 0, moverCount, nonMoverCount);
			}
		}
		//------------------------------------------------------------------
		// No matter what side this is on, change the cursor and select.
		target->setTargeted(true);
		TeamPtr targetTeam = target->getTeam();
		if (targetTeam && Team::home->isFriendly(targetTeam) && target->getFlag(OBJECT_FLAG_CANREFIT) && target->getFlag(OBJECT_FLAG_MECHBAY))
		{
			if (canRepairBay(target))
			{
				currentCursor = mState_REPAIR;
			}
			else
			{
				currentCursor = mState_XREPAIR;
			}
		}
		else if (targetTeam && !controlGui.getGuard() && Team::home->isFriendly(targetTeam) && gos_GetKeyStatus((gosEnum_KeyIndex)commands[FORCE_FIRE_KEY].key) != KEY_HELD // not forcing fire
			&& controlGui.getCurrentRange() == FireRangeType::optimal) // range attacks now are force fire
		{
			return makeNoTargetCursor(false, lineOfSight, 0, 0, moverCount, nonMoverCount);
		}
	}
	break;
	case BRIDGE:
		target = nullptr;
		return makeNoTargetCursor(false, lineOfSight, 0, 0, moverCount, nonMoverCount);
		break;
	case DEBRIS:
	{
		//----------------------------------
		// Do nothing for Debris right now.
	}
	break;
	}
	if (controlGui.getGuard())
	{
		currentCursor = mState_GUARD;
		if (target)
			target->setTargeted(true);
	}
	else if (gos_GetKeyStatus((gosEnum_KeyIndex)commands[FORCE_FIRE_KEY].key) == KEY_HELD || (controlGui.getCurrentRange() != FireRangeType::optimal && controlGui.getCurrentRange() != FireRangeType::current))
	{
		currentCursor = makeRangeCursor(lineOfSight);
	}
	return currentCursor;
}

int32_t
MissionInterfaceManager::makeRangeCursor(bool lineOfSight)
{
	int32_t currentCursor = mState_NORMAL;
	int32_t currentRange = controlGui.getCurrentRange();
	if (bEnergyWeapons)
		return lineOfSight ? mState_ENERGY_WEAPONS_LOS : mState_ENERGY_WEAPONS;
	switch (currentRange)
	{
	case FireRangeType::shortest:
		currentCursor = lineOfSight ? mState_SHRTRNG_LOS : mState_SHRTRNG_ATTACK;
		break;
	case FireRangeType::medium:
		currentCursor = lineOfSight ? mState_MEDRNG_LOS : mState_MEDRNG_ATTACK;
		break;
	case FireRangeType::extended:
		currentCursor = lineOfSight ? mState_LONGRNG_LOS : mState_LONGRNG_ATTACK;
		break;
	case FireRangeType::optimal:
		currentCursor = lineOfSight ? mState_ATTACK_LOS : mState_GENERIC_ATTACK;
		break;
	case FireRangeType::current:
		currentCursor = lineOfSight ? mState_CURPOS_ATTACK_LOS : mState_CURPOS_ATTACK;
		break;
	}
	if (controlGui.getFireFromCurrentPos())
		currentCursor = lineOfSight ? mState_CURPOS_ATTACK_LOS : mState_CURPOS_ATTACK;
	return currentCursor;
}

int32_t
MissionInterfaceManager::makeNoTargetCursor(bool passable, bool lineOfSight, bool bCtrl,
	bool bGui, int32_t moverCount, int32_t nonMoverCount)
{
	//-------------------------------------------------------------------------
	// We are not over enything.  Switch to move cursor based on input.
	// If we are on passable terrain, move.
	// If passable and runCommand, run.
	// If passable and jumpCommand and canJump, jump.
	// If not passable, dont.
	// If passable and jumpCommand and cantJump, dont.
	// We're probably going to use this alot!
	int32_t commanderid = Commander::home->getId();
	int32_t cursorType = makeMoveCursor(lineOfSight);
	if (!moverCount)
		cursorType = mState_NORMAL;
	if (gos_GetKeyStatus((gosEnum_KeyIndex)commands[FORCE_FIRE_KEY].key) == KEY_HELD // not forcing fire
		|| controlGui.getFireFromCurrentPos())
	{
		return makeRangeCursor(lineOfSight);
	}
	if (!passable && !selectionIsHelicopters())
		cursorType = mState_DONT;
	if (controlGui.isSelectingInfoObject())
	{
		cursorType = mState_DONT;
		return cursorType;
	}
	else if (controlGui.isAddingAirstrike() && !paintingVtol[commanderid])
	{
		if (bGui && (!controlGui.isOverTacMap() || !controlGui.isButtonPressed(ControlGui::SENSOR_PROBE)))
			cursorType = mState_NORMAL;
		else
		{
			if (controlGui.getButton(ControlGui::SENSOR_PROBE)->state == ControlButton::PRESSED)
			{
				cursorType = lineOfSight ? mState_SENSORSTRIKE : mState_UNCERTAIN_AIRSTRIKE;
			}
			else
				cursorType = lineOfSight ? mState_AIRSTRIKE : mState_UNCERTAIN_AIRSTRIKE;
		}
		return cursorType;
	}
	else if (controlGui.isAddingVehicle())
	{
		if (canAddVehicle(wPos) && !bGui)
			cursorType = mState_VEHICLE;
		else if (!paintingVtol[commanderid] && !bGui)
			cursorType = mState_CANTVEHICLE;
		else if (bGui)
			cursorType = mState_NORMAL;
	}
	else if (controlGui.isAddingSalvage())
	{
		if (canRecover(wPos) && !bGui)
			cursorType = mState_SALVAGE;
		else if (!paintingVtol[commanderid] & !bGui)
			cursorType = mState_XSALVAGE;
		else if (bGui)
			cursorType = mState_NORMAL;
	}
	else if (userInput->getKeyDown(WAYPOINT_KEY) || controlGui.getMines())
	{
		if (controlGui.getMines())
		{
			if (!passable)
				cursorType = mState_XMINES;
			else
				cursorType = mState_LAYMINES;
		}
		else
		{
			cursorType = lineOfSight ? mState_WALKWAYPT_LOS : mState_WALKWAYPT;
			if (controlGui.getRun())
				cursorType = lineOfSight ? mState_RUNWAYPT_LOS : mState_RUNWAYPT;
			else if (controlGui.getJump())
				cursorType = lineOfSight ? mState_JUMPWAYPT_LOS : mState_JUMPWAYPT;
		}
		return cursorType;
	}
	else if (controlGui.getJump())
	{
		if (canJumpToWPos())
		{
			if (bCtrl)
				cursorType = lineOfSight ? mState_JUMPWAYPT_LOS : mState_JUMPWAYPT;
			else
				cursorType = makeJumpCursor(lineOfSight);
			controlGui.setRolloverHelpText(IDS_UNIT_HELP);
		}
		else
			cursorType = mState_DONT;
		if (!passable && !selectionIsHelicopters())
			cursorType = mState_DONT;
	}
	else if (controlGui.getRun() && moverCount)
	{
		if (bCtrl)
			cursorType = lineOfSight ? mState_RUNWAYPT_LOS : mState_RUNWAYPT;
		else
			cursorType = makeRunCursor(lineOfSight);
		if (!passable && !selectionIsHelicopters())
			cursorType = mState_DONT;
		else
			controlGui.setRolloverHelpText(IDS_UNIT_HELP);
	}
	else if (controlGui.getMines())
	{
		cursorType = mState_LAYMINES;
		if (!passable)
			cursorType = mState_XMINES;
	}
	else if (controlGui.getSalvage())
	{
		cursorType = makeMoveCursor(lineOfSight);
		if (!passable)
			cursorType = mState_DONT;
	}
	else if (controlGui.getRepair())
	{
		cursorType = makeMoveCursor(lineOfSight);
		if (!passable)
			cursorType = mState_DONT;
	}
	else if (controlGui.getGuard())
	{
		cursorType = lineOfSight ? mState_GUARD_LOS : mState_GUARD;
		if (!passable)
			cursorType = mState_DONT;
	}
	else if (controlGui.getGuardTower())
	{
		cursorType = lineOfSight ? mState_GUARDTOWER_LOS : mState_GUARDTOWER;
	}
	else if (moverCount)
	{
		controlGui.setRolloverHelpText(IDS_UNIT_HELP);
	}
	userInput->setMouseFrame(0);
	return cursorType;
}

void MissionInterfaceManager::addAirstrike()
{
	if (controlGui.isButtonPressed(ControlGui::LARGE_AIRSTRIKE))
		bigAirStrike();
	else if (controlGui.isButtonPressed(ControlGui::LARGE_AIRSTRIKE))
		smlAirStrike();
	else if (controlGui.isButtonPressed(ControlGui::SENSOR_PROBE))
		snsAirStrike();
	else
		gosASSERT(false);
	controlGui.unPressAllVehicleButtons();
}

//-----------------------------------------------------------------------------

std::unique_ptr<Mover>
BringInReinforcement(
	int32_t vehicleID, int32_t rosterIndex, int32_t commanderid, Stuff::Vector3D pos, bool exists)
{
	MoverInitData data;
	memset(&data, 0, sizeof(data));
	std::wstring_view vehicleFile =
		(std::wstring_view)MissionInterfaceManager::instance()->getSupportVehicleNameFromID(vehicleID);
	if (!vehicleFile)
	{
		wchar_t s[1024];
		sprintf(s, "BringInReinforcement: null behicleFile (%d)", vehicleID);
		STOP((s));
		// return(nullptr);
	}
	strcpy(data.pilotFileName, vehicleFile);
	strcpy(data.brainFileName, "pbrain");
	data.controlType = 2;
	data.controlDataType = 2;
	data.rosterIndex = rosterIndex;
	data.objNumber = vehicleID;
	data.position = pos;
	data.rotation = 0;
	data.teamID = Commander::commanders[commanderid]->team->getId();
	data.commanderid = commanderid;
	data.active = 1;
	data.exists = exists;
	data.capturable = 0;
	data.basecolour = prefs.basecolour;
	data.highlightcolour1 = prefs.highlightcolour;
	data.highlightcolour2 = prefs.highlightcolour;
	if (MPlayer)
	{
		MC2Player* pInfo = MPlayer->getPlayerInfo(commanderid);
		if (pInfo)
		{
			data.basecolour = MPlayer->colors[pInfo->basecolour[BASECOLOR_TEAM]];
			data.highlightcolour1 = MPlayer->colors[pInfo->stripecolour];
			data.highlightcolour2 = MPlayer->colors[pInfo->stripecolour];
		}
	}
	int32_t moverId = mission->addMover(&data);
	if ((moverId != -1) && (moverId != 0))
	{
		// Start the helicopters landed!!
		std::unique_ptr<Mover> newMover = (std::unique_ptr<Mover>)ObjectManager->get(moverId);
		if (!newMover)
			return nullptr;
		if (newMover->getMoveType() == MOVETYPE_AIR)
			newMover->startShutDown();
		if (!MPlayer)
			newMover->setLocalMoverId(0);
		// Somehow this is set during a quickLoad?
		newMover->disableThisFrame = false;
		return (newMover);
	}
	return nullptr;
}

//-----------------------------------------------------------------------------

void MissionInterfaceManager::addVehicle(const Stuff::Vector3D& pos)
{
	//	LogisticsPilot* pPilot =
	// LogisticsData::instance->getFirstAvailablePilot(); 	std::wstring_view pChar =
	// pPilot->getFileName(); 	if ( !pChar ) 		return;
	// Check if vehicleFile is nullptr.  If it is, simply get it.
	// Can happen if we get here BEFORE beginVtol?
	// Happens when we select a vehicle which costs exactly the number of points
	// we have left. Heidi deletes the points, which shuts off the button, which
	// causes getVehicleName to return nullptr. D'OH!!! Delete the points in
	// getVehicleName when the vehicle is actually added!!!
	//
	// -fs
	if (!vehicleFile)
		vehicleFile = controlGui.getVehicleName(vehicleID[Commander::home->getId()]);
	if (!vehicleFile)
		return;
	reinforcement = BringInReinforcement(
		vehicleID[Commander::home->getId()], 255, Commander::home->getId(), pos, true);
}

void MissionInterfaceManager::beginVtol(
	int32_t supportID, int32_t commanderid, Stuff::Vector3D* reinforcePos, std::unique_ptr<Mover> salvageTarget)
{
	Stuff::Vector3D vtolPos = wPos;
	if (reinforcePos)
		vtolPos = *reinforcePos;
	if (supportID == -1)
	{
		vehicleFile = controlGui.getVehicleName(vehicleID[commanderid]);
	}
	else
	{
		//-----------------------
		// used in multiplayer...
		vehicleID[commanderid] = supportID;
		vehicleFile = controlGui.getVehicleNameFromID(supportID);
	}
	if (vehicleID[commanderid] == 0)
	{
		wchar_t s[1024];
		sprintf(s,
			"beginvtol: supportID = %d, commanderid = %d, vehicleID = %d, "
			"vehicleFile = %s",
			supportID, commanderid, vehicleID[commanderid], vehicleFile);
		PAUSE((s));
	}
	if (!vTol[commanderid])
	{
		if (vehicleID[commanderid] != 147) // NOT a recovery vehicle.  Standard VTOL
		{
			if (supportID == -1)
			{
				// Do not call this unless the commanderid passed in equals us
				// or will you do something outside here?
				if (MPlayer && !reinforcePos)
				{
					vPos[commanderid] = vtolPos;
					MPlayer->sendReinforcement(
						vehicleID[commanderid], 255, "noone", commanderid, vPos[commanderid], 0);
					return;
				}
			}
			AppearanceType* appearanceType =
				appearanceTypeList->getAppearance(BLDG_TYPE << 24, "vtol");
			vTol[commanderid] = new BldgAppearance;
			vTol[commanderid]->init(appearanceType);
			if (!dustCloud[commanderid] && prefs.useNonWeaponEffects)
			{
				if (strcmp(weaponEffects->GetEffectName(VTOL_DUST_CLOUD), "NONE") != 0)
				{
					//--------------------------------------------
					// Yes, load it on up.
					unsigned flags = 0;
					Check_Object(gosFX::EffectLibrary::Instance);
					gosFX::Effect::Specification* gosEffectSpec =
						gosFX::EffectLibrary::Instance->Find(
							weaponEffects->GetEffectName(VTOL_DUST_CLOUD));
					if (gosEffectSpec)
					{
						dustCloud[commanderid] = gosFX::EffectLibrary::Instance->MakeEffect(
							gosEffectSpec->m_effectID, flags);
						gosASSERT(dustCloud[commanderid] != nullptr);
						MidLevelRenderer::MLRTexturePool::Instance->LoadImages();
					}
				}
			}
		}
		else // IS a recovery vehicle.  DO NOT create VTOL, use KARNOV instead.
			// NO Vehicle created either
		{
			if (supportID == -1)
			{
				if (MPlayer && !reinforcePos) // Probably same as above here, too.
				{
					vPos[commanderid] = vtolPos;
					if (!salvageTarget)
						STOP(("MissionGUI.beginvtol: null target PASSED IN."));
					std::wstring_view newPilotName =
						(std::wstring_view)LogisticsData::instance->getBestPilot(salvageTarget->tonnage);
					if (vehicleID[commanderid] != 147)
						STOP(("missiongui.beginvtol: bad vehicleID"));
					MPlayer->sendReinforcement(vehicleID[commanderid],
						salvageTarget->netRosterIndex, newPilotName, commanderid, vPos[commanderid],
						3);
					return;
				}
			}
			AppearanceType* appearanceType =
				appearanceTypeList->getAppearance(BLDG_TYPE << 24, "karnov");
			vTol[commanderid] = new BldgAppearance;
			vTol[commanderid]->init(appearanceType);
			if (MPlayer)
				mechToRecover[commanderid] =
					MPlayer->moverRoster[MPlayer->reinforcements[commanderid][1]];
			else
				mechToRecover[commanderid] = salvageTarget;
			mechRecovered[commanderid] =
				false; // Need to know when mech is done so Karnov can fly away.
			if (!dustCloud[commanderid] && prefs.useNonWeaponEffects)
			{
				if (strcmp(weaponEffects->GetEffectName(KARNOV_DUST_CLOUD), "NONE") != 0)
				{
					//--------------------------------------------
					// Yes, load it on up.
					unsigned flags = 0;
					Check_Object(gosFX::EffectLibrary::Instance);
					gosFX::Effect::Specification* gosEffectSpec =
						gosFX::EffectLibrary::Instance->Find(
							weaponEffects->GetEffectName(KARNOV_DUST_CLOUD));
					if (gosEffectSpec)
					{
						dustCloud[commanderid] = gosFX::EffectLibrary::Instance->MakeEffect(
							gosEffectSpec->m_effectID, flags);
						gosASSERT(dustCloud[commanderid] != nullptr);
						MidLevelRenderer::MLRTexturePool::Instance->LoadImages();
					}
				}
			}
			if (!recoveryBeam[commanderid])
			{
				if (strcmp(weaponEffects->GetEffectName(KARNOV_RECOVERY_BEAM), "NONE") != 0)
				{
					//--------------------------------------------
					// Yes, load it on up.
					unsigned flags = 0;
					Check_Object(gosFX::EffectLibrary::Instance);
					gosFX::Effect::Specification* gosEffectSpec =
						gosFX::EffectLibrary::Instance->Find(
							weaponEffects->GetEffectName(KARNOV_RECOVERY_BEAM));
					if (gosEffectSpec)
					{
						recoveryBeam[commanderid] = gosFX::EffectLibrary::Instance->MakeEffect(
							gosEffectSpec->m_effectID, flags);
						gosASSERT(recoveryBeam[commanderid] != nullptr);
						MidLevelRenderer::MLRTexturePool::Instance->LoadImages();
					}
				}
			}
		}
	}
	if (commanderid == Commander::home->getId())
	{
		switch (vehicleID[commanderid])
		{
		case 120: // Minelayer
			soundSystem->playSupportSample(SUPPORT_MINELAYER);
			break;
		case 182: // Repair
			soundSystem->playSupportSample(SUPPORT_REPAIR);
			break;
		case 393: // Scout
			soundSystem->playSupportSample(SUPPORT_SCOUT);
			break;
		case 147: // Recovery
			soundSystem->playSupportSample(SUPPORT_RECOVER);
			break;
		case 415: // Artillery
			soundSystem->playSupportSample(SUPPORT_ARTILLERY);
			break;
		}
	}
	Stuff::Vector3D newPos = vtolPos;
	float rotation = 0.f; // this needs to be pointing 180 degrees from drop point
	if (vehicleID[commanderid] == 147)
	{
		// Move wPos to be over the cockpit of the downed mech.
		if (mechToRecover[commanderid] && mechToRecover[commanderid]->appearance)
			newPos = mechToRecover[commanderid]->appearance->getNodeNamePosition("cockpit");
	}
	if (vehicleID[commanderid] != 147)
		soundSystem->playDigitalSample(VTOL_ANIMATE, newPos);
	else
		soundSystem->playDigitalSample(SALVAGE_CRAFT, newPos);
	vTol[commanderid]->setObjectParameters(newPos, 0, false, Team::home->id, 0);
	eye->update();
	vTol[commanderid]->update();
	vTol[commanderid]->setInView(true);
	vTol[commanderid]->setVisibility(1, 1);
	vTol[commanderid]->rotation = rotation;
	if (vehicleID[commanderid] != 147)
		vTol[commanderid]->setGesture(0); // Start Animation at beginning
	else
		vTol[commanderid]->setGesture(1); // Start Animation with Landing -- KARNOV
	if (dustCloud[commanderid])
	{
		Stuff::LinearMatrix4D shapeOrigin;
		Stuff::LinearMatrix4D localToWorld;
		Stuff::LinearMatrix4D localResult;
		Stuff::Vector3D dustPos = vTol[commanderid]->position;
		Stuff::Point3D wakePos;
		wakePos.x = -dustPos.x;
		wakePos.y = dustPos.z;
		wakePos.z = dustPos.y;
		shapeOrigin.BuildRotation(Stuff::EulerAngles(0.0f, 0.0f, 0.0f));
		shapeOrigin.BuildTranslation(wakePos);
		gosFX::Effect::ExecuteInfo info((Stuff::Time)scenarioTime, &shapeOrigin, nullptr);
		dustCloud[commanderid]->SetLoopOff();
		dustCloud[commanderid]->SetExecuteOn();
		dustCloud[commanderid]->Start(&info);
	}
	paintingVtol[commanderid] = 1;
	vTolSpeed = -75.f;
	vPos[commanderid] = vtolPos;
	vehicleDropped[commanderid] = false;
	if (!MPlayer || commanderid == MPlayer->commanderid)
		controlGui.disableAllVehicleButtons();
}

bool MissionInterfaceManager::canSalvage(GameObject* pMover)
{
	bool lineOfSight = Team::home->teamLineOfSight(pMover->getPosition(), 0.0f);
	return (controlGui.forceGroupBar.getIconCount() < MAX_ICONS && target->isDisabled() && lineOfSight && target->getObjectClass() == BATTLEMECH)
		? 1
		: 0;
}

void MissionInterfaceManager::doSalvage()
{
	if (canSalvage(target))
	{
		TacticalOrder tacOrder;
		tacOrder.init(OrderOriginType::player, TacticalOrderCode::recover);
		tacOrder.targetWID = target->getWatchID();
		tacOrder.attackParams.type = AttackType::none;
		tacOrder.attackParams.method = AttackMethod::ramming;
		tacOrder.attackParams.pursue = true;
		tacOrder.moveparams.wayPath.mode[0] = TravelModeType::fast;
		handleOrders(tacOrder);
	}
}

bool MissionInterfaceManager::canRepair(GameObject* pMover)
{
	if (!pMover)
		return 0;
	if (!pMover->isMover())
		return 0;
	if (Team::home->isEnemy(pMover->getTeam()))
		return 0;
	if (pMover->isDisabled() || pMover->isDestroyed())
		return 0;
	// find the selected guy
	int32_t watchID = 0;
	Team* pTeam = Team::home;
	for (size_t i = 0; i < pTeam->getRosterSize(); i++)
	{
		Mover* pTmpMover = (Mover*)pTeam->getMover(i);
		if (pTmpMover->isSelected() && pTmpMover->getCommander()->getId() == Commander::home->getId())
		{
			if (!watchID)
				watchID = pTmpMover->getWatchID();
			else
				watchID = -1;
		}
	}
	if (((Mover*)pMover)->needsRefit() || (((Mover*)pMover)->refitBuddyWID == watchID && watchID))
		return true;
	return 0;
}
void MissionInterfaceManager::doRepair(GameObject* who)
{
	TacticalOrder tacOrder;
	tacOrder.init(OrderOriginType::player, TacticalOrderCode::refit, false);
	tacOrder.targetWID = who->getWatchID();
	tacOrder.selectionindex = -1;
	tacOrder.moveparams.wayPath.mode[0] =
		controlGui.getWalk() ? TravelModeType::slow : TravelModeType::fast;
	tacOrder.moveparams.faceObject = true;
	tacOrder.moveparams.fromArea = -1;
	tacOrder.moveparams.wait = false;
	handleOrders(tacOrder);
	soundSystem->playDigitalSample(BUTTON5);
	controlGui.setDefaultSpeed();
}

bool MissionInterfaceManager::canRepairBay(GameObject* bay)
{
	if (bay && bay->getRefitPoints() > 0.0f)
	{
		Team* pTeam = Team::home;
		for (size_t i = 0; i < pTeam->getRosterSize(); i++)
		{
			Mover* pMover = (Mover*)pTeam->getMover(i);
			if (pMover->isSelected() && pMover->needsRefit() && pMover->getCommander()->getId() == Commander::home->getId())
			{
				return true;
			}
		}
	}
	return false;
}
void MissionInterfaceManager::doRepairBay(GameObject* who)
{
	TacticalOrder tacOrder;
	tacOrder.init(OrderOriginType::player, TacticalOrderCode::getfixed, false);
	tacOrder.targetWID = who->getWatchID();
	tacOrder.selectionindex = -1;
	tacOrder.moveparams.wayPath.mode[0] =
		controlGui.getWalk() ? TravelModeType::slow : TravelModeType::fast;
	tacOrder.moveparams.faceObject = true;
	tacOrder.moveparams.fromArea = -1;
	tacOrder.moveparams.wait = false;
	handleOrders(tacOrder);
	soundSystem->playDigitalSample(BUTTON5);
	controlGui.setDefaultSpeed();
}

int32_t
MissionInterfaceManager::vehicleCommand()
{
	if (controlGui.getVehicleCommand())
		controlGui.setVehicleCommand(false);
	else
		controlGui.setVehicleCommand(true);
	return 0;
}

int32_t
MissionInterfaceManager::showObjectives()
{
	controlGui.startObjectives(!controlGui.objectivesStarted());
	return 0;
}

int32_t
MissionInterfaceManager::showObjectives(bool on)
{
	controlGui.startObjectives(on);
	return 0;
}

int32_t
MissionInterfaceManager::togglePause()
{
	if (!bPausedWithoutMenu)
	{
		bPaused ^= 1;
	}
	else
		bPausedWithoutMenu = 0;
	soundSystem->playDigitalSample(LOG_NEXTBACKBUTTONS);
	if (bPaused)
	{
		controlGui.beginPause();
	}
	else
	{
		controlGui.endPause();
	}
	return 1;
}

int32_t
MissionInterfaceManager::togglePauseWithoutMenu()
{
	bPausedWithoutMenu;
	if (!bPausedWithoutMenu)
	{
		if (!bPaused)
		{
			bPausedWithoutMenu = 1;
			bPaused = 1;
			return 1;
		}
		else
		{
			return 0;
		}
	}
	else
	{
		gosASSERT(bPaused);
		bPausedWithoutMenu = 0;
		bPaused = 0;
		return 1;
	}
}

bool MissionInterfaceManager::isPaused()
{
	return bPaused;
}

bool MissionInterfaceManager::isPausedWithoutMenu()
{
	return bPausedWithoutMenu;
}

void MissionInterfaceManager::swapResolutions()
{
	controlGui.swapResolutions(Environment.screenwidth);
	resolution = Environment.screenwidth;
	keyboardRef->init();
}

int32_t
MissionInterfaceManager::switchTab()
{
	controlGui.switchTabs(1);
	return 1;
}

int32_t
MissionInterfaceManager::reverseSwitchTab()
{
	controlGui.switchTabs(-1);
	return 1;
}

int32_t
MissionInterfaceManager::infoCommand()
{
	if (!controlGui.infoButtonPressed())
		controlGui.pressInfoButton();
	return 1;
}

int32_t
MissionInterfaceManager::infoButtonReleased()
{
	if (controlGui.infoButtonPressed())
		controlGui.pressInfoButton();
	return 1;
}
int32_t
MissionInterfaceManager::energyWeapons()
{
	if (gos_GetKeyStatus((gosEnum_KeyIndex)(commands[ENERGY_WEAPON_INDEX].key & 0x0000ffff)) == KEY_HELD)
		bEnergyWeapons = 1;
	else
		bEnergyWeapons = 0;
	return 0;
}
int32_t
MissionInterfaceManager::sendAirstrike()
{
	controlGui.pressAirstrikeButton();
	return 1;
}
int32_t
MissionInterfaceManager::sendLargeAirstrike()
{
	controlGui.pressLargeAirstrikeButton();
	return 1;
}
int32_t
MissionInterfaceManager::gotoNextNavMarker()
{
	// need to find the next nav marker....
	for (CObjectives::EIterator iter = Team::home->objectives.Begin(); !iter.IsDone(); iter++)
	{
		if ((*iter)->Status(Team::home->objectives) == OS_UNDETERMINED && (*iter)->DisplayMarker() == NAV)
		{
			Stuff::Vector3D pos;
			pos.x = (*iter)->MarkerX();
			pos.y = (*iter)->MarkerY();
			if (!pos.x && !pos.y)
				continue;
			pos.z = land->getTerrainElevation(pos);
			LocationNode path;
			path.location = pos;
			path.run = true;
			path.next = nullptr;
			TacticalOrder tacOrder;
			tacOrder.init(OrderOriginType::player, TacticalOrderCode::movetopoint, false);
			tacOrder.initWayPath(&path);
			tacOrder.moveparams.wait = false;
			tacOrder.moveparams.wayPath.mode[0] =
				controlGui.getWalk() ? TravelModeType::slow : TravelModeType::fast;
			if (controlGui.getMines())
				tacOrder.moveparams.mode = SpecialMoveMode::minelaying;
			tacOrder.pack(nullptr, nullptr);
			handleOrders(tacOrder);
			soundSystem->playDigitalSample(BUTTON5);
			return 1;
		}
	}
	// if we got here there weren't any valid markers, play bad sound
	soundSystem->playDigitalSample(INVALID_GUI);
	return 0;
}
int32_t
MissionInterfaceManager::sendSensorStrike()
{
	controlGui.pressSensorStrikeButton();
	return 1;
}

int32_t
MissionInterfaceManager::quickDebugInfo()
{
#ifndef FINAL
	static double lastTime = 0.0;
	if ((lastTime + 0.5) < gos_GetElapsedTime())
	{
		int32_t row, col;
		land->worldToCell(wPos, row, col);
		wchar_t debugString[256];
		if (target)
			sprintf(debugString,
				"INFO = %s(%c%c) %d(W%d) [%d, %d] (%.4f, %.4f, %.4f) "
				"#Pth=%d(%d) OBJ = %s\n",
				terrainStr[GameMap->getTerrain(row, col)],
				GameMap->getPassable(row, col) ? 'T' : 'F',
				GameMap->getForest(row, col) ? 'T' : 'F',
				GlobalMoveMap[target->getMoveLevel()]->calcArea(row, col),
				GameMap->getShallowWater(row, col) ? 1 : (GameMap->getDeepWater(row, col) ? 2 : 0),
				row, col, wPos.x, wPos.y, wPos.z, PathManager->m_numpaths, PathManager->m_peakpaths,
				target->getName());
		else
			sprintf(debugString,
				"INFO = %s(%c%c) %d(W%d) [%d, %d] (%.4f, %.4f, %.4f) "
				"#Pth=%d(%d)\n",
				terrainStr[GameMap->getTerrain(row, col)],
				GameMap->getPassable(row, col) ? 'T' : 'F',
				GameMap->getForest(row, col) ? 'T' : 'F', GlobalMoveMap[0]->calcArea(row, col),
				GameMap->getShallowWater(row, col) ? 1 : (GameMap->getDeepWater(row, col) ? 2 : 0),
				row, col, wPos.x, wPos.y, wPos.z, PathManager->m_numpaths, PathManager->m_peakpaths);
		DEBUGWINS_print(debugString);
		sprintf(debugString, "REQ =");
		wchar_t s[10];
		for (size_t i = 0; i < 25; i++)
		{
			sprintf(s, " %02d", PathManager->m_sourcetally[i]);
			strcat(debugString, s);
		}
		DEBUGWINS_print(debugString);
		lastTime = gos_GetElapsedTime();
	}
#endif
	return (1);
}

int32_t
MissionInterfaceManager::setGameObjectWindow()
{
#ifndef FINAL
	static double lastTime = 0.0;
	if ((lastTime + 0.5) < gos_GetElapsedTime())
	{
		DEBUGWINS_setGameObject(-1, target);
		lastTime = gos_GetElapsedTime();
	}
#endif
	return (1);
}

int32_t
MissionInterfaceManager::pageGameObjectWindow1()
{
#ifndef FINAL
	static double lastTime = 0.0;
	if ((lastTime + 0.5) < gos_GetElapsedTime())
	{
		if (DebugGameObject[0] && DebugGameObject[0]->isMover())
			((std::unique_ptr<Mover>)DebugGameObject[0])->debugPage++;
		lastTime = gos_GetElapsedTime();
	}
#endif
	return (1);
}

int32_t
MissionInterfaceManager::pageGameObjectWindow2()
{
#ifndef FINAL
	static double lastTime = 0.0;
	if ((lastTime + 0.5) < gos_GetElapsedTime())
	{
		if (DebugGameObject[1] && DebugGameObject[1]->isMover())
			((std::unique_ptr<Mover>)DebugGameObject[1])->debugPage++;
		lastTime = gos_GetElapsedTime();
	}
#endif
	return (1);
}

int32_t
MissionInterfaceManager::pageGameObjectWindow3()
{
#ifndef FINAL
	static double lastTime = 0.0;
	if ((lastTime + 0.5) < gos_GetElapsedTime())
	{
		if (DebugGameObject[2] && DebugGameObject[2]->isMover())
			((std::unique_ptr<Mover>)DebugGameObject[2])->debugPage++;
		lastTime = gos_GetElapsedTime();
	}
#endif
	return (1);
}

int32_t
MissionInterfaceManager::rotateObjectLeft()
{
#ifndef FINAL
	static double lastTime = 0.0;
	if ((lastTime + 0.5) < gos_GetElapsedTime())
	{
		if (target)
			target->rotate(4.0);
		lastTime = gos_GetElapsedTime();
	}
#endif
	return (1);
}

int32_t
MissionInterfaceManager::rotateObjectRight()
{
#ifndef FINAL
	static double lastTime = 0.0;
	if ((lastTime + 0.5) < gos_GetElapsedTime())
	{
		if (target)
			target->rotate(-4.0);
		lastTime = gos_GetElapsedTime();
	}
#endif
	return (1);
}

int32_t
MissionInterfaceManager::jumpToDebugGameObject1()
{
#ifndef FINAL
	DEBUGWINS_viewGameObject(0);
#endif
	return (1);
}

int32_t
MissionInterfaceManager::jumpToDebugGameObject2()
{
#ifndef FINAL
	DEBUGWINS_viewGameObject(1);
#endif
	return (1);
}

int32_t
MissionInterfaceManager::jumpToDebugGameObject3()
{
#ifndef FINAL
	DEBUGWINS_viewGameObject(2);
#endif
	return (1);
}

int32_t
MissionInterfaceManager::toggleDebugWins()
{
#ifndef FINAL
	static int32_t debugWinsState = 0;
	static double lastTime = 0.0;
	if ((lastTime + 0.5) < gos_GetElapsedTime())
	{
		debugWinsState = (++debugWinsState % 6);
		bool debugStateFlags[6][7] = {{false, false, false, false, false, false, false},
			{false, false, false, false, false, true, false},
			{false, false, false, false, false, true, true},
			{false, true, true, true, false, true, true},
			{true, true, true, true, false, true, true},
			{false, false, false, false, true, false, true}};
		DEBUGWINS_display(debugStateFlags[debugWinsState]);
		lastTime = gos_GetElapsedTime();
	}
#endif
	return (1);
}

extern bool ShowMovers;
int32_t
MissionInterfaceManager::showMovers()
{
#ifndef FINAL
	static double lastTime = 0.0;
	if ((lastTime + 0.5) < gos_GetElapsedTime())
	{
		ShowMovers = !ShowMovers;
		lastTime = gos_GetElapsedTime();
	}
#endif
	return (1);
}

int32_t
MissionInterfaceManager::cullPathAreas()
{
#ifndef FINAL
	static double lastTime = 0.0;
	if ((lastTime + 0.5) < gos_GetElapsedTime())
	{
		CullPathAreas = !CullPathAreas;
		lastTime = gos_GetElapsedTime();
	}
#endif
	return (1);
}

int32_t
MissionInterfaceManager::zeroHPrime()
{
	static double lastTime = 0.0;
	if ((lastTime + 0.5) < gos_GetElapsedTime())
	{
		ZeroHPrime = !ZeroHPrime;
		lastTime = gos_GetElapsedTime();
	}
	return (1);
}

int32_t
MissionInterfaceManager::calcValidAreaTable()
{
	static double lastTime = 0.0;
	if ((lastTime + 0.5) < gos_GetElapsedTime())
	{
		CalcValidAreaTable = !CalcValidAreaTable;
		lastTime = gos_GetElapsedTime();
	}
	return (1);
}

int32_t
MissionInterfaceManager::teleport()
{
#ifndef FINAL
	static double lastTime = 0.0;
	if ((lastTime + 0.5) < gos_GetElapsedTime())
	{
		Team* pTeam = Team::home;
		Stuff::Vector3D moveGoals[MAX_MOVERS];
		int32_t numMovers = 0;
		for (size_t i = 0; i < pTeam->getRosterSize(); i++)
		{
			Mover* mover = pTeam->getMover(i);
			if (mover->isSelected() && mover->getCommander()->getId() == Commander::home->getId())
				numMovers++;
		}
		MoverGroup::calcMoveGoals(wPos, numMovers, moveGoals);
		numMovers = 0;
		for (i = 0; i < pTeam->getRosterSize(); i++)
		{
			Mover* mover = (Mover*)pTeam->getMover(i);
			if (mover->isSelected() && mover->getCommander()->getId() == Commander::home->getId())
				((std::unique_ptr<Mover>)mover)->setTeleportPosition(moveGoals[numMovers++]);
		}
		lastTime = gos_GetElapsedTime();
	}
#endif
	return (1);
}

extern GameLog* LRMoveLog;

extern bool quitGame;

int32_t
MissionInterfaceManager::globalMapLog()
{
#ifndef FINAL
	static double lastTime = 0.0;
	if (MPlayer) // Otherwise we crash in singlePlayer!!
	{
		if ((lastTime + 0.5) < gos_GetElapsedTime())
		{
			// MPlayer->leaveSession();
			// quitGame = true;
			// GlobalMap::toggleLog();
			lastTime = gos_GetElapsedTime();
		}
	}
#endif
	return (1);
}

int32_t
MissionInterfaceManager::brainDead()
{
#ifndef FINAL
	static double lastTime = 0.0;
	if ((lastTime + 0.5) < gos_GetElapsedTime())
	{
		for (size_t i = 1; i < MAX_TEAMS; i++)
			MechWarrior::brainsEnabled[i] = !MechWarrior::brainsEnabled[i];
		lastTime = gos_GetElapsedTime();
	}
#endif
	return (1);
}

int32_t
MissionInterfaceManager::goalPlan()
{
#ifndef FINAL
	static double lastTime = 0.0;
	if ((lastTime + 0.5) < gos_GetElapsedTime())
	{
		Team* pTeam = Team::home;
		int32_t numMovers = 0;
		for (size_t i = 0; i < pTeam->getRosterSize(); i++)
		{
			Mover* mover = pTeam->getMover(i);
			if (mover->isSelected() && mover->getCommander()->getId() == Commander::home->getId())
				numMovers++;
		}
		for (i = 0; i < pTeam->getRosterSize(); i++)
		{
			Mover* mover = (Mover*)pTeam->getMover(i);
			if (mover->isSelected() && mover->getCommander()->getId() == Commander::home->getId())
			{
				((std::unique_ptr<Mover>)mover)
					->getPilot()
					->setUseGoalPlan(!((std::unique_ptr<Mover>)mover)->getPilot()->getUseGoalPlan());
				((std::unique_ptr<Mover>)mover)
					->getPilot()
					->setMainGoal(GOAL_ACTION_NONE, nullptr, nullptr, -1.0);
			}
		}
		lastTime = gos_GetElapsedTime();
	}
#endif
	return (1);
}

int32_t
MissionInterfaceManager::enemyGoalPlan()
{
#ifndef FINAL
	static double lastTime = 0.0;
	if ((lastTime + 0.5) < gos_GetElapsedTime())
	{
		EnemiesGoalPlan = !EnemiesGoalPlan;
		for (size_t i = 0; i < ObjectManager->getNumMovers(); i++)
		{
			Mover* mover = ObjectManager->getMover(i);
			if (mover->getTeam() != Team::home)
			{
				((std::unique_ptr<Mover>)mover)->getPilot()->setUseGoalPlan(EnemiesGoalPlan);
				((std::unique_ptr<Mover>)mover)
					->getPilot()
					->setMainGoal(GOAL_ACTION_NONE, nullptr, nullptr, -1.0);
			}
		}
		lastTime = gos_GetElapsedTime();
	}
#endif
	return (1);
}

int32_t
MissionInterfaceManager::showVictim()
{
#ifndef FINAL
	static double lastTime = 0.0;
	if ((lastTime + 0.5) < gos_GetElapsedTime())
	{
		if (DebugGameObject[0] && DebugGameObject[0]->isMover())
		{
			GameObjectPtr targetObj =
				((std::unique_ptr<Mover>)DebugGameObject[0])->getPilot()->getCurTacOrder()->getTarget();
			if (targetObj)
				DEBUGWINS_setGameObject(-1, targetObj);
		}
		lastTime = gos_GetElapsedTime();
	}
#endif
	return (1);
}

void damageObject(GameObjectPtr victim, float damage)
{
	WeaponShotInfo shot;
	switch (victim->getObjectClass())
	{
	case BATTLEMECH:
	case GROUNDVEHICLE:
		shot.init(nullptr, -2, damage,
			victim->calcHitLocation(nullptr, -1, ATTACKSOURCE_WEAPONFIRE, 0), 0);
		victim->handleWeaponHit(&shot, (MPlayer != nullptr));
		break;
	default:
		shot.init(nullptr, -1, damage, 0, 0);
		victim->handleWeaponHit(&shot, (MPlayer != nullptr));
		break;
	}
}

int32_t
MissionInterfaceManager::damageObject1()
{
#ifndef FINAL
	static double lastTime = 0.0;
	if ((lastTime + 0.5) < gos_GetElapsedTime())
	{
		if (target)
			target->repairAll();
		lastTime = gos_GetElapsedTime();
	}
#endif
	return (1);
}

int32_t
MissionInterfaceManager::damageObject2()
{
#ifndef FINAL
	static double lastTime = 0.0;
	if ((lastTime + 0.5) < gos_GetElapsedTime())
	{
		if (target)
			damageObject(target, 4.0);
		lastTime = gos_GetElapsedTime();
	}
#endif
	return (1);
}

int32_t
MissionInterfaceManager::damageObject3()
{
#ifndef FINAL
	static double lastTime = 0.0;
	if ((lastTime + 0.5) < gos_GetElapsedTime())
	{
		if (target)
			damageObject(target, 9.0);
		lastTime = gos_GetElapsedTime();
	}
#endif
	return (1);
}

int32_t
MissionInterfaceManager::damageObject4()
{
#ifndef FINAL
	static double lastTime = 0.0;
	if ((lastTime + 0.5) < gos_GetElapsedTime())
	{
		if (target)
			damageObject(target, 16.0);
		lastTime = gos_GetElapsedTime();
	}
#endif
	return (1);
}

int32_t
MissionInterfaceManager::damageObject5()
{
#ifndef FINAL
	static double lastTime = 0.0;
	if ((lastTime + 0.5) < gos_GetElapsedTime())
	{
		if (target)
			damageObject(target, 25.0);
		lastTime = gos_GetElapsedTime();
	}
#endif
	return (1);
}

int32_t
MissionInterfaceManager::damageObject6()
{
#ifndef FINAL
	static double lastTime = 0.0;
	if ((lastTime + 0.5) < gos_GetElapsedTime())
	{
		if (target)
			damageObject(target, 36.0);
		lastTime = gos_GetElapsedTime();
	}
#endif
	return (1);
}

int32_t
MissionInterfaceManager::damageObject7()
{
#ifndef FINAL
	static double lastTime = 0.0;
	if ((lastTime + 0.5) < gos_GetElapsedTime())
	{
		if (target)
			damageObject(target, 49.0);
		lastTime = gos_GetElapsedTime();
	}
#endif
	return (1);
}

int32_t
MissionInterfaceManager::damageObject8()
{
#ifndef FINAL
	static double lastTime = 0.0;
	if ((lastTime + 0.5) < gos_GetElapsedTime())
	{
		if (target)
			damageObject(target, 64.0);
		lastTime = gos_GetElapsedTime();
	}
#endif
	return (1);
}

int32_t
MissionInterfaceManager::damageObject9()
{
#ifndef FINAL
	static double lastTime = 0.0;
	if ((lastTime + 0.5) < gos_GetElapsedTime())
	{
		if (target)
			damageObject(target, 81);
		lastTime = gos_GetElapsedTime();
	}
#endif
	return (1);
}

int32_t
MissionInterfaceManager::damageObject0()
{
#ifndef FINAL
	static double lastTime = 0.0;
	if ((lastTime + 0.5) < gos_GetElapsedTime())
	{
		if (target)
		{
			if (target->isMover())
				((std::unique_ptr<Mover>)target)->disable(0);
			else
				damageObject(target, 100.0);
		}
		lastTime = gos_GetElapsedTime();
	}
#endif
	return (1);
}

int32_t
MissionInterfaceManager::toggleCompass()
{
	((GameCamera*)eye)->toggleCompass();
	return 0;
}

bool MissionInterfaceManager::selectionIsHelicopters()
{
	Team* pTeam = Team::home;
	for (size_t i = 0; i < pTeam->getRosterSize(); i++)
	{
		Mover* pMover = (Mover*)pTeam->getMover(i);
		if (pMover->isSelected() && pMover->getCommander()->getId() == Commander::home->getId())
		{
			if (pMover->getMoveType() != MOVETYPE_AIR)
			{
				return 0;
			}
		}
	}
	return true;
}

int32_t
MissionInterfaceManager::saveHotKeys(FitIniFile& file)
{
	file.writeBlock("Keyboard");
	for (size_t i = 0; i < MAX_COMMAND; i++)
	{
		wchar_t header[256];
		sprintf(header, "Key%ld", i);
		file.writeIdLong(header, commands[i].key);
	}
	file.writeIdLong("WayPointKey", WAYPOINT_KEY);
	file.writeIdBoolean("UseLeftRightMouseProfile", useLeftRightMouseProfile);
	return 0;
}
int32_t
MissionInterfaceManager::loadHotKeys(FitIniFile& file)
{
	if (OldKeys[0] == -1)
	{
		for (size_t i = 0; i < MAX_COMMAND; i++)
		{
			OldKeys[i] = commands[i].key;
		}
	}
	if (NO_ERROR == file.seekBlock("Keyboard"))
	{
		for (size_t i = 0; i < MAX_COMMAND; i++)
		{
			wchar_t header[256];
			sprintf(header, "Key%ld", i);
			file.readIdLong(header, commands[i].key);
		}
		int32_t tmp;
		file.readIdLong("WayPointKey", tmp);
		WAYPOINT_KEY = gosEnum_KeyIndex(tmp);
		file.readIdBoolean("UseLeftRightMouseProfile", useLeftRightMouseProfile);
		return 0;
	}
	return -1;
}

int32_t
MissionInterfaceManager::setHotKey(
	int32_t whichCommand, gosEnum_KeyIndex newKey, bool bShift, bool bControl, bool bAlt)
{
	gosASSERT(whichCommand < MAX_COMMAND);
	int32_t oldKey = commands[whichCommand].key;
	int32_t key = newKey;
	if (bShift)
		key |= SHIFT;
	if (bControl)
		key |= CTRL;
	if (bAlt)
		key |= ALT;
	if (commands[whichCommand].key & WAYPT)
		key |= WAYPT;
	else
	{
		// change corresponding waypoint keys
		for (size_t i = 0; i < MAX_COMMAND; i++)
		{
			if (((commands[i].key & 0x0000ffff) == oldKey) && (commands[i].key & WAYPT))
			{
				commands[i].key = key | WAYPT;
			}
		}
	}
	commands[whichCommand].key = key;
	return 0;
}

int32_t
MissionInterfaceManager::getHotKey(
	int32_t whichCommand, gosEnum_KeyIndex& newKey, bool& bShift, bool& bControl, bool& bAlt)
{
	gosASSERT(whichCommand < MAX_COMMAND);
	int32_t key = commands[whichCommand].key;
	if (key & SHIFT)
		bShift = true;
	if (key & CTRL)
		bControl = true;
	if (key & ALT)
		bAlt = true;
	key &= 0x0000ffff;
	newKey = (gosEnum_KeyIndex)key;
	return 0;
}

int32_t
MissionInterfaceManager::setWayPointKey(gosEnum_KeyIndex key)
{
	WAYPOINT_KEY = key;
	return 0;
}

void MissionInterfaceManager::setAOEStyle()
{
	useLeftRightMouseProfile = true;
}
void MissionInterfaceManager::setMCStyle()
{
	useLeftRightMouseProfile = false;
}

bool MissionInterfaceManager::isAOEStyle()
{
	return useLeftRightMouseProfile == true;
}
bool MissionInterfaceManager::isMCStyle()
{
	return useLeftRightMouseProfile == false;
}

void MissionInterfaceManager::doEject(GameObject* who)
{
	TacticalOrder tacOrder;
	tacOrder.init(OrderOriginType::player, TacticalOrderCode::eject, true);
	tacOrder.pack(nullptr, nullptr);
	std::unique_ptr<Mover> pMover = (std::unique_ptr<Mover>)who;
	if (MPlayer && !MPlayer->isServer())
		MPlayer->sendPlayerOrder(&tacOrder, false, 1, &pMover);
	else
		pMover->handleTacticalOrder(tacOrder);
}

bool MissionInterfaceManager::hotKeyIsPressed(int32_t whichCommand)
{
	int32_t key = commands[whichCommand].key;
	bool bShift = 0;
	bool bControl = 0;
	bool bAlt = 0;
	if (key & SHIFT)
		bShift = true;
	if (key & CTRL)
		bControl = true;
	if (key & ALT)
		bAlt = true;
	key &= 0x0000ffff;
	bool shiftDn = userInput->shift();
	bool ctrlDn = userInput->ctrl();
	bool altDn = userInput->alt();
	if (gos_GetKeyStatus((gosEnum_KeyIndex)(key & 0x000000ff)) == KEY_PRESSED || gos_GetKeyStatus((gosEnum_KeyIndex)(key & 0x000000ff)) == KEY_HELD)
	{
		if (shiftDn == bShift && ctrlDn == bControl && altDn == bAlt)
		{
			return true;
		}
	}
	return false;
}

void testKeyStuff()
{
	bool bShift, bCtrl, bAlt;
	gosEnum_KeyIndex key = KEY_F9;
	MissionInterfaceManager::instance()->getHotKey(5, key, bShift, bCtrl, bAlt);
	MissionInterfaceManager::instance()->setHotKey(5, KEY_L, 0, 0, 0);
	MissionInterfaceManager::instance()->getHotKey(6, key, bShift, bCtrl, bAlt);
	FitIniFile file;
	file.open("keyboards.fit");
	MissionInterfaceManager::instance()->loadHotKeys(file);
	MissionInterfaceManager::instance()->getHotKey(6, key, bShift, bCtrl, bAlt);
}

int32_t
MissionInterfaceManager::toggleHotKeys()
{
	bDrawHotKeys ^= 1;
	if (bDrawHotKeys)
	{
		keyboardRef->reseed(commands);
		if (!isPaused())
			togglePauseWithoutMenu();
	}
	else if (!bDrawHotKeys && isPaused() && isPausedWithoutMenu())
		togglePauseWithoutMenu();
	return 1;
}

void MissionInterfaceManager::drawHotKeys()
{
	keyboardRef->render();
	return;
}

void MissionInterfaceManager::drawHotKey(std::wstring_view keyString, std::wstring_view descString, int32_t x, int32_t y)
{
	hotKeyFont.render(keyString, 0, y, x, Environment.screenheight, 0xffffffff, 0, 1);
	hotKeyFont.render(descString, x + (10 * Environment.screenwidth / 640.f), y,
		Environment.screenwidth, Environment.screenheight, 0xffffffff, 0, 0);
}

void MissionInterfaceManager::doGuardTower()
{
	for (size_t i = 0; i < Team::home->getRosterSize(); i++)
	{
		Mover* pMover = Team::home->getMover(i);
		if (pMover->isSelected() && pMover->getCommander()->getId() == Commander::home->getId())
		{
			pMover->suppressionFire = true;
		}
	}
	TacticalOrder tacOrder;
	tacOrder.init(OrderOriginType::player, TacticalOrderCode::attackpoint);
	// tacOrder.init(OrderOriginType::player,  target ? TacticalOrderCode::attackobject
	// : TacticalOrderCode::attackpoint);  tacOrder.targetWID = target ?
	// target->getWatchID() : 0;  if ( !target )
	tacOrder.attackParams.targetpoint = wPos;
	tacOrder.attackParams.type = bEnergyWeapons ? AttackType::conservingammo : AttackType::destroy;
	tacOrder.attackParams.method = AttackMethod::ranged;
	tacOrder.attackParams.range = FireRangeType::current;
	tacOrder.attackParams.pursue = false;
	tacOrder.moveparams.faceObject = true;
	tacOrder.moveparams.fromArea = -1;
	tacOrder.moveparams.wait = false;
	handleOrders(tacOrder);
	controlGui.setDefaultSpeed();
}

int32_t
MissionInterfaceManager::toggleHoldPosition()
{
	controlGui.toggleHoldPosition();
	return 1;
}

int32_t
MissionInterfaceManager::handleChatKey()
{
	controlGui.toggleChat(0);
	return 1;
}

int32_t
MissionInterfaceManager::handleTeamChatKey()
{
	controlGui.toggleChat(1);
	return 1;
}

float slopeTest[8] = {0.09849140f, 0.30334668f, 0.53451114f, 0.82067879f, 1.21850353f, 1.87086841f,
	3.29655821f, 10.15317039f};
int32_t
MissionInterfaceManager::makeMoveCursor(bool bLineOfSite)
{
	int32_t rotation = calcRotation();
	// leigh put these in the file funny
	// OK, rotations go clockwise starting from 180...
	gosASSERT(rotation >= 0 && rotation < 32);
	return mState_WALK1 + 32 * bLineOfSite + rotation;
}

int32_t
MissionInterfaceManager::makeJumpCursor(bool bLineOfSite)
{
	int32_t rotation = calcRotation();
	// leigh put these in the file funny
	// OK, rotations go clockwise starting from 180...
	gosASSERT(rotation >= 0 && rotation < 32);
	return mState_JUMP1 + 32 * bLineOfSite + rotation;
}

int32_t
MissionInterfaceManager::makeRunCursor(bool bLineOfSite)
{
	int32_t rotation = calcRotation();
	// leigh put these in the file funny
	// OK, rotations go clockwise starting from 180...
	gosASSERT(rotation >= 0 && rotation < 32);
	return mState_RUN1 + 32 * bLineOfSite + rotation;
}

int32_t
MissionInterfaceManager::calcRotation()
{
	Stuff::Vector3D pos;
	pos.x = 0.f;
	pos.y = 0.f;
	pos.z = 0.f;
	float count = 0;
	for (size_t i = 0; i < Team::home->getRosterSize(); i++)
	{
		Mover* pMover = Team::home->getMover(i);
		if (pMover->isSelected() && pMover->getCommander()->getId() == Commander::home->getId())
		{
			pos.x += pMover->getPosition().x;
			pos.y += pMover->getPosition().y;
			pos.z += pMover->getPosition().z;
			count += 1.f;
		}
	}
	if (count && (turn > 3))
	{
		pos.x /= count;
		pos.y /= count;
		pos.z /= count;
	}
	else
		return mState_NORMAL;
	// Now works by finding the vector in world space between the mouse cursor
	// and the movers.  It then normalizes that vector and multiplies by 200
	// to make large.  It then adds that vector to the camera position and we
	// do the projections based on the camera center (which is always the
	// center of the screen) and the new vector.  Complex but it works!!
	// -fs
	Stuff::Vector3D actualPos;
	actualPos.Subtract(pos, wPos);
	actualPos.z = 0.0f;
	float actualLength = actualPos.GetLength();
	if (actualLength > Stuff::SMALL)
		actualPos /= actualLength;
	actualPos *= 200.0f;
	Stuff::Vector3D camPos = eye->getPosition();
	Stuff::Vector4D screenPosMover;
	Stuff::Vector4D screenPosGoal;
	actualPos.Add(camPos, actualPos);
	eye->projectZ(actualPos, screenPosMover);
	// need to find second position
	eye->projectZ(camPos, screenPosGoal);
	// CRAZY ATAN code.  Out for now.
	// -fs
	/*
	// determine angle between the two
	float deltaY = -(screenPosGoal.y - screenPosMover.y);
	float deltaX = screenPosGoal.x - screenPosMover.x;

	float theta = 0;
	if ( deltaY && deltaX )
	{
		theta = atan( deltaY/deltaX );
		theta *= RADS_TO_DEGREES;
	}

	if ( deltaX < 0 )
		theta += 180.f;

	int32_t rotation = ((theta/360.f * 32.f));
	*/
	// Do it the MC1 way!
	float slope = 0.0f;
	if (fabs(screenPosGoal.x - screenPosMover.x) > Stuff::SMALL)
		slope = fabs(screenPosGoal.y - screenPosMover.y) / fabs(screenPosGoal.x - screenPosMover.x);
	int32_t counter = 0;
	int32_t rotation = 0;
	while (slope > slopeTest[counter] && counter < 8)
		counter++;
	if (screenPosGoal.y < screenPosMover.y)
	{
		if (screenPosGoal.x < screenPosMover.x) // quadrant 0
			rotation = counter;
		else // quadrant 1
			rotation = 8 + (8 - counter);
	}
	else
	{
		if (screenPosGoal.x > screenPosMover.x) // quadrant 2
		{
			rotation = 16 + counter;
		}
		else // quadrant 3
		{
			rotation = 24 + (8 - counter);
			if (rotation == 32)
				rotation = 0;
		}
	}
	return rotation;
}

//--------------------------------------------------------------------------------------
void MissionInterfaceManager::Save(FitIniFilePtr file)
{
	// If VTOL or Karnov are active AND HAVEN'T FINISHED THEIR OPERATION, save
	// them, what vehicle they are going to drop or what mech they were fixing.
	int32_t vtolNum = Commander::home->getId();
	if (vTol[vtolNum] && paintingVtol[vtolNum] && (!vehicleDropped[vtolNum] && !mechRecovered[vtolNum]))
	{
		// Save 'em
		file->writeBlock("VTOLData");
		file->writeIdFloat("VtolPosX", vPos[vtolNum].x);
		file->writeIdFloat("VtolPosY", vPos[vtolNum].y);
		file->writeIdFloat("VtolPosZ", vPos[vtolNum].z);
		file->writeIdLong("VehicleID", vehicleID[vtolNum]);
		if (mechToRecover[vtolNum])
			file->writeIdLong("SalvageMechWID", mechToRecover[vtolNum]->getWatchID());
		else
			file->writeIdLong("SalvageMechWID", 0);
		file->writeIdLong("VTOLFrame", vTol[vtolNum]->currentFrame);
		file->writeIdLong("VTOLGesture", vTol[vtolNum]->getCurrentGestureId());
	}
}

//--------------------------------------------------------------------------------------
void MissionInterfaceManager::Load(FitIniFilePtr file)
{
	// Is there any VTOL data here?  OK if not!
	int32_t vtolNum = Commander::home->getId();
	int32_t commanderid = vtolNum;
	if (file->seekBlock("VTOLData") == NO_ERROR)
	{
		// Load 'em
		file->readIdFloat("VtolPosX", vPos[vtolNum].x);
		file->readIdFloat("VtolPosY", vPos[vtolNum].y);
		file->readIdFloat("VtolPosZ", vPos[vtolNum].z);
		file->readIdLong("VehicleID", vehicleID[vtolNum]);
		if (vehicleID[vtolNum] != 147) // NOT a recovery vehicle.  Standard VTOL
		{
			// Go ahead and make a VTOL.  We're gonna need it!
			AppearanceType* appearanceType =
				appearanceTypeList->getAppearance(BLDG_TYPE << 24, "vtol");
			vTol[commanderid] = new BldgAppearance;
			vTol[commanderid]->init(appearanceType);
		}
		else
		{
			AppearanceType* appearanceType =
				appearanceTypeList->getAppearance(BLDG_TYPE << 24, "karnov");
			vTol[commanderid] = new BldgAppearance;
			vTol[commanderid]->init(appearanceType);
		}
		int32_t mechWID = 0;
		file->readIdLong("SalvageMechWID", mechWID);
		mechToRecover[vtolNum] = (std::unique_ptr<Mover>)ObjectManager->getByWatchID(mechWID);
		file->readIdFloat("VTOLFrame", vTol[vtolNum]->currentFrame);
		int32_t gestureId = 0;
		file->readIdLong("VTOLGesture", gestureId);
		vTol[vtolNum]->setGesture(gestureId);
		// Now setup everything else.
		// Pretty much same code as beginVtol
		vehicleFile = controlGui.getVehicleNameFromID(vehicleID[vtolNum]);
		//		if ( !vTol[vtolNum] )
		{
			if (vehicleID[vtolNum] != 147) // NOT a recovery vehicle.  Standard VTOL
			{
				if (!dustCloud[commanderid] && prefs.useNonWeaponEffects)
				{
					if (strcmp(weaponEffects->GetEffectName(VTOL_DUST_CLOUD), "NONE") != 0)
					{
						//--------------------------------------------
						// Yes, load it on up.
						unsigned flags = 0;
						Check_Object(gosFX::EffectLibrary::Instance);
						gosFX::Effect::Specification* gosEffectSpec =
							gosFX::EffectLibrary::Instance->Find(
								weaponEffects->GetEffectName(VTOL_DUST_CLOUD));
						if (gosEffectSpec)
						{
							dustCloud[commanderid] = gosFX::EffectLibrary::Instance->MakeEffect(
								gosEffectSpec->m_effectID, flags);
							gosASSERT(dustCloud[commanderid] != nullptr);
							MidLevelRenderer::MLRTexturePool::Instance->LoadImages();
						}
					}
				}
			}
			else // IS a recovery vehicle.  DO NOT create VTOL, use KARNOV
				// instead.  NO Vehicle created either
			{
				mechRecovered[commanderid] = false; // Need to know when mech is
					// done so Karnov can fly
					// away.
				if (!dustCloud[commanderid] && prefs.useNonWeaponEffects)
				{
					if (strcmp(weaponEffects->GetEffectName(KARNOV_DUST_CLOUD), "NONE") != 0)
					{
						//--------------------------------------------
						// Yes, load it on up.
						unsigned flags = 0;
						Check_Object(gosFX::EffectLibrary::Instance);
						gosFX::Effect::Specification* gosEffectSpec =
							gosFX::EffectLibrary::Instance->Find(
								weaponEffects->GetEffectName(KARNOV_DUST_CLOUD));
						if (gosEffectSpec)
						{
							dustCloud[commanderid] = gosFX::EffectLibrary::Instance->MakeEffect(
								gosEffectSpec->m_effectID, flags);
							gosASSERT(dustCloud[commanderid] != nullptr);
							MidLevelRenderer::MLRTexturePool::Instance->LoadImages();
						}
					}
				}
				if (!recoveryBeam[commanderid])
				{
					if (strcmp(weaponEffects->GetEffectName(KARNOV_RECOVERY_BEAM), "NONE") != 0)
					{
						//--------------------------------------------
						// Yes, load it on up.
						unsigned flags = 0;
						Check_Object(gosFX::EffectLibrary::Instance);
						gosFX::Effect::Specification* gosEffectSpec =
							gosFX::EffectLibrary::Instance->Find(
								weaponEffects->GetEffectName(KARNOV_RECOVERY_BEAM));
						if (gosEffectSpec)
						{
							recoveryBeam[commanderid] = gosFX::EffectLibrary::Instance->MakeEffect(
								gosEffectSpec->m_effectID, flags);
							gosASSERT(recoveryBeam[commanderid] != nullptr);
							MidLevelRenderer::MLRTexturePool::Instance->LoadImages();
						}
					}
				}
			}
		}
		// Should we play another sample to let them know what is coming in
		// after the load?  For now, no!
		/*
		switch (vehicleID)
		{
			case 120:	//Minelayer
				soundSystem->playSupportSample(SUPPORT_MINELAYER);
			break;

			case 182:	//Repair
				soundSystem->playSupportSample(SUPPORT_REPAIR);
			break;

			case 221:	//Scout
				soundSystem->playSupportSample(SUPPORT_SCOUT);
			break;

			case 147:	//Recovery
				soundSystem->playSupportSample(SUPPORT_RECOVER);
			break;

			case 415:	//Artillery
				soundSystem->playSupportSample(SUPPORT_RECOVER);
			break;
		}
		*/
		if (vehicleID[commanderid] != 147)
			soundSystem->playDigitalSample(VTOL_ANIMATE, vPos[commanderid]);
		else
			soundSystem->playDigitalSample(SALVAGE_CRAFT, vPos[commanderid]);
		Stuff::Vector3D newPos = vPos[commanderid];
		float rotation = 0.f; // this needs to be pointing 180 degrees from drop point
		vTol[commanderid]->setObjectParameters(newPos, 0, false, Team::home->id, 0);
		// eye->update();
		vTol[commanderid]->update();
		vTol[commanderid]->setInView(true);
		vTol[commanderid]->setVisibility(1, 1);
		vTol[commanderid]->rotation = rotation;
		if (dustCloud[commanderid])
		{
			Stuff::LinearMatrix4D shapeOrigin;
			Stuff::LinearMatrix4D localToWorld;
			Stuff::LinearMatrix4D localResult;
			Stuff::Vector3D dustPos = vTol[commanderid]->position;
			Stuff::Point3D wakePos;
			wakePos.x = -dustPos.x;
			wakePos.y = dustPos.z;
			wakePos.z = dustPos.y;
			shapeOrigin.BuildRotation(Stuff::EulerAngles(0.0f, 0.0f, 0.0f));
			shapeOrigin.BuildTranslation(wakePos);
			gosFX::Effect::ExecuteInfo info((Stuff::Time)scenarioTime, &shapeOrigin, nullptr);
			dustCloud[commanderid]->SetLoopOff();
			dustCloud[commanderid]->SetExecuteOn();
			dustCloud[commanderid]->Start(&info);
		}
		paintingVtol[commanderid] = 1;
		vTolSpeed = -75.f;
		vehicleDropped[commanderid] = false;
		mechRecovered[commanderid] = false;
		controlGui.disableAllVehicleButtons();
	}
}

void MissionInterfaceManager::updateRollovers()
{
	/*	For all (both LOS and non-LOS) the run cursors, use strings 45149/45150
		For all the jump cursors, use strings 45151/45152
		For all the walk waypoint cursors, use strings 45153/45154
		For all the run waypoint cursors, use strings 45155/45156
		For all the jump waypoint cursors, use strings 45157/45158
		For all the patrol path cursors, use strings 45159/45160
		For all the standard attack cursors, use strings 45161/45162
		For all the ammo conservation cursors, use strings 45163/45164
		For all the fire from current position cursors, use strings 45165/45166
		For all the int16_t-range attack cursors, use strings 45167/45168
		For all the medium-range attack cursors, use strings 45169/45170
		For all the int32_t-range attack cursors, use strings 45171/45172
		For all the force-fire attack cursors, use strings 45173/45174
		For the guard cursor, use strings 45147/45148 (one each for right-click
	   and left-click models).*/
	int32_t cursor = userInput->getMouseCursor();
	if ((cursor >= mState_RUN1 && cursor <= mState_RUN1 + 64))
	{
		helpTextID = 0;
		controlGui.setRolloverHelpText(IDS_RUN_CURSOR_LEFT_HELP);
	}
	else if ((cursor >= mState_JUMP1 && cursor <= mState_JUMP1 + 64))
	{
		helpTextID = 0;
		controlGui.setRolloverHelpText(IDS_JUMP_CURSOR_LEFT_HELP);
	}
	else
	{
		switch (cursor)
		{
		case mState_GUARD:
			if (target)
			{
				helpTextID = 0;
				controlGui.setRolloverHelpText(IDS_GUARD_RECIPIENT_LEFT_HELP);
			}
			break;
		case mState_JUMPWAYPT:
		case mState_JUMPWAYPT_LOS:
			helpTextID = 0;
			controlGui.setRolloverHelpText(IDS_JUMP_WAYPOINT_CURSOR_LEFT_HELP);
			break;
		case mState_RUNWAYPT:
		case mState_RUNWAYPT_LOS:
			helpTextID = 0;
			controlGui.setRolloverHelpText(IDS_RUN_WAYPOINT_CURSOR_LEFT_HELP);
			break;
		case mState_WALKWAYPT:
		case mState_WALKWAYPT_LOS:
			helpTextID = 0;
			controlGui.setRolloverHelpText(IDS_WALK_WAYPOINT_CURSOR_LEFT_HELP);
			break;
		case mState_LINK:
			helpTextID = 0;
			controlGui.setRolloverHelpText(IDS_PATROL_CURSOR_LEFT_HELP);
			break;
		case mState_GENERIC_ATTACK:
		case mState_ATTACK_LOS:
			helpTextID = 0;
			controlGui.setRolloverHelpText(IDS_ATTACK_CURSOR_LEFT_HELP);
			break;
		case mState_ENERGY_WEAPONS:
		case mState_ENERGY_WEAPONS_LOS:
			helpTextID = 0;
			controlGui.setRolloverHelpText(IDS_AMMO_CONSERVE_CURSOR_LEFT_HELP);
			break;
		case mState_CURPOS_ATTACK:
		case mState_CURPOS_ATTACK_LOS:
			helpTextID = 0;
			if (gos_GetKeyStatus((gosEnum_KeyIndex)commands[FORCE_FIRE_KEY].key) == KEY_HELD)
				controlGui.setRolloverHelpText(IDS_FORCE_FIRE_CURSOR_LEFT_HELP);
			else
				controlGui.setRolloverHelpText(IDS_FIRE_FROM_CURRENT_POSITION_CURSOR_LEFT_HELP);
			break;
		case mState_SHRTRNG_ATTACK:
		case mState_SHRTRNG_LOS:
			helpTextID = 0;
			controlGui.setRolloverHelpText(IDS_SHORT_RANGE_CURSOR_LEFT_HELP);
			break;
		case mState_MEDRNG_ATTACK:
		case mState_MEDRNG_LOS:
			helpTextID = 0;
			controlGui.setRolloverHelpText(IDS_MEDIUM_RANGE_CURSOR_LEFT_HELP);
			break;
		case mState_LONGRNG_ATTACK:
		case mState_LONGRNG_LOS:
			helpTextID = 0;
			controlGui.setRolloverHelpText(IDS_LONG_RANGE_CURSOR_LEFT_HELP);
			break;
		}
	}
}

//--------------------------------------------------------------------------------------
