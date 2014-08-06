#define TERRAINBRUSH_CPP
/*************************************************************************************************\
terrainBrush.cpp			: Implementation of the terrainBrush component.
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//
\*************************************************************************************************/

#include "terrainBrush.h"

int TerrainBrush::s_lastType = 0;

Action* TerrainBrush::applyToSelection()
{
	ActionPaintTile* pRetAction = new ActionPaintTile();
	for ( int i = 0; i < land->realVerticesMapSide; ++i )
	{
		for ( int j = 0; j < land->realVerticesMapSide; ++j )
		{
			if ( land->isVertexSelected( j, i ) )
			{
				pRetAction->addChangedVertexInfo( j, i );
				land->setTerrain( j, i, terrainType );
			}
		}
	}

	return pRetAction;
}
//*************************************************************************************************
// end of file ( terrainBrush.cpp )
