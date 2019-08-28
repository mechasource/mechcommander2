/*************************************************************************************************\
FlattenBrush.cpp			: Implementation of the FlattenBrush component.
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//
\*************************************************************************************************/

#include "FlattenBrush.h"

//-------------------------------------------------------------------------------------------------
FlattenBrush::FlattenBrush()
{
	pCurAction = nullptr;
}

//-------------------------------------------------------------------------------------------------
FlattenBrush::~FlattenBrush() {}

//-------------------------------------------------------------------------------------------------
bool
FlattenBrush::beginPaint()
{
	pCurAction = new ActionPaintTile();
	return true;
}

//-------------------------------------------------------------------------------------------------
Action*
FlattenBrush::endPaint()
{
	ActionPaintTile* pRetAction = pCurAction;
	if (pCurAction)
	{
		if (!pCurAction->vertexInfoList.Count())
		{
			delete pCurAction;
			pRetAction = nullptr;
		}
	}
	//	land->reCalcLight();
	land->recalcWater();
	pCurAction = nullptr;
	return pRetAction;
}

//-------------------------------------------------------------------------------------------------
bool
FlattenBrush::paint(Stuff::Vector3D& worldPos, int32_t screenX, int32_t screenY)
{
	int32_t closestX = floor((worldPos.x - land->mapTopLeft3d.x) / land->worldUnitsPerVertex + .5);
	int32_t closestY = floor((land->mapTopLeft3d.y - worldPos.y) / land->worldUnitsPerVertex + .5);
	float height = 0.f;
	float viable = 0.f;
	for (size_t i = closestX - 1; i < closestX + 2; ++i)
	{
		for (size_t j = closestY - 1; j < closestY + 2; ++j)
		{
			if (j < land->realVerticesMapSide && j > -1 && i < land->realVerticesMapSide && i > -1)
			{
				height += land->getTerrainElevation(j, i);
				viable += 1.0f;
			}
		}
	}
	height /= viable;
	if (closestY > -1 && closestY < land->realVerticesMapSide && closestX < land->realVerticesMapSide && closestX > -1)
		flattenVertex(closestY, closestX, height);
	return true;
}

//-------------------------------------------------------------------------------------------------
Action*
FlattenBrush::applyToSelection()
{
	float height = getAverageheightOfSelection();
	return applyheightToSelection(height);
}

//-------------------------------------------------------------------------------------------------
float
FlattenBrush::getAverageheightOfSelection()
{
	float height = 0.f;
	float count = 0.0f;
	for (size_t j = 0; j < land->realVerticesMapSide; ++j)
	{
		for (size_t i = 0; i < land->realVerticesMapSide; ++i)
		{
			if (land->isVertexSelected(j, i))
			{
				count += 1.f;
				height += land->getTerrainElevation(j, i);
			}
		}
	}
	height /= count;
	return height;
}

//-------------------------------------------------------------------------------------------------
Action*
FlattenBrush::applyheightToSelection(float height)
{
	beginPaint();
	for (size_t j = 0; j < land->realVerticesMapSide; ++j)
	{
		for (size_t i = 0; i < land->realVerticesMapSide; ++i)
		{
			if (land->isVertexSelected(j, i))
			{
				flattenVertex(j, i, height);
			}
		}
	}
	return endPaint();
}

//-------------------------------------------------------------------------------------------------
void
FlattenBrush::flattenVertex(int32_t row, int32_t col, float val)
{
#if 1 /*flattening without "area effect"*/
	{
		int32_t i = col;
		int32_t j = row;
		if (i > -1 && i < land->realVerticesMapSide && j > -1 && j < land->realVerticesMapSide)
		{
			pCurAction->addChangedVertexInfo(j, i);
			land->setVertexheight(j * land->realVerticesMapSide + i, val);
		}
	}
#else /*flattening without "area effect"*/
	for (size_t i = col - 1; i < col + 2; ++i)
	{
		for (size_t j = row - 1; j < row + 2; ++j)
		{
			if (i > -1 && i < land->realVerticesMapSide && j > -1 && j < land->realVerticesMapSide)
			{
				pCurAction->addChangedVertexInfo(j, i);
				land->setVertexheight(j * land->realVerticesMapSide + i, val);
			}
		}
	}
#endif /*flattening without "area effect"*/
}

// end of file ( FlattenBrush.cpp )
