//===========================================================================//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#pragma once
#define MLR_MLRHEADERS_HPP

#if !defined(MLR_MLR_HPP)
	#include <MLR\MLR.hpp>
#endif

using namespace MidLevelRenderer;
using namespace Stuff;

#include <MLR\MLRPointCloud.hpp>
#include <MLR\MLRTriangleCloud.hpp>
#include <MLR\MLRAmbientLight.hpp>
#include <MLR\MLRInfiniteLight.hpp>
#include <MLR\MLRInfiniteLightWithFalloff.hpp>
#include <MLR\MLRLightMap.hpp>
#include <MLR\MLRPointLight.hpp>
#include <MLR\MLRSpotLight.hpp>

#include <MLR\MLRPrimitiveBase.hpp>
#include <MLR\MLRIndexedPrimitiveBase.hpp>
#include <MLR\MLR_I_PMesh.hpp>
#include <MLR\MLR_I_C_PMesh.hpp>
#include <MLR\MLR_I_L_PMesh.hpp>
#include <MLR\MLR_I_DT_PMesh.hpp>
#include <MLR\MLR_I_C_DT_PMesh.hpp>
#include <MLR\MLR_I_L_DT_PMesh.hpp>
#include <MLR\MLRNGonCloud.hpp>
#include <MLR\MLRCardCloud.hpp>

#include <MLR\MLR_I_MT_PMesh.hpp>

#include <MLR\MLR_I_DeT_PMesh.hpp>
#include <MLR\MLR_I_C_DeT_PMesh.hpp>
#include <MLR\MLR_I_L_DeT_PMesh.hpp>

#include <MLR\MLR_I_TMesh.hpp>
#include <MLR\MLR_I_DeT_TMesh.hpp>
#include <MLR\MLR_I_C_TMesh.hpp>
#include <MLR\MLR_I_L_TMesh.hpp>

#include <MLR\MLR_Terrain.hpp>
#include <MLR\MLR_Terrain2.hpp>

#include <MLR\MLRLineCloud.hpp>
#include <MLR\MLRIndexedTriangleCloud.hpp>

#include <MLR\MLR_I_DT_TMesh.hpp>
#include <MLR\MLR_I_C_DT_TMesh.hpp>
#include <MLR\MLR_I_L_DT_TMesh.hpp>
#include <MLR\MLR_I_C_DeT_TMesh.hpp>
#include <MLR\MLR_I_L_DeT_TMesh.hpp>

#include <MLR\MLRLookUpLight.hpp>