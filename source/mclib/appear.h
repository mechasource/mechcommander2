//---------------------------------------------------------------------------
//
// Appear.h -- File contains the Basic Game Appearance Declaration
//
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#pragma once

#ifndef APPEAR_H
#define APPEAR_H
//---------------------------------------------------------------------------
// Include Files
#ifndef DAPPEAR_H
#include "dappear.h"
#endif

#ifndef DAPRTYPE_H
#include <daprtype.h>
#endif

#ifndef FLOATHELP_H
#include "floathelp.h"
#endif

#include <stuff/stuff.hpp>
//---------------------------------------------------------------------------
// Macro definitions
#ifndef MAX_ULONG
#define MAX_ULONG		0x70000000		//Must be able to square this and still get big number!!
#endif

#ifndef NO_ERROR
#define NO_ERROR			0
#endif

#define HUD_DEPTH		0.0001f			//HUD Objects draw over everything else.

#define WIDTH			19				// really half the width...
#define HEIGHT			4

#define	BARTEST			0.001f

extern FloatHelpPtr globalFloatHelp;
extern size_t currentFloatHelp;


// would have loved to make object flags, but it looks like we're out...
#define DRAW_NORMAL						0x00
#define DRAW_BARS						0x01
#define DRAW_TEXT						0x02
#define DRAW_BRACKETS					0x04
#define DRAW_COLORED					0x08


struct _ScenarioMapCellInfo;
struct _GameObjectFootPrint;
//---------------------------------------------------------------------------
// Class definitions
class Appearance
{
	//Data Members
	//-------------
	protected:
		Stuff::Vector4D				screenPos;		//Where am I on Screen?  INCLUDES correct Z now!

		bool						visible;		//Current FOW status to help draw
		bool						seen;			//Current FOW status to help draw
		
	public:

		bool						inView;			//Can I be Seen?
		Stuff::Vector4D				upperLeft;		//used to draw select boxes.  Can be 3D Now!
		Stuff::Vector4D				lowerRight;		//used to draw select boxes.
		
		float						barStatus;		//Status Bar Length.
		uint32_t						barColor;		//Status Bar Color.
	
	//Member Functions
	//-----------------
	public:

		PVOID operator new (size_t mySize);
		void operator delete (PVOID us);
			
		Appearance (void)
		{
			inView = FALSE;
			screenPos.x = screenPos.y = screenPos.z = screenPos.w = -999.0f;
			upperLeft.x = upperLeft.y = upperLeft.z = upperLeft.w = -999.0f;
			lowerRight.x = lowerRight.y = lowerRight.z = lowerRight.w = -999.0f;
			barStatus = 1.0;
			barColor = 0x0;

			visible = seen = false;
		}
		
		virtual void init (AppearanceTypePtr tree = NULL, GameObjectPtr obj = NULL)
		{
			inView = FALSE;
			screenPos.x = screenPos.y = screenPos.z = screenPos.w = -999.0f;
			upperLeft.x = upperLeft.y = upperLeft.z = upperLeft.w = -999.0f;
			lowerRight.x = lowerRight.y = lowerRight.z = lowerRight.w = -999.0f;
			barStatus = 1.0;
			barColor = 0x0;

			visible = seen = false;
			(void)tree;(void)obj;
		}
		
		virtual void initFX (void)
		{
		}

		virtual void destroy (void)
		{
			init(void);
		}
		
		virtual ~Appearance (void)
		{
			destroy(void);
		}
		
		virtual int32_t update (bool animate = true)
		{
			//Perform any frame by frame tasks.  Animations, etc.
			(void)animate;
			return NO_ERROR;
		}
		
		virtual int32_t render (int32_t depthFixup = 0)
		{
			//Decide whether or not I can be seen and add me to render list.
			(void)depthFixup;
			return NO_ERROR;
		}

		virtual int32_t renderShadows (void)
		{
			return NO_ERROR;
		}

		virtual AppearanceTypePtr getAppearanceType (void)
		{
			return NULL;
		}

		bool canBeSeen (void)
		{
			return(inView);
		}
		
		void setInView (bool viewStatus)
		{
			inView = viewStatus;
		}
				
		Stuff::Vector4D getScreenPos (void)
		{
			return screenPos;
		}

		virtual bool isMouseOver (float px, float py)
		{
			(void)px; (void)py;
			return FALSE;		//Never over a base appearance
		}
				
		virtual void drawSelectBox (uint32_t color);

		virtual void drawSelectBrackets (uint32_t color);

		virtual void drawBars (void);

		void drawTextHelp (PSTR text);
		void drawTextHelp (PSTR text, size_t color);

		void drawPilotName(PSTR text, size_t color ); // next line below drawTextHelp

		
		virtual bool recalcBounds (void)
		{
			//-------------------------------------------------------
			// returns TRUE is this appearance is Visible this frame
			inView = FALSE;
			return inView;
		}

		virtual void setGesture (size_t /*gestureId*/)
		{
		}

		virtual int32_t setGestureGoal (int32_t /*gestureId*/)
		{
			return 0;
		}
		
		virtual void setVelocityPercentage (float /*percent*/)
		{
		}
		
		virtual int32_t getFrameNumber (void)
		{
			return 0;
		}

		virtual size_t getAppearanceClass (void)
		{
			return BASE_APPEARANCE;
		}
		
		virtual void setBarStatus (float stat)
		{
			barStatus = stat;
			if (barStatus > 1.0f)
				barStatus = 1.0f;
		}

		virtual void setBarColor (uint32_t clr)
		{
			barColor = clr;
		}

		virtual bool getInTransition (void)
		{
			return false;
		}

		virtual int32_t getNumFramesInGesture (int32_t /*gestureId*/)
		{
			return 0;
		}

		virtual int32_t getOldGestureGoal (void)
		{
			return -1;
		}
		
		virtual int32_t getCurrentGestureGoal (void)
		{
			return -1;
		}

		virtual int32_t getCurrentGestureId (void)
		{
			return 0;
		}

		virtual bool isInJump (void)
		{
			return false;
		}

		virtual bool isJumpSetup (void)
		{
			return false;
		}

		virtual bool isJumpAirborne (void)
		{
			return false;
		}

		virtual bool haveFallen (void)
		{
			return false;
		}

		virtual void setObjectNameId (int32_t /*objId*/)
		{
		}

		virtual bool setJumpParameters (Stuff::Vector3D& /*jumpGoal*/)
		{
			return false;
		}

		virtual void setWeaponNodeUsed (int32_t /*nodeId*/)
		{
		}

		virtual int32_t getLowestWeaponNode (void)
		{
			return 0;
		}
		
		virtual int32_t getWeaponNode (int32_t /*weapontype*/)
		{
			return 0;
		}
		
		virtual float getWeaponNodeRecycle (int32_t /*node*/)
		{
			return 0.0f;
		}

		virtual void resetWeaponNodes (void)
		{
		}

		virtual void setWeaponNodeRecycle(int32_t /*nodeId*/, float /*time*/)
		{
		}
		
		virtual Stuff::Vector3D getSmokeNodePosition (int32_t /*nodeId*/)
		{
			Stuff::Vector3D position;
			position.x = position.y = position.z = 0.0f;

			return position;
		}
		
		virtual Stuff::Vector3D getDustNodePosition (int32_t /*nodeId*/)
		{
			Stuff::Vector3D position;
			position.x = position.y = position.z = 0.0f;

			return position;
		}
		
 		virtual Stuff::Vector3D getWeaponNodePosition (int32_t /*node*/)
		{
			Stuff::Vector3D position;
			position.x = position.y = position.z = 0.0f;

			return position;
		}

		virtual Stuff::Vector3D getNodePosition (int32_t /*nodeId*/)
		{
			Stuff::Vector3D position;
			position.x = position.y = position.z = 0.0f;

			return position;
		}
		
		virtual Stuff::Vector3D getNodeNamePosition (PSTR /*nodeName*/)
		{
			Stuff::Vector3D position;
			position.x = position.y = position.z = 0.0f;

			return position;
		}
		
		virtual Stuff::Vector3D getNodeIdPosition (int32_t /*nodeId*/)
		{
			Stuff::Vector3D position;
			position.x = position.y = position.z = 0.0f;

			return position;
		}

		virtual void setCombatMode (bool /*combatMode*/)
		{
		}

		virtual float getVelocityMagnitude (void)
		{
			return 0.0f;
		}

		virtual float getVelocityOfGesture (int32_t /*gestureId*/)
		{
			return 0.0f;
		}


		virtual void setBrake (bool /*brake*/)
		{
		}

		virtual void setObjectParameters (Stuff::Vector3D& /*pos*/, float /*rot*/, int32_t /*selected*/, int32_t /*team*/, int32_t /*homeRelations*/)
		{
		}

		virtual void setMoverParameters (float /*turretRot*/, float lArmRot = 0.0f, float rArmRot = 0.0f, bool isAirborne = false)
		{
			lArmRot; rArmRot; isAirborne;
		}

		virtual void updateFootprints (void)
		{
		}

		virtual void setPaintScheme (void)
		{
		}

		virtual void setPaintScheme (uint32_t /*red*/, uint32_t /*green*/, uint32_t /*blue*/)
		{
		}

		virtual void getPaintScheme (uint32_t& /*red*/, uint32_t& /*green*/, uint32_t& /*blue*/)
		{
		}

		virtual void resetPaintScheme (uint32_t /*red*/, uint32_t /*green*/, uint32_t /*blue*/)
		{
		}
		
		virtual void setDebugMoveMode(void)
		{
		}

		virtual void setSingleStepMode(void)
		{
		}
		
		virtual void setPrevFrame(void)
		{
		}
		
		virtual void setNextFrame(void)
		{
		}

		virtual void setVisibility (bool vis, bool sen)
		{
			visible = vis;
			seen = sen;
		}

		virtual void setSensorLevel (int32_t /*lvl*/)
		{
		}
		
		virtual void hitFront (void)
		{
		}
	
		virtual void hitBack (void)
		{
		}
		
		virtual void hitLeft (void)
		{
		}

		virtual void hitRight (void)
		{
		}
		
		virtual void setObjStatus (int32_t /*oStatus*/)
		{
		}
		
		virtual int32_t calcCellsCovered (Stuff::Vector3D& /*pos*/, pint16_t /*cellList*/)
		{
			return(0);
		}

		virtual void markTerrain (_ScenarioMapCellInfo* /*pInfo*/, int32_t /*type*/, int32_t /*counter*/)
		{
		}
		
		virtual int32_t markMoveMap (bool /*passable*/, int32_t* /*lineOfSightRect*/, bool useheight = false, pint16_t cellList = NULL)
		{
			useheight;cellList;
			return(0);
		}

		virtual void markLOS (bool clearIt = false)
		{
			clearIt;
		}
		
		virtual void scale (float scaleFactor)
		{
			scaleFactor;
		}
		
		virtual bool playDestruction (void)
		{
			return false;
		}
		
		virtual float getRadius (void)
		{
			return 0.0f;
		}
		
		virtual void flashBuilding (float /*duration*/, float /*flashDuration*/, uint32_t /*color*/)
		{
		}
		
		virtual void setHighlightColor( int32_t /*argb*/ )
		{
		}
	
		virtual float getTopZ (void)
		{
			return 0.0f;
		}
		
		virtual void blowLeftArm (void)
		{
		}
		
		virtual void blowRightArm (void)
		{
		}

		virtual void setFilterState (bool /*state*/)
		{
		}
		
		virtual Stuff::Vector3D getVelocity (void)
		{
			Stuff::Vector3D result;
			result.Zero(void);
			return result;
		}

		virtual bool isSelectable(void)
		{
			return true;
		}
		
		virtual void setIsHudElement (void)
		{
		}

		virtual int32_t getObjectNameId (void)
		{
			return -1;
		}

		virtual bool getIsLit (void)
		{
			return false;
		}
		
		virtual void setLightsOut (bool /*lightFlag*/)
		{
		
		}

		virtual bool PerPolySelect (int32_t /*mouseX*/, int32_t /*mouseY*/)
		{
			return true;
		}

		virtual bool isForestClump (void)
		{	
			return false;
		}
		
		virtual Stuff::Point3D getRootNodeCenter (void)
		{
			Stuff::Point3D result;
			result.Zero(void);
			return result;
		}
		
		virtual void setAlphaValue (uint8_t /*aVal*/)
		{
		}

		void drawIcon(  size_t bmpHandle, size_t bmpWidth,
						  size_t bmpHeight, size_t color, 
						  size_t where = 0 );
						  
		virtual void setSkyNumber (int32_t /*skyNum*/)
		{
		}

		virtual void setMechName( PCSTR /*pName*/ ){}
		
		virtual void startSmoking (int32_t /*smokeLvl*/)
		{
		}
		
		virtual void startWaterWake (void)
		{
		}
		
		virtual void stopWaterWake (void)
		{
		}
		
		virtual void playEjection (void)
		{
		}
		
		virtual void startActivity (int32_t /*effectId*/, bool /*loop*/)
		{
		}
		
		virtual void stopActivity (void)
		{
		}

		virtual Stuff::Vector3D getHitNode (void)
		{
			Stuff::Point3D result;
			result.Zero(void);
			return result;
		}

		virtual Stuff::Vector3D getHitNodeLeft (void)
		{
			Stuff::Point3D result;
			result.Zero(void);
			return result;
		}

		virtual Stuff::Vector3D getHitNodeRight (void)
		{
			Stuff::Point3D result;
			result.Zero(void);
			return result;
		}

		virtual bool getRightArmOff (void)
		{
			return false;
		}

		virtual bool getLeftArmOff (void)
		{
			return false;
		}

		virtual bool hasAnimationData (int32_t /*gestureId*/)
		{
			return false;
		}
};		

//---------------------------------------------------------------------------
#endif
