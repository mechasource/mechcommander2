//---------------------------------------------------------------------------
//
//	dbactor.h - This file contains the header for the Static ground object
// appearance class
//
//	MechCommander 2
//
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#ifndef BDACTOR_H
#define BDACTOR_H

//---------------------------------------------------------------------------
// Include files
//#include "appear.h"
//#include "apprtype.h"
//#include "move.h"
//#include "msl.h"
//#include "ObjectAppearance.h"
//#include <gosfx/gosfxheaders.hpp>

//**************************************************************************************

#define MAX_BD_ANIMATIONS 10
//***********************************************************************
//
// BldgAppearanceType
//
//***********************************************************************
class BldgAppearanceType : public AppearanceType
{
public:
	TG_TypeMultiShapePtr bldgShape[MAX_LODS];
	float lodDistance[MAX_LODS];

	TG_TypeMultiShapePtr bldgShadowShape;

	TG_TypeMultiShapePtr bldgDmgShape;

	TG_TypeMultiShapePtr bldgDmgShadowShape;

	TG_AnimateShapePtr bdAnimData[MAX_BD_ANIMATIONS];
	bool bdAnimLoop[MAX_BD_ANIMATIONS];
	bool bdReverse[MAX_BD_ANIMATIONS];
	bool bdRandom[MAX_BD_ANIMATIONS];
	int32_t bdStartF[MAX_BD_ANIMATIONS];

	char rotationalNodeId[TG_NODE_ID];
	char destructEffect[60];

	bool spinMe;
	bool isForestClump;

	uint32_t terrainLightRGB;
	float terrainLightIntensity;
	float terrainLightInnerRadius;
	float terrainLightOuterRadius;

	int32_t numWeaponNodes;
	NodeData* nodeData;

public:
	void init(void)
	{
		int32_t i = 0;
		for (i = 0; i < MAX_LODS; i++)
		{
			bldgShape[i]   = nullptr;
			lodDistance[i] = 0.0f;
		}
		bldgShadowShape	= nullptr;
		bldgDmgShape	   = nullptr;
		bldgDmgShadowShape = nullptr;
		for (i = 0; i < MAX_BD_ANIMATIONS; i++)
			bdAnimData[i] = nullptr;
		destructEffect[0] = 0;
		spinMe			  = false;
	}

	BldgAppearanceType(void) { init(void); }

	~BldgAppearanceType(void) { destroy(void); }

	void setAnimation(TG_MultiShapePtr shape, uint32_t animationNum);

	int32_t getNumFrames(int32_t animationNum)
	{
		if ((animationNum >= 0) && (animationNum < MAX_BD_ANIMATIONS) && (bdAnimData[animationNum]))
			return bdAnimData[animationNum]->GetNumFrames(void);
		return 0.0f;
	}

	float getFrameRate(int32_t animationNum)
	{
		if ((animationNum >= 0) && (animationNum < MAX_BD_ANIMATIONS) && (bdAnimData[animationNum]))
			return bdAnimData[animationNum]->GetFrameRate(void);
		return 0.0f;
	}

	void setFrameRate(int32_t animationNum, float nFrameRate)
	{
		if ((animationNum >= 0) && (animationNum < MAX_BD_ANIMATIONS) && (bdAnimData[animationNum]))
			bdAnimData[animationNum]->SetFrameRate(nFrameRate);
	}

	bool isReversed(int32_t animationNum)
	{
		if ((animationNum >= 0) && (animationNum < MAX_BD_ANIMATIONS) && (bdAnimData[animationNum]))
			return bdReverse[animationNum];
		return false;
	}

	bool isLooped(int32_t animationNum)
	{
		if ((animationNum >= 0) && (animationNum < MAX_BD_ANIMATIONS) && (bdAnimData[animationNum]))
			return bdAnimLoop[animationNum];
		return false;
	}

	bool isRandom(int32_t animationNum)
	{
		if ((animationNum >= 0) && (animationNum < MAX_BD_ANIMATIONS) && (bdAnimData[animationNum]))
			return bdRandom[animationNum];
		return false;
	}

	virtual void init(PSTR fileName);

	virtual void destroy(void);
};

//***********************************************************************
//
// BldgAppearance
//
//***********************************************************************
class BldgAppearance : public ObjectAppearance
{
public:
	BldgAppearanceType* appearType;
	TG_MultiShapePtr bldgShape;
	TG_MultiShapePtr bldgShadowShape;

	int32_t bdAnimationState;
	float currentFrame;
	float bdFrameRate;
	bool isReversed;
	bool isLooping;
	bool setFirstFrame;
	bool canTransition;

	float turretYaw;
	float turretPitch;

	float hazeFactor;

	int32_t status;

	gosFX::Effect* destructFX;
	gosFX::Effect* activity;
	gosFX::Effect* activity1;
	bool isActivitying;

	float OBBRadius;
	float highZ;

	float SpinAngle;

	float flashDuration;
	float duration;
	float currentFlash;
	bool drawFlash;
	uint32_t flashColor;

	int32_t currentLOD;

	int32_t* nodeUsed;  // Used to stagger the weapon nodes for firing.
	float* nodeRecycle; // Used for ripple fire to find out if the node has
						// fired recently.

	TG_LightPtr pointLight;
	uint32_t lightId;
	bool forceLightsOut;
	bool beenInView;

	bool fogLightSet;
	uint32_t lightRGB;
	uint32_t fogRGB;

	int32_t rotationalNodeId;
	int32_t hitNodeId;
	int32_t activityNodeId;
	int32_t activityNode1Id;

public:
	virtual void init(AppearanceTypePtr tree = nullptr, GameObjectPtr obj = nullptr);

	virtual AppearanceTypePtr getAppearanceType(void) { return appearType; }

	BldgAppearance(void) { init(void); }

	virtual int32_t update(bool animate = true);
	virtual int32_t render(int32_t depthFixup = 0);

	virtual int32_t renderShadows(void);

	virtual void destroy(void);

	~BldgAppearance(void) { destroy(void); }

	virtual bool recalcBounds(void);

	virtual bool getInTransition(void) { return (canTransition == false); }

	void setFadeTable(puint8_t fTable) { fadeTable = fTable; }

	virtual void setGesture(size_t gestureId);

	virtual int32_t getCurrentGestureId(void) { return bdAnimationState; }

	virtual size_t getAppearanceClass(void) { return BUILDING_APPR_TYPE; }

	virtual void setObjectNameId(int32_t objId) { objectNameId = objId; }

	virtual bool isMouseOver(float px, float py);

	virtual void setObjectParameters(Stuff::Vector3D& pos, float rot, int32_t selected,
		int32_t alignment, int32_t homeRelations);

	virtual void setMoverParameters(
		float turretRot, float lArmRot = 0.0f, float rArmRot = 0.0f, bool isAirborne = false);

	virtual void setObjStatus(int32_t oStatus);

	virtual int32_t calcCellsCovered(Stuff::Vector3D& pos, pint16_t cellList);

	virtual void markTerrain(_ScenarioMapCellInfo* pInfo, int32_t type, int32_t counter);

	virtual int32_t markMoveMap(bool passable, int32_t* lineOfSightRect, bool useheight = false,
		pint16_t cellList = nullptr);

	virtual void markLOS(bool clearIt = false);

	void calcAdjCell(int32_t& row, int32_t& col);

	virtual void scale(float scaleFactor) { bldgShape->ScaleShape(scaleFactor); }

	virtual bool playDestruction(void);

	virtual float getRadius(void) { return OBBRadius; }

	virtual void flashBuilding(float duration, float flashDuration, uint32_t color);

	virtual float getTopZ(void) { return highZ; }

	virtual void setWeaponNodeUsed(int32_t nodeId);

	virtual int32_t getWeaponNode(int32_t weapontype);

	virtual float getWeaponNodeRecycle(int32_t node);

	virtual Stuff::Vector3D getWeaponNodePosition(int32_t node);

	virtual bool isSelectable() { return !appearType->spinMe; }

	virtual void setFilterState(bool state) { bldgShape->GetMultiType()->SetFilter(state); }

	virtual void setIsHudElement(void) { bldgShape->SetIsHudElement(void); }

	virtual bool getIsLit(void) { return (appearType->terrainLightRGB != 0xffffffff); }

	virtual void setLightsOut(bool lightFlag) { forceLightsOut = lightFlag; }

	virtual bool PerPolySelect(int32_t mouseX, int32_t mouseY);

	virtual bool isForestClump(void) { return appearType->isForestClump; }

	virtual Stuff::Point3D getRootNodeCenter(void)
	{
		Stuff::Point3D result = bldgShape->GetRootNodeCenter(void);
		return result;
	}

	virtual Stuff::Vector3D getNodeNamePosition(PSTR nodeName);

	virtual void startActivity(int32_t effectId, bool loop);
	virtual void stopActivity(void);

	virtual Stuff::Vector3D getHitNode(void);

	virtual bool hasAnimationData(int32_t gestureId)
	{
		return (appearType->bdAnimData[gestureId] != nullptr);
	}

	virtual Stuff::Vector3D getNodeIdPosition(int32_t nodeId);
};

//***************************************************************************

//***********************************************************************
//
// TreeAppearanceType
//
//***********************************************************************
class TreeAppearanceType : public AppearanceType
{
public:
	TG_TypeMultiShapePtr treeShape[MAX_LODS];
	float lodDistance[MAX_LODS];

	TG_TypeMultiShapePtr treeShadowShape;

	TG_TypeMultiShapePtr treeDmgShape;

	TG_TypeMultiShapePtr treeDmgShadowShape;

	TG_AnimateShapePtr treeAnimData[MAX_BD_ANIMATIONS];
	bool isForestClump;

public:
	void init(void)
	{
		int32_t i = 0;
		for (i = 0; i < MAX_LODS; i++)
		{
			treeShape[i]   = nullptr;
			lodDistance[i] = 0.0f;
		}
		for (i = 0; i < MAX_BD_ANIMATIONS; i++)
			treeAnimData[i] = nullptr;
		treeShadowShape	= nullptr;
		treeDmgShape	   = nullptr;
		treeDmgShadowShape = nullptr;
	}

	TreeAppearanceType(void) { init(void); }

	~TreeAppearanceType(void) { destroy(void); }

	virtual void init(PSTR fileName);

	virtual void destroy(void);
};

//***********************************************************************
//
// TreeAppearance
//
//***********************************************************************
class TreeAppearance : public ObjectAppearance
{
public:
	TreeAppearanceType* appearType;
	TG_MultiShapePtr treeShape;
	TG_MultiShapePtr treeShadowShape;

	float hazeFactor;
	float pitch;
	float yaw;
	int32_t status;

	float OBBRadius;

	int32_t currentLOD;

	bool forceLightsOut;
	bool beenInView;

	bool fogLightSet;
	uint32_t lightRGB;
	uint32_t fogRGB;

public:
	virtual void init(AppearanceTypePtr tree = nullptr, GameObjectPtr obj = nullptr);

	virtual AppearanceTypePtr getAppearanceType(void) { return appearType; }

	TreeAppearance(void) { init(void); }

	virtual int32_t update(bool animate = true);
	virtual int32_t render(int32_t depthFixup = 0);

	virtual int32_t renderShadows(void);

	virtual void destroy(void);

	~TreeAppearance(void) { destroy(void); }

	virtual size_t getAppearanceClass(void) { return TREE_APPR_TYPE; }

	virtual bool recalcBounds(void);

	void setFadeTable(puint8_t fTable) { fadeTable = fTable; }

	virtual void setObjectNameId(int32_t objId) { objectNameId = objId; }

	virtual bool isMouseOver(float px, float py);

	virtual void setObjectParameters(Stuff::Vector3D& pos, float rot, int32_t selected,
		int32_t alignment, int32_t homeRelations);

	virtual void setMoverParameters(
		float pitchAngle, float lArmRot = 0.0f, float rArmRot = 0.0f, bool isAirborne = false);

	virtual void setObjStatus(int32_t oStatus);

	virtual void scale(float scaleFactor) { treeShape->ScaleShape(scaleFactor); }

	virtual float getRadius(void) { return OBBRadius; }

	virtual void setLightsOut(bool lightFlag) { forceLightsOut = lightFlag; }

	virtual bool isForestClump(void) { return appearType->isForestClump; }

	virtual void markTerrain(_ScenarioMapCellInfo* pInfo, int32_t type, int32_t counter);

	virtual Stuff::Point3D getRootNodeCenter(void)
	{
		Stuff::Point3D result = treeShape->GetRootNodeCenter(void);
		return result;
	}

	virtual void markLOS(bool clearIt = false);
};

//***************************************************************************

#endif
