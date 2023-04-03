/*************************************************************************************************\
SimpleCamera.cpp	: Implementation of the SimpleCamera component.
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//
\*************************************************************************************************/

#include "stdinc.h"
#include "simplecamera.h"
#include "appear.h"
#include "mclib.h"
#include "mech3d.h"
#include "mission.h"
#include "bdactor.h"

extern bool useShadows;
extern bool drawOldWay;
extern bool useFog;

MidLevelRenderer::MLRClipper* theClipper = nullptr;

////////////////////////////////////////////////
SimpleCamera::SimpleCamera()
{
	pObject = nullptr;
	Camera::init();
	wchar_t path[256];
	strcpy(path, cameraPath);
	strcat(path, "cameras.fit");
	FitIniFile camFile;
	if (NO_ERROR != camFile.open(path))
	{
		STOP(("Need Camera File "));
	}
	Camera::init(&camFile);
	AltitudeTight = 650;
	rotation = -45.f;
	bIsComponent = 0;
	rotateLightRight(90.0f);
	bIsInMission = false;
}

SimpleCamera::~SimpleCamera()
{
	// Why did we not delete here??
	// It was commented out.
	// -fs
	if (appearanceTypeList && appearanceTypeList->pointerCanBeDeleted(pObject))
		delete pObject;
	pObject = nullptr;
	// We have to do this here because we always load the damned sensor shape.
	// ONLY if we are running it in logistics.  DO NOT DELETE THESE IN THE
	// MIDDLE OF A MISSION!!!
	if (!bIsInMission)
	{
		if (GVAppearanceType::SensorTriangleShape)
		{
			delete GVAppearanceType::SensorTriangleShape;
			GVAppearanceType::SensorTriangleShape = nullptr;
		}
		if (GVAppearanceType::SensorCircleShape)
		{
			delete GVAppearanceType::SensorCircleShape;
			GVAppearanceType::SensorCircleShape = nullptr;
		}
		if (Mech3DAppearanceType::SensorSquareShape)
		{
			delete Mech3DAppearanceType::SensorSquareShape;
			Mech3DAppearanceType::SensorSquareShape = nullptr;
		}
	}
}

void SimpleCamera::init(float left, float top, float right, float bottom)
{
	bounds[0] = left;
	bounds[1] = top;
	bounds[2] = right;
	bounds[3] = bottom;
}

////////////////////////////////////////////////
void SimpleCamera::render()
{
	render(0, 0);
}

void SimpleCamera::render(int32_t xOffset, int32_t yOffset)
{
	if (xOffset != 0 && yOffset != 0) // don't know how to do this
		return;
	if (pObject)
	{
		if (bIsComponent)
		{
			lightRed = 196;
			lightGreen = 196;
			lightBlue = 220;
			ambientRed = 196;
			ambientGreen = 196;
			ambientBlue = 196;
		}
		gos_PushRenderStates();
		oldCam = eye;
		eye = this;
		useFog = 0;
		gos_GetViewport(&viewMulX, &viewMulY, &viewAddX, &viewAddY);
		//--------------------------------------------------------
		// Get new viewport values to scale stuff.  No longer uses
		// VFX stuff for this.  ALL GOS NOW!
		screenResolution.x = viewMulX;
		screenResolution.y = viewMulY;
		calculateProjectionConstants();
		TG_Shape::SetViewport(viewMulX, viewMulY, viewAddX, viewAddY);
		globalScaleFactor = getScaleFactor();
		globalScaleFactor *= viewMulX / Environment.screenwidth; // Scale Mechs to ScreenRES
		//-----------------------------------------------
		setLightcolour(1, 0xffffffff);
		setLightIntensity(1, 1.0);
		MidLevelRenderer::MLRState default_state;
		default_state.SetBackFaceOn();
		default_state.SetDitherOn();
		default_state.SetTextureCorrectionOn();
		default_state.SetZBufferCompareOn();
		default_state.SetZBufferWriteOn();
		default_state.SetFilterMode(MidLevelRenderer::MLRState::BiLinearFilterMode);
		Stuff::RGBAcolour fcolour;
		fcolour.red = 0;
		fcolour.green = 0;
		fcolour.blue = 0;
		float z = 1.0;
		MidLevelRenderer::PerspectiveMode = usePerspective;
		theClipper->StartDraw(cameraOrigin, cameraToClip, fcolour, &fcolour, default_state, &z);
		MidLevelRenderer::GOSVertex::farClipReciprocal =
			(1.0f - cameraToClip(2, 2)) / cameraToClip(3, 2);
		//--------------------------------
		// Set States for Software Renderer
		if (Environment.Renderer == 3)
		{
			gos_SetRenderState(gos_State_AlphaMode, gos_Alpha_OneZero);
			gos_SetRenderState(gos_State_ShadeMode, gos_ShadeGouraud);
			gos_SetRenderState(gos_State_MonoEnable, 1);
			gos_SetRenderState(gos_State_Perspective, 0);
			gos_SetRenderState(gos_State_Clipping, 1);
			gos_SetRenderState(gos_State_Specular, 0);
			gos_SetRenderState(gos_State_Dither, 0);
			gos_SetRenderState(gos_State_TextureMapBlend, gos_BlendModulate);
			gos_SetRenderState(gos_State_Filter, gos_FilterNone);
			gos_SetRenderState(gos_State_TextureAddress, gos_TextureWrap);
			gos_SetRenderState(gos_State_ZCompare, 1);
			gos_SetRenderState(gos_State_ZWrite, 1);
		}
		//--------------------------------
		// Set States for Hardware Renderer
		else
		{
			gos_SetRenderState(gos_State_AlphaMode, gos_Alpha_OneZero);
			gos_SetRenderState(gos_State_ShadeMode, gos_ShadeGouraud);
			gos_SetRenderState(gos_State_MonoEnable, 0);
			gos_SetRenderState(gos_State_Perspective, 1);
			gos_SetRenderState(gos_State_Clipping, 1);
			gos_SetRenderState(gos_State_Specular, 1);
			gos_SetRenderState(gos_State_Dither, 1);
			gos_SetRenderState(gos_State_TextureMapBlend, gos_BlendModulate);
			gos_SetRenderState(gos_State_TextureAddress, gos_TextureWrap);
			gos_SetRenderState(gos_State_ZCompare, 1);
			gos_SetRenderState(gos_State_ZWrite, 1);
		}
		pObject->render();
		if (!drawOldWay)
			mcTextureManager->renderLists();
		eye = oldCam;
		gos_PopRenderStates();
	}
}

////////////////////////////////////////////////
int32_t
SimpleCamera::update()
{
	if (pObject)
	{
		turn++; // Must increment this now or matrices NEVER change!!
		// reset the TGL RAM pools.
		colorPool->reset();
		vertexPool->reset();
		facePool->reset();
		shadowPool->reset();
		trianglePool->reset();
		// reset the TGL RAM pools.
		colorPool->reset();
		vertexPool->reset();
		facePool->reset();
		shadowPool->reset();
		trianglePool->reset();
		mcTextureManager->clearArrays();
		mcTextureManager->update();
		Camera::update();
		//--------------------------------------------------------
		// Get new viewport values to scale stuff.  No longer uses
		// VFX stuff for this.  ALL GOS NOW!
		screenResolution.x = viewMulX;
		screenResolution.y = viewMulY;
		calculateProjectionConstants();
		float offsetX = bounds[2] + bounds[0] - viewMulX;
		offsetX /= 2;
		offsetX += fudgeX;
		float offsetY = bounds[1] + bounds[3] - viewMulY;
		offsetY /= 2;
		offsetY += fudgeY; // hack, just to get exactly where Dorje wants it
		TG_Shape::SetViewport(viewMulX, viewMulY, offsetX, offsetY);
		useShadows = 0;
		oldCam = eye;
		eye = this;
		Camera::update();
		ZoomTight();
		pObject->recalcBounds();
		pObject->scale(shapeScale);
		// we don't want to center around the feet
		Stuff::Vector3D mechPos = pObject->getRootNodeCenter();
		mechPos.x = -mechPos.x / 2.f;
		float tmp = -mechPos.y / 2.f;
		mechPos.y = -mechPos.z / 2.f;
		mechPos.z = tmp;
		float rotation = frameLength * rotationIncrement + pObject->rotation;
		pObject->setObjectParameters(mechPos, rotation, 0, 0, 0);
		pObject->update();
		pObject->setVisibility(true, true);
		eye = oldCam;
	}
	return 0;
}

void SimpleCamera::setMech(
	std::wstring_view fileName, int32_t basecolour, int32_t highlight1, int32_t highlight2)
{
	shapeScale = 0.0f;
	bIsComponent = 0;
	fudgeX = 5;
	fudgeY = 10;
	AltitudeTight = 650;
	// moving this to above the spot where we create the appearancetypelist
	if (appearanceTypeList && appearanceTypeList->pointerCanBeDeleted(pObject))
		delete pObject;
	if (!appearanceTypeList)
		Mission::initBareMinimum();
	rotationIncrement = 0;
	pObject = nullptr;
	if (!fileName)
	{
		//		allNormal();
		return;
	}
	wchar_t NoPathFileName[256];
	_splitpath(fileName, nullptr, nullptr, NoPathFileName, nullptr);
	wchar_t testName[256];
	strcpy(testName, NoPathFileName);
	strcat(testName, "enc");
	FullPathFileName path;
	path.init(tglPath, testName, ".ini");
	// MUST ALWAYS CALL GET, EVEN IF WE HAVE AN APPEARANCE TYPE OR REFERENCE
	// COUNT DOES NOT INCREASE!
	Mech3DAppearanceType* appearanceType = nullptr;
	if (fileExists(path))
		appearanceType = (Mech3DAppearanceType*)appearanceTypeList->getAppearance(
			MECH_TYPE << 24, (std::wstring_view)testName);
	else
		appearanceType = (Mech3DAppearanceType*)appearanceTypeList->getAppearance(
			MECH_TYPE << 24, (std::wstring_view)NoPathFileName);
	pObject = new Mech3DAppearance;
	pObject->init(appearanceType);
	pObject->setGestureGoal(2);
	pObject->resetPaintScheme(highlight1, highlight2, basecolour);
	pObject->rotation = rotation;
	activate();
	setPosition(position, 0);
	ZoomTight();
}

void SimpleCamera::setVehicle(std::wstring_view fileName, int32_t base, int32_t highlight, int32_t h2)
{
	shapeScale = 0.0f;
	bIsComponent = 0;
	fudgeX = 5;
	fudgeY = 10;
	AltitudeTight = 650;
	if (!appearanceTypeList)
		Mission::initBareMinimum();
	rotationIncrement = 90;
	if (appearanceTypeList && appearanceTypeList->pointerCanBeDeleted(pObject))
		delete pObject;
	pObject = nullptr;
	if (!fileName)
		return;
	wchar_t NoPathFileName[256];
	_splitpath(fileName, nullptr, nullptr, NoPathFileName, nullptr);
	wchar_t testName[256];
	strcpy(testName, fileName);
	strcat(testName, "enc");
	FullPathFileName path;
	path.init(tglPath, testName, ".ini");
	// MUST ALWAYS CALL GET, EVEN IF WE HAVE AN APPEARANCE TYPE OR REFERENCE
	// COUNT DOES NOT INCREASE!
	GVAppearanceType* appearanceType = nullptr;
	if (fileExists(path))
		appearanceType =
			(GVAppearanceType*)appearanceTypeList->getAppearance(GV_TYPE << 24, (std::wstring_view)testName);
	else
		appearanceType = (GVAppearanceType*)appearanceTypeList->getAppearance(
			GV_TYPE << 24, (std::wstring_view)NoPathFileName);
	pObject = new GVAppearance;
	pObject->init(appearanceType);
	pObject->setGestureGoal(2);
	pObject->resetPaintScheme(base, highlight, h2);
	pObject->rotation = rotation;
	activate();
	setPosition(position);
	ZoomTight();
}

void SimpleCamera::setComponent(std::wstring_view fileName)
{
	shapeScale = 0.0f;
	bIsComponent = 1;
	AltitudeTight = 580;
	fudgeX = 0;
	fudgeY = 0;
	if (!appearanceTypeList)
		Mission::initBareMinimum();
	if (appearanceTypeList && appearanceTypeList->pointerCanBeDeleted(pObject))
		delete pObject;
	pObject = nullptr;
	if (!fileName)
		return;
	wchar_t testName[256];
	strcpy(testName, fileName);
	strcat(testName, "enc");
	FullPathFileName path;
	path.init(tglPath, testName, ".ini");
	BldgAppearanceType* appearanceType = nullptr;
	if (fileExists(path))
	{
		appearanceType =
			(BldgAppearanceType*)appearanceTypeList->getAppearance(BLDG_TYPE << 24, (std::wstring_view)testName);
	}
	else
		appearanceType =
			(BldgAppearanceType*)appearanceTypeList->getAppearance(BLDG_TYPE << 24, (std::wstring_view)fileName);
	// MUST ALWAYS CALL GET, EVEN IF WE HAVE AN APPEARANCE TYPE OR REFERENCE
	// COUNT DOES NOT INCREASE!
	pObject = new BldgAppearance;
	pObject->init(appearanceType);
	pObject->resetPaintScheme(0xffff7e00, 0xffff7e00, 0xffbcbcbc);
	pObject->rotation = rotation;
	rotationIncrement = 90;
	activate();
	setPosition(position);
	ZoomTight();
}
void SimpleCamera::setScale(float newAltitude)
{
	shapeScale = newAltitude;
}

void SimpleCamera::setBuilding(std::wstring_view pBuilding)
{
	shapeScale = 0.0f;
	setComponent(pBuilding);
	AltitudeTight = 800;
	bIsComponent = 0;
}

void SimpleCamera::setObject(
	std::wstring_view pFileName, int32_t type, int32_t base, int32_t highlight, int32_t h2)
{
	if (!pFileName || !strlen(pFileName))
	{
		if (appearanceTypeList && appearanceTypeList->pointerCanBeDeleted(pObject))
			delete pObject;
		pObject = nullptr;
		return;
	}
	switch (type)
	{
	case BLDG_TYPE:
		setBuilding(pFileName);
		break;
	case TREED_TYPE:
		setBuilding(pFileName); // this might not work....
		break;
	case GV_TYPE:
		setVehicle(pFileName, base, highlight, h2);
		break;
	case MECH_TYPE:
		setMech(pFileName, base, highlight, h2);
		break;
	default:
		// gosASSERT( !"camera just got an unknown type!" );
		NODEFAULT;
	}
}

void SimpleCamera::setcolours(int32_t base, int32_t highlight, int32_t h2)
{
	pObject->resetPaintScheme(base, highlight, h2);
}

void SimpleCamera::zoomIn(float howMuch)
{
	AltitudeTight = 650.f / howMuch;
} // scale for things that can't
