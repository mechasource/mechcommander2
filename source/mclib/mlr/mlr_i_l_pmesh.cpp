//===========================================================================//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#include "stdinc.h"

#include <gameos.hpp>
#include <mlr/gosvertexpool.hpp>
#include <mlr/mlrtexture.hpp>
#include <mlr/mlrtexturepool.hpp>
#include <mlr/mlrshape.hpp>
#include <mlr/mlr_i_l_pmesh.hpp>

using namespace MidLevelRenderer;

//#############################################################################

#if defined(TRACE_ENABLED) && defined(MLR_TRACE)
BitTrace* MLR_I_L_PMesh_Clip;
#endif

//#############################################################################
//######## MLRIndexedPolyMesh with color and lighting one texture layer ######
//#############################################################################

MLR_I_L_PMesh::ClassData* MLR_I_L_PMesh::DefaultData = nullptr;

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
void
MLR_I_L_PMesh::InitializeClass()
{
	_ASSERT(!DefaultData);
	// _ASSERT(gos_GetCurrentHeap() == StaticHeap);
	DefaultData = new ClassData(MLR_I_L_PMeshClassID, "MidLevelRenderer::MLR_I_L_PMesh",
		MLR_I_C_PMesh::DefaultData, (MLRPrimitiveBase::Factory)&Make);
	Register_Object(DefaultData);
#if defined(TRACE_ENABLED) && defined(MLR_TRACE)
	MLR_I_L_PMesh_Clip = new BitTrace("MLR_I_L_PMesh_Clip");
	Register_Object(MLR_I_L_PMesh_Clip);
#endif
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
void
MLR_I_L_PMesh::TerminateClass()
{
	Unregister_Object(DefaultData);
	delete DefaultData;
	DefaultData = nullptr;
#if defined(TRACE_ENABLED) && defined(MLR_TRACE)
	Unregister_Object(MLR_I_L_PMesh_Clip);
	delete MLR_I_L_PMesh_Clip;
#endif
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
MLR_I_L_PMesh::MLR_I_L_PMesh(ClassData* class_data, std::iostream stream, uint32_t version) :
	MLR_I_C_PMesh(DefaultData, stream, version)
{
	// Check_Pointer(this);
	(void)class_data;
	Check_Pointer(stream);
	// _ASSERT(gos_GetCurrentHeap() == Heap);
	switch (version)
	{
	case 1:
	case 2:
	{
		// STOP(("This class got created only after version 2 !"));
	}
	break;
	default:
	{
		MemoryStreamIO_Read(stream, &normals);
	}
	break;
	}
	litColors.SetLength(colors.GetLength());
	actualColors = &colors;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
MLR_I_L_PMesh::MLR_I_L_PMesh(ClassData* class_data) :
	MLR_I_C_PMesh(class_data), normals(0)
{
	// Check_Pointer(this);
	// _ASSERT(gos_GetCurrentHeap() == Heap);
	actualColors = &colors;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/*
void
MLR_I_L_PMesh::Copy(MLRIndexedPolyMesh *polyMesh)
{
//Check_Pointer(this);

size_t len;
Stuff::Vector3D *_normals;

MLR_I_C_PMesh::Copy(polyMesh);

polyMesh->GetNormalData(&_normals, &len);
SetNormalData(_normals, len);
}
*/

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
MLR_I_L_PMesh::~MLR_I_L_PMesh()
{
	// Check_Object(this);
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
MLR_I_L_PMesh*
MLR_I_L_PMesh::Make(std::iostream stream, uint32_t version)
{
	Check_Object(stream);
#ifdef _GAMEOS_HPP_
	gos_PushCurrentHeap(Heap);
#endif
	MLR_I_L_PMesh* mesh = new MLR_I_L_PMesh(DefaultData, stream, version);
	gos_PopCurrentHeap();
	return mesh;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
void
MLR_I_L_PMesh::Save(std::iostream stream)
{
	// Check_Object(this);
	Check_Object(stream);
	MLR_I_C_PMesh::Save(stream);
	MemoryStreamIO_Write(stream, &normals);
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
void
MLR_I_L_PMesh::TestInstance(void) const
{
	_ASSERT(IsDerivedFrom(DefaultData));
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
void
MLR_I_L_PMesh::SetNormalData(const Stuff::Vector3D* data, size_t dataSize)
{
	// Check_Object(this);
	Check_Pointer(data);
	_ASSERT(coords.GetLength() == 0 || dataSize == coords.GetLength());
	_ASSERT(colors.GetLength() == 0 || dataSize == colors.GetLength());
	_ASSERT(texCoords.GetLength() == 0 || dataSize == texCoords.GetLength());
	normals.AssignData(data, dataSize);
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
void
MLR_I_L_PMesh::GetNormalData(Stuff::Vector3D** data, psize_t dataSize)
{
	// Check_Object(this);
	*data = normals.GetData();
	*dataSize = normals.GetLength();
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
void
MLR_I_L_PMesh::SetColorData(
#if COLOR_AS_DWORD
	pcuint32_t data,
#else
	const Stuff::RGBAColor* data,
#endif
	size_t dataSize)
{
	// Check_Object(this);
	Check_Pointer(data);
	// _ASSERT(gos_GetCurrentHeap() == Heap);
	_ASSERT(coords.GetLength() == 0 || dataSize == coords.GetLength());
	_ASSERT(texCoords.GetLength() == 0 || dataSize == texCoords.GetLength());
	litColors.SetLength(dataSize);
	colors.AssignData(data, dataSize);
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
void
MLR_I_L_PMesh::PaintMe(
#if COLOR_AS_DWORD
	pcuint32_t paintMe
#else
	const Stuff::RGBAColor* paintMe
#endif

)
{
	// Check_Object(this);
	// _ASSERT(gos_GetCurrentHeap() == Heap);
#if 0
	_ASSERT(colors.GetLength() == litColors.GetLength());
#else
	if (colors.GetLength() == litColors.GetLength())
	{
		litColors.SetLength(colors.GetLength());
	}
#endif
	size_t k, len = litColors.GetLength();
#if COLOR_AS_DWORD
	uint32_t argb = GOSCopyColor(paintMe);
	for (k = 0; k < len; k++)
	{
		litColors[k] = argb;
	}
#else
	for (k = 0; k < len; k++)
	{
		litColors[k] = *paintMe;
	}
#endif
	// set the to use colors to the original colors ...
	// only lighting could overwrite this;
	actualColors = &litColors;
}

#undef I_SAY_YES_TO_DUAL_TEXTURES
#define I_SAY_YES_TO_COLOR
#define I_SAY_YES_TO_LIGHTING

#define CLASSNAME MLR_I_L_PMesh

#if defined(TRACE_ENABLED) && defined(MLR_TRACE)
#define SET_MLR_PMESH_CLIP() MLR_I_L_PMesh_Clip->Set()
#define CLEAR_MLR_PMESH_CLIP() MLR_I_L_PMesh_Clip->Clear()
#else
#define SET_MLR_PMESH_CLIP()
#define CLEAR_MLR_PMESH_CLIP()
#endif

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This include contains follwing functions:
// void MLR_I_L_PMesh::TransformNoClip(Matrix4D*, GOSVertexPool*);
// int32_t MLR_I_L_PMesh::Clip(MLRClippingState, GOSVertexPool*);
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#include <mlr/mlrprimitiveclipping.inl>

#undef I_SAY_YES_TO_COLOR
#undef I_SAY_YES_TO_LIGHTING

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This include contains follwing functions:
// void Lighting (MLRLight**, int32_t nrLights);
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#include <mlr/mlrprimitivelighting.inl>
#undef CLASSNAME

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
MLRShape*
MidLevelRenderer::CreateIndexedIcosahedron_Color_Lit(IcoInfo& icoInfo, MLRState* state)
{
#ifdef _GAMEOS_HPP_
	gos_PushCurrentHeap(Heap);
#endif
	MLRShape* ret = new MLRShape(20);
	Register_Object(ret);
	size_t i, j, k;
	uint32_t nrTri = static_cast<uint32_t>(ceil(icoInfo.all * pow(4.0f, icoInfo.depth)));
	Stuff::Point3D v[3];
	if (3 * nrTri >= Limits::Max_Number_Vertices_Per_Mesh)
	{
		nrTri = Limits::Max_Number_Vertices_Per_Mesh / 3;
	}
	puint8_t lengths = new uint8_t[nrTri];
	Register_Pointer(lengths);
	for (i = 0; i < nrTri; i++)
	{
		lengths[i] = 3;
	}
	Stuff::Point3D* coords = new Stuff::Point3D[nrTri * 3];
	Register_Pointer(coords);
	Stuff::Point3D* collapsedCoords = nullptr;
	if (icoInfo.indexed == true)
	{
		collapsedCoords = new Stuff::Point3D[nrTri * 3];
		Register_Pointer(collapsedCoords);
	}
	puint16_t index = new uint16_t[nrTri * 3];
	Register_Pointer(index);
	Stuff::Vector2DScalar* texCoords = new Stuff::Vector2DScalar[nrTri * 3];
	Register_Pointer(texCoords);
	Stuff::RGBAColor* colors = new Stuff::RGBAColor[nrTri * 3];
	Register_Pointer(colors);
	Stuff::Vector3D* normals = new Stuff::Vector3D[nrTri * 3];
	Register_Pointer(normals);
	size_t uniquePoints = 0;
	for (k = 0; k < 20; k++)
	{
		MidLevelRenderer::triDrawn = 0;
		MLR_I_L_PMesh* mesh = new MLR_I_L_PMesh();
		Register_Object(mesh);
		// setup vertex position information
		for (j = 0; j < 3; j++)
		{
			v[j].x = vdata[tindices[k][j]][0];
			v[j].y = vdata[tindices[k][j]][1];
			v[j].z = vdata[tindices[k][j]][2];
		}
		subdivide(coords, v[0], v[1], v[2], icoInfo.depth, nrTri, icoInfo.radius);
		mesh->SetSubprimitiveLengths(lengths, nrTri);
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
		if (state == nullptr)
		{
			for (i = 0; i < uniquePoints; i++)
			{
				texCoords[i] = Stuff::Vector2DScalar(0.0f, 0.0f);
			}
		}
		else
		{
			mesh->SetReferenceState(*state);
			if (state->GetTextureHandle() > 0)
			{
				if (icoInfo.indexed == true)
				{
					for (i = 0; i < uniquePoints; i++)
					{
						texCoords[i] = Stuff::Vector2DScalar((1.0f + collapsedCoords[i].x) / 2.0f,
							(1.0f + collapsedCoords[i].y) / 2.0f);
					}
				}
				else
				{
					for (i = 0; i < nrTri; i++)
					{
						texCoords[3 * i] = Stuff::Vector2DScalar(
							(1.0f + coords[3 * i].x) / 2.0f, (1.0f + coords[3 * i].y) / 2.0f);
						texCoords[3 * i + 1] =
							Stuff::Vector2DScalar((1.0f + coords[3 * i + 1].x) / 2.0f,
								(1.0f + coords[3 * i + 1].y) / 2.0f);
						texCoords[3 * i + 2] =
							Stuff::Vector2DScalar((1.0f + coords[3 * i + 2].x) / 2.0f,
								(1.0f + coords[3 * i + 2].y) / 2.0f);
					}
				}
			}
			else
			{
				for (i = 0; i < uniquePoints; i++)
				{
					texCoords[i] = Stuff::Vector2DScalar(0.0f, 0.0f);
				}
			}
		}
		mesh->SetTexCoordData(texCoords, uniquePoints);
		if (icoInfo.indexed == true)
		{
			for (i = 0; i < uniquePoints; i++)
			{
				colors[i] = Stuff::RGBAColor((1.0f + collapsedCoords[i].x) / 2.0f,
					(1.0f + collapsedCoords[i].y) / 2.0f, (1.0f + collapsedCoords[i].z) / 2.0f,
					1.0f);
				normals[i].Normalize(collapsedCoords[i]);
			}
		}
		else
		{
			for (i = 0; i < uniquePoints; i++)
			{
				colors[i] = Stuff::RGBAColor((1.0f + coords[i].x) / 2.0f,
					(1.0f + coords[i].y) / 2.0f, (1.0f + coords[i].z) / 2.0f, 1.0f);
				normals[i].Normalize(coords[i]);
			}
		}
		mesh->SetColorData(colors, uniquePoints);
		mesh->SetNormalData(normals, uniquePoints);
		ret->Add(mesh);
		mesh->DetachReference();
	}
	Unregister_Pointer(normals);
	delete[] normals;
	Unregister_Pointer(colors);
	delete[] colors;
	Unregister_Pointer(texCoords);
	delete[] texCoords;
	Unregister_Pointer(index);
	delete[] index;
	if (icoInfo.indexed == true)
	{
		Unregister_Pointer(collapsedCoords);
		delete[] collapsedCoords;
	}
	Unregister_Pointer(coords);
	delete[] coords;
	Unregister_Pointer(lengths);
	delete[] lengths;
	gos_PopCurrentHeap();
	return ret;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/*
MLR_I_L_PMesh*
MidLevelRenderer::CreateIndexedCube(
float half,
Stuff::RGBAColor *eightColors,
Stuff::Vector3D *eightNormals,
MLRState *state
)
{
gos_PushCurrentHeap(Heap);
MLR_I_L_PMesh *ret = new MLR_I_L_PMesh;
Register_Object(ret);

Stuff::Point3D *coords = new Stuff::Point3D [8];
Register_Object(coords);

coords[0] = Stuff::Point3D( half, -half, half);
coords[1] = Stuff::Point3D(-half, -half, half);
coords[2] = Stuff::Point3D( half, -half, -half);
coords[3] = Stuff::Point3D(-half, -half, -half);
coords[4] = Stuff::Point3D(-half, half, half);
coords[5] = Stuff::Point3D( half, half, half);
coords[6] = Stuff::Point3D( half, half, -half);
coords[7] = Stuff::Point3D(-half, half, -half);

puint8_t lengths = new uint8_t [6];
Register_Pointer(lengths);

size_t i;

for(i=0;i<6;i++)
{
lengths[i] = 4;
}

ret->SetPrimitiveLength(lengths, 6);

ret->SetCoordData(coords, 8);

puint16_t index = new uint16_t [6*4];
Register_Pointer(index);

index[0] = 0;
index[1] = 2;
index[2] = 6;
index[3] = 5;

index[4] = 0;
index[5] = 5;
index[6] = 4;
index[7] = 1;

index[8] = 5;
index[9] = 6;
index[10] = 7;
index[11] = 4;

index[12] = 2;
index[13] = 3;
index[14] = 7;
index[15] = 6;

index[16] = 1;
index[17] = 4;
index[18] = 7;
index[19] = 3;

index[20] = 0;
index[21] = 1;
index[22] = 3;
index[23] = 2;

ret->SetIndexData(index, 6*4);

ret->FindFacePlanes();

if(eightColors!=nullptr)
{
#if COLOR_AS_DWORD
uint32_t *dwColor = new uint32_t [8];
Register_Pointer(dwColor);

for(i=0;i<8;i++)
{
dwColor[i] = GOSCopyColor(eightColors+i);
}

ret->SetColorData(dwColor, 8);

Unregister_Pointer(dwColor);
delete [] dwColor;
#else
ret->SetColorData(eightColors, 8);
#endif
}

if(eightNormals!=nullptr)
{
ret->SetNormalData(eightNormals, 8);
}

Stuff::Vector2DScalar *texCoords = new Stuff::Vector2DScalar[8];
Register_Object(texCoords);

texCoords[0] = Stuff::Vector2DScalar(0.0f, 0.0f);
texCoords[1] = Stuff::Vector2DScalar(0.0f, 0.0f);
texCoords[2] = Stuff::Vector2DScalar(0.0f, 0.0f);
texCoords[3] = Stuff::Vector2DScalar(0.0f, 0.0f);

texCoords[4] = Stuff::Vector2DScalar(0.0f, 0.0f);
texCoords[5] = Stuff::Vector2DScalar(0.0f, 0.0f);
texCoords[6] = Stuff::Vector2DScalar(0.0f, 0.0f);
texCoords[7] = Stuff::Vector2DScalar(0.0f, 0.0f);

if(state != nullptr)
{
ret->SetReferenceState(*state);
if(state->GetTexture() > 0)
{
texCoords[0] = Stuff::Vector2DScalar(0.0f, 0.0f);
texCoords[1] = Stuff::Vector2DScalar(1.0f, 0.0f);
texCoords[2] = Stuff::Vector2DScalar(0.25f, 0.25f);
texCoords[3] = Stuff::Vector2DScalar(0.75f, 0.25f);

texCoords[4] = Stuff::Vector2DScalar(1.0f, 1.0f);
texCoords[5] = Stuff::Vector2DScalar(0.0f, 1.0f);
texCoords[6] = Stuff::Vector2DScalar(0.25f, 0.75f);
texCoords[7] = Stuff::Vector2DScalar(0.75f, 0.75f);
}
}
ret->SetTexCoordData(texCoords, 8);

Unregister_Object(texCoords);
delete [] texCoords;

Unregister_Pointer(index);
delete [] index;

Unregister_Pointer(lengths);
delete [] lengths;

Unregister_Object(coords);
delete [] coords;

gos_PopCurrentHeap();
return ret;
}
*/