/*******************************************************************************
 common.h - MechCommander common utilities

 MechCommander 2 source code

 SPDX-License-Identifier: BSD-3-Clause
 This file is part of the MechCommander 2 project and is free for use 
 by anyone under the terms of 
 Creative Commons Attribution-NonCommercial 4.0 International Public License
 See the the file license.txt and https://creativecommons.org

 2014-08-16 Jerker Bäck, created

================================================================================
 RcsID = $Id$ */

#pragma once

#include <cstdint>
#include <type_traits>

// give more meaning to temporary and disabled code
#define CONSIDERED_OBSOLETE 0
#define CONSIDERED_UNSUPPORTED 0
#define CONSIDERED_DISABLED 0
#define CONSIDERED_UNUSED 0
#define USE_INLINE_ASSEMBLER_CODE 0

#ifdef _DEBUG
#define NODEFAULT _ASSERT(0)
#else
#define NODEFAULT __assume(0)
#endif

#define MECH_IMPEXP extern "C"
#define MECH_CALL __stdcall

// mechcmd2
#ifdef _DEBUG
// _ARMOR;LAB_ONLY;USE_PROTOTYPES;STRICT;WIN32;_DEBUG;_WINDOWS;BUGLOG
//#define _ARMOR 1
//#define LAB_ONLY 1
#else
// NDEBUG;_WINDOWS;WIN32;NOMINMAX;FINAL
// NDEBUG;LAB_ONLY;_WINDOWS;WIN32;NOMINMAX;BUGLOG	- profile
// #define LAB_ONLY 0
// #define FINAL
#endif


//#include <EnumClass.h>
//#include <magic_enum.hpp>	// https://github.com/Neargye/magic_enum

// enum bitmask operations <type_traits>
#ifndef ENABLE_ENUM_BITMASKS
#define ENABLE_ENUM_BITMASKS(e) _BITMASK_OPS(_EMPTY_ARGUMENT, e)
//#define ENABLE_ENUM_BITMASKS(e) DEFINE_ENUM_FLAG_OPERATORS(e)
//#define ENABLE_ENUM_BITMASKS(e) enableEnumClassBitmask(e)
#endif

template <class Tenum>
constexpr std::common_type_t<uint16_t, std::underlying_type_t<Tenum>>
to_integer(Tenum e) noexcept
{
	return static_cast<std::common_type_t<uint16_t, std::underlying_type_t<Tenum>>>(e);
}

// return static_cast<std::underlying_type_t<Enum>>(e);

#ifndef __cpp_lib_to_underlying
template <class Tenum>
constexpr std::underlying_type_t<Tenum> std::to_underlying(Tenum e) noexcept // (since C++23)
{
	return static_cast<std::underlying_type_t<Tenum>>(e);
}
#endif

#if CONSIDERED_UNUSED
[[nodiscard]] constexpr Tenum operator~(Tenum lhs) noexcept
{
	return static_cast<Tenum>(~static_cast<std::underlying_type_t<Tenum>>(lhs));
}
[[nodiscard]] constexpr Tenum operator&(Tenum lhs, Tenum rhs) noexcept
{
	return static_cast<Tenum>(static_cast<std::underlying_type_t<Tenum>>(lhs) & static_cast<std::underlying_type_t<Tenum>>(rhs));
}
[[nodiscard]] constexpr Tenum operator|(Tenum lhs, Tenum rhs) noexcept
{
	return static_cast<Tenum>(static_cast<std::underlying_type_t<Tenum>>(lhs) | static_cast<std::underlying_type_t<Tenum>>(rhs));
}
[[nodiscard]] constexpr Tenum operator^(Tenum lhs, Tenum rhs) noexcept
{
	return static_cast<Tenum>(static_cast<std::underlying_type_t<Tenum>>(lhs) ^ static_cast<std::underlying_type_t<Tenum>>(rhs));
}

constexpr Tenum& operator&=(Tenum& lhs, Tenum rhs) noexcept
{
	return lhs = lhs & rhs;
}
constexpr Tenum& operator|=(Tenum& lhs, Tenum rhs) noexcept
{
	return lhs = lhs | rhs;
}
constexpr Tenum& operator^=(Tenum& lhs, Tenum rhs) noexcept
{
	return lhs = lhs ^ rhs;
}

template <class Tenum>
[[nodiscard]] constexpr bool _Bitmask_includes_any(Tenum lhs, Tenum _Elements) noexcept
{
	return (lhs & _Elements) != Tenum {};
}
template <class Tenum>
[[nodiscard]] constexpr bool _Bitmask_includes_all(Tenum lhs, Tenum _Elements) noexcept
{
	return (lhs & _Elements) == _Elements;
}

//using namespace magic_enum::bitwise_operators; // out-of-the-box bitwise operators for enums.

#endif

namespace literals
{

//using namespace std::literals::string_view_literals;

constexpr std::wstring_view pathseparator {L"/\\"}; // = "/\\"sv;

constexpr std::wstring_view emptystring {L"<empty string>"}; // = "<empty string>"sv;
constexpr std::wstring_view invalidstring {L"<invalid string>"}; // = "<invalid string>"sv;
constexpr std::wstring_view notavailable {L"N/A"}; // = "N/A"sv;
constexpr std::wstring_view unknown {L"<unknown>"}; // = "<unknown>"sv;
constexpr std::wstring_view iso8601date {L"%04u-%02u-%02u %02u:%02u:%02u"}; // = "%04u-%02u-%02u %02u:%02u:%02u"sv;
constexpr std::wstring_view dmy {L"%02u%c%02u%c%04u"}; // = "%02u%c%02u%c%04u"sv;
constexpr std::wstring_view ymd {L"%04u%c%02u%c%02u"}; // = "%04u%c%02u%c%02u"sv;
constexpr std::wstring_view clock24 {L" %s%u%c%02u"}; // = " %s%u%c%02u"sv;
constexpr std::wstring_view clock12 {L" %s%u%c%02u%c"}; // = " %s%u%c%02u%c"sv;
constexpr std::wstring_view attributeformat {L"%s%s%s%s%s%s%s%s"}; // = "%s%s%s%s%s%s%s%s"sv;
constexpr std::wstring_view hexformat8 {L"0x%08X"}; // = "0x%08X"sv;

} // namespace literals

extern "C"
{
#ifndef LONG_TYPES_DEFINED
#define LONG_TYPES_DEFINED
typedef signed long long32_t;
typedef unsigned long ulong32_t;
#endif
}

namespace mechcmd
{

// Throw a std::system_error if the HRESULT indicates failure.
inline HRESULT ThrowIfFailed(HRESULT hr)
{
#ifdef __WIL_RESULTMACROS_INCLUDED
	THROW_IF_FAILED(hr);
#else
	if (FAILED(hr))
		throw std::system_error {hr, std::system_category(), __func__};
#endif
	return hr;
}

// Windows API functions that return false on failure and set LastErrorValue
inline HRESULT ThrowIfFalse(bool expr)
{
	if (static_cast<bool>(expr) == false)
	{
#ifdef __WIL_RESULTMACROS_INCLUDED
		THROW_LAST_ERROR();
#else
		throw std::system_error {static_cast<int32_t>(::GetLastError()), std::system_category(), __func__};
#endif
	}

	return S_OK;
}

// Windows API functions that return a nullptr pointer on failure and set LastErrorValue
template <typename Ttype>
Ttype ThrowIfNullptr(Ttype ptr)
{
#ifdef __WIL_RESULTMACROS_INCLUDED
	THROW_LAST_ERROR_IF_NULL(ptr);
#else
	if (ptr == nullptr)
	{
		throw std::system_error {static_cast<int32_t>(::GetLastError()), std::system_category(), __func__};
	}
#endif
	return ptr;
}

// Windows API functions that return 0 on failure and set LastErrorValue
template <typename Ttype>
Ttype ThrowIfZero(Ttype expr)
{
	if (expr == 0)
	{
#ifdef __WIL_RESULTMACROS_INCLUDED
		THROW_LAST_ERROR();
#else
		throw std::system_error {static_cast<int32_t>(::GetLastError()), std::system_category(), __func__};
#endif
	}
	return expr;
}

// Windows API functions that return false on failure but don't set LastErrorValue
inline HRESULT
ThrowHresultIfFalse(bool expr, HRESULT hr)
{
	if (static_cast<bool>(expr) == false)
	{
#ifdef __WIL_RESULTMACROS_INCLUDED
		THROW_HR(hr);
#else
		throw std::system_error {hr, std::system_category(), __func__};
#endif
	}
	return S_OK;
}

// Windows API functions that return a nullptr pointer on failure but don't set LastErrorValue
template <typename Ttype>
Ttype ThrowHresultIfNullptr(Ttype ptr, HRESULT hr)
{
	if (ptr == nullptr)
	{
#ifdef __WIL_RESULTMACROS_INCLUDED
		THROW_HR(hr);
#else
		throw std::system_error {hr, std::system_category(), __func__};
#endif
	}
	return ptr;
}

// Windows API functions that return 0 on failure but don't set LastErrorValue
template <typename Ttype>
Ttype ThrowHresultIfZero(Ttype expr, HRESULT hr)
{
	if (expr == 0)
	{
#ifdef __WIL_RESULTMACROS_INCLUDED
		THROW_HR(hr);
#else
		throw std::system_error {hr, std::system_category(), __func__};
#endif
	}
	return expr;
}

template <typename Ttype>
uint16_t LoWord(Ttype type)
{
	return static_cast<uint16_t>(static_cast<uintptr_t>(type) & 0xffff);
}
template <typename Ttype>
uint16_t HiWord(Ttype type)
{
	return static_cast<uint16_t>((static_cast<uintptr_t>(type) >> 16) & 0xffff);
}
template <typename Ttype>
int32_t GetXlparam(Ttype type)
{
	return static_cast<int32_t>(static_cast<uintptr_t>(type) & 0xffff);
}
template <typename Ttype>
int32_t GetYlparam(Ttype type)
{
	return static_cast<int32_t>((static_cast<uintptr_t>(type) >> 16) & 0xffff);
}

namespace ModuleHelper
{
extern "C" IMAGE_DOS_HEADER __ImageBase;

inline HINSTANCE GetModuleInstance(void) noexcept
{
	return reinterpret_cast<HINSTANCE>(&__ImageBase);
}

inline HINSTANCE GetResourceInstance(void) noexcept
{
	return reinterpret_cast<HINSTANCE>(&__ImageBase);
}

}; // namespace ModuleHelper

HRESULT LoadResourceString(_In_ HMODULE instancehandle, _Out_ std::wstring_view& resstring, _In_ uint16_t resid, _In_ uint16_t langid = UINT16_MAX);
HRESULT LoadResourceString(_In_ uint16_t resid, _Out_ std::wstring_view& resstring);

} // namespace mechcmd

#endif // _MECHUTILS_H_
