//===========================================================================//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#include "stdafx.h"
#include "gosfxheaders.hpp"
#include <mlr\mlrcardcloud.hpp>

//############################################################################
//########################  gosFX::Card__Specification  #############################
//############################################################################

//const Stuff::Point3D &_v,
//const Stuff::Matrix4D &m,
//Stuff::Scalar *uv
//int foggy

//void junk(void)
//{
//	Stuff::Point3D p3d;
//	Stuff::Matrix4D m4d;
//	Stuff::Scalar uv;
//
//	MidLevelRenderer::GOSVertex obj;
//	obj.GOSTransformNoClip(p3d, m4d, &uv, 0);
//};

#if 0
int __stdcall MidLevelRenderer::GOSVertex::GOSTransformNoClip(void *v, void *m, float *uv, int foggy)
{
	int v4; // ecx@0
	void *v5; // edx@6
	double v6; // st7@6
	double v7; // st6@6
	double v8; // st5@6
	int v9; // eax@6
	double v10; // st4@6
	double v11; // st3@6
	double v12; // st2@6
	double v13; // st4@6
	double v14; // st2@6
	double v15; // st3@6
	double v16; // st2@6
	double v17; // st4@6
	double v18; // st2@6
	double v19; // st3@6
	double v20; // st5@6
	unsigned __int16 v21; // ax@7
	int v22; // ecx@9
	int v24; // [sp+Ch] [bp-2Ch]@1
	int v25; // [sp+10h] [bp-28h]@24
	unsigned int v26; // [sp+14h] [bp-24h]@23
	float v27; // [sp+18h] [bp-20h]@22
	int v28; // [sp+1Ch] [bp-1Ch]@15
	int v29; // [sp+20h] [bp-18h]@13
	unsigned int v30; // [sp+24h] [bp-14h]@12
	float v31; // [sp+28h] [bp-10h]@11
	float *f; // [sp+2Ch] [bp-Ch]@6
	float x; // [sp+30h] [bp-8h]@1
	unsigned int v34; // [sp+34h] [bp-4h]@1
	int v35; // [sp+38h] [bp+0h]@1

	memset(&v24, 0xCCCCCCCCu, 0x2Cu);
	v34 = (unsigned int)&v35 ^ __security_cookie;
	LODWORD(x) = v4;
	if ( Stuff::ArmorLevel > 0
		&& (!LODWORD(x) || LODWORD(x) == 0xFFB1FFB1)
		&& InternalFunctionPause("Failed (this) && reinterpret_cast<int>(this)!=Stuff::SNAN_NEGATIVE_LONG") )
		__debugbreak();
	Stuff::Check_Object_Function<Stuff::Point3D_const>(v);
	Stuff::Check_Object_Function<Stuff::Matrix4D_const>(m);
	f = (float *)LODWORD(x);
	v5 = m;
	v6 = *(float *)v;
	v7 = *((float *)v + 1);
	v8 = *((float *)v + 2);
	v9 = LODWORD(x);
	v10 = *((float *)m + 5) * v7;
	v11 = *((float *)m + 6) * v8;
	*(float *)(LODWORD(x) + 12) = *((float *)m + 13) * v7
		+ *((float *)m + 15)
		+ *((float *)m + 14) * v8
		+ *((float *)m + 12) * v6;
	v12 = v10;
	v13 = *((float *)m + 9) * v7;
	v14 = v11 + *((float *)v5 + 7) + v12 + *((float *)v5 + 4) * v6;
	v15 = *((float *)m + 10) * v8;
	*(float *)(v9 + 4) = v14;
	v16 = v13;
	v17 = *((float *)m + 1) * v7;
	v18 = v15 + *((float *)v5 + 11) + v16 + *((float *)v5 + 8) * v6;
	v19 = *((float *)m + 2) * v8;
	*(float *)(v9 + 8) = v18;
	v20 = *(float *)m * v6 + v19 + *((float *)m + 3) + v17;
	*(float *)v9 = v20;

	if ( foggy )
	{
		v21 = Stuff::Truncate_Float_To_Word(*(_DWORD *)(LODWORD(x) + 12));
		*(_BYTE *)(LODWORD(x) + 23) = *((_BYTE *)&MidLevelRenderer::GOSVertex::fogTable + 1024 * (foggy - 1) + v21);
	}
	else
	{
		*(_BYTE *)(LODWORD(x) + 23) = -1;
	}
	v22 = LODWORD(x);
	*(float *)(LODWORD(x) + 12) = 1.0 / *(float *)(LODWORD(x) + 12);
	if ( Stuff::ArmorLevel > 0 )
	{
		if ( (unsigned __int8)MidLevelRenderer::MLRState::GetHasMaxUVs(v22) )
		{
			v29 = (v31 = MidLevelRenderer::MLRState::GetMaxUV(), v31 > *uv)
				&& (*(float *)&v30 = MidLevelRenderer::MLRState::GetMaxUV(),
				*uv > COERCE_FLOAT(*(_OWORD *)&_mm_xor_ps((__m128)v30, (__m128)_xmm)));
			v28 = v29;
		}
		else
		{
			v28 = 1;
		}
		if ( !v28
			&& InternalFunctionPause("Failed MLRState::GetHasMaxUVs() ? (uv[0]<MLRState::GetMaxUV() && uv[0]>-MLRState::GetMaxUV()) : 1") )
			__debugbreak();
	}
	if ( Stuff::ArmorLevel > 0 )
	{
		if ( (unsigned __int8)MidLevelRenderer::MLRState::GetHasMaxUVs(0) )
		{
			v25 = (v27 = MidLevelRenderer::MLRState::GetMaxUV(), v27 > uv[1])
				&& (*(float *)&v26 = MidLevelRenderer::MLRState::GetMaxUV(),
				uv[1] > COERCE_FLOAT(*(_OWORD *)&_mm_xor_ps((__m128)v26, (__m128)_xmm)));
			v24 = v25;
		}
		else
		{
			v24 = 1;
		}
		if ( !v24
			&& InternalFunctionPause("Failed MLRState::GetHasMaxUVs() ? (uv[1]<MLRState::GetMaxUV() && uv[1]>-MLRState::GetMaxUV()) : 1") )
			__debugbreak();
	}
	*(_DWORD *)(LODWORD(x) + 24) = *(_DWORD *)uv;
	*(_DWORD *)(LODWORD(x) + 28) = *((_DWORD *)uv + 1);
	*LODWORD(x) = *LODWORD(x) * *(float *)(LODWORD(x) + 12);
	*(float *)(LODWORD(x) + 4) = *(float *)(LODWORD(x) + 4) * *(float *)(LODWORD(x) + 12);
	*(float *)(LODWORD(x) + 8) = *(float *)(LODWORD(x) + 8) * *(float *)(LODWORD(x) + 12);
	if ( Stuff::ArmorLevel > 0
		&& *(float *)(LODWORD(x) + 12) <= Stuff::SMALL
		&& InternalFunctionPause("Failed rhw > Stuff::SMALL") )
		__debugbreak();
	if ( Stuff::ArmorLevel > 0 && *LODWORD(x) < 0.0 && InternalFunctionPause("Failed x >= 0.0f") )
		__debugbreak();
	if ( Stuff::ArmorLevel > 0 && *(float *)(LODWORD(x) + 4) < 0.0 && InternalFunctionPause("Failed y >= 0.0f") )
		__debugbreak();
	if ( Stuff::ArmorLevel > 0 && *(float *)(LODWORD(x) + 8) < 0.0 && InternalFunctionPause("Failed z >= 0.0f") )
		__debugbreak();
	if ( Stuff::ArmorLevel > 0 && *LODWORD(x) > 1.0 && InternalFunctionPause("Failed x <= 1.0f") )
		__debugbreak();
	if ( Stuff::ArmorLevel > 0 && *(float *)(LODWORD(x) + 4) > 1.0 && InternalFunctionPause("Failed y <= 1.0f") )
		__debugbreak();
	if ( Stuff::ArmorLevel > 0 && *(float *)(LODWORD(x) + 8) >= 1.0 && InternalFunctionPause("Failed z < 1.0f") )
		__debugbreak();
	*LODWORD(x) = (float)(*LODWORD(x) * MidLevelRenderer::ViewportScalars::MulX)
		+ MidLevelRenderer::ViewportScalars::AddX;
	*(float *)(LODWORD(x) + 4) = (float)(*(float *)(LODWORD(x) + 4) * MidLevelRenderer::ViewportScalars::MulY)
		+ MidLevelRenderer::ViewportScalars::AddY;
	__security_check_cookie();
	return _RTC_CheckEsp();
}

struct Point3D: public Vector3D
{
	const Point3D Identity;

	//
	// Constructors
	//
	Point3D() {}
	Point3D(
		Scalar x,
		Scalar y,
		Scalar z
		):
	Vector3D(x,y,z)
	{}
	Point3D(const Point3D &p):
	Vector3D(p)
	{}
	explicit Point3D(const Vector3D &v)
	{*this = v;}
	explicit Point3D(const Vector4D &v)
	{*this = v;}
	explicit Point3D(const Origin3D &origin)
	{*this = origin;}
	explicit Point3D(const AffineMatrix4D &matrix)
	{*this = matrix;}
	explicit Point3D(const YawPitchRange &polar)
	{*this = polar;}

	//
	// Assignment operators
	//
	Point3D&
		operator=(const Vector3D& v)
	{Vector3D::operator=(v); return *this;}
	Point3D&
		operator=(const Vector4D& v);
	Point3D&
		operator=(const Origin3D &p);
	Point3D&
		operator=(const AffineMatrix4D &matrix);
	Point3D&
		operator=(const YawPitchRange &polar)
	{Vector3D::operator=(polar); return *this;}

	//
	// Math operators
	//
	Point3D&
		Negate(const Vector3D &v)
	{Vector3D::Negate(v); return *this;}

	Point3D&
		Add(
		const Vector3D& v1,
		const Vector3D& v2
		)
	{Vector3D::Add(v1,v2); return *this;}
	Point3D&
		operator+=(const Vector3D& v)
	{return Add(*this,v);}

	Point3D&
		Subtract(
		const Point3D& p,
		const Vector3D& v
		)
	{Vector3D::Subtract(p,v); return *this;}
	Point3D&
		operator-=(const Vector3D& v)
	{return Subtract(*this,v);}

	Scalar
		operator*(const Vector3D& v) const
	{return Vector3D::operator*(v);}

	Point3D&
		Multiply(
		const Vector3D& v,
		Scalar scale
		)
	{Vector3D::Multiply(v,scale); return *this;}
	Point3D&
		Multiply(
		const Point3D& p,
		Scalar scale
		)
	{Vector3D::Multiply(p,scale); return *this;}
	Point3D&
		operator*=(Scalar value)
	{return Multiply(*this,value);}

	Point3D&
		Multiply(
		const Point3D& p,
		const Vector3D& v
		)
	{Vector3D::Multiply(p,v); return *this;}
	Point3D&
		operator*=(const Vector3D &v)
	{return Multiply(*this,v);}

	Point3D&
		Divide(
		const Vector3D& v,
		Scalar scale
		)
	{Vector3D::Divide(v,scale); return *this;}
	Point3D&
		operator/=(Scalar value)
	{return Divide(*this,value);}

	Point3D&
		Divide(
		const Vector3D& v1,
		const Vector3D& v2
		)
	{Vector3D::Divide(v1,v2); return *this;}
	Point3D&
		operator/=(const Vector3D &v)
	{return Divide(*this,v);}

	//
	// Transforms
	//
	Point3D&
		Multiply(
		const Vector3D &v,
		const AffineMatrix4D &m
		)
	{Vector3D::Multiply(v,m); return *this;}
	Point3D&
		Multiply(
		const Point3D &p,
		const AffineMatrix4D &m
		);
	Point3D&
		operator*=(const AffineMatrix4D &m)
	{Point3D src(*this); return Multiply(src,m);}
	Point3D&
		MultiplyByInverse(
		const Vector3D &v,
		const LinearMatrix4D &m
		)
	{Vector3D::MultiplyByInverse(v,m); return *this;}
	Point3D&
		MultiplyByInverse(
		const Point3D &p,
		const LinearMatrix4D &m
		);

	//
	// Miscellaneous functions
	//
	Point3D&
		Combine(
		const Vector3D& v1,
		Scalar t1,
		const Vector3D& v2,
		Scalar t2
		)
	{Vector3D::Combine(v1,t1,v2,t2); return *this;}

	//
	// Template support functions
	//
	Point3D&
		Lerp(
		const Vector3D& v1,
		const Vector3D& v2,
		Scalar t
		)
	{Vector3D::Lerp(v1,v2,t); return *this;}

	static bool
		TestClass();
};

#endif

//------------------------------------------------------------------------------
//
gosFX::Card__Specification::Card__Specification(
	Stuff::MemoryStream *stream,
	int gfx_version
):
	Singleton__Specification(gosFX::CardClassID, stream, gfx_version)
{
	Check_Pointer(this);
	Verify(m_class == CardClassID);
	Verify(gos_GetCurrentHeap() == Heap);

	m_halfHeight.Load(stream, gfx_version);
	m_aspectRatio.Load(stream, gfx_version);

	//
	//-------------------------------------------------------------------
	// If we are reading an old version of the card cloud, ignore all the
	// animation on the UV channels
	//-------------------------------------------------------------------
	//
	if (gfx_version < 10)
	{
		m_index.m_ageCurve.SetCurve(0.0f);
		m_index.m_seedCurve.SetCurve(1.0f);
		m_index.m_seeded = false;

		SeededCurveOf<ComplexCurve, LinearCurve,Curve::e_ComplexLinearType> temp;
		temp.Load(stream, gfx_version);
		Stuff::Scalar v = temp.ComputeValue(0.0f, 0.0f);
		m_UOffset.SetCurve(v);

		temp.Load(stream, gfx_version);
		v = temp.ComputeValue(0.0f, 0.0f);
		m_VOffset.SetCurve(v);

		m_USize.Load(stream, gfx_version);
		m_VSize.Load(stream, gfx_version);

		m_animated = false;
	}

	//
	//------------------------------
	// Otherwise, read in the curves
	//------------------------------
	//
	else
	{
		m_index.Load(stream, gfx_version);
		m_UOffset.Load(stream, gfx_version);
		m_VOffset.Load(stream, gfx_version);
		m_USize.Load(stream, gfx_version);
		m_VSize.Load(stream, gfx_version);
		*stream >> m_animated;
	}
	SetWidth();
}

//------------------------------------------------------------------------------
//
gosFX::Card__Specification::Card__Specification():
	Singleton__Specification(gosFX::CardClassID)
{
	Check_Pointer(this);
	Verify(gos_GetCurrentHeap() == Heap);
	m_animated = false;
	m_width = 1;
}

//------------------------------------------------------------------------------
//
gosFX::Card__Specification*
	gosFX::Card__Specification::Make(
		Stuff::MemoryStream *stream,
		int gfx_version
	)
{
	Check_Object(stream);

	gos_PushCurrentHeap(Heap);
	Card__Specification *spec =
		new gosFX::Card__Specification(stream, gfx_version);
	gos_PopCurrentHeap();

	return spec;
}

//------------------------------------------------------------------------------
//
void
	gosFX::Card__Specification::Save(Stuff::MemoryStream *stream)
{
	Check_Object(this);
	Check_Object(stream);
	Singleton__Specification::Save(stream);

	m_halfHeight.Save(stream);
	m_aspectRatio.Save(stream);
	m_index.Save(stream);
	m_UOffset.Save(stream);
	m_VOffset.Save(stream);
	m_USize.Save(stream);
	m_VSize.Save(stream);
	*stream << m_animated;
}

//------------------------------------------------------------------------------
//
void 
	gosFX::Card__Specification::BuildDefaults()
{

	Check_Object(this);
	Singleton__Specification::BuildDefaults();

	m_alignZUsingX = false;
	m_alignZUsingY = false;

	m_halfHeight.m_ageCurve.SetCurve(1.0f);
	m_halfHeight.m_seeded = false;
	m_halfHeight.m_seedCurve.SetCurve(1.0f);

	m_aspectRatio.m_ageCurve.SetCurve(1.0f);
	m_aspectRatio.m_seeded = false;
	m_aspectRatio.m_seedCurve.SetCurve(1.0f);

	m_index.m_ageCurve.SetCurve(0.0f);
	m_index.m_seeded = false;
	m_index.m_seedCurve.SetCurve(1.0f);

	m_UOffset.SetCurve(0.0f);
	m_VOffset.SetCurve(0.0f);
	m_USize.SetCurve(1.0f);
	m_VSize.SetCurve(1.0f);

	m_animated = false;
	m_width = 1;
}


//------------------------------------------------------------------------------
//
bool 
	gosFX::Card__Specification::IsDataValid(bool fix_data)
{

	Check_Object(this);

	Stuff::Scalar max_offset, min_offset;
	Stuff::Scalar max_scale, min_scale;
	m_USize.ExpensiveComputeRange(&min_scale, &max_scale);
	Stuff::Scalar lower = min_scale;
	if (lower > 0.0f)
		lower = 0.0f;
	Stuff::Scalar upper = max_scale;

	//
	//------------------------------------
	// Calculate the worst case UV offsets
	//------------------------------------
	//
	m_VOffset.ExpensiveComputeRange(&min_offset, &max_offset);
	lower += min_offset;
	upper += max_offset;

	if (upper > 99.0f || lower < -99.0f)
	{
		if(fix_data)
		{
		m_VOffset.SetCurve(0.0f);
		PAUSE(("Warning: Curve \"VOffset\" in Effect \"%s\" Is Out of Range and has been Reset",(char *)m_name));
		}
			else
		return false;
	}
	
	m_VSize.ExpensiveComputeRange(&min_scale, &max_scale);
	lower = min_scale;
	if (lower > 0.0f)
		lower = 0.0f;
	upper = max_scale;

	//
	//------------------------------------
	// Calculate the worst case UV offsets
	//------------------------------------
	//
	max_offset, min_offset;
	m_UOffset.ExpensiveComputeRange(&min_offset, &max_offset);
	lower += min_offset;
	upper += max_offset;

	if (upper > 99.0f || lower < -99.0f)
	{
		if(fix_data)
		{
			m_UOffset.SetCurve(0.0f);
			PAUSE(("Warning: Curve \"UOffset\" in Effect \"%s\" Is Out of Range and has been Reset",(char *)m_name));

		}
		else
			return false;
	}

return		Singleton__Specification::IsDataValid(fix_data);
}


//------------------------------------------------------------------------------
//
void
	gosFX::Card__Specification::Copy(Card__Specification *spec)
{
	Check_Object(this);
	Check_Object(spec);

	Singleton__Specification::Copy(spec);

	gos_PushCurrentHeap(Heap);
	m_halfHeight = spec->m_halfHeight;
	m_aspectRatio = spec->m_aspectRatio;
	m_index = spec->m_index;
	m_UOffset = spec->m_UOffset;
	m_VOffset = spec->m_VOffset;
	m_USize = spec->m_USize;
	m_VSize = spec->m_VSize;
	m_animated = spec->m_animated;
	m_width = spec->m_width;
	gos_PopCurrentHeap();
}

//------------------------------------------------------------------------------
//
void
	gosFX::Card__Specification::SetWidth()
{
	m_width =
		static_cast<BYTE>(1.0f / m_USize.ComputeValue(0.0f, 0.0f));
}

//############################################################################
//##############################  gosFX::Card  ################################
//############################################################################

gosFX::Card::ClassData*
	gosFX::Card::DefaultData = NULL;

//------------------------------------------------------------------------------
//
void
	gosFX::Card::InitializeClass()
{
	Verify(!DefaultData);
	Verify(gos_GetCurrentHeap() == Heap);
	DefaultData =
		new ClassData(
			CardClassID,
			"gosFX::Card",
			Singleton::DefaultData,
			(Effect::Factory)&Make,
			(Specification::Factory)&Specification::Make
		);
	Register_Object(DefaultData);
}

//------------------------------------------------------------------------------
//
void
	gosFX::Card::TerminateClass()
{
	Unregister_Object(DefaultData);
	delete DefaultData;
	DefaultData = NULL;
}

//------------------------------------------------------------------------------
//
gosFX::Card::Card(
	Specification *spec,
	unsigned flags
):
	Singleton(DefaultData, spec, flags),
	m_cardCount(1)
{
	Check_Object(spec);
	Verify(gos_GetCurrentHeap() == Heap);

	gos_PushCurrentHeap(MidLevelRenderer::Heap);
	m_cardCloud = new MidLevelRenderer::MLRCardCloud(1);
	Register_Object(m_cardCloud);
	gos_PopCurrentHeap();

	m_cardCloud->SetData(
		&m_cardCount,
		m_vertices,
		m_colors,
		m_uvs
	);
}

//------------------------------------------------------------------------------
//
gosFX::Card::~Card()
{
	Unregister_Object(m_cardCloud);
	delete m_cardCloud;
}

//------------------------------------------------------------------------------
//
gosFX::Card*
	gosFX::Card::Make(
		Specification *spec,
		unsigned flags
	)
{
	Check_Object(spec);

	gos_PushCurrentHeap(Heap);
	Card *cloud = new gosFX::Card(spec, flags);
	gos_PopCurrentHeap();

	return cloud;
}

//------------------------------------------------------------------------------
//
void
	gosFX::Card::Start(ExecuteInfo *info)
{
	Check_Object(this);
	Check_Object(info);
	Singleton::Start(info);

	Specification *spec = GetSpecification();
	Check_Object(spec);
	m_halfY = spec->m_halfHeight.ComputeValue(m_age, m_seed);
	m_halfX = m_halfY * spec->m_aspectRatio.ComputeValue(m_age, m_seed);
	m_radius = Stuff::Sqrt(m_halfX * m_halfX + m_halfY * m_halfY);
	m_cardCloud->TurnOn(0);
}

//------------------------------------------------------------------------------
//
bool
	gosFX::Card::Execute(ExecuteInfo *info)
{
	Check_Object(this);
	Check_Object(info);

	if (!IsExecuted())
		return false;

	//
	//------------------------
	// Do the effect animation
	//------------------------
	//
	if (!Singleton::Execute(info))
		return false;

	//
	//-----------------------------------------
	// Animate the parent then get our pointers
	//-----------------------------------------
	//
	Set_Statistic(Card_Count, Card_Count+1);
	Specification *spec = GetSpecification();
	Check_Object(spec);

	//
	//----------------
	// Animate the uvs
	//----------------
	//
	Stuff::Scalar u = spec->m_UOffset.ComputeValue(m_age, m_seed);
	Stuff::Scalar v = spec->m_VOffset.ComputeValue(m_age, m_seed);
	Stuff::Scalar u2 = spec->m_USize.ComputeValue(m_age, m_seed);
	Stuff::Scalar v2 = spec->m_VSize.ComputeValue(m_age, m_seed);

	//
	//--------------------------------------------------------------
	// If we are animated, figure out the row/column to be displayed
	//--------------------------------------------------------------
	//
	if (spec->m_animated)
	{
		BYTE columns =
			Stuff::Truncate_Float_To_Byte(
				spec->m_index.ComputeValue(m_age, m_seed)
			);
		BYTE rows = static_cast<BYTE>(columns / spec->m_width);
		columns = static_cast<BYTE>(columns - rows*spec->m_width);

		//
		//---------------------------
		// Now compute the end points
		//---------------------------
		//
		u += u2*columns;
		v += v2*rows;
	}
	u2 += u;
	v2 += v;

	m_uvs[0].x = u;
	m_uvs[0].y = v2;
	m_uvs[1].x = u2;
	m_uvs[1].y = v2;
	m_uvs[2].x = u2;
	m_uvs[2].y = v;
	m_uvs[3].x = u;
	m_uvs[3].y = v;

	//
	//------------------
	// Fill in the color
	//------------------
	//
	m_colors[0] = m_color;
	m_colors[1] = m_color;
	m_colors[2] = m_color;
	m_colors[3] = m_color;

	//
	//---------------------
	// Fill in the position
	//---------------------
	//
	m_vertices[0].x = m_scale*m_halfX;
	m_vertices[0].y = -m_scale*m_halfY;
	m_vertices[0].z = 0.0f;

	m_vertices[1].x = -m_scale*m_halfX;
	m_vertices[1].y = -m_scale*m_halfY;
	m_vertices[1].z = 0.0f;

	m_vertices[2].x = -m_scale*m_halfX;
	m_vertices[2].y = m_scale*m_halfY;
	m_vertices[2].z = 0.0f;

	m_vertices[3].x = m_scale*m_halfX;
	m_vertices[3].y = m_scale*m_halfY;
	m_vertices[3].z = 0.0f;

	return true;
}

//------------------------------------------------------------------------------
//
void
	gosFX::Card::Kill()
{
	Check_Object(this);

	m_cardCloud->TurnOff(0);
	Singleton::Kill();
}

//------------------------------------------------------------------------------
//
void gosFX::Card::Draw(DrawInfo *info)
{
	Check_Object(this);
	Check_Object(info);

	//
	//----------------------------
	// Set up the common draw info
	//----------------------------
	//
	MidLevelRenderer::DrawEffectInformation dInfo;
	dInfo.effect = m_cardCloud;
	Specification *spec = GetSpecification();
	Check_Object(spec);
	dInfo.state.Combine(info->m_state, spec->m_state);
	dInfo.clippingFlags = info->m_clippingFlags;
	Stuff::LinearMatrix4D local_to_world;
	local_to_world.Multiply(m_localToParent, *info->m_parentToWorld);
	dInfo.effectToWorld = &local_to_world;

	//
	//--------------------------------------------------------------
	// Check the orientation mode.  The first case is XY orientation
	//--------------------------------------------------------------
	//
	if (spec->m_alignZUsingX)
	{
		Stuff::Point3D
			camera_in_world(info->m_clipper->GetCameraToWorldMatrix());
		Stuff::Point3D card_in_world(local_to_world);
		Stuff::Vector3D look_at;
		look_at.Subtract(camera_in_world, card_in_world);
		if (spec->m_alignZUsingY)
			local_to_world.AlignLocalAxisToWorldVector(
				look_at,
				Stuff::Z_Axis,
				Stuff::Y_Axis,
				Stuff::X_Axis
			);
		else
			local_to_world.AlignLocalAxisToWorldVector(
				look_at,
				Stuff::Z_Axis,
				Stuff::X_Axis,
				-1
			);
	}

	//
	//-------------------------------------------------------
	// Each matrix needs to be aligned to the camera around Y
	//-------------------------------------------------------
	//
	else if (spec->m_alignZUsingY)
	{
		Stuff::Point3D
			camera_in_world(info->m_clipper->GetCameraToWorldMatrix());
		Stuff::Point3D card_in_world(local_to_world);
		Stuff::Vector3D look_at;
		look_at.Subtract(camera_in_world, card_in_world);
		local_to_world.AlignLocalAxisToWorldVector(
			look_at,
			Stuff::Z_Axis,
			Stuff::Y_Axis,
			-1
		);
	}

	//
	//---------------------
	// Now just do the draw
	//---------------------
	//
	info->m_clipper->DrawEffect(&dInfo);
	Singleton::Draw(info);
}

//------------------------------------------------------------------------------
//
void
	gosFX::Card::TestInstance() const
{
	Verify(IsDerivedFrom(DefaultData));
}
