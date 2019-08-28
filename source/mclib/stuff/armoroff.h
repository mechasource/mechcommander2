//===========================================================================//
// File:    debugoff.hpp                                                     //
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#pragma once

#ifndef _ARMOROFF_HPP_
#define _ARMOROFF_HPP_

#ifndef SPEW
#define SPEW(x) ((void)0)
#endif

namespace Stuff
{

#if defined(LAB_ONLY) && !defined(NO_ANALYSIS)
#define USE_TIME_ANALYSIS
#define USE_TRACE_LOG
#define USE_EVENT_STATISTICS
#endif

//#define _ASSERT(c) ((void)0)
#define Warn(c) ((void)0)
#define Check_Pointer(p) ((void)0)
#define Mem_Copy(destination, source, length, available) memcpy(destination, source, length)
#define Str_Copy(destination, source, available) strcpy_s(destination, available, source)
#define Str_Cat(destination, source, available) strcat(destination, source)

#define Check_Object(p) ((void)0)
#define Check_Signature(p) ((void)0)
#define Cast_Pointer(type, ptr) (MSSUPPRESS_WARNING(4946) reinterpret_cast<type>(ptr))
#define Cast_Object(type, ptr) static_cast<type>(ptr)
#define Spew(x, y) ((void)0)
} // namespace Stuff
#endif
