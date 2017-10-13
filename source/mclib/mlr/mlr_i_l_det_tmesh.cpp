//===========================================================================//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#include "stdafx.h"

#include <gameos.hpp>
#include <mlr/mlrshape.hpp>
#include <mlr/mlr_i_l_tmesh.hpp>
#include <mlr/mlr_i_l_det_pmesh.hpp>
#include <mlr/mlr_i_l_det_tmesh.hpp>

using namespace MidLevelRenderer;

//#############################################################################

#if defined(TRACE_ENABLED) && defined(MLR_TRACE)
BitTrace* MLR_I_L_DeT_TMesh;
#endif

//#############################################################################
//###### MLRIndexedTriMesh with color but no lighting one texture layer  ######
//#############################################################################

MLR_I_L_DeT_TMesh::ClassData* MLR_I_L_DeT_TMesh::DefaultData = nullptr;

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
void MLR_I_L_DeT_TMesh::InitializeClass(void)
{
	Verify(!DefaultData);
	// Verify(gos_GetCurrentHeap() == StaticHeap);
	DefaultData = new ClassData(
		MLR_I_L_DeT_TMeshClassID, "MidLevelRenderer::MLR_I_L_DeT_TMesh",
		MLR_I_C_DeT_TMesh::DefaultData, (MLRPrimitiveBase::Factory)&Make);
	Register_Object(DefaultData);
#if defined(TRACE_ENABLED) && defined(MLR_TRACE)
	MLR_I_L_DeT_TMesh = new BitTrace("MLR_I_L_DeT_TMesh");
	Register_Object(MLR_I_L_DeT_TMesh);
#endif
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
void
MLR_I_L_DeT_TMesh::TerminateClass()
{
	Unregister_Object(DefaultData);
	delete DefaultData;
	DefaultData = nullptr;

#if defined(TRACE_ENABLED) && defined(MLR_TRACE)
	Unregister_Object(MLR_I_L_DeT_TMesh);
	delete MLR_I_L_DeT_TMesh;
#endif
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
MLR_I_L_DeT_TMesh::MLR_I_L_DeT_TMesh(
	ClassData* class_data,
	Stuff::MemoryStream* stream,
	uint32_t version
):
	MLR_I_C_DeT_TMesh(class_data, stream, version)
{
	//Check_Pointer(this);
	Check_Pointer(stream);
	//Verify(gos_GetCurrentHeap() == Heap);
	switch(version)
	{
		case 1:
		case 2:
		{
#ifdef _GAMEOS_HPP_
			STOP(("This class got created only after version 2 !"));
#endif
		}
		break;
		default:
		{
			MemoryStreamIO_Read(stream, &normals);
		}
		break;
	}
	litColors.SetLength(colors.GetLength());
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
MLR_I_L_DeT_TMesh::MLR_I_L_DeT_TMesh(ClassData* class_data):
	MLR_I_C_DeT_TMesh(class_data), normals(0)
{
	//Check_Pointer(this);
	//Verify(gos_GetCurrentHeap() == Heap);
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
MLR_I_L_DeT_TMesh::~MLR_I_L_DeT_TMesh()
{
	// Check_Object(this);
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
MLR_I_L_DeT_TMesh*
MLR_I_L_DeT_TMesh::Make(
	Stuff::MemoryStream* stream,
	uint32_t version
)
{
	Check_Object(stream);
#ifdef _GAMEOS_HPP_
	gos_PushCurrentHeap(Heap);
#endif
	MLR_I_L_DeT_TMesh* mesh = new MLR_I_L_DeT_TMesh(DefaultData, stream, version);
#ifdef _GAMEOS_HPP_
	gos_PopCurrentHeap();
#endif

	return mesh;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
void
MLR_I_L_DeT_TMesh::Save(Stuff::MemoryStream* stream)
{
	// Check_Object(this);
	Check_Object(stream);
	MLR_I_C_DeT_TMesh::Save(stream);
	MemoryStreamIO_Write(stream, &normals);
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
bool
MLR_I_L_DeT_TMesh::Copy(MLR_I_L_DeT_PMesh* pMesh)
{
	//Check_Pointer(this);
	Check_Object(pMesh);
	size_t len;
	Stuff::Vector3D* _normals;
	MLR_I_C_DeT_TMesh::Copy(pMesh);
	pMesh->GetNormalData(&_normals, &len);
	SetNormalData(_normals, len);
	return true;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
void MLR_I_L_DeT_TMesh::Copy(
	MLR_I_L_TMesh* tMesh, MLRState detailState, 
	float xOff, float yOff, float xFac, float yFac)
{
	// Check_Object(this);
	Check_Object(tMesh);
	//Verify(gos_GetCurrentHeap() == Heap);
	size_t len;
	Stuff::Vector3D* _normals;
	MLR_I_C_DeT_TMesh::Copy(tMesh, detailState, xOff, yOff, xFac, yFac);
	tMesh->GetNormalData(&_normals, &len);
	SetNormalData(_normals, len);
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
void
MLR_I_L_DeT_TMesh::SetNormalData(
	const Stuff::Vector3D* data,
	size_t dataSize
)
{
	// Check_Object(this);
	Check_Pointer(data);
	Verify(coords.GetLength() == 0 || dataSize == coords.GetLength());
	Verify(colors.GetLength() == 0 || dataSize == colors.GetLength());
	Verify(texCoords.GetLength() == 0 || dataSize == texCoords.GetLength());
	normals.AssignData(data, dataSize);
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
void
MLR_I_L_DeT_TMesh::GetNormalData(
	Stuff::Vector3D** data,
	psize_t dataSize
)
{
	// Check_Object(this);
	*data = normals.GetData();
	*dataSize = normals.GetLength();
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
void
MLR_I_L_DeT_TMesh::SetColorData(
#if COLOR_AS_DWORD
	pcuint32_t data,
#else
	const Stuff::RGBAColor* data,
#endif
	size_t dataSize)
{
	// Check_Object(this);
	Check_Pointer(data);
	//Verify(gos_GetCurrentHeap() == Heap);
	Verify(coords.GetLength() == 0 || dataSize == coords.GetLength());
	Verify(texCoords.GetLength() == 0 || dataSize == texCoords.GetLength());
	litColors.SetLength(dataSize);
	colors.AssignData(data, dataSize);
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
void
MLR_I_L_DeT_TMesh::PaintMe(
#if COLOR_AS_DWORD
	pcuint32_t paintMe
#else
	const Stuff::RGBAColor* paintMe
#endif

)
{
	// Check_Object(this);
#if 1
	Verify(colors.GetLength() == litColors.GetLength());
#else
	if(colors.GetLength() == litColors.GetLength())
	{
		litColors.SetLength(colors.GetLength());
	}
#endif
	size_t k, len = litColors.GetLength();
#if COLOR_AS_DWORD
	uint32_t argb = GOSCopyColor(paintMe);
	for(k = 0; k < len; k++)
	{
		litColors[k] = argb;
	}
#else
	for(k = 0; k < len; k++)
	{
		litColors[k] = *paintMe;
	}
#endif
	// set the to use colors to the original colors ...
	// only lighting could overwrite this;
	actualColors = &litColors;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
void
MLR_I_L_DeT_TMesh::TestInstance(void) const
{
	Verify(IsDerivedFrom(DefaultData));
}

extern uint32_t gEnableTextureSort, gEnableAlphaSort;

#define I_SAY_YES_TO_DETAIL_TEXTURES
#undef I_SAY_YES_TO_DUAL_TEXTURES
#define I_SAY_YES_TO_COLOR
#define I_SAY_YES_TO_LIGHTING

#define CLASSNAME MLR_I_L_DeT_TMesh

#if defined(TRACE_ENABLED) && defined(MLR_TRACE)
#define SET_MLR_TMESH_CLIP() MLR_I_L_DeT_TMesh->Set()
#define CLEAR_MLR_TMESH_CLIP() MLR_I_L_DeT_TMesh->Clear()
#else
#define SET_MLR_TMESH_CLIP()
#define CLEAR_MLR_TMESH_CLIP()
#endif

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//	This include contains follwing functions:
//	void MLR_I_L_DeT_TMesh::TransformNoClip(Matrix4D*, GOSVertexPool*);
//	int32_t MLR_I_L_DeT_TMesh::Clip(MLRClippingState, GOSVertexPool*);
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#include <mlr/mlrtriangleclipping.inl>

#undef I_SAY_YES_TO_DETAIL_TEXTURES
#undef I_SAY_YES_TO_COLOR
#undef I_SAY_YES_TO_LIGHTING

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//	This include contains follwing functions:
//	void	Lighting (MLRLight**, int32_t nrLights);
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#include <mlr/mlrtrianglelighting.inl>
#undef CLASSNAME

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
MLRShape*
MidLevelRenderer::CreateIndexedTriIcosahedron_Color_Lit_DetTex(
	IcoInfo& icoInfo,
	MLRState* state,
	MLRState* stateDet
)
{
	#ifdef _GAMEOS_HPP_
	gos_PushCurrentHeap(Heap);
#endif
	MLRShape* ret = new MLRShape(20);
	Register_Object(ret);
	size_t i, j, k;
	uint32_t nrTri = static_cast<uint32_t>(ceil(icoInfo.all * pow(4.0f, icoInfo.depth)));
	Stuff::Point3D v[3];
	if(3 * nrTri >= Limits::Max_Number_Vertices_Per_Mesh)
	{
		nrTri = Limits::Max_Number_Vertices_Per_Mesh / 3;
	}
	Stuff::Point3D* coords = new Stuff::Point3D [nrTri * 3];
	Register_Pointer(coords);
	Stuff::Point3D* collapsedCoords = nullptr;
	if(icoInfo.indexed == true)
	{
		collapsedCoords = new Stuff::Point3D [nrTri * 3];
		Register_Pointer(collapsedCoords);
	}
	puint16_t index = new uint16_t [nrTri * 3];
	Register_Pointer(index);
	Stuff::Vector2DScalar* texCoords = new Stuff::Vector2DScalar[nrTri * 3];
	Register_Pointer(texCoords);
	Stuff::RGBAColor* colors = new Stuff::RGBAColor[nrTri * 3];
	Register_Pointer(colors);
	Stuff::Vector3D* normals = new Stuff::Vector3D[nrTri * 3];
	Register_Pointer(normals);
	uint32_t uniquePoints = 0;
	for(k = 0; k < 20; k++)
	{
		triDrawn = 0;
		MLR_I_L_DeT_TMesh* mesh = new MLR_I_L_DeT_TMesh();
		Register_Object(mesh);
// setup vertex position information
		for(j = 0; j < 3; j++)
		{
			v[j].x = vdata[tindices[k][j]][0];
			v[j].y = vdata[tindices[k][j]][1];
			v[j].z = vdata[tindices[k][j]][2];
		}
		subdivide(coords, v[0], v[1], v[2], icoInfo.depth, nrTri, icoInfo.radius);
		mesh->SetSubprimitiveLengths(nullptr, nrTri);
		if(icoInfo.indexed == true)
		{
			uniquePoints = 1;
			collapsedCoords[0] = coords[0];
			index[0] = 0;
			for(i = 1; i < nrTri * 3; i++)
			{
				for(j = 0; j < uniquePoints; j++)
				{
					if(coords[i] == collapsedCoords[j])
					{
						break;
					}
				}
				if(j == uniquePoints)
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
			for(i = 0; i < nrTri * 3; i++)
			{
				index[i] = static_cast<uint16_t>(i);
			}
			mesh->SetCoordData(coords, nrTri * 3);
		}
		mesh->SetIndexData(index, nrTri * 3);
		mesh->FindFacePlanes();
		if(state == nullptr)
		{
			for(i = 0; i < uniquePoints; i++)
			{
				texCoords[i] = Stuff::Vector2DScalar(0.0f, 0.0f);
			}
		}
		else
		{
			state->SetMultiTextureMode(MLRState::MultiTextureLightmapMode);
			mesh->SetReferenceState(*state);
			state->SetMultiTextureMode(MLRState::MultiTextureOffMode);
			if(state->GetTextureHandle() > 0)
			{
				if(icoInfo.indexed == true)
				{
					for(i = 0; i < uniquePoints; i++)
					{
						texCoords[i] =
							Stuff::Vector2DScalar(
								(1.0f + collapsedCoords[i].x) / 2.0f,
								(1.0f + collapsedCoords[i].y) / 2.0f
							);
					}
				}
				else
				{
					for(i = 0; i < nrTri; i++)
					{
						texCoords[3 * i] =
							Stuff::Vector2DScalar(
								(1.0f + coords[3 * i].x) / 2.0f,
								(1.0f + coords[3 * i].y) / 2.0f
							);
						texCoords[3 * i + 1] =
							Stuff::Vector2DScalar(
								(1.0f + coords[3 * i + 1].x) / 2.0f,
								(1.0f + coords[3 * i + 1].y) / 2.0f
							);
						texCoords[3 * i + 2] =
							Stuff::Vector2DScalar(
								(1.0f + coords[3 * i + 2].x) / 2.0f,
								(1.0f + coords[3 * i + 2].y) / 2.0f
							);
					}
				}
			}
			else
			{
				for(i = 0; i < uniquePoints; i++)
				{
					texCoords[i] = Stuff::Vector2DScalar(0.0f, 0.0f);
				}
			}
		}
		mesh->SetTexCoordData(texCoords, uniquePoints);
		if(icoInfo.indexed == true)
		{
			for(i = 0; i < uniquePoints; i++)
			{
				colors[i] =
					Stuff::RGBAColor(
						(1.0f + collapsedCoords[i].x) / 2.0f,
						(1.0f + collapsedCoords[i].y) / 2.0f,
						(1.0f + collapsedCoords[i].z) / 2.0f,
						1.0f
					);
				normals[i].Normalize(collapsedCoords[i]);
			}
		}
		else
		{
			for(i = 0; i < uniquePoints; i++)
			{
				colors[i] =
					Stuff::RGBAColor(
						(1.0f + coords[i].x) / 2.0f,
						(1.0f + coords[i].y) / 2.0f,
						(1.0f + coords[i].z) / 2.0f,
						1.0f
					);
				normals[i].Normalize(coords[i]);
			}
		}
		mesh->SetNormalData(normals, uniquePoints);
		mesh->SetColorData(colors, uniquePoints);
		mesh->SetDetailData(0.0f, 0.0f, 16.0f, 16.0f, 50.0f, 70.0f);
		mesh->SetReferenceState(*stateDet, 1);
		ret->Add(mesh);
		mesh->DetachReference();
	}
	Unregister_Pointer(normals);
	delete [] normals;
	Unregister_Pointer(colors);
	delete [] colors;
	Unregister_Pointer(texCoords);
	delete [] texCoords;
	Unregister_Pointer(index);
	delete [] index;
	if(icoInfo.indexed == true)
	{
		Unregister_Pointer(collapsedCoords);
		delete [] collapsedCoords;
	}
	Unregister_Pointer(coords);
	delete [] coords;
#ifdef _GAMEOS_HPP_
	gos_PopCurrentHeap();
#endif

	return ret;
}