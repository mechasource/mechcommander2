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
#define NO_ERROR		0
#endif

#define MAX_WATER_DETAIL_TEXTURES	256

#define TOTAL_COLORMAP_TYPES		5
//---------------------------------------------------------------------------
// Class Definitions
typedef struct _ColorMapTextures
{
	ULONG 		mcTextureNodeIndex;
} ColorMapTextures;

typedef struct _ColorMapRAM
{
	PUCHAR 	ourRAM;
} ColorMapRAM;

class TerrainColorMap
{
	//Data Members
	//-------------
	protected:
		PUCHAR				ColorMap;
		
		ULONG					numTextures;
		
		float					numTexturesAcross;
		float					fractionPerTexture;
		
		ColorMapTextures		*textures;
		ColorMapRAM				*txmRAM;
		
		UserHeapPtr				colorMapHeap;
		UserHeapPtr				colorMapRAMHeap;
		
		PUCHAR				detailTextureRAM;
		ULONG					detailTextureNodeIndex;
		float					detailTextureTilingFactor;
		
		PUCHAR				waterTextureRAM;
		ULONG					waterTextureNodeIndex;
		float					waterTextureTilingFactor;
		
		ULONG					numWaterDetailFrames;
		ULONG					waterDetailNodeIndex[MAX_WATER_DETAIL_TEXTURES];
		float					waterDetailFrameRate;
		float					waterDetailTilingFactor;

		static ULONG			terrainTypeIDs[ TOTAL_COLORMAP_TYPES ];
		
	public:
	
		bool					colorMapStarted;
		
		float					hGauss;
		float					roughDistance;

	//Member Functions
	//-----------------
	protected:

	public:
	
		void init (void);

		TerrainColorMap (void)
		{
			init();
		}

		void destroy (void);

		~TerrainColorMap (void)
		{
			destroy();
		}

		int32_t init (PSTR fileName);

		void getColorMapData (PUCHAR ourRAM, int32_t index, int32_t width);
				
		ULONG getTextureHandle (VertexPtr vMin, VertexPtr vMax, TerrainUVData *uvData);

		ULONG getDetailHandle (void)
		{
			mcTextureManager->get_gosTextureHandle(detailTextureNodeIndex);
			return (detailTextureNodeIndex);
		}
		int32_t saveDetailTexture(PCSTR fileName);

		ULONG getWaterTextureHandle (void)
		{
			mcTextureManager->get_gosTextureHandle(waterTextureNodeIndex);
			return waterTextureNodeIndex;
		}
		int32_t saveWaterTexture(PCSTR fileName);

		ULONG getWaterDetailHandle (int32_t frameNum)
		{
			if ((frameNum >= 0) && (frameNum < (int32_t)numWaterDetailFrames))
			{
				mcTextureManager->get_gosTextureHandle(waterDetailNodeIndex[frameNum]);
				return waterDetailNodeIndex[frameNum];
			}
			else
				return 0xffffffff;
		}
		int32_t saveWaterDetail(PCSTR fileName);

		ULONG getWaterDetailNumFrames (void)
		{
			return numWaterDetailFrames;
		}
		
		float getWaterDetailFrameRate (void)
		{
			return waterDetailFrameRate;
		}

		float getDetailTilingFactor (void)
		{
			return detailTextureTilingFactor;
		}

		float getWaterTextureTilingFactor(void)
		{
			return waterTextureTilingFactor;
		}

		float getWaterDetailTilingFactor(void)
		{
			return waterDetailTilingFactor;
		}
		
		void setWaterDetailFrameRate (float frameRate)
		{
			waterDetailFrameRate = frameRate;
		}

		void setDetailTilingFactor (float tf)
		{
			detailTextureTilingFactor = tf;
		}

		void setWaterTextureTilingFactor (float tf)
		{
			waterTextureTilingFactor = tf;
		}

		void setWaterDetailTilingFactor (float tf)
		{
			waterDetailTilingFactor = tf;
		}

		int32_t saveTilingFactors(FitIniFile *fitFile);
		
 		//Mike, these functions will reload these textures from disk.
		// This allows us to change them in the editor and reload here.
		// Pass in the filename of the mission!!!!
		void resetBaseTexture (PSTR fileName);
		void resetDetailTexture (PCSTR fileName);
		void resetWaterTexture (PCSTR fileName);
		void resetWaterDetailTextures (PSTR fileName);
		
		//Pass in filename of height map to write new data to.
		void refractalizeBaseMesh (PSTR fileName, int32_t Threshold, int32_t Noise);
		
		void burnInShadows (bool doBumpPass = true, PSTR  fileName = NULL);
		
		void recalcLight(PSTR fileName);

		static int32_t getNumTypes (void)
		{
			return TOTAL_COLORMAP_TYPES;
		}

		static int32_t getTextureNameID (int32_t idNum)
		{
			if ((idNum >= 0) && (idNum < TOTAL_COLORMAP_TYPES))
				return terrainTypeIDs[idNum];

			return -1;
		}

		//Used by editor for TacMap
		void getScaledColorMap (PUCHAR bfr, int32_t width);
};

typedef TerrainColorMap *TerrainColorMapPtr;
//---------------------------------------------------------------------------
#endif

