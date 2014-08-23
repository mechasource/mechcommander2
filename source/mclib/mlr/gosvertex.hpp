//===========================================================================//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#pragma once

#ifndef MLR_GOSVERTEX_HPP
#define MLR_GOSVERTEX_HPP

#include <stuff/vector2d.hpp>
#include <stuff/vector4d.hpp>
#include <stuff/color.hpp>
#include <mlr/mlr.hpp>
#include <mlr/mlrstate.hpp>

namespace Stuff {
	class Vector4D;
	class RGBAColor;
	class Point3D;
	class Matrix4D;
}

namespace MidLevelRenderer {

	//##########################################################################
	//####################    GOSVertex    ##############################
	//##########################################################################

	class GOSVertex :
		public gos_VERTEX
	{
	public:
		GOSVertex();

		static float
			farClipReciprocal;

		inline GOSVertex&
			operator=(const GOSVertex& V)
		{
			Check_Pointer(this);

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

		inline GOSVertex&
			operator=(const Stuff::Vector4D& v)
		{
			Check_Pointer(this);

			Verify(!Stuff::Small_Enough(v.w));

			//					Tell_Value(v);

			rhw = 1.0f / v.w;

			x = v.x * rhw;
			Verify(x>=0.0f && x<=1.0f);

			y = v.y * rhw;
			Verify(y>=0.0f && y<=1.0f);

			z = v.z * rhw;
			Verify(z>=0.0f && z<1.0f);

			return *this;
		}

		inline GOSVertex&
			operator=(const Stuff::RGBAColor& c)
		{
			Check_Pointer(this);

			//					DEBUG_STREAM << "c = <" << c.alpha << ", " << c.red << ", ";
			//					DEBUG_STREAM << c.green << ", " << c.blue << ">" << endl;

			float f;
			f = c.alpha * 255.99f;
			Clamp(f, 0.0f, 255.f);
			argb = Stuff::Round_Float_To_Byte (f);

			f = c.red * 255.99f;
			Clamp(f, 0.0f, 255.f);
			argb = (argb << 8) | Stuff::Round_Float_To_Byte (f);

			f = c.green * 255.99f;
			Clamp(f, 0.0f, 255.f);
			argb = (argb << 8) | Stuff::Round_Float_To_Byte (f);

			f = c.blue * 255.99f;
			Clamp(f, 0.0f, 255.f);
			argb = (argb << 8) | Stuff::Round_Float_To_Byte (f);

			//					DEBUG_STREAM << "0x" << hex << argb << dec << endl;

			return *this;
		}

		inline GOSVertex&
			operator=(const ULONG c)
		{
			Check_Pointer(this);

			argb = c;

			return *this;
		}

		inline GOSVertex&
			operator=(const Stuff::Vector2DOf<float>& uv)
		{
			Check_Pointer(this);

			u = uv[0];
			v = uv[1];

			return *this;
		}

		inline void
			GOSTransformNoClip(
			const Stuff::Point3D& v,
			const Stuff::Matrix4D& m,
			float *uv
#if FOG_HACK
			, int foggy
#endif
			);

#if FOG_HACK
		static UCHAR	GOSVertex::fogTable[Limits::Max_Number_Of_FogStates][1024];
		static void		SetFogTableEntry(int entry, float fogNearClip, float fogFarClip, float fogDensity);
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

	//typedef Stuff::Vector2DOf<float> Vector2DScalar;

	const float float_cheat = 12582912.0f/256.0f;
	const float One_Over_256 = 1.0f/256.0f;

	//#pragma warning (disable : 4725)

	void
		GOSVertex::GOSTransformNoClip(
		const Stuff::Point3D &_v,
		const Stuff::Matrix4D &m,
		float *uv
#if FOG_HACK
		, int foggy
#endif
		)

	{
		Check_Pointer(this);
		Check_Object(&_v);
		Check_Object(&m);

#if USE_ASSEMBLER_CODE
		float *f = &x;
		_asm {
			mov         edx, m
			mov         eax, _v

			fld         dword ptr [eax]         //  v.x
			fld         dword ptr [eax+4]       //  v.y
			fld         dword ptr [eax+8]       //  v.z

			mov         eax, f

			fld         dword ptr [edx+34h]     //  m[1][3]
			fmul        st, st(2)               //  v.y

			fld         dword ptr [edx+38h]     //  m[2][3]
			fmul        st, st(2)               //  v.z

			fxch        st(1)
			fadd        dword ptr [edx+3Ch]     //  m[3][3]

			fld         dword ptr [edx+30h]     //  m[0][3]
			fmul        st, st(5)               //  v.x

			fxch        st(2)
			faddp       st(1),st

			fld         dword ptr [edx+14h]     //  m[1][1]
			fmul        st, st(4)               //  v.y

			fxch        st(2)
			faddp       st(1),st

			fld         dword ptr [edx+18h]     //  m[2][1]
			fmul        st, st(3)               //  v.z

			fxch        st(1)
			fstp        dword ptr [eax+0Ch]     //  w

			fadd        dword ptr [edx+1Ch]     //  m[3][1]

			fld         dword ptr [edx+10h]     //  m[0][1]
			fmul        st, st(5)               //  v.x

			fxch        st(2)
			faddp       st(1),st

			fld         dword ptr [edx+24h]     //  m[1][2]
			fmul        st, st(4)               //  v.y

			fxch        st(2)
			faddp       st(1),st

			fld         dword ptr [edx+28h]     //  m[2][2]
			fmul        st, st(3)               //  v.z

			fxch        st(1)
			fstp        dword ptr [eax+4]       //  y

			fadd        dword ptr [edx+2Ch]     //  m[3][2]

			fld         dword ptr [edx+20h]     //  m[0][2]
			fmul        st, st(5)               //  v.x

			fxch        st(2)
			faddp       st(1),st

			fld         dword ptr [edx+4]       //  m[1][0]
			fmul        st, st(4)               //  v.y

			fxch        st(2)
			faddp       st(1),st

			fld         dword ptr [edx+8]       //  m[2][0]
			fmul        st, st(3)               //  v.z

			fxch        st(1)
			fstp        dword ptr [eax+8]       //  z

			fadd        dword ptr [edx+0Ch]     //  m[3][0]

			fld         dword ptr [edx]         //  m[0][0]
			fmul        st, st(5)               //  v.x

			fxch        st(2)
			faddp       st(1),st

			faddp       st(1),st

	//      fld1

	//      fdivrp      st(1),st

			//  get rid of x, y, z
			fstp        st(1)
			fstp        st(1)
			fstp        st(1)

			fstp        dword ptr [eax]         //  x

		}
#else
		x = _v.x*m(0,0) + _v.y*m(1,0) + _v.z*m(2,0) + m(3,0);
		y = _v.x*m(0,1) + _v.y*m(1,1) + _v.z*m(2,1) + m(3,1);
		z = _v.x*m(0,2) + _v.y*m(1,2) + _v.z*m(2,2) + m(3,2);
		rhw = _v.x*m(0,3) + _v.y*m(1,3) + _v.z*m(2,3) + m(3,3);
#endif

#if 0	//USE_ASSEMBLER_CODE
		_asm {
	;       gos_vertices[0].w = 1.0f/coords[offset].w;

			mov         ecx, f
			fld1

			fdiv        dword ptr [ecx+0Ch]

			fst         dword ptr [ecx+0Ch]

	;       gos_vertices[0].x = coords[offset].x * gos_vertices[0].w;
			fld         st(0)
			fmul        dword ptr [ecx]
			fstp        dword ptr [ecx]

	;       gos_vertices[0].y = coords[offset].y * gos_vertices[0].w;
			fld         dword ptr [ecx+4]
			fmul        st,st(1)
			fstp        dword ptr [ecx+4]
			;       gos_vertices[0].z = coords[offset].z * gos_vertices[0].w;

	;       fld         dword ptr [ecx+8]
			fmul        dword ptr [ecx+8]

			fstp        dword ptr [ecx+8]
	;       fstp        st(0)
		}
#else
#if FOG_HACK
		if(foggy)
		{
			*((puint8_t )&frgb + 3) =
				fogTable[foggy-1][Stuff::Truncate_Float_To_Word(rhw)];
		}
		else
		{
			*((puint8_t )&frgb + 3) = 0xff;
		}
#endif

		rhw = 1.0f/rhw;

		Verify(	MLRState::GetHasMaxUVs() ? (uv[0]<MLRState::GetMaxUV() && uv[0]>-MLRState::GetMaxUV()) : 1 );
		Verify(	MLRState::GetHasMaxUVs() ? (uv[1]<MLRState::GetMaxUV() && uv[1]>-MLRState::GetMaxUV()) : 1 );

		u = uv[0];
		v = uv[1];

		x = x * rhw;
		y = y * rhw;
		z = z * rhw;
#endif

		Verify(rhw > Stuff::SMALL);

		Verify(x >= 0.0f);
		Verify(y >= 0.0f);
		Verify(z >= 0.0f);

		Verify(x <= 1.0f);
		Verify(y <= 1.0f);
		Verify(z < 1.0f);

		x = x*ViewportScalars::MulX + ViewportScalars::AddX;
		y = y*ViewportScalars::MulY + ViewportScalars::AddY;
	}

	//	create a dword color out of 4 rgba floats
	inline ULONG
		GOSCopyColor( const Stuff::RGBAColor *color )
	{
		float f;
		ULONG argb;

#if USE_ASSEMBLER_CODE

		_asm {
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

			fld     dword ptr [esi+4]

			mov     ecx, dword ptr [esi+4]

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

;           fld     dword ptr [esi+8]

			mov     ecx, dword ptr [esi+8]

			fadd    dword ptr [esi+8]
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
		f = color->alpha * 255.99f;
		Clamp(f, 0.0f, 255.f);
		argb = Stuff::Positive_Float_To_Byte (f);

		f = colors->red * 255.99f;
		Clamp(f, 0.0f, 255.f);
		argb = (gos_vertices[0].argb << 8) | Stuff::Positive_Float_To_Byte (f);

		f = colors->green * 255.99f;
		Clamp(f, 0.0f, 255.f);
		argb = (gos_vertices[0].argb << 8) | Stuff::Positive_Float_To_Byte (f);

		f = colors->blue * 255.99f;
		Clamp(f, 0.0f, 255.f);
		argb = (gos_vertices[0].argb << 8) | Stuff::Positive_Float_To_Byte (f);

#endif
		return argb;
	}

	inline ULONG
		Color_DWORD_Lerp (
		ULONG _from,
		ULONG _to,
		float _lerp
		)
	{
		Stuff::RGBAColor from, to, lerp;

		from.blue = (_from & 0xff) * One_Over_256;
		_from = _from>>8;
		from.green = (_from & 0xff) * One_Over_256;
		_from = _from>>8;
		from.red = (_from & 0xff) * One_Over_256;
		_from = _from>>8;
		from.alpha = (_from & 0xff) * One_Over_256;
		// ====
		to.blue = (_to & 0xff) * One_Over_256;
		_to = _to>>8;
		to.green = (_to & 0xff) * One_Over_256;
		_to = _to>>8;
		to.red = (_to & 0xff) * One_Over_256;
		_to = _to>>8;
		to.alpha = (_to & 0xff) * One_Over_256;
		lerp.Lerp(from, to, _lerp);

		return GOSCopyColor(&lerp);
	}

//######################################################################################################################
//	the lines below will produce following functions:
//
//	bool GOSCopyData(GOSVertex*, const Stuff::Vector4D*, int);
//	bool GOSCopyData(GOSVertex*, const Stuff::Vector4D*, const ULONG*, int);
//	bool GOSCopyData(GOSVertex*, const Stuff::Vector4D*, const RGBAColor*, int);
//	bool GOSCopyData(GOSVertex*, const Stuff::Vector4D*, const Vector2DScalar*, int);
//	bool GOSCopyData(GOSVertex*, const Stuff::Vector4D*, const ULONG*, const Vector2DScalar*, int);
//	bool GOSCopyData(GOSVertex*, const Stuff::Vector4D*, const RGBAColor*, const Vector2DScalar*, int);
//
//	bool GOSCopyTriangleData(GOSVertex*, const Stuff::Vector4D*, int, int, int);
//	bool GOSCopyTriangleData(GOSVertex*, const Stuff::Vector4D*, const ULONG*, int, int, int);
//	bool GOSCopyTriangleData(GOSVertex*, const Stuff::Vector4D*, const RGBAColor*, int, int, int);
//	bool GOSCopyTriangleData(GOSVertex*, const Stuff::Vector4D*, const Vector2DScalar*, int, int, int);
//	bool GOSCopyTriangleData(GOSVertex*, const Stuff::Vector4D*, const ULONG*, const Vector2DScalar*, int, int, int);
//	bool GOSCopyTriangleData(GOSVertex*, const Stuff::Vector4D*, const RGBAColor*, const Vector2DScalar*, int, int, int);
//#######################################################################################################################

//aaargh => create template?

#define I_SAY_YES_TO_COLOR
#define I_SAY_YES_TO_TEXTURE
#define I_SAY_YES_TO_DWORD_COLOR
#include <mlr/gosvertexmanipulation.hpp>

#undef I_SAY_YES_TO_DWORD_COLOR
#include <mlr/gosvertexmanipulation.hpp>

#undef I_SAY_YES_TO_COLOR
#include <mlr/gosvertexmanipulation.hpp>

#define I_SAY_YES_TO_COLOR
#undef I_SAY_YES_TO_TEXTURE
#include <mlr/gosvertexmanipulation.hpp>

#define I_SAY_YES_TO_DWORD_COLOR
#include <mlr/gosvertexmanipulation.hpp>

#undef I_SAY_YES_TO_COLOR
#include <mlr/gosvertexmanipulation.hpp>

// #define MLR_GOSVERTEXMANIPULATION_HPP

}
#endif
