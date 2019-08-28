//===========================================================================//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#pragma once

#ifndef MLR_MLRSPOTLIGHT_HPP
#define MLR_MLRSPOTLIGHT_HPP

#include "mlr/mlrinfinitelightwithfalloff.h"

namespace MidLevelRenderer
{

//##########################################################################
//########################    MLRSpotLight    ##############################
//##########################################################################

class MLRSpotLight : public MLRInfiniteLightWithFalloff
{
public:
	static void __stdcall InitializeClass(void);
	static void __stdcall TerminateClass(void);

	MLRSpotLight(void);
	MLRSpotLight(std::iostream stream, uint32_t version);
	MLRSpotLight(Stuff::Page* page);
	~MLRSpotLight(void);

	void Save(std::iostream stream);
	void Write(Stuff::Page* page);

	virtual LightType GetLightType()
	{
		// Check_Object(this);
		return SpotLight;
	}

	//
	// spotlight spread.  This value is only valid if the light had falloff
	//
	bool GetSpreadAngle(Stuff::Radian* angle);
	void SetSpreadAngle(const Stuff::Radian& radian);

	void SetSpreadAngle(const Stuff::Degree& degree);

	float GetTanSpreadAngle()
	{
		// Check_Object(this);
		return tanSpreadAngle;
	}

	float GetCosSpreadAngle()
	{
		// Check_Object(this);
		return cosSpreadAngle;
	}

	virtual void LightVertex(const MLRVertexData&);

	void SetLightMap(MLRLightMap* light_map);

	MLRLightMap* GetLightMap()
	{
		// Check_Object(this);
		return lightMap;
	}

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
	MLRLightMap* lightMap;

	Stuff::Radian spreadAngle;
	float tanSpreadAngle, cosSpreadAngle;
};
} // namespace MidLevelRenderer
#endif
