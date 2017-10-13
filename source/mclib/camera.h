//---------------------------------------------------------------------------
//
// Camera.h -- File contains the camera class definitions
//
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#ifndef CAMERA_H
#define CAMERA_H
//---------------------------------------------------------------------------
// Include Files

#ifndef DCAMERA_H
#include "dcamera.h"
#endif

#ifndef TERRAIN_H
#include <terrain.h>
#endif

#ifndef MATHFUNC_H
#include "mathfunc.h"
#endif

#ifndef TGL_H
#include "tgl.h"
#endif

#ifndef INIFILE_H
#include "inifile.h"
#endif

#include <stuff/stuff.hpp>

inline int16_t float2short(float _in)
{
#if 1
	return int16_t(floor(_in));
#else
	_in -= 0.5f;
	_in += 12582912.0f;
	return (*(int16_t*)&_in);
#endif
}

extern float zero;
extern float one;
extern float point1;

//---------------------------------------------------------------------------
// Also defined in Stuff but not getting into here somehow.
enum Axes
{
	X_Axis,
	Y_Axis,
	Z_Axis,
	W_Axis
};

#define MAX_LODS 3

#define MAX_VIEWS 4
#define F2_VIEW 0
#define F3_VIEW 1
#define F4_VIEW 2
#define F5_VIEW 3

//---------------------------------------------------------------------------
class Camera
{
	// Data Members
	//-------------

  protected:
	float projectionAngle; // Angle of orthogonal projection

	Stuff::Vector3D screenResolution; // Resolution of screen in pixels
	Stuff::Vector3D screenCenter;	 // Center coordinate of screen
	Stuff::Vector3D lookVector;		  // Direction camera is looking.
	Stuff::Vector3D physicalPos; // Actual Physical Position of camera in world.

	CameraClass cameraClass; // Type of camera.w

	// data for Camera Movement
	Stuff::Vector3D position;  // Position of camera in 3Space.
	float cameraRotation;	  // Current Direction camera is looking.
	float worldCameraRotation; // Current Direction camera is facing in World
							   // Frame of reference.

	float zoomLevelLODScale[MAX_LODS]; // Data to help calc which LOD at which
									   // zoom.

	Stuff::LinearMatrix4D cameraOrigin; // Translation and rotation of Camera
	Stuff::LinearMatrix4D worldToCameraMatrix; // Inverse of the above.

	Stuff::YawPitchRange cameraDirection; // Direction camera is looking.

	Stuff::Vector2DOf<float> cameraShift; // Position camera is looking At.

	Stuff::Matrix4D
		cameraToClip; // Camera Clip Matrix--Used for projection and zoom.
	Stuff::Matrix4D worldToClip; // Matrix used to bring a point from world
								 // space to camera/clip space
	Stuff::Matrix4D clipToWorld; // Matrix used to bring a point from
								 // camera/clip space to world space

	float startZInverse; // Used to help interpolate the Screen Coords in
						 // InverseProjectZ
	float startWInverse;
	float zPerPixel;
	float wPerPixel;

	TG_LightPtr* worldLights; // Lighting for the entire world.
	int32_t numLights;		  // Number of lights in the above list.  Always
					   // MAX_LIGHTS!

	TG_LightPtr* activeLights; // This is the light list to process every frame.
	int32_t numActiveLights;   // Number of lights active.  Actually Correct.

	TG_LightPtr* terrainLights; // This is the light list to process every frame
								// for TERRAIN ONLY
	int32_t numTerrainLights; // Number of lights active for terrain.  Actually
							  // Correct.

	// Camera Scripting stuff
	Stuff::Vector3D goalPosition;
	Stuff::Vector3D lookPosition;
	float goalPosTime;

	Stuff::Vector3D goalRotation;
	float goalRotTime;

	float goalFOV;
	float goalFOVTime;

	Stuff::Vector3D velocity;
	Stuff::Vector3D goalVelocity;
	float goalVelTime;

	int32_t lookTargetObject;

	float letterBoxPos;
	float letterBoxTime;
	uint8_t letterBoxAlpha;

	bool startEnding;

	bool inFadeMode;
	uint32_t fadeColor;
	uint32_t fadeAlpha;
	uint32_t fadeStart;
	float timeLeftToFade;
	float startFadeTime;

  public:
	bool active;		 // Is camera active (ie. drawing itself)
	bool ready;			 // Is camera ready to be activated?
	bool usePerspective; // Switch camera from Parallel to perspective view

	Stuff::Vector3D lightDirection; // Direction of Spot Light

	float lightYaw; // Direction of Light in Azimuth, elevation
	float lightPitch;

	float lightFinalYaw;
	float lightFinalPitch;
	float lightTimeToFinal;

	float newScaleFactor; // Smooth Zooming
	float camera_fov;	 // Smooth Perspective zooming.
	float cosHalfFOV;	 // Cosine of half the FOV for view cone.

	float viewMulX, viewMulY, // GOS Viewport data variables
		viewAddX, viewAddY;

	uint8_t lightRed;   // Red component of World Light
	uint8_t lightGreen; // Green component of World Light
	uint8_t lightBlue;  // Blue component of World Light

	uint8_t ambientRed;   // Red component of World Light
	uint8_t ambientGreen; // Green component of World Light
	uint8_t ambientBlue;  // Blue component of World Light

	bool terrainShadowColorEnabled;
	uint8_t terrainShadowRed;   // Red component of World Light
	uint8_t terrainShadowGreen; // Green component of World Light
	uint8_t terrainShadowBlue;  // Blue component of World Light

	// Day/Night transition data.
	float dayLightPitch;

	uint8_t dayLightRed;   // Red component of World Light
	uint8_t dayLightGreen; // Green component of World Light
	uint8_t dayLightBlue;  // Blue component of World Light

	uint8_t dayAmbientRed;   // Red component of World Light
	uint8_t dayAmbientGreen; // Green component of World Light
	uint8_t dayAmbientBlue;  // Blue component of World Light

	uint8_t sunsetLightRed;   // Red component of World Light
	uint8_t sunsetLightGreen; // Green component of World Light
	uint8_t sunsetLightBlue;  // Blue component of World Light

	uint8_t nightLightRed;   // Red component of World Light
	uint8_t nightLightGreen; // Green component of World Light
	uint8_t nightLightBlue;  // Blue component of World Light

	uint8_t nightAmbientRed;   // Red component of World Light
	uint8_t nightAmbientGreen; // Green component of World Light
	uint8_t nightAmbientBlue;  // Blue component of World Light

	bool terrainLightNight; // What state are the terrain lights in?
	bool terrainLightCalc;  // Should we activate all terrain lights this frame
						   // and burn their lights in?

	bool isNight; // Flag indicating if we are in NightMode
	// Based solely on light angle.

	float nightFactor; // How "night" is it?  1.0f being full night, 0.0f being
					   // full day!

	float day2NightTransitionTime; // Time in Seconds that light goes from day
								   // to night.
	float dayLightTime;		// Current dayToNight Transition time.
	bool forceShadowRecalc; // Has the sun/moon moved enough for shadows to have
							// changed?

	uint8_t seenRed;   // Red component of World Light
	uint8_t seenGreen; // Green component of World Light
	uint8_t seenBlue;  // Blue component of World Light

	uint8_t baseRed;   // Red component of World Light
	uint8_t baseGreen; // Green component of World Light
	uint8_t baseBlue;  // Blue component of World Light

	float cameraShiftZ;
	float goalPositionZ;

	float fogStart;		   // Altitude at which fog starts
	float fogFull;		   // Altitude at which fog is Full;
	uint32_t dayFogColor;  // Color of FOG.
	float fogTransparency; // How much of the sky color will show through
	uint32_t fogColor;	 // Color of FOG.
	float cameraAltitude;
	float cameraAltitudeDesired; // What would I like the camera to be at!
								 // Maybe smaller due to low angle!

	static float globalScaleFactor; // Global Rescale Factor.

	static float MaxClipDistance;
	static float MinHazeDistance;
	static float DistanceFactor;

	static float MinNearPlane;
	static float MaxNearPlane;

	static float MinFarPlane;
	static float MaxFarPlane;

	static float HazeFactor;

	static float verticalSphereClipConstant;
	static float horizontalSphereClipConstant;
	static float NearPlaneDistance;
	static float FarPlaneDistance;

	static float AltitudeMinimum;
	static float AltitudeMaximumLo;
	static float AltitudeMaximumHi;
	static float AltitudeTight;
	static float AltitudeDefault;
	static float MaxLetterBoxTime;

	static float MIN_PERSPECTIVE;
	static float MAX_PERSPECTIVE;
	static float MIN_ORTHO;
	static float MAX_ORTHO;

	static bool inMovieMode;
	static bool forceMovieEnd;
	static float MaxLetterBoxPos;

	static float cameraTilt[MAX_VIEWS];
	static float cameraZoom[MAX_VIEWS];

	static frameOfRef cameraFrame;

	// Member Functions
	//-----------------

  public:
	virtual void init(void)
	{
		projectionAngle	= 30;
		screenResolution.x = 400.0;
		screenResolution.y = 400.0;
		position.Zero(void);
		ready = active = FALSE;
		cameraClass	= INVALID_CAMERA;
		setCameraRotation(0.0, 0.0);
		cameraShiftZ = goalPositionZ = 0.0f;
		lightDirection.Zero(void);
		newScaleFactor = 0.5;
		cameraShift.x = cameraShift.y = 0.0;
		worldLights					  = nullptr;
		activeLights				  = nullptr;
		terrainLights				  = nullptr;
		fogStart = fogFull = 0.0;
		dayFogColor		   = 0xffffffff;
		fogTransparency	= 1.0;
		fogColor		   = 0xffffffff;
		usePerspective	 = false;
		viewMulX = viewMulY = viewAddX = viewAddY = 0.0f;
		isNight									  = false;
		forceShadowRecalc						  = false;
		goalPosition.Zero(void);
		lookPosition.Zero(void);
		goalPosTime	= 0.0;
		goalRotation.x = goalRotation.y = goalRotation.z = 0.0;
		goalRotTime										 = 0.0;
		goalFOV											 = 0.0;
		goalFOVTime										 = 0.0;
		velocity.Zero(void);
		goalVelocity.Zero(void);
		goalVelTime			  = 0.0;
		lookTargetObject	  = -1;
		terrainLightNight	 = false;
		terrainLightCalc	  = true;
		letterBoxPos		  = 0.0f;
		letterBoxTime		  = 0.0f;
		letterBoxAlpha		  = 0x0;
		startEnding			  = false;
		inFadeMode			  = false;
		fadeColor			  = 0x0;
		fadeAlpha			  = 0x0;
		timeLeftToFade		  = 0.0f;
		startFadeTime		  = 0.0f;
		cameraAltitude		  = 1200.0f;
		cameraAltitudeDesired = 1200.0f;
		nightFactor			  = 0.0f;
	}

	Camera(void) { init(void); }

	int32_t init(FitIniFile* cameraFile);

	void destroy(void);

	~Camera(void) { destroy(void); }

	float getProjectionAngle(void) { return (projectionAngle); }

	void setCameraRotation(float angle, float angleWorld);
	float getCameraRotation(void);

	//---------------------------------------------------------------------------
	bool projectZ(Stuff::Vector3D& point, Stuff::Vector4D& screen)
	{
		//--------------------------------------------------------------------
		// Now run the NEW project code
		Stuff::Vector4D xformCoords;
		Stuff::Point3D coords;
		coords.x = -point.x;
		coords.y = point.z;
		coords.z = point.y;
		xformCoords.Multiply(coords, worldToClip);
		if (usePerspective)
		{
			//---------------------------------------
			// Perspective Transform
			float rhw = 1.0f;
			if (xformCoords.w != 0.0f)
				rhw = 1.0f / xformCoords.w;
			screen.x = (xformCoords.x * rhw) * viewMulX + viewAddX;
			screen.y = (xformCoords.y * rhw) * viewMulY + viewAddY;
			screen.z = (xformCoords.z * rhw);
			screen.w = fabs(rhw);
		}
		else
		{
			//---------------------------------------
			// Parallel Transform
			screen.x = (1.0f - xformCoords.x) * viewMulX + viewAddX;
			screen.y = (1.0f - xformCoords.y) * viewMulY + viewAddY;
			screen.z = xformCoords.z;
			screen.w = 0.000001f;
		}
		if ((screen.x < 0) || (screen.y < 0) ||
			(screen.x > screenResolution.x) || (screen.y > screenResolution.y))
			return FALSE;
		return TRUE;
	}

	void inverseProjectZ(Stuff::Vector4D& screen, Stuff::Vector3D& point);

	void projectCamera(Stuff::Vector3D& point);

	size_t inverseProject(
		Stuff::Vector2DOf<int32_t>& screenPos, Stuff::Vector3D& point);

	void getClosestVertex(
		Stuff::Vector2DOf<int32_t>& screenPos, int32_t& row, int32_t& col);

	void setOrthogonal(void);
	virtual void setCameraOrigin(void);
	void calculateProjectionConstants(void);
	void calculateTopViewConstants(void);

	void changeResolution(Stuff::Vector3D newRes)
	{
		screenResolution = newRes;
		calculateProjectionConstants(void);
	}

	void prepareBackground(void);

	CameraClass getClass(void) { return cameraClass; }

	void setClass(CameraClass newClass) { cameraClass = newClass; }

	Stuff::Vector3D getPosition(void) { return position; }

	void updateDaylight(bool bInitialize = false);

	virtual int32_t update(void);
	virtual void render(void);

	virtual int32_t activate(void);
	virtual void reset(void) {}

	void deactivate(void);

	void setLightColor(int32_t index, uint32_t argb)
	{
		if ((index >= 0) && (index < numLights) && worldLights[index])
			worldLights[index]->SetaRGB(argb);
	}

	uint32_t getLightColor(int32_t index)
	{
		if ((index >= 0) && (index < numLights) && worldLights[index])
			return worldLights[index]->GetaRGB(void);
		return 0x00ffffff;
	}

	void setLightIntensity(int32_t index, float intensity)
	{
		if ((index >= 0) && (index < numLights) && worldLights[index])
			worldLights[index]->intensity = intensity;
	}

	// Return the ACTIVE lights.  NOT the total light list!!
	TG_LightPtr getWorldLight(int32_t index)
	{
		if ((index >= 0) && (index < numActiveLights) && activeLights[index])
			return activeLights[index];
		return nullptr;
	}

	TG_LightPtr* getWorldLights(void) { return activeLights; }

	int32_t getNumLights(void) { return numActiveLights; }

	TG_LightPtr getTerrainLight(int32_t index)
	{
		if ((index >= 0) && (index < numTerrainLights) && terrainLights[index])
			return terrainLights[index];
		return nullptr;
	}

	TG_LightPtr* getTerrainLights(void) { return terrainLights; }

	int32_t getNumTerrainLights(void) { return numTerrainLights; }

	int32_t addWorldLight(TG_LightPtr light)
	{
		numLights		= MAX_LIGHTS_IN_WORLD;
		bool lightAdded = false;
		for (size_t i = 0; i < MAX_LIGHTS_IN_WORLD; i++)
		{
			if (!worldLights[i])
			{
				worldLights[i] = light;
				lightAdded	 = true;
				return i;
			}
		}
		return -1;
	}

	void removeWorldLight(uint32_t lightNum, TG_LightPtr light)
	{
		if ((lightNum < MAX_LIGHTS_IN_WORLD) &&
			(worldLights[lightNum] == light))
		{
			worldLights[lightNum] =
				nullptr; // Up to class that created light to free it!!!!!!
			return;
		}
		// If we get here, the light we passed in either had an invalid index OR
		// did not match the pointer
		// at the location we passed in.  Scan all lights for a match and toss
		// any and all that do match
		for (size_t i = 0; i < MAX_LIGHTS_IN_WORLD; i++)
		{
			if (worldLights[i] == light)
				worldLights[i] = nullptr;
		}
	}

	Stuff::Vector3D getScreenRes(void) { return screenResolution; }

	bool getIsNight(void) { return isNight; }

	float getNightFactor(void) { return nightFactor; }

	int32_t getScreenResX(void) { return float2long(screenResolution.x); }

	int32_t getScreenResY(void) { return float2long(screenResolution.y); }

	float fgetScreenResX(void) { return screenResolution.x; }

	float fgetScreenResY(void) { return screenResolution.y; }

	float getScaleFactor(void) { return newScaleFactor; }

	int32_t getScaleLOD(void)
	{
		for (size_t i = 0; i < MAX_LODS; i++)
		{
			if (newScaleFactor >= zoomLevelLODScale[i])
				return i;
		}
		return (MAX_LODS - 1); // ALWAYS return worst case if necessary
	}

	//-----------------------------------------
	// Returns Vector in Direction of Camera.
	// For BackFacing Object Checks
	Stuff::Vector3D getLookVector(void) { return lookVector; }

	Stuff::Vector3D getCameraOrigin(void) { return physicalPos; }

	float getTiltFactor(void)
	{
		float result = 1.0;
		if (usePerspective)
		{
			result = 0.5f +
					 ((projectionAngle - 28.0f) * 0.016666667f * 0.5); // 1/60th
		}
		return result;
	}

	//----------------------------------------------
	// Camera Positioning Functions
	void setCameraView(int32_t viewNum);

	void allNormal(void)
	{
		tiltNormal(void);
		ZoomNormal(void);
		rotateNormal(void);
	}

	void allDefault(void) { ZoomDefault(void); }

	void allMaxIn(void) { ZoomMax(void); }

	virtual void allTight(void) { ZoomTight(void); }

	void zoomValue(float value);
	void ZoomIn(float amount);
	void ZoomOut(float amount);
	void ZoomNormal(void);
	void ZoomDefault(void);
	void ZoomTight(void);
	void ZoomMax(void);
	void ZoomMin(void);

	void tiltValue(float value);
	void tiltUp(float amount);
	void tiltDown(float amount);
	void tiltNormal(void);

	void movePosLeft(float amount, Stuff::Vector3D& pos);
	void movePosRight(float amount, Stuff::Vector3D& pos);
	void movePosUp(float amount, Stuff::Vector3D& pos);
	void movePosDown(float amount, Stuff::Vector3D& pos);

	virtual void moveLeft(float amount);
	virtual void moveRight(float amount);
	virtual void moveUp(float amount);
	virtual void moveDown(float amount);

	void rotateLeft(float amount);
	void rotateRight(float amount);

	void rotateNormal(void);

	void rotateLightLeft(float amount);
	void rotateLightRight(float amount);
	void rotateLightUp(float amount);
	void rotateLightDown(float amount);

	uint8_t getLightRed(float intensity);
	uint8_t getLightGreen(float intensity);
	uint8_t getLightBlue(float intensity);

	void setPosition(Stuff::Vector3D newPosition, bool swoopy = true);

	bool save(FitIniFile* fileName);

	float getFarClipDistance(void);
	void setFarClipDistance(float farClipDistance);
	float getNearClipDistance(void);
	void setNearClipDistance(float nearClipDistance);
	float getMaximumCameraAltitude(void);
	void setMaximumCameraAltitude(float maxAltitude);

	//--------------------------------------------
	// Camera Scripting API
	void setMovieMode(void)
	{
		if (!inMovieMode)
		{
			inMovieMode   = true;
			startEnding   = false;
			forceMovieEnd = false;
			letterBoxPos  = 0.0f; // Start letterboxing;
			letterBoxTime = 0.0f;
		}
	}

	void endMovieMode(void)
	{
		if (inMovieMode)
			startEnding = true; // Start the letterbox shrinking.  When fully
								// shrunk, inMovieMode goes false!
	}

	void fadeToColor(uint32_t color, float timeToFade)
	{
		if (!inFadeMode && (timeToFade > Stuff::SMALL))
		{
			inFadeMode	 = true;
			fadeStart	  = fadeColor;
			fadeColor	  = color;
			timeLeftToFade = 0.0f;
			startFadeTime  = timeToFade;
		}
	}

	void forceMovieToEnd(void) // ABL Script must handle this going true!!!!
	{
		forceMovieEnd = true;
	}

	void setFieldOfView(float fov) { camera_fov = fov; }

	float getFieldOfView(void) { return camera_fov; }

	void setFieldOfViewGoal(float fov, float time)
	{
		goalFOV		= fov;
		goalFOVTime = time;
	}

	float getFieldOfViewGoal(void) { return goalFOV; }

	void setGoalPosition(Stuff::Vector3D goalPos) { goalPosition = goalPos; }

	void setLookPosition(Stuff::Vector3D goalPos) { lookPosition = goalPos; }

	Stuff::Vector3D getGoalPosition(void) { return goalPosition; }

	Stuff::Vector3D getLookPosition(void) { return lookPosition; }

	void setGoalPosTime(float time) { goalPosTime = time; }

	Stuff::Vector3D getRotation(void)
	{
		Stuff::Vector3D rot(0.0, 0.0, 0.0);
		rot.x = projectionAngle;
		rot.y = cameraRotation;
		return rot;
	}

	void setRotation(Stuff::Vector3D rot)
	{
		projectionAngle = rot.x;
		setCameraRotation(rot.y, rot.y);
		cameraAltitude = rot.z;
		if (cameraAltitude < AltitudeMinimum)
			cameraAltitude = AltitudeMinimum;
		float anglePercent = (projectionAngle - MIN_PERSPECTIVE) /
							 (MAX_PERSPECTIVE - MIN_PERSPECTIVE);
		float testMax =
			Camera::AltitudeMaximumLo +
			((Camera::AltitudeMaximumHi - Camera::AltitudeMaximumLo) *
				anglePercent);
		newScaleFactor = 1.0f - ((cameraAltitude - AltitudeMinimum) / testMax);
	}

	void setGoalRotation(Stuff::Vector3D goalRot) { goalRotation = goalRot; }

	Stuff::Vector3D getGoalRotation(void) { return goalRotation; }

	void setGoalRotTime(float time) { goalRotTime = time; }

	Stuff::Vector3D getVelocity(void) { return velocity; }

	void setVelocity(Stuff::Vector3D vel) { velocity = vel; }

	Stuff::Vector3D getGoalVelocity(void) { return goalVelocity; }

	void setGoalVelocity(Stuff::Vector3D goalVel) { goalVelocity = goalVel; }

	void setGoalVelTime(float time) { goalVelTime = time; }

	void updateGoalVelocity(void);
	void updateGoalPosition(Stuff::Vector3D& pos);
	void updateGoalRotation(void);
	void updateGoalFOV(void);

	void updateLetterboxAndFade(void);

	void setCameraTargetId(int32_t targId) { lookTargetObject = targId; }

	int32_t getCameraTargetId(void) { return lookTargetObject; }

	void setInverseProject(float sZ, float sW, float zPP, float wPP)
	{
		startZInverse = sZ;
		startWInverse = sW;
		zPerPixel	 = zPP;
		wPerPixel	 = wPP;
	}
};

//---------------------------------------------------------------------------
extern CameraPtr eye;

//---------------------------------------------------------------------------
#endif

//---------------------------------------------------------------------------
//
// Edit log
//
//---------------------------------------------------------------------------
