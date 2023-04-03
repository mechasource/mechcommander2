//===========================================================================//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#include "stdinc.h"

#include "mlr/mlr_terrain.h"

namespace MidLevelRenderer
{

//#############################################################################

#if defined(TRACE_ENABLED) && defined(MLR_TRACE)
BitTrace* MLR_Terrain_Clip;
#endif

//#############################################################################
//## MLRTerrain with no color no lighting w/ detail texture, uv's from xyz ###
//#############################################################################

MLR_Terrain::ClassData* MLR_Terrain::DefaultData = nullptr;

std::vector<Stuff::Vector2DScalar>* MLR_Terrain::clipTexCoords;

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
void MLR_Terrain::InitializeClass()
{
	_ASSERT(!DefaultData);
	// _ASSERT(gos_GetCurrentHeap() == StaticHeap);
	DefaultData = new ClassData(MLR_TerrainClassID, "MidLevelRenderer::MLR_Terrain",
		MLR_I_DeT_TMesh::DefaultData, (MLRPrimitiveBase::Factory)&Make);
	Register_Object(DefaultData);
	clipTexCoords = new std::vector<Stuff::Vector2DScalar>(Limits::Max_Number_Vertices_Per_Mesh);
	Register_Object(clipTexCoords);
#if defined(TRACE_ENABLED) && defined(MLR_TRACE)
	MLR_Terrain_Clip = new BitTrace("MLR_Terrain_Clip");
	Register_Object(MLR_Terrain_Clip);
#endif
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
void MLR_Terrain::TerminateClass()
{
	Unregister_Object(DefaultData);
	delete DefaultData;
	DefaultData = nullptr;
	Unregister_Object(clipTexCoords);
	delete clipTexCoords;
#if defined(TRACE_ENABLED) && defined(MLR_TRACE)
	Unregister_Object(MLR_Terrain_Clip);
	delete MLR_Terrain_Clip;
#endif
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
MLR_Terrain::MLR_Terrain(ClassData* class_data, std::iostream stream, uint32_t version)
	: MLR_I_DeT_TMesh(class_data, stream, version)
{
	// Check_Pointer(this);
	Check_Pointer(stream);
	// _ASSERT(gos_GetCurrentHeap() == Heap);
	texCoords.SetLength(0);
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
MLR_Terrain::MLR_Terrain(ClassData* class_data)
	: MLR_I_DeT_TMesh(class_data)
{
	// Check_Pointer(this);
	// _ASSERT(gos_GetCurrentHeap() == Heap);
	texCoords.SetLength(0);
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
MLR_Terrain::~MLR_Terrain()
{
	// Check_Object(this);
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
MLR_Terrain*
MLR_Terrain::Make(std::iostream stream, uint32_t version)
{
	Check_Object(stream);
#ifdef _GAMEOS_HPP_
	gos_PushCurrentHeap(Heap);
#endif
	MLR_Terrain* terrain = new MLR_Terrain(DefaultData, stream, version);
	gos_PopCurrentHeap();
	return terrain;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
void MLR_Terrain::Save(std::iostream stream)
{
	// Check_Object(this);
	Check_Object(stream);
	MLR_I_DeT_TMesh::Save(stream);
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
void MLR_Terrain::TestInstance(void) const
{
	_ASSERT(IsDerivedFrom(DefaultData));
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
void MLR_Terrain::SetUVData(float bpf, float xmin, float xmax, float zmin, float zmax)
{
	borderPixelFun = bpf;
	minX = xmin;
	minZ = zmin;
	xUVFac = (1.0f - 2 * borderPixelFun) / (xmax - xmin);
	zUVFac = (1.0f - 2 * borderPixelFun) / (zmax - zmin);
}

#define I_SAY_YES_TO_DETAIL_TEXTURES
#undef I_SAY_YES_TO_DUAL_TEXTURES
#undef I_SAY_YES_TO_COLOR
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
#undef I_SAY_YES_TO_LIGHTING

#define I_SAY_YES_TO_TERRAIN

#define CLASSNAME MLR_Terrain

#if defined(TRACE_ENABLED) && defined(MLR_TRACE)
#define SET_MLR_TMESH_CLIP() MLR_Terrain_Clip->Set()
#define CLEAR_MLR_TMESH_CLIP() MLR_Terrain_Clip->Clear()
#else
#define SET_MLR_TMESH_CLIP()
#define CLEAR_MLR_TMESH_CLIP()
#endif

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This include contains follwing functions:
// void MLR_Terrain::TransformNoClip(Matrix4D*, GOSVertexPool*);
// int32_t MLR_Terrain::Clip(MLRClippingState, GOSVertexPool*);
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#include "mlr/mlrtriangleclipping.inl"

#undef I_SAY_YES_TO_DETAIL_TEXTURES

#undef I_SAY_YES_TO_TERRAIN

#undef CLASSNAME

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
MLRShape*
MidLevelRenderer::CreateIndexedTriIcosahedron_TerrainTest(
	IcoInfo& icoInfo, MLRState* state, MLRState* stateDet)
{
#ifdef _GAMEOS_HPP_
	gos_PushCurrentHeap(Heap);
#endif
	MLRShape* ret = new MLRShape(20);
	Register_Object(ret);
	int32_t i, j, k;
	uint32_t nrTri = static_cast<uint32_t>(ceil(icoInfo.all * pow(4.0f, icoInfo.depth)));
	Point3D v[3];
	if (3 * nrTri >= Limits::Max_Number_Vertices_Per_Mesh)
	{
		nrTri = Limits::Max_Number_Vertices_Per_Mesh / 3;
	}
	Point3D* coords = new Point3D[nrTri * 3];
	Register_Pointer(coords);
	Point3D* collapsedCoords = nullptr;
	if (icoInfo.indexed == true)
	{
		collapsedCoords = new Point3D[nrTri * 3];
		Register_Pointer(collapsedCoords);
	}
	uint16_t* index = new uint16_t[nrTri * 3];
	Register_Pointer(index);
	int32_t uniquePoints = 0;
	for (k = 0; k < 20; k++)
	{
		triDrawn = 0;
		MLR_Terrain* mesh = new MLR_Terrain();
		Register_Object(mesh);
		mesh->SetUVData(2.0f, -1.0f, 1.0f, -1.0f, 1.0f);
		// setup vertex position information
		for (j = 0; j < 3; j++)
		{
			v[j].x = vdata[tindices[k][j]][0];
			v[j].y = vdata[tindices[k][j]][1];
			v[j].z = vdata[tindices[k][j]][2];
		}
		subdivide(coords, v[0], v[1], v[2], icoInfo.depth, nrTri, icoInfo.radius);
		mesh->SetSubprimitiveLengths(nullptr, nrTri);
		if (icoInfo.indexed == true)
		{
			uniquePoints = 1;
			collapsedCoords[0] = coords[0];
			index[0] = 0;
			for (i = 1; i < nrTri * 3; i++)
			{
				for (j = 0; j < uniquePoints; j++)
				{
					if (coords[i] == collapsedCoords[j])
					{
						break;
					}
				}
				if (j == uniquePoints)
				{
					collapsedCoords[uniquePoints++] = coords[i];
				}
				index[i] = static_cast<uint16_t>(j);
			}
			mesh->SetCoordData(collapsedCoords, uniquePoints);
		}
		else
		{
			uniquePoints = nrTri * 3;
			for (i = 0; i < nrTri * 3; i++)
			{
				index[i] = static_cast<uint16_t>(i);
			}
			mesh->SetCoordData(coords, nrTri * 3);
		}
		mesh->SetIndexData(index, nrTri * 3);
		mesh->FindFacePlanes();
		if (state != nullptr)
		{
			mesh->SetReferenceState(*state);
		}
		mesh->SetDetailData(0.0f, 0.0f, 16.0f, 16.0f, 50.0f, 70.0f);
		mesh->SetReferenceState(*stateDet, 1);
		ret->Add(mesh);
		mesh->DetachReference();
	}
	Unregister_Pointer(index);
	delete[] index;
	if (icoInfo.indexed == true)
	{
		Unregister_Pointer(collapsedCoords);
		delete[] collapsedCoords;
	}
	Unregister_Pointer(coords);
	delete[] coords;
	gos_PopCurrentHeap();
	return ret;
}

} // namespace MidLevelRenderer
