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
// NOT ANY MORE
// Now we no longer coalesce textures.  Causes too many problems with rendering
// Now it simply checks for redundant filenames and can instance a texture
// instead of reloading it.
//
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//
//
//----------------------------------------------------------------------
#ifndef TXMMGR_H
#define TXMMGR_H

#ifndef DSTD_H
#include "dstd.h"
#endif

#ifndef HEAP_H
#include "heap.h"
#endif

#include <string.h>
//#include "gameos.hpp"
//----------------------------------------------------------------------
enum MC_TextureKey
{
	MC_INVALIDTEXTUREKEY = 0,
	MC_Solid,
	MC_Keyed,
	MC_Alpha
};

//----------------------------------------------------------------------
// Can store up to 1024 8x8 textures in 1 256x256.
#define MC_MAXTEXTURES 4096
#define CACHED_OUT_HANDLE 0xFFFFFACE // If this value is in m_texturehandle, texture is cached out and must be cached in.
#define MAX_MC2_GOS_TEXTURES 750
#define TEXTURE_CACHE_SIZE (40 * 1024 * 1024) // Extra space here to facilitate editting
#define MAX_CACHE_SIZE (40 * 1024 * 1024) // Actual amount map must run in!
#define MC_MAXFACES 50000
#define MAX_LZ_BUFFER_SIZE ((256 * 256 * 4) + 1024)

#define MC2_ISTERRAIN 1
#define MC2_DRAWSOLID 2
#define MC2_DRAWALPHA 4
#define MC2_ISSHADOWS 8
#define MC2_ISEFFECTS 16
#define MC2_DRAWONEIN 32
#define MC2_ISCRATERS 64
#define MC2_ISCOMPASS 128
#define MC2_ISSPOTLGT 256
#define MC2_ISHUDLMNT 512

//----------------------------------------------------------------------
// No More MC_BlockInfos.  m_texturehandle is protected to enforce caching.
class MC_TextureManager;

//---------------------------------------------------------------------------
struct MC_VertexArrayNode
{
	friend MC_TextureManager;

	uint32_t textureindex = 0;
	uint32_t flags = 0; // Marks texture render state and terrain or not, etc.
	int32_t numvertices = 0; // Number of vertices this texture will be used to draw this frame.
	gos_VERTEX* currentvertex = nullptr; // CurrentVertex data being added.
	gos_VERTEX* vertices = nullptr; // Pointer into the vertex Pool for this texture to draw.

	//void init(void) {}
	//void destroy(void); // Frees all blocks, free GOS_TextureHandle, blank all data.
};

//----------------------------------------------------------------------
struct MC_TextureNode
{
	friend MC_TextureManager;

protected:
	uint32_t m_texturehandle // handle returned by GOS

		public : wchar_t* m_nodename = nullptr; // Used for Unique nodes so I can just return the handle!
	uint32_t m_uniqueinstance = 0; // Texture is modifiable.  DO NOT CACHE OUT!!!!!!
	uint32_t m_neverflush; // Textures used by Userinput, etc.  DO NOT CACHE OUT!!!!!!
	uint32_t m_numusers = 0; // Pushed up for each user using. Users can "free" a texture which will decrement the number and actually free it if number is 0
	gos_TextureFormat m_key = gos_Texture_Solid; // Used to recreate texture if cached out.
	gos_TextureHints m_hints = gosHint_DisableMipmap; // Used to recreate texture if cached out.
	uint32_t m_width = 0; // Used to recreate texture if cached out.
	uint32_t m_lzcompsize = static_cast<uint32_t>(-1); // Size of Compressed version.
	uint32_t m_lastused = static_cast<uint32_t>(-1); // Last Game turn texture was used.  Used to cache textures.
	uint32_t* m_texturedata = nullptr; // Raw texture data.  Texture is stored here in system RAM
	// if we overrun the max number of GOS HAndles.
	// When the texture is needed, another least used GOS handle is
	// cached out and this one is copied in.
	// This means that all instances where the game refers
	// To m_texturehandle must be replaced with a get_gosTextureHandle
	// Or texture may not be cached in and it will draw the texture
	// at this location instead.  Just like some video cards do!
	// This value can be nullptr if texture is on card.
	// Should only be alloced when we need to cache.
	// This will keep system memory usage to a minimum.
	MC_VertexArrayNode* vertexData = nullptr; // This holds the vertex draw data.  nullptr if not used.
	MC_VertexArrayNode* vertexData2 = nullptr;
	MC_VertexArrayNode* vertexData3 = nullptr;

	void init(void)
	{
		m_texturehandle = 0xffffffff;
		m_numusers = 0;
		m_nodename = nullptr;
		m_texturedata = nullptr;
		m_lastused = -1; // NEVER been used.
		m_key = gos_Texture_Solid;
		m_hints = gosHint_DisableMipmap;
		m_width = 0;
		m_uniqueinstance = 0x0;
		m_neverflush = false;
		vertexData = nullptr;
		vertexData2 = nullptr;
		vertexData3 = nullptr;
		m_lzcompsize = 0xffffffff;
	}

	uint32_t findFirstAvailableBlock(void);

	void destroy(void); // Frees all blocks, free GOS_TextureHandle, blank all data.

	void removeBlock(uint32_t blockNum); // Just free one block.  DO NOT FREE GOS_TextureHandle.

	void markBlock(uint32_t blockNum);

	uint32_t get_gosTextureHandle(void); // If texture is not in VidRAM, cache a
		// texture out and cache this one in.
};

//---------------------------------------------------------------------------
class gos_VERTEXManager : public HeapManager
{
	// Data Members
	//-------------
protected:
	int32_t totalvertices; // Total number of vertices in pool.
	int32_t currentvertex; // Pointer to next available vertex in pool.

	// Member Functions
	//-----------------
public:
	void init(void)
	{
		HeapManager::init(void);
		totalvertices = 0;
		currentvertex = 0;
	}

	gos_VERTEXManager(void)
		: HeapManager()
	{
		init(void);
	}

	void destroy(void)
	{
		HeapManager::destroy(void);
		reset(void);
		totalvertices = 0;
	}

	~gos_VERTEXManager(void)
	{
		destroy(void);
	}

	void init(int32_t maxVertices)
	{
		totalvertices = maxVertices;
		uint32_t heapSize = totalvertices * sizeof(gos_VERTEX);
		createHeap(heapSize);
		commitHeap(void);
		reset(void);
	}

	gos_VERTEX* getVertexBlock(uint32_t numvertices)
	{
		gos_VERTEX* start = (gos_VERTEX*)getHeapPtr(void);
		start = &(start[currentvertex]);
		currentvertex += numvertices;
		gosASSERT(currentvertex < totalvertices);
		return start;
	}

	void reset(void)
	{
		currentvertex = 0;
	}
};

//----------------------------------------------------------------------
class MC_TextureManager
{
	friend MC_TextureNode;

	// Data Members
	//------------
protected:
	MC_TextureNode* masterTextureNodes; // Dynamically allocated from an MC
		// Heap.
	int32_t currentUsedTextures; // Number of textures on video card.

	MC_VertexArrayNode* masterVertexNodes; // Dynamically allocated from an MC Heap.
	int32_t nextAvailableVertexNode; // index to next available vertex Node

	UserHeapPtr textureCacheHeap; // Heap used to cache textures from vidCard to
		// system RAM.
	UserHeapPtr textureStringHeap; // Heap used to store filenames of textures
		// so no dupes.
	bool textureManagerInstrumented; // Texture Manager Instrumented.
	int32_t totalCacheMisses; //NUmber of times flush has been called.\

	static gos_VERTEXManager* gvManager; // Stores arrays of vertices for draw.
	static uint8_t* lzBuffer1; // Used to compress/decompress textures from cache.
	static uint8_t* lzBuffer2; // Used to compress/decompress textures from cache.
	/* iBufferRefCount is used to help determine if lzBuffer1&2 are valid. The
	assumption is that if there are no valid MC_TextureManagers then lzBuffer1&2
	are not valid. */
	static int32_t iBufferRefCount;

	uint16_t* indexArray; // Master Vertex Index array.

	// Upto four different kinds of untextured triangle!
	MC_VertexArrayNode* vertexData; // This holds the vertex draw data for UNTEXTURED triangles!
	MC_VertexArrayNode* vertexData2; // This holds the vertex draw data for
		// UNTEXTURED triangles!
	MC_VertexArrayNode* vertexData3; // This holds the vertex draw data for
		// UNTEXTURED triangles!
	MC_VertexArrayNode* vertexData4; // This holds the vertex draw data for
		// UNTEXTURED triangles!

	// Member Functions
	//-----------------
public:
	void init(void)
	{
		masterTextureNodes = nullptr;
		textureCacheHeap = nullptr;
		textureStringHeap = nullptr;
		textureManagerInstrumented = false;
		totalCacheMisses = 0;
		currentUsedTextures = 0;
		indexArray = nullptr;
		masterVertexNodes = nullptr;
		nextAvailableVertexNode = 0;
		vertexData = vertexData2 = vertexData3 = vertexData4 = nullptr;
	}

	MC_TextureManager(void)
	{
		init(void);
		MC_TextureManager::iBufferRefCount++;
	}

	void destroy(void);

	~MC_TextureManager(void);

	void start(void); // This function creates the Blank Nodes from an MC Heap.

	//-----------------------------------------------------------------------------
	// Returns whether or not loadTexture() would return a handle to a texture
	// instance that was already loaded in memory. This function was motivated
	// by the fact that that an existing texture instance can been modified in
	// memory after it's loaded, and thus be different the from an instance that
	// would be loaded from disk.
	uint32_t textureInstanceExists(std::wstring_view textureFullPathName, gos_TextureFormat key, uint32_t hints,
		uint32_t uniqueInstance = 0x0, uint32_t nFlush = 0x0);

	//-----------------------------------------------------------------------------
	// Returns the TextureNode Id based on what you asked for.
	//
	// uniqueInstance is an ID for the instance of the texture. If its value
	// matches that of an already existing instance of the texture, the handle
	// of the existing instance will be returned. Used for Mech colouration
	// possibly, damage states, etc.
	uint32_t loadTexture(std::wstring_view textureFullPathName, gos_TextureFormat key, uint32_t hints,
		uint32_t uniqueInstance = 0x0, uint32_t nFlush = 0x0);

	int32_t saveTexture(uint32_t textureindex, std::wstring_view textureFullPathName);

	//-----------------------------------------------------------------------------
	// Returns the TextureNode Id based on what you asked for.
	uint32_t textureFromMemory(uint32_t* data, gos_TextureFormat key, uint32_t hints,
		uint32_t width, uint32_t bitDepth = 4);

	// increments the ref count
	uint32_t copyTexture(uint32_t texNodeID);

	//------------------------------------------------------
	// Tosses ALL of the textureNodes and frees GOS Handles
	void flush(bool justTextures = false);

	//------------------------------------------------------
	// Frees up gos_VERTEX manager memory
	void freeVertices(void);

	//------------------------------------------------------
	// Creates gos_VERTEX Manager and allocates RAM.  Will not allocate if
	// already done!
	void startVertices(int32_t maxVertices = 30000);

	//------------------------------------------------------
	// Frees a specific texture.
	void removeTexture(uint32_t m_texturehandle);

	//------------------------------------------------------
	// Frees a specific textureNode.
	void removeTextureNode(uint32_t textureNode);

	//-----------------------------------------------------------------
	// Gets m_texturehandle for Node ID.  Does all caching necessary.
	uint32_t get_gosTextureHandle(uint32_t nodeId)
	{
		if (nodeId != 0xffffffff)
			return masterTextureNodes[nodeId].get_gosTextureHandle(void);
		else
			return nodeId;
	}

	void addTriangle(uint32_t nodeId, uint32_t flags)
	{
		if ((nodeId < MC_MAXTEXTURES) && (nextAvailableVertexNode < MC_MAXTEXTURES))
		{
			if (!masterTextureNodes[nodeId].vertexData)
			{
				masterTextureNodes[nodeId].vertexData =
					&(masterVertexNodes[nextAvailableVertexNode]);
				gosASSERT(masterTextureNodes[nodeId].vertexData->numvertices == 0);
				gosASSERT(masterTextureNodes[nodeId].vertexData->vertices == nullptr);
				nextAvailableVertexNode++;
				masterTextureNodes[nodeId].vertexData->flags = flags;
				masterTextureNodes[nodeId].vertexData->textureindex = nodeId;
			}
			else if (masterTextureNodes[nodeId].vertexData && (masterTextureNodes[nodeId].vertexData->flags != flags) && !masterTextureNodes[nodeId].vertexData2)
			{
				masterTextureNodes[nodeId].vertexData2 =
					&(masterVertexNodes[nextAvailableVertexNode]);
				gosASSERT(masterTextureNodes[nodeId].vertexData2->numvertices == 0);
				gosASSERT(masterTextureNodes[nodeId].vertexData2->vertices == nullptr);
				nextAvailableVertexNode++;
				masterTextureNodes[nodeId].vertexData2->flags = flags;
				masterTextureNodes[nodeId].vertexData2->textureindex = nodeId;
			}
			else if (masterTextureNodes[nodeId].vertexData && (masterTextureNodes[nodeId].vertexData->flags != flags) && masterTextureNodes[nodeId].vertexData2 && (masterTextureNodes[nodeId].vertexData2->flags != flags) && !masterTextureNodes[nodeId].vertexData3)
			{
				masterTextureNodes[nodeId].vertexData3 =
					&(masterVertexNodes[nextAvailableVertexNode]);
				gosASSERT(masterTextureNodes[nodeId].vertexData3->numvertices == 0);
				gosASSERT(masterTextureNodes[nodeId].vertexData3->vertices == nullptr);
				nextAvailableVertexNode++;
				masterTextureNodes[nodeId].vertexData3->flags = flags;
				masterTextureNodes[nodeId].vertexData3->textureindex = nodeId;
			}
			if (masterTextureNodes[nodeId].vertexData->flags == flags)
				masterTextureNodes[nodeId].vertexData->numvertices += 3;
			else if (masterTextureNodes[nodeId].vertexData2 && masterTextureNodes[nodeId].vertexData2->flags == flags)
				masterTextureNodes[nodeId].vertexData2->numvertices += 3;
			else if (masterTextureNodes[nodeId].vertexData3 && masterTextureNodes[nodeId].vertexData3->flags == flags)
				masterTextureNodes[nodeId].vertexData3->numvertices += 3;
#ifdef _DEBUG
			else
				STOP(("Could not AddTriangles.  No flags match vertex data"));
#endif
		}
		else if (nextAvailableVertexNode < MC_MAXTEXTURES)
		{
			// Add this one to the untextured vertexBuffers
			if (!vertexData)
			{
				vertexData = &(masterVertexNodes[nextAvailableVertexNode]);
				gosASSERT(vertexData->numvertices == 0);
				gosASSERT(vertexData->vertices == nullptr);
				nextAvailableVertexNode++;
				vertexData->flags = flags;
				vertexData->textureindex = 0;
			}
			else if (vertexData && (vertexData->flags != flags) && !vertexData2)
			{
				vertexData2 = &(masterVertexNodes[nextAvailableVertexNode]);
				gosASSERT(vertexData2->numvertices == 0);
				gosASSERT(vertexData2->vertices == nullptr);
				nextAvailableVertexNode++;
				vertexData2->flags = flags;
				vertexData2->textureindex = 0;
			}
			else if (vertexData && (vertexData->flags != flags) && vertexData2 && (vertexData2->flags != flags) && !vertexData3)
			{
				vertexData3 = &(masterVertexNodes[nextAvailableVertexNode]);
				gosASSERT(vertexData3->numvertices == 0);
				gosASSERT(vertexData3->vertices == nullptr);
				nextAvailableVertexNode++;
				vertexData3->flags = flags;
				vertexData3->textureindex = 0;
			}
			else if (vertexData && (vertexData->flags != flags) && vertexData2 && (vertexData2->flags != flags) && vertexData3 && (vertexData3->flags != flags) && !vertexData4)
			{
				vertexData4 = &(masterVertexNodes[nextAvailableVertexNode]);
				gosASSERT(vertexData4->numvertices == 0);
				gosASSERT(vertexData4->vertices == nullptr);
				nextAvailableVertexNode++;
				vertexData4->flags = flags;
				vertexData4->textureindex = 0;
			}
			if (vertexData->flags == flags)
				vertexData->numvertices += 3;
			else if (vertexData2 && vertexData2->flags == flags)
				vertexData2->numvertices += 3;
			else if (vertexData3 && vertexData3->flags == flags)
				vertexData3->numvertices += 3;
			else if (vertexData4 && vertexData4->flags == flags)
				vertexData4->numvertices += 3;
#ifdef _DEBUG
			else
				PAUSE(("Could not AddTriangles.  Too many untextured triangle "
					   "types"));
#endif
		}
	}

	void addVertices(uint32_t nodeId, gos_VERTEX* data, uint32_t flags)
	{
		// This function adds the actual vertex data to the texture Node.
		if (nodeId < MC_MAXTEXTURES)
		{
			if (masterTextureNodes[nodeId].vertexData && masterTextureNodes[nodeId].vertexData->flags == flags)
			{
				gos_VERTEX* vertices = masterTextureNodes[nodeId].vertexData->currentvertex;
				if (!vertices && !masterTextureNodes[nodeId].vertexData->vertices)
				{
					masterTextureNodes[nodeId].vertexData->currentvertex = vertices =
						masterTextureNodes[nodeId].vertexData->vertices = gvManager->getVertexBlock(
							masterTextureNodes[nodeId].vertexData->numvertices);
				}
				if (vertices < (masterTextureNodes[nodeId].vertexData->vertices + masterTextureNodes[nodeId].vertexData->numvertices))
				{
#if 0
					if((data[0].u > 64.0f) ||
							(data[0].u < -64.0f) ||
							(data[1].u > 64.0f) ||
							(data[1].u < -64.0f) ||
							(data[2].u > 64.0f) ||
							(data[2].u < -64.0f))
					{
						PAUSE(("Vertex U Out of range"));
					}
					if((data[0].v > 64.0f) ||
							(data[0].v < -64.0f) ||
							(data[1].v > 64.0f) ||
							(data[1].v < -64.0f) ||
							(data[2].v > 64.0f) ||
							(data[2].v < -64.0f))
					{
						PAUSE(("Vertex V Out of range"));
					}
#endif
					if (vertices)
						memcpy(vertices, data, sizeof(gos_VERTEX) * 3);
					vertices += 3;
				}
				masterTextureNodes[nodeId].vertexData->currentvertex = vertices;
			}
			else if (masterTextureNodes[nodeId].vertexData2 && masterTextureNodes[nodeId].vertexData2->flags == flags)
			{
				gos_VERTEX* vertices = masterTextureNodes[nodeId].vertexData2->currentvertex;
#if defined(_DEBUG) || defined(_ARMOR)
				gos_VERTEX* oldVertices = vertices;
				gos_VERTEX* oldStart = (masterTextureNodes[nodeId].vertexData2->vertices + masterTextureNodes[nodeId].vertexData2->numvertices);
				(void)oldVertices;
				(void)oldStart; // 4189
				gosASSERT(oldVertices < oldStart);
#endif
				if (!vertices && !masterTextureNodes[nodeId].vertexData2->vertices)
				{
					masterTextureNodes[nodeId].vertexData2->currentvertex = vertices =
						masterTextureNodes[nodeId].vertexData2->vertices =
							gvManager->getVertexBlock(
								masterTextureNodes[nodeId].vertexData2->numvertices);
				}
				if (vertices < (masterTextureNodes[nodeId].vertexData2->vertices + masterTextureNodes[nodeId].vertexData2->numvertices))
				{
#if 0
					if((data[0].u > 64.0f) ||
							(data[0].u < -64.0f) ||
							(data[1].u > 64.0f) ||
							(data[1].u < -64.0f) ||
							(data[2].u > 64.0f) ||
							(data[2].u < -64.0f))
					{
						PAUSE(("Vertex U Out of range"));
					}
					if((data[0].v > 64.0f) ||
							(data[0].v < -64.0f) ||
							(data[1].v > 64.0f) ||
							(data[1].v < -64.0f) ||
							(data[2].v > 64.0f) ||
							(data[2].v < -64.0f))
					{
						PAUSE(("Vertex V Out of range"));
					}
#endif
					if (vertices)
						memcpy(vertices, data, sizeof(gos_VERTEX) * 3);
					vertices += 3;
				}
				masterTextureNodes[nodeId].vertexData2->currentvertex = vertices;
			}
			else if (masterTextureNodes[nodeId].vertexData3 && masterTextureNodes[nodeId].vertexData3->flags == flags)
			{
				gos_VERTEX* vertices = masterTextureNodes[nodeId].vertexData3->currentvertex;
#if defined(_DEBUG) || defined(_ARMOR)
				gos_VERTEX* oldVertices = vertices;
				gos_VERTEX* oldStart = (masterTextureNodes[nodeId].vertexData3->vertices + masterTextureNodes[nodeId].vertexData3->numvertices);
				(void)oldVertices;
				(void)oldStart; // 4189
				gosASSERT(oldVertices < oldStart);
#endif
				if (!vertices && !masterTextureNodes[nodeId].vertexData3->vertices)
				{
					masterTextureNodes[nodeId].vertexData3->currentvertex = vertices =
						masterTextureNodes[nodeId].vertexData3->vertices =
							gvManager->getVertexBlock(
								masterTextureNodes[nodeId].vertexData3->numvertices);
				}
				if (vertices < (masterTextureNodes[nodeId].vertexData3->vertices + masterTextureNodes[nodeId].vertexData3->numvertices))
				{
#if 0
					if((data[0].u > 64.0f) ||
							(data[0].u < -64.0f) ||
							(data[1].u > 64.0f) ||
							(data[1].u < -64.0f) ||
							(data[2].u > 64.0f) ||
							(data[2].u < -64.0f))
					{
						PAUSE(("Vertex U Out of range"));
					}
					if((data[0].v > 64.0f) ||
							(data[0].v < -64.0f) ||
							(data[1].v > 64.0f) ||
							(data[1].v < -64.0f) ||
							(data[2].v > 64.0f) ||
							(data[2].v < -64.0f))
					{
						PAUSE(("Vertex V Out of range"));
					}
#endif
					if (vertices)
						memcpy(vertices, data, sizeof(gos_VERTEX) * 3);
					vertices += 3;
				}
				masterTextureNodes[nodeId].vertexData3->currentvertex = vertices;
			}
			else // If we got here, something is really wrong
			{
#ifdef _DEBUG
				PAUSE(("flags do not match either set of vertex Data"));
#endif
			}
		}
		else
		{
			if (vertexData && vertexData->flags == flags)
			{
				gos_VERTEX* vertices = vertexData->currentvertex;
				if (!vertices && !vertexData->vertices)
				{
					vertexData->currentvertex = vertices = vertexData->vertices =
						gvManager->getVertexBlock(vertexData->numvertices);
				}
				if (vertices <= (vertexData->vertices + vertexData->numvertices))
				{
					memcpy(vertices, data, sizeof(gos_VERTEX) * 3);
					vertices += 3;
				}
				vertexData->currentvertex = vertices;
			}
			else if (vertexData2 && vertexData2->flags == flags)
			{
				gos_VERTEX* vertices = vertexData2->currentvertex;
				if (!vertices && !vertexData2->vertices)
				{
					vertexData2->currentvertex = vertices = vertexData2->vertices =
						gvManager->getVertexBlock(vertexData2->numvertices);
				}
				if (vertices <= (vertexData2->vertices + vertexData2->numvertices))
				{
					memcpy(vertices, data, sizeof(gos_VERTEX) * 3);
					vertices += 3;
				}
				vertexData2->currentvertex = vertices;
			}
			else if (vertexData3 && vertexData3->flags == flags)
			{
				gos_VERTEX* vertices = vertexData3->currentvertex;
				if (!vertices && !vertexData3->vertices)
				{
					vertexData3->currentvertex = vertices = vertexData3->vertices =
						gvManager->getVertexBlock(vertexData3->numvertices);
				}
				if (vertices <= (vertexData3->vertices + vertexData3->numvertices))
				{
					memcpy(vertices, data, sizeof(gos_VERTEX) * 3);
					vertices += 3;
				}
				vertexData3->currentvertex = vertices;
			}
			else if (vertexData4 && vertexData4->flags == flags)
			{
				gos_VERTEX* vertices = vertexData4->currentvertex;
				if (!vertices && !vertexData4->vertices)
				{
					vertexData4->currentvertex = vertices = vertexData4->vertices =
						gvManager->getVertexBlock(vertexData4->numvertices);
				}
				if (vertices <= (vertexData4->vertices + vertexData4->numvertices))
				{
					memcpy(vertices, data, sizeof(gos_VERTEX) * 3);
					vertices += 3;
				}
				vertexData4->currentvertex = vertices;
			}
			else // If we got here, something is really wrong
			{
#ifdef _DEBUG
				PAUSE(("flags do not match any set of untextured vertex Data"));
#endif
			}
		}
	}

	void clearArrays(void)
	{
		for (size_t i = 0; i < MC_MAXTEXTURES; i++)
		{
			masterTextureNodes[i].vertexData = nullptr;
			masterTextureNodes[i].vertexData2 = nullptr;
			masterTextureNodes[i].vertexData3 = nullptr;
		}
		vertexData = vertexData2 = vertexData3 = vertexData4 = nullptr;
		memset(masterVertexNodes, 0, sizeof(MC_VertexArrayNode) * MC_MAXTEXTURES);
		nextAvailableVertexNode = 0;
		gvManager->reset(void);
	}

	// Sends down the triangle lists
	void renderLists(void);

	uint32_t getwidth(uint32_t nodeId)
	{
		if (nodeId != 0xffffffff)
			return masterTextureNodes[nodeId].m_width;
		else
			return nodeId;
	}

	//----------------------------------------------------------------------------
	// FlushCache copies a single flushable texture from VidRAM to system Memory
	// No real logic yet, just find the first unused texture and swap it.
	// DO NOT want to implement a priority system at all.
	// Returns FALSE if no texture can be cached out this frame.
	bool flushCache(void);

	//-----------------------------------------------------------------------
	// This routine will run through the TXM Cache on a regular basis and free
	// up GOS Handles which haven't been used in some time.  Some Time TBD
	// more accurately with time.
	uint32_t update(void);

	bool checkCacheHeap(void)
	{
		if (textureCacheHeap->totalCoreLeft() <= (TEXTURE_CACHE_SIZE - MAX_CACHE_SIZE))
		{
			return false;
		}
		return true;
	}
};

//----------------------------------------------------------------------
extern MC_TextureManager* mcTextureManager;

//----------------------------------------------------------------------
#endif