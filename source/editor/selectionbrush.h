#ifndef SELECTIONBRUSH_H
#define SELECTIONBRUSH_H
/*************************************************************************************************\
SelectionBrush.h	: Interface for the SelectionBrush component.
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//
\*************************************************************************************************/

#ifndef BRUSH_H
#include "Brush.h"
#endif

class ActionPaintTile;
class ModifyBuildingAction;
class EditorObject;

class SelectionBrush: public Brush
{
	public:

		SelectionBrush( bool Area, int radius );
		virtual ~SelectionBrush();
		virtual bool beginPaint();
		virtual Action* endPaint();
		virtual bool paint( Stuff::Vector3D& worldPos, int screenX, int screenY );
		virtual bool canPaint( Stuff::Vector3D& worldPos, int screenX, int screenY, int flags ) { return true; } 
		virtual void render( int screenX, int screenY );
		bool    paintSmooth( Stuff::Vector3D& worldPos, int screenX, int screenY, int radius );

	private:

		// suppressed
		SelectionBrush( const SelectionBrush& electionBrush );
		SelectionBrush& operator=( const SelectionBrush& electionBrush );
		static float calcNewHeight( int vertexRow, int vertexCol, float screenDeltaY );
		bool   paintSmoothArea( Stuff::Vector3D& worldPos, int screenX, int screenY, float radiusX, float radiusY,
									   int j, int i);

		Stuff::Vector4D		lastPos;
		Stuff::Vector3D		firstWorldPos;
		Stuff::Vector3D		lastWorldPos;

		bool bPainting;
		bool bArea;
		bool bDrag;
		EditorObject* pDragBuilding;
		bool bFirstClick;

		int lastRow;
		int lastCol;

		ActionPaintTile*	pCurAction;
		ModifyBuildingAction*	pCurModifyBuildingAction;

		int smoothRadius;


};


//*************************************************************************************************
#endif  // end of file ( SelectionBrush.h )
