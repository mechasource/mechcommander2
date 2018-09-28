//-------------------------------------------------------------------------------
//
// Mech 3D layer.  Controls how the mech moves through animations
//
// For MechCommander 2
//
// Replace Mactor for better looking mechs!
//
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#pragma once

#ifndef MECH3D_H
#define MECH3D_H

//-------------------------------------------------------------------------------
// Include Files
//#include "apprtype.h"
//#include <objectappearance.h>
//#include "msl.h"
//#include <gosfx/gosfxheaders.hpp>

//-------------------------------------------------------------------------------
// Structs used by layer.
//
typedef struct _GestureData
{
	float startVel;				 // Gesture Starts at this speed
	float endVel;				 // Gesture Ends at this speed
	int32_t frameStart;			 // Which frame does gesture start at.
	bool reverse;				 // Should we play backwards?
	int32_t rightFootDownFrame0; // When should the right foot make a poof and print?
	int32_t rightFootDownFrame1; // Sometimes the foot's down twice.
	int32_t leftFootDownFrame0;  // When should the left foot make a poof and print?
	int32_t leftFootDownFrame1;  // Sometimes the foot's down twice.
} GestureData;

typedef struct _PaintSchemata
{
	uint32_t redColor; // Replace all Reds in texture with this color scaled by intensity of red!
	uint32_t
		greenColor; // Replace all Greens in texture with this color scaled by intensity of green!
	uint32_t blueColor; // Replace all Blues in texture with this color scaled by intensity of blue!
} PaintSchemata;

typedef PaintSchemata* PaintSchemataPtr;

//-------------------------------------------------------------------------------
// Macro Definitions

enum gesture_const : uint32_t
{
	GesturePark			  = 0,
	GestureStandToPark	= 1,
	GestureStand		  = 2,
	GestureStandToWalk	= 3, // OBSOLETE!
	GestureWalk			  = 4,
	GestureParkToStand	= 5,
	GestureWalkToRun	  = 6, // OBSOLETE!
	GestureRun			  = 7,
	GestureRunToWalk	  = 8, // OBSOLETE!
	GestureReverse		  = 9,
	GestureStandToReverse = 10, // OBSOLETE!
	GestureLimpLeft		  = 11,
	GestureLimpRight	  = 12,
	GestureIdle			  = 13,
	GestureFallBackward   = 14,
	GestureFallForward	= 15,
	GestureHitFront		  = 16,
	GestureHitBack		  = 17,
	GestureHitLeft		  = 18,
	GestureHitRight		  = 19,
	GestureJump			  = 20,
	GestureRollover		  = 21,
	GestureGetUp		  = 22,
	GestureFallenForward  = 23,
	GestureFallenBackward = 24,
	MaxGestures			  = 25,
};

#define MAX_MECH_ANIMATIONS MaxGestures

enum mech3dappearancetype_const : uint32_t
{
	NUM_MECH_STATES			   = 10,
	MECH_STATE_PARKED		   = 0,
	MECH_STATE_STANDING		   = 1,
	MECH_STATE_WALKING		   = 2,
	MECH_STATE_RUNNING		   = 3,
	MECH_STATE_REVERSE		   = 4,
	MECH_STATE_LIMPING_LEFT	= 5,
	MECH_STATE_JUMPING		   = 6,
	MECH_STATE_FALLEN_FORWARD  = 7,
	MECH_STATE_FALLEN_BACKWARD = 8,
	MECH_STATE_LIMPING_RIGHT   = 9,
};

constexpr const float BASE_NODE_RECYCLE_TIME = 0.25f;

extern char MechStateByGesture[];

//-------------------------------------------------------------------------------
// class Mech3DAppearance
class Mech3DAppearanceType : public AppearanceType
{
	//--------------------------------------------
	// Stores Multi-Shapes, Animation Data, etc.
public:
	TG_TypeMultiShapePtr mechShape[MAX_LODS];
	TG_TypeMultiShapePtr mechShadowShape;
	float lodDistance[MAX_LODS];
	TG_AnimateShapePtr mechAnim[MAX_MECH_ANIMATIONS];

	TG_TypeMultiShapePtr leftArm;
	TG_TypeMultiShapePtr rightArm;

	TG_TypeMultiShapePtr mechForwardDmgShape;
	TG_TypeMultiShapePtr mechBackwardDmgShape;

	int32_t rightFootprintType; // Footprint type for this mech.
	int32_t leftFootprintType;  // Footprint type for this mech.
	int32_t shadowScalar;		// Values to scale shadow for this type of mech
	int32_t textureSide;		// Size of texture edge in pixels

	GestureData gestures[MaxGestures];

	int32_t numSmokeNodes;
	int32_t numJumpNodes;
	int32_t numWeaponNodes;
	int32_t numFootNodes;
	NodeData* nodeData;

	static TG_TypeMultiShapePtr SensorSquareShape;

	static bool animationLoadingEnabled;
	static void disableAnimationLoading(void) { animationLoadingEnabled = false; }
	static void enableAnimationLoading(void) { animationLoadingEnabled = true; }

public:
	void init(void)
	{
		int32_t i = 0;
		for (i = 0; i < MAX_LODS; i++)
		{
			mechShape[i]   = nullptr;
			lodDistance[i] = 0.0f;
		}
		for (i = 0; i < MAX_MECH_ANIMATIONS; i++)
			mechAnim[i] = nullptr;
		rightFootprintType = leftFootprintType = -1;
		shadowScalar						   = 0;   // For stupid shadows if we need to draw them.
		textureSide							   = 128; // For stupid shadows if we need to draw them.
		nodeData							   = nullptr;
		numSmokeNodes = numWeaponNodes = numJumpNodes = 0;
		mechShadowShape								  = nullptr;
	}

	Mech3DAppearanceType(void) { init(void); }

	void destroy(void);

	~Mech3DAppearanceType(void) { destroy(void); }

	virtual void init(PSTR fileName);

	int32_t getTotalNodes(void)
	{
		return numSmokeNodes + numWeaponNodes + numJumpNodes + numFootNodes;
	}

	void setAnimation(TG_MultiShapePtr shape, uint32_t animationNum);

	float getStartVel(int32_t animationNum)
	{
		if ((animationNum >= 0) && (animationNum < MaxGestures))
			return gestures[animationNum].startVel;
		return 0.0f;
	}

	float getEndVel(int32_t animationNum)
	{
		if ((animationNum >= 0) && (animationNum < MaxGestures))
			return gestures[animationNum].endVel;
		return 0.0f;
	}

	float getFrameRate(int32_t animationNum)
	{
		if ((animationNum >= 0) && (animationNum < MaxGestures) && (mechAnim[animationNum]))
			return mechAnim[animationNum]->GetFrameRate(void);
		return 0.0f;
	}

	int32_t getNumFrames(int32_t animationNum)
	{
		if ((animationNum >= 0) && (animationNum < MaxGestures) && (mechAnim[animationNum]))
			return mechAnim[animationNum]->GetNumFrames(void);
		return 0.0f;
	}

	int32_t getJumpLandFrame(void) { return (gestures[GestureJump].frameStart & 0x0000ffff); }

	int32_t getJumpTakeoffFrame(void) { return (gestures[GestureJump].frameStart >> 16); }
};

typedef Mech3DAppearanceType* Mech3DAppearanceTypePtr;

#define MAX_DUST_POOFS 3
//-------------------------------------------------------------------------------
typedef struct _MechAppearanceData
{
	float frameNum;
	float mechFrameRate;

	bool leftArmOff;
	bool rightArmOff;
	bool fallen;
	bool forceStop;
	bool atTransitionToNextGesture;
	bool inReverse;
	bool inJump;
	bool jumpSetup;
	bool jumpFXSetup;
	bool jumpAirborne;
	bool oncePerFrame;
	bool lockRotation;

	bool inDebugMoveMode;
	bool singleStepMode;
	bool nextStep;
	bool prevStep;

	float velocity;
	int32_t status;
	int32_t sensorLevel;

	int32_t currentStateGoal;
	int32_t currentGestureId;
	int32_t transitionState;
	int32_t oldStateGoal;
	int32_t hitGestureId; // What gesture should I go back to when hit?

	float currentFrame; // One for each part.
	int32_t currentLOD;

	int32_t nodeUsed[10];  // Used to stagger the weapon nodes for firing.
	float nodeRecycle[10]; // Used for ripple fire to find out if the node has
						   // fired recently.

	Stuff::Vector3D jumpDestination;
	Stuff::Vector3D jumpVelocity; // Real velocity vector now.  Just plug into
								  // mech velocity.

	float baseRootNodeHeight;

	int32_t isSmoking;
	bool isWaking;
	bool isDusting;
	bool fallDust;
	bool isHelicopter;

} MechAppearanceData;

class Mech3DAppearance : public ObjectAppearance
{
	//-------------------------------------------------------------------------------------
	// Stores instance specific data.  Frame counts, position, velocity, texture
	// map, etc.
protected:
	Mech3DAppearanceTypePtr mechType;

	TG_MultiShapePtr mechShape;
	TG_MultiShapePtr mechShadowShape;
	TG_MultiShapePtr sensorSquareShape;
	TG_MultiShapePtr sensorTriangleShape;
	float sensorSpin;

	TG_MultiShapePtr leftArm;
	TG_MultiShapePtr rightArm;

	Stuff::Vector3D leftArmPos;
	Stuff::Vector3D rightArmPos;

	float leftArmHazeFactor;
	float rightArmHazeFactor;
	bool leftArmInView;
	bool rightArmInView;

	float frameNum;
	float mechFrameRate;

	float torsoRotation;
	float leftArmRotation;
	float rightArmRotation;

	bool leftArmOff;
	bool rightArmOff;
	bool fallen;
	bool forceStop;
	bool atTransitionToNextGesture;
	bool inReverse;
	bool inJump;
	bool jumpSetup;
	bool jumpFXSetup;
	bool jumpAirborne;
	bool oncePerFrame;
	bool lockRotation;

	bool inDebugMoveMode;
	bool singleStepMode;
	bool nextStep;
	bool prevStep;

	float velocity;
	int32_t status;
	int32_t sensorLevel;

	int32_t currentStateGoal;
	int32_t currentGestureId;
	int32_t transitionState;
	int32_t oldStateGoal;
	int32_t hitGestureId; // What gesture should I go back to when hit?

	float currentFrame; // One for each part.
	int32_t currentLOD;

	TG_LightPtr pointLight;
	uint32_t lightId;

	float idleTime; // Elapsed time since I've done something.
	// If it gets larger then X, play the idle animation.

	Stuff::Vector3D jumpDestination;
	Stuff::Vector3D jumpVelocity; // Real velocity vector now.  Just plug into
								  // mech velocity.
	float hazeFactor;

	int32_t* nodeUsed;  // Used to stagger the weapon nodes for firing.
	float* nodeRecycle; // Used for ripple fire to find out if the node has
						// fired recently.

	Stuff::Vector3D footPos[2]; // Used to store previous frame foot positions.
	// For foot poofs and footprints.
	// NEW  GOS FX
	gosFX::Effect* rightDustPoofEffect[MAX_DUST_POOFS];
	gosFX::Effect* leftDustPoofEffect[MAX_DUST_POOFS];
	gosFX::Effect* smokeEffect;
	gosFX::Effect* jumpJetEffect;
	gosFX::Effect* rightShoulderBoom;
	gosFX::Effect* leftShoulderBoom;
	gosFX::Effect* criticalSmoke;
	gosFX::Effect* waterWake;
	gosFX::Effect* helicopterDustCloud;
	gosFX::Effect* rightArmSmoke;
	gosFX::Effect* leftArmSmoke;

	bool movedThisFrame;

	Stuff::Point3D leftShoulderPos;
	Stuff::Point3D rightShoulderPos;

	int32_t currentRightPoof;
	int32_t currentLeftPoof;

	int32_t leftFootPoofDraw[MAX_DUST_POOFS];
	int32_t rightFootPoofDraw[MAX_DUST_POOFS];

	Stuff::Point3D lFootPosition[MAX_DUST_POOFS];
	Stuff::Point3D rFootPosition[MAX_DUST_POOFS];
	bool rightFootDone0;
	bool leftFootDone0;
	bool rightFootDone1;
	bool leftFootDone1;

	bool inCombatMode;

	int32_t isSmoking;
	bool isWaking;
	bool isDusting;
	bool fallDust;
	bool isHelicopter;

	float OBBRadius;

	uint32_t localTextureHandle;
	float baseRootNodeHeight;

	uint32_t psRed;
	uint32_t psBlue;
	uint32_t psGreen;

	char mechName[64];

	// Arm off coolness
	Stuff::Vector3D dVel[2];
	Stuff::Vector3D dRot[2];
	Stuff::Vector3D dAcc[2];
	Stuff::Vector3D dRVel[2];
	Stuff::Vector3D dRacc[2];
	float dTime[2];

	float flashDuration;
	float duration;
	float currentFlash;
	bool drawFlash;
	uint32_t flashColor;

	int32_t rotationalNodeIndex;
	int32_t hitLeftNodeIndex;
	int32_t hitRightNodeIndex;
	int32_t rootNodeIndex;
	int32_t leftArmNodeIndex;
	int32_t rightArmNodeIndex;
	int32_t lightCircleNodeIndex;
	float baseRootNodeDifference;

public:
	static PaintSchemataPtr paintSchemata;
	static uint32_t numPaintSchemata;

public:
	Mech3DAppearance(void) { init(void); }

	~Mech3DAppearance(void) { destroy(void); }

	virtual void destroy(void);

	virtual void init(AppearanceTypePtr tree = nullptr, GameObjectPtr obj = nullptr);

	virtual void initFX(void);

	virtual AppearanceTypePtr getAppearanceType(void) { return mechType; }

	virtual int32_t update(bool animate = true);
	virtual int32_t render(int32_t depthFixup = 0);
	virtual int32_t renderShadows(void);

	virtual void updateFootprints(void);

	virtual void updateGeometry(void);

	virtual void setPaintScheme(void);

	virtual void setPaintScheme(uint32_t red, uint32_t green, uint32_t blue);

	virtual void getPaintScheme(uint32_t& red, uint32_t& green, uint32_t& blue);

	virtual void resetPaintScheme(uint32_t red, uint32_t green, uint32_t blue);

	virtual bool isInJump(void) { return inJump; }

	void flashBuilding(float dur, float fDuration, uint32_t color);

	virtual bool isJumpSetup(void) { return jumpSetup; }

	virtual bool isJumpAirborne(void) { return jumpAirborne; }

	virtual bool setJumpParameters(Stuff::Vector3D& end);

	virtual bool haveFallen(void) { return fallen; }

	virtual bool isMouseOver(float px, float py);

	virtual bool recalcBounds(void);

	virtual void setBrake(bool brake) { forceStop = brake; }

	bool checkStop(void) { return forceStop; }

	virtual int32_t getCurrentGestureId(void) { return currentGestureId; }

	virtual void setGesture(size_t gestureId)
	{
		//--------------------------------
		// We are at the goal.  Store off
		// our current gesture state.
		//
		// NOTE: This is only used to force the helicopters to be parked
		// at startup.  Any other attempt to use this will probably not work!!
		//
		// These two lines force the select box to match the helicopter's
		// position in the sky
		currentGestureId = 2;
		inView			 = true;
		update(void);
		currentGestureId = gestureId;
		oldStateGoal	 = 0;  // This is ONLY valid for park!!!!
		currentStateGoal = -1; // Not going anywhere
		transitionState  = 0;  // No Transition necessary.
		currentFrame	 = mechType->getNumFrames(currentGestureId) - 1;
	}

	virtual int32_t setGestureGoal(int32_t stateGoal)
	{
		// This will set which gesture we are trying to get to.
		//-----------------------------------------------------------
		// Smart(tm) Gesture code goes here.
		//
		// NO gesture queueing.  Current request is current goal.
		//
		if (oncePerFrame)
			return 0;
		if (oldStateGoal == stateGoal)
			return (0);
		if (currentStateGoal == stateGoal)
			return (0);
		if ((stateGoal < MECH_STATE_PARKED) || (stateGoal > MECH_STATE_LIMPING_RIGHT))
			return (0);
		if (currentStateGoal != -1)
			return 0;
		if ((currentGestureId == GestureHitFront) || (currentGestureId == GestureHitBack) ||
			(currentGestureId == GestureHitLeft) || (currentGestureId == GestureHitRight))
		{
			return 0;
		}
		if ((stateGoal == MECH_STATE_JUMPING) && !jumpSetup)
			return 0;
		//---------------------------------------------------------------
		// If we are still on our way to something, check if the current
		// Gesture will allow a change IMMEDIATELY.  Set the appropriate
		// variables if this is TRUE and go on.  Otherwise, IGNORE THIS
		// COMMAND.  MECH will continually ask to change if necessary.
		transitionState			  = 0; // Start at beginning gestureId of this goal.
		currentStateGoal		  = stateGoal;
		atTransitionToNextGesture = true;
		oncePerFrame			  = true;
		return -1;
	}

	virtual int32_t getFrameNumber(int32_t partNum) { return frameNum; }

	virtual float getNumFramesInGesture(size_t gestureId)
	{
		if (mechType)
			return mechType->getNumFrames(gestureId);
		return (0.0f);
	}

	virtual float getVelocityOfGesture(size_t gestureId)
	{
		float result	  = mechType->getStartVel(gestureId);
		float otherResult = mechType->getEndVel(gestureId);
		if (otherResult == result)
			return (result);
		else
		{
			float vel   = otherResult - result;
			otherResult = getNumFramesInGesture(gestureId);
			if (otherResult > Stuff::SMALL)
			{
				vel *= currentFrame / otherResult;
				vel += result;
			}
			return vel;
		}
	}

	virtual Stuff::Vector3D getVelocity(void) { return jumpVelocity; }

	virtual float getVelocityMagnitude(void)
	{
		if (jumpAirborne)
			return velocity;
		else
			return getVelocityOfGesture(currentGestureId);
	}

	virtual void setCombatMode(bool combatMode)
	{
		//------------------------------------------------------------
		// However we move the arms from gun to normal mode goes here.
		inCombatMode = combatMode;
	}

	virtual void setObjectParameters(Stuff::Vector3D& pos, float legRot, int32_t selected,
		int32_t alignment, int32_t homeRelations);

	virtual void setMoverParameters(
		float turretRot, float lArmRot = 0.0f, float rArmRot = 0.0f, bool isAirborne = false);

	void debugUpdate(int32_t whichOne);

	void setJustDoIt(void);

	void clearJustDoIt(void);

	virtual void setObjectNameId(int32_t objId) { objectNameId = objId; }

	static void LoadPaintSchemata(void);

	virtual size_t getAppearanceClass(void) { return MECH_APPR_TYPE; }

	virtual void setDebugMoveMode(void) { inDebugMoveMode = true; }

	//--------------------------------------------
	// Once site Objects are in place, go get 'em
	virtual void setWeaponNodeUsed(int32_t nodeId);

	virtual Stuff::Vector3D getWeaponNodePosition(int32_t nodeId);

	virtual Stuff::Vector3D getNodePosition(int32_t nodeId);

	virtual Stuff::Vector3D getNodeNamePosition(PSTR nodeName);

	virtual int32_t getWeaponNode(int32_t weapontype);

	virtual int32_t getLowestWeaponNode(void);

	virtual float getWeaponNodeRecycle(int32_t node);

	virtual void resetWeaponNodes(void);

	virtual void setWeaponNodeRecycle(int32_t nodeId, float time);

	virtual void setSingleStepMode(void) { singleStepMode ^= true; }

	virtual void setPrevFrame(void) { prevStep = true; }

	virtual void setNextFrame(void) { nextStep = true; }

	virtual void setSensorLevel(int32_t lvl) { sensorLevel = lvl; }

	//------------------------------------------------------------------------------------------
	// Puts mech into hit mode IF and ONLY IF the mech is standing, walking,
	// running or limping.
	// NO OTHER GESTURE IS
	// VALID!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	virtual void hitFront(void);

	virtual void hitBack(void);

	virtual void hitLeft(void);

	virtual void hitRight(void);

	virtual void blowLeftArm(void);

	virtual void blowRightArm(void);

	virtual void setObjStatus(int32_t oStatus);

	virtual bool PerPolySelect(int32_t mouseX, int32_t mouseY);

	virtual Stuff::Point3D getRootNodeCenter(void)
	{
		Stuff::Point3D result = mechShape->GetRootNodeCenter(void);
		return result;
	}

	virtual void setAlphaValue(uint8_t aVal)
	{
		mechShape->SetAlphaValue(aVal);
		// Sensor shape fades in opposite direction from mover
		sensorSquareShape->SetAlphaValue(0xff - aVal);
		sensorTriangleShape->SetAlphaValue(0xff - aVal);
	}

	virtual void setMechName(PCSTR pName) { strcpy(mechName, pName); }

	virtual void startSmoking(int32_t smokeLvl);
	virtual void startWaterWake(void);
	virtual void stopWaterWake(void);
	virtual void playEjection(void);

	void copyTo(MechAppearanceData* data);
	void copyFrom(MechAppearanceData* data);

	bool leftArmRecalc(void);
	bool rightArmRecalc(void);

	virtual bool getRightArmOff(void) { return rightArmOff; }

	virtual bool getLeftArmOff(void) { return leftArmOff; }

	virtual Stuff::Vector3D getHitNodeLeft(void);
	virtual Stuff::Vector3D getHitNodeRight(void);

	virtual Stuff::Vector3D getNodeIdPosition(int32_t nodeId);
};

//-------------------------------------------------------------------------------

#endif
