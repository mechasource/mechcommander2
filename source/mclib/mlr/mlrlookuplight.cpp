//===========================================================================//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#include "stdinc.h"

#include "mlr/mlrlookuplight.h"

namespace MidLevelRenderer
{

//#############################################################################
//#########################    MLRLookUpLight ################################
//#############################################################################

MLRLookUpLight::ClassData* MLRLookUpLight::DefaultData = nullptr;

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
void MLRLookUpLight::InitializeClass()
{
	_ASSERT(!DefaultData);
	// _ASSERT(gos_GetCurrentHeap() == StaticHeap);
	DefaultData = new ClassData(
		MLRLookUpLightClassID, "MidLevelRenderer::MLRLookUpLight", MLRInfiniteLight::DefaultData);
	Check_Object(DefaultData);
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
void MLRLookUpLight::TerminateClass()
{
	Check_Object(DefaultData);
	delete DefaultData;
	DefaultData = nullptr;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
MLRLookUpLight::MLRLookUpLight()
	: MLRInfiniteLight(DefaultData)
{
	// _ASSERT(gos_GetCurrentHeap() == Heap);
	mapOrigin.x = 0.0f;
	mapOrigin.y = 0.0f;
	mapOrigin.z = 0.0f;
	mapZoneCountX = 1, mapZoneCountZ = 1;
	zoneSizeX = 1260.0f, zoneSizeZ = 1260.0f;
	one_Over_zoneSizeX = 1.0f / zoneSizeX;
	one_Over_zoneSizeZ = 1.0f / zoneSizeZ;
	maps = nullptr;
	mapName = "";
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
MLRLookUpLight::MLRLookUpLight(std::iostream stream, uint32_t version)
	: MLRInfiniteLight(DefaultData, stream, version)
{
	Check_Object(stream);
	// _ASSERT(gos_GetCurrentHeap() == Heap);
	*stream >> mapOrigin;
	*stream >> mapZoneCountX >> mapZoneCountZ;
	*stream >> zoneSizeX >> zoneSizeZ;
	*stream >> mapName;
	one_Over_zoneSizeX = 1.0f / zoneSizeX;
	one_Over_zoneSizeZ = 1.0f / zoneSizeZ;
	maps = new uint8_t*[mapZoneCountX * mapZoneCountZ];
	Check_Pointer(maps);
	for (size_t i = 0; i < mapZoneCountX * mapZoneCountZ; i++)
	{
		maps[i] = new uint8_t[256 * 256];
		Check_Pointer(maps[i]);
		stream->ReadBytes(maps[i], 256 * 256);
	}
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
MLRLookUpLight::MLRLookUpLight(Stuff::Page* page)
	: MLRInfiniteLight(DefaultData, page)
{
	Check_Object(page);
	// _ASSERT(gos_GetCurrentHeap() == Heap);
	maps = nullptr;
	std::wstring_view data;
	mapOrigin.x = 0.0f;
	mapOrigin.y = 0.0f;
	mapOrigin.z = 0.0f;
	if (page->GetEntry("MapOrigin", &data))
	{
		sscanf_s(data, "%f %f %f", &mapOrigin.x, &mapOrigin.y, &mapOrigin.z);
	}
	mapZoneCountX = 1, mapZoneCountZ = 1;
	if (page->GetEntry("MapSize", &data))
	{
		sscanf_s(data, "%d %d", &mapZoneCountX, &mapZoneCountZ);
	}
	zoneSizeX = 1280.0f, zoneSizeZ = 1280.0f;
	if (page->GetEntry("ZoneSize", &data))
	{
		sscanf_s(data, "%f %f", &zoneSizeX, &zoneSizeX);
	}
	one_Over_zoneSizeX = 1.0f / zoneSizeX;
	one_Over_zoneSizeZ = 1.0f / zoneSizeZ;
	mapName = "";
	if (page->GetEntry("MapName", &data))
	{
		mapName = data;
	}
	LoadMap();
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
MLRLookUpLight::~MLRLookUpLight()
{
	if (maps != nullptr)
	{
		for (size_t i = 0; i < mapZoneCountX * mapZoneCountZ; i++)
		{
			Check_Pointer(maps[i]);
			delete[] maps[i];
			maps[i] = nullptr;
		}
		Check_Pointer(maps);
		delete[] maps;
		maps = nullptr;
	}
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
void MLRLookUpLight::Save(std::iostream stream)
{
	// Check_Object(this);
	Check_Object(stream);
	MLRInfiniteLight::Save(stream);
	*stream << mapOrigin;
	*stream << mapZoneCountX << mapZoneCountZ;
	*stream << zoneSizeX << zoneSizeZ;
	*stream << mapName;
	for (size_t i = 0; i < mapZoneCountX * mapZoneCountZ; i++)
	{
		stream->WriteBytes(maps[i], 256 * 256);
	}
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
void MLRLookUpLight::Write(Stuff::Page* page)
{
	// Check_Object(this);
	Check_Object(page);
	MLRInfiniteLight::Write(page);
	wchar_t data[256];
	sprintf(data, "%f %f %f", mapOrigin.x, mapOrigin.y, mapOrigin.z);
	_ASSERT(strlen(data) < sizeof(data));
	page->SetEntry("MapOrigin", data);
	sprintf(data, "%d %d", mapZoneCountX, mapZoneCountZ);
	_ASSERT(strlen(data) < sizeof(data));
	page->SetEntry("MapSize", data);
	sprintf(data, "%f %f", zoneSizeX, zoneSizeZ);
	_ASSERT(strlen(data) < sizeof(data));
	page->SetEntry("ZoneSize", data);
	page->SetEntry("MapName", mapName);
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
void MLRLookUpLight::SetMapSizeAndName(int32_t x, int32_t z, std::wstring_view name)
{
	// Check_Object(this);
	if (maps != nullptr)
	{
		for (size_t i = 0; i < mapZoneCountX * mapZoneCountZ; i++)
		{
			Check_Pointer(maps[i]);
			delete[] maps[i];
			maps[i] = nullptr;
		}
		Check_Pointer(maps);
		delete[] maps;
		maps = nullptr;
	}
	mapZoneCountX = x;
	mapZoneCountZ = z;
	mapName = name;
	LoadMap();
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
bool MLRLookUpLight::LoadMap()
{
	// Check_Object(this);
	std::fstream element_stream(mapName);
	uint8_t* map = new uint8_t[mapZoneCountX * mapZoneCountZ * 256 * 256];
	Check_Pointer(map);
	element_stream.ReadBytes(map, mapZoneCountX * mapZoneCountZ * 256 * 256);
	_ASSERT(maps == nullptr);
	maps = new uint8_t*[mapZoneCountX * mapZoneCountZ];
	Check_Pointer(maps);
	int32_t i, j, k;
	for (j = 0; j < mapZoneCountZ; j++)
	{
		for (i = 0; i < mapZoneCountX; i++)
		{
			maps[j * mapZoneCountX + i] = new uint8_t[256 * 256];
			Check_Pointer(maps[j * mapZoneCountX + i]);
		}
	}
	uint8_t* uptr = map;
	for (j = 0; j < mapZoneCountZ; j++)
	{
		for (k = 0; k < 256; k++)
		{
			for (i = 0; i < mapZoneCountX; i++)
			{
				Mem_Copy(&maps[j * mapZoneCountX + i][k * 256], uptr, 256, (256 - k) * 256);
				uptr += 256;
				//					&map[(256*j+k)*(mapZoneCountX*256) + i*256]
			}
		}
	}
	Check_Pointer(map);
	delete map;
	return false;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
void MLRLookUpLight::SetLightToShapeMatrix(const LinearMatrix4D& worldToShape)
{
	// Check_Object(this);
	lightToShape.Multiply(lightToWorld, worldToShape);
	shapeToWorld.Invert(worldToShape);
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
void MLRLookUpLight::TestInstance()
{
	_ASSERT(IsDerivedFrom(DefaultData));
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
void MLRLookUpLight::LightVertex(const MLRVertexData& vertexData)
{
	UnitVector3D light_z;
	GetInShapeDirection(light_z);
	const float One_Over_255 = 1.f / 255.0f;
	Point3D worldPoint;
	worldPoint.Multiply(*(vertexData.point), shapeToWorld);
	float prep_x = mapZoneCountX * zoneSizeX - worldPoint.x + mapOrigin.x;
	float prep_z = mapZoneCountZ * zoneSizeZ - worldPoint.z + mapOrigin.z;
	int32_t map_x = Truncate_Float_To_Word(prep_x * one_Over_zoneSizeX);
	int32_t map_z = Truncate_Float_To_Word(prep_z * one_Over_zoneSizeZ);
	_ASSERT(map_x >= 0 && map_x < mapZoneCountX);
	_ASSERT(map_z >= 0 && map_z < mapZoneCountZ);
	int32_t off_x =
		Truncate_Float_To_Word((prep_x - map_x * zoneSizeX) * 256.0f * one_Over_zoneSizeX);
	int32_t off_z =
		Truncate_Float_To_Word((prep_z - map_z * zoneSizeZ) * 256.0f * one_Over_zoneSizeZ);
	_ASSERT(off_x >= 0 && off_x < 256);
	_ASSERT(off_z >= 0 && off_z < 256);
	float mapIntensity = maps[map_z * mapZoneCountX + map_x][(off_z << 8) + off_x] * One_Over_255;
	//
	//-------------------------------------------------------------------
	// Now we reduce the light level falling on the vertex based upon the
	// cosine of the angle between light and normal
	//-------------------------------------------------------------------
	//
	float cosine = -(light_z * (*vertexData.normal)) * mapIntensity * intensity;
#if COLOR_AS_DWORD
	TO_DO;
#else // COLOR_AS_DWORD
	RGBcolour light_color(color);
	if (cosine > SMALL)
	{
		light_color.red *= cosine;
		light_color.green *= cosine;
		light_color.blue *= cosine;
		vertexData.color->red += light_color.red;
		vertexData.color->green += light_color.green;
		vertexData.color->blue += light_color.blue;
	}
#endif // COLOR_AS_DWORD
}

} // namespace MidLevelRenderer
