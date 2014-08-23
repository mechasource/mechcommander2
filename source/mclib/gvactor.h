//---------------------------------------------------------------------------
//
//	gvactor.h - This file contains the header for the Ground Vehicle Actor class
//
//	MechCommander 2
//
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#ifndef GVACTOR_H
#define GVACTOR_H

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

#include <gosfx/gosfxheaders.hpp>
//**************************************************************************************
#ifndef NO_ERROR
#define NO_ERROR						0
#endif

#define	GV_PART_BODY				0
#define	GV_PART_TURRET				1
#define	NUM_GV_PARTS				2

#define MAX_GV_ANIMATIONS			10
//***********************************************************************
//
// GVAppearanceType
//
//***********************************************************************
class GVAppearanceType : public AppearanceType
{
	public:
	
		TG_TypeMultiShapePtr		gvShape[MAX_LODS];
		float						lodDistance[MAX_LODS];
		
		TG_TypeMultiShapePtr		gvShadowShape;
		
 		TG_TypeMultiShapePtr		gvDmgShape;
		
		char						destructEffect[60];
		
		TG_AnimateShapePtr			gvAnimData[MAX_GV_ANIMATIONS];
		bool						gvAnimLoop[MAX_GV_ANIMATIONS];
		bool						gvReverse[MAX_GV_ANIMATIONS];
		bool						gvRandom[MAX_GV_ANIMATIONS];
		int32_t						gvStartF[MAX_GV_ANIMATIONS];
 		
		char						rotationalNodeId[TG_NODE_ID];
		
		int32_t						numSmokeNodes;		//Where damage smoke comes from.
		int32_t						numWeaponNodes;		//Where weapons fire from.
		int32_t						numFootNodes;		//Where dust trail, contrail comes out of.
		NodeData					*nodeData;
		
		static TG_TypeMultiShapePtr	SensorTriangleShape;
		static TG_TypeMultiShapePtr	SensorCircleShape;
		
	public:
	
		void init (void)
		{
			int32_t i=0;
			for (i=0;i<MAX_LODS;i++)
			{
				gvShape[i] = NULL;
				lodDistance[i] = 0.0f;
			}
			
			gvShadowShape = NULL;
			
			gvDmgShape = NULL;

			for (i=0;i<MAX_GV_ANIMATIONS;i++)
			{
				gvAnimData[i] = NULL;
				gvAnimLoop[i] = false;
				gvReverse[i] = false;
				gvRandom[i] = false;
				gvStartF[i] = 0;			
			}
				
			destructEffect[0] = 0;
		}
	
		GVAppearanceType (void)
		{
			init();
		}

		~GVAppearanceType (void)
		{
			destroy();
		}

		virtual void init (PSTR fileName);
		
		virtual void destroy (void);
		
		void setAnimation (TG_MultiShapePtr shape, ULONG animationNum);
		
		int32_t getNumFrames (int32_t animationNum)
		{
			if ((animationNum >= 0) && (animationNum < MAX_GV_ANIMATIONS) && (gvAnimData[animationNum]))
				return gvAnimData[animationNum]->GetNumFrames();

			return 0.0f;
		}

		float getFrameRate (int32_t animationNum)
		{
			if ((animationNum >= 0) && (animationNum < MAX_GV_ANIMATIONS) && (gvAnimData[animationNum]))
				return gvAnimData[animationNum]->GetFrameRate();

			return 0.0f;
		}

		bool isReversed (int32_t animationNum)
		{
			if ((animationNum >= 0) && (animationNum < MAX_GV_ANIMATIONS) && (gvAnimData[animationNum]))
				return gvReverse[animationNum];

			return false;
		}
		
		bool isLooped (int32_t animationNum)
		{
			if ((animationNum >= 0) && (animationNum < MAX_GV_ANIMATIONS) && (gvAnimData[animationNum]))
				return gvAnimLoop[animationNum];

			return false;
		}
		
		bool isRandom (int32_t animationNum)
		{
			if ((animationNum >= 0) && (animationNum < MAX_GV_ANIMATIONS) && (gvAnimData[animationNum]))
				return gvRandom[animationNum];

			return false;
		}
		
		int32_t getTotalNodes (void)
		{
			return numSmokeNodes + numWeaponNodes + numFootNodes;
		}

};

//***********************************************************************
//
// GVAppearance
//
//***********************************************************************
class GVAppearance : public ObjectAppearance
{
	public:

		GVAppearanceType*							appearType;
		
		TG_MultiShapePtr							gvShape;
		TG_MultiShapePtr							gvShadowShape;
		
		TG_MultiShapePtr							sensorCircleShape;
		TG_MultiShapePtr							sensorTriangleShape;
		float										sensorSpin;
									
		int32_t										objectNameId;
	
		float										turretRotation;
		float										pitch;
		float										roll;
		float										actualTurretRotation;
		float										velocityMagnitude;
		bool										inDebugMoveMode;
		int32_t										sensorLevel;
		float										hazeFactor;
		int32_t										status;
			
   		gosFX::Effect								*destructFX;
   		gosFX::Effect								*waterWake;
   		gosFX::Effect								*dustCloud;
		gosFX::Effect								*activity;
		bool										isWaking;
		bool										isActivitying;
		bool										movedThisFrame;
		bool										dustCloudStart;
		
		float										OBBRadius;
		
		int32_t										gvAnimationState;
		float										currentFrame;
		float										gvFrameRate;
		bool										isReversed;
		bool										isLooping;
		bool										setFirstFrame;
		bool										canTransition;
		bool										isInfantry;
		
		int32_t										currentLOD;
		
 		int32_t										*nodeUsed;				//Used to stagger the weapon nodes for firing.
		float										*nodeRecycle;			//Used for ripple fire to find out if the node has fired recently.
		
		ULONG										localTextureHandle;
		
		ULONG										psRed;
		ULONG										psBlue;
		ULONG										psGreen;

		float										flashDuration;
		float										duration;
		float										currentFlash;
		bool										drawFlash;
		ULONG										flashColor;
		
		int32_t										rotationalNodeIndex;
		int32_t										dustNodeIndex;
		int32_t										activityNodeIndex;
		int32_t										hitNodeId;
		int32_t										weaponNodeId[4];

 	public:

		virtual void init (AppearanceTypePtr tree = NULL, GameObjectPtr obj = NULL);

		virtual AppearanceTypePtr getAppearanceType (void)
		{
			return appearType;
		}

		GVAppearance (void)
		{
			init();
		}

		virtual int32_t update (bool animate = true);
		void updateGeometry (void);
		
		virtual int32_t render (int32_t depthFixup = 0);

		virtual int32_t renderShadows (void);
		
		virtual void destroy (void);

		virtual size_t getAppearanceClass (void)
		{
			return VEHICLE_APPR_TYPE;
		}
	
		~GVAppearance (void)
		{
			destroy();
		}

		virtual void setPaintScheme (void);

		virtual void setPaintScheme (ULONG red, ULONG green, ULONG blue);

		virtual void getPaintScheme (ULONG &red, ULONG &green, ULONG &blue);

		virtual void resetPaintScheme (ULONG red, ULONG green, ULONG blue);
		
 		virtual bool recalcBounds (void);

		virtual void flashBuilding (float duration, float flashDuration, ULONG color);

		
		void setFadeTable (PUCHAR fTable)
		{
			fadeTable = fTable;
		}
		
		virtual void setObjectNameId (int32_t objId)
		{
			objectNameId = objId;
		}

		virtual bool isMouseOver (float px, float py);
		
		virtual void setObjectParameters (Stuff::Vector3D &pos, float rot, int32_t selected, int32_t team, int32_t homeRelations);

		virtual void setMoverParameters (float turretRot, float lArmRot = 0.0f, float rArmRot = 0.0f, bool isAirborne = false);

		void debugUpdate (void);

		virtual void setSensorLevel (int32_t lvl)
		{
			sensorLevel = lvl;
		}
		
		virtual void setObjStatus (int32_t oStatus);
		
		virtual bool playDestruction (void);
		
		virtual bool getInTransition (void)
		{
			return (canTransition == false);
		}

		virtual void setGesture (size_t gestureId);
		
		virtual int32_t getCurrentGestureId (void)
		{
			return gvAnimationState;
		}

		//--------------------------------------------
		// Once site Objects are in place, go get 'em
		virtual void setWeaponNodeUsed (int32_t nodeId);
		
		virtual Stuff::Vector3D getWeaponNodePosition (int32_t nodeId);
		
		virtual Stuff::Vector3D getSmokeNodePosition (int32_t nodeId);
		
		virtual Stuff::Vector3D getDustNodePosition (int32_t nodeId);
		
		virtual int32_t getWeaponNode (int32_t weapontype);
		
		virtual float getWeaponNodeRecycle (int32_t node);

		virtual int32_t getLowestWeaponNode (void);
		
		virtual Stuff::Vector3D getNodeNamePosition (PSTR nodeName);
		
 		virtual bool PerPolySelect (int32_t mouseX, int32_t mouseY);
		
		virtual Stuff::Point3D getRootNodeCenter (void)
		{
			Stuff::Point3D result = gvShape->GetRootNodeCenter();
			return result;
		}
		
		virtual void setAlphaValue (UCHAR aVal)
		{
			gvShape->SetAlphaValue(aVal);
			
			//Sensor shape fades in opposite direction from mover
			sensorCircleShape->SetAlphaValue(0xff - aVal);
			sensorTriangleShape->SetAlphaValue(0xff - aVal);
		}

		virtual void scale (float scaleFactor)
		{
			gvShape->ScaleShape(scaleFactor);
		}
		
		virtual void startWaterWake (void);
		virtual void stopWaterWake (void);
		
		virtual void startActivity (int32_t effectId, bool loop);
		virtual void stopActivity (void);

		virtual Stuff::Vector3D getNodeIdPosition (int32_t nodeId);

		virtual Stuff::Vector3D getHitNode (void);
};

//***************************************************************************

#endif


