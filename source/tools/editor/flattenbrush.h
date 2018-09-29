/*************************************************************************************************\
FlattenBrush.h			: Interface for the FlattenBrush component.
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//
\*************************************************************************************************/

#pragma once

#ifndef FLATTENBRUSH_H
#define FLATTENBRUSH_H

//#include "brush.h"
//#include "action.h"

//*************************************************************************************************

/**************************************************************************************************
CLASS DESCRIPTION
FlattenBrush:
**************************************************************************************************/

class FlattenBrush : public Brush
{
public:
	FlattenBrush(void);
	virtual ~FlattenBrush(void);

	virtual bool beginPaint(void);
	virtual Action* endPaint(void);
	virtual bool paint(Stuff::Vector3D& worldPos, int32_t screenX, int32_t screenY);
	virtual bool canPaint(
		Stuff::Vector3D& worldPos, int32_t screenX, int32_t screenY, int32_t flags)
	{
		return true;
	}
	virtual bool canPaintSelection() { return true; }
	virtual Action* applyToSelection(void);

	Action* applyHeightToSelection(float height);
	float getAverageHeightOfSelection();

private:
	ActionPaintTile* pCurAction;
	void flattenVertex(int32_t row, int32_t col, float val);
};
//*************************************************************************************************
#endif // end of file ( FLATTENBRUSH_H.h )
