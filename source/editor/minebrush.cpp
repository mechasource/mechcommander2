#define MINEBRUSH_CPP
/*************************************************************************************************\
MineBrush.cpp			: Implementation of the MineBrush component.
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//
\*************************************************************************************************/

#include "MineBrush.h"
#include "EditorObjectMgr.h"
#include "Move.h"


bool MineBrush::beginPaint()
{
	if ( !pAction )
		pAction = new MineAction();

	return true;
}

Action* MineBrush::endPaint()
{
	Action* pRetAction = NULL;

	if (pAction)
	{
		if ( pAction->mineInfoList.Count() )
			pRetAction =  pAction;
		else
		{
			delete pAction;
		}
	}
	
	pAction = NULL;
	return pRetAction;
}

bool MineBrush::paint( Stuff::Vector3D& worldPos, int screenX, int screenY  )
{
	long tileC;
	long tileR;

	land->worldToTile( worldPos, tileR, tileC );

	if (!( tileR < Terrain::realVerticesMapSide || tileR > -1 
		|| tileC < Terrain::realVerticesMapSide || tileC > -1 )) {
		return false;
	}

	long cellRow;
	long cellCol;
	land->worldToCell(worldPos, cellRow, cellCol);

	if (1 == GameMap->getMine(cellRow, cellCol)) {
		return true;
	}

	MineAction::CTileMineInfo info;
	info.row = tileR;
	info.column = tileC;
	info.mineState = GameMap->getMine( tileR, tileC);
	pAction->AddChangedTileMineInfo( info );
	GameMap->setMine(cellRow, cellCol, 1);

	return true;
}

bool MineBrush::canPaint( Stuff::Vector3D& worldPos, int screenX, int screenY, int flags )
{
	return true;
}

bool MineBrush::canPaintSelection( )
{
	return land->hasSelection();
}

Action* MineBrush::applyToSelection()
{
	MineAction* pRetAction = new MineAction();
	for ( int i = 0; i < land->realVerticesMapSide; ++i )
	{
		for ( int j = 0; j < land->realVerticesMapSide; ++j )
		{
			if ( land->isVertexSelected( j, i ) )
			{
				MineAction::CTileMineInfo info;
				info.row = j;
				info.column = i;
				info.mineState = GameMap->getMine( j, i );
				pRetAction->AddChangedTileMineInfo( info );
				GameMap->setMine( j, i, 1 );
			}
		}
	}

	return pRetAction;
}

bool MineBrush::MineAction::undo()
{
	for ( MINE_INFO_LIST::EIterator iter = mineInfoList.Begin();
		!iter.IsDone(); iter++ )
	{
		// get current values
		unsigned long lMineState = GameMap->getMine( (*iter).row, (*iter).column );

		// reset to old values
		GameMap->setMine( (*iter).row, (*iter).column, (*iter).mineState );

		// save current valuds
		(*iter).mineState = lMineState;
	}

	return true;
}

bool MineBrush::MineAction::redo()
{
	return undo();
}

void MineBrush::MineAction::AddChangedTileMineInfo( CTileMineInfo& info )
{
	for( MINE_INFO_LIST::EIterator iter = mineInfoList.Begin();
		!iter.IsDone(); iter++ )
		{
			if ( info.row == (*iter).row && info.column == (*iter).column )
				return;
		}

		mineInfoList.Append( info );
}




//*************************************************************************************************
// end of file ( MineBrush.cpp )
