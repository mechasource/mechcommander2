//==========================================================================//
// File:	 gosFX_Singleton.cpp										    //
// Contents: Base gosFX::Singleton Component								//
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//
//
//############################################################################
//########################  gosFX::Singleton__Specification
//#############################
//############################################################################

#include "stdinc.h"
#include "gosfx/gosfxheaders.h"

//------------------------------------------------------------------------------
//
gosFX::Singleton__Specification::Singleton__Specification(
	Stuff::RegisteredClass::ClassID class_id, std::iostream stream, uint32_t gfx_version)
	: Effect__Specification(class_id, stream, gfx_version)
{
	// Check_Pointer(this);
	Check_Object(stream);
	// _ASSERT(gos_GetCurrentHeap() == Heap);
	//
	//-------------------
	// Load in the curves
	//-------------------
	//
	m_red.Load(stream, gfx_version);
	m_green.Load(stream, gfx_version);
	m_blue.Load(stream, gfx_version);
	m_alpha.Load(stream, gfx_version);
	m_scale.Load(stream, gfx_version);
	*stream >> m_alignZUsingX >> m_alignZUsingY;
}

//------------------------------------------------------------------------------
//
gosFX::Singleton__Specification::Singleton__Specification(Stuff::RegisteredClass::ClassID class_id)
	: Effect__Specification(class_id)
{
	// Check_Pointer(this);
	// _ASSERT(gos_GetCurrentHeap() == Heap);
}

//------------------------------------------------------------------------------
//
void gosFX::Singleton__Specification::Save(std::iostream stream)
{
	// Check_Object(this);
	Check_Object(stream);
	Effect__Specification::Save(stream);
	//
	//----------------
	// Save our curves
	//----------------
	//
	m_red.Save(stream);
	m_green.Save(stream);
	m_blue.Save(stream);
	m_alpha.Save(stream);
	m_scale.Save(stream);
	*stream << m_alignZUsingX << m_alignZUsingY;
}

//------------------------------------------------------------------------------
//
void gosFX::Singleton__Specification::BuildDefaults()
{
	// Check_Object(this);
	Effect__Specification::BuildDefaults();
	m_red.m_ageCurve.SetCurve(1.0f);
	m_red.m_seeded = false;
	m_red.m_seedCurve.SetCurve(1.0f);
	m_green.m_ageCurve.SetCurve(1.0f);
	m_green.m_seeded = false;
	m_green.m_seedCurve.SetCurve(1.0f);
	m_blue.m_ageCurve.SetCurve(1.0f);
	m_blue.m_seeded = false;
	m_blue.m_seedCurve.SetCurve(1.0f);
	m_alpha.m_ageCurve.SetCurve(1.0f);
	m_alpha.m_seeded = false;
	m_alpha.m_seedCurve.SetCurve(1.0f);
	m_scale.m_ageCurve.SetCurve(1.0f);
	m_scale.m_seeded = false;
	m_scale.m_seedCurve.SetCurve(1.0f);
}

//------------------------------------------------------------------------------
//
bool gosFX::Singleton__Specification::IsDataValid(bool fix_data)
{
	// Check_Object(this);
	float min, max;
	m_scale.ExpensiveComputeRange(&min, &max);
	if (min < 0.0f)
		if (fix_data)
		{
			m_scale.m_ageCurve.SetCurve(1.0f);
			m_scale.m_seeded = false;
			m_scale.m_seedCurve.SetCurve(1.0f);
			// PAUSE(("Warning: Curve \"scale\" in Effect \"%s\" Is Out of Range
			// and has been Reset", (std::wstring_view)m_name));
		}
		else
			return false;
	return Effect__Specification::IsDataValid(fix_data);
}
//------------------------------------------------------------------------------
//
void gosFX::Singleton__Specification::Copy(Singleton__Specification* spec)
{
	// Check_Object(this);
	Check_Object(spec);
	Effect__Specification::Copy(spec);
	//
	//----------------
	// Copy the curves
	//----------------
	//
#ifdef _GAMEOS_HPP_
	// gos_PushCurrentHeap(Heap);
#endif
	m_red = spec->m_red;
	m_green = spec->m_green;
	m_blue = spec->m_blue;
	m_alpha = spec->m_alpha;
	m_scale = spec->m_scale;
	m_alignZUsingX = spec->m_alignZUsingX;
	m_alignZUsingY = spec->m_alignZUsingY;
	// gos_PopCurrentHeap();
}

//############################################################################
//############################  gosFX::Singleton ###############################
//############################################################################

gosFX::Singleton::ClassData* gosFX::Singleton::DefaultData = nullptr;

//------------------------------------------------------------------------------
//
void gosFX::Singleton::InitializeClass()
{
	_ASSERT(!DefaultData);
	// _ASSERT(gos_GetCurrentHeap() == Heap);
	DefaultData =
		new ClassData(SingletonClassID, "gosFX::Singleton", Effect::DefaultData, nullptr, nullptr);
	Register_Object(DefaultData);
}

//------------------------------------------------------------------------------
//
void gosFX::Singleton::TerminateClass()
{
	Unregister_Object(DefaultData);
	delete DefaultData;
	DefaultData = nullptr;
}

//------------------------------------------------------------------------------
//
gosFX::Singleton::Singleton(ClassData* class_data, Specification* spec, uint32_t flags)
	: Effect(class_data, spec, flags)
{
	// Check_Pointer(this);
	Check_Object(spec);
	// _ASSERT(gos_GetCurrentHeap() == Heap);
}

//------------------------------------------------------------------------------
//
void gosFX::Singleton::TestInstance(void) const
{
	_ASSERT(IsDerivedFrom(DefaultData));
}

//------------------------------------------------------------------------------
//
bool gosFX::Singleton::Execute(ExecuteInfo* info)
{
	// Check_Object(this);
	Check_Object(info);
	if (!IsExecuted())
		return false;
	//
	//------------------------
	// Do the effect animation
	//------------------------
	//
	if (!Effect::Execute(info))
		return false;
	//
	//-----------------------------------------
	// Animate the parent then get our pointers
	//-----------------------------------------
	//
	Specification* spec = GetSpecification();
	Check_Object(spec);
	//
	//------------------
	// Animate the color
	//------------------
	//
	m_color.red = spec->m_red.ComputeValue(m_age, m_seed);
	m_color.green = spec->m_green.ComputeValue(m_age, m_seed);
	m_color.blue = spec->m_blue.ComputeValue(m_age, m_seed);
	m_color.alpha = spec->m_alpha.ComputeValue(m_age, m_seed);
	m_scale = spec->m_scale.ComputeValue(m_age, m_seed);
	//
	//------------------------------
	// Calculate the bounding volume
	//------------------------------
	//
	Stuff::OBB bounds;
	bounds.sphereRadius = m_radius * m_scale;
	if (bounds.sphereRadius < Stuff::SMALL)
		bounds.sphereRadius = 0.01f;
	bounds.localToParent = m_localToParent;
	bounds.axisExtents = Stuff::Vector3D::Identity;
	info->m_bounds->Union(*info->m_bounds, bounds);
	return true;
}
