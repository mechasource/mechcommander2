//===========================================================================//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#pragma once

#ifndef MLR_MLRINDEXEDTRIANGLECLOUD_HPP
#define MLR_MLRINDEXEDTRIANGLECLOUD_HPP

#include "mlr/mlrtrianglecloud.h"

namespace MidLevelRenderer
{

//##########################################################################
//##################    MLRIndexedTriangleCloud    #########################
//##########################################################################

class MLRIndexedTriangleCloud : public MLRTriangleCloud
{
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Initialization
	//
public:
	static void __stdcall InitializeClass(void);
	static void __stdcall TerminateClass(void);

	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Constructors/Destructors
	//
public:
	MLRIndexedTriangleCloud(uint32_t nr)
		: MLRTriangleCloud(nr)
	{
		// _ASSERT(gos_GetCurrentHeap() == Heap);
		usedNrOfPoints = nullptr;
		// Check_Pointer(this);
		drawMode = SortData::TriIndexedList;
	}
	~MLRIndexedTriangleCloud(void)
	{
		// Check_Object(this);
	}

	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	//
	MLRIndexedTriangleCloud::MLRIndexedTriangleCloud(uint32_t nr)
		: MLRTriangleCloud(nr)
	{
		// _ASSERT(gos_GetCurrentHeap() == Heap);
		usedNrOfPoints = nullptr;
		// Check_Pointer(this);
		drawMode = SortData::TriIndexedList;
	}

	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	//
	MLRIndexedTriangleCloud::~MLRIndexedTriangleCloud(void)

		void SetData(const size_t* tri_count, const size_t* point_count, const uint16_t* index_data,
			const Stuff::Point3D* point_data, const Stuff::RGBAcolour* color_data,
			const Stuff::Vector2DScalar* uv_data);

	void Draw(DrawEffectInformation*, GOSVertexPool*, MLRSorter*);

	uint32_t Clip(MLRClippingState, GOSVertexPool*);

	virtual uint16_t* GetGOSIndices(int32_t = 0)
	{
		// Check_Object(this);
		return gos_indices;
	}

	int32_t GetNumGOSIndices()
	{
		// Check_Object(this);
		return numGOSIndices;
	}

	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Class Data Support
	//
public:
	static ClassData* DefaultData;

	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Testing
	//
public:
	void TestInstance(void) const;

protected:
	const size_t* usedNrOfPoints;
	const uint16_t* index;
	const Stuff::Vector2DScalar* texCoords;

	static std::vector<Stuff::Vector2DScalar>*
		clipExtraTexCoords; // , Max_Number_Vertices_Per_Mesh
	static std::vector<uint16_t>* clipExtraIndex; // , Max_Number_Vertices_Per_Mesh
	static std::vector<uint8_t>* visibleIndexedVertices;

	uint16_t* gos_indices;
	uint16_t numGOSIndices;
};
} // namespace MidLevelRenderer
#endif
