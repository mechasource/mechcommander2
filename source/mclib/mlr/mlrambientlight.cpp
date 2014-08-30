//===========================================================================//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#include "stdafx.h"

#include <mlr/mlrambientlight.hpp>

using namespace MidLevelRenderer;

//#############################################################################
//###########################    MLRAmbientLight    ###########################
//#############################################################################

MLRAmbientLight::ClassData*
	MLRAmbientLight::DefaultData = nullptr;

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
void
	MLRAmbientLight::InitializeClass()
{
	Verify(!DefaultData);
	// Verify(gos_GetCurrentHeap() == StaticHeap);
	DefaultData =
		new ClassData(
			MLRAmbientLightClassID,
			"MidLevelRenderer::MLRAmbientLight",
			MLRLight::DefaultData
		);
	Register_Object(DefaultData);
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
void
	MLRAmbientLight::TerminateClass()
{
	Unregister_Object(DefaultData);
	delete DefaultData;
	DefaultData = nullptr;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
MLRAmbientLight::MLRAmbientLight() :
	MLRLight(DefaultData)
{
	//Verify(gos_GetCurrentHeap() == Heap);
	lightMask = MLRState::FaceLightingMode|MLRState::VertexLightingMode;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
MLRAmbientLight::MLRAmbientLight(
	Stuff::MemoryStream *stream,
	uint32_t version
) :
	MLRLight(DefaultData, stream, version)
{
	Check_Object(stream);
	//Verify(gos_GetCurrentHeap() == Heap);
	lightMask = MLRState::FaceLightingMode|MLRState::VertexLightingMode;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
MLRAmbientLight::MLRAmbientLight(Stuff::Page *page) :
	MLRLight(DefaultData, page)
{
	Check_Object(page);
	//Verify(gos_GetCurrentHeap() == Heap);
	lightMask = MLRState::FaceLightingMode|MLRState::VertexLightingMode;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
MLRAmbientLight::~MLRAmbientLight()
{
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
void
	MLRAmbientLight::TestInstance()
{
	Verify(IsDerivedFrom(DefaultData));
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
void
	MLRAmbientLight::LightVertex(const MLRVertexData& vertexData)
{
#if COLOR_AS_DWORD
	TO_DO;
#else
	vertexData.color->red += (color.red*intensity);
	vertexData.color->green += (color.green*intensity);
	vertexData.color->blue += (color.blue*intensity);
#endif
}