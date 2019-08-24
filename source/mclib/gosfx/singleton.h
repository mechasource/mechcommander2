//==========================================================================//
// File:	 gosFX_Singleton.hpp									    	//
// Contents: Base Singleton Particle									    //
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//
//
#pragma once

//#include "gosfx/gosfx.h"
//#include "gosfx/effect.h"

namespace gosFX
{
//############################################################################
//########################  Singleton__Specification
//#############################
//############################################################################

class Singleton__Specification : public Effect__Specification
{
	//----------------------------------------------------------------------
	// Constructors/Destructors
	//
protected:
	Singleton__Specification(Stuff::RegisteredClass::ClassID class_id, std::iostream stream,
		uint32_t gfx_version);

public:
	Singleton__Specification(Stuff::RegisteredClass::ClassID class_id);

	void Copy(Singleton__Specification* spec);

	void Save(std::iostream stream);

	void BuildDefaults(void);

	bool IsDataValid(bool fix_data = false);

	//-------------------------------------------------------------------------
	// FCurves
	//
public:
	SeededCurveOf<ComplexCurve, LinearCurve, Curve::e_ComplexLinearType> m_red, m_green, m_blue,
		m_alpha;
	SeededCurveOf<ComplexCurve, ComplexCurve, Curve::e_ComplexComplexType> m_scale;

	bool m_alignZUsingX, m_alignZUsingY;
};

//############################################################################
//##############################  Singleton  #############################
//############################################################################

class _declspec(novtable) Singleton : public Effect
{
public:
	static void __stdcall InitializeClass(void);
	static void __stdcall TerminateClass(void);

	static ClassData* DefaultData;

	typedef Singleton__Specification Specification;

protected:
	std::vector<char> m_data;

	Singleton(ClassData* class_data, Specification* spec, uint32_t flags);

	//----------------------------------------------------------------------------
	// Class Data Support
	//
public:
	Specification* GetSpecification()
	{
		// Check_Object(this);
		return Cast_Object(Specification*, m_specification);
	}

	//----------------------------------------------------------------------------
	// API
	//
public:
	bool Execute(ExecuteInfo* info);

protected:
	Stuff::RGBAColor m_color;
	float m_radius, m_scale;

	//----------------------------------------------------------------------------
	// Testing
	//
public:
	void TestInstance(void) const;
};
} // namespace gosFX
