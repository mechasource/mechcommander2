//---------------------------------------------------------------------------
//
// TerrTxm.h -- File contains class definitions for the Terrain Textures
//
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#ifndef TERRTXM_H
#define TERRTXM_H
//---------------------------------------------------------------------------

//#include "dterrtxm.h"
//#include "mapdata.h"
//#include "txmmgr.h"

//---------------------------------------------------------------------------
// Macro Definitions

#define BLANK_TILE					(-1)
#define MC_MAX_TERRAIN_TXMS			3000
#define MC_MASK_NUM					14
#define MC_MAX_MIP_LEVELS			1
#define USE_SOFTWARE_MIPMAP
#define BASE_CEMENT_TYPE			10
#define START_CEMENT_TYPE			13
#define END_CEMENT_TYPE				20

extern int32_t TERRAIN_TXM_SIZE;
//---------------------------------------------------------------------------
// Class Definitions
extern int32_t tileCacheReqs;
extern int32_t tileCacheHits;
extern int32_t tileCacheMiss;

#define	MC2_TERRAIN_CEMENT_FLAG		0x00000001
#define MC2_TERRAIN_ALPHA_FLAG		0x00000002

//----------------------------------------------------
struct TerrainTXM
{
	uint32_t			flags;						//Tells me if this texture is a cement texture, has alpha, etc.
	uint32_t			mcTextureNodeIndex;			//Pointer to MCTextureNode which is used to cache handles if necessary
};

struct MC_TerrainType
{
	int32_t			terrainId;
	char*			terrainName;
	char*			maskName;
	int32_t			terrainPriority;
	puint8_t*		textureData;
	puint8_t*		maskData;
	int32_t			baseTXMIndex;				//Index of Highest MIP Level of Texture.
	uint32_t	terrainMapRGB;				//Rgb Color used for TacMap.
	int32_t			nameId;
};

struct MC_DetailType
{
	int32_t			detailId;
	char*			detailName;
	puint8_t*		detailData;
	int32_t			numDetails;
	float			frameRate;
	float			tilingFactor;		//How often does texture repeat
	int32_t			baseTXMIndex;
};

struct MC_OverlayType
{
	int32_t			overlayId;
	char*			overlayName;
	int32_t			numTextures;
	int32_t			oldOverlayId;
	bool			isMLRAppearance;
	puint8_t*		overlayData;
	int32_t			baseTXMIndex;
	uint32_t	terrainMapRGB;
};

struct TransitionType
{
	uint32_t 			baseTXMIndex;				//When we build transition, where did we stick it
	uint32_t 			transitionIndex;			//Code/Hash/Whatever to uniquely ID this transition.
	uint32_t			overlayIndex;				//Overlays blended onto texture now.
};

//---------------------------------------------------------------------------
class TerrainTextures
{
	//Data Members
	//-------------
protected:

	static int32_t			numTxms;
	TerrainTXM*			textures;
	static int32_t			nextAvailable;
	int32_t				firstTransition;

	int32_t				numTypes;
	MC_TerrainTypePtr	types;

	int32_t				numOverlays;
	MC_OverlayTypePtr	overlays;

	int32_t				numDetails;
	MC_DetailTypePtr	details;

	int32_t				numTransitions;
	TransitionTypePtr	transitions;
	int32_t				nextTransition;

	UserHeapPtr			tileHeap;
	UserHeapPtr			tileRAMHeap;

	int32_t				globalMipLevel;
	int32_t				firstOverlay;

	bool				quickLoad;				//Checks for list o magic textures and then checks that each magic texture exists.
	char*				localBaseName;
	File*				listTransitionFile;

	static bool			terrainTexturesInstrumented;

public:

	//Member Functions
	//-----------------
protected:

	int32_t initTexture(int32_t typeNum);
	void initMask(int32_t typeNum);
	int32_t initDetail(int32_t typeNum, int32_t detailNum);
	int32_t initOverlay(int32_t overlayNum, int32_t txmNum, PSTR txmName);

	int32_t loadTextureMemory(PSTR textureName, int32_t mipSize);
	int32_t loadOverlayMemory(PSTR overlayName);
	int32_t textureFromMemory(puint8_t ourRam, int32_t mipSize);
	int32_t loadDetailMemory(PSTR detailName, int32_t mipSize);
	int32_t textureFromMemoryAlpha(puint8_t ourRam, int32_t mipSize);

	int32_t createTransition(uint32_t typeInfo, uint32_t overlayInfo = 0);
	void combineTxm(puint8_t dest, uint32_t binNumber, int32_t type, int32_t mipLevel);
	void combineOverlayTxm(puint8_t dest, int32_t type, int32_t mipLevel);

public:

	void init(void)
	{
		tileHeap = nullptr;
		tileRAMHeap = nullptr;
		//----------------------------------------------
		// Stores composited and raw textures in VidMem
		numTxms = MC_MAX_TERRAIN_TXMS;
		nextAvailable = 0;
		textures = nullptr;
		firstTransition = -1;
		//--------------------------------------------------
		// Stores Base Terrain Textures and Masks in SysMem
		numTypes = 0;
		types = nullptr;
		//----------------------------------------------------------
		// Stores Overlay Terrain Textures in VidMem and MLR Shapes
		numOverlays = 0;
		overlays = nullptr;
		numDetails = 0;
		details = nullptr;
		//--------------------------------------------------------
		// Stores Transition Texture in VidMem.
		numTransitions = 0;
		transitions = nullptr;
		nextTransition = 0;
		//------------------------------------------------------
		// Used to convert maps to new formats when textures change.
		firstOverlay = 0;
		//---------------------------------------------------------------------------------
		//Checks for list o magic textures and then checks that each magic texture exists.
		quickLoad = false;
		localBaseName = nullptr;
		listTransitionFile = nullptr;
	}

	TerrainTextures(void)
	{
		init(void);
	}

	void destroy(void);

	~TerrainTextures(void)
	{
		destroy(void);
	}

	int32_t init(PSTR fileName, PSTR baseName);

	int32_t setTexture(uint32_t typeInfo, uint32_t overlayInfo);

	int32_t setDetail(uint32_t typeInfo, uint32_t frameNum);

	float getDetailTilingFactor(int32_t typeInfo)
	{
		if(typeInfo < numDetails)
			return details[typeInfo].tilingFactor;
		return 64.0f;
	}

	int32_t getNumTypes(void) const
	{
		return numTypes;
	}

	int32_t getTextureNameID(int32_t id) const
	{
		return types[id].nameId;
	}

	float getDetailFrameRate(int32_t typeInfo)
	{
		if((typeInfo < numDetails) && details[typeInfo].frameRate > Stuff::SMALL)
			return (1.0f / details[typeInfo].frameRate);
		return 0.066666667f;		//15 FPS
	}

	int32_t setOverlay(uint32_t overlayInfo);
	int32_t getOverlayHandle(Overlays id, int32_t Offset);
	void getOverlayInfoFromHandle(int32_t handle, Overlays& id, uint32_t& Offset);

	void purgeTransitions(void);

	uint32_t getTextureTypeRGB(int32_t typeInfo)
	{
		gosASSERT(typeInfo < numTypes);
		return types[typeInfo].terrainMapRGB;
	}

	uint32_t getOverlayTypeRGB(int32_t overlayNum)
	{
		gosASSERT(overlayNum < numOverlays);
		return overlays[overlayNum].terrainMapRGB;
	}

	int32_t getTexturePriority(int32_t typeInfo)
	{
		gosASSERT(typeInfo < numTypes);
		return types[typeInfo].terrainPriority;
	}

	uint32_t getTexture(uint32_t texture)
	{
		if((int32_t)texture >= nextAvailable)
			return nullptr;
		if(textures[texture].mcTextureNodeIndex == 0xffffffff)
			return nullptr;
		return (mcTextureManager->get_gosTextureHandle(textures[texture].mcTextureNodeIndex));
	}

	uint32_t getTextureHandle(uint32_t texture)
	{
		if((int32_t)texture >= nextAvailable)
			return 0xffffffff;
		mcTextureManager->get_gosTextureHandle(textures[texture].mcTextureNodeIndex);
		return (textures[texture].mcTextureNodeIndex);
	}

	uint32_t getDetail(uint32_t dTexture)
	{
		if((int32_t)dTexture >= nextAvailable)
			return nullptr;
		if(textures[dTexture].mcTextureNodeIndex == 0xffffffff)
			return nullptr;
		return (mcTextureManager->get_gosTextureHandle(textures[dTexture].mcTextureNodeIndex));
	}

	uint32_t getDetailHandle(uint32_t dTexture)
	{
		if((int32_t)dTexture >= nextAvailable)
			return nullptr;
		mcTextureManager->get_gosTextureHandle(textures[dTexture].mcTextureNodeIndex);
		return (textures[dTexture].mcTextureNodeIndex);
	}

	void setMipLevel(int32_t mipLevel)
	{
		if((mipLevel >= 0) && (mipLevel < MC_MAX_MIP_LEVELS))
			globalMipLevel = mipLevel;
	}

	int32_t getFirstOverlay(void)
	{
		return firstOverlay;
	}

	bool isCement(uint32_t typeInfo)
	{
		return (textures[typeInfo].flags & MC2_TERRAIN_CEMENT_FLAG) == MC2_TERRAIN_CEMENT_FLAG;
	}

	bool isAlpha(uint32_t typeInfo)
	{
		return (textures[typeInfo].flags & MC2_TERRAIN_ALPHA_FLAG) == MC2_TERRAIN_ALPHA_FLAG;
	}

	static void initializeStatistics(void);

	void update(void);
};

//---------------------------------------------------------------------------
#endif
