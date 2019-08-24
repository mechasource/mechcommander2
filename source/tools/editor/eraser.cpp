#define ERASER_CPP
/*************************************************************************************************\
eraser.cpp			: Implementation of the eraser component.
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//
\*************************************************************************************************/

#include "eraser.h"
#include "MineBrush.h"

#ifndef OBJECTMGR_H
#include "editorobjectmgr.h"
#endif

//---------------------------------------------------------------------------
inline bool
isCementType(uint32_t type)
{
	bool isCement =
		((type == BASE_CEMENT_TYPE) || ((type >= START_CEMENT_TYPE) && (type <= END_CEMENT_TYPE)));
	return isCement;
}

bool
Eraser::beginPaint()
{
	gosASSERT(!pCurAction);
	pCurAction = new EraserAction;
	return true;
}
Action*
Eraser::endPaint()
{
	Action* pRetAction = nullptr;
	if (pCurAction)
	{
		if (pCurAction->vertexInfoList.Count() || pCurAction->bldgAction.objInfoPtrList.Count())
		{
			pRetAction = pCurAction;
		}
		else
		{
			delete pCurAction;
		}
	}
	pCurAction = nullptr;
	return pRetAction;
}

#define DEFAULT_TERRAIN 2
bool
Eraser::paint(Stuff::Vector3D& worldPos, int32_t screenX, int32_t screenY)
{
	EditorObject* pInfo = EditorObjectMgr::instance()->getObjectAtPosition(worldPos);
	if (pInfo)
	{
		CTeams originalTeams = EditorData::instance->TeamsRef();
		EditorData::instance->handleObjectInvalidation(pInfo);
		if (!(originalTeams == EditorData::instance->TeamsRef()))
		{
			pCurAction->teamsAction.PreviousTeams(originalTeams);
			pCurAction->teamsActionIsSet = true;
		}
		{
			BuildingLink* pLink = EditorObjectMgr::instance()->getLinkWithParent(pInfo);
			if (pLink)
			{
				pCurAction->linkAction.AddToListOnce(
					LinkBrush::LinkInfo(pLink, LinkBrush::LinkInfo::REMOVE));
				EditorObjectMgr::instance()->deleteLink(pLink);
			}
			{
				pLink = EditorObjectMgr::instance()->getLinkWithBuilding(pInfo);
				if (pLink)
				{
					pCurAction->linkAction.AddToListOnce(
						LinkBrush::LinkInfo(pLink, LinkBrush::LinkInfo::EDIT));
					pLink->RemoveObject(pInfo);
				}
			}
		}
		pCurAction->bldgAction.addBuildingInfo(*pInfo); // undo!
		bool retval = EditorObjectMgr::instance()->deleteObject(pInfo);
		return retval;
	}
	else
	{
		int32_t tileRow, tileCol;
		int32_t cellRow, cellCol;
		land->worldToTileCell(worldPos, tileRow, tileCol, cellRow, cellCol);
		cellRow += tileRow * terrain_const::MAPCELL_DIM;
		cellCol += tileCol * terrain_const::MAPCELL_DIM;
		Overlays type;
		uint32_t Offset;
		land->getOverlay(tileRow, tileCol, type, Offset);
		if (type != INVALID_OVERLAY)
		{
			pCurAction->addChangedVertexInfo(tileRow, tileCol); // undo!
			land->setOverlay(tileRow, tileCol, INVALID_OVERLAY, 0);
			return true;
		}
		if (GameMap->getMine(cellRow, cellCol))
		{
			GameMap->setMine(cellRow, cellCol, 0);
		}
		int32_t terrainType = land->getTerrain(tileRow, tileCol);
		if (isCementType(terrainType))
		{
			land->setTerrain(tileRow, tileCol, DEFAULT_TERRAIN);
		}
	}
	return false;
}
bool
Eraser::canPaint(Stuff::Vector3D& worldPos, int32_t screenX, int32_t screenY, int32_t flags)
{
	if (EditorObjectMgr::instance()->getObjectAtPosition(worldPos))
		return true;
	int32_t tileRow, tileCol;
	int32_t cellRow, cellCol;
	land->worldToTileCell(worldPos, tileRow, tileCol, cellRow, cellCol);
	cellRow += tileRow * terrain_const::MAPCELL_DIM;
	cellCol += tileCol * terrain_const::MAPCELL_DIM;
	Overlays type;
	uint32_t Offset;
	land->getOverlay(tileRow, tileCol, type, Offset);
	if (type != INVALID_OVERLAY)
	{
		return true;
	}
	int32_t terrainType = land->getTerrain(tileRow, tileCol);
	if (isCementType(terrainType))
	{
		return true;
	}
	if (GameMap->getMine(cellRow, cellCol))
	{
		return true;
	}
	return false;
}

Action*
Eraser::applyToSelection()
{
	EraserAction* pRetAction = new EraserAction;
	{
		CTeams originalTeams = EditorData::instance->TeamsRef();
		/*first remove all references to objects that are about to be deleted*/
		EditorObjectMgr::EDITOR_OBJECT_LIST selectedObjectsList =
			EditorObjectMgr::instance()->getSelectedObjectList();
		EditorObjectMgr::EDITOR_OBJECT_LIST::EIterator it = selectedObjectsList.Begin();
		while (!it.IsDone())
		{
			const EditorObject* pInfo = (*it);
			if (pInfo)
			{
				EditorData::instance->handleObjectInvalidation(pInfo);
			}
			it++;
		}
		if (!(originalTeams == EditorData::instance->TeamsRef()))
		{
			/*record undo info regarding removal of references to deleted
			objects before we actually delete the objects (because we need info
			from the actual objects before they're deleted)*/
			pRetAction->teamsAction.PreviousTeams(originalTeams);
			pRetAction->teamsActionIsSet = true;
		}
	}
	EditorObjectMgr::EDITOR_OBJECT_LIST selectedObjectsList =
		EditorObjectMgr::instance()->getSelectedObjectList();
	EditorObjectMgr::EDITOR_OBJECT_LIST::EIterator it = selectedObjectsList.Begin();
	while (!it.IsDone())
	{
		EditorObject* pInfo = (*it);
		if (pInfo)
		{
			{
				BuildingLink* pLink = EditorObjectMgr::instance()->getLinkWithParent(pInfo);
				if (pLink)
				{
					pRetAction->linkAction.AddToListOnce(
						LinkBrush::LinkInfo(pLink, LinkBrush::LinkInfo::REMOVE));
					EditorObjectMgr::instance()->deleteLink(pLink);
				}
				{
					pLink = EditorObjectMgr::instance()->getLinkWithBuilding(pInfo);
					if (pLink)
					{
						pRetAction->linkAction.AddToListOnce(
							LinkBrush::LinkInfo(pLink, LinkBrush::LinkInfo::EDIT));
						pLink->RemoveObject(pInfo);
					}
				}
			}
			pRetAction->bldgAction.addBuildingInfo(*pInfo); // undo!
			EditorObjectMgr::instance()->deleteObject(pInfo);
		}
		it++;
	}
	// EditorObjectMgr::instance()->deleteSelectedObjects();
	for (size_t i = 0; i < land->realVerticesMapSide; ++i)
	{
		for (size_t j = 0; j < land->realVerticesMapSide; ++j)
		{
			if (land->isVertexSelected(j, i))
			{
				pRetAction->addChangedVertexInfo(j, i);
				land->setOverlay(j, i, INVALID_OVERLAY, 0);
				land->setTerrain(j, i, DEFAULT_TERRAIN);
				for (size_t icell = 0; icell < terrain_const::MAPCELL_DIM; icell++)
				{
					for (size_t jcell = 0; jcell < terrain_const::MAPCELL_DIM; jcell++)
					{
						int32_t cellRow = j * terrain_const::MAPCELL_DIM + jcell;
						int32_t cellCol = i * terrain_const::MAPCELL_DIM + icell;
						GameMap->setMine(cellRow, cellCol, 0);
					}
				}
			}
		}
	}
	return pRetAction;
}

bool
Eraser::EraserAction::undo()
{
	bool bRetVal = bldgAction.redo();
	bRetVal = linkAction.undo() && bRetVal;
	/*teamAction.undo() must occur after bldgAction.redo() (after the deleted
	buildings have been restored)*/
	if (teamsActionIsSet)
	{
		bRetVal = teamsAction.undo() && bRetVal;
	}
	bRetVal = ActionPaintTile::undo() && bRetVal;
	return bRetVal;
}

bool
Eraser::EraserAction::redo()
{
	bool bRetVal = true;
	/*teamAction.redo() must occur before bldgAction.undo() (before the
	 * buildings are deleted)*/
	if (teamsActionIsSet)
	{
		bRetVal = teamsAction.redo();
	}
	bRetVal = linkAction.redo() && bRetVal;
	bRetVal = bldgAction.undo() && bRetVal;
	bRetVal = ActionPaintTile::undo() && bRetVal;
	return bRetVal;
}
// end of file ( eraser.cpp )
