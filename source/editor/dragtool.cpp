/*************************************************************************************************\
DragTool.cpp			: Implementation of the DragTool component.
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//
\*************************************************************************************************/

#include "DragTool.h"
#include "EditorInterface.h"

DragTool::DragTool()
{
	lastX = lastY = -1;
}

//-------------------------------------------------------------------------------------------------

DragTool::~DragTool()
{
}


bool DragTool::beginPaint()
{
	lastX = lastY = -1;
	return true;
}
		
Action* DragTool::endPaint()
{
	return NULL;
}

bool DragTool::paint( Stuff::Vector3D& worldPos, int screenX, int screenY  )
{
	if ( lastX != -1 )
	{
		eye->moveLeft( (screenX - lastX)/eye->getScaleFactor() );
		eye->moveDown( (lastY - screenY)/eye->getScaleFactor() );
		EditorInterface::instance()->syncScrollBars();
		EditorInterface::instance()->SafeRunGameOSLogic();
	}
	
	lastX = screenX;
	lastY = screenY;

	return true;
}


//*************************************************************************************************
// end of file ( DragTool.cpp )
