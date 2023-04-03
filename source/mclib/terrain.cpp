//---------------------------------------------------------------------------
//
// Terrain.cpp -- File contains calss definitions for the Terrain
//
//	MechCommander 2
//
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

//---------------------------------------------------------------------------
// Include Files
#ifndef TERRAIN_H
#include "terrain.h"
#endif

#ifndef VERTEX_H
#include "vertex.h"
#endif

#ifndef CAMERA_H
#include "camera.h"
#endif

#ifndef TERRTXM_H
#include "terrtxm.h"
#endif

#ifndef DBASEGUI_H
#include "dbasegui.h"
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

#ifndef PACKET_H
#include "packet.h"
#endif

#ifndef INIFILE_H
#include "FitIniFile.h"
#endif

#ifndef TGAINFO_H
#include "tgainfo.h"
#endif

//---------------------------------------------------------------------------
// Static Globals
float worldUnitsPerMeter = 5.01f;
float metersPerWorldUnit = 0.2f;
int32_t terrainLineChanged = 0;

MapDataPtr Terrain::mapData = nullptr;
TerrainTexturesPtr Terrain::terrainTextures = nullptr;
TerraincolourMapPtr Terrain::terrainTextures2 = nullptr;

const int32_t Terrain::verticesBlockSide = 20; // Changes for new terrain?
int32_t Terrain::blocksMapSide = 0; // Calced during load.

int32_t Terrain::visibleVerticesPerSide = 0; // Passed in.

const float Terrain::worldUnitsPerVertex = 128.0;
const float Terrain::worldUnitsPerCell = Terrain::worldUnitsPerVertex / terrain_const::MAPCELL_DIM;
const float Terrain::halfWorldUnitsPerCell = Terrain::worldUnitsPerCell / 2.0f;
const float Terrain::metersPerCell = Terrain::worldUnitsPerCell * metersPerWorldUnit;
const float Terrain::worldUnitsBlockSide =
	Terrain::worldUnitsPerVertex * Terrain::verticesBlockSide;
const float Terrain::oneOverWorldUnitsPerVertex = 1.0f / Terrain::worldUnitsPerVertex;
const float Terrain::oneOverWorldUnitsPerCell = 1.0f / Terrain::worldUnitsPerCell;
const float Terrain::oneOverMetersPerCell = 1.0f / Terrain::metersPerCell;
const float Terrain::oneOverVerticesBlockSide = 1.0f / Terrain::verticesBlockSide;

float Terrain::worldUnitsMapSide = 0.0; // Calced during load.
float Terrain::oneOverWorldUnitsMapSide = 0.0f;
int32_t Terrain::halfVerticesMapSide = 0;
int32_t Terrain::realVerticesMapSide = 0;

Stuff::Vector3D Terrain::mapTopLeft3d; // Calced during load.

UserHeapPtr Terrain::terrainHeap = nullptr; // Setup at load time.
std::wstring_view Terrain::terrainName = nullptr;
std::wstring_view Terrain::colorMapName = nullptr;

int32_t Terrain::numObjBlocks = 0;
ObjBlockInfo* Terrain::objBlockInfo = nullptr;
bool* Terrain::objVertexActive = nullptr;

float* Terrain::tileRowToWorldCoord = nullptr;
float* Terrain::tileColToWorldCoord = nullptr;
float* Terrain::cellToWorldCoord = nullptr;
float* Terrain::cellColToWorldCoord = nullptr;
float* Terrain::cellRowToWorldCoord = nullptr;

float Terrain::waterElevation = 0.0f;
float Terrain::frameAngle = 0.0f;
float Terrain::frameCos = 1.0f;
float Terrain::frameCosAlpha = 1.0f;
uint32_t Terrain::alphaMiddle = 0xaf000000;
uint32_t Terrain::alphaEdge = 0x3f000000;
uint32_t Terrain::alphaDeep = 0xff000000;
float Terrain::waterFreq = 4.0f;
float Terrain::waterAmplitude = 10.0f;

int32_t Terrain::userMin = 0;
int32_t Terrain::userMax = 0;
uint32_t Terrain::baseTerrain = 0;
uint8_t Terrain::fractalThreshold = 1;
uint8_t Terrain::fractalNoise = 0;
bool Terrain::recalcShadows = false;
bool Terrain::recalcLight = false;

Clouds* Terrain::cloudLayer = nullptr;

bool drawTerrainGrid = false; // Override locally in editor so game don't come
	// with these please!  Love -fs
bool drawLOSGrid = false;
bool drawTerrainTiles = true;
bool drawTerrainOverlays = true;
bool drawTerrainMines = true;
bool renderObjects = true;
bool renderTrees = true;

TerrainPtr land = nullptr;

int32_t* usedBlockList; // Used to determine what objects to deal with.
int32_t* moverBlockList;

uint32_t blockMemSize = 0; // Misc flags.
bool useOldProject = FALSE;
bool projectAll = FALSE;
bool useClouds = false;
bool useFog = true;
bool useVertexLighting = true;
bool useFaceLighting = false;
extern bool useRealLOS;

uint8_t godMode = 0; // Can I simply see everything, enemy and friendly?

extern int32_t DrawDebugCells;

#define MAX_TERRAIN_HEAP_SIZE 1024000

int32_t visualRangeTable[256];
extern bool justResaveAllMaps;
//---------------------------------------------------------------------------
// These are used to determine what terrain objects to process.
// They date back to GenCon 1996!!
void addBlockToList(int32_t blockNum)
{
	int32_t totalBlocks = Terrain::blocksMapSide * Terrain::blocksMapSide;
	for (size_t i = 0; i < totalBlocks; i++)
	{
		if (usedBlockList[i] == blockNum)
		{
			return;
		}
		else if (usedBlockList[i] == -1)
		{
			usedBlockList[i] = blockNum;
			return;
		}
	}
}

//---------------------------------------------------------------------------
void addMoverToList(int32_t blockNum)
{
	int32_t totalBlocks = Terrain::blocksMapSide * Terrain::blocksMapSide;
	for (size_t i = 0; i < totalBlocks; i++)
	{
		if (moverBlockList[i] == blockNum)
		{
			return;
		}
		else if (moverBlockList[i] == -1)
		{
			moverBlockList[i] = blockNum;
			return;
		}
	}
}

//---------------------------------------------------------------------------
void clearList(void)
{
	int32_t totalBlocks = Terrain::blocksMapSide * Terrain::blocksMapSide;
	blockMemSize = totalBlocks * sizeof(int32_t);
	if (usedBlockList)
		memset(usedBlockList, -1, blockMemSize);
}

//---------------------------------------------------------------------------
void clearMoverList(void)
{
	int32_t totalBlocks = Terrain::blocksMapSide * Terrain::blocksMapSide;
	blockMemSize = totalBlocks * sizeof(int32_t);
	if (moverBlockList)
		memset(moverBlockList, -1, blockMemSize);
}

//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// class Terrain
void Terrain::init(void)
{
	vertexList = nullptr;
	numberVertices = 0;
	quadList = nullptr;
	numberQuads = 0;
}

//---------------------------------------------------------------------------
void Terrain::initMapCellArrays(void)
{
	if (!tileRowToWorldCoord)
	{
		tileRowToWorldCoord = (float*)terrainHeap->Malloc(sizeof(float) * realVerticesMapSide);
		gosASSERT(tileRowToWorldCoord != nullptr);
	}
	if (!tileColToWorldCoord)
	{
		tileColToWorldCoord = (float*)terrainHeap->Malloc(sizeof(float) * realVerticesMapSide);
		gosASSERT(tileColToWorldCoord != nullptr);
	}
	if (!cellToWorldCoord)
	{
		cellToWorldCoord = (float*)terrainHeap->Malloc(sizeof(float) * terrain_const::MAPCELL_DIM);
		gosASSERT(cellToWorldCoord != nullptr);
	}
	if (!cellColToWorldCoord)
	{
		cellColToWorldCoord = (float*)terrainHeap->Malloc(
			sizeof(float) * realVerticesMapSide * terrain_const::MAPCELL_DIM);
		gosASSERT(cellColToWorldCoord != nullptr);
	}
	if (!cellRowToWorldCoord)
	{
		cellRowToWorldCoord = (float*)terrainHeap->Malloc(
			sizeof(float) * realVerticesMapSide * terrain_const::MAPCELL_DIM);
		gosASSERT(cellRowToWorldCoord != nullptr);
	}
	int32_t i = 0;
	int32_t height = realVerticesMapSide, width = height;
	for (i = 0; i < height; i++)
		tileRowToWorldCoord[i] = (worldUnitsMapSide / 2.0) - (i * worldUnitsPerVertex);
	for (i = 0; i < width; i++)
		tileColToWorldCoord[i] = (i * worldUnitsPerVertex) - (worldUnitsMapSide / 2.0);
	for (i = 0; i < terrain_const::MAPCELL_DIM; i++)
		cellToWorldCoord[i] = (worldUnitsPerVertex / (float)terrain_const::MAPCELL_DIM) * i;
	int32_t maxCell = height * terrain_const::MAPCELL_DIM;
	for (i = 0; i < maxCell; i++)
		cellRowToWorldCoord[i] = (worldUnitsMapSide / 2.0) - (i * worldUnitsPerCell);
	maxCell = width * terrain_const::MAPCELL_DIM;
	for (i = 0; i < maxCell; i++)
		cellColToWorldCoord[i] = (i * worldUnitsPerCell) - (worldUnitsMapSide / 2.0);
}

//---------------------------------------------------------------------------
int32_t
Terrain::init(PacketFile* pakFile, int32_t whichPacket, uint32_t visibleVertices,
	volatile float& percent, float percentRange)
{
	clearList();
	clearMoverList();
	int32_t result = pakFile->seekPacket(whichPacket);
	if (result != NO_ERROR)
		STOP(("Unable to seek Packet %d in file %s", whichPacket, pakFile->getFilename()));
	int32_t tmp = pakFile->getPacketSize();
	realVerticesMapSide = sqrt(float(tmp / sizeof(PostcompVertex)));
	if (!justResaveAllMaps && (realVerticesMapSide != 120) && (realVerticesMapSide != 100) && (realVerticesMapSide != 80) && (realVerticesMapSide != 60))
	{
		PAUSE(("This map size NO longer supported %d.  Must be 120, 100, 80 or "
			   "60 now!  Can Continue, for NOW!!",
			realVerticesMapSide));
		//		return -1;
	}
	init(realVerticesMapSide, pakFile, visibleVertices, percent, percentRange);
	return (NO_ERROR);
}

//---------------------------------------------------------------------------
void Terrain::getcolourMapName(FitIniFile* file)
{
	if (file)
	{
		if (file->seekBlock("colourMap") == NO_ERROR)
		{
			wchar_t mapName[1024];
			if (file->readIdString("colourMapName", mapName, 1023) == NO_ERROR)
			{
				colorMapName = new wchar_t[strlen(mapName) + 1];
				strcpy(colorMapName, mapName);
				return;
			}
		}
	}
	colorMapName = nullptr;
}

//---------------------------------------------------------------------------
void Terrain::setcolourMapName(std::wstring_view mapName)
{
	if (colorMapName)
	{
		delete[] colorMapName;
		colorMapName = nullptr;
	}
	if (mapName)
	{
		colorMapName = new wchar_t[strlen(mapName) + 1];
		strcpy(colorMapName, mapName);
	}
}

//---------------------------------------------------------------------------
void Terrain::savecolourMapName(FitIniFile* file)
{
	if (file && colorMapName)
	{
		file->writeBlock("colourMap");
		file->writeIdString("colourMapName", colorMapName);
	}
}

//---------------------------------------------------------------------------
int32_t
Terrain::init(uint32_t verticesPerMapSide, PacketFile* pakFile, uint32_t visibleVertices,
	volatile float& percent, float percentRange)
{
	// Did we pass in the hi-res colormap?
	// If so, convert back to old verticesPerMapSide!
	if (verticesPerMapSide > 300)
		verticesPerMapSide /= 12.8;
	realVerticesMapSide = verticesPerMapSide;
	halfVerticesMapSide = realVerticesMapSide >> 1;
	blocksMapSide = realVerticesMapSide / verticesBlockSide;
	worldUnitsMapSide = realVerticesMapSide * worldUnitsPerVertex;
	if (worldUnitsMapSide > Stuff::SMALL)
		oneOverWorldUnitsMapSide = 1.0f / worldUnitsMapSide;
	else
		oneOverWorldUnitsMapSide = 0.0f;
	Terrain::numObjBlocks = blocksMapSide * blocksMapSide;
	visibleVerticesPerSide = visibleVertices;
	terrainHeapSize = MAX_TERRAIN_HEAP_SIZE;
	//-----------------------------------------------------------------
	// Startup to Terrain Heap
	if (!terrainHeap)
	{
		terrainHeap = new UserHeap;
		gosASSERT(terrainHeap != nullptr);
		terrainHeap->init(terrainHeapSize, "TERRAIN");
	}
	percent += percentRange / 5.f;
	//-----------------------------------------------------------------
	// Startup the Terrain Texture Maps
	if (!terrainTextures)
	{
		wchar_t baseName[256];
		if (pakFile)
		{
			_splitpath(pakFile->getFilename(), nullptr, nullptr, baseName, nullptr);
		}
		else
		{
			strcpy(baseName, "newmap");
		}
		terrainTextures = new TerrainTextures;
		terrainTextures->init("textures", baseName);
	}
	percent += percentRange / 5.f;
	if (!pakFile && !realVerticesMapSide)
		return NO_ERROR;
	//-----------------------------------------------------------------
	// Startup the Terrain colour Map
	if (!terrainTextures2 && pakFile)
	{
		wchar_t name[1024];
		_splitpath(pakFile->getFilename(), nullptr, nullptr, name, nullptr);
		terrainName = new wchar_t[strlen(name) + 1];
		strcpy(terrainName, name);
		if (colorMapName)
			strcpy(name, colorMapName);
		FullPathFileName tgacolourMapName;
		tgacolourMapName.init(texturePath, name, ".tga");
		FullPathFileName tgacolourMapBurninName;
		tgacolourMapBurninName.init(texturePath, name, ".burnin.tga");
		FullPathFileName tgacolourMapJPGName;
		tgacolourMapJPGName.init(texturePath, name, ".burnin.jpg");
		if (fileExists(tgacolourMapName) || fileExists(tgacolourMapBurninName) || fileExists(tgacolourMapJPGName))
		{
			terrainTextures2 = new TerraincolourMap; // Otherwise, this will stay
				// nullptr and we know not to
				// use them
		}
	}
	percent += percentRange / 5.f;
	mapTopLeft3d.x = -worldUnitsMapSide / 2.0f;
	mapTopLeft3d.y = worldUnitsMapSide / 2.0f;
	percent += percentRange / 5.f;
	//----------------------------------------------------------------------
	// Setup number of blocks
	int32_t numberBlocks = blocksMapSide * blocksMapSide;
	numObjBlocks = numberBlocks;
	objBlockInfo = (ObjBlockInfo*)terrainHeap->Malloc(sizeof(ObjBlockInfo) * numObjBlocks);
	gosASSERT(objBlockInfo != nullptr);
	memset(objBlockInfo, 0, sizeof(ObjBlockInfo) * numObjBlocks);
	objVertexActive =
		(bool*)terrainHeap->Malloc(sizeof(bool) * realVerticesMapSide * realVerticesMapSide);
	gosASSERT(objVertexActive != nullptr);
	memset(objVertexActive, 0, sizeof(bool) * numObjBlocks);
	moverBlockList = (int32_t*)terrainHeap->Malloc(sizeof(int32_t) * numberBlocks);
	gosASSERT(moverBlockList != nullptr);
	usedBlockList = (int32_t*)terrainHeap->Malloc(sizeof(int32_t) * numberBlocks);
	gosASSERT(usedBlockList != nullptr);
	clearList();
	clearMoverList();
	//----------------------------------------------------------------------
	// Calculate size of each mapblock
	int32_t blockSize = verticesBlockSide * verticesBlockSide;
	blockSize *= sizeof(PostcompVertex);
	//----------------------------------------------------------------------
	// Create the MapBlock Manager and allocate its RAM
	if (!mapData)
	{
		mapData = new MapData;
		if (pakFile)
			mapData->newInit(pakFile, realVerticesMapSide * realVerticesMapSide);
		else
			mapData->newInit(realVerticesMapSide * realVerticesMapSide);
		mapTopLeft3d.z = mapData->getTopLeftElevation();
	}
	percent += percentRange / 5.f;
	//----------------------------------------------------------------------
	// Create the VertexList
	numberVertices = 0;
	vertexList = (VertexPtr)terrainHeap->Malloc(sizeof(Vertex) * visibleVertices * visibleVertices);
	gosASSERT(vertexList != nullptr);
	memset(vertexList, 0, sizeof(Vertex) * visibleVertices * visibleVertices);
	//----------------------------------------------------------------------
	// Create the QuadList
	numberQuads = 0;
	quadList = (TerrainQuadPtr)terrainHeap->Malloc(
		sizeof(TerrainQuad) * visibleVertices * visibleVertices);
	gosASSERT(quadList != nullptr);
	memset(quadList, 0, sizeof(TerrainQuad) * visibleVertices * visibleVertices);
	//-------------------------------------------------------------------
	initMapCellArrays();
	//-----------------------------------------------------------------
	// Startup the Terrain colour Map
	if (terrainTextures2 && !(terrainTextures2->colorMapStarted))
	{
		if (colorMapName)
			terrainTextures2->init(colorMapName);
		else
			terrainTextures2->init(terrainName);
	}
	return NO_ERROR;
}

void Terrain::resetVisibleVertices(int32_t maxVisibleVertices)
{
	terrainHeap->Free(vertexList);
	vertexList = nullptr;
	terrainHeap->Free(quadList);
	quadList = nullptr;
	visibleVerticesPerSide = maxVisibleVertices;
	//----------------------------------------------------------------------
	// Create the VertexList
	numberVertices = 0;
	vertexList = (VertexPtr)terrainHeap->Malloc(
		sizeof(Vertex) * visibleVerticesPerSide * visibleVerticesPerSide);
	gosASSERT(vertexList != nullptr);
	memset(vertexList, 0, sizeof(Vertex) * visibleVerticesPerSide * visibleVerticesPerSide);
	//----------------------------------------------------------------------
	// Create the QuadList
	numberQuads = 0;
	quadList = (TerrainQuadPtr)terrainHeap->Malloc(
		sizeof(TerrainQuad) * visibleVerticesPerSide * visibleVerticesPerSide);
	gosASSERT(quadList != nullptr);
	memset(quadList, 0, sizeof(TerrainQuad) * visibleVerticesPerSide * visibleVerticesPerSide);
}

//---------------------------------------------------------------------------
bool Terrain::IsValidTerrainPosition(Stuff::Vector3D pos)
{
	float metersCheck = (Terrain::worldUnitsMapSide / 2.0f);
	if ((pos.x > -metersCheck) && (pos.x < metersCheck) && (pos.y > -metersCheck) && (pos.y < metersCheck))
	{
		return true;
	}
	return false;
}

//---------------------------------------------------------------------------
bool Terrain::IsEditorSelectTerrainPosition(Stuff::Vector3D pos)
{
	float metersCheck = (Terrain::worldUnitsMapSide / 2.0f) - Terrain::worldUnitsPerVertex;
	if ((pos.x > -metersCheck) && (pos.x < metersCheck) && (pos.y > -metersCheck) && (pos.y < metersCheck))
	{
		return true;
	}
	return false;
}

//---------------------------------------------------------------------------
bool Terrain::IsGameSelectTerrainPosition(Stuff::Vector3D pos)
{
	float metersCheck = (Terrain::worldUnitsMapSide / 2.0f) - (Terrain::worldUnitsPerVertex * 2.0f);
	if ((pos.x > -metersCheck) && (pos.x < metersCheck) && (pos.y > -metersCheck) && (pos.y < metersCheck))
	{
		return true;
	}
	return false;
}

//---------------------------------------------------------------------------
void Terrain::purgeTransitions(void)
{
	terrainTextures->purgeTransitions();
	mapData->calcTransitions();
}

//---------------------------------------------------------------------------
void Terrain::destroy(void)
{
	if (terrainTextures)
	{
		terrainTextures->destroy();
		delete terrainTextures;
		terrainTextures = nullptr;
	}
	if (terrainTextures2)
	{
		terrainTextures2->destroy();
		delete terrainTextures2;
		terrainTextures2 = nullptr;
	}
	delete mapData;
	mapData = nullptr;
	if (terrainName)
	{
		delete[] terrainName;
		terrainName = nullptr;
	}
	if (colorMapName)
	{
		delete[] colorMapName;
		colorMapName = nullptr;
	}
	if (tileRowToWorldCoord)
	{
		terrainHeap->Free(tileRowToWorldCoord);
		tileRowToWorldCoord = nullptr;
	}
	if (tileColToWorldCoord)
	{
		terrainHeap->Free(tileColToWorldCoord);
		tileColToWorldCoord = nullptr;
	}
	if (cellToWorldCoord)
	{
		terrainHeap->Free(cellToWorldCoord);
		cellToWorldCoord = nullptr;
	}
	if (cellColToWorldCoord)
	{
		terrainHeap->Free(cellColToWorldCoord);
		cellColToWorldCoord = nullptr;
	}
	if (cellRowToWorldCoord)
	{
		terrainHeap->Free(cellRowToWorldCoord);
		cellRowToWorldCoord = nullptr;
	}
	if (moverBlockList)
	{
		terrainHeap->Free(moverBlockList);
		moverBlockList = nullptr;
	}
	if (usedBlockList)
	{
		terrainHeap->Free(usedBlockList);
		usedBlockList = nullptr;
	}
	if (vertexList)
	{
		terrainHeap->Free(vertexList);
		vertexList = nullptr;
	}
	if (quadList)
	{
		terrainHeap->Free(quadList);
		quadList = nullptr;
	}
	if (objBlockInfo)
	{
		terrainHeap->Free(objBlockInfo);
		objBlockInfo = nullptr;
	}
	if (objVertexActive)
	{
		terrainHeap->Free(objVertexActive);
		objVertexActive = nullptr;
	}
	if (terrainHeap)
	{
		terrainHeap->destroy();
		delete terrainHeap;
		terrainHeap = nullptr;
	}
	numberVertices = numberQuads = halfVerticesMapSide = realVerticesMapSide =
		visibleVerticesPerSide = blocksMapSide = 0;
	worldUnitsMapSide = 0.0f;
	mapTopLeft3d.Zero();
	numObjBlocks = 0;
	recalcShadows = recalcLight = false;
	// Reset these.  This will fix the mine problem.
	TerrainQuad::rainLightLevel = 1.0f;
	TerrainQuad::lighteningLevel = 0;
	TerrainQuad::mineTextureHandle = 0xffffffff;
	TerrainQuad::blownTextureHandle = 0xffffffff;
}

extern float textureOffset;
//---------------------------------------------------------------------------
int32_t
Terrain::update(void)
{
	//-----------------------------------------------------------------
	// Startup the Terrain colour Map
	if (terrainTextures2 && !(terrainTextures2->colorMapStarted))
	{
		if (colorMapName)
			terrainTextures2->init(colorMapName);
		else
			terrainTextures2->init(terrainName);
	}
	//----------------------------------------------------------------
	// Nothing is ever visible.  We recalc every frame.  True LOS!
	//	Terrain::VisibleBits->resetAll(0);
	if (godMode)
	{
		//		Terrain::VisibleBits->resetAll(0xff);
	}
	if (turn > terrainLineChanged + 10)
	{
		if (userInput->getKeyDown(KEY_UP) && userInput->ctrl() && userInput->alt() && !userInput->shift())
		{
			textureOffset += 0.1f;
			;
			terrainLineChanged = turn;
		}
		if (userInput->getKeyDown(KEY_DOWN) && userInput->ctrl() && userInput->alt() && !userInput->shift())
		{
			textureOffset -= 0.1f;
			;
			terrainLineChanged = turn;
		}
	}
	//---------------------------------------------------------------------
	Terrain::mapData->update();
	Terrain::mapData->makeLists(vertexList, numberVertices, quadList, numberQuads);
	return TRUE;
}

//---------------------------------------------------------------------------
void Terrain::setOverlayTile(int32_t block, int32_t vertex, int32_t offset)
{
	mapData->setOverlayTile(block, vertex, offset);
}

//---------------------------------------------------------------------------
void Terrain::setOverlay(int32_t tileR, int32_t tileC, Overlays type, uint32_t offset)
{
	mapData->setOverlay(tileR, tileC, type, offset);
}

//---------------------------------------------------------------------------
void Terrain::setTerrain(int32_t tileR, int32_t tileC, int32_t terrainType)
{
	mapData->setTerrain(tileR, tileC, terrainType);
}

//---------------------------------------------------------------------------
int32_t
Terrain::getTerrain(int32_t tileR, int32_t tileC)
{
	return mapData->getTerrain(tileR, tileC);
}

//---------------------------------------------------------------------------
void Terrain::calcWater(float waterDepth, float waterShallowDepth, float waterAlphaDepth)
{
	mapData->calcWater(waterDepth, waterShallowDepth, waterAlphaDepth);
}

//---------------------------------------------------------------------------
int32_t
Terrain::getOverlayTile(int32_t block, int32_t vertex)
{
	return (mapData->getOverlayTile(block, vertex));
}

//---------------------------------------------------------------------------
void Terrain::getOverlay(int32_t tileR, int32_t tileC, enum Overlays& type, uint32_t& Offset)
{
	mapData->getOverlay(tileR, tileC, type, Offset);
}

//---------------------------------------------------------------------------
void Terrain::setVertexheight(int32_t VertexIndex, float Val)
{
	if (VertexIndex > -1 && VertexIndex < realVerticesMapSide * realVerticesMapSide)
		mapData->setVertexheight(VertexIndex, Val);
}

//---------------------------------------------------------------------------
float Terrain::getVertexheight(int32_t VertexIndex)
{
	if (VertexIndex > -1 && VertexIndex < realVerticesMapSide * realVerticesMapSide)
		return mapData->getVertexheight(VertexIndex);
	return -1.f;
}

//---------------------------------------------------------------------------
void Terrain::render(void)
{
	//-----------------------------------
	// render the cloud layer
	if (Terrain::cloudLayer)
		Terrain::cloudLayer->render();
	//-----------------------------------
	// Draw resulting terrain quads
	TerrainQuadPtr currentQuad = quadList;
	uint32_t fogcolour = eye->fogcolour;
	for (size_t i = 0; i < numberQuads; i++)
	{
		if (drawTerrainTiles)
			currentQuad->draw();
		if (drawTerrainTiles)
			currentQuad->drawMine();
		//--------------------------
		// Used to debug stuff
		if (drawTerrainGrid)
		{
			if (useFog)
				gos_SetRenderState(gos_State_Fog, 0);
			currentQuad->drawLine();
			if (useFog)
				gos_SetRenderState(gos_State_Fog, (int32_t)&fogcolour);
		}
		else if (DrawDebugCells)
		{
			if (useFog)
				gos_SetRenderState(gos_State_Fog, 0);
			currentQuad->drawDebugCellLine();
			if (useFog)
				gos_SetRenderState(gos_State_Fog, (int32_t)&fogcolour);
		}
		else if (drawLOSGrid)
		{
			if (useFog)
				gos_SetRenderState(gos_State_Fog, 0);
			currentQuad->drawLOSLine();
			if (useFog)
				gos_SetRenderState(gos_State_Fog, (int32_t)&fogcolour);
		}
		currentQuad++;
	}
}

//---------------------------------------------------------------------------
void Terrain::renderWater(void)
{
	//-----------------------------------
	// Draw resulting terrain quads
	TerrainQuadPtr currentQuad = quadList;
	for (size_t i = 0; i < numberQuads; i++)
	{
		if (drawTerrainTiles)
			currentQuad->drawWater();
		currentQuad++;
	}
}

float cosineEyeHalfFOV = 0.0f;
#define MAX_CAMERA_RADIUS (250.0f)
#define CLIP_THRESHOLD_DISTANCE (768.0f)

// a full triangle.
#define VERTEX_EXTENT_RADIUS (384.0f)

float leastZ = 1.0f, leastW = 1.0f;
float mostZ = -1.0f, mostW = -1.0;
float leastWY = 0.0f, mostWY = 0.0f;
extern bool InEditor;
//---------------------------------------------------------------------------
void Terrain::geometry(void)
{
	//---------------------------------------------------------------------
	leastZ = 1.0f;
	leastW = 1.0f;
	mostZ = -1.0f;
	mostW = -1.0;
	leastWY = 0.0f;
	mostWY = 0.0f;
	//-----------------------------------
	// Transform entire list of vertices
	VertexPtr currentvertex = vertexList;
	Stuff::Vector3D cameraPos;
	cameraPos.x = -eye->getCameraOrigin().x;
	cameraPos.y = eye->getCameraOrigin().z;
	cameraPos.z = eye->getCameraOrigin().y;
	float vClipConstant = eye->verticalSphereClipConstant;
	float hClipConstant = eye->horizontalSphereClipConstant;
	int32_t i = 0;
	for (i = 0; i < numberVertices; i++)
	{
		//----------------------------------------------------------------------------------------
		// Figure out if we are in front of camera or not.  Should be faster
		// then actual project! Should weed out VAST overwhelming majority of
		// vertices!
		bool onScreen = false;
		//-----------------------------------------------------------------
		// Find angle between lookVector of Camera and vector from camPos
		// to Target.  If angle is less then halfFOV, object is visible.
		if (eye->usePerspective)
		{
			//-------------------------------------------------------------------
			// NEW METHOD from the WAY BACK Days
			onScreen = true;
			Stuff::Vector3D vPosition;
			vPosition.x = currentvertex->vx;
			vPosition.y = currentvertex->vy;
			vPosition.z = currentvertex->pVertex->elevation;
			Stuff::Vector3D objectCenter;
			objectCenter.Subtract(vPosition, cameraPos);
			Camera::cameraFrame.trans_to_frame(objectCenter);
			float distanceToEye = objectCenter.GetApproximateLength();
			Stuff::Vector3D clipVector = objectCenter;
			clipVector.z = 0.0f;
			float distanceToClip = clipVector.GetApproximateLength();
			float clip_distance = fabs(1.0f / objectCenter.y);
			if (distanceToClip > CLIP_THRESHOLD_DISTANCE)
			{
				// Is vertex on Screen OR close enough to screen that its
				// triangle MAY be visible?
				// WE have removed the atans here by simply taking the tan of
				// the angle we want above.
				float object_angle = fabs(objectCenter.z) * clip_distance;
				float extent_angle = VERTEX_EXTENT_RADIUS / distanceToEye;
				if (object_angle > (vClipConstant + extent_angle))
				{
					// In theory, we would return here.  Object is NOT on
					// screen.
					onScreen = false;
				}
				else
				{
					object_angle = fabs(objectCenter.x) * clip_distance;
					if (object_angle > (hClipConstant + extent_angle))
					{
						// In theory, we would return here.  Object is NOT on
						// screen.
						onScreen = false;
					}
				}
			}
			if (onScreen)
			{
				if (distanceToEye > Camera::MaxClipDistance)
				{
					currentvertex->hazeFactor = 1.0f;
				}
				else if (distanceToEye > Camera::MinHazeDistance)
				{
					currentvertex->hazeFactor =
						(distanceToEye - Camera::MinHazeDistance) * Camera::DistanceFactor;
				}
				else
				{
					currentvertex->hazeFactor = 0.0f;
				}
				//---------------------------------------
				// Vertex is at edge of world or beyond.
				Stuff::Vector3D vPos(
					currentvertex->vx, currentvertex->vy, currentvertex->pVertex->elevation);
				bool isVisible = Terrain::IsGameSelectTerrainPosition(vPos) || drawTerrainGrid;
				if (!isVisible)
				{
					currentvertex->hazeFactor = 1.0f;
					onScreen = true;
				}
			}
			else
			{
				currentvertex->hazeFactor = 1.0f;
			}
		}
		else
		{
			currentvertex->hazeFactor = 0.0f;
			onScreen = true;
		}
		bool inView = false;
		Stuff::Vector4D screenPos(-10000.0f, -10000.0f, -10000.0f, -10000.0f);
		if (onScreen)
		{
			Stuff::Vector3D vertex3D(
				currentvertex->vx, currentvertex->vy, currentvertex->pVertex->elevation);
			inView = eye->projectZ(vertex3D, screenPos);
			currentvertex->px = screenPos.x;
			currentvertex->py = screenPos.y;
			currentvertex->pz = screenPos.z;
			currentvertex->pw = screenPos.w;
			//----------------------------------------------------------------------------------
			// We must transform these but should NOT draw any face where all
			// three are fogged.
			//			if (currentvertex->hazeFactor == 1.0f)
			//				onScreen = false;
		}
		else
		{
			currentvertex->px = currentvertex->py = 10000.0f;
			currentvertex->pz = -0.5f;
			currentvertex->pw = 0.5f;
			currentvertex->hazeFactor = 0.0f;
		}
		//------------------------------------------------------------
		// Fix clip.  Vertices can all be off screen and triangle
		// still needs to be drawn!
		if (eye->usePerspective && Environment.Renderer != 3)
		{
			currentvertex->clipInfo = onScreen;
		}
		else
			currentvertex->clipInfo = inView;
		if (currentvertex->clipInfo) // ONLY set TRUE ones.  Otherwise we just
			// reset the FLAG each vertex!
		{
			setObjBlockActive(currentvertex->getBlockNumber(), true);
			setObjVertexActive(currentvertex->vertexNum, true);
			if (inView)
			{
				if (screenPos.z < leastZ)
				{
					leastZ = screenPos.z;
				}
				if (screenPos.z > mostZ)
				{
					mostZ = screenPos.z;
				}
				if (screenPos.w < leastW)
				{
					leastW = screenPos.w;
					leastWY = screenPos.y;
				}
				if (screenPos.w > mostW)
				{
					mostW = screenPos.w;
					mostWY = screenPos.y;
				}
			}
		}
		currentvertex++;
	}
	//-----------------------------------
	// setup terrain quad textures
	// Also sets up mine data.
	TerrainQuadPtr currentQuad = quadList;
	for (i = 0; i < numberQuads; i++)
	{
		currentQuad->setupTextures();
		currentQuad++;
	}
	float ywRange = 0.0f, yzRange = 0.0f;
	if (fabs(mostWY - leastWY) > Stuff::SMALL)
	{
		ywRange = (mostW - leastW) / (mostWY - leastWY);
		yzRange = (mostZ - leastZ) / (mostWY - leastWY);
	}
	eye->setInverseProject(mostZ, leastW, yzRange, ywRange);
	//-----------------------------------
	// update the cloud layer
	if (Terrain::cloudLayer)
		Terrain::cloudLayer->update();
}

//---------------------------------------------------------------------------
float Terrain::getTerrainElevation(Stuff::Vector3D& position)
{
	float result = mapData->terrainElevation(position);
	return (result);
}

//---------------------------------------------------------------------------
float Terrain::getTerrainElevation(int32_t tileR, int32_t tileC)
{
	return mapData->terrainElevation(tileR, tileC);
}

//---------------------------------------------------------------------------
uint32_t
Terrain::getTexture(int32_t tileR, int32_t tileC)
{
	return mapData->getTexture(tileR, tileC);
}

//---------------------------------------------------------------------------
float Terrain::getTerrainAngle(Stuff::Vector3D& position, Stuff::Vector3D* normal)
{
	float result = mapData->terrainAngle(position, normal);
	return (result);
}

//---------------------------------------------------------------------------
float Terrain::getTerrainLight(Stuff::Vector3D& position)
{
	float result = mapData->terrainLight(position);
	return (result);
}

//---------------------------------------------------------------------------
Stuff::Vector3D
Terrain::getTerrainNormal(Stuff::Vector3D& position)
{
	Stuff::Vector3D result = Terrain::mapData->terrainNormal(position);
	return (result);
}

//---------------------------------------------------------------------------
// Uses a simple value to mark radius.  It never changes now!!
// First value in range table!!
void Terrain::markSeen(Stuff::Vector3D& looker, byte who, float specialUnitExpand)
{
	return;
	/*		Not needed anymore.  Real LOS now.
	//-----------------------------------------------------------
	// This function marks vertices has being seen by a given side.
	Stuff::Vector3D position = looker;
	position.x -= mapTopLeft3d.x;
	position.y = mapTopLeft3d.y - looker.y;

	Stuff::Vector2DOf<float> upperLeft;
	upperLeft.x = floor(position.x * oneOverWorldUnitsPerVertex);
	upperLeft.y = floor(position.y * oneOverWorldUnitsPerVertex);

	Stuff::Vector2DOf<int32_t> meshOffset;
	meshOffset.x = float2long(upperLeft.x);
	meshOffset.y = float2long(upperLeft.y);

	uint32_t xCenter = meshOffset.x;
	uint32_t yCenter = meshOffset.y;

	//Figure out altitude above minimum terrain altitude and look up in table.
	float baseElevation = MapData::waterDepth;
	if (MapData::waterDepth < Terrain::userMin)
		baseElevation = Terrain::userMin;

	float altitude = position.z - baseElevation;
	float altitudeIntegerRange = (Terrain::userMax - baseElevation) *
	0.00390625f; int32_t altLevel = 0; if (altitudeIntegerRange > Stuff::SMALL)
		altLevel = altitude / altitudeIntegerRange;

	if (altLevel < 0)
		altLevel = 0;

	if (altLevel > 255)
		altLevel = 255;

	float radius = visualRangeTable[altLevel];

	radius += (radius * specialUnitExpand);

	if (radius <= 0.0f)
		return;

	//-----------------------------------------------------
	// Who is the shift value to create the mask
	uint8_t wer = (1 << who);

	VisibleBits->setCircle(xCenter,yCenter,float2long(radius),wer);
	*/
}

//---------------------------------------------------------------------------
// Uses dist passed in as radius.
void Terrain::markRadiusSeen(Stuff::Vector3D& looker, float dist, byte who)
{
	return;
	// Not needed.  Real LOS now!
	/*
	if (dist <= 0.0f)
		return;

	//-----------------------------------------------------------
	// This function marks vertices has being seen by
	// a given side.
	dist *= worldUnitsPerMeter;
	dist *= Terrain::oneOverWorldUnitsPerVertex;

	Stuff::Vector3D position = looker;
	position.x -= mapTopLeft3d.x;
	position.y = mapTopLeft3d.y - looker.y;

	Stuff::Vector2DOf<float> upperLeft;
	upperLeft.x = floor(position.x * oneOverWorldUnitsPerVertex);
	upperLeft.y = floor(position.y * oneOverWorldUnitsPerVertex);

	Stuff::Vector2DOf<int32_t> meshOffset;
	meshOffset.x = floor(upperLeft.x);
	meshOffset.y = floor(upperLeft.y);

	uint32_t xCenter = meshOffset.x;
	uint32_t yCenter = meshOffset.y;

	//-----------------------------------------------------
	// Who is the shift value to create the mask
	uint8_t wer = (1 << who);

	VisibleBits->setCircle(xCenter,yCenter,dist,wer);
	*/
}

//---------------------------------------------------------------------------
void Terrain::setObjBlockActive(int32_t blockNum, bool active)
{
	if ((blockNum >= 0) && (blockNum < numObjBlocks))
		objBlockInfo[blockNum].active = active;
}

//---------------------------------------------------------------------------
void Terrain::clearObjBlocksActive(void)
{
	for (size_t i = 0; i < numObjBlocks; i++)
		setObjBlockActive(i, false);
}

//---------------------------------------------------------------------------
void Terrain::setObjVertexActive(int32_t vertexNum, bool active)
{
	if ((vertexNum >= 0) && (vertexNum < (realVerticesMapSide * realVerticesMapSide)))
		objVertexActive[vertexNum] = active;
}

//---------------------------------------------------------------------------
void Terrain::clearObjVerticesActive(void)
{
	memset(objVertexActive, 0, sizeof(bool) * realVerticesMapSide * realVerticesMapSide);
}

//---------------------------------------------------------------------------
int32_t
Terrain::save(PacketFile* fileName, int32_t whichPacket, bool quickSave)
{
	if (!quickSave)
	{
		recalcShadows = true;
		mapData->calcLight();
	}
	else
	{
		recalcShadows = false;
	}
	return mapData->save(fileName, whichPacket);
}

//-----------------------------------------------------
bool Terrain::save(FitIniFile* fitFile)
{
	// write out the water info
#ifdef _DEBUG
	int32_t result =
#endif
		fitFile->writeBlock("Water");
	gosASSERT(result > 0);
	fitFile->writeIdFloat("Elevation", mapData->waterDepth);
	fitFile->writeIdFloat("Frequency", waterFreq);
	fitFile->writeIdFloat("Ampliture", waterAmplitude);
	fitFile->writeIdULong("AlphaShallow", alphaEdge);
	fitFile->writeIdULong("AlphaMiddle", alphaMiddle);
	fitFile->writeIdULong("AlphaDeep", alphaDeep);
	fitFile->writeIdFloat("AlphaDepth", mapData->alphaDepth);
	fitFile->writeIdFloat("ShallowDepth", mapData->shallowDepth);
	fitFile->writeBlock("Terrain");
	fitFile->writeIdLong("UserMin", userMin);
	fitFile->writeIdLong("UserMax", userMax);
	fitFile->writeIdFloat("TerrainMinX", tileColToWorldCoord[0]);
	fitFile->writeIdFloat("TerrainMinY", tileRowToWorldCoord[0]);
	fitFile->writeIdUChar("Noise", fractalNoise);
	fitFile->writeIdUChar("Threshold", fractalThreshold);
	if (terrainTextures2)
	{
		terrainTextures2->saveTilingFactors(fitFile);
	}
	return true;
}

bool Terrain::load(FitIniFile* fitFile)
{
	// write out the water info
	int32_t result = fitFile->seekBlock("Water");
	gosASSERT(result == NO_ERROR);
	result = fitFile->readIdFloat("Elevation", mapData->waterDepth);
	gosASSERT(result == NO_ERROR);
	waterElevation = mapData->waterDepth;
	result = fitFile->readIdFloat("Frequency", waterFreq);
	gosASSERT(result == NO_ERROR);
	result = fitFile->readIdFloat("Ampliture", waterAmplitude);
	gosASSERT(result == NO_ERROR);
	result = fitFile->readIdULong("AlphaShallow", alphaEdge);
	gosASSERT(result == NO_ERROR);
	result = fitFile->readIdULong("AlphaMiddle", alphaMiddle);
	gosASSERT(result == NO_ERROR);
	result = fitFile->readIdULong("AlphaDeep", alphaDeep);
	gosASSERT(result == NO_ERROR);
	result = fitFile->readIdFloat("AlphaDepth", mapData->alphaDepth);
	gosASSERT(result == NO_ERROR);
	result = fitFile->readIdFloat("ShallowDepth", mapData->shallowDepth);
	gosASSERT(result == NO_ERROR);
	fitFile->seekBlock("Terrain");
	fitFile->readIdLong("UserMin", userMin);
	fitFile->readIdLong("UserMax", userMax);
	fitFile->readIdUChar("Noise", fractalNoise);
	fitFile->readIdUChar("Threshold", fractalThreshold);
	return true;
}

//---------------------------------------------------------------------------
void Terrain::unselectAll()
{
	mapData->unselectAll();
}

//---------------------------------------------------------------------------
void Terrain::selectVerticesInRect(
	const Stuff::Vector4D& topLeft, const Stuff::Vector4D& bottomRight, bool bToggle)
{
	Stuff::Vector3D worldPos;
	Stuff::Vector4D screenPos;
	int32_t xMin, xMax;
	int32_t yMin, yMax;
	if (topLeft.x < bottomRight.x)
	{
		xMin = topLeft.x;
		xMax = bottomRight.x;
	}
	else
	{
		xMin = bottomRight.x;
		xMax = topLeft.x;
	}
	if (topLeft.y < bottomRight.y)
	{
		yMin = topLeft.y;
		yMax = bottomRight.y;
	}
	else
	{
		yMin = bottomRight.y;
		yMax = topLeft.y;
	}
	for (size_t i = 0; i < realVerticesMapSide; ++i)
	{
		for (size_t j = 0; j < realVerticesMapSide; ++j)
		{
			worldPos.y = tileRowToWorldCoord[j];
			worldPos.x = tileColToWorldCoord[i];
			worldPos.z = mapData->terrainElevation(j, i);
			eye->projectZ(worldPos, screenPos);
			if (screenPos.x >= xMin && screenPos.x <= xMax && screenPos.y >= yMin && screenPos.y <= yMax)
			{
				mapData->selectVertex(j, i, true, bToggle);
			}
		}
	}
}

//---------------------------------------------------------------------------
bool Terrain::hasSelection()
{
	return mapData->selection();
}

//---------------------------------------------------------------------------
bool Terrain::isVertexSelected(int32_t tileR, int32_t tileC)
{
	return mapData->isVertexSelected(tileR, tileC);
}

//---------------------------------------------------------------------------
bool Terrain::selectVertex(int32_t tileR, int32_t tileC, bool bSelect)
{
	// We never use the return value so just send back false.
	if ((tileR <= -1) || (tileR >= realVerticesMapSide))
		return false;
	if ((tileC <= -1) || (tileC >= realVerticesMapSide))
		return false;
	mapData->selectVertex(tileR, tileC, bSelect, 0);
	return true;
}

//---------------------------------------------------------------------------
float Terrain::getHighestVertex(int32_t& tileR, int32_t& tileC)
{
	float highest = -9999999.; // an absurdly small number
	for (size_t i = 0; i < realVerticesMapSide * realVerticesMapSide; ++i)
	{
		float tmp = getVertexheight(i);
		if (tmp > highest)
		{
			highest = tmp;
			tileR = i / realVerticesMapSide;
			tileC = i % realVerticesMapSide;
		}
	}
	return highest;
}

//---------------------------------------------------------------------------
float Terrain::getLowestVertex(int32_t& tileR, int32_t& tileC)
{
	float lowest = 9999999.; // an absurdly big number
	for (size_t i = 0; i < realVerticesMapSide * realVerticesMapSide; ++i)
	{
		float tmp = getVertexheight(i);
		if (tmp < lowest)
		{
			lowest = tmp;
			tileR = i / realVerticesMapSide;
			tileC = i % realVerticesMapSide;
		}
	}
	return lowest;
}

//---------------------------------------------------------------------------
void Terrain::setUserSettings(int32_t min, int32_t max, int32_t terrainType)
{
	userMin = min;
	userMax = max;
	baseTerrain = terrainType;
}

//---------------------------------------------------------------------------
void Terrain::getUserSettings(int32_t& min, int32_t& max, int32_t& terrainType)
{
	min = userMin;
	max = userMax;
	terrainType = baseTerrain;
}

//---------------------------------------------------------------------------
void Terrain::recalcWater()
{
	mapData->recalcWater();
}

//---------------------------------------------------------------------------
void Terrain::reCalcLight(bool doShadows)
{
	recalcLight = true;
	recalcShadows = doShadows;
	// Do a new burnin for the colormap
	if (terrainTextures2)
	{
		if (colorMapName)
			terrainTextures2->recalcLight(colorMapName);
		else
			terrainTextures2->recalcLight(terrainName);
	}
}

//---------------------------------------------------------------------------
void Terrain::clearShadows()
{
	mapData->clearShadows();
}

//---------------------------------------------------------------------------

int32_t
Terrain::getWater(Stuff::Vector3D& worldPos)
{
	//-------------------------------------------------
	// Get elevation at this point and compare to deep
	// water altitude for this map.
	float elevation = getTerrainElevation(worldPos);
	if (elevation < (waterElevation - MapData::shallowDepth))
		return (2);
	if (elevation < waterElevation)
		return (1);
	return (0);
}

//---------------------------------------------------------------------------
