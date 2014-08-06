#ifndef TERRAINBRUSH_H
#define TERRAINBRUSH_H
/*************************************************************************************************\
TerrainBrush.h		: Interface for the TerrainBrush component. used to paint textures
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//
\*************************************************************************************************/

#ifndef BRUSH_H
#include "Brush.h"
#endif
#include "Action.h"

class TerrainBrush: public Brush
{
	public:

		inline TerrainBrush( int Type )
		{
			if ( Type == -1 )
				Type = s_lastType;

			terrainType = Type;
			s_lastType = Type;
			pAction = NULL;
		}
		virtual ~TerrainBrush(){}

		bool beginPaint()
		{
			if ( pAction )
			{
				gosASSERT( false );
			}

			pAction = new ActionPaintTile;

			gosASSERT( pAction );

			return true;
		}
		Action* endPaint()
		{
			Action* pRetAction = pAction;
			pAction  = NULL;
			return pRetAction;
		}
		virtual bool paint( Stuff::Vector3D& worldPos, int screenX, int screenY )
		{
			long tileC;
			long tileR;

			Stuff::Vector2DOf<long> screenPos( screenX, screenY );

			eye->getClosestVertex( screenPos, tileR, tileC );

			if ( tileR < Terrain::realVerticesMapSide && tileR > -1 
				&& tileC < Terrain::realVerticesMapSide && tileC > -1 )
			{
				pAction->addChangedVertexInfo( tileR, tileC );	// for undo
				land->setTerrain( tileR, tileC, terrainType );
				return true;
			}

			return false;
		}
		virtual bool canPaint( Stuff::Vector3D& worldPos, int screenX, int screenY, int flags ) { return true; } 

		virtual Action* applyToSelection();



	private:

		// SUPPRESS THESE!
		TerrainBrush( const TerrainBrush& TerrainBrush );
		TerrainBrush& operator=( const TerrainBrush& TerrainBrush );

		int terrainType;

		static int s_lastType;

		ActionPaintTile* pAction;
};


//*************************************************************************************************
#endif  // end of file ( TerrainBrush.h )
