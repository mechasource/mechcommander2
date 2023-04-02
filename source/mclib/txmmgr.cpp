//---------------------------------------------------------------------
//
//
// This class will manage the texture memory provided by GOS
// GOS gives me a maximum of 256 256x256 pixel texture pages.
// I want GOS to think I only use 256x256 textures.  This class
// will insure that GOS believes that completely and provided
// smaller texture surfaces out of the main surface if necessary
// as well as returning the necessary UVs to get to the other surface.
//
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//
#include "stdinc.h"

#include "txmmgr.h"
#include "tgainfo.h"
#include "file.h"
#include "timing.h"
#include "camera.h"
#include "lz.h"
#include "cident.h"
#include "paths.h"

//#include "gameos.hpp"
#include "mlr/mlr.h"
#include "gosfx/gosfxheaders.h"

//---------------------------------------------------------------------------
// static globals
MC_TextureManager* mcTextureManager = nullptr;
gos_VERTEXManager* MC_TextureManager::gvManager = nullptr;
uint8_t* MC_TextureManager::lzBuffer1 = nullptr;
uint8_t* MC_TextureManager::lzBuffer2 = nullptr;
int32_t MC_TextureManager::iBufferRefCount = 0;

bool MLRVertexLimitReached = false;
extern bool useFog;

uint32_t actualTextureSize = 0;
uint32_t compressedTextureSize = 0;

#define MAX_SENDDOWN 10002

//------------------------------------------------------
// Frees up gos_VERTEX manager memory
void
MC_TextureManager::freeVertices(void)
{
	if (gvManager)
	{
		gvManager->destroy();
		delete gvManager;
		gvManager = nullptr;
	}
}

//------------------------------------------------------
// Creates gos_VERTEX Manager and allocates RAM.  Will not allocate if already
// done!
void
MC_TextureManager::startVertices(int32_t maxVertices)
{
	if (gvManager == nullptr)
	{
		gvManager = new gos_VERTEXManager;
		gvManager->init(maxVertices);
		gvManager->reset();
	}
}

//----------------------------------------------------------------------
// Class MC_TextureManager
void
MC_TextureManager::start(void)
{
	init();
	//------------------------------------------
	// Create nodes from systemHeap.
	int32_t nodeRAM = MC_MAXTEXTURES * sizeof(MC_TextureNode);
	masterTextureNodes = (MC_TextureNode*)systemHeap->Malloc(nodeRAM);
	gosASSERT(masterTextureNodes != nullptr);
	int32_t i;
	for (i = 0; i < MC_MAXTEXTURES; i++)
		masterTextureNodes[i].init();
	//-------------------------------------------
	// Create VertexNodes from systemHeap
	nodeRAM = MC_MAXTEXTURES * sizeof(MC_VertexArrayNode);
	masterVertexNodes = (MC_VertexArrayNode*)systemHeap->Malloc(nodeRAM);
	gosASSERT(masterVertexNodes != nullptr);
	memset(masterVertexNodes, 0, nodeRAM);
	textureCacheHeap = new UserHeap;
	textureCacheHeap->init(TEXTURE_CACHE_SIZE, "TXMCache");
	textureCacheHeap->setMallocFatals(false);
	textureStringHeap = new UserHeap;
	textureStringHeap->init(512000, "TXMString");
	if (!textureManagerInstrumented)
	{
		StatisticFormat("");
		StatisticFormat("MechCommander 2 Texture Manager");
		StatisticFormat("===============================");
		StatisticFormat("");
		AddStatistic("Handles Used", "Handles", gos_DWORD, &(currentUsedTextures), Stat_Total);
		AddStatistic("Cache Misses", "", gos_DWORD, &(totalCacheMisses), Stat_Total);
		StatisticFormat("");
		StatisticFormat("");
		textureManagerInstrumented = true;
	}
	indexArray = (uint16_t*)systemHeap->Malloc(sizeof(uint16_t) * MC_MAXFACES);
	for (i = 0; i < MC_MAXFACES; i++)
		indexArray[i] = i;
	// Add an Empty Texture node for all untextured triangles to go down into.
	masterTextureNodes[0].m_texturehandle = 0;
	masterTextureNodes[0].m_nodename = nullptr;
	masterTextureNodes[0].m_uniqueinstance = false;
	masterTextureNodes[0].m_neverflush = 0x1;
	masterTextureNodes[0].m_numusers = 0;
	masterTextureNodes[0].m_key = gos_Texture_Solid;
	masterTextureNodes[0].m_hints = 0;
	masterTextureNodes[0].m_width = 0;
	masterTextureNodes[0].m_lastused = -1;
	masterTextureNodes[0].m_texturedata = nullptr;
}

extern std::iostream effectStream;
extern MidLevelRenderer::MLRClipper* theClipper;
//----------------------------------------------------------------------
void
MC_TextureManager::destroy(void)
{
	if (masterTextureNodes)
	{
		//-----------------------------------------------------
		// Traverses list of texture nodes and frees each one.
		int32_t usedCount = 0;
		for (size_t i = 0; i < MC_MAXTEXTURES; i++)
			masterTextureNodes[i].destroy(); // Destroy for nodes whacks GOS handle
		currentUsedTextures = usedCount; // Can this have been the damned bug all along!?
	}
	gos_PushCurrentHeap(MidLevelRenderer::Heap);
	delete MidLevelRenderer::MLRTexturePool::Instance;
	MidLevelRenderer::MLRTexturePool::Instance = nullptr;
	delete theClipper;
	theClipper = nullptr;
#ifdef _GAMEOS_HPP_
	gos_PopCurrentHeap();
#endif

	//------------------------------------------------------
	// Shutdown the GOS FX and MLR.
	gos_PushCurrentHeap(gosFX::Heap);
	delete gosFX::EffectLibrary::Instance;
	gosFX::EffectLibrary::Instance = nullptr;
	delete effectStream;
	effectStream = nullptr;
	delete gosFX::LightManager::Instance;
	gosFX::LightManager::Instance = nullptr;
#ifdef _GAMEOS_HPP_
	gos_PopCurrentHeap();
#endif

	//------------------------------------------
	// free SystemHeap Memory
	systemHeap->Free(masterTextureNodes);
	masterTextureNodes = nullptr;
	systemHeap->Free(masterVertexNodes);
	masterVertexNodes = nullptr;
	delete textureCacheHeap;
	textureCacheHeap = nullptr;
	delete textureStringHeap;
	textureStringHeap = nullptr;
}

//----------------------------------------------------------------------
MC_TextureManager::~MC_TextureManager(void)
{
	MC_TextureManager::iBufferRefCount--;
	if (0 == MC_TextureManager::iBufferRefCount)
	{
		if (lzBuffer1)
		{
			gosASSERT(lzBuffer2 != nullptr);
			if (textureCacheHeap)
			{
				textureCacheHeap->Free(lzBuffer1);
				textureCacheHeap->Free(lzBuffer2);
			}
			lzBuffer1 = nullptr;
			lzBuffer2 = nullptr;
		}
	}
	destroy();
}

//----------------------------------------------------------------------
void
MC_TextureManager::flush(bool justTextures)
{
	if (masterTextureNodes)
	{
		//-----------------------------------------------------
		// Traverses list of texture nodes and frees each one.
		int32_t usedCount = 0;
		for (size_t i = 0; i < MC_MAXTEXTURES; i++)
		{
			if (!masterTextureNodes[i].m_neverflush)
				masterTextureNodes[i].destroy(); // Destroy for nodes whacks GOS handle
		}
		currentUsedTextures = usedCount; // Can this have been the damned bug all along!?
	}
	// If we just wanted to free up RAM, just return and let the MUNGA stuff go
	// later.
	if (justTextures)
		return;
	gos_PushCurrentHeap(MidLevelRenderer::Heap);
	delete MidLevelRenderer::MLRTexturePool::Instance;
	MidLevelRenderer::MLRTexturePool::Instance = nullptr;
	delete theClipper;
	theClipper = nullptr;
#ifdef _GAMEOS_HPP_
	gos_PopCurrentHeap();
#endif

	//------------------------------------------------------
	// Shutdown the GOS FX and MLR.
	gos_PushCurrentHeap(gosFX::Heap);
	delete gosFX::EffectLibrary::Instance;
	gosFX::EffectLibrary::Instance = nullptr;
	delete effectStream;
	effectStream = nullptr;
	delete gosFX::LightManager::Instance;
	gosFX::LightManager::Instance = nullptr;
#ifdef _GAMEOS_HPP_
	gos_PopCurrentHeap();
#endif

	//------------------------------------------------------
	// Restart MLR and the GOSFx
	gos_PushCurrentHeap(MidLevelRenderer::Heap);
	MidLevelRenderer::TGAFilePool* pool = new MidLevelRenderer::TGAFilePool("data\\tgl\\128\\");
	MidLevelRenderer::MLRTexturePool::Instance = new MidLevelRenderer::MLRTexturePool(pool);
	MidLevelRenderer::MLRSortByOrder* cameraSorter =
		new MidLevelRenderer::MLRSortByOrder(MidLevelRenderer::MLRTexturePool::Instance);
	theClipper = new MidLevelRenderer::MLRClipper(0, cameraSorter);
#ifdef _GAMEOS_HPP_
	gos_PopCurrentHeap();
#endif

	//------------------------------------------------------
	// ReStart the GOS FX.
	gos_PushCurrentHeap(gosFX::Heap);
	gosFX::EffectLibrary::Instance = new gosFX::EffectLibrary();
	Check_Object(gosFX::EffectLibrary::Instance);
	FullPathFileName effectsName;
	effectsName.init(effectsPath, "mc2.fx", "");
	MechFile effectFile;
	int32_t result = effectFile.open(effectsName);
	if (result != NO_ERROR)
		STOP(("Could not find MC2.fx"));
	int32_t effectsSize = effectFile.fileSize();
	uint8_t* effectsData = (uint8_t*)systemHeap->Malloc(effectsSize);
	effectFile.read(effectsData, effectsSize);
	effectFile.close();
	effectStream = new std::iostream(effectsData, effectsSize);
	gosFX::EffectLibrary::Instance->Load(effectStream);
	gosFX::LightManager::Instance = new gosFX::LightManager();
#ifdef _GAMEOS_HPP_
	gos_PopCurrentHeap();
#endif

	systemHeap->Free(effectsData);
}

//----------------------------------------------------------------------
void
MC_TextureManager::removeTextureNode(uint32_t textureNode)
{
	if (textureNode != 0xffffffff)
	{
		//-----------------------------------------------------------
		masterTextureNodes[textureNode].destroy();
		if (masterTextureNodes[textureNode].m_texturedata)
		{
			textureCacheHeap->Free(masterTextureNodes[textureNode].m_texturedata);
			masterTextureNodes[textureNode].m_texturedata = nullptr;
			if (masterTextureNodes[textureNode].m_nodename)
			{
				textureStringHeap->Free(masterTextureNodes[textureNode].m_nodename);
				masterTextureNodes[textureNode].m_nodename = nullptr;
			}
		}
	}
}

//----------------------------------------------------------------------
void
MC_TextureManager::removeTexture(uint32_t gosHandle)
{
	int32_t i;
	//-----------------------------------------------------------
	for (i = 0; i < MC_MAXTEXTURES; i++)
	{
		if ((masterTextureNodes[i].m_texturehandle == gosHandle))
		{
			masterTextureNodes[i].m_numusers--;
			break;
		}
	}
	if (i < MC_MAXTEXTURES && masterTextureNodes[i].m_numusers == 0)
	{
		masterTextureNodes[i].destroy();
		if (masterTextureNodes[i].m_texturedata)
		{
			textureCacheHeap->Free(masterTextureNodes[i].m_texturedata);
			masterTextureNodes[i].m_texturedata = nullptr;
			if (masterTextureNodes[i].m_nodename)
			{
				textureStringHeap->Free(masterTextureNodes[i].m_nodename);
				masterTextureNodes[i].m_nodename = nullptr;
			}
		}
	}
}

#define cache_Threshold 150
//----------------------------------------------------------------------
bool
MC_TextureManager::flushCache(void)
{
	bool cacheNotFull = false;
	totalCacheMisses++;
	currentUsedTextures = 0;
	int32_t i;
	// Count ACTUAL number of textures being used.
	// ALSO can't count on turn being right.  Logistics does not update unless
	// simple Camera is up!!
	for (i = 0; i < MC_MAXTEXTURES; i++)
	{
		if ((masterTextureNodes[i].m_texturehandle != CACHED_OUT_HANDLE) && (masterTextureNodes[i].m_texturehandle != 0xffffffff))
		{
			currentUsedTextures++;
		}
	}
	// If we are now below the magic number, return that the cache is NOT full.
	if (currentUsedTextures < MAX_MC2_GOS_TEXTURES)
		return true;
	for (i = 0; i < MC_MAXTEXTURES; i++)
	{
		if ((masterTextureNodes[i].m_texturehandle != CACHED_OUT_HANDLE) && (masterTextureNodes[i].m_texturehandle != 0xffffffff) && (!masterTextureNodes[i].m_uniqueinstance))
		{
			if (masterTextureNodes[i].m_lastused <= (turn - cache_Threshold))
			{
				//----------------------------------------------------------------
				// Cache this badboy out.  Textures don't change.  Just Destroy!
				if (masterTextureNodes[i].m_texturehandle)
					gos_DestroyTexture(masterTextureNodes[i].m_texturehandle);
				masterTextureNodes[i].m_texturehandle = CACHED_OUT_HANDLE;
				currentUsedTextures--;
				cacheNotFull = true;
				return cacheNotFull;
			}
		}
	}
	for (i = 0; i < MC_MAXTEXTURES; i++)
	{
		if ((masterTextureNodes[i].m_texturehandle != CACHED_OUT_HANDLE) && (masterTextureNodes[i].m_texturehandle != 0xffffffff) && (masterTextureNodes[i].m_texturehandle) && (!masterTextureNodes[i].m_uniqueinstance))
		{
			if (masterTextureNodes[i].m_lastused <= (turn - 30))
			{
				//----------------------------------------------------------------
				// Cache this badboy out.  Textures don't change.  Just Destroy!
				if (masterTextureNodes[i].m_texturehandle)
					gos_DestroyTexture(masterTextureNodes[i].m_texturehandle);
				masterTextureNodes[i].m_texturehandle = CACHED_OUT_HANDLE;
				currentUsedTextures--;
				cacheNotFull = true;
				return cacheNotFull;
			}
		}
	}
	for (i = 0; i < MC_MAXTEXTURES; i++)
	{
		if ((masterTextureNodes[i].m_texturehandle != CACHED_OUT_HANDLE) && (masterTextureNodes[i].m_texturehandle != 0xffffffff) && (!masterTextureNodes[i].m_uniqueinstance))
		{
			if (masterTextureNodes[i].m_lastused <= (turn - 1))
			{
				//----------------------------------------------------------------
				// Cache this badboy out.  Textures don't change.  Just Destroy!
				if (masterTextureNodes[i].m_texturehandle)
					gos_DestroyTexture(masterTextureNodes[i].m_texturehandle);
				masterTextureNodes[i].m_texturehandle = CACHED_OUT_HANDLE;
				currentUsedTextures--;
				cacheNotFull = true;
				return cacheNotFull;
			}
		}
	}
	// gosASSERT(cacheNotFull);
	return cacheNotFull;
}

//----------------------------------------------------------------------
// Draws all textures with isTerrain set that are solid first,
// then draws all alpha with isTerrain set.
void
MC_TextureManager::renderLists(void)
{
	if (Environment.Renderer == 3)
	{
		gos_SetRenderState(gos_State_AlphaMode, gos_Alpha_OneZero);
		gos_SetRenderState(gos_State_ShadeMode, gos_ShadeFlat);
		gos_SetRenderState(gos_State_MonoEnable, 1);
		gos_SetRenderState(gos_State_Perspective, 0);
		gos_SetRenderState(gos_State_Clipping, 1);
		gos_SetRenderState(gos_State_AlphaTest, 0);
		gos_SetRenderState(gos_State_Specular, 0);
		gos_SetRenderState(gos_State_Dither, 0);
		gos_SetRenderState(gos_State_TextureMapBlend, gos_BlendDecal);
		gos_SetRenderState(gos_State_Filter, gos_FilterNone);
		gos_SetRenderState(gos_State_TextureAddress, gos_TextureClamp);
		gos_SetRenderState(gos_State_ZCompare, 1);
		gos_SetRenderState(gos_State_ZWrite, 1);
	}
	else
	{
		gos_SetRenderState(gos_State_AlphaMode, gos_Alpha_OneZero);
		gos_SetRenderState(gos_State_ShadeMode, gos_ShadeGouraud);
		gos_SetRenderState(gos_State_MonoEnable, 0);
		gos_SetRenderState(gos_State_Perspective, 1);
		gos_SetRenderState(gos_State_Clipping, 1);
		gos_SetRenderState(gos_State_AlphaTest, 0);
		gos_SetRenderState(gos_State_Specular, 1);
		gos_SetRenderState(gos_State_Dither, 1);
		gos_SetRenderState(gos_State_TextureMapBlend, gos_BlendModulate);
		gos_SetRenderState(gos_State_Filter, gos_FilterBiLinear);
		gos_SetRenderState(gos_State_TextureAddress, gos_TextureClamp);
		gos_SetRenderState(gos_State_ZCompare, 1);
		gos_SetRenderState(gos_State_ZWrite, 1);
	}
	uint32_t fogcolour = eye->fogcolour;
	//-----------------------------------------------------
	// FOG time.  Set Render state to FOG on!
	if (useFog)
	{
		gos_SetRenderState(gos_State_Fog, (int32_t)&fogcolour);
	}
	else
	{
		gos_SetRenderState(gos_State_Fog, 0);
	}
	int32_t i;
	for (i = 0; i < nextAvailableVertexNode; i++)
	{
		if ((masterVertexNodes[i].flags & MC2_DRAWSOLID) && (masterVertexNodes[i].vertices))
		{
			if (masterVertexNodes[i].flags & MC2_ISTERRAIN)
				gos_SetRenderState(gos_State_TextureAddress, gos_TextureClamp);
			else
				gos_SetRenderState(gos_State_TextureAddress, gos_TextureWrap);
			uint32_t totalvertices = masterVertexNodes[i].numvertices;
			if (masterVertexNodes[i].currentvertex != (masterVertexNodes[i].vertices + masterVertexNodes[i].numvertices))
			{
				totalvertices = masterVertexNodes[i].currentvertex - masterVertexNodes[i].vertices;
			}
			if (totalvertices && (totalvertices < MAX_SENDDOWN))
			{
				gos_SetRenderState(gos_State_Texture, masterTextureNodes[masterVertexNodes[i].textureindex].get_gosTextureHandle());
				gos_RenderIndexedArray(masterVertexNodes[i].vertices, totalvertices, indexArray, totalvertices);
			}
			else if (totalvertices > MAX_SENDDOWN)
			{
				gos_SetRenderState(gos_State_Texture, masterTextureNodes[masterVertexNodes[i].textureindex].get_gosTextureHandle());
				// Must divide up vertices into batches of 10,000 each to send
				// down.
				// Somewhere around 20000 to 30000 it really gets screwy!!!
				int32_t currentVertices = 0;
				while (currentVertices < totalvertices)
				{
					gos_VERTEX* v = masterVertexNodes[i].vertices + currentVertices;
					int32_t tVertices = totalvertices - currentVertices;
					if (tVertices > MAX_SENDDOWN)
						tVertices = MAX_SENDDOWN;
					gos_RenderIndexedArray(v, tVertices, indexArray, tVertices);
					currentVertices += tVertices;
				}
			}
			// Reset the list to zero length to avoid drawing more then once!
			// Also comes in handy if gameLogic is not called.
			masterVertexNodes[i].currentvertex = masterVertexNodes[i].vertices;
		}
	}
	if (Environment.Renderer == 3)
	{
		// Do NOT draw the water as transparent in software
		gos_SetRenderState(gos_State_TextureAddress, gos_TextureWrap);
	}
	else
	{
		gos_SetRenderState(gos_State_AlphaMode, gos_Alpha_AlphaInvAlpha);
		gos_SetRenderState(gos_State_AlphaTest, 1);
		gos_SetRenderState(gos_State_TextureAddress, gos_TextureWrap);
	}
	for (i = 0; i < nextAvailableVertexNode; i++)
	{
		if ((masterVertexNodes[i].flags & MC2_ISTERRAIN) && (masterVertexNodes[i].flags & MC2_DRAWALPHA) && !(masterVertexNodes[i].flags & MC2_ISCRATERS) && (masterVertexNodes[i].vertices))
		{
			uint32_t totalvertices = masterVertexNodes[i].numvertices;
			if (masterVertexNodes[i].currentvertex != (masterVertexNodes[i].vertices + masterVertexNodes[i].numvertices))
			{
				totalvertices = masterVertexNodes[i].currentvertex - masterVertexNodes[i].vertices;
			}
			if (totalvertices && (totalvertices < MAX_SENDDOWN))
			{
				gos_SetRenderState(gos_State_Texture,
					masterTextureNodes[masterVertexNodes[i].textureindex].get_gosTextureHandle());
				gos_RenderIndexedArray(
					masterVertexNodes[i].vertices, totalvertices, indexArray, totalvertices);
			}
			else if (totalvertices > MAX_SENDDOWN)
			{
				gos_SetRenderState(gos_State_Texture,
					masterTextureNodes[masterVertexNodes[i].textureindex].get_gosTextureHandle());
				// Must divide up vertices into batches of 10,000 each to send
				// down.
				// Somewhere around 20000 to 30000 it really gets screwy!!!
				int32_t currentVertices = 0;
				while (currentVertices < totalvertices)
				{
					gos_VERTEX* v = masterVertexNodes[i].vertices + currentVertices;
					int32_t tVertices = totalvertices - currentVertices;
					if (tVertices > MAX_SENDDOWN)
						tVertices = MAX_SENDDOWN;
					gos_RenderIndexedArray(v, tVertices, indexArray, tVertices);
					currentVertices += tVertices;
				}
			}
			// Reset the list to zero length to avoid drawing more then once!
			// Also comes in handy if gameLogic is not called.
			masterVertexNodes[i].currentvertex = masterVertexNodes[i].vertices;
		}
	}
	if (Environment.Renderer == 3)
	{
		gos_SetRenderState(gos_State_AlphaMode, gos_Alpha_AlphaInvAlpha);
		gos_SetRenderState(gos_State_AlphaTest, 1);
		gos_SetRenderState(gos_State_ShadeMode, gos_ShadeFlat);
	}
	gos_SetRenderState(gos_State_TextureAddress, gos_TextureClamp);
	gos_SetRenderState(gos_State_ZWrite, 0);
	gos_SetRenderState(gos_State_ShadeMode, gos_ShadeFlat);
	// Draw the Craters after the detail textures on the terrain.  There should
	// never be anything here in the OLD universe.
	// DO NOT draw craters or footprints in software
	if (Environment.Renderer != 3)
	{
		for (i = 0; i < nextAvailableVertexNode; i++)
		{
			if (!(masterVertexNodes[i].flags & MC2_ISTERRAIN) && (masterVertexNodes[i].flags & MC2_DRAWALPHA) && (masterVertexNodes[i].flags & MC2_ISCRATERS) && (masterVertexNodes[i].vertices))
			{
				uint32_t totalvertices = masterVertexNodes[i].numvertices;
				if (masterVertexNodes[i].currentvertex != (masterVertexNodes[i].vertices + masterVertexNodes[i].numvertices))
				{
					totalvertices =
						masterVertexNodes[i].currentvertex - masterVertexNodes[i].vertices;
				}
				if (totalvertices && (totalvertices < MAX_SENDDOWN))
				{
					gos_SetRenderState(gos_State_Texture,
						masterTextureNodes[masterVertexNodes[i].textureindex]
							.get_gosTextureHandle());
					gos_RenderIndexedArray(
						masterVertexNodes[i].vertices, totalvertices, indexArray, totalvertices);
				}
				else if (totalvertices > MAX_SENDDOWN)
				{
					gos_SetRenderState(gos_State_Texture,
						masterTextureNodes[masterVertexNodes[i].textureindex]
							.get_gosTextureHandle());
					// Must divide up vertices into batches of 10,000 each to
					// send down.
					// Somewhere around 20000 to 30000 it really gets screwy!!!
					int32_t currentVertices = 0;
					while (currentVertices < totalvertices)
					{
						gos_VERTEX* v = masterVertexNodes[i].vertices + currentVertices;
						int32_t tVertices = totalvertices - currentVertices;
						if (tVertices > MAX_SENDDOWN)
							tVertices = MAX_SENDDOWN;
						gos_RenderIndexedArray(v, tVertices, indexArray, tVertices);
						currentVertices += tVertices;
					}
				}
				// Reset the list to zero length to avoid drawing more then
				// once!  Also comes in handy if gameLogic is not called.
				masterVertexNodes[i].currentvertex = masterVertexNodes[i].vertices;
			}
		}
	}
	if (Environment.Renderer != 3)
	{
		gos_SetRenderState(gos_State_ShadeMode, gos_ShadeGouraud);
		gos_SetRenderState(gos_State_ZWrite, 1);
	}
	// Draw the Overlays after the detail textures on the terrain.  There should
	// never be anything here in the OLD universe.
	for (i = 0; i < nextAvailableVertexNode; i++)
	{
		if ((masterVertexNodes[i].flags & MC2_ISTERRAIN) && (masterVertexNodes[i].flags & MC2_DRAWALPHA) && (masterVertexNodes[i].flags & MC2_ISCRATERS) && (masterVertexNodes[i].vertices))
		{
			uint32_t totalvertices = masterVertexNodes[i].numvertices;
			if (masterVertexNodes[i].currentvertex != (masterVertexNodes[i].vertices + masterVertexNodes[i].numvertices))
			{
				totalvertices = masterVertexNodes[i].currentvertex - masterVertexNodes[i].vertices;
			}
			if (totalvertices && (totalvertices < MAX_SENDDOWN))
			{
				gos_SetRenderState(gos_State_Texture,
					masterTextureNodes[masterVertexNodes[i].textureindex].get_gosTextureHandle());
				gos_RenderIndexedArray(
					masterVertexNodes[i].vertices, totalvertices, indexArray, totalvertices);
			}
			else if (totalvertices > MAX_SENDDOWN)
			{
				gos_SetRenderState(gos_State_Texture,
					masterTextureNodes[masterVertexNodes[i].textureindex].get_gosTextureHandle());
				// Must divide up vertices into batches of 10,000 each to send
				// down.
				// Somewhere around 20000 to 30000 it really gets screwy!!!
				int32_t currentVertices = 0;
				while (currentVertices < totalvertices)
				{
					gos_VERTEX* v = masterVertexNodes[i].vertices + currentVertices;
					int32_t tVertices = totalvertices - currentVertices;
					if (tVertices > MAX_SENDDOWN)
						tVertices = MAX_SENDDOWN;
					gos_RenderIndexedArray(v, tVertices, indexArray, tVertices);
					currentVertices += tVertices;
				}
			}
			// Reset the list to zero length to avoid drawing more then once!
			// Also comes in handy if gameLogic is not called.
			masterVertexNodes[i].currentvertex = masterVertexNodes[i].vertices;
		}
	}
	if (Environment.Renderer == 3)
	{
		gos_SetRenderState(gos_State_TextureAddress, gos_TextureWrap);
		gos_SetRenderState(gos_State_ShadeMode, gos_ShadeFlat);
		gos_SetRenderState(gos_State_ZWrite, 1);
		gos_SetRenderState(gos_State_ZCompare, 2);
	}
	else
	{
		gos_SetRenderState(gos_State_TextureAddress, gos_TextureWrap);
		gos_SetRenderState(gos_State_ShadeMode, gos_ShadeFlat);
		gos_SetRenderState(gos_State_MonoEnable, 1);
		gos_SetRenderState(gos_State_Perspective, 0);
		gos_SetRenderState(gos_State_Specular, 1);
		gos_SetRenderState(gos_State_ZWrite, 1);
		gos_SetRenderState(gos_State_ZCompare, 2);
	}
	// NEVER draw shadows in Software.
	if (Environment.Renderer != 3)
	{
		for (i = 0; i < nextAvailableVertexNode; i++)
		{
			if ((masterVertexNodes[i].flags & MC2_ISSHADOWS) && (masterVertexNodes[i].flags & MC2_DRAWALPHA) && (masterVertexNodes[i].vertices))
			{
				uint32_t totalvertices = masterVertexNodes[i].numvertices;
				if (masterVertexNodes[i].currentvertex != (masterVertexNodes[i].vertices + masterVertexNodes[i].numvertices))
				{
					totalvertices =
						masterVertexNodes[i].currentvertex - masterVertexNodes[i].vertices;
				}
				if (totalvertices && (totalvertices < MAX_SENDDOWN))
				{
					gos_SetRenderState(gos_State_Texture,
						masterTextureNodes[masterVertexNodes[i].textureindex]
							.get_gosTextureHandle());
					gos_RenderIndexedArray(
						masterVertexNodes[i].vertices, totalvertices, indexArray, totalvertices);
				}
				else if (totalvertices > MAX_SENDDOWN)
				{
					gos_SetRenderState(gos_State_Texture,
						masterTextureNodes[masterVertexNodes[i].textureindex]
							.get_gosTextureHandle());
					// Must divide up vertices into batches of 10,000 each to
					// send down.
					// Somewhere around 20000 to 30000 it really gets screwy!!!
					int32_t currentVertices = 0;
					while (currentVertices < totalvertices)
					{
						gos_VERTEX* v = masterVertexNodes[i].vertices + currentVertices;
						int32_t tVertices = totalvertices - currentVertices;
						if (tVertices > MAX_SENDDOWN)
							tVertices = MAX_SENDDOWN;
						gos_RenderIndexedArray(v, tVertices, indexArray, tVertices);
						currentVertices += tVertices;
					}
				}
				// Reset the list to zero length to avoid drawing more then
				// once!  Also comes in handy if gameLogic is not called.
				masterVertexNodes[i].currentvertex = masterVertexNodes[i].vertices;
			}
		}
	}
	gos_SetRenderState(gos_State_ZCompare, 1);
	if (Environment.Renderer != 3)
	{
		gos_SetRenderState(gos_State_ShadeMode, gos_ShadeGouraud);
		gos_SetRenderState(gos_State_ZWrite, 1);
	}
	for (i = 0; i < nextAvailableVertexNode; i++)
	{
		if (!(masterVertexNodes[i].flags & MC2_ISTERRAIN) && !(masterVertexNodes[i].flags & MC2_ISSHADOWS) && !(masterVertexNodes[i].flags & MC2_ISCOMPASS) && !(masterVertexNodes[i].flags & MC2_ISCRATERS) && (masterVertexNodes[i].flags & MC2_DRAWALPHA) && (masterVertexNodes[i].vertices))
		{
			uint32_t totalvertices = masterVertexNodes[i].numvertices;
			if (masterVertexNodes[i].currentvertex != (masterVertexNodes[i].vertices + masterVertexNodes[i].numvertices))
			{
				totalvertices = masterVertexNodes[i].currentvertex - masterVertexNodes[i].vertices;
			}
			if (totalvertices && (totalvertices < MAX_SENDDOWN))
			{
				gos_SetRenderState(gos_State_Texture,
					masterTextureNodes[masterVertexNodes[i].textureindex].get_gosTextureHandle());
				gos_RenderIndexedArray(
					masterVertexNodes[i].vertices, totalvertices, indexArray, totalvertices);
			}
			else if (totalvertices > MAX_SENDDOWN)
			{
				gos_SetRenderState(gos_State_Texture,
					masterTextureNodes[masterVertexNodes[i].textureindex].get_gosTextureHandle());
				// Must divide up vertices into batches of 10,000 each to send
				// down.
				// Somewhere around 20000 to 30000 it really gets screwy!!!
				int32_t currentVertices = 0;
				while (currentVertices < totalvertices)
				{
					gos_VERTEX* v = masterVertexNodes[i].vertices + currentVertices;
					int32_t tVertices = totalvertices - currentVertices;
					if (tVertices > MAX_SENDDOWN)
						tVertices = MAX_SENDDOWN;
					gos_RenderIndexedArray(v, tVertices, indexArray, tVertices);
					currentVertices += tVertices;
				}
			}
			// Reset the list to zero length to avoid drawing more then once!
			// Also comes in handy if gameLogic is not called.
			masterVertexNodes[i].currentvertex = masterVertexNodes[i].vertices;
		}
	}
	if (Environment.Renderer == 3)
	{
		gos_SetRenderState(gos_State_ShadeMode, gos_ShadeGouraud);
		gos_SetRenderState(gos_State_ZCompare, 1);
		gos_SetRenderState(gos_State_Fog, 0);
		gos_SetRenderState(gos_State_AlphaMode, gos_Alpha_OneOne);
		gos_SetRenderState(gos_State_TextureMapBlend, gos_BlendModulateAlpha);
		gos_SetRenderState(gos_State_ZWrite, 0);
		gos_SetRenderState(gos_State_MonoEnable, 1);
	}
	else
	{
		gos_SetRenderState(gos_State_ShadeMode, gos_ShadeGouraud);
		gos_SetRenderState(gos_State_Perspective, 1);
		gos_SetRenderState(gos_State_ZCompare, 1);
		gos_SetRenderState(gos_State_Fog, 0);
		gos_SetRenderState(gos_State_Specular, 0);
		gos_SetRenderState(gos_State_AlphaMode, gos_Alpha_OneOne);
		gos_SetRenderState(gos_State_TextureMapBlend, gos_BlendModulateAlpha);
		gos_SetRenderState(gos_State_ZWrite, 0);
		gos_SetRenderState(gos_State_MonoEnable, 0);
	}
	for (i = 0; i < nextAvailableVertexNode; i++)
	{
		if ((masterVertexNodes[i].flags & MC2_ISEFFECTS) && (masterVertexNodes[i].vertices))
		{
			uint32_t totalvertices = masterVertexNodes[i].numvertices;
			if (masterVertexNodes[i].currentvertex != (masterVertexNodes[i].vertices + masterVertexNodes[i].numvertices))
			{
				totalvertices = masterVertexNodes[i].currentvertex - masterVertexNodes[i].vertices;
			}
			if (totalvertices && (totalvertices < MAX_SENDDOWN))
			{
				gos_SetRenderState(gos_State_Texture,
					masterTextureNodes[masterVertexNodes[i].textureindex].get_gosTextureHandle());
				gos_RenderIndexedArray(
					masterVertexNodes[i].vertices, totalvertices, indexArray, totalvertices);
			}
			else if (totalvertices > MAX_SENDDOWN)
			{
				gos_SetRenderState(gos_State_Texture,
					masterTextureNodes[masterVertexNodes[i].textureindex].get_gosTextureHandle());
				// Must divide up vertices into batches of 10,000 each to send
				// down.
				// Somewhere around 20000 to 30000 it really gets screwy!!!
				int32_t currentVertices = 0;
				while (currentVertices < totalvertices)
				{
					gos_VERTEX* v = masterVertexNodes[i].vertices + currentVertices;
					int32_t tVertices = totalvertices - currentVertices;
					if (tVertices > MAX_SENDDOWN)
						tVertices = MAX_SENDDOWN;
					gos_RenderIndexedArray(v, tVertices, indexArray, tVertices);
					currentVertices += tVertices;
				}
			}
			// Reset the list to zero length to avoid drawing more then once!
			// Also comes in handy if gameLogic is not called.
			masterVertexNodes[i].currentvertex = masterVertexNodes[i].vertices;
		}
	}
	gos_SetRenderState(gos_State_ZWrite, 1);
	for (i = 0; i < nextAvailableVertexNode; i++)
	{
		if ((masterVertexNodes[i].flags & MC2_ISSPOTLGT) && (masterVertexNodes[i].vertices))
		{
			uint32_t totalvertices = masterVertexNodes[i].numvertices;
			if (masterVertexNodes[i].currentvertex != (masterVertexNodes[i].vertices + masterVertexNodes[i].numvertices))
			{
				totalvertices = masterVertexNodes[i].currentvertex - masterVertexNodes[i].vertices;
			}
			if (totalvertices && (totalvertices < MAX_SENDDOWN))
			{
				gos_SetRenderState(gos_State_Texture,
					masterTextureNodes[masterVertexNodes[i].textureindex].get_gosTextureHandle());
				gos_RenderIndexedArray(
					masterVertexNodes[i].vertices, totalvertices, indexArray, totalvertices);
			}
			else if (totalvertices > MAX_SENDDOWN)
			{
				gos_SetRenderState(gos_State_Texture,
					masterTextureNodes[masterVertexNodes[i].textureindex].get_gosTextureHandle());
				// Must divide up vertices into batches of 10,000 each to send
				// down.
				// Somewhere around 20000 to 30000 it really gets screwy!!!
				int32_t currentVertices = 0;
				while (currentVertices < totalvertices)
				{
					gos_VERTEX* v = masterVertexNodes[i].vertices + currentVertices;
					int32_t tVertices = totalvertices - currentVertices;
					if (tVertices > MAX_SENDDOWN)
						tVertices = MAX_SENDDOWN;
					gos_RenderIndexedArray(v, tVertices, indexArray, tVertices);
					currentVertices += tVertices;
				}
			}
			// Reset the list to zero length to avoid drawing more then once!
			// Also comes in handy if gameLogic is not called.
			masterVertexNodes[i].currentvertex = masterVertexNodes[i].vertices;
		}
	}
	gos_SetRenderState(gos_State_ZWrite, 0);
	gos_SetRenderState(gos_State_ZCompare, 0);
	gos_SetRenderState(gos_State_Perspective, 1);
	gos_SetRenderState(gos_State_AlphaMode, gos_Alpha_AlphaInvAlpha);
	gos_SetRenderState(gos_State_AlphaTest, 1);
	for (i = 0; i < nextAvailableVertexNode; i++)
	{
		if ((masterVertexNodes[i].flags & MC2_ISCOMPASS) && (masterVertexNodes[i].vertices))
		{
			uint32_t totalvertices = masterVertexNodes[i].numvertices;
			if (masterVertexNodes[i].currentvertex != (masterVertexNodes[i].vertices + masterVertexNodes[i].numvertices))
			{
				totalvertices = masterVertexNodes[i].currentvertex - masterVertexNodes[i].vertices;
			}
			if (totalvertices && (totalvertices < MAX_SENDDOWN))
			{
				gos_SetRenderState(gos_State_Texture,
					masterTextureNodes[masterVertexNodes[i].textureindex].get_gosTextureHandle());
				gos_RenderIndexedArray(
					masterVertexNodes[i].vertices, totalvertices, indexArray, totalvertices);
			}
			else if (totalvertices > MAX_SENDDOWN)
			{
				gos_SetRenderState(gos_State_Texture,
					masterTextureNodes[masterVertexNodes[i].textureindex].get_gosTextureHandle());
				// Must divide up vertices into batches of 10,000 each to send
				// down.
				// Somewhere around 20000 to 30000 it really gets screwy!!!
				int32_t currentVertices = 0;
				while (currentVertices < totalvertices)
				{
					gos_VERTEX* v = masterVertexNodes[i].vertices + currentVertices;
					int32_t tVertices = totalvertices - currentVertices;
					if (tVertices > MAX_SENDDOWN)
						tVertices = MAX_SENDDOWN;
					gos_RenderIndexedArray(v, tVertices, indexArray, tVertices);
					currentVertices += tVertices;
				}
			}
			// Reset the list to zero length to avoid drawing more then once!
			// Also comes in handy if gameLogic is not called.
			masterVertexNodes[i].currentvertex = masterVertexNodes[i].vertices;
		}
	}
	gos_SetRenderState(gos_State_Filter, gos_FilterNone);
	for (i = 0; i < nextAvailableVertexNode; i++)
	{
		if ((masterVertexNodes[i].flags & MC2_ISHUDLMNT) && (masterVertexNodes[i].vertices))
		{
			uint32_t totalvertices = masterVertexNodes[i].numvertices;
			if (masterVertexNodes[i].currentvertex != (masterVertexNodes[i].vertices + masterVertexNodes[i].numvertices))
			{
				totalvertices = masterVertexNodes[i].currentvertex - masterVertexNodes[i].vertices;
			}
			if (totalvertices && (totalvertices < MAX_SENDDOWN))
			{
				gos_SetRenderState(gos_State_Texture,
					masterTextureNodes[masterVertexNodes[i].textureindex].get_gosTextureHandle());
				gos_RenderIndexedArray(
					masterVertexNodes[i].vertices, totalvertices, indexArray, totalvertices);
			}
			else if (totalvertices > MAX_SENDDOWN)
			{
				gos_SetRenderState(gos_State_Texture,
					masterTextureNodes[masterVertexNodes[i].textureindex].get_gosTextureHandle());
				// Must divide up vertices into batches of 10,000 each to send
				// down.
				// Somewhere around 20000 to 30000 it really gets screwy!!!
				int32_t currentVertices = 0;
				while (currentVertices < totalvertices)
				{
					gos_VERTEX* v = masterVertexNodes[i].vertices + currentVertices;
					int32_t tVertices = totalvertices - currentVertices;
					if (tVertices > MAX_SENDDOWN)
						tVertices = MAX_SENDDOWN;
					gos_RenderIndexedArray(v, tVertices, indexArray, tVertices);
					currentVertices += tVertices;
				}
			}
			// Reset the list to zero length to avoid drawing more then once!
			// Also comes in handy if gameLogic is not called.
			masterVertexNodes[i].currentvertex = masterVertexNodes[i].vertices;
		}
	}
	// Must turn zCompare back on for FXs
	gos_SetRenderState(gos_State_ZCompare, 1);
}

//----------------------------------------------------------------------
uint32_t
MC_TextureManager::update(void)
{
	uint32_t numTexturesFreed = 0;
	currentUsedTextures = 0;
	for (size_t i = 0; i < MC_MAXTEXTURES; i++)
	{
		if ((masterTextureNodes[i].m_texturehandle != CACHED_OUT_HANDLE) && (masterTextureNodes[i].m_texturehandle != 0xffffffff))
		{
			if (!masterTextureNodes[i].m_uniqueinstance && !(masterTextureNodes[i].m_neverflush & 1)) // Only uncachable if
				// BIT 1 is set,
				// otherwise, cache 'em
				// out!
			{
				if (masterTextureNodes[i].m_lastused <= (turn - 60))
				{
					//----------------------------------------------------------------
					// Cache this badboy out.  Textures don't change.  Just
					// Destroy!
					if (masterTextureNodes[i].m_texturehandle)
						gos_DestroyTexture(masterTextureNodes[i].m_texturehandle);
					masterTextureNodes[i].m_texturehandle = CACHED_OUT_HANDLE;
					numTexturesFreed++;
				}
			}
			// Count ACTUAL number of textures being used.
			// ALSO can't count on turn being right.  Logistics does not update
			// unless simple Camera is up!!
			if (masterTextureNodes[i].m_texturehandle != CACHED_OUT_HANDLE)
				currentUsedTextures++;
		}
	}
	return numTexturesFreed;
}

//----------------------------------------------------------------------
uint32_t
MC_TextureManager::textureFromMemory(
	uint32_t* data, gos_TextureFormat key, uint32_t hints, uint32_t width, uint32_t bitDepth)
{
	int32_t i = 0;
	//--------------------------------------------------------
	// If we called this, we KNOW the texture is NOT loaded!
	//
	// Find first empty NODE
	for (i = 0; i < MC_MAXTEXTURES; i++)
	{
		if (masterTextureNodes[i].m_texturehandle == 0xffffffff)
		{
			break;
		}
	}
	if (i == MC_MAXTEXTURES)
		STOP(("TOO Many textures in game.  We have exceeded 4096 game handles"));
	//--------------------------------------------------------
	// New Method.  Just store memory footprint of texture.
	// DO NOT create GOS handle until we need it.
	masterTextureNodes[i].m_texturehandle = CACHED_OUT_HANDLE;
	masterTextureNodes[i].m_nodename = nullptr;
	masterTextureNodes[i].m_numusers = 1;
	masterTextureNodes[i].m_key = key;
	masterTextureNodes[i].m_hints = hints;
	//------------------------------------------
	// Find and store the width.
	masterTextureNodes[i].m_width = width;
	int32_t txmSize = width * width * bitDepth;
	if (!lzBuffer1)
	{
		lzBuffer1 = (uint8_t*)textureCacheHeap->Malloc(MAX_LZ_BUFFER_SIZE);
		gosASSERT(lzBuffer1 != nullptr);
		lzBuffer2 = (uint8_t*)textureCacheHeap->Malloc(MAX_LZ_BUFFER_SIZE);
		gosASSERT(lzBuffer2 != nullptr);
	}
	actualTextureSize += txmSize;
	uint32_t txmCompressSize = LZCompress(lzBuffer2, (uint8_t*)data, txmSize);
	compressedTextureSize += txmCompressSize;
	//-------------------------------------------------------
	// Create a block of cache memory to hold this texture.
	if (!masterTextureNodes[i].m_texturedata)
		masterTextureNodes[i].m_texturedata = (uint32_t*)textureCacheHeap->Malloc(txmCompressSize);
	// No More RAM.  Do not display this texture anymore.
	if (masterTextureNodes[i].m_texturedata == nullptr)
		masterTextureNodes[i].m_texturehandle = 0;
	else
	{
		memcpy(masterTextureNodes[i].m_texturedata, lzBuffer2, txmCompressSize);
		masterTextureNodes[i].m_lzcompsize = txmCompressSize;
	}
	//------------------
	return (i);
}

//----------------------------------------------------------------------
uint32_t
MC_TextureManager::textureInstanceExists(const std::wstring_view& textureFullPathName, gos_TextureFormat key,
	uint32_t hints, uint32_t uniqueInstance, uint32_t nFlush)
{
	int32_t i = 0;
	//--------------------------------------
	// Is this texture already Loaded?
	for (i = 0; i < MC_MAXTEXTURES; i++)
	{
		if (masterTextureNodes[i].m_nodename)
		{
			if (_stricmp(masterTextureNodes[i].m_nodename, textureFullPathName) == 0)
			{
				if (uniqueInstance == masterTextureNodes[i].m_uniqueinstance)
				{
					masterTextureNodes[i].m_numusers++;
					return (i); // Return the texture Node Id Now.
				}
				else
				{
					//------------------------------------------------
					// Copy the texture from old handle to a new one.
					// Return the NEW handle.
					//
					// There should be no code here!!!
				}
			}
		}
	}
	return 0;
}

//----------------------------------------------------------------------
uint32_t
MC_TextureManager::loadTexture(const std::wstring_view& textureFullPathName, gos_TextureFormat key,
	uint32_t hints, uint32_t uniqueInstance, uint32_t nFlush)
{
	int32_t i = 0;
	//--------------------------------------
	// Is this texture already Loaded?
	for (i = 0; i < MC_MAXTEXTURES; i++)
	{
		if (masterTextureNodes[i].m_nodename && (_stricmp(masterTextureNodes[i].m_nodename, textureFullPathName) == 0))
		{
			if (uniqueInstance == masterTextureNodes[i].m_uniqueinstance)
			{
				masterTextureNodes[i].m_numusers++;
				return (i); // Return the texture Node Id Now.
			}
			else
			{
				//------------------------------------------------
				// Copy the texture from old handle to a new one.
				// Return the NEW handle.
				//
				// There should be no code here!!!
			}
		}
	}
	//--------------------------------------------------
	// If we get here, texture has not been loaded yet.
	// Load it now!
	//
	// Find first empty NODE
	for (i = 0; i < MC_MAXTEXTURES; i++)
	{
		if (masterTextureNodes[i].m_texturehandle == 0xffffffff)
		{
			break;
		}
	}
	if (i == MC_MAXTEXTURES)
		STOP(("TOO Many textures in game.  We have exceeded 4096 game handles"));
	if (key == gos_Texture_Alpha && Environment.Renderer == 3)
	{
		key = gos_Texture_Keyed;
	}
	//--------------------------------------------------------
	// New Method.  Just store memory footprint of texture.
	// DO NOT create GOS handle until we need it.
	masterTextureNodes[i].m_texturehandle = CACHED_OUT_HANDLE;
	masterTextureNodes[i].m_nodename =
		(const std::wstring_view&)textureStringHeap->Malloc(strlen(textureFullPathName) + 1);
	gosASSERT(masterTextureNodes[i].m_nodename != nullptr);
	strcpy(masterTextureNodes[i].m_nodename, textureFullPathName);
	masterTextureNodes[i].m_numusers = 1;
	masterTextureNodes[i].m_key = key;
	masterTextureNodes[i].m_hints = hints;
	masterTextureNodes[i].m_uniqueinstance = uniqueInstance;
	masterTextureNodes[i].m_neverflush = nFlush;
	//----------------------------------------------------------------------------------------------
	// Store 0xf0000000 & fileSize in width so that cache knows to create new
	// texture from memory. This way, we never need to know anything about the
	// texture AND we can store PMGs in memory instead of TGAs which use WAY
	// less RAM!
	MechFile textureFile;
#ifdef _DEBUG
	int32_t textureFileOpenResult =
#endif
		textureFile.open(textureFullPathName);
	gosASSERT(textureFileOpenResult == NO_ERROR);
	int32_t txmSize = textureFile.fileSize();
	if (!lzBuffer1)
	{
		lzBuffer1 = (uint8_t*)textureCacheHeap->Malloc(MAX_LZ_BUFFER_SIZE);
		gosASSERT(lzBuffer1 != nullptr);
		lzBuffer2 = (uint8_t*)textureCacheHeap->Malloc(MAX_LZ_BUFFER_SIZE);
		gosASSERT(lzBuffer2 != nullptr);
	}
	// Try reading the RAW data out of the fastFile.
	// If it succeeds, we just saved a complete compress, decompress and two
	// memcpys!!
	//
	int32_t result = textureFile.readRAW(masterTextureNodes[i].m_texturedata, textureCacheHeap);
	if (!result)
	{
		textureFile.read(lzBuffer1, txmSize);
		textureFile.close();
		actualTextureSize += txmSize;
		uint32_t txmCompressSize = LZCompress(lzBuffer2, lzBuffer1, txmSize);
		compressedTextureSize += txmCompressSize;
		masterTextureNodes[i].m_texturedata = (uint32_t*)textureCacheHeap->Malloc(txmCompressSize);
		if (masterTextureNodes[i].m_texturedata == nullptr)
			masterTextureNodes[i].m_texturehandle = 0;
		else
			memcpy(masterTextureNodes[i].m_texturedata, lzBuffer2, txmCompressSize);
		masterTextureNodes[i].m_lzcompsize = txmCompressSize;
	}
	else
	{
		masterTextureNodes[i].m_lzcompsize = result;
	}
	masterTextureNodes[i].m_width = 0xf0000000 + txmSize;
	//-------------------
	return (i);
}

//----------------------------------------------------------------------
int32_t
MC_TextureManager::saveTexture(uint32_t textureindex, const std::wstring_view& textureFullPathName)
{
	if ((MC_MAXTEXTURES <= textureindex) || (nullptr == masterTextureNodes[textureindex].m_texturedata))
	{
		return (~NO_ERROR);
	}
	MechFile textureFile;
	int32_t textureFileOpenResult = textureFile.create(textureFullPathName);
	if (NO_ERROR != textureFileOpenResult)
	{
		textureFile.close();
		return textureFileOpenResult;
	}
	{
		if (masterTextureNodes[textureindex].m_width == 0)
		{
			textureFile.close();
			return (~NO_ERROR); // These faces have no texture!!
		}
		{
			//------------------------------------------
			// Badboys are now LZ Compressed in texture cache.
			size_t origSize = LZDecomp(MC_TextureManager::lzBuffer2,
				(uint8_t*)masterTextureNodes[textureindex].m_texturedata,
				masterTextureNodes[textureindex].m_lzcompsize);
			if (origSize != (masterTextureNodes[textureindex].m_width & 0x0fffffff))
				STOP(("Decompressed to different size from original!  Txm:%s  "
					  "width:%d  DecompSize:%d",
					masterTextureNodes[textureindex].m_nodename,
					(masterTextureNodes[textureindex].m_width & 0x0fffffff), origSize));
			if (origSize >= MAX_LZ_BUFFER_SIZE)
				STOP(("Texture TOO large: %s", masterTextureNodes[textureindex].m_nodename));
			textureFile.write(MC_TextureManager::lzBuffer2, origSize);
		}
		textureFile.close();
	}
	return NO_ERROR;
}

uint32_t
MC_TextureManager::copyTexture(uint32_t texNodeID)
{
	gosASSERT(texNodeID < MC_MAXTEXTURES);
	if (masterTextureNodes[texNodeID].m_texturehandle != -1)
	{
		masterTextureNodes[texNodeID].m_numusers++;
		return texNodeID;
	}
	else
	{
		STOP(("tried to copy an invalid texture"));
	}
	return -1;
}
//----------------------------------------------------------------------
// MC_TextureNode
uint32_t
MC_TextureNode::get_gosTextureHandle(void) // If texture is not in
	// VidRAM, cache a texture
	// out and cache this one in.
{
	if (m_texturehandle == 0xffffffff)
	{
		// Somehow this texture is bad.  Probably we are using a handle which
		// got purged between missions.
		// Just send back, NO TEXTURE and we should be able to debug from there
		// because the tri will have no texture!!
		return 0x0;
	}
	if (m_texturehandle != CACHED_OUT_HANDLE)
	{
		m_lastused = turn;
		return m_texturehandle;
	}
	else
	{
		if ((mcTextureManager->currentUsedTextures >= MAX_MC2_GOS_TEXTURES) && !mcTextureManager->flushCache())
			return 0x0; // No texture!
		if (m_width == 0)
			return 0; // These faces have no texture!!
		if (!m_texturedata)
			return 0x0; // No Texture.  Cache is out of RAM!!
		if (m_width > 0xf0000000)
		{
			//------------------------------------------
			// Cache this badboy IN.
			// Badboys are now LZ Compressed in texture cache.
			// Uncompress, then memcpy.
			size_t origSize =
				LZDecomp(MC_TextureManager::lzBuffer2, (uint8_t*)m_texturedata, m_lzcompsize);
			if (origSize != (m_width & 0x0fffffff))
				STOP(("Decompressed to different size from original!  Txm:%s  "
					  "width:%d  DecompSize:%d",
					m_nodename, (m_width & 0x0fffffff), origSize));
			if (origSize >= MAX_LZ_BUFFER_SIZE)
				STOP(("Texture TOO large: %s", m_nodename));
			m_texturehandle = gos_NewTextureFromMemory(
				m_key, m_nodename, MC_TextureManager::lzBuffer2, (m_width & 0x0fffffff), m_hints);
			mcTextureManager->currentUsedTextures++;
			m_lastused = turn;
			return m_texturehandle;
		}
		else
		{
			m_texturehandle = gos_NewEmptyTexture(m_key, m_nodename, m_width, m_hints);
			mcTextureManager->currentUsedTextures++;
			//------------------------------------------
			// Cache this badboy IN.
			TEXTUREPTR pTextureData;
			gos_LockTexture(m_texturehandle, 0, 0, &pTextureData);
			//-------------------------------------------------------
			// Create a block of cache memory to hold this texture.
			uint32_t txmSize = pTextureData.height * pTextureData.height * sizeof(uint32_t);
			gosASSERT(m_texturedata);
			LZDecomp(MC_TextureManager::lzBuffer2, (uint8_t*)m_texturedata, m_lzcompsize);
			memcpy(pTextureData.pTexture, MC_TextureManager::lzBuffer2, txmSize);
			//------------------------
			// Unlock the texture
			gos_UnLockTexture(m_texturehandle);
			m_lastused = turn;
			return m_texturehandle;
		}
	}
}

//----------------------------------------------------------------------
void
MC_TextureNode::destroy(void)
{
	if ((m_texturehandle != CACHED_OUT_HANDLE) && (m_texturehandle != 0xffffffff) && (m_texturehandle != 0x0))
	{
		gos_DestroyTexture(m_texturehandle);
	}
	mcTextureManager->textureStringHeap->Free(m_nodename);
	mcTextureManager->textureCacheHeap->Free(m_texturedata);
	init();
}

//----------------------------------------------------------------------
