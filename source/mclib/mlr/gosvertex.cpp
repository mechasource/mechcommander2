//===========================================================================//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#include "stdafx.h"

#include <mlr/gosvertexpool.hpp>
#include <mlr/gosvertex.hpp>

using namespace MidLevelRenderer;

float GOSVertex::farClipReciprocal;

float ViewportScalars::MulX;
float ViewportScalars::MulY;
float ViewportScalars::AddX;
float ViewportScalars::AddY;

#if FOG_HACK
uint8_t GOSVertex::fogTable[Limits::Max_Number_Of_FogStates][1024];
#endif

//#############################################################################
//############################ GOSVertex #################################
//#############################################################################

GOSVertex::GOSVertex()
{
	x = 0.0f;
	y = 0.0f;
	z = 0.0f;
	rhw = 1.0f;
	argb = 0xffffffff;
	u = 0.0f;
	v = 0.0f;
	frgb = 0xffffffff;
}

void
GOSVertex::SetFogTableEntry(int32_t entry, float nearFog, float farFog, float fogDensity)
{
	float Fog;
	Verify(farFog > nearFog);
	entry--;
	GOSVertex::fogTable[entry][0] = 0;
	for(size_t t1 = 0; t1 < 1024; t1++)
	{
		if(0.0f == fogDensity)
		{
			Fog = (farFog - t1) / (farFog - nearFog); // 0.0 = Linear fog table (from Start to End)
		}
		else
		{
			if(fogDensity < 1.0f)
			{
				Fog = (float)exp(-fogDensity * t1); // 0.0->1.0 = FOG_EXP
			}
			else
			{
				Fog = (float)exp(-((fogDensity - 1.0f) * t1) * ((fogDensity - 1.0f) * t1)); // 1.0->2.0 = FOG_EXP2
			}
		}
		if(Fog < 0.0f)
			Fog = 0.0f;
		if(Fog > 1.0f)
			Fog = 1.0f;
		GOSVertex::fogTable[entry][t1] = (uint8_t)(255.9f * Fog);
	}
}

GOSVertexPool::GOSVertexPool(void)
{
	// //Verify(gos_GetCurrentHeap() == Heap);
	lastUsed = 0;
	lastUsed2uv = 0;
	lastUsedIndex = 0;
	// gos_PushCurrentHeap(StaticHeap);
	vertices.SetLength(Limits::Max_Number_Vertices_Per_Frame + 4 * Limits::Max_Number_ScreenQuads_Per_Frame + 1);
	vertices2uv.SetLength(Limits::Max_Number_Vertices_Per_Frame + 4 * Limits::Max_Number_ScreenQuads_Per_Frame + 1);
	indices.SetLength(Limits::Max_Number_Vertices_Per_Frame + 16);
	verticesDB.SetLength(2 * Limits::Max_Number_Vertices_Per_Mesh);
	vertices2uvDB.SetLength(2 * Limits::Max_Number_Vertices_Per_Mesh);
	indicesDB.SetLength(2 * Limits::Max_Number_Vertices_Per_Mesh);
	// gos_PopCurrentHeap();
	vertexAlignment = 32 - ((size_t)vertices.GetData() & 31);
	vertexAlignment2uv = 32 - ((size_t)vertices2uv.GetData() & 31);
	indicesAlignment = 32 - ((size_t)indices.GetData() & 31);
}

void GOSVertexPool::Reset(void)
{
	// Check_Object(this);
	lastUsed = 0;
	lastUsed2uv = 0;
	lastUsedIndex = 0;
	MLRVertexLimitReached = false;
}

