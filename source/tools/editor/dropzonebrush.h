/*************************************************************************************************\
DropZoneBrush.h			: Interface for the DropZoneBrush component.
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//
\*************************************************************************************************/

#pragma once

#ifndef DROPZONEBRUSH_H
#define DROPZONEBRUSH_H

//#include "buildingbrush.h"

//*************************************************************************************************

/**************************************************************************************************
CLASS DESCRIPTION
DropZoneBrush:
**************************************************************************************************/
class DropZoneBrush: public BuildingBrush
{
public:

	DropZoneBrush(int32_t alignment = 0, bool bVTol = 0);
	virtual bool paint(Stuff::Vector3D& worldPos, int32_t screenX, int32_t screenY);
	virtual bool canPaint(Stuff::Vector3D& worldPos, int32_t screenX, int32_t screenY, int32_t flags);

private:

	bool bVTol;



};


//*************************************************************************************************
#endif  // end of file ( DropZoneBrush.h )
