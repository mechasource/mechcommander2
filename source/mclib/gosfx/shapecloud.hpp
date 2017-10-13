//==========================================================================//
// File:	 gosFX_ShapeCloud.hpp											//
// Contents: ShapeCloud Component											//
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//
//
#pragma once

//#include <gosfx/gosfx.hpp>
//#include <gosfx/spinningcloud.hpp>
//#include <mlr/mlr.hpp>

namespace MidLevelRenderer
{
class MLRShape;
}

namespace gosFX
{
//############################################################################
//########################  ShapeCloud__Specification
//#############################
//############################################################################

class ShapeCloud;

class ShapeCloud__Specification : public SpinningCloud__Specification
{
	friend class ShapeCloud;

	//----------------------------------------------------------------------
	// Constructors/Destructors
	//
  protected:
	ShapeCloud__Specification(
		Stuff::MemoryStream* stream, uint32_t gfx_version);

  public:
	ShapeCloud__Specification(MidLevelRenderer::MLRShape* shape);
	~ShapeCloud__Specification(void);

	void Save(Stuff::MemoryStream* stream);

	static ShapeCloud__Specification* Make(
		Stuff::MemoryStream* stream, uint32_t gfx_version);

	void Copy(ShapeCloud__Specification* spec);

	void SetShape(MidLevelRenderer::MLRShape* shape);

  protected:
	MidLevelRenderer::MLRShape* m_shape;
	float m_radius;
};

//############################################################################
//########################  SpinningCloud__Particle
//#############################
//############################################################################

class ShapeCloud__Particle : public SpinningCloud__Particle
{
  public:
	Stuff::RGBAColor m_color;
};

//############################################################################
//#############################  ShapeCloud  #################################
//############################################################################

class ShapeCloud : public SpinningCloud
{
	//----------------------------------------------------------------------------
	// Class Registration Support
	//
  public:
	static void __stdcall InitializeClass(void);
	static void __stdcall TerminateClass(void);

	typedef ShapeCloud__Specification Specification;
	typedef ShapeCloud__Particle Particle;

	//----------------------------------------------------------------------------
	// Class Data Support
	//
  protected:
	ShapeCloud(Specification* spec, uint32_t flags);

  public:
	static ShapeCloud* Make(Specification* spec, uint32_t flags);

	Specification* GetSpecification()
	{
		// Check_Object(this);
		return Cast_Object(Specification*, m_specification);
	}
	Particle* GetParticle(uint32_t index)
	{
		// Check_Object(this);
		Check_Object(GetSpecification());
		return Cast_Pointer(Particle*,
			&m_data[index * GetSpecification()->m_particleClassSize]);
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
	bool AnimateParticle(uint32_t index,
		const Stuff::LinearMatrix4D* world_to_new_local, Stuff::Time till);
	void CreateNewParticle(uint32_t index, Stuff::Point3D* translation);

  public:
	void Draw(DrawInfo* info);
};
}
