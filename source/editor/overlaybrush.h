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

		OverlayBrush( int32_t Type, int32_t Offset = 0 )
		{ 
			type = (Overlays)Type;
			offset = Offset; 
			pAction = NULL; 
		}
		virtual ~OverlayBrush(){}	

		virtual void render( int32_t ScreenMousex, int32_t ScreenMouseY ){} // need to figure this out

		virtual bool canPaint( Stuff::Vector3D&, int32_t screenX, int32_t screenY, int32_t& flags ){ return true; }
		virtual bool paint( Stuff::Vector3D& worldPos, int32_t screenX, int32_t screenY );

		bool beginPaint(); // not doing anything yet
		Action* endPaint();

	private:

		OverlayBrush( const OverlayBrush& verlayBrush );
		OverlayBrush& operator=( const OverlayBrush& verlayBrush );
		OverlayBrush();

		Overlays	type;		// the kind of overlay we are painting 
		int32_t			offset;

		int32_t lastTileR;
		int32_t lastTileC;

		ActionPaintTile* pAction;

	


public:
		


};


//*************************************************************************************************
#endif  // end of file ( OverlayBrush.h )
