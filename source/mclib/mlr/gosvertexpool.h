//===========================================================================//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#pragma once

#ifndef MLR_GOSVERTEXPOOL_HPP
#define MLR_GOSVERTEXPOOL_HPP

//#include <stuff/marray.hpp>
#include <mlr/gosvertex.hpp>
#include <mlr/gosvertex2uv.hpp>

extern bool MLRVertexLimitReached;

namespace MidLevelRenderer
{

//##########################################################################
//######################    GOSVertexPool    ###############################
//##########################################################################

class GOSVertexPool
{
public:
	GOSVertexPool(void);

	void Reset(void);

	//
	//------------------------------------------------------------------------------------------------------------
	//
	size_t GetLength(void)
	{
		// Check_Object(this);
		return vertices.GetLength() - 1;
	}

	uint32_t GetLast(void)
	{
		// Check_Object(this);
		return lastUsed;
	}

	uint32_t Increase(uint32_t add = 1)
	{
		// Check_Object(this);
		lastUsed += add;
		_ASSERT(lastUsed < Limits::Max_Number_Vertices_Per_Frame);
		MLRVertexLimitReached = (lastUsed > (Limits::Max_Number_Vertices_Per_Frame - 2000));
		return lastUsed;
	}

	GOSVertex* GetActualVertexPool(bool db = false)
	{
		// Check_Object(this);
		if (db)
		{
			return verticesDB.GetData();
		}
		else
		{
			return (GOSVertex*)((PSTR)(vertices.GetData() + lastUsed) + vertexAlignment);
		}
	}

	//
	//------------------------------------------------------------------------------------------------------------
	//
	size_t GetLength2UV(void)
	{
		// Check_Object(this);
		return vertices2uv.GetLength() - 1;
	}

	uint32_t GetLast2UV(void)
	{
		// Check_Object(this);
		return lastUsed2uv;
	}

	uint32_t Increase2UV(uint32_t add = 1)
	{
		// Check_Object(this);
		lastUsed2uv += add;
		_ASSERT(lastUsed2uv < Limits::Max_Number_Vertices_Per_Frame);
		return lastUsed2uv;
	}

	GOSVertex2UV* GetActualVertexPool2UV(bool db = false)
	{
		// Check_Object(this);
		if (db)
		{
			return vertices2uvDB.GetData();
		}
		else
		{
			return (GOSVertex2UV*)((PSTR)(vertices2uv.GetData() + lastUsed2uv) + vertexAlignment);
		}
	}

	//
	//------------------------------------------------------------------------------------------------------------
	//
	uint32_t GetLastIndex(void)
	{
		// Check_Object(this);
		return lastUsedIndex;
	}

	uint32_t IncreaseIndex(uint32_t add = 1)
	{
		// Check_Object(this);
		lastUsedIndex += add;
		_ASSERT(lastUsedIndex < Limits::Max_Number_Vertices_Per_Frame);
		return lastUsedIndex;
	}

	puint16_t GetActualIndexPool(bool usedb = false)
	{
		// Check_Object(this);
		if (usedb)
		{
			return indicesDB.GetData();
		}
		else
		{
			return (puint16_t)((char*)(indices.GetData() + lastUsedIndex) + indicesAlignment);
		}
	}

	//
	//------------------------------------------------------------------------------------------------------------
	//
	void TestInstance(void)
	{
		_ASSERT(lastUsed < Limits::Max_Number_Vertices_Per_Frame);
		_ASSERT(lastUsed2uv < Limits::Max_Number_Vertices_Per_Frame);
		_ASSERT(lastUsedIndex < Limits::Max_Number_Vertices_Per_Frame);
	}

protected:
	uint32_t lastUsed;
	uint32_t lastUsed2uv;
	uint32_t lastUsedIndex;
	uint32_t vertexAlignment;
	uint32_t vertexAlignment2uv;
	uint32_t indicesAlignment;

	std::vector<GOSVertex> vertices;		// Max_Number_Vertices_Per_Frame+4*Max_Number_ScreenQuads_Per_Frame
	std::vector<GOSVertex2UV> vertices2uv;	// , Max_Number_Vertices_Per_Frame+4*Max_Number_ScreenQuads_Per_Frame
	std::vector<uint16_t> indices;			// , Max_Number_Vertices_Per_Frame

	std::vector<GOSVertex> verticesDB;		// , Max_Number_Vertices_Per_Mesh
	std::vector<GOSVertex2UV> vertices2uvDB;// , Max_Number_Vertices_Per_Mesh
	std::vector<uint16_t> indicesDB;		// , Max_Number_Vertices_Per_Mesh

private:
	GOSVertexPool(const GOSVertexPool&);
};

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
} // namespace MidLevelRenderer
#endif
