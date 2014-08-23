//===========================================================================//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#pragma once

#ifndef MLR_MLRCLIPTRICK_HPP
#define MLR_MLRCLIPTRICK_HPP

#include <stuff/vector4d.hpp>

//	defined in MLRPrimitiveBase.cpp
extern int32_t clipTrick[6][2];

inline void
DoClipTrick(Vector4D& v, int32_t ct)
{
	v[clipTrick[ct][0]] = clipTrick[ct][1] ? v.w-SMALL : SMALL;
}

inline void
DoCleanClipTrick(Vector4D& v, int32_t ct)
{
	v[clipTrick[ct][0]] = clipTrick[ct][1] ? v.w : 0.0f;
}
#endif
