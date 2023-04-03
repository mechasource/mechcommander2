//===========================================================================//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#include "stdinc.h"

#include "mlr/mlr_i_tmesh.h"

namespace MidLevelRenderer
{

//#############################################################################

#if defined(TRACE_ENABLED) && defined(MLR_TRACE)
BitTrace* MLR_I_TMesh_Clip;
#endif

extern uint32_t gEnableLightMaps;

#define UV_TEST 0
#define SPEW_AWAY 0
//#define TOP_DOWN_ONLY

//#############################################################################
//###### MLRIndexedTriMesh with no color no lighting one texture layer  ######
//#############################################################################

MLR_I_TMesh::ClassData* MLR_I_TMesh::DefaultData = nullptr;
extern std::vector<Stuff::Vector2DScalar>* lightMapUVs;
extern std::vector<float>* lightMapSqFalloffs;

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
void MLR_I_TMesh::InitializeClass()
{
	_ASSERT(!DefaultData);
	// _ASSERT(gos_GetCurrentHeap() == StaticHeap);
	DefaultData = new ClassData(MLR_I_TMeshClassID, "MidLevelRenderer::MLR_I_TMesh",
		MLRIndexedPrimitiveBase::DefaultData, (MLRPrimitiveBase::Factory)&Make);
	Register_Object(DefaultData);
#if defined(TRACE_ENABLED) && defined(MLR_TRACE)
	MLR_I_TMesh_Clip = new BitTrace("MLR_I_TMesh_Clip");
	Register_Object(MLR_I_TMesh_Clip);
#endif
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
void MLR_I_TMesh::TerminateClass()
{
	Unregister_Object(DefaultData);
	delete DefaultData;
	DefaultData = nullptr;
#if defined(TRACE_ENABLED) && defined(MLR_TRACE)
	Unregister_Object(MLR_I_TMesh_Clip);
	delete MLR_I_TMesh_Clip;
#endif
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
MLR_I_TMesh::MLR_I_TMesh(ClassData* class_data, std::iostream stream, uint32_t version)
	: MLRIndexedPrimitiveBase(class_data, stream, version)
{
	// Check_Pointer(this);
	Check_Pointer(stream);
	// _ASSERT(gos_GetCurrentHeap() == Heap);
	numOfTriangles = index.GetLength() / 3;
	facePlanes.SetLength(numOfTriangles);
	testList.SetLength(numOfTriangles);
	FindFacePlanes();
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
MLR_I_TMesh::MLR_I_TMesh(ClassData* class_data)
	: MLRIndexedPrimitiveBase(class_data)
{
	// Check_Pointer(this);
	// _ASSERT(gos_GetCurrentHeap() == Heap);
	drawMode = SortData::TriIndexedList;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
MLR_I_TMesh::~MLR_I_TMesh()
{
	// Check_Object(this);
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
MLR_I_TMesh*
MLR_I_TMesh::Make(std::iostream stream, uint32_t version)
{
	Check_Object(stream);
#ifdef _GAMEOS_HPP_
	gos_PushCurrentHeap(Heap);
#endif
	MLR_I_TMesh* mesh = new MLR_I_TMesh(DefaultData, stream, version);
	gos_PopCurrentHeap();
	return mesh;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
void MLR_I_TMesh::Save(std::iostream stream)
{
	// Check_Object(this);
	Check_Object(stream);
	MLRIndexedPrimitiveBase::Save(stream);
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
void MLR_I_TMesh::TestInstance(void) const
{
	_ASSERT(IsDerivedFrom(DefaultData));
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
bool MLR_I_TMesh::Copy(MLR_I_PMesh* pMesh)
{
	// Check_Object(this);
	Check_Object(pMesh);
	// _ASSERT(gos_GetCurrentHeap() == Heap);
	size_t len;
	uint16_t* _index;
	Point3D* _coords;
	Stuff::Vector2DScalar* _texCoords;
	pMesh->GetCoordData(&_coords, &len);
	SetCoordData(_coords, len);
	pMesh->GetIndexData(&_index, &len);
	SetIndexData(_index, len);
	SetSubprimitiveLengths(nullptr, pMesh->GetNumPrimitives());
	facePlanes.SetLength(GetNumPrimitives());
	testList.SetLength(GetNumPrimitives());
	FindFacePlanes();
	pMesh->GetTexCoordData(&_texCoords, &len);
	SetTexCoordData(_texCoords, len);
	referenceState = pMesh->GetReferenceState();
	visibleIndexedVerticesKey = false;
	visibleIndexedVertices.SetLength(coords.GetLength());
	return true;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
void MLR_I_TMesh::InitializeDrawPrimitive(uint8_t vis, int32_t parameter)
{
	MLRIndexedPrimitiveBase::InitializeDrawPrimitive(vis, parameter);
	if (parameter & 1)
	{
		ResetTestList();
	}
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
void MLR_I_TMesh::FindFacePlanes()
{
	// Check_Object(this);
	int32_t i, j, numPrimitives = GetNumPrimitives();
	Vector3D v;
	_ASSERT(index.GetLength() > 0);
	for (i = 0, j = 0; i < numPrimitives; ++i, j += 3)
	{
		facePlanes[i].BuildPlane(coords[index[j]], coords[index[j + 1]], coords[index[j + 2]]);
		;
	}
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
int32_t
MLR_I_TMesh::FindBackFace(const Point3D& u)
{
	// Check_Object(this);
	int32_t ret = 0;
	uint8_t* iPtr;
	Plane* p;
	if (numOfTriangles <= 0)
	{
		visible = 0;
		return 0;
	}
	p = &facePlanes[numOfTriangles - 1];
	iPtr = &testList[0];
	if (state.GetBackFaceMode() == MLRState::BackFaceOffMode)
	{
		ResetTestList();
		ret = 1;
	}
	else
	{
		memset(iPtr, 0, numOfTriangles);
		for (size_t i = numOfTriangles - 1; i >= 0; p--, i--)
		{
			if (p->GetDistanceTo(u) >= 0.0f)
			{
				iPtr[i] = (uint8_t)(ret = 1);
			}
		}
	}
	visible = (uint8_t)ret;
	FindVisibleVertices();
	return ret;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
void MLR_I_TMesh::ResetTestList()
{
	int32_t i, numPrimitives = GetNumPrimitives();
	uint8_t* iPtr = &testList[0];
	for (i = 0; i < numPrimitives; i++, iPtr++)
	{
		*iPtr = 1;
	}
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
int32_t
MLR_I_TMesh::FindVisibleVertices()
{
	// Check_Object(this);
	_ASSERT(index.GetLength() > 0);
	int32_t ret, i, j;
	uint32_t* indices = (uint32_t*)index.GetData();
	for (i = 0, j = 0, ret = 0; i < (numOfTriangles >> 1); ++i)
	{
		if (testList[i << 1] != 0)
		{
			visibleIndexedVertices[(*(indices + j)) & 0xffff] = 1;
			visibleIndexedVertices[(*(indices + j)) >> 16] = 1;
			visibleIndexedVertices[(*(indices + j + 1)) & 0xffff] = 1;
			ret = 1;
		}
		if (testList[(i << 1) + 1] != 0)
		{
			visibleIndexedVertices[(*(indices + j + 1)) >> 16] = 1;
			visibleIndexedVertices[(*(indices + j + 2)) & 0xffff] = 1;
			visibleIndexedVertices[(*(indices + j + 2)) >> 16] = 1;
			ret = 1;
		}
		j += 3;
	}
	if (numOfTriangles & 1)
	{
		if (testList[numOfTriangles - 1] != 0)
		{
			j <<= 1;
			visibleIndexedVertices[index[j++]] = 1;
			visibleIndexedVertices[index[j++]] = 1;
			visibleIndexedVertices[index[j++]] = 1;
			ret = 1;
		}
	}
	visibleIndexedVerticesKey = true;
	return ret;
}

extern uint32_t gEnableTextureSort, gEnableAlphaSort;

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/*
void
	MLR_I_TMesh::Transform(Matrix4D *mat)
{
	// Check_Object(this);

	_ASSERT(index.GetLength() > 0);
	int32_t i, len = coords.GetLength();

	if(visibleIndexedVerticesKey == false)
	{
		FindVisibleVertices();
	}

	Stuff::Vector4D *v4d = transformedCoords.GetData();
	Stuff::Point3D *p3d = coords.GetData();
	MLRClippingState *cs = clipPerVertex.GetData();
	uint8_t* viv = visibleIndexedVertices.GetData();

	for(i=0;i<len;i++,p3d++,v4d++,cs++,viv++)
	{
		if(*viv == 0)
		{
			continue;
		}

		v4d->Multiply(*p3d, *mat);
#ifdef LAB_ONLY
		Statistics::MLR_TransformedVertices++;
#endif

		cs->Clip4dVertex(v4d);
//
//--------------------------------------------------------
// I claims all vertices are in. lets check it. who knows
//--------------------------------------------------------
//
#ifdef LAB_ONLY
		if( (*cs)==0)
		{
#if defined(_ARMOR)
			if(ArmorLevel > 3)
			{
//
//--------------------------------------------------------
// I claims all vertices are in. lets check it. who knows
//--------------------------------------------------------
//
				_ASSERT(v4d->x >= 0.0f && v4d->x <= v4d->w );
				_ASSERT(v4d->y >= 0.0f && v4d->y <= v4d->w );
				_ASSERT(v4d->z >= 0.0f && v4d->z <= v4d->w );
			}
#endif
			Statistics::MLR_NonClippedVertices++;
		}
		else
		{
			Statistics::MLR_ClippedVertices++;
		}
#endif
	}
}
*/

#undef I_SAY_YES_TO_DUAL_TEXTURES
#undef I_SAY_YES_TO_COLOR
#undef I_SAY_YES_TO_LIGHTING

#define CLASSNAME MLR_I_TMesh

#if defined(TRACE_ENABLED) && defined(MLR_TRACE)
#define SET_MLR_TMESH_CLIP() MLR_I_TMesh_Clip->Set()
#define CLEAR_MLR_TMESH_CLIP() MLR_I_TMesh_Clip->Clear()
#else
#define SET_MLR_TMESH_CLIP()
#define CLEAR_MLR_TMESH_CLIP()
#endif

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//	This include contains follwing functions:
//	void MLR_I_TMesh::TransformNoClip(Matrix4D*, GOSVertexPool*);
//	int32_t MLR_I_TMesh::Clip(MLRClippingState, GOSVertexPool*);
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#include "mlr/mlrtriangleclipping.inl"

#undef CLASSNAME

//---------------------------------------------------------------------------
//
void MLR_I_TMesh::Lighting(MLRLight* const* lights, int32_t nrLights)
{
	int32_t i;
	MLRLightMap* lightMap;
	for (i = 0; i < nrLights; i++)
	{
		lightMap = lights[i]->GetLightMap();
		if (lightMap != nullptr)
		{
			Start_Timer(LightMap_Light_Time);
			LightMapLighting(lights[i]);
			Stop_Timer(LightMap_Light_Time);
		}
	}
}

extern RGBAcolour errorcolour;
extern bool
CheckForBigTriangles(std::vector<Stuff::Vector2DScalar>* lightMapUVs, int32_t stride);

//---------------------------------------------------------------------------
//
void MLR_I_TMesh::LightMapLighting(MLRLight* light)
{
	int32_t i, j, k, len = numOfTriangles;
	LinearMatrix4D matrix = LinearMatrix4D::Identity;
	Point3D lightPosInShape, hitPoint;
	UnitVector3D up, left, forward;
	bool lm;
	float f, rhf, falloff = 1.0f, distance;
	MLRLightMap* lightMap = light->GetLightMap();
	if ((!lightMap) || (!gEnableLightMaps))
	{
		return;
	}
	float bigUV = MLRState::GetMaxUV();
	int32_t tooBig = 0;
	switch (light->GetLightType())
	{
	case MLRLight::PointLight:
	{
		Check_Object(lightMap);
		lightMap->AddState(referenceState.GetPriority() + 1);
		light->GetInShapePosition(lightPosInShape);
		for (i = 0, j = 0, k = 0; i < len; i++, j += 3)
		{
			if (testList[i] == 0)
			{
				continue;
			}
			f = facePlanes[i].GetDistanceTo(lightPosInShape);
			lm = false;
			if (f > 0.0f && Cast_Object(MLRInfiniteLightWithFalloff*, light)->GetFalloff(f, falloff) == true)
			{
				rhf = 1.0f / f;
				matrix = LinearMatrix4D::Identity;
				matrix.AlignLocalAxisToWorldVector(facePlanes[i].normal, Z_Axis, X_Axis, Y_Axis);
				matrix.GetLocalLeftInWorld(&left);
				matrix.GetLocalUpInWorld(&up);
				matrix.GetLocalForwardInWorld(&forward);
				_ASSERT(Small_Enough(up * left));
#if defined(_ARMOR)
				float diff = forward * left;
				_ASSERT(Small_Enough(diff));
				diff = up * forward;
				_ASSERT(Small_Enough(diff));
#endif
				Check_Object(&forward);
				hitPoint.Multiply(forward, -f);
				hitPoint += lightPosInShape;
				tooBig = 0;
				for (k = 0; k < 3; k++)
				{
					Vector3D vec(coords[index[k + j]]);
					vec -= hitPoint;
					(*lightMapUVs)[k][0] = -(left * vec) * rhf;
					(*lightMapUVs)[k][1] = -(up * vec) * rhf;
					if ((*lightMapUVs)[k][0] >= -0.5f && (*lightMapUVs)[k][0] <= 0.5f && (*lightMapUVs)[k][1] >= -0.5f && (*lightMapUVs)[k][1] <= 0.5f)
					{
						lm = true;
					}
					if ((*lightMapUVs)[k][0] < -bigUV || (*lightMapUVs)[k][0] > bigUV || (*lightMapUVs)[k][1] < -bigUV || (*lightMapUVs)[k][1] > bigUV)
					{
						tooBig++;
					}
				}
			}
			else
			{
				continue;
			}
			if (tooBig == 0 && (lm == true || CheckForBigTriangles(lightMapUVs, 3) == true))
			{
				lightMap->SetPolygonMarker(0);
				lightMap->AddUShort(3);
#if 0
					Vector3D vec(coords[index[j]]);
					SPEW(("micgaert", "\nvertex1 = %f,%f,%f", vec.x, vec.y, vec.z));
					vec = coords[index[j + 1]];
					SPEW(("micgaert", "vertex2 = %f,%f,%f", vec.x, vec.y, vec.z));
					vec = coords[index[j + 2]];
					SPEW(("micgaert", "vertex3 = %f,%f,%f", vec.x, vec.y, vec.z));
					vec = facePlanes[i].normal;
					SPEW(("micgaert", "normal = %f,%f,%f", vec.x, vec.y, vec.z));
					SPEW(("micgaert", "forward = %f,%f,%f", forward.x, forward.y, forward.z));
					SPEW(("micgaert", "distance = %f", f));
					SPEW(("micgaert", "light = %f,%f,%f", lightPosInShape.x, lightPosInShape.y, lightPosInShape.z));
					SPEW(("micgaert", "projection = %f,%f,%f", hitPoint.x, hitPoint.y, hitPoint.z));
#endif
				float sq_falloff = falloff * falloff * light->GetIntensity();
				RGBAcolour color(sq_falloff, sq_falloff, sq_falloff, 1.0f);
				lightMap->Addcolour(color);
				for (k = 0; k < 3; k++)
				{
					lightMap->AddCoord(coords[index[k + j]]);
				}
				for (k = 0; k < 3; k++)
				{
					lightMap->AddUVs((*lightMapUVs)[k][0] + 0.5f, (*lightMapUVs)[k][1] + 0.5f);
					//				DEBUG_STREAM << k << " " << lightMapUVs[k][0] << "
					//" << lightMapUVs[k][0] << "\n";
				}
			}
		}
	}
	break;
	case MLRLight::SpotLight:
	{
		int32_t behindCount = 0, falloffCount = 0;
		Check_Object(lightMap);
		lightMap->AddState(referenceState.GetPriority() + 1);
		light->GetInShapePosition(matrix);
		lightPosInShape = matrix;
		float tanSpeadAngle = Cast_Object(MLRSpotLight*, light)->GetTanSpreadAngle();
#ifndef TOP_DOWN_ONLY
		matrix.GetLocalLeftInWorld(&left);
		matrix.GetLocalUpInWorld(&up);
		matrix.GetLocalForwardInWorld(&forward);
#else
		forward = UnitVector3D(0.0f, -1.0f, 0.0);
		up = UnitVector3D(1.0f, 0.0f, 0.0);
		left = UnitVector3D(0.0f, 0.0f, 1.0);
#endif
		_ASSERT(Small_Enough(up * left));
		for (i = 0, j = 0, k = 0; i < len; i++, j += 3)
		{
			behindCount = 0;
			falloffCount = 0;
			if (testList[i] == 0)
			{
				continue;
			}
			lm = false;
			if (!facePlanes[i].IsSeenBy(lightPosInShape))
			{
				continue;
			}
#if SPEW_AWAY
			float maxX, maxZ;
			float minX, minZ;
			minX = maxX = coords[index[j]].x;
			minZ = maxZ = coords[index[j]].z;
			if (minX > coords[index[j + 1]].x)
			{
				minX = coords[index[j + 1]].x;
			}
			if (minX > coords[index[j + 2]].x)
			{
				minX = coords[index[j + 2]].x;
			}
			if (minZ > coords[index[j + 1]].z)
			{
				minZ = coords[index[j + 1]].z;
			}
			if (minX > coords[index[j + 2]].z)
			{
				minZ = coords[index[j + 2]].z;
			}
			if (maxX < coords[index[j + 1]].x)
			{
				maxX = coords[index[j + 1]].x;
			}
			if (maxX < coords[index[j + 2]].x)
			{
				maxX = coords[index[j + 2]].x;
			}
			if (maxZ < coords[index[j + 1]].z)
			{
				maxZ = coords[index[j + 1]].z;
			}
			if (maxX < coords[index[j + 2]].z)
			{
				maxZ = coords[index[j + 2]].z;
			}
			if (lightPosInShape.x > minX && lightPosInShape.x < maxX && lightPosInShape.z > minZ && lightPosInShape.z < maxZ)
			{
				SPEW(("micgaert", "On Target !!"));
			}
#endif
			tooBig = 0;
			for (k = 0; k < 3; k++)
			{
				Vector3D vec;
				float oneOver;
				vec.Subtract(coords[index[k + j]], lightPosInShape);
#ifndef TOP_DOWN_ONLY
				distance = (vec * forward);
#else
				distance = -vec.y;
#endif
#if SPEW_AWAY
				SPEW(("micgaert", "vertex%d = %f,%f,%f", k, coords[index[k + j]].x,
					coords[index[k + j]].y, coords[index[k + j]].z));
				SPEW(("micgaert", "distance = %f", distance));
#endif
				if (distance > SMALL)
				{
					if (Cast_Object(MLRInfiniteLightWithFalloff*, light)
							->GetFalloff(distance, falloff)
						== false)
					{
						falloffCount++;
					}
					(*lightMapSqFalloffs)[k] = falloff * falloff * light->GetIntensity();
					oneOver
#if 0
							= 1.0f / (2.0f * distance * tanSpeadAngle);
#else
						= OneOverApproximate(2.0f * distance * tanSpeadAngle);
#endif
				}
				else
				{
					behindCount++;
					oneOver = 1.0f / 50.0f;
					(*lightMapSqFalloffs)[k] = 0.0f;
#if SPEW_AWAY
					SPEW(("micgaert", "Behind"));
#endif
				}
#ifndef TOP_DOWN_ONLY
				(*lightMapUVs)[k][0] = (left * vec) * oneOver;
				(*lightMapUVs)[k][1] = -(up * vec) * oneOver;
#else
				(*lightMapUVs)[k][0] = vec.x * oneOver;
				(*lightMapUVs)[k][1] = -vec.z * oneOver;
#endif
#if SPEW_AWAY
				SPEW(("micgaert", "uv%d = %f,%f", k, (*lightMapUVs)[k][0], (*lightMapUVs)[k][1]));
#endif
				if ((*lightMapUVs)[k][0] >= -0.5f && (*lightMapUVs)[k][0] <= 0.5f && (*lightMapUVs)[k][1] >= -0.5f && (*lightMapUVs)[k][1] <= 0.5f)
				{
					lm = true;
				}
				if ((*lightMapUVs)[k][0] < -bigUV || (*lightMapUVs)[k][0] > bigUV || (*lightMapUVs)[k][1] < -bigUV || (*lightMapUVs)[k][1] > bigUV)
				{
					tooBig++;
				}
			}
#if 1
			if (tooBig == 0 && behindCount < 3 && falloffCount < 3 && ((lm == true) || CheckForBigTriangles(lightMapUVs, 3) == true))
			{
				lightMap->SetPolygonMarker(1);
				lightMap->AddUShort(3);
				for (k = 0; k < 3; k++)
				{
					lightMap->AddCoord(coords[index[k + j]]);
				}
				for (k = 0; k < 3; k++)
				{
					lightMap->Addcolour((*lightMapSqFalloffs)[k], (*lightMapSqFalloffs)[k],
						(*lightMapSqFalloffs)[k], 1.0f);
				}
				for (k = 0; k < 3; k++)
				{
					lightMap->AddUVs((*lightMapUVs)[k][0] + 0.5f, (*lightMapUVs)[k][1] + 0.5f);
					//				DEBUG_STREAM << k << " " << lightMapUVs[k][0] << "
					//" << lightMapUVs[k][0] << "\n";
				}
#if SPEW_AWAY
				SPEW(("micgaert", "See the Light !"));
#endif
			}
#if SPEW_AWAY
			Vector3D vec = facePlanes[i].normal;
			SPEW(("micgaert", "normal = %f,%f,%f", vec.x, vec.y, vec.z));
			SPEW(("micgaert", "forward = %f,%f,%f", forward.x, forward.y, forward.z));
			SPEW(("micgaert", "left = %f,%f,%f", left.x, left.y, left.z));
			SPEW(("micgaert", "up = %f,%f,%f", up.x, up.y, up.z));
			SPEW(("micgaert", "light = %f,%f,%f\n", lightPosInShape.x, lightPosInShape.y,
				lightPosInShape.z));
#endif
#else
			if (tooBig != 0)
			{
				lightMap->SetPolygonMarker(1);
				lightMap->AddUShort(3);
				for (k = 0; k < 3; k++)
				{
					lightMap->AddCoord(coords[index[k + j]]);
				}
				for (k = 0; k < 3; k++)
				{
					lightMap->Addcolour(RGBAcolour(0.0f, 0.0f, 0.5f, 1.0f));
				}
				for (k = 0; k < 3; k++)
				{
					lightMap->AddUVs(0.5f, 0.5f);
					//				DEBUG_STREAM << k << " " << lightMapUVs[k][0] << "
					//" << lightMapUVs[k][0] << "\n";
				}
			}
			else if (behindCount != 0)
			{
				lightMap->SetPolygonMarker(1);
				lightMap->AddUShort(3);
				for (k = 0; k < 3; k++)
				{
					lightMap->AddCoord(coords[index[k + j]]);
				}
				for (k = 0; k < 3; k++)
				{
					lightMap->Addcolour(RGBAcolour(0.5f, 0.0f, 0.0f, 1.0f));
				}
				for (k = 0; k < 3; k++)
				{
					lightMap->AddUVs(0.5f, 0.5f);
					//				DEBUG_STREAM << k << " " << lightMapUVs[k][0] << "
					//" << lightMapUVs[k][0] << "\n";
				}
			}
			else if (behindCount == 0 && (lm == true || CheckForBigTriangles(&lightMapUVs, 3) == true))
			{
				lightMap->SetPolygonMarker(1);
				lightMap->AddUShort(3);
				for (k = 0; k < 3; k++)
				{
					lightMap->AddCoord(coords[index[k + j]]);
				}
				for (k = 0; k < 3; k++)
				{
					lightMap->Addcolour(
						lightMapSqFalloffs[k], lightMapSqFalloffs[k], lightMapSqFalloffs[k], 1.0f);
				}
				for (k = 0; k < 3; k++)
				{
					lightMap->AddUVs(lightMapUVs[k][0] + 0.5f, lightMapUVs[k][1] + 0.5f);
					//				DEBUG_STREAM << k << " " << lightMapUVs[k][0] << "
					//" << lightMapUVs[k][0] << "\n";
				}
			}
			else if (CheckForBigTriangles(&lightMapUVs, 3) == false)
			{
				lightMap->SetPolygonMarker(1);
				lightMap->AddUShort(3);
				for (k = 0; k < 3; k++)
				{
					lightMap->AddCoord(coords[index[k + j]]);
				}
				for (k = 0; k < 3; k++)
				{
					lightMap->Addcolour(errorcolour);
				}
				for (k = 0; k < 3; k++)
				{
					lightMap->AddUVs(0.5f, 0.5f);
					//				DEBUG_STREAM << k << " " << lightMapUVs[k][0] << "
					//" << lightMapUVs[k][0] << "\n";
				}
			}
#endif
		}
	}
	break;
	default:
		STOP(("MLR_I_PMesh::LightMapLighting: What you want me to do ?"));
		break;
	}
}

//---------------------------------------------------------------------------
//
bool MLR_I_TMesh::CastRay(Line3D* line, Normal3D* normal)
{
	// Check_Object(this);
	Check_Object(line);
	Check_Pointer(normal);
	//
	//---------------------------------------------------------------------
	// We have to spin through each of the polygons stored in the shape and
	// collide the ray against each
	//---------------------------------------------------------------------
	//
	int32_t poly_start = 0;
	bool hit = false;
	for (size_t polygon = 0; polygon < numOfTriangles; poly_start += 3, ++polygon)
	{
		//
		//---------------------------------
		// See if the line misses the plane
		//---------------------------------
		//
		float product;
		const Plane* plane = &facePlanes[polygon];
		Check_Object(plane);
		float distance = line->GetDistanceTo(*plane, &product);
		if (distance < 0.0f || distance > line->length)
		{
			continue;
		}
		bool negate = false;
		if (product > -SMALL)
		{
			if (GetCurrentState().GetBackFaceMode() == MLRState::BackFaceOnMode)
			{
				continue;
			}
			negate = true;
		}
		//
		//-------------------------------------------
		// Figure out where on the plane the line hit
		//-------------------------------------------
		//
		Point3D impact;
		line->Project(distance, &impact);
		//
		//-------------------------------------------------------------------
		// We now need to find out which cardinal plane we should project the
		// triangle onto
		//-------------------------------------------------------------------
		//
		int32_t s, t;
		float nx = abs(plane->normal.x);
		float ny = abs(plane->normal.y);
		float nz = abs(plane->normal.z);
		if (nx > ny)
		{
			if (nx > nz)
			{
				s = Y_Axis;
				t = Z_Axis;
			}
			else
			{
				s = X_Axis;
				t = Y_Axis;
			}
		}
		else if (ny > nz)
		{
			s = Z_Axis;
			t = X_Axis;
		}
		else
		{
			s = X_Axis;
			t = Y_Axis;
		}
		//
		//----------------------------------------
		// Initialize the vertex and leg variables
		//----------------------------------------
		//
		Point3D *v1, *v2, *v3;
		v1 = &coords[index[poly_start]];
		v2 = &coords[index[poly_start + 1]];
		v3 = &coords[index[poly_start + 2]];
		//
		//---------------------------------------
		// Get the projection of the impact point
		//---------------------------------------
		//
		float s0 = impact[s] - (*v1)[s];
		float t0 = impact[t] - (*v1)[t];
		float s1 = (*v2)[s] - (*v1)[s];
		float t1 = (*v2)[t] - (*v1)[t];
		//
		//------------------------------------------------------------
		// For each triangle, figure out what the second leg should be
		//------------------------------------------------------------
		//
		bool local_hit = false;
		Check_Pointer(v3);
		float s2 = (*v3)[s] - (*v1)[s];
		float t2 = (*v3)[t] - (*v1)[t];
		//
		//--------------------------------
		// Now, see if we hit the triangle
		//--------------------------------
		//
		if (Small_Enough(s1))
		{
			_ASSERT(!Small_Enough(s2));
			float beta = s0 / s2;
			if (beta >= 0.0f && beta < 1.0f)
			{
				_ASSERT(!Small_Enough(t1));
				float alpha = (t0 - beta * t2) / t1;
				local_hit = (alpha >= 0.0f && alpha + beta <= 1.0f);
			}
		}
		else
		{
			float beta = (t0 * s1 - s0 * t1);
			float alpha = (t2 * s1 - s2 * t1);
			beta /= alpha;
			if (beta >= 0.0f && beta <= 1.0f)
			{
				alpha = (s0 - beta * s2) / s1;
				local_hit = (alpha >= 0.0f && alpha + beta <= 1.0f);
			}
		}
		//
		//
		//----------------------------------------------------
		// handle the hit status, and move to the next polygon
		//----------------------------------------------------
		//
		if (local_hit)
		{
			hit = true;
			line->length = distance;
			if (negate)
				normal->Negate(plane->normal);
			else
				*normal = plane->normal;
			_ASSERT(*normal * line->direction <= -SMALL);
		}
	}
	//
	//----------------------
	// Return the hit status
	//----------------------
	//
	return hit;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
MLR_I_TMesh*
MidLevelRenderer::CreateIndexedTriCube_Nocolour_NoLit(float half, MLRState* state)
{
#if 0
	gos_PushCurrentHeap(Heap);
	MLR_I_TMesh* ret = new MLR_I_TMesh();
	Register_Object(ret);
	Point3D* coords = new Point3D [8];
	Register_Object(coords);
	coords[0] = Point3D(half, -half,  half);
	coords[1] = Point3D(-half, -half,  half);
	coords[2] = Point3D(half, -half, -half);
	coords[3] = Point3D(-half, -half, -half);
	coords[4] = Point3D(-half,  half,  half);
	coords[5] = Point3D(half,  half,  half);
	coords[6] = Point3D(half,  half, -half);
	coords[7] = Point3D(-half,  half, -half);
	uint8_t* lengths = new uint8_t [6];
	Register_Pointer(lengths);
	int32_t i;
	for(i = 0; i < 6; i++)
	{
		lengths[i] = 4;
	}
	ret->SetSubprimitiveLengths(lengths, 6);
	ret->SetCoordData(coords, 8);
	uint16_t* index = new uint16_t [6 * 4];
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
	Stuff::Vector2DScalar* texCoords = new Stuff::Vector2DScalar[8];
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
#endif
	return nullptr;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
MLRShape*
MidLevelRenderer::CreateIndexedTriIcosahedron_Nocolour_NoLit(
	IcoInfo& icoInfo, MLRState* state)
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
	Stuff::Vector2DScalar* texCoords = new Stuff::Vector2DScalar[nrTri * 3];
	Register_Pointer(texCoords);
	int32_t uniquePoints = 0;
	for (k = 0; k < 20; k++)
	{
		triDrawn = 0;
		MLR_I_TMesh* mesh = new MLR_I_TMesh();
		Register_Object(mesh);
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
		ret->Add(mesh);
		mesh->DetachReference();
	}
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
	gos_PopCurrentHeap();
	return ret;
}

} // namespace MidLevelRenderer
