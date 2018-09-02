/*******************************************************************************
 stdinc.h - mechcmd2 standard includes

 MechCommander 2 source code

 2014-07-24 jerker_back, created

 $LastChangedBy$

================================================================================
 RcsID = $Id$ */

#pragma once

#define STRICT
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#define _CRTDBG_MAP_ALLOC

#define _WIN32_WINNT 0x0600 // minimum Vista
#include <winsdkver.h>
#include <sdkddkver.h>
#ifndef WINAPI_FAMILY
#define WINAPI_FAMILY WINAPI_FAMILY_DESKTOP_APP
#endif

#ifdef _MSC_VER
#define DISABLE_WARNING_PUSH(x)                                                                    \
	__pragma(warning(push)); __pragma(warning(disable : x))
#define DISABLE_WARNING_POP __pragma(warning(pop))
#define ADD_LIBRARY(x) __pragma(comment(lib, x))
#else
#define DISABLE_WARNING_PUSH
#define DISABLE_WARNING_POP
#define ADD_LIBRARY
#endif

#define _ATL_APARTMENT_THREADED
#define _ATL_NO_AUTOMATIC_NAMESPACE
#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS
#define ATL_NO_ASSERT_ON_DESTROY_NONEXISTENT_WINDOW
#define DIRECTINPUT_VERSION         0x0800

// #define _CRT_SECURE_NO_WARNINGS 1

// disable useless warnings when compiling with -Wall
#pragma warning(disable : 4514 4710 4711)
// comment out for diagnostic messages
#pragma warning(disable : 4266 4625 4820)

// temporary disable warnings when compiling with -Wall
DISABLE_WARNING_PUSH(4191 4350 4365 4774 4571 4623 4626 5026 5027 5039 4514 4710 4711 4625 4820 26439 26495)
#include <cstdlib>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cmath>
#include <limits>
#include <algorithm>
#include <utility>
#include <string>
#include <iostream>
#include <sstream>
#include <iosfwd>
#include <iterator>
#include <exception>
#include <stdexcept>
#include <vector>
#include <set>
#include <map>
#include <crtdbg.h>
#include <cppunittest.h>
DISABLE_WARNING_POP

DISABLE_WARNING_PUSH(4191 4365 5039 5045 6011)
#include <atlbase.h>
#include <imagehlp.h>
#include <mmsystem.h>
#include <d3dtypes.h>
#include <ddraw.h>
#include <d3d.h>
#include <dsound.h>
#include <dinput.h>
#include <mmstream.h>
DISABLE_WARNING_POP

#include <mechtypes.h>

#define MECH_IMPEXP	extern "C"
#define MECH_CALL	__stdcall

// mechcmd2
#ifdef _DEBUG
// _ARMOR;LAB_ONLY;USE_PROTOTYPES;STRICT;WIN32;_DEBUG;_WINDOWS;BUGLOG
#define _ARMOR 1
#define LAB_ONLY 1
#else
// NDEBUG;_WINDOWS;WIN32;NOMINMAX;FINAL
// NDEBUG;LAB_ONLY;_WINDOWS;WIN32;NOMINMAX;BUGLOG	- profile
// #define LAB_ONLY 0
// #define FINAL
#endif

namespace Utilities
{

// HRESULT WINAPI FormatCLSID(_Out_writes_(nCharacters) PWCHAR pszCLSID,_In_
// size_t nCharacters,_In_ REFGUID clsid);  HRESULT WINAPI
// PrivateUpdateRegistry(_In_ BOOL bRegister,_In_ uint32_t nID,_In_ REFGUID
// clsid,_In_ REFGUID libid,_In_opt_ uint32_t dwOleMisc,_In_opt_
// ATL::_ATL_REGMAP_ENTRY* pregMap);

// ModuleHelper - helper functions for ATL3 and ATL7 module classes (modified
// from WTL)
namespace ModuleHelper
{
inline HINSTANCE GetModuleInstance(void)
{
#if (_ATL_VER >= 0x0700)
	return ATL::_AtlBaseModule.GetModuleInstance();
#else
	return ATL::_pModule->GetModuleInstance();
#endif
}

inline HINSTANCE GetResourceInstance(void)
{
#if (_ATL_VER >= 0x0700)
	return ATL::_AtlBaseModule.GetResourceInstance();
#else
	return ATL::_pModule->GetResourceInstance();
#endif
}

inline HINSTANCE SetResourceInstance(_In_ HINSTANCE hInstance)
{
#if (_ATL_VER >= 0x0700)
	return ATL::_AtlBaseModule.SetResourceInstance(hInstance);
#else
	return ATL::_pModule->SetResourceInstance(hInstance);
#endif
}

inline void AddCreateWndData(
	_Inout_ ATL::_AtlCreateWndData* pData, _In_ PVOID pObject)
{
#if (_ATL_VER >= 0x0700)
	ATL::_AtlWinModule.AddCreateWndData(pData, pObject);
#else
	ATL::_pModule->AddCreateWndData(pData, pObject);
#endif
}

inline PVOID ExtractCreateWndData(void)
{
#if (_ATL_VER >= 0x0700)
	return ATL::_AtlWinModule.ExtractCreateWndData();
#else
	return ATL::_pModule->ExtractCreateWndData();
#endif
}

inline void AtlTerminate(void)
{
#if (_ATL_VER >= 0x0700)
	return ATL::_AtlWinModule.Term();
#else
	return ATL::_pModule->Term();
#endif
}

#if (_ATL_VER >= 0x0700)
inline ATL::CAtlModule* GetModulePtr(void) { return ATL::_pAtlModule; }
#else
inline ATL::CComModule* GetModulePtr(void) { return ATL::_pModule; }
#endif

inline bool AtlInitFailed(void)
{
#if (_ATL_VER >= 0x0700)
	return ATL::CAtlBaseModule::m_bInitFailed;
#else
	return ATL::_bInitFailed;
#endif
}

inline void AtlSetTraceLevel(_In_ uint32_t nLevel)
{
#if defined _DEBUG && (_ATL_VER >= 0x0700)
	ATL::CTrace::SetLevel(nLevel);
#else
	UNREFERENCED_PARAMETER(nLevel);
#endif
}

}; // namespace ModuleHelper
}; // namespace Utilities

using namespace Utilities;

#ifndef _CONSIDERED_OBSOLETE
#define _CONSIDERED_OBSOLETE 0
#endif
#ifndef _CONSIDERED_UNSUPPORTED
#define _CONSIDERED_UNSUPPORTED 0
#endif
#ifndef _CONSIDERED_DISABLED
#define _CONSIDERED_DISABLED 0
#endif

//#ifdef _MSC_VER
//#pragma comment(linker,"/manifestdependency:\"type='win32'
//name='Microsoft.Windows.Common-Controls' version='6.0.0.0'
//processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
//#endif
