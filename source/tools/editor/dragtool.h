/*************************************************************************************************\
DragTool.h			: Interface for the DragTool component.  When you hold down the space bar,
						this thing lets you scroll around		
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//
\*************************************************************************************************/

#pragma once

#ifndef DRAGTOOL_H
#define DRAGTOOL_H

// #include "brush.h"

class DragTool: public Brush
{
	public:

		DragTool();
		virtual ~DragTool();

		virtual bool beginPaint();
		virtual Action* endPaint();
		virtual bool paint( Stuff::Vector3D& worldPos, int32_t screenX, int32_t screenY  );
		virtual bool canPaint( Stuff::Vector3D& worldPos, int32_t screenX, int32_t screenY, int32_t flags ) { return true; } 
		virtual bool canPaintSelection( ){ return false; }


	private:

		int32_t lastX;
		int32_t lastY;


};


//*************************************************************************************************
#endif  // end of file ( DragTool.h )
