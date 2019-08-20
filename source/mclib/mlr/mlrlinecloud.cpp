//===========================================================================//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#include "stdinc.h"

#include <gameos.hpp>
#include <mlr/mlrsorter.hpp>
#include <mlr/mlrclipper.hpp>
#include <mlr/mlrprimitivebase.hpp>
#include <mlr/mlrcliptrick.hpp>
#include <mlr/mlrlinecloud.hpp>

using namespace MidLevelRenderer;

//#############################################################################
//#########################    MLRLineCloud    ###############################
//#############################################################################

MLRLineCloud::ClassData* MLRLineCloud::DefaultData = nullptr;

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
void
MLRLineCloud::InitializeClass()
{
	_ASSERT(!DefaultData);
	// _ASSERT(gos_GetCurrentHeap() == StaticHeap);
	DefaultData = new ClassData(
		MLRLineCloudClassID, "MidLevelRenderer::MLRLineCloud", MLREffect::DefaultData);
	Register_Object(DefaultData);
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
void
MLRLineCloud::TerminateClass()
{
	Unregister_Object(DefaultData);
	delete DefaultData;
	DefaultData = nullptr;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
MLRLineCloud::MLRLineCloud(uint32_t nr, uint32_t _type) :
	MLREffect(uint32_t(2 * nr), DefaultData), type(_type)
{
	// _ASSERT(gos_GetCurrentHeap() == Heap);
	usedNrOfVertices = 0;
	// Check_Pointer(this);
	drawMode = SortData::LineCloud;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
MLRLineCloud::~MLRLineCloud()
{
	// Check_Object(this);
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
void
MLRLineCloud::SetData(
	pcsize_t count, const Stuff::Point3D* point_data, const Stuff::RGBAColor* color_data)
{
	// Check_Pointer(this);
	usedNrOfVertices = count;
	_ASSERT(*usedNrOfVertices <= maxNrOf);
	points = point_data;
	colors = color_data;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
void
MLRLineCloud::Draw(
	DrawEffectInformation* dInfo, GOSVertexPool* allVerticesToDraw, MLRSorter* sorter)
{
	// Check_Object(this);
	worldToEffect.Invert(*dInfo->effectToWorld);
	Transform(*usedNrOfVertices, 1);
#if 0
	Lighting(*shape->worldToShape, dInfo->activeLights, dInfo->nrOfActiveLights);
#endif
	if (Clip(dInfo->clippingFlags, allVerticesToDraw))
	{
		sorter->AddEffect(this, dInfo->state);
	}
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
void MLRLineCloud::Transform(size_t, size_t)
{
	// Check_Object(this);
	Start_Timer(Transform_Time);
	size_t i;
	for (i = 0; i < *usedNrOfVertices; i++)
	{
		if (IsOn(i) == false)
		{
			continue;
		}
		(*transformedCoords)[i].Multiply(points[i], effectToClipMatrix);
	}
	Stop_Timer(Transform_Time);
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
uint32_t
MLRLineCloud::Clip(MLRClippingState clippingFlags, GOSVertexPool* vt)
{
	//--------------------------------------
	// See if we don't have to draw anything
	//--------------------------------------
	//
	size_t i;
	numGOSVertices = 0;
	if (clippingFlags.GetClippingState() == 0 || usedNrOfVertices <= 0)
	{
		if (usedNrOfVertices <= 0)
		{
			visible = 0;
		}
		else
		{
			Check_Object(vt);
			gos_vertices = vt->GetActualVertexPool();
			for (i = 0; i < *usedNrOfVertices; i++)
			{
				if (IsOn(i) == false)
				{
					continue;
				}
				GOSCopyData(&gos_vertices[numGOSVertices], transformedCoords->GetData(), colors, i);
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
	MLRClippingState theAnd, theOr, startClip, endClip;
	for (i = 0; i < *usedNrOfVertices; i += 2)
	{
		//		if(IsOn(i) == false)
		//		{
		//			continue;
		//		}
		startClip.Clip4dVertex(v4d + i);
		endClip.Clip4dVertex(v4d + i + 1);
		theAnd = *(puint32_t)&startClip & *(puint32_t)&endClip;
		theOr = *(puint32_t)&startClip | *(puint32_t)&endClip;
		theAnd = theOr = 0; // ASSUME NO CLIPPING NEEDED FOR MC2.  Its just not done here!
		if (theAnd != 0)
		{
			continue;
		}
		if (theOr == 0)
		{
			GOSCopyData(&gos_vertices[numGOSVertices], v4d, colors, i);
			numGOSVertices++;
			GOSCopyData(&gos_vertices[numGOSVertices], v4d, colors, i + 1);
			numGOSVertices++;
			continue;
		}
		float a = 0.0f;
		uint32_t l, ct = 0;
		uint32_t mask;
		if (theOr.GetNumberOfSetBits() == 1)
		{
			Stuff::Vector4D p4d;
			Stuff::RGBAColor cc;
			mask = 1;
			ct = 0;
			a = 0.0f;
			for (l = 0; l < MLRClippingState::NextBit; l++)
			{
				if (theOr.IsClipped(mask))
				{
					//
					//-------------------------------------------
					// Find the clipping interval from bc0 to bc1
					//-------------------------------------------
					//
					a = GetLerpFactor(l, (*transformedCoords)[i], (*transformedCoords)[i + 1]);
					_ASSERT(a >= 0.0f && a <= 1.0f);
					ct = l;
					break;
				}
				mask <<= 1;
			}
			p4d.Lerp((*transformedCoords)[i], (*transformedCoords)[i + 1], a);
			DoClipTrick(p4d, ct);
			cc.Lerp(colors[i], colors[i + 1], a);
			if (startClip == 0)
			{
				GOSCopyData(&gos_vertices[numGOSVertices], transformedCoords->GetData(), colors, i);
				numGOSVertices++;
				GOSCopyData(&gos_vertices[numGOSVertices], &p4d, &cc, 0);
				numGOSVertices++;
			}
			else
			{
				GOSCopyData(&gos_vertices[numGOSVertices], &p4d, &cc, 0);
				numGOSVertices++;
				GOSCopyData(
					&gos_vertices[numGOSVertices], transformedCoords->GetData(), colors, i + 1);
				numGOSVertices++;
			}
		}
		else
		{
			Stuff::Vector4D p4d0, p4d1;
			Stuff::RGBAColor cc0, cc1;
			p4d0 = (*transformedCoords)[i];
			p4d1 = (*transformedCoords)[i + 1];
			cc0 = colors[i];
			cc1 = colors[i + 1];
			mask = 1;
			ct = 0;
			a = 0.0f;
			for (l = 0; l < MLRClippingState::NextBit; l++)
			{
				if (theOr.IsClipped(mask))
				{
					//
					//-----------------------------------
					// Clip each vertex against the plane
					//-----------------------------------
					//
					if (startClip.IsClipped(mask))
					{
						a = GetLerpFactor(l, p4d0, p4d1);
						_ASSERT(a >= 0.0f && a <= 1.0f);
						p4d0.Lerp(p4d0, p4d1, a);
						DoClipTrick(p4d0, l);
						startClip.Clip4dVertex(&p4d0);
						cc0.Lerp(cc0, cc1, a);
					}
					if (endClip.IsClipped(mask))
					{
						a = GetLerpFactor(l, p4d0, p4d1);
						_ASSERT(a >= 0.0f && a <= 1.0f);
						p4d1.Lerp(p4d0, p4d1, a);
						DoClipTrick(p4d1, l);
						endClip.Clip4dVertex(&p4d1);
						cc1.Lerp(cc0, cc1, a);
					}
				}
				if (*(pint32_t)&startClip & *(pint32_t)&endClip)
				{
					break;
				}
				mask = mask << 1;
			}
			if (l == MLRClippingState::NextBit)
			{
				GOSCopyData(&gos_vertices[numGOSVertices], &p4d0, &cc0, 0);
				numGOSVertices++;
				GOSCopyData(&gos_vertices[numGOSVertices], &p4d1, &cc1, 0);
				numGOSVertices++;
			}
		}
	}
	vt->Increase(numGOSVertices);
	visible = uint32_t(numGOSVertices ? 1 : 0);
	return visible;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
void
MLRLineCloud::TestInstance(void) const
{
	if (usedNrOfVertices)
	{
		_ASSERT(*usedNrOfVertices <= maxNrOf);
	}
}
