//==========================================================================//
// File:	 gosFX_DebrisCloud.hpp											//
// Contents: DebrisCloud Component											//
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//
//
#pragma once

//#include "gosfx/gosfx.h"
//#include "gosfx/spinningcloud.h"
//#include "mlr/mlr.h"

namespace MidLevelRenderer
{
class MLRShape;
}

namespace gosFX
{
//############################################################################
//#######################  DebrisCloud__Specification  #######################
//############################################################################

class DebrisCloud;

class DebrisCloud__Specification : public Effect__Specification
{
	friend class DebrisCloud;

	//----------------------------------------------------------------------
	// Constructors/Destructors
	//
protected:
	DebrisCloud__Specification(std::iostream stream, uint32_t gfx_version);

public:
	DebrisCloud__Specification(void);
	~DebrisCloud__Specification(void);

	void Save(std::iostream stream);

	static DebrisCloud__Specification* Make(std::iostream stream, uint32_t gfx_version);

	void Copy(DebrisCloud__Specification* spec);

	void Load(std::iostream stream);

	void LoadGeometry(std::iostream stream);

	void BuildDefaults(void);

	virtual bool IsDataValid(bool fix_data = false);

	//-------------------------------------------------------------------------
	// FCurves
	//
public:
	ConstantCurve m_minimumDeviation;
	SplineCurve m_maximumDeviation;
	SeededCurveOf<ComplexCurve, ComplexCurve, Curve::e_ComplexComplexType> m_startingSpeed;
	SeededCurveOf<ComplexCurve, SplineCurve, Curve::e_ComplexSplineType> m_pLifeSpan;
	SeededCurveOf<ConstantCurve, LinearCurve, Curve::e_ConstantLinearType> m_pEtherVelocityY;
	SeededCurveOf<SplineCurve, LinearCurve, Curve::e_SplineLinearType> m_pAccelerationY;
	SeededCurveOf<ComplexCurve, ComplexCurve, Curve::e_ComplexComplexType> m_pDrag;
	SeededCurveOf<ComplexCurve, LinearCurve, Curve::e_ComplexLinearType> m_pAlpha;
	SeededCurveOf<ConstantCurve, LinearCurve, Curve::e_ConstantLinearType> m_pSpin;

	//-------------------------------------------------------------------------
	// Data
	//
	Stuff::Point3D centerOfForce;

	std::vector<MidLevelRenderer::MLRShape*> debrisPieces;
	std::vector<Stuff::Point3D> debrisPositions;
	std::vector<Stuff::Sphere> debrisSpheres;
	std::vector<float> debrisSeed;
};

//############################################################################
//#########################  DebrisCloud__Particle  #########################
//############################################################################

class DebrisCloud__Particle
{
public:
	Stuff::LinearMatrix4D m_localToParent;

	float m_age, m_ageRate, m_seed;

	Stuff::Vector3D m_angularVelocity, m_linearVelocity;

	float m_alpha;

	void TestInstance(void) const {}
};

//############################################################################
//#############################  DebrisCloud  ################################
//############################################################################

class DebrisCloud : public Effect
{
	//----------------------------------------------------------------------------
	// Class Registration Support
	//
public:
	static void __stdcall InitializeClass(void);
	static void __stdcall TerminateClass(void);

	typedef DebrisCloud__Specification Specification;
	typedef DebrisCloud__Particle Particle;

	//----------------------------------------------------------------------------
	// Class Data Support
	//
protected:
	DebrisCloud(Specification* spec, uint32_t flags);

public:
	static DebrisCloud* Make(Specification* spec, uint32_t flags);

	Specification* GetSpecification()
	{
		// Check_Object(this);
		return Cast_Object(Specification*, m_specification);
	}
	Particle* GetParticle(uint32_t index)
	{
		// Check_Object(this);
		Check_Object(GetSpecification());
		return Cast_Pointer(Particle*, &debrisPieces[index]);
	}

	static ClassData* DefaultData;

	//----------------------------------------------------------------------------
	// Testing
	//
public:
	void TestInstance(void) const;

	//----------------------------------------------------------------------------
	// API
	//
protected:
	bool Execute(ExecuteInfo* info);
	bool AnimateParticle(uint32_t index, const Stuff::LinearMatrix4D* world_to_new_local, Stuff::Time till);
	virtual void DestroyParticle(uint32_t index);
	void ComputeNewLinearVelocity(Particle* particle, float time_slice);

public:
	void Start(ExecuteInfo* info);
	void Kill(void);
	bool HasFinished(void);
	void Draw(DrawInfo* info);

protected:
	std::vector<DebrisCloud__Particle> debrisPieces;
};
} // namespace gosFX
