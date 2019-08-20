//===========================================================================//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#pragma once

#ifndef MLR_MLRCLIPTRICK_HPP
#define MLR_MLRCLIPTRICK_HPP

#include <stuff/vector4d.hpp>

// defined in mlrprimitivebase.cpp
extern size_t clipTrick[6][2];

inline void DoClipTrick(Stuff::Vector4D& v, size_t ct)
{
	v[clipTrick[ct][0]] = clipTrick[ct][1] ? v.w - Stuff::SMALL : Stuff::SMALL;
}

inline void DoCleanClipTrick(Stuff::Vector4D& v, size_t ct)
{
	v[clipTrick[ct][0]] = clipTrick[ct][1] ? v.w : 0.0f;
}
#endif
