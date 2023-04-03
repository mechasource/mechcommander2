//===========================================================================//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#include "stdinc.h"

#include "mlr/mlrambientlight.h"

namespace MidLevelRenderer
{

//#############################################################################
//###########################    MLRAmbientLight    ###########################
//#############################################################################

MLRAmbientLight::ClassData* MLRAmbientLight::DefaultData = nullptr;

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
void MLRAmbientLight::InitializeClass()
{
	_ASSERT(!DefaultData);
	// _ASSERT(gos_GetCurrentHeap() == StaticHeap);
	DefaultData = new ClassData(
		MLRAmbientLightClassID, "MidLevelRenderer::MLRAmbientLight", MLRLight::DefaultData);
	Register_Object(DefaultData);
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
void MLRAmbientLight::TerminateClass()
{
	Unregister_Object(DefaultData);
	delete DefaultData;
	DefaultData = nullptr;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
MLRAmbientLight::MLRAmbientLight()
	: MLRLight(DefaultData)
{
	// _ASSERT(gos_GetCurrentHeap() == Heap);
	lightMask = MLRState::FaceLightingMode | MLRState::VertexLightingMode;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
MLRAmbientLight::MLRAmbientLight(std::iostream stream, uint32_t version)
	: MLRLight(DefaultData, stream, version)
{
	Check_Object(stream);
	// _ASSERT(gos_GetCurrentHeap() == Heap);
	lightMask = MLRState::FaceLightingMode | MLRState::VertexLightingMode;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
MLRAmbientLight::MLRAmbientLight(Stuff::Page* page)
	: MLRLight(DefaultData, page)
{
	Check_Object(page);
	// _ASSERT(gos_GetCurrentHeap() == Heap);
	lightMask = MLRState::FaceLightingMode | MLRState::VertexLightingMode;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
MLRAmbientLight::~MLRAmbientLight() { }

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
void MLRAmbientLight::TestInstance()
{
	_ASSERT(IsDerivedFrom(DefaultData));
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
void MLRAmbientLight::LightVertex(const MLRVertexData& vertexData)
{
#if COLOR_AS_DWORD
	TO_DO;
#else
	vertexData.color->red += (color.red * intensity);
	vertexData.color->green += (color.green * intensity);
	vertexData.color->blue += (color.blue * intensity);
#endif
}

} // namespace MidLevelRenderer
