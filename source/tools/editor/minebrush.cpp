#define MINEBRUSH_CPP
/*************************************************************************************************\
MineBrush.cpp			: Implementation of the MineBrush component.
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//
\*************************************************************************************************/

#include "MineBrush.h"
#include "editorobjectmgr.h"
#include "Move.h"


bool MineBrush::beginPaint()
{
	if(!pAction)
		pAction = new MineAction();
	return true;
}

Action* MineBrush::endPaint()
{
	Action* pRetAction = nullptr;
	if(pAction)
	{
		if(pAction->mineInfoList.Count())
			pRetAction =  pAction;
		else
		{
			delete pAction;
		}
	}
	pAction = nullptr;
	return pRetAction;
}

bool MineBrush::paint(Stuff::Vector3D& worldPos, int32_t screenX, int32_t screenY)
{
	int32_t tileC;
	int32_t tileR;
	land->worldToTile(worldPos, tileR, tileC);
	if(!(tileR < Terrain::realVerticesMapSide || tileR > -1
			|| tileC < Terrain::realVerticesMapSide || tileC > -1))
	{
		return false;
	}
	int32_t cellRow;
	int32_t cellCol;
	land->worldToCell(worldPos, cellRow, cellCol);
	if(1 == GameMap->getMine(cellRow, cellCol))
	{
		return true;
	}
	MineAction::CTileMineInfo info;
	info.row = tileR;
	info.column = tileC;
	info.mineState = GameMap->getMine(tileR, tileC);
	pAction->AddChangedTileMineInfo(info);
	GameMap->setMine(cellRow, cellCol, 1);
	return true;
}

bool MineBrush::canPaint(Stuff::Vector3D& worldPos, int32_t screenX, int32_t screenY, int32_t flags)
{
	return true;
}

bool MineBrush::canPaintSelection()
{
	return land->hasSelection();
}

Action* MineBrush::applyToSelection()
{
	MineAction* pRetAction = new MineAction();
	for(auto i = 0; i < land->realVerticesMapSide; ++i)
	{
		for(auto j = 0; j < land->realVerticesMapSide; ++j)
		{
			if(land->isVertexSelected(j, i))
			{
				MineAction::CTileMineInfo info;
				info.row = j;
				info.column = i;
				info.mineState = GameMap->getMine(j, i);
				pRetAction->AddChangedTileMineInfo(info);
				GameMap->setMine(j, i, 1);
			}
		}
	}
	return pRetAction;
}

bool MineBrush::MineAction::undo()
{
	for(MINE_INFO_LIST::EIterator iter = mineInfoList.Begin();
			!iter.IsDone(); iter++)
	{
		// get current values
		uint32_t lMineState = GameMap->getMine((*iter).row, (*iter).column);
		// reset to old values
		GameMap->setMine((*iter).row, (*iter).column, (*iter).mineState);
		// save current valuds
		(*iter).mineState = lMineState;
	}
	return true;
}

bool MineBrush::MineAction::redo()
{
	return undo();
}

void MineBrush::MineAction::AddChangedTileMineInfo(CTileMineInfo& info)
{
	for(MINE_INFO_LIST::EIterator iter = mineInfoList.Begin();
			!iter.IsDone(); iter++)
	{
		if(info.row == (*iter).row && info.column == (*iter).column)
			return;
	}
	mineInfoList.Append(info);
}




//*************************************************************************************************
// end of file ( MineBrush.cpp )
