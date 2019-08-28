//---------------------------------------------------------------------------
//
// TerrTxm2.h -- File contains class definitions for the Terrain Textures
//
//	MechCommander 2
//
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#ifndef TERRTXM2_H
#define TERRTXM2_H
//---------------------------------------------------------------------------
// Include Files
#ifndef QUAD_H
#include "quad.h"
#endif

#ifndef TXMMGR_H
#include "txmmgr.h"
#endif

#ifndef INIFILE_H
#include "inifile.h"
#endif

//---------------------------------------------------------------------------
// Macro Definitions
#ifndef NO_ERROR
#define NO_ERROR 0
#endif

#define MAX_WATER_DETAIL_TEXTURES 256

#define TOTAL_COLORMAP_TYPES 5
//---------------------------------------------------------------------------
// Class Definitions
typedef struct _colourMapTextures
{
	uint32_t mcTextureNodeIndex;
} colourMapTextures;

typedef struct _colourMapRAM
{
	uint8_t* ourRAM;
} colourMapRAM;

class TerraincolourMap
{
	// Data Members
	//-------------
protected:
	uint8_t* colourMap;

	uint32_t numTextures;

	float numTexturesAcross;
	float fractionPerTexture;

	colourMapTextures* textures;
	colourMapRAM* txmRAM;

	UserHeapPtr colorMapHeap;
	UserHeapPtr colorMapRAMHeap;

	uint8_t* detailTextureRAM;
	uint32_t detailTextureNodeIndex;
	float detailTextureTilingFactor;

	uint8_t* waterTextureRAM;
	uint32_t waterTextureNodeIndex;
	float waterTextureTilingFactor;

	uint32_t numWaterDetailFrames;
	uint32_t waterDetailNodeIndex[MAX_WATER_DETAIL_TEXTURES];
	float waterDetailFrameRate;
	float waterDetailTilingFactor;

	static uint32_t terrainTypeIDs[TOTAL_COLORMAP_TYPES];

public:
	bool colorMapStarted;

	float hGauss;
	float roughDistance;

	// Member Functions
	//-----------------
protected:
public:
	void init(void);

	TerraincolourMap(void) { init(void); }

	void destroy(void);

	~TerraincolourMap(void) { destroy(void); }

	int32_t init(const std::wstring_view& fileName);

	void getcolourMapData(uint8_t* ourRAM, int32_t index, int32_t width);

	uint32_t getTextureHandle(VertexPtr vMin, VertexPtr vMax, TerrainUVData* uvData);

	uint32_t getDetailHandle(void)
	{
		mcTextureManager->get_gosTextureHandle(detailTextureNodeIndex);
		return (detailTextureNodeIndex);
	}
	int32_t saveDetailTexture(const std::wstring_view& fileName);

	uint32_t getWaterTextureHandle(void)
	{
		mcTextureManager->get_gosTextureHandle(waterTextureNodeIndex);
		return waterTextureNodeIndex;
	}
	int32_t saveWaterTexture(const std::wstring_view& fileName);

	uint32_t getWaterDetailHandle(int32_t frameNum)
	{
		if ((frameNum >= 0) && (frameNum < (int32_t)numWaterDetailFrames))
		{
			mcTextureManager->get_gosTextureHandle(waterDetailNodeIndex[frameNum]);
			return waterDetailNodeIndex[frameNum];
		}
		else
			return 0xffffffff;
	}
	int32_t saveWaterDetail(const std::wstring_view& fileName);

	uint32_t getWaterDetailNumFrames(void) { return numWaterDetailFrames; }

	float getWaterDetailFrameRate(void) { return waterDetailFrameRate; }

	float getDetailTilingFactor(void) { return detailTextureTilingFactor; }

	float getWaterTextureTilingFactor(void) { return waterTextureTilingFactor; }

	float getWaterDetailTilingFactor(void) { return waterDetailTilingFactor; }

	void setWaterDetailFrameRate(float frameRate) { waterDetailFrameRate = frameRate; }

	void setDetailTilingFactor(float tf) { detailTextureTilingFactor = tf; }

	void setWaterTextureTilingFactor(float tf) { waterTextureTilingFactor = tf; }

	void setWaterDetailTilingFactor(float tf) { waterDetailTilingFactor = tf; }

	int32_t saveTilingFactors(FitIniFile* fitFile);

	// Mike, these functions will reload these textures from disk.
	// This allows us to change them in the editor and reload here.
	// Pass in the filename of the mission!!!!
	void resetBaseTexture(const std::wstring_view& fileName);
	void resetDetailTexture(const std::wstring_view& fileName);
	void resetWaterTexture(const std::wstring_view& fileName);
	void resetWaterDetailTextures(const std::wstring_view& fileName);

	// Pass in filename of height map to write new data to.
	void refractalizeBaseMesh(const std::wstring_view& fileName, int32_t Threshold, int32_t Noise);

	void burnInShadows(bool doBumpPass = true, const std::wstring_view& fileName = nullptr);

	void recalcLight(const std::wstring_view& fileName);

	static int32_t getNumTypes(void) { return TOTAL_COLORMAP_TYPES; }

	static int32_t getTextureNameID(int32_t idNum)
	{
		if ((idNum >= 0) && (idNum < TOTAL_COLORMAP_TYPES))
			return terrainTypeIDs[idNum];
		return -1;
	}

	// Used by editor for TacMap
	void getScaledcolourMap(uint8_t* bfr, int32_t width);
};

typedef TerraincolourMap* TerraincolourMapPtr;
//---------------------------------------------------------------------------
#endif
