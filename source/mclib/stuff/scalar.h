//===========================================================================//
// File:	scalar.hpp                                                       //
// Contents: Base information used by all MUNGA source files                 //
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#pragma once

#ifndef _SCALAR_HPP_
#define _SCALAR_HPP_

#include "stuff/style.h"
// #include "stuff/memorystream.h"

namespace Stuff
{

#if !defined(M_PI)
#define M_PI 3.14159265358979323846
#endif

typedef float Scalar;
const float Pi = static_cast<float>(M_PI);
const float Pi_Over_2 = static_cast<float>(M_PI / 2.0);
const float Pi_Over_3 = static_cast<float>(M_PI / 3.0);
const float Pi_Over_4 = static_cast<float>(M_PI / 4.0);
const float Pi_Over_6 = static_cast<float>(M_PI / 6.0);
const float Pi_Over_8 = static_cast<float>(M_PI / 8.0);
const float Two_Pi = static_cast<float>(2.0 * M_PI);
const float Degrees_Per_Radian = static_cast<float>(180.0 / M_PI);
const float Radians_Per_Degree = static_cast<float>(M_PI / 180.0);
const int32_t FP_ONE_BITS = 0x3F800000;

inline float
Fabs(float a)
{
	return static_cast<float>(fabs(a));
}
inline float
Lerp(float a, float b, float t)
{
	return a * (1.0f - t) + b * t;
}
inline bool
Small_Enough(float x, float e = SMALL)
{
	return Fabs(x) <= e;
}
inline bool
Close_Enough(float x, float y, float e = SMALL)
{
	return Fabs(x - y) <= e;
}
inline bool
Close_Enough(int32_t x, int32_t y, float e = SMALL)
{
	return Fabs(static_cast<float>(x - y)) <= e;
}

inline uint16_t
Round_Float_To_Word(float val)
{
	_ASSERT(val >= 0.0f && val < 65536.0f);
	val += 12582912.0f;
	return *Cast_Pointer(uint16_t*, &val);
}

inline uint8_t
Round_Float_To_Byte(float val)
{
	_ASSERT(val >= 0.0f && val < 256.0f);
	val += 12582912.0f;
	return *Cast_Pointer(uint8_t*, &val);
}

inline uint16_t
Truncate_Float_To_Word(float val)
{
	_ASSERT(val >= 0.0f && val < 65536.0f);
	val -= 0.5f;
	val += 12582912.0f;
	return *Cast_Pointer(uint16_t*, &val);
}

inline uint8_t
Truncate_Float_To_Byte(float val)
{
	_ASSERT(val >= 0.0f && val < 256.0f);
	val -= 0.5f;
	val += 12582912.0f;
	return *Cast_Pointer(uint8_t*, &val);
}

uint32_t
Scaled_Float_To_Bits(float val, float min, float max, uint32_t number_of_bits);
float
Scaled_Float_From_Bits(uint32_t val, float min, float max, uint32_t number_of_bits);
uint32_t
Scaled_Int_To_Bits(int32_t val, int32_t min, int32_t max, uint32_t number_of_bits);
int32_t
Scaled_Int_From_Bits(uint32_t val, int32_t min, int32_t max, uint32_t number_of_bits);
int32_t
Round(float value);

// mg: I made some statistic test and came up with an max error of 6%
static inline float
SqrtApproximate(float f)
{
	_ASSERT(f >= 0.0f);
#if USE_INLINE_ASSEMBLER_CODE
	float temp;
	_asm
	{
		// __int32 i = (AsInt32(f) >> 1) + (INT32_FLOAT_ONE >> 1);
			mov         eax, f
			sar         eax, 1
			add         eax, 1FC00000h
			mov			temp, eax
	}
	return temp;
#else
	return static_cast<float>(sqrt(f));
#endif
}

static inline float
OneOverApproximate(float f)
{
	_ASSERT(f != 0.0f);
#if USE_INLINE_ASSEMBLER_CODE
	float temp;
	int32_t _i = 2 * FP_ONE_BITS - *(int32_t*) & (f);
	temp = *(float*)&_i;
	temp = temp * (2.0f - (f)*temp);
	return temp;
#else
	return 1.0f / f;
#endif
}

void
Find_Roots(float a, // a*x*x + b*x + c = 0
	float b, float c, float* center, float* range);

inline float
Sqrt(float value)
{
	_ASSERT(value >= 0.0f);
	return static_cast<float>(sqrt(value));
}

inline float
Arctan(float y, float x)
{
	_ASSERT(!Small_Enough(y) || !Small_Enough(x));
	return static_cast<float>(atan2(y, x));
}

inline float
Arccos(float x)
{
	_ASSERT(x >= -1.0f && x <= 1.0f);
	return static_cast<float>(acos(x));
}

inline float
Arcsin(float x)
{
	_ASSERT(x >= -1.0f && x <= 1.0f);
	return static_cast<float>(asin(x));
}

inline float
Power(float x, float y)
{
	_ASSERT(x >= 0);
	return static_cast<float>(pow(static_cast<double>(x), static_cast<double>(y)));
}

inline float
Exp(float v)
{
	return static_cast<float>(exp(v));
}

inline float
Sin(float v)
{
	return static_cast<float>(sin(v));
}

inline float
Cos(float v)
{
	return static_cast<float>(cos(v));
}

inline float
Tan(float v)
{
	return static_cast<float>(tan(v));
}

inline float
AtoF(const std::wstring_view& v)
{
	return static_cast<float>(atof(v));
}

typedef double Time;
} // namespace Stuff

namespace MemoryStreamIO
{
#if CONSIDERED_DISABLED
inline std::istream&
Read(std::istream& stream, float* output)
{
	return stream.read(output, sizeof(*output));
}
inline std::ostream&
Write(std::ostream& stream, const float* input)
{
	return stream.write(input, sizeof(*input));
}
#endif
} // namespace MemoryStreamIO
#endif
