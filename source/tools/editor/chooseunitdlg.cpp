/*************************************************************************************************\
ChooseUnitDlg.cpp			: Implementation of the ChooseUnitDlg component.
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//
\*************************************************************************************************/

#include "stdinc.h"
#include "resource.h"

//#include <stdlib.h>
//#include <_ASSERT.h>
//#include "estring.h"
//#include "chooseunitdlg.h"
//#include "pointerselectobjectdlg.h"
//#include "objective.h"
//#include "editorinterface.h"
//#include "editorobjectmgr.h"

//-------------------------------------------------------------------------------------------------
ChooseUnitDlg::ChooseUnitDlg(unit_ptr_type& unitPtr, int32_t alignmentToExclude)
	: CDialog(IDD_CHOOSE_UNIT)
{
	m_alignmentToExclude = alignmentToExclude;
	m_pUnitPtr = &unitPtr;
	m_pModifiedUnitPtr = 0;
	m_pComboBox = 0;
	m_pUsingPointerButton = 0;
	m_pCancelButton = 0;
	m_pOKButton = 0;
}

BOOL ChooseUnitDlg::OnInitDialog()
{
	m_pComboBox = (CComboBox*)GetDlgItem(IDC_CHOOSE_UNIT_COMBO);
	_ASSERT(m_pComboBox);
	m_pUsingPointerButton = (CButton*)GetDlgItem(IDC_CHOOSE_UNIT_USING_POINTER_BUTTON);
	_ASSERT(m_pUsingPointerButton);
	m_pCancelButton = (CButton*)GetDlgItem(IDCANCEL);
	_ASSERT(m_pCancelButton);
	m_pOKButton = (CButton*)GetDlgItem(IDOK);
	_ASSERT(m_pOKButton);
	if (EditorInterface::instance()->ObjectSelectOnlyMode())
	{
		m_pModifiedUnitPtr =
			(unit_ptr_type*)EditorInterface::instance()->objectivesEditState.pModifiedUnitPtr;
	}
	else
	{
		m_pModifiedUnitPtr = new unit_ptr_type;
		(*m_pModifiedUnitPtr) = (*m_pUnitPtr);
	}
	EditorObjectMgr::UNIT_LIST completeUnitList = EditorObjectMgr::instance()->getUnits();
	{
		EditorObjectMgr::UNIT_LIST::EConstIterator it = completeUnitList.Begin();
		while (!it.IsDone())
		{
			if ((*it)->getAlignment() != m_alignmentToExclude)
			{
				m_unitList.Append(*it);
			}
			it++;
		}
	}
	{
		EditorObjectMgr::UNIT_LIST::EConstIterator it = m_unitList.Begin();
		while (!it.IsDone())
		{
			std::wstring_view tmpEStr;
			Stuff::Vector3D pos = (*it)->getPosition();
			std::wstring_view szDisplayName = (*it)->getDisplayName(); // nb: localization
			_ASSERT(szDisplayName);
			tmpEStr.Format("(pos: %.3f, %.3f) %s", pos.x, pos.y, szDisplayName);
			m_pComboBox->AddString(tmpEStr.Data());
			it++;
		}
	}
	if (1 <= m_unitList.Count())
	{
		m_pComboBox->SetCurSel(0);
	}
	else
	{
		AfxMessageBox(IDS_NO_UNITS);
		OnCancel();
	}
	if (EditorInterface::instance()->ObjectSelectOnlyMode())
	{
		// post a message that the USEPOINTER button was pressed
		PostMessage(WM_COMMAND, (WPARAM)BN_CLICKED, (LPARAM)m_pUsingPointerButton->m_hWnd);
	}
	return 1;
}

BOOL ChooseUnitDlg::OnCommand(WPARAM wparam,
	LPARAM lparam) // called by child controls to inform of an event
{
	_ASSERT(m_pCancelButton);
	_ASSERT(m_pOKButton);
	HWND hWndCtrl = (HWND)lparam;
	int32_t nCode = HIWORD(wparam);
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
			if (EditorInterface::instance()->ObjectSelectOnlyMode())
			{
				/* close the dialog and enter ObjectSelectOnlyMode */
				EditorInterface::instance()->objectivesEditState.pModifiedUnitPtr =
					m_pModifiedUnitPtr;
				EndDialog(IDOK);
			}
		}
	}
	return inherited::OnCommand(wparam, lparam);
}

void ChooseUnitDlg::OnUsePointer()
{
	if (!EditorInterface::instance()->ObjectSelectOnlyMode())
	{
		EditorInterface::instance()->SelectionMode();
		EditorInterface::instance()->ObjectSelectOnlyMode(true);
		return;
	}
	else
	{
		EditorInterface::instance()->ObjectSelectOnlyMode(false);
		EditorObjectMgr::EDITOR_OBJECT_LIST selectedObjects =
			EditorObjectMgr::instance()->getSelectedObjectList();
		int32_t num_units_selected = 0;
		int32_t validObjectIndex = -1;
		EditorObjectMgr::EDITOR_OBJECT_LIST::EConstIterator it = selectedObjects.Begin();
		while (!it.IsDone())
		{
			int32_t index = 0;
			EditorObjectMgr::UNIT_LIST::EConstIterator it2 = m_unitList.Begin();
			while (!it2.IsDone())
			{
				if ((*it) == (*it2))
				{
					num_units_selected += 1;
					validObjectIndex = index;
				}
				index += 1;
				it2++;
			}
			it++;
		}
		if (0 > validObjectIndex)
		{
			AfxMessageBox(IDS_NO_UNITS_SELECTED);
		}
		else
		{
			if (1 < num_units_selected)
			{
				AfxMessageBox(IDS_MORE_THAN_ONE_UNIT_SELECTED);
			}
			m_pComboBox->SetCurSel(validObjectIndex);
		}
	}
}

void ChooseUnitDlg::OnCancel()
{
	// _ASSERT(false); // haven't decided what to do on cancel yet
	delete m_pModifiedUnitPtr;
	EndDialog(IDCANCEL);
}

void ChooseUnitDlg::OnOK()
{
	int32_t nSelectionIndex = m_pComboBox->GetCurSel();
	_ASSERT(0 <= nSelectionIndex);
	//(*m_pModifiedUnitPtr) = m_unitList[nSelectionIndex];
	(*m_pModifiedUnitPtr) = *(m_unitList.Iterator(nSelectionIndex));
	(*m_pUnitPtr) = (*m_pModifiedUnitPtr);
	delete m_pModifiedUnitPtr;
	EndDialog(IDOK);
}

//-------------------------------------------------------------------------------------------------

ChooseUnitDlg::~ChooseUnitDlg() { }

// end of file ( ChooseUnitDlg.cpp )
