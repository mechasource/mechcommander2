#ifndef OBJECTSELECTIONBRUSH_H
#define OBJECTSELECTIONBRUSH_H
/*************************************************************************************************\
ObjectSelectionBrush.h	: Interface for the ObjectSelectionBrush component.
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//
\*************************************************************************************************/

#ifndef BRUSH_H
#include "Brush.h"
#endif

#include "elist.h"

class ActionPaintTile;
class EditorObject;

typedef EList <EditorObject *, EditorObject *> EditorObjectPointerList;

class ObjectSelectionBrush: public Brush
{
	public:

		ObjectSelectionBrush();
		virtual ~ObjectSelectionBrush();
		virtual bool beginPaint();
		virtual Action* endPaint();
		virtual bool paint( Stuff::Vector3D& worldPos, int screenX, int screenY );
		virtual bool canPaint( Stuff::Vector3D& worldPos, int screenX, int screenY, int flags ) { return true; } 
		virtual void render( int screenX, int screenY );
		EditorObjectPointerList selectedObjectPointerList();

	private:

		// suppressed
		ObjectSelectionBrush( const ObjectSelectionBrush& electionBrush );
		ObjectSelectionBrush& operator=( const ObjectSelectionBrush& electionBrush );

		Stuff::Vector4D		lastPos;
		Stuff::Vector3D		lastWorldPos;

		bool bPainting;
		bool bFirstClick;

		ActionPaintTile*	pCurAction;
};


//*************************************************************************************************
#endif  // end of file ( ObjectSelectionBrush.h )
