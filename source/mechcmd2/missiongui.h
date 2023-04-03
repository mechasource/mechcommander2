//--------------------------------------------------------------------------------------
//
// MechCommander 2 -- Copyright (c) 1998 FASA Interactive
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

#pragma once

#ifndef MISSIONGUI_H
#define MISSIONGUI_H

#include "mclib.h"
#include "mover.h"
#include "controlgui.h"

class KeyboardRef;
enum COMMAND_KEY_INDEX
{
	ENERGY_WEAPON_INDEX = 3,
	GUARD_COMMAND_INDEX = 8,
	FORCE_FIRE_KEY = 11,
	EJECT_COMMAND_INDEX = 27,
	OBJECTVIES_COMMAND_KEY = 59,
	INFO_COMMAND_INDEX = 64

};
//--------------------------------------------------------------------------------------
// Externs
extern UserHeapPtr guiHeap;
PVOID
gMalloc(size_t size);
void gFree(PVOID me);

#define MAX_COMMAND 107
//--------------------------------------------------------------------------------------
class MissionInterfaceManager
{
	//-------------------------------------------------------------------
	// This baby runs it all for the Missions
	//
	// It has an update which is called every frame.
	//
	// It has an init which takes a FIT file and creates the interface
	// from the .FIT file.  This allows artists to correctly setup
	// the interface for each screen using data files.
	//
	// Its also has a render which is called AFTER the camera render
	// so that the interface ALWAYS sits on top of the world.
	//
	// It has an event constructor to create events and pass them down
	// the interface chain.
	//
	// Essentially, this is the iface of MechCommander 2

public:
	typedef int32_t (MissionInterfaceManager::*COMMAND)(void);

	struct Command
	{
		int32_t key;
		int32_t cursor;
		int32_t cursorLOS;
		bool singleClick;
		COMMAND function;
		COMMAND releaseFunction;
		int32_t hotKeyDescriptionText;
	};

	static Command* getCommands()
	{
		return commands;
	}
	static int32_t* getOldKeys()
	{
		return OldKeys;
	}

protected:
	static int32_t mouseX;
	static int32_t mouseY;
	static gosEnum_KeyIndex WAYPOINT_KEY;

	static float pauseWndVelocity;

	static Command commands[MAX_COMMAND];
	static int32_t OldKeys[MAX_COMMAND];

	int32_t terrainLineChanged;

	//-------------------------------------------
	// Data to control scroll, rotation and zoom
	float baseFrameLength;

	float zoomInc;
	float rotationInc;
	float scrollInc;

	float screenScrollLeft;
	float screenScrollRight;
	float screenScrollUp;
	float screenScrollDown;

	float realRotation;
	float degPerSecRot;

	int32_t zoomChoice; // Can range from 0 to 3 right now.

	//----------------------------------------------
	// Mouse Event Data
	bool isDragging;
	float vTolSpeed;
	std::wstring_view vehicleFile;
	int32_t vehicleID[MAX_TEAMS];
	Stuff::Vector3D dragStart;
	Stuff::Vector3D dragEnd;
	bool selectClicked; // left click
	bool commandClicked; // usually right clicked
	bool cameraClicked;

	ControlGui controlGui;
	Stuff::Vector3D wPos; // World Position of camera cursor.

	// vTol
	Stuff::Vector3D vPos[MAX_TEAMS]; // vehicle position
	BldgAppearance* vTol[MAX_TEAMS];
	bool paintingVtol[MAX_TEAMS];
	float vTolTime[MAX_TEAMS];
	gosFX::Effect* recoveryBeam[MAX_TEAMS];
	gosFX::Effect* dustCloud[MAX_TEAMS];
	bool mechRecovered[MAX_TEAMS];
	bool vehicleDropped[MAX_TEAMS];
	std::unique_ptr<Mover> mechToRecover[MAX_TEAMS];

	bool bPaused;
	bool bPausedWithoutMenu;
	bool bEnergyWeapons;
	bool bDrawHotKeys;
	int32_t resolution;

	//-----------------------------------------------
	// Attila (Strategic Commander) Data
	float attilaXAxis;
	float attilaYAxis;
	float attilaRZAxis;

	//-----------------------------------------------
	// Tutorial Data
	bool animationRunning;
	float timeLeftToScroll;
	int32_t targetButtonId;
	int32_t buttonNumFlashes;
	float buttonFlashTime;
	bool targetIsButton;
	bool targetIsPressed;
	bool guiFrozen;

	bool bForcedShot;
	bool bAimedShot;

	static MissionInterfaceManager* s_instance;

public:
	MissionInterfaceManager(void)
	{
		init(void);
	}

	~MissionInterfaceManager(void)
	{
		destroy(void);
	}

	static MissionInterfaceManager* instance()
	{
		return s_instance;
	}

	void init(void);

	void init(FitIniFilePtr loader);
	void initTacMap(PacketFile* file, int32_t packet);

	virtual void destroy(void);

	virtual void update(void);
	virtual void drawVTOL(void);
	virtual void render(void);

	void initMechs()
	{
		controlGui.initMechs(void);
	}

	std::wstring_view getSupportVehicleNameFromID(int32_t ID)
	{
		return controlGui.getVehicleNameFromID(ID);
	}

	void addMover(std::unique_ptr<Mover> mover)
	{
		controlGui.addMover(mover);
	}
	void removeMover(std::unique_ptr<Mover> mover)
	{
		controlGui.removeMover(mover);
	}

	int32_t update(bool bLeftClick, bool bRightClick, int32_t mouseX, int32_t mouseY,
		GameObject* pTarget,
		bool bLOS); // check for each key in the list

	void updateVTol(void);

	ControlGui* getControlGui(void)
	{
		return &controlGui;
	}

	void playMovie(std::wstring_view filename)
	{
		controlGui.playMovie(filename);
	}

	int32_t handleOrders(TacticalOrder& order);
	bool anySelectedWithoutAreaEffect(void);
	void updateWaypoints(void);

	void setTarget(GameObject* pTarget)
	{
		target = pTarget;
	}
	static void selectForceGroup(int32_t forceGroup, bool deselectOthers);
	void doRepair(GameObject* who);
	void doRepairBay(GameObject* who);
	void doEject(GameObject* who);
	bool canRepair(GameObject* pMover);
	bool canRepairBay(GameObject* bay);

	void setMechRecovered(int32_t teamID, bool set)
	{
		mechRecovered[teamID] = set;
	}

	void updateOldStyle(bool shiftDn, bool altDn, bool ctrlDn, bool bGui, bool lineOfSight,
		bool passable, int32_t moverCount, int32_t nonMoverCount);
	void updateAOEStyle(bool shiftDn, bool altDn, bool ctrlDn, bool bGui, bool lineOfSight,
		bool passable, int32_t moverCount, int32_t nonMoverCount);

	static int32_t saveHotKeys(FitIniFile& file);
	static int32_t loadHotKeys(FitIniFile& file);

	static int32_t setHotKey(
		int32_t whichCommand, gosEnum_KeyIndex key, bool bShift, bool bControl, bool bAlt);
	int32_t getHotKey(
		int32_t whichCommand, gosEnum_KeyIndex& newKey, bool& bShift, bool& bControl, bool& bAlt);
	bool hotKeyIsPressed(int32_t whichCommand);

	int32_t setWayPointKey(gosEnum_KeyIndex key);

	void setAOEStyle(void);
	void setMCStyle(void);

	bool isAOEStyle(void);
	bool isMCStyle(void);

	bool isPaused(void);
	bool isPausedWithoutMenu(void);
	int32_t togglePause(void);
	int32_t togglePauseWithoutMenu(void);
	int32_t toggleHoldPosition(void);
	int32_t showObjectives(void);
	int32_t showObjectives(bool on);
	void doMove(const Stuff::Vector3D& pos);
	void doGuard(GameObject* pObj);
	int32_t toggleHotKeys(void);
	void beginVtol(int32_t supportID, int32_t commanderid = 0,
		Stuff::Vector3D* reinforcePos = nullptr, std::unique_ptr<Mover> salvageTarget = nullptr);

	// Tutorial Stuff goes here.
	bool startAnimation(
		int32_t buttonId, bool isButton, bool isPressed, float timeToScroll, int32_t numFlashes);
	void setTutorialText(std::wstring_view text);

	bool isInCalloutAnimation()
	{
		return animationRunning;
	}

	void freezeGUI(bool flag)
	{
		guiFrozen = flag;
	}

	// In-Mission Save
	void Save(FitIniFilePtr file);
	void Load(FitIniFilePtr file);

private:
	void makeForceGroup(int32_t forceGroup);
	bool moveCameraAround(bool lineOfSight, bool passable, bool ctrl, bool bGui, int32_t moverCount,
		int32_t nonMoverCount);
	bool canJump(void); // selected units can jump
	bool canJumpToWPos(void);
	void doDrag(bool bGui);

	int32_t attackShort(void);
	int32_t attackMedium(void);
	int32_t attackLong(void);
	int32_t alphaStrike(void);
	int32_t defaultAttack(void);
	int32_t jump(void);
	int32_t stopJump(void);
	int32_t fireFromCurrentPos(void);
	int32_t stopFireFromCurrentPos(void);
	int32_t guard(void);
	int32_t stopGuard(void);
	int32_t conserveAmmo(void);
	int32_t selectVisible(void);
	int32_t addVisibleToSelection(void);
	int32_t aimLeg(void);
	int32_t aimArm(void);
	int32_t aimHead(void);
	int32_t removeCommand(void);
	int32_t powerUp(void);
	int32_t powerDown(void);
	int32_t changeSpeed(void);
	int32_t stopChangeSpeed(void);
	int32_t eject(void);
	int32_t forceShot(void);

	int32_t scrollUp(void);
	int32_t scrollDown(void);
	int32_t scrollLeft(void);
	int32_t scrollRight(void);
	int32_t zoomOut(void);
	int32_t zoomIn(void);
	int32_t zoomChoiceOut(void);
	int32_t zoomChoiceIn(void);
	int32_t rotateLeft(void);
	int32_t rotateRight(void);
	int32_t tiltUp(void);
	int32_t tiltDown(void);
	int32_t centerCamera(void);
	int32_t rotateLightLeft(void);
	int32_t rotateLightRight(void);
	int32_t rotateLightUp(void);
	int32_t rotateLightDown(void);
	int32_t switchTab(void);
	int32_t reverseSwitchTab(void);

	int32_t drawTerrain(void);
	int32_t drawOverlays(void);
	int32_t drawBuildings(void);
	int32_t showGrid(void);
	int32_t recalcLights(void);
	int32_t drawClouds(void);
	int32_t drawFog(void);
	int32_t usePerspective(void);
	int32_t drawWaterEffects(void);
	int32_t recalcWater(void);
	int32_t drawShadows(void);
	int32_t changeLighting(void);
	int32_t vehicleCommand(void);
	int32_t toggleGUI(void);
	int32_t drawTGLShapes(void);
	int32_t infoCommand(void);
	int32_t infoButtonReleased(void);
	int32_t energyWeapons(void);
	int32_t sendAirstrike(void);
	int32_t sendLargeAirstrike(void);
	int32_t gotoNextNavMarker(void);
	int32_t sendSensorStrike(void);

	int32_t handleChatKey(void);
	int32_t handleTeamChatKey(void);

	bool makePatrolPath(void);

	int32_t bigAirStrike(void);
	int32_t smlAirStrike(void);
	int32_t snsAirStrike(void);

	int32_t cameraNormal(void);
	int32_t cameraDefault(void);
	int32_t cameraMaxIn(void);
	int32_t cameraTight(void);
	int32_t cameraFour(void);
	int32_t toggleCompass(void);

	int32_t cameraAssign0(void);
	int32_t cameraAssign1(void);
	int32_t cameraAssign2(void);
	int32_t cameraAssign3(void);

	static GameObject* target;

	void printDebugInfo(void);

	void doAttack(void);
	void doJump(void);
	void doGuardTower(void);

	void doSalvage(void);

	int32_t quickDebugInfo(void);
	int32_t setGameObjectWindow(void);
	int32_t pageGameObjectWindow1(void);
	int32_t pageGameObjectWindow2(void);
	int32_t pageGameObjectWindow3(void);
	int32_t jumpToDebugGameObject1(void);
	int32_t jumpToDebugGameObject2(void);
	int32_t jumpToDebugGameObject3(void);
	int32_t toggleDebugWins(void);
	int32_t teleport(void);
	int32_t showMovers(void);
	int32_t cullPathAreas(void);
	int32_t zeroHPrime(void);
	int32_t calcValidAreaTable(void);
	int32_t globalMapLog(void);
	int32_t brainDead(void);
	int32_t goalPlan(void);
	int32_t enemyGoalPlan(void);
	int32_t showVictim(void);
	int32_t damageObject1(void);
	int32_t damageObject2(void);
	int32_t damageObject3(void);
	int32_t damageObject4(void);
	int32_t damageObject5(void);
	int32_t damageObject6(void);
	int32_t damageObject7(void);
	int32_t damageObject8(void);
	int32_t damageObject9(void);
	int32_t damageObject0(void);
	int32_t rotateObjectLeft(void);
	int32_t rotateObjectRight(void);

	bool canAddVehicle(const Stuff::Vector3D& pos);
	bool canRecover(const Stuff::Vector3D& pos);
	int32_t makeNoTargetCursor(bool passable, bool lineOfSight, bool ctrl, bool bGui,
		int32_t moverCount, int32_t nonMoverCount);
	int32_t makeRangeCursor(bool LOS);
	int32_t makeTargetCursor(bool lineOfSight, int32_t moverCount, int32_t nonMoverCount);
	int32_t makeMoveCursor(bool bLineOfSite);
	int32_t makeJumpCursor(bool bLineOfSite);
	int32_t makeRunCursor(bool bLineOfSite);
	void updateRollovers(void);

	void addAirstrike(void);
	void addVehicle(const Stuff::Vector3D& pos);
	void updateTarget(bool bCursorIsInGui);
	void drawWayPointPaths(void);

	int32_t calcRotation(void);

	void swapResolutions(void);

	bool canSalvage(GameObject* pMover);
	bool selectionIsHelicopters();
	void drawHotKey(std::wstring_view string, std::wstring_view descStr, int32_t x, int32_t y);
	void drawHotKeys(void);

	Stuff::Vector3D makeAirStrikeTarget(const Stuff::Vector3D& pos);

	GameObject* oldTargets[MAX_ICONS];

	aFont hotKeyFont;
	aFont hotKeyHeaderFont;

	bool lastUpdateDoubleClick;

	float swapTime;

	KeyboardRef* keyboardRef;

	std::unique_ptr<Mover> reinforcement;
};

typedef MissionInterfaceManager* MissionInterfaceManagerPtr;

//--------------------------------------------------------------------------------------
#endif