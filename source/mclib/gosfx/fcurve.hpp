//===========================================================================//
// File:	fcurve.hpp                                                       //
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#pragma once

//#include <gosfx/gosfx.hpp>

namespace gosFX
{

class Curve
{
  public:
	typedef enum CurveType
	{
		e_ConstantType,
		e_LinearType,
		e_SplineType,
		e_ComplexType,
		e_ComplexLinearType,
		e_ComplexComplexType,
		e_ComplexSplineType,
		e_ConstantComplexType,
		e_ConstantLinearType,
		e_ConstantSplineType,
		e_SplineLinearType,
		e_SplineSplineType
	} CurveType;

	const CurveType m_type;

	Curve(CurveType type) : m_type(type) {}

	float ExpensiveCompute(float tme, int32_t curvenum = 0);

	void ExpensiveComputeRange(float* low, float* hi, int32_t curvenum);

	void ExpensiveComputeRange(float* low, float* hi);

	void Save(Stuff::MemoryStream* stream);
	void Load(Stuff::MemoryStream* stream, uint32_t gfx_version);

	float Mid(int32_t curvenum = 0);

	bool IsComplex(void);

	void SetSeedFlagIfComplex(bool vflag);

	int32_t GetSeedFlagIfComplex(void);

	void TranslateTo(float pos, int32_t curvenum = 0);

	void TranslateBy(float delta, int32_t curvenum = 0);

	void LocalScale(float sfactor, int32_t curvenum = 0);

	void AxisScale(float sfactor, float axis = 0.0f, int32_t curvenum = 0);

	Curve* GetSubCurve(int32_t curvenum);

	void TestInstance(void) const {}
};

//######################################################################
//####################    ConstantCurve    #############################
//######################################################################

class ConstantCurve : public Curve
{
  public:
	float m_value;

	ConstantCurve(CurveType type = e_ConstantType) : Curve(type) {}

	ConstantCurve& operator=(const ConstantCurve& curve)
	{
		m_value = curve.m_value;
		return *this;
	}

	void Save(Stuff::MemoryStream* stream);
	void Load(Stuff::MemoryStream* stream, uint32_t gfx_version);

	//------------------------------------------------------
	// for Set...Key(), true=math good, false=math unstable
	//------------------------------------------------------
	bool SetCurve(float v)
	{
		// Check_Object(this);
		m_value = v;
		return true;
	}

	float ComputeValue(float, float)
	{
		// Check_Object(this);
		return m_value;
	}
	float ComputeSlope(float)
	{
		// Check_Object(this);
		return 0.0f;
	}

	void ComputeRange(float* low, float* hi)
	{
		// Check_Object(this);
		Check_Pointer(low);
		Check_Pointer(hi);
		if (hi && low)
			*hi = *low = m_value;
	}
};

//######################################################################
//#####################    LinearCurve    ##############################
//######################################################################

class LinearCurve : public ConstantCurve
{
  public:
	float m_slope;

	LinearCurve(CurveType type = e_LinearType) : ConstantCurve(type) {}

	LinearCurve& operator=(const LinearCurve& curve)
	{
		m_slope = curve.m_slope;
		m_value = curve.m_value;
		return *this;
	}

	void Save(Stuff::MemoryStream* stream);
	void Load(Stuff::MemoryStream* stream, uint32_t gfx_version);

	//------------------------------------------------------
	// for Set...Key(), true=math good, false=math unstable
	//------------------------------------------------------
	bool SetCurve(float v);
	bool SetCurve(float v0, float v1);

	float ComputeValue(float t, float)
	{
		// Check_Object(this);
		return m_slope * t + m_value;
	}
	float ComputeSlope(float)
	{
		// Check_Object(this);
		return m_slope;
	}

	void ComputeRange(float* low, float* hi);
};

//######################################################################
//######################    SplineCurve    ##############################
//######################################################################

class SplineCurve : public LinearCurve
{
  public:
	float m_a, m_b;

	SplineCurve() : LinearCurve(e_SplineType) {}

	SplineCurve& operator=(const SplineCurve& curve);

	void Save(Stuff::MemoryStream* stream);
	void Load(Stuff::MemoryStream* stream, uint32_t gfx_version);

	//------------------------------------------------------
	// for Set...Key(), true=math good, false=math unstable
	//------------------------------------------------------
	bool SetCurve(float v);
	bool SetCurve(float v0, float v1);
	bool SetCurve(float v0, float s0, float v1, float s1);

	float ComputeValue(float t, float)
	{
		// Check_Object(this);
		return ((m_a * t + m_b) * t + m_slope) * t + m_value;
	}
	float ComputeSlope(float t)
	{
		// Check_Object(this);
		return (3.0f * m_a * t + 2.0f * m_b) * t + m_slope;
	}

	void ComputeRange(float* low, float* hi);
};

//######################################################################
//######################    CurveKey    ###############################
//######################################################################

class CurveKey
{
  public:
	float m_time, m_slope, m_value;

	//------------------------------------------------------
	// for Set...Key(), true=math good, false=math unstable
	//------------------------------------------------------
	bool SetConstantKey(float key_time, float v);
	bool SetLinearKey(float key_time, float v0, float v1, float dt);

	float ComputeValue(float t)
	{
		// Check_Object(this);
		return m_slope * t + m_value;
	}
	float ComputeSlope(float)
	{
		// Check_Object(this);
		return m_slope;
	}

	void ComputeRange(float* low, float* hi, float dt);

	void TestInstance(void) const {}
};

//######################################################################
//########################    ComplexCurve    ################################
//######################################################################

class ComplexCurve : public Curve
{
  protected:
	Stuff::DynamicArrayOf<CurveKey> m_keys;

  public:
	ComplexCurve(void);
	ComplexCurve(const ComplexCurve& fcurve);
	ComplexCurve(Stuff::MemoryStream* stream, uint32_t gfx_version);

	ComplexCurve& operator=(const ComplexCurve& fcurve);

	void Save(Stuff::MemoryStream* stream);
	void Load(Stuff::MemoryStream* stream, uint32_t gfx_version);

	//-----------------------------------------------------------------
	// Warning:  both index of and pointer to CurveKey's are volitile
	//           client code should store neither between transactions
	//-----------------------------------------------------------------

	CurveKey& operator[](int32_t index)
	{
		// Check_Object(this);
		return m_keys[index];
	}
	size_t GetKeyCount()
	{
		// Check_Object(this);
		return m_keys.GetLength(void);
	}
	int32_t GetKeyIndex(float time)
	{
		// Check_Object(this);
		for (auto i = 0; i < m_keys.GetLength(); ++i)
			if (m_keys[i].m_time > time)
				break;
		return --i;
	}

	int32_t InsertKey(float time);
	void DeleteKey(int32_t index);

	void SetCurve(float v);
	void SetCurve(float starting_value, float ending_value);

	float ComputeValue(float time, float)
	{
		// Check_Object(this);
		CurveKey& key = (*this)[GetKeyIndex(time)];
		return key.ComputeValue(time - key.m_time);
	}
	float ComputeSlope(float time)
	{
		// Check_Object(this);
		CurveKey& key = (*this)[GetKeyIndex(time)];
		return key.ComputeSlope(time - key.m_time);
	}

	void ComputeRange(float* low, float* hi);
};

//######################################################################
//#####################    SeededCurveOf    ############################
//######################################################################

template <class C, class S, Curve::CurveType type>
class SeededCurveOf : public Curve
{
  public:
	C m_ageCurve;
	S m_seedCurve;
	bool m_seeded;

	SeededCurveOf() : Curve(type) {}

	SeededCurveOf<C, S, type>& operator=(
		const SeededCurveOf<C, S, type>& curve);
	void Save(Stuff::MemoryStream* stream);
	void Load(Stuff::MemoryStream* stream, uint32_t gfx_version);

	float ComputeValue(float age, float seed)
	{
		// Check_Object(this);
		float result = m_ageCurve.ComputeValue(age, 0.0f);
		if (m_seeded)
			result *= m_seedCurve.ComputeValue(seed, 0.0f);
		return result;
	}
	void ComputeRange(float* low, float* hi);

	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Testing
	//
  public:
	void TestInstance(void) const {}
};

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

template <class C, class S, Curve::CurveType type>
SeededCurveOf<C, S, type>& SeededCurveOf<C, S, type>::operator=(
	const SeededCurveOf<C, S, type>& curve)
{
	m_ageCurve  = curve.m_ageCurve;
	m_seedCurve = curve.m_seedCurve;
	m_seeded	= curve.m_seeded;
	return *this;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

template <class C, class S, Curve::CurveType type>
void SeededCurveOf<C, S, type>::Save(Stuff::MemoryStream* stream)
{
	m_ageCurve.Save(stream);
	m_seedCurve.Save(stream);
	*stream << m_seeded;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

template <class C, class S, Curve::CurveType type>
void SeededCurveOf<C, S, type>::Load(
	Stuff::MemoryStream* stream, uint32_t gfx_version)
{
	m_ageCurve.Load(stream, gfx_version);
	m_seedCurve.Load(stream, gfx_version);
	*stream >> m_seeded;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

template <class C, class S, Curve::CurveType type>
void SeededCurveOf<C, S, type>::ComputeRange(float* low, float* hi)
{
	float low_age, hi_age;
	m_ageCurve.ComputeRange(&low_age, &hi_age);
	float low_seed, hi_seed;
	m_seedCurve.ComputeRange(&low_seed, &hi_seed);
	*low = *hi = low_age * low_seed;
	float temp = low_age * hi_seed;
	if (temp < *low)
		*low = temp;
	else if (temp > *hi)
		*hi = temp;
	temp = hi_age * low_seed;
	if (temp < *low)
		*low = temp;
	else if (temp > *hi)
		*hi = temp;
	temp = hi_age * hi_seed;
	if (temp < *low)
		*low = temp;
	else if (temp > *hi)
		*hi = temp;
}
}
