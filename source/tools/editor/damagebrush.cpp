#define DAMAGEBRUSH_CPP
/*************************************************************************************************\
DamageBrush.cpp			: Implementation of the DamageBrush component.
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//
\*************************************************************************************************/

#include "DamageBrush.h"
#include "editorobjectmgr.h"

bool DamageBrush::beginPaint()
{
	if (!pAction)
		pAction = new ModifyBuildingAction();
	return true;
}

Action* DamageBrush::endPaint()
{
	Action* pRetAction = nullptr;
	if (pAction)
	{
		if (pAction->isNotNull())
			pRetAction = pAction;
		else
		{
			delete pAction;
		}
	}
	pAction = nullptr;
	return pRetAction;
}

bool DamageBrush::paint(
	Stuff::Vector3D& worldPos, int32_t screenX, int32_t screenY)
{
	EditorObject* pObject = const_cast<EditorObject*>(
		EditorObjectMgr::instance()->getObjectAtPosition(worldPos));
	if (pObject)
	{
		pAction->addBuildingInfo(*pObject);
		pObject->setDamage(damage);
	}
	return true;
}

bool DamageBrush::canPaint(
	Stuff::Vector3D& worldPos, int32_t screenX, int32_t screenY, int32_t flags)
{
	const EditorObject* pObject =
		EditorObjectMgr::instance()->getObjectAtPosition(worldPos);
	return pObject ? true : false;
}

bool DamageBrush::canPaintSelection()
{
	return EditorObjectMgr::instance()->hasSelection();
}

Action* DamageBrush::applyToSelection()
{
	ModifyBuildingAction* pRetAction = new ModifyBuildingAction;
	EditorObjectMgr::EDITOR_OBJECT_LIST selectedObjectsList =
		EditorObjectMgr::instance()->getSelectedObjectList();
	EditorObjectMgr::EDITOR_OBJECT_LIST::EIterator it =
		selectedObjectsList.Begin();
	while (!it.IsDone())
	{
		EditorObject* pInfo = (*it);
		if (pInfo)
		{
			pRetAction->addBuildingInfo(*pInfo);
			pInfo->setDamage(damage);
		}
		it++;
	}
	if (!(pRetAction->isNotNull()))
	{
		delete pRetAction;
		pRetAction = nullptr;
	}
	return pRetAction;
}

//*************************************************************************************************
// end of file ( DamageBrush.cpp )
