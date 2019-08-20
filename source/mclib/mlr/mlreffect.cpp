//===========================================================================//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#include "stdinc.h"
#include <gameos.hpp>
#include <mlr/mlr.hpp>
#include <mlr/mlreffect.hpp>

using namespace MidLevelRenderer;

//#############################################################################
//############################   MLREffect    #################################
//#############################################################################

void
EffectClipPolygon::Init(void)
{
	// _ASSERT(gos_GetCurrentHeap() == StaticHeap);
	coords.SetLength(Limits::Max_Number_Vertices_Per_Polygon);
	colors.SetLength(Limits::Max_Number_Vertices_Per_Polygon);
	texCoords.SetLength(Limits::Max_Number_Vertices_Per_Polygon);
	clipPerVertex.SetLength(Limits::Max_Number_Vertices_Per_Polygon);
}

void
EffectClipPolygon::Destroy(void)
{
	coords.SetLength(0);
	colors.SetLength(0);
	texCoords.SetLength(0);
	clipPerVertex.SetLength(0);
}

//#############################################################################
//############################   MLREffect    #################################
//#############################################################################

MLREffect::ClassData* MLREffect::DefaultData = nullptr;
EffectClipPolygon* MLREffect::clipBuffer;
std::vector<Stuff::Vector4D>* MLREffect::transformedCoords;

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
void
MLREffect::InitializeClass(void)
{
	_ASSERT(!DefaultData);
	// _ASSERT(gos_GetCurrentHeap() == StaticHeap);
	DefaultData = new ClassData(
		MLREffectClassID, "MidLevelRenderer::MLREffect", RegisteredClass::DefaultData);
	Register_Object(DefaultData);
	transformedCoords = new std::vector<Stuff::Vector4D>(Limits::Max_Number_Vertices_Per_Mesh);
	Register_Object(transformedCoords);
	clipBuffer = new EffectClipPolygon[2];
	Register_Pointer(clipBuffer);
	clipBuffer[0].Init();
	clipBuffer[1].Init();
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
void
MLREffect::TerminateClass(void)
{
	clipBuffer[1].Destroy();
	clipBuffer[0].Destroy();
	Unregister_Pointer(clipBuffer);
	delete[] clipBuffer;
	Unregister_Object(transformedCoords);
	delete transformedCoords;
	Unregister_Object(DefaultData);
	delete DefaultData;
	DefaultData = nullptr;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
MLREffect::MLREffect(uint32_t nr, ClassData* class_data) :
	RegisteredClass(class_data)
{
	// _ASSERT(gos_GetCurrentHeap() == Heap);
	visible = 0;
	maxNrOf = nr;
	testList.SetLength(maxNrOf);
	for (size_t i = 0; i < maxNrOf; i++)
	{
		testList[i] = 0;
	}
	TurnAllOff();
	TurnAllVisible();
	worldToEffect = Stuff::LinearMatrix4D::Identity;
	gos_vertices = nullptr;
	numGOSVertices = 0;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
MLREffect::~MLREffect(void) {}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
void
MLREffect::Transform(size_t nrOfUsedEffects, size_t nrOfVertices)
{
	// Check_Object(this);
	Start_Timer(Transform_Time);
	size_t i, j, k;
	for (i = 0, j = 0; i < nrOfUsedEffects; i++, j += nrOfVertices)
	{
		if (IsOn(i) == false)
		{
			continue;
		}
		for (k = j; k < j + nrOfVertices; k++)
		{
			(*transformedCoords)[k].Multiply(points[k], effectToClipMatrix);
		}
	}
	Stop_Timer(Transform_Time);
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
void
MLREffect::TurnAllOn(void)
{
	// Check_Object(this);
	size_t i;
	for (i = 0; i < maxNrOf; i++)
	{
		testList[i] |= 2;
	}
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
void
MLREffect::TurnAllOff(void)
{
	// Check_Object(this);
	size_t i;
	for (i = 0; i < maxNrOf; i++)
	{
		testList[i] &= ~2;
	}
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
void
MLREffect::TurnAllVisible(void)
{
	// Check_Object(this);
	size_t i;
	for (i = 0; i < maxNrOf; i++)
	{
		testList[i] |= 1;
	}
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
void
MLREffect::TurnAllInVisible(void)
{
	// Check_Object(this);
	size_t i;
	for (i = 0; i < maxNrOf; i++)
	{
		testList[i] &= ~1;
	}
}
