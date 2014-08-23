//--------------------------------------------------------------------------------------
//
// MechCommander 2 -- Copyright (c) 1998 FASA Interactive
//
// This header contains the mission classes for the GUI
//
// GUI is now single update driven.  An event comes in, the manager decides who its for
// and passes the event down.  Eveything still know how to draw etc.
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

#include <mclib.h>
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
PVOID gMalloc (size_t size);
void gFree (PVOID me);

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

	typedef int32_t (MissionInterfaceManager::*COMMAND)( void );

	struct Command
		{
			int32_t	key;
			int32_t	cursor;
			int32_t	cursorLOS;
			bool	singleClick;
			COMMAND function;
			COMMAND	releaseFunction;
			int32_t	hotKeyDescriptionText;
		};

	static Command*		getCommands() { return commands; }
	static int32_t*		getOldKeys() { return OldKeys; }
	
	protected:


	   static int32_t mouseX;
	   static int32_t mouseY;
	   static gosEnum_KeyIndex WAYPOINT_KEY;

	   static float pauseWndVelocity;

	

		static Command		commands[MAX_COMMAND];
		static int32_t			OldKeys[MAX_COMMAND];

		int32_t terrainLineChanged;

		//-------------------------------------------
		// Data to control scroll, rotation and zoom
		float						baseFrameLength;
		
		float						zoomInc;
		float						rotationInc;
		float						scrollInc;
		
		float						screenScrollLeft;
		float						screenScrollRight;
		float						screenScrollUp;
		float						screenScrollDown;
		
		float						realRotation;
		float						degPerSecRot;
		
		int32_t						zoomChoice;			//Can range from 0 to 3 right now.
		
		//----------------------------------------------
		// Mouse Event Data
		bool						isDragging;
		float						vTolSpeed;
		PCSTR					vehicleFile;
		int32_t						vehicleID[MAX_TEAMS];
		Stuff::Vector3D				dragStart;
		Stuff::Vector3D				dragEnd;
		bool						selectClicked;			// left click
		bool						commandClicked;			// usually right clicked
		bool						cameraClicked;
		
		ControlGui					controlGui;
		Stuff::Vector3D				wPos;					//World Position of camera cursor.
		
		//vTol
		Stuff::Vector3D				vPos[MAX_TEAMS];		// vehicle position
		BldgAppearance*				vTol[MAX_TEAMS];
		bool						paintingVtol[MAX_TEAMS];
		float						vTolTime[MAX_TEAMS];
		gosFX::Effect				*recoveryBeam[MAX_TEAMS];
		gosFX::Effect				*dustCloud[MAX_TEAMS];
		bool						mechRecovered[MAX_TEAMS];
		bool						vehicleDropped[MAX_TEAMS];
		MoverPtr					mechToRecover[MAX_TEAMS];
		
		bool						bPaused;
		bool						bPausedWithoutMenu;
		bool						bEnergyWeapons;
		bool						bDrawHotKeys;
		int32_t						resolution;
		
		//-----------------------------------------------
		// Attila (Strategic Commander) Data
		float						attilaXAxis;
		float						attilaYAxis;
		float						attilaRZAxis;

		//-----------------------------------------------
		// Tutorial Data
		bool						animationRunning;
		float						timeLeftToScroll;
		int32_t						targetButtonId;
		int32_t						buttonNumFlashes;
		float						buttonFlashTime;
		bool						targetIsButton;
		bool						targetIsPressed;
		bool						guiFrozen;

		bool						bForcedShot;
		bool						bAimedShot;
		
		static	MissionInterfaceManager* s_instance;
				
	public:
	
		MissionInterfaceManager (void)
		{
			init();
		}
		
		~MissionInterfaceManager (void)
		{
			destroy();
		}

		static MissionInterfaceManager* instance(){ return s_instance; }
		
		void init (void);
		
		void init (FitIniFilePtr loader);
		void initTacMap( PacketFile* file, int32_t packet );
	
		virtual void destroy (void);
		
		virtual void update (void);
		virtual void drawVTOL (void);
		virtual void render (void);
						
		void	initMechs(){ controlGui.initMechs(); }	

		PCSTR getSupportVehicleNameFromID (int32_t ID) { return controlGui.getVehicleNameFromID(ID); }

		void	addMover (MoverPtr mover){ controlGui.addMover(mover); }	
		void	removeMover (MoverPtr mover){ controlGui.removeMover(mover); }	

		int32_t update( bool bLeftClick, bool bRightClick, int32_t mouseX, int32_t mouseY, GameObject* pTarget, bool bLOS ); // check for each key in the list
		
		void	updateVTol();
		
		ControlGui *getControlGui (void)
		{
			return &controlGui;
		}

		void playMovie (PSTR filename)
		{
			controlGui.playMovie(filename);
		}

		int32_t handleOrders( TacticalOrder& order );
		bool anySelectedWithoutAreaEffect (void);
		void updateWaypoints (void);
		
		void setTarget( GameObject* pTarget ){ target = pTarget; }
		static void selectForceGroup( int32_t forceGroup, bool deselectOthers );
		void doRepair( GameObject* who);
		void doRepairBay( GameObject* who);
		void doEject( GameObject* who );
		bool	canRepair( GameObject* pMover );
		bool	canRepairBay ( GameObject* bay );

		void setMechRecovered (int32_t teamID, bool set) {
			mechRecovered[teamID] = set;
		}

		void updateOldStyle(bool shiftDn, bool altDn, bool ctrlDn, bool bGui, 
			bool lineOfSight, bool passable, int32_t moverCount, int32_t nonMoverCount );
		void updateAOEStyle(bool shiftDn, bool altDn, bool ctrlDn, bool bGui, 
			bool lineOfSight, bool passable, int32_t moverCount, int32_t nonMoverCount );

		static int32_t saveHotKeys( FitIniFile& file );
		static int32_t loadHotKeys( FitIniFile& file );

		static int32_t setHotKey( int32_t whichCommand, gosEnum_KeyIndex key, bool bShift, bool bControl, bool bAlt );
		int32_t getHotKey( int32_t whichCommand, gosEnum_KeyIndex& newKey, bool& bShift, bool& bControl, bool& bAlt );
		bool hotKeyIsPressed( int32_t whichCommand );

		int32_t setWayPointKey( gosEnum_KeyIndex key );

		void setAOEStyle();
		void setMCStyle();

		bool isAOEStyle();
		bool isMCStyle();

		bool	isPaused();
		bool	isPausedWithoutMenu();
		int32_t		togglePause();
		int32_t		togglePauseWithoutMenu();
		int32_t		toggleHoldPosition();
		int32_t		showObjectives();
		int32_t		showObjectives(bool on);
		void	doMove(const Stuff::Vector3D& pos);
		void	doGuard(GameObject* pObj);
		int32_t toggleHotKeys();
		void	beginVtol(int32_t supportID, int32_t commanderID = 0, Stuff::Vector3D* reinforcePos = NULL, MoverPtr salvageTarget = NULL);

		//Tutorial Stuff goes here.
		bool startAnimation(int32_t buttonId,bool isButton,bool isPressed,float timeToScroll,int32_t numFlashes);
		void setTutorialText(PCSTR text);

		bool isInCalloutAnimation ()
		{
			return animationRunning;
		}
		
		void freezeGUI (bool flag)
		{
			guiFrozen = flag;
		}

		//In-Mission Save
		void Save (FitIniFilePtr file);
		void Load (FitIniFilePtr file);

	private:

		void makeForceGroup( int32_t forceGroup );
		bool moveCameraAround( bool lineOfSight, bool passable, bool ctrl, bool bGui, int32_t moverCount, int32_t nonMoverCount );
		bool canJump(); // selected units can jump
		bool canJumpToWPos();
		void doDrag(bool bGui);


		int32_t attackShort();
		int32_t attackMedium();
		int32_t attackLong();
		int32_t alphaStrike();
		int32_t defaultAttack();
		int32_t jump();
		int32_t stopJump();
		int32_t fireFromCurrentPos();
		int32_t stopFireFromCurrentPos();
		int32_t guard();
		int32_t stopGuard();
		int32_t conserveAmmo();
		int32_t selectVisible();
		int32_t addVisibleToSelection();
		int32_t aimLeg();
		int32_t aimArm();
		int32_t aimHead();
		int32_t removeCommand();
		int32_t powerUp();
		int32_t powerDown();
		int32_t changeSpeed();
		int32_t stopChangeSpeed();
		int32_t eject();
		int32_t forceShot();

		int32_t scrollUp();
		int32_t scrollDown();
		int32_t scrollLeft();
		int32_t scrollRight();
		int32_t zoomOut();
		int32_t zoomIn();
		int32_t zoomChoiceOut();
		int32_t zoomChoiceIn();
		int32_t rotateLeft();
		int32_t rotateRight();
		int32_t tiltUp();
		int32_t tiltDown();
		int32_t centerCamera();
		int32_t rotateLightLeft();
		int32_t rotateLightRight();
		int32_t rotateLightUp();
		int32_t rotateLightDown();
		int32_t switchTab();
		int32_t reverseSwitchTab();

		int32_t drawTerrain();
		int32_t drawOverlays();
		int32_t drawBuildings();
		int32_t showGrid();
		int32_t recalcLights();
		int32_t drawClouds();
		int32_t drawFog();
		int32_t usePerspective();
		int32_t drawWaterEffects();
		int32_t recalcWater();
		int32_t drawShadows();
		int32_t changeLighting();
		int32_t vehicleCommand();
		int32_t toggleGUI();
		int32_t	drawTGLShapes();
		int32_t infoCommand();
		int32_t infoButtonReleased();
		int32_t energyWeapons();
		int32_t sendAirstrike();
		int32_t sendLargeAirstrike();
		int32_t gotoNextNavMarker();
		int32_t sendSensorStrike();
		
		int32_t	handleChatKey();
		int32_t	handleTeamChatKey();


		bool makePatrolPath();
		
		int32_t bigAirStrike();
		int32_t smlAirStrike();
		int32_t snsAirStrike();
		
		int32_t cameraNormal();
		int32_t cameraDefault();
		int32_t cameraMaxIn();
		int32_t cameraTight();
		int32_t cameraFour();
		int32_t toggleCompass();
		
		int32_t cameraAssign0();
		int32_t cameraAssign1();
		int32_t cameraAssign2();
		int32_t cameraAssign3();

		static GameObject*	target;

		void printDebugInfo();

		void doAttack();
		void doJump();
		void doGuardTower();
		
		void doSalvage();

		int32_t quickDebugInfo ();
		int32_t setGameObjectWindow ();
		int32_t pageGameObjectWindow1 ();
		int32_t pageGameObjectWindow2 ();
		int32_t pageGameObjectWindow3 ();
		int32_t jumpToDebugGameObject1 ();
		int32_t jumpToDebugGameObject2 ();
		int32_t jumpToDebugGameObject3 ();
		int32_t toggleDebugWins ();
		int32_t teleport ();
		int32_t showMovers ();
		int32_t cullPathAreas ();
		int32_t zeroHPrime ();
		int32_t calcValidAreaTable ();
		int32_t globalMapLog ();
		int32_t brainDead ();
		int32_t goalPlan ();
		int32_t enemyGoalPlan ();
		int32_t showVictim ();
		int32_t damageObject1 ();
		int32_t damageObject2 ();
		int32_t damageObject3 ();
		int32_t damageObject4 ();
		int32_t damageObject5 ();
		int32_t damageObject6 ();
		int32_t damageObject7 ();
		int32_t damageObject8 ();
		int32_t damageObject9 ();
		int32_t damageObject0 ();
		int32_t rotateObjectLeft ();
		int32_t rotateObjectRight ();

		bool	canAddVehicle( const Stuff::Vector3D& pos );
		bool	canRecover( const Stuff::Vector3D& pos );
		int32_t	makeNoTargetCursor( bool passable, bool lineOfSight, bool ctrl, bool bGui,
			int32_t moverCount, int32_t nonMoverCount );
		int32_t	makeRangeCursor( bool LOS);
		int32_t	makeTargetCursor( bool lineOfSight, int32_t moverCount, int32_t nonMoverCount );
		int32_t	makeMoveCursor( bool bLineOfSite );
		int32_t	makeJumpCursor( bool bLineOfSite );
		int32_t	makeRunCursor( bool bLineOfSite );
		void	updateRollovers();



		void	addAirstrike();
		void	addVehicle( const Stuff::Vector3D& pos );
		void	updateTarget(bool bCursorIsInGui);
		void	drawWayPointPaths();

		int32_t	calcRotation();



		void	swapResolutions();
		

		bool	canSalvage( GameObject* pMover );
		bool	selectionIsHelicopters( );
		void	drawHotKey( PCSTR string, PCSTR descStr, int32_t x, int32_t y );
		void	drawHotKeys();



		Stuff::Vector3D makeAirStrikeTarget( const Stuff::Vector3D& pos );

		GameObject*		oldTargets[MAX_ICONS];

		aFont			hotKeyFont;
		aFont			hotKeyHeaderFont;

		bool			lastUpdateDoubleClick;

		float			swapTime;

		KeyboardRef*	keyboardRef;

		MoverPtr		reinforcement;
};

typedef MissionInterfaceManager *MissionInterfaceManagerPtr;

//--------------------------------------------------------------------------------------
#endif