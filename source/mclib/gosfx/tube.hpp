//==========================================================================//
// File:	 gosFX_Tube.hpp										            //
// Contents: Base Tube Profile									            //
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//
//
#pragma once

//#include <gosfx/gosfx.hpp>
//#include <gosfx/effect.hpp>

namespace MidLevelRenderer
{
class MLRIndexedTriangleCloud;
}

namespace gosFX
{
//############################################################################
//########################  Tube__Specification  #############################
//############################################################################

class Tube__Specification : public Effect__Specification
{
	//----------------------------------------------------------------------
	// Constructors/Destructors
	//
  protected:
	Tube__Specification(Stuff::MemoryStream* stream, uint32_t gfx_version);

  public:
	Tube__Specification(void);

	static Tube__Specification* Make(
		Stuff::MemoryStream* stream, uint32_t gfx_version);

	void Copy(Tube__Specification* spec);

	void Save(Stuff::MemoryStream* stream);

	void BuildDefaults(void);

	bool IsDataValid(bool fix_data = false);

	bool CalculateUBias(bool adjust);

	//-------------------------------------------------------------------------
	// FCurves
	//
  public:
	ConstantCurve m_profilesPerSecond, m_pLifeSpan;
	LinearCurve m_emitterSizeX, m_emitterSizeY, m_emitterSizeZ;
	ConstantCurve m_minimumDeviation;
	ComplexCurve m_maximumDeviation;
	SeededCurveOf<ComplexCurve, SplineCurve, Curve::e_ComplexSplineType>
		m_pDisplacement, m_pScale;
	SeededCurveOf<ComplexCurve, LinearCurve, Curve::e_ComplexLinearType> m_pRed,
		m_pGreen, m_pBlue, m_pAlpha;
	SeededCurveOf<ComplexCurve, SplineCurve, Curve::e_ComplexSplineType>
		m_pUOffset, m_pVOffset;
	SeededCurveOf<SplineCurve, SplineCurve, Curve::e_SplineSplineType> m_pUSize,
		m_pVSize;

	//----------------------------------------------------------------------
	// Data
	//
  public:
	int32_t m_maxProfileCount;

	enum ProfileType
	{
		e_Ribbon,
		e_AlignedRibbon,
		e_Triangle,
		e_Square,
		e_Cross,
		e_Pentagon,
		e_Hexagon,
		e_VerticalRibbon
	} m_profileType;
	bool m_insideOut;

	Stuff::DynamicArrayOf<Stuff::Point3D> m_vertices;
	Stuff::DynamicArrayOf<Stuff::Vector2DOf<float>> m_uvs;

	float m_UBias;

	void BuildTemplate(void);
};

//############################################################################
//########################  Tube__Profile  #############################
//############################################################################

class Tube__Profile
{
  public:
	Stuff::LinearMatrix4D m_profileToWorld;
	Stuff::UnitVector3D m_direction;
	float m_age, m_ageRate, m_seed;

	void TestInstance(void) const {}
};

//############################################################################
//##############################  Tube  #############################
//############################################################################

class Tube : public Effect
{
  public:
	static void __stdcall InitializeClass(void);
	static void __stdcall TerminateClass(void);

	static ClassData* DefaultData;

	typedef Tube__Specification Specification;
	typedef Tube__Profile Profile;

  protected:
	int32_t m_headProfile;
	int32_t m_tailProfile;
	size_t m_activeProfileCount;
	size_t m_triangleCount;
	size_t m_vertexCount;
	float m_birthAccumulator;

	Stuff::DynamicArrayOf<Profile> m_profiles;
	Stuff::DynamicArrayOf<char> m_data;
	MidLevelRenderer::MLRIndexedTriangleCloud* m_mesh;
	Stuff::Point3D* m_P_vertices;
	Stuff::RGBAColor* m_P_colors;
	Stuff::Vector2DOf<float>* m_P_uvs;

	void BuildMesh(puint16_t indices);

	Tube(Specification* spec, uint32_t flags);

	//----------------------------------------------------------------------------
	// Class Data Support
	//
  public:
	~Tube(void);

	static Tube* Make(Specification* spec, uint32_t flags);

	Specification* GetSpecification()
	{
		// Check_Object(this);
		return Cast_Object(Specification*, m_specification);
	}
	Profile* GetProfile(uint32_t index)
	{
		// Check_Object(this);
		Check_Object(GetSpecification());
		return &m_profiles[index];
	}

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
	bool AnimateProfile(uint32_t index, uint32_t profile,
		const Stuff::LinearMatrix4D& world_to_new_local, Stuff::Time till,
		Stuff::Sphere* sphere);
	void CreateNewProfile(uint32_t index, const Stuff::LinearMatrix4D& origin);
	void DestroyProfile(uint32_t index);
	void ComputeNewLinearVelocity(Profile* particle, float time_slice);

  public:
	void Start(ExecuteInfo* info);
	void Kill(void);
	bool HasFinished(void);
	void Draw(DrawInfo* info);
};
}
