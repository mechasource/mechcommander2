//===========================================================================//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#include "stdafx.h"

#include <gameos.hpp>
//#include <mlr/gosvertexpool.hpp>
//#include <mlr/mlrtexture.hpp>
//#include <mlr/mlrtexturepool.hpp>
#include <mlr/mlrshape.hpp>
#include <mlr/mlr_i_c_det_pmesh.hpp>

using namespace MidLevelRenderer;

//#############################################################################

#if defined(TRACE_ENABLED) && defined(MLR_TRACE)
BitTrace* MLR_I_C_DeT_PMesh_Clip;
#endif

//#############################################################################
//###### MLRIndexedPolyMesh with color but no lighting two texture layers ####$
//#############################################################################

MLR_I_C_DeT_PMesh::ClassData* MLR_I_C_DeT_PMesh::DefaultData = nullptr;

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
void MLR_I_C_DeT_PMesh::InitializeClass()
{
	Verify(!DefaultData);
	// Verify(gos_GetCurrentHeap() == StaticHeap);
	DefaultData =
		new ClassData(
		MLR_I_C_DeT_PMeshClassID,
		"MidLevelRenderer::MLR_I_C_DeT_PMesh",
		MLR_I_DeT_PMesh::DefaultData,
		(MLRPrimitiveBase::Factory)&Make
	);
	Register_Object(DefaultData);
#if defined(TRACE_ENABLED) && defined(MLR_TRACE)
	MLR_I_C_DeT_PMesh_Clip = new BitTrace("MLR_I_C_DeT_PMesh_Clip");
	Register_Object(MLR_I_C_DeT_PMesh_Clip);
#endif
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
void
MLR_I_C_DeT_PMesh::TerminateClass()
{
	Unregister_Object(DefaultData);
	delete DefaultData;
	DefaultData = nullptr;
#if defined(TRACE_ENABLED) && defined(MLR_TRACE)
	Unregister_Object(MLR_I_C_DeT_PMesh_Clip);
	delete MLR_I_C_DeT_PMesh_Clip;
#endif
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
MLR_I_C_DeT_PMesh::MLR_I_C_DeT_PMesh(
	ClassData* class_data, Stuff::MemoryStream* stream, uint32_t version)
	: MLR_I_DeT_PMesh(class_data, stream, version)
{
	//Check_Pointer(this);
	Check_Pointer(stream);
	//Verify(gos_GetCurrentHeap() == Heap);
	switch(version)
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
			Stuff::DynamicArrayOf<uint32_t> smallColors;
			MemoryStreamIO_Read(stream, &smallColors);
			size_t i, len = smallColors.GetLength();
			colors.SetLength(len);
			uint32_t theColor;
			for(i = 0; i < len; i++)
			{
				theColor = smallColors[i];
				colors[i].blue = (theColor & 0xff) * One_Over_256;
				theColor = theColor >> 8;
				colors[i].green = (theColor & 0xff) * One_Over_256;
				theColor = theColor >> 8;
				colors[i].red = (theColor & 0xff) * One_Over_256;
				theColor = theColor >> 8;
				colors[i].alpha = (theColor & 0xff) * One_Over_256;
			}
#endif
		}
		break;
	}
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
MLR_I_C_DeT_PMesh::MLR_I_C_DeT_PMesh(ClassData* class_data):
	MLR_I_DeT_PMesh(class_data), colors(0)
{
	//Check_Pointer(this);
	//Verify(gos_GetCurrentHeap() == Heap);
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
MLR_I_C_DeT_PMesh::~MLR_I_C_DeT_PMesh()
{
	// Check_Object(this);
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
MLR_I_C_DeT_PMesh* MLR_I_C_DeT_PMesh::Make(
	Stuff::MemoryStream* stream, uint32_t version)
{
	Check_Object(stream);
#if _CONSIDERED_OBSOLETE
	#ifdef _GAMEOS_HPP_
	gos_PushCurrentHeap(Heap);
#endif
	MLR_I_C_DeT_PMesh* mesh = new MLR_I_C_DeT_PMesh(DefaultData, stream, version);
	gos_PopCurrentHeap();
#else
	MLR_I_C_DeT_PMesh* mesh = new MLR_I_C_DeT_PMesh(DefaultData, stream, version);
#endif
	return mesh;
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
void
MLR_I_C_DeT_PMesh::Save(Stuff::MemoryStream* stream)
{
	// Check_Object(this);
	Check_Object(stream);
	MLR_I_DeT_PMesh::Save(stream);
#if COLOR_AS_DWORD
	MemoryStreamIO_Write(stream, &colors);
#else
	Stuff::DynamicArrayOf<uint32_t> smallColors;
	size_t i, len = colors.GetLength();
	const Stuff::RGBAColor* data = colors.GetData();
	smallColors.SetLength(len);
	for(i = 0; i < len; i++)
	{
		smallColors[i] = GOSCopyColor(data + i);
	}
	MemoryStreamIO_Write(stream, &smallColors);
#endif
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
void
MLR_I_C_DeT_PMesh::TestInstance(void) const
{
	Verify(IsDerivedFrom(DefaultData));
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
void MLR_I_C_DeT_PMesh::Copy(
	MLR_I_C_PMesh*, MLRState detailState, float xOff, float yOff, float xFac, float yFac)
{
	//Check_Pointer(this);
	(void)xOff;
	(void)yOff;
	(void)xFac;
	(void)yFac;
	// STOP(("No conversion implemented yet !?"));
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
void MLR_I_C_DeT_PMesh::SetColorData(
#if COLOR_AS_DWORD
	pcuint32_t data,
#else
	const Stuff::RGBAColor* data,
#endif
	size_t dataSize)
{
	// Check_Object(this);
	Check_Pointer(data);
	Verify(coords.GetLength() == 0 || dataSize == coords.GetLength());
	Verify(texCoords.GetLength() == 0 || dataSize == texCoords.GetLength());
	colors.AssignData(data, dataSize);
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
void MLR_I_C_DeT_PMesh::GetColorData(
#if COLOR_AS_DWORD
	puint32_t* data,
#else
	Stuff::RGBAColor** data,
#endif
	psize_t dataSize)
{
	// Check_Object(this);
	*data = colors.GetData();
	*dataSize = colors.GetLength();
}

//
void
MLR_I_C_DeT_PMesh::PaintMe(
#if COLOR_AS_DWORD
	pcuint32_t paintMe
#else
	const Stuff::RGBAColor* paintMe
#endif
)
{
	// Check_Object(this);
	// original color is lost !!!;
	size_t k, len = colors.GetLength();
#if COLOR_AS_DWORD
	uint32_t argb = GOSCopyColor(paintMe);
	for(k = 0; k < len; k++)
	{
		colors[k] = argb;
	}
#else
	for(k = 0; k < len; k++)
	{
		colors[k] = *paintMe;
	}
#endif
}

#define I_SAY_YES_TO_DETAIL_TEXTURES
#define I_SAY_YES_TO_COLOR
#undef I_SAY_YES_TO_DUAL_TEXTURES
#undef I_SAY_YES_TO_LIGHTING

#define CLASSNAME MLR_I_C_DeT_PMesh

#if defined(TRACE_ENABLED) && defined(MLR_TRACE)
#define SET_MLR_PMESH_CLIP() MLR_I_C_DeT_PMesh_Clip->Set()
#define CLEAR_MLR_PMESH_CLIP() MLR_I_C_DeT_PMesh_Clip->Clear()
#else
#define SET_MLR_PMESH_CLIP()
#define CLEAR_MLR_PMESH_CLIP()
#endif

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// This include contains follwing functions:
// void MLR_I_C_DeT_PMesh::TransformNoClip(Matrix4D*, GOSVertexPool*);
// int32_t MLR_I_C_DeT_PMesh::Clip(MLRClippingState, GOSVertexPool*);
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#include <mlr/mlrprimitiveclipping.inl>

#undef I_SAY_YES_TO_DETAIL_TEXTURES
#undef I_SAY_YES_TO_COLOR
#undef CLASSNAME

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
MLR_I_C_DeT_PMesh* MidLevelRenderer::CreateIndexedCube_Color_NoLit_DetTex(
	float half, Stuff::RGBAColor* eightColors, MLRState* state, MLRState* state1,
	float xOff, float yOff, float xFac, float yFac)
{
	#ifdef _GAMEOS_HPP_
	gos_PushCurrentHeap(Heap);
#endif
	MLR_I_C_DeT_PMesh* ret = new MLR_I_C_DeT_PMesh;
	Register_Object(ret);
	ret->SetDetailData(xOff, yOff, xFac, yFac);
	Stuff::Point3D* coords = new Stuff::Point3D [8];
	Register_Pointer(coords);
	coords[0] = Stuff::Point3D(half, -half, half);
	coords[1] = Stuff::Point3D(-half, -half, half);
	coords[2] = Stuff::Point3D(half, -half, -half);
	coords[3] = Stuff::Point3D(-half, -half, -half);
	coords[4] = Stuff::Point3D(-half, half, half);
	coords[5] = Stuff::Point3D(half, half, half);
	coords[6] = Stuff::Point3D(half, half, -half);
	coords[7] = Stuff::Point3D(-half, half, -half);
	puint8_t lengths = new uint8_t [6];
	Register_Pointer(lengths);
	int32_t i;
	for(i = 0; i < 6; i++)
	{
		lengths[i] = 4;
	}
	ret->SetSubprimitiveLengths(lengths, 6);
	ret->SetCoordData(coords, 8);
	puint16_t index = new uint16_t [6 * 4];
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
	if(eightColors != nullptr)
	{
#if COLOR_AS_DWORD
		uint32_t* dwColor = new uint32_t [8];
		Register_Pointer(dwColor);
		for(i = 0; i < 8; i++)
		{
			dwColor[i] = GOSCopyColor(eightColors + i);
		}
		ret->SetColorData(dwColor, 8);
		Unregister_Pointer(dwColor);
		delete [] dwColor;
#else
		ret->SetColorData(eightColors, 8);
#endif
	}
	Stuff::Vector2DScalar* texCoords = new Stuff::Vector2DScalar[8];
	Register_Pointer(texCoords);
	for(i = 0; i < 8; i++)
	{
		texCoords[i] = Stuff::Vector2DScalar(0.0f, 0.0f);
	}
	if(state != nullptr)
	{
		ret->SetReferenceState(*state);
		if(state->GetTextureHandle() > 0)
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
	}
	ret->SetTexCoordData(texCoords, 8);
	Unregister_Pointer(texCoords);
	delete [] texCoords;
	Unregister_Pointer(index);
	delete [] index;
	Unregister_Pointer(lengths);
	delete [] lengths;
	Unregister_Pointer(coords);
	delete [] coords;
	gos_PopCurrentHeap();
	return ret;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
MLRShape* MidLevelRenderer::CreateIndexedIcosahedron_Color_NoLit_DetTex(
	IcoInfo& icoInfo, MLRState* state, MLRState* stateDet)
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
	puint8_t lengths = new uint8_t[nrTri];
	Register_Pointer(lengths);
	for(i = 0; i < nrTri; i++)
	{
		lengths[i] = 3;
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
	uint32_t uniquePoints = 0;
	for(k = 0; k < 20; k++)
	{
		MidLevelRenderer::triDrawn = 0;
		MLR_I_C_DeT_PMesh* mesh = new MLR_I_C_DeT_PMesh();
		Register_Object(mesh);
		// setup vertex position information
		for(j = 0; j < 3; j++)
		{
			v[j].x = vdata[tindices[k][j]][0];
			v[j].y = vdata[tindices[k][j]][1];
			v[j].z = vdata[tindices[k][j]][2];
		}
		subdivide(coords, v[0], v[1], v[2], icoInfo.depth, nrTri, icoInfo.radius);
		mesh->SetSubprimitiveLengths(lengths, nrTri);
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
			mesh->SetReferenceState(*state);
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
			}
		}
		mesh->SetColorData(colors, uniquePoints);
		mesh->SetDetailData(0.0f, 0.0f, 16.0f, 16.0f);
		mesh->SetReferenceState(*stateDet, 1);
		ret->Add(mesh);
		mesh->DetachReference();
	}
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
	Unregister_Pointer(lengths);
	delete [] lengths;
	gos_PopCurrentHeap();
	return ret;
}
