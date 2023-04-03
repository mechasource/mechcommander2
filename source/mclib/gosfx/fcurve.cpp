//===========================================================================//
// File:	fcurve.cpp                                                       //
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#include "stdinc.h"

#include "fcurve.hpp"

//#include "gosfx/gosfxheaders.h"

//##########################################################################
//########################      Curve          #############################
//##########################################################################

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
bool gosFX::Curve::IsComplex()
{
	// Check_Object(this);

	return static_cast<uint32_t>(m_type) & (static_cast<uint32_t>(CurveType::e_ComplexLinearType) | static_cast<uint32_t>(CurveType::e_ComplexComplexType) | static_cast<uint32_t>(CurveType::e_ComplexSplineType) | static_cast<uint32_t>(CurveType::e_ConstantComplexType) | static_cast<uint32_t>(CurveType::e_ConstantLinearType) | static_cast<uint32_t>(CurveType::e_ConstantSplineType) | static_cast<uint32_t>(CurveType::e_SplineLinearType) | static_cast<uint32_t>(CurveType::e_SplineSplineType));
}

void gosFX::Curve::Save(std::ostream& stream)
{
	// Check_Object(this);
	switch (m_type)
	{
	case CurveType::e_ComplexLinearType:
	{
		SeededCurveOf<ComplexCurve, LinearCurve, Curve::CurveType::e_ComplexLinearType>* SCurve =
			(SeededCurveOf<ComplexCurve, LinearCurve, Curve::CurveType::e_ComplexLinearType>*)this;
		SCurve->Save(stream);
	}
	break;
	case CurveType::e_ComplexComplexType:
	{
		SeededCurveOf<ComplexCurve, ComplexCurve, CurveType::e_ComplexComplexType>* SCurve =
			(SeededCurveOf<ComplexCurve, ComplexCurve, CurveType::e_ComplexComplexType>*)this;
		SCurve->Save(stream);
	}
	break;
	case CurveType::e_ComplexSplineType:
	{
		SeededCurveOf<ComplexCurve, SplineCurve, CurveType::e_ComplexSplineType>* SCurve =
			(SeededCurveOf<ComplexCurve, SplineCurve, CurveType::e_ComplexSplineType>*)this;
		SCurve->Save(stream);
	}
	break;
	case CurveType::e_ConstantComplexType:
	{
		SeededCurveOf<ConstantCurve, ComplexCurve, CurveType::e_ConstantComplexType>* SCurve =
			(SeededCurveOf<ConstantCurve, ComplexCurve, CurveType::e_ConstantComplexType>*)this;
		SCurve->Save(stream);
	}
	break;
	case CurveType::e_ConstantLinearType:
	{
		SeededCurveOf<ConstantCurve, LinearCurve, CurveType::e_ConstantLinearType>* SCurve =
			(SeededCurveOf<ConstantCurve, LinearCurve, CurveType::e_ConstantLinearType>*)this;
		SCurve->Save(stream);
	}
	break;
	case CurveType::e_ConstantSplineType:
	{
		SeededCurveOf<ConstantCurve, SplineCurve, CurveType::e_ConstantSplineType>* SCurve =
			(SeededCurveOf<ConstantCurve, SplineCurve, CurveType::e_ConstantSplineType>*)this;
		SCurve->Save(stream);
	}
	break;
	case CurveType::e_SplineLinearType:
	{
		SeededCurveOf<SplineCurve, LinearCurve, CurveType::e_SplineLinearType>* SCurve =
			(SeededCurveOf<SplineCurve, LinearCurve, CurveType::e_SplineLinearType>*)this;
		SCurve->Save(stream);
	}
	break;
	case CurveType::e_SplineSplineType:
	{
		SeededCurveOf<SplineCurve, SplineCurve, CurveType::e_SplineSplineType>* SCurve =
			(SeededCurveOf<SplineCurve, SplineCurve, CurveType::e_SplineSplineType>*)this;
		SCurve->Save(stream);
	}
	break;
	default:
		break;
	}
}

void gosFX::Curve::Load(std::iostream stream, uint32_t gfx_version)
{
	// Check_Object(this);
	switch (m_type)
	{
	case CurveType::e_ComplexLinearType:
	{
		SeededCurveOf<ComplexCurve, LinearCurve, Curve::CurveType::e_ComplexLinearType>* SCurve =
			(SeededCurveOf<ComplexCurve, LinearCurve, Curve::CurveType::e_ComplexLinearType>*)this;
		SCurve->Load(stream, gfx_version);
	}
	break;
	case CurveType::e_ComplexComplexType:
	{
		SeededCurveOf<ComplexCurve, ComplexCurve, CurveType::e_ComplexComplexType>* SCurve =
			(SeededCurveOf<ComplexCurve, ComplexCurve, CurveType::e_ComplexComplexType>*)this;
		SCurve->Load(stream, gfx_version);
	}
	break;
	case CurveType::e_ComplexSplineType:
	{
		SeededCurveOf<ComplexCurve, SplineCurve, CurveType::e_ComplexSplineType>* SCurve =
			(SeededCurveOf<ComplexCurve, SplineCurve, CurveType::e_ComplexSplineType>*)this;
		SCurve->Load(stream, gfx_version);
	}
	break;
	case CurveType::e_ConstantComplexType:
	{
		SeededCurveOf<ConstantCurve, ComplexCurve, CurveType::e_ConstantComplexType>* SCurve =
			(SeededCurveOf<ConstantCurve, ComplexCurve, CurveType::e_ConstantComplexType>*)this;
		SCurve->Load(stream, gfx_version);
	}
	break;
	case CurveType::e_ConstantLinearType:
	{
		SeededCurveOf<ConstantCurve, LinearCurve, CurveType::e_ConstantLinearType>* SCurve =
			(SeededCurveOf<ConstantCurve, LinearCurve, CurveType::e_ConstantLinearType>*)this;
		SCurve->Load(stream, gfx_version);
	}
	break;
	case CurveType::e_ConstantSplineType:
	{
		SeededCurveOf<ConstantCurve, SplineCurve, CurveType::e_ConstantSplineType>* SCurve =
			(SeededCurveOf<ConstantCurve, SplineCurve, CurveType::e_ConstantSplineType>*)this;
		SCurve->Load(stream, gfx_version);
	}
	break;
	case CurveType::e_SplineLinearType:
	{
		SeededCurveOf<SplineCurve, LinearCurve, CurveType::e_SplineLinearType>* SCurve =
			(SeededCurveOf<SplineCurve, LinearCurve, CurveType::e_SplineLinearType>*)this;
		SCurve->Load(stream, gfx_version);
	}
	break;
	case CurveType::e_SplineSplineType:
	{
		SeededCurveOf<SplineCurve, SplineCurve, CurveType::e_SplineSplineType>* SCurve =
			(SeededCurveOf<SplineCurve, SplineCurve, CurveType::e_SplineSplineType>*)this;
		SCurve->Load(stream, gfx_version);
	}
	break;
	default:
		break;
	}
}

void gosFX::Curve::SetSeedFlagIfComplex(bool vflag)
{
	// Check_Object(this);
	switch (m_type)
	{
	case CurveType::e_ComplexLinearType:
	{
		SeededCurveOf<ComplexCurve, LinearCurve, Curve::CurveType::e_ComplexLinearType>* SCurve =
			(SeededCurveOf<ComplexCurve, LinearCurve, Curve::CurveType::e_ComplexLinearType>*)this;
		SCurve->m_seeded = vflag;
	}
	break;
	case CurveType::e_ComplexComplexType:
	{
		SeededCurveOf<ComplexCurve, ComplexCurve, CurveType::e_ComplexComplexType>* SCurve =
			(SeededCurveOf<ComplexCurve, ComplexCurve, CurveType::e_ComplexComplexType>*)this;
		SCurve->m_seeded = vflag;
	}
	break;
	case CurveType::e_ComplexSplineType:
	{
		SeededCurveOf<ComplexCurve, SplineCurve, CurveType::e_ComplexSplineType>* SCurve =
			(SeededCurveOf<ComplexCurve, SplineCurve, CurveType::e_ComplexSplineType>*)this;
		SCurve->m_seeded = vflag;
	}
	break;
	case CurveType::e_ConstantComplexType:
	{
		SeededCurveOf<ConstantCurve, ComplexCurve, CurveType::e_ConstantComplexType>* SCurve =
			(SeededCurveOf<ConstantCurve, ComplexCurve, CurveType::e_ConstantComplexType>*)this;
		SCurve->m_seeded = vflag;
	}
	break;
	case CurveType::e_ConstantLinearType:
	{
		SeededCurveOf<ConstantCurve, LinearCurve, CurveType::e_ConstantLinearType>* SCurve =
			(SeededCurveOf<ConstantCurve, LinearCurve, CurveType::e_ConstantLinearType>*)this;
		SCurve->m_seeded = vflag;
	}
	break;
	case CurveType::e_ConstantSplineType:
	{
		SeededCurveOf<ConstantCurve, SplineCurve, CurveType::e_ConstantSplineType>* SCurve =
			(SeededCurveOf<ConstantCurve, SplineCurve, CurveType::e_ConstantSplineType>*)this;
		SCurve->m_seeded = vflag;
	}
	break;
	case CurveType::e_SplineLinearType:
	{
		SeededCurveOf<SplineCurve, LinearCurve, CurveType::e_SplineLinearType>* SCurve =
			(SeededCurveOf<SplineCurve, LinearCurve, CurveType::e_SplineLinearType>*)this;
		SCurve->m_seeded = vflag;
	}
	break;
	case CurveType::e_SplineSplineType:
	{
		SeededCurveOf<SplineCurve, SplineCurve, CurveType::e_SplineSplineType>* SCurve =
			(SeededCurveOf<SplineCurve, SplineCurve, CurveType::e_SplineSplineType>*)this;
		SCurve->m_seeded = vflag;
	}
	break;
	default:
		break;
	}
}

int32_t
gosFX::Curve::GetSeedFlagIfComplex()
{
	// Check_Object(this);
	switch (m_type)
	{
	case CurveType::e_ComplexLinearType:
	{
		SeededCurveOf<ComplexCurve, LinearCurve, Curve::CurveType::e_ComplexLinearType>* SCurve =
			(SeededCurveOf<ComplexCurve, LinearCurve, Curve::CurveType::e_ComplexLinearType>*)this;
		return SCurve->m_seeded ? 1 : 0;
	}
	break;
	case CurveType::e_ComplexComplexType:
	{
		SeededCurveOf<ComplexCurve, ComplexCurve, CurveType::e_ComplexComplexType>* SCurve =
			(SeededCurveOf<ComplexCurve, ComplexCurve, CurveType::e_ComplexComplexType>*)this;
		return SCurve->m_seeded ? 1 : 0;
	}
	break;
	case CurveType::e_ComplexSplineType:
	{
		SeededCurveOf<ComplexCurve, SplineCurve, CurveType::e_ComplexSplineType>* SCurve =
			(SeededCurveOf<ComplexCurve, SplineCurve, CurveType::e_ComplexSplineType>*)this;
		return SCurve->m_seeded ? 1 : 0;
	}
	break;
	case CurveType::e_ConstantComplexType:
	{
		SeededCurveOf<ConstantCurve, ComplexCurve, CurveType::e_ConstantComplexType>* SCurve =
			(SeededCurveOf<ConstantCurve, ComplexCurve, CurveType::e_ConstantComplexType>*)this;
		return SCurve->m_seeded ? 1 : 0;
	}
	break;
	case CurveType::e_ConstantLinearType:
	{
		SeededCurveOf<ConstantCurve, LinearCurve, CurveType::e_ConstantLinearType>* SCurve =
			(SeededCurveOf<ConstantCurve, LinearCurve, CurveType::e_ConstantLinearType>*)this;
		return SCurve->m_seeded ? 1 : 0;
	}
	break;
	case CurveType::e_ConstantSplineType:
	{
		SeededCurveOf<ConstantCurve, SplineCurve, CurveType::e_ConstantSplineType>* SCurve =
			(SeededCurveOf<ConstantCurve, SplineCurve, CurveType::e_ConstantSplineType>*)this;
		return SCurve->m_seeded ? 1 : 0;
	}
	break;
	case CurveType::e_SplineLinearType:
	{
		SeededCurveOf<SplineCurve, LinearCurve, CurveType::e_SplineLinearType>* SCurve =
			(SeededCurveOf<SplineCurve, LinearCurve, CurveType::e_SplineLinearType>*)this;
		return SCurve->m_seeded ? 1 : 0;
	}
	break;
	case CurveType::e_SplineSplineType:
	{
		SeededCurveOf<SplineCurve, SplineCurve, CurveType::e_SplineSplineType>* SCurve =
			(SeededCurveOf<SplineCurve, SplineCurve, CurveType::e_SplineSplineType>*)this;
		return SCurve->m_seeded ? 1 : 0;
	}
	break;
	default:
		break;
	}
	return -1;
}

float gosFX::Curve::ExpensiveCompute(float tme, int32_t curvenum)
{
	// Check_Object(this);
	switch (m_type)
	{
	case CurveType::e_ConstantType:
	{
		ConstantCurve* SCurve = (ConstantCurve*)this;
		return SCurve->ComputeValue(tme, 0.0f);
	}
	break;
	case CurveType::e_LinearType:
	{
		LinearCurve* SCurve = (LinearCurve*)this;
		return SCurve->ComputeValue(tme, 0.0f);
	}
	break;
	case CurveType::e_SplineType:
	{
		SplineCurve* SCurve = (SplineCurve*)this;
		return SCurve->ComputeValue(tme, 0.0f);
	}
	break;
	case CurveType::e_ComplexType:
	{
		ComplexCurve* SCurve = (ComplexCurve*)this;
		return SCurve->ComputeValue(tme, 0.0f);
	}
	break;
	case CurveType::e_ComplexLinearType:
	{
		SeededCurveOf<ComplexCurve, LinearCurve, Curve::CurveType::e_ComplexLinearType>* SCurve =
			(SeededCurveOf<ComplexCurve, LinearCurve, Curve::CurveType::e_ComplexLinearType>*)this;
		if (curvenum == 0)
			return SCurve->m_ageCurve.ExpensiveCompute(tme, curvenum);
		else
			return SCurve->m_seedCurve.ExpensiveCompute(tme, curvenum);
	}
	break;
	case CurveType::e_ComplexComplexType:
	{
		SeededCurveOf<ComplexCurve, ComplexCurve, CurveType::e_ComplexComplexType>* SCurve =
			(SeededCurveOf<ComplexCurve, ComplexCurve, CurveType::e_ComplexComplexType>*)this;
		if (curvenum == 0)
			return SCurve->m_ageCurve.ExpensiveCompute(tme, curvenum);
		else
			return SCurve->m_seedCurve.ExpensiveCompute(tme, curvenum);
	}
	break;
	case CurveType::e_ComplexSplineType:
	{
		SeededCurveOf<ComplexCurve, SplineCurve, CurveType::e_ComplexSplineType>* SCurve =
			(SeededCurveOf<ComplexCurve, SplineCurve, CurveType::e_ComplexSplineType>*)this;
		if (curvenum == 0)
			return SCurve->m_ageCurve.ExpensiveCompute(tme, curvenum);
		else
			return SCurve->m_seedCurve.ExpensiveCompute(tme, curvenum);
	}
	break;
	case CurveType::e_ConstantComplexType:
	{
		SeededCurveOf<ConstantCurve, ComplexCurve, CurveType::e_ConstantComplexType>* SCurve =
			(SeededCurveOf<ConstantCurve, ComplexCurve, CurveType::e_ConstantComplexType>*)this;
		if (curvenum == 0)
			return SCurve->m_ageCurve.ExpensiveCompute(tme, curvenum);
		else
			return SCurve->m_seedCurve.ExpensiveCompute(tme, curvenum);
	}
	break;
	case CurveType::e_ConstantLinearType:
	{
		SeededCurveOf<ConstantCurve, LinearCurve, CurveType::e_ConstantLinearType>* SCurve =
			(SeededCurveOf<ConstantCurve, LinearCurve, CurveType::e_ConstantLinearType>*)this;
		if (curvenum == 0)
			return SCurve->m_ageCurve.ExpensiveCompute(tme, curvenum);
		else
			return SCurve->m_seedCurve.ExpensiveCompute(tme, curvenum);
	}
	break;
	case CurveType::e_ConstantSplineType:
	{
		SeededCurveOf<ConstantCurve, SplineCurve, CurveType::e_ConstantSplineType>* SCurve =
			(SeededCurveOf<ConstantCurve, SplineCurve, CurveType::e_ConstantSplineType>*)this;
		if (curvenum == 0)
			return SCurve->m_ageCurve.ExpensiveCompute(tme, curvenum);
		else
			return SCurve->m_seedCurve.ExpensiveCompute(tme, curvenum);
	}
	break;
	case CurveType::e_SplineLinearType:
	{
		SeededCurveOf<SplineCurve, LinearCurve, CurveType::e_SplineLinearType>* SCurve =
			(SeededCurveOf<SplineCurve, LinearCurve, CurveType::e_SplineLinearType>*)this;
		if (curvenum == 0)
			return SCurve->m_ageCurve.ExpensiveCompute(tme, curvenum);
		else
			return SCurve->m_seedCurve.ExpensiveCompute(tme, curvenum);
	}
	break;
	case CurveType::e_SplineSplineType:
	{
		SeededCurveOf<SplineCurve, SplineCurve, CurveType::e_SplineSplineType>* SCurve =
			(SeededCurveOf<SplineCurve, SplineCurve, CurveType::e_SplineSplineType>*)this;
		if (curvenum == 0)
			return SCurve->m_ageCurve.ExpensiveCompute(tme, curvenum);
		else
			return SCurve->m_seedCurve.ExpensiveCompute(tme, curvenum);
	}
	break;
	default:
		break;
	}
	return 0.0f;
}

void gosFX::Curve::ExpensiveComputeRange(float* low, float* hi, int32_t curvenum)
{
	// Check_Object(this);
	Check_Pointer(low);
	Check_Pointer(hi);
	switch (m_type)
	{
	case CurveType::e_ConstantType:
	{
		ConstantCurve* SCurve = (ConstantCurve*)this;
		SCurve->ComputeRange(low, hi);
	}
	break;
	case CurveType::e_LinearType:
	{
		LinearCurve* SCurve = (LinearCurve*)this;
		SCurve->ComputeRange(low, hi);
	}
	break;
	case CurveType::e_SplineType:
	{
		SplineCurve* SCurve = (SplineCurve*)this;
		SCurve->ComputeRange(low, hi);
	}
	break;
	case CurveType::e_ComplexType:
	{
		ComplexCurve* SCurve = (ComplexCurve*)this;
		SCurve->ComputeRange(low, hi);
	}
	break;
	case CurveType::e_ComplexLinearType:
	{
		SeededCurveOf<ComplexCurve, LinearCurve, Curve::CurveType::e_ComplexLinearType>* SCurve =
			(SeededCurveOf<ComplexCurve, LinearCurve, Curve::CurveType::e_ComplexLinearType>*)this;
		if (curvenum == 0)
			SCurve->m_ageCurve.ComputeRange(low, hi);
		else
			SCurve->m_seedCurve.ComputeRange(low, hi);
	}
	break;
	case CurveType::e_ComplexComplexType:
	{
		SeededCurveOf<ComplexCurve, ComplexCurve, CurveType::e_ComplexComplexType>* SCurve =
			(SeededCurveOf<ComplexCurve, ComplexCurve, CurveType::e_ComplexComplexType>*)this;
		if (curvenum == 0)
			SCurve->m_ageCurve.ComputeRange(low, hi);
		else
			SCurve->m_seedCurve.ComputeRange(low, hi);
	}
	break;
	case CurveType::e_ComplexSplineType:
	{
		SeededCurveOf<ComplexCurve, SplineCurve, CurveType::e_ComplexSplineType>* SCurve =
			(SeededCurveOf<ComplexCurve, SplineCurve, CurveType::e_ComplexSplineType>*)this;
		if (curvenum == 0)
			SCurve->m_ageCurve.ComputeRange(low, hi);
		else
			SCurve->m_seedCurve.ComputeRange(low, hi);
	}
	break;
	case CurveType::e_ConstantComplexType:
	{
		SeededCurveOf<ConstantCurve, ComplexCurve, CurveType::e_ConstantComplexType>* SCurve =
			(SeededCurveOf<ConstantCurve, ComplexCurve, CurveType::e_ConstantComplexType>*)this;
		if (curvenum == 0)
			SCurve->m_ageCurve.ComputeRange(low, hi);
		else
			SCurve->m_seedCurve.ComputeRange(low, hi);
	}
	break;
	case CurveType::e_ConstantLinearType:
	{
		SeededCurveOf<ConstantCurve, LinearCurve, CurveType::e_ConstantLinearType>* SCurve =
			(SeededCurveOf<ConstantCurve, LinearCurve, CurveType::e_ConstantLinearType>*)this;
		if (curvenum == 0)
			SCurve->m_ageCurve.ComputeRange(low, hi);
		else
			SCurve->m_seedCurve.ComputeRange(low, hi);
	}
	break;
	case CurveType::e_ConstantSplineType:
	{
		SeededCurveOf<ConstantCurve, SplineCurve, CurveType::e_ConstantSplineType>* SCurve =
			(SeededCurveOf<ConstantCurve, SplineCurve, CurveType::e_ConstantSplineType>*)this;
		if (curvenum == 0)
			SCurve->m_ageCurve.ComputeRange(low, hi);
		else
			SCurve->m_seedCurve.ComputeRange(low, hi);
	}
	break;
	case CurveType::e_SplineLinearType:
	{
		SeededCurveOf<SplineCurve, LinearCurve, CurveType::e_SplineLinearType>* SCurve =
			(SeededCurveOf<SplineCurve, LinearCurve, CurveType::e_SplineLinearType>*)this;
		if (curvenum == 0)
			SCurve->m_ageCurve.ComputeRange(low, hi);
		else
			SCurve->m_seedCurve.ComputeRange(low, hi);
	}
	break;
	case CurveType::e_SplineSplineType:
	{
		SeededCurveOf<SplineCurve, SplineCurve, CurveType::e_SplineSplineType>* SCurve =
			(SeededCurveOf<SplineCurve, SplineCurve, CurveType::e_SplineSplineType>*)this;
		if (curvenum == 0)
			SCurve->m_ageCurve.ComputeRange(low, hi);
		else
			SCurve->m_seedCurve.ComputeRange(low, hi);
	}
	break;
	default:
		break;
	}
}

void gosFX::Curve::ExpensiveComputeRange(float* low, float* hi)
{
	// Check_Object(this);
	Check_Pointer(low);
	Check_Pointer(hi);
	switch (m_type)
	{
	case CurveType::e_ConstantType:
	{
		ConstantCurve* SCurve = (ConstantCurve*)this;
		SCurve->ComputeRange(low, hi);
	}
	break;
	case CurveType::e_LinearType:
	{
		LinearCurve* SCurve = (LinearCurve*)this;
		SCurve->ComputeRange(low, hi);
	}
	break;
	case CurveType::e_SplineType:
	{
		SplineCurve* SCurve = (SplineCurve*)this;
		SCurve->ComputeRange(low, hi);
	}
	break;
	case CurveType::e_ComplexType:
	{
		ComplexCurve* SCurve = (ComplexCurve*)this;
		SCurve->ComputeRange(low, hi);
	}
	break;
	case CurveType::e_ComplexLinearType:
	{
		SeededCurveOf<ComplexCurve, LinearCurve, Curve::CurveType::e_ComplexLinearType>* SCurve =
			(SeededCurveOf<ComplexCurve, LinearCurve, Curve::CurveType::e_ComplexLinearType>*)this;
		SCurve->ComputeRange(low, hi);
	}
	break;
	case CurveType::e_ComplexComplexType:
	{
		SeededCurveOf<ComplexCurve, ComplexCurve, CurveType::e_ComplexComplexType>* SCurve =
			(SeededCurveOf<ComplexCurve, ComplexCurve, CurveType::e_ComplexComplexType>*)this;
		SCurve->ComputeRange(low, hi);
	}
	break;
	case CurveType::e_ComplexSplineType:
	{
		SeededCurveOf<ComplexCurve, SplineCurve, CurveType::e_ComplexSplineType>* SCurve =
			(SeededCurveOf<ComplexCurve, SplineCurve, CurveType::e_ComplexSplineType>*)this;
		SCurve->ComputeRange(low, hi);
	}
	break;
	case CurveType::e_ConstantComplexType:
	{
		SeededCurveOf<ConstantCurve, ComplexCurve, CurveType::e_ConstantComplexType>* SCurve =
			(SeededCurveOf<ConstantCurve, ComplexCurve, CurveType::e_ConstantComplexType>*)this;
		SCurve->ComputeRange(low, hi);
	}
	break;
	case CurveType::e_ConstantLinearType:
	{
		SeededCurveOf<ConstantCurve, LinearCurve, CurveType::e_ConstantLinearType>* SCurve =
			(SeededCurveOf<ConstantCurve, LinearCurve, CurveType::e_ConstantLinearType>*)this;
		SCurve->ComputeRange(low, hi);
	}
	break;
	case CurveType::e_ConstantSplineType:
	{
		SeededCurveOf<ConstantCurve, SplineCurve, CurveType::e_ConstantSplineType>* SCurve =
			(SeededCurveOf<ConstantCurve, SplineCurve, CurveType::e_ConstantSplineType>*)this;
		SCurve->ComputeRange(low, hi);
	}
	break;
	case CurveType::e_SplineLinearType:
	{
		SeededCurveOf<SplineCurve, LinearCurve, CurveType::e_SplineLinearType>* SCurve =
			(SeededCurveOf<SplineCurve, LinearCurve, CurveType::e_SplineLinearType>*)this;
		SCurve->ComputeRange(low, hi);
	}
	break;
	case CurveType::e_SplineSplineType:
	{
		SeededCurveOf<SplineCurve, SplineCurve, CurveType::e_SplineSplineType>* SCurve =
			(SeededCurveOf<SplineCurve, SplineCurve, CurveType::e_SplineSplineType>*)this;
		SCurve->ComputeRange(low, hi);
	}
	break;
	default:
		break;
	}
}

float gosFX::Curve::Mid(int32_t curvenum)
{
	float min, max;
	ExpensiveComputeRange(&min, &max, curvenum);
	return min + (max - min) * 0.5f;
}

void gosFX::Curve::TranslateTo(float pos, int32_t curvenum)
{
	// Check_Object(this);
	switch (m_type)
	{
	case CurveType::e_ConstantType:
	{
		ConstantCurve* SCurve = (ConstantCurve*)this;
		SCurve->SetCurve(pos);
	}
	break;
	case CurveType::e_LinearType:
	{
		LinearCurve* SCurve = (LinearCurve*)this;
		float s;
		s = SCurve->ComputeSlope(0.0f);
		SCurve->SetCurve(pos, pos + s);
	}
	break;
	case CurveType::e_SplineType:
	{
		SplineCurve* SCurve = (SplineCurve*)this;
		float v0, v1, s0, s1;
		v0 = SCurve->ComputeValue(0.0f, 0.0f);
		v1 = SCurve->ComputeValue(1.0f, 0.0f);
		s0 = SCurve->ComputeSlope(0.0f);
		s1 = SCurve->ComputeSlope(1.0f);
		v1 += pos - v0;
		v0 = pos;
		SCurve->SetCurve(v0, s0, v1, s1);
	}
	break;
	case CurveType::e_ComplexType:
	{
		ComplexCurve* SCurve = (ComplexCurve*)this;
		float delta, x1, x2, y1, y2, slp;
		delta = pos - SCurve->ComputeValue(0.0f, 0.0f);
		int32_t key;
		for (key = 0; key < SCurve->GetKeyCount() - 1; key++)
		{
			x1 = (*SCurve)[key].m_time;
			x2 = (*SCurve)[key + 1].m_time;
			y1 = SCurve->ComputeValue(x1, 0.0f);
			y2 = SCurve->ComputeValue(x2, 0.0f);
			y1 += delta;
			y2 += delta;
			(*SCurve)[key].SetLinearKey(x1, y1, y2, x2 - x1);
		}
		x1 = (*SCurve)[key].m_time;
		y1 = SCurve->ComputeValue(x1, 0.0f);
		y1 += delta;
		slp = (*SCurve)[key].m_slope;
		(*SCurve)[SCurve->GetKeyCount() - 1].SetLinearKey(x1, y1, y1 + slp * 0.1f, 0.1f);
	}
	break;
	case CurveType::e_ComplexLinearType:
	{
		SeededCurveOf<ComplexCurve, LinearCurve, Curve::CurveType::e_ComplexLinearType>* SCurve =
			(SeededCurveOf<ComplexCurve, LinearCurve, Curve::CurveType::e_ComplexLinearType>*)this;
		if (curvenum == 0)
			SCurve->m_ageCurve.TranslateTo(pos, curvenum);
		else
			SCurve->m_seedCurve.TranslateTo(pos, curvenum);
	}
	break;
	case CurveType::e_ComplexComplexType:
	{
		SeededCurveOf<ComplexCurve, ComplexCurve, CurveType::e_ComplexComplexType>* SCurve =
			(SeededCurveOf<ComplexCurve, ComplexCurve, CurveType::e_ComplexComplexType>*)this;
		if (curvenum == 0)
			SCurve->m_ageCurve.TranslateTo(pos, curvenum);
		else
			SCurve->m_seedCurve.TranslateTo(pos, curvenum);
	}
	break;
	case CurveType::e_ComplexSplineType:
	{
		SeededCurveOf<ComplexCurve, SplineCurve, CurveType::e_ComplexSplineType>* SCurve =
			(SeededCurveOf<ComplexCurve, SplineCurve, CurveType::e_ComplexSplineType>*)this;
		if (curvenum == 0)
			SCurve->m_ageCurve.TranslateTo(pos, curvenum);
		else
			SCurve->m_seedCurve.TranslateTo(pos, curvenum);
	}
	break;
	case CurveType::e_ConstantComplexType:
	{
		SeededCurveOf<ConstantCurve, ComplexCurve, CurveType::e_ConstantComplexType>* SCurve =
			(SeededCurveOf<ConstantCurve, ComplexCurve, CurveType::e_ConstantComplexType>*)this;
		if (curvenum == 0)
			SCurve->m_ageCurve.TranslateTo(pos, curvenum);
		else
			SCurve->m_seedCurve.TranslateTo(pos, curvenum);
	}
	break;
	case CurveType::e_ConstantLinearType:
	{
		SeededCurveOf<ConstantCurve, LinearCurve, CurveType::e_ConstantLinearType>* SCurve =
			(SeededCurveOf<ConstantCurve, LinearCurve, CurveType::e_ConstantLinearType>*)this;
		if (curvenum == 0)
			SCurve->m_ageCurve.TranslateTo(pos, curvenum);
		else
			SCurve->m_seedCurve.TranslateTo(pos, curvenum);
	}
	break;
	case CurveType::e_ConstantSplineType:
	{
		SeededCurveOf<ConstantCurve, SplineCurve, CurveType::e_ConstantSplineType>* SCurve =
			(SeededCurveOf<ConstantCurve, SplineCurve, CurveType::e_ConstantSplineType>*)this;
		if (curvenum == 0)
			SCurve->m_ageCurve.TranslateTo(pos, curvenum);
		else
			SCurve->m_seedCurve.TranslateTo(pos, curvenum);
	}
	break;
	case CurveType::e_SplineLinearType:
	{
		SeededCurveOf<SplineCurve, LinearCurve, CurveType::e_SplineLinearType>* SCurve =
			(SeededCurveOf<SplineCurve, LinearCurve, CurveType::e_SplineLinearType>*)this;
		if (curvenum == 0)
			SCurve->m_ageCurve.TranslateTo(pos, curvenum);
		else
			SCurve->m_seedCurve.TranslateTo(pos, curvenum);
	}
	break;
	case CurveType::e_SplineSplineType:
	{
		SeededCurveOf<SplineCurve, SplineCurve, CurveType::e_SplineSplineType>* SCurve =
			(SeededCurveOf<SplineCurve, SplineCurve, CurveType::e_SplineSplineType>*)this;
		if (curvenum == 0)
			SCurve->m_ageCurve.TranslateTo(pos, curvenum);
		else
			SCurve->m_seedCurve.TranslateTo(pos, curvenum);
	}
	break;
	default:
		break;
	}
}

void gosFX::Curve::TranslateBy(float delta, int32_t curvenum)
{
	// Check_Object(this);
	switch (m_type)
	{
	case CurveType::e_ConstantType:
	{
		ConstantCurve* SCurve = (ConstantCurve*)this;
		SCurve->SetCurve(SCurve->ComputeValue(0.0f, 0.0f) + delta);
	}
	break;
	case CurveType::e_LinearType:
	{
		LinearCurve* SCurve = (LinearCurve*)this;
		float s, v;
		s = SCurve->ComputeSlope(0.0f);
		v = SCurve->ComputeValue(0.0f, 0.0f);
		SCurve->SetCurve(v + delta, v + delta + s);
	}
	break;
	case CurveType::e_SplineType:
	{
		SplineCurve* SCurve = (SplineCurve*)this;
		float v0, v1, s0, s1;
		v0 = SCurve->ComputeValue(0.0f, 0.0f);
		v1 = SCurve->ComputeValue(1.0f, 0.0f);
		s0 = SCurve->ComputeSlope(0.0f);
		s1 = SCurve->ComputeSlope(1.0f);
		v1 += delta;
		v0 += delta;
		SCurve->SetCurve(v0, s0, v1, s1);
	}
	break;
	case CurveType::e_ComplexType:
	{
		ComplexCurve* SCurve = (ComplexCurve*)this;
		float x1, x2, y1, y2;
		int32_t key;
		for (key = 0; key < SCurve->GetKeyCount() - 1; key++)
		{
			x1 = (*SCurve)[key].m_time;
			x2 = (*SCurve)[key + 1].m_time;
			y1 = SCurve->ComputeValue(x1, 0.0f);
			y2 = SCurve->ComputeValue(x2, 0.0f);
			y1 += delta;
			y2 += delta;
			(*SCurve)[key].SetLinearKey(x1, y1, y2, x2 - x1);
		}
		x1 = (*SCurve)[key].m_time;
		y1 = SCurve->ComputeValue(x1, 0.0f);
		y1 += delta;
		(*SCurve)[SCurve->GetKeyCount() - 1].SetLinearKey(x1, y1, y1, 1.0f);
	}
	break;
	case CurveType::e_ComplexLinearType:
	{
		SeededCurveOf<ComplexCurve, LinearCurve, Curve::CurveType::e_ComplexLinearType>* SCurve =
			(SeededCurveOf<ComplexCurve, LinearCurve, Curve::CurveType::e_ComplexLinearType>*)this;
		if (curvenum == 0)
			SCurve->m_ageCurve.TranslateBy(delta, curvenum);
		else
			SCurve->m_seedCurve.TranslateBy(delta, curvenum);
	}
	break;
	case CurveType::e_ComplexComplexType:
	{
		SeededCurveOf<ComplexCurve, ComplexCurve, CurveType::e_ComplexComplexType>* SCurve =
			(SeededCurveOf<ComplexCurve, ComplexCurve, CurveType::e_ComplexComplexType>*)this;
		if (curvenum == 0)
			SCurve->m_ageCurve.TranslateBy(delta, curvenum);
		else
			SCurve->m_seedCurve.TranslateBy(delta, curvenum);
	}
	break;
	case CurveType::e_ComplexSplineType:
	{
		SeededCurveOf<ComplexCurve, SplineCurve, CurveType::e_ComplexSplineType>* SCurve =
			(SeededCurveOf<ComplexCurve, SplineCurve, CurveType::e_ComplexSplineType>*)this;
		if (curvenum == 0)
			SCurve->m_ageCurve.TranslateBy(delta, curvenum);
		else
			SCurve->m_seedCurve.TranslateBy(delta, curvenum);
	}
	break;
	case CurveType::e_ConstantComplexType:
	{
		SeededCurveOf<ConstantCurve, ComplexCurve, CurveType::e_ConstantComplexType>* SCurve =
			(SeededCurveOf<ConstantCurve, ComplexCurve, CurveType::e_ConstantComplexType>*)this;
		if (curvenum == 0)
			SCurve->m_ageCurve.TranslateBy(delta, curvenum);
		else
			SCurve->m_seedCurve.TranslateBy(delta, curvenum);
	}
	break;
	case CurveType::e_ConstantLinearType:
	{
		SeededCurveOf<ConstantCurve, LinearCurve, CurveType::e_ConstantLinearType>* SCurve =
			(SeededCurveOf<ConstantCurve, LinearCurve, CurveType::e_ConstantLinearType>*)this;
		if (curvenum == 0)
			SCurve->m_ageCurve.TranslateBy(delta, curvenum);
		else
			SCurve->m_seedCurve.TranslateBy(delta, curvenum);
	}
	break;
	case CurveType::e_ConstantSplineType:
	{
		SeededCurveOf<ConstantCurve, SplineCurve, CurveType::e_ConstantSplineType>* SCurve =
			(SeededCurveOf<ConstantCurve, SplineCurve, CurveType::e_ConstantSplineType>*)this;
		if (curvenum == 0)
			SCurve->m_ageCurve.TranslateBy(delta, curvenum);
		else
			SCurve->m_seedCurve.TranslateBy(delta, curvenum);
	}
	break;
	case CurveType::e_SplineLinearType:
	{
		SeededCurveOf<SplineCurve, LinearCurve, CurveType::e_SplineLinearType>* SCurve =
			(SeededCurveOf<SplineCurve, LinearCurve, CurveType::e_SplineLinearType>*)this;
		if (curvenum == 0)
			SCurve->m_ageCurve.TranslateBy(delta, curvenum);
		else
			SCurve->m_seedCurve.TranslateBy(delta, curvenum);
	}
	break;
	case CurveType::e_SplineSplineType:
	{
		SeededCurveOf<SplineCurve, SplineCurve, CurveType::e_SplineSplineType>* SCurve =
			(SeededCurveOf<SplineCurve, SplineCurve, CurveType::e_SplineSplineType>*)this;
		if (curvenum == 0)
			SCurve->m_ageCurve.TranslateBy(delta, curvenum);
		else
			SCurve->m_seedCurve.TranslateBy(delta, curvenum);
	}
	break;
	default:
		break;
	}
}

void gosFX::Curve::LocalScale(float sfactor, int32_t curvenum)
{
	// Check_Object(this);
	AxisScale(sfactor, Mid(curvenum), curvenum);
}

void gosFX::Curve::AxisScale(float sfactor, float axis, int32_t curvenum)
{
	// Check_Object(this);
	switch (m_type)
	{
	case CurveType::e_ConstantType:
	{
		ConstantCurve* SCurve = (ConstantCurve*)this;
		SCurve->SetCurve(axis + (SCurve->ComputeValue(0.0f, 0.0f) - axis) * sfactor);
	}
	break;
	case CurveType::e_LinearType:
	{
		LinearCurve* SCurve = (LinearCurve*)this;
		float v0, v1;
		v0 = SCurve->ComputeValue(0.0f, 0.0f);
		v1 = SCurve->ComputeValue(1.0f, 0.0f);
		v0 = axis + (v0 - axis) * sfactor;
		v1 = axis + (v1 - axis) * sfactor;
		SCurve->SetCurve(v0, v1);
	}
	break;
	case CurveType::e_SplineType:
	{
		SplineCurve* SCurve = (SplineCurve*)this;
		float v0, v1, s0, s1;
		v0 = SCurve->ComputeValue(0.0f, 0.0f);
		v1 = SCurve->ComputeValue(1.0f, 0.0f);
		s0 = SCurve->ComputeSlope(0.0f);
		s1 = SCurve->ComputeSlope(1.0f);
		v0 = axis + (v0 - axis) * sfactor;
		v1 = axis + (v1 - axis) * sfactor;
		s0 *= sfactor;
		s1 *= sfactor;
		SCurve->SetCurve(v0, s0, v1, s1);
	}
	break;
	case CurveType::e_ComplexType:
	{
		ComplexCurve* SCurve = (ComplexCurve*)this;
		float x1, x2, y1, y2;
		int32_t key;
		for (key = 0; key < SCurve->GetKeyCount() - 1; key++)
		{
			x1 = (*SCurve)[key].m_time;
			x2 = (*SCurve)[key + 1].m_time;
			y1 = SCurve->ComputeValue(x1, 0.0f);
			y2 = SCurve->ComputeValue(x2, 0.0f);
			y1 = axis + (y1 - axis) * sfactor;
			y2 = axis + (y2 - axis) * sfactor;
			(*SCurve)[key].SetLinearKey(x1, y1, y2, x2 - x1);
		}
		x1 = (*SCurve)[key].m_time;
		y1 = SCurve->ComputeValue(x1, 0.0f);
		y1 = axis + (y1 - axis) * sfactor;
		(*SCurve)[SCurve->GetKeyCount() - 1].SetLinearKey(x1, y1, y1, 1.0f);
	}
	break;
	case CurveType::e_ComplexLinearType:
	{
		SeededCurveOf<ComplexCurve, LinearCurve, Curve::CurveType::e_ComplexLinearType>* SCurve =
			(SeededCurveOf<ComplexCurve, LinearCurve, Curve::CurveType::e_ComplexLinearType>*)this;
		if (curvenum == 0)
			SCurve->m_ageCurve.AxisScale(sfactor, axis, curvenum);
		else
			SCurve->m_seedCurve.AxisScale(sfactor, axis, curvenum);
	}
	break;
	case CurveType::e_ComplexComplexType:
	{
		SeededCurveOf<ComplexCurve, ComplexCurve, CurveType::e_ComplexComplexType>* SCurve =
			(SeededCurveOf<ComplexCurve, ComplexCurve, CurveType::e_ComplexComplexType>*)this;
		if (curvenum == 0)
			SCurve->m_ageCurve.AxisScale(sfactor, axis, curvenum);
		else
			SCurve->m_seedCurve.AxisScale(sfactor, axis, curvenum);
	}
	break;
	case CurveType::e_ComplexSplineType:
	{
		SeededCurveOf<ComplexCurve, SplineCurve, CurveType::e_ComplexSplineType>* SCurve =
			(SeededCurveOf<ComplexCurve, SplineCurve, CurveType::e_ComplexSplineType>*)this;
		if (curvenum == 0)
			SCurve->m_ageCurve.AxisScale(sfactor, axis, curvenum);
		else
			SCurve->m_seedCurve.AxisScale(sfactor, axis, curvenum);
	}
	break;
	case CurveType::e_ConstantComplexType:
	{
		SeededCurveOf<ConstantCurve, ComplexCurve, CurveType::e_ConstantComplexType>* SCurve =
			(SeededCurveOf<ConstantCurve, ComplexCurve, CurveType::e_ConstantComplexType>*)this;
		if (curvenum == 0)
			SCurve->m_ageCurve.AxisScale(sfactor, axis, curvenum);
		else
			SCurve->m_seedCurve.AxisScale(sfactor, axis, curvenum);
	}
	break;
	case CurveType::e_ConstantLinearType:
	{
		SeededCurveOf<ConstantCurve, LinearCurve, CurveType::e_ConstantLinearType>* SCurve =
			(SeededCurveOf<ConstantCurve, LinearCurve, CurveType::e_ConstantLinearType>*)this;
		if (curvenum == 0)
			SCurve->m_ageCurve.AxisScale(sfactor, axis, curvenum);
		else
			SCurve->m_seedCurve.AxisScale(sfactor, axis, curvenum);
	}
	break;
	case CurveType::e_ConstantSplineType:
	{
		SeededCurveOf<ConstantCurve, SplineCurve, CurveType::e_ConstantSplineType>* SCurve =
			(SeededCurveOf<ConstantCurve, SplineCurve, CurveType::e_ConstantSplineType>*)this;
		if (curvenum == 0)
			SCurve->m_ageCurve.AxisScale(sfactor, axis, curvenum);
		else
			SCurve->m_seedCurve.AxisScale(sfactor, axis, curvenum);
	}
	break;
	case CurveType::e_SplineLinearType:
	{
		SeededCurveOf<SplineCurve, LinearCurve, CurveType::e_SplineLinearType>* SCurve =
			(SeededCurveOf<SplineCurve, LinearCurve, CurveType::e_SplineLinearType>*)this;
		if (curvenum == 0)
			SCurve->m_ageCurve.AxisScale(sfactor, axis, curvenum);
		else
			SCurve->m_seedCurve.AxisScale(sfactor, axis, curvenum);
	}
	break;
	case CurveType::e_SplineSplineType:
	{
		SeededCurveOf<SplineCurve, SplineCurve, CurveType::e_SplineSplineType>* SCurve =
			(SeededCurveOf<SplineCurve, SplineCurve, CurveType::e_SplineSplineType>*)this;
		if (curvenum == 0)
			SCurve->m_ageCurve.AxisScale(sfactor, axis, curvenum);
		else
			SCurve->m_seedCurve.AxisScale(sfactor, axis, curvenum);
	}
	break;
	default:
		break;
	}
}

gosFX::Curve*
gosFX::Curve::GetSubCurve(int32_t curvenum)
{
	// Check_Object(this);
	switch (m_type)
	{
	case CurveType::e_ConstantType:
	case CurveType::e_LinearType:
	case CurveType::e_SplineType:
	case CurveType::e_ComplexType:
	{
		if (curvenum == 0)
			return this;
		else
			return nullptr;
	}
	break;
	case CurveType::e_ComplexLinearType:
	{
		SeededCurveOf<ComplexCurve, LinearCurve, Curve::CurveType::e_ComplexLinearType>* SCurve =
			(SeededCurveOf<ComplexCurve, LinearCurve, Curve::CurveType::e_ComplexLinearType>*)this;
		if (curvenum == 0)
			return &(SCurve->m_ageCurve);
		else
			return &(SCurve->m_seedCurve);
	}
	break;
	case CurveType::e_ComplexComplexType:
	{
		SeededCurveOf<ComplexCurve, ComplexCurve, CurveType::e_ComplexComplexType>* SCurve =
			(SeededCurveOf<ComplexCurve, ComplexCurve, CurveType::e_ComplexComplexType>*)this;
		if (curvenum == 0)
			return &(SCurve->m_ageCurve);
		else
			return &(SCurve->m_seedCurve);
	}
	break;
	case CurveType::e_ComplexSplineType:
	{
		SeededCurveOf<ComplexCurve, SplineCurve, CurveType::e_ComplexSplineType>* SCurve =
			(SeededCurveOf<ComplexCurve, SplineCurve, CurveType::e_ComplexSplineType>*)this;
		if (curvenum == 0)
			return &(SCurve->m_ageCurve);
		else
			return &(SCurve->m_seedCurve);
	}
	break;
	case CurveType::e_ConstantComplexType:
	{
		SeededCurveOf<ConstantCurve, ComplexCurve, CurveType::e_ConstantComplexType>* SCurve =
			(SeededCurveOf<ConstantCurve, ComplexCurve, CurveType::e_ConstantComplexType>*)this;
		if (curvenum == 0)
			return &(SCurve->m_ageCurve);
		else
			return &(SCurve->m_seedCurve);
	}
	break;
	case CurveType::e_ConstantLinearType:
	{
		SeededCurveOf<ConstantCurve, LinearCurve, CurveType::e_ConstantLinearType>* SCurve =
			(SeededCurveOf<ConstantCurve, LinearCurve, CurveType::e_ConstantLinearType>*)this;
		if (curvenum == 0)
			return &(SCurve->m_ageCurve);
		else
			return &(SCurve->m_seedCurve);
	}
	break;
	case CurveType::e_ConstantSplineType:
	{
		SeededCurveOf<ConstantCurve, SplineCurve, CurveType::e_ConstantSplineType>* SCurve =
			(SeededCurveOf<ConstantCurve, SplineCurve, CurveType::e_ConstantSplineType>*)this;
		if (curvenum == 0)
			return &(SCurve->m_ageCurve);
		else
			return &(SCurve->m_seedCurve);
	}
	break;
	case CurveType::e_SplineLinearType:
	{
		SeededCurveOf<SplineCurve, LinearCurve, CurveType::e_SplineLinearType>* SCurve =
			(SeededCurveOf<SplineCurve, LinearCurve, CurveType::e_SplineLinearType>*)this;
		if (curvenum == 0)
			return &(SCurve->m_ageCurve);
		else
			return &(SCurve->m_seedCurve);
	}
	break;
	case CurveType::e_SplineSplineType:
	{
		SeededCurveOf<SplineCurve, SplineCurve, CurveType::e_SplineSplineType>* SCurve =
			(SeededCurveOf<SplineCurve, SplineCurve, CurveType::e_SplineSplineType>*)this;
		if (curvenum == 0)
			return &(SCurve->m_ageCurve);
		else
			return &(SCurve->m_seedCurve);
	}
	break;
	default:
		break;
	}
	return nullptr;
}

//##########################################################################
//########################    ConstantCurve    #############################
//##########################################################################

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
void gosFX::ConstantCurve::Save(std::iostream stream)
{
	// Check_Object(this);
	Check_Object(stream);
	*stream << m_value;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
void gosFX::ConstantCurve::Load(std::iostream stream, uint32_t gfx_version)
{
	// Check_Pointer(this);
	Check_Object(stream);
	*stream >> m_value;
}

//##########################################################################
//#########################    LinearCurve    ##############################
//##########################################################################

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
void gosFX::LinearCurve::Save(std::iostream stream)
{
	// Check_Object(this);
	Check_Object(stream);
	ConstantCurve::Save(stream);
	*stream << m_slope;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
void gosFX::LinearCurve::Load(std::iostream stream, uint32_t gfx_version)
{
	// Check_Pointer(this);
	Check_Object(stream);
	ConstantCurve::Load(stream, gfx_version);
	*stream >> m_slope;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// return:  true=math good, false=math unstable
bool gosFX::LinearCurve::SetCurve(float v)
{
	// Check_Object(this);
	m_slope = 0.0f;
	m_value = v;
	return true;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// return:  true=math good, false=math unstable
bool gosFX::LinearCurve::SetCurve(float v0, float v1)
{
	// Check_Object(this);
	m_slope = v1 - v0;
	m_value = v0;
	return Stuff::Close_Enough(m_slope + v0, v1);
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
void gosFX::LinearCurve::ComputeRange(float* low, float* hi)
{
	// Check_Object(this);
	Check_Pointer(low);
	Check_Pointer(hi);
	//
	//------------------------------------------------------------------------
	// We know that we will have to test the function at the beginning and end
	// of the segment, so go ahead and do that now
	//------------------------------------------------------------------------
	//
	*hi = *low = m_value;
	float t = m_slope + m_value;
	if (t > *hi)
		*hi = t;
	else if (t < *low)
		*low = t;
}

//##########################################################################
//##########################    SplineCurve    ##############################
//##########################################################################

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
gosFX::SplineCurve&
gosFX::SplineCurve::operator=(const SplineCurve& curve)
{
	m_a = curve.m_a;
	m_b = curve.m_b;
	m_slope = curve.m_slope;
	m_value = curve.m_value;
	return *this;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
void gosFX::SplineCurve::Save(std::iostream stream)
{
	// Check_Object(this);
	Check_Object(stream);
	LinearCurve::Save(stream);
	*stream << m_b << m_a;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
void gosFX::SplineCurve::Load(std::iostream stream, uint32_t gfx_version)
{
	// Check_Pointer(this);
	Check_Object(stream);
	LinearCurve::Load(stream, gfx_version);
	*stream >> m_b >> m_a;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// return:  true=math good, false=math unstable
bool gosFX::SplineCurve::SetCurve(float v)
{
	// Check_Object(this);
	m_a = m_b = m_slope = 0.0f;
	m_value = v;
	return true;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// return:  true=math good, false=math unstable
bool gosFX::SplineCurve::SetCurve(float v0, float v1)
{
	// Check_Object(this);
	m_a = 0.0f;
	m_b = 0.0f;
	m_slope = v1 - v0;
	m_value = v0;
	return Stuff::Close_Enough(m_slope + v0, v1);
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// return:  true=math good, false=math unstable
bool gosFX::SplineCurve::SetCurve(float v0, float s0, float v1, float s1)
{
	// Check_Object(this);
	m_a = (s1 + s0) + 2.0f * (v0 - v1);
	m_b = (s1 - s0 - 3.0f * m_a) * 0.5f;
	m_slope = s0;
	m_value = v0;
	return (Stuff::Close_Enough(m_a + m_b + m_slope + m_value, v1) && Stuff::Close_Enough(3.0f * m_a + 2.0f * m_b + m_slope, s1));
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
void gosFX::SplineCurve::ComputeRange(float* low, float* hi)
{
	// Check_Object(this);
	Check_Pointer(low);
	Check_Pointer(hi);
	//
	//------------------------------------------------------------------------
	// We know that we will have to test the function at the beginning and end
	// of the segment, so go ahead and do that now
	//------------------------------------------------------------------------
	//
	*hi = *low = m_value;
	float t = ComputeValue(1.0f, 0.0f);
	if (t > *hi)
		*hi = t;
	else if (t < *low)
		*low = t;
	//
	//----------------------------------------------------------------------
	// If the curve is not cubic, we just have to look for the local min/max
	// at the solution to 2*m_b*t + m_slope == 0.  If the curve is linear, we
	// just return
	//----------------------------------------------------------------------
	//
	if (!m_a)
	{
		if (m_b)
		{
			t = -0.5f * m_slope / m_b;
			if (t > 0.0f && t < 1.0f)
			{
				t = ComputeValue(t, 0.0f);
				if (t < *low)
					*low = t;
				else if (t > *hi)
					*hi = t;
			}
		}
		return;
	}
	//
	//----------------------------------------------------------------------
	// Now we need to deal with the cubic.  Its min/max will be at either of
	// the two roots of the equation 3*m_a*t*t + 2*m_b*t + m_slope == 0
	//----------------------------------------------------------------------
	//
	float da = 3.0f * m_a;
	float db = 2.0f * m_b;
	float range = db * db - 4.0f * da * m_slope;
	if (range < 0.0f)
		return;
	da = 0.5f / da;
	db = -db * da;
	range = Stuff::Sqrt(range) * da;
	//
	//------------------------------------------------------------------------
	// db now holds the midpoint between the two solutions, which will be at +
	// or - range from that point
	//------------------------------------------------------------------------
	//
	t = db - range;
	if (t > 0.0f && t < 1.0f)
	{
		t = ComputeValue(t, 0.0f);
		if (t < *low)
			*low = t;
		else if (t > *hi)
			*hi = t;
	}
	t = db + range;
	if (t > 0.0f && t < 1.0f)
	{
		t = ComputeValue(t, 0.0f);
		if (t < *low)
			*low = t;
		else if (t > *hi)
			*hi = t;
	}
}

//##########################################################################
//##########################    CurveKey    ###############################
//##########################################################################

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// return:  true=math good, false=math unstable
bool gosFX::CurveKey::SetConstantKey(float key_time, float v)
{
	// Check_Object(this);
	m_time = key_time;
	m_slope = 0.0f;
	m_value = v;
	return true;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// return:  true=math good, false=math unstable
bool gosFX::CurveKey::SetLinearKey(float key_time, float v0, float v1, float dt)
{
	// Check_Object(this);
	_ASSERT(dt > Stuff::SMALL);
	m_time = key_time;
	m_slope = (v1 - v0) / dt;
	m_value = v0;
	return Stuff::Close_Enough(m_slope * dt + v0, v1);
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
void gosFX::CurveKey::ComputeRange(float* low, float* hi, float dt)
{
	// Check_Object(this);
	Check_Pointer(low);
	Check_Pointer(hi);
	//
	//------------------------------------------------------------------------
	// We know that we will have to test the function at the beginning and end
	// of the segment, so go ahead and do that now
	//------------------------------------------------------------------------
	//
	*hi = *low = m_value;
	if (dt < Stuff::SMALL)
		return;
	float t = ComputeValue(dt);
	if (t > *hi)
		*hi = t;
	else if (t < *low)
		*low = t;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
gosFX::ComplexCurve::ComplexCurve()
	: Curve(CurveType::e_ComplexType)
{
	// Check_Pointer(this);
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
gosFX::ComplexCurve::ComplexCurve(const ComplexCurve& fcurve)
	: Curve(CurveType::e_ComplexType)
{
	// Check_Pointer(this);
	m_keys = fcurve.m_keys;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
gosFX::ComplexCurve::ComplexCurve(std::iostream stream, uint32_t gfx_version)
	: Curve(CurveType::e_ComplexType)
{
	// Check_Pointer(this);
	Check_Object(stream);
	Load(stream, gfx_version);
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
gosFX::ComplexCurve&
gosFX::ComplexCurve::operator=(const ComplexCurve& fcurve)
{
	// Check_Pointer(this);
	m_keys = fcurve.m_keys;
	return *this;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
void gosFX::ComplexCurve::Save(std::iostream stream)
{
	// Check_Object(this);
	Check_Object(stream);
	Stuff::MemoryStreamIO_Write(stream, &m_keys);
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
void gosFX::ComplexCurve::Load(std::iostream stream, uint32_t gfx_version)
{
	// Check_Pointer(this);
	Check_Object(stream);
	if (gfx_version < 15)
	{
		float duration;
		*stream >> duration;
	}
	Stuff::MemoryStreamIO_Read(stream, &m_keys);
	//
	//--------------------------------------------------------------------------
	// If we are reading a previous version, make sure the tail doesn't go below
	// zero
	//--------------------------------------------------------------------------
	//
	CurveKey* key = &m_keys[m_keys.GetLength() - 1];
	float dt = 1.0f - key->m_time;
	Min_Clamp(dt, 0.0f);
	float low, hi;
	key->ComputeRange(&low, &hi, dt);
	if (low < 0.0f)
	{
		key->SetLinearKey(key->m_time, key->m_value, 0.0f, dt);
	}
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
int32_t
gosFX::ComplexCurve::InsertKey(float time)
{
// Check_Object(this);
#ifdef _GAMEOS_HPP_
	// gos_PushCurrentHeap(Heap);
#endif
	//
	//-----------------------------------------------------------------------
	// Figure out where to insert the next key, then increase the size of the
	// key array and shift the m_keys after the insert point up one slot
	//-----------------------------------------------------------------------
	//
	//#if CONSIDERED_OBSOLETE
	size_t before = GetKeyIndex(time);
	size_t key_count = m_keys.size();
	m_keys.resize(key_count + 1);
	for (size_t i = key_count - 1; i >= before; --i)
		m_keys[i + 1] = m_keys[i];

	CurveKey* key;
	if (key_count > 0)
		key = &(*this)[++before];
	else
		key = &(*this)[before];
	Check_Object(key);
	//#endif

	//
	//-----------------------------------------------------------------------
	// If this is an insert as opposed to an append, we need to set the key
	// values of the new segment so they smoothly complete the original curve
	// being inserted into
	//-----------------------------------------------------------------------
	//
	if (before < key_count)
	{
		float t = time - key->m_time;
		float v0 = key->ComputeValue(t);
		t = key[1].m_time - key->m_time;
		float v1 = key->ComputeValue(t);
		key->SetLinearKey(time, v0, v1, key[1].m_time - time);
	}
	//
	//-----------------------------------------------------------------------
	// Otherwise, we are appending, so all we can do is establish m_a key equal
	// to the the previous key at this m_time
	//-----------------------------------------------------------------------
	//
	else if (key_count > 0)
	{
		_ASSERT(before == key_count);
		float t = time - key->m_time;
		float v0 = key->ComputeValue(t);
		t += 1.0f;
		float v1 = key->ComputeValue(t);
		key->SetLinearKey(time, v0, v1, 1.0f);
	}
	//
	//-----------------------------------------------------------------
	// In we are inserting the first key, make m_a constant m_value of zero
	//-----------------------------------------------------------------
	//
	else
		key->SetConstantKey(time, 0.0f);
	//
	//-------------------------
	// Return the new key index
	//-------------------------
	//
	// gos_PopCurrentHeap();
	return before;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
void gosFX::ComplexCurve::DeleteKey(int32_t index)
{
	// Check_Object(this);
	_ASSERT(index > 0 && index < m_keys.GetLength());
#ifdef _GAMEOS_HPP_
	// gos_PushCurrentHeap(Heap);
#endif
	//
	//---------------------------------------------------
	// If this is the last key, we just resize and return
	//---------------------------------------------------
	//
	int32_t key_count = m_keys.GetLength();
	if (index == key_count - 1)
	{
		m_keys.SetLength(index);
		// gos_PopCurrentHeap();
		return;
	}
	//
	//-----------------------------------------------------------
	// Compute the ending values of this key, then shift the m_keys
	//-----------------------------------------------------------
	//
	CurveKey* key = &(*this)[index];
	Check_Object(key);
	float t = key[1].m_time - key->m_time;
	float v1 = key->ComputeValue(t);
	for (size_t i = index + 1; i < key_count; ++i)
		m_keys[i - 1] = m_keys[i];
	//
	//-------------------------------------------------
	// Now connect the previous key to the new next key
	//-------------------------------------------------
	//
	--key;
	key->SetLinearKey(key->m_time, key->m_value, v1, key[1].m_time - key->m_time);
	m_keys.SetLength(key_count - 1);
	// gos_PopCurrentHeap();
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
void gosFX::ComplexCurve::SetCurve(float m_value)
{
// Check_Object(this);
#ifdef _GAMEOS_HPP_
	// gos_PushCurrentHeap(Heap);
#endif
	m_keys.SetLength(1);
	m_keys[0].SetConstantKey(0.0f, m_value);
	// gos_PopCurrentHeap();
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
void gosFX::ComplexCurve::SetCurve(float starting_value, float ending_value)
{
// Check_Object(this);
//
//-----------------------------------------------------------------------
// Build the linear step, then the constant m_value at the end of the curve
//-----------------------------------------------------------------------
//
#ifdef _GAMEOS_HPP_
	// gos_PushCurrentHeap(Heap);
#endif
	m_keys.SetLength(1);
	m_keys[0].SetLinearKey(0.0f, starting_value, ending_value, 1.0f);
	// gos_PopCurrentHeap();
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
void gosFX::ComplexCurve::ComputeRange(float* low, float* hi)
{
	// Check_Object(this);
	Check_Pointer(low);
	Check_Pointer(hi);
	//
	//--------------------------------------------
	// If the key is empty, set everything to zero
	//--------------------------------------------
	//
	int32_t key_count = m_keys.GetLength();
	if (!key_count)
	{
		*low = *hi = 0.0f;
		return;
	}
	//
	//----------------------------
	// Deal with the last keyframe
	//----------------------------
	//
	CurveKey* key = &m_keys[--key_count];
	float dt = 1.0f - key->m_time;
	Min_Clamp(dt, 0.0f);
	key->ComputeRange(low, hi, dt);
	//
	//------------------------------------
	// Now deal with the preceeding frames
	//------------------------------------
	//
	while (--key_count >= 0)
	{
		--key;
		float h, l;
		key->ComputeRange(&l, &h, key[1].m_time - key->m_time);
		if (h > *hi)
			*hi = h;
		if (l < *low)
			*low = l;
	}
}
