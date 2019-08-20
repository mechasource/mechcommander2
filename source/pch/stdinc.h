/*******************************************************************************
 stdinc.h - mechcmd2 standard includes

 MechCommander 2 source code

 2014-07-24 Jerker Beck, created

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
#define DISABLE_WARNING_PUSH(x) \
	__pragma(warning(push));    \
	__pragma(warning(disable    \
					 : x))
#define DISABLE_WARNING_POP __pragma(warning(pop))
#define SUPPRESS_WARNING(x) __pragma(warning(suppress \
											 : x))
#define ADD_LIBRARY(x) __pragma(comment(lib, x))
#else
#define DISABLE_WARNING_PUSH
#define DISABLE_WARNING_POP
#define SUPPRESS_WARNING(x)
#define ADD_LIBRARY
#endif

#define _ATL_APARTMENT_THREADED
#define _ATL_NO_AUTOMATIC_NAMESPACE
#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS
#define ATL_NO_ASSERT_ON_DESTROY_NONEXISTENT_WINDOW
#define DIRECTINPUT_VERSION 0x0800

// #define _CRT_SECURE_NO_WARNINGS 1

// temporary disable warnings when compiling with -Wall
DISABLE_WARNING_PUSH(4191 4350 4355 4365 4774 4571 4619 4623 4626 4946 5026 5027 5039 5040 4514 4710 4711 4625 4820 26439 26495)
#include <cstdlib>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cmath>
#include <cassert>
#include <limits>
#include <algorithm>
#include <utility>
#include <initializer_list>
#include <iterator>
#include <string>
#include <vector>
#include <set>
#include <map>
#include <iostream>
#include <fstream>
#include <sstream>
#include <iosfwd>
#include <exception>
#include <stdexcept>
#include <functional>
#include <filesystem>
#include <variant>
#include <comdef.h>
#include <crtdbg.h>
#include <cppunittest.h>
// Concurrency
#include <agents.h>
#include <amp.h>
DISABLE_WARNING_POP

namespace stdfs = std::filesystem;

DISABLE_WARNING_PUSH(4061 4191 4365 5039 5045 6011 6387 28251)
#include <atlbase.h>
#include <imagehlp.h>
#include <mmsystem.h>
#include <d3dtypes.h>
#include <ddraw.h>
#include <d3d.h>
#include <dsound.h>
#include <dinput.h>
#include <mmstream.h>
#include <directxmath.h>
#include <d2d1_2.h>
#include <d3d11_2.h>
#include <dwrite_2.h>
#include <d3d12.h>
#include <wincodec.h>
#include <uianimation.h>
#include <dcompanimation.h>
DISABLE_WARNING_POP

DISABLE_WARNING_PUSH(4061 4127 4365 4514 4571 4625 4626 4820 4946 5026 5027 6326 28020)
#include <nlohmann/json.hpp>
DISABLE_WARNING_POP
// namespace json = nlohmann::json;

#include <mechtypes.h>

// disable useless warnings when compiling with -Wall
//#pragma warning(disable : 4514 4710 4711)
// comment out for diagnostic messages
//#pragma warning(disable : 4266 4625 4820)

#pragma warning(disable : 4514) // unreferenced inline function has been removed
#pragma warning(disable : 4626) // assignment operator was implicitly defined as deleted
#pragma warning(disable : 4820) // bytes padding added
#pragma warning(disable : 5027) // move assignment operator was implicitly defined as deleted

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

#ifndef IN_TEST_MODE
#define IN_TEST_MODE 0
#endif

#define USE_INLINE_ASSEMBLER_CODE 0
#define _CONSIDERED_TEMPORARILY_DISABLED 0
#define _CONSIDERED_OBSOLETE 0
#define _CONSIDERED_UNSUPPORTED 0
#define _CONSIDERED_DISABLED 0
#define _CONSIDERED_UNUSED 0

//#ifdef _MSC_VER
//#pragma comment(linker,"/manifestdependency:\"type='win32'
// name='Microsoft.Windows.Common-Controls' version='6.0.0.0'
// processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
//#endif
