/*******************************************************************************
 mechutils.h - MechCommander common utilities

 MechCommander 2 source code

 This file is part of the MechCommander 2 project and is free for use 
 by anyone under the terms of 
 Creative Commons Attribution-NonCommercial 4.0 International Public License
 See the the file license.txt and https://creativecommons.org

 2014-08-16 Jerker Back, created

================================================================================
 RcsID = $Id$ */

#pragma once

#ifndef _MECHUTILS_H_
#define _MECHUTILS_H_

#ifdef __cplusplus
extern "C"
{
#endif

#ifndef LONG_TYPES_DEFINED
#define LONG_TYPES_DEFINED
typedef signed long long32_t;
typedef unsigned long ulong32_t;
#endif

#ifdef __cplusplus
}
#endif
#pragma pack(pop)

namespace mechcmd {

// Throw a std::system_error if the HRESULT indicates failure.
inline HRESULT 
ThrowIfFailed(HRESULT hr)
{
#ifdef __WIL_RESULTMACROS_INCLUDED
	THROW_IF_FAILED(hr);
#else
	if (FAILED(hr))
		throw std::system_error{hr, std::system_category(), __func__};
#endif
	return hr;
}

// Windows API functions that return false on failure and set LastErrorValue
inline HRESULT
ThrowIfFalse(bool expr)
{
	if (static_cast<bool>(expr) == false)
	{
#ifdef __WIL_RESULTMACROS_INCLUDED
		THROW_LAST_ERROR();
#else
		throw std::system_error{static_cast<int32_t>(::GetLastError()), std::system_category(), __func__};
#endif
	}

	return S_OK;
}

// Windows API functions that return a nullptr pointer on failure and set LastErrorValue
template <typename Ttype> Ttype 
ThrowIfNullptr(Ttype ptr)
{
#ifdef __WIL_RESULTMACROS_INCLUDED
	THROW_LAST_ERROR_IF_NULL(ptr);
#else
	if (ptr == nullptr)
	{
		throw std::system_error{static_cast<int32_t>(::GetLastError()), std::system_category(), __func__};
	}
#endif
	return ptr;
}

// Windows API functions that return 0 on failure and set LastErrorValue
template <typename Ttype> Ttype
ThrowIfZero(Ttype expr)
{
	if (expr == 0)
	{
#ifdef __WIL_RESULTMACROS_INCLUDED
		THROW_LAST_ERROR();
#else
		throw std::system_error{static_cast<int32_t>(::GetLastError()), std::system_category(), __func__};
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
		throw std::system_error{hr, std::system_category(), __func__};
#endif
	}
	return S_OK;
}

// Windows API functions that return a nullptr pointer on failure but don't set LastErrorValue
template <typename Ttype> Ttype 
ThrowHresultIfNullptr(Ttype ptr, HRESULT hr)
{
	if (ptr == nullptr)
	{
#ifdef __WIL_RESULTMACROS_INCLUDED
		THROW_HR(hr);
#else
		throw std::system_error{hr, std::system_category(), __func__};
#endif
	}
	return ptr;
}

// Windows API functions that return 0 on failure but don't set LastErrorValue
template <typename Ttype> Ttype
ThrowHresultIfZero(Ttype expr, HRESULT hr)
{
	if (expr == 0)
	{
#ifdef __WIL_RESULTMACROS_INCLUDED
		THROW_HR(hr);
#else
		throw std::system_error{hr, std::system_category(), __func__};
#endif
	}
	return expr;
}

template <typename Ttype> uint16_t 
LoWord(Ttype type)
{
	return static_cast<uint16_t>(static_cast<uintptr_t>(type) & 0xffff);
}
template <typename Ttype> uint16_t 
HiWord(Ttype type)
{
	return static_cast<uint16_t>((static_cast<uintptr_t>(type) >> 16) & 0xffff);
}
template <typename Ttype> int32_t
GetXlparam(Ttype type)
{
	return static_cast<int32_t>(static_cast<uintptr_t>(type) & 0xffff);
}
template <typename Ttype> int32_t
GetYlparam(Ttype type)
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

#endif	// _MECHUTILS_H_
