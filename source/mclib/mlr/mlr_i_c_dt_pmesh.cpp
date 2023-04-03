//===========================================================================//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#include "stdinc.h"

//#include "gameos.hpp"
#include "mlr/gosvertexpool.h"
#include "mlr/mlrtexture.h"
#include "mlr/mlrtexturepool.h"
#include "mlr/mlrshape.h"
#include "mlr/mlr_i_c_dt_pmesh.h"

namespace MidLevelRenderer
{

//#############################################################################

#if defined(TRACE_ENABLED) && defined(MLR_TRACE)
BitTrace* MLR_I_C_DT_PMesh_Clip;
#endif

//#############################################################################
//###### MLRIndexedPolyMesh with color but no lighting two texture layers ####$
//#############################################################################

MLR_I_C_DT_PMesh::ClassData* MLR_I_C_DT_PMesh::DefaultData = nullptr;

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
void MLR_I_C_DT_PMesh::InitializeClass()
{
	_ASSERT(!DefaultData);
	// _ASSERT(gos_GetCurrentHeap() == StaticHeap);
	DefaultData = new ClassData(MLR_I_C_DT_PMeshClassID, "MidLevelRenderer::MLR_I_C_DT_PMesh",
		MLR_I_DT_PMesh::DefaultData, (MLRPrimitiveBase::Factory)&Make);
	Register_Object(DefaultData);
#if defined(TRACE_ENABLED) && defined(MLR_TRACE)
	MLR_I_C_DT_PMesh_Clip = new BitTrace("MLR_I_C_DT_PMesh_Clip");
	Register_Object(MLR_I_C_DT_PMesh_Clip);
#endif
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
void MLR_I_C_DT_PMesh::TerminateClass()
{
	Unregister_Object(DefaultData);
	delete DefaultData;
	DefaultData = nullptr;
#if defined(TRACE_ENABLED) && defined(MLR_TRACE)
	Unregister_Object(MLR_I_C_DT_PMesh_Clip);
	delete MLR_I_C_DT_PMesh_Clip;
#endif
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
MLR_I_C_DT_PMesh::MLR_I_C_DT_PMesh(ClassData* class_data, std::iostream stream, uint32_t version)
	: MLR_I_DT_PMesh(class_data, stream, version)
{
	// Check_Pointer(this);
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
#if COLOR_AS_DWORD
		MemoryStreamIO_Read(stream, &colors);
#else
		std::vector<uint32_t> smallcolours;
		MemoryStreamIO_Read(stream, &smallcolours);
		size_t i, len = smallcolours.GetLength();
		colors.SetLength(len);
		uint32_t thecolour;
		for (i = 0; i < len; i++)
		{
			thecolour = smallcolours[i];
			colors[i].blue = (thecolour & 0xff) * One_Over_256;
			thecolour = thecolour >> 8;
			colors[i].green = (thecolour & 0xff) * One_Over_256;
			thecolour = thecolour >> 8;
			colors[i].red = (thecolour & 0xff) * One_Over_256;
			thecolour = thecolour >> 8;
			colors[i].alpha = (thecolour & 0xff) * One_Over_256;
		}
#endif
	}
	break;
	}
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
MLR_I_C_DT_PMesh::MLR_I_C_DT_PMesh(ClassData* class_data)
	: MLR_I_DT_PMesh(class_data)
	, colors(0)
{
	// _ASSERT(gos_GetCurrentHeap() == Heap);
	// Check_Pointer(this);
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
MLR_I_C_DT_PMesh::~MLR_I_C_DT_PMesh()
{
	// Check_Object(this);
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
MLR_I_C_DT_PMesh*
MLR_I_C_DT_PMesh::Make(std::iostream stream, uint32_t version)
{
	Check_Object(stream);
#if CONSIDERED_OBSOLETE
	gos_PushCurrentHeap(Heap);
	MLR_I_C_DT_PMesh* mesh = new MLR_I_C_DT_PMesh(DefaultData, stream, version);
	gos_PopCurrentHeap();
#else
	MLR_I_C_DT_PMesh* mesh = new MLR_I_C_DT_PMesh(DefaultData, stream, version);
#endif
	return mesh;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
void MLR_I_C_DT_PMesh::Save(std::iostream stream)
{
	// Check_Object(this);
	Check_Object(stream);
	MLR_I_DT_PMesh::Save(stream);
#if COLOR_AS_DWORD
	MemoryStreamIO_Write(stream, &colors);
#else
	std::vector<uint32_t> smallcolours;
	size_t i, len = colors.GetLength();
	const Stuff::RGBAcolour* data = colors.GetData();
	smallcolours.SetLength(len);
	for (i = 0; i < len; i++)
	{
		smallcolours[i] = GOSCopycolour(data + i);
	}
	MemoryStreamIO_Write(stream, &smallcolours);
#endif
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
void MLR_I_C_DT_PMesh::TestInstance(void) const
{
	_ASSERT(IsDerivedFrom(DefaultData));
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
void MLR_I_C_DT_PMesh::SetcolourData(
#if COLOR_AS_DWORD
	const uint32_t* data,
#else
	const Stuff::RGBAcolour* data,
#endif
	size_t dataSize)
{
	// Check_Object(this);
	Check_Pointer(data);
	_ASSERT(coords.GetLength() == 0 || dataSize == coords.GetLength());
	// _ASSERT(texCoords.GetLength() == 0 || dataSize == texCoords.GetLength());
	colors.AssignData(data, dataSize);
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
void MLR_I_C_DT_PMesh::GetcolourData(
#if COLOR_AS_DWORD
	uint32_t** data,
#else
	Stuff::RGBAcolour** data,
#endif
	size_t* dataSize)
{
	// Check_Object(this);
	*data = colors.GetData();
	*dataSize = colors.GetLength();
}

//
void MLR_I_C_DT_PMesh::PaintMe(
#if COLOR_AS_DWORD
	const uint32_t* paintMe)
#else
	const Stuff::RGBAcolour* paintMe)
#endif
{
	// Check_Object(this);
	// original color is lost !!!;
	size_t k, len = colors.GetLength();
#if COLOR_AS_DWORD
	uint32_t argb = GOSCopycolour(paintMe);
	for (k = 0; k < len; k++)
	{
		colors[k] = argb;
	}
#else
	for (k = 0; k < len; k++)
	{
		colors[k] = *paintMe;
	}
#endif
}

#define I_SAY_YES_TO_DUAL_TEXTURES
#define I_SAY_YES_TO_COLOR
#undef I_SAY_YES_TO_LIGHTING

#define CLASSNAME MLR_I_C_DT_PMesh

#if defined(TRACE_ENABLED) && defined(MLR_TRACE)
#define SET_MLR_PMESH_CLIP() MLR_I_C_DT_PMesh_Clip->Set()
#define CLEAR_MLR_PMESH_CLIP() MLR_I_C_DT_PMesh_Clip->Clear()
#else
#define SET_MLR_PMESH_CLIP()
#define CLEAR_MLR_PMESH_CLIP()
#endif

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This include contains follwing functions:
// void MLR_I_C_DT_PMesh::TransformNoClip(Matrix4D*, GOSVertexPool*);
// int32_t MLR_I_C_DT_PMesh::Clip(MLRClippingState, GOSVertexPool*);
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#include "mlr/mlrprimitiveclipping.inl"

#undef I_SAY_YES_TO_DUAL_TEXTURES
#undef I_SAY_YES_TO_COLOR
#undef CLASSNAME

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
MLR_I_C_DT_PMesh*
MidLevelRenderer::CreateIndexedCube_colour_NoLit_2Tex(
	float half, Stuff::RGBAcolour* eightcolours, MLRState* state, MLRState* state1)
{
#ifdef _GAMEOS_HPP_
	gos_PushCurrentHeap(Heap);
#endif
	MLR_I_C_DT_PMesh* ret = new MLR_I_C_DT_PMesh;
	Register_Object(ret);
	Stuff::Point3D* coords = new Stuff::Point3D[8];
	Register_Pointer(coords);
	coords[0] = Stuff::Point3D(half, -half, half);
	coords[1] = Stuff::Point3D(-half, -half, half);
	coords[2] = Stuff::Point3D(half, -half, -half);
	coords[3] = Stuff::Point3D(-half, -half, -half);
	coords[4] = Stuff::Point3D(-half, half, half);
	coords[5] = Stuff::Point3D(half, half, half);
	coords[6] = Stuff::Point3D(half, half, -half);
	coords[7] = Stuff::Point3D(-half, half, -half);
	uint8_t* lengths = new uint8_t[6];
	Register_Pointer(lengths);
	size_t i;
	for (i = 0; i < 6; i++)
	{
		lengths[i] = 4;
	}
	ret->SetSubprimitiveLengths(lengths, 6);
	ret->SetCoordData(coords, 8);
	uint16_t* index = new uint16_t[6 * 4];
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
	ret->SetIndexData(index, 6 * 4);
	ret->FindFacePlanes();
	if (eightcolours != nullptr)
	{
#if COLOR_AS_DWORD
		uint32_t* colour = new uint32_t[8];
		Register_Object(colour);
		for (i = 0; i < 8; i++)
		{
			colour[i] = GOSCopycolour(eightcolours + i);
		}
		ret->SetcolourData(colour, 8);
		Unregister_Pointer(colour);
		delete[] colour;
#else
		ret->SetcolourData(eightcolours, 8);
#endif
	}
	Stuff::Vector2DScalar* texCoords = new Stuff::Vector2DScalar[16];
	Register_Pointer(texCoords);
	for (i = 0; i < 16; i++)
	{
		texCoords[i] = Stuff::Vector2DScalar(0.0f, 0.0f);
	}
	if (state != nullptr)
	{
		ret->SetReferenceState(*state);
		if (state->GetTextureHandle() > 0)
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
		ret->SetReferenceState(*state1, 1);
		if (state1->GetTextureHandle() > 0)
		{
			texCoords[8] = texCoords[0];
			texCoords[9] = texCoords[1];
			texCoords[10] = texCoords[2];
			texCoords[11] = texCoords[3];
			texCoords[12] = texCoords[4];
			texCoords[13] = texCoords[5];
			texCoords[14] = texCoords[6];
			texCoords[15] = texCoords[7];
		}
	}
	ret->SetTexCoordData(texCoords, 16);
	Unregister_Pointer(texCoords);
	delete[] texCoords;
	Unregister_Pointer(lengths);
	delete[] lengths;
	Unregister_Pointer(index);
	delete[] index;
	Unregister_Pointer(coords);
	delete[] coords;
	gos_PopCurrentHeap();
	return ret;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
MLRShape*
MidLevelRenderer::CreateIndexedIcosahedron_colour_NoLit_2Tex(
	IcoInfo& icoInfo, MLRState* state, MLRState* state2)
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
	uint8_t* lengths = new uint8_t[nrTri];
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
	uint16_t* index = new uint16_t[nrTri * 3];
	Register_Pointer(index);
	Stuff::Vector2DScalar* texCoords = new Stuff::Vector2DScalar[2 * nrTri * 3];
	Register_Pointer(texCoords);
	Stuff::RGBAcolour* colors = new Stuff::RGBAcolour[nrTri * 3];
	Register_Pointer(colors);
	size_t uniquePoints = 0;
	for (k = 0; k < 20; k++)
	{
		MidLevelRenderer::triDrawn = 0;
		MLR_I_C_DT_PMesh* mesh = new MLR_I_C_DT_PMesh();
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
			for (i = 0; i < 2 * uniquePoints; i++)
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
						texCoords[i + uniquePoints] =
							Stuff::Vector2DScalar((1.0f + collapsedCoords[i].x) / 2.0f,
								(1.0f + collapsedCoords[i].z) / 2.0f);
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
						texCoords[3 * i + nrTri] = Stuff::Vector2DScalar(
							(1.0f + coords[3 * i].x) / 2.0f, (1.0f + coords[3 * i].z) / 2.0f);
						texCoords[3 * i + 1 + nrTri] =
							Stuff::Vector2DScalar((1.0f + coords[3 * i + 1].x) / 2.0f,
								(1.0f + coords[3 * i + 1].z) / 2.0f);
						texCoords[3 * i + 2 + nrTri] =
							Stuff::Vector2DScalar((1.0f + coords[3 * i + 2].x) / 2.0f,
								(1.0f + coords[3 * i + 2].z) / 2.0f);
					}
				}
			}
			else
			{
				for (i = 0; i < 2 * uniquePoints; i++)
				{
					texCoords[i] = Stuff::Vector2DScalar(0.0f, 0.0f);
				}
			}
		}
		mesh->SetTexCoordData(texCoords, 2 * uniquePoints);
		if (icoInfo.indexed == true)
		{
			for (i = 0; i < uniquePoints; i++)
			{
				colors[i] = Stuff::RGBAcolour((1.0f + collapsedCoords[i].x) / 2.0f,
					(1.0f + collapsedCoords[i].y) / 2.0f, (1.0f + collapsedCoords[i].z) / 2.0f,
					1.0f);
			}
		}
		else
		{
			for (i = 0; i < uniquePoints; i++)
			{
				colors[i] = Stuff::RGBAcolour((1.0f + coords[i].x) / 2.0f,
					(1.0f + coords[i].y) / 2.0f, (1.0f + coords[i].z) / 2.0f, 1.0f);
			}
		}
		mesh->SetcolourData(colors, uniquePoints);
		mesh->SetReferenceState(*state2, 1);
		ret->Add(mesh);
		mesh->DetachReference();
	}
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

} // namespace MidLevelRenderer
