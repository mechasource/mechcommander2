//----------------------------------------------------------------------------
//
// Action.cpp - implementation file for the abstract action object, and
//				the mgr that holds action objects. (otherwise known as the
//				undo manager )
//
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#include "terrain.h"
#include "Action.h"
#include "TerrTxm.h"

ActionUndoMgr* ActionUndoMgr::instance = NULL;

//************************************************************************
// Function:	c'tor
// ParamsIn:	none
// ParamsOut:	none
// Returns:		nothing	
// Description:
//************************************************************************
ActionUndoMgr::ActionUndoMgr() 
{
	m_CurrentPos = -1;
	m_PosOfLastSave = -1;
	gosASSERT( !instance );
	instance = this;
}
//************************************************************************
// Function:	d'tor
// ParamsIn:	none
// ParamsOut:	none
// Returns:		nothing
// Description:	empties list
//************************************************************************
ActionUndoMgr::~ActionUndoMgr()
{
	Reset();
}

//***********************************************************************
// Function:	AddAction
// ParamsIn:	Action to add
// ParamsOut:	none
// Returns:		void
// Descripition: adds the passed in action to the undo list
//***********************************************************************
void ActionUndoMgr::AddAction( Action* pAction )
{
	gosASSERT( pAction );

	if ( m_listUndoActions.Count() )
	{
		if (m_PosOfLastSave > m_CurrentPos)
		{
			m_PosOfLastSave = -1;
		}
		ACTION_LIST::EIterator iter = m_listUndoActions.End();
		for ( int i = m_listUndoActions.Count() - 1; i > m_CurrentPos; -- i )
		{
			delete (*iter);
			iter--;
			m_listUndoActions.DeleteTail();
		}
	}

	m_listUndoActions.Append( pAction );
	m_CurrentPos = m_listUndoActions.Count() - 1;

}// fun AddAction

//***********************************************************************
// Function:	EmptyUndoList
// ParamsIn:	none
// ParamsOut:	none
// Returns:		void
// Descripition:clears out the undo list
//***********************************************************************
void ActionUndoMgr::EmptyUndoList()
{
	
	for (ACTION_LIST::EIterator pos = m_listUndoActions.Begin(); !pos.IsDone(); pos++ )
	{
		delete (*pos);
	}
	m_listUndoActions.Clear();
	m_CurrentPos = -1;
	m_PosOfLastSave = -1;
}

//***********************************************************************
// Function:	GetRedoString
// ParamsIn:	none
// ParamsOut:	none
// Returns:		string to put in the Undo prompt
// Descripition: this is the string that should go in the redo prompt
//***********************************************************************
const char* ActionUndoMgr::GetRedoString()
{
	const char* strRet = NULL;

	if ( HaveRedo() )
	{
		
		ACTION_POS tmp = m_CurrentPos;
		tmp++;
		ACTION_LIST::EIterator iter = m_listUndoActions.Iterator( tmp );
		return (*iter)->getDescription();
	}

	return strRet;
}

//***********************************************************************
// Function:	GetUndoString
// ParamsIn:	none
// ParamsOut:	none
// Returns:		string to put in the Udo prompt
// Descripition: this is the string that should go in the undo prompt
//***********************************************************************
const char* ActionUndoMgr::GetUndoString()
{
	const char* strRet = NULL;
	if ( HaveUndo() )
	{
		ACTION_LIST::EIterator iter = m_listUndoActions.Iterator( m_CurrentPos );
		return (*iter)->getDescription();
	}

	return strRet;
}


//***********************************************************************
// Function:	HaveRedo
// ParamsIn:	none
// ParamsOut:	none
// Returns:		whether there is a redo action to perform
// Descripition: call to see wherther you can perform a redo
//***********************************************************************
bool ActionUndoMgr::HaveRedo() const
{
	return m_CurrentPos + 1 != m_listUndoActions.Count();
}

//***********************************************************************
// Function:	HaveUndo
// ParamsIn:	none
// ParamsOut:	none
// Returns:		whether there is undo action to perform
// Descripition: call to see whether you can perform an undo action
//***********************************************************************
bool ActionUndoMgr::HaveUndo() const
{
	return m_CurrentPos != -1;
}

//************************************************************************
// Function:	Redo
// ParamsIn:	none
// ParamsOut;	none
// Returns:		success of operation
// Description:	gets the action at the front of the redo list, performs
//				it and adds it to the undo list
//************************************************************************
bool ActionUndoMgr::Redo()
{
	gosASSERT( HaveRedo() );

	m_CurrentPos++;
	ACTION_LIST::EIterator iter = m_listUndoActions.Iterator( m_CurrentPos );

	return (*iter)->redo();


}

//************************************************************************
// Function:	Reset  
// ParamsIn:	none
// ParamsOut:	none
// Returns:		nothing
// Description:	Empties all of the actions from the do and undo lists
//************************************************************************
void ActionUndoMgr::Reset()
{
	EmptyUndoList();

}

//************************************************************************
// Function:	Undo
// ParamsIn:	none
// ParamsOut;	none
// Returns:		success of operation
// Description:	gets the action at the front of the undo list, performs
//				it and adds it to the redo list
//************************************************************************
bool ActionUndoMgr::Undo()
{
	bool bRetVal = false;

	gosASSERT( HaveUndo() );


	ACTION_LIST::EIterator iter = m_listUndoActions.Iterator( m_CurrentPos );
	bRetVal = (*iter)->undo();
	m_CurrentPos --;

	return bRetVal;
}

//************************************************************************
// Function:	NoteThatASaveHasJustOccurred
// ParamsIn:	none
// ParamsOut;	none
// Returns:		
// Description:	
//************************************************************************
void ActionUndoMgr::NoteThatASaveHasJustOccurred()
{
	m_PosOfLastSave = m_CurrentPos;
}

//************************************************************************
// Function:	ThereHasBeenANetChangeFromWhenLastSaved
// ParamsIn:	none
// ParamsOut;	none
// Returns:		
// Description:	
//************************************************************************
bool ActionUndoMgr::ThereHasBeenANetChangeFromWhenLastSaved()
{
	if (m_PosOfLastSave == m_CurrentPos)
	{
		return false;
	}
	else
	{
		return true;
	}
}



//-----------------------------------------------------------------------
// Function:	ActionPaintTile::Redo
// ParamsIn:	none
// ParamsOut:	none
// Returns:		success of operation
// Description: redoes a smart paint operation
////-----------------------------------------------------------------------
bool ActionPaintTile::redo()
{
	return doRedo();
}


bool ActionPaintTile::doRedo()
{ 
	for ( VERTEX_INFO_LIST::EIterator iter = vertexInfoList.Begin();
		!iter.IsDone(); iter++ )
	{
		// get current values
		int terrain = land->getTerrain( (*iter).row, (*iter).column );
		int texture = land->getTexture( (*iter).row, (*iter).column );
		float elv = land->getTerrainElevation( (*iter).row, (*iter).column );

		Overlays overlay;
		unsigned long offset;

		// reset to old values
		land->terrainTextures->getOverlayInfoFromHandle( (*iter).textureData, overlay, offset );
		land->setOverlay( (*iter).row, (*iter).column, overlay, offset );
		land->setTerrain( (*iter).row, (*iter).column, (*iter).terrainData );
		land->setVertexHeight( (*iter).row * land->realVerticesMapSide + (*iter).column, (*iter).elevation );

		// save current valuds
		(*iter).terrainData = terrain;
		(*iter).textureData = texture;
		(*iter).elevation = elv;

	}

	return true;
}

//-----------------------------------------------------------------------
// Function:	Undo
// ParamsIn:	none
// ParamsOut:	none
// Returns:		nothing
// Description: undos a smart paint operation
////-----------------------------------------------------------------------
bool ActionPaintTile::undo()
{ 
	// actually, undo does the same thing as redo.
	return doRedo(); 
}

//-----------------------------------------------------------------------
// Function:	AddTileInfo
// ParamsIn:	info to be added to the undo list
// ParamsOut:	none
// Returns:		nothing
// Description: this function checks to make sure that the object isn't
//				already in the list before adding it.
////-----------------------------------------------------------------------
void ActionPaintTile::addVertexInfo( VertexInfo& info )
{
	for( VERTEX_INFO_LIST::EIterator iter = vertexInfoList.Begin();
		!iter.IsDone(); iter++ )
		{
			if ( info.row == (*iter).row && info.column == (*iter).column )
				return;
		}

		vertexInfoList.Append( info );
}

//-----------------------------------------------------------------------
// Function:	AddChangedVertexInfo
// ParamsIn:	none
// ParamsOut:	none
// Returns:		nothing
// Description:
////-----------------------------------------------------------------------
void ActionPaintTile::addChangedVertexInfo( int row, int column )
{

	// get the info and add it
	for( VERTEX_INFO_LIST::EIterator iter = vertexInfoList.Begin();
		!iter.IsDone(); iter++ )
		{
			if ( row == (*iter).row && column == (*iter).column )
				return;	

		}

	// if we made it here, it isn't in there already
	VertexInfo info( row, column );
	vertexInfoList.Append( info );


}

bool ActionPaintTile::getOldHeight( int row, int column, float& height )
{
	for( VERTEX_INFO_LIST::EIterator iter = vertexInfoList.Begin();
		!iter.IsDone(); iter++ )
		{
			if ( row == (*iter).row && column == (*iter).column )
			{
				height = (*iter).elevation;
				return true;
			}
		}

		return false;
}

VertexInfo::VertexInfo( long newRow, long newColumn )
{
	gosASSERT( newRow > -1 && newColumn > -1 );
	gosASSERT( newRow < land->realVerticesMapSide && newColumn < land->realVerticesMapSide );

	row = newRow;
	column = newColumn;

	elevation = land->getTerrainElevation( row, column );
	terrainData = land->getTerrain( row, column );
	textureData = land->getTexture( row, column );

}


#ifndef EDITOROBJECTMGR_H
#include "EditorObjectMgr.h"
#endif
ModifyBuildingAction::~ModifyBuildingAction()
{
	for ( OBJ_INFO_PTR_LIST::EIterator iter = buildingCopyPtrs.Begin(); !iter.IsDone(); iter++)
	{
		delete (*iter);
	}
}

bool ModifyBuildingAction::doRedo()
{
	bool bRetVal = true;

	OBJ_INFO_PTR_LIST::EIterator iter2 = buildingPtrs.Begin();
	OBJ_APPEAR_LIST::EIterator iter3 = buildingAppearanceCopies.Begin();
	OBJ_ID_LIST::EIterator iter4 = buildingIDs.Begin();
	for ( OBJ_INFO_PTR_LIST::EIterator iter = buildingCopyPtrs.Begin();
		!iter.IsDone(); iter++)
		{
			//EditorObject *pBuilding = (*iter2);
			EditorObject *pBuilding = EditorObjectMgr::instance()->getObjectAtLocation((*iter4).x, (*iter4).y);
			if (pBuilding)
			{
				EditorObject *pBuildingSwap = (*iter)->Clone();
				ObjectAppearance AppearanceSwap = (*iter3);

				(*iter)->CastAndCopy(*pBuilding);
				(*iter3) = (*(pBuilding->appearance()));

				(*pBuilding).CastAndCopy(*pBuildingSwap);
				(*(pBuilding->appearance())) = AppearanceSwap;

				delete pBuildingSwap;

				{
					/*this is just to make sure the visuals are up-to-date*/
					bool d = pBuilding->getDamage();
					pBuilding->setDamage(!d);
					pBuilding->setDamage(d);
				}

				long row, column;
				pBuilding->getCells(row, column);
				EditorObjectMgr::instance()->moveBuilding(pBuilding, row, column);

				(*iter4).x = pBuilding->getPosition().x;
				(*iter4).y = pBuilding->getPosition().y;
			}
			else 
			{
				gosASSERT(false);
			}

			iter2++;
			iter3++;
			iter4++;
		}

	return bRetVal;
}

bool ModifyBuildingAction::redo()
{
	return doRedo();
}

bool ModifyBuildingAction::undo()
{
	// actually, undo does the same thing as redo.
	return doRedo(); 
}

void ModifyBuildingAction::addBuildingInfo(EditorObject& info)
{

	if ((0 < buildingPtrs.Count()) && (OBJ_INFO_PTR_LIST::INVALID_ITERATOR != buildingPtrs.Find(&info)))
	{
		return;
	}

	// if we made it here, it isn't in there already
	EditorObject *pInfoCopy = info.Clone();
	buildingCopyPtrs.Append( pInfoCopy );
	buildingAppearanceCopies.Append(*(info.appearance()));

	buildingPtrs.Append( &info );
	CObjectID id;
	id.x = info.getPosition().x;
	id.y = info.getPosition().y;
	buildingIDs.Append(id);
}

void ModifyBuildingAction::updateNotedObjectPositions()
{
	OBJ_ID_LIST::EIterator iter4 = buildingIDs.Begin();
	for ( OBJ_INFO_PTR_LIST::EIterator iter = buildingPtrs.Begin(); !iter.IsDone(); iter++)
	{
		EditorObject *pBuilding = (*iter);
		if (pBuilding)
		{
			(*iter4).x = pBuilding->getPosition().x;
			(*iter4).y = pBuilding->getPosition().y;
		}
		else 
		{
			gosASSERT(false);
		}

		iter4++;
	}
}
