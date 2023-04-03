//===========================================================================//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#pragma once

#ifndef MLR_MLRLOOKUPLIGHT_HPP
#define MLR_MLRLOOKUPLIGHT_HPP

#include "stuff/point3d.h"
#include "mlr/mlrinfinitelight.h"

namespace MidLevelRenderer
{

//##########################################################################
//######################## MLRLookUpLight ############################
//##########################################################################

class MLRLookUpLight : public MLRInfiniteLight
{
public:
	static void __stdcall InitializeClass(void);
	static void __stdcall TerminateClass(void);

	MLRLookUpLight(void);
	MLRLookUpLight(std::iostream stream, uint32_t version);
	MLRLookUpLight(Stuff::Page* page);
	~MLRLookUpLight(void);

	void Save(std::iostream stream);
	void Write(Stuff::Page* page);

	virtual LightType GetLightType(void)
	{
		// Check_Object(this);
		return LookUpLight;
	}

	virtual void LightVertex(const MLRVertexData&);

	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// LookUp Light Specific
	//
	void SetMapOrigin(float x, float y, float z)
	{
		// Check_Object(this);
		mapOrigin.x = x;
		mapOrigin.y = y;
		mapOrigin.z = z;
	}

	Stuff::Point3D GetMapOrigin(void)
	{
		// Check_Object(this);
		return mapOrigin;
	}

	void SetMapSizeAndName(int32_t x, int32_t z, std::wstring_view name);

	int32_t GetMapZoneCountX(void)
	{
		// Check_Object(this);
		return mapZoneCountX;
	}
	int32_t GetMapZoneCountZ(void)
	{
		// Check_Object(this);
		return mapZoneCountZ;
	}

	std::wstring_view
	GetMapName(void)
	{
		// Check_Object(this);
		return mapName;
	}

	void SetMapZoneSizeX(float x)
	{
		// Check_Object(this);
		zoneSizeX = x;
		_ASSERT(x > Stuff::SMALL);
		one_Over_zoneSizeX = 1.0f / x;
	}
	void SetMapZoneSizeZ(float z)
	{
		// Check_Object(this);
		zoneSizeZ = z;
		_ASSERT(z > Stuff::SMALL);
		one_Over_zoneSizeZ = 1.0f / z;
	}

	float GetMapZoneSizeX(void)
	{
		// Check_Object(this);
		return zoneSizeX;
	}
	float GetMapZoneSizeZ(void)
	{
		// Check_Object(this);
		return zoneSizeZ;
	}

	void SetLightToShapeMatrix(const Stuff::LinearMatrix4D&);

	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Class Data Support
	//
public:
	static ClassData* DefaultData;

	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Testing
	//
public:
	void TestInstance(void);

protected:
	bool LoadMap(void);

	Stuff::Point3D mapOrigin;
	float zoneSizeX, zoneSizeZ;
	float one_Over_zoneSizeX, one_Over_zoneSizeZ;

	int32_t mapZoneCountX, mapZoneCountZ;
	std::wstring mapName;

	uint8_t** maps;

	Stuff::LinearMatrix4D shapeToWorld;
};
} // namespace MidLevelRenderer
#endif
