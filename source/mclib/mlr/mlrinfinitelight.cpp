//===========================================================================//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#include "stdinc.h"

#include "mlr/mlrinfinitelight.h"

namespace MidLevelRenderer {

//#############################################################################
//###############################    MLRInfiniteLight
//##################################
//#############################################################################

MLRInfiniteLight::ClassData* MLRInfiniteLight::DefaultData = nullptr;

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
void
MLRInfiniteLight::InitializeClass()
{
	_ASSERT(!DefaultData);
	// _ASSERT(gos_GetCurrentHeap() == StaticHeap);
	DefaultData = new ClassData(
		MLRInfiniteLightClassID, "MidLevelRenderer::MLRInfiniteLight", MLRLight::DefaultData);
	Register_Object(DefaultData);
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
void
MLRInfiniteLight::TerminateClass()
{
	Unregister_Object(DefaultData);
	delete DefaultData;
	DefaultData = nullptr;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
MLRInfiniteLight::MLRInfiniteLight(ClassData* class_data) :
	MLRLight(class_data)
{
	// _ASSERT(gos_GetCurrentHeap() == Heap);
	lightMask = MLRState::FaceLightingMode | MLRState::VertexLightingMode;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
MLRInfiniteLight::MLRInfiniteLight(ClassData* class_data, std::iostream stream, uint32_t version) :
	MLRLight(class_data, stream, version)
{
	Check_Object(stream);
	// _ASSERT(gos_GetCurrentHeap() == Heap);
	lightMask = MLRState::FaceLightingMode | MLRState::VertexLightingMode;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
MLRInfiniteLight::MLRInfiniteLight(ClassData* class_data, Stuff::Page* page) :
	MLRLight(class_data, page)
{
	Check_Object(page);
	// _ASSERT(gos_GetCurrentHeap() == Heap);
	lightMask = MLRState::FaceLightingMode | MLRState::VertexLightingMode;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
MLRInfiniteLight::~MLRInfiniteLight() {}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
void
MLRInfiniteLight::TestInstance()
{
	_ASSERT(IsDerivedFrom(DefaultData));
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
void
MLRInfiniteLight::LightVertex(const MLRVertexData& vertexData)
{
	UnitVector3D light_z;
	GetInShapeDirection(light_z);
	//
	//-------------------------------------------------------------------
	// Now we reduce the light level falling on the vertex based upon the
	// cosine of the angle between light and normal
	//-------------------------------------------------------------------
	//
	float cosine = -(light_z * (*vertexData.normal)) * intensity;
#if COLOR_AS_DWORD
	TO_DO;
#else
	RGBColor light_color(color);
	if (cosine > SMALL)
	{
		light_color.red *= cosine;
		light_color.green *= cosine;
		light_color.blue *= cosine;
		vertexData.color->red += light_color.red;
		vertexData.color->green += light_color.green;
		vertexData.color->blue += light_color.blue;
	}
#endif
}

} // namespace MidLevelRenderer
