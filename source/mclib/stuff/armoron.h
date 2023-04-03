//===========================================================================//
// File:		debug3on.hpp                                                 //
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#pragma once

#ifndef _ARMORON_HPP_
#define _ARMORON_HPP_

#ifndef SPEW
extern "C" void __cdecl InternalFunctionSpew(std::wstring_view group, std::wstring_view message, ...);
#define SPEW(x) InternalFunctionSpew x
#endif

namespace Stuff
{

#if !defined(NO_ANALYSIS)
#define USE_TIME_ANALYSIS
#define USE_TRACE_LOG
#define USE_EVENT_STATISTICS
#endif

#if CONSIDERED_OBSOLETE
#define _ASSERT(c)                         \
	do                                     \
	{                                      \
		if (Stuff::ArmorLevel > 0 && !(c)) \
			PAUSE(("Failed " #c));         \
	}                                      \
	MSSUPPRESS_WARNING(4127)               \
	while (0)
#endif
#define Verify(c) ATLASSERT(c)

#define Warn(c)                           \
	do                                    \
	{                                     \
		if (Stuff::ArmorLevel > 0 && (c)) \
			SPEW((0, #c));                \
	}                                     \
	MSSUPPRESS_WARNING(4127)              \
	while (0)

#if defined(_M_IX86)
#define Check_Pointer(p) _ASSERT((p) && reinterpret_cast<size_t>(p) != Stuff::SNAN_NEGATIVE_LONG)
#else
#define Check_Pointer(p) _ASSERT(p)
#endif

template <class T>
T Cast_Pointer_Function(T p)
{
	if (ArmorLevel > 0)
		Check_Pointer(p);
	return p;
}

#define Cast_Pointer(type, ptr) \
	MSSUPPRESS_WARNING(4946)    \
	Stuff::Cast_Pointer_Function(reinterpret_cast<type>(ptr))

#define Mem_Copy(destination, source, length, available)                                                                            \
	do                                                                                                                              \
	{                                                                                                                               \
		Check_Pointer(destination);                                                                                                 \
		Check_Pointer(source);                                                                                                      \
		_ASSERT((length) <= (available));                                                                                           \
		_ASSERT((size_t)(abs(reinterpret_cast<PSTR>(destination) - reinterpret_cast<std::wstring_view>(source))) >= length); \
		memcpy(destination, source, length);                                                                                        \
	}                                                                                                                               \
	MSSUPPRESS_WARNING(4127)                                                                                                        \
	while (0)

#define Str_Copy(destination, source, available)                            \
	do                                                                      \
	{                                                                       \
		Check_Pointer(destination);                                         \
		Check_Pointer(source);                                              \
		_ASSERT((strlen(source) + 1) <= (available));                       \
		_ASSERT(size_t(abs(destination - source)) >= (strlen(source) + 1)); \
		strcpy_s(destination, available, source);                           \
	}                                                                       \
	MSSUPPRESS_WARNING(4127)                                                \
	while (0)

#define Str_Cat(destination, source, available)                             \
	do                                                                      \
	{                                                                       \
		Check_Pointer(destination);                                         \
		Check_Pointer(source);                                              \
		_ASSERT((strlen(destination) + strlen(source) + 1) <= (available)); \
		strcat(destination, source);                                        \
	}                                                                       \
	MSSUPPRESS_WARNING(4127)                                                \
	while (0)

#define Check_Signature(p) Stuff::Is_Signature_Bad(p)

template <class T>
void Check_Object_Function(T* p)
{
	switch (ArmorLevel)
	{
	case 1:
		Check_Pointer(p);
		break;
	case 2:
		Check_Signature(p);
		break;
	case 3:
	case 4:
		Check_Signature(p);
		p->TestInstance();
		break;
	}
}

#define Check_Object(p) Stuff::Check_Object_Function(p)

//
// Cast_Object will only work for polymorphic objects,
// non-polymorphic objects use Cast_Pointer
//

template <class T>
T Cast_Object_Function(T p)
{
	switch (ArmorLevel)
	{
	case 1:
		Check_Pointer(p);
		break;
	case 2:
		Check_Signature(p);
		break;
	case 3:
		Check_Signature(p);
		p->TestInstance();
		break;
	case 4:
		Check_Signature(p);
		p->TestInstance();
		_ASSERT(dynamic_cast<T>(p) != nullptr);
		break;
	}
	return p;
}

#define Cast_Object(type, ptr) Stuff::Cast_Object_Function(static_cast<type>(ptr))
} // namespace Stuff

#endif