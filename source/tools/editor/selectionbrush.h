/*************************************************************************************************\
SelectionBrush.h	: Interface for the SelectionBrush component.
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//
\*************************************************************************************************/

#pragma once

#ifndef SELECTIONBRUSH_H
#define SELECTIONBRUSH_H

//#include "brush.h"

class ActionPaintTile;
class ModifyBuildingAction;
class EditorObject;

class SelectionBrush: public Brush
{
public:

	SelectionBrush(bool Area, int32_t radius);
	virtual ~SelectionBrush(void);
	virtual bool beginPaint(void);
	virtual Action* endPaint(void);
	virtual bool paint(Stuff::Vector3D& worldPos, int32_t screenX, int32_t screenY);
	virtual bool canPaint(Stuff::Vector3D& worldPos, int32_t screenX, int32_t screenY, int32_t flags)
	{
		return true;
	}
	virtual void render(int32_t screenX, int32_t screenY);
	bool    paintSmooth(Stuff::Vector3D& worldPos, int32_t screenX, int32_t screenY, int32_t radius);

private:

	// suppressed
	SelectionBrush(const SelectionBrush& electionBrush);
	SelectionBrush& operator=(const SelectionBrush& electionBrush);
	static float calcNewHeight(int32_t vertexRow, int32_t vertexCol, float screenDeltaY);
	bool   paintSmoothArea(Stuff::Vector3D& worldPos, int32_t screenX, int32_t screenY, float radiusX, float radiusY,
						   int32_t j, int32_t i);

	Stuff::Vector4D		lastPos;
	Stuff::Vector3D		firstWorldPos;
	Stuff::Vector3D		lastWorldPos;

	bool bPainting;
	bool bArea;
	bool bDrag;
	EditorObject* pDragBuilding;
	bool bFirstClick;

	int32_t lastRow;
	int32_t lastCol;

	ActionPaintTile*	pCurAction;
	ModifyBuildingAction*	pCurModifyBuildingAction;

	int32_t smoothRadius;


};


//*************************************************************************************************
#endif  // end of file ( SelectionBrush.h )
