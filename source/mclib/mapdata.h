//---------------------------------------------------------------------------
//
// MapData.h -- File contains class definitions for the Terrain Mesh
//
//	MechCommander 2
//
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#ifndef MAPDATA_H
#define MAPDATA_H
//---------------------------------------------------------------------------
// Include Files

#ifndef DMAPDATA_H
#include "dmapdata.h"
#endif

#ifndef HEAP_H
#include "heap.h"
#endif

#ifndef PACKET_H
#include "packet.h"
#endif

#ifndef VERTEX_H
#include "vertex.h"
#endif

#ifndef QUAD_H
#include "quad.h"
#endif

#include "stuff/stuff.h"

//---------------------------------------------------------------------------
enum Overlays
{
	INVALID_OVERLAY = -1,
	DIRT_ROAD = 0,
	PAVED_ROAD = 1,
	ROUGH = 2,
	DIRT_ROAD_TO_PAVED_ROAD = 3,
	PAVED_ROAD_CROSSING_DIRT_ROAD = 4,
	TWO_LANE_DIRT_ROAD = 5,
	DAMAGED_ROAD = 6,
	RUNWAY = 7,
	X_DIRT_PAVED = 8,
	X_DIRT_2LANEDIRT = 9,
	X_DIRT_DAMAGED = 10,
	X_2LANEDIRT_PAVED = 11,
	X_DAMAGED_PAVED = 12,
	X_2LANEDIRT_DAMAGED = 13,
	OBRIDGE = 14,
	X_PAVED_ROAD_BRIDGE = 15,
	DAMAGED_BRIDGE = 16,
	NUM_OVERLAY_TYPES = 17
};

//---------------------------------------------------------------------------
// Classes
class MapData : public HeapManager
{
	// Data Members
	//-------------
protected:
	PostcompVertexPtr blocks;
	PostcompVertexPtr blankVertex;
	int32_t hasSelection;

public:
	Stuff::Vector2DOf<float> topLeftVertex;

	static float shallowDepth;
	static float waterDepth;
	static float alphaDepth;
	static uint32_t WaterTXMData;

	// Member Functions
	//-----------------
public:
	PVOID operator new(size_t mySize);
	void operator delete(PVOID us);

	void init(void)
	{
		HeapManager::init(void);
		topLeftVertex.Zero(void);
		blocks = nullptr;
		blankVertex = nullptr;
		hasSelection = false;
		shallowDepth = 0.0f;
		waterDepth = 0.0f;
		alphaDepth = 0.0f;
		WaterTXMData = 0xffffffff;
	}

	MapData(void) :
		HeapManager() { init(void); }

	void destroy(void);

	~MapData(void) { destroy(void); }

	int32_t init(const std::wstring_view& fileName, int32_t numBlocks, int32_t blockSize);

	void newInit(PacketFile* file, uint32_t numvertices);
	void newInit(uint32_t numvertices);

	int32_t update(void);
	void makeLists(
		VertexPtr vertexList, int32_t& numVerts, TerrainQuadPtr quadList, int32_t& numTiles);

	Stuff::Vector2DOf<float> getTopLeftVertex(void) { return topLeftVertex; }

	void calcLight(void);
	void clearShadows(void);

	float terrainElevation(Stuff::Vector3D& position);
	float terrainElevation(int32_t tileR, int32_t tileC);

	float terrainAngle(Stuff::Vector3D& position, Stuff::Vector3D* normal = nullptr);
	Stuff::Vector3D terrainNormal(Stuff::Vector3D& position);
	float terrainLight(Stuff::Vector3D& position);

	float getTopLeftElevation(void);

	// old overlay stuff
	void setOverlayTile(int32_t block, int32_t vertex, int32_t offset);
	int32_t getOverlayTile(int32_t block, int32_t vertex);

	// new overlay stuff
	void setOverlay(int32_t tileR, int32_t tileC, Overlays type, uint32_t Offset);
	void getOverlay(int32_t tileR, int32_t tileC, Overlays& type, uint32_t& Offset);
	void setTerrain(int32_t tileR, int32_t tileC, int32_t terrainType);
	int32_t getTerrain(int32_t tileR, int32_t tileC);

	void setVertexheight(int32_t vertexIndex, float value);
	float getVertexheight(int32_t vertexIndex);

	PostcompVertexPtr getData(void) { return blocks; }

	uint32_t getTexture(int32_t tileR, int32_t tileC);

	int32_t save(PacketFile* file, int32_t whichPacket);

	void calcWater(float waterDepth, float waterShallowDepth, float waterAlphaDepth);
	void recalcWater(void); // Uses above values already passed in to just recalc the water

	float waterElevation() { return waterDepth; }

	void markSeen(Stuff::Vector2DOf<float>& topLeftPosition, VertexPtr vertexList,
		Stuff::Vector3D& looker, Stuff::Vector3D& lookVector, float cone, float dist, byte who);

	void unselectAll(void);
	void unhighlightAll(void);
	void highlightAllTransitionsOver2(void);
	void selectVertex(uint32_t tileRow, uint32_t tileCol, bool bSelect, bool bToggle);
	bool selection() { return hasSelection ? true : false; }
	bool isVertexSelected(uint32_t tileRow, uint32_t tileCol);

	void calcTransitions(void);
};

//-----------------------------------------------------------------------------------------------
#endif
