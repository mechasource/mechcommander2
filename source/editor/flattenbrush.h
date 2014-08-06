#ifndef FLATTENBRUSH_H
#define FLATTENBRUSH_H
/*************************************************************************************************\
FlattenBrush.h			: Interface for the FlattenBrush component.
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//
\*************************************************************************************************/

#ifndef BRUSH_H
#include "Brush.h"
#endif

#ifndef ACTION_H
#include "Action.h"
#endif

//*************************************************************************************************

/**************************************************************************************************
CLASS DESCRIPTION
FlattenBrush:
**************************************************************************************************/
class FlattenBrush: public Brush
{
	public:

		FlattenBrush();
		virtual ~FlattenBrush();

		virtual bool beginPaint();
		virtual Action* endPaint();
		virtual bool paint( Stuff::Vector3D& worldPos, int screenX, int screenY );
		virtual bool canPaint( Stuff::Vector3D& worldPos, int screenX, int screenY, int flags ) { return true; } 
		virtual bool canPaintSelection( ){ return true; }
		virtual Action* applyToSelection();

		Action* applyHeightToSelection( float height );
		float	getAverageHeightOfSelection( );


	private:

		ActionPaintTile*	pCurAction;
		void flattenVertex( int row, int col, float val );

};


//*************************************************************************************************
#endif  // end of file ( FLATTENBRUSH_H.h )
