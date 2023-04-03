//===========================================================================//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#pragma once

#ifndef MLR_GOSVERTEX_HPP
#define MLR_GOSVERTEX_HPP

#include "stuff/vector2d.h"
#include "stuff/vector4d.h"
#include "stuff/color.h"
#include "mlr/mlr.h"
#include "mlr/mlrstate.h"

#ifndef _GAMEOS_HPP_
struct gos_VERTEX
{
	float x, y; // Screen coords    - must be 0.0 to Environment.screenwidth/height (no clipping occurs unless gos_State_Clipping is true)
	float z; // 0.0 to 0.99999   - Used for visibility check in zbuffer (1.0 is not valid)
	float rhw; // 0.0 to 1.0       - reciprocal of homogeneous w - Used for perspective correct textures, fog and clipping
	uint32_t argb; // Vertex color and alpha (alpha of 255 means solid, 0=transparent)
	uint32_t frgb; // Specular color and fog
	float u, v; // Texture coordinates
};
typedef gos_VERTEX* pgos_VERTEX;
#endif

ID3D11Buffer buf1;
ID3D12Buffer buf2;

namespace Stuff
{
class Vector4D;
class RGBAcolour;
class Point3D;
class Matrix4D;
} // namespace Stuff

namespace MidLevelRenderer
{

//##########################################################################
//####################    GOSVertex    ##############################
//##########################################################################

class GOSVertex : public gos_VERTEX
{
public:
	GOSVertex(void);

	static float farClipReciprocal;

	inline GOSVertex& operator=(const GOSVertex& V)
	{
		// Check_Pointer(this);
		x = V.x;
		y = V.y;
		z = V.z;
		rhw = V.rhw;
		argb = V.argb;
		frgb = V.frgb;
		u = V.u;
		v = V.v;
		return *this;
	};

	inline GOSVertex& operator=(const Stuff::Vector4D& v4d)
	{
		// Check_Pointer(this);
		_ASSERT(!Stuff::Small_Enough(v4d.w));
		//					Tell_Value(v);
		rhw = 1.0f / v4d.w;
		x = v4d.x * rhw;
		_ASSERT(x >= 0.0f && x <= 1.0f);
		y = v4d.y * rhw;
		_ASSERT(y >= 0.0f && y <= 1.0f);
		z = v4d.z * rhw;
		_ASSERT(z >= 0.0f && z < 1.0f);
		return *this;
	}

	inline GOSVertex& operator=(const Stuff::RGBAcolour& c)
	{
		// Check_Pointer(this);
		//					DEBUG_STREAM << "c = <" << c.alpha << ", " << c.red << ",
		//";
		//					DEBUG_STREAM << c.green << ", " << c.blue << ">" <<
		// endl;
		float f;
		f = c.alpha * 255.99f;
		Clamp(f, 0.0f, 255.f);
		argb = Stuff::Round_Float_To_Byte(f);
		f = c.red * 255.99f;
		Clamp(f, 0.0f, 255.f);
		argb = (argb << 8) | Stuff::Round_Float_To_Byte(f);
		f = c.green * 255.99f;
		Clamp(f, 0.0f, 255.f);
		argb = (argb << 8) | Stuff::Round_Float_To_Byte(f);
		f = c.blue * 255.99f;
		Clamp(f, 0.0f, 255.f);
		argb = (argb << 8) | Stuff::Round_Float_To_Byte(f);
		//					DEBUG_STREAM << "0x" << hex << argb << dec << endl;
		return *this;
	}

	inline GOSVertex& operator=(const uint32_t c)
	{
		// Check_Pointer(this);
		argb = c;
		return *this;
	}

	inline GOSVertex& operator=(const Stuff::Vector2DOf<float>& uv)
	{
		// Check_Pointer(this);
		u = uv[0];
		v = uv[1];
		return *this;
	}

	inline void GOSTransformNoClip(const Stuff::Point3D& v, const Stuff::Matrix4D& m, float* uv
#if FOG_HACK
		,
		uint32_t foggy
#endif
	);

#if FOG_HACK
	static uint8_t fogTable[Limits::Max_Number_Of_FogStates][1024];
	static void SetFogTableEntry(
		int32_t entry, float fogNearClip, float fogFarClip, float fogDensity);
#endif

protected:
};

struct ViewportScalars
{
	static float MulX;
	static float MulY;
	static float AddX;
	static float AddY;
};

// typedef Stuff::Vector2DOf<float> Vector2DScalar;

const float float_cheat = 12582912.0f / 256.0f;
const float One_Over_256 = 1.0f / 256.0f;

//#pragma warning (disable : 4725)

void GOSVertex::GOSTransformNoClip(const Stuff::Point3D& _v, const Stuff::Matrix4D& m, float* uv
#if FOG_HACK
	,
	uint32_t foggy
#endif
)

{
	// Check_Pointer(this);
	Check_Object(&_v);
	Check_Object(&m);
#if USE_INLINE_ASSEMBLER_CODE
	float* f = &x;
	_asm
		{
			mov         edx, m
			mov         eax, _v

			fld         dword ptr [eax] //  v.x
			fld         dword ptr [eax+4] //  v.y
			fld         dword ptr [eax+8] //  v.z

			mov         eax, f

			fld         dword ptr [edx+34h] //  m[1][3]
			fmul        st, st(2) //  v.y

			fld         dword ptr [edx + 38h] //  m[2][3]
			fmul        st, st(2) //  v.z

			fxch        st(1)
			fadd        dword ptr [edx + 3Ch] //  m[3][3]

			fld         dword ptr [edx + 30h] //  m[0][3]
			fmul        st, st(5) //  v.x

			fxch        st(2)
			faddp       st(1), st

			fld         dword ptr [edx + 14h] //  m[1][1]
			fmul        st, st(4) //  v.y

			fxch        st(2)
			faddp       st(1), st

			fld         dword ptr [edx + 18h] //  m[2][1]
			fmul        st, st(3) //  v.z

			fxch        st(1)
			fstp        dword ptr [eax + 0Ch] //  w

			fadd        dword ptr [edx + 1Ch] //  m[3][1]

			fld         dword ptr [edx + 10h] //  m[0][1]
			fmul        st, st(5) //  v.x

			fxch        st(2)
			faddp       st(1), st

			fld         dword ptr [edx + 24h] //  m[1][2]
			fmul        st, st(4) //  v.y

			fxch        st(2)
			faddp       st(1), st

			fld         dword ptr [edx + 28h] //  m[2][2]
			fmul        st, st(3) //  v.z

			fxch        st(1)
			fstp        dword ptr [eax + 4] //  y

			fadd        dword ptr [edx + 2Ch] //  m[3][2]

			fld         dword ptr [edx + 20h] //  m[0][2]
			fmul        st, st(5) //  v.x

			fxch        st(2)
			faddp       st(1), st

			fld         dword ptr [edx + 4] //  m[1][0]
			fmul        st, st(4) //  v.y

			fxch        st(2)
			faddp       st(1), st

			fld         dword ptr [edx + 8] //  m[2][0]
			fmul        st, st(3) //  v.z

			fxch        st(1)
			fstp        dword ptr [eax + 8] //  z

			fadd        dword ptr [edx + 0Ch] //  m[3][0]

			fld         dword ptr [edx] //  m[0][0]
			fmul        st, st(5) //  v.x

			fxch        st(2)
			faddp       st(1), st

			faddp       st(1), st

				//      fld1

				//      fdivrp      st(1),st

				//  get rid of x, y, z
			fstp        st(1)
			fstp        st(1)
			fstp        st(1)

			fstp        dword ptr [eax] //  x

		}
#else
	x = _v.x * m(0, 0) + _v.y * m(1, 0) + _v.z * m(2, 0) + m(3, 0);
	y = _v.x * m(0, 1) + _v.y * m(1, 1) + _v.z * m(2, 1) + m(3, 1);
	z = _v.x * m(0, 2) + _v.y * m(1, 2) + _v.z * m(2, 2) + m(3, 2);
	rhw = _v.x * m(0, 3) + _v.y * m(1, 3) + _v.z * m(2, 3) + m(3, 3);
#endif
#if 0 // USE_INLINE_ASSEMBLER_CODE
		_asm
		{
			;
			gos_vertices[0].w = 1.0f/coords[offset].w;

			mov         ecx, f
			fld1

			fdiv        dword ptr [ecx+0Ch]

			fst         dword ptr [ecx+0Ch]

			;
			gos_vertices[0].x = coords[offset].x * gos_vertices[0].w;
			fld         st(0)
			fmul        dword ptr [ecx]
			fstp        dword ptr [ecx]

			;
			gos_vertices[0].y = coords[offset].y * gos_vertices[0].w;
			fld         dword ptr [ecx + 4]
			fmul        st, st(1)
			fstp        dword ptr [ecx + 4]
			;
			gos_vertices[0].z = coords[offset].z * gos_vertices[0].w;

			;
			fld         dword ptr [ecx + 8]
			fmul        dword ptr [ecx + 8]

			fstp        dword ptr [ecx + 8]
			;
			fstp        st(0)
		}
#else
#if FOG_HACK
	if (foggy)
	{
		*((uint8_t*)&frgb + 3) = fogTable[foggy - 1][Stuff::Truncate_Float_To_Word(rhw)];
	}
	else
	{
		*((uint8_t*)&frgb + 3) = 0xff;
	}
#endif
	rhw = 1.0f / rhw;
	_ASSERT(MLRState::GetHasMaxUVs()
			? (uv[0] < MLRState::GetMaxUV() && uv[0] > -MLRState::GetMaxUV())
			: 1);
	_ASSERT(MLRState::GetHasMaxUVs()
			? (uv[1] < MLRState::GetMaxUV() && uv[1] > -MLRState::GetMaxUV())
			: 1);
	u = uv[0];
	v = uv[1];
	x = x * rhw;
	y = y * rhw;
	z = z * rhw;
#endif
	_ASSERT(rhw > Stuff::SMALL);
	_ASSERT(x >= 0.0f);
	_ASSERT(y >= 0.0f);
	_ASSERT(z >= 0.0f);
	_ASSERT(x <= 1.0f);
	_ASSERT(y <= 1.0f);
	_ASSERT(z < 1.0f);
	x = x * ViewportScalars::MulX + ViewportScalars::AddX;
	y = y * ViewportScalars::MulY + ViewportScalars::AddY;
}

//	create a dword color out of 4 rgba floats
inline uint32_t
GOSCopycolour(const Stuff::RGBAcolour* color)
{
	float f;
	uint32_t argb = 0;
#if USE_INLINE_ASSEMBLER_CODE
	_asm
	{
			fld     float_cheat

			mov     esi, dword ptr [color]

			fld     dword ptr [esi + 0Ch]

			mov     ecx, dword ptr [esi + 0Ch]

			fadd    st, st(1)

			rcl     ecx, 1
			sbb     eax, eax
			xor     eax, -1

			fstp    f
			xor     ecx, ecx

			mov     ebx, f
			and     ebx, eax

			test    ebx, 0000ff00h

			seta    cl

			xor     eax, eax
			sub     eax, ecx

			or      ebx, eax
			and     ebx, 000000ffh

			mov     argb, ebx

			fld     dword ptr [esi]

			mov     ecx, dword ptr [esi]

			fadd    st, st(1)
			rcl     ecx, 1
			sbb     eax, eax
			xor     eax, -1

			fstp    f
			xor     ecx, ecx

			mov     ebx, f
			and     ebx, eax

			test    ebx, 0000ff00h

			seta    cl

			xor     eax, eax
			sub     eax, ecx

			or      ebx, eax
			and     ebx, 000000ffh

			mov     ecx, argb
			shl     ecx, 8

			or      ecx, ebx
			mov     argb, ecx

			fld     dword ptr [esi + 4]

			mov     ecx, dword ptr [esi + 4]

			fadd    st, st(1)
			rcl     ecx, 1
			sbb     eax, eax
			xor     eax, -1

			fstp    f
			xor     ecx, ecx

			mov     ebx, f
			and     ebx, eax

			test    ebx, 0000ff00h

			seta    cl

			xor     eax, eax
			sub     eax, ecx

			or      ebx, eax
			and     ebx, 000000ffh

			mov     ecx, argb
			shl     ecx, 8

			or      ecx, ebx
			mov     argb, ecx

			;
			fld     dword ptr [esi + 8]

			mov     ecx, dword ptr [esi + 8]

			fadd    dword ptr [esi + 8]
			rcl     ecx, 1
			sbb     eax, eax
			xor     eax, -1

			fstp    f
			xor     ecx, ecx

			mov     ebx, f
			and     ebx, eax

			test    ebx, 0000ff00h

			seta    cl

			xor     eax, eax
			sub     eax, ecx

			or      ebx, eax
			and     ebx, 000000ffh

			mov     ecx, argb
			shl     ecx, 8

			or      ecx, ebx
			mov     argb, ecx
	}
#else
	// !!! this code was left in broken state !!!
	// Positive_Float_To_Byte(), colors and gos_vertices variables missing
	// GOSVertex* gos_vertices;

#if _CONSIDERED_UNSUPPORTED
	f = color->alpha * 255.99f;
	Clamp(f, 0.0f, 255.f);
	argb = Stuff::Positive_Float_To_Byte(f);
	f = color->red * 255.99f;
	Clamp(f, 0.0f, 255.f);
	argb = (gos_vertices[0].argb << 8) | Stuff::Positive_Float_To_Byte(f);
	f = color->green * 255.99f;
	Clamp(f, 0.0f, 255.f);
	argb = (gos_vertices[0].argb << 8) | Stuff::Positive_Float_To_Byte(f);
	f = color->blue * 255.99f;
	Clamp(f, 0.0f, 255.f);
	argb = (gos_vertices[0].argb << 8) | Stuff::Positive_Float_To_Byte(f);
#else
	(void)color;
	(void)f;
	(void)argb;
#pragma message("this code was left in broken state")
#endif
#endif
	return argb;
}

inline uint32_t
colour_DWORD_Lerp(uint32_t _from, uint32_t _to, float _lerp)
{
	Stuff::RGBAcolour from, to, lerp;
	from.blue = (_from & 0xff) * One_Over_256;
	_from = _from >> 8;
	from.green = (_from & 0xff) * One_Over_256;
	_from = _from >> 8;
	from.red = (_from & 0xff) * One_Over_256;
	_from = _from >> 8;
	from.alpha = (_from & 0xff) * One_Over_256;
	// ====
	to.blue = (_to & 0xff) * One_Over_256;
	_to = _to >> 8;
	to.green = (_to & 0xff) * One_Over_256;
	_to = _to >> 8;
	to.red = (_to & 0xff) * One_Over_256;
	_to = _to >> 8;
	to.alpha = (_to & 0xff) * One_Over_256;
	lerp.Lerp(from, to, _lerp);
	return GOSCopycolour(&lerp);
}

//######################################################################################################################
//	the lines below will produce following functions:
//
//	bool GOSCopyData(GOSVertex*, const Stuff::Vector4D*, int32_t);
//	bool GOSCopyData(GOSVertex*, const Stuff::Vector4D*, const uint32_t* , int32_t);
//	bool GOSCopyData(GOSVertex*, const Stuff::Vector4D*, const RGBAcolour*,
// int32_t); 	bool GOSCopyData(GOSVertex*, const Stuff::Vector4D*, const
// Vector2DScalar*, int32_t); 	bool GOSCopyData(GOSVertex*, const
// Stuff::Vector4D*, const uint32_t* , const Vector2DScalar*, int32_t); 	bool
// GOSCopyData(GOSVertex*, const Stuff::Vector4D*, const RGBAcolour*, const
// Vector2DScalar*, int32_t);
//
//	bool GOSCopyTriangleData(GOSVertex*, const Stuff::Vector4D*, int32_t,
// int32_t, int32_t); 	bool GOSCopyTriangleData(GOSVertex*, const
// Stuff::Vector4D*, const uint32_t* , int32_t, int32_t, int32_t); 	bool
// GOSCopyTriangleData(GOSVertex*, const Stuff::Vector4D*, const RGBAcolour*,
// int32_t, int32_t, int32_t); 	bool GOSCopyTriangleData(GOSVertex*, const
// Stuff::Vector4D*, const Vector2DScalar*, int32_t, int32_t, int32_t); 	bool
// GOSCopyTriangleData(GOSVertex*, const Stuff::Vector4D*, const uint32_t* , const
// Vector2DScalar*, int32_t, int32_t, int32_t); 	bool
// GOSCopyTriangleData(GOSVertex*, const Stuff::Vector4D*, const RGBAcolour*,
// const Vector2DScalar*, int32_t, int32_t, int32_t);
//#######################################################################################################################

// aaargh => create template?

#define I_SAY_YES_TO_COLOR
#define I_SAY_YES_TO_TEXTURE
#define I_SAY_YES_TO_DWORD_COLOR
#include "mlr/gosvertexmanipulation.inl"

#undef I_SAY_YES_TO_DWORD_COLOR
#include "mlr/gosvertexmanipulation.inl"

#undef I_SAY_YES_TO_COLOR
#include "mlr/gosvertexmanipulation.inl"

#define I_SAY_YES_TO_COLOR
#undef I_SAY_YES_TO_TEXTURE
#include "mlr/gosvertexmanipulation.inl"

#define I_SAY_YES_TO_DWORD_COLOR
#include "mlr/gosvertexmanipulation.inl"

#undef I_SAY_YES_TO_COLOR
#include "mlr/gosvertexmanipulation.inl"

// #define MLR_GOSVERTEXMANIPULATION_HPP
} // namespace MidLevelRenderer
#endif
