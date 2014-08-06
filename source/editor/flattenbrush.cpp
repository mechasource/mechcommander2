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
	pCurAction = NULL;
}

//-------------------------------------------------------------------------------------------------
FlattenBrush::~FlattenBrush()
{
}

//-------------------------------------------------------------------------------------------------
bool FlattenBrush::beginPaint()
{
	pCurAction = new ActionPaintTile();
	return true;
}

//-------------------------------------------------------------------------------------------------
Action* FlattenBrush::endPaint()
{
	ActionPaintTile* pRetAction = pCurAction;
	if (pCurAction)
	{
		if ( !pCurAction->vertexInfoList.Count() )
		{
			delete pCurAction;
			pRetAction = NULL;

		}
	}

//	land->reCalcLight();
	land->recalcWater();
	pCurAction = NULL;
	return pRetAction;
}

//-------------------------------------------------------------------------------------------------
bool FlattenBrush::paint( Stuff::Vector3D& worldPos, int screenX, int screenY )
{
	int closestX = floor( (worldPos.x - land->mapTopLeft3d.x)/land->worldUnitsPerVertex + .5 );
	int closestY = floor( (land->mapTopLeft3d.y - worldPos.y)/land->worldUnitsPerVertex + .5 );

	float height = 0.f;

	float viable = 0.f;

	for ( int i = closestX - 1; i < closestX + 2; ++i )
	{
		for ( int j = closestY - 1; j < closestY + 2; ++j )
		{
			if ( j < land->realVerticesMapSide && j > -1 
				&& i < land->realVerticesMapSide && i > -1 )
			{
				height += land->getTerrainElevation( j, i );
				viable += 1.0f;
			}
		}
	}

	height/=viable;

	if ( closestY > -1 && closestY < land->realVerticesMapSide 
		&& closestX < land->realVerticesMapSide && closestX > -1 )
		flattenVertex( closestY, closestX, height );

	return true;
}

//-------------------------------------------------------------------------------------------------
Action* FlattenBrush::applyToSelection()
{

	float height = getAverageHeightOfSelection();

	return applyHeightToSelection( height );

}

//-------------------------------------------------------------------------------------------------
float FlattenBrush::getAverageHeightOfSelection( )
{
	float height = 0.f;
	float count = 0.0f;

	for ( int j = 0; j < land->realVerticesMapSide; ++j )
	{
		for ( int i = 0; i < land->realVerticesMapSide; ++i )
		{
			if ( land->isVertexSelected( j, i ) )
			{
				count += 1.f;
				height += land->getTerrainElevation( j, i );
			}
		}
	}

	height /= count;

	return height;

}

//-------------------------------------------------------------------------------------------------
Action* FlattenBrush::applyHeightToSelection( float height )
{
	beginPaint();

	for ( int  j = 0; j < land->realVerticesMapSide; ++j )
	{
		for ( int i = 0; i < land->realVerticesMapSide; ++i )
		{
			if ( land->isVertexSelected( j, i ) )
			{
				flattenVertex( j, i, height );
			}
		}
	}

	return endPaint();

}

//-------------------------------------------------------------------------------------------------
void FlattenBrush::flattenVertex( int row, int col, float val )
{
#if 1 /*flattening without "area effect"*/
	{
		int i = col;
		int j = row;
		if ( i > -1 && i < land->realVerticesMapSide 
			&& j > -1 && j < land->realVerticesMapSide )
		{
			pCurAction->addChangedVertexInfo( j, i );
			land->setVertexHeight( j * land->realVerticesMapSide + i, val );
		}
	}
#else /*flattening without "area effect"*/
	for ( int i = col - 1; i < col + 2; ++i )
	{
		for ( int j = row - 1; j < row + 2; ++j )
		{
			if ( i > -1 && i < land->realVerticesMapSide 
				&& j > -1 && j < land->realVerticesMapSide )
			{
				pCurAction->addChangedVertexInfo( j, i );
				land->setVertexHeight( j * land->realVerticesMapSide + i, val );
			}
		}
	}
#endif /*flattening without "area effect"*/
}


//*************************************************************************************************
// end of file ( FlattenBrush.cpp )
