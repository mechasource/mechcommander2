/*******************************************************************************
 Copyright (c) 2011-2014, Jerker Back. All rights reserved.

 Permission to use, copy, modify, and distribute this software for any
 purpose with or without fee is hereby granted, provided that the following 
 conditions are met (OSI approved BSD 2-clause license):

 1. Redistributions of source code must retain the above copyright notice, 
    this list of conditions and the following disclaimer.
 2. Redistributions in binary form must reproduce the above copyright notice, 
    this list of conditions and the following disclaimer in the documentation 
    and/or other materials provided with the distribution.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
 AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
 IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE 
 DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE 
 FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL 
 DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR 
 SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER 
 CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, 
 OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
 OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*******************************************************************************/
/*******************************************************************************
 stdafx.h - mclib standard includes

 MechCommander 2 source code

 2014-07-24 jerker_back, created
 
 $LastChangedBy$
 
================================================================================
 RcsID = $Id$ */

#pragma once

#ifndef STRICT
#define STRICT
#endif

#define ATL_SUPPRESS_WARNING_PUSH(x)    __pragma(warning(push)); __pragma(warning(disable: x))
#define ATL_SUPPRESS_WARNING_POP        __pragma(warning(pop))
#define ATL_SUPPRESS_WARNING(x)         __pragma(warning(suppress:x))
#define ATL_ADD_LIBRARY(x)				__pragma(comment(lib, x))

#define  _WIN32_WINNT   0x0501	// minimum Windows XP
#include <sdkddkver.h>

#define _ATL_APARTMENT_THREADED
#define _ATL_NO_AUTOMATIC_NAMESPACE
#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS
#define ATL_NO_ASSERT_ON_DESTROY_NONEXISTENT_WINDOW

#define _CRT_SECURE_NO_WARNINGS 1

// disable useless warnings when compiling with -Wall
#pragma warning(disable: 4514 4710 4711)
// comment out for diagnostic messages
#pragma warning(disable: 4625 4820)

// temporary disable warnings when compiling with -Wall
#pragma warning(push)
#pragma warning(disable: 4191 4365)
#include <atlbase.h>
#if (_ATL_VER < 0x0700)	// linking with system (WDK) atl.dll
extern ATL::CComModule& _Module;
ATL_ADD_LIBRARY("atlthunk.lib")
#endif

#include <stdint.h>
#include <limits.h>
#include <ctype.h>
#include <stdlib.h>
#include <memory.h>
#include <string.h>
#include <stdio.h>
#include <time.h>
#include <float.h>
#include <math.h>
#include <tchar.h>
#include <imagehlp.h>
#include <mmsystem.h>
#include <d3dtypes.h>
#include <ddraw.h>
#pragma warning(pop)

#include <mechtypes.h>
#include <mechclassids.h>

// MClib
#ifdef _DEBUG
// _ARMOR;LAB_ONLY;WIN32;_DEBUG;_WINDOWS
// _ARMOR;LAB_ONLY;WIN32;_DEBUG;_WINDOWS;USE_PROTOTYPES;STRICT;
// _ARMOR;LAB_ONLY;WIN32;_DEBUG;_WINDOWS;USE_PROTOTYPES;STRICT;TRACE_ENABLE;
#define _ARMOR		1
#define LAB_ONLY	1
#else 
// NDEBUG;_WINDOWS;WIN32;FINAL
// NDEBUG;_WINDOWS;WIN32;RELEASE;USE_PROTOTYPES;STRICT;
// NDEBUG;_WINDOWS;WIN32;LAB_ONLY;USE_PROTOTYPES;STRICT;	- profile
#endif


namespace Utilities {

//HRESULT WINAPI FormatCLSID(_Out_writes_(nCharacters) PWCHAR pszCLSID,_In_ size_t nCharacters,_In_ REFGUID clsid);
//HRESULT WINAPI PrivateUpdateRegistry(_In_ BOOL bRegister,_In_ uint32_t nID,_In_ REFGUID clsid,_In_ REFGUID libid,_In_opt_ uint32_t dwOleMisc,_In_opt_ ATL::_ATL_REGMAP_ENTRY* pregMap);

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

inline HINSTANCE GetResourceInstance()
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

inline void AddCreateWndData(_Inout_ ATL::_AtlCreateWndData* pData,_In_ PVOID pObject)
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

inline void AtlSetTraceLevel(_In_ uint32_t nLevel)
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
