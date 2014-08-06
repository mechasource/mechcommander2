#ifndef DRAGTOOL_H
#define DRAGTOOL_H
/*************************************************************************************************\
DragTool.h			: Interface for the DragTool component.  When you hold down the space bar,
						this thing lets you scroll around		
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//
\*************************************************************************************************/

#ifndef BRUSH_H
#include "Brush.h"
#endif

class DragTool: public Brush
{
	public:

		DragTool();
		virtual ~DragTool();

		virtual bool beginPaint();
		virtual Action* endPaint();
		virtual bool paint( Stuff::Vector3D& worldPos, int screenX, int screenY  );
		virtual bool canPaint( Stuff::Vector3D& worldPos, int screenX, int screenY, int flags ) { return true; } 
		virtual bool canPaintSelection( ){ return false; }


	private:

		int lastX;
		int lastY;


};


//*************************************************************************************************
#endif  // end of file ( DragTool.h )
