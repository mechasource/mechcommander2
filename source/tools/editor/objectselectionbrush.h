#ifndef OBJECTSELECTIONBRUSH_H
#define OBJECTSELECTIONBRUSH_H
/*************************************************************************************************\
ObjectSelectionBrush.h	: Interface for the ObjectSelectionBrush component.
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//
\*************************************************************************************************/

#ifndef BRUSH_H
#include "brush.h"
#endif

#include "elist.h"

class ActionPaintTile;
class EditorObject;

typedef EList<EditorObject*, EditorObject*> EditorObjectPointerList;

class ObjectSelectionBrush : public Brush
{
public:
	ObjectSelectionBrush(void);
	virtual ~ObjectSelectionBrush(void);
	virtual bool beginPaint(void);
	virtual Action* endPaint(void);
	virtual bool paint(Stuff::Vector3D& worldPos, int32_t screenX, int32_t screenY);
	virtual bool canPaint(
		Stuff::Vector3D& worldPos, int32_t screenX, int32_t screenY, int32_t flags)
	{
		return true;
	}
	virtual void render(int32_t screenX, int32_t screenY);
	EditorObjectPointerList selectedObjectPointerList(void);

private:
	// suppressed
	ObjectSelectionBrush(const ObjectSelectionBrush& electionBrush);
	ObjectSelectionBrush& operator=(const ObjectSelectionBrush& electionBrush);

	Stuff::Vector4D lastPos;
	Stuff::Vector3D lastWorldPos;

	bool bPainting;
	bool bFirstClick;

	ActionPaintTile* pCurAction;
};

#endif // end of file ( ObjectSelectionBrush.h )
