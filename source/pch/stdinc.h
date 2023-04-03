/*******************************************************************************
 stdinc.h - mechcmd2 standard includes

 MechCommander 2 source code

 This file is part of the MechCommander 2 project and is free for use 
 by anyone under the terms of 
 Creative Commons Attribution-NonCommercial 4.0 International Public License
 See the the file license.txt and https://creativecommons.org

 2014-07-24 Jerker Bäck, created

================================================================================
 RcsID = $Id$ */

// clang-format off
// *INDENT-OFF*

#pragma once

#define STRICT
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#define NODRAWTEXT
//#define NOGDI
#define NOBITMAP
#define NOMCX
#define NOSERVICE
//#define NOHELP
#define _CRTDBG_MAP_ALLOC

#if defined(_MSC_VER) && !defined(__clang__)
#define MSDISABLE_WARNING(x) __pragma(warning(disable \
											  : x))
#define MSDISABLE_WARNING_PUSH(x) \
	__pragma(warning(push));      \
	__pragma(warning(disable      \
					 : x))
#define MSDISABLE_WARNING_POP __pragma(warning(pop))
#define MSSUPPRESS_WARNING(x) __pragma(warning(suppress \
											   : x))
#define MSADD_LIBRARY(x) __pragma(comment(lib, x))
#else
#define MSDISABLE_WARNING(x)
#define MSDISABLE_WARNING_PUSH(x)
#define MSDISABLE_WARNING_POP
#define MSSUPPRESS_WARNING(x)
#define MSADD_LIBRARY(x)
#endif

#define _WIN32_WINNT 0x0601 // minimum Windows 7
#include <winsdkver.h>
#include <sdkddkver.h>
#ifndef WINAPI_FAMILY
#define WINAPI_FAMILY WINAPI_FAMILY_DESKTOP_APP
#endif

// #define _CRT_SECURE_NO_WARNINGS 1

// temporary disable warnings when compiling with -Wall
MSDISABLE_WARNING_PUSH(4191 4350 4355 4365 4774 4571 4619 4623 4626 4855 4946 5026 5027 5039 5040 5204 4514 4710 4711 4625 4820 5045 6385 26439 26495 28204)
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <cstdio>
#include <cwctype>
#include <cwchar>
#include <cmath>
#include <cassert>
#include <limits>
#include <locale>
#include <memory>
#include <string>
#include <algorithm>
#include <locale>
#include <initializer_list>
#include <iterator>
#include <string>
#include <vector>
#include <set>
#include <map>
#include <array>
#include <list>
#include <unordered_map>
#include <unordered_set>
#include <iostream>
#include <fstream>
#include <sstream>
#include <iosfwd>
#include <functional>
#include <utility>
#include <type_traits>
#include <stdexcept>
#include <filesystem>
#include <variant>
#include <atomic>
#include <mutex>
#include <future>
#include <malloc.h>
#include <memory.h>
#include <agents.h>
#include <amp.h>
#include <crtdbg.h>
MSDISABLE_WARNING_POP

namespace stdfs = std::filesystem;

#define _ATL_ALL_USER_WARNINGS
#define _ATL_ALL_WARNINGS
#define _ATL_APARTMENT_THREADED
#define _ATL_NO_AUTOMATIC_NAMESPACE

MSDISABLE_WARNING_PUSH(4061 4191 4365 4986 5039 5045 5204 6011 6385 6387 28204 28251)
#include <atlbase.h>
#include <atlwin.h>
//#include <windows.h>
MSDISABLE_WARNING_POP

MSDISABLE_WARNING_PUSH(4062 4191 4265 4355 4365 4571 4619 4623 4625 4626 4668 4710 4820 5026 5027 5039 5045 5204 6001 6340 28204)
#include <comdef.h>
#include <wrl/wrappers/corewrappers.h>
#include <wrl/client.h>
namespace mswrl = Microsoft::WRL;
#include <winrt/base.h>
#include <wil/resource.h>
#include <wil/result.h>
#include <wil/com.h>
#include <wil/safecast.h>
MSDISABLE_WARNING_POP

#define D3DX12_NO_STATE_OBJECT_HELPERS
#define _XM_NO_XMVECTOR_OVERLOADS_
#define XAUDIO2_HELPER_FUNCTIONS
#define USING_XAUDIO2_REDIST
#define _WIN7_PLATFORM_UPDATE

MSDISABLE_WARNING_PUSH(4061 4191 4365 4668 4820 5039 5045 5204 6011 6387 28204 28251)
#include <imagehlp.h>
#include <wincodec.h>
#include <mmreg.h>
#include <mmstream.h>
#include <mmsystem.h>
#include <uianimation.h>
#include <dcompanimation.h>
#include <Audioclient.h>
#include <DirectXCollision.h>
#include <DirectXColors.h>
#include <DirectXMath.h>
#include <DirectXPackedVector.h>
#include <Xinput.h>
#include <d3d12.h>
#include <dxgi1_2.h>
#include <d3d11_1.h>

#include <d3dtypes.h>
#include <ddraw.h>
#include <d3d.h>
#include <dsound.h>
#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>
#include <directxmath.h>
#include <d2d1_2.h>
#include <d3d11_2.h>
#include <dwrite_2.h>
#include <d3d12.h>
MSDISABLE_WARNING_POP

//namespace dxmath = DirectX::SimpleMath;

MSDISABLE_WARNING_PUSH(4061 4127 4365 4514 4571 4623 4625 4626 4820 4946 5026 5027 6326 26439 28020)
#include <nlohmann/json.hpp>
MSDISABLE_WARNING_POP
// namespace json = nlohmann::json;

// disable useless warnings when compiling with -Wall
//#pragma warning(disable : 4514 4710 4711)
// comment out for diagnostic messages
//#pragma warning(disable : 4266 4625 4820)

#pragma warning(disable : 4061) // enumerator in switch of enum is not explicitly handled by a case label
#pragma warning(disable : 4514) // unreferenced inline function has been removed
#pragma warning(disable : 4625) // copy constructor was implicitly defined as deleted
#pragma warning(disable : 4626) // assignment operator was implicitly defined as deleted
#pragma warning(disable : 4710) // function not inlined
#pragma warning(disable : 4820) // bytes padding added
#pragma warning(disable : 5026) // move constructor was implicitly defined as deleted
#pragma warning(disable : 5027) // move assignment operator was implicitly defined as deleted
#pragma warning(disable : 5045) // Compiler will insert Spectre mitigation for memory load if /Qspectre switch specified

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

#ifndef IN_TEST_MODE
#define IN_TEST_MODE 1
#endif

//#ifdef _MSC_VER
//#pragma comment(linker,"/manifestdependency:\"type='win32'
// name='Microsoft.Windows.Common-Controls' version='6.0.0.0'
// processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
//#endif

#if CONSIDERED_DISABLED

#endif
