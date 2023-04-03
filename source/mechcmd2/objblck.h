//---------------------------------------------------------------------------
//
// ObjBlck.h -- File contains class definitions for the Terrain Object classes
//
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#pragma once

#ifndef OBJBLCK_H
#define OBJBLCK_H
//---------------------------------------------------------------------------
// Include Files
#include "dstd.h"
#include "dobjblck.h"
#include "dobjtype.h"
#include <dpacket.h>
#include "dheap.h"
#include <cvmath.h>
#include <dobjque.h>

//---------------------------------------------------------------------------
// Macro Definitions
#ifndef NO_ERROR
#define NO_ERROR 0
#endif

//---------------------------------------------------------
// This number is the number of reserved objects per block
// There are always this many entries in the block.
// This is so that blocks do not change size so packet file
// is easier to deal with.  Number is not fixed in stone.
// Blocks should compress really well since the objdata
// for empty slots should repeat often.
#define MAX_OBJECTS_PER_BLOCK 200

// Error codes
// Starts with 0xBAAA0014

#define NO_RAM_FOR_TERRAIN_OBJECT_FILE 0xBAAA0014
#define NO_RAM_FOR_TERRAIN_OBJECT_HEAP 0xBAAA0015
#define NO_RAM_FOR_OBJECT_BLOCK_NUM 0xBAAA0016
#define NO_RAM_FOR_OBJECT_LISTS 0xBAAA0017
#define NO_RAM_FOR_OBJECT_DATA_BLOCK 0xBAAA0018
#define NO_RAM_FOR_OBJECT_BLOCK_USER 0xBAAA0019
#define NO_RAM_FOR_LAST_BLOCK 0xBAAA001A
#define NO_RAM_FOR_CENTER_BLOCK 0xBAAA001B
#define OBJECTBLOCK_OUTOFRANGE 0xBAAA001C
#define NO_AVAILABLE_OBJQUEUE 0xBAAA001D
#define COULDNT_MAKE_TERRAIN_OBJECT 0xBAAA001E
#define OBJECTBLOCK_NULL 0xBAAA001F
#define BLOCK_NOT_CACHED 0xBAAA0020
#define COULDNT_CREATE_OBJECT 0xBAAA0021
#define OBJECT_NOT_FOUND 0xBAAA0022

//---------------------------------------------------------------------------
// Class Definitions
#pragma pack(1)
#ifdef TERRAINEDIT
struct OldObjData
{
	ObjectTypeNumber objTypeNum; // Type number of object
	int16_t pixelOffsetX; // Distance from vertex 0 of tile.
	int16_t pixelOffsetY; // Distance from vertex 0 of tile.
	int16_t vertexNumber; // Vertex Number in Block.
	int16_t blockNumber; // Which terrain Block.
	uint32_t damage; // Damage
	float positionX; // Where, physically is object.  TOO DAMNED HARD TO
		// CALCULATE!!!!!!!!
	float positionY;
};
#endif

struct ObjData
{
	int16_t objTypeNum; // Type number of object
	uint16_t vertexNumber; // Vertex Number in Block.
	uint16_t blockNumber; // Which terrain Block.
	uint8_t damage; // Damage
};

struct MiscObjectData
{
	int32_t blockNumber; // Terrain Block I occupy
	int32_t vertexNumber; // Terrain Vertex I occupy
	int32_t objectTypeNum; // ObjectTypeNumber for this overlay tile
	int32_t damaged; // Is this overlay tile damaged or not
};

#pragma pack()

typedef ObjData* ObjDataPtr;
//---------------------------------------------------------------------------
class ObjectBlockManager
{
	// Data Members
	//-------------
protected:
	uint32_t terrainObjectHeapSize; // Size of TerrainObject Heap
	UserHeapPtr terrainObjectHeap; // Pointer to Heap.

	ObjectQueueNodePtr* objectQueues; // Array of QueueNode Ptrs for each
	// Active block of terrain.

	uint32_t numObjectsInDataBlock; // Number of objects to be read/written
	ObjDataPtr objDataBlock; // Block of object data to read/write

	PacketFilePtr objectDataFile; // Packet file with blocks in it.

	// Member Functions
	//-----------------
protected:
	int32_t setupObjectQueue(uint32_t blockNum, uint32_t blockSize);

public:
	void init(void)
	{
		terrainObjectHeapSize = 0;
		terrainObjectHeap = nullptr;
		objectQueues = nullptr;
		numObjectsInDataBlock = 0;
		objDataBlock = nullptr;
		objectDataFile = nullptr;
	}

	ObjectBlockManager(void)
	{
		init(void);
	}

	void destroy(void);

	int32_t init(std::wstring_view packetFileName);

	~ObjectBlockManager(void)
	{
		destroy(void);
	}

	PacketFilePtr getObjectDataFile(void)
	{
		return (objectDataFile);
	}

	int32_t update(BOOL createAll = FALSE);

	void updateAllObjects(void);

	void destroyAllObjects(void);

	ObjectQueueNodePtr getObjectList(int32_t idNum)
	{
		return objectQueues[idNum];
	}

#ifdef TERRAINEDIT
	int32_t addObject(ObjectTypeNumber objNum, vector_2d& pOffset, vector_2d& numbers,
		vector_3d& position, int32_t dmg = 0, int32_t expTime = -1);
	int32_t removeObject(BaseObjectPtr deadObject);
#endif
};

//---------------------------------------------------------------------------
#endif
