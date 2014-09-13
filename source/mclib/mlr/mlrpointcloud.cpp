//===========================================================================//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#include "stdafx.h"
#include <gameos.hpp>
#include <mlr/mlrsorter.hpp>
#include <mlr/mlrclipper.hpp>
#include <mlr/mlrpointcloud.hpp>

using namespace MidLevelRenderer;

//#############################################################################
//#########################    MLRPointCloud    ###############################
//#############################################################################

MLRPointCloud::ClassData* MLRPointCloud::DefaultData = nullptr;

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
void
MLRPointCloud::InitializeClass()
{
	Verify(!DefaultData);
	// Verify(gos_GetCurrentHeap() == StaticHeap);
	DefaultData =
		new ClassData(
		MLRPointCloudClassID,
		"MidLevelRenderer::MLRPointCloud",
		MLREffect::DefaultData
	);
	Register_Object(DefaultData);
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
void
MLRPointCloud::TerminateClass()
{
	Unregister_Object(DefaultData);
	delete DefaultData;
	DefaultData = nullptr;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
MLRPointCloud::MLRPointCloud(uint32_t nr, uint32_t _type) :
	MLREffect(nr, DefaultData), type(_type)
{
	//Verify(gos_GetCurrentHeap() == Heap);
	usedNrOfVertices = 0;
	Check_Pointer(this);
	drawMode = SortData::PointCloud;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
MLRPointCloud::~MLRPointCloud()
{
	Check_Object(this);
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
void
MLRPointCloud::SetData(
	pcsize_t count,
	const Stuff::Point3D* point_data,
	const Stuff::RGBAColor* color_data
)
{
	Check_Pointer(this);
	usedNrOfVertices = count;
	Verify(*usedNrOfVertices <= maxNrOf);
	points = point_data;
	colors = color_data;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
void
MLRPointCloud::Draw(DrawEffectInformation* dInfo, GOSVertexPool* allVerticesToDraw, MLRSorter* sorter)
{
	Check_Object(this);
	worldToEffect.Invert(*dInfo->effectToWorld);
	Transform(*usedNrOfVertices, 1);
#if 0
	Lighting(*shape->worldToShape, dInfo->activeLights, dInfo->nrOfActiveLights);
#endif
	if(Clip(dInfo->clippingFlags, allVerticesToDraw))
	{
		sorter->AddEffect(this, dInfo->state);
	}
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
void
MLRPointCloud::Transform(size_t, size_t)
{
	Check_Object(this);
	Start_Timer(Transform_Time);
	size_t i;
	for(i = 0; i < *usedNrOfVertices; i++)
	{
		if(IsOn(i) == false)
		{
			continue;
		}
		(*transformedCoords)[i].Multiply(points[i], effectToClipMatrix);
	}
	Stop_Timer(Transform_Time);
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
uint32_t MLRPointCloud::Clip(MLRClippingState clippingFlags, GOSVertexPool* vt)
{
	//--------------------------------------
	// See if we don't have to draw anything
	//--------------------------------------
	//
	size_t i;
	numGOSVertices = 0;
	if(clippingFlags.GetClippingState() == 0 || usedNrOfVertices <= 0)
	{
		if(usedNrOfVertices <= 0)
		{
			visible = 0;
		}
		else
		{
			Check_Object(vt);
			gos_vertices = vt->GetActualVertexPool();
			for(i = 0; i < *usedNrOfVertices; i++)
			{
//				if(IsOn(i) == false)
//				{
//					continue;
//				}
				GOSCopyData(
					&gos_vertices[numGOSVertices],
					transformedCoords->GetData(),
					colors,
					i
				);
				numGOSVertices++;
			}
			Check_Object(vt);
			vt->Increase(numGOSVertices);
			visible = uint32_t(numGOSVertices ? 1 : 0);
		}
		return visible;
	}
	Check_Object(vt);
	gos_vertices = vt->GetActualVertexPool();
	Stuff::Vector4D* v4d = transformedCoords->GetData();
	for(i = 0; i < *usedNrOfVertices; i++, v4d++)
	{
//		if(IsOn(i) == false)
//		{
//			continue;
//		}
//		if( clippingFlags.IsFarClipped() && v4d->w < v4d->z)
//		{
//			continue;
//		}
//		if( clippingFlags.IsNearClipped() && v4d->z < 0.0f)
//		{
//			continue;
//		}
//		if( clippingFlags.IsRightClipped() && v4d->x < 0.0f)
//		{
//			continue;
//		}
//		if( clippingFlags.IsLeftClipped() && v4d->w < v4d->x)
//		{
//			continue;
//		}
//		if( clippingFlags.IsBottomClipped() && v4d->y < 0.0f)
//		{
//			continue;
//		}
//		if(clippingFlags.IsTopClipped() && v4d->w < v4d->y)
//		{
//			continue;
//		}
		GOSCopyData(
			&gos_vertices[numGOSVertices],
			transformedCoords->GetData(),
			colors,
			i
		);
//		if(
//			//clippingFlags.IsLeftClipped() &&
//			(gos_vertices[numGOSVertices].x + ((type)?type*4.f : 4.f)) > Environment.screenWidth
//			)
//		{
//			continue;
//		}
//
//		if(
//			//clippingFlags.IsTopClipped() &&
//			(gos_vertices[numGOSVertices].y + ((type)?type*4.f : 4.f)) > Environment.screenHeight
//			)
//		{
//			continue;
//		}
//
		numGOSVertices++;
	}
	vt->Increase(numGOSVertices);
	visible = uint32_t(numGOSVertices ? 1 : 0);
	if(visible)
	{
	}
	else
	{
	}
	return visible;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
void
MLRPointCloud::TestInstance(void) const
{
	if(usedNrOfVertices)
	{
		Verify(*usedNrOfVertices <= maxNrOf);
	}
}

