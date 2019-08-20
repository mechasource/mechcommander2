//===========================================================================//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#pragma once

#ifndef MLR_MLRINDEXEDPRIMITIVEBASE_HPP
#define MLR_MLRINDEXEDPRIMITIVEBASE_HPP

#include <mlr/mlrprimitivebase.hpp>

namespace MidLevelRenderer
{

extern std::vector<uint16_t> clipExtraIndex;
extern std::vector<uint16_t> indexOffset;

//##########################################################################
//################### MLRIndexedPrimitiveBase ########################
//##########################################################################

class MLRIndexedPrimitiveBase : public MLRPrimitiveBase
{
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Initialization
	//
public:
#if _CONSIDERED_OBSOLETE
	static void __stdcall InitializeClass(void);
	static void __stdcall TerminateClass(void);
	static ClassData* DefaultData;
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Constructors/Destructors
	//
protected:
	MLRIndexedPrimitiveBase(ClassData* class_data, std::istream& stream, uint32_t version);
	MLRIndexedPrimitiveBase(ClassData* class_data);
#endif

protected:
	~MLRIndexedPrimitiveBase(void);

public:
	static MLRIndexedPrimitiveBase* Make(std::istream& stream, uint32_t version);
	virtual void Save(std::ostream& stream);

	virtual void InitializeDrawPrimitive(uint8_t, int32_t = 0);

	virtual void Lighting(MLRLight* const*, uint32_t nrLights) = 0;

	virtual void SetCoordData(const Stuff::Point3D* array, size_t point_count);
	virtual void SetIndexData(puint16_t index_array, size_t index_count);
	virtual void GetIndexData(puint16_t* index_array, psize_t index_count);

	virtual std::vector<uint16_t>& GetGOSIndices(uint16_t = 0)
	{
		// Check_Object(this);
		return gos_indices;
	}

	uint16_t GetNumGOSIndices(void)
	{
		// Check_Object(this);
		return numGOSIndices;
	}

	virtual void Transform(Stuff::Matrix4D*);

	virtual void TransformNoClip(Stuff::Matrix4D*, GOSVertexPool*, bool = false) = 0;

	void TheIndexer(size_t num)
	{
		// Check_Object(this);
		colorIndexes.resize(num);
		for (uint16_t i = 0u; i < num; i++)
		{
			colorIndexes[i] = i;
		}
	}

	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Testing
	//
public:
	void TestInstance(void) const;

	virtual size_t GetSize(void)
	{
		// Check_Object(this);
		size_t ret = MLRPrimitiveBase::GetSize();
		ret += visibleIndexedVertices.size();
		ret += colorIndexes.size();
		return ret;
	}

	bool CheckIndicies(void);

protected:
	bool visibleIndexedVerticesKey;
	std::vector<uint8_t> visibleIndexedVertices;
	std::vector<uint16_t> colorIndexes; // List of color indexes
	std::vector<uint16_t> gos_indices;

	//static std::vector<uint16_t> clipExtraIndex;	// , Max_Number_Vertices_Per_Mesh
	uint16_t numGOSIndices;
};
} // namespace MidLevelRenderer
#endif
