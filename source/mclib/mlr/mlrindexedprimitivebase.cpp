//===========================================================================//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#include "stdinc.h"
#include "mlr/mlrindexedprimitivebase.h"

namespace MidLevelRenderer {

std::vector<uint16_t> clipExtraIndex;
std::vector<uint16_t> indexOffset;

//#############################################################################
//#####################    MLRIndexedPrimitiveBase    #########################
//#############################################################################

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//

#if CONSIDERED_OBSOLETE

MLRIndexedPrimitiveBase::ClassData* MLRIndexedPrimitiveBase::DefaultData = nullptr;

void
MLRIndexedPrimitiveBase::InitializeClass()
{
	_ASSERT(!DefaultData);
	// _ASSERT(gos_GetCurrentHeap() == StaticHeap);
	DefaultData = new ClassData(MLRIndexedPrimitiveBaseClassID,
		"MidLevelRenderer::MLRIndexedPrimitiveBase", MLRPrimitiveBase::DefaultData, nullptr);
	Register_Object(DefaultData);
	// clipExtraIndex = new std::vector<uint16_t>(Limits::Max_Number_Vertices_Per_Mesh);
	// Register_Pointer(clipExtraIndex);
	// indexOffset = new uint16_t[Limits::Max_Number_Vertices_Per_Mesh + 1];
	// Register_Pointer(indexOffset);
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
void
MLRIndexedPrimitiveBase::TerminateClass()
{
	Unregister_Pointer(clipExtraIndex);
	delete clipExtraIndex;
	// Unregister_Pointer(indexOffset);
	// delete[] indexOffset;
	Unregister_Object(DefaultData);
	delete DefaultData;
	DefaultData = nullptr;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
MLRIndexedPrimitiveBase::MLRIndexedPrimitiveBase(
	ClassData* class_data, std::iostream stream, uint32_t version) :
	MLRPrimitiveBase(class_data, stream, version)
{
	// Check_Pointer(this);
	Check_Object(stream);
	// _ASSERT(gos_GetCurrentHeap() == Heap);
	switch (version)
	{
	case 1:
	case 2:
	{
		STOP(("This class got created only after version 2 !"));
	}
	break;
	default:
	{
		MemoryStreamIO_Read(stream, &colorIndexes);
	}
	break;
	}
	visibleIndexedVerticesKey = false;
	visibleIndexedVertices.SetLength(coords.GetLength());
}

#endif

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
void
MLRIndexedPrimitiveBase::Save(std::iostream stream)
{
	// Check_Object(this);
	Check_Object(stream);
	MLRPrimitiveBase::Save(stream);
	MemoryStreamIO_Write(stream, &colorIndexes);
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
MLRIndexedPrimitiveBase::MLRIndexedPrimitiveBase(ClassData* class_data) :
	MLRPrimitiveBase(class_data), colorIndexes(0)
{
	// _ASSERT(gos_GetCurrentHeap() == Heap);
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
MLRIndexedPrimitiveBase::~MLRIndexedPrimitiveBase() {}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
void
MLRIndexedPrimitiveBase::TestInstance(void) const
{
	_ASSERT(IsDerivedFrom(DefaultData));
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
void
MLRIndexedPrimitiveBase::InitializeDrawPrimitive(uint8_t vis, int32_t parameter)
{
	MLRPrimitiveBase::InitializeDrawPrimitive(vis, parameter);
	// gos_indices = nullptr;
	numGOSIndices = 0; // = -1;
	visibleIndexedVerticesKey = false;
	// size_t i, len = visibleIndexedVertices.size();
	for (size_t i = 0u; i < visibleIndexedVertices.size(); i++)
	{
		visibleIndexedVertices[i] = 0;
	}
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
void
MLRIndexedPrimitiveBase::SetCoordData(const Point3D* data, size_t dataSize)
{
	// Check_Object(this);
	Check_Pointer(data);
	// _ASSERT(gos_GetCurrentHeap() == Heap);
	_ASSERT(texCoords.GetLength() == 0 || dataSize == texCoords.GetLength());
#if defined(MAX_NUMBER_VERTICES)
	_ASSERT(dataSize <= MAX_NUMBER_VERTICES);
#endif
	coords.AssignData(data, dataSize);
	if (colorIndexes.GetLength() > size_t(0) && visibleIndexedVertices.GetLength() != (size_t)dataSize)
	{
		visibleIndexedVertices.SetLength(dataSize);
	}
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
void
MLRIndexedPrimitiveBase::SetIndexData(puint16_t index_array, int32_t index_count)
{
	// Check_Object(this);
	Check_Pointer(index_array);
	// _ASSERT(gos_GetCurrentHeap() == Heap);
	if (coords.GetLength() > 0)
	{
		visibleIndexedVertices.SetLength(coords.GetLength());
	}
#ifdef _ARMOR
	int32_t len = coords.GetLength();
	for (size_t i = 0; i < index_count; i++)
	{
		_ASSERT(index_array[i] < len);
	}
#endif
	colorIndexes.AssignData(index_array, index_count);
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
void
MLRIndexedPrimitiveBase::GetIndexData(puint16_t* index_array, pint32_t index_count)
{
	// Check_Object(this);
	*index_array = colorIndexes.GetData();
	*index_count = colorIndexes.GetLength();
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
void
MLRIndexedPrimitiveBase::Transform(Matrix4D* mat)
{
	// Check_Object(this);
	int32_t i, len = coords.GetLength();
	for (i = 0; i < len; i++)
	{
		(*transformedCoords)[i].Multiply(coords[i], *mat);
	}
#ifdef LAB_ONLY
	Set_Statistic(TransformedVertices, TransformedVertices + len);
#endif
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
bool
MLRIndexedPrimitiveBase::CheckIndicies()
{
	for (size_t i = 0; i < numGOSIndices; i++)
	{
		if (gos_indices[i] >= numGOSVertices)
		{
			STOP(("Invalid indicies detected !"));
		}
	}
	return true;
}

} // namespace MidLevelRenderer
