//===========================================================================//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#pragma once

#define DISABLE_WARNING_PUSH(x) \
	__pragma(warning(push));    \
	__pragma(warning(disable    \
					 : x))
#define DISABLE_WARNING_POP __pragma(warning(pop))
#define SUPPRESS_WARNING(x) __pragma(warning(suppress \
											 : x))
#define ATL_ADD_LIBRARY(x) __pragma(comment(lib, x))

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN // Exclude rarely-used stuff from Windows headers
#endif

#define _WIN32_WINNT 0x0501 // _WIN32_WINNT_WINXP
#include <sdkddkver.h>

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS // some CString constructors will be \
	// explicit

// turns off MFC's hiding of some common and often safely ignored warning
// messages
#define _AFX_ALL_WARNINGS
// disable useless warnings when compiling with -Wall
#pragma warning(disable : 4514 4710 4711)
// comment out for diagnostic messages
#pragma warning(disable : 4625 4820)

// temporary disable warnings when compiling with -Wall
#pragma warning(push)
#pragma warning(disable : 4191 4263 4264 4266 4350 4365 4626 4668 4917 4986 4987) // this is \
	// for                                                                                   \
	// Visual                                                                                \
	// Studio                                                                                \
	// 2013                                                                                  \
	// MFC
#include <string>
#include <vector>
#include <list>

#include <afxwin.h> // MFC core and standard components
#include <afxext.h> // MFC extensions
#include <afxdisp.h> // MFC Automation classes
#ifndef _AFX_NO_OLE_SUPPORT
#include <afxdtctl.h> // MFC support for Internet Explorer 4 Common Controls
#endif
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h> // MFC support for Windows Common Controls
#endif
#include <emmintrin.h> // MMX, SSE, SSE2 intrinsic support
#pragma warning(pop)

#include <mechtypes.h>

#ifdef _DEBUG
// WIN32;_DEBUG;_WINDOWS;LAB_ONLY;_ARMOR;WINVER=0x0501
#define LAB_ONLY 1
#define _ARMOR 1
#else
// WIN32;NDEBUG;_WINDOWS;WINVER=0x0501
// WIN32;NDEBUG;_WINDOWS;WINVER=0x0501 - profile
#endif

// MechCommander GameOS :/
// #include "mfcplatform.hpp"

#pragma comment(linker, \
	"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

#ifndef _CONSIDERED_OBSOLETE
#define _CONSIDERED_OBSOLETE 0
#endif
#ifndef _CONSIDERED_UNSUPPORTED
#define _CONSIDERED_UNSUPPORTED 0
#endif
#ifndef _CONSIDERED_DISABLED
#define _CONSIDERED_DISABLED 0
#endif

//..\GameOS\include;..\MCLIB;.\window;"$(VCInstallDir)PlatformSDK\Include\mfc";"$(VCInstallDir)PlatformSDK\Include\atl"
//..\mclib\$(ConfigurationName)\mclib.lib
//..\gameos\lib\$(ConfigurationName)\gameos.lib
//..\gameos\lib\$(ConfigurationName)\mfcplatform.lib
//..\mclib\gosfx\$(ConfigurationName)\gosfx.lib
//..\mclib\mlr\$(ConfigurationName)\mlr.lib
//..\mclib\stuff\$(ConfigurationName)\stuff.lib
//..\gameos\lib\$(ConfigurationName)\zlib.lib
