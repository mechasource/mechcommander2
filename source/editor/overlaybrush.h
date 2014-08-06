#ifndef OVERLAYBRUSH_H
#define OVERLAYBRUSH_H
/*************************************************************************************************\
OverlayBrush.h		: Interface for the OverlayBrush component.  This is the thing you use
						to create overlays
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//
\*************************************************************************************************/

#ifndef BRUSH_H
#include "Brush.h"
#endif

class ActionPaintTile;

//*************************************************************************************************

class OverlayBrush: public Brush
{
	public:

		OverlayBrush( int Type, int Offset = 0 )
		{ 
			type = (Overlays)Type;
			offset = Offset; 
			pAction = NULL; 
		}
		virtual ~OverlayBrush(){}	

		virtual void render( int ScreenMousex, int ScreenMouseY ){} // need to figure this out

		virtual bool canPaint( Stuff::Vector3D&, int screenX, int screenY, int& flags ){ return true; }
		virtual bool paint( Stuff::Vector3D& worldPos, int screenX, int screenY );

		bool beginPaint(); // not doing anything yet
		Action* endPaint();

	private:

		OverlayBrush( const OverlayBrush& verlayBrush );
		OverlayBrush& operator=( const OverlayBrush& verlayBrush );
		OverlayBrush();

		Overlays	type;		// the kind of overlay we are painting 
		int			offset;

		int lastTileR;
		int lastTileC;

		ActionPaintTile* pAction;

	


public:
		


};


//*************************************************************************************************
#endif  // end of file ( OverlayBrush.h )
