//---------------------------------------------------------------------
//
// crater.h -- Crater/footprint manager for MechCommander 2
//
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#ifndef CRATER_H
#define CRATER_H
//---------------------------------------------------------------------
// Include Files
#ifndef HEAP_H
#include "heap.h"
#endif

#ifndef PACKET_H
#include "packet.h"
#endif


#include <stuff/stuff.hpp>

//---------------------------------------------------------------------
// Macro Definitions
enum craterTypes
{
	INVALID_CRATER = -1,
	SML_FOOTPRINT = 0,
	AVG_FOOTPRINT,
	BIG_FOOTPRINT,
	ULLER_FOOTPRINT,
	MADCAT_FOOTPRINT,
	LEFT_MASAKARI_FOOTPRINT,
	RIGHT_MASAKARI_FOOTPRINT,
	BUSH_FOOTPRINT,
	NOVACAT_FOOTPRINT,
	TURKINA_FOOTPRINT = 63,
	CRATER_1 = 64,
	CRATER_2,
	CRATER_3,
	CRATER_4,
	MAX_CRATER_SHAPES
};

#define FOOTPRINT_ROTATIONS		16
#define BIG_CRATER_OFFSET		0
#define SMALL_CRATER_OFFSET		1
//---------------------------------------------------------------------
// struct CraterData
typedef struct _CraterData
{
	int32_t			craterShapeId;
	Stuff::Vector3D position[4];
	Stuff::Vector4D screenPos[4];
} CraterData;

typedef CraterData* CraterDataPtr;
//---------------------------------------------------------------------
// class CraterManager
class CraterManager
{
	//Data Members
	//-------------
protected:

	HeapManagerPtr		craterPosHeap;
	UserHeapPtr			craterShpHeap;

	uint32_t		craterPosHeapSize;
	uint32_t		craterShpHeapSize;

	PacketFilePtr		craterFile;

	uint32_t		maxCraters;
	uint32_t		currentCrater;
	CraterDataPtr		craterList;
	int32_t				numCraterTextures;

	uint32_t*				craterTextureIndices;
	uint32_t*				craterTextureHandles;

	//Member Functions
	//-----------------
protected:

public:

	void init(void)
	{
		craterPosHeap = nullptr;
		craterShpHeap = nullptr;
		craterPosHeapSize = craterShpHeapSize = 0;
		currentCrater = maxCraters = 0;
		craterList = nullptr;
		numCraterTextures = 0;
		craterTextureHandles = nullptr;
		craterTextureIndices = nullptr;
		craterFile = nullptr;
	}

	CraterManager(void)
	{
		init(void);
	}

	int32_t init(int32_t numCraters, uint32_t craterTypeSize, PSTR craterFileName);

	~CraterManager(void)
	{
		destroy(void);
	}

	void destroy(void);

	int32_t addCrater(int32_t craterType, Stuff::Vector3D& position, float rotation);

	int32_t update(void);
	void render(void);
};

//---------------------------------------------------------------------
typedef CraterManager* CraterManagerPtr;
extern CraterManagerPtr craterManager;

//---------------------------------------------------------------------
#endif