//==========================================================================//
// File:	 gosFX_Shape.hpp											    //
// Contents: Shape Component											    //
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//
//
#pragma once

//#include <gosfx/gosfx.hpp>
//#include <gosfx/singleton.hpp>
//#include <mlr/mlr.hpp>

namespace MidLevelRenderer
{
class MLRShape;
}

namespace gosFX
{
//############################################################################
//########################  Shape__Specification  #############################
//############################################################################

class Shape;

class Shape__Specification : public Singleton__Specification
{
	friend class Shape;

	//----------------------------------------------------------------------
	// Constructors/Destructors
	//
  protected:
	Shape__Specification(Stuff::MemoryStream* stream, uint32_t gfx_version);

  public:
	Shape__Specification(MidLevelRenderer::MLRShape* shape);
	~Shape__Specification(void);

	void Save(Stuff::MemoryStream* stream);
	static Shape__Specification* Make(
		Stuff::MemoryStream* stream, uint32_t gfx_version);
	void Copy(Shape__Specification* spec);
	void SetShape(MidLevelRenderer::MLRShape* shape);

  protected:
	MidLevelRenderer::MLRShape* m_shape;
	float m_radius;
};

//############################################################################
//#############################  Shape  #################################
//############################################################################

class Shape : public Singleton
{
	//----------------------------------------------------------------------------
	// Class Registration Support
	//
  public:
	static void __stdcall InitializeClass(void);
	static void __stdcall TerminateClass(void);

	typedef Shape__Specification Specification;

	//----------------------------------------------------------------------------
	// Class Data Support
	//
  protected:
	Shape(Specification* spec, uint32_t flags);

  public:
	static Shape* Make(Specification* spec, uint32_t flags);

	Specification* GetSpecification(void)
	{
		// Check_Object(this);
		return Cast_Object(Specification*, m_specification);
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
  public:
	void Draw(DrawInfo* info);
};
}
