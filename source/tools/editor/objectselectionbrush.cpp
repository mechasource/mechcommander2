/*************************************************************************************************\
ObjectSelectionBrush.cpp			: Implementation of the ObjectSelectionBrush component.
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//
\*************************************************************************************************/
// #define OBJECTSELECTIONBRUSH_CPP

#include "stdafx.h"

// test header dependencies one by one
//#include "action.h"
//#include "booleanflagissetdialog.h"
//#include "brush.h"
//#include "buildingbrush.h"
//#include "buildinglink.h"
//#include "buildingsettingsdlg.h"
//#include "campaigndata.h"
//#include "campaigndialog.h"
//#include "choosebuildingdlg.h"
//#include "chooseunitdlg.h"
//#include "damagebrush.h"
//#include "dlgfileopen.h"
//#include "dragtool.h"
//#include "dropzonebrush.h"
//#include "editforestdlg.h"
//#include "editorcamera.h"
//#include "editordata.h"
//#include "editorinterface.h"
//#include "editormessages.h"
//#include "editormfc.h"
//#include "editorobjectmgr.h"
//#include "editorobjects.h"
//#include "editortacmap.h"
//#include "editorview.h"
//#include "eraser.h"
//#include "flattenbrush.h"
//#include "fogdlg.h"
//#include "forest.h"
//#include "forestdlg.h"
//#include "fractaldialog.h"
//#include "groupdialog.h"
//#include "heightdlg.h"
//#include "linkbrush.h"
//#include "mainfrm.h"
//#include "mainmenu.h"
//#include "mapsizedlg.h"
//#include "messagebox.h"
//#include "minebrush.h"
//#include "missiondialog.h"
//#include "missionsettingsdlg.h"
//#include "newsinglemission.h"
//#include "objective.h"
//#include "objectivedlg.h"
//#include "objectivesdlg.h"
//#include "objectselectionbrush.h"
//#include "overlaybrush.h"
//#include "playersettingsdlg.h"
//#include "pointerselectobjectdlg.h"
//#include "resourcestringselectiondlg.h"
//#include "selectionbrush.h"
//#include "selectslopedialog.h"
//#include "selectterraintypedlg.h"
//#include "singlevaluedlg.h"
//#include "sundlg.h"
//#include "targetareadlg.h"
//#include "terrainbrush.h"
//#include "terraindlg.h"
//#include "textmessagedlg.h"
//#include "tgawnd.h"
//#include "tilingfactorsdialog.h"
//#include "unitsettingsdlg.h"
//#include "usertextedit.h"
//#include "viewconditionoractiondlg.h"
//#include "waterdlg.h"
//#include "wavedlg.h"

#if _CONSIDERED_OBSOLETE
/* -------------------------------------------------------------------------- */

#include "objectselectionbrush.h"
#include "camera.h"
#include "editorobjectmgr.h"
#include "action.h"
#include "utilities.h"
#include "editormessages.h"


ObjectSelectionBrush::ObjectSelectionBrush()
{
	bPainting = false;
	pCurAction = nullptr;
	lastPos.x = lastPos.y = lastPos.z = lastPos.w = 0.0f;		//Keep the FPU exception from going off!
	bFirstClick = false;
}

ObjectSelectionBrush::~ObjectSelectionBrush()
{
	if(EditorObjectMgr::instance)
		EditorObjectMgr::instance->unselectAll();
	if(land)
		land->unselectAll();
}

bool ObjectSelectionBrush::beginPaint()
{
	lastPos.x = lastPos.y = 0.0;
	bPainting = true;
	bFirstClick = !bFirstClick;
	return true;
}

Action* ObjectSelectionBrush::endPaint()
{
	bPainting = false;
	Action* pRetAction = nullptr;
	if(pCurAction)
	{
		if(pCurAction->vertexInfoList.Count())
		{
			pRetAction = pCurAction;
			pCurAction = nullptr;
			land->recalcWater();
			land->reCalcLight();
		}
		else
		{
			delete pCurAction;
			pCurAction = nullptr;
		}
	}
	return pRetAction;
}

bool ObjectSelectionBrush::paint(Stuff::Vector3D& worldPos, int32_t screenX, int32_t screenY)
{
	Stuff::Vector4D endPos;
	endPos.x = screenX;
	endPos.y = screenY;
	//if ( bFirstClick ) // otherwise, do a new area select
	{
		bool bShift = GetAsyncKeyState(KEY_LSHIFT);
		// select the objects
		if(lastPos.x != 0.0 && lastPos.y != 0.0)
		{
			if(!bShift)
			{
				land->unselectAll();
				EditorObjectMgr::instance->unselectAll();
			}
			eye->projectZ(lastWorldPos, lastPos);
			EditorObjectMgr::instance->select(lastPos, endPos);
			land->selectVerticesInRect(lastPos, endPos);
		}
		else
		{
			if(lastPos != endPos)
			{
				land->unselectAll();
				EditorObjectMgr::instance->unselectAll();
			}
			lastPos = endPos;
			Stuff::Vector2DOf<int32_t> screenPos;
			screenPos.x = screenX;
			screenPos.y = screenY;
			eye->inverseProject(screenPos, lastWorldPos);
			const EditorObject* pInfo = EditorObjectMgr::instance->getObjectAtPosition(worldPos);
			if(pInfo)
			{
				if(!bShift || (bShift && pInfo->isSelected() == false))
					(const_cast<EditorObject*>(pInfo))->select(true);
				else
					(const_cast<EditorObject*>(pInfo))->select(false);
			}
			else
			{
				int32_t tileR, tileC;
				land->worldToTile(worldPos, tileR, tileC);
				if(tileR > -1 && tileR < land->realVerticesMapSide
						&& tileC > -1 && tileC < land->realVerticesMapSide)
				{
					// figure out which vertex is closest
					if(fabs(worldPos.x - land->tileColToWorldCoord[tileC]) >= land->worldUnitsPerVertex / 2)
						tileC++;
					if(fabs(worldPos.y - land->tileRowToWorldCoord[tileR]) >= land->worldUnitsPerVertex / 2)
						tileR++;
					if(!bShift || (bShift && !land->isVertexSelected(tileR, tileC)))
						land->selectVertex(tileR, tileC);
					else // shift key, object is selected
						land->selectVertex(tileR, tileC, false);
				}
			}
		}
	}
	return true;
}

void ObjectSelectionBrush::render(int32_t screenX, int32_t screenY)
{
	if(bPainting)
	{
		//------------------------------------------
		Stuff::Vector4D Screen;
		eye->projectZ(lastWorldPos, Screen);
		RECT rect = { screenX, screenY, Screen.x, Screen.y };
		drawRect(rect, 0x30ffffff);
		drawEmptyRect(rect, 0xff000000, 0xff000000);
	}
	if(!bPainting)
	{
		EditorInterface::instance()->ChangeCursor(IDC_TARGET);
	}
}

EditorObjectPointerList ObjectSelectionBrush::selectedObjectPointerList()
{
	return EditorObjectMgr::instance->getSelectedObjectPointerList();
}

#endif

//*************************************************************************************************
// end of file ( ObjectSelectionBrush.cpp )
