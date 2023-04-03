//===========================================================================//
// File:	style.hh                                                         //
// Contents: Base information used by all MUNGA source files                 //
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#pragma once

#ifndef _STYLE_HPP_
#define _STYLE_HPP_

// #include "stuff/stuff.h"

#if defined(_ARMOR)
#include "stuff/armoron.h"
#else
#include "stuff/armoroff.h"
#endif

namespace Stuff
{

extern int32_t ArmorLevel;

enum
{
	SNAN_NEGATIVE_LONG = 0xffb1ffb1
};

#define Register_Pointer(p) Check_Pointer(p)
#define Unregister_Pointer(p) Check_Pointer(p)
#define Register_Object(p) Check_Object(p)
#define Unregister_Object(p) Check_Object(p)

#if defined(USE_TIME_ANALYSIS) || defined(USE_ACTIVE_PROFILE) || defined(USE_TRACE_LOG)
#define TRACE_ENABLED
#endif

#if defined(TRACE_ENABLED) && !defined(LAB_ONLY)
#undef TRACE_ENABLED
#endif

#define DECLARE_TIMER(scope, t) scope int64_t t##FrameTime
#define DEFINE_TIMER(c, t) int64_t c::t##FrameTime

#if defined(LAB_ONLY)
#define Initialize_Timer(t, name)                                \
	do                                                           \
	{                                                            \
		AddStatistic(name, "%", gos_timedata, &t##FrameTime, 0); \
	}                                                            \
	MSSUPPRESS_WARNING(4127)                                     \
	while (0)                                                    \
		;
#define Start_Timer(t)               \
	do                               \
	{                                \
		t##FrameTime -= GetCycles(); \
	}                                \
	MSSUPPRESS_WARNING(4127)         \
	while (0)                        \
		;
#define Stop_Timer(t)                \
	do                               \
	{                                \
		t##FrameTime += GetCycles(); \
	}                                \
	MSSUPPRESS_WARNING(4127)         \
	while (0)                        \
		;
#define Set_Statistic(s, v) (s = v)
#else
#define Initialize_Timer(t, name)
#define Start_Timer(t)
#define Stop_Timer(t)
#define Set_Statistic(s, v)
#endif

void __stdcall Flood_Memory_With_NAN(PVOID where, size_t how_much);

#define Test_Assumption(c)                       \
	while (!(c))                                 \
	{                                            \
		SPEW((GROUP_STUFF_TEST, #c " failed!")); \
		return false;                            \
	}

#define Test_Message(m) SPEW((GROUP_STUFF_TEST, m));

#if defined(_ARMOR)
class MemoryRegister;

class Signature
{
	friend class MemoryRegister;

private:
	enum Mark
	{
		Valid = 0x7B135795L,
		Destroyed = 0x4FED1231L
	} mark;

protected:
	Signature(void)
	{
		mark = Valid;
	}
	~Signature(void)
	{
		mark = Destroyed;
	}

public:
	friend void Is_Signature_Bad(const volatile Signature* p);
};

inline void
Is_Signature_Bad(const volatile void* p)
{
	(void)p;
	Check_Pointer(p);
}
#endif

//##########~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//# MACROS #
//##########

#define ELEMENTS(Array) (sizeof(Array) / sizeof(*Array))

#if defined(__sgi__)
#define _stricmp(s1, s2) (strcasecmp(s1, s2))
#define _strnicmp(s1, s2, n) (strncasecmp(s1, s2, n))
#endif

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Spews
//
#if !defined(Spew)
inline void
Spew(std::wstring_view group, int32_t value)
{
	SPEW((group, "%d+", value));
}

inline void
Spew(std::wstring_view group, float value)
{
	SPEW((group, "%f+", value));
}

inline void
Spew(std::wstring_view group, std::wstring_view value)
{
	SPEW((group, "%s+", value));
}
#endif

//#############~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//# TEMPLATES #
//#############

extern float SMALL;

//#define abs(value) ((value>0) ? value : -value)
#define Clamp(v, f, c) \
	if (v < f)         \
		v = f;         \
	else if (v > c)    \
		v = c;         \
	else               \
		(void)0
#define Max_Clamp(v, c) \
	if (v > c)          \
		v = c;          \
	else                \
		(void)0
#define Is_Many_Bits(value) (Lowbit(value) ^ value)
#define Low_Bit(value) (value & (-value))
#define Min_Clamp(v, f) \
	if (v < f)          \
		v = f;          \
	else                \
		(void)0
#define Max(a, b) ((a > b) ? a : b)
#define Min(a, b) ((a < b) ? a : b)
#define Sgn(value) ((value < -SMALL) ? -1 : value > SMALL)

//############~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//# TYPEDEFS #
//############

enum
{
	INT_BITS = 32
};
} // namespace Stuff

#ifndef __PLACEMENT_NEW_INLINE
#define __PLACEMENT_NEW_INLINE
inline PVOID
operator new(size_t size, PVOID where)
{
	Check_Pointer(where);
	return where;
}
#endif

#endif