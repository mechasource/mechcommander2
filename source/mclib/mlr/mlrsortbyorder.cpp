//===========================================================================//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#include "stdinc.h"

#include "mlr/mlrsortbyorder.h"

namespace MidLevelRenderer
{

//#############################################################################
//############################    MLRSortByOrder
//################################
//#############################################################################

extern uint32_t gEnableTextureSort, gEnableAlphaSort, gEnableLightMaps;
MLRSortByOrder::ClassData* MLRSortByOrder::DefaultData = nullptr;

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
void MLRSortByOrder::InitializeClass()
{
	_ASSERT(!DefaultData);
	// _ASSERT(gos_GetCurrentHeap() == StaticHeap);
	DefaultData = new ClassData(
		MLRSortByOrderClassID, "MidLevelRenderer::MLRSortByOrder", MLRSorter::DefaultData);
	Register_Object(DefaultData);
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
void MLRSortByOrder::TerminateClass()
{
	Unregister_Object(DefaultData);
	delete DefaultData;
	DefaultData = nullptr;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
MLRSortByOrder::MLRSortByOrder(MLRTexturePool* tp)
	: MLRSorter(DefaultData, tp)
{
	// _ASSERT(gos_GetCurrentHeap() == Heap);
	int32_t i;
	gos_PushCurrentHeap(StaticHeap);
	for (i = 0; i < MLRState::PriorityCount; i++)
	{
		lastUsedInBucket[i] = 0;
		priorityBuckets[i].SetLength(
			Limits::Max_Number_Primitives_Per_Frame + Limits::Max_Number_ScreenQuads_Per_Frame);
	}
	alphaSort.SetLength(2 * Limits::Max_Number_Vertices_Per_Frame);
	for (i = 0; i < alphaSort.GetLength(); i++)
	{
		alphaSort[i] = new SortAlpha;
	}
	gos_PopCurrentHeap();
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
MLRSortByOrder::~MLRSortByOrder()
{
	int32_t i;
	gos_PushCurrentHeap(StaticHeap);
	for (i = 0; i < alphaSort.GetLength(); i++)
	{
		delete alphaSort[i];
	}
	alphaSort.SetLength(0);
	gos_PopCurrentHeap();
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
void MLRSortByOrder::Reset()
{
	// Check_Object(this);
	int32_t i;
	for (i = 0; i < MLRState::PriorityCount; i++)
	{
		lastUsedInBucket[i] = 0;
	}
	MLRSorter::Reset();
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
void MLRSortByOrder::AddPrimitive(MLRPrimitiveBase* pt, uint32_t pass)
{
	// Check_Object(this);
	Check_Object(pt);
	SortData* sd = nullptr;
	switch (pt->GetSortDataMode())
	{
	case SortData::TriList:
	case SortData::TriIndexedList:
		sd = SetRawData(pt, pass);
		break;
	}
	uint32_t priority = pt->GetCurrentState(pass).GetPriority();
	if (sd != nullptr)
	{
		priorityBuckets[priority][lastUsedInBucket[priority]++] = sd;
	}
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
void MLRSortByOrder::AddEffect(MLREffect* ef, const MLRState& state)
{
	// Check_Object(this);
	Check_Object(ef);
	SortData* sd = nullptr;
	switch (ef->GetSortDataMode())
	{
	case SortData::PointCloud:
		sd = SetRawData(ef->GetGOSVertices(), ef->GetNumGOSVertices(), state, SortData::PointCloud);
		sd->numIndices = ef->GetType(0);
		break;
	case SortData::LineCloud:
		sd = SetRawData(ef->GetGOSVertices(), ef->GetNumGOSVertices(), state, SortData::LineCloud);
		sd->numIndices = ef->GetType(0);
		break;
	case SortData::TriList:
		sd = SetRawData(ef->GetGOSVertices(), ef->GetNumGOSVertices(), state, SortData::TriList);
		break;
	case SortData::TriIndexedList:
	{
		MLRIndexedTriangleCloud* itc = Cast_Object(MLRIndexedTriangleCloud*, ef);
		sd = SetRawIndexedData(itc->GetGOSVertices(), itc->GetNumGOSVertices(),
			itc->GetGOSIndices(), itc->GetNumGOSIndices(), state, SortData::TriIndexedList);
	}
	break;
	}
	uint32_t priority = state.GetPriority();
	if (sd != nullptr)
	{
		priorityBuckets[priority][lastUsedInBucket[priority]++] = sd;
	}
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
void MLRSortByOrder::AddScreenQuads(GOSVertex* vertices, const DrawScreenQuadsInformation* dInfo)
{
	_ASSERT(dInfo->currentNrOfQuads != 0 && (dInfo->currentNrOfQuads & 3) == 0);
	SortData* sd = SetRawData(vertices, dInfo->currentNrOfQuads, dInfo->state, SortData::Quads);
	uint32_t priority = dInfo->state.GetPriority();
	if (sd != nullptr)
	{
		priorityBuckets[priority][lastUsedInBucket[priority]++] = sd;
	}
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
void MLRSortByOrder::AddSortRawData(SortData* sd)
{
	// Check_Object(this);
	if (sd == nullptr)
	{
		return;
	}
	uint32_t priority = sd->state.GetPriority();
	priorityBuckets[priority][lastUsedInBucket[priority]++] = sd;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
void MLRSortByOrder::RenderNow()
{
	// Check_Object(this);
	//
	// So GameOS knows how int32_t the transform and clip and lighting took of
	// update renderers
	//
	std::vector<SortData*>* priorityBucket;
	std::vector<ToBeDrawnPrimitive*>* priorityBucketNotDrawn;
	GOSVertex::farClipReciprocal = farClipReciprocal;
	int32_t i, j, k;
	//	Limits::Max_Number_Primitives_Per_Frame +
	// Max_Number_ScreenQuads_Per_Frame
	MLRPrimitiveBase* primitive;
	for (i = 0; i < MLRState::PriorityCount; i++)
	{
#ifdef CalDraw
		ToBeDrawnPrimitive* tbdp;
		int32_t alphaToSort = 0;
		if (lastUsedInBucketNotDrawn[i])
		{
			_ASSERT(lastUsedInBucketNotDrawn[i] <= Limits::Max_Number_Primitives_Per_Frame + Limits::Max_Number_ScreenQuads_Per_Frame);
			if (gEnableTextureSort && i != MLRState::AlphaPriority)
			{
				Start_Timer(Texture_Sorting_Time);
				// do a shell sort
				int32_t ii, jj, hh;
				ToBeDrawnPrimitive* tempSortData;
				priorityBucketNotDrawn = &priorityBucketsNotDrawn[i];
				for (hh = 1; hh < lastUsedInBucketNotDrawn[i] / 9; hh = 3 * hh + 1)
					;
				for (; hh > 0; hh /= 3)
				{
					for (ii = hh; ii < lastUsedInBucketNotDrawn[i]; ii++)
					{
						tempSortData = (*priorityBucketNotDrawn)[ii];
						jj = ii;
						while (jj >= hh && (*priorityBucketNotDrawn)[jj - hh]->state.GetTextureHandle() > tempSortData->state.GetTextureHandle())
						{
							(*priorityBucketNotDrawn)[jj] = (*priorityBucketNotDrawn)[jj - hh];
							jj -= hh;
						}
						(*priorityBucketNotDrawn)[jj] = tempSortData;
					}
				}
				Stop_Timer(Texture_Sorting_Time);
			}
			if (i != MLRState::AlphaPriority)
			{
				for (j = 0; j < lastUsedInBucketNotDrawn[i]; j++)
				{
					tbdp = priorityBucketsNotDrawn[i][j];
					Check_Pointer(tbdp);
					primitive = tbdp->primitive;
					Check_Object(primitive);
					int32_t nrOfLightMaps = 0;
					for (k = 0; k < tbdp->nrOfActiveLights; k++)
					{
						Check_Object(tbdp->activeLights[k]);
						nrOfLightMaps += (tbdp->activeLights[k]->GetLightMap() != nullptr) ? 1 : 0;
						tbdp->activeLights[k]->SetLightToShapeMatrix(tbdp->worldToShape);
					}
					if (!gEnableLightMaps)
					{
						nrOfLightMaps = 0;
					}
					if (nrOfLightMaps)
					{
						MLRLightMap::SetDrawData(ToBeDrawnPrimitive::allVerticesToDraw,
							&tbdp->shapeToClipMatrix, tbdp->clippingFlags, tbdp->state);
					}
					if (primitive->FindBackFace(tbdp->cameraPosition))
					{
						primitive->Lighting(tbdp->activeLights, tbdp->nrOfActiveLights);
						if (tbdp->clippingFlags.GetClippingState() != 0)
						{
							if (primitive->TransformAndClip(&tbdp->shapeToClipMatrix,
									tbdp->clippingFlags, ToBeDrawnPrimitive::allVerticesToDraw,
									true))
							{
								if (primitive->GetVisible())
								{
									for (k = 0; k < primitive->GetNumPasses(); k++)
									{
										DrawPrimitive(primitive, k);
									}
								}
							}
						}
						else
						{
							primitive->TransformNoClip(&tbdp->shapeToClipMatrix,
								ToBeDrawnPrimitive::allVerticesToDraw, true);
							for (k = 0; k < primitive->GetNumPasses(); k++)
							{
								DrawPrimitive(primitive, k);
							}
						}
#ifdef LAB_ONLY
						Set_Statistic(Number_Of_Primitives, Number_Of_Primitives + 1);
						if (primitive->IsDerivedFrom(MLRIndexedPrimitiveBase::DefaultData))
						{
							Point3D* coords;
							uint16_t* indices;
							int32_t nr;
							(Cast_Pointer(MLRIndexedPrimitiveBase*, primitive))
								->GetIndexData(&indices, &nr);
							Set_Statistic(NumAllIndices, NumAllIndices + nr);
							primitive->GetCoordData(&coords, &nr);
							Set_Statistic(NumAllVertices, NumAllVertices + nr);
							Set_Statistic(Index_Over_Vertex_Ratio,
								(float)NumAllIndices / (float)NumAllVertices);
						}
#endif
					}
					if (nrOfLightMaps)
					{
						MLRLightMap::DrawLightMaps(this);
					}
				}
			}
			else
			{
				SortData* sd = nullptr;
				for (j = 0; j < lastUsedInBucketNotDrawn[i]; j++)
				{
					tbdp = priorityBucketsNotDrawn[i][j];
					primitive = tbdp->primitive;
					if (primitive->FindBackFace(tbdp->cameraPosition))
					{
						primitive->Lighting(tbdp->activeLights, tbdp->nrOfActiveLights);
						if (tbdp->clippingFlags.GetClippingState() != 0)
						{
							if (primitive->TransformAndClip(&tbdp->shapeToClipMatrix,
									tbdp->clippingFlags, ToBeDrawnPrimitive::allVerticesToDraw,
									true))
							{
								if (!primitive->GetVisible())
								{
									continue;
								}
							}
						}
						else
						{
							primitive->TransformNoClip(&tbdp->shapeToClipMatrix,
								ToBeDrawnPrimitive::allVerticesToDraw, true);
						}
#ifdef LAB_ONLY
						Set_Statistic(Number_Of_Primitives, Number_Of_Primitives + 1);
						if (primitive->IsDerivedFrom(MLRIndexedPrimitiveBase::DefaultData))
						{
							Point3D* coords;
							uint16_t* indices;
							int32_t nr;
							(Cast_Pointer(MLRIndexedPrimitiveBase*, primitive))
								->GetIndexData(&indices, &nr);
							Set_Statistic(NumAllIndices, NumAllIndices + nr);
							primitive->GetCoordData(&coords, &nr);
							Set_Statistic(NumAllVertices, NumAllVertices + nr);
							Set_Statistic(Index_Over_Vertex_Ratio,
								(float)NumAllIndices / (float)NumAllVertices);
						}
#endif
					}
					else
					{
						continue;
					}
					if (primitive->GetNumGOSVertices() > 0)
						for (k = 0; k < primitive->GetNumPasses(); k++)
						{
							sd = SetRawData(primitive, k);
							Check_Pointer(sd);
							if (gEnableAlphaSort && (sd->type == SortData::TriList || sd->type == SortData::TriIndexedList))
							{
								SortData::LoadSortAlphaFunc alphaFunc = sd->LoadSortAlpha[sd->type];
								_ASSERT(alphaToSort + sd->numvertices / 3 < 2 * Limits::Max_Number_Vertices_Per_Frame);
								alphaToSort += (sd->*alphaFunc)(alphaSort.GetData() + alphaToSort);
							}
							else
							{
								if (theCurrentState != sd->state)
								{
									SetDifferences(theCurrentState, sd->state);
									theCurrentState = sd->state;
								}
								SortData::DrawFunc drawFunc = sd->Draw[sd->type];
								(sd->*drawFunc)();
							}
						}
				}
			}
		}
#endif
		if (lastUsedInBucket[i])
		{
			_ASSERT(lastUsedInBucket[i] <= Limits::Max_Number_Primitives_Per_Frame + Limits::Max_Number_ScreenQuads_Per_Frame);
			if (gEnableTextureSort && i != MLRState::AlphaPriority)
			{
				Start_Timer(Texture_Sorting_Time);
				// do a shell sort
				int32_t ii, jj, hh;
				SortData* tempSortData;
				priorityBucket = &priorityBuckets[i];
				for (hh = 1; hh < lastUsedInBucket[i] / 9; hh = 3 * hh + 1)
					;
				for (; hh > 0; hh /= 3)
				{
					for (ii = hh; ii < lastUsedInBucket[i]; ii++)
					{
						tempSortData = (*priorityBucket)[ii];
						jj = ii;
						while (jj >= hh && (*priorityBucket)[jj - hh]->state.GetTextureHandle() > tempSortData->state.GetTextureHandle())
						{
							(*priorityBucket)[jj] = (*priorityBucket)[jj - hh];
							jj -= hh;
						}
						(*priorityBucket)[jj] = tempSortData;
					}
				}
				Stop_Timer(Texture_Sorting_Time);
			}
			if (i != MLRState::AlphaPriority)
			{
				for (j = 0; j < lastUsedInBucket[i]; j++)
				{
					SortData* sd = priorityBuckets[i][j];
					Check_Pointer(sd);
					if (theCurrentState != sd->state)
					{
						SetDifferences(theCurrentState, sd->state);
						theCurrentState = sd->state;
					}
					SortData::DrawFunc drawFunc = sd->Draw[sd->type];
					//				_clear87();
					(sd->*drawFunc)();
					//				_clear87();
				}
			}
			else
			{
				for (j = 0; j < lastUsedInBucket[i]; j++)
				{
					SortData* sd = priorityBuckets[i][j];
					Check_Pointer(sd);
					if (gEnableAlphaSort && (sd->type == SortData::TriList || sd->type == SortData::TriIndexedList))
					{
						SortData::LoadSortAlphaFunc alphaFunc = sd->LoadSortAlpha[sd->type];
						_ASSERT(alphaToSort + sd->numvertices / 3 < 2 * Limits::Max_Number_Vertices_Per_Frame);
						alphaToSort += (sd->*alphaFunc)(alphaSort.GetData() + alphaToSort);
					}
					else
					{
						if (theCurrentState != sd->state)
						{
							SetDifferences(theCurrentState, sd->state);
							theCurrentState = sd->state;
						}
						SortData::DrawFunc drawFunc = sd->Draw[sd->type];
						(sd->*drawFunc)();
					}
				}
			}
		}
		if (alphaToSort > 0)
		{
			Start_Timer(Alpha_Sorting_Time);
			// do a shell sort
			int32_t ii, jj, hh;
			SortAlpha* tempSortAlpha;
			std::vector<SortAlpha*>* alphaArray;
			alphaArray = &alphaSort;
			for (hh = 1; hh < alphaToSort / 9; hh = 3 * hh + 1)
				;
			for (; hh > 0; hh /= 3)
			{
				for (ii = hh; ii < alphaToSort; ii++)
				{
					tempSortAlpha = (*alphaArray)[ii];
					jj = ii;
					while (jj >= hh && (*alphaArray)[jj - hh]->distance < tempSortAlpha->distance)
					{
						(*alphaArray)[jj] = (*alphaArray)[jj - hh];
						jj -= hh;
					}
					(*alphaArray)[jj] = tempSortAlpha;
				}
			}
			Stop_Timer(Alpha_Sorting_Time);
			for (ii = 0; ii < alphaToSort; ii++)
			{
				if (theCurrentState != *alphaSort[ii]->state)
				{
					SetDifferences(theCurrentState, *alphaSort[ii]->state);
					theCurrentState = *alphaSort[ii]->state;
				}
				Start_Timer(GOS_Draw_Time);
				if ((alphaSort[ii]->triangle[0].z >= 0.0f) && (alphaSort[ii]->triangle[0].z < 1.0f) && (alphaSort[ii]->triangle[1].z >= 0.0f) && (alphaSort[ii]->triangle[1].z < 1.0f) && (alphaSort[ii]->triangle[2].z >= 0.0f) && (alphaSort[ii]->triangle[2].z < 1.0f))
				{
					gos_DrawTriangles(&(alphaSort[ii]->triangle[0]), 3);
				}
				Stop_Timer(GOS_Draw_Time);
			}
			Set_Statistic(NumberOfAlphaSortedTriangles, NumberOfAlphaSortedTriangles + alphaToSort);
		}
	}
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
void MLRSortByOrder::TestInstance(void) const
{
	_ASSERT(IsDerivedFrom(DefaultData));
}

} // namespace MidLevelRenderer
