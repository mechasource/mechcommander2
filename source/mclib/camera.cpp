//---------------------------------------------------------------------------
//
// Camera.cpp -- File contains the camera class code
//
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#include "stdinc.h"

#include "camera.h"
#include "terrain.h"
#include "vertex.h"
#include "crater.h"
#include "terrtxm.h"
#include "cident.h"
#include "paths.h"
#include "timing.h"
#include "userinput.h"

extern void
AG_ellipse_draw(
	PANE* pane, int32_t xc, int32_t yc, int32_t width, int32_t height, int32_t color);
extern void
AG_ellipse_fill(
	PANE* pane, int32_t xc, int32_t yc, int32_t width, int32_t height, int32_t color);
extern void
AG_StatusBar(
	PANE* pane, int32_t X0, int32_t Y0, int32_t X1, int32_t Y1, int32_t colour, int32_t width);
extern void
AG_shape_draw(
	PANE* pane, PVOIDshape_table, int32_t shape_number, int32_t hotX, int32_t hotY);
extern void
AG_shape_translate_draw(
	PANE* pane, PVOIDshape_table, int32_t shape_number, int32_t hotX, int32_t hotY);
extern void
AG_shape_lookaside(uint8_t* palette);

//#pragma warning(disable:4305/*double to float truncation*/)

inline float
agsqrt(float _a, float _b)
{
	return sqrt(_a * _a + _b * _b);
}

//---------------------------------------------------------------------------
// Static Globals

wchar_t WindowTitle[1024]; // Global window title (GetWindowText is VERY slow)

int32_t topCtrlUpd = 0;

extern int32_t scenarioEndTurn;
extern float actualTime;

#define TABLE_ENTRY_SIZE 256
#define SCALE_LEVELS 7
#define RAMPED_RED 239
#define RAMPED_GREEN 248
#define NO_RAM_FOR_SCALE_TABLE 0x12120001

#define NIGHT_START_PITCH (25.0f)
#define NIGHT_LIGHT_PITCH (10.0f)
#define TERRAIN_LIGHTS_ON (40.0f)

#ifdef _DEBUG
extern int32_t currentHotSpot;
#endif

// RNA - moved this out of the #ifdef PROFILE so it's available in release
extern int32_t displayProfileData;

extern bool drawTerrainGrid;
extern int32_t mechElements;

int32_t leaveSwoopyOff = 0;

extern int32_t tileCacheReqs;
extern int32_t tileCacheHits;
extern int32_t tileCacheMiss;

bool drawCameraCircle = false;
extern bool gamePaused;
extern bool gameAsked;
uint8_t* pauseShape = nullptr;
uint8_t* askedShape = nullptr;
extern bool gRestartRender;
bool MaxObjectsDrawn = false;

float elevationAdjustFactor = 50.0;

float Camera::MIN_PERSPECTIVE = 10.0f;
float Camera::MIN_ORTHO = 18.0f;
float Camera::MAX_PERSPECTIVE = 88.0f;
float Camera::MAX_ORTHO = 88.0f;

#define NORM_PERSPECTIVE 35.0f

float zoomMax = 1.0;
float zoomMin = 0.3f;
float FOVMax = 75.0f;
float FOVMin = 20.0f;
float Camera::AltitudeMinimum = 560.0f;
float Camera::AltitudeMaximumLo = 1500.0f;
float Camera::AltitudeMaximumHi = 1600.0f;
float Camera::AltitudeDefault = 1200.0f;
float Camera::AltitudeTight = 800.0f;

float Camera::globalScaleFactor = 1.0;

float Camera::MaxClipDistance = 4000.0f;
float Camera::MinHazeDistance = 3000.0f;
float Camera::HazeFactor = 0.0f;
float Camera::DistanceFactor = 1.0f / (MaxClipDistance - MinHazeDistance);
float Camera::NearPlaneDistance = -400.0f;
float Camera::FarPlaneDistance = 61555.0f;

float Camera::MinNearPlane = -100.0f;
float Camera::MaxNearPlane = -400.0f;

float Camera::MinFarPlane = 61500.0f;
float Camera::MaxFarPlane = 61555.0f;

float Camera::verticalSphereClipConstant = 0.0f;
float Camera::horizontalSphereClipConstant = 0.0f;

bool Camera::inMovieMode = false;
bool Camera::forceMovieEnd = false;
float Camera::MaxLetterBoxTime = 5.0f; // Takes this many seconds to get into letter box mode
float Camera::MaxLetterBoxPos =
	0.15f; // This is the percentage of the screen covered by EACH black bar!

float Camera::cameraTilt[MAX_VIEWS] = {
	NORM_PERSPECTIVE,
	NORM_PERSPECTIVE,
	NORM_PERSPECTIVE,
	NORM_PERSPECTIVE,
};

float Camera::cameraZoom[MAX_VIEWS] = {Camera::AltitudeDefault, Camera::AltitudeDefault,
	Camera::AltitudeDefault, Camera::AltitudeDefault};

frameOfRef Camera::cameraFrame;

extern bool recalcLight;

const float CAM_THRESHOLD = 150.0f;

//---------------------------------------------------------------------------
// Camera Class
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
int32_t
Camera::init(FitIniFilePtr cameraFile)
{
	int32_t result = cameraFile->seekBlock("Cameras");
	gosASSERT(result == NO_ERROR);
	result = cameraFile->readIdFloat("ProjectionAngle", projectionAngle);
	gosASSERT(result == NO_ERROR);
	result = cameraFile->readIdFloat("PositionX", position.x);
	gosASSERT(result == NO_ERROR);
	result = cameraFile->readIdFloat("PositionY", position.y);
	gosASSERT(result == NO_ERROR);
	result = cameraFile->readIdFloat("PositionZ", position.z);
	gosASSERT(result == NO_ERROR);
	cameraShiftZ = position.z;
	result = cameraFile->readIdBoolean("Ready", ready);
	gosASSERT(result == NO_ERROR);
	result = cameraFile->readIdUChar("LightRed", lightRed);
	gosASSERT(result == NO_ERROR);
	result = cameraFile->readIdUChar("LightGreen", lightGreen);
	gosASSERT(result == NO_ERROR);
	result = cameraFile->readIdUChar("LightBlue", lightBlue);
	gosASSERT(result == NO_ERROR);
	result = cameraFile->readIdUChar("AmbientRed", ambientRed);
	gosASSERT(result == NO_ERROR);
	result = cameraFile->readIdUChar("AmbientGreen", ambientGreen);
	gosASSERT(result == NO_ERROR);
	result = cameraFile->readIdUChar("AmbientBlue", ambientBlue);
	gosASSERT(result == NO_ERROR);
	uint8_t tmpUCHAR;
	result = cameraFile->readIdUChar("TerrainShadowcolourEnabled", tmpUCHAR);
	if ((result != NO_ERROR) || (0 == tmpUCHAR))
	{
		terrainShadowcolourEnabled = false;
	}
	else
	{
		terrainShadowcolourEnabled = true;
	}
	result = cameraFile->readIdUChar("TerrainShadowRed", terrainShadowRed);
	if (result != NO_ERROR)
	{
		terrainShadowRed = ambientRed;
	}
	result = cameraFile->readIdUChar("TerrainShadowGreen", terrainShadowGreen);
	if (result != NO_ERROR)
	{
		terrainShadowGreen = ambientGreen;
	}
	result = cameraFile->readIdUChar("TerrainShadowBlue", terrainShadowBlue);
	if (result != NO_ERROR)
	{
		terrainShadowBlue = ambientBlue;
	}
	result = cameraFile->readIdUChar("SeenRed", seenRed);
	gosASSERT(result == NO_ERROR);
	result = cameraFile->readIdUChar("SeenGreen", seenGreen);
	gosASSERT(result == NO_ERROR);
	result = cameraFile->readIdUChar("SeenBlue", seenBlue);
	gosASSERT(result == NO_ERROR);
	result = cameraFile->readIdUChar("BaseRed", baseRed);
	gosASSERT(result == NO_ERROR);
	result = cameraFile->readIdUChar("BaseGreen", baseGreen);
	gosASSERT(result == NO_ERROR);
	result = cameraFile->readIdUChar("BaseBlue", baseBlue);
	gosASSERT(result == NO_ERROR);
	result = cameraFile->readIdFloat("LightDirYaw", lightYaw);
	gosASSERT(result == NO_ERROR);
	result = cameraFile->readIdFloat("LightDirPitch", lightPitch);
	gosASSERT(result == NO_ERROR);
	result = cameraFile->readIdFloat("DayToNightTime", day2NightTransitionTime);
	if (result != NO_ERROR)
	{
		day2NightTransitionTime = -1.0f;
		dayLightTime = 0.0f;
		gosASSERT((lightYaw >= -360.0) && (lightYaw <= 360.0));
		// gosASSERT((lightPitch >= 0.0) && (lightPitch <= 90.0));
		lightDirection.x = cos((lightYaw + 45.0f) * DEGREES_TO_RADS);
		lightDirection.y = sin((lightYaw + 45.0f) * DEGREES_TO_RADS);
		lightDirection.z = sin(lightPitch * DEGREES_TO_RADS);
		// initialize these so we can edit 'em
		dayAmbientRed = ambientRed;
		dayAmbientBlue = ambientBlue;
		dayAmbientGreen = ambientGreen;
		dayLightRed = lightRed;
		dayLightGreen = lightGreen;
		dayLightBlue = lightBlue;
		sunsetLightRed = 205 /*arbitrary default*/;
		sunsetLightGreen = 84 /*arbitrary default*/;
		sunsetLightBlue = 31 /*arbitrary default*/;
		nightAmbientRed = 32 /*arbitrary default*/;
		nightAmbientBlue = 32 /*arbitrary default*/;
		nightAmbientGreen = 32 /*arbitrary default*/;
		;
		nightLightRed = 64 /*arbitrary default*/;
		nightLightGreen = 64 /*arbitrary default*/;
		nightLightBlue = 64 /*arbitrary default*/;
		dayLightPitch = lightPitch;
	}
	else // They want the time of day to change!
	{
		result = cameraFile->readIdFloat("DayLightPitch", dayLightPitch);
		gosASSERT(result == NO_ERROR);
		result = cameraFile->readIdUChar("DayLightRed", dayLightRed);
		gosASSERT(result == NO_ERROR);
		result = cameraFile->readIdUChar("DayLightGreen", dayLightGreen);
		gosASSERT(result == NO_ERROR);
		result = cameraFile->readIdUChar("DayLightBlue", dayLightBlue);
		gosASSERT(result == NO_ERROR);
		result = cameraFile->readIdUChar("DayAmbientRed", dayAmbientRed);
		gosASSERT(result == NO_ERROR);
		result = cameraFile->readIdUChar("DayAmbientGreen", dayAmbientGreen);
		gosASSERT(result == NO_ERROR);
		result = cameraFile->readIdUChar("DayAmbientBlue", dayAmbientBlue);
		gosASSERT(result == NO_ERROR);
		result = cameraFile->readIdUChar("SunsetLightRed", sunsetLightRed);
		if (NO_ERROR != result)
		{
			sunsetLightRed = 254 /*arbitrary default*/;
			sunsetLightGreen = 68 /*arbitrary default*/;
			sunsetLightBlue = 5 /*arbitrary default*/;
		}
		else
		{
			result = cameraFile->readIdUChar("SunsetLightGreen", sunsetLightGreen);
			gosASSERT(result == NO_ERROR);
			result = cameraFile->readIdUChar("SunsetLightBlue", sunsetLightBlue);
			gosASSERT(result == NO_ERROR);
		}
		result = cameraFile->readIdUChar("NightLightRed", nightLightRed);
		gosASSERT(result == NO_ERROR);
		result = cameraFile->readIdUChar("NightLightGreen", nightLightGreen);
		gosASSERT(result == NO_ERROR);
		result = cameraFile->readIdUChar("NightLightBlue", nightLightBlue);
		gosASSERT(result == NO_ERROR);
		result = cameraFile->readIdUChar("NightAmbientRed", nightAmbientRed);
		gosASSERT(result == NO_ERROR);
		result = cameraFile->readIdUChar("NightAmbientGreen", nightAmbientGreen);
		gosASSERT(result == NO_ERROR);
		result = cameraFile->readIdUChar("NightAmbientBlue", nightAmbientBlue);
		gosASSERT(result == NO_ERROR);
		dayLightTime = 0.0f;
		lightPitch = dayLightPitch;
		ambientRed = dayAmbientRed;
		ambientBlue = dayAmbientBlue;
		ambientGreen = dayAmbientGreen;
		lightRed = dayLightRed;
		lightBlue = dayLightBlue;
		lightGreen = dayLightGreen;
		gosASSERT((lightYaw >= -360.0) && (lightYaw <= 360.0));
		// gosASSERT((lightPitch >= 0.0) && (lightPitch <= 90.0));
		lightDirection.x = cos((lightYaw + 45.0f) * DEGREES_TO_RADS);
		lightDirection.y = sin((lightYaw + 45.0f) * DEGREES_TO_RADS);
		lightDirection.z = sin(lightPitch * DEGREES_TO_RADS);
	}
	result = cameraFile->readIdFloat("NewScale", newScaleFactor);
	gosASSERT(result == NO_ERROR);
	float startRotation = 0.0;
	result = cameraFile->readIdFloat("StartRotation", startRotation);
	gosASSERT(result == NO_ERROR);
	setCameraRotation(startRotation, startRotation);
	result = cameraFile->readIdFloatArray("LODScales", zoomLevelLODScale, MAX_LODS);
	gosASSERT(result == NO_ERROR);
	result = cameraFile->readIdFloat("ElevationAdjustFactor", elevationAdjustFactor);
	gosASSERT(result == NO_ERROR);
	result = cameraFile->readIdFloat("ZoomMax", zoomMax);
	gosASSERT(result == NO_ERROR);
	result = cameraFile->readIdFloat("ZoomMin", zoomMin);
	gosASSERT(result == NO_ERROR);
	FOVMax = 90.0f;
	result = cameraFile->readIdFloat("FOVMax", FOVMax);
	//	gosASSERT(result == NO_ERROR);
	FOVMin = 20.0f;
	result = cameraFile->readIdFloat("FOVMin", FOVMin);
	//	gosASSERT(result == NO_ERROR);
	setClass(BASE_CAMERA);
	// Replace with TGL
	if (!worldLights) // only do the alloc once
	{
		worldLights = (TG_LightPtr*)systemHeap->Malloc(sizeof(TG_LightPtr) * MAX_LIGHTS_IN_WORLD);
		memset(worldLights, 0, sizeof(TG_LightPtr) * MAX_LIGHTS_IN_WORLD);
		numLights = 2;
		//---------------------------------------------------------
		// First light is sun.
		worldLights[0] = (TG_LightPtr)systemHeap->Malloc(sizeof(TG_Light));
		worldLights[0]->init(TG_LIGHT_INFINITE);
	}
	if (!activeLights) // only do the alloc once
	{
		activeLights = (TG_LightPtr*)systemHeap->Malloc(sizeof(TG_LightPtr) * MAX_LIGHTS_IN_WORLD);
		memset(activeLights, 0, sizeof(TG_LightPtr) * MAX_LIGHTS_IN_WORLD);
		numActiveLights = 0;
	}
	if (!terrainLights) // only do the alloc once
	{
		terrainLights = (TG_LightPtr*)systemHeap->Malloc(sizeof(TG_LightPtr) * MAX_LIGHTS_IN_WORLD);
		memset(terrainLights, 0, sizeof(TG_LightPtr) * MAX_LIGHTS_IN_WORLD);
		numTerrainLights = 0;
	}
	Stuff::LinearMatrix4D lightToWorldMatrix;
	lightToWorldMatrix.BuildTranslation(Stuff::Point3D(0.0, 0.0, 0.0));
	lightToWorldMatrix.BuildRotation(Stuff::EulerAngles(
		lightPitch * DEGREES_TO_RADS, (lightYaw + 135.0) * DEGREES_TO_RADS, 0.0f));
	worldLights[0]->SetLightToWorld(&lightToWorldMatrix);
	//---------------------------------------------------------
	// Second light is ambient.
	worldLights[1] = (TG_LightPtr)systemHeap->Malloc(sizeof(TG_Light));
	worldLights[1]->init(TG_LIGHT_AMBIENT);
	//---------------------
	// Read in Fog Values.
	result = cameraFile->readIdFloat("FogStart", fogStart);
	gosASSERT(result == NO_ERROR);
	result = cameraFile->readIdFloat("FogFull", fogFull);
	gosASSERT(result == NO_ERROR);
	result = cameraFile->readIdULong("Fogcolour", fogcolour);
	dayFogcolour = fogcolour;
	gosASSERT(result == NO_ERROR);
	result = cameraFile->readIdFloat("FogTransparency", fogTransparency);
#if 0
	if(result != NO_ERROR)
	{
		/* This is an old file where Fogcolour includes the color of the sky showing through. */
		/* arbitrary assumed daytime sky color */
		static const float fDaySkyRed = 100.0f;
		static const float fDaySkyGreen = 162.0f;
		static const float fDaySkyBlue = 255.0f;
		const float fDayFogRed = (float)((dayFogcolour >> 16) & 0xff);
		const float fDayFogGreen = (float)((dayFogcolour >> 8) & 0xff);
		const float fDayFogBlue = (float)((dayFogcolour) & 0xff);
		/* We assume that the fog color specified by the user is a combination of the diffuse
		light scattered by the fog and the light from the sky showing through the fog. There's
		no way to know how transparent the fog is meant to be without the user specifying it,
		but for now we'll assume the maximum transparency possible given the daytime fog
		color and an assumed daytime sky color. */
		float fFogTransparency = 1.0f;
		if((0.0f < fDaySkyRed) && (fDayFogRed / fDaySkyRed < fFogTransparency))
		{
			fFogTransparency = fDayFogRed / fDaySkyRed;
		}
		if((0.0f < fDaySkyGreen) && (fDayFogGreen / fDaySkyGreen < fFogTransparency))
		{
			fFogTransparency = fDayFogGreen / fDaySkyGreen;
		}
		if((0.0f < fDaySkyBlue) && (fDayFogBlue / fDaySkyBlue < fFogTransparency))
		{
			fFogTransparency = fDayFogBlue / fDaySkyBlue;
		}
		/* Here we subtract out the sky color component to get the actual color of the fog. */
		int32_t opaqueDayFogRed = fDayFogRed - fFogTransparency * fDaySkyRed;
		int32_t opaqueDayFogGreen = fDayFogGreen - fFogTransparency * fDaySkyGreen;
		int32_t opaqueDayFogBlue = fDayFogBlue - fFogTransparency * fDaySkyBlue;
		gosASSERT(0 == ((~0xff) & opaqueDayFogRed));
		gosASSERT(0 == ((~0xff) & opaqueDayFogGreen));
		gosASSERT(0 == ((~0xff) & opaqueDayFogBlue));
		dayFogcolour = (opaqueDayFogRed << 16) + (opaqueDayFogGreen << 8) + opaqueDayFogBlue;
		fogTransparency = fFogTransparency;
	}
	gosASSERT(result == NO_ERROR);
#endif
	int32_t userMin, userMax, baseTerrain;
	cameraFile->readIdLong("UserMin", userMin);
	cameraFile->readIdLong("UserMax", userMax);
	cameraFile->readIdLong("BaseTerrain", baseTerrain);
	land->setUserSettings(userMin, userMax, baseTerrain);
	//-------------------------------------------
	// To get it working again.
	usePerspective = true;
	camera_fov = 40.0f;
	cosHalfFOV =
		cos(camera_fov * 0.7071f * DEGREES_TO_RADS); // Cosine of half the FOV for view cone.
	float anglePercent = (projectionAngle - MIN_PERSPECTIVE) / (MAX_PERSPECTIVE - MIN_PERSPECTIVE);
	float testMax = Camera::AltitudeMaximumLo + ((Camera::AltitudeMaximumHi - Camera::AltitudeMaximumLo) * anglePercent);
	newScaleFactor = 1.0f - ((cameraAltitude - AltitudeMinimum) / testMax);
	terrainLightNight = false;
	terrainLightCalc = true;
	//	if (ready)
	//	{
	//		activate();
	//		update();
	//	}
	return (NO_ERROR);
}

//---------------------------------------------------------------------------
void
Camera::destroy(void)
{
	if (worldLights)
	{
		systemHeap->Free(worldLights[1]);
		worldLights[1] = nullptr;
		systemHeap->Free(worldLights[0]);
		worldLights[0] = nullptr;
		systemHeap->Free(worldLights);
		worldLights = nullptr;
	}
	if (activeLights)
	{
		systemHeap->Free(activeLights);
		activeLights = nullptr;
	}
	if (terrainLights)
	{
		systemHeap->Free(terrainLights);
		terrainLights = nullptr;
	}
}

//---------------------------------------------------------------------------
void
Camera::getClosestVertex(Stuff::Vector2DOf<int32_t>& screenPos, int32_t& row, int32_t& col)
{
	//--------------------------------------------------------
	// New method.  Use the terrain window to figure out
	// where we are by finding the vertex closest to the clicked
	// position and returning its 3d pos.
	VertexPtr topVertex = land->getVertexList();
	uint32_t numvertices = land->getNumVertices();
	VertexPtr closestVertex = nullptr;
	int32_t whichVertex = 0;
	//
	// Redone all using integers and no square root.
	//
	uint32_t i = 0;
	uint32_t tvx = screenPos.x;
	uint32_t tvy = screenPos.y;
	float dx, dy, cd = 1e38f, dist;
	for (i = 0; i < numvertices; i++)
	{
		dx = (tvx - topVertex->px);
		dy = (tvy - topVertex->py);
		dist = dx * dx + dy * dy;
		if (dist < cd)
		{
			cd = dist;
			closestVertex = topVertex;
			whichVertex = i;
		}
		topVertex++;
	}
	int32_t block = closestVertex->blockVertex >> 16;
	int32_t vertex = closestVertex->blockVertex & 0x0000ffff;
	int32_t blockX = block % Terrain::blocksMapSide;
	int32_t blockY = block / Terrain::blocksMapSide;
	int32_t vertexX = vertex % Terrain::verticesBlockSide;
	int32_t vertexY = vertex / Terrain::verticesBlockSide;
	col = blockX * Terrain::verticesBlockSide + vertexX;
	row = blockY * Terrain::verticesBlockSide + vertexY;
}

//---------------------------------------------------------------------------

inline void
mapTileCellToWorldPos(
	int32_t tileR, int32_t tileC, int32_t cellR, int32_t cellC, Stuff::Vector3D& worldPos)
{
	worldPos.x = Terrain::tileColToWorldCoord[tileC] + Terrain::cellToWorldCoord[cellC] + Terrain::halfWorldUnitsPerCell;
	worldPos.y = Terrain::tileRowToWorldCoord[tileR] - Terrain::cellToWorldCoord[cellR] - Terrain::halfWorldUnitsPerCell;
	worldPos.z = (float)0.0;
}

//---------------------------------------------------------------------------

inline void
mapCellToWorldPos(int32_t cellR, int32_t cellC, Stuff::Vector3D& worldPos)
{
	worldPos.x = Terrain::cellColToWorldCoord[cellC] + Terrain::halfWorldUnitsPerCell;
	worldPos.y = Terrain::cellRowToWorldCoord[cellR] - Terrain::halfWorldUnitsPerCell;
	worldPos.z = (float)0.0;
}

//---------------------------------------------------------------------------
inline bool
overThisTile(TerrainQuadPtr tile, int32_t mouseX, int32_t mouseY)
{
	if (tile)
	{
		Stuff::Point3D v0, v1, v2;
		v0.x = tile->vertices[0]->px;
		v0.y = tile->vertices[0]->py;
		v1.x = tile->vertices[1]->px;
		v1.y = tile->vertices[1]->py;
		v2.x = tile->vertices[2]->px;
		v2.y = tile->vertices[2]->py;
		v0.z = v1.z = v2.z = 0.0f;
		// Can trivially reject IF vertex values are these, this triangle was
		// NOT on screen!!
		if ((v0.x == 10000.0f) || (v0.y == 10000.0f) || (v1.x == 10000.0f) || (v1.y == 10000.0f) || (v2.x == 10000.0f) || (v2.y == 10000.0f))
			return false;
		// Using the above vertex Data, determine if the mouse is over this
		// poly!
		//
		// Theorem:
		// Given the sides of the triangle defined by the lines v0v1, v1v2 and
		// v2v0 in the form Ax + By + C = 0
		//
		// the point mousex, mousey lies inside the triangle if and only if
		//
		//  	A0 * mouseX + B0 * mouseY + C0 = D0
		//		A1 * mouseX * B1 * mouseY + c1 = D1
		//		A2 * mouseX + B2 * mouseY + c2 = D2
		//
		// Dx is the same sign for each line as the correct sign for clockwise
		// or counterclockwise vertices!
		//
		Stuff::Vector3D line1;
		Stuff::Vector3D line2;
		line1.Subtract(v0, v1);
		line2.Subtract(v1, v2);
		float order = line2.x * line1.y - line1.x * line2.y;
		order = sign(order);
		float A0 = -(v0.y - v1.y);
		float B0 = (v0.x - v1.x);
		float C0 = -B0 * (v0.y) - A0 * (v0.x);
		float D0 = A0 * mouseX + B0 * mouseY + C0;
		float A1 = -(v1.y - v2.y);
		float B1 = (v1.x - v2.x);
		float C1 = -B1 * (v1.y) - A1 * (v1.x);
		float D1 = A1 * mouseX + B1 * mouseY + C1;
		float A2 = -(v2.y - v0.y);
		float B2 = (v2.x - v0.x);
		float C2 = -B2 * (v2.y) - A2 * (v2.x);
		float D2 = A2 * mouseX + B2 * mouseY + C2;
		if ((sign(D0) == order) && (sign(D0) == sign(D1)) && (sign(D0) == sign(D2)))
			return true;
		// Tiles are TWO Polys.  Check number two.
		v0.x = tile->vertices[0]->px;
		v0.y = tile->vertices[0]->py;
		v1.x = tile->vertices[2]->px;
		v1.y = tile->vertices[2]->py;
		v2.x = tile->vertices[3]->px;
		v2.y = tile->vertices[3]->py;
		v0.z = v1.z = v2.z = 0.0f;
		// Can trivially reject IF vertex values are these, this triangle was
		// NOT on screen!!
		if ((v0.x == 10000.0f) || (v0.y == 10000.0f) || (v1.x == 10000.0f) || (v1.y == 10000.0f) || (v2.x == 10000.0f) || (v2.y == 10000.0f))
			return false;
		// Using the above vertex Data, determine if the mouse is over this
		// poly!
		//
		// Theorem:
		// Given the sides of the triangle defined by the lines v0v1, v1v2 and
		// v2v0 in the form Ax + By + C = 0
		//
		// the point mousex, mousey lies inside the triangle if and only if
		//
		//  	A0 * mouseX + B0 * mouseY + C0 = D0
		//		A1 * mouseX * B1 * mouseY + c1 = D1
		//		A2 * mouseX + B2 * mouseY + c2 = D2
		//
		// Dx is the same sign for each line as the correct sign for clockwise
		// or counterclockwise vertices!
		//
		line1.Subtract(v0, v1);
		line2.Subtract(v1, v2);
		order = line2.x * line1.y - line1.x * line2.y;
		order = sign(order);
		A0 = -(v0.y - v1.y);
		B0 = (v0.x - v1.x);
		C0 = -B0 * (v0.y) - A0 * (v0.x);
		D0 = A0 * mouseX + B0 * mouseY + C0;
		A1 = -(v1.y - v2.y);
		B1 = (v1.x - v2.x);
		C1 = -B1 * (v1.y) - A1 * (v1.x);
		D1 = A1 * mouseX + B1 * mouseY + C1;
		A2 = -(v2.y - v0.y);
		B2 = (v2.x - v0.x);
		C2 = -B2 * (v2.y) - A2 * (v2.x);
		D2 = A2 * mouseX + B2 * mouseY + C2;
		if ((sign(D0) == order) && (sign(D0) == sign(D1)) && (sign(D0) == sign(D2)))
			return true;
	}
	return false;
}

//---------------------------------------------------------------------------
uint32_t
Camera::inverseProject(Stuff::Vector2DOf<int32_t>& screenPos, Stuff::Vector3D& point)
{
	if (turn < 4)
	{
		point.x = point.y = point.z = 0.0f;
		return 0;
	}
	//-----------------------------------------------------------
	// Pick the tile that has the vertex with the LEAST Z value.
	TerrainQuadPtr currentTile = land->getQuadList();
	uint32_t numTiles = land->getNumQuads();
	TerrainQuadPtr closestTiles[100];
	memset(closestTiles, 0, sizeof(TerrainQuadPtr) * 100);
	TerrainQuadPtr closestTile = nullptr;
	int32_t currentClosest = 0;
	VertexPtr closestVertex = nullptr;
	for (size_t i = 0; i < (int32_t)numTiles; i++)
	{
		if ((currentTile->vertices[0]->clipInfo) || (currentTile->vertices[1]->clipInfo) || (currentTile->vertices[2]->clipInfo) || (currentTile->vertices[3]->clipInfo))
		{
			bool isOver = overThisTile(currentTile, screenPos.x, screenPos.y);
			if (isOver && (currentClosest < 100))
			{
				// We are.  Save it into the closestTiles list.
				closestTiles[currentClosest] = currentTile;
				currentClosest++;
				//			if (currentClosest >= 100)
				//				STOP(("FOUND Too Many Tiles Under the mouse
				// cursor"));
			}
		}
		currentTile++;
	}
	if (currentClosest > 1)
	{
		float leastZ = 1.0f;
		for (size_t i = 0; i < currentClosest; i++)
		{
			// Find the least Z tile.
			if ((closestTiles[i]->vertices[0]->pz > 0.0f) && (closestTiles[i]->vertices[1]->pz > 0.0f) && (closestTiles[i]->vertices[2]->pz > 0.0f) && (closestTiles[i]->vertices[3]->pz > 0.0f))
			{
				if (closestTiles[i]->vertices[0]->pz < leastZ)
				{
					leastZ = closestTiles[i]->vertices[0]->pz;
					closestTile = closestTiles[i];
				}
				if (closestTiles[i]->vertices[1]->pz < leastZ)
				{
					leastZ = closestTiles[i]->vertices[1]->pz;
					closestTile = closestTiles[i];
				}
				if (closestTiles[i]->vertices[2]->pz < leastZ)
				{
					leastZ = closestTiles[i]->vertices[2]->pz;
					closestTile = closestTiles[i];
				}
				if (closestTiles[i]->vertices[3]->pz < leastZ)
				{
					leastZ = closestTiles[i]->vertices[3]->pz;
					closestTile = closestTiles[i];
				}
			}
		}
	}
	else if (currentClosest)
	{
		closestTile = closestTiles[0];
	}
	if (closestTile)
	{
#ifdef _DEBUG
		closestTile->selected = TRUE;
		closestTile->vertices[0]->selected = TRUE;
#endif
		//-----------------------------------------------------------------------------
		// Once we have the tile, use the vertex[0] to find out which cell we
		// are in.
		closestVertex = closestTile->vertices[0];
		//-------------------------------------------------------------------
		// Really new method.  Snap to center of closest Cell.  This is ideal
		// because movement should always be to this location and Heidi and Mike
		// need cell resolution in the Editor!
		Stuff::Vector4D cellCenter;
		int32_t cellCenterC = -1;
		int32_t cellCenterR = -1;
		float cellwidth = Terrain::worldUnitsPerCell;
		float halfCellwidth = cellwidth / 2.0f;
		int32_t tileC = -1, tileR = -1;
		int32_t VerticesMapSideDivTwo = Terrain::realVerticesMapSide / 2;
		int32_t MetersMapSideDivTwo =
			VerticesMapSideDivTwo * float2long(Terrain::worldUnitsPerVertex);
		int32_t dx, dy, cd = 1 << 30, dist, tvx = screenPos.x, tvy = screenPos.y;
		for (size_t cellC = 0; cellC < 3; cellC++)
		{
			for (size_t cellR = 0; cellR < 3; cellR++)
			{
				point.x = closestVertex->vx;
				point.y = closestVertex->vy;
				point.z = closestVertex->pVertex->elevation;
				point.x += (cellC)*cellwidth + halfCellwidth;
				point.y -= (cellR)*cellwidth + halfCellwidth;
				point.z = land->getTerrainElevation(point);
				eye->projectZ(point, cellCenter);
				dx = (tvx - float2long(cellCenter.x));
				dy = (tvy - float2long(cellCenter.y));
				dist = dx * dx + dy * dy;
				if (dist < cd)
				{
					cd = dist;
					cellCenterC = cellC;
					cellCenterR = cellR;
					tileC = float2long(
						point.x * Terrain::oneOverWorldUnitsPerVertex + VerticesMapSideDivTwo);
					tileR = float2long(
						(MetersMapSideDivTwo - point.y) * Terrain::oneOverWorldUnitsPerVertex);
				}
			}
		}
		if (tileR < 0)
			tileR = 0;
		if (tileR >= Terrain::realVerticesMapSide)
			tileR = Terrain::realVerticesMapSide - 1;
		if (tileC < 0)
			tileC = 0;
		if (tileC >= Terrain::realVerticesMapSide)
			tileC = Terrain::realVerticesMapSide - 1;
		mapTileCellToWorldPos(tileR, tileC, cellCenterR, cellCenterC, point);
		point.z = land->getTerrainElevation(point);
	}
	else
	{
		// not in any tile, must be off map.  We're going to return the coord of
		// the closest vertex
		float dis = 999999999.f;
		int32_t closeRow = 0, closeCol = 0;
		Stuff::Vector3D tmpWorld;
		Stuff::Vector4D tmpScreen;
		/*for ( int32_t column = 0; column < land->realVerticesMapSide; column
		+= land->realVerticesMapSide - 1 )
		{
			for ( int32_t row = 0; row < land->realVerticesMapSide; ++row )
			{
				tmpWorld.x = land->tileColToWorldCoord[column];
				tmpWorld.y = land->tileRowToWorldCoord[row];
				tmpWorld.z = land->getTerrainElevation( row, column );

				projectZ( tmpWorld, tmpScreen );

				float tmpDis = (tmpScreen.x - screenPos.x) * (tmpScreen.x -
		screenPos.x ) + (tmpScreen.y - screenPos.y) * (tmpScreen.y - screenPos.y
		); if ( tmpDis < dis )
				{
					closeRow = row;
					closeCol = column;
					dis = tmpDis;
				}
			}
		}*/
		for (size_t column = 0; column < land->realVerticesMapSide; column++)
		{
			for (size_t row = 0; row < land->realVerticesMapSide;
				 row++ /*row += land->realVerticesMapSide - 1*/)
			{
				tmpWorld.x = land->tileColToWorldCoord[column];
				tmpWorld.y = land->tileRowToWorldCoord[row];
				tmpWorld.z = land->getTerrainElevation(row, column);
				projectZ(tmpWorld, tmpScreen);
				float tmpDis = (tmpScreen.x - screenPos.x) * (tmpScreen.x - screenPos.x) + (tmpScreen.y - screenPos.y) * (tmpScreen.y - screenPos.y);
				if (tmpDis < dis)
				{
					closeRow = row;
					closeCol = column;
					dis = tmpDis;
				}
			}
		}
		point.x = land->tileColToWorldCoord[closeCol];
		point.y = land->tileRowToWorldCoord[closeRow];
		point.z = land->getTerrainElevation(closeRow, closeCol);
		return 1; // don't return success for bogus value
	}
	return (0);
}

//---------------------------------------------------------------------------
void
Camera::updateGoalVelocity(void)
{
	if (goalVelTime > 0.0)
	{
		Stuff::Vector3D velDiff;
		velDiff.Subtract(goalVelocity, velocity);
		velDiff /= goalVelTime;
		velDiff *= frameLength;
		goalVelTime -= frameLength;
		if (goalVelTime < 0.0)
			velocity = goalVelocity;
		else
			velocity.Add(velocity, velDiff);
	}
	else
	{
		goalVelTime = 0.0;
		goalVelocity = velocity;
	}
}

//---------------------------------------------------------------------------
void
Camera::updateGoalFOV(void)
{
	if (goalFOVTime > 0.0)
	{
		float fovDiff = goalFOV - camera_fov;
		fovDiff /= goalFOVTime;
		fovDiff *= frameLength;
		goalFOVTime -= frameLength;
		if (goalFOVTime < 0.0)
		{
			camera_fov = goalFOV;
		}
		else
		{
			camera_fov += fovDiff;
		}
	}
	else
	{
		goalFOVTime = 0.0;
	}
}

//---------------------------------------------------------------------------
void
Camera::updateGoalRotation(void)
{
	if (goalRotTime > 0.0)
	{
		float goalProjectionAngle = goalRotation.x - projectionAngle;
		goalProjectionAngle /= goalRotTime;
		goalProjectionAngle *= frameLength;
		float goalCameraRotation = goalRotation.y - cameraRotation;
		goalCameraRotation /= goalRotTime;
		goalCameraRotation *= frameLength;
		goalRotTime -= frameLength;
		if (goalRotTime < 0.0)
		{
			projectionAngle = goalRotation.x;
			cameraRotation = goalRotation.y;
			setCameraRotation(cameraRotation, cameraRotation);
		}
		else
		{
			projectionAngle += goalProjectionAngle;
			cameraRotation += goalCameraRotation;
			setCameraRotation(cameraRotation, cameraRotation);
		}
	}
	else
	{
		goalRotTime = 0.0;
	}
}

//---------------------------------------------------------------------------
void
Camera::updateGoalPosition(Stuff::Vector3D& pos)
{
	if (goalPosTime > 0.0)
	{
		Stuff::Vector3D posDiff;
		posDiff.Subtract(goalPosition, pos);
		posDiff /= goalPosTime;
		posDiff *= frameLength;
		goalPosTime -= frameLength;
		if (goalPosTime < 0.0f)
			pos = goalPosition;
		else
			pos.Add(position, posDiff);
	}
	else
	{
		goalPosTime = 0.0;
		goalPosition = pos;
	}
}

//---------------------------------------------------------------------------
static const float rhatx = 1.0; // cos(0/*degrees*/ * DEGREES_TO_RADS);
static const float rhaty = 0.0; // sin(0/*degrees*/ * DEGREES_TO_RADS);
static const float ghatx = -0.5; // cos(120/*degrees*/ * DEGREES_TO_RADS);
static const float ghaty = 0.86602540378443864676372317075294f; // sin(120/*degrees*/ *
	// DEGREES_TO_RADS);
static const float bhatx = -0.5; // cos(240/*degrees*/ * DEGREES_TO_RADS);
static const float bhaty = -0.86602540378443864676372317075294f; // sin(240/*degrees*/ *
	// DEGREES_TO_RADS);
static const float two_pi = 6.283185307179586476925286766559f;

/* all params range from 0.0 to 1.0 */
static void
rgb2hsi(float r, float g, float b, float& hue, float& saturation, float& intensity)
{
	intensity = (r + g + b) / 3.0;
	if (0.0 >= intensity)
	{
		saturation = 0.0;
		hue = 0.0;
		return;
	}
	float min = r;
	if (g < min)
	{
		min = g;
	}
	if (b < min)
	{
		min = b;
	}
	saturation = 1.0 - (min / intensity);
	float cx = r * rhatx + g * ghatx + b * bhatx;
	float cy = r * rhaty + g * ghaty + b * bhaty;
	hue = atan2(cy, cx);
	if (0.0 > hue)
	{
		hue += two_pi;
	}
	hue /= two_pi;
}

static void
hsi2rgb(float hue, float saturation, float intensity, float& r, float& g, float& b)
{
	float thue = hue;
	if ((1.0f / 3.0f) > hue)
	{
	}
	else if ((2.0f / 3.0f) > hue)
	{
		thue -= 1.0f / 3.0f;
	}
	else
	{
		thue -= 2.0f / 3.0f;
	}
	float chatx = cos(two_pi * thue);
	float chaty = sin(two_pi * thue);
	float ti = 3.0f * intensity;
	{
		b = (1.0f - saturation) * intensity;
		float tib = ti - b;
		float denominator = (chaty * (ghatx - rhatx) - chatx * (ghaty - rhaty));
		if (0.0f == denominator)
		{
			/* I suspect it's mathematically impossible to get here, but I'm too
			 * lazy to make sure. */
			denominator = 0.000001f;
		}
		g = ((b * bhaty + tib * rhaty) * chatx - (b * bhatx + tib * rhatx) * chaty) / denominator;
		r = tib - g;
	}
	if ((1.0f / 3.0f) > hue)
	{
	}
	else if ((2.0f / 3.0f) > hue)
	{
		float swap = b;
		b = g;
		g = r;
		r = swap;
	}
	else
	{
		float swap = b;
		b = r;
		r = g;
		g = swap;
	}
}

/* interpolation with dy/dx == 0 at x = 0 and x = 1 */
float
smoothConnectingInterpolation(float y0, float y1, float x)
{
	float a = 0.5 * (y0 - y1);
	float b = 0.5 * (y0 + y1);
	float retval = a * cos(x * (0.5 * two_pi)) + b;
	return retval;
}

void
Camera::updateDaylight(bool bInitialize)
{
	float day2NightTransitionTime = this->day2NightTransitionTime;
	if (bInitialize)
	{
		if (day2NightTransitionTime <= 0.0f)
		{
			day2NightTransitionTime = 3600.0f /*seconds*/;
		}
		dayLightTime = dayLightPitch / 360.0f /*degrees*/ * day2NightTransitionTime;
	}
	if (day2NightTransitionTime > 0.0f)
	{
		while (dayLightTime > day2NightTransitionTime)
		{
			dayLightTime -= day2NightTransitionTime;
		}
		float oneOverDay2NightTransitionTime = 1.0f / day2NightTransitionTime;
		lightPitch = dayLightTime * oneOverDay2NightTransitionTime * 360.0 /*degrees*/;
		/* arbitrary assumed daytime sky color */
		static const float fDaySkyRed = 193.0f;
		static const float fDaySkyGreen = 231.0f;
		static const float fDaySkyBlue = 238.0f;
		float fSkyRed;
		float fSkyGreen;
		float fSkyBlue;
		float fAmbientRed;
		float fAmbientGreen;
		float fAmbientBlue;
		float fLightRed;
		float fLightGreen;
		float fLightBlue;
		if (180.0f /*degrees*/ >= lightPitch)
		{
			// Set flags to help renderer with light sources.
			float lightAltitude = lightPitch;
			if (lightPitch > 90.0f)
				lightAltitude = 180.0f - lightPitch;
			isNight = false;
			if (lightAltitude < NIGHT_LIGHT_PITCH)
				isNight = true;
			if (lightAltitude > NIGHT_START_PITCH)
			{
				nightFactor = 0.0f;
			}
			else
			{
				nightFactor =
					(NIGHT_START_PITCH - lightAltitude) / (NIGHT_START_PITCH - NIGHT_LIGHT_PITCH);
				if (nightFactor > 1.0f)
					nightFactor = 1.0f;
			}
			/* daytime */
			float fSineOfPitch = sin(lightPitch * DEGREES_TO_RADS);
			/* these calculations are sort of based on optics */
			// Do not let day ambient go darker then nightAmbient for smooth
			// transitions.
			fAmbientRed = (dayAmbientRed - nightAmbientRed) * fSineOfPitch + nightAmbientRed;
			fAmbientGreen =
				(dayAmbientGreen - nightAmbientGreen) * fSineOfPitch + nightAmbientGreen;
			fAmbientBlue = (dayAmbientBlue - nightAmbientBlue) * fSineOfPitch + nightAmbientBlue;
			/* these sunset color calculations are not based on optics*/
			float fAngleFromTheHorizon = 90.0 /*degrees*/ - fabs(90.0 /*degrees*/ - lightPitch);
			/*static*/ const float fSunSetStartAngle = 35.0 /*degrees*/;
			if (fSunSetStartAngle > fAngleFromTheHorizon)
			{
				const float fSunSetRed = sunsetLightRed;
				const float fSunSetGreen = sunsetLightGreen;
				const float fSunSetBlue = sunsetLightBlue;
				float fSunSetHue;
				float fSunSetSaturation;
				float fSunSetIntensity;
				rgb2hsi(fSunSetRed / 255.0f, fSunSetGreen / 255.0f, fSunSetBlue / 255.0f,
					fSunSetHue, fSunSetSaturation, fSunSetIntensity);
				float fDayLightHue;
				float fDayLightSaturation;
				float fDayLightIntensity;
				rgb2hsi(dayLightRed / 255.0f, dayLightGreen / 255.0f, dayLightBlue / 255.0f,
					fDayLightHue, fDayLightSaturation, fDayLightIntensity);
				float fSunSetProportion = 0.75 /*arbitrary*/ * sin((fSunSetStartAngle - fAngleFromTheHorizon) * (90.0f /*degrees*/ / fSunSetStartAngle) * DEGREES_TO_RADS);
				float fOneMinusSunSetProportion = 1.0f - fSunSetProportion;
				/* for sunlight color, linear interpolation in HSI color space
				is a better model than linear interpolation in RGB color space
			  */
				float H = fSunSetHue * fSunSetProportion + fDayLightHue * fOneMinusSunSetProportion;
				float S = fSunSetSaturation * fSunSetProportion + fDayLightSaturation * fOneMinusSunSetProportion;
				float I = fSunSetIntensity * fSunSetProportion + fDayLightIntensity * fOneMinusSunSetProportion;
				float R, G, B;
				hsi2rgb(H, S, I, R, G, B);
				if (R > 0.99f)
				{
					R = 0.99f;
				}
				if (G > 0.99f)
				{
					G = 0.99f;
				}
				if (B > 0.99f)
				{
					B = 0.99f;
				}
				if (R < 0.0f)
				{
					R = 0.0f;
				}
				if (G < 0.0f)
				{
					G = 0.0f;
				}
				if (B < 0.0f)
				{
					B = 0.0f;
				}
				fLightRed = R * 255.0f;
				fLightGreen = G * 255.0f;
				fLightBlue = B * 255.0f;
				/* Basically I'm using an arbitrary harcoded formula (which may
				need some tweaking) to approximate the sky color as it
				progresses to sunset. For more accurate results it would
				probably be better to store a table of colors obtained from
				photographs. */
				fSunSetProportion = sin((fSunSetStartAngle - fAngleFromTheHorizon) * (90.0f /*degrees*/ / fSunSetStartAngle) * DEGREES_TO_RADS);
				float fSunAzimuth = lightYaw;
				if (90.0 /*degrees*/ < lightPitch)
				{
					fSunAzimuth += 180.0 /*degress*/;
				}
				float fAzimuthCameraAngleRelativeToTheSun =
					fmod(fabs(fSunAzimuth - (getCameraRotation() - 135.0)), 360.0 /*degrees*/);
				if (180.0 /*degress*/ < fAzimuthCameraAngleRelativeToTheSun)
				{
					fAzimuthCameraAngleRelativeToTheSun =
						360.0 /*degrees*/ - fAzimuthCameraAngleRelativeToTheSun;
				}
				fSunSetProportion *= (1.0 - pow(fAzimuthCameraAngleRelativeToTheSun / 180.0 /*degress*/, 2.0 /*arbitrary*/));
				fOneMinusSunSetProportion = 1.0 - fSunSetProportion;
				float fDaySkyHue;
				float fDaySkySaturation;
				float fDaySkyIntensity;
				rgb2hsi(fDaySkyRed / 255.0f, fDaySkyGreen / 255.0f, fDaySkyBlue / 255.0f,
					fDaySkyHue, fDaySkySaturation, fDaySkyIntensity);
				H = fSunSetHue * fSunSetProportion + fDaySkyHue * fOneMinusSunSetProportion;
				/*static*/ const float fGreenHue = 1.0f / 3.0f;
				float za;
				if (fGreenHue > H)
				{
					/*static*/ const float fGreenHueMargin = 1.0 / 6.0 + 0.12 /*arbitrary*/;
					if ((fGreenHue - fGreenHueMargin) > H)
					{
						za = 0.0;
					}
					else
					{
						za = smoothConnectingInterpolation(
							0, 1, (H - (fGreenHue - fGreenHueMargin)) / fGreenHueMargin);
						za = pow(fabs(za), 0.15f /*arbitrary*/);
					}
				}
				else
				{
					/*static*/ const float fGreenHueMargin = 1.0 / 6.0 + 0.00 /*arbitrary*/;
					if ((fGreenHue + fGreenHueMargin) < H)
					{
						za = 0.0;
					}
					else
					{
						za = smoothConnectingInterpolation(
							0, 1, ((fGreenHue + fGreenHueMargin) - H) / fGreenHueMargin);
						za = pow(fabs(za), 0.15f /*arbitrary*/);
					}
				}
				S = fSunSetSaturation * fSunSetProportion + fDaySkySaturation * fOneMinusSunSetProportion;
				S = 0.0f /*arbitrary*/ * za + S * (1.0f - za);
				/*static*/ const float brightHue = 0.25 * 1.0 / 3.0; /*arbitrary*/
				float zb;
				if (brightHue > H)
				{
					if (brightHue != fSunSetHue)
					{
						zb = sin((H - fSunSetHue) / (brightHue - fSunSetHue) * 0.35f * two_pi);
					}
					else
					{
						zb = 0.0;
					}
				}
				else
				{
					zb = sin((fDaySkyHue - H) / (fDaySkyHue - brightHue) * 0.35f * two_pi);
				}
				zb = pow(fabs(zb), 1.0f);
				I = fSunSetIntensity * fSunSetProportion + fDaySkyIntensity * fOneMinusSunSetProportion;
				I = 0.85f /*arbitrary*/ * zb + I * (1.0f - zb);
				I *= (1.0 - (fAzimuthCameraAngleRelativeToTheSun / 180.0 /*degress*/) * (1.0 - (fAngleFromTheHorizon / fSunSetStartAngle)));
				hsi2rgb(H, S, I, R, G, B);
				if (R > 0.99f)
				{
					R = 0.99f;
				}
				if (G > 0.99f)
				{
					G = 0.99f;
				}
				if (B > 0.99f)
				{
					B = 0.99f;
				}
				if (R < 0.0f)
				{
					R = 0.0f;
				}
				if (G < 0.0f)
				{
					G = 0.0f;
				}
				if (B < 0.0f)
				{
					B = 0.0f;
				}
				fSkyRed = R * 255.0f;
				fSkyGreen = G * 255.0f;
				fSkyBlue = B * 255.0f;
				/*static*/ const float fFadeToDarknessStartAngle = 5.0 /*degrees*/;
				if (fFadeToDarknessStartAngle > fAngleFromTheHorizon)
				{
					float fOneMinusDarknessProportion = sin((fAngleFromTheHorizon) * (90.0f /*degrees*/ / fFadeToDarknessStartAngle) * DEGREES_TO_RADS);
					float fDarknessProportion = 1.0f - fOneMinusDarknessProportion;
					fLightRed = fLightRed * fOneMinusDarknessProportion + nightLightRed * fDarknessProportion;
					fLightGreen = fLightGreen * fOneMinusDarknessProportion + nightLightGreen * fDarknessProportion;
					fLightBlue = fLightBlue * fOneMinusDarknessProportion + nightLightBlue * fDarknessProportion;
					fSkyRed = fSkyRed * fOneMinusDarknessProportion;
					fSkyGreen = fSkyGreen * fOneMinusDarknessProportion;
					fSkyBlue = fSkyBlue * fOneMinusDarknessProportion;
				}
			}
			else
			{
				fLightRed = dayLightRed;
				fLightGreen = dayLightGreen;
				fLightBlue = dayLightBlue;
				fSkyRed = fDaySkyRed;
				fSkyGreen = fDaySkyGreen;
				fSkyBlue = fDaySkyBlue;
			}
		}
		else
		{
			// Set flags to help renderer with light sources.
			isNight = true;
			nightFactor = 1.0f;
			/* nighttime */
			float moonPitch = 180.0f /*degrees*/ - fabs(180.0f /*degrees*/ - lightPitch);
			lightPitch = moonPitch;
			// float fSineOfPitch = sin(moonPitch * DEGREES_TO_RADS);
			/* these calculations are sort of based on optics */
			// DO NOT ever get darker then the night Ambient.  Change hue with
			// moon. Not realistic but very unplayable in the pitch dark.
			fAmbientRed = nightAmbientRed; // * fSineOfPitch;
			fAmbientGreen = nightAmbientGreen; // * fSineOfPitch;
			fAmbientBlue = nightAmbientBlue; // * fSineOfPitch;
			fLightRed = nightLightRed;
			fLightGreen = nightLightGreen;
			fLightBlue = nightLightBlue;
			fSkyRed = 0.0;
			fSkyGreen = 0.0;
			fSkyBlue = 0.0;
		}
		ambientRed = fAmbientRed;
		ambientBlue = fAmbientBlue;
		ambientGreen = fAmbientGreen;
		lightRed = fLightRed;
		lightGreen = fLightGreen;
		lightBlue = fLightBlue;
		Stuff::LinearMatrix4D lightToWorldMatrix;
		lightToWorldMatrix.BuildTranslation(Stuff::Point3D(0.0f, 0.0f, 0.0f));
		lightToWorldMatrix.BuildRotation(Stuff::EulerAngles(
			lightPitch * DEGREES_TO_RADS, (lightYaw + 135.0) * DEGREES_TO_RADS, 0.0f));
		worldLights[0]->SetLightToWorld(&lightToWorldMatrix);
		/* In the real world, fog color is generally varies with the ambient
		color, but in our game fog color more reflects "sky color near the
		horizon" */
		float fFogTransparency = fogTransparency;
		float fOpaqueDayFogRed = (float)((dayFogcolour >> 16) & 0xff);
		float fOpaqueDayFogGreen = (float)((dayFogcolour >> 8) & 0xff);
		float fOpaqueDayFogBlue = (float)((dayFogcolour)&0xff);
		static const float fBlackSkyLevel = 32.0f; /* If the light is below this level, the sky is
													  effectively black. */
		// float fFogRed = fLightRed - fBlackSkyLevel;
		// float fFogGreen = fLightGreen - fBlackSkyLevel;
		// float fFogBlue = fLightBlue - fBlackSkyLevel;
		float fFogRed = fAmbientRed - fBlackSkyLevel;
		float fFogGreen = fAmbientGreen - fBlackSkyLevel;
		float fFogBlue = fAmbientBlue - fBlackSkyLevel;
		if (0.0f > fFogRed)
		{
			fFogRed = 0.0f;
		}
		if (0.0f > fFogGreen)
		{
			fFogGreen = 0.0f;
		}
		if (0.0f > fFogBlue)
		{
			fFogBlue = 0.0f;
		}
		// float fTmpRed = (float)(dayLightRed) - fBlackSkyLevel;
		// float fTmpGreen = (float)(dayLightGreen) - fBlackSkyLevel;
		// float fTmpBlue = (float)(dayLightBlue) - fBlackSkyLevel;
		float fTmpRed = (float)(dayAmbientRed)-fBlackSkyLevel;
		float fTmpGreen = (float)(dayAmbientGreen)-fBlackSkyLevel;
		float fTmpBlue = (float)(dayAmbientBlue)-fBlackSkyLevel;
		float epsilon = 1.0;
		if (epsilon > fTmpRed)
		{
			fTmpRed = epsilon;
		}
		if (epsilon > fTmpGreen)
		{
			fTmpGreen = epsilon;
		}
		if (epsilon > fTmpBlue)
		{
			fTmpBlue = epsilon;
		}
		float fOneMinusFogTransparency = 1.0 - fFogTransparency;
		fFogRed = fFogRed / fTmpRed * fOpaqueDayFogRed * fOneMinusFogTransparency;
		fFogGreen = fFogGreen / fTmpGreen * fOpaqueDayFogGreen * fOneMinusFogTransparency;
		fFogBlue = fFogBlue / fTmpBlue * fOpaqueDayFogBlue * fOneMinusFogTransparency;
		/* Here we add in the sky color component. */
		fFogRed = fFogRed + fFogTransparency * fSkyRed;
		fFogGreen = fFogGreen + fFogTransparency * fSkyGreen;
		fFogBlue = fFogBlue + fFogTransparency * fSkyBlue;
		uint32_t fogRed = fFogRed;
		uint32_t fogGreen = fFogGreen;
		uint32_t fogBlue = fFogBlue;
		if (0xff < fogRed)
		{
			fogRed = 0xff;
		}
		if (0xff < fogGreen)
		{
			fogGreen = 0xff;
		}
		if (0xff < fogBlue)
		{
			fogBlue = 0xff;
		}
		fogcolour = (fogRed << 16) + (fogGreen << 8) + fogBlue;
		dayLightTime += frameLength;
	}
}

//---------------------------------------------------------------------------
void
Camera::updateLetterboxAndFade(void)
{
	// OK, if we are inMovieMode and we haven't finished letterboxing, finish
	// it!
	if (!startEnding && inMovieMode && (letterBoxTime != MaxLetterBoxTime))
	{
		letterBoxTime += frameLength;
		float letterBoxPercent = letterBoxTime / MaxLetterBoxTime;
		if (letterBoxTime > MaxLetterBoxTime)
		{
			letterBoxPercent = 1.0f;
			letterBoxTime = MaxLetterBoxTime;
		}
		letterBoxPos = letterBoxPercent * MaxLetterBoxPos;
		letterBoxAlpha = (letterBoxPercent * 255.0f);
	}
	// Now, if we are in startEnding and we haven't finished DE-Letterboxing,
	// finish it.
	if (startEnding && inMovieMode && (letterBoxPos != 0.0f))
	{
		letterBoxTime -= frameLength;
		float letterBoxPercent = letterBoxTime / MaxLetterBoxTime;
		if (letterBoxTime < 0.0f)
		{
			letterBoxPercent = 0.0f;
			inMovieMode = false;
			startEnding = false;
		}
		letterBoxPos = letterBoxPercent * MaxLetterBoxPos;
		letterBoxAlpha = (letterBoxPercent * 255.0f);
	}
	// If we are fading toward a color, continue the fade.
	// We only fades during movie Mode at present!!
	if (inMovieMode && inFadeMode)
	{
		timeLeftToFade += frameLength;
		float fadePercent = timeLeftToFade / startFadeTime;
		if (timeLeftToFade > startFadeTime)
		{
			fadePercent = 1.0f;
			inFadeMode = false;
		}
		fadeAlpha =
			(fadeStart >> 24) + (fadePercent * (float(fadecolour >> 24) - float(fadeStart >> 24)));
	}
}

//---------------------------------------------------------------------------
int32_t
Camera::update(void)
{
	//---------------------------------------------------------
	// This is the guts.  This routine will be used to
	// move the camera, cut to different shots, etc.
	switch (cameraClass)
	{
	case BASE_CAMERA:
		break;
	default:
		// return INVALID_CAMERA;
		NODEFAULT;
	}
	float oldNightFactor = nightFactor;
	// updateDaylight();
	//----------------------------------------------------
	// Check if light moved.
	// Should not be necessary every frame.
	// TEST FOR NOW!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	lightDirection.x = cos((lightYaw + 45.0f) * DEGREES_TO_RADS);
	lightDirection.y = sin((lightYaw + 45.0f) * DEGREES_TO_RADS);
	lightDirection.z = sin(lightPitch * DEGREES_TO_RADS);
	lightDirection.Normalize(lightDirection);
	// If we are not transitioning, we need to set the night/day flags and
	// factors.  If we ARE transitioning, they have already been set in
	// updateDaylight!
	//
	// WE ARE NEVER TRANSITIONING ANYMORE
	// if (day2NightTransitionTime <= 0.0f)
	{
		// Set flags to help renderer with light sources.
		float lightAltitude = lightPitch;
		if (lightPitch > 90.0f)
			lightAltitude = 180.0f - lightPitch;
		//--------------------------------------------------
		// Set is night based on pitch.
		// Do NOT draw shadows for infinite light if NIGHT!
		if (lightAltitude < NIGHT_LIGHT_PITCH)
		{
			isNight = true;
			nightFactor = 1.0f;
		}
		else
		{
			isNight = false;
			if (lightAltitude > NIGHT_START_PITCH)
			{
				nightFactor = 0.0f;
			}
			else
			{
				nightFactor =
					(NIGHT_START_PITCH - lightPitch) / (NIGHT_START_PITCH - NIGHT_LIGHT_PITCH);
			}
		}
	}
	//-----------------------------------------------------
	// Check if we need to re-burn the terrain lights.
	if (!terrainLightCalc)
	{
		if ((!terrainLightNight) && (nightFactor > oldNightFactor) && (nightFactor > TERRAIN_LIGHTS_ON))
		{
			terrainLightCalc = true;
			terrainLightNight = true;
		}
		if ((terrainLightNight) && (nightFactor < oldNightFactor) && (nightFactor < TERRAIN_LIGHTS_ON))
		{
			terrainLightCalc = true;
			terrainLightNight = false;
		}
	}
	//--------------------------------------------------
	// Camera Scripting Here.
	// Basically, update anything we have to do.
	Stuff::Vector3D newPosition = position;
	//------------------
	// Velocity Update
	updateGoalVelocity();
	if ((velocity.x != 0.0) && (velocity.y != 0.0) && (velocity.z != 0.0))
	{
		Stuff::Vector3D posOffset;
		posOffset = velocity;
		posOffset *= frameLength;
		newPosition += posOffset;
	}
	updateGoalPosition(newPosition);
	updateGoalRotation();
	updateGoalFOV();
	setPosition(newPosition); // THIS ALWAYS sets z position for us now!!!!!
	//----------------------------------------------
	// Update fades and letterbox transitions here.
	updateLetterboxAndFade();
	//----------------------
	// Setup Matrices, etc.
	cameraShift.x = newPosition.x;
	cameraShift.y = newPosition.y;
	if (usePerspective)
		cameraShiftZ = newPosition.z;
	//--------------------------------------------------------
	// Get new viewport values to scale stuff.  No longer uses
	// VFX stuff for this.  ALL GOS NOW!
	// gos_GetViewport(&viewMulX, &viewMulY, &viewAddX, &viewAddY);
	if (land)
	{
		if (newScaleFactor >= 0.75)
			Terrain::terrainTextures->setMipLevel(0);
		else if (newScaleFactor >= 0.3)
		{
			Terrain::terrainTextures->setMipLevel(1);
		}
		else if (newScaleFactor >= 0.2)
		{
			Terrain::terrainTextures->setMipLevel(2);
		}
		else
			Terrain::terrainTextures->setMipLevel(3);
	}
	//--------------------------------------------------------
	// Get new viewport values to scale stuff.  No longer uses
	// VFX stuff for this.  ALL GOS NOW!
	screenResolution.x = viewMulX;
	screenResolution.y = viewMulY;
	calculateProjectionConstants();
	globalScaleFactor = getScaleFactor();
	globalScaleFactor *= viewMulX / Environment.screenwidth; // Scale Mechs to ScreenRES
	//-----------------------------------------------
	// Set Ambient for this pass of rendering
	uint32_t lightRGB = (ambientRed << 16) + (ambientGreen << 8) + ambientBlue;
	setLightcolour(1, lightRGB);
	setLightIntensity(1, 1.0);
	//---------------------------------------------------------------------------------
	// Check which lights are on screen and deactivate those which are NOT on
	// screen!
	numActiveLights = numTerrainLights = 0;
	for (size_t i = 0; i < MAX_LIGHTS_IN_WORLD; i++)
	{
		if (worldLights[i])
		{
			TG_LightPtr light = worldLights[i];
			// IF we are a terrain Light, we are ONLY active when day changes to
			// Night or vice/versa. On screen matters not.
			if (light->lightType == TG_LIGHT_TERRAIN)
			{
				Stuff::Vector4D dummy;
				light->active = projectZ(light->position, dummy);
				if (light->active)
				{
					if (terrainLightCalc)
						activeLights[numActiveLights++] = light;
					terrainLights[numTerrainLights++] = light;
				}
				continue;
			}
			// If we are infinite or Ambient, we MUST be active
			if ((light->lightType == TG_LIGHT_AMBIENT) || (light->lightType == TG_LIGHT_INFINITE))
			{
				light->active = true;
				activeLights[numActiveLights++] = light;
				terrainLights[numTerrainLights++] = light;
				continue;
			}
			// IF we are a Spot or Point light, we must know if we are onscreen.
			// This is complicated because these lights have a radius!
			// For now, simple position check.  Make complicated later!
			if (light->lightType >= TG_LIGHT_POINT && light->lightType < TG_LIGHT_TERRAIN)
			{
				Stuff::Vector4D dummy;
				light->active = projectZ(light->position, dummy);
				activeLights[numActiveLights++] = light;
				terrainLights[numTerrainLights++] = light;
			}
		}
	}
	TG_Shape::SetCameraMatrices(&cameraOrigin, &cameraToClip);
	TG_Shape::SetFog(fogcolour, fogStart, fogFull);
	active = true;
	terrainLightCalc = false;
	return NO_ERROR;
}

float currentScaleFactor = 0.0;
bool lastZoom = false;

//---------------------------------------------------------------------------
void
Camera::render(void)
{
	//------------------------------------------------------
	// At present, these actually draw.  Later they will
	// add elements to the draw list and sort and draw.
	// The later time has arrived.  We begin sorting immediately.
	// NO LONGER NEED TO SORT!
	// zbuffer time has arrived.  Share and Enjoy!
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
	TG_Shape::SetViewport(viewMulX, viewMulY, viewAddX, viewAddY);
	userInput->setViewport(viewMulX, viewMulY, viewAddX, viewAddY);
	//--------------------------------------------------------
	// Get new viewport values to scale stuff.  No longer uses
	// VFX stuff for this.  ALL GOS NOW!
	screenResolution.x = viewMulX;
	screenResolution.y = viewMulY;
	calculateProjectionConstants();
	globalScaleFactor = getScaleFactor();
	globalScaleFactor *= viewMulX / Environment.screenwidth; // Scale Mechs to ScreenRES
	//-----------------------------------------------
	// Set Ambient for this pass of rendering
	uint32_t lightRGB = (ambientRed << 16) + (ambientGreen << 8) + ambientBlue;
	eye->setLightcolour(1, lightRGB);
	eye->setLightIntensity(1, 1.0);
	if (active && turn > 1)
	{
		land->render(); // render the Terrain
		craterManager->render(); // render the craters and footprints
		// Only the GameCamera knows about this.  Heidi, override this function
		// in EditorCamera  and have your objectManager draw.
		//		ObjectManager->render(true, true, true);	//render all other
		// objects
	}
	//-----------------------------------------------------
}

//---------------------------------------------------------------------------
int32_t
Camera::activate(void)
{
	//------------------------------------------
	// If camera is already active, just return
	if (ready && active)
	{
		updateDaylight(true);
		return (NO_ERROR);
	}
	return NO_ERROR;
}

float zero = 0.0f;
float one = 1.0f;
float point1 = 0.000001f;

//---------------------------------------------------------------------------
// Returns a FAIRLY accurate interpretation.  NOT completely accurate unless
// a REAL W and Z are passed in.  Will see if there's was a to better
// interpolate those. More then accurate enough for the TacMap at present.
// Really improves frameRate ALOT! -fs
void
Camera::inverseProjectZ(Stuff::Vector4D& screen, Stuff::Vector3D& point)
{
	if (!usePerspective)
	{
		Stuff::Vector2DOf<int32_t> screenPos;
		screenPos.x = screen.x;
		screenPos.y = screen.y;
		inverseProject(screenPos, point);
		return;
	}
	point.x = point.y = point.z = 0.0f;
	if ((viewMulX < Stuff::SMALL) || (viewMulY < Stuff::SMALL))
		return;
	if ((screen.w == 0.0f) || (screen.z == 0.0f))
	{
		if (startZInverse > 1.0f)
			startZInverse = 1.0f;
		if (zPerPixel < 0.0f)
			zPerPixel = fabs(zPerPixel);
		screen.z = startZInverse - (zPerPixel * screen.y);
		screen.w = startWInverse + (wPerPixel * screen.y);
	}
	Stuff::Vector4D xformCoords;
	Stuff::Vector4D coords;
	coords.w = 1.0f / screen.w;
	coords.x = (screen.x - viewAddX) / (screen.w * viewMulX);
	coords.y = (screen.y - viewAddY) / (screen.w * viewMulY);
	coords.z = (screen.z / screen.w);
	xformCoords.Multiply(coords, clipToWorld);
	if (xformCoords.w < 0.0)
		xformCoords.Negate(xformCoords);
	point.x = -xformCoords.x;
	point.y = xformCoords.z;
	point.z = xformCoords.y;
}

//---------------------------------------------------------------------------
void
Camera::projectCamera(Stuff::Vector3D& point)
{
	//--------------------------------------------------------------------
	// This is already in -x,z,y reference frame.
	point *= worldToCameraMatrix;
}

//------------------------------------------------------------------------------
void
Camera::setOrthogonal(void)
{
	if (!usePerspective)
	{
		//--------------------
		// Parallel Projection
		float invCamScale = 1.0 / newScaleFactor;
		float left_clip, right_clip, top_clip, bottom_clip, far_clip, near_clip;
		left_clip = invCamScale * 300.0;
		top_clip = left_clip * ((float)Environment.screenheight / (float)Environment.screenwidth);
		right_clip = -invCamScale * 300.0;
		bottom_clip =
			right_clip * ((float)Environment.screenheight / (float)Environment.screenwidth);
		near_clip = -2000.0;
		far_clip = 8000.0;
		//
		//------------------------------------------------------------------------
		// Set up the camera to clip matrix.  This matrix takes camera space
		// coordinates and maps them into a homogeneous culling space where
		// valid X, Y, and Z axis values (when divided by W) will all be between
		// 0 and 1
		//------------------------------------------------------------------------
		//
		cameraToClip(LEFT_AXIS, LEFT_AXIS) = 1.0f / (left_clip - right_clip);
		cameraToClip(LEFT_AXIS, UP_AXIS) = 0.0f;
		cameraToClip(LEFT_AXIS, FORWARD_AXIS) = 0.0f;
		cameraToClip(LEFT_AXIS, 3) = 0.0f;
		cameraToClip(UP_AXIS, LEFT_AXIS) = 0.0f;
		cameraToClip(UP_AXIS, UP_AXIS) = 1.0f / (top_clip - bottom_clip);
		cameraToClip(UP_AXIS, FORWARD_AXIS) = 0.0f;
		cameraToClip(UP_AXIS, 3) = 0.0f;
		cameraToClip(FORWARD_AXIS, LEFT_AXIS) = 0.0;
		cameraToClip(FORWARD_AXIS, UP_AXIS) = 0.0;
		cameraToClip(FORWARD_AXIS, FORWARD_AXIS) = 1.0f / (far_clip - near_clip);
		cameraToClip(FORWARD_AXIS, 3) = 0.0;
		cameraToClip(3, LEFT_AXIS) = -right_clip / (left_clip - right_clip);
		cameraToClip(3, UP_AXIS) = -bottom_clip / (top_clip - bottom_clip);
		cameraToClip(3, FORWARD_AXIS) = -near_clip / (far_clip - near_clip);
		cameraToClip(3, 3) = 1.0f;
	}
	else
	{
		//-----------------------
		// Perspective Projection
		//
		float far_clip, near_clip;
		near_clip = Camera::NearPlaneDistance;
		far_clip = Camera::FarPlaneDistance;
		float horizontal_fov = camera_fov * DEGREES_TO_RADS;
		float height2width = ((float)Environment.screenheight / (float)Environment.screenwidth);
		//
		//-------------------------------------------------------
		// Calculate the horizontal, vertical, and forward ranges
		//-------------------------------------------------------
		//
		float left_clip = -(float)(near_clip * tan(horizontal_fov * 0.5f));
		float top_clip = -left_clip * height2width;
		float bottom_clip = -top_clip;
		float right_clip = -left_clip;
		float horizontal_range = APPLY_LEFT_SIGN(1.0f) / (left_clip - right_clip);
		float vertical_range = APPLY_UP_SIGN(1.0f) / (top_clip - bottom_clip);
		float depth_range = APPLY_FORWARD_SIGN(1.0f) / (far_clip - near_clip);
		//
		//------------------------------------------------------------------------
		// Set up the camera to clip matrix.  This matrix takes camera space
		// coordinates and maps them into a homogeneous culling space where
		// valid X, Y, and Z axis values (when divided by W) will all be between
		// 0 and 1
		//------------------------------------------------------------------------
		//
		cameraToClip(LEFT_AXIS, LEFT_AXIS) = near_clip * horizontal_range;
		cameraToClip(LEFT_AXIS, UP_AXIS) = 0.0f;
		cameraToClip(LEFT_AXIS, FORWARD_AXIS) = 0.0f;
		cameraToClip(LEFT_AXIS, 3) = 0.0f;
		cameraToClip(UP_AXIS, LEFT_AXIS) = 0.0f;
		cameraToClip(UP_AXIS, UP_AXIS) = near_clip * vertical_range;
		cameraToClip(UP_AXIS, FORWARD_AXIS) = 0.0f;
		cameraToClip(UP_AXIS, 3) = 0.0f;
		cameraToClip(FORWARD_AXIS, LEFT_AXIS) = -right_clip * horizontal_range;
		cameraToClip(FORWARD_AXIS, UP_AXIS) = -bottom_clip * vertical_range;
		cameraToClip(FORWARD_AXIS, FORWARD_AXIS) = far_clip * depth_range;
		cameraToClip(FORWARD_AXIS, 3) = 1.0f;
		cameraToClip(3, LEFT_AXIS) = 0.0f;
		cameraToClip(3, UP_AXIS) = 0.0f;
		cameraToClip(3, FORWARD_AXIS) = -far_clip * near_clip * depth_range;
		cameraToClip(3, 3) = 0.0f;
	}
}

extern bool useLOSAngle;
extern float worldUnitsPerMeter;
Stuff::Vector3D actualPosition;

#define MOVE_IN_INC 50.0f
#define ELEVATION_BUFFER 125.0f
#define ANGLE_UP_INC 0.25f

//-----------------------------------------------------------------------------------------------
bool
CameraLineOfSight(Stuff::Vector3D position, Stuff::Vector3D targetposition)
{
	int32_t posCellR, posCellC;
	int32_t tarCellR, tarCellC;
	land->worldToCell(position, posCellR, posCellC);
	land->worldToCell(targetposition, tarCellR, tarCellC);
	//-----------------------------------------------------
	// Once we allow teams to have alliances (for contacts,
	// etc.), simply set all nec. team bits in this mask...
	//------------------------------------------------------------------------------------------
	// Within magic radius.  Check REAL LOS now.
	// Check is really simple.
	// Find deltaCellRow and deltaCellCol and iterate over them from source to
	// dest. If the magic line ever goes BELOW the terrainElevation PLUS
	// localElevation return false.
	Stuff::Vector3D startPos, endPos;
	startPos.Zero();
	endPos.Zero();
	int32_t tCellRow = tarCellR, tCellCol = tarCellC;
	int32_t mCellRow = posCellR, mCellCol = posCellC;
	land->getCellPos(tCellRow, tCellCol, endPos);
	startPos = position;
	//	land->getCellPos(mCellRow,mCellCol,startPos);
	//	startPos.z = position.z;
	Stuff::Vector3D deltaCellVec;
	deltaCellVec.y = tCellRow - mCellRow;
	deltaCellVec.x = tCellCol - mCellCol;
	deltaCellVec.z = 0.0f;
	float startheight = startPos.z;
	float length = deltaCellVec.GetApproximateLength();
	if (length > Stuff::SMALL)
	{
		float colLength = deltaCellVec.x / length;
		float rowLength = deltaCellVec.y / length;
		float heightLen = (endPos.z - startPos.z) / length;
		float lastCol = fabs(colLength * 5.0);
		float lastRow = fabs(rowLength * 5.0);
		float startCellRow = mCellRow;
		float startCellCol = mCellCol;
		float endCellRow = tCellRow;
		float endCellCol = tCellCol;
		Stuff::Vector3D currentPos = startPos;
		Stuff::Vector3D dist;
		dist.Subtract(endPos, currentPos);
		bool colDone = false, rowDone = false;
		while (!colDone || !rowDone)
		{
			if (fabs(startCellRow - endCellRow) > lastRow) // DO NOT INCLUDE LAST CELL!!!!!
			{
				startCellRow += rowLength;
			}
			else
			{
				rowDone = true;
			}
			if (fabs(startCellCol - endCellCol) > lastCol) // DO NOT INCLUDE LAST CELL!!!!!
			{
				startCellCol += colLength;
			}
			else
			{
				colDone = true;
			}
			startheight += heightLen;
			int32_t startCellC = startCellCol;
			int32_t startCellR = startCellRow;
			land->getCellPos(startCellR, startCellC, currentPos);
			// float localElev = (worldUnitsPerMeter * 4.0f *
			// (float)GameMap->getLocalheight(startCellR,startCellC));
			// currentPos.z += localElev;
			if (startheight < currentPos.z)
			{
				return false;
			}
		}
	}
	return true;
}

//------------------------------------------------------------
void
Camera::setCameraOrigin(void)
{
	if (!usePerspective)
	{
		//--------------------
		// Parallel Projection
		cameraDirection.range = -3000.0f;
		// cameraDirection.pitch = cameraDirection.pitch;
		cameraOrigin.BuildRotation(
			Stuff::EulerAngles(cameraDirection.pitch, cameraDirection.yaw, 0.0f));
		Stuff::Point3D translation(cameraDirection);
		translation.x -= cameraShift.x;
		translation.z += cameraShift.y;
		translation.y += cameraShiftZ;
		cameraOrigin.BuildTranslation(translation);
		lookVector.Normalize(translation);
	}
	else
	{
		//--------------------------------
		// Perspective projection
		bool isOKView = false;
		float localAltitude = cameraAltitude;
		float localAngle = projectionAngle;
		while (!isOKView)
		{
			cameraDirection.range = localAltitude;
			cameraDirection.pitch = -localAngle * DEGREES_TO_RADS;
			cameraOrigin.BuildRotation(
				Stuff::EulerAngles(cameraDirection.pitch, cameraDirection.yaw, 0.0f));
			Stuff::Point3D translation(cameraDirection);
			lookVector = translation;
			lookVector.Normalize(lookVector);
			lookVector.Negate(lookVector);
			translation.x += -cameraShift.x;
			translation.z += cameraShift.y;
			translation.y += cameraShiftZ;
			actualPosition.x = -translation.x;
			actualPosition.y = translation.z;
			actualPosition.z = translation.y - ELEVATION_BUFFER;
			if (land && (turn > 3) && (useLOSAngle))
			{
				// bool isLOS = CameraLineOfSight(actualPosition,position);
				bool isLOS = actualPosition.z > (land->getTerrainElevation(actualPosition) + 75.0f);
				if (!isLOS)
				{
					// Try moving closer to get above terrain.
					// if (localAltitude > AltitudeMinimum)
					//{
					//	isOKView = false;
					//	localAltitude -= MOVE_IN_INC;
					//}
					// else	//We are as tight as we can go, pull localAngle up.
					{
						isOKView = false;
						localAngle += ANGLE_UP_INC;
					}
				}
				else
				{
					isOKView = true;
					cameraOrigin.BuildTranslation(translation);
					physicalPos = translation;
				}
			}
			else
			{
				isOKView = true;
				cameraOrigin.BuildTranslation(translation);
				physicalPos = translation;
			}
		}
	}
	//------------------------------------------------------------
	// DO Exactly what MLR does to create its Internal Matrices.
	Stuff::LinearMatrix4D cameraToWorldMatrix(cameraOrigin);
	worldToCameraMatrix.Invert(cameraToWorldMatrix);
	worldToClip.Multiply(worldToCameraMatrix, cameraToClip);
	if (usePerspective)
		clipToWorld.Invert(worldToClip);
}

//---------------------------------------------------------------------------
void
Camera::calculateProjectionConstants(void)
{
	cameraDirection.yaw = cameraRotation * DEGREES_TO_RADS;
	cameraDirection.pitch = projectionAngle * DEGREES_TO_RADS;
	screenCenter.x = screenResolution.x / 2.0;
	screenCenter.y = screenResolution.y / 2.0;
	screenCenter.z = screenResolution.z = 0.0f;
	if (usePerspective)
	{
		float oneOverResMag = screenResolution.GetLength();
		if (oneOverResMag > Stuff::SMALL)
			oneOverResMag = 1.0f / oneOverResMag;
		verticalSphereClipConstant =
			tan(screenResolution.y * oneOverResMag * camera_fov * 0.7071f * DEGREES_TO_RADS);
		horizontalSphereClipConstant =
			tan(screenResolution.x * oneOverResMag * camera_fov * 0.7071f * DEGREES_TO_RADS);
		cameraFrame.reset_to_world_frame();
		Stuff::Vector3D rotationVector(-projectionAngle, 0.0f, cameraRotation);
		cameraFrame.rotate(rotationVector);
	}
	setOrthogonal(); // Setup the camera Clip matrix.
	setCameraOrigin();
}

//---------------------------------------------------------------------------
void
Camera::calculateTopViewConstants(void)
{
	//-----------------------------------------------------------
	// Make camera look down on terrain.
	cameraDirection.yaw = cameraRotation * DEGREES_TO_RADS;
	cameraDirection.pitch = -90.0f * DEGREES_TO_RADS;
	screenCenter.x = screenResolution.x / 2.0;
	screenCenter.y = screenResolution.y / 2.0;
	setOrthogonal(); // Setup the camera Clip matrix.
	setCameraOrigin();
}

//---------------------------------------------------------------------------
void
Camera::deactivate(void)
{
	//--------------------------------------------------------
	// Anything a camera needs to do on shutdown goes in here.
	active = false;
}

//---------------------------------------------------------------------------
void
Camera::setPosition(Stuff::Vector3D newPosition, bool swoopy)
{
	position = newPosition;
	if (land)
	{
		float maxVisual = (Terrain::worldUnitsMapSide / 2) - (Terrain::worldUnitsPerVertex * 2.0f);
		// This keeps the camera from moving too close to the edge.
		//  -fs
		if (!drawTerrainGrid)
			maxVisual -= (Terrain::worldUnitsPerVertex * 6.0f);
		if (position.x > maxVisual)
			position.x = maxVisual;
		if (position.y > maxVisual)
			position.y = maxVisual;
		if (position.x < -maxVisual)
			position.x = -maxVisual;
		if (position.y < -maxVisual)
			position.y = -maxVisual;
		cameraShiftZ = land->getTerrainElevation(position);
		if (cameraShiftZ < Terrain::waterElevation)
			cameraShiftZ = Terrain::waterElevation;
		if (swoopy)
		{
			if (fabs(position.z - cameraShiftZ) > CAM_THRESHOLD)
			{
				goalPositionZ = cameraShiftZ;
			}
			if (fabs(goalPositionZ - position.z) > 10.0f)
			{
				float offsetZ = goalPositionZ - position.z;
				offsetZ *= 0.5f * frameLength;
				position.z += offsetZ;
			}
			cameraShiftZ = position.z;
		}
		else
		{
			goalPositionZ = position.z = cameraShiftZ;
		}
	}
#if 0 // Used to restrict camera to the diamond.  Not used Anymore!	
	//--------------------------------------------------------------
	// Use a different clip constant for the corner to clip them
	// off diagonally!  Or the designers must wet them!!!!!
	float maxVisual = Terrain::worldUnitsPerVertex * Terrain::blocksMapSide * Terrain::verticesBlockSide / 2;
	float zoomOutClip = 550.0;
	float zoomInClip = 375.0;
	float MAX_CORNER_DISTANCE = 1300.0;
	Stuff::Vector3D cornerPos1(maxVisual, 0, 0), cornerPos2(0, maxVisual, 0), cornerPos3(-maxVisual, 0, 0), cornerPos4(0, -maxVisual, 0);
	Stuff::Vector3D distance1, distance2, distance3, distance4;
	distance1.Subtract(newPosition, cornerPos1);
	distance2.Subtract(newPosition, cornerPos2);
	distance3.Subtract(newPosition, cornerPos3);
	distance4.Subtract(newPosition, cornerPos4);
	float dist1 = distance1.GetLength();
	float dist2 = distance2.GetLength();
	float dist3 = distance3.GetLength();
	float dist4 = distance4.GetLength();
	if(newScaleFactor <= 0.5)
	{
		float realMaxDistance = zoomOutClip + MAX_CORNER_DISTANCE;
		float clipConstant = zoomOutClip;
		if(dist1 < realMaxDistance)
		{
			clipConstant = dist1 - MAX_CORNER_DISTANCE;
		}
		else if(dist2 < realMaxDistance)
		{
			clipConstant = dist2 - MAX_CORNER_DISTANCE;
		}
		else if(dist3 < realMaxDistance)
		{
			clipConstant = dist3 - MAX_CORNER_DISTANCE;
		}
		else if(dist4 < realMaxDistance)
		{
			clipConstant = dist4 - MAX_CORNER_DISTANCE;
		}
		if(clipConstant > 0.0)
		{
			zoomOutClip = MAX_CORNER_DISTANCE - (clipConstant / zoomOutClip * (MAX_CORNER_DISTANCE - zoomOutClip));
		}
		else
		{
			zoomOutClip = MAX_CORNER_DISTANCE;
		}
	}
	else
	{
		float realMaxDistance = zoomInClip + MAX_CORNER_DISTANCE;
		float clipConstant = zoomInClip;
		if(dist1 < realMaxDistance)
		{
			clipConstant = dist1 - MAX_CORNER_DISTANCE;
		}
		else if(dist2 < realMaxDistance)
		{
			clipConstant = dist2 - MAX_CORNER_DISTANCE;
		}
		else if(dist3 < realMaxDistance)
		{
			clipConstant = dist3 - MAX_CORNER_DISTANCE;
		}
		else if(dist4 < realMaxDistance)
		{
			clipConstant = dist4 - MAX_CORNER_DISTANCE;
		}
		if(clipConstant > 0.0)
		{
			zoomInClip = MAX_CORNER_DISTANCE - (clipConstant / zoomInClip * (MAX_CORNER_DISTANCE - zoomInClip));
		}
		else
		{
			zoomInClip = MAX_CORNER_DISTANCE;
		}
	}
	//--------------------------------------------------------------
	// New camera clip code here.  Actually easy once thought out!
	if(newScaleFactor <= 0.5)
		maxVisual -= zoomOutClip;
	else
		maxVisual -= zoomInClip;
	float clipChk1 = newPosition.y - newPosition.x;
	float clipChk2 = newPosition.y + newPosition.x;
	bool clipA = (clipChk1 > maxVisual);
	bool clipB = (clipChk1 < -maxVisual);
	bool clipC = (clipChk2 > maxVisual);
	bool clipD = (clipChk2 < -maxVisual);
	if(!clipA && !clipB && !clipC && !clipD)
		return;		//No worries.  Camera on MAP.
	//------------------------------------------------------------
	// Simple checks first.  Off of the corner.  Force to corner.
	if(clipA && clipD)
	{
		position.x = -maxVisual;
		position.y = 0.0;
		return;
	}
	if(clipA && clipC)
	{
		position.x = 0.0;
		position.y = maxVisual;
		return;
	}
	if(clipB && clipD)
	{
		position.x = 0;
		position.y = -maxVisual;
		return;
	}
	if(clipB && clipC)
	{
		position.x = maxVisual;
		position.y = 0;
		return;
	}
	//---------------------------------------------------
	// Calc based on Single Clip value
	if(clipA)
	{
		float newX = (newPosition.y + newPosition.x - maxVisual) / 2.0;
		float newY = newX + maxVisual;
		position.x = newX;
		position.y = newY;
		return;
	}
	if(clipB)
	{
		float newX = (newPosition.y + newPosition.x + maxVisual) / 2.0;
		float newY = newX - maxVisual;
		position.x = newX;
		position.y = newY;
		return;
	}
	if(clipC)
	{
		float newX = (-newPosition.y + newPosition.x + maxVisual) / 2.0;
		float newY = -newX + maxVisual;
		position.x = newX;
		position.y = newY;
		return;
	}
	if(clipD)
	{
		float newX = (-newPosition.y + newPosition.x - maxVisual) / 2.0;
		float newY = -newX - maxVisual;
		position.x = newX;
		position.y = newY;
		return;
	}
#ifdef _DEBUG
	bool CameraClipImpossible = false;
#endif
	gosASSERT(CameraClipImpossible);
#endif
}

//---------------------------------------------------------------------------
void
Camera::setCameraView(int32_t viewNum)
{
	if ((viewNum >= 0) && (viewNum < MAX_VIEWS))
	{
		zoomValue(cameraZoom[viewNum]);
		tiltValue(cameraTilt[viewNum]);
	}
}

//---------------------------------------------------------------------------
void
Camera::zoomValue(float value)
{
	if (!usePerspective)
	{
		newScaleFactor = value;
		if (newScaleFactor > zoomMax)
			newScaleFactor = zoomMax;
	}
	else
	{
		cameraAltitude = value;
		cameraAltitudeDesired = cameraAltitude;
		if (cameraAltitude < AltitudeMinimum)
			cameraAltitude = AltitudeMinimum;
		float anglePercent =
			(projectionAngle - MIN_PERSPECTIVE) / (MAX_PERSPECTIVE - MIN_PERSPECTIVE);
		float testMax = Camera::AltitudeMaximumLo + ((Camera::AltitudeMaximumHi - Camera::AltitudeMaximumLo) * anglePercent);
		newScaleFactor = 1.0f - ((cameraAltitude - AltitudeMinimum) / testMax);
	}
	cameraShiftZ = position.z;
}

//---------------------------------------------------------------------------
void
Camera::ZoomIn(float amount)
{
	if (!usePerspective)
	{
		newScaleFactor += amount;
		if (newScaleFactor > zoomMax)
			newScaleFactor = zoomMax;
	}
	else
	{
		cameraAltitude -= (amount * 1500.0f);
		cameraAltitudeDesired = cameraAltitude;
		if (cameraAltitude < AltitudeMinimum)
			cameraAltitude = AltitudeMinimum;
		float anglePercent =
			(projectionAngle - MIN_PERSPECTIVE) / (MAX_PERSPECTIVE - MIN_PERSPECTIVE);
		float testMax = Camera::AltitudeMaximumLo + ((Camera::AltitudeMaximumHi - Camera::AltitudeMaximumLo) * anglePercent);
		newScaleFactor = 1.0f - ((cameraAltitude - AltitudeMinimum) / testMax);
	}
	cameraShiftZ = position.z;
}

//---------------------------------------------------------------------------
void
Camera::ZoomOut(float amount)
{
	if (!usePerspective)
	{
		newScaleFactor -= amount;
		if (newScaleFactor < zoomMin)
			newScaleFactor = zoomMin;
	}
	else
	{
		float anglePercent =
			(projectionAngle - MIN_PERSPECTIVE) / (MAX_PERSPECTIVE - MIN_PERSPECTIVE);
		float testMax = Camera::AltitudeMaximumLo + ((Camera::AltitudeMaximumHi - Camera::AltitudeMaximumLo) * anglePercent);
		cameraAltitude += (amount * 1500.0f);
		cameraAltitudeDesired = cameraAltitude;
		if (cameraAltitude > testMax)
			cameraAltitude = testMax;
		newScaleFactor = 1.0f - ((cameraAltitude - AltitudeMinimum) / testMax);
	}
	cameraShiftZ = position.z;
}

//---------------------------------------------------------------------------
void
Camera::ZoomNormal(void)
{
	if (!usePerspective)
	{
		newScaleFactor = 0.5f;
	}
	else
	{
		camera_fov = 40.0f;
		cosHalfFOV = cos(camera_fov * DEGREES_TO_RADS);
		float anglePercent =
			(projectionAngle - MIN_PERSPECTIVE) / (MAX_PERSPECTIVE - MIN_PERSPECTIVE);
		float testMax = Camera::AltitudeMaximumLo + ((Camera::AltitudeMaximumHi - Camera::AltitudeMaximumLo) * anglePercent);
		cameraAltitude = AltitudeDefault;
		cameraAltitudeDesired = cameraAltitude;
		newScaleFactor = 1.0f - ((cameraAltitude - AltitudeMinimum) / testMax);
	}
	cameraShiftZ = position.z;
}

//---------------------------------------------------------------------------
void
Camera::ZoomMin(void)
{
	if (!usePerspective)
	{
		newScaleFactor = 0.5f;
	}
	else
	{
		camera_fov = 40.0f;
		cosHalfFOV = cos(camera_fov * DEGREES_TO_RADS);
		float anglePercent =
			(projectionAngle - MIN_PERSPECTIVE) / (MAX_PERSPECTIVE - MIN_PERSPECTIVE);
		float testMax = Camera::AltitudeMaximumLo + ((Camera::AltitudeMaximumHi - Camera::AltitudeMaximumLo) * anglePercent);
		cameraAltitude = testMax;
		cameraAltitudeDesired = cameraAltitude;
		newScaleFactor = 1.0f - ((cameraAltitude - AltitudeMinimum) / testMax);
	}
	cameraShiftZ = position.z;
}

//---------------------------------------------------------------------------
void
Camera::ZoomDefault(void)
{
	if (!usePerspective)
	{
		newScaleFactor = 0.5f;
	}
	else
	{
		camera_fov = 40.0f;
		cosHalfFOV = cos(camera_fov * DEGREES_TO_RADS);
		cameraAltitude = AltitudeDefault;
		cameraAltitudeDesired = cameraAltitude;
		float anglePercent =
			(projectionAngle - MIN_PERSPECTIVE) / (MAX_PERSPECTIVE - MIN_PERSPECTIVE);
		float testMax = Camera::AltitudeMaximumLo + ((Camera::AltitudeMaximumHi - Camera::AltitudeMaximumLo) * anglePercent);
		newScaleFactor = 1.0f - ((cameraAltitude - AltitudeMinimum) / testMax);
	}
	cameraShiftZ = position.z;
}

//---------------------------------------------------------------------------
void
Camera::ZoomTight(void)
{
	if (!usePerspective)
	{
		newScaleFactor = 0.5f;
	}
	else
	{
		camera_fov = 40.0f;
		cosHalfFOV = cos(camera_fov * DEGREES_TO_RADS);
		cameraAltitude = AltitudeTight;
		cameraAltitudeDesired = cameraAltitude;
		float anglePercent =
			(projectionAngle - MIN_PERSPECTIVE) / (MAX_PERSPECTIVE - MIN_PERSPECTIVE);
		float testMax = Camera::AltitudeMaximumLo + ((Camera::AltitudeMaximumHi - Camera::AltitudeMaximumLo) * anglePercent);
		newScaleFactor = 1.0f - ((cameraAltitude - AltitudeMinimum) / testMax);
	}
	cameraShiftZ = position.z;
}

//---------------------------------------------------------------------------
void
Camera::ZoomMax(void)
{
	if (!usePerspective)
	{
		newScaleFactor = 0.5f;
	}
	else
	{
		camera_fov = 40.0f;
		cosHalfFOV = cos(camera_fov * DEGREES_TO_RADS);
		cameraAltitude = AltitudeMinimum;
		cameraAltitudeDesired = cameraAltitude;
		float anglePercent =
			(projectionAngle - MIN_PERSPECTIVE) / (MAX_PERSPECTIVE - MIN_PERSPECTIVE);
		float testMax = Camera::AltitudeMaximumLo + ((Camera::AltitudeMaximumHi - Camera::AltitudeMaximumLo) * anglePercent);
		newScaleFactor = 1.0f - ((cameraAltitude - AltitudeMinimum) / testMax);
	}
	cameraShiftZ = position.z;
}

//---------------------------------------------------------------------------
void
Camera::tiltValue(float value)
{
	projectionAngle = value;
	if (usePerspective)
	{
		if (projectionAngle < MIN_PERSPECTIVE)
			projectionAngle = MIN_PERSPECTIVE;
	}
	else
	{
		if (projectionAngle < MIN_ORTHO)
			projectionAngle = MIN_ORTHO;
	}
	calculateProjectionConstants();
}

//---------------------------------------------------------------------------
void
Camera::tiltUp(float amount)
{
	projectionAngle -= amount;
	if (usePerspective)
	{
		if (projectionAngle < MIN_PERSPECTIVE)
			projectionAngle = MIN_PERSPECTIVE;
	}
	else
	{
		if (projectionAngle < MIN_ORTHO)
			projectionAngle = MIN_ORTHO;
	}
	calculateProjectionConstants();
}

//---------------------------------------------------------------------------
void
Camera::tiltDown(float amount)
{
	projectionAngle += amount;
	if (projectionAngle > MAX_PERSPECTIVE)
		projectionAngle = MAX_PERSPECTIVE;
	calculateProjectionConstants();
}

//---------------------------------------------------------------------------
void
Camera::tiltNormal(void)
{
	projectionAngle = NORM_PERSPECTIVE;
	calculateProjectionConstants();
}

//---------------------------------------------------------------------------
void
Camera::rotateNormal(void)
{
	float newCamAngle = -170.0f;
	float newCamWorld = -170.0f;
	setCameraRotation(newCamAngle, newCamWorld);
	cameraShiftZ = position.z;
}

//---------------------------------------------------------------------------
void
Camera::movePosLeft(float amount, Stuff::Vector3D& pos)
{
	Stuff::Vector3D direction;
	direction.x = -0.7071f;
	direction.y = -0.7071f;
	direction.z = 0.0;
	OppRotate(direction, cameraRotation);
	direction *= amount;
	pos.x += direction.x;
	pos.y += direction.y;
}

//---------------------------------------------------------------------------
void
Camera::movePosRight(float amount, Stuff::Vector3D& pos)
{
	Stuff::Vector3D direction;
	direction.x = 0.7071f;
	direction.y = 0.7071f;
	direction.z = 0.0;
	OppRotate(direction, cameraRotation);
	direction *= amount;
	pos.x += direction.x;
	pos.y += direction.y;
}

//---------------------------------------------------------------------------
void
Camera::movePosUp(float amount, Stuff::Vector3D& pos)
{
	Stuff::Vector3D direction;
	direction.x = -0.7071f;
	direction.y = 0.7071f;
	direction.z = 0.0;
	OppRotate(direction, cameraRotation);
	direction *= amount;
	pos.x += direction.x;
	pos.y += direction.y;
}

//---------------------------------------------------------------------------
void
Camera::movePosDown(float amount, Stuff::Vector3D& pos)
{
	Stuff::Vector3D direction;
	direction.x = 0.7071f;
	direction.y = -0.7071f;
	direction.z = 0.0;
	OppRotate(direction, cameraRotation);
	direction *= amount;
	pos.x += direction.x;
	pos.y += direction.y;
}

//---------------------------------------------------------------------------
void
Camera::moveLeft(float amount)
{
	Stuff::Vector3D direction;
	if (!usePerspective)
	{
		direction.x = -1.0;
		direction.y = 0.0;
		direction.z = 0.0;
	}
	else
	{
		direction.x = 1.0;
		direction.y = 0.0;
		direction.z = 0.0;
	}
	OppRotate(direction, worldCameraRotation);
	direction *= amount;
	Stuff::Vector3D newPosition = position;
	newPosition.x += direction.x;
	newPosition.y += direction.y;
	setPosition(newPosition);
}

//---------------------------------------------------------------------------
void
Camera::moveRight(float amount)
{
	Stuff::Vector3D direction;
	if (!usePerspective)
	{
		direction.x = 1.0;
		direction.y = 0.0;
		direction.z = 0.0;
	}
	else
	{
		direction.x = -1.0;
		direction.y = 0.0;
		direction.z = 0.0;
	}
	OppRotate(direction, worldCameraRotation);
	direction *= amount;
	Stuff::Vector3D newPosition = position;
	newPosition.x += direction.x;
	newPosition.y += direction.y;
	setPosition(newPosition);
}

//---------------------------------------------------------------------------
void
Camera::moveUp(float amount)
{
	Stuff::Vector3D direction;
	if (!usePerspective)
	{
		direction.x = 0.0;
		direction.y = 1.0;
		direction.z = 0.0;
	}
	else
	{
		direction.x = 0.0;
		direction.y = -1.0;
		direction.z = 0.0;
	}
	OppRotate(direction, worldCameraRotation);
	direction *= amount;
	Stuff::Vector3D newPosition = position;
	newPosition.x += direction.x;
	newPosition.y += direction.y;
	setPosition(newPosition);
}

//---------------------------------------------------------------------------
void
Camera::moveDown(float amount)
{
	Stuff::Vector3D direction;
	if (!usePerspective)
	{
		direction.x = 0.0;
		direction.y = -1.0;
		direction.z = 0.0;
	}
	else
	{
		direction.x = 0.0;
		direction.y = 1.0;
		direction.z = 0.0;
	}
	OppRotate(direction, worldCameraRotation);
	direction *= amount;
	Stuff::Vector3D newPosition = position;
	newPosition.x += direction.x;
	newPosition.y += direction.y;
	setPosition(newPosition);
}

//---------------------------------------------------------------------------
void
Camera::rotateLightLeft(float amount)
{
	lightYaw += amount;
	if (lightYaw > 360.0)
		lightYaw -= 360;
	// Replace with TGL Lights
	Stuff::LinearMatrix4D lightToWorldMatrix;
	lightToWorldMatrix.BuildTranslation(Stuff::Point3D(0.0, 0.0, 0.0));
	lightToWorldMatrix.BuildRotation(Stuff::EulerAngles(
		lightPitch * DEGREES_TO_RADS, (lightYaw + 135.0) * DEGREES_TO_RADS, 0.0f));
	worldLights[0]->SetLightToWorld(&lightToWorldMatrix);
}

//---------------------------------------------------------------------------
void
Camera::rotateLightRight(float amount)
{
	lightYaw -= amount;
	if (lightYaw < -360.0)
		lightYaw += 360;
	// Replace with TGL Lights
	Stuff::LinearMatrix4D lightToWorldMatrix;
	lightToWorldMatrix.BuildTranslation(Stuff::Point3D(0.0, 0.0, 0.0));
	lightToWorldMatrix.BuildRotation(Stuff::EulerAngles(
		lightPitch * DEGREES_TO_RADS, (lightYaw + 135.0) * DEGREES_TO_RADS, 0.0f));
	worldLights[0]->SetLightToWorld(&lightToWorldMatrix);
}

//---------------------------------------------------------------------------
void
Camera::rotateLightUp(float amount)
{
	lightPitch += amount;
	if (lightPitch > 90.0)
		lightPitch = 90.0;
	// Replace with TGL Lights
	Stuff::LinearMatrix4D lightToWorldMatrix;
	lightToWorldMatrix.BuildTranslation(Stuff::Point3D(0.0, 0.0, 0.0));
	lightToWorldMatrix.BuildRotation(Stuff::EulerAngles(
		lightPitch * DEGREES_TO_RADS, (lightYaw + 135.0) * DEGREES_TO_RADS, 0.0f));
	worldLights[0]->SetLightToWorld(&lightToWorldMatrix);
}

//---------------------------------------------------------------------------
void
Camera::rotateLightDown(float amount)
{
	lightPitch -= amount;
	if (lightPitch < 5.0f)
		lightPitch = 5.0f;
	// Replace with TGL Lights
	Stuff::LinearMatrix4D lightToWorldMatrix;
	lightToWorldMatrix.BuildTranslation(Stuff::Point3D(0.0, 0.0, 0.0));
	lightToWorldMatrix.BuildRotation(Stuff::EulerAngles(
		lightPitch * DEGREES_TO_RADS, (lightYaw + 135.0) * DEGREES_TO_RADS, 0.0f));
	worldLights[0]->SetLightToWorld(&lightToWorldMatrix);
}

//---------------------------------------------------------------------------
void
Camera::rotateLeft(float amount)
{
	float newCamAngle = cameraRotation + amount;
	float newCamWorld = worldCameraRotation + amount;
	setCameraRotation(newCamAngle, newCamWorld);
	cameraShiftZ = position.z;
}

//---------------------------------------------------------------------------
void
Camera::rotateRight(float amount)
{
	float newCamAngle = cameraRotation - amount;
	float newCamWorld = worldCameraRotation - amount;
	setCameraRotation(newCamAngle, newCamWorld);
	cameraShiftZ = position.z;
}

//---------------------------------------------------------------------------
void
Camera::setCameraRotation(float angle, float worldAngle)
{
	worldCameraRotation = worldAngle;
	if (worldCameraRotation > 180)
		worldCameraRotation -= 360;
	if (worldCameraRotation < -180)
		worldCameraRotation += 360;
	cameraRotation = angle;
	if (cameraRotation > 180)
		cameraRotation -= 360;
	if (cameraRotation < -180)
		cameraRotation += 360;
	cameraDirection.yaw = cameraRotation * DEGREES_TO_RADS;
	cameraDirection.pitch = -projectionAngle * DEGREES_TO_RADS;
	cameraDirection.range = 0.0;
}

//---------------------------------------------------------------------------
float
Camera::getCameraRotation(void)
{
	return worldCameraRotation;
}

#define OBSCURED_FACTOR 0.5f;
//---------------------------------------------------------------------------
uint8_t
Camera::getLightRed(float intensity)
{
	// ASM and Inline
	if (intensity > 0.0)
	{
		int32_t fResult = float2long(lightRed * intensity + ambientRed);
		if (fResult < 0xff)
			return fResult;
		else
			return 0xff;
	}
	return ambientRed;
}

//---------------------------------------------------------------------------
uint8_t
Camera::getLightGreen(float intensity)
{
	// ASM and Inline
	if (intensity > 0.0)
	{
		int32_t fResult = float2long(lightGreen * intensity + ambientGreen);
		if (fResult < 0xff)
			return fResult;
		else
			return 0xff;
	}
	return ambientGreen;
}

//---------------------------------------------------------------------------
uint8_t
Camera::getLightBlue(float intensity)
{
	// ASM and Inline
	if (intensity > 0.0)
	{
		int32_t fResult = float2long(lightBlue * intensity + ambientBlue);
		if (fResult < 0xff)
			return fResult;
		else
			return 0xff;
	}
	return ambientBlue;
}

bool
Camera::save(FitIniFile* file)
{
	file->writeBlock("Cameras");
	file->writeIdFloat("ProjectionAngle", projectionAngle);
	file->writeIdFloat("PositionX", position.x);
	file->writeIdFloat("PositionY", position.y);
	file->writeIdFloat("PositionZ", 0.0);
	file->writeIdBoolean("Ready", true);
	file->writeIdUChar("LightRed", lightRed);
	file->writeIdUChar("LightGreen", lightGreen);
	file->writeIdUChar("LightBlue", lightBlue);
	file->writeIdUChar("AmbientBlue", ambientBlue);
	file->writeIdUChar("AmbientGreen", ambientGreen);
	file->writeIdUChar("AmbientRed", ambientRed);
	if (terrainShadowcolourEnabled)
	{
		file->writeIdUChar("TerrainShadowcolourEnabled", 1);
	}
	else
	{
		file->writeIdUChar("TerrainShadowcolourEnabled", 0);
	}
	file->writeIdUChar("TerrainShadowBlue", terrainShadowBlue);
	file->writeIdUChar("TerrainShadowGreen", terrainShadowGreen);
	file->writeIdUChar("TerrainShadowRed", terrainShadowRed);
	file->writeIdUChar("DayLightRed", dayLightRed);
	file->writeIdUChar("DayLightGreen", dayLightGreen);
	file->writeIdUChar("DayLightBlue", dayLightBlue);
	file->writeIdUChar("DayAmbientBlue", dayAmbientBlue);
	file->writeIdUChar("DayAmbientGreen", dayAmbientGreen);
	file->writeIdUChar("DayAmbientRed", dayAmbientRed);
	file->writeIdUChar("SunsetLightRed", sunsetLightRed);
	file->writeIdUChar("SunsetLightGreen", sunsetLightGreen);
	file->writeIdUChar("SunsetLightBlue", sunsetLightBlue);
	file->writeIdUChar("NightLightRed", nightLightRed);
	file->writeIdUChar("NightLightGreen", nightLightGreen);
	file->writeIdUChar("NightLightBlue", nightLightBlue);
	file->writeIdUChar("NightAmbientBlue", nightAmbientBlue);
	file->writeIdUChar("NightAmbientGreen", nightAmbientGreen);
	file->writeIdUChar("NightAmbientRed", nightAmbientRed);
	file->writeIdUChar("SeenBlue", seenBlue);
	file->writeIdUChar("SeenGreen", seenGreen);
	file->writeIdUChar("SeenRed", seenRed);
	file->writeIdUChar("BaseBlue", baseBlue);
	file->writeIdUChar("BaseGreen", baseGreen);
	file->writeIdUChar("BaseRed", baseRed);
	file->writeIdFloat("LightDirPitch", lightPitch);
	file->writeIdFloat("DayLightPitch", dayLightPitch);
	file->writeIdFloat("DayToNightTime", day2NightTransitionTime);
	file->writeIdFloat("LightDirYaw", lightYaw);
	file->writeIdFloat("NewScale", newScaleFactor);
	file->writeIdFloat("StartRotation", getCameraRotation());
	file->writeIdFloatArray("LODScales", zoomLevelLODScale, 3);
	file->writeIdFloat("ElevationAdjustFactor", elevationAdjustFactor);
	file->writeIdFloat("ZoomMax", zoomMax);
	file->writeIdFloat("ZoomMin", zoomMin);
	file->writeIdFloat("FOVMax", FOVMax);
	file->writeIdFloat("FOVMin", FOVMin);
	file->writeIdFloat("FogStart", fogStart);
	file->writeIdFloat("FogFull", fogFull);
	file->writeIdULong("Fogcolour", dayFogcolour);
	file->writeIdFloat("FogTransparency", fogTransparency);
	int32_t userMin, userMax;
	int32_t baseTerrain;
	land->getUserSettings(userMin, userMax, baseTerrain);
	file->writeIdLong("UserMin", userMin);
	file->writeIdLong("UserMax", userMax);
	file->writeIdLong("BaseTerrain", baseTerrain);
	return true;
}

float
Camera::getFarClipDistance()
{
	return MaxClipDistance;
}

void
Camera::setFarClipDistance(float farClipDistance)
{
	MaxClipDistance = farClipDistance;
	DistanceFactor = 1.0f / (MaxClipDistance - MinHazeDistance);
}

float
Camera::getNearClipDistance()
{
	return NearPlaneDistance;
}

void
Camera::setNearClipDistance(float nearClipDistance)
{
	NearPlaneDistance = nearClipDistance;
}

float
Camera::getMaximumCameraAltitude()
{
	float anglePercent = (projectionAngle - MIN_PERSPECTIVE) / (MAX_PERSPECTIVE - MIN_PERSPECTIVE);
	float testMax = Camera::AltitudeMaximumLo + ((Camera::AltitudeMaximumHi - Camera::AltitudeMaximumLo) * anglePercent);
	return testMax;
}
void
Camera::setMaximumCameraAltitude(float maxAltitude)
{
	AltitudeMaximumLo = maxAltitude;
}

//---------------------------------------------------------------------------
