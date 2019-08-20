//---------------------------------------------------------------------------
//
//	Genactor.h - This file contains the header for the generic 3D appearance
// class
//
//	MechCommander 2
//
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#ifndef GENACTOR_H
#define GENACTOR_H

//---------------------------------------------------------------------------
// Include files
#ifndef APPEAR_H
#include "appear.h"
#endif

#ifndef APPRTYPE_H
#include "apprtype.h"
#endif

#ifndef MSL_H
#include "msl.h"
#endif

#ifndef OBJECTAPPEARANCE_H
#include "ObjectAppearance.h"
#endif

//**************************************************************************************
#ifndef NO_ERROR
#define NO_ERROR 0
#endif

#define MAX_GEN_ANIMATIONS 5

//***********************************************************************
//
// GenericAppearanceType
//
//***********************************************************************
class GenericAppearanceType : public AppearanceType
{
public:
	TG_TypeMultiShapePtr genShape;
	TG_TypeMultiShapePtr genDmgShape;

	TG_AnimateShapePtr genAnimData[MAX_GEN_ANIMATIONS];
	bool genAnimLoop[MAX_GEN_ANIMATIONS];
	bool genReverse[MAX_GEN_ANIMATIONS];
	bool genRandom[MAX_GEN_ANIMATIONS];
	int32_t genStartF[MAX_GEN_ANIMATIONS];

	char rotationalNodeId[TG_NODE_ID];
	char textureName[50];
	uint32_t dotRGB;

public:
	void init(void)
	{
		genShape = nullptr;
		for (size_t i = 0; i < MAX_GEN_ANIMATIONS; i++)
			genAnimData[i] = nullptr;
		textureName[0] = 0;
		dotRGB = 0x00ffffff;
	}

	GenericAppearanceType(void) { init(void); }

	~GenericAppearanceType(void) { destroy(void); }

	void setAnimation(TG_MultiShapePtr shape, uint32_t animationNum);

	int32_t getNumFrames(int32_t animationNum)
	{
		if ((animationNum >= 0) && (animationNum < MAX_GEN_ANIMATIONS) && (genAnimData[animationNum]))
			return genAnimData[animationNum]->GetNumFrames(void);
		return 0.0f;
	}

	float getFrameRate(int32_t animationNum)
	{
		if ((animationNum >= 0) && (animationNum < MAX_GEN_ANIMATIONS) && (genAnimData[animationNum]))
			return genAnimData[animationNum]->GetFrameRate(void);
		return 0.0f;
	}

	bool isReversed(int32_t animationNum)
	{
		if ((animationNum >= 0) && (animationNum < MAX_GEN_ANIMATIONS) && (genAnimData[animationNum]))
			return genReverse[animationNum];
		return false;
	}

	bool isLooped(int32_t animationNum)
	{
		if ((animationNum >= 0) && (animationNum < MAX_GEN_ANIMATIONS) && (genAnimData[animationNum]))
			return genAnimLoop[animationNum];
		return false;
	}

	bool isRandom(int32_t animationNum)
	{
		if ((animationNum >= 0) && (animationNum < MAX_GEN_ANIMATIONS) && (genAnimData[animationNum]))
			return genRandom[animationNum];
		return false;
	}

	virtual void init(PSTR fileName);

	virtual void destroy(void);
};

//***********************************************************************
//
// GenericAppearance
//
//***********************************************************************
class GenericAppearance : public ObjectAppearance
{
public:
	GenericAppearanceType* appearType;
	TG_MultiShapePtr genShape;

	int32_t genAnimationState;
	float currentFrame;
	float genFrameRate;
	bool isReversed;
	bool isLooping;
	bool setFirstFrame;
	bool canTransition;

	float hazeFactor;
	float pitch;

	int32_t status;

	float OBBRadius;

	int32_t skyNumber;

public:
	virtual void init(AppearanceTypePtr tree = nullptr, GameObjectPtr obj = nullptr);

	virtual AppearanceTypePtr getAppearanceType(void) { return appearType; }

	void changeSkyToSkyNum(PSTR txmName, PSTR newName);

	GenericAppearance(void) { init(void); }

	virtual int32_t update(bool animate = true);
	virtual int32_t render(int32_t depthFixup = 0);

	virtual int32_t renderShadows(void);

	virtual void destroy(void);

	~GenericAppearance(void) { destroy(void); }

	virtual bool recalcBounds(void);

	virtual bool getInTransition(void) { return (canTransition == false); }

	virtual void setGesture(size_t gestureId);

	virtual int32_t getCurrentGestureId(void) { return genAnimationState; }

	virtual size_t getAppearanceClass(void) { return GENERIC_APPR_TYPE; }

	virtual void setObjectNameId(int32_t objId) { objectNameId = objId; }

	virtual bool isMouseOver(float px, float py);

	virtual void setObjectParameters(Stuff::Vector3D& pos, float rot, int32_t selected,
		int32_t alignment, int32_t homeRelations);

	virtual void setMoverParameters(
		float turretRot, float lArmRot = 0.0f, float rArmRot = 0.0f, bool isAirborne = false);

	virtual void setObjStatus(int32_t oStatus);

	virtual void markTerrain(_ScenarioMapCellInfo* pInfo, int32_t type, int32_t counter);

	virtual void markMoveMap(bool passable);

	virtual void setIsHudElement(void) { genShape->SetIsHudElement(void); }

	virtual void scale(float scaleFactor) { genShape->ScaleShape(scaleFactor); }

	virtual Stuff::Point3D getRootNodeCenter(void)
	{
		Stuff::Point3D result = genShape->GetRootNodeCenter(void);
		return result;
	}

	virtual void setSkyNumber(int32_t skyNum);
};

//***************************************************************************

#endif
