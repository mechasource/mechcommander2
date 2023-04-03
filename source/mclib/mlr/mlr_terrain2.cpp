//===========================================================================//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#include "stdinc.h"

#include "mlr/mlr_terrain2.h"

namespace MidLevelRenderer
{

//#############################################################################

#if defined(TRACE_ENABLED) && defined(MLR_TRACE)
BitTrace* MLR_Terrain2_Clip;
#endif

extern uint32_t gEnableLightMaps;

//#############################################################################
//## MLRTerrain with no color no lighting w/ detail texture, uv's from xyz  ###
//#############################################################################

std::vector<Stuff::Vector2DScalar>* MLR_Terrain2::detailTexCoords;

MLR_Terrain2::ClassData* MLR_Terrain2::DefaultData = nullptr;

extern std::vector<Stuff::Vector2DScalar>* lightMapUVs;
extern std::vector<float>* lightMapSqFalloffs;

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
void MLR_Terrain2::InitializeClass()
{
	_ASSERT(!DefaultData);
	// _ASSERT(gos_GetCurrentHeap() == StaticHeap);
	DefaultData = new ClassData(MLR_Terrain2ClassID, "MidLevelRenderer::MLR_Terrain2",
		MLR_I_DeT_TMesh::DefaultData, (MLRPrimitiveBase::Factory)&Make);
	Register_Object(DefaultData);
	detailTexCoords = new std::vector<Stuff::Vector2DScalar>(Limits::Max_Number_Vertices_Per_Mesh);
	Register_Object(detailTexCoords);
#if defined(TRACE_ENABLED) && defined(MLR_TRACE)
	MLR_Terrain2_Clip = new BitTrace("MLR_Terrain2_Clip");
	Register_Object(MLR_Terrain2_Clip);
#endif
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
void MLR_Terrain2::TerminateClass()
{
	Unregister_Object(DefaultData);
	delete DefaultData;
	DefaultData = nullptr;
	Unregister_Object(detailTexCoords);
	delete detailTexCoords;
#if defined(TRACE_ENABLED) && defined(MLR_TRACE)
	Unregister_Object(MLR_Terrain2_Clip);
	delete MLR_Terrain2_Clip;
#endif
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
MLR_Terrain2::MLR_Terrain2(ClassData* class_data, std::iostream stream, uint32_t version)
	: MLR_I_DeT_TMesh(class_data, stream, version)
{
	// Check_Pointer(this);
	Check_Pointer(stream);
	// _ASSERT(gos_GetCurrentHeap() == Heap);
	*stream >> tileX >> tileZ;
	*stream >> maxDepth >> maxAllDepth;
	if (version > 4)
	{
		float* fptr = &frame[0][0];
		for (size_t i = 0; i < 32; i++)
		{
			*stream >> *fptr++;
		}
	}
	else
	{
		float xOffset, zOffset, xGrid, zGrid;
		*stream >> xOffset >> zOffset;
		*stream >> xGrid >> zGrid;
		frame[0][0] = xOffset;
		frame[0][1] = zOffset;
		frame[0][2] = xOffset + 8 * xGrid;
		frame[0][3] = zOffset + 8 * zGrid;
		frame[1][0] = xOffset + 4 * (tileX / 4) * xGrid;
		frame[1][1] = zOffset + 4 * (tileZ / 4) * zGrid;
		frame[1][2] = frame[1][0] + 4 * xGrid;
		frame[1][3] = frame[1][1] + 4 * zGrid;
		int32_t i;
		for (i = 2; i < 8; i++)
		{
			frame[i][0] = xOffset + tileX * xGrid;
			frame[i][1] = zOffset + tileZ * zGrid;
			frame[i][2] = frame[i][0] + xGrid;
			frame[i][3] = frame[i][1] + zGrid;
		}
	}
	*stream >> borderPixelFun;
	uint8_t textureFlags, mask = 1;
	*stream >> textureFlags;
	for (size_t i = 0; i < 8; i++)
	{
		if (textureFlags | mask)
		{
			textures[i] = 1;
		}
		else
		{
			textures[i] = 0;
		}
		mask <<= 1;
	}
	Check_Object(MLRTexturePool::Instance);
	MLRTexture* orgTexture = (*MLRTexturePool::Instance)[referenceState.GetTextureHandle()];
	Check_Object(orgTexture);
	std::wstring_view texName = orgTexture->GetTextureName();
	wchar_t texRoot[1024], name[1024];
	size_t len;
	if ((len = strlen(texName)) > 0)
	{
		_ASSERT(len > 8);
		int32_t i, d = texName[len - 6] - '0';
		strncpy(texRoot, texName, len - 7);
		texRoot[len - 7] = '\0';
		textures[d] = referenceState.GetTextureHandle();
		MLRTexture* texture;
		uint8_t mask = 1;
		for (i = 0; i < 8; i++)
		{
			if (textureFlags & mask)
			{
				sprintf(name, "%s_%1d_%02x%02x", texRoot, i, (7 - tileX) / (1 << (maxAllDepth - i)),
					(7 - tileZ) / (1 << (maxAllDepth - i)));
				texture = (*MLRTexturePool::Instance)(name, 0);
				if (!texture)
				{
					texture = MLRTexturePool::Instance->Add(name, 0);
				}
				Check_Object(texture);
				texture->SetHint(orgTexture->GetHint());
				textures[i] = texture->GetTextureHandle();
			}
			else
			{
				textures[i] = 0;
			}
			mask <<= 1;
		}
		i++;
		for (; i < 8; i++)
		{
			textures[i] = 0;
		}
	}
	else
	{
		for (size_t i = 0; i < 8; i++)
		{
			textures[i] = 0;
		}
	}
	_ASSERT(textures[0] != 0);
	currentDepth = -1;
	SetCurrentDepth(1);
	referenceState.SetLightingMode(MLRState::LightMapLightingMode);
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
MLR_Terrain2::MLR_Terrain2(ClassData* class_data)
	: MLR_I_DeT_TMesh(class_data)
{
	// Check_Pointer(this);
	// _ASSERT(gos_GetCurrentHeap() == Heap);
	tileX = 0;
	tileZ = 0;
	maxDepth = 0;
	maxAllDepth = 0;
	borderPixelFun = 0.0f;
	float* fptr = &frame[0][0];
	int32_t i;
	for (i = 0; i < 32; i++)
	{
		*fptr++ = 0.0f;
	}
	for (i = 0; i < 8; i++)
	{
		textures[i] = 0;
	}
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
MLR_Terrain2::~MLR_Terrain2()
{
	// Check_Object(this);
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
MLR_Terrain2*
MLR_Terrain2::Make(std::iostream stream, uint32_t version)
{
	Check_Object(stream);
#ifdef _GAMEOS_HPP_
	gos_PushCurrentHeap(Heap);
#endif
	MLR_Terrain2* terrain = new MLR_Terrain2(DefaultData, stream, version);
	gos_PopCurrentHeap();
	return terrain;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
void MLR_Terrain2::Save(std::iostream stream)
{
	// Check_Object(this);
	Check_Object(stream);
	texCoords.SetLength(0);
	MLR_I_DeT_TMesh::Save(stream);
	CalculateUVs();
	*stream << tileX << tileZ;
	*stream << maxDepth << maxAllDepth;
	float* fptr = &frame[0][0];
	for (size_t i = 0; i < 32; i++)
	{
		*stream << *fptr++;
	}
	*stream << borderPixelFun;
	uint8_t textureFlags = 0;
	//	HACK
	/*
		Check_Object(MLRTexturePool::Instance);
		MLRTexture *orgTexture =
	(*MLRTexturePool::Instance)[referenceState.GetTextureHandle()]; std::wstring_view
	texName = orgTexture->GetTextureName(); wchar_t texRoot[1024], name[1024];

		size_t len;
		if((len = strlen(texName)) > 0)
		{
			_ASSERT(len>8);
			int32_t i, d = texName[len-6] - '0';
			_ASSERT(d==maxDepth);
			strncpy(texRoot, texName, len-7);
			texRoot[len-7] = '\0';

			textures[d] = referenceState.GetTextureHandle();

			MLRTexture *texture;

			uint8_t mask = 1;
			for(i=0;i<d;i++)
			{
				sprintf(name, "%s_%1d_%02x%02x", texRoot, i,
	tileX/(1<<(maxAllDepth-i)), tileZ/(1<<(maxAllDepth-i)));

				_ASSERT((1<<i)>(tileX/(1<<(maxAllDepth-i))));
				_ASSERT((1<<i)>(tileZ/(1<<(maxAllDepth-i))));
	//				SPEW(("micgaert", "%s", name));

				texture = (*MLRTexturePool::Instance)(name, 0);
				if (texture)
				{
					textureFlags |= mask;
					textures[i] = texture->GetTextureHandle();
				}
				mask <<= 1;
			}
		}
		_ASSERT(textures[0]!=0);
	*/
	textureFlags = 3;
	//	HACK
	*stream << textureFlags;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
void MLR_Terrain2::TestInstance(void) const
{
	_ASSERT(IsDerivedFrom(DefaultData));
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
void MLR_Terrain2::SetCurrentDepth(uint8_t d)
{
	if (d == currentDepth)
	{
		return;
	}
	else
	{
		_ASSERT(d <= maxAllDepth);
		uint8_t dt;
		dt = d < maxDepth ? d : maxDepth;
		while (dt > 0 && textures[dt] == 0)
		{
			dt--;
		}
		currentDepth = dt;
	}
	CalculateUVs();
	referenceState.SetTextureHandle(textures[currentDepth]);
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
void MLR_Terrain2::SetLevelTexture(int32_t lev, int32_t handle)
{
	// Check_Object(this);
	_ASSERT(lev >= 0 && lev < 8);
	textures[lev] = handle;
	if (lev == currentDepth)
	{
		referenceState.SetTextureHandle(textures[currentDepth]);
	}
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
void MLR_Terrain2::CalculateUVs()
{
	if (texCoords.GetLength() != coords.GetLength())
	{
#ifdef _GAMEOS_HPP_
		gos_PushCurrentHeap(Heap);
#endif
		texCoords.SetLength(coords.GetLength());
		gos_PopCurrentHeap();
	}
	float maxX = frame[currentDepth][2];
	float maxZ = frame[currentDepth][3];
	float OneOverX = 1.0f / (frame[currentDepth][2] - frame[currentDepth][0]);
	float OneOverZ = 1.0f / (frame[currentDepth][3] - frame[currentDepth][1]);
	for (size_t i = 0; i < texCoords.GetLength(); i++)
	{
		texCoords[i][0] =
			borderPixelFun + (1.0f - 2 * borderPixelFun) * (maxX - coords[i].x) * OneOverX;
		texCoords[i][1] =
			borderPixelFun + (1.0f - 2 * borderPixelFun) * (maxZ - coords[i].z) * OneOverZ;
		_ASSERT(texCoords[i][0] >= 0.0 && texCoords[i][0] <= 1.0);
		_ASSERT(texCoords[i][1] >= 0.0 && texCoords[i][1] <= 1.0);
	}
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//

#define I_SAY_YES_TO_DETAIL_TEXTURES
#define I_SAY_YES_TO_TERRAIN2
#undef I_SAY_YES_TO_DUAL_TEXTURES
#undef I_SAY_YES_TO_COLOR
#undef I_SAY_YES_TO_TERRAIN
#undef I_SAY_YES_TO_LIGHTING

#define CLASSNAME MLR_Terrain2

#if defined(TRACE_ENABLED) && defined(MLR_TRACE)
#define SET_MLR_TMESH_CLIP() MLR_Terrain2_Clip->Set()
#define CLEAR_MLR_TMESH_CLIP() MLR_Terrain2_Clip->Clear()
#else
#define SET_MLR_TMESH_CLIP()
#define CLEAR_MLR_TMESH_CLIP()
#endif

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//	This include contains follwing functions:
//	void MLR_Terrain2::TransformNoClip(Matrix4D*, GOSVertexPool*);
//	int32_t MLR_Terrain2::Clip(MLRClippingState, GOSVertexPool*);
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#include "mlr/mlrtriangleclipping.inl"

#undef I_SAY_YES_TO_DETAIL_TEXTURES
#undef I_SAY_YES_TO_TERRAIN2

#undef CLASSNAME

extern RGBAcolour errorcolour;
extern bool
CheckForBigTriangles(std::vector<Stuff::Vector2DScalar>* lightMapUVs, uint32_t stride);

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
void MLR_Terrain2::LightMapLighting(MLRLight* light)
{
	if (!gEnableLightMaps)
	{
		return;
	}
	int32_t i, j, k, len = numOfTriangles;
	LinearMatrix4D matrix = LinearMatrix4D::Identity;
	Point3D lightPosInShape, hitPoint;
	UnitVector3D up, left, forward;
	bool lm;
	float falloff = 1.0f, distance;
	MLRLightMap* lightMap = light->GetLightMap();
	if ((!lightMap) || (!gEnableLightMaps))
	{
		return;
	}
	int32_t tooBig = 0;
	float bigUV = MLRState::GetMaxUV();
	switch (light->GetLightType())
	{
	case MLRLight::PointLight:
	{
		Check_Object(lightMap);
		lightMap->AddState(referenceState.GetPriority() + 1);
		light->GetInShapePosition(lightPosInShape);
		float n, f;
		Cast_Object(MLRPointLight*, light)->GetFalloffDistance(n, f);
		float One_Over_Falloff = 1.0f / (3.0f * f);
		for (i = 0, j = 0, k = 0; i < len; i++, j += 3)
		{
			if (testList[i] == 0)
			{
				continue;
			}
			f = facePlanes[i].GetDistanceTo(lightPosInShape);
			lm = false;
			tooBig = 0;
			for (k = 0; k < 3; k++)
			{
				Vector3D vec(coords[index[k + j]]);
				vec -= lightPosInShape;
				(*lightMapUVs)[k][0] = vec.x * One_Over_Falloff;
				(*lightMapUVs)[k][1] = vec.z * One_Over_Falloff;
				falloff = 1.0f;
				if ((*lightMapUVs)[k][0] >= -0.5f && (*lightMapUVs)[k][0] <= 0.5f && (*lightMapUVs)[k][1] >= -0.5f && (*lightMapUVs)[k][1] <= 0.5f)
				{
					lm = true;
				}
				if ((*lightMapUVs)[k][0] < -bigUV || (*lightMapUVs)[k][0] > bigUV || (*lightMapUVs)[k][1] < -bigUV || (*lightMapUVs)[k][1] > bigUV)
				{
					tooBig++;
				}
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
				RGBAcolour color;
				light->Getcolour(color);
				color.red *= sq_falloff;
				color.green *= sq_falloff;
				color.blue *= sq_falloff;
				color.alpha = 1.0f;
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

} // namespace MidLevelRenderer
