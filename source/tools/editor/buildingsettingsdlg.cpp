//===========================================================================//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

// BuildingSettingsDlg.cpp : implementation file
//

#include "stdinc.h"

#include "resource.h"
#include "buildingsettingsdlg.h"
#include "editorobjects.h"
#include "editorobjectmgr.h"
#include "editorinterface.h" // just for the undo manager

//#include "unitdesg.h" /* just for definition of MIN_TERRAIN_PART_ID
// and MAX_MAP_CELL_WIDTH */

/////////////////////////////////////////////////////////////////////////////
// BuildingSettingsDlg dialog

BuildingSettingsDlg::BuildingSettingsDlg(
	EList<EditorObject*, EditorObject*>& newList /*=nullptr*/, ActionUndoMgr& undoMgr) :
	CDialog(BuildingSettingsDlg::IDD),
	units(newList)
{
	//{{AFX_DATA_INIT(BuildingSettingsDlg)
	m_Alignment = -1;
	m_x = 0.0f;
	m_y = 0.0f;
	m_partID = 0;
	m_forestName = _T("");
	//}}AFX_DATA_INIT
	pUndoMgr = &undoMgr;
	pAction = nullptr;
}

void
BuildingSettingsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(BuildingSettingsDlg)
	DDX_Control(pDX, IDC_MECH, m_Mech);
	DDX_Control(pDX, IDC_GROUP, m_Group);
	DDX_Radio(pDX, IDC_ALIGN1, m_Alignment);
	DDX_Text(pDX, IDC_BS_X_EDIT, m_x);
	DDX_Text(pDX, IDC_BS_Y_EDIT, m_y);
	DDX_Text(pDX, IDC_BS_PARTID_EDIT, m_partID);
	DDX_Text(pDX, IDC_FOREST_NAME, m_forestName);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(BuildingSettingsDlg, CDialog)
//{{AFX_MSG_MAP(BuildingSettingsDlg)
ON_CBN_SELCHANGE(IDC_GROUP, OnSelchangeGroup)
ON_CBN_SELCHANGE(IDC_MECH, OnSelchangeMech)
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// BuildingSettingsDlg message handlers

void
BuildingSettingsDlg::OnSelchangeGroup()
{
	m_Mech.ResetContent();
	int32_t group = m_Group.GetCurSel();
	group = m_Group.GetItemData(group);
	const std::wstring_view& MechNames[256];
	int32_t count = 256;
	EditorObjectMgr::instance()->getBuildingNamesInGroup(group, MechNames, count);
	for (size_t i = 0; i < count; ++i)
	{
		m_Mech.AddString(MechNames[i]);
	}
	m_Mech.SetCurSel(0);
}

void
BuildingSettingsDlg::applyChanges()
{
	// get the type info from the dlg box
	int32_t index = m_Group.GetCurSel();
	if (index != -1)
	{
		int32_t group = m_Group.GetItemData(index);
		int32_t indexInGroup = m_Mech.GetCurSel();
		if (indexInGroup != -1)
		{
			for (EDITOROBJECT_LIST::EIterator iter = units.Begin(); !iter.IsDone(); iter++)
			{
				(*iter)->setAppearance(group, indexInGroup);
			}
		}
	}
	// now set the alignment
	UpdateData(true);
	index = m_Alignment;
	if (index != -1)
	{
		for (EDITOROBJECT_LIST::EIterator iter = units.Begin(); !iter.IsDone(); iter++)
		{
			(*iter)->setAlignment(index);
		}
	}
	uint32_t base = 0, color1 = 0, color2 = 0;
	bool bBase = false;
	bool bcolour1 = false;
	bool bcolour2 = false;
	// now figure out the colors
	CWnd* pWnd = GetDlgItem(IDC_BASE);
	if (pWnd)
	{
		CString tmpStr;
		pWnd->GetWindowText(tmpStr);
		if (tmpStr.GetLength())
		{
			bBase = true;
			tmpStr.Replace("0x", "");
			sscanf_s(tmpStr, "%x", &base);
			base |= 0xff000000;
		}
	}
	pWnd = GetDlgItem(IDC_HIGHLIGHT1);
	if (pWnd)
	{
		CString tmpStr;
		pWnd->GetWindowText(tmpStr);
		if (tmpStr.GetLength())
		{
			bcolour1 = true;
			tmpStr.Replace("0x", "");
			sscanf_s(tmpStr, "%x", &color1);
			color1 |= 0xff000000;
		}
	}
	pWnd = GetDlgItem(IDC_HIGHLIGHT2);
	if (pWnd)
	{
		CString tmpStr;
		pWnd->GetWindowText(tmpStr);
		if (tmpStr.GetLength())
		{
			bcolour2 = true;
			tmpStr.Replace("0x", "");
			sscanf_s(tmpStr, "%x", &color2);
			color2 |= 0xff000000;
		}
	}
}

void
BuildingSettingsDlg::OnOK()
{
	if (nullptr != pUndoMgr)
	{
		pUndoMgr->AddAction(pAction);
	}
	else
	{
		delete pAction;
	}
	pAction = nullptr;
	applyChanges();
	CDialog::OnOK();
}

BOOL
BuildingSettingsDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	pAction = new ModifyBuildingAction;
	EDITOROBJECT_LIST::EIterator iter;
	for (iter = units.Begin(); !iter.IsDone(); iter++)
	{
		pAction->addBuildingInfo(*(*iter));
	}
	updateMemberVariables();
	return TRUE; // return TRUE unless you set the focus to a control
		// EXCEPTION: OCX Property Pages should return FALSE
}

void
BuildingSettingsDlg::OnSelchangeMech()
{
	int32_t group = m_Group.GetCurSel();
	group = m_Group.GetItemData(group);
	/*int32_t indexInGroup =*/m_Mech.GetCurSel();
}

void
BuildingSettingsDlg::updateMemberVariables()
{
	int32_t forest = -1;
	bool bForests = true;
	EditorObject* pEditorObject = units.GetHead();
	m_Alignment = pEditorObject->getAlignment();
	if (m_Alignment == -1)
		m_Alignment = 8; // Move it to the neutral select button
	for (EDITOROBJECT_LIST::EIterator iter = units.Begin(); !iter.IsDone(); iter++)
	{
		if (((*iter)->getAlignment() != m_Alignment) && ((*iter)->getAlignment() != -1))
		{
			m_Alignment = -1;
			break;
		}
		if ((*iter)->getForestID() != -1)
		{
			if (forest == -1)
				forest = (*iter)->getForestID();
			else if (forest != (*iter)->getForestID())
				bForests = false;
		}
	}
	if (forest != -1)
	{
		const Forest* pForest = EditorObjectMgr::instance()->getForest(forest);
		if (pForest)
		{
			m_forestName = pForest->getFileName();
		}
	}
	EditorObjectMgr* pMgr = EditorObjectMgr::instance();
	int32_t groupCount = pMgr->getBuildingGroupCount();
	const std::wstring_view&* pGroups = new const std::wstring_view&[groupCount];
	m_Group.ResetContent();
	pMgr->getBuildingGroupNames(pGroups, groupCount);
	int32_t count = 0;
	for (size_t i = 0; i < groupCount; ++i)
	{
		if ((4 /*mech group*/ == i) || (6 /*vehicle group*/ == i))
		{
			continue;
		}
		m_Group.AddString(pGroups[i]);
		m_Group.SetItemData(count, (uint32_t)i);
		count += 1;
	}
	delete[] pGroups;
	// make sure all the units we are editing are in the same group
	int32_t group = units.GetHead()->getGroup();
	for (iter = units.Begin(); !iter.IsDone(); iter++)
	{
		if ((*iter)->getGroup() != group)
		{
			group = -1;
			break;
		}
	}
	if (group != -1) // we found a valid group
	{
		const std::wstring_view& pGroupName = pMgr->getGroupName(group);
		int32_t index = m_Group.FindString(-1, pGroupName);
		m_Group.SetCurSel(index);
		// OK, now fill in the index....
		const std::wstring_view& MechNames[256];
		int32_t count = 256;
		m_Mech.ResetContent();
		pMgr->getBuildingNamesInGroup(group, MechNames, count);
		for (size_t i = 0; i < count; ++i)
		{
			m_Mech.AddString(MechNames[i]);
		}
		// ok, now determine if all of the mechs are the same.
		int32_t indexInGroup = units.GetHead()->getIndexInGroup();
		for (iter = units.Begin(); !iter.IsDone(); iter++)
		{
			if ((*iter)->getIndexInGroup() != indexInGroup)
			{
				indexInGroup = -1;
				break;
			}
		}
		if (indexInGroup != -1)
		{
			const std::wstring_view& pName = units.GetHead()->getDisplayName();
			index = m_Mech.FindString(-1, pName);
			if (index != -1)
			{
				m_Mech.SetCurSel(index);
			}
		}
	}
	m_x = pEditorObject->getPosition().x;
	m_y = pEditorObject->getPosition().y;
	int32_t row, column;
	pEditorObject->getCells(row, column);
	m_partID = MIN_TERRAIN_PART_ID + row * MAX_MAP_CELL_WIDTH + column;
	UpdateData(false);
}

void
BuildingSettingsDlg::OnCancel()
{
	pAction->undo();
	delete pAction;
	pAction = nullptr;
	CDialog::OnCancel();
}
