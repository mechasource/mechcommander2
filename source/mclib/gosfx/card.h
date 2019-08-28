//===========================================================================//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#pragma once

//#include "gosfx/gosfx.h"
//#include "gosfx/singleton.h"
//#include "mlr/mlr.h"

namespace MidLevelRenderer
{
class MLRCardCloud;
}

namespace gosFX
{
//############################################################################
//########################  Card__Specification  #############################
//############################################################################

class Card;

class Card__Specification : public Singleton__Specification
{
	friend class Card;

	//----------------------------------------------------------------------
	// Constructors/Destructors
	//
protected:
	Card__Specification(std::iostream stream, uint32_t gfx_version);

public:
	Card__Specification(void);

	void Save(std::iostream stream);
	void BuildDefaults(void);
	bool IsDataValid(bool fix_data = false);
	static Card__Specification* Make(std::iostream stream, uint32_t gfx_version);
	void Copy(Card__Specification* spec);

	//-------------------------------------------------------------------------
	// FCurves
	//
public:
	SeededCurveOf<ConstantCurve, ComplexCurve, Curve::e_ConstantComplexType> m_halfheight;
	SeededCurveOf<ConstantCurve, ComplexCurve, Curve::e_ConstantComplexType> m_aspectRatio;
	SeededCurveOf<ComplexCurve, SplineCurve, Curve::e_ComplexSplineType> m_index;
	ConstantCurve m_UOffset;
	ConstantCurve m_VOffset;
	ConstantCurve m_USize;
	ConstantCurve m_VSize;
	bool m_animated;
	uint8_t m_width;

	void Setwidth(void);
};

//############################################################################
//#############################  Card  #################################
//############################################################################

class Card : public Singleton
{
	//----------------------------------------------------------------------------
	// Class Registration Support
	//
public:
	static void __stdcall InitializeClass(void);
	static void __stdcall TerminateClass(void);

	typedef Card__Specification Specification;

	//----------------------------------------------------------------------------
	// Class Data Support
	//
protected:
	Card(Specification* spec, uint32_t flags);
	~Card(void);

	float m_halfX;
	float m_halfY;
	MidLevelRenderer::MLRCardCloud* m_cardCloud;
	Stuff::Point3D m_vertices[4];
	Stuff::RGBAcolour m_colors[4];
	Stuff::Vector2DOf<float> m_uvs[4];
	const size_t* m_cardCount;

public:
	static Card* Make(Specification* spec, uint32_t flags);
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
	void Start(ExecuteInfo* info);
	bool Execute(ExecuteInfo* info);
	void Draw(DrawInfo* info);
	void Kill(void);
};
} // namespace gosFX
