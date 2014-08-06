#ifndef DROPZONEBRUSH_H
#define DROPZONEBRUSH_H
/*************************************************************************************************\
DropZoneBrush.h			: Interface for the DropZoneBrush component.
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//
\*************************************************************************************************/

#ifndef BUILDINGBRUSH_H
#include "BuildingBrush.h"
#endif

//*************************************************************************************************

/**************************************************************************************************
CLASS DESCRIPTION
DropZoneBrush:
**************************************************************************************************/
class DropZoneBrush: public BuildingBrush
{
	public:

	DropZoneBrush( int alignment = 0, bool bVTol = 0 );
	virtual bool paint( Stuff::Vector3D& worldPos, int screenX, int screenY  );
	virtual bool canPaint( Stuff::Vector3D& worldPos, int screenX, int screenY, int flags );

	private:

		bool bVTol;



};


//*************************************************************************************************
#endif  // end of file ( DropZoneBrush.h )
