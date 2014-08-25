//---------------------------------------------------------------------------
//
// Quad.h -- File contains class definitions for the Terrain Quads
//
//	MechCommander 2
//
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#ifndef QUAD_H
#define QUAD_H
//---------------------------------------------------------------------------
// Include Files
#ifndef DQUAD_H
#include "dquad.h"
#endif

#ifndef DVERTEX_H
#include "dvertex.h"
#endif

#ifndef DSTD_H
#include "dstd.h"
#endif

//---------------------------------------------------------------------------
typedef struct _TerrainUVData
{
	float	minU;
	float	minV;
	float	maxU;
	float 	maxV;
} TerrainUVData;

typedef struct _MineResult
{
	uint32_t mineData;
	
	void init (void)
	{
		mineData = 0;
	}
	
	void setMine (uint32_t pos, uint8_t data)
	{
		uint32_t result = (data << (pos << 1));
		mineData |= result;
	}
	
	uint32_t getMine(uint32_t pos)
	{
		uint32_t result = (mineData >> (pos << 1)) & (0x3);
		return result;
	}
	
} MineResult;

class TerrainQuad
{
	//Data Members
	//-------------
	public:

		VertexPtr			vertices[4];			//Pointers to vertices defining this tile.

		uint32_t				terrainHandle;			//Handle to texture to draw.
		uint32_t				terrainDetailHandle;	//Handle to detail texture to draw.
		uint32_t				waterHandle;			//Handle to water texture to draw.
		uint32_t				waterDetailHandle;		//Handle to Water Detail texture to draw.
		uint32_t				overlayHandle;			//Handle to overlay texture to draw.

		uint32_t				uvMode;					//Is this a top or bottom triangle?
		MineResult			mineResult;				//Is there a mine or exploded mine in this cell on this tile?
			
		TerrainUVData		uvData;					//Stores the min and max UVs for this face.
			
		bool				isCement;				//Need to know if this tile is cement for a number of reasons
	
		static float		rainLightLevel;			//How much to darken terrain based on rain
		static uint32_t		lighteningLevel;		//How much to lighten terrain based on lightening.
		static uint32_t 		mineTextureHandle;		//Handle to the mine textures.
		static uint32_t 		blownTextureHandle;

		
#ifdef _DEBUG
		bool				selected;			//Debug to show which triangle I am on.
#endif

	//Member Functions
	//-----------------
	public:

		void init (void)
		{
			for (int32_t i=0;i<4;i++)
				vertices[i] = NULL;
			
			terrainHandle = 0xffffffff;
			terrainDetailHandle = 0xffffffff;
			waterHandle = 0xffffffff;
			waterDetailHandle = 0xffffffff;

			uvMode = 0;
			
			uvData.maxU = uvData.minU = uvData.minV = uvData.maxV = 0.0f;
			
			mineResult.init(void);
			
			isCement = false;
		}

		TerrainQuad (void)
		{
			init(void);
		}

		void destroy (void)
		{
		}

		~TerrainQuad (void)
		{
			destroy(void);
		}

		int32_t init (VertexPtr v0, VertexPtr v1, VertexPtr v2, VertexPtr v3);

		void setupTextures (void);

		void draw (void);
		void drawMine (void);
		void drawLine (void);
		void drawDebugCellLine (void);
		void drawLOSLine (void);
		void drawWater (void);
};

//---------------------------------------------------------------------------
#endif
