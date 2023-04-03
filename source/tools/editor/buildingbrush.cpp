/*************************************************************************************************\
BuildingBrush.cpp	: Implementation of the BuildingBrush component.
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//
\*************************************************************************************************/
// #define BUILDINGBRUSH_CPP

#include "stdinc.h"

#include "buildingbrush.h"
#include "editorobjectmgr.h"
#include "terrain.h"
#include "editorobjects.h"
#include "editorinterface.h"
#include "resource.h"

BuildingBrush::BuildingBrush(int32_t group, int32_t IndexInGroup, int32_t Alignment)
{
	group = group;
	indexInGroup = IndexInGroup;
	pAction = nullptr;
	pCursor = EditorObjectMgr::instance()->getAppearance(group, IndexInGroup);
	pCursor->teamId = Alignment;
	pCursor->setInView(true);
	pCursor->setVisibility(true, true);
	pCursor->position = eye->getPosition();
	pCursor->update();
	alignment = Alignment;
}

BuildingBrush::BuildingBrush()
{
	pCursor = nullptr;
	group = indexInGroup = -1;
	pAction = nullptr;
}

BuildingBrush::~BuildingBrush()
{
	if (pCursor)
		delete pCursor;
}

bool BuildingBrush::canPaint(
	Stuff::Vector3D& worldPos, int32_t screenX, int32_t screenY, int32_t flags)
{
	if (!EditorObjectMgr::instance()->canAddBuilding(
			worldPos, pCursor->rotation, group, indexInGroup))
	{
		pCursor->setHighlightcolour(0x007f0000);
		return false; // no two things on top of each other
	}
	pCursor->setHighlightcolour(0x00007f00);
	return true;
}

bool BuildingBrush::beginPaint()
{
	gosASSERT(!pAction);
	pAction = new BuildingAction;
	return true; // need to set up undo here
}

bool BuildingBrush::paint(Stuff::Vector3D& worldPos, int32_t screenX, int32_t screenY)
{
	if ((AppearanceTypeList::appearanceHeap->totalCoreLeft() < 1000 /*arbitrary*/) || (AppearanceTypeList::appearanceHeap->totalCoreLeft() < 0.01 /*arbitrary*/ * AppearanceTypeList::appearanceHeap->size()))
	{
		AfxMessageBox(IDS_APPEARANCE_HEAP_EXHAUSTED);
		{
			/*The preceding message box will cause the app (EditorInterface) to
			miss the "left mouse button up" event, so we'll act on it here.*/
			POINT point;
			GetCursorPos(&point);
			EditorInterface::instance()->handleLeftButtonUp(point.x, point.y);
			ReleaseCapture();
		}
		return false;
	}
	else
	{
		EditorObject* pInfo = EditorObjectMgr::instance()->addBuilding(
			worldPos, group, indexInGroup, alignment, pCursor->rotation);
		{
			if (pInfo && pAction)
				pAction->addBuildingInfo(*pInfo);
			return true;
		}
	}
}

Action*
BuildingBrush::endPaint()
{
	if (pAction)
	{
		if (!pAction->objInfoPtrList.Count())
		{
			delete pAction;
			pAction = nullptr;
		}
	}
	Action* pRetAction = pAction;
	pAction = nullptr;
	return pRetAction;
}

bool BuildingBrush::BuildingAction::undo()
{
	bool bRetVal = true;
	/*
	OBJ_APPEAR_LIST::EIterator iter3 = buildingAppearanceCopies.Begin();
	for ( OBJ_INFO_LIST::EIterator iter = positions.Begin();
		!iter.IsDone(); iter++ )
	{
		EditorObject* pObj =
	EditorObjectMgr::instance()->getObjectAtLocation((*iter3).position.x,
	(*iter3).position.y); if (pObj)
		{
			bRetVal = EditorObjectMgr::instance()->deleteBuilding( pObj ) &&
	bRetVal;
		}
		else
		{
			gosASSERT(false);
		}

		iter3++;
	}
	*/
	for (OBJ_INFO_PTR_LIST::EIterator iter = objInfoPtrList.Begin(); !iter.IsDone(); iter++)
	{
		ObjectAppearance* pAppearance = (*(*iter)).appearance();
		EditorObject* pObj = EditorObjectMgr::instance()->getObjectAtLocation(
			pAppearance->position.x, pAppearance->position.y);
		if (pObj)
		{
			bRetVal = EditorObjectMgr::instance()->deleteBuilding(pObj) && bRetVal;
		}
		else
		{
			gosASSERT(false);
		}
	}
	return bRetVal;
}

bool BuildingBrush::BuildingAction::redo()
{
	bool bRetVal = true;
	for (OBJ_INFO_PTR_LIST::EIterator iter = objInfoPtrList.Begin(); !iter.IsDone(); iter++)
	{
		ObjectAppearance* pAppearance = (*(*iter)).appearance();
		EditorObject* pObj = EditorObjectMgr::instance()->addBuilding(pAppearance->position,
			EditorObjectMgr::instance()->getGroup((*(*iter)).getID()),
			EditorObjectMgr::instance()->getIndexInGroup((*(*iter)).getID()), pAppearance->teamId,
			pAppearance->rotation);
		if (pObj)
		{
			(*pObj).CastAndCopy((*(*iter)));
		}
		else
		{
			gosASSERT(false);
			bRetVal = false;
		}
	}
	return bRetVal;
}

void BuildingBrush::BuildingAction::addBuildingInfo(EditorObject& info)
{
	EditorObject* pCopy = info.Clone();
	gosASSERT(pCopy);
	objInfoPtrList.Append(pCopy);
}

void BuildingBrush::update(int32_t ScreenMouseX, int32_t ScreenMouseY)
{
	if (!pCursor)
		return;
	Stuff::Vector3D pos;
	Stuff::Vector2DOf<int32_t> pt;
	pt.x = ScreenMouseX;
	pt.y = ScreenMouseY;
	eye->inverseProject(pt, pos);
	if (!EditorObjectMgr::instance()->canAddBuilding(pos, pCursor->rotation, group, indexInGroup))
		pCursor->setHighlightcolour(0x00400000);
	else
		pCursor->setHighlightcolour(0x00004000);
	pCursor->position = pos;
	pCursor->recalcBounds();
	pCursor->update(); // Safe tp call here now because we run the first update
		// in the constructor which caches in texture
	// NOT TRUE WITH RIA CODE!!!!!  Must have a separate update or NO Triangles
	// get added!!!
	pCursor->setVisibility(true, true);
}

void BuildingBrush::render(int32_t ScreenMouseX, int32_t ScreenMouseY)
{
	if (!pCursor)
		return;
	/*
	Stuff::Vector3D pos;
	Stuff::Vector2DOf<int32_t> pt;
	pt.x = ScreenMouseX;
	pt.y = ScreenMouseY;
	eye->inverseProject( pt, pos );

	if ( !EditorObjectMgr::instance()->canAddBuilding( pos, group, indexInGroup ) )
		pCursor->setHighlightcolour( 0x00400000 );
	else
		pCursor->setHighlightcolour( 0x00004000 );

	pCursor->position = pos;
	pCursor->recalcBounds();
	pCursor->update();			//Safe tp call here now because we run the first update in the
	constructor which caches in texture
								//NOT TRUE WITH RIA CODE!!!!!  Must have a separate update or NO
	Triangles get added!!! pCursor->setVisibility( true, true );
	*/
	// This may cause cursor to lag.  Check it and see.
	pCursor->render();
}

void BuildingBrush::rotateBrush(int32_t direction)
{
	int32_t ID = EditorObjectMgr::instance()->getID(group, indexInGroup);
	int32_t fitID = EditorObjectMgr::instance()->getFitID(ID);
	if ((EditorObjectMgr::WALL == EditorObjectMgr::instance()->getSpecialType(ID)) || (33 /*repair bay*/ == fitID))
	{
		pCursor->rotation += direction * 90;
	}
	else
	{
		pCursor->rotation += direction * 45;
	}
}

// end of file ( BuildingBrush.cpp )
