

/*************************************************************************************************\
ChooseBuildingDlg.cpp			: Implementation of the ChooseBuildingDlg component.
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//
\*************************************************************************************************/

#include "resource.h"

#include <stdlib.h>
#include <assert.h>
#include "EString.h"

#include "ChooseBuildingDlg.h"
#include "Objective.h"

#include "EditorInterface.h"
#include "EditorObjectMgr.h"


//-------------------------------------------------------------------------------------------------
ChooseBuildingDlg::ChooseBuildingDlg( building_ptr_type &buildingPtr ):CDialog(IDD_CHOOSE_BUILDING)
{
	m_pBuildingPtr = &buildingPtr;
	m_pModifiedBuildingPtr = 0;
	m_pComboBox = 0;
	m_pUsingPointerButton = 0;
	m_pCancelButton = 0;
	m_pOKButton = 0;
}

BOOL ChooseBuildingDlg::OnInitDialog()
{
	m_pComboBox = (CComboBox *)GetDlgItem(IDC_CHOOSE_BUILDING_COMBO);
	assert( m_pComboBox );

	m_pUsingPointerButton = (CButton *)GetDlgItem(IDC_CHOOSE_BUILDING_USING_POINTER_BUTTON);
	assert( m_pUsingPointerButton );

	m_pCancelButton = (CButton *)GetDlgItem(IDCANCEL);
	assert( m_pCancelButton );

	m_pOKButton = (CButton *)GetDlgItem(IDOK);
	assert( m_pOKButton );

	if (EditorInterface::instance()->ObjectSelectOnlyMode()) {
		m_pModifiedBuildingPtr = (building_ptr_type *)EditorInterface::instance()->objectivesEditState.pModifiedBuildingPtr;
	} else {
		m_pModifiedBuildingPtr = new building_ptr_type;
		(*m_pModifiedBuildingPtr) = (*m_pBuildingPtr);
	}

	m_buildingList.Clear();
	EditorObjectMgr::BUILDING_LIST completeBuildingList = EditorObjectMgr::instance()->getBuildings();
	EditorObjectMgr::BUILDING_LIST::EConstIterator it2 = completeBuildingList.Begin();
	while (!it2.IsDone()) {
		//if ((BUILDING == typeNum) || (TREEBUILDING == typeNum)) {
		if (true) {
			m_buildingList.Append(*it2);
		}
		it2++;
	}

	EditorObjectMgr::BUILDING_LIST::EConstIterator it = m_buildingList.Begin();
	while (!it.IsDone()) {
		EString tmpEStr;
		Stuff::Vector3D pos = (*it)->getPosition();
		const char *szDisplayName = (*it)->getDisplayName(); // nb: localization
		assert(szDisplayName);
		tmpEStr.Format("(pos: %.3f, %.3f) %s", pos.x, pos.y, szDisplayName);
		m_pComboBox->AddString(tmpEStr.Data());
		it++;
	}

	if (1 <= m_buildingList.Count()) {
		m_pComboBox->SetCurSel(0);
	} else {
		AfxMessageBox(IDS_NO_STRUCTURES);
		OnCancel();
	}

	if (EditorInterface::instance()->ObjectSelectOnlyMode()) {
		// post a message that the USEPOINTER button was pressed
		PostMessage(WM_COMMAND, (WPARAM)BN_CLICKED, (LPARAM)m_pUsingPointerButton->m_hWnd);
	}
	return 1;
}

BOOL ChooseBuildingDlg::OnCommand(WPARAM wParam, LPARAM lParam) // called by child controls to inform of an event
{
	assert( m_pCancelButton );
	assert( m_pOKButton );

	HWND hWndCtrl = (HWND)lParam;
	int nCode = HIWORD(wParam);

	if (hWndCtrl == m_pComboBox->m_hWnd)
	{
		if (LBN_SELCHANGE == nCode)
		{
		}
	}
	else if (hWndCtrl == m_pUsingPointerButton->m_hWnd)
	{
		if (BN_CLICKED == nCode)
		{
			OnUsePointer();
			if (EditorInterface::instance()->ObjectSelectOnlyMode()) {
				/* close the dialog and enter ObjectSelectOnlyMode */
				EditorInterface::instance()->objectivesEditState.pModifiedBuildingPtr = m_pModifiedBuildingPtr;
				EndDialog(IDOK);
			}
		}
	}
	return inherited::OnCommand(wParam, lParam);
}

void ChooseBuildingDlg::OnUsePointer()
{
	if (!EditorInterface::instance()->ObjectSelectOnlyMode()) {
		EditorInterface::instance()->SelectionMode();
		EditorInterface::instance()->ObjectSelectOnlyMode(true);
		return;
	} else {
		EditorInterface::instance()->ObjectSelectOnlyMode(false);

		EditorObjectMgr::EDITOR_OBJECT_LIST selectedObjects = EditorObjectMgr::instance()->getSelectedObjectList();
		int num_buildings_selected = 0;
		int validObjectIndex = -1;
		EditorObjectMgr::EDITOR_OBJECT_LIST::EConstIterator it = selectedObjects.Begin();
		while (!it.IsDone()) {
			int index = 0;
			EditorObjectMgr::BUILDING_LIST::EConstIterator it2 = m_buildingList.Begin();
			while (!it2.IsDone()) {
				if ((*it) == (*it2)) {
					num_buildings_selected += 1;
					validObjectIndex = index;
				}
				index += 1;
				it2++;
			}
			it++;
		}
		if (0 > validObjectIndex) {
			AfxMessageBox(IDS_NO_BUILDINGS_SELECTED);
		} else {
			if (1 < num_buildings_selected) {
				AfxMessageBox(IDS_MORE_THAN_ONE_BUILDING_SELECTED);
			}
			m_pComboBox->SetCurSel(validObjectIndex);
		}
	}
}

void ChooseBuildingDlg::OnCancel()
{
	//assert(false); // haven't decided what to do on cancel yet
	delete m_pModifiedBuildingPtr;
	EndDialog(IDCANCEL);
}

void ChooseBuildingDlg::OnOK()
{
	int nSelectionIndex = m_pComboBox->GetCurSel();
	assert(0 <= nSelectionIndex);
	//(*m_pModifiedBuildingPtr) = m_buildingList[nSelectionIndex];
	(*m_pModifiedBuildingPtr) = *(m_buildingList.Iterator(nSelectionIndex));

	(*m_pBuildingPtr) = (*m_pModifiedBuildingPtr);
	delete m_pModifiedBuildingPtr;

	EndDialog(IDOK);
}


//-------------------------------------------------------------------------------------------------

ChooseBuildingDlg::~ChooseBuildingDlg()
{
}


//*************************************************************************************************
// end of file ( ChooseBuildingDlg.cpp )
