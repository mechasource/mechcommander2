//---------------------------------------------------------------------------
//
// Terrain.h -- File contains class definitions for the terrain class.
//
//	MechCommander 2
//
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#pragma once

#ifndef TERRAIN_H
#define TERRAIN_H

//---------------------------------------------------------------------------

//#include "mapdata.h"
//#include <terrtxm.h>
//#include "terrtxm2.h"
//#include "bitflag.h"
//#include "inifile.h"
//#include "mathfunc.h"
//#include "dquad.h"
//#include "dvertex.h"
//#include "clouds.h"

//---------------------------------------------------------------------------
// Macro Definitions

#define	MAPCELL_DIM				3
#define	MAX_MAP_CELL_WIDTH		720
#define TACMAP_SIZE				128.f

//------------------------------------------------
// Put back in Move code when Glenn moves it over.
// 07/28/99 these numbers didn't correspond to clan/IS despite comment, so I chagned 'em.
// These MUST be these numbers or the game will not mark LOS correctly!
#define NOTEAM					-1
//#define TEAM1					0 	//this is PLAYER TEAM -- Single Player
#define TEAM2					1	//this is OPFOR TEAM -- Single Player
#define TEAM3					2	// this is allies 
#define TEAM4					3
#define TEAM5					4
#define TEAM6					5
#define TEAM7					6
#define TEAM8					7

//-------------------------------------------
// 08/01/99 -- Must have generic alignments or Heidi goes WAY south!
#define EDITOR_TEAMNONE			-1		//Allied
#define EDITOR_TEAM1			0		//Player
#define EDITOR_TEAM2			1		//Enemy
#define EDITOR_TEAM3			2
#define EDITOR_TEAM4			3
#define EDITOR_TEAM5			4
#define EDITOR_TEAM6			5
#define EDITOR_TEAM7			6
#define EDITOR_TEAM8			7

//---------------------------------------------------------------------------
// Used by the object system to load the objects on the terrain.
typedef struct _ObjBlockInfo
{
	bool		active;
	int32_t		numCollidableObjects;
	int32_t		numObjects;					// includes collidable objects
	int32_t		firstHandle;				// collidables, followed by non
} ObjBlockInfo;

//---------------------------------------------------------------------------
//Everything goes through here now.
// This will understand the original MC2 format and new format and will convert between
class Terrain
{
	//Data Members
	//-------------
protected:

	uint32_t							terrainHeapSize;

	int32_t									numberVertices;
	int32_t									numberQuads;
	VertexPtr								vertexList;
	TerrainQuadPtr							quadList;

public:
	//For editor
	static int32_t								userMin;
	static int32_t								userMax;
	static uint32_t					baseTerrain;
	static uint8_t					fractalThreshold;
	static uint8_t					fractalNoise;

	static int32_t								halfVerticesMapSide;		//Half of the below value.
	static int32_t								realVerticesMapSide;		//Number of vertices on each side of map.

	static const int32_t						verticesBlockSide;			//Always 20.
	static int32_t								blocksMapSide;				//Calced from above and
	static float							worldUnitsMapSide;			//Total world units map is across.
	static float							oneOverWorldUnitsMapSide;	//Inverse of the above.

	static int32_t								visibleVerticesPerSide;		//How many should I process to be sure I got all I could see.

	static const float						worldUnitsPerVertex;		//How many world Units between each vertex.  128.0f in current universe.
	static const float						worldUnitsPerCell;			//How many world units between cells.  42.66666667f ALWAYS!!!!
	static const float						halfWorldUnitsPerCell;		//Above divided by two.
	static const float						metersPerCell;				//Number of meters per cell.  8.53333333f ALWAYS!!
	static const float						oneOverWorldUnitsPerVertex;	//Above numbers inverse.
	static const float						oneOverWorldUnitsPerCell;
	static const float						oneOverMetersPerCell;
	static const float						oneOverVerticesBlockSide;
	static const float						worldUnitsBlockSide;		//Total world units each block of 20 vertices is.  2560.0f in current universe.

	static Stuff::Vector3D					mapTopLeft3d;				//Where does the terrain start.

	static MapDataPtr						mapData;					//Pointer to class that manages terrain mesh data.
	static TerrainTexturesPtr				terrainTextures;			//Pointer to class that manages terrain textures.
	static TerrainColorMapPtr				terrainTextures2;			//Pointer to class that manages the NEW color map terrain texture.
	static UserHeapPtr						terrainHeap;				//Heap used for terrain.

//		static ByteFlag							*VisibleBits;				//What can currently be seen

	static char*							 terrainName;				//Name of terrain data file.
	static char*								colorMapName;				//Name of colormap, if different from terrainName.

	static float							oneOverWorldUnitsPerElevationLevel;

	static float							waterElevation;				//Actual height of water in world units.
	static float							frameAngle;					//Used to animate the waves
	static float							frameCos;
	static float							frameCosAlpha;
	static uint32_t 							alphaMiddle;				//Used to alpha the water into the shore.
	static uint32_t 							alphaEdge;
	static uint32_t 							alphaDeep;
	static float							waterFreq;					//Used to animate waves.
	static float							waterAmplitude;

	static int32_t		   						numObjBlocks;				//Stores terrain object info.
	static ObjBlockInfo*						objBlockInfo;				//Dynamically allocate this please!!

	static bool*								objVertexActive;			//Stores whether or not this vertices objects need to be updated

	static float*							 tileRowToWorldCoord;		//Arrays used to help change from tile and cell to actual world position.
	static float*							 tileColToWorldCoord;		//TILE functions will be obsolete with new system.
	static float*							 cellToWorldCoord;
	static float*							 cellColToWorldCoord;
	static float*							 cellRowToWorldCoord;

	static bool								recalcShadows;				//Should we recalc the shadow map!
	static bool								recalcLight;				//Should we recalc the light data.

	static Clouds*							cloudLayer;

	//Member Functions
	//-----------------
public:

	void init(void);

	Terrain(void)
	{
		init(void);
	}

	void destroy(void);

	~Terrain(void)
	{
		destroy(void);
	}

	int32_t init(PacketFile* file, int32_t whichPacket, uint32_t visibleVertices,
				 volatile float& progress, float progressRange); // open an existing file
	int32_t init(uint32_t verticesPerMapSide, PacketFile* file, uint32_t visibleVertices,
				 volatile float& percent,
				 float percentRange); // pass in null for a blank new map

	float getTerrainElevation(Stuff::Vector3D& position);
	int16_t getTerrainType(Stuff::Vector3D& position);
	float getTerrainAngle(Stuff::Vector3D& position, Stuff::Vector3D* normal = nullptr);
	Stuff::Vector3D getTerrainNormal(Stuff::Vector3D& position);
	float getTerrainLight(Stuff::Vector3D& position);
	bool isVisible(Stuff::Vector3D& looker, Stuff::Vector3D& looked_at);

	float getWaterElevation()
	{
		return mapData->waterElevation(void);
	}

	void markSeen(Stuff::Vector3D& looker, byte who, float specialUnitExpand);
	void markRadiusSeen(Stuff::Vector3D& looker, float dist, byte who);

	int32_t update(void);
	void render(void);
	void renderWater(void);

	void geometry(void);

	void drawTopView(void);

	static bool IsValidTerrainPosition(Stuff::Vector3D pos);
	static bool IsEditorSelectTerrainPosition(Stuff::Vector3D pos);
	static bool IsGameSelectTerrainPosition(Stuff::Vector3D pos);

	int32_t save(PacketFile* fileName, int32_t whichPacket, bool QuickSave = false);
	bool save(FitIniFile* fitFile);   // save stuff like water info
	bool load(FitIniFile* fitFile);

	// old overlay stuff
	void setOverlayTile(int32_t block, int32_t vertex, int32_t offset);
	int32_t getOverlayTile(int32_t block, int32_t vertex);

	// new overlay stuff
	void setOverlay(int32_t tileR, int32_t tileC, Overlays type, uint32_t Offset);
	void getOverlay(int32_t tileR, int32_t tileC, Overlays& type, uint32_t& Offset);
	void setTerrain(int32_t tileR, int32_t tileC, int32_t terrainType);
	int32_t	 getTerrain(int32_t tileR, int32_t tileC);
	uint32_t getTexture(int32_t tileR, int32_t tileC);
	float getTerrainElevation(int32_t tileR, int32_t tileC);

	void  setVertexHeight(int32_t vertexIndex, float value);
	float getVertexHeight(int32_t vertexIndex);

	void calcWater(float waterDepth, float waterShallowDepth, float waterAlphaDepth);

	void updateAllObjects(void);

	void setObjBlockActive(int32_t blockNum, bool active);
	void clearObjBlocksActive(void);

	inline void worldToTile(const Stuff::Vector3D& pos, int32_t& tileR, int32_t& tileC);
	inline void worldToCell(const Stuff::Vector3D& pos, int32_t& cellR, int32_t& cellC);
	inline void worldToTileCell(const Stuff::Vector3D& pos, int32_t& tileR, int32_t& tileC, int32_t& cellR, int32_t& cellC);
	inline void tileCellToWorld(int32_t tileR, int32_t tileC, int32_t cellR, int32_t cellC, Stuff::Vector3D& worldPos);
	inline void cellToWorld(int32_t cellR, int32_t cellC, Stuff::Vector3D& worldPos);

	inline void getCellPos(int32_t cellR, int32_t cellC,  Stuff::Vector3D& cellPos);

	void initMapCellArrays(void);

	void unselectAll(void);
	void selectVerticesInRect(const Stuff::Vector4D& topLeft, const Stuff::Vector4D& bottomRight, bool bToggle);
	bool hasSelection(void);
	bool isVertexSelected(int32_t tileR, int32_t tileC);
	bool selectVertex(int32_t tileR, int32_t tileC, bool bSelect = true);

	float getHighestVertex(int32_t& tileR, int32_t& tileC);
	float getLowestVertex(int32_t& tileR, int32_t& tileC);

	static void setUserSettings(int32_t min, int32_t max, int32_t terrainType);
	static void getUserSettings(int32_t& min, int32_t& max, int32_t& terrainType);

	void recalcWater(void);
	void reCalcLight(bool doShadows = false);
	void clearShadows(void);

	int32_t getWater(Stuff::Vector3D& worldPos);

	float getClipRange()
	{
		return 0.5 * worldUnitsPerVertex * (float)(visibleVerticesPerSide);
	}

	void setClipRange(float clipRange)
	{
		visibleVerticesPerSide = 2.0 * clipRange / worldUnitsPerVertex;
	}

	void purgeTransitions(void);

	TerrainQuadPtr getQuadList(void)
	{
		return(quadList);
	}

	VertexPtr getVertexList(void)
	{
		return(vertexList);
	}

	int32_t getNumVertices(void)
	{
		return(numberVertices);
	}

	int32_t getNumQuads(void)
	{
		return(numberQuads);
	}

	void setObjVertexActive(int32_t vertexNum, bool active);

	void clearObjVerticesActive(void);

	void resetVisibleVertices(int32_t maxVisibleVertices);

	void getColorMapName(FitIniFile* file);
	void setColorMapName(PSTR mapName);
	void saveColorMapName(FitIniFile* file);
};

typedef Terrain* TerrainPtr;

extern TerrainPtr land;

//---------------------------------------------------------------------------

inline void Terrain::worldToTile(const Stuff::Vector3D& pos, int32_t& tileR, int32_t& tileC)
{
	float tmpX = pos.x - land->mapTopLeft3d.x;
	float tmpY = land->mapTopLeft3d.y - pos.y;
	tileC = float2long(tmpX * oneOverWorldUnitsPerVertex);
	tileR =	float2long(tmpY * oneOverWorldUnitsPerVertex);
}

//---------------------------------------------------------------------------

inline void Terrain::worldToCell(const Stuff::Vector3D& pos, int32_t& cellR, int32_t& cellC)
{
	cellC = float2long((pos.x - land->mapTopLeft3d.x) * (oneOverWorldUnitsPerVertex * 3.0f));
	cellR = float2long((land->mapTopLeft3d.y - pos.y) * (oneOverWorldUnitsPerVertex * 3.0f));
}

//---------------------------------------------------------------------------

inline void Terrain::worldToTileCell(const Stuff::Vector3D& pos, int32_t& tileR, int32_t& tileC, int32_t& cellR, int32_t& cellC)
{
	float tmpX = pos.x - land->mapTopLeft3d.x;
	float tmpY = land->mapTopLeft3d.y - pos.y;
	tileC = tmpX * oneOverWorldUnitsPerVertex;
	tileR =	tmpY * oneOverWorldUnitsPerVertex;
	if((tileC < 0) ||
			(tileR < 0) ||
			(tileC >= Terrain::realVerticesMapSide) ||
			(tileR >= Terrain::realVerticesMapSide))
	{
#ifdef _DEBUG
		PAUSE(("called worldToTileCell with POS out of bounds? Result TC:%d TR:%d", tileC, tileR));
#endif
		tileC = tileR = 0;
	}
	cellC = (pos.x - tileColToWorldCoord[tileC]) * oneOverWorldUnitsPerCell;
	cellR = (tileRowToWorldCoord[tileR] - pos.y) * oneOverWorldUnitsPerCell;
}

//---------------------------------------------------------------------------

inline void Terrain::tileCellToWorld(int32_t tileR, int32_t tileC, int32_t cellR, int32_t cellC, Stuff::Vector3D& worldPos)
{
	if((tileC < 0) ||
			(tileR < 0) ||
			(tileC >= Terrain::realVerticesMapSide) ||
			(tileR >= Terrain::realVerticesMapSide) ||
			(cellC < 0) ||
			(cellR < 0) ||
			(cellC >= MAPCELL_DIM) ||
			(cellR >= MAPCELL_DIM))
	{
#ifdef _DEBUG
		PAUSE(("called cellToWorld with tile or cell out of bounds. TC:%d TR:%d CR:%d CC:%d", tileC, tileR, cellR, cellC));
#endif
		tileR = tileC = cellR = cellC = 0;
	}
	else
	{
		worldPos.x = tileColToWorldCoord[tileC] + cellToWorldCoord[cellC] + halfWorldUnitsPerCell;
		worldPos.y = tileRowToWorldCoord[tileR] - cellToWorldCoord[cellR] - halfWorldUnitsPerCell;
		worldPos.z = (float)0.0;
	}
}

//---------------------------------------------------------------------------

inline void Terrain::cellToWorld(int32_t cellR, int32_t cellC, Stuff::Vector3D& worldPos)
{
	if((cellR < 0) ||
			(cellC < 0) ||
			(cellR >= (Terrain::realVerticesMapSide * MAPCELL_DIM)) ||
			(cellC >= (Terrain::realVerticesMapSide * MAPCELL_DIM)))
	{
#ifdef _DEBUG
		PAUSE(("called cellToWorld with cell out of bounds. CellR:%d   CellC:%d", cellR, cellC));
#endif
		worldPos.x = worldPos.y = worldPos.z = 0.0f;
	}
	else
	{
		worldPos.x = cellColToWorldCoord[cellC] + halfWorldUnitsPerCell;
		worldPos.y = cellRowToWorldCoord[cellR] - halfWorldUnitsPerCell;
		worldPos.z = (float)0.0;
	}
}

//---------------------------------------------------------------------------

inline void Terrain::getCellPos(int32_t cellR, int32_t cellC,  Stuff::Vector3D& cellPos)
{
	cellPos.x = (cellC * (worldUnitsPerVertex / 3.)) + (worldUnitsPerVertex / 6.);
	cellPos.y = (cellR * (worldUnitsPerVertex / 3.)) + (worldUnitsPerVertex / 6.);
	cellPos.x += land->mapTopLeft3d.x;
	cellPos.y = land->mapTopLeft3d.y - cellPos.y;
	cellPos.z = land->getTerrainElevation(cellPos);
}

//---------------------------------------------------------------------------
#endif

//---------------------------------------------------------------------------
//
// Edit Log
//
//---------------------------------------------------------------------------
