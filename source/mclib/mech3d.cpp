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

#include "stdinc.h"

#include "mech3d.h"
#include "camera.h"
#include "dbasegui.h"
#include "terrain.h"
#endif

#ifndef MSTATES_H
#include "mstates.h"
#endif

#ifndef OBJSTATUS_H
#include "objstatus.h"
#endif

#ifndef CIDENT_H
#include "cident.h"
#endif

#ifndef PATHS_H
#include "paths.h"
#endif

#ifndef USERINPUT_H
#include "userinput.h"
#endif

#ifndef TIMING_H
#include "timing.h"
#endif

#ifndef UTILITIES_H
#include "utilities.h"
#endif

#ifndef CEVFX_H
#include "cevfx.h"
#endif

#ifndef TXMMGR_H
#include "txmmgr.h"
#endif

#ifndef CELINE_H
#include "celine.h"
#endif

#ifndef WEAPONFX_H
#include "weaponfx.h"
#endif

#ifndef CRATER_H
#include "crater.h"
#endif

#ifndef GVACTOR_H
#include "gvactor.h"
#endif

//-------------------------------------------------------------------------------
// Static Globals
extern float worldUnitsPerMeter;
extern bool useFog;
extern float metersPerWorldUnit;
extern MidLevelRenderer::MLRClipper* theClipper;
bool useNonWeaponEffects = true;
bool useHighObjectDetail = true;
bool InEditor = false;

extern bool MLRVertexLimitReached;
extern bool useShadows;

#define BODYSTATE_NORMAL 0
#define BODYSTATE_STANDING 1
#define BODYSTATE_PARKED 2
#define BODYSTATE_FALLEN_BACKWARD 3
#define BODYSTATE_FALLEN_FORWARD 4

#define EXPAND_FACTOR (1.25f)

#define HELICOPTER_FACTOR (25.0f)

char MechStateByGesture[MAX_MECH_ANIMATIONS] = {
	BODYSTATE_PARKED, // GesturePark					 0 -- Park
	BODYSTATE_STANDING, // GestureParkToStand			 1 -- ParkToStand
	BODYSTATE_STANDING, // GestureStand					 2 -- Stand
	BODYSTATE_NORMAL, // GestureStandToWalk			 3 -- StandToWalk
	BODYSTATE_NORMAL, // GestureWalk					 4 -- Walk
	BODYSTATE_PARKED, // GestureStandToPark			 5 -- StandToPark
	BODYSTATE_NORMAL, // GestureWalkToRun				 6 -- WalkToRun
	BODYSTATE_NORMAL, // GestureRun					 7 -- Run
	BODYSTATE_NORMAL, // GestureRunToWalk				 8 -- RunToWalk
	BODYSTATE_NORMAL, // GestureReverse				 9 -- Reverse
	BODYSTATE_NORMAL, // GestureStandToReverse		10 -- StandToReverse
	BODYSTATE_NORMAL, // GestureLimpLeft				11 -- LimpLeft
	BODYSTATE_NORMAL, // GestureLimpRight				12 -- LimpRight
	BODYSTATE_STANDING, // GestureIdle					13 -- Idle
	BODYSTATE_FALLEN_BACKWARD, // GestureFallBackward			14 --
	// FallBackward
	BODYSTATE_FALLEN_FORWARD, // GestureFallForward			15 --
	// FallForward
	BODYSTATE_NORMAL, // GestureHitFront				16 -- HitFront
	BODYSTATE_NORMAL, // GestureHitBack				17 -- HitBack
	BODYSTATE_NORMAL, // GestureHitLeft				18 -- HitLeft
	BODYSTATE_NORMAL, // GestureHitRight				19 -- HitRight
	BODYSTATE_NORMAL, // GestureJump					20 -- Jump
	BODYSTATE_FALLEN_FORWARD, // GestureRollover				21 -- Rollover
	// from FallBack to FallForward
	BODYSTATE_FALLEN_FORWARD, // GestureGetup					22 -- Get up
	// from FallForward
	BODYSTATE_FALLEN_FORWARD, // GestureFallenForward			23 -- Fallen
	// Forward (Single Frame, mech on ground)
	BODYSTATE_FALLEN_FORWARD, // GestureFallenBackward		24 -- Fallen
	// Backward (Single Frame, mech on ground)
};

#define MAX_ANIMATION_FILES 25
#define idleMAX (20.0f)

#define FX_POOF_ID 32
#define FX_JUMP_ID 43
#define FX_SMOKE_ID 48

#define SPIN_RATE 90.0f
#define JUMP_PITCH (90.0f)

extern bool reloadBounds;
PSTR MechAnimationNames[MaxGestures + 2] = {
	"StandToPark", // GesturePark							 0 -- Park
	"ParkToStand", // GestureParkToStand					 1 --
	// ParkToStand
	"", // GestureStand							 2 -- Stand
	"STtoWK", // GestureStandToWalk					 3 -- StandToWalk
	"Walk", // GestureWalk							 4 -- Walk
	"StandToPark", // GestureStandToPark					 5 --
	// StandToPark
	"WKtoRN", // GestureWalkToRun						 6 -- WalkToRun
	"Run", // GestureRun							 7 -- Run
	"RNToWK", // GestureRunToWalk						 8 -- RunToWalk
	"Walk", // GestureReverse						 9 -- Reverse
	"WKtoST", // GestureStandToReverse					10 -- StandToReverse
	"LimpLeft", // GestureLimpLeft						11 -- LimpLeft
	"LimpRight", // GestureLimpRight						12 -- LimpRight
	"Idle", // GestureIdle							13 -- Idle
	"FallBackward", // GestureFallBackward					14 --
	// FallBackward
	"FallForward", // GestureFallForward					15 --
	// FallForward
	"HitFront", // GestureHitFront						16 -- HitFront
	"HitBack", // GestureHitBack						17 -- HitBack
	"HitLeft", // GestureHitLeft						18 -- HitLeft
	"HitRight", // GestureHitRight						19 -- HitRight
	"Jump", // GestureJump							20 -- Jump
	"GetupBack", // GestureRollover						21 -- Getup from
	// FallBack
	"GetupFront", // GestureGetup							22 -- Getup from
	// FallForward
	"FallForward", // GestureFallenForward					23 -- Fallen Forward
	// (Single Frame, mech on ground)
	"FallBackward", // GestureFallenBackward					24 -- Fallen Backward
	// (Single Frame, mech on ground)
	"FallBackwardDam", // Destroyed States
	"FallForwardDam" // Destroyed States
};

PaintSchemataPtr Mech3DAppearance::paintSchemata = nullptr;
uint32_t Mech3DAppearance::numPaintSchemata = 0;

TG_TypeMultiShapePtr Mech3DAppearanceType::SensorSquareShape = nullptr;

extern int32_t ObjectTextureSize;

#define FOOTPRINT_SLOP 2

bool Mech3DAppearanceType::animationLoadingEnabled = true;

//-------------------------------------------------------------------------------
// class Mech3DAppearanceType
void
Mech3DAppearanceType::init(PSTR fileName)
{
	AppearanceType::init(fileName);
	//---------------------------------------------------
	// Load the Gesture Data.  Should be in filename.ini
	FullPathFileName mechIniName;
	mechIniName.init(tglPath, fileName, ".ini");
	gosASSERT(fileExists(mechIniName));
	FitIniFile mechFile;
	int32_t result = mechFile.open(mechIniName);
	gosASSERT(result == NO_ERROR);
	for (size_t i = 0; i < MaxGestures; i++)
	{
		char blockId[256];
		sprintf(blockId, "Gestures%d", i);
		result = mechFile.seekBlock(blockId);
		gosASSERT(result == NO_ERROR);
		result = mechFile.readIdFloat("StartVel", gestures[i].startVel);
		gosASSERT(result == NO_ERROR);
		result = mechFile.readIdFloat("EndVel", gestures[i].endVel);
		gosASSERT(result == NO_ERROR);
		result = mechFile.readIdLong("StartFrame", gestures[i].frameStart);
		gosASSERT(result == NO_ERROR);
		result = mechFile.readIdBoolean("Reverse", gestures[i].reverse);
		gosASSERT(result == NO_ERROR);
		result = mechFile.readIdLong("RightFootDown0", gestures[i].rightFootDownFrame0);
		if (result != NO_ERROR)
			gestures[i].rightFootDownFrame0 = 99999;
		result = mechFile.readIdLong("RightFootDown1", gestures[i].rightFootDownFrame1);
		if (result != NO_ERROR)
			gestures[i].rightFootDownFrame1 = 99999;
		result = mechFile.readIdLong("LeftFootDown0", gestures[i].leftFootDownFrame0);
		if (result != NO_ERROR)
			gestures[i].leftFootDownFrame0 = 99999;
		result = mechFile.readIdLong("LeftFootDown1", gestures[i].leftFootDownFrame1);
		if (result != NO_ERROR)
			gestures[i].leftFootDownFrame1 = 99999;
	}
	result = mechFile.seekBlock("TGLData");
	if (result != NO_ERROR)
		STOP(("Mech %s has no TGL Data", fileName));
	char aseFileName[512];
	result = mechFile.readIdString("FileName", aseFileName, 511);
	int32_t i;
	if (result != NO_ERROR)
	{
		// Check for LOD filenames
		char aseFileName[512];
		for (i = 0; i < MAX_LODS; i++)
		{
			char baseName[256];
			char baseLODDist[256];
			sprintf(baseName, "FileName%d", i);
			sprintf(baseLODDist, "Distance%d", i);
			result = mechFile.readIdString(baseName, aseFileName, 511);
			if (result == NO_ERROR)
			{
				result = mechFile.readIdFloat(baseLODDist, lodDistance[i]);
				if (result != NO_ERROR)
					STOP(("LOD %d has no distance value in file %s", i, fileName));
				//----------------------------------------------
				// Base LOD shape.  In stand Pose by default.
				mechShape[i] = new TG_TypeMultiShape;
				gosASSERT(mechShape[i] != nullptr);
				FullPathFileName mechName;
				mechName.init(tglPath, aseFileName, ".ase");
				mechShape[i]->LoadTGMultiShapeFromASE(mechName);
			}
			else if (!i)
			{
				STOP(("No base LOD for shape %s", fileName));
			}
		}
	}
	else
	{
		FullPathFileName mechName;
		mechName.init(tglPath, aseFileName, ".ase");
		//----------------------------------------------
		// Base shape.  In stand Pose by default.
		mechShape[0] = new TG_TypeMultiShape;
		gosASSERT(mechShape[0] != nullptr);
		mechShape[0]->LoadTGMultiShapeFromASE(mechName);
	}
	result = mechFile.readIdString("ShadowName", aseFileName, 511);
	if (result == NO_ERROR)
	{
		//----------------------------------------------
		// Base Shadow shape.
		mechShadowShape = new TG_TypeMultiShape;
		gosASSERT(mechShadowShape != nullptr);
		FullPathFileName gvName;
		gvName.init(tglPath, aseFileName, ".ase");
		mechShadowShape->LoadTGMultiShapeFromASE(gvName);
	}
	//------------------------------------
	// Load Arms to blow off Next.
	char leftArmName[1024];
	sprintf(leftArmName, "%sLeftArm", fileName);
	FullPathFileName mechLeftArmName;
	mechLeftArmName.init(tglPath, leftArmName, ".ase");
	char rightArmName[1024];
	sprintf(rightArmName, "%sRightArm", fileName);
	FullPathFileName mechRightArmName;
	mechRightArmName.init(tglPath, rightArmName, ".ase");
	rightArm = new TG_TypeMultiShape;
	gosASSERT(rightArm != nullptr);
	result = rightArm->LoadTGMultiShapeFromASE(mechRightArmName);
	if (result)
	{
		delete rightArm;
		rightArm = nullptr;
	}
	leftArm = new TG_TypeMultiShape;
	gosASSERT(leftArm != nullptr);
	result = leftArm->LoadTGMultiShapeFromASE(mechLeftArmName);
	if (result)
	{
		delete leftArm;
		leftArm = nullptr;
	}
	//----------------------------------------------
	//-----------------------------------------------
	// Load Damaged States
	// They are named mechnamefallForward and mechnameFallBackward
	//
	char forwardName[1024];
	sprintf(forwardName, "%sFallForwardDam", fileName);
	char backwardName[1024];
	sprintf(backwardName, "%sFallForwardDam", fileName);
	{
		FullPathFileName dmgName;
		dmgName.init(tglPath, forwardName, ".ase");
		mechForwardDmgShape = new TG_TypeMultiShape;
		gosASSERT(mechForwardDmgShape != nullptr);
		mechForwardDmgShape->LoadTGMultiShapeFromASE(dmgName);
		if (!mechForwardDmgShape->GetNumShapes())
		{
			delete mechForwardDmgShape;
			mechForwardDmgShape = nullptr;
		}
	}
	{
		FullPathFileName dmgName;
		dmgName.init(tglPath, backwardName, ".ase");
		mechBackwardDmgShape = new TG_TypeMultiShape;
		gosASSERT(mechBackwardDmgShape != nullptr);
		mechBackwardDmgShape->LoadTGMultiShapeFromASE(dmgName);
		if (!mechBackwardDmgShape->GetNumShapes())
		{
			delete mechBackwardDmgShape;
			mechBackwardDmgShape = nullptr;
		}
	}
	//-----------------------
	// Load Animations Next.
	if (animationLoadingEnabled)
	{
		for (i = 0; i < MAX_ANIMATION_FILES; i++)
		{
			char animName[512];
			sprintf(animName, "%s%s", fileName, MechAnimationNames[i]);
			FullPathFileName animPath;
			animPath.init(tglPath, animName, ".ase");
			FullPathFileName otherPath;
			otherPath.init(tglPath, animName, ".agl");
			if (fileExists(animPath) || fileExists(otherPath))
			{
				mechAnim[i] = new TG_AnimateShape;
				gosASSERT(mechAnim[i] != nullptr);
				//---------------------------------------------------------------------------------------------
				// If this animation does not exist, it is not a problem!
				// Mech will simply freeze until animation is "over" and then
				// move to next animation in chain.
				mechAnim[i]->LoadTGMultiShapeAnimationFromASE(animPath, mechShape[0]);
				if (gestures[i].reverse)
					mechAnim[i]->ReverseFrameRate();
			}
			else
				mechAnim[i] = nullptr;
		}
	}
	//------------------------------
	// Load up the foot print type.
	result = mechFile.seekBlock("FootPrint");
	if (result != NO_ERROR)
	{
		leftFootprintType = 0;
		rightFootprintType = 0;
	}
	result = mechFile.readIdLong("FootprintType", rightFootprintType);
	if (result != NO_ERROR)
	{
		result = mechFile.readIdLong("RightFootprintType", rightFootprintType);
		if (result != NO_ERROR)
		{
			leftFootprintType = 0;
			rightFootprintType = 0;
		}
		result = mechFile.readIdLong("LeftFootprintType", leftFootprintType);
		if (result != NO_ERROR)
			leftFootprintType = rightFootprintType;
	}
	else
	{
		leftFootprintType = rightFootprintType;
	}
	//-----------------------------------------------
	// Load up the Node Data.
	result = mechFile.seekBlock("Nodes");
	if (result != NO_ERROR)
		numSmokeNodes = numWeaponNodes = numJumpNodes = 0;
	result = mechFile.readIdLong("NumSmoke", numSmokeNodes);
	if (result != NO_ERROR)
		numSmokeNodes = 0;
	result = mechFile.readIdLong("NumWeapon", numWeaponNodes);
	if (result != NO_ERROR)
		numWeaponNodes = 0;
	result = mechFile.readIdLong("NumJumpjet", numJumpNodes);
	if (result != NO_ERROR)
		numJumpNodes = 0;
	result = mechFile.readIdLong("NumFeet", numFootNodes);
	if (result != NO_ERROR)
		numFootNodes = 0;
	if (numJumpNodes + numWeaponNodes + numSmokeNodes + numFootNodes)
	{
		nodeData = (NodeData*)AppearanceTypeList::appearanceHeap->Malloc(
			sizeof(NodeData) * (numJumpNodes + numWeaponNodes + numSmokeNodes + numFootNodes));
		gosASSERT(nodeData != nullptr);
		for (i = 0; i < numSmokeNodes; i++)
		{
			char blockId[512];
			sprintf(blockId, "SmokeNode%d", i);
			result = mechFile.seekBlock(blockId);
			gosASSERT(result == NO_ERROR);
			char smokeName[512];
			result = mechFile.readIdString("SmokeNodeName", smokeName, 511);
			gosASSERT(result == NO_ERROR);
			nodeData[i].nodeId =
				(PSTR)AppearanceTypeList::appearanceHeap->Malloc(strlen(smokeName) + 1);
			gosASSERT(nodeData[i].nodeId != nullptr);
			strcpy(nodeData[i].nodeId, smokeName);
			nodeData[i].weaponType = MECH3D_WEAPONTYPE_NONE;
		}
		for (i = 0; i < numWeaponNodes; i++)
		{
			char blockId[512];
			sprintf(blockId, "WeaponNode%d", i);
			result = mechFile.seekBlock(blockId);
			gosASSERT(result == NO_ERROR);
			char weaponName[512];
			result = mechFile.readIdString("WeaponNodeName", weaponName, 511);
			gosASSERT(result == NO_ERROR);
			int32_t weaponType;
			result = mechFile.readIdLong("WeaponType", weaponType);
			gosASSERT(result == NO_ERROR);
			nodeData[i + numSmokeNodes].nodeId =
				(PSTR)AppearanceTypeList::appearanceHeap->Malloc(strlen(weaponName) + 1);
			gosASSERT(nodeData[i + numSmokeNodes].nodeId != nullptr);
			strcpy(nodeData[i + numSmokeNodes].nodeId, weaponName);
			nodeData[i + numSmokeNodes].weaponType = weaponType;
		}
		for (i = 0; i < numJumpNodes; i++)
		{
			char blockId[512];
			sprintf(blockId, "JumpJetNode%d", i);
			result = mechFile.seekBlock(blockId);
			gosASSERT(result == NO_ERROR);
			char jumpName[512];
			result = mechFile.readIdString("JumpNodeName", jumpName, 511);
			gosASSERT(result == NO_ERROR);
			nodeData[i + numSmokeNodes + numWeaponNodes].nodeId =
				(PSTR)AppearanceTypeList::appearanceHeap->Malloc(strlen(jumpName) + 1);
			gosASSERT(nodeData[i + numSmokeNodes + numWeaponNodes].nodeId != nullptr);
			strcpy(nodeData[i + numSmokeNodes + numWeaponNodes].nodeId, jumpName);
			nodeData[i + numSmokeNodes + numWeaponNodes].weaponType = MECH3D_WEAPONTYPE_NONE;
		}
		for (i = 0; i < numFootNodes; i++)
		{
			char blockId[512];
			sprintf(blockId, "FootNode%d", i);
			result = mechFile.seekBlock(blockId);
			gosASSERT(result == NO_ERROR);
			char footName[512];
			result = mechFile.readIdString("FootNodeName", footName, 511);
			gosASSERT(result == NO_ERROR);
			nodeData[i + numSmokeNodes + numWeaponNodes + numJumpNodes].nodeId =
				(PSTR)AppearanceTypeList::appearanceHeap->Malloc(strlen(footName) + 1);
			gosASSERT(
				nodeData[i + numSmokeNodes + numWeaponNodes + numJumpNodes].nodeId != nullptr);
			strcpy(nodeData[i + numSmokeNodes + numWeaponNodes + numJumpNodes].nodeId, footName);
			nodeData[i + numSmokeNodes + numWeaponNodes + numJumpNodes].weaponType =
				MECH3D_WEAPONTYPE_NONE;
		}
	}
	//----------------------------------------------
	// Load up sensor textures if not yet defined.
	// For helicopters, load up the vehicle one!!
	if (SensorSquareShape == nullptr && numJumpNodes)
	{
		FullPathFileName sensorName;
		sensorName.init(tglPath, "squarecontact", ".ase");
		SensorSquareShape = new TG_TypeMultiShape;
		gosASSERT(SensorSquareShape != nullptr);
		SensorSquareShape->LoadTGMultiShapeFromASE(sensorName);
	}
	if (GVAppearanceType::SensorCircleShape == nullptr && !numJumpNodes)
	{
		FullPathFileName sensorName;
		sensorName.init(tglPath, "circularcontact", ".ase");
		GVAppearanceType::SensorCircleShape = new TG_TypeMultiShape;
		gosASSERT(GVAppearanceType::SensorCircleShape != nullptr);
		GVAppearanceType::SensorCircleShape->LoadTGMultiShapeFromASE(sensorName);
	}
	if (GVAppearanceType::SensorTriangleShape == nullptr)
	{
		FullPathFileName sensorName;
		sensorName.init(tglPath, "trianglecontact", ".ase");
		GVAppearanceType::SensorTriangleShape = new TG_TypeMultiShape;
		gosASSERT(GVAppearanceType::SensorTriangleShape != nullptr);
		GVAppearanceType::SensorTriangleShape->LoadTGMultiShapeFromASE(sensorName);
	}
}

//----------------------------------------------------------------------------
void
Mech3DAppearanceType::destroy(void)
{
	AppearanceType::destroy();
	int32_t i = 0;
	for (i = 0; i < MAX_LODS; i++)
	{
		if (mechShape[i])
		{
			delete mechShape[i];
			mechShape[i] = nullptr;
		}
	}
	if (mechShadowShape)
	{
		delete mechShadowShape;
		mechShadowShape = nullptr;
	}
	if (rightArm)
	{
		delete rightArm;
		rightArm = nullptr;
	}
	if (leftArm)
	{
		delete leftArm;
		leftArm = nullptr;
	}
	if (mechForwardDmgShape)
	{
		delete mechForwardDmgShape;
		mechForwardDmgShape = nullptr;
	}
	if (mechBackwardDmgShape)
	{
		delete mechBackwardDmgShape;
		mechBackwardDmgShape = nullptr;
	}
	for (i = 0; i < MAX_ANIMATION_FILES; i++)
	{
		delete mechAnim[i];
		mechAnim[i] = nullptr;
	}
	for (i = 0; i < getTotalNodes(); i++)
	{
		AppearanceTypeList::appearanceHeap->Free(nodeData[i].nodeId);
		nodeData[i].nodeId = nullptr;
	}
	AppearanceTypeList::appearanceHeap->Free(nodeData);
	nodeData = nullptr;
	numSmokeNodes = numWeaponNodes = numJumpNodes = 0;
}

//----------------------------------------------------------------------------
void
Mech3DAppearanceType::setAnimation(TG_MultiShapePtr shape, uint32_t animationNum)
{
	gosASSERT(shape != nullptr);
	gosASSERT(animationNum != 0xffffffff);
	gosASSERT(animationNum < MaxGestures);
	if (mechAnim[animationNum])
		mechAnim[animationNum]->SetAnimationState(shape);
	else
		shape->ClearAnimation();
}

//-----------------------------------------------------------------------------

#define MAX_MECHS 1
Stuff::Vector3D debugMechActorPosition[MAX_MECHS];
float mechDebugAngle[MAX_MECHS];
float torsoDebugAngle[MAX_MECHS];
//-----------------------------------------------------------------------------
// class Mech3DAppearance
void
Mech3DAppearance::resetWeaponNodes(void)
{
	// THis should never be called after the game inits!!
	for (size_t i = 0; i < mechType->numWeaponNodes; i++)
	{
		nodeUsed[i] = 0;
		nodeRecycle[i] = BASE_NODE_RECYCLE_TIME;
	}
}

//-----------------------------------------------------------------------------
void
Mech3DAppearance::setWeaponNodeUsed(int32_t weaponNode)
{
	// THis should never be called after the game inits!!
	weaponNode -= mechType->numSmokeNodes;
	if ((weaponNode >= 0) && (weaponNode < mechType->numWeaponNodes))
	{
		nodeUsed[weaponNode]++;
		nodeRecycle[weaponNode] = BASE_NODE_RECYCLE_TIME;
	}
}

//-----------------------------------------------------------------------------
void
Mech3DAppearance::setWeaponNodeRecycle(int32_t nodeId, float time)
{
	nodeId -= mechType->numSmokeNodes;
	if ((nodeId >= 0) && (nodeId < mechType->numWeaponNodes))
		nodeRecycle[nodeId] = time;
}

//-----------------------------------------------------------------------------
Stuff::Vector3D
Mech3DAppearance::getWeaponNodePosition(int32_t nodeId)
{
	Stuff::Vector3D result = position;
	if ((nodeId < mechType->numSmokeNodes) || (nodeId >= (mechType->numSmokeNodes + mechType->numWeaponNodes)))
		return result;
	if (!inView)
		return result;
	// We already know we are using this node.  Do NOT increment recycle or
	// nodeUsed!
	//-------------------------------------------
	// Create Matrix to conform to.
	Stuff::UnitQuaternion qRotation;
	float yaw = rotation * DEGREES_TO_RADS;
	qRotation = Stuff::EulerAngles(0.0f, yaw, 0.0f);
	Stuff::Point3D xlatPosition;
	xlatPosition.x = -position.x;
	xlatPosition.y = position.z;
	xlatPosition.z = position.y;
	Stuff::UnitQuaternion torsoRot;
	torsoRot = Stuff::EulerAngles(0.0f, (torsoRotation * DEGREES_TO_RADS), 0.0f);
	if (rotationalNodeIndex == -1)
		rotationalNodeIndex = mechShape->SetNodeRotation("joint_torso", &torsoRot);
	mechShape->SetNodeRotation(rotationalNodeIndex, &torsoRot);
	result = mechShape->GetTransformedNodePosition(
		&xlatPosition, &qRotation, mechType->nodeData[nodeId].nodeId);
	if ((result.x == 0.0f) && (result.y == 0.0f) && (result.z == 0.0f))
		result = position;
	return result;
}

//-----------------------------------------------------------------------------
Stuff::Vector3D
Mech3DAppearance::getNodeNamePosition(PSTR nodeName)
{
	Stuff::Vector3D result = position;
	if (!inView)
		return result;
	//-------------------------------------------
	// Create Matrix to conform to.
	Stuff::UnitQuaternion qRotation;
	float yaw = rotation * DEGREES_TO_RADS;
	qRotation = Stuff::EulerAngles(0.0f, yaw, 0.0f);
	Stuff::Point3D xlatPosition;
	xlatPosition.x = -position.x;
	xlatPosition.y = position.z;
	xlatPosition.z = position.y;
	Stuff::UnitQuaternion torsoRot;
	torsoRot = Stuff::EulerAngles(0.0f, (torsoRotation * DEGREES_TO_RADS), 0.0f);
	if (rotationalNodeIndex == -1)
		rotationalNodeIndex = mechShape->SetNodeRotation("joint_torso", &torsoRot);
	mechShape->SetNodeRotation(rotationalNodeIndex, &torsoRot);
	result = mechShape->GetTransformedNodePosition(&xlatPosition, &qRotation, nodeName);
	if ((result.x == 0.0f) && (result.y == 0.0f) && (result.z == 0.0f))
		result = position;
	return result;
}

//-----------------------------------------------------------------------------
Stuff::Vector3D
Mech3DAppearance::getNodeIdPosition(int32_t nodeId)
{
	Stuff::Vector3D result = position;
	if (!inView)
		return result;
	//-------------------------------------------
	// Create Matrix to conform to.
	Stuff::UnitQuaternion qRotation;
	float yaw = rotation * DEGREES_TO_RADS;
	qRotation = Stuff::EulerAngles(0.0f, yaw, 0.0f);
	Stuff::Point3D xlatPosition;
	xlatPosition.x = -position.x;
	xlatPosition.y = position.z;
	xlatPosition.z = position.y;
	Stuff::UnitQuaternion torsoRot;
	torsoRot = Stuff::EulerAngles(0.0f, (torsoRotation * DEGREES_TO_RADS), 0.0f);
	if (rotationalNodeIndex == -1)
		rotationalNodeIndex = mechShape->SetNodeRotation("joint_torso", &torsoRot);
	mechShape->SetNodeRotation(rotationalNodeIndex, &torsoRot);
	result = mechShape->GetTransformedNodePosition(&xlatPosition, &qRotation, nodeId);
	if ((result.x == 0.0f) && (result.y == 0.0f) && (result.z == 0.0f))
		result = position;
	return result;
}

//-----------------------------------------------------------------------------
Stuff::Vector3D
Mech3DAppearance::getNodePosition(int32_t nodeId)
{
	Stuff::Vector3D result = position;
	if ((nodeId < 0) || (nodeId >= mechType->getTotalNodes()))
		return result;
	if (!inView)
		return result;
	//-------------------------------------------
	// Create Matrix to conform to.
	Stuff::UnitQuaternion qRotation;
	float yaw = rotation * DEGREES_TO_RADS;
	qRotation = Stuff::EulerAngles(0.0f, yaw, 0.0f);
	Stuff::Point3D xlatPosition;
	xlatPosition.x = -position.x;
	xlatPosition.y = position.z;
	xlatPosition.z = position.y;
	Stuff::UnitQuaternion torsoRot;
	torsoRot = Stuff::EulerAngles(0.0f, (torsoRotation * DEGREES_TO_RADS), 0.0f);
	if (rotationalNodeIndex == -1)
		rotationalNodeIndex = mechShape->SetNodeRotation("joint_torso", &torsoRot);
	mechShape->SetNodeRotation(rotationalNodeIndex, &torsoRot);
	result = mechShape->GetTransformedNodePosition(
		&xlatPosition, &qRotation, mechType->nodeData[nodeId].nodeId);
	return result;
}

//-----------------------------------------------------------------------------
int32_t
Mech3DAppearance::getLowestWeaponNode(void)
{
	//------------------------------------------------
	// Scan all weapon nodes and find least used one.
	int32_t bestNode = -1;
	float lowestPosZ;
	int32_t numSmokeNodes = mechType->numSmokeNodes;
	lowestPosZ = 9999999999999.0f;
	for (size_t i = 0; i < mechType->numWeaponNodes; i++)
	{
		Stuff::Vector3D nodePosition = getNodePosition(i + numSmokeNodes);
		if (nodePosition.z < lowestPosZ)
		{
			lowestPosZ = nodePosition.z;
			bestNode = i + numSmokeNodes;
		}
	}
	if ((lowestPosZ == 0.0f) || (bestNode < 0) || (bestNode >= mechType->getTotalNodes()))
		return -1;
	return bestNode;
}

//-----------------------------------------------------------------------------
int32_t
Mech3DAppearance::getWeaponNode(int32_t weaponType)
{
	//------------------------------------------------
	// Scan all weapon nodes and find least used one.
	// BIG change here.  This is ONLY called at load time.
	// NEVER during actual game execution.  In this way,
	// Weapons always fire from the same nodes!!
	int32_t leastUsed = 999999999;
	int32_t bestNode = -1;
	int32_t numSmokeNodes = mechType->numSmokeNodes;
	for (size_t i = 0; i < mechType->numWeaponNodes; i++)
	{
		switch (weaponType)
		{
		case MECH3D_WEAPONTYPE_MISSILE:
			if ((mechType->nodeData[numSmokeNodes + i].weaponType == weaponType) || (mechType->nodeData[numSmokeNodes + i].weaponType == MECH3D_WEAPONTYPE_ANY) || (mechType->nodeData[numSmokeNodes + i].weaponType == MECH3D_WEAPONTYPE_NONENERGY))
			{
				// This node is blown off.
				if (rightArmOff && mechType->nodeData[i + numSmokeNodes].isRArmNode)
					continue;
				// This node is blown off.
				if (leftArmOff && mechType->nodeData[i + numSmokeNodes].isLArmNode)
					continue;
				if (nodeUsed[i] < leastUsed)
				{
					leastUsed = nodeUsed[i];
					bestNode = i + numSmokeNodes;
				}
			}
			break;
		case MECH3D_WEAPONTYPE_BALLISTIC:
			if ((mechType->nodeData[numSmokeNodes + i].weaponType == weaponType) || (mechType->nodeData[numSmokeNodes + i].weaponType == MECH3D_WEAPONTYPE_ANY) || (mechType->nodeData[numSmokeNodes + i].weaponType == MECH3D_WEAPONTYPE_DIRECT) || (mechType->nodeData[numSmokeNodes + i].weaponType == MECH3D_WEAPONTYPE_NONENERGY))
			{
				// This node is blown off.
				if (rightArmOff && mechType->nodeData[i + numSmokeNodes].isRArmNode)
					continue;
				// This node is blown off.
				if (leftArmOff && mechType->nodeData[i + numSmokeNodes].isLArmNode)
					continue;
				if (nodeUsed[i] < leastUsed)
				{
					leastUsed = nodeUsed[i];
					bestNode = i + numSmokeNodes;
				}
			}
			break;
		case MECH3D_WEAPONTYPE_ENERGY:
			if ((mechType->nodeData[numSmokeNodes + i].weaponType == weaponType) || (mechType->nodeData[numSmokeNodes + i].weaponType == MECH3D_WEAPONTYPE_DIRECT) || (mechType->nodeData[numSmokeNodes + i].weaponType == MECH3D_WEAPONTYPE_ANY))
			{
				// This node is blown off.
				if (rightArmOff && mechType->nodeData[i + numSmokeNodes].isRArmNode)
					continue;
				// This node is blown off.
				if (leftArmOff && mechType->nodeData[i + numSmokeNodes].isLArmNode)
					continue;
				if (nodeUsed[i] < leastUsed)
				{
					leastUsed = nodeUsed[i];
					bestNode = i + numSmokeNodes;
				}
			}
			break;
		case MECH3D_WEAPONTYPE_ANY:
			// This node is blown off.
			if (rightArmOff && mechType->nodeData[i + numSmokeNodes].isRArmNode)
				continue;
			// This node is blown off.
			if (leftArmOff && mechType->nodeData[i + numSmokeNodes].isLArmNode)
				continue;
			if (nodeUsed[i] < leastUsed)
			{
				leastUsed = nodeUsed[i];
				bestNode = i + numSmokeNodes;
			}
			break;
		default:
			STOP(("Sent down a bad weapon type %d", weaponType));
		}
	}
	if ((bestNode < 0) || (bestNode >= mechType->getTotalNodes()))
		return -1;
	// This should never be called AFTER the game inits!!!
	setWeaponNodeUsed(bestNode);
	return bestNode;
}

//-----------------------------------------------------------------------------
float
Mech3DAppearance::getWeaponNodeRecycle(int32_t node)
{
	node -= mechType->numSmokeNodes;
	if ((node >= 0) && (node < mechType->numWeaponNodes))
		return nodeRecycle[node];
	return 9999.0f; // NOT a weapon node, never recycled!!
}

//-----------------------------------------------------------------------------
void
Mech3DAppearance::init(AppearanceTypePtr tree, GameObjectPtr obj)
{
	Appearance::init(tree, obj);
	mechType = (Mech3DAppearanceType*)tree;
	mechName[0] = 0;
	pilotNameID = -1;
	paintScheme = -1;
	objectNameId = 30862;
	baseRootNodeHeight = -99999.9f;
	baseRootNodeDifference = 0.0f;
	rotationalNodeIndex = -1;
	hitLeftNodeIndex = hitRightNodeIndex = -1;
	rootNodeIndex = lightCircleNodeIndex = -1;
	leftArmNodeIndex = rightArmNodeIndex = -1;
	screenPos.x = screenPos.y = screenPos.z = screenPos.w = -999.0f;
	frameNum = 0.0f;
	rightShoulderPos.x = rightShoulderPos.y = rightShoulderPos.z = -99999.9f;
	leftShoulderPos.x = leftShoulderPos.y = leftShoulderPos.z = -99999.9f;
	idleTime = RandomNumber(idleMAX);
	hazeFactor = 0.0f;
	rotation = torsoRotation = leftArmRotation = rightArmRotation = 0;
	leftArmOff = rightArmOff = fallen = false;
	position.x = position.y = position.z = 0.0f;
	velocity = 0.0f;
	teamId = paintScheme = -1;
	homeTeamRelationship = 0;
	selected = 0;
	status = 0;
	lockRotation = oncePerFrame = false;
	sensorLevel = 0;
	sensorSpin = 0.0f;
	OBBRadius = 0.0f;
	currentLOD = 0;
	jumpVelocity.Zero();
	jumpFXSetup = false;
	nodeUsed = nullptr;
	nodeRecycle = nullptr;
	currentRightPoof = currentLeftPoof = 0;
	localTextureHandle = 0xffffffff;
	isSmoking = -1;
	isWaking = false;
	isDusting = false;
	movedThisFrame = false;
	fallDust = false;
	criticalSmoke = nullptr;
	smokeEffect = nullptr;
	waterWake = nullptr;
	helicopterDustCloud = nullptr;
	isHelicopter = false;
	leftArm = rightArm = nullptr;
	rightArmSmoke = leftArmSmoke = nullptr;
	currentFlash = duration = flashDuration = 0.0f;
	flashColor = 0x00000000;
	drawFlash = false;
	// Default to Bright RGB.
	psRed = psGreen = psBlue = 0xffffffff;
	if (mechType)
	{
		mechShape = mechType->mechShape[0]->CreateFrom();
		sensorTriangleShape = GVAppearanceType::SensorTriangleShape->CreateFrom();
		if (mechType->leftArm)
			leftArm = mechType->leftArm->CreateFrom();
		if (mechType->rightArm)
			rightArm = mechType->rightArm->CreateFrom();
		//-------------------------------------------------
		// Load the texture and store its handle.
		int32_t i;
		for (i = 0; i < sensorTriangleShape->GetNumTextures(); i++)
		{
			char txmName[1024];
			sensorTriangleShape->GetTextureName(i, txmName, 256);
			char texturePath[1024];
			sprintf(texturePath, "%s%d\\", tglPath, ObjectTextureSize);
			FullPathFileName textureName;
			textureName.init(texturePath, txmName, "");
			if (fileExists(textureName))
			{
				if (strnicmp(txmName, "a_", 2) == 0)
				{
					uint32_t gosTextureHandle = mcTextureManager->loadTexture(
						textureName, gos_Texture_Alpha, gosHint_DisableMipmap | gosHint_DontShrink);
					sensorTriangleShape->SetTextureHandle(i, gosTextureHandle);
					sensorTriangleShape->SetTextureAlpha(i, true);
				}
				else
				{
					uint32_t gosTextureHandle = mcTextureManager->loadTexture(
						textureName, gos_Texture_Solid, gosHint_DisableMipmap | gosHint_DontShrink);
					sensorTriangleShape->SetTextureHandle(i, gosTextureHandle);
					sensorTriangleShape->SetTextureAlpha(i, false);
				}
			}
			else
			{
				// PAUSE(("Warning: %s texture name not found",textureName));
				sensorTriangleShape->SetTextureHandle(i, 0xffffffff);
			}
		}
		if (mechType->numJumpNodes)
		{
			sensorSquareShape = Mech3DAppearanceType::SensorSquareShape->CreateFrom();
			//-------------------------------------------------
			// Load the texture and store its handle.
			for (i = 0; i < sensorSquareShape->GetNumTextures(); i++)
			{
				char txmName[1024];
				sensorSquareShape->GetTextureName(i, txmName, 256);
				char texturePath[1024];
				sprintf(texturePath, "%s%d\\", tglPath, ObjectTextureSize);
				FullPathFileName textureName;
				textureName.init(texturePath, txmName, "");
				if (fileExists(textureName))
				{
					if (strnicmp(txmName, "a_", 2) == 0)
					{
						uint32_t gosTextureHandle = mcTextureManager->loadTexture(textureName,
							gos_Texture_Alpha, gosHint_DisableMipmap | gosHint_DontShrink);
						sensorSquareShape->SetTextureHandle(i, gosTextureHandle);
						sensorSquareShape->SetTextureAlpha(i, true);
					}
					else
					{
						uint32_t gosTextureHandle = mcTextureManager->loadTexture(textureName,
							gos_Texture_Solid, gosHint_DisableMipmap | gosHint_DontShrink);
						sensorSquareShape->SetTextureHandle(i, gosTextureHandle);
						sensorSquareShape->SetTextureAlpha(i, false);
					}
				}
				else
				{
					// PAUSE(("Warning: %s texture name not
					// found",textureName));
					sensorSquareShape->SetTextureHandle(i, 0xffffffff);
				}
			}
		}
		else
		{
			sensorSquareShape = GVAppearanceType::SensorCircleShape->CreateFrom();
			//-------------------------------------------------
			// Load the texture and store its handle.
			for (i = 0; i < sensorSquareShape->GetNumTextures(); i++)
			{
				char txmName[1024];
				sensorSquareShape->GetTextureName(i, txmName, 256);
				char texturePath[1024];
				sprintf(texturePath, "%s%d\\", tglPath, ObjectTextureSize);
				FullPathFileName textureName;
				textureName.init(texturePath, txmName, "");
				if (fileExists(textureName))
				{
					if (strnicmp(txmName, "a_", 2) == 0)
					{
						uint32_t gosTextureHandle = mcTextureManager->loadTexture(textureName,
							gos_Texture_Alpha, gosHint_DisableMipmap | gosHint_DontShrink);
						sensorSquareShape->SetTextureHandle(i, gosTextureHandle);
						sensorSquareShape->SetTextureAlpha(i, true);
					}
					else
					{
						uint32_t gosTextureHandle = mcTextureManager->loadTexture(textureName,
							gos_Texture_Solid, gosHint_DisableMipmap | gosHint_DontShrink);
						sensorSquareShape->SetTextureHandle(i, gosTextureHandle);
						sensorSquareShape->SetTextureAlpha(i, false);
					}
				}
				else
				{
					// PAUSE(("Warning: %s texture name not
					// found",textureName));
					sensorSquareShape->SetTextureHandle(i, 0xffffffff);
				}
			}
		}
		if (mechType->numWeaponNodes)
		{
			nodeUsed = (int32_t*)AppearanceTypeList::appearanceHeap->Malloc(
				sizeof(int32_t) * mechType->numWeaponNodes);
			gosASSERT(nodeUsed != nullptr);
			memset(nodeUsed, 0, sizeof(int32_t) * mechType->numWeaponNodes);
			nodeRecycle = (float*)AppearanceTypeList::appearanceHeap->Malloc(
				sizeof(float) * mechType->numWeaponNodes);
			gosASSERT(nodeRecycle != nullptr);
			for (size_t i = 0; i < mechType->numWeaponNodes; i++)
				nodeRecycle[i] = 0.0f;
		}
		for (i = 0; i < mechType->numWeaponNodes; i++)
		{
			if (mechShape->isChildOf(mechType->nodeData->nodeId, "joint_ruarm"))
				mechType->nodeData[i + mechType->numSmokeNodes].isRArmNode = true;
			else
				mechType->nodeData[i + mechType->numSmokeNodes].isRArmNode = false;
			if (mechShape->isChildOf(mechType->nodeData->nodeId, "joint_luarm"))
				mechType->nodeData[i + mechType->numSmokeNodes].isLArmNode = true;
			else
				mechType->nodeData[i + mechType->numSmokeNodes].isLArmNode = false;
		}
		Stuff::Vector3D boxCoords[8];
		Stuff::Vector3D nodeCenter = mechShape->GetRootNodeCenter();
		boxCoords[0].x = position.x + mechShape->GetMinBox().x + nodeCenter.x;
		boxCoords[0].y = position.y + mechShape->GetMinBox().z + nodeCenter.z;
		boxCoords[0].z = position.z + mechShape->GetMaxBox().y + nodeCenter.y;
		boxCoords[1].x = position.x + mechShape->GetMinBox().x + nodeCenter.x;
		boxCoords[1].y = position.y + mechShape->GetMaxBox().z + nodeCenter.z;
		boxCoords[1].z = position.z + mechShape->GetMaxBox().y + nodeCenter.y;
		boxCoords[2].x = position.x + mechShape->GetMaxBox().x + nodeCenter.x;
		boxCoords[2].y = position.y + mechShape->GetMaxBox().z + nodeCenter.z;
		boxCoords[2].z = position.z + mechShape->GetMaxBox().y + nodeCenter.y;
		boxCoords[3].x = position.x + mechShape->GetMaxBox().x + nodeCenter.x;
		boxCoords[3].y = position.y + mechShape->GetMinBox().z + nodeCenter.z;
		boxCoords[3].z = position.z + mechShape->GetMaxBox().y + nodeCenter.y;
		boxCoords[4].x = position.x + mechShape->GetMinBox().x + nodeCenter.x;
		boxCoords[4].y = position.y + mechShape->GetMinBox().z + nodeCenter.z;
		boxCoords[4].z = position.z + mechShape->GetMinBox().y + nodeCenter.y;
		boxCoords[5].x = position.x + mechShape->GetMaxBox().x + nodeCenter.x;
		boxCoords[5].y = position.y + mechShape->GetMinBox().z + nodeCenter.z;
		boxCoords[5].z = position.z + mechShape->GetMinBox().y + nodeCenter.y;
		boxCoords[6].x = position.x + mechShape->GetMaxBox().x + nodeCenter.x;
		boxCoords[6].y = position.y + mechShape->GetMaxBox().z + nodeCenter.z;
		boxCoords[6].z = position.z + mechShape->GetMinBox().y + nodeCenter.y;
		boxCoords[7].x = position.x + mechShape->GetMinBox().x + nodeCenter.x;
		boxCoords[7].y = position.y + mechShape->GetMaxBox().z + nodeCenter.z;
		boxCoords[7].z = position.z + mechShape->GetMinBox().y + nodeCenter.y;
		float testRadius = 0.0;
		for (i = 0; i < 8; i++)
		{
			testRadius = boxCoords[i].GetLength();
			if (OBBRadius < testRadius)
				OBBRadius = testRadius;
		}
		mechType->boundsUpperLeftX = (-OBBRadius);
		mechType->boundsUpperLeftY = (-OBBRadius * 2.0);
		mechType->boundsLowerRightX = (OBBRadius);
		mechType->boundsLowerRightY = (OBBRadius);
		if (!mechType->getDesignerTypeBounds())
		{
			mechType->typeUpperLeft.x = mechShape->GetMaxBox().x + nodeCenter.x;
			mechType->typeUpperLeft.y = mechShape->GetMaxBox().z + nodeCenter.z;
			mechType->typeUpperLeft.z = mechShape->GetMaxBox().y + nodeCenter.y;
			mechType->typeLowerRight.x = mechShape->GetMinBox().x + nodeCenter.x;
			mechType->typeLowerRight.y = mechShape->GetMinBox().z + nodeCenter.z;
			mechType->typeLowerRight.z = mechShape->GetMinBox().y + nodeCenter.y;
			// Now expand box by some percentage to make selection easier.
			mechType->typeUpperLeft.x *= EXPAND_FACTOR;
			mechType->typeUpperLeft.y *= EXPAND_FACTOR;
			mechType->typeUpperLeft.z *= EXPAND_FACTOR;
			mechType->typeLowerRight.x *= EXPAND_FACTOR;
			mechType->typeLowerRight.y *= EXPAND_FACTOR;
			mechType->typeLowerRight.z *= EXPAND_FACTOR;
		}
	}
	mechFrameRate = 30.0f;
	forceStop = atTransitionToNextGesture = inReverse = inJump = false;
	inDebugMoveMode = singleStepMode = nextStep = prevStep = false;
	currentStateGoal = -1; // Always start ready to change gestures
	currentGestureId = 2; // Always start in Stand Mode
	transitionState = 0;
	oldStateGoal = 1; // Always start in Stand Mode
	currentFrame = 0.0f;
	pointLight = nullptr;
	lightId = 0xffffffff;
	hitGestureId = -1;
	jumpSetup = false;
	jumpAirborne = inJump = false;
	footPos[0].x = footPos[0].y = footPos[0].z = 1000.0f;
	footPos[1].x = footPos[1].y = footPos[1].z = 1000.0f;
	for (size_t i = 0; i < MAX_DUST_POOFS; i++)
	{
		rightDustPoofEffect[i] = nullptr;
		leftDustPoofEffect[i] = nullptr;
	}
	smokeEffect = nullptr;
	jumpJetEffect = nullptr;
	rightShoulderBoom = leftShoulderBoom = nullptr;
	leftFootDone0 = rightFootDone0 = leftFootDone1 = rightFootDone1 = false;
	leftFootPoofDraw[0] = leftFootPoofDraw[1] = leftFootPoofDraw[2] = rightFootPoofDraw[0] =
		rightFootPoofDraw[1] = rightFootPoofDraw[2] = false;
	if (mechType && mechType->mechShadowShape)
	{
		mechShadowShape = mechType->mechShadowShape->CreateFrom();
		//-------------------------------------------------
		// Load the texture and store its handle.
		for (size_t i = 0; i < mechShadowShape->GetNumTextures(); i++)
		{
			char txmName[1024];
			mechShadowShape->GetTextureName(i, txmName, 256);
			char texturePath[1024];
			sprintf(texturePath, "%s%d\\", tglPath, ObjectTextureSize);
			FullPathFileName textureName;
			textureName.init(texturePath, txmName, "");
			if (fileExists(textureName))
			{
				if (strnicmp(txmName, "a_", 2) == 0)
				{
					uint32_t gosTextureHandle = mcTextureManager->loadTexture(
						textureName, gos_Texture_Alpha, gosHint_DisableMipmap | gosHint_DontShrink);
					gosASSERT(gosTextureHandle != 0xffffffff);
					mechShadowShape->SetTextureHandle(i, gosTextureHandle);
					mechShadowShape->SetTextureAlpha(i, true);
				}
				else
				{
					uint32_t gosTextureHandle = mcTextureManager->loadTexture(
						textureName, gos_Texture_Solid, gosHint_DisableMipmap | gosHint_DontShrink);
					gosASSERT(gosTextureHandle != 0xffffffff);
					mechShadowShape->SetTextureHandle(i, gosTextureHandle);
					mechShadowShape->SetTextureAlpha(i, false);
				}
			}
			else
			{
				mechShadowShape->SetTextureHandle(i, 0xffffffff);
			}
		}
	}
	else
	{
		mechShadowShape = nullptr;
	}
}

void
Mech3DAppearance::initFX(void)
{
	//-----------------------------------------------
	// Create FX here so they are always ready to go!
	if (!InEditor)
	{
		if (useNonWeaponEffects && strcmp(weaponEffects->GetEffectName(FX_POOF_ID), "NONE") != 0)
		{
			//--------------------------------------------
			// Yes, load it on up.
			uint32_t flags = gosFX::Effect::ExecuteFlag;
			Check_Object(gosFX::EffectLibrary::Instance);
			gosFX::Effect::Specification* gosEffectSpec =
				gosFX::EffectLibrary::Instance->Find(weaponEffects->GetEffectName(FX_POOF_ID));
			if (gosEffectSpec)
			{
				for (size_t i = 0; i < MAX_DUST_POOFS; i++)
				{
					rightDustPoofEffect[i] = gosFX::EffectLibrary::Instance->MakeEffect(
						gosEffectSpec->m_effectID, flags);
					gosASSERT(rightDustPoofEffect[i] != nullptr);
				}
				MidLevelRenderer::MLRTexturePool::Instance->LoadImages();
			}
		}
		if (useNonWeaponEffects && strcmp(weaponEffects->GetEffectName(FX_POOF_ID), "NONE") != 0)
		{
			//--------------------------------------------
			// Yes, load it on up.
			uint32_t flags = gosFX::Effect::ExecuteFlag;
			Check_Object(gosFX::EffectLibrary::Instance);
			gosFX::Effect::Specification* gosEffectSpec =
				gosFX::EffectLibrary::Instance->Find(weaponEffects->GetEffectName(FX_POOF_ID));
			if (gosEffectSpec)
			{
				for (size_t i = 0; i < MAX_DUST_POOFS; i++)
				{
					leftDustPoofEffect[i] = gosFX::EffectLibrary::Instance->MakeEffect(
						gosEffectSpec->m_effectID, flags);
					gosASSERT(leftDustPoofEffect[i] != nullptr);
				}
				MidLevelRenderer::MLRTexturePool::Instance->LoadImages();
			}
		}
		if (useNonWeaponEffects && strcmp(weaponEffects->GetEffectName(FX_SMOKE_ID), "NONE") != 0)
		{
			//--------------------------------------------
			// Yes, load it on up.
			uint32_t flags = gosFX::Effect::ExecuteFlag | gosFX::Effect::LoopFlag;
			Check_Object(gosFX::EffectLibrary::Instance);
			gosFX::Effect::Specification* gosEffectSpec =
				gosFX::EffectLibrary::Instance->Find(weaponEffects->GetEffectName(FX_SMOKE_ID));
			if (gosEffectSpec)
			{
				smokeEffect =
					gosFX::EffectLibrary::Instance->MakeEffect(gosEffectSpec->m_effectID, flags);
				gosASSERT(smokeEffect != nullptr);
				MidLevelRenderer::MLRTexturePool::Instance->LoadImages();
			}
		}
		if (strcmp(weaponEffects->GetEffectName(FX_JUMP_ID), "NONE") != 0)
		{
			//--------------------------------------------
			// Yes, load it on up.
			uint32_t flags = gosFX::Effect::ExecuteFlag | gosFX::Effect::LoopFlag;
			Check_Object(gosFX::EffectLibrary::Instance);
			gosFX::Effect::Specification* gosEffectSpec =
				gosFX::EffectLibrary::Instance->Find(weaponEffects->GetEffectName(FX_JUMP_ID));
			if (gosEffectSpec)
			{
				jumpJetEffect =
					gosFX::EffectLibrary::Instance->MakeEffect(gosEffectSpec->m_effectID, flags);
				gosASSERT(jumpJetEffect != nullptr);
				MidLevelRenderer::MLRTexturePool::Instance->LoadImages();
			}
		}
	}
}

//------------------------------------------------------------------------------------------
// Puts mech into hit mode IF and ONLY IF the mech is standing, walking,
// reversing, running or limping.
// NO OTHER GESTURE IS
// VALID!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
void
Mech3DAppearance::hitFront(void)
{
	if ((currentGestureId == GestureStand) || (currentGestureId == GestureWalk) || (currentGestureId == GestureReverse) || (currentGestureId == GestureRun) || (currentGestureId == GestureLimpLeft) || (currentGestureId == GestureLimpRight))
	{
		hitGestureId = currentGestureId;
		currentGestureId = GestureHitFront;
		int32_t firstFrame = mechType->gestures[currentGestureId].frameStart;
		atTransitionToNextGesture = false;
		inReverse = false;
		currentFrame = firstFrame;
	}
}

//-----------------------------------------------------------------------------
void
Mech3DAppearance::hitBack(void)
{
	if ((currentGestureId == GestureStand) || (currentGestureId == GestureWalk) || (currentGestureId == GestureReverse) || (currentGestureId == GestureRun) || (currentGestureId == GestureLimpLeft) || (currentGestureId == GestureLimpRight))
	{
		hitGestureId = currentGestureId;
		currentGestureId = GestureHitBack;
		int32_t firstFrame = mechType->gestures[currentGestureId].frameStart;
		atTransitionToNextGesture = false;
		inReverse = false;
		currentFrame = firstFrame;
	}
}

//-----------------------------------------------------------------------------
void
Mech3DAppearance::hitLeft(void)
{
	if ((currentGestureId == GestureStand) || (currentGestureId == GestureWalk) || (currentGestureId == GestureReverse) || (currentGestureId == GestureRun) || (currentGestureId == GestureLimpLeft) || (currentGestureId == GestureLimpRight))
	{
		hitGestureId = currentGestureId;
		currentGestureId = GestureHitLeft;
		int32_t firstFrame = mechType->gestures[currentGestureId].frameStart;
		atTransitionToNextGesture = false;
		inReverse = false;
		currentFrame = firstFrame;
	}
}

//-----------------------------------------------------------------------------
void
Mech3DAppearance::hitRight(void)
{
	if ((currentGestureId == GestureStand) || (currentGestureId == GestureWalk) || (currentGestureId == GestureReverse) || (currentGestureId == GestureRun) || (currentGestureId == GestureLimpLeft) || (currentGestureId == GestureLimpRight))
	{
		hitGestureId = currentGestureId;
		currentGestureId = GestureHitRight;
		int32_t firstFrame = mechType->gestures[currentGestureId].frameStart;
		atTransitionToNextGesture = false;
		inReverse = false;
		currentFrame = firstFrame;
	}
}

//-----------------------------------------------------------------------------
bool
Mech3DAppearance::PerPolySelect(int32_t mouseX, int32_t mouseY)
{
	return mechShape->PerPolySelect(mouseX, mouseY);
}

//-----------------------------------------------------------------------------
void
Mech3DAppearance::LoadPaintSchemata(void)
{
	FullPathFileName paintName;
	paintName.init(tglPath, "paintSchemata", ".fit");
	FitIniFile paintFile;
	int32_t result = paintFile.open(paintName);
	gosASSERT(result == NO_ERROR);
	result = paintFile.seekBlock("Main");
	gosASSERT(result == NO_ERROR);
	result = paintFile.readIdULong("NumPaintSchemes", numPaintSchemata);
	gosASSERT(result == NO_ERROR);
	paintSchemata = (PaintSchemataPtr)AppearanceTypeList::appearanceHeap->Malloc(
		numPaintSchemata * sizeof(PaintSchemata));
	gosASSERT(paintSchemata != nullptr);
	memset(paintSchemata, 0xff, numPaintSchemata * sizeof(PaintSchemata));
	for (size_t i = 0; i < numPaintSchemata; i++)
	{
		char blockId[512];
		sprintf(blockId, "Scheme%d", i);
		result = paintFile.seekBlock(blockId);
		gosASSERT(result == NO_ERROR);
		result = paintFile.readIdULong("RedColor", paintSchemata[i].redColor);
		gosASSERT(result == NO_ERROR);
		result = paintFile.readIdULong("GreenColor", paintSchemata[i].greenColor);
		gosASSERT(result == NO_ERROR);
		result = paintFile.readIdULong("BlueColor", paintSchemata[i].blueColor);
		gosASSERT(result == NO_ERROR);
	}
	paintFile.close();
}

//-----------------------------------------------------------------------------
void
Mech3DAppearance::setPaintScheme(void)
{
	//----------------------------------------------------------------------------
	// Simple really.  Get the texture memory, apply the paint scheme, let it
	// go!
	uint32_t gosHandle = mcTextureManager->get_gosTextureHandle(mechShape->GetTextureHandle(0));
	if (gosHandle && gosHandle != 0xffffffff)
	{
		//-------------------
		// Lock the texture.
		TEXTUREPTR textureData;
		gos_LockTexture(gosHandle, 0, 0, &textureData);
		//-------------------------------------------------------
		uint32_t* textureMemory = textureData.pTexture;
		for (size_t i = 0; i < textureData.Height; i++)
		{
			for (size_t j = 0; j < textureData.Height; j++)
			{
				//---------------------------------------------
				// Make Color from PaintScheme.
				uint32_t baseColor = *textureMemory;
				float baseColorRed = float((baseColor & 0x00ff0000) >> 16);
				float baseColorGreen = float((baseColor & 0x0000ff00) >> 8);
				float baseColorBlue = float(baseColor & 0x000000ff);
				uint32_t newColor = *textureMemory; // Black by default.
				if ((!baseColorGreen) && (!baseColorBlue))
				{
					baseColorRed *= 0.00390625f; // Divide by 256;
					baseColorRed = 1.0 - baseColorRed;
					baseColorRed *= baseColorRed; // Log colors
					baseColorRed = 1.0 - baseColorRed;
					float newColorRed = float((psRed & 0x00ff0000) >> 16);
					newColorRed *= baseColorRed;
					uint8_t red = (uint8_t)newColorRed;
					float newColorGreen = float((psRed & 0x0000ff00) >> 8);
					newColorGreen *= baseColorRed;
					uint8_t green = (uint8_t)newColorGreen;
					float newColorBlue = float(psRed & 0x000000ff);
					newColorBlue *= baseColorRed;
					uint8_t blue = (uint8_t)newColorBlue;
					newColor = (0xff << 24) + (red << 16) + (green << 8) + (blue);
				}
				else if ((!baseColorRed) && (!baseColorBlue))
				{
					baseColorGreen *= 0.00390625f; // Divide by 256;
					baseColorGreen = 1.0 - baseColorGreen;
					baseColorGreen *= baseColorGreen; // Log colors
					baseColorGreen = 1.0 - baseColorGreen;
					float newColorRed = float((psGreen & 0x00ff0000) >> 16);
					newColorRed *= baseColorGreen;
					uint8_t red = (uint8_t)newColorRed;
					float newColorGreen = float((psGreen & 0x0000ff00) >> 8);
					newColorGreen *= baseColorGreen;
					uint8_t green = (uint8_t)newColorGreen;
					float newColorBlue = float(psGreen & 0x000000ff);
					newColorBlue *= baseColorGreen;
					uint8_t blue = (uint8_t)newColorBlue;
					newColor = (0xff << 24) + (red << 16) + (green << 8) + (blue);
				}
				else if ((!baseColorRed) && (!baseColorGreen))
				{
					baseColorBlue *= 0.00390625f; // Divide by 256;
					baseColorBlue = 1.0 - baseColorBlue;
					baseColorBlue *= baseColorBlue; // Log colors
					baseColorBlue = 1.0 - baseColorBlue;
					float newColorRed = float((psBlue & 0x00ff0000) >> 16);
					newColorRed *= baseColorBlue;
					uint8_t red = (uint8_t)newColorRed;
					float newColorGreen = float((psBlue & 0x0000ff00) >> 8);
					newColorGreen *= baseColorBlue;
					uint8_t green = (uint8_t)newColorGreen;
					float newColorBlue = float(psBlue & 0x000000ff);
					newColorBlue *= baseColorBlue;
					uint8_t blue = (uint8_t)newColorBlue;
					newColor = (0xff << 24) + (red << 16) + (green << 8) + (blue);
				}
				*textureMemory = newColor;
				textureMemory++;
			}
		}
		//------------------------
		// Unlock the texture
		gos_UnLockTexture(gosHandle);
	}
}

//---------------------------------------------------------------------------
uint32_t
bgrTorgb(uint32_t frontRGB)
{
	uint32_t tmp;
	tmp = (((0x00ff0000) & frontRGB) >> 16) + ((0x0000ff00) & frontRGB) + (((0x000000ff) & frontRGB) << 16);
	return (tmp);
}

//-----------------------------------------------------------------------------
void
Mech3DAppearance::setPaintScheme(uint32_t mcRed, uint32_t mcGreen, uint32_t mcBlue)
{
#ifdef BGR
	// These come into here bgr instead of RGB.  CONVERT!
	psRed = bgrTorgb(mcRed);
	psBlue = bgrTorgb(mcBlue);
	psGreen = bgrTorgb(mcGreen);
#else /*BGR*/
	psRed = mcRed;
	psBlue = mcBlue;
	psGreen = mcGreen;
#endif /*BGR*/
	setPaintScheme();
}

//-----------------------------------------------------------------------------
void
Mech3DAppearance::getPaintScheme(uint32_t& red, uint32_t& green, uint32_t& blue)
{
#ifdef BGR
	red = bgrTorgb(psRed);
	blue = bgrTorgb(psBlue);
	green = bgrTorgb(psGreen);
#else /*BGR*/
	red = psRed;
	blue = psBlue;
	green = psGreen;
#endif /*BGR*/
}

//-----------------------------------------------------------------------------
void
Mech3DAppearance::resetPaintScheme(uint32_t red, uint32_t green, uint32_t blue)
{
	//---------------------------------------------------------------------------------
	// Simple really.  Toss the current texture, reload the RGB and reapply the
	// colors
	uint32_t gosHandle = mcTextureManager->get_gosTextureHandle(localTextureHandle);
	mcTextureManager->removeTexture(gosHandle);
	//-------------------------------------------------
	// Load the texture and store its handle.
	char txmName[1024];
	mechShape->GetTextureName(0, txmName, 256);
	char texturePath[1024];
	sprintf(texturePath, "%s%d\\", tglPath, ObjectTextureSize);
	FullPathFileName textureName;
	textureName.init(texturePath, txmName, "");
	// uint32_t paintInstance = (red << 16) + (green << 8) + (blue);
	/* The texture manager asks for a unique 32bit identifier for every texture
	instance. However, it requires 72 bits to fully describe a mech texture (the
	base color (stored in the variable "red"), highlight color1 (blue), and
	highlight color2 (green), each of which is a 24bit number made up of 8bit r,
	g, and b components). Instead of creating a mapping between the mech
	textures used (probably less than 2^32 of them) and 32bit identifiers, for
	the sake of expediency I'm just taking the 3 most significant bits of the 9
	rgb components to make a 27 bit identifier. This means that two mech
	textures that are close in color (i.e. all of the 3 most significant bits of
	the 9 rgb components are the same) will be treated as the same texture,
	which is not necessarily a bad thing in our case.  LOD is never needed
	because if the texture is different, its NAME will be different!!*/
	uint32_t ccbase = ((red >> 5) & 7) + (((red >> 13) & 7) << 3) + (((red >> 21) & 7) << 6);
	uint32_t cchighlight1 =
		((green >> 5) & 7) + (((green >> 13) & 7) << 3) + (((green >> 21) & 7) << 6);
	uint32_t cchighlight2 =
		((blue >> 5) & 7) + (((blue >> 13) & 7) << 3) + (((blue >> 21) & 7) << 6);
	uint32_t paintInstance = (ccbase << 18) + (cchighlight1 << 9) + (cchighlight2);
	if (fileExists(textureName))
	{
		uint32_t textureInstanceAlreadyExists = mcTextureManager->textureInstanceExists(textureName,
			gos_Texture_Solid, gosHint_DisableMipmap | gosHint_DontShrink, paintInstance);
		if (!textureInstanceAlreadyExists)
			localTextureHandle = mcTextureManager->loadTexture(textureName, gos_Texture_Solid,
				gosHint_DisableMipmap | gosHint_DontShrink, paintInstance);
		else
			localTextureHandle = textureInstanceAlreadyExists;
		mechShape->SetTextureHandle(0, localTextureHandle);
		mechShape->SetTextureAlpha(0, false);
		if (leftArm)
		{
			leftArm->SetTextureHandle(0, localTextureHandle);
			leftArm->SetTextureAlpha(0, false);
		}
		if (rightArm)
		{
			rightArm->SetTextureHandle(0, localTextureHandle);
			rightArm->SetTextureAlpha(0, false);
		}
		if (textureInstanceAlreadyExists)
		{
			/* In this case, the texture returned should already have the paint
			scheme applied. */
			// Still need to store psRed/psGreen/psBlue!!!!
			psRed = red;
			psGreen = green;
			psBlue = blue;
			return;
		}
	}
	else
	{
		// PAUSE(("Warning: %s texture name not found",textureName));
		mechShape->SetTextureHandle(0, 0xffffffff);
		if (leftArm)
			leftArm->SetTextureHandle(0, 0xffffffff);
		if (rightArm)
			rightArm->SetTextureHandle(0, 0xffffffff);
	}
	setPaintScheme(red, green, blue);
}

//-----------------------------------------------------------------------------
void
Mech3DAppearance::setObjectParameters(
	Stuff::Vector3D& pos, float Rot, int32_t sel, int32_t team, int32_t homeRelations)
{
	movedThisFrame = false;
	if ((rotation != Rot) || (pos != position))
		movedThisFrame = true;
	if ((currentGestureId == 23) || (currentGestureId == 24) || (currentGestureId == 21) || (currentGestureId == 22) || (currentGestureId == 14) || (currentGestureId == 15))
	{
	}
	else
	{
		rotation = Rot;
		if (rotation > 180)
			rotation -= 360;
		if (rotation < -180)
			rotation += 360;
	}
	position = pos;
	selected = sel;
	teamId = team;
	homeTeamRelationship = homeRelations;
}

//-----------------------------------------------------------------------------
void
Mech3DAppearance::setMoverParameters(
	float turretRot, float lArmRot, float rArmRot, bool isAirborne)
{
	isHelicopter = isAirborne;
	if ((currentGestureId == 23) || (currentGestureId == 24) || (currentGestureId == 21) || (currentGestureId == 22) || (currentGestureId == 14) || (currentGestureId == 15))
	{
	}
	else
	{
		torsoRotation = turretRot;
		leftArmRotation = lArmRot;
		rightArmRotation = rArmRot;
		if (torsoRotation > 180)
			torsoRotation -= 360;
		if (torsoRotation < -180)
			torsoRotation += 360;
	}
}

//-----------------------------------------------------------------------------
void
Mech3DAppearance::updateFootprints(void)
{
}

//-----------------------------------------------------------------------------
void
Mech3DAppearance::debugUpdate(int32_t whichOne)
{
	if (!inDebugMoveMode)
		return;
	//----------------------------------------
	// Adjust mechDebugAngle based on Input
	if (userInput->getKeyDown(KEY_LEFT) && userInput->ctrl())
	{
		mechDebugAngle[whichOne] += 11.25;
		if (mechDebugAngle[whichOne] > 180.0)
			mechDebugAngle[whichOne] -= 360;
	}
	if (userInput->getKeyDown(KEY_RIGHT) && userInput->ctrl())
	{
		mechDebugAngle[whichOne] -= 11.25;
		if (mechDebugAngle[whichOne] < -180.0)
			mechDebugAngle[whichOne] += 360.0;
	}
	//----------------------------------------
	// Adjust torsoDebugAngle based on Input
	if (userInput->getKeyDown(KEY_UP) && userInput->ctrl())
	{
		torsoDebugAngle[whichOne] += 11.25;
		if (torsoDebugAngle[whichOne] > 180.0)
			torsoDebugAngle[whichOne] -= 360;
	}
	if (userInput->getKeyDown(KEY_DOWN) && userInput->ctrl())
	{
		torsoDebugAngle[whichOne] -= 11.25;
		if (torsoDebugAngle[whichOne] < -180.0)
			torsoDebugAngle[whichOne] += 360.0;
	}
	//----------------------------------------
	// Adjust GestureGoal based on Input
	if (userInput->getKeyDown(KEY_0) && userInput->ctrl())
	{
		setGestureGoal(0); // PARK
	}
	if (userInput->getKeyDown(KEY_1) && userInput->ctrl())
	{
		setGestureGoal(1); // STAND
	}
	if (userInput->getKeyDown(KEY_2) && userInput->ctrl())
	{
		setGestureGoal(2); // WALK
	}
	if (userInput->getKeyDown(KEY_3) && userInput->ctrl())
	{
		setGestureGoal(3); // RUN
	}
	if (userInput->getKeyDown(KEY_4) && userInput->ctrl())
	{
		setGestureGoal(4); // REVERSE
	}
	if (userInput->getKeyDown(KEY_5) && userInput->ctrl())
	{
		setGestureGoal(5); // LIMP
	}
	if (userInput->getKeyDown(KEY_6) && userInput->ctrl())
	{
		setGestureGoal(6); // JUMP
	}
	if (userInput->getKeyDown(KEY_7) && userInput->ctrl())
	{
		setGestureGoal(7); // FALL FORWARD
	}
	if (userInput->getKeyDown(KEY_8) && userInput->ctrl())
	{
		setGestureGoal(8); // FALL BACKWARD
	}
	//------------------------------------------------------------------
	// Adjust position based on mech Velocity which is based on gesture
	Stuff::Vector3D velocity;
	velocity.x = 0.7071f;
	velocity.z = 0.0;
	velocity.y = -0.7071f;
	Rotate(velocity, -mechDebugAngle[whichOne]);
	float velMag = getVelocityMagnitude();
	//-----------------------------------------
	// Take slope being walked on into account.
	// Use for ground vehicles for sure.
	/*
	Stuff::Vector3D currentNormal =
	land->getTerrainNormal(debugMechActorPosition); float angle =
	angle_from(velocity,currentNormal); if (angle != 90.0)
	{
		float hillFactor = cos(angle * DEGREES_TO_RADS) * velMag;
		velMag += hillFactor;
	}
	*/
	velocity *= velMag * worldUnitsPerMeter;
	velocity *= frameLength;
	debugMechActorPosition[whichOne] += velocity;
	debugMechActorPosition[whichOne].z =
		land->getTerrainElevation(debugMechActorPosition[whichOne]);
	setObjectParameters(debugMechActorPosition[whichOne], mechDebugAngle[whichOne], true, 0, 0);
	update();
	recalcBounds();
	updateFootprints();
}

//-----------------------------------------------------------------------------
bool
Mech3DAppearance::isMouseOver(float px, float py)
{
	if (inView)
	{
		if ((px <= lowerRight.x) && (py <= lowerRight.y) && (px >= upperLeft.x) && (py >= upperLeft.y))
		{
			return inView;
		}
		else
		{
			return false;
		}
	}
	return (inView);
}

//-----------------------------------------------------------------------------
bool
Mech3DAppearance::recalcBounds(void)
{
	Stuff::Vector4D tempPos;
	bool wasInView = inView;
	inView = false;
	float eyeDistance = 0.0f;
	if (eye)
	{
		// ALWAYS need to do this or select is YAYA
		eye->projectZ(position, screenPos);
		//--------------------------------------------------
		// First, if we are using perspective, figure out
		// if object too far from camera.  Far Clip Plane.
		if (eye->usePerspective)
		{
			Stuff::Point3D Distance;
			Stuff::Point3D objPosition;
			Stuff::Point3D eyePosition(eye->getCameraOrigin());
			objPosition.x = -position.x;
			objPosition.y = position.z;
			objPosition.z = position.y;
			Distance.Subtract(objPosition, eyePosition);
			eyeDistance = Distance.GetApproximateLength();
			if (eyeDistance > Camera::MaxClipDistance)
			{
				hazeFactor = 1.0f;
				inView = false;
			}
			else if (eyeDistance > Camera::MinHazeDistance)
			{
				Camera::HazeFactor =
					(eyeDistance - Camera::MinHazeDistance) * Camera::DistanceFactor;
				inView = true;
			}
			else
			{
				Camera::HazeFactor = 0.0f;
				inView = true;
			}
			//-----------------------------------------------------------------
			// If inside farClip plane, check if behind camera.
			// Find angle between lookVector of Camera and vector from camPos
			// to Target.  If angle is less then halfFOV, object is visible.
			if (inView)
			{
				Stuff::Vector3D Distance;
				Stuff::Point3D objPosition;
				Stuff::Point3D eyePosition(eye->getCameraOrigin());
				objPosition.x = -position.x;
				objPosition.y = position.z;
				objPosition.z = position.y;
				Distance.Subtract(objPosition, eyePosition);
				Distance.Normalize(Distance);
				float cosine = Distance * eye->getLookVector();
				if (cosine > eye->cosHalfFOV)
					inView = true;
				else
					inView = false;
			}
		}
		else
		{
			Camera::HazeFactor = 0.0f;
			inView = true;
		}
		if (inView)
		{
			if (reloadBounds)
				mechType->reinit();
			// mechType->boundsLowerRightY = (OBBRadius * eye->getTiltFactor()
			// * 2.0f);
			//-------------------------------------------------------------------------
			// do a rough check if on screen.  If no where near, do NOT do the
			// below. Mighty mighty slow!!!! Use the original check done before
			// all this 3D madness.  Dig out sourceSafe tomorrow!
			tempPos = screenPos;
			upperLeft.x = tempPos.x;
			upperLeft.y = tempPos.y;
			lowerRight.x = tempPos.x;
			lowerRight.y = tempPos.y;
			upperLeft.x += (mechType->boundsUpperLeftX * eye->getScaleFactor());
			upperLeft.y += (mechType->boundsUpperLeftY * eye->getScaleFactor());
			lowerRight.x += (mechType->boundsLowerRightX * eye->getScaleFactor());
			lowerRight.y += (mechType->boundsLowerRightY * eye->getScaleFactor());
			if ((lowerRight.x >= 0) && (lowerRight.y >= 0) && (upperLeft.x <= eye->getScreenResX()) && (upperLeft.y <= eye->getScreenResY()))
			{
				// We are on screen.  Figure out selection box.
				Stuff::Vector3D boxCoords[8];
				Stuff::Vector4D bcsp[8];
				if (rootNodeIndex == -1)
					rootNodeIndex = mechShape->GetNodeNameId("joint_root");
				// Gotta let the NodeIdPosition know that its matrix is valid or
				// invalid so this actually does clip mech to screen!!
				// Leave the old one in place until we are inView again!
				// Should fix flickering on screen edge?  It does.  Must let
				// jump flicker or he never comes down.
				inView = wasInView;
				if (inView || (currentGestureId == 20))
					baseRootNodeDifference =
						(getNodeIdPosition(rootNodeIndex).z - position.z) - baseRootNodeHeight;
				if (inView && isHelicopter)
					baseRootNodeDifference -= HELICOPTER_FACTOR;
				inView = true;
				if (InEditor)
				{
					if (isHelicopter)
						baseRootNodeDifference = 0.0f - HELICOPTER_FACTOR;
					else
						baseRootNodeDifference = 0.0f;
				}
				boxCoords[0].x = position.x + mechType->typeUpperLeft.x;
				boxCoords[0].y = position.y + mechType->typeUpperLeft.y;
				boxCoords[0].z = position.z + mechType->typeUpperLeft.z + baseRootNodeDifference;
				boxCoords[1].x = position.x + mechType->typeUpperLeft.x;
				boxCoords[1].y = position.y + mechType->typeLowerRight.y;
				boxCoords[1].z = position.z + mechType->typeUpperLeft.z + baseRootNodeDifference;
				boxCoords[2].x = position.x + mechType->typeLowerRight.x;
				boxCoords[2].y = position.y + mechType->typeUpperLeft.y;
				boxCoords[2].z = position.z + mechType->typeUpperLeft.z + baseRootNodeDifference;
				boxCoords[3].x = position.x + mechType->typeLowerRight.x;
				boxCoords[3].y = position.y + mechType->typeLowerRight.y;
				boxCoords[3].z = position.z + mechType->typeUpperLeft.z + baseRootNodeDifference;
				boxCoords[4].x = position.x + mechType->typeLowerRight.x;
				boxCoords[4].y = position.y + mechType->typeLowerRight.y;
				boxCoords[4].z = position.z + mechType->typeLowerRight.z + baseRootNodeDifference;
				boxCoords[5].x = position.x + mechType->typeLowerRight.x;
				boxCoords[5].y = position.y + mechType->typeUpperLeft.y;
				boxCoords[5].z = position.z + mechType->typeLowerRight.z + baseRootNodeDifference;
				boxCoords[6].x = position.x + mechType->typeUpperLeft.x;
				boxCoords[6].y = position.y + mechType->typeLowerRight.y;
				boxCoords[6].z = position.z + mechType->typeLowerRight.z + baseRootNodeDifference;
				boxCoords[7].x = position.x + mechType->typeUpperLeft.x;
				boxCoords[7].y = position.y + mechType->typeUpperLeft.y;
				boxCoords[7].z = position.z + mechType->typeLowerRight.z + baseRootNodeDifference;
				float maxX = 0.0f, maxY = 0.0f;
				float minX = 0.0f, minY = 0.0f;
				for (size_t i = 0; i < 8; i++)
				{
					eye->projectZ(boxCoords[i], bcsp[i]);
					if (!i)
					{
						maxX = minX = bcsp[i].x;
						maxY = minY = bcsp[i].y;
					}
					if (i)
					{
						if (bcsp[i].x > maxX)
							maxX = bcsp[i].x;
						if (bcsp[i].x < minX)
							minX = bcsp[i].x;
						if (bcsp[i].y > maxY)
							maxY = bcsp[i].y;
						if (bcsp[i].y < minY)
							minY = bcsp[i].y;
					}
				}
				upperLeft.x = minX;
				upperLeft.y = minY;
				lowerRight.x = maxX;
				lowerRight.y = maxY;
				if ((lowerRight.x >= 0) && (lowerRight.y >= 0) && (upperLeft.x <= eye->getScreenResX()) && (upperLeft.y <= eye->getScreenResY()))
				{
					inView = true;
					if (status != OBJECT_STATUS_DESTROYED)
					{
						bool baseLOD = true;
						uint32_t selectLOD = 0;
						if (useHighObjectDetail)
						{
							//-------------------------------------------------------------------------------
							// Set LOD of Model here because we have the
							// distance and we KNOW we can see it!
							for (size_t i = 1; i < MAX_LODS; i++)
							{
								if (mechType->mechShape[i] && mechType->mechShape[i]->GetNumShapes() && (eyeDistance > mechType->lodDistance[i]))
								{
									baseLOD = false;
									selectLOD = i;
								}
							}
						}
						else
						{
							if (mechType->mechShape[1] && mechType->mechShape[1]->GetNumShapes())
							{
								baseLOD = false;
								selectLOD = 1;
							}
						}
						// we are at this LOD level.
						if (selectLOD != (uint32_t)currentLOD)
						{
							currentLOD = selectLOD;
							uint8_t alphaValue = mechShape->GetAlphaValue();
							// mechShape->ClearAnimation();	//DO NOT do this
							// with animating things!!
							delete mechShape;
							mechShape = nullptr;
							mechShape = mechType->mechShape[currentLOD]->CreateFrom();
							mechShape->SetAlphaValue(alphaValue);
							resetPaintScheme(psRed, psGreen, psBlue);
							if (rightArmOff)
								mechShape->StopUsing("joint_ruarm");
							if (leftArmOff)
								mechShape->StopUsing("joint_luarm");
						}
						if (currentLOD && baseLOD)
						{
							// we are at the Base LOD level.
							currentLOD = 0;
							uint8_t alphaValue = mechShape->GetAlphaValue();
							// treeShape->ClearAnimation();
							delete mechShape;
							mechShape = nullptr;
							mechShape = mechType->mechShape[currentLOD]->CreateFrom();
							mechShape->SetAlphaValue(alphaValue);
							// Automatically reloads texture and stores it.
							resetPaintScheme(psRed, psGreen, psBlue);
							if (rightArmOff)
								mechShape->StopUsing("joint_ruarm");
							if (leftArmOff)
								mechShape->StopUsing("joint_luarm");
						}
						mechType->setAnimation(mechShape, currentGestureId);
					}
				}
				else
				{
					inView = false; // Did alot of extra work checking this, but
						// WHY draw and insult to injury?
				}
			}
			else
			{
				inView = false;
			}
		}
	}
	return (inView);
}

//-----------------------------------------------------------------------------
int32_t
Mech3DAppearance::render(int32_t depthFixup)
{
	// Force textures to reload due to unique instance.
	mechShape->SetTextureHandle(0, localTextureHandle);
	if (rightArm)
		rightArm->SetTextureHandle(0, localTextureHandle);
	if (leftArm)
		leftArm->SetTextureHandle(0, localTextureHandle);
	if (inView)
	{
		if (visible)
		{
			int32_t color = SD_BLUE;
			uint32_t highLight = 0x007f7f7f;
			if ((teamId > -1) && (teamId < 8))
			{
				static uint32_t highLightTable[3] = {0x00007f00, 0x0000007f, 0x007f0000};
				static int32_t colorTable[3] = {
					SB_GREEN | 0xff000000, SB_BLUE | 0xff000000, SB_RED | 0xff000000};
				color = colorTable[homeTeamRelationship];
				highLight = highLightTable[homeTeamRelationship];
			}
			if (selected & DRAW_COLORED && duration <= 0)
			{
				mechShape->SetARGBHighLight(highLight);
			}
			else
			{
				mechShape->SetARGBHighLight(highlightColor);
			}
			Camera::HazeFactor = hazeFactor;
			if (drawFlash)
			{
				mechShape->SetARGBHighLight(flashColor);
			}
			//---------------------------------------------
			// Call Multi-shape render stuff here.
			mechShape->Render(true);
			if (selected & DRAW_BARS)
			{
				drawBars();
			}
			if (selected & DRAW_BRACKETS)
			{
				drawSelectBrackets(color);
			}
			if ((objectNameId != -1) && (selected & DRAW_TEXT) && !sensorLevel)
			{
				drawBars();
				if (strlen(mechName))
				{
					drawTextHelp(mechName, color);
				}
				else
				{
					char tmpString[255];
					cLoadString(objectNameId, tmpString, 254);
					drawTextHelp(tmpString, color);
				}
				if (strlen(pilotName))
				{
					drawPilotName(pilotName, color);
				}
				else if (pilotNameID != -1) // only draw your own
				{
					char tmpPilotName[255];
					cLoadString(pilotNameID, tmpPilotName, 254);
					drawPilotName(tmpPilotName, color);
				}
			}
			//------------------------------------------------
			// Render GOSFX
			if (!InEditor)
			{
				gosFX::Effect::DrawInfo drawInfo;
				drawInfo.m_clipper = theClipper;
				MidLevelRenderer::MLRState mlrState;
				mlrState.SetDitherOn();
				mlrState.SetTextureCorrectionOn();
				mlrState.SetZBufferCompareOn();
				mlrState.SetZBufferWriteOn();
				drawInfo.m_state = mlrState;
				drawInfo.m_clippingFlags = 0x0;
				if (!isWaking && useNonWeaponEffects)
				{
					for (size_t i = 0; i < MAX_DUST_POOFS; i++)
					{
						if (rightFootPoofDraw[i] && rightDustPoofEffect[i] && rightDustPoofEffect[i]->IsExecuted())
						{
							Stuff::LinearMatrix4D shapeOrigin;
							Stuff::LinearMatrix4D localToWorld;
							shapeOrigin.BuildRotation(Stuff::EulerAngles(0.0f, 0.0f, 0.0f));
							shapeOrigin.BuildTranslation(rFootPosition[i]);
							drawInfo.m_parentToWorld = &shapeOrigin;
							if (!MLRVertexLimitReached)
								rightDustPoofEffect[i]->Draw(&drawInfo);
						}
						if (leftFootPoofDraw[i] && leftDustPoofEffect[i] && leftDustPoofEffect[i]->IsExecuted())
						{
							Stuff::LinearMatrix4D shapeOrigin;
							Stuff::LinearMatrix4D localToWorld;
							shapeOrigin.BuildRotation(Stuff::EulerAngles(0.0f, 0.0f, 0.0f));
							shapeOrigin.BuildTranslation(lFootPosition[i]);
							drawInfo.m_parentToWorld = &shapeOrigin;
							if (!MLRVertexLimitReached)
								leftDustPoofEffect[i]->Draw(&drawInfo);
						}
					}
				}
				if ((currentGestureId == GestureJump) && inJump && jumpJetEffect && jumpJetEffect->IsExecuted())
				{
					int32_t jumpNodeId = mechType->numSmokeNodes + mechType->numWeaponNodes;
					Stuff::Vector3D jumpNodePos = getNodePosition(jumpNodeId);
					Stuff::Point3D actualPosition;
					Stuff::LinearMatrix4D shapeOrigin;
					Stuff::LinearMatrix4D localToWorld;
					Stuff::LinearMatrix4D localResult;
					actualPosition.x = -jumpNodePos.x;
					actualPosition.y = jumpNodePos.z;
					actualPosition.z = jumpNodePos.y;
					shapeOrigin.BuildRotation(Stuff::EulerAngles(0.0f, 0.0f, 0.0f));
					shapeOrigin.BuildTranslation(actualPosition);
					Stuff::UnitQuaternion effectRot;
					effectRot = Stuff::EulerAngles(
						JUMP_PITCH * DEGREES_TO_RADS, rotation * DEGREES_TO_RADS, 0.0f);
					localToWorld.Multiply(gosFX::Effect_Against_Motion, effectRot);
					localResult.Multiply(localToWorld, shapeOrigin);
					drawInfo.m_parentToWorld = &localResult;
					if (!MLRVertexLimitReached)
						jumpJetEffect->Draw(&drawInfo);
				}
				//----------------------------------------------------------------
				// Arm Blown FX
				if (leftShoulderBoom)
				{
					Stuff::LinearMatrix4D shapeOrigin;
					Stuff::LinearMatrix4D localToWorld;
					Stuff::LinearMatrix4D localResult;
					if (leftArmNodeIndex == -1)
						leftArmNodeIndex = mechShape->GetNodeNameId("joint_luarm");
					Stuff::Vector3D leftNodePos = getNodeIdPosition(leftArmNodeIndex);
					leftShoulderPos.x = -leftNodePos.x;
					leftShoulderPos.y = leftNodePos.z;
					leftShoulderPos.z = leftNodePos.y;
					shapeOrigin.BuildRotation(Stuff::EulerAngles(0.0f, 0.0f, 0.0f));
					shapeOrigin.BuildTranslation(leftShoulderPos);
					Stuff::UnitQuaternion effectRot;
					effectRot = Stuff::EulerAngles(0.0f, rotation * DEGREES_TO_RADS, 0.0f);
					localToWorld.Multiply(gosFX::Effect_Against_Motion, effectRot);
					localResult.Multiply(localToWorld, shapeOrigin);
					drawInfo.m_parentToWorld = &localResult;
					if (!MLRVertexLimitReached)
						leftShoulderBoom->Draw(&drawInfo);
				}
				if (rightShoulderBoom)
				{
					Stuff::LinearMatrix4D shapeOrigin;
					Stuff::LinearMatrix4D localToWorld;
					Stuff::LinearMatrix4D localResult;
					if (rightArmNodeIndex == -1)
						rightArmNodeIndex = mechShape->GetNodeNameId("joint_ruarm");
					Stuff::Vector3D rightNodePos = getNodeIdPosition(rightArmNodeIndex);
					rightShoulderPos.x = -rightNodePos.x;
					rightShoulderPos.y = rightNodePos.z;
					rightShoulderPos.z = rightNodePos.y;
					shapeOrigin.BuildRotation(Stuff::EulerAngles(0.0f, 0.0f, 0.0f));
					shapeOrigin.BuildTranslation(rightShoulderPos);
					Stuff::UnitQuaternion effectRot;
					effectRot = Stuff::EulerAngles(0.0f, rotation * DEGREES_TO_RADS, 0.0f);
					localToWorld.Multiply(gosFX::Effect_Against_Motion, effectRot);
					localResult.Multiply(localToWorld, shapeOrigin);
					drawInfo.m_parentToWorld = &localResult;
					if (!MLRVertexLimitReached)
						rightShoulderBoom->Draw(&drawInfo);
				}
				//------------------------------------------------
				// All other FXs
				if (isSmoking != -1)
				{
					Stuff::LinearMatrix4D shapeOrigin;
					Stuff::LinearMatrix4D localToWorld;
					Stuff::Vector3D smokeNodePos = getNodePosition(0);
					Stuff::Point3D smokePos;
					smokePos.x = -smokeNodePos.x;
					smokePos.y = smokeNodePos.z;
					smokePos.z = smokeNodePos.y;
					shapeOrigin.BuildRotation(Stuff::EulerAngles(0.0f, 0.0f, 0.0f));
					shapeOrigin.BuildTranslation(smokePos);
					drawInfo.m_parentToWorld = &shapeOrigin;
					if (isSmoking > 0)
					{
						// Loops until told to stop.
						if (!MLRVertexLimitReached)
							criticalSmoke->Draw(&drawInfo);
					}
					else
					{
						if (!MLRVertexLimitReached)
							smokeEffect->Draw(&drawInfo);
					}
				}
				if (waterWake && isWaking && (currentGestureId != 20)) // Do NOT draw wake if we are
					// jumping.  keep calcing it,
					// though!
				{
					Stuff::LinearMatrix4D shapeOrigin;
					Stuff::LinearMatrix4D localToWorld;
					Stuff::LinearMatrix4D localResult;
					Stuff::Point3D wakePos;
					wakePos.x = -position.x;
					wakePos.y = Terrain::waterElevation;
					wakePos.z = position.y;
					shapeOrigin.BuildRotation(Stuff::EulerAngles(0.0f, 0.0f, 0.0f));
					shapeOrigin.BuildTranslation(wakePos);
					Stuff::UnitQuaternion effectRot;
					effectRot = Stuff::EulerAngles(
						90.0f * DEGREES_TO_RADS, rotation * DEGREES_TO_RADS, 0.0f);
					if (!inReverse)
						localToWorld.Multiply(gosFX::Effect_Against_Motion, effectRot);
					else
						localToWorld.Multiply(gosFX::Effect_Into_Motion, effectRot);
					localResult.Multiply(localToWorld, shapeOrigin);
					drawInfo.m_parentToWorld = &localResult;
					if (!MLRVertexLimitReached)
						waterWake->Draw(&drawInfo);
				}
				if (isDusting && helicopterDustCloud)
				{
					Stuff::LinearMatrix4D shapeOrigin;
					Stuff::LinearMatrix4D localToWorld;
					Stuff::LinearMatrix4D localResult;
					Stuff::Point3D wakePos;
					wakePos.x = -position.x;
					wakePos.y = position.z;
					wakePos.z = position.y;
					shapeOrigin.BuildRotation(Stuff::EulerAngles(0.0f, 0.0f, 0.0f));
					shapeOrigin.BuildTranslation(wakePos);
					/*
					Stuff::UnitQuaternion effectRot;
					effectRot = Stuff::EulerAngles(0.0f,rotation *
					DEGREES_TO_RADS,0.0f);
					localToWorld.Multiply(gosFX::Effect_Against_Motion,effectRot);
					localResult.Multiply(localToWorld,shapeOrigin);
					*/
					drawInfo.m_parentToWorld = &shapeOrigin;
					if (!MLRVertexLimitReached)
						helicopterDustCloud->Draw(&drawInfo);
				}
			}
#ifdef DRAW_BOX
			//---------------------------------------------------------
			// Render the Bounding Box to see if it is OK.
			Stuff::Vector3D nodeCenter = mechShape->GetRootNodeCenter();
			Stuff::Vector3D boxCoords[8];
			boxCoords[0].x = position.x + mechType->typeUpperLeft.x;
			boxCoords[0].y = position.y + mechType->typeUpperLeft.y;
			boxCoords[0].z = position.z + mechType->typeUpperLeft.z;
			boxCoords[1].x = position.x + mechType->typeUpperLeft.x;
			boxCoords[1].y = position.y + mechType->typeLowerRight.y;
			boxCoords[1].z = position.z + mechType->typeUpperLeft.z;
			boxCoords[2].x = position.x + mechType->typeLowerRight.x;
			boxCoords[2].y = position.y + mechType->typeUpperLeft.y;
			boxCoords[2].z = position.z + mechType->typeUpperLeft.z;
			boxCoords[3].x = position.x + mechType->typeLowerRight.x;
			boxCoords[3].y = position.y + mechType->typeLowerRight.y;
			boxCoords[3].z = position.z + mechType->typeUpperLeft.z;
			boxCoords[4].x = position.x + mechType->typeLowerRight.x;
			boxCoords[4].y = position.y + mechType->typeLowerRight.y;
			boxCoords[4].z = position.z + mechType->typeLowerRight.z;
			boxCoords[5].x = position.x + mechType->typeLowerRight.x;
			boxCoords[5].y = position.y + mechType->typeUpperLeft.y;
			boxCoords[5].z = position.z + mechType->typeLowerRight.z;
			boxCoords[6].x = position.x + mechType->typeUpperLeft.x;
			boxCoords[6].y = position.y + mechType->typeLowerRight.y;
			boxCoords[6].z = position.z + mechType->typeLowerRight.z;
			boxCoords[7].x = position.x + mechType->typeUpperLeft.x;
			boxCoords[7].y = position.y + mechType->typeUpperLeft.y;
			boxCoords[7].z = position.z + mechType->typeLowerRight.z;
			Stuff::Vector4D screenPos[8];
			for (size_t i = 0; i < 8; i++)
			{
				eye->projectZ(boxCoords[i], screenPos[i]);
			}
			{
				LineElement newElement(screenPos[0], screenPos[1], XP_WHITE, nullptr, -1);
				newElement.draw();
			}
			{
				LineElement newElement(screenPos[0], screenPos[4], XP_WHITE, nullptr, -1);
				newElement.draw();
			}
			{
				LineElement newElement(screenPos[0], screenPos[3], XP_WHITE, nullptr, -1);
				newElement.draw();
			}
			{
				LineElement newElement(screenPos[5], screenPos[4], XP_WHITE, nullptr, -1);
				newElement.draw();
			}
			{
				LineElement newElement(screenPos[5], screenPos[6], XP_WHITE, nullptr, -1);
				newElement.draw();
			}
			{
				LineElement newElement(screenPos[5], screenPos[3], XP_WHITE, nullptr, -1);
				newElement.draw();
			}
			{
				LineElement newElement(screenPos[2], screenPos[3], XP_WHITE, nullptr, -1);
				newElement.draw();
			}
			{
				LineElement newElement(screenPos[2], screenPos[6], XP_WHITE, nullptr, -1);
				newElement.draw();
			}
			{
				LineElement newElement(screenPos[2], screenPos[1], XP_WHITE, nullptr, -1);
				newElement.draw();
			}
			{
				LineElement newElement(screenPos[7], screenPos[1], XP_WHITE, nullptr, -1);
				newElement.draw();
			}
			{
				LineElement newElement(screenPos[7], screenPos[6], XP_WHITE, nullptr, -1);
				newElement.draw();
			}
			{
				LineElement newElement(screenPos[7], screenPos[4], XP_WHITE, nullptr, -1);
				newElement.draw();
			}
#endif
		}
		if ((sensorLevel > 0) && (sensorLevel < 5))
		{
			//---------------------------------------
			// Draw Sensor Contact here.
			if (sensorLevel > 1)
			{
				sensorSquareShape->Render();
			}
			else
			{
				sensorTriangleShape->Render();
			}
		}
	}
	if (rightArmOff && rightArm && rightArmInView)
	{
		Camera::HazeFactor = rightArmHazeFactor;
		rightArm->Render(true);
		if (rightArmSmoke)
		{
			gosFX::Effect::DrawInfo drawInfo;
			drawInfo.m_clipper = theClipper;
			MidLevelRenderer::MLRState mlrState;
			mlrState.SetDitherOn();
			mlrState.SetTextureCorrectionOn();
			mlrState.SetZBufferCompareOn();
			mlrState.SetZBufferWriteOn();
			drawInfo.m_state = mlrState;
			drawInfo.m_clippingFlags = 0x0;
			Stuff::LinearMatrix4D shapeOrigin;
			Stuff::LinearMatrix4D localToWorld;
			Stuff::LinearMatrix4D localResult;
			rightShoulderPos.x = -rightArmPos.x;
			rightShoulderPos.y = rightArmPos.z;
			rightShoulderPos.z = rightArmPos.y;
			shapeOrigin.BuildRotation(Stuff::EulerAngles(0.0f, 0.0f, 0.0f));
			shapeOrigin.BuildTranslation(rightShoulderPos);
			Stuff::UnitQuaternion effectRot;
			effectRot = Stuff::EulerAngles(0.0f, rotation * DEGREES_TO_RADS, 0.0f);
			localToWorld.Multiply(gosFX::Effect_Against_Motion, effectRot);
			localResult.Multiply(localToWorld, shapeOrigin);
			drawInfo.m_parentToWorld = &localResult;
			if (!MLRVertexLimitReached)
				rightArmSmoke->Draw(&drawInfo);
		}
	}
	if (leftArmOff && leftArm && leftArmInView)
	{
		Camera::HazeFactor = leftArmHazeFactor;
		leftArm->Render(true);
		if (leftArmSmoke)
		{
			gosFX::Effect::DrawInfo drawInfo;
			drawInfo.m_clipper = theClipper;
			MidLevelRenderer::MLRState mlrState;
			mlrState.SetDitherOn();
			mlrState.SetTextureCorrectionOn();
			mlrState.SetZBufferCompareOn();
			mlrState.SetZBufferWriteOn();
			drawInfo.m_state = mlrState;
			drawInfo.m_clippingFlags = 0x0;
			Stuff::LinearMatrix4D shapeOrigin;
			Stuff::LinearMatrix4D localToWorld;
			Stuff::LinearMatrix4D localResult;
			leftShoulderPos.x = -leftArmPos.x;
			leftShoulderPos.y = leftArmPos.z;
			leftShoulderPos.z = leftArmPos.y;
			shapeOrigin.BuildRotation(Stuff::EulerAngles(0.0f, 0.0f, 0.0f));
			shapeOrigin.BuildTranslation(leftShoulderPos);
			Stuff::UnitQuaternion effectRot;
			effectRot = Stuff::EulerAngles(0.0f, rotation * DEGREES_TO_RADS, 0.0f);
			localToWorld.Multiply(gosFX::Effect_Against_Motion, effectRot);
			localResult.Multiply(localToWorld, shapeOrigin);
			drawInfo.m_parentToWorld = &localResult;
			if (!MLRVertexLimitReached)
				leftArmSmoke->Draw(&drawInfo);
		}
	}
	return NO_ERROR;
}

//-----------------------------------------------------------------------------
int32_t
Mech3DAppearance::renderShadows(void)
{
	mechShape->SetTextureHandle(0, localTextureHandle);
	if (inView && visible)
	{
		//---------------------------------------------
		// Call Multi-shape render stuff here.
		// DONT RENDER UNTIL FINAL MECH DATA FROM MR CHOI
		//---------------------------------------------
		// Call Multi-shape render stuff here.
		if (mechShadowShape)
			mechShadowShape->RenderShadows(true);
		else
			mechShape->RenderShadows(true);
	}
	return (NO_ERROR);
}

bool oneMechPlease = false;
#ifdef LAB_ONLY
int64_t MCTimeAnimationCalc = 0;
int64_t x;
#endif

//-----------------------------------------------------------------------------
void
Mech3DAppearance::setObjStatus(int32_t oStatus)
{
	if ((status != oStatus) && (!InEditor))
	{
		if (oStatus == OBJECT_STATUS_DESTROYED)
		{
			if (currentGestureId == 23)
			{
				if (mechType->mechForwardDmgShape)
				{
					mechShape->ClearAnimation();
					delete mechShape;
					mechShape = nullptr;
					mechShape = mechType->mechForwardDmgShape->CreateFrom();
				}
			}
			else if (currentGestureId == 24)
			{
				if (mechType->mechBackwardDmgShape)
				{
					mechShape->ClearAnimation();
					delete mechShape;
					mechShape = nullptr;
					mechShape = mechType->mechBackwardDmgShape->CreateFrom();
				}
			}
			currentLOD = 0;
			//-------------------------------------------------
			// Load the texture and store its handle.
			for (size_t i = 0; i < mechShape->GetNumTextures(); i++)
			{
				char txmName[1024];
				mechShape->GetTextureName(i, txmName, 256);
				char texturePath[1024];
				sprintf(texturePath, "%s%d\\", tglPath, ObjectTextureSize);
				FullPathFileName textureName;
				textureName.init(texturePath, txmName, "");
				if (fileExists(textureName))
				{
					if (strnicmp(txmName, "a_", 2) == 0)
					{
						uint32_t gosTextureHandle = 0;
						if (!i)
						{
							localTextureHandle =
								mcTextureManager->loadTexture(textureName, gos_Texture_Alpha,
									gosHint_DisableMipmap | gosHint_DontShrink, true);
							mechShape->SetTextureHandle(i, localTextureHandle);
							mechShape->SetTextureAlpha(i, true);
						}
						else
						{
							gosTextureHandle = mcTextureManager->loadTexture(textureName,
								gos_Texture_Alpha, gosHint_DisableMipmap | gosHint_DontShrink);
							mechShape->SetTextureHandle(i, gosTextureHandle);
							mechShape->SetTextureAlpha(i, true);
						}
					}
					else
					{
						uint32_t gosTextureHandle = 0;
						if (!i)
						{
							localTextureHandle =
								mcTextureManager->loadTexture(textureName, gos_Texture_Solid,
									gosHint_DisableMipmap | gosHint_DontShrink, true);
							mechShape->SetTextureHandle(i, localTextureHandle);
							mechShape->SetTextureAlpha(i, false);
						}
						else
						{
							gosTextureHandle = mcTextureManager->loadTexture(textureName,
								gos_Texture_Solid, gosHint_DisableMipmap | gosHint_DontShrink);
							mechShape->SetTextureHandle(i, gosTextureHandle);
							mechShape->SetTextureAlpha(i, false);
						}
					}
				}
				else
				{
					// PAUSE(("Warning: %s texture name not
					// found",textureName));
					mechShape->SetTextureHandle(i, 0xffffffff);
				}
			}
		}
		if (status == OBJECT_STATUS_DESTROYED && oStatus == OBJECT_STATUS_NORMAL)
		{
			STOP(("Destroyed Mech just came back to life!!"));
		}
	}
	status = oStatus;
}

//-----------------------------------------------------------------------------
void
Mech3DAppearance::updateGeometry(void)
{
#ifdef LAB_ONLY
	x = GetCycles();
#endif
	// Always override with our local instance.
	mechShape->SetTextureHandle(0, localTextureHandle);
	if (rightArm)
		rightArm->SetTextureHandle(0, localTextureHandle);
	if (leftArm)
		leftArm->SetTextureHandle(0, localTextureHandle);
	if ((status == OBJECT_STATUS_DESTROYED) || (status == OBJECT_STATUS_DISABLED) || (status == OBJECT_STATUS_SHUTDOWN))
	{
		mechShape->SetLightsOut(true);
	}
	else
	{
		mechShape->SetLightsOut(false);
	}
	Stuff::Point3D xlatPosition;
	xlatPosition.x = -position.x;
	xlatPosition.y = position.z;
	xlatPosition.z = position.y;
	// Update flashing regardless of view!!!
	if (duration > 0.0f)
	{
		duration -= frameLength;
		currentFlash -= frameLength;
		if (currentFlash < 0.0f)
		{
			drawFlash ^= true;
			currentFlash = flashDuration;
		}
	}
	else
	{
		drawFlash = false;
	}
	// if (visible)
	{
		//-------------------------------------------
		// Does math necessary to draw mech.
		Stuff::UnitQuaternion qRotation;
		float yaw = rotation * DEGREES_TO_RADS;
		qRotation = Stuff::EulerAngles(0.0f, yaw, 0.0f);
		uint8_t lightr, lightg, lightb;
		float lightIntensity = 1.0f;
		if (land)
			lightIntensity = land->getTerrainLight(position);
		lightr = eye->getLightRed(lightIntensity);
		lightg = eye->getLightGreen(lightIntensity);
		lightb = eye->getLightBlue(lightIntensity);
		uint32_t lightRGB = (lightr << 16) + (lightg << 8) + lightb;
		eye->setLightColor(0, lightRGB);
		eye->setLightIntensity(0, 1.0);
		uint32_t fogRGB = 0xff << 24;
		float fogStart = eye->fogStart;
		float fogFull = eye->fogFull;
		if (useFog && (xlatPosition.y < fogStart))
		{
			float fogFactor = fogStart - xlatPosition.y;
			if (fogFactor < 0.0)
				fogRGB = 0xff << 24;
			else
			{
				fogFactor /= (fogStart - fogFull);
				if (fogFactor <= 1.0)
				{
					fogFactor *= fogFactor;
					fogFactor = 1.0 - fogFactor;
					fogFactor *= 256.0;
				}
				else
				{
					fogFactor = 256.0;
				}
				uint8_t fogResult = fogFactor;
				fogRGB = fogResult << 24;
			}
		}
		else
		{
			fogRGB = 0xff << 24;
		}
		if (useFog)
			mechShape->SetFogRGB(fogRGB);
		else
			mechShape->SetFogRGB(0xffffffff);
		//-----------------------------------
		// Test of Mech as Point Light Source
		if (!pointLight && eye->isNight)
		{
			if (lightCircleNodeIndex == -1)
				lightCircleNodeIndex = mechShape->GetNodeNameId("SLCircle_anubis");
			Stuff::Vector3D lightPos = getNodeIdPosition(lightCircleNodeIndex);
			if (lightPos != position)
			{
				pointLight = (TG_LightPtr)malloc(sizeof(TG_Light));
				pointLight->init(TG_LIGHT_SPOT);
				lightId = eye->addWorldLight(pointLight);
				pointLight->SetaRGB(0xffffff00);
				pointLight->SetIntensity(0.15f);
				pointLight->SetFalloffDistances(50.0f, 250.0f);
			}
		}
		if (pointLight)
		{
			if (visible && (sensorLevel > 4) && !InEditor)
			{
				if (lightCircleNodeIndex == -1)
					lightCircleNodeIndex = mechShape->GetNodeNameId("SLCircle_anubis");
				Stuff::Vector3D lightPos = getNodeIdPosition(lightCircleNodeIndex);
				Stuff::Point3D ourPosition;
				ourPosition.x = -lightPos.x;
				ourPosition.y = lightPos.z;
				ourPosition.z = lightPos.y;
				pointLight->direction = ourPosition;
				pointLight->spotDir.x = -position.x;
				pointLight->spotDir.y = position.z;
				pointLight->spotDir.z = position.y;
				pointLight->maxSpotLength = 50.0f;
				Stuff::LinearMatrix4D lightToWorldMatrix;
				lightToWorldMatrix.BuildTranslation(ourPosition);
				lightToWorldMatrix.BuildRotation(Stuff::EulerAngles(0.0f, 0.0f, 0.0f));
				pointLight->SetLightToWorld(&lightToWorldMatrix);
				pointLight->SetPosition(&lightPos);
				pointLight->active = true;
			}
			else
			{
				pointLight->active = false;
			}
		}
		//----------------------------------------------------
		// Set Animation State Here.
		// ONLY ONE case now:
		//		-We are in an art generated state,  Just playback animation
		// All cases covered now.
		if (status != OBJECT_STATUS_DESTROYED)
		{
			mechType->setAnimation(mechShape, currentGestureId);
			mechShape->SetFrameNum(currentFrame);
			if (mechShadowShape)
			{
				mechType->setAnimation(mechShadowShape, currentGestureId);
				mechShadowShape->SetFrameNum(currentFrame);
			}
		}
		Stuff::UnitQuaternion torsoRot;
		torsoRot = Stuff::EulerAngles(0.0f, (torsoRotation * DEGREES_TO_RADS), 0.0f);
		if (rotationalNodeIndex == -1)
			rotationalNodeIndex = mechShape->SetNodeRotation("joint_torso", &torsoRot);
		mechShape->SetNodeRotation(rotationalNodeIndex, &torsoRot);
		if (mechShadowShape)
			mechShape->SetUseShadow(false);
		if (mechShadowShape && useShadows)
		{
			if (rotationalNodeIndex == -1)
				rotationalNodeIndex = mechShadowShape->SetNodeRotation("joint_torso", &torsoRot);
			mechShadowShape->SetNodeRotation(rotationalNodeIndex, &torsoRot);
			mechShadowShape->SetNodeRotation("joint_torso", &torsoRot);
			mechShadowShape->SetLightList(eye->getWorldLights(), eye->getNumLights());
			mechShadowShape->TransformMultiShape(&xlatPosition, &qRotation);
		}
		mechShape->SetLightList(eye->getWorldLights(), eye->getNumLights());
		mechShape->TransformMultiShape(&xlatPosition, &qRotation);
	}
	if (visible && (sensorLevel > 4) && !InEditor && useNonWeaponEffects)
	{
		//--------------------------------------------------------------
		// Having already transformed the mech, the foot poofs go here.
		Stuff::Vector3D rFootPos, lFootPos;
		int32_t footId1 = mechType->getTotalNodes() - 2;
		int32_t footId0 = mechType->getTotalNodes() - 1;
		rFootPos = getNodePosition(footId1);
		lFootPos = getNodePosition(footId0);
		if (!rightFootDone0 && (currentFrame >= (mechType->gestures[currentGestureId].rightFootDownFrame0 - FOOTPRINT_SLOP)) && (currentFrame <= (mechType->gestures[currentGestureId].rightFootDownFrame0 + FOOTPRINT_SLOP)))
		{
			// Foot is on ground.  Poof and footprint.
			rightFootDone0 = true;
			if (rightDustPoofEffect[currentRightPoof] && (currentGestureId != 13))
			{
				Stuff::Point3D actualPosition;
				Stuff::LinearMatrix4D shapeOrigin;
				Stuff::LinearMatrix4D localToWorld;
				rFootPosition[currentRightPoof].x = -rFootPos.x;
				rFootPosition[currentRightPoof].y = rFootPos.z;
				rFootPosition[currentRightPoof].z = rFootPos.y;
				shapeOrigin.BuildRotation(Stuff::EulerAngles(0.0f, 0.0f, 0.0f));
				shapeOrigin.BuildTranslation(rFootPosition[currentRightPoof]);
				gosFX::Effect::ExecuteInfo info((Stuff::Time)scenarioTime, &shapeOrigin, nullptr);
				rightDustPoofEffect[currentRightPoof]->Start(&info);
				rightFootPoofDraw[currentRightPoof] = true;
				currentRightPoof++;
				if (currentRightPoof >= MAX_DUST_POOFS)
					currentRightPoof = 0;
			}
			// Draw footprint here.
			if (craterManager && !isWaking)
				craterManager->addCrater(mechType->rightFootprintType, rFootPos, rotation);
		}
		if (!rightFootDone1 && (currentFrame >= (mechType->gestures[currentGestureId].rightFootDownFrame1 - FOOTPRINT_SLOP)) && (currentFrame <= (mechType->gestures[currentGestureId].rightFootDownFrame1 + FOOTPRINT_SLOP)))
		{
			// Foot is on ground.  Poof and footprint.
			rightFootDone1 = true;
			if (rightDustPoofEffect[currentRightPoof] && (currentGestureId != 13))
			{
				Stuff::Point3D actualPosition;
				Stuff::LinearMatrix4D shapeOrigin;
				Stuff::LinearMatrix4D localToWorld;
				rFootPosition[currentRightPoof].x = -rFootPos.x;
				rFootPosition[currentRightPoof].y = rFootPos.z;
				rFootPosition[currentRightPoof].z = rFootPos.y;
				shapeOrigin.BuildRotation(Stuff::EulerAngles(0.0f, 0.0f, 0.0f));
				shapeOrigin.BuildTranslation(rFootPosition[currentRightPoof]);
				gosFX::Effect::ExecuteInfo info((Stuff::Time)scenarioTime, &shapeOrigin, nullptr);
				rightDustPoofEffect[currentRightPoof]->Start(&info);
				rightFootPoofDraw[currentRightPoof] = true;
				currentRightPoof++;
				if (currentRightPoof >= MAX_DUST_POOFS)
					currentRightPoof = 0;
			}
			// Draw footprint here.
			if (craterManager && !isWaking)
				craterManager->addCrater(mechType->rightFootprintType, rFootPos, rotation);
		}
		if (rightFootDone0 && (currentFrame < (mechType->gestures[currentGestureId].rightFootDownFrame0 - FOOTPRINT_SLOP)) || (currentFrame > (mechType->gestures[currentGestureId].rightFootDownFrame0 + FOOTPRINT_SLOP)))
		{
			rightFootDone0 = false;
		}
		if (rightFootDone1 && (currentFrame < (mechType->gestures[currentGestureId].rightFootDownFrame1 - FOOTPRINT_SLOP)) || (currentFrame > (mechType->gestures[currentGestureId].rightFootDownFrame1 + FOOTPRINT_SLOP)))
		{
			rightFootDone1 = false;
		}
		if (!leftFootDone0 && (currentFrame >= (mechType->gestures[currentGestureId].leftFootDownFrame0 - FOOTPRINT_SLOP)) && (currentFrame <= (mechType->gestures[currentGestureId].leftFootDownFrame0 + FOOTPRINT_SLOP)))
		{
			// Foot is on ground.  Poof and footprint.
			leftFootDone0 = true;
			if (leftDustPoofEffect[currentLeftPoof] && (currentGestureId != 13))
			{
				Stuff::Point3D actualPosition;
				Stuff::LinearMatrix4D shapeOrigin;
				Stuff::LinearMatrix4D localToWorld;
				lFootPosition[currentLeftPoof].x = -lFootPos.x;
				lFootPosition[currentLeftPoof].y = lFootPos.z;
				lFootPosition[currentLeftPoof].z = lFootPos.y;
				shapeOrigin.BuildRotation(Stuff::EulerAngles(0.0f, 0.0f, 0.0f));
				shapeOrigin.BuildTranslation(lFootPosition[currentLeftPoof]);
				gosFX::Effect::ExecuteInfo info((Stuff::Time)scenarioTime, &shapeOrigin, nullptr);
				leftDustPoofEffect[currentLeftPoof]->Start(&info);
				leftFootPoofDraw[currentLeftPoof] = true;
				currentLeftPoof++;
				if (currentLeftPoof >= MAX_DUST_POOFS)
					currentLeftPoof = 0;
			}
			// Draw footprint here.
			if (craterManager && !isWaking)
				craterManager->addCrater(mechType->leftFootprintType, lFootPos, rotation);
		}
		if (!leftFootDone1 && (currentFrame >= (mechType->gestures[currentGestureId].leftFootDownFrame1 - FOOTPRINT_SLOP)) && (currentFrame <= (mechType->gestures[currentGestureId].leftFootDownFrame1 + FOOTPRINT_SLOP)))
		{
			// Foot is on ground.  Poof and footprint.
			leftFootDone1 = true;
			if (leftDustPoofEffect[currentLeftPoof] && (currentGestureId != 13))
			{
				Stuff::Point3D actualPosition;
				Stuff::LinearMatrix4D shapeOrigin;
				Stuff::LinearMatrix4D localToWorld;
				lFootPosition[currentLeftPoof].x = -lFootPos.x;
				lFootPosition[currentLeftPoof].y = lFootPos.z;
				lFootPosition[currentLeftPoof].z = lFootPos.y;
				shapeOrigin.BuildRotation(Stuff::EulerAngles(0.0f, 0.0f, 0.0f));
				shapeOrigin.BuildTranslation(lFootPosition[currentLeftPoof]);
				gosFX::Effect::ExecuteInfo info((Stuff::Time)scenarioTime, &shapeOrigin, nullptr);
				leftDustPoofEffect[currentLeftPoof]->Start(&info);
				leftFootPoofDraw[currentLeftPoof] = true;
				currentLeftPoof++;
				if (currentLeftPoof >= MAX_DUST_POOFS)
					currentLeftPoof = 0;
			}
			// Draw footprint here.
			if (craterManager && !isWaking)
				craterManager->addCrater(mechType->leftFootprintType, lFootPos, rotation);
		}
		if (leftFootDone0 && (currentFrame < (mechType->gestures[currentGestureId].leftFootDownFrame0 - FOOTPRINT_SLOP)) || (currentFrame > (mechType->gestures[currentGestureId].leftFootDownFrame0 + FOOTPRINT_SLOP)))
		{
			leftFootDone0 = false;
		}
		if (leftFootDone1 && (currentFrame < (mechType->gestures[currentGestureId].leftFootDownFrame1 - FOOTPRINT_SLOP)) || (currentFrame > (mechType->gestures[currentGestureId].leftFootDownFrame1 + FOOTPRINT_SLOP)))
		{
			leftFootDone1 = false;
		}
	}
	Stuff::UnitQuaternion totalRotation;
	sensorSpin += SPIN_RATE * frameLength;
	if (sensorSpin > 180)
		sensorSpin -= 360;
	if (sensorSpin < -180)
		sensorSpin += 360;
	totalRotation = Stuff::EulerAngles(0.0f, sensorSpin * DEGREES_TO_RADS, 0.0f);
	float baseRootNodeDifference = baseRootNodeHeight;
	if (isHelicopter)
		baseRootNodeDifference -= HELICOPTER_FACTOR;
	xlatPosition.y += baseRootNodeDifference;
	//----------------------------------------------
	// Do geometry here to draw sensor contact
	sensorTriangleShape->SetFogRGB(0xffffffff);
	sensorTriangleShape->SetLightList(eye->getWorldLights(), eye->getNumLights());
	sensorTriangleShape->TransformMultiShape(&xlatPosition, &totalRotation);
	//----------------------------------------------
	// Do geometry here to draw sensor contact
	sensorSquareShape->SetFogRGB(0xffffffff);
	sensorSquareShape->SetLightList(eye->getWorldLights(), eye->getNumLights());
	sensorSquareShape->TransformMultiShape(&xlatPosition, &totalRotation);
	//-----------------------------------------
	// Create Jump FX Here.
	if (!jumpFXSetup && (currentGestureId == GestureJump) && inJump && jumpJetEffect)
	{
		int32_t jumpNodeId = mechType->numSmokeNodes + mechType->numWeaponNodes;
		Stuff::Vector3D jumpNodePos = getNodePosition(jumpNodeId);
		Stuff::Point3D actualPosition;
		Stuff::LinearMatrix4D shapeOrigin;
		Stuff::LinearMatrix4D localToWorld;
		Stuff::LinearMatrix4D localResult;
		actualPosition.x = -jumpNodePos.x;
		actualPosition.y = jumpNodePos.z;
		actualPosition.z = jumpNodePos.y;
		shapeOrigin.BuildRotation(Stuff::EulerAngles(0.0f, 0.0f, 0.0f));
		shapeOrigin.BuildTranslation(actualPosition);
		Stuff::UnitQuaternion effectRot;
		effectRot =
			Stuff::EulerAngles(JUMP_PITCH * DEGREES_TO_RADS, rotation * DEGREES_TO_RADS, 0.0f);
		localToWorld.Multiply(gosFX::Effect_Against_Motion, effectRot);
		localResult.Multiply(localToWorld, shapeOrigin);
		gosFX::Effect::ExecuteInfo info((Stuff::Time)scenarioTime, &localResult, nullptr);
		jumpJetEffect->Start(&info);
		jumpFXSetup = true;
	}
	//------------------------------------------------
	// Update GOSFX
	for (size_t i = 0; i < MAX_DUST_POOFS; i++)
	{
		if (rightFootPoofDraw[i] && rightDustPoofEffect[i] && rightDustPoofEffect[i]->IsExecuted())
		{
			Stuff::LinearMatrix4D shapeOrigin;
			Stuff::LinearMatrix4D localToWorld;
			shapeOrigin.BuildRotation(Stuff::EulerAngles(0.0f, 0.0f, 0.0f));
			shapeOrigin.BuildTranslation(rFootPosition[i]);
			Stuff::OBB boundingBox;
			gosFX::Effect::ExecuteInfo info((Stuff::Time)scenarioTime, &shapeOrigin, &boundingBox);
			bool result = rightDustPoofEffect[i]->Execute(&info);
			if (!result)
			{
				rightFootPoofDraw[i] = false;
			}
		}
		if (leftFootPoofDraw[i] && leftDustPoofEffect[i] && leftDustPoofEffect[i]->IsExecuted())
		{
			Stuff::LinearMatrix4D shapeOrigin;
			Stuff::LinearMatrix4D localToWorld;
			shapeOrigin.BuildRotation(Stuff::EulerAngles(0.0f, 0.0f, 0.0f));
			shapeOrigin.BuildTranslation(lFootPosition[i]);
			Stuff::OBB boundingBox;
			gosFX::Effect::ExecuteInfo info((Stuff::Time)scenarioTime, &shapeOrigin, &boundingBox);
			bool result = leftDustPoofEffect[i]->Execute(&info);
			if (!result)
			{
				leftFootPoofDraw[i] = false;
			}
		}
	}
	if ((currentGestureId == GestureJump) && inJump && jumpJetEffect && jumpJetEffect->IsExecuted())
	{
		int32_t jumpNodeId = mechType->numSmokeNodes + mechType->numWeaponNodes;
		Stuff::Vector3D jumpNodePos = getNodePosition(jumpNodeId);
		Stuff::Point3D actualPosition;
		Stuff::LinearMatrix4D shapeOrigin;
		Stuff::LinearMatrix4D localToWorld;
		Stuff::LinearMatrix4D localResult;
		actualPosition.x = -jumpNodePos.x;
		actualPosition.y = jumpNodePos.z;
		actualPosition.z = jumpNodePos.y;
		shapeOrigin.BuildRotation(Stuff::EulerAngles(0.0f, 0.0f, 0.0f));
		shapeOrigin.BuildTranslation(actualPosition);
		Stuff::UnitQuaternion effectRot;
		effectRot =
			Stuff::EulerAngles(JUMP_PITCH * DEGREES_TO_RADS, rotation * DEGREES_TO_RADS, 0.0f);
		localToWorld.Multiply(gosFX::Effect_Against_Motion, effectRot);
		localResult.Multiply(localToWorld, shapeOrigin);
		Stuff::OBB boundingBox;
		gosFX::Effect::ExecuteInfo info((Stuff::Time)scenarioTime, &localResult, &boundingBox);
		jumpJetEffect->Execute(&info);
	}
	if (!inJump && jumpFXSetup)
	{
		jumpJetEffect->Stop();
		jumpFXSetup = false;
	}
	//----------------------------------------------------------------
	// Arm Blown FX
	if (leftShoulderBoom)
	{
		Stuff::LinearMatrix4D shapeOrigin;
		Stuff::LinearMatrix4D localToWorld;
		Stuff::LinearMatrix4D localResult;
		if (leftArmNodeIndex == -1)
			leftArmNodeIndex = mechShape->GetNodeNameId("joint_luarm");
		Stuff::Vector3D leftNodePos = getNodeIdPosition(leftArmNodeIndex);
		leftShoulderPos.x = -leftNodePos.x;
		leftShoulderPos.y = leftNodePos.z;
		leftShoulderPos.z = leftNodePos.y;
		shapeOrigin.BuildRotation(Stuff::EulerAngles(0.0f, 0.0f, 0.0f));
		shapeOrigin.BuildTranslation(leftShoulderPos);
		Stuff::UnitQuaternion effectRot;
		effectRot = Stuff::EulerAngles(0.0f, rotation * DEGREES_TO_RADS, 0.0f);
		localToWorld.Multiply(gosFX::Effect_Against_Motion, effectRot);
		localResult.Multiply(localToWorld, shapeOrigin);
		Stuff::OBB boundingBox;
		gosFX::Effect::ExecuteInfo info((Stuff::Time)scenarioTime, &localResult, &boundingBox);
		bool result = leftShoulderBoom->Execute(&info);
		if (!result)
		{
			leftShoulderBoom->Kill();
			delete leftShoulderBoom;
			leftShoulderBoom = nullptr;
		}
	}
	if (rightShoulderBoom)
	{
		Stuff::LinearMatrix4D shapeOrigin;
		Stuff::LinearMatrix4D localToWorld;
		Stuff::LinearMatrix4D localResult;
		if (rightArmNodeIndex == -1)
			rightArmNodeIndex = mechShape->GetNodeNameId("joint_ruarm");
		Stuff::Vector3D rightNodePos = getNodeIdPosition(rightArmNodeIndex);
		rightShoulderPos.x = -rightNodePos.x;
		rightShoulderPos.y = rightNodePos.z;
		rightShoulderPos.z = rightNodePos.y;
		shapeOrigin.BuildRotation(Stuff::EulerAngles(0.0f, 0.0f, 0.0f));
		shapeOrigin.BuildTranslation(rightShoulderPos);
		Stuff::UnitQuaternion effectRot;
		effectRot = Stuff::EulerAngles(0.0f, rotation * DEGREES_TO_RADS, 0.0f);
		localToWorld.Multiply(gosFX::Effect_Against_Motion, effectRot);
		localResult.Multiply(localToWorld, shapeOrigin);
		Stuff::OBB boundingBox;
		gosFX::Effect::ExecuteInfo info((Stuff::Time)scenarioTime, &localResult, &boundingBox);
		bool result = rightShoulderBoom->Execute(&info);
		if (!result)
		{
			rightShoulderBoom->Kill();
			delete rightShoulderBoom;
			rightShoulderBoom = nullptr;
		}
	}
	//-------------------------------------------------
	// All other effects
	if ((criticalSmoke && (isSmoking > 0)) || (smokeEffect && (isSmoking == 0)))
	{
		Stuff::LinearMatrix4D shapeOrigin;
		Stuff::LinearMatrix4D localToWorld;
		Stuff::Vector3D smokeNodePos = getNodePosition(0);
		Stuff::Point3D smokePos;
		smokePos.x = -smokeNodePos.x;
		smokePos.y = smokeNodePos.z;
		smokePos.z = smokeNodePos.y;
		shapeOrigin.BuildRotation(Stuff::EulerAngles(0.0f, 0.0f, 0.0f));
		shapeOrigin.BuildTranslation(smokePos);
		Stuff::OBB boundingBox;
		gosFX::Effect::ExecuteInfo info((Stuff::Time)scenarioTime, &shapeOrigin, &boundingBox);
		if (isSmoking > 0)
		{
			// Loops until told to stop.
			criticalSmoke->Execute(&info);
		}
		else
		{
			smokeEffect->Execute(&info);
		}
	}
	if (waterWake && isWaking)
	{
		if (movedThisFrame)
		{
			waterWake->SetLoopOn();
			waterWake->SetExecuteOn();
		}
		else
		{
			waterWake->SetLoopOff();
			waterWake->SetExecuteOn();
		}
		Stuff::LinearMatrix4D shapeOrigin;
		Stuff::LinearMatrix4D localToWorld;
		Stuff::LinearMatrix4D localResult;
		Stuff::Point3D wakePos;
		wakePos.x = -position.x;
		wakePos.y = Terrain::waterElevation;
		wakePos.z = position.y;
		shapeOrigin.BuildRotation(Stuff::EulerAngles(0.0f, 0.0f, 0.0f));
		shapeOrigin.BuildTranslation(wakePos);
		Stuff::UnitQuaternion effectRot;
		effectRot = Stuff::EulerAngles(90.0f * DEGREES_TO_RADS, rotation * DEGREES_TO_RADS, 0.0f);
		if (!inReverse)
			localToWorld.Multiply(gosFX::Effect_Against_Motion, effectRot);
		else
			localToWorld.Multiply(gosFX::Effect_Into_Motion, effectRot);
		localResult.Multiply(localToWorld, shapeOrigin);
		Stuff::OBB boundingBox;
		gosFX::Effect::ExecuteInfo info((Stuff::Time)scenarioTime, &localResult, &boundingBox);
		waterWake->Execute(&info);
	}
	if (helicopterDustCloud && isDusting)
	{
		Stuff::LinearMatrix4D shapeOrigin;
		Stuff::LinearMatrix4D localToWorld;
		Stuff::LinearMatrix4D localResult;
		Stuff::Point3D wakePos;
		wakePos.x = -position.x;
		wakePos.y = position.z;
		wakePos.z = position.y;
		shapeOrigin.BuildRotation(Stuff::EulerAngles(0.0f, 0.0f, 0.0f));
		shapeOrigin.BuildTranslation(wakePos);
		/*
		Stuff::UnitQuaternion effectRot;
		effectRot = Stuff::EulerAngles(0.0f,rotation * DEGREES_TO_RADS,0.0f);
		localToWorld.Multiply(gosFX::Effect_Against_Motion,effectRot);
		localResult.Multiply(localToWorld,shapeOrigin);
		*/
		Stuff::OBB boundingBox;
		gosFX::Effect::ExecuteInfo info((Stuff::Time)scenarioTime, &shapeOrigin, &boundingBox);
		bool result = helicopterDustCloud->Execute(&info);
		if (!result)
		{
			helicopterDustCloud->Kill();
			isDusting = false;
		}
	}
#ifdef LAB_ONLY
	x = GetCycles() - x;
	MCTimeAnimationCalc += x;
#endif
}

#ifdef _DEBUG
FilePtr logFile = nullptr;
#endif

//-----------------------------------------------------------------------------
bool
Mech3DAppearance::setJumpParameters(Stuff::Vector3D& end)
{
	if (!inJump)
	{
		jumpSetup = true;
		jumpDestination = end;
		return true;
	}
	return false;
}

//-----------------------------------------------------------------------------
bool
Mech3DAppearance::leftArmRecalc(void)
{
	if (!leftArm)
		return false;
	leftArmInView = false;
	float eyeDistance = 0.0f;
	//--------------------------------------------------
	// First, if we are using perspective, figure out
	// if object too far from camera.  Far Clip Plane.
	if (eye->usePerspective)
	{
		Stuff::Point3D Distance;
		Stuff::Point3D objPosition;
		Stuff::Point3D eyePosition(eye->getCameraOrigin());
		objPosition.x = -leftArmPos.x;
		objPosition.y = leftArmPos.z;
		objPosition.z = leftArmPos.y;
		Distance.Subtract(objPosition, eyePosition);
		eyeDistance = Distance.GetApproximateLength();
		if (eyeDistance > Camera::MaxClipDistance)
		{
			leftArmHazeFactor = 1.0f;
			leftArmInView = false;
		}
		else if (eyeDistance > Camera::MinHazeDistance)
		{
			leftArmHazeFactor = (eyeDistance - Camera::MinHazeDistance) * Camera::DistanceFactor;
			leftArmInView = true;
		}
		else
		{
			leftArmHazeFactor = 0.0f;
			leftArmInView = true;
		}
		//-----------------------------------------------------------------
		// If inside farClip plane, check if behind camera.
		// Find angle between lookVector of Camera and vector from camPos
		// to Target.  If angle is less then halfFOV, object is visible.
		if (inView)
		{
			Distance.Normalize(Distance);
			float cosine = Distance * eye->getLookVector();
			if (cosine > eye->cosHalfFOV)
				leftArmInView = true;
			else
				leftArmInView = false;
		}
	}
	return leftArmInView;
}

//-----------------------------------------------------------------------------
bool
Mech3DAppearance::rightArmRecalc(void)
{
	if (!rightArm)
		return false;
	rightArmInView = false;
	float eyeDistance = 0.0f;
	//--------------------------------------------------
	// First, if we are using perspective, figure out
	// if object too far from camera.  Far Clip Plane.
	if (eye->usePerspective)
	{
		Stuff::Point3D Distance;
		Stuff::Point3D objPosition;
		Stuff::Point3D eyePosition(eye->getCameraOrigin());
		objPosition.x = -rightArmPos.x;
		objPosition.y = rightArmPos.z;
		objPosition.z = rightArmPos.y;
		Distance.Subtract(objPosition, eyePosition);
		eyeDistance = Distance.GetApproximateLength();
		if (eyeDistance > Camera::MaxClipDistance)
		{
			rightArmHazeFactor = 1.0f;
			rightArmInView = false;
		}
		else if (eyeDistance > Camera::MinHazeDistance)
		{
			rightArmHazeFactor = (eyeDistance - Camera::MinHazeDistance) * Camera::DistanceFactor;
			rightArmInView = true;
		}
		else
		{
			rightArmHazeFactor = 0.0f;
			rightArmInView = true;
		}
		//-----------------------------------------------------------------
		// If inside farClip plane, check if behind camera.
		// Find angle between lookVector of Camera and vector from camPos
		// to Target.  If angle is less then halfFOV, object is visible.
		if (inView)
		{
			Distance.Normalize(Distance);
			float cosine = Distance * eye->getLookVector();
			if (cosine > eye->cosHalfFOV)
				rightArmInView = true;
			else
				rightArmInView = false;
		}
	}
	return rightArmInView;
}

//-----------------------------------------------------------------------------
int32_t
Mech3DAppearance::update(bool animate)
{
#ifdef _DEBUG
	if (!logFile)
	{
		logFile = new MechFile;
		int32_t result = logFile->create("gesture.log");
		if (result != NO_ERROR)
		{
			delete logFile;
			logFile = nullptr;
		}
	}
#endif
	//----------------------------------------
	// Recycle the weapon Nodes
	if (nodeRecycle)
	{
		for (size_t i = 0; i < mechType->numWeaponNodes; i++)
		{
			if (nodeRecycle[i] > 0.0f)
			{
				nodeRecycle[i] -= frameLength;
				if (nodeRecycle[i] < 0.0f)
					nodeRecycle[i] = 0.0f;
			}
		}
	}
	oncePerFrame = false;
	bool setFirstFrame = false;
	if ((currentGestureId == GestureHitFront) || (currentGestureId == GestureHitBack) || (currentGestureId == GestureHitLeft) || (currentGestureId == GestureHitRight))
	{
		if (atTransitionToNextGesture)
		{
			currentGestureId = hitGestureId;
			hitGestureId = -1;
			setFirstFrame = true;
			int32_t firstFrame = mechType->gestures[currentGestureId].frameStart;
			if (firstFrame < 0) // Start at end of animation.  Probably to
				// reverse.  Like StandToPark
				firstFrame = mechType->getNumFrames(currentGestureId) - 1;
			inReverse = false;
			if (mechType->getFrameRate(currentGestureId) < 0.0)
				inReverse = true;
			atTransitionToNextGesture = false;
			currentFrame = firstFrame;
		}
	}
	//----------------------------------------------
	// If mech is in 2 and does NOT have a target.
	// Fix please
	if (!inCombatMode && (currentGestureId == 2))
	{
		idleTime += frameLength;
		if (idleTime > idleMAX)
		{
			currentGestureId = 13;
		}
	}
	else
	{
		idleTime = 0.0f;
	}
	if ((currentGestureId == 13) && atTransitionToNextGesture)
	{
		currentGestureId = 2;
		currentFrame = 0;
	}
	//--------------------------------------------------------------
	// Check to see if we need to stop NOW!!!!!!!!!
	if (checkStop() && !inDebugMoveMode && !jumpSetup && (getVelocityMagnitude() != 0.0f) && (currentGestureId != GestureJump))
	{
		//--------------------------------
		// We are at the goal.  Store off
		// our current gesture state.
		currentGestureId = 2; // Force us to the stand gesture
		oldStateGoal = 1; // We always came FROM STAND
		currentStateGoal = -1; // Not going anywhere
		transitionState = 0; // No Transition necessary.  So be ready at
			// beginning if we need one!
		currentFrame = 0; // In the "how did this ever work file"  Duh, need to
			// reset the frame!
	}
	int32_t newGestureId = -1; // Assume we do not need to transition.
	//--------------------------------------------------------------
	// Check if currentStateGoal is -1.
	// If it is not, we need to move to a new gesture to get to our goal.
	// Check if we can change gestures.  If yes, go through change over.
	// Combines three separate old chunks o code.
	if (currentStateGoal != -1)
	{
		//--------------------------------------------
		// Find out how we get to Goal.  If result is
		// -1, we are already at goal.
		int32_t arrayIdx = (oldStateGoal * GESTURE_OFFSET_SIZE) + (currentStateGoal * MAX_TRANSITION_GESTURES) + transitionState;
		newGestureId = transitionArray[arrayIdx];
		if (atTransitionToNextGesture) // This is only set when we are ready for
			// the next gesture if these is one!
		{
			transitionState++; // Ready to go to next state.  Increment the
				// transition counter for next frame!
			int32_t firstFrame = currentFrame;
			if (newGestureId == -1) // We have reached our goal.
			{
				//--------------------------------
				// We are at the goal.  Store off
				// our current gesture state.
				oldStateGoal = currentStateGoal;
				if (oldStateGoal == MECH_STATE_JUMPING)
					oldStateGoal = 1; // We are always standing AFTER a jump or a fall!
				currentStateGoal = -1;
				transitionState = 0;
				atTransitionToNextGesture = false;
				if (mechType->getFrameRate(currentGestureId) < 0.0)
					inReverse = true;
			}
			else
			//----------------------------------------------------
			// reset all frames to zero for this gesture.
			// NOTE:  This may be a two way gesture.  Most will be
			// set to zero, but some may need to be set to last.
			//
			{
				//----------------------------------------------
				// Now actually reset everything and setup flags
				// to facilitate the playing of the new gesture
				currentGestureId = newGestureId;
				setFirstFrame = true;
				firstFrame = mechType->gestures[currentGestureId].frameStart;
				if (currentGestureId == GestureJump)
					firstFrame = 0; // We store information in the firstFrame of
						// the jump.  They always start at 0!
				if (firstFrame < 0) // Start at end of animation.  Probably to
					// reverse.  Like StandToPark
					firstFrame = mechType->getNumFrames(currentGestureId) - 1;
				inReverse = false;
				if (mechType->getFrameRate(currentGestureId) < 0.0)
					inReverse = true;
				atTransitionToNextGesture = false;
			}
			currentFrame = firstFrame;
		}
	}
	if ((status == OBJECT_STATUS_DESTROYED) || (status == OBJECT_STATUS_DISABLED))
	{
		//----------------------------------------------------
		// This will bypass animation if we go the old way.
		// Just set stateGoal to 7 or 8 if its not.
		if (currentStateGoal < 7)
		{
			if (RollDice(50))
			{
				setGestureGoal(7);
			}
			else
			{
				setGestureGoal(8);
			}
		}
	}
	//------------------------------------------------------------
	mechFrameRate = mechType->getFrameRate(currentGestureId);
	if (mechFrameRate < 0.0)
		mechFrameRate = -mechFrameRate;
	//---------------------
	// Do jump magic here.
	if ((currentGestureId == GestureJump) && !inJump)
	{
		inJump = true;
		jumpSetup = false;
		//-----------------------------------------------------
		// Distance is straight line distance.  This is only
		// the case if the mech is FACING the correct way.  If
		// not, we must increase distance based on facing.
		float takeoffFrame = mechType->getJumpTakeoffFrame();
		float landingFrame = mechType->getJumpLandFrame();
		//-----------------------------------------
		// Get mech flying toward targetPosition.
		Stuff::Vector3D ownerPosition = position;
		Stuff::Vector3D targetPosition = jumpDestination;
		jumpVelocity.Subtract(targetPosition, ownerPosition);
		float velMag = jumpVelocity.GetLength() / ((landingFrame - takeoffFrame) / 30.0f);
		jumpVelocity.Normalize(jumpVelocity);
		jumpVelocity *= velMag;
		//---------------------------------------------
		// Jump Velocity is now a velocity vector which
		// will move the mech WITHOUT turning to the landing point.
	}
	if (currentGestureId == GestureJump && inJump)
	{
		if (currentFrame >= mechType->getJumpTakeoffFrame())
		{
			jumpAirborne = true;
		}
		if ((currentFrame >= mechType->getJumpLandFrame()) && jumpAirborne)
		{
			jumpAirborne = false;
			jumpVelocity.Zero();
			if (jumpJetEffect)
			{
				jumpJetEffect->Stop(); // Done drawing it.
			}
		}
	}
	//--------------------------------------------------------
	// Force frames to last frame of FALL if fallen
	fallen = false;
	if ((currentGestureId == GestureFallenForward) || (currentGestureId == GestureFallenBackward))
	{
		fallen = TRUE;
		if (currentStateGoal != -1)
			atTransitionToNextGesture = TRUE;
		// Try playing the helicopter dust cloud for grins to see how it looks!
		// Only play once!!!  Until they get back up.
		if (!fallDust)
		{
			playEjection();
			fallDust = true;
		}
	}
	else if (currentGestureId == GesturePark)
	{
		fallDust = false;
		if (currentStateGoal != -1)
			atTransitionToNextGesture = TRUE;
	}
	else // Normal Frame Increment
	{
		fallDust = false;
		if (animate)
		{
			float frameInc = 0.0f;
			if (singleStepMode && nextStep)
			{
				frameInc = 1.0f;
			}
			else if (singleStepMode && prevStep)
			{
				frameInc = -1.0f;
			}
			else if (!singleStepMode)
			{
				//--------------------------------------------------------
				// Make sure animation runs no faster than mechFrameRate fps.
				frameInc = mechFrameRate * frameLength;
			}
			//------------------------------------------
			// Always ASSUME we cannot transition yet.
			// Saves myriad compares below.
			atTransitionToNextGesture = false;
			//---------------------------------------
			// Increment Frames -- Everything else!
			if ((frameInc != 0.0f) || (singleStepMode))
			{
				if (!setFirstFrame) // DO NOT ANIMATE ON FIRST FRAME!  Wait a
					// bit!
				{
					if (inReverse)
						currentFrame -= frameInc;
					else
						currentFrame += frameInc;
				}
				//--------------------------------------
				// Check Positive overflow of gesture
				if (currentFrame >= mechType->getNumFrames(currentGestureId))
				{
					if ((currentStateGoal == -1) && (hitGestureId == -1) && (currentGestureId != GestureIdle)) // Are we just
						// Looping or
						// transitioning?
					{
						currentFrame -= mechType->getNumFrames(currentGestureId);
					}
					else
					{
						//-------------------------------------------
						// We are at the end and ready to transition
						// Force us back to the previous frame so draw
						// doesn't go off into lala land.
						atTransitionToNextGesture = TRUE;
						currentFrame = mechType->getNumFrames(currentGestureId) - 1.0f;
						if (currentGestureId == GestureJump)
						{
							inJump = false;
							jumpSetup = false;
							oldStateGoal = 1; // We always started a Jump standing!
						}
					}
				}
				//--------------------------------------
				// Check negative overflow of gesture
				if (currentFrame < 0)
				{
					if ((currentStateGoal == -1) && (hitGestureId == -1))
					{
						currentFrame += mechType->getNumFrames(currentGestureId);
					}
					else
					{
						//-------------------------------------------
						// We are at the end and ready to transition
						// Force us back to the previous frame so draw
						// doesn't go off into lala land.
						atTransitionToNextGesture = TRUE;
						currentFrame = 0.0f;
					}
				}
			}
			else // This is a  single frame gesture.  We are ALWAYS ready to
				// transition in this case!
			{
				if (currentStateGoal != -1)
					atTransitionToNextGesture = TRUE;
			}
		}
	}
	if (currentFrame < 0.0f)
		currentFrame = 0.0f;
	//		STOP(("CurrentFrame of animation can never be less then zero.  Frame
	//%f,  NumFrames %f, GestureId
	//%d",currentFrame,mechType->getNumFrames(currentGestureId),currentGestureId));
	if ((currentFrame > 0.0f) && (currentFrame >= mechType->getNumFrames(currentGestureId)))
		currentFrame = 0.0f;
//		STOP(("CurrentFrame is greater then numFrames.  Frame %f,  NumFrames %f,
// GestureId
//%d",currentFrame,mechType->getNumFrames(currentGestureId),currentGestureId));
#ifdef _DEBUG
	if (logFile && (inDebugMoveMode))
	{
		char msg[1024];
		sprintf(msg, "Gesture: %d, Frame: %f, SGoal: %d, OGoal: %d, Rot: %f", currentGestureId,
			currentFrame, currentStateGoal, oldStateGoal, rotation);
		logFile->writeLine(msg);
	}
#endif
	if ((turn < 3) || inView || (currentGestureId == GestureJump)) // Gotta get the weapon nodes working!!
		updateGeometry();
	//----------------------------------------------------------------------
	// If currentGestureId is 2 and baseRootNodeHeight is not set, set it!!
	if (baseRootNodeHeight == -99999.9f)
	{
		if (rootNodeIndex == -1)
			rootNodeIndex = mechShape->GetNodeNameId("joint_root");
		bool oldInView = inView;
		inView = true;
		baseRootNodeHeight = (getNodeIdPosition(rootNodeIndex).z - position.z);
		inView = oldInView;
	}
	//------------------------------------------------
	// If arms are off, process their geometry here!
	// MUST do every frame.  We don't know where the arms are!!!
	Stuff::Point3D xlatPosition;
	Stuff::UnitQuaternion qRotation;
	if (leftArmOff && leftArm && leftArmRecalc())
	{
		//--------------------------------------------------------
		// Update the dynamics and position here for leftArm
		float speed = dVel[1].GetLength();
		if (speed)
		{
			Stuff::Vector3D velDiff = dAcc[1];
			velDiff *= frameLength;
			dVel[1].Add(dVel[1], velDiff);
			speed = dVel[1].GetLength();
			if (speed < Stuff::SMALL)
			{
				dVel[1].x = dVel[1].y = dVel[1].z = 0.0;
			}
			Stuff::Vector3D posDiff = dVel[1];
			posDiff *= frameLength;
			leftArmPos.Add(leftArmPos, posDiff);
			float elev = land->getTerrainElevation(leftArmPos);
			if (leftArmPos.z < elev)
			{
				leftArmPos.z = elev;
				dRacc[1].Zero();
				dRVel[1].Zero();
				dTime[1] -= frameLength;
				if (dTime[1] < 0.0)
					dVel[1].x = dVel[1].y = dVel[1].z = 0.0;
			}
			Stuff::Vector3D rvDiff = dRacc[1];
			rvDiff *= frameLength;
			dRVel[1].Add(dRVel[1], rvDiff);
			Stuff::Vector3D rotDiff = dRVel[1];
			rotDiff *= frameLength;
			dRot[1].Add(dRot[1], rotDiff);
		}
		xlatPosition.x = -leftArmPos.x;
		xlatPosition.y = leftArmPos.z;
		xlatPosition.z = leftArmPos.y;
		qRotation = Stuff::EulerAngles(
			dRot[1].x * DEGREES_TO_RADS, dRot[1].y * DEGREES_TO_RADS, dRot[1].z * DEGREES_TO_RADS);
		leftArm->SetFogRGB(0xffffffff);
		leftArm->SetLightList(eye->getWorldLights(), eye->getNumLights());
		leftArm->TransformMultiShape(&xlatPosition, &qRotation);
		if (leftArmSmoke)
		{
			Stuff::LinearMatrix4D shapeOrigin;
			Stuff::LinearMatrix4D localToWorld;
			Stuff::LinearMatrix4D localResult;
			leftShoulderPos.x = -leftArmPos.x;
			leftShoulderPos.y = leftArmPos.z;
			leftShoulderPos.z = leftArmPos.y;
			shapeOrigin.BuildRotation(Stuff::EulerAngles(0.0f, 0.0f, 0.0f));
			shapeOrigin.BuildTranslation(leftShoulderPos);
			Stuff::UnitQuaternion effectRot;
			effectRot = Stuff::EulerAngles(0.0f, rotation * DEGREES_TO_RADS, 0.0f);
			localToWorld.Multiply(gosFX::Effect_Against_Motion, effectRot);
			localResult.Multiply(localToWorld, shapeOrigin);
			Stuff::OBB boundingBox;
			gosFX::Effect::ExecuteInfo info((Stuff::Time)scenarioTime, &localResult, &boundingBox);
			bool result = leftArmSmoke->Execute(&info);
			if (!result)
			{
				leftArmSmoke->Kill();
				delete leftArmSmoke;
				leftArmSmoke = nullptr;
			}
		}
	}
	if (rightArmOff && rightArm && rightArmRecalc())
	{
		//--------------------------------------------------------
		// Update the dynamics and position here for leftArm
		float speed = dVel[0].GetLength();
		if (speed)
		{
			Stuff::Vector3D velDiff = dAcc[0];
			velDiff *= frameLength;
			dVel[0].Add(dVel[0], velDiff);
			speed = dVel[0].GetLength();
			if (speed < Stuff::SMALL)
			{
				dVel[0].x = dVel[0].y = dVel[0].z = 0.0;
			}
			Stuff::Vector3D posDiff = dVel[0];
			posDiff *= frameLength;
			rightArmPos.Add(rightArmPos, posDiff);
			float elev = land->getTerrainElevation(rightArmPos);
			if (rightArmPos.z < elev)
			{
				rightArmPos.z = elev;
				dRacc[0].Zero();
				dRVel[0].Zero();
				dTime[0] -= frameLength;
				if (dTime[0] < 0.0)
					dVel[0].x = dVel[0].y = dVel[0].z = 0.0;
			}
			Stuff::Vector3D rvDiff = dRacc[0];
			rvDiff *= frameLength;
			dRVel[0].Add(dRVel[0], rvDiff);
			Stuff::Vector3D rotDiff = dRVel[0];
			rotDiff *= frameLength;
			dRot[0].Add(dRot[0], rotDiff);
		}
		xlatPosition.x = -rightArmPos.x;
		xlatPosition.y = rightArmPos.z;
		xlatPosition.z = rightArmPos.y;
		qRotation = Stuff::EulerAngles(
			dRot[0].x * DEGREES_TO_RADS, dRot[0].y * DEGREES_TO_RADS, dRot[0].z * DEGREES_TO_RADS);
		rightArm->SetFogRGB(0xffffffff);
		rightArm->SetLightList(eye->getWorldLights(), eye->getNumLights());
		rightArm->TransformMultiShape(&xlatPosition, &qRotation);
		if (rightArmSmoke)
		{
			Stuff::LinearMatrix4D shapeOrigin;
			Stuff::LinearMatrix4D localToWorld;
			Stuff::LinearMatrix4D localResult;
			rightShoulderPos.x = -rightArmPos.x;
			rightShoulderPos.y = rightArmPos.z;
			rightShoulderPos.z = rightArmPos.y;
			shapeOrigin.BuildRotation(Stuff::EulerAngles(0.0f, 0.0f, 0.0f));
			shapeOrigin.BuildTranslation(rightShoulderPos);
			Stuff::UnitQuaternion effectRot;
			effectRot = Stuff::EulerAngles(0.0f, rotation * DEGREES_TO_RADS, 0.0f);
			localToWorld.Multiply(gosFX::Effect_Against_Motion, effectRot);
			localResult.Multiply(localToWorld, shapeOrigin);
			Stuff::OBB boundingBox;
			gosFX::Effect::ExecuteInfo info((Stuff::Time)scenarioTime, &localResult, &boundingBox);
			bool result = rightArmSmoke->Execute(&info);
			if (!result)
			{
				rightArmSmoke->Kill();
				delete rightArmSmoke;
				rightArmSmoke = nullptr;
			}
		}
	}
	nextStep = prevStep = false;
	return TRUE;
}

//-----------------------------------------------------------------------------
void
Mech3DAppearance::blowLeftArm(void)
{
	if (leftArmOff)
		return;
	// For now, the shoulder just gets a location which does not change.
	// Can easily move to a node if a node becomes available.
	//
	if (strcmp(weaponEffects->GetEffectName(SHOULDER_POP_ID), "NONE") != 0)
	{
		//--------------------------------------------
		// Yes, load it on up.
		uint32_t flags = gosFX::Effect::ExecuteFlag;
		Check_Object(gosFX::EffectLibrary::Instance);
		gosFX::Effect::Specification* gosEffectSpec =
			gosFX::EffectLibrary::Instance->Find(weaponEffects->GetEffectName(SHOULDER_POP_ID));
		if (gosEffectSpec)
		{
			leftShoulderBoom =
				gosFX::EffectLibrary::Instance->MakeEffect(gosEffectSpec->m_effectID, flags);
			gosASSERT(leftShoulderBoom != nullptr);
			MidLevelRenderer::MLRTexturePool::Instance->LoadImages();
		}
	}
	if (strcmp(weaponEffects->GetEffectName(ARM_FLYING_ID), "NONE") != 0)
	{
		//--------------------------------------------
		// Yes, load it on up.
		uint32_t flags = gosFX::Effect::ExecuteFlag;
		Check_Object(gosFX::EffectLibrary::Instance);
		gosFX::Effect::Specification* gosEffectSpec =
			gosFX::EffectLibrary::Instance->Find(weaponEffects->GetEffectName(ARM_FLYING_ID));
		if (gosEffectSpec)
		{
			leftArmSmoke =
				gosFX::EffectLibrary::Instance->MakeEffect(gosEffectSpec->m_effectID, flags);
			gosASSERT(leftArmSmoke != nullptr);
			MidLevelRenderer::MLRTexturePool::Instance->LoadImages();
		}
	}
	if (leftArmNodeIndex == -1)
		leftArmNodeIndex = mechShape->GetNodeNameId("joint_luarm");
	Stuff::Vector3D leftNodePos = getNodeIdPosition(leftArmNodeIndex);
	leftArmPos = leftNodePos;
	if (leftShoulderBoom)
	{
		Stuff::LinearMatrix4D shapeOrigin;
		Stuff::LinearMatrix4D localToWorld;
		Stuff::LinearMatrix4D localResult;
		leftShoulderPos.x = -leftNodePos.x;
		leftShoulderPos.y = leftNodePos.z;
		leftShoulderPos.z = leftNodePos.y;
		shapeOrigin.BuildRotation(Stuff::EulerAngles(0.0f, 0.0f, 0.0f));
		shapeOrigin.BuildTranslation(leftShoulderPos);
		Stuff::UnitQuaternion effectRot;
		effectRot = Stuff::EulerAngles(0.0f * DEGREES_TO_RADS, rotation * DEGREES_TO_RADS, 0.0f);
		localToWorld.Multiply(gosFX::Effect_Against_Motion, effectRot);
		localResult.Multiply(localToWorld, shapeOrigin);
		gosFX::Effect::ExecuteInfo info((Stuff::Time)scenarioTime, &localResult, nullptr);
		leftShoulderBoom->Start(&info);
	}
	if (leftArmSmoke)
	{
		Stuff::LinearMatrix4D shapeOrigin;
		Stuff::LinearMatrix4D localToWorld;
		Stuff::LinearMatrix4D localResult;
		leftShoulderPos.x = -leftArmPos.x;
		leftShoulderPos.y = leftArmPos.z;
		leftShoulderPos.z = leftArmPos.y;
		shapeOrigin.BuildRotation(Stuff::EulerAngles(0.0f, 0.0f, 0.0f));
		shapeOrigin.BuildTranslation(leftShoulderPos);
		Stuff::UnitQuaternion effectRot;
		effectRot = Stuff::EulerAngles(0.0f * DEGREES_TO_RADS, rotation * DEGREES_TO_RADS, 0.0f);
		localToWorld.Multiply(gosFX::Effect_Against_Motion, effectRot);
		localResult.Multiply(localToWorld, shapeOrigin);
		gosFX::Effect::ExecuteInfo info((Stuff::Time)scenarioTime, &localResult, nullptr);
		leftArmSmoke->Start(&info);
	}
	leftArmOff = true;
	// Calc initial Velocity, rotation and set Acceleration to down in World.
	int32_t xlatBase = 12.0f + 50.0 / 2.0f;
	int32_t upBase = 25.0f + 100.0;
	int32_t rotBase = 25.0f + 100.0;
	dVel[1].x = RandomNumber(xlatBase * 2.0) - xlatBase;
	dVel[1].y = RandomNumber(xlatBase * 2.0) - xlatBase;
	dVel[1].z = RandomNumber(upBase) + upBase;
	dRVel[1].x = RandomNumber(rotBase * 2.0) - rotBase;
	dRVel[1].y = RandomNumber(rotBase * 2.0) - rotBase;
	dRVel[1].z = RandomNumber(rotBase * 2.0) - rotBase;
	dRot[1].Zero();
	dAcc[1].x = dVel[1].x * 0.1f;
	dAcc[1].y = dVel[1].y * 0.1f;
	dAcc[1].z = dVel[1].z * 0.5f;
	dAcc[1].Negate(dAcc[1]);
	dRacc[1].x = dRVel[1].x * 0.1f;
	dRacc[1].y = dRVel[1].y * 0.1f;
	dRacc[1].z = dRVel[1].z * 0.1f;
	dRacc[1].Negate(dRacc[1]);
	dTime[1] = 4.0f;
	mechShape->StopUsing("joint_luarm");
}

//-----------------------------------------------------------------------------
void
Mech3DAppearance::blowRightArm(void)
{
	if (rightArmOff)
		return;
	// For now, the shoulder just gets a location which does not change.
	// Can easily move to a node if a node becomes available.
	//
	if (strcmp(weaponEffects->GetEffectName(SHOULDER_POP_ID), "NONE") != 0)
	{
		//--------------------------------------------
		// Yes, load it on up.
		uint32_t flags = gosFX::Effect::ExecuteFlag;
		Check_Object(gosFX::EffectLibrary::Instance);
		gosFX::Effect::Specification* gosEffectSpec =
			gosFX::EffectLibrary::Instance->Find(weaponEffects->GetEffectName(SHOULDER_POP_ID));
		if (gosEffectSpec)
		{
			rightShoulderBoom =
				gosFX::EffectLibrary::Instance->MakeEffect(gosEffectSpec->m_effectID, flags);
			gosASSERT(rightShoulderBoom != nullptr);
			MidLevelRenderer::MLRTexturePool::Instance->LoadImages();
		}
	}
	if (strcmp(weaponEffects->GetEffectName(ARM_FLYING_ID), "NONE") != 0)
	{
		//--------------------------------------------
		// Yes, load it on up.
		uint32_t flags = gosFX::Effect::ExecuteFlag;
		Check_Object(gosFX::EffectLibrary::Instance);
		gosFX::Effect::Specification* gosEffectSpec =
			gosFX::EffectLibrary::Instance->Find(weaponEffects->GetEffectName(ARM_FLYING_ID));
		if (gosEffectSpec)
		{
			rightArmSmoke =
				gosFX::EffectLibrary::Instance->MakeEffect(gosEffectSpec->m_effectID, flags);
			gosASSERT(rightArmSmoke != nullptr);
			MidLevelRenderer::MLRTexturePool::Instance->LoadImages();
		}
	}
	if (rightArmNodeIndex == -1)
		rightArmNodeIndex = mechShape->GetNodeNameId("joint_ruarm");
	Stuff::Vector3D rightNodePos = getNodeIdPosition(rightArmNodeIndex);
	rightArmPos = rightNodePos;
	if (rightShoulderBoom)
	{
		Stuff::LinearMatrix4D shapeOrigin;
		Stuff::LinearMatrix4D localToWorld;
		Stuff::LinearMatrix4D localResult;
		rightShoulderPos.x = -rightNodePos.x;
		rightShoulderPos.y = rightNodePos.z;
		rightShoulderPos.z = rightNodePos.y;
		shapeOrigin.BuildRotation(Stuff::EulerAngles(0.0f, 0.0f, 0.0f));
		shapeOrigin.BuildTranslation(rightShoulderPos);
		Stuff::UnitQuaternion effectRot;
		effectRot = Stuff::EulerAngles(0.0f * DEGREES_TO_RADS, rotation * DEGREES_TO_RADS, 0.0f);
		localToWorld.Multiply(gosFX::Effect_Against_Motion, effectRot);
		localResult.Multiply(localToWorld, shapeOrigin);
		gosFX::Effect::ExecuteInfo info((Stuff::Time)scenarioTime, &localResult, nullptr);
		rightShoulderBoom->Start(&info);
	}
	if (rightArmSmoke)
	{
		Stuff::LinearMatrix4D shapeOrigin;
		Stuff::LinearMatrix4D localToWorld;
		Stuff::LinearMatrix4D localResult;
		rightShoulderPos.x = -rightArmPos.x;
		rightShoulderPos.y = rightArmPos.z;
		rightShoulderPos.z = rightArmPos.y;
		shapeOrigin.BuildRotation(Stuff::EulerAngles(0.0f, 0.0f, 0.0f));
		shapeOrigin.BuildTranslation(rightShoulderPos);
		Stuff::UnitQuaternion effectRot;
		effectRot = Stuff::EulerAngles(0.0f * DEGREES_TO_RADS, rotation * DEGREES_TO_RADS, 0.0f);
		localToWorld.Multiply(gosFX::Effect_Against_Motion, effectRot);
		localResult.Multiply(localToWorld, shapeOrigin);
		gosFX::Effect::ExecuteInfo info((Stuff::Time)scenarioTime, &localResult, nullptr);
		rightArmSmoke->Start(&info);
	}
	rightArmOff = true;
	// Calc initial Velocity, rotation and set Acceleration to down in World.
	int32_t xlatBase = 12.0f + 50.0 / 2.0f;
	int32_t upBase = 25.0f + 100.0;
	int32_t rotBase = 25.0f + 100.0;
	dVel[0].x = RandomNumber(xlatBase * 2.0) - xlatBase;
	dVel[0].y = RandomNumber(xlatBase * 2.0) - xlatBase;
	dVel[0].z = RandomNumber(upBase) + upBase;
	dRVel[0].x = RandomNumber(rotBase * 2.0) - rotBase;
	dRVel[0].y = RandomNumber(rotBase * 2.0) - rotBase;
	dRVel[0].z = RandomNumber(rotBase * 2.0) - rotBase;
	dRot[0].Zero();
	dAcc[0].x = dVel[0].x * 0.1f;
	dAcc[0].y = dVel[0].y * 0.1f;
	dAcc[0].z = dVel[0].z * 0.5f;
	dAcc[0].Negate(dAcc[0]);
	dRacc[0].x = dRVel[0].x * 0.1f;
	dRacc[0].y = dRVel[0].y * 0.1f;
	dRacc[0].z = dRVel[0].z * 0.1f;
	dRacc[0].Negate(dRacc[0]);
	dTime[0] = 4.0f;
	mechShape->StopUsing("joint_ruarm");
}

//-----------------------------------------------------------------------------
void
Mech3DAppearance::startSmoking(int32_t smokeLvl)
{
	// Check if we are already playing one.  If not, smoke away
	// First, check if its even loaded.
	// can easily preload this.  Should we?  Memory?
	if (!useNonWeaponEffects)
		return;
	if ((smokeLvl > 0) && !criticalSmoke)
	{
		if (strcmp(weaponEffects->GetEffectName(CRITICAL_SMOKE_ID), "NONE") != 0)
		{
			//--------------------------------------------
			// Yes, load it on up.
			uint32_t flags = gosFX::Effect::ExecuteFlag | gosFX::Effect::LoopFlag;
			Check_Object(gosFX::EffectLibrary::Instance);
			gosFX::Effect::Specification* gosEffectSpec = gosFX::EffectLibrary::Instance->Find(
				weaponEffects->GetEffectName(CRITICAL_SMOKE_ID));
			if (gosEffectSpec)
			{
				criticalSmoke =
					gosFX::EffectLibrary::Instance->MakeEffect(gosEffectSpec->m_effectID, flags);
				gosASSERT(criticalSmoke != nullptr);
				MidLevelRenderer::MLRTexturePool::Instance->LoadImages();
			}
		}
	}
	if ((smokeLvl == 0) && !smokeEffect)
	{
		if (strcmp(weaponEffects->GetEffectName(MECH_SMOKE_ID), "NONE") != 0)
		{
			//--------------------------------------------
			// Yes, load it on up.
			uint32_t flags = gosFX::Effect::ExecuteFlag | gosFX::Effect::LoopFlag;
			Check_Object(gosFX::EffectLibrary::Instance);
			gosFX::Effect::Specification* gosEffectSpec =
				gosFX::EffectLibrary::Instance->Find(weaponEffects->GetEffectName(MECH_SMOKE_ID));
			if (gosEffectSpec)
			{
				smokeEffect =
					gosFX::EffectLibrary::Instance->MakeEffect(gosEffectSpec->m_effectID, flags);
				gosASSERT(smokeEffect != nullptr);
				MidLevelRenderer::MLRTexturePool::Instance->LoadImages();
			}
		}
	}
	if (smokeLvl != -1)
	{
		if ((isSmoking == -1) || ((isSmoking != smokeLvl) && (smokeLvl != -1)))
		{
			Stuff::LinearMatrix4D shapeOrigin;
			Stuff::LinearMatrix4D localToWorld;
			Stuff::Vector3D smokeNodePos = getNodePosition(0); // Always SMOKE if it exists!!
			Stuff::Point3D smokePos;
			smokePos.x = -smokeNodePos.x;
			smokePos.y = smokeNodePos.z;
			smokePos.z = smokeNodePos.y;
			shapeOrigin.BuildRotation(Stuff::EulerAngles(0.0f, 0.0f, 0.0f));
			shapeOrigin.BuildTranslation(smokePos);
			gosFX::Effect::ExecuteInfo info((Stuff::Time)scenarioTime, &shapeOrigin, nullptr);
			if (smokeLvl > 0)
				criticalSmoke->Start(&info);
			else
				smokeEffect->Start(&info);
		}
	}
	else
	{
		if (smokeEffect)
			smokeEffect->Kill();
		if (criticalSmoke)
			criticalSmoke->Kill();
	}
	isSmoking = smokeLvl;
}

//-----------------------------------------------------------------------------
void
Mech3DAppearance::startWaterWake(void)
{
	// Check if we are already playing one.  If not, wake city.
	// Check if we are a helicopter OR we are jumping.  No WAKE if either is
	// true.
	if (isHelicopter || currentGestureId == 20)
		return;
	if (!useNonWeaponEffects)
		return;
	// First, check if its even loaded.
	// can easily preload this.  Should we?  Memory?
	if (useNonWeaponEffects && !waterWake)
	{
		if (strcmp(weaponEffects->GetEffectName(MECH_WATER_WAKE), "NONE") != 0)
		{
			//--------------------------------------------
			// Yes, load it on up.
			uint32_t flags = gosFX::Effect::ExecuteFlag | gosFX::Effect::LoopFlag;
			Check_Object(gosFX::EffectLibrary::Instance);
			gosFX::Effect::Specification* gosEffectSpec =
				gosFX::EffectLibrary::Instance->Find(weaponEffects->GetEffectName(MECH_WATER_WAKE));
			if (gosEffectSpec)
			{
				waterWake =
					gosFX::EffectLibrary::Instance->MakeEffect(gosEffectSpec->m_effectID, flags);
				gosASSERT(waterWake != nullptr);
				MidLevelRenderer::MLRTexturePool::Instance->LoadImages();
			}
		}
	}
	if (waterWake && !isWaking) // Start the effect if we are not running it yet!!
	{
		Stuff::LinearMatrix4D shapeOrigin;
		Stuff::LinearMatrix4D localToWorld;
		Stuff::LinearMatrix4D localResult;
		Stuff::Point3D wakePos;
		wakePos.x = -position.x;
		wakePos.y = Terrain::waterElevation; // Wake is at Water Level!
		wakePos.z = position.y;
		shapeOrigin.BuildRotation(Stuff::EulerAngles(0.0f, 0.0f, 0.0f));
		shapeOrigin.BuildTranslation(wakePos);
		Stuff::UnitQuaternion effectRot;
		effectRot = Stuff::EulerAngles(90.0f * DEGREES_TO_RADS, rotation * DEGREES_TO_RADS, 0.0f);
		if (!inReverse)
			localToWorld.Multiply(gosFX::Effect_Against_Motion, effectRot);
		else
			localToWorld.Multiply(gosFX::Effect_Into_Motion, effectRot);
		localResult.Multiply(localToWorld, shapeOrigin);
		gosFX::Effect::ExecuteInfo info((Stuff::Time)scenarioTime, &localResult, nullptr);
		waterWake->Start(&info);
		isWaking = true;
	}
}

//-----------------------------------------------------------------------------
void
Mech3DAppearance::stopWaterWake(void)
{
	if (waterWake && isWaking) // Stop the effect if we are running it!!
		waterWake->Kill();
	isWaking = false;
}

//-----------------------------------------------------------------------------
void
Mech3DAppearance::playEjection(void)
{
	if (InEditor)
		return;
	// Check if we are already playing one.  If not, dustCloud.  Use for falls,
	// too?  First, check if its even loaded.
	// can easily preload this.  Should we?  Memory?
	if (useNonWeaponEffects && !helicopterDustCloud)
	{
		if (strcmp(weaponEffects->GetEffectName(HELICOPTER_DUST_CLOUD), "NONE") != 0)
		{
			//--------------------------------------------
			// Yes, load it on up.
			uint32_t flags = gosFX::Effect::ExecuteFlag;
			Check_Object(gosFX::EffectLibrary::Instance);
			gosFX::Effect::Specification* gosEffectSpec = gosFX::EffectLibrary::Instance->Find(
				weaponEffects->GetEffectName(HELICOPTER_DUST_CLOUD));
			if (gosEffectSpec)
			{
				helicopterDustCloud =
					gosFX::EffectLibrary::Instance->MakeEffect(gosEffectSpec->m_effectID, flags);
				gosASSERT(helicopterDustCloud != nullptr);
				MidLevelRenderer::MLRTexturePool::Instance->LoadImages();
			}
		}
	}
	if (!isDusting && helicopterDustCloud) // Start the effect if we are not running it yet!!
	{
		Stuff::LinearMatrix4D shapeOrigin;
		Stuff::LinearMatrix4D localToWorld;
		Stuff::LinearMatrix4D localResult;
		Stuff::Point3D wakePos;
		wakePos.x = -position.x;
		wakePos.y = position.z; // dustCloud is just centered on position.
		wakePos.z = position.y;
		shapeOrigin.BuildRotation(Stuff::EulerAngles(0.0f, 0.0f, 0.0f));
		shapeOrigin.BuildTranslation(wakePos);
		/*
		Stuff::UnitQuaternion effectRot;
		effectRot = Stuff::EulerAngles(0.0f,rotation * DEGREES_TO_RADS,0.0f);
		localToWorld.Multiply(gosFX::Effect_Against_Motion,effectRot);
		localResult.Multiply(localToWorld,shapeOrigin);
		*/
		gosFX::Effect::ExecuteInfo info((Stuff::Time)scenarioTime, &shapeOrigin, nullptr);
		helicopterDustCloud->Start(&info);
		isDusting = true;
	}
}

//-----------------------------------------------------------------------------
void
Mech3DAppearance::destroy(void)
{
	AppearanceTypeList::appearanceHeap->Free(paintSchemata);
	paintSchemata = nullptr;
	if (mechShape)
		delete mechShape;
	mechShape = nullptr;
	if (mechShadowShape)
	{
		delete mechShadowShape;
		mechShadowShape = nullptr;
	}
	if (leftArm)
		delete leftArm;
	leftArm = nullptr;
	if (rightArm)
		delete rightArm;
	rightArm = nullptr;
#ifdef _DEBUG
	if (logFile)
	{
		delete logFile;
		logFile = nullptr;
	}
#endif
	for (size_t i = 0; i < MAX_DUST_POOFS; i++)
	{
		if (rightDustPoofEffect[i])
		{
			rightDustPoofEffect[i]->Kill();
			delete rightDustPoofEffect[i];
			rightDustPoofEffect[i] = nullptr;
		}
		if (leftDustPoofEffect[i])
		{
			leftDustPoofEffect[i]->Kill();
			delete leftDustPoofEffect[i];
			leftDustPoofEffect[i] = nullptr;
		}
	}
	if (smokeEffect)
	{
		smokeEffect->Kill();
		delete smokeEffect;
		smokeEffect = nullptr;
	}
	if (jumpJetEffect)
	{
		jumpJetEffect->Kill();
		delete jumpJetEffect;
		jumpJetEffect = nullptr;
	}
	if (rightShoulderBoom)
	{
		rightShoulderBoom->Kill();
		delete rightShoulderBoom;
		rightShoulderBoom = nullptr;
	}
	if (leftShoulderBoom)
	{
		leftShoulderBoom->Kill();
		delete leftShoulderBoom;
		leftShoulderBoom = nullptr;
	}
	if (sensorSquareShape)
	{
		delete sensorSquareShape;
		sensorSquareShape = nullptr;
	}
	if (sensorTriangleShape)
	{
		delete sensorTriangleShape;
		sensorTriangleShape = nullptr;
	}
	appearanceTypeList->removeAppearance(mechType);
	if (InEditor)
	{
		AppearanceTypeList::appearanceHeap->Free(nodeUsed);
		nodeUsed = nullptr;
		AppearanceTypeList::appearanceHeap->Free(nodeRecycle);
		nodeRecycle = nullptr;
	}
}

//*****************************************************************************
void
Mech3DAppearance::copyTo(MechAppearanceData* data)
{
	data->frameNum = frameNum;
	data->mechFrameRate = mechFrameRate;
	data->leftArmOff = leftArmOff;
	data->rightArmOff = rightArmOff;
	data->fallen = fallen;
	data->forceStop = forceStop;
	data->atTransitionToNextGesture = atTransitionToNextGesture;
	data->inReverse = inReverse;
	data->inJump = inJump;
	data->jumpSetup = jumpSetup;
	data->jumpFXSetup = jumpFXSetup;
	data->jumpAirborne = jumpAirborne;
	data->oncePerFrame = oncePerFrame;
	data->lockRotation = lockRotation;
	data->velocity = velocity;
	data->status = status;
	data->currentStateGoal = currentStateGoal;
	data->currentGestureId = currentGestureId;
	data->transitionState = transitionState;
	data->oldStateGoal = oldStateGoal;
	data->hitGestureId = hitGestureId;
	data->currentFrame = currentFrame;
	data->currentLOD = currentLOD;
	if (mechType->numWeaponNodes > 10)
		STOP(("Mech Has too many weapon nodes to save %d", mechType->numWeaponNodes));
	memcpy(data->nodeUsed, nodeUsed, 10);
	memcpy(data->nodeRecycle, nodeRecycle, 10);
	data->isSmoking = isSmoking;
	data->isWaking = isWaking;
	data->isDusting = isDusting;
	data->fallDust = fallDust;
	data->isHelicopter = isHelicopter;
	data->baseRootNodeHeight = baseRootNodeHeight;
	data->jumpDestination = jumpDestination;
	data->jumpVelocity = jumpVelocity;
}

//*****************************************************************************
void
Mech3DAppearance::copyFrom(MechAppearanceData* data)
{
	frameNum = data->frameNum;
	mechFrameRate = data->mechFrameRate;
	leftArmOff = data->leftArmOff;
	rightArmOff = data->rightArmOff;
	fallen = data->fallen;
	forceStop = data->forceStop;
	atTransitionToNextGesture = data->atTransitionToNextGesture;
	inReverse = data->inReverse;
	inJump = data->inJump;
	jumpSetup = data->jumpSetup;
	jumpFXSetup = data->jumpFXSetup;
	jumpAirborne = data->jumpAirborne;
	oncePerFrame = data->oncePerFrame;
	lockRotation = data->lockRotation;
	velocity = data->velocity;
	// Let mech reset status when it reloads so that the destroyed shapes come
	// up.
	//	status = data->status;
	currentStateGoal = data->currentStateGoal;
	currentGestureId = data->currentGestureId;
	transitionState = data->transitionState;
	oldStateGoal = data->oldStateGoal;
	hitGestureId = data->hitGestureId;
	currentFrame = data->currentFrame;
	currentLOD = data->currentLOD;
	memcpy(nodeUsed, data->nodeUsed, 10);
	memcpy(nodeRecycle, data->nodeRecycle, 10);
	isSmoking = -1;
	if (data->isSmoking >= 0)
		startSmoking(data->isSmoking);
	isWaking = false;
	if (data->isWaking)
		startWaterWake();
	isDusting = false;
	if (data->isDusting)
		playEjection();
	fallDust = false;
	if (data->fallDust)
		playEjection();
	if (data->inJump)
	{
		int32_t jumpNodeId = mechType->numSmokeNodes + mechType->numWeaponNodes;
		Stuff::Vector3D jumpNodePos = getNodePosition(jumpNodeId);
		Stuff::Point3D actualPosition;
		Stuff::LinearMatrix4D shapeOrigin;
		Stuff::LinearMatrix4D localToWorld;
		Stuff::LinearMatrix4D localResult;
		actualPosition.x = -jumpNodePos.x;
		actualPosition.y = jumpNodePos.z;
		actualPosition.z = jumpNodePos.y;
		shapeOrigin.BuildRotation(Stuff::EulerAngles(0.0f, 0.0f, 0.0f));
		shapeOrigin.BuildTranslation(actualPosition);
		Stuff::UnitQuaternion effectRot;
		effectRot =
			Stuff::EulerAngles(JUMP_PITCH * DEGREES_TO_RADS, rotation * DEGREES_TO_RADS, 0.0f);
		localToWorld.Multiply(gosFX::Effect_Against_Motion, effectRot);
		localResult.Multiply(localToWorld, shapeOrigin);
		gosFX::Effect::ExecuteInfo info((Stuff::Time)scenarioTime, &localResult, nullptr);
		jumpJetEffect->Start(&info);
	}
	isHelicopter = data->isHelicopter;
	baseRootNodeHeight = data->baseRootNodeHeight;
	jumpDestination = data->jumpDestination;
	jumpVelocity = data->jumpVelocity;
}

void
Mech3DAppearance::flashBuilding(float dur, float fDuration, uint32_t color)
{
	duration = dur;
	flashDuration = fDuration;
	flashColor = color;
	drawFlash = true;
	currentFlash = flashDuration;
}

Stuff::Vector3D
Mech3DAppearance::getHitNodeLeft(void)
{
	if (hitLeftNodeIndex == -1)
		hitLeftNodeIndex = mechShape->GetNodeNameId("hit_left");
	Stuff::Vector3D result = position;
	if (!inView)
		return result;
	//-------------------------------------------
	// Create Matrix to conform to.
	Stuff::UnitQuaternion qRotation;
	float yaw = rotation * DEGREES_TO_RADS;
	qRotation = Stuff::EulerAngles(0.0f, yaw, 0.0f);
	Stuff::Point3D xlatPosition;
	xlatPosition.x = -position.x;
	xlatPosition.y = position.z;
	xlatPosition.z = position.y;
	Stuff::UnitQuaternion torsoRot;
	torsoRot = Stuff::EulerAngles(0.0f, (torsoRotation * DEGREES_TO_RADS), 0.0f);
	if (rotationalNodeIndex == -1)
		rotationalNodeIndex = mechShape->SetNodeRotation("joint_torso", &torsoRot);
	mechShape->SetNodeRotation(rotationalNodeIndex, &torsoRot);
	result = mechShape->GetTransformedNodePosition(&xlatPosition, &qRotation, hitLeftNodeIndex);
	if ((result.x == 0.0f) && (result.y == 0.0f) && (result.z == 0.0f))
		result = position;
	return result;
}

Stuff::Vector3D
Mech3DAppearance::getHitNodeRight(void)
{
	if (hitRightNodeIndex == -1)
		hitRightNodeIndex = mechShape->GetNodeNameId("hit_right");
	Stuff::Vector3D result = position;
	if (!inView)
		return result;
	//-------------------------------------------
	// Create Matrix to conform to.
	Stuff::UnitQuaternion qRotation;
	float yaw = rotation * DEGREES_TO_RADS;
	qRotation = Stuff::EulerAngles(0.0f, yaw, 0.0f);
	Stuff::Point3D xlatPosition;
	xlatPosition.x = -position.x;
	xlatPosition.y = position.z;
	xlatPosition.z = position.y;
	Stuff::UnitQuaternion torsoRot;
	torsoRot = Stuff::EulerAngles(0.0f, (torsoRotation * DEGREES_TO_RADS), 0.0f);
	if (rotationalNodeIndex == -1)
		rotationalNodeIndex = mechShape->SetNodeRotation("joint_torso", &torsoRot);
	mechShape->SetNodeRotation(rotationalNodeIndex, &torsoRot);
	result = mechShape->GetTransformedNodePosition(&xlatPosition, &qRotation, hitRightNodeIndex);
	if ((result.x == 0.0f) && (result.y == 0.0f) && (result.z == 0.0f))
		result = position;
	return result;
}

//*****************************************************************************
