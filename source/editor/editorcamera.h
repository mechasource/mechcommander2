#ifndef EDITORCAMERA_H
#define EDITORCAMERA_H
/*************************************************************************************************\
EditorCamera.h			: Interface for the EditorCamera component.
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//
\*************************************************************************************************/

#ifndef CAMERA_H
#include "Camera.h"
#endif

#ifndef EDITOROBJECTMGR_H
#include "EditorObjectMgr.h"
#endif

#ifndef OBJSTATUS_H
#include "objstatus.h"
#endif

//*************************************************************************************************

/**************************************************************************************************
CLASS DESCRIPTION
EditorCamera:  draws the terrain and objects and stuff
**************************************************************************************************/
extern bool useShadows;
extern bool s_bSensorMapEnabled;
extern bool drawOldWay;
extern bool useFog;

extern MidLevelRenderer::MLRClipper * theClipper;

bool useLOSAngle = false;

class EditorCamera : public Camera
{
	//Data Members
	//-------------
public:

		AppearancePtr			compass;
		AppearancePtr			theSky;
		bool					drawCompass;
		float					lastShadowLightPitch;
		long					cameraLineChanged;
		long					oldSkyNumber;

		EditorCamera (void)
		{
			init();
		}

		virtual void init (void)
		{
			Camera::init();
			compass = NULL;
			drawCompass = true;
			cameraLineChanged = 0;
			theSky = NULL;
		}

	virtual void reset (void)
	{
		//Must toss these between map loads to clear out their texture memory!!
		delete compass;
		compass = NULL;
		
		delete theSky;
		theSky = NULL;
	}
	
 	virtual void render (void)
	{
		//------------------------------------------------------
		// At present, these actually draw.  Later they will 
		// add elements to the draw list and sort and draw.
		// The later time has arrived.  We begin sorting immediately.
		// NO LONGER NEED TO SORT!
		// ZBuffer time has arrived.  Share and Enjoy!
		// Everything SIMPLY draws at the execution point into the zBuffer
		// at the correct depth.  Miracles occur at that point!
		// Big code change but it removes a WHOLE bunch of code and memory!
		
		//--------------------------------------------------------
		// Get new viewport values to scale stuff.  No longer uses
		// VFX stuff for this.  ALL GOS NOW!
		gos_GetViewport(&viewMulX, &viewMulY, &viewAddX, &viewAddY);
		//--------------------------------------------------------
		// Get new viewport values to scale stuff.  No longer uses
		// VFX stuff for this.  ALL GOS NOW!
		screenResolution.x = viewMulX;
		screenResolution.y = viewMulY;
		calculateProjectionConstants();
	
		TG_Shape::SetViewport(viewMulX,viewMulY,viewAddX,viewAddY);
	

		globalScaleFactor = getScaleFactor();
		globalScaleFactor *= viewMulX / Environment.screenWidth;		//Scale Mechs to ScreenRES
		
		//-----------------------------------------------
		// Set Ambient for this pass of rendering	
		DWORD lightRGB = (ambientRed<<16)+(ambientGreen<<8)+ambientBlue;
			
		eye->setLightColor(1,lightRGB);
		eye->setLightIntensity(1,1.0);
	
		MidLevelRenderer::MLRState default_state;
		default_state.SetBackFaceOn();
		default_state.SetDitherOn();
		default_state.SetTextureCorrectionOn();
		default_state.SetZBufferCompareOn();
		default_state.SetZBufferWriteOn();
	
		default_state.SetFilterMode(MidLevelRenderer::MLRState::BiLinearFilterMode);
		
		float z = 1.0f;
		Stuff::RGBAColor fColor;
		fColor.red = (float)((fogColor >> 16) & 0xff);
		fColor.green = (float)((fogColor >> 8) & 0xff);
		fColor.blue = (float)((fogColor) & 0xff);
	
		MidLevelRenderer::PerspectiveMode = usePerspective;
		theClipper->StartDraw(cameraOrigin, cameraToClip, fColor, &fColor, default_state, &z);
		MidLevelRenderer::GOSVertex::farClipReciprocal = (1.0f-cameraToClip(2, 2))/cameraToClip(3, 2);

		if (active && turn > 1)
		{
			if (theSky)
				theSky->render(1);
				
			land->render();								//render the Terrain
	
			//If you ever want craters in the editor, just turn this on.  No way to save 'em though!
			//craterManager->render();					//render the craters and footprints
	
			//Only the GameCamera knows about this.  Heidi, override this function in EditorCamera
			//and have your objectManager draw.

			if (!s_bSensorMapEnabled)
				EditorObjectMgr::instance()->render();		//render all other objects

			land->renderWater();

			if (!s_bSensorMapEnabled && useShadows)
				EditorObjectMgr::instance()->renderShadows();	//render all other objects

			if (!drawOldWay)
			{
				if (compass && (turn > 3) && drawCompass)
					compass->render(-1);		//Force this to zBuffer in front of everything
			}

			if (!drawOldWay)
				mcTextureManager->renderLists();

			//theClipper->RenderNow();		//Draw the FX

			/* The editor interface needs to be drawn last, as it draws things "on top" of the
			rendered scene. */
			if ( EditorInterface::instance() )
			{
				EditorInterface::instance()->render();
				/* We need to call renderLists() again to render the "object placement" cursor
				that, if active, was placed in a render list in the "EditorInterface::instance()->render()"
				call. renderLists() seems to have an automatic mechanism for not redrawing
				things it has already drawn. Pretty much everything else drawn by
				EditorInterface is "rendered immediately" (not placed in a renderList). */
				mcTextureManager->renderLists();
			}
		}
	
	 	//-----------------------------------------------------
		if (drawOldWay)
		{
			gos_SetRenderState( gos_State_ZCompare, 0);
			gos_SetRenderState(	gos_State_ZWrite, 0);
			gos_SetRenderState( gos_State_Perspective, 1);
	
			if (compass && (turn > 3) && drawCompass)
				compass->render();
		}
 	}

	virtual long activate (void)
	{
		// If camera is already active, just return
		if (ready && active)
			return(NO_ERR);
		
		//Can set initial position and stuff here.
		//updateDaylight(true);
		
		lastShadowLightPitch = lightPitch;

		allNormal();

		return NO_ERR;
	}

	virtual long update (void)
	{
		// calculate new near and far plane distance based on 
		// Current altitude above terrain.
		float anglePercent = (projectionAngle - MIN_PERSPECTIVE) / (MAX_PERSPECTIVE - MIN_PERSPECTIVE);
		float testMax = Camera::AltitudeMaximumLo + ((Camera::AltitudeMaximumHi - Camera::AltitudeMaximumLo) * anglePercent);

		float altitudePercent = (cameraAltitude - AltitudeMinimum) / (testMax - AltitudeMinimum);
		Camera::NearPlaneDistance = MinNearPlane + ((MaxNearPlane - MinNearPlane) * altitudePercent);
		Camera::FarPlaneDistance = MinFarPlane + ((MaxFarPlane - MinFarPlane) * altitudePercent);
	
		if (!compass && (turn > 3))	//Create it!
		{
			//Gotta check for the list too because a NEW map has no objects on it and this list
			// Doesn't exist until objects are placed.  Strange but true...
			if ( !appearanceTypeList )
			{
				appearanceTypeList = new AppearanceTypeList;
				gosASSERT(appearanceTypeList != NULL);
				
				appearanceTypeList->init(2048000);
			}

			AppearanceType* appearanceType = appearanceTypeList->getAppearance( BLDG_TYPE << 24, "compass" );
			compass = new BldgAppearance;
			compass->init( appearanceType );
		}
		
		if (!theSky && (turn > 3))
		{
			//Startup the SKYBox
			long appearanceType = (GENERIC_APPR_TYPE << 24);
		
			AppearanceTypePtr genericAppearanceType = NULL;
			genericAppearanceType = appearanceTypeList->getAppearance(appearanceType,"skybox");
			if (!genericAppearanceType)
			{
				char msg[1024];
				sprintf(msg,"No Generic Appearance Named %s","skybox");
				Fatal(0,msg);
			}
			  
			theSky = new GenericAppearance;
			gosASSERT(theSky != NULL);
		
			//--------------------------------------------------------------
			gosASSERT(genericAppearanceType->getAppearanceClass() == GENERIC_APPR_TYPE);
			theSky->init((GenericAppearanceType*)genericAppearanceType, NULL);
			
			theSky->setSkyNumber(EditorData::instance->TheSkyNumber());
			oldSkyNumber = EditorData::instance->TheSkyNumber();
		}

		//Did they change the skyNumber on us?
		if (theSky && (oldSkyNumber != EditorData::instance->TheSkyNumber()))
		{
			theSky->setSkyNumber(EditorData::instance->TheSkyNumber());
			oldSkyNumber = EditorData::instance->TheSkyNumber();
		}
		
		long result = Camera::update();

		if ((cameraLineChanged + 10) < turn)
		{
			if (userInput->getKeyDown(KEY_BACKSLASH) && !userInput->ctrl() && !userInput->alt() && !userInput->shift())
			{
				drawCompass ^= true;
				cameraLineChanged = turn;
			}
		}

		#define MAX_SHADOW_PITCH_CHANGE	(5.0f)
		//Always recalc here or shadows never change in editor!!
		forceShadowRecalc = true;

		if (compass && (turn > 3))
   		{
   	   		bool oldFog = useFog;
   			bool oldShadows = useShadows;
   	   		useFog = false;
   			useShadows = false;
   	   		
   	   		Stuff::Vector3D pos = getPosition();
   	   		compass->setObjectParameters(pos,0.0f,false,0,0);
   	   		compass->setMoverParameters(0.0f);
   	   		compass->setGesture(0);
   	   		compass->setObjStatus(OBJECT_STATUS_DESTROYED);
   	   		compass->setInView(true);
   	   		compass->setVisibility(true,true);
   	   		compass->setFilterState(true);
   			compass->setIsHudElement();
   	   		compass->update();		   //Force it to try and draw or stuff will not work!
			
			if (theSky)
			{
				Stuff::Vector3D pos = getPosition();
				
				theSky->setObjectParameters(pos,0.0f,false,0,0);
				theSky->setMoverParameters(0.0f);
				theSky->setGesture(0);
				theSky->setObjStatus(OBJECT_STATUS_NORMAL);
				theSky->setInView(true);
				theSky->setVisibility(true,true);
				theSky->setFilterState(true);
				theSky->setIsHudElement();
				theSky->update();		   //Force it to try and draw or stuff will not work!
			}
					
   	   		useFog = oldFog;
   			useShadows = oldShadows;
   		}

		return result;
	}


};

//*************************************************************************************************
#endif  // end of file ( EditorCamera.h )
