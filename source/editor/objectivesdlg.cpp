//===========================================================================//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

// ObjectivesDlg.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "ObjectivesDlg.h"

#include "assert.h"
#include "EditorInterface.h"

#if 0 /*gos doesn't like this */
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif /*gos doesn't like this */


/////////////////////////////////////////////////////////////////////////////
// ObjectivesDlg dialog


ObjectivesDlg::ObjectivesDlg(CWnd* pParent /*=NULL*/)
	: CDialog(ObjectivesDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(ObjectivesDlg)
	m_TeamEdit = 0;
	//}}AFX_DATA_INIT

	nSelectionIndex = -1;
}


void ObjectivesDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(ObjectivesDlg)
	DDX_Control(pDX, IDC_OBJECTIVES_EDIT_BUTTON, m_EditButton);
	DDX_Control(pDX, IDC_OBJECTIVES_ADD_BUTTON, m_AddButton);
	DDX_Control(pDX, IDC_OBJECTIVES_LIST, m_List);
	DDX_Text(pDX, IDC_OBJECTIVES_TEAM_EDIT, m_TeamEdit);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(ObjectivesDlg, CDialog)
	//{{AFX_MSG_MAP(ObjectivesDlg)
	ON_BN_CLICKED(IDC_OBJECTIVES_ADD_BUTTON, OnObjectivesAddButton)
	ON_BN_CLICKED(IDC_OBJECTIVES_REMOVE_BUTTON, OnObjectivesRemoveButton)
	ON_BN_CLICKED(IDC_OBJECTIVES_EDIT_BUTTON, OnObjectivesEditButton)
	ON_BN_CLICKED(IDC_OBJECTIVES_COPY_BUTTON, OnObjectivesCopyButton)
	ON_BN_CLICKED(IDC_OBJECTIVES_MOVE_UP_BUTTON, OnObjectivesMoveUpButton)
	ON_BN_CLICKED(IDC_OBJECTIVES_MOVE_DOWN_BUTTON, OnObjectivesMoveDownButton)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// ObjectivesDlg message handlers

static void syncObjectivesListWithListBox(const CObjectives *pObjectives, CListBox *pList) {
	pList->ResetContent();
	CObjectives::EConstIterator it = pObjectives->Begin();
	while (!it.IsDone())
	{
		EString tmpEStr;
		tmpEStr = _TEXT("[");
		if (1 == (*it)->Priority()) {
			tmpEStr += _TEXT("*");
		} else {
			//tmpEStr += _TEXT(" ");
		}
		if ((*it)->PreviousPrimaryObjectiveMustBeComplete()) {
			tmpEStr += _TEXT("^");
		} else {
			//tmpEStr += _TEXT(" ");
		}
		if ((*it)->AllPreviousPrimaryObjectivesMustBeComplete()) {
			tmpEStr += _TEXT("A");
		} else {
			//tmpEStr += _TEXT(" ");
		}
		if ((*it)->IsHiddenTrigger()) {
			tmpEStr += _TEXT("H");
		} else {
			//tmpEStr += _TEXT(" ");
		}
		if ((*it)->ActivateOnFlag()) {
			tmpEStr += _TEXT("F");
		} else {
			//tmpEStr += _TEXT(" ");
		}
		tmpEStr += _TEXT("] ");
		tmpEStr += ((*it)->LocalizedTitle());
		pList->AddString(tmpEStr.Data());
		it++;
	}
}

BOOL ObjectivesDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	m_TeamEdit = m_ModifiedObjectives.Alignment() + 1;
	syncObjectivesListWithListBox(&m_ModifiedObjectives, &m_List);
	m_List.SetCurSel(nSelectionIndex);
	UpdateData(FALSE);
	
	if (EditorInterface::instance()->ObjectSelectOnlyMode()) {
		if (CObjectivesEditState::ADD == EditorInterface::instance()->objectivesEditState.objectiveFunction) {
			// post a message that the ADD button was pressed
			PostMessage(WM_COMMAND, MAKEWPARAM(IDC_OBJECTIVES_ADD_BUTTON, BN_CLICKED), (LPARAM)((&m_AddButton)->m_hWnd));
		} else if (CObjectivesEditState::EDIT == EditorInterface::instance()->objectivesEditState.objectiveFunction) {
			// post a message that the EDIT button was pressed
			PostMessage(WM_COMMAND, MAKEWPARAM(IDC_OBJECTIVES_EDIT_BUTTON, BN_CLICKED), (LPARAM)((&m_EditButton)->m_hWnd));
		} else { assert(false); }
	} else {
		EditorInterface::instance()->objectivesEditState.alignment = m_ModifiedObjectives.Alignment();
		EditorInterface::instance()->objectivesEditState.ModifiedObjectives = m_ModifiedObjectives;
		EditorInterface::instance()->objectivesEditState.nSelectionIndex = m_List.GetCurSel();
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void ObjectivesDlg::OnObjectivesAddButton() 
{
	CObjective *pNewObjective = new CObjective(m_ModifiedObjectives.Alignment());
	assert(pNewObjective);

	/* This call may set the editor into ObjectSelectOnlyMode (i.e.: set the  value of
	EditorInterface::instance()->ObjectSelectOnlyMode() to true) */
	bool result = pNewObjective->EditDialog();

	if (EditorInterface::instance()->ObjectSelectOnlyMode()) {
		/* close the dialog and enter ObjectSelectOnlyMode */
		UpdateData(TRUE);
		EditorInterface::instance()->objectivesEditState.objectiveFunction = CObjectivesEditState::ADD;
		EditorInterface::instance()->objectivesEditState.alignment = m_ModifiedObjectives.Alignment();
		EditorInterface::instance()->objectivesEditState.ModifiedObjectives = m_ModifiedObjectives;
		EditorInterface::instance()->objectivesEditState.nSelectionIndex = m_List.GetCurSel();
		delete pNewObjective; pNewObjective = 0;
		EndDialog(IDOK);
		return;
	} else {
		if (true == result) {
			m_ModifiedObjectives.Append(pNewObjective);
			m_List.SetCurSel(m_List.GetCount() - 1);
			syncObjectivesListWithListBox(&m_ModifiedObjectives, &m_List);
			nSelectionIndex = m_List.GetCurSel();
		} else {
			delete pNewObjective; pNewObjective = 0;
		}
		return;
	}
}

void ObjectivesDlg::OnObjectivesRemoveButton() 
{
	nSelectionIndex = m_List.GetCurSel();
	if ((0 <= nSelectionIndex) && (m_ModifiedObjectives.Count() > nSelectionIndex)) {
		// should put up confirmation box here
		delete *(m_ModifiedObjectives.Iterator(nSelectionIndex));
		m_ModifiedObjectives.Delete(nSelectionIndex);
		syncObjectivesListWithListBox(&m_ModifiedObjectives, &m_List);
		if (0 < m_List.GetCount()) {
			if (m_List.GetCount() <= (long)nSelectionIndex) {
				nSelectionIndex = m_List.GetCount() - 1;
			}
			m_List.SetCurSel(nSelectionIndex);
		}
	}
	nSelectionIndex = m_List.GetCurSel();
}

void ObjectivesDlg::OnObjectivesEditButton() 
{
	CObjective *pSelectedObjective = 0;
	if (!EditorInterface::instance()->ObjectSelectOnlyMode()) {
		nSelectionIndex = m_List.GetCurSel();
		if ((0 <= nSelectionIndex) && (((int)(m_ModifiedObjectives.Count())) > nSelectionIndex)) {
			pSelectedObjective = *(m_ModifiedObjectives.Iterator(nSelectionIndex));
		} else {
			return;
		}
	} else {
		assert(CObjectivesEditState::EDIT == EditorInterface::instance()->objectivesEditState.objectiveFunction);
		pSelectedObjective = *(m_ModifiedObjectives.Iterator(nSelectionIndex));
	}
	assert(0 != pSelectedObjective);
	assert(0 <= nSelectionIndex);

	/* This call may set the editor into ObjectSelectOnlyMode (i.e.: set the  value of
	EditorInterface::instance()->ObjectSelectOnlyMode() to true) */
	pSelectedObjective->EditDialog();

	if (EditorInterface::instance()->ObjectSelectOnlyMode()) {
		UpdateData(TRUE);
		EditorInterface::instance()->objectivesEditState.objectiveFunction = CObjectivesEditState::EDIT;
		EditorInterface::instance()->objectivesEditState.alignment = m_ModifiedObjectives.Alignment();
		EditorInterface::instance()->objectivesEditState.ModifiedObjectives = m_ModifiedObjectives;
		EditorInterface::instance()->objectivesEditState.nSelectionIndex = m_List.GetCurSel();
		EndDialog(IDOK);
		return;
	} else {
		nSelectionIndex = m_List.GetCurSel();
		syncObjectivesListWithListBox(&m_ModifiedObjectives, &m_List);
		if ((0 <= nSelectionIndex) && (m_List.GetCount() > nSelectionIndex)) {
			m_List.SetCurSel(nSelectionIndex);
		}
		nSelectionIndex = m_List.GetCurSel();
		return;
	}
}

void ObjectivesDlg::OnObjectivesCopyButton() 
{
	nSelectionIndex = m_List.GetCurSel();
	if ((0 <= nSelectionIndex) && (m_ModifiedObjectives.Count() > nSelectionIndex)) {
		CObjective *pSelectedObjective = 0;
		pSelectedObjective = *(m_ModifiedObjectives.Iterator(nSelectionIndex));
		assert(0 != pSelectedObjective);

		CObjective *pNewObjective = new CObjective(pSelectedObjective->Alignment());
		pNewObjective->Init();
		*pNewObjective = *pSelectedObjective;
		m_ModifiedObjectives.Append(pNewObjective);
		syncObjectivesListWithListBox(&m_ModifiedObjectives, &m_List);
		m_List.SetCurSel(m_List.GetCount() - 1);
		nSelectionIndex = m_List.GetCurSel();
	}
}

void ObjectivesDlg::OnObjectivesMoveUpButton() 
{
	nSelectionIndex = m_List.GetCurSel();
	if ((1 <= nSelectionIndex) && (m_ModifiedObjectives.Count() > nSelectionIndex)) {
		CObjective *pSelectedObjective = 0;
		pSelectedObjective = *(m_ModifiedObjectives.Iterator(nSelectionIndex));
		assert(0 != pSelectedObjective);

		m_ModifiedObjectives.Delete(nSelectionIndex);
		m_ModifiedObjectives.Insert(pSelectedObjective, nSelectionIndex - 1);
		syncObjectivesListWithListBox(&m_ModifiedObjectives, &m_List);
		m_List.SetCurSel(nSelectionIndex - 1);
		nSelectionIndex = m_List.GetCurSel();
	}
}

void ObjectivesDlg::OnObjectivesMoveDownButton() 
{
	nSelectionIndex = m_List.GetCurSel();
	if ((0 <= nSelectionIndex) && (m_ModifiedObjectives.Count() - 1 > nSelectionIndex)) {
		CObjective *pSelectedObjective = 0;
		pSelectedObjective = *(m_ModifiedObjectives.Iterator(nSelectionIndex));
		assert(0 != pSelectedObjective);

		m_ModifiedObjectives.Delete(nSelectionIndex);
		if (m_ModifiedObjectives.Count() -1 == nSelectionIndex) {
			m_ModifiedObjectives.Append(pSelectedObjective);
		} else {
			m_ModifiedObjectives.Insert(pSelectedObjective, nSelectionIndex + 1);
		}
		syncObjectivesListWithListBox(&m_ModifiedObjectives, &m_List);
		m_List.SetCurSel(nSelectionIndex + 1);
		nSelectionIndex = m_List.GetCurSel();
	}
}
