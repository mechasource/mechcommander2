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
// Include Files
#ifndef DTERRTXM_H
#include "dterrtxm.h"
#endif

#ifndef MAPDATA_H
#include "mapdata.h"
#endif

#ifndef TXMMGR_H
#include "txmmgr.h"
#endif

//---------------------------------------------------------------------------
// Macro Definitions
#ifndef NO_ERROR
#define NO_ERROR		0
#endif

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
	ULONG			flags;						//Tells me if this texture is a cement texture, has alpha, etc.
	ULONG			mcTextureNodeIndex;			//Pointer to MCTextureNode which is used to cache handles if necessary
};

struct MC_TerrainType
{
	int32_t			terrainId;
	char			*terrainName;
	char			*maskName;
	int32_t			terrainPriority;
	PUCHAR		*textureData;
	PUCHAR		*maskData;
	int32_t			baseTXMIndex;				//Index of Highest MIP Level of Texture.
	ULONG	terrainMapRGB;				//Rgb Color used for TacMap.
	int32_t			nameId;
};

struct MC_DetailType
{
	int32_t			detailId;
	char			*detailName;
	PUCHAR		*detailData;
	int32_t			numDetails;
	float			frameRate;
	float			tilingFactor;		//How often does texture repeat
	int32_t			baseTXMIndex;
};

struct MC_OverlayType
{
	int32_t			overlayId;
	char			*overlayName;
	int32_t			numTextures;
	int32_t			oldOverlayId;
	bool			isMLRAppearance;
	PUCHAR		*overlayData;				
	int32_t			baseTXMIndex;
	ULONG	terrainMapRGB;
};

struct TransitionType
{
	ULONG 			baseTXMIndex;				//When we build transition, where did we stick it
	ULONG 			transitionIndex;			//Code/Hash/Whatever to uniquely ID this transition.
	ULONG			overlayIndex;				//Overlays blended onto texture now.
};

//---------------------------------------------------------------------------
class TerrainTextures
{
	//Data Members
	//-------------
	protected:

		static int32_t			numTxms;
		TerrainTXM			*textures;
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
		char				*localBaseName;
		File				*listTransitionFile;	

		static bool			terrainTexturesInstrumented;

	public:

	//Member Functions
	//-----------------
	protected:

		int32_t initTexture (int32_t typeNum);
		void initMask (int32_t typeNum);
		int32_t initDetail (int32_t typeNum, int32_t detailNum);
		int32_t initOverlay (int32_t overlayNum, int32_t txmNum, PSTR txmName);

		int32_t loadTextureMemory (PSTR textureName, int32_t mipSize);
		int32_t loadOverlayMemory (PSTR overlayName);
		int32_t textureFromMemory (PUCHAR ourRam, int32_t mipSize);
		int32_t loadDetailMemory  (PSTR detailName, int32_t mipSize);
		int32_t textureFromMemoryAlpha (PUCHAR ourRam, int32_t mipSize);

		int32_t createTransition (ULONG typeInfo, ULONG overlayInfo = 0);
		void combineTxm (PUCHAR dest, ULONG binNumber, int32_t type, int32_t mipLevel);
		void combineOverlayTxm (PUCHAR dest, int32_t type, int32_t mipLevel);

	public:
	
		void init (void)
		{
			tileHeap = NULL;
			tileRAMHeap = NULL;

			//----------------------------------------------
			// Stores composited and raw textures in VidMem
			numTxms = MC_MAX_TERRAIN_TXMS;
			nextAvailable = 0;
			textures = NULL;
			firstTransition = -1;

			//--------------------------------------------------
			// Stores Base Terrain Textures and Masks in SysMem
			numTypes = 0;
			types = NULL;

			//----------------------------------------------------------
			// Stores Overlay Terrain Textures in VidMem and MLR Shapes
			numOverlays = 0;
			overlays = NULL;

			numDetails = 0;
			details = NULL;
			
			//--------------------------------------------------------
			// Stores Transition Texture in VidMem.
			numTransitions = 0;
			transitions = NULL;
			nextTransition = 0;
			
			//------------------------------------------------------
			// Used to convert maps to new formats when textures change.
			firstOverlay = 0;

			//---------------------------------------------------------------------------------
			//Checks for list o magic textures and then checks that each magic texture exists. 
			quickLoad = false;
			localBaseName = NULL;
			listTransitionFile = NULL;
		}

		TerrainTextures (void)
		{
			init();
		}

		void destroy (void);

		~TerrainTextures (void)
		{
			destroy();
		}

		int32_t init (PSTR fileName, PSTR baseName);
		
		int32_t setTexture (ULONG typeInfo, ULONG overlayInfo);
		
		int32_t setDetail (ULONG typeInfo, ULONG frameNum);
		
		float getDetailTilingFactor (int32_t typeInfo)
		{
			if (typeInfo < numDetails)
				return details[typeInfo].tilingFactor;
				
			return 64.0f;
		}

		int32_t getNumTypes() const { return numTypes; }

		int32_t getTextureNameID( int32_t id ) const { return types[id].nameId; }
		
		float getDetailFrameRate (int32_t typeInfo)
		{
			if ((typeInfo < numDetails) && details[typeInfo].frameRate > Stuff::SMALL)
				return (1.0f / details[typeInfo].frameRate);
				
			return 0.066666667f;		//15 FPS
		}

		int32_t setOverlay (ULONG overlayInfo);
		int32_t getOverlayHandle( Overlays id, int Offset );
		void getOverlayInfoFromHandle( int32_t handle, Overlays& id, ULONG& Offset );

		void purgeTransitions (void);
		
		ULONG getTextureTypeRGB (int32_t typeInfo)
		{
			gosASSERT(typeInfo < numTypes);
			return types[typeInfo].terrainMapRGB;
		}

		ULONG getOverlayTypeRGB (int32_t overlayNum)
		{
			gosASSERT(overlayNum < numOverlays);
			return overlays[overlayNum].terrainMapRGB;
		}

		int32_t getTexturePriority (int32_t typeInfo)
		{
			gosASSERT(typeInfo < numTypes);
			return types[typeInfo].terrainPriority;
		}

		ULONG getTexture (ULONG texture)
		{
			if ((int32_t)texture >= nextAvailable) 
				return NULL;

			if ( textures[texture].mcTextureNodeIndex == 0xffffffff )
				return NULL;

			return (mcTextureManager->get_gosTextureHandle(textures[texture].mcTextureNodeIndex));
		}

		ULONG getTextureHandle (ULONG texture)
		{
			if ((int32_t)texture >= nextAvailable) 
				return 0xffffffff;

			mcTextureManager->get_gosTextureHandle(textures[texture].mcTextureNodeIndex);
			return (textures[texture].mcTextureNodeIndex);
		}

		ULONG getDetail (ULONG dTexture)
		{
			if ((int32_t)dTexture >= nextAvailable) 
				return NULL;

			if ( textures[dTexture].mcTextureNodeIndex == 0xffffffff )
				return NULL;

			return (mcTextureManager->get_gosTextureHandle(textures[dTexture].mcTextureNodeIndex));
		}

		ULONG getDetailHandle (ULONG dTexture)
		{
			if ((int32_t)dTexture >= nextAvailable) 
				return NULL;

			mcTextureManager->get_gosTextureHandle(textures[dTexture].mcTextureNodeIndex);
			return (textures[dTexture].mcTextureNodeIndex);
		}

		void setMipLevel (int32_t mipLevel)
		{
			if ((mipLevel >= 0) && (mipLevel < MC_MAX_MIP_LEVELS))
				globalMipLevel = mipLevel;
		}
		
		int32_t getFirstOverlay (void)
		{
			return firstOverlay;
		}
		
		bool isCement (ULONG typeInfo)
		{
			return (textures[typeInfo].flags & MC2_TERRAIN_CEMENT_FLAG) == MC2_TERRAIN_CEMENT_FLAG;
		}
		
		bool isAlpha (ULONG typeInfo)
		{
			return (textures[typeInfo].flags & MC2_TERRAIN_ALPHA_FLAG) == MC2_TERRAIN_ALPHA_FLAG;
		}

		static void initializeStatistics();
		
		void update (void);
};

//---------------------------------------------------------------------------
#endif
