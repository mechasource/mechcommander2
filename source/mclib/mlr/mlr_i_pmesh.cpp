//===========================================================================//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#include "stdinc.h"

#include <gameos.hpp>
#include <stuff/line.hpp>
#include <mlr/mlrsorter.hpp>
#include <mlr/mlrlight.hpp>
#include <mlr/mlrlightmap.hpp>
#include <mlr/mlrspotlight.hpp>
#include <mlr/mlrinfinitelightwithfalloff.hpp>
#include <mlr/mlrshape.hpp>
#include <mlr/mlr_i_pmesh.hpp>

using namespace MidLevelRenderer;

//#############################################################################

#if defined(TRACE_ENABLED) && defined(MLR_TRACE)
BitTrace* MLR_I_PMesh_Clip;
#endif

#define UV_TEST 0

extern uint32_t gEnableLightMaps;

//#############################################################################
//###### MLRIndexedPolyMesh with no color no lighting one texture layer  ######
//#############################################################################

MLR_I_PMesh::ClassData* MLR_I_PMesh::DefaultData = nullptr;
std::vector<Stuff::Vector2DScalar>* lightMapUVs;
std::vector<float>* lightMapSqFalloffs;

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
void MLR_I_PMesh::InitializeClass(void)
{
	_ASSERT(!DefaultData);
	// _ASSERT(gos_GetCurrentHeap() == StaticHeap);
	DefaultData = new ClassData(MLR_I_PMeshClassID, "MidLevelRenderer::MLR_I_PMesh",
		MLRIndexedPrimitiveBase::DefaultData, (MLRPrimitiveBase::Factory)&Make);
	Register_Object(DefaultData);
#if defined(TRACE_ENABLED) && defined(MLR_TRACE)
	MLR_I_PMesh_Clip = new BitTrace("MLR_I_PMesh_Clip");
	Register_Object(MLR_I_PMesh_Clip);
#endif
	lightMapUVs = new std::vector<Stuff::Vector2DScalar>(Limits::Max_Number_Vertices_Per_Polygon);
	Register_Object(lightMapUVs);
	lightMapSqFalloffs = new std::vector<float>(Limits::Max_Number_Vertices_Per_Polygon);
	Register_Object(lightMapSqFalloffs);
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
void MLR_I_PMesh::TerminateClass(void)
{
	Unregister_Object(lightMapSqFalloffs);
	delete lightMapSqFalloffs;
	Unregister_Object(lightMapUVs);
	delete lightMapUVs;
	Unregister_Object(DefaultData);
	delete DefaultData;
	DefaultData = nullptr;
#if defined(TRACE_ENABLED) && defined(MLR_TRACE)
	Unregister_Object(MLR_I_PMesh_Clip);
	delete MLR_I_PMesh_Clip;
#endif
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
MLR_I_PMesh::MLR_I_PMesh(ClassData* class_data, std::iostream stream, uint32_t version)
	: MLRIndexedPrimitiveBase(class_data, stream, version)
{
	// Check_Pointer(this);
	Check_Pointer(stream);
	// _ASSERT(gos_GetCurrentHeap() == Heap);
	facePlanes.SetLength(GetNumPrimitives());
	testList.SetLength(GetNumPrimitives());
	FindFacePlanes();
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
MLR_I_PMesh::MLR_I_PMesh(ClassData* class_data) : MLRIndexedPrimitiveBase(class_data)
{
	// Check_Pointer(this);
	// _ASSERT(gos_GetCurrentHeap() == Heap);
	drawMode = SortData::TriIndexedList;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/*
void
	MLR_I_PMesh::Copy(MLRIndexedPolyMesh *polyMesh)
{
	//Check_Pointer(this);
	//_ASSERT(gos_GetCurrentHeap() == Heap);

	size_t len;
	puint16_t _index;
	Stuff::Point3D *_coords;
	Stuff::Vector2DScalar *_texCoords;
	puint8_t _lengths;

	polyMesh->GetCoordData(&_coords, &len);
	SetCoordData(_coords, len);

	polyMesh->GetIndexData(&_index, &len);
	SetIndexData(_index, len);

	polyMesh->GetSubprimitiveLengths(&_lengths);
	SetSubprimitiveLengths(_lengths, polyMesh->GetNumPrimitives());

	facePlanes.SetLength(GetNumPrimitives());
	testList.SetLength(GetNumPrimitives());

	FindFacePlanes();

	polyMesh->GetTexCoordData(&_texCoords, &len);
	SetTexCoordData(_texCoords, len);

	referenceState = polyMesh->GetReferenceState();

	visibleIndexedVerticesKey = false;
	visibleIndexedVertices.SetLength(coords.GetLength());
}
*/

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
MLR_I_PMesh::~MLR_I_PMesh(void)
{
	// Check_Object(this);
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
MLR_I_PMesh* MLR_I_PMesh::Make(std::iostream stream, uint32_t version)
{
	Check_Object(stream);
#ifdef _GAMEOS_HPP_
	gos_PushCurrentHeap(Heap);
#endif
	MLR_I_PMesh* mesh = new MLR_I_PMesh(DefaultData, stream, version);
#ifdef _GAMEOS_HPP_
	gos_PopCurrentHeap();
#endif

	return mesh;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
void MLR_I_PMesh::TestInstance(void) const { _ASSERT(IsDerivedFrom(DefaultData)); }

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
void MLR_I_PMesh::InitializeDrawPrimitive(uint8_t vis, int32_t parameter)
{
	MLRIndexedPrimitiveBase::InitializeDrawPrimitive(vis, parameter);
	if (parameter & 1)
	{
		ResetTestList();
	}
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
void MLR_I_PMesh::SetSubprimitiveLengths(puint8_t data, size_t numPrimitives)
{
	// Check_Object(this);
	// _ASSERT(gos_GetCurrentHeap() == Heap);
	lengths.AssignData(data, numPrimitives);
	facePlanes.SetLength(numPrimitives);
	testList.SetLength(numPrimitives);
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
void MLR_I_PMesh::GetSubprimitiveLengths(puint8_t* pdata, psize_t plength)
{
	// Check_Object(this);
	*plength = lengths.GetLength();
	*pdata   = lengths.GetData();
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
void MLR_I_PMesh::FindFacePlanes(void)
{
	// Check_Object(this);
	size_t i, j, stride, numPrimitives = GetNumPrimitives();
	Stuff::Vector3D v;
	_ASSERT(index.GetLength() > 0);
	for (i = 0, j = 0; i < numPrimitives; i++)
	{
		stride = lengths[i];
		facePlanes[i].BuildPlane(coords[index[j]], coords[index[j + 1]], coords[index[j + 2]]);
		j += stride;
	}
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
int32_t MLR_I_PMesh::FindBackFace(const Stuff::Point3D& u)
{
	// Check_Object(this);
	size_t i, numPrimitives = GetNumPrimitives();
	int32_t ret = 0;
	size_t len  = lengths.GetLength();
	puint8_t iPtr;
	Stuff::Plane* p;

	if (len <= 0)
	{
		visible = 0;
		return 0;
	}
	p	= &facePlanes[0];
	iPtr = &testList[0];
	if (state.GetBackFaceMode() == MLRState::BackFaceOffMode)
	{
		ResetTestList();
		ret = 1;
	}
	else
	{
		for (i = 0; i < numPrimitives; i++, p++, iPtr++)
		{
			*iPtr = uint8_t((p->GetDistanceTo(u) >= 0.0f) ? 0xff : 0);
			ret += *iPtr;
		}
		visible = uint8_t(ret ? 1 : 0);
	}
	visible = uint8_t(ret ? 1 : 0);
	FindVisibleVertices();
	return ret;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
void MLR_I_PMesh::ResetTestList(void)
{
	size_t i, numPrimitives = GetNumPrimitives();
	puint8_t iPtr = &testList[0];
	for (i = 0; i < numPrimitives; i++, iPtr++)
	{
		*iPtr = 1;
	}
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
uint32_t MLR_I_PMesh::FindVisibleVertices(void)
{
	// Check_Object(this);
	_ASSERT(index.GetLength() > 0);
	size_t ret, i, j, k, stride, len = lengths.GetLength();
	_ASSERT(index.GetLength() > 0);
	for (i = 0, j = 0, ret = 0; i < len; i++)
	{
		stride = lengths[i];
		_ASSERT(stride >= 3);
		if (testList[i] == 0)
		{
			j += stride;
			continue;
		}
		for (k = j; k < j + stride; k++)
		{
			visibleIndexedVertices[index[k]] = 1;
			ret++;
		}
		j += stride;
	}
	visibleIndexedVerticesKey = true;
	return ret;
}

extern uint32_t gEnableTextureSort, gEnableAlphaSort;

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
void MLR_I_PMesh::Transform(Stuff::Matrix4D* mat)
{
	// Check_Object(this);
	Start_Timer(Transform_Time);
	_ASSERT(index.GetLength() > 0);
	size_t i, len = coords.GetLength();
	if (visibleIndexedVerticesKey == false)
	{
		FindVisibleVertices();
	}
	Stuff::Vector4D* v4d = transformedCoords->GetData();
	Stuff::Point3D* p3d  = coords.GetData();
	MLRClippingState* cs = clipPerVertex->GetData();
	puint8_t viv		 = visibleIndexedVertices.GetData();
	for (i = 0; i < len; i++, p3d++, v4d++, cs++, viv++)
	{
		if (*viv == 0)
		{
			continue;
		}
		v4d->Multiply(*p3d, *mat);
#ifdef LAB_ONLY
		Set_Statistic(TransformedVertices, TransformedVertices + 1);
#endif
		cs->Clip4dVertex(v4d);
//
//--------------------------------------------------------
// I claims all vertices are in. lets check it. who knows
//--------------------------------------------------------
//
#ifdef LAB_ONLY
		if ((*cs) == 0)
		{
#if defined(_ARMOR)
			if (Stuff::ArmorLevel > 3)
			{
				//
				//--------------------------------------------------------
				// I claims all vertices are in. lets check it. who knows
				//--------------------------------------------------------
				//
				_ASSERT(v4d->x >= 0.0f && v4d->x <= v4d->w);
				_ASSERT(v4d->y >= 0.0f && v4d->y <= v4d->w);
				_ASSERT(v4d->z >= 0.0f && v4d->z <= v4d->w);
			}
#endif
			Set_Statistic(NonClippedVertices, NonClippedVertices + 1);
		}
		else
		{
			Set_Statistic(ClippedVertices, ClippedVertices + 1);
		}
#endif
	}
	Stop_Timer(Transform_Time);
}

#undef I_SAY_YES_TO_DUAL_TEXTURES
#undef I_SAY_YES_TO_COLOR
#undef I_SAY_YES_TO_LIGHTING

#define CLASSNAME MLR_I_PMesh

#if defined(TRACE_ENABLED) && defined(MLR_TRACE)
#define SET_MLR_PMESH_CLIP() MLR_I_PMesh_Clip->Set()
#define CLEAR_MLR_PMESH_CLIP() MLR_I_PMesh_Clip->Clear()
#else
#define SET_MLR_PMESH_CLIP()
#define CLEAR_MLR_PMESH_CLIP()
#endif

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//	This include contains follwing functions:
//	void MLR_I_PMesh::TransformNoClip(Matrix4D*, GOSVertexPool*);
//	int32_t MLR_I_PMesh::Clip(MLRClippingState, GOSVertexPool*);
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#include <mlr/mlrprimitiveclipping.inl>

#undef CLASSNAME

//---------------------------------------------------------------------------
//
void MLR_I_PMesh::Lighting(MLRLight* const* lights, uint32_t nrLights)
{
	size_t i;
	MLRLightMap* lightMap;
	for (i = 0; i < nrLights; i++)
	{
		lightMap = lights[i]->GetLightMap();
		if (lightMap != nullptr)
		{
			LightMapLighting(lights[i]);
		}
	}
}

Stuff::RGBAColor errorColor;

//---------------------------------------------------------------------------
//
// warning C6244: Local declaration of 'lightMapUVs' hides previous declaration
// at line '34'
bool CheckForBigTriangles(std::vector<Stuff::Vector2DScalar>* lightMapUVs2, uint32_t stride)
{
	size_t k1, k0;
	float u1v0, u0v1;
	size_t up = 0, down = 0, right = 0, left = 0;

	for (k0 = 0; k0 < stride; k0++)
	{
		if ((*lightMapUVs2)[k0][1] > 0.5f)
		{
			up++;
		}
		if ((*lightMapUVs2)[k0][1] < -0.5f)
		{
			down++;
		}
		if ((*lightMapUVs2)[k0][0] > 0.5f)
		{
			right++;
		}
		if ((*lightMapUVs2)[k0][0] < -0.5f)
		{
			left++;
		}
	}
	if (up == stride || down == stride || left == stride || right == stride)
	{
		errorColor = Stuff::RGBAColor(0.5f, 0.5f, 0.0f, 1.0f);
		return false;
	}
#if 1
	float sign = -1.0f;
	if (((*lightMapUVs2)[1][0] - (*lightMapUVs2)[0][0]) *
			((*lightMapUVs2)[stride - 1][1] - (*lightMapUVs2)[0][1]) >
		((*lightMapUVs2)[1][1] - (*lightMapUVs2)[0][1]) *
			((*lightMapUVs2)[stride - 1][0] - (*lightMapUVs2)[0][0]))
	{
		sign = 1.0f;
	}
#endif
	for (k0 = 0; k0 < stride; k0++)
	{
		k1   = (k0 + 1 < stride) ? k0 + 1 : 0;
		u1v0 = (*lightMapUVs2)[k1][0] * (*lightMapUVs2)[k0][1];
		u0v1 = (*lightMapUVs2)[k0][0] * (*lightMapUVs2)[k1][1];
		if (sign * (u1v0 - u0v1) < 0.0f)
		//		if( (u1v0 - u0v1) < 0.0f )
		{
			continue;
		}
		if ((((*lightMapUVs2)[k1][0] * (*lightMapUVs2)[k1][0] -
				 2.0f * (*lightMapUVs2)[k1][0] * (*lightMapUVs2)[k0][0] +
				 (*lightMapUVs2)[k0][0] * (*lightMapUVs2)[k0][0]) +
				((*lightMapUVs2)[k1][1] * (*lightMapUVs2)[k1][1] -
					2.0f * (*lightMapUVs2)[k1][1] * (*lightMapUVs2)[k0][1] +
					(*lightMapUVs2)[k0][1] * (*lightMapUVs2)[k0][1])) <
			(2.0f * (u1v0 * u1v0 - 2.0f * u1v0 * u0v1 + u0v1 * u0v1)))
		{
#if UV_TEST
			DEBUG_STREAM << k0 << endl;
			for (size_t i = 0; i < stride; i++)
			{
				DEBUG_STREAM << (*lightMapUVs2)[i][0] << " " << (*lightMapUVs2)[i][1] << endl;
			}
			DEBUG_STREAM << endl;
#endif
			errorColor = Stuff::RGBAColor(0.0f, 0.5f, 0.0f, 1.0f);
			return false;
		}
	}
	return true;
}

//---------------------------------------------------------------------------
//
void MLR_I_PMesh::LightMapLighting(MLRLight* light)
{
	if (!gEnableLightMaps)
	{
		return;
	}
	uint16_t stride;
	size_t i, j, k, len = lengths.GetLength();
	Stuff::LinearMatrix4D matrix = Stuff::LinearMatrix4D::Identity;
	Stuff::Point3D lightPosInShape, hitPoint;
	Stuff::UnitVector3D up, left, forward;
	bool lm;
	float f, rhf, falloff = 1.0f, distance;
	MLRLightMap* lightMap = light->GetLightMap();
	if ((!lightMap) || (!gEnableLightMaps))
	{
		return;
	}
	switch (light->GetLightType())
	{
	case MLRLight::PointLight:
	{
		lightMap->AddState(referenceState.GetPriority() + 1);
		light->GetInShapePosition(lightPosInShape);
		for (i = 0, j = 0, k = 0; i < len; i++, j += stride)
		{
			stride = lengths[i];
			if (testList[i] == 0)
			{
				continue;
			}
			f  = facePlanes[i].GetDistanceTo(lightPosInShape);
			lm = false;
			if (f > 0.0f &&
				Cast_Object(MLRInfiniteLightWithFalloff*, light)->GetFalloff(f, falloff) == true)
			{
				rhf	= 1.0f / f;
				matrix = Stuff::LinearMatrix4D::Identity;
				matrix.AlignLocalAxisToWorldVector(
					facePlanes[i].normal, Stuff::Z_Axis, Stuff::Y_Axis, Stuff::X_Axis);
				matrix.GetWorldLeftInLocal(&left);
				matrix.GetWorldUpInLocal(&up);
				matrix.GetWorldForwardInLocal(&forward);
				_ASSERT(Stuff::Small_Enough(up * left));
#if defined(_ARMOR)
				float diff = forward * left;
				_ASSERT(Stuff::Small_Enough(diff));
				diff = up * forward;
				_ASSERT(Stuff::Small_Enough(diff));
#endif
				Check_Object(&forward);
				hitPoint.Multiply(forward, -f);
				hitPoint += lightPosInShape;
				for (k = 0; k < stride; k++)
				{
					Stuff::Vector3D vec(coords[index[k + j]]);
					vec -= hitPoint;
					(*lightMapUVs)[k][0] = -(left * vec) * rhf;
					(*lightMapUVs)[k][1] = -(up * vec) * rhf;
					if ((*lightMapUVs)[k][0] >= -0.5f && (*lightMapUVs)[k][0] <= 0.5f &&
						(*lightMapUVs)[k][1] >= -0.5f && (*lightMapUVs)[k][1] <= 0.5f)
					{
						lm = true;
					}
				}
			}
			else
			{
				continue;
			}
			if (lm == true || CheckForBigTriangles(lightMapUVs, stride) == true)
			{
				lightMap->SetPolygonMarker(0);
				lightMap->AddUShort(stride);
				float sq_falloff = falloff * falloff * light->GetIntensity();
				Stuff::RGBAColor color(sq_falloff, sq_falloff, sq_falloff, 1.0f);
				lightMap->AddColor(color);
				for (k = 0; k < stride; k++)
				{
					lightMap->AddCoord(coords[index[k + j]]);
				}
				for (k = 0; k < stride; k++)
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
		lightMap->AddState(referenceState.GetPriority() + 1);
		light->GetInShapePosition(matrix);
		lightPosInShape		= matrix;
		float tanSpeadAngle = Cast_Object(MLRSpotLight*, light)->GetTanSpreadAngle();
		matrix.GetLocalLeftInWorld(&left);
		matrix.GetLocalUpInWorld(&up);
		matrix.GetLocalForwardInWorld(&forward);
		_ASSERT(Stuff::Small_Enough(up * left));
		for (i = 0, j = 0, k = 0; i < len; i++, j += stride)
		{
			behindCount  = 0;
			falloffCount = 0;
			stride		 = lengths[i];
			if (testList[i] == 0)
			{
				continue;
			}
			lm = false;
			if (!facePlanes[i].IsSeenBy(lightPosInShape))
			{
				continue;
			}
			int32_t tooBig = 0;
			for (k = 0; k < stride; k++)
			{
				Stuff::Vector3D vec;
				vec.Subtract(coords[index[k + j]], lightPosInShape);
				distance = (vec * forward);
				if (distance > Stuff::SMALL)
				{
					if (Cast_Object(MLRInfiniteLightWithFalloff*, light)
							->GetFalloff(distance, falloff) == false)
					{
						falloffCount++;
					}
					(*lightMapSqFalloffs)[k] = falloff * falloff * light->GetIntensity();
				}
				else
				{
					behindCount++;
					break;
				}
				(*lightMapUVs)[k][0] = (left * vec) / (2.0f * distance * tanSpeadAngle);
				(*lightMapUVs)[k][1] = -(up * vec) / (2.0f * distance * tanSpeadAngle);
				if ((*lightMapUVs)[k][0] >= -0.5f && (*lightMapUVs)[k][0] <= 0.5f &&
					(*lightMapUVs)[k][1] >= -0.5f && (*lightMapUVs)[k][1] <= 0.5f)
				{
					lm = true;
				}
				if ((*lightMapUVs)[k][0] < -1.5f || (*lightMapUVs)[k][0] > 1.5f ||
					(*lightMapUVs)[k][1] < -1.5f && (*lightMapUVs)[k][1] > 1.5f)
				{
					tooBig++;
				}
			}
#if 1
			if (tooBig == 0 && behindCount == 0 && falloffCount < stride &&
				(lm == true || CheckForBigTriangles(lightMapUVs, stride) == true))
			{
				lightMap->SetPolygonMarker(1);
				lightMap->AddUShort(stride);
				for (k = 0; k < stride; k++)
				{
					lightMap->AddCoord(coords[index[k + j]]);
				}
				for (k = 0; k < stride; k++)
				{
					lightMap->AddColor((*lightMapSqFalloffs)[k], (*lightMapSqFalloffs)[k],
						(*lightMapSqFalloffs)[k], 1.0f);
				}
				for (k = 0; k < stride; k++)
				{
					lightMap->AddUVs((*lightMapUVs)[k][0] + 0.5f, (*lightMapUVs)[k][1] + 0.5f);
					//				DEBUG_STREAM << k << " " << lightMapUVs[k][0] << "
					//" << lightMapUVs[k][0] << "\n";
				}
			}
#else
			if (tooBig != 0)
			{
				lightMap->SetPolygonMarker(1);
				lightMap->AddUShort(stride);
				for (k = 0; k < stride; k++)
				{
					lightMap->AddCoord(coords[index[k + j]]);
				}
				for (k = 0; k < stride; k++)
				{
					lightMap->AddColor(Stuff::RGBAColor(0.0f, 0.0f, 0.5f, 1.0f));
				}
				for (k = 0; k < stride; k++)
				{
					lightMap->AddUVs(0.5f, 0.5f);
					//				DEBUG_STREAM << k << " " << lightMapUVs[k][0] << "
					//" << lightMapUVs[k][0] << "\n";
				}
			}
			else if (behindCount != 0)
			{
				lightMap->SetPolygonMarker(1);
				lightMap->AddUShort(stride);
				for (k = 0; k < stride; k++)
				{
					lightMap->AddCoord(coords[index[k + j]]);
				}
				for (k = 0; k < stride; k++)
				{
					lightMap->AddColor(Stuff::RGBAColor(0.5f, 0.0f, 0.0f, 1.0f));
				}
				for (k = 0; k < stride; k++)
				{
					lightMap->AddUVs(0.5f, 0.5f);
					//				DEBUG_STREAM << k << " " << lightMapUVs[k][0] << "
					//" << lightMapUVs[k][0] << "\n";
				}
			}
			else if (behindCount == 0 &&
				(lm == true || CheckForBigTriangles(&lightMapUVs, stride) == true))
			{
				lightMap->SetPolygonMarker(1);
				lightMap->AddUShort(stride);
				for (k = 0; k < stride; k++)
				{
					lightMap->AddCoord(coords[index[k + j]]);
				}
				for (k = 0; k < stride; k++)
				{
					lightMap->AddColor(
						lightMapSqFalloffs[k], lightMapSqFalloffs[k], lightMapSqFalloffs[k], 1.0f);
				}
				for (k = 0; k < stride; k++)
				{
					lightMap->AddUVs(lightMapUVs[k][0] + 0.5f, lightMapUVs[k][1] + 0.5f);
					//				DEBUG_STREAM << k << " " << lightMapUVs[k][0] << "
					//" << lightMapUVs[k][0] << "\n";
				}
			}
			else if (CheckForBigTriangles(&lightMapUVs, stride) == false)
			{
				lightMap->SetPolygonMarker(1);
				lightMap->AddUShort(stride);
				for (k = 0; k < stride; k++)
				{
					lightMap->AddCoord(coords[index[k + j]]);
				}
				for (k = 0; k < stride; k++)
				{
					lightMap->AddColor(errorColor);
				}
				for (k = 0; k < stride; k++)
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

	case MLRLight::AmbientLight:
	case MLRLight::InfiniteLight:
	case MLRLight::InfiniteLightWithFallOff:
	case MLRLight::LookUpLight:
	default:
		// STOP(("MLR_I_PMesh::LightMapLighting: What you want me to do ?"));
		// warning C4061: enumerator 'LookUpLight' in switch of enum
		// 'MidLevelRenderer::MLRLight::LightType' is not explicitly handled by
		// a case label
		NODEFAULT;
		break;
	}
}

//---------------------------------------------------------------------------
//
bool MLR_I_PMesh::CastRay(Stuff::Line3D* line, Stuff::Normal3D* normal)
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
	size_t poly_start = 0, numPrimitives = GetNumPrimitives();
	bool hit = false;
	for (size_t polygon = 0; polygon < numPrimitives; ++polygon)
	{
		size_t stride = lengths[polygon];
		_ASSERT(stride > 2);
		//
		//---------------------------------
		// See if the line misses the plane
		//---------------------------------
		//
		float product;
		const Stuff::Plane* plane = &facePlanes[polygon];
		Check_Object(plane);
		float distance = line->GetDistanceTo(*plane, &product);
		if (distance < 0.0f || distance > line->length)
		{
			poly_start += stride;
			continue;
		}
		bool negate = false;
		if (product > -Stuff::SMALL)
		{
			if (GetCurrentState().GetBackFaceMode() == MLRState::BackFaceOnMode)
			{
				poly_start += stride;
				continue;
			}
			negate = true;
		}
		//
		//-------------------------------------------
		// Figure out where on the plane the line hit
		//-------------------------------------------
		//
		Stuff::Point3D impact;
		line->Project(distance, &impact);
		//
		//-------------------------------------------------------------------
		// We now need to find out which cardinal plane we should project the
		// triangle onto
		//-------------------------------------------------------------------
		//
		size_t s, t;
		float nx = abs(plane->normal.x);
		float ny = abs(plane->normal.y);
		float nz = abs(plane->normal.z);
		if (nx > ny)
		{
			if (nx > nz)
			{
				s = Stuff::Y_Axis;
				t = Stuff::Z_Axis;
			}
			else
			{
				s = Stuff::X_Axis;
				t = Stuff::Y_Axis;
			}
		}
		else if (ny > nz)
		{
			s = Stuff::Z_Axis;
			t = Stuff::X_Axis;
		}
		else
		{
			s = Stuff::X_Axis;
			t = Stuff::Y_Axis;
		}
		//
		//----------------------------------------
		// Initialize the vertex and leg variables
		//----------------------------------------
		//
		Stuff::Point3D* v1;
		Stuff::Point3D* v2;
		Stuff::Point3D* v3;
		v1 = &coords[index[poly_start]];
		v2 = &coords[index[poly_start + 1]];
		v3 = &coords[index[poly_start + 2]];
		//
		//---------------------------------------
		// Get the projection of the impact point
		//---------------------------------------
		//
		float s0 = (impact[s] - (*v1)[s]);
		float t0 = (impact[t] - (*v1)[t]);
		float s1 = ((*v2)[s] - (*v1)[s]);
		float t1 = ((*v2)[t] - (*v1)[t]);
		//
		//------------------------------------------------------------
		// For each triangle, figure out what the second leg should be
		//------------------------------------------------------------
		//
		bool local_hit = false;
		size_t next_v  = 3;
	Test_Triangle:
		Check_Pointer(v3);
		float s2 = (*v3)[s] - (*v1)[s];
		float t2 = (*v3)[t] - (*v1)[t];
		//
		//--------------------------------
		// Now, see if we hit the triangle
		//--------------------------------
		//
		if (Stuff::Small_Enough(s1))
		{
			_ASSERT(!Stuff::Small_Enough(s2));
			float beta = s0 / s2;
			if (beta >= 0.0f && beta < 1.0f)
			{
				_ASSERT(!Stuff::Small_Enough(t1));
				float alpha = (t0 - beta * t2) / t1;
				local_hit   = (alpha >= 0.0f && alpha + beta <= 1.0f);
			}
		}
		else
		{
			float beta  = (t0 * s1 - s0 * t1);
			float alpha = (t2 * s1 - s2 * t1);
			beta /= alpha;
			if (beta >= 0.0f && beta <= 1.0f)
			{
				alpha	 = (s0 - beta * s2) / s1;
				local_hit = (alpha >= 0.0f && alpha + beta <= 1.0f);
			}
		}
		//
		//-----------------------------
		// Set up for the next triangle
		//-----------------------------
		//
		if (next_v < stride && !local_hit)
		{
			v2 = v3;
			v3 = &coords[index[poly_start + next_v++]];
			s1 = s2;
			t1 = t2;
			goto Test_Triangle;
		}
		//
		//----------------------------------------------------
		// Handle the hit status, and move to the next polygon
		//----------------------------------------------------
		//
		if (local_hit)
		{
			hit			 = true;
			line->length = distance;
			if (negate)
				normal->Negate(plane->normal);
			else
				*normal = plane->normal;
			_ASSERT(*normal * line->direction <= -Stuff::SMALL);
		}
		poly_start += stride;
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
MLR_I_PMesh* MidLevelRenderer::CreateIndexedCube_NoColor_NoLit(float half, MLRState* state)
{
#ifdef _GAMEOS_HPP_
	gos_PushCurrentHeap(Heap);
#endif
	MLR_I_PMesh* ret = new MLR_I_PMesh();
	Register_Object(ret);
	Stuff::Point3D* coords = new Stuff::Point3D[8];
	Register_Object(coords);
	coords[0]		 = Stuff::Point3D(half, -half, half);
	coords[1]		 = Stuff::Point3D(-half, -half, half);
	coords[2]		 = Stuff::Point3D(half, -half, -half);
	coords[3]		 = Stuff::Point3D(-half, -half, -half);
	coords[4]		 = Stuff::Point3D(-half, half, half);
	coords[5]		 = Stuff::Point3D(half, half, half);
	coords[6]		 = Stuff::Point3D(half, half, -half);
	coords[7]		 = Stuff::Point3D(-half, half, -half);
	puint8_t lengths = new uint8_t[6];
	Register_Pointer(lengths);
	int32_t i;
	for (i = 0; i < 6; i++)
	{
		lengths[i] = 4;
	}
	ret->SetSubprimitiveLengths(lengths, 6);
	ret->SetCoordData(coords, 8);
	puint16_t index = new uint16_t[6 * 4];
	Register_Pointer(index);
	index[0]  = 0;
	index[1]  = 2;
	index[2]  = 6;
	index[3]  = 5;
	index[4]  = 0;
	index[5]  = 5;
	index[6]  = 4;
	index[7]  = 1;
	index[8]  = 5;
	index[9]  = 6;
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
	Register_Pointer(texCoords);
	texCoords[0] = Stuff::Vector2DScalar(0.0f, 0.0f);
	texCoords[1] = Stuff::Vector2DScalar(0.0f, 0.0f);
	texCoords[2] = Stuff::Vector2DScalar(0.0f, 0.0f);
	texCoords[3] = Stuff::Vector2DScalar(0.0f, 0.0f);
	texCoords[4] = Stuff::Vector2DScalar(0.0f, 0.0f);
	texCoords[5] = Stuff::Vector2DScalar(0.0f, 0.0f);
	texCoords[6] = Stuff::Vector2DScalar(0.0f, 0.0f);
	texCoords[7] = Stuff::Vector2DScalar(0.0f, 0.0f);
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
	}
	ret->SetTexCoordData(texCoords, 8);
	Unregister_Object(texCoords);
	delete[] texCoords;
	Unregister_Pointer(index);
	delete[] index;
	Unregister_Pointer(lengths);
	delete[] lengths;
	Unregister_Object(coords);
	delete[] coords;
#ifdef _GAMEOS_HPP_
	gos_PopCurrentHeap();
#endif

	return ret;
}

uint32_t MidLevelRenderer::triDrawn = 0;

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
void MidLevelRenderer::subdivide(Stuff::Point3D* coords, Stuff::Point3D& v1, Stuff::Point3D& v2,
	Stuff::Point3D& v3, uint32_t depth, uint32_t tri2draw, float rad)
{
	Stuff::Point3D v12(0.0f, 0.0f, 0.0f), v23(0.0f, 0.0f, 0.0f), v31(0.0f, 0.0f, 0.0f);
	size_t i;
	if (depth == 0)
	{
		if (tri2draw > triDrawn)
		{
			coords[3 * triDrawn].Multiply(v1, rad);
			coords[3 * triDrawn + 1].Multiply(v3, rad);
			coords[3 * triDrawn + 2].Multiply(v2, rad);
		}
		triDrawn++;
		return;
	}
	for (i = 0; i < 3; i++)
	{
		v12[i] = v1[i] + v2[i];
		v23[i] = v2[i] + v3[i];
		v31[i] = v3[i] + v1[i];
	}
	v12.Normalize(v12);
	v23.Normalize(v23);
	v31.Normalize(v31);
	MidLevelRenderer::subdivide(coords, v1, v12, v31, depth - 1, tri2draw, rad);
	MidLevelRenderer::subdivide(coords, v2, v23, v12, depth - 1, tri2draw, rad);
	MidLevelRenderer::subdivide(coords, v3, v31, v23, depth - 1, tri2draw, rad);
	MidLevelRenderer::subdivide(coords, v12, v23, v31, depth - 1, tri2draw, rad);
}

float MidLevelRenderer::vdata[12][3] = {{-ICO_X, 0.0f, ICO_Z}, {ICO_X, 0.0f, ICO_Z},
	{-ICO_X, 0.0f, -ICO_Z}, {ICO_X, 0.0f, -ICO_Z}, {0.0f, ICO_Z, ICO_X}, {0.0f, ICO_Z, -ICO_X},
	{0.0f, -ICO_Z, ICO_X}, {0.0f, -ICO_Z, -ICO_X}, {ICO_Z, ICO_X, 0.0f}, {-ICO_Z, ICO_X, 0.0f},
	{ICO_Z, -ICO_X, 0.0f}, {-ICO_Z, -ICO_X, 0.0f}};

uint32_t MidLevelRenderer::tindices[20][3] = {{0, 4, 1}, {0, 9, 4}, {9, 5, 4}, {4, 5, 8}, {4, 8, 1},
	{8, 10, 1}, {8, 3, 10}, {5, 3, 8}, {5, 2, 3}, {2, 7, 3}, {7, 10, 3}, {7, 6, 10}, {7, 11, 6},
	{11, 0, 6}, {0, 1, 6}, {6, 1, 10}, {9, 0, 11}, {9, 11, 2}, {9, 2, 5}, {7, 2, 11}};

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
MLRShape* MidLevelRenderer::CreateIndexedIcosahedron_NoColor_NoLit(
	IcoInfo& icoInfo, MLRState* state)
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
	size_t uniquePoints = 0;
	for (k = 0; k < 20; k++)
	{
		triDrawn		  = 0;
		MLR_I_PMesh* mesh = new MLR_I_PMesh();
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
			uniquePoints	   = 1;
			collapsedCoords[0] = coords[0];
			index[0]		   = 0;
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
	Unregister_Pointer(lengths);
	delete[] lengths;
#ifdef _GAMEOS_HPP_
	gos_PopCurrentHeap();
#endif

	return ret;
}
