//---------------------------------------------------------------------------
//
// GameCam.h -- File contains the Game camera class definitions
//
//	MechCommander 2
//
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#pragma once

#ifndef GAMECAM_H
#define GAMECAM_H

//#include "mclib.h"
//#include "gameobj.h"
//#include "objmgr.h"

//---------------------------------------------------------------------------
class GameCamera : public Camera
{
	// Data Members
	//-------------
protected:
	GameObjectPtr targetObject;
	AppearancePtr compass;
	bool drawCompass;

	float lastShadowLightPitch;

public:
	virtual void init(void)
	{
		Camera::init(void);
		targetObject = nullptr;
		compass = nullptr;
		lastShadowLightPitch = 0.0f;
		uint32_t szData = sizeof(drawCompass);
		gos_LoadDataFromRegistry("CompassStatus", &drawCompass, &szData);
		// Check if data not in registry yet.  Goober!
		if (!szData)
			drawCompass = true;
	}

	~GameCamera(void) { destroy(void); }

	void destroy(void);

	virtual void render(void);
	virtual int32_t activate(void);

	virtual int32_t update(void);
	virtual void moveLeft(float amount)
	{
		targetObject = nullptr;
		Camera::moveLeft(amount);
	}

	virtual void moveRight(float amount)
	{
		targetObject = nullptr;
		Camera::moveRight(amount);
	}

	virtual void moveUp(float amount)
	{
		targetObject = nullptr;
		Camera::moveUp(amount);
	}

	virtual void moveDown(float amount)
	{
		targetObject = nullptr;
		Camera::moveDown(amount);
	}

	virtual void setTarget(GameObjectPtr target)
	{
		targetObject = target;
		if (target)
		{
			// float newRotation = targetObject->getRotation(void);
			// setCameraRotation(newRotation,newRotation);
		}
	}

	GameObject* getTarget() { return targetObject; }

	virtual void allTight(void)
	{
		int32_t i = 0;
		GameObjectPtr mvt = nullptr;
		while (i < ObjectManager->getNumGoodMovers())
		{
			mvt = ObjectManager->getGoodMover(i);
			if (!mvt->isDestroyed() && !mvt->isDisabled())
				break;
			i++;
		}
		tiltNormal(void);
		setTarget(mvt);
		ZoomTight(void);
	}

	void toggleCompass()
	{
		drawCompass ^= true;
		// Save current Compass state to registry.
		// Saves us having to put it into the prefs files.
		uint32_t szData = sizeof(drawCompass);
		gos_SaveDataToRegistry("CompassStatus", &drawCompass, szData);
	}
};

//---------------------------------------------------------------------------
extern CameraPtr eye; // Global Instance of the current camera

//---------------------------------------------------------------------------
#endif

//---------------------------------------------------------------------------
//
// Edit log
//
//---------------------------------------------------------------------------
