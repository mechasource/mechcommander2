//===========================================================================//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//

#pragma once

#ifndef _MLRTRIANGLELIGHTING_HPP_
#define _MLRTRIANGLELIGHTING_HPP_

#include "mlr/mlrlight.h"

void CLASSNAME::Lighting(MLRLight* const* lights, uint32_t nrLights)
{
	// Check_Object(this);
	//
	//----------------------------------------------------------------------
	// If no lights or normals are specified, use the original vertex colors
	//----------------------------------------------------------------------
	//
	actualcolours = &colors;
	uint32_t state_mask = GetCurrentState().GetLightingMode();
	if (nrLights == 0 || normals.GetLength() == 0 || state_mask == MLRState::LightingOffMode)
		return;
	Check_Pointer(lights);
	//
	//-------------------------------
	// See if we need vertex lighting
	//-------------------------------
	//
	if (state_mask & MLRState::VertexLightingMode)
	{
		Start_Timer(Vertex_Light_Time);
		_ASSERT(colors.GetLength() == litcolours.GetLength());
		_ASSERT(normals.GetLength() == colors.GetLength());
		_ASSERT(coords.GetLength() == colors.GetLength());
		size_t i, k, len = colors.GetLength();
		MLRVertexData vertexData;
#if COLOR_AS_DWORD
		TO_DO;
#else
		Stuff::RGBAcolour* color = &colors[0];
#endif
		//
		//--------------------------------
		// Now light the array of vertices
		//--------------------------------
		//
		vertexData.point = &coords[0];
		vertexData.color = &litcolours[0];
		vertexData.normal = &normals[0];
		for (k = 0; k < len; k++)
		{
			if (visibleIndexedVertices[k] != 0)
			{
				vertexData.color->red = 0.0f;
				vertexData.color->green = 0.0f;
				vertexData.color->blue = 0.0f;
				vertexData.color->alpha = color->alpha;
				for (i = 0; i < nrLights; i++)
				{
					MLRLight* light = lights[i];
					Check_Object(light);
					uint32_t mask = state_mask & light->GetLightMask();
					if (!mask)
						continue;
					if (mask & MLRState::VertexLightingMode)
					{
						if (GetCurrentState().GetBackFaceMode() != MLRState::BackFaceOffMode || light->GetLightType() == MLRLight::AmbientLight)
						{
							light->LightVertex(vertexData);
							Set_Statistic(LitVertices, LitVertices + 1);
						}
					}
				}
				vertexData.color->red *= color->red;
				vertexData.color->green *= color->green;
				vertexData.color->blue *= color->blue;
				vertexData.color->alpha *= color->alpha;
			}
			vertexData.point++;
			vertexData.color++;
			vertexData.normal++;
			color++;
		}
		actualcolours = &litcolours;
		Stop_Timer(Vertex_Light_Time);
	}
	if (state_mask & MLRState::LightMapLightingMode)
	{
		Start_Timer(LightMap_Light_Time);
		size_t i;
		for (i = 0; i < nrLights; i++)
		{
			MLRLight* light = lights[i];
			Check_Object(light);
			MLRLightMap* lm = light->GetLightMap();
			if (lm == nullptr)
			{
				continue;
			}
			//			_ASSERT(state.GetAlphaMode() == MLRState::OneZeroMode);
			uint32_t mask = state_mask & light->GetLightMask();
			if (!mask)
				continue;
			if (mask & MLRState::LightMapLightingMode)
			{
				LightMapLighting(lights[i]);
			}
		}
		Stop_Timer(LightMap_Light_Time);
	}
}

#endif
