//===========================================================================//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#pragma once

//#include <gosfx/gosfx.hpp>
//#include <gosfx/spinningcloud.hpp>
//#include <mlr/mlr.hpp>

namespace MidLevelRenderer
{
class MLRCardCloud;
}

namespace gosFX
{
//############################################################################
//########################  CardCloud__Specification
//#############################
//############################################################################

class CardCloud__Specification : public SpinningCloud__Specification
{
	//----------------------------------------------------------------------
	// Constructors/Destructors
	//
protected:
	CardCloud__Specification(std::iostream stream, uint32_t gfx_version);

public:
	CardCloud__Specification(void);

	static CardCloud__Specification* Make(std::iostream stream, uint32_t gfx_version);
	void Copy(CardCloud__Specification* spec);
	void Save(std::iostream stream);
	void BuildDefaults(void);
	virtual bool IsDataValid(bool fix_data = false);

	//-------------------------------------------------------------------------
	// FCurves
	//
public:
	SeededCurveOf<ComplexCurve, ComplexCurve, Curve::e_ComplexComplexType> m_halfHeight;
	SeededCurveOf<ComplexCurve, ComplexCurve, Curve::e_ComplexComplexType> m_aspectRatio;
	SeededCurveOf<ComplexCurve, SplineCurve, Curve::e_ComplexSplineType> m_pIndex;
	ConstantCurve m_UOffset;
	ConstantCurve m_VOffset;
	ConstantCurve m_USize;
	ConstantCurve m_VSize;

	bool m_animated;
	uint8_t m_width;

	void SetWidth(void);
};

//############################################################################
//########################  SpinningCloud__Particle
//#############################
//############################################################################

class CardCloud__Particle : public SpinningCloud__Particle
{
public:
	float m_halfX, m_halfY;
};

//############################################################################
//#############################  CardCloud  #################################
//############################################################################

class CardCloud : public SpinningCloud
{
	//----------------------------------------------------------------------------
	// Class Registration Support
	//
public:
	static void __stdcall InitializeClass(void);
	static void __stdcall TerminateClass(void);

	typedef CardCloud__Specification Specification;
	typedef CardCloud__Particle Particle;

public:
	enum
	{
		ParticleSize = sizeof(Particle) + 4 * sizeof(Stuff::Point3D) + sizeof(Stuff::RGBAColor) +
			4 * sizeof(Stuff::Vector2DOf<float>)
	};

protected:
	MidLevelRenderer::MLRCardCloud*
		m_cloudImplementation; // point to an MLR triangle cloud by Michael
	Stuff::Point3D* m_P_vertices;
	Stuff::RGBAColor* m_P_color;
	Stuff::Vector2DOf<float>* m_P_uvs;

	//----------------------------------------------------------------------------
	// Class Data Support
	//
protected:
	CardCloud(Specification* spec, uint32_t flags);

public:
	~CardCloud(void);

	static CardCloud* Make(Specification* spec, uint32_t flags);

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
