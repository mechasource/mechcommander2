// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently,
// but are changed infrequently

#pragma once

#ifndef STRICT
#define STRICT
#endif

#define ATL_SUPPRESS_WARNING_PUSH(x)    __pragma(warning(push)); __pragma(warning(disable: x))
#define ATL_SUPPRESS_WARNING_POP        __pragma(warning(pop))
#define ATL_ADD_LIBRARY(x)				__pragma(comment(lib, x))

#define  _WIN32_WINNT   0x0600	// minimum Windows Vista
#include <sdkddkver.h>

#define _ATL_APARTMENT_THREADED
#define _ATL_NO_AUTOMATIC_NAMESPACE
#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS
#define ATL_NO_ASSERT_ON_DESTROY_NONEXISTENT_WINDOW

#define _CRT_SECURE_NO_WARNINGS 1

#include <atlbase.h>
#if (_ATL_VER < 0x0700)	// linking with system (WDK) atl.dll
extern ATL::CComModule& _Module;
ATL_ADD_LIBRARY("atlthunk.lib")
#endif

#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>
#include <math.h>
#include <tchar.h>
#include <imagehlp.h>
#include <mmsystem.h>
#include <ddraw.h>

// MClib
#ifdef _DEBUG
// _ARMOR;LAB_ONLY;USE_PROTOTYPES;STRICT;WIN32;_DEBUG;_WINDOWS;BUGLOG
#define _ARMOR		1
#define LAB_ONLY	1
#else 
// NDEBUG;_WINDOWS;WIN32;NOMINMAX;FINAL
// NDEBUG;LAB_ONLY;_WINDOWS;WIN32;NOMINMAX;BUGLOG	- profile
// #define LAB_ONLY 0
// #define FINAL
#endif

#include <mclib.h>

namespace Utilities {

//HRESULT WINAPI FormatCLSID(_Out_writes_(nCharacters) PWCHAR pszCLSID,_In_ size_t nCharacters,_In_ REFGUID clsid);
//HRESULT WINAPI PrivateUpdateRegistry(_In_ BOOL bRegister,_In_ UINT nID,_In_ REFGUID clsid,_In_ REFGUID libid,_In_opt_ ULONG dwOleMisc,_In_opt_ ATL::_ATL_REGMAP_ENTRY* pregMap);

// ModuleHelper - helper functions for ATL3 and ATL7 module classes (modified from WTL)
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

inline void AddCreateWndData(_Inout_ ATL::_AtlCreateWndData* pData,_In_ void* pObject)
{
#if (_ATL_VER >= 0x0700)
	ATL::_AtlWinModule.AddCreateWndData(pData, pObject);
#else
	ATL::_pModule->AddCreateWndData(pData, pObject);
#endif
}

inline void* ExtractCreateWndData(void)
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
inline ATL::CAtlModule* GetModulePtr(void)
{
	return ATL::_pAtlModule;
}
#else
inline ATL::CComModule* GetModulePtr(void)
{
	return ATL::_pModule;
}
#endif

inline bool AtlInitFailed(void)
{
#if (_ATL_VER >= 0x0700)
	return ATL::CAtlBaseModule::m_bInitFailed;
#else
	return ATL::_bInitFailed;
#endif
}

inline void AtlSetTraceLevel(_In_ UINT nLevel)
{
#if defined _DEBUG && (_ATL_VER >= 0x0700)
	ATL::CTrace::SetLevel(nLevel);
#else
	UNREFERENCED_PARAMETER(nLevel);
#endif
}

};	// namespace ModuleHelper
};	// namespace Utilities

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
//#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
//#endif
