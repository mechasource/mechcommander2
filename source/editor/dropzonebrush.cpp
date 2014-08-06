#define DROPZONEBRUSH_CPP
/*************************************************************************************************\
dropZoneBrush.cpp			: Implementation of the dropZoneBrush component.
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//
\*************************************************************************************************/

#include "dropZoneBrush.h"
#include "EditorObjectMgr.h"

DropZoneBrush::DropZoneBrush(int align, bool bVtol)
{
	alignment = align;
	bVTol = bVtol;
}

bool DropZoneBrush::paint( Stuff::Vector3D& worldPos, int screenX, int screenY )
{
	EditorObject* pInfo = EditorObjectMgr::instance()->addDropZone( worldPos, alignment, bVTol );
	if ( pInfo && pAction )
	{
		pAction->addBuildingInfo( *pInfo );
		return true;
	}

	return false;
}

bool DropZoneBrush::canPaint( Stuff::Vector3D& worldPos, int screenX, int screenY, int flags )
{
	return EditorObjectMgr::instance()->canAddDropZone( worldPos, alignment, bVTol );
}
//*************************************************************************************************
// end of file ( dropZoneBrush.cpp )
