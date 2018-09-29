/*************************************************************************************************\
MineBrush.h			: Interface for the MineBrush component.
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//
\*************************************************************************************************/

#pragma once

#ifndef MINEBRUSH_H
#define MINEBRUSH_H

//#include "brush.h"
//#include "action.h"
//#include "editorobjects.h"

//*************************************************************************************************

/**************************************************************************************************
CLASS DESCRIPTION
MineBrush:
**************************************************************************************************/
class MineBrush : public Brush
{
public:
	MineBrush() { pAction = nullptr; }
	virtual ~MineBrush() {}
	virtual bool beginPaint(void);
	virtual Action* endPaint(void);
	virtual bool paint(Stuff::Vector3D& worldPos, int32_t screenX, int32_t screenY);
	virtual bool canPaint(
		Stuff::Vector3D& worldPos, int32_t screenX, int32_t screenY, int32_t flags);
	virtual bool canPaintSelection();
	virtual Action* applyToSelection(void);

private:
	class MineAction : public Action
	{
	public:
		virtual ~MineAction() {}
		virtual bool redo(void);
		virtual bool undo(void);

		struct CTileMineInfo
		{
			int32_t row;
			int32_t column;
			uint32_t mineState;
		};
		typedef EList<CTileMineInfo, const CTileMineInfo&> MINE_INFO_LIST;
		MINE_INFO_LIST mineInfoList;

		void AddChangedTileMineInfo(CTileMineInfo& info);
	};

	MineAction* pAction;
};

//*************************************************************************************************
#endif // end of file ( MineBrush.h )
