//==========================================================================//
// File:	 gosFX_PertCloud.hpp											//
// Contents: PertCloud Component											//
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
class MLRNGonCloud;
}

namespace gosFX
{
//############################################################################
//########################  PertCloud__Specification
//#############################
//############################################################################

class PertCloud__Specification : public SpinningCloud__Specification
{
	//----------------------------------------------------------------------
	// Constructors/Destructors
	//
protected:
	PertCloud__Specification(std::iostream stream, uint32_t gfx_version);

public:
	PertCloud__Specification(uint32_t sides);

	static PertCloud__Specification* Make(std::iostream stream, uint32_t gfx_version);

	void Copy(PertCloud__Specification* spec);

	void Save(std::iostream stream);

	void BuildDefaults(void);

	bool IsDataValid(bool fix_data = false);

	//-------------------------------------------------------------------------
	// FCurves
	//
public:
	SeededCurveOf<ComplexCurve, SplineCurve, Curve::e_ComplexSplineType> m_size;
	SeededCurveOf<ConstantCurve, SplineCurve, Curve::e_ConstantSplineType> m_perturbation;
	SeededCurveOf<ComplexCurve, LinearCurve, Curve::e_ComplexLinearType> m_pCenterRed,
		m_pCenterGreen, m_pCenterBlue, m_pCenterAlpha;

	uint32_t m_vertices;
};

//############################################################################
//########################  SpinningCloud__Particle
//#############################
//############################################################################

class PertCloud__Particle : public SpinningCloud__Particle
{
public:
	Stuff::Point3D m_vertices[MidLevelRenderer::Limits::Max_Number_Of_NGon_Vertices];
};

//############################################################################
//#############################  PertCloud  ##################################
//############################################################################

class PertCloud : public SpinningCloud
{
	//----------------------------------------------------------------------------
	// Class Registration Support
	//
public:
	static void __stdcall InitializeClass(void);
	static void __stdcall TerminateClass(void);

	typedef PertCloud__Specification Specification;
	typedef PertCloud__Particle Particle;

protected:
	MidLevelRenderer::MLRNGonCloud* m_cloudImplementation; // Pert to an MLR Pert cloud by Jim
	Stuff::Point3D* m_P_vertices;
	Stuff::RGBAColor* m_P_color;

	//----------------------------------------------------------------------------
	// Class Data Support
	//
protected:
	PertCloud(Specification* spec, uint32_t flags);

public:
	~PertCloud(void);

	static PertCloud* Make(Specification* spec, uint32_t flags);

	Specification* GetSpecification()
	{
		// Check_Object(this);
		return Cast_Object(Specification*, m_specification);
	}
	Particle* GetParticle(uint32_t index)
	{
		// Check_Object(this);
		Check_Object(GetSpecification());
		return Cast_Pointer(Particle*, &m_data[index * GetSpecification()->m_particleClassSize]);
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
	bool AnimateParticle(
		uint32_t index, const Stuff::LinearMatrix4D* world_to_new_local, Stuff::Time till);
	void CreateNewParticle(uint32_t index, Stuff::Point3D* translation);
	void DestroyParticle(uint32_t index);

public:
	void Draw(DrawInfo* info);
};
} // namespace gosFX
