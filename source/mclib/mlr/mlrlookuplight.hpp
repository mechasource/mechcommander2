//===========================================================================//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#pragma once

#ifndef MLR_MLRLOOKUPLIGHT_HPP
#define MLR_MLRLOOKUPLIGHT_HPP

#include <stuff/point3d.hpp>
#include <mlr/mlrinfinitelight.hpp>

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
	MLRLookUpLight(Stuff::MemoryStream* stream, uint32_t version);
	MLRLookUpLight(Stuff::Page* page);
	~MLRLookUpLight(void);

	void Save(Stuff::MemoryStream* stream);
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

	void SetMapSizeAndName(int32_t x, int32_t z, PCSTR name);

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

	PCSTR
	GetMapName(void)
	{
		// Check_Object(this);
		return mapName;
	}

	void SetMapZoneSizeX(float x)
	{
		// Check_Object(this);
		zoneSizeX = x;
		Verify(x > Stuff::SMALL);
		one_Over_zoneSizeX = 1.0f / x;
	}
	void SetMapZoneSizeZ(float z)
	{
		// Check_Object(this);
		zoneSizeZ = z;
		Verify(z > Stuff::SMALL);
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
	Stuff::MString mapName;

	puint8_t* maps;

	Stuff::LinearMatrix4D shapeToWorld;
};
}
#endif
