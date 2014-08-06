//===========================================================================//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

// CampaignDialog.cpp : implementation file
//

#include "stdafx.h"
#include "CampaignDialog.h"
#include "CampaignData.h"
#include "GroupDialog.h"
#include "assert.h"
#include "../MCLib/Utilities.h" /*for cLoadString*/
#include "UserTextEdit.h"
#include "../MCLib/Paths.h"

#if 0 /*gos doesn't like this */
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif /*gos doesn't like this */

/////////////////////////////////////////////////////////////////////////////
// CCampaignDialog dialog


CCampaignDialog::CCampaignDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CCampaignDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CCampaignDialog)
	m_CBillsEdit = 0;
	m_FinalVideoEdit = _T("");
	m_NameEdit = _T("");
	m_NameResourceStringIDEdit = _T("");
	//}}AFX_DATA_INIT
}


void CCampaignDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCampaignDialog)
	DDX_Control(pDX, IDC_CA_GROUP_LIST, m_GroupListControl);
	DDX_Text(pDX, IDC_CA_CBILLS_EDIT, m_CBillsEdit);
	DDX_Text(pDX, IDC_CA_FINAL_VIDEO_EDIT, m_FinalVideoEdit);
	DDX_Text(pDX, IDC_CA_NAME_EDIT, m_NameEdit);
	DDX_Text(pDX, IDC_CA_NAME_RESOURCE_STRING_ID_EDIT, m_NameResourceStringIDEdit);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CCampaignDialog, CDialog)
	//{{AFX_MSG_MAP(CCampaignDialog)
	ON_BN_CLICKED(IDC_CA_FINAL_VIDEO_BROWSE_BUTTON, OnCaFinalVideoBrowseButton)
	ON_BN_CLICKED(ID_CA_EXIT, OnCaExit)
	ON_BN_CLICKED(IDC_CA_ADD_BUTTON, OnCaAddButton)
	ON_BN_CLICKED(ID_CA_SAVE_AS_BUTTON, OnCaSaveAsButton)
	ON_BN_CLICKED(ID_CA_LOAD_BUTTON, OnCaLoadButton)
	ON_BN_CLICKED(IDC_CA_EDIT_BUTTON, OnCaEditButton)
	ON_BN_CLICKED(IDC_CA_REMOVE_BUTTON, OnCaRemoveButton)
	ON_BN_CLICKED(IDC_CA_MOVE_UP_BUTTON, OnCaMoveUpButton)
	ON_BN_CLICKED(IDC_CA_MOVE_DOWN_BUTTON, OnCaMoveDownButton)
	ON_BN_CLICKED(IDC_CA_NAME_EDIT_BUTTON, OnCaNameEditButton)
	ON_BN_CLICKED(ID_CA_SAVE_BUTTON, OnCaSaveButton)
	ON_WM_CLOSE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCampaignDialog message handlers

void CCampaignDialog::OnCaFinalVideoBrowseButton() 
{
	UpdateData(TRUE);
	while (true) {
		CFileDialog selectFileDialog(TRUE,_T("AVI"),_T("*.AVI"),
						 OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT|OFN_NOCHANGEDIR,
						 _T("Movie (*.AVI)|*.AVI|"));
		selectFileDialog.m_ofn.lpstrInitialDir = moviePath;
		if( selectFileDialog.DoModal()==IDOK ) {
			m_FinalVideoEdit = selectFileDialog.GetFileTitle();
			break;
		} else {
			break;
		}
	}
	UpdateData(FALSE);
}

void CCampaignDialog::OnCaExit() 
{
	int res = PromptAndSaveIfNecessary();
	if (IDCANCEL != res) {
		EndDialog(IDOK);
	}
}

static void setGroupListBoxValues(CListBox &GroupListBox, const CGroupList &GroupList) {
	GroupListBox.ResetContent();
	CGroupList::EConstIterator it;
	for (it = GroupList.Begin(); !it.IsDone(); it++) {
		CString tmpCStr;
		tmpCStr = _TEXT("[");
		tmpCStr += ((*it).m_Label);
		tmpCStr += _TEXT("] ");
		GroupListBox.AddString(tmpCStr);
	}
}

static void setGroupDialogValues(CGroupDialog &groupDialog, const CGroupData &groupData) {
	groupDialog.m_LabelEdit = groupData.m_Label;
	groupDialog.m_NumMissionsToCompleteEdit = groupData.m_NumMissionsToComplete;
	groupDialog.m_OperationFileEdit = groupData.m_OperationFile;
	groupDialog.m_PreVideoFileEdit = groupData.m_PreVideoFile;
	groupDialog.m_VideoFileEdit = groupData.m_VideoFile;
	groupDialog.m_MissionList = groupData.m_MissionList;
	groupDialog.m_TuneNumber = groupData.m_TuneNumber;
}

static void setGroupDataValues(CGroupData &groupData, const CGroupDialog &groupDialog) {
	groupData.m_Label = groupDialog.m_LabelEdit;
	groupData.m_NumMissionsToComplete = groupDialog.m_NumMissionsToCompleteEdit;
	groupData.m_OperationFile = groupDialog.m_OperationFileEdit;
	groupData.m_PreVideoFile = groupDialog.m_PreVideoFileEdit;
	groupData.m_VideoFile = groupDialog.m_VideoFileEdit;
	groupData.m_MissionList = groupDialog.m_MissionList;
	groupData.m_TuneNumber = groupDialog.m_TuneNumber;
}

void CCampaignDialog::OnCaAddButton() 
{
	CGroupData groupData;
	CGroupDialog groupDialog;
	setGroupDialogValues(groupDialog, groupData);
	int ret = groupDialog.DoModal();
	if (IDOK == ret) {
		setGroupDataValues(groupData, groupDialog);
		m_CampaignData.m_GroupList.Append(groupData);
		UpdateData(TRUE);
		setGroupListBoxValues(m_GroupListControl, m_CampaignData.m_GroupList);
		m_GroupListControl.SetCurSel(m_GroupListControl.GetCount() - 1);
		UpdateData(FALSE);
	}
}

static void setCampaignDialogValues(CCampaignDialog &campaignDialog, const CCampaignData &campaignData) {
	campaignDialog.m_NameUnlocalizedText = campaignData.m_Name;
	campaignDialog.m_NameUseResourceString = campaignData.m_NameUseResourceString;
	campaignDialog.m_NameResourceStringID = campaignData.m_NameResourceStringID;
	campaignDialog.m_CBillsEdit = campaignData.m_CBills;
	campaignDialog.m_FinalVideoEdit = campaignData.m_FinalVideo;
	setGroupListBoxValues(campaignDialog.m_GroupListControl, campaignData.m_GroupList);
	campaignDialog.UpdateData(FALSE);
}

static void setCampaignDataValues(CCampaignData &campaignData, CCampaignDialog &campaignDialog) {
	campaignDialog.UpdateData();
	campaignData.m_Name = campaignDialog.m_NameUnlocalizedText;
	campaignData.m_NameUseResourceString = campaignDialog.m_NameUseResourceString;
	campaignData.m_NameResourceStringID = campaignDialog.m_NameResourceStringID;
	campaignData.m_CBills = campaignDialog.m_CBillsEdit;
	campaignData.m_FinalVideo = campaignDialog.m_FinalVideoEdit;
}

int CCampaignDialog::SaveAs() 
{
	CFileDialog selectFileDialog(FALSE,_T("FIT"),_T("*.FIT"),
					 OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT|OFN_NOCHANGEDIR,
					 _T("Campaign File (*.FIT)|*.FIT|"));
	{
		/* if the mission directory doesn't exist, we attempt to create it */
		int curDirStrSize = GetCurrentDirectory(0, NULL);
		TCHAR *curDirStr = new TCHAR[curDirStrSize];
		GetCurrentDirectory(curDirStrSize, curDirStr);
		BOOL result = SetCurrentDirectory(campaignPath);
		SetCurrentDirectory(curDirStr);
		delete curDirStr; curDirStr = 0;

		if (0 == result) {
			CreateDirectory(campaignPath, NULL);
		}
	}
	selectFileDialog.m_ofn.lpstrInitialDir = campaignPath;

	int retval = selectFileDialog.DoModal();
	if( IDOK == retval ) {
		setCampaignDataValues(m_CampaignData, *this);
		m_CampaignData.m_PathName = selectFileDialog.GetPathName();
		m_CampaignData.Save(m_CampaignData.m_PathName);
		m_LastSavedCampaignData = m_CampaignData;
	}
	return retval;
}

void CCampaignDialog::OnCaSaveAsButton() 
{
	SaveAs();
}

void CCampaignDialog::OnCaLoadButton() 
{
	int res = PromptAndSaveIfNecessary();
	if (IDCANCEL == res) {
		return;
	}
	CFileDialog selectFileDialog(TRUE,_T("FIT"),_T("*.FIT"),
					 OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT|OFN_NOCHANGEDIR,
					 _T("Campaign File (*.FIT)|*.FIT|"));
	selectFileDialog.m_ofn.lpstrInitialDir = campaignPath;
	if( selectFileDialog.DoModal()==IDOK ) {
		CString pathname = selectFileDialog.GetPathName();
		m_CampaignData.Clear();
		m_CampaignData.Read(pathname);
		m_CampaignData.m_PathName = pathname;
		m_LastSavedCampaignData = m_CampaignData;
		setCampaignDialogValues(*this, m_CampaignData);
		UpdateData(FALSE);
		UpdateNameDisplay();
	}
}

void CCampaignDialog::OnCaEditButton() 
{
	int selectedItemIndex = m_GroupListControl.GetCurSel();
	if ((0 <= selectedItemIndex) && ((int)m_CampaignData.m_GroupList.Count() > selectedItemIndex)) {
		CGroupData &groupDataRef = m_CampaignData.m_GroupList[selectedItemIndex];
		CGroupDialog groupDialog;
		setGroupDialogValues(groupDialog, groupDataRef);
		if (IDOK == groupDialog.DoModal()) {
			setGroupDataValues(groupDataRef, groupDialog);
			UpdateData(TRUE);
			setGroupListBoxValues(m_GroupListControl, m_CampaignData.m_GroupList);
			UpdateData(FALSE);
		}
	}
}

void CCampaignDialog::OnCaRemoveButton() 
{
	int selectedItemIndex = m_GroupListControl.GetCurSel();
	if ((0 <= selectedItemIndex) && ((int)m_CampaignData.m_GroupList.Count() > selectedItemIndex)) {
		// should put up confirmation box here
		CGroupList::EIterator it = m_CampaignData.m_GroupList.Begin();
		int index;
		for (index = 0; index < selectedItemIndex; index++) {
			it++;
			assert(!it.IsDone());
		}
		m_CampaignData.m_GroupList.Delete(it);
		UpdateData(TRUE);
		int selectedItemIndex = m_GroupListControl.GetCurSel();
		setGroupListBoxValues(m_GroupListControl, m_CampaignData.m_GroupList);
		if ((LB_ERR != selectedItemIndex) && (0 < m_GroupListControl.GetCount())) {
			if (m_GroupListControl.GetCount() <= (long)selectedItemIndex) {
				selectedItemIndex = m_GroupListControl.GetCount() - 1;
			}
			m_GroupListControl.SetCurSel(selectedItemIndex);
		}
		UpdateData(FALSE);
	}
}

void CCampaignDialog::OnCaMoveUpButton() 
{
	int selectedItemIndex = m_GroupListControl.GetCurSel();
	if ((1 <= selectedItemIndex) && ((int)m_CampaignData.m_GroupList.Count() > selectedItemIndex)) {
		CGroupData tmpGroupData;
		tmpGroupData = (*m_CampaignData.m_GroupList.Iterator(selectedItemIndex));

		m_CampaignData.m_GroupList.Delete(selectedItemIndex);
		m_CampaignData.m_GroupList.Insert(tmpGroupData, selectedItemIndex - 1);
		UpdateData(TRUE);
		setGroupListBoxValues(m_GroupListControl, m_CampaignData.m_GroupList);
		m_GroupListControl.SetCurSel(selectedItemIndex - 1);
		UpdateData(FALSE);
	}
}

void CCampaignDialog::OnCaMoveDownButton() 
{
	int selectedItemIndex = m_GroupListControl.GetCurSel();
	if ((0 <= selectedItemIndex) && ((int)m_CampaignData.m_GroupList.Count() - 1 > selectedItemIndex)) {
		CGroupData tmpGroupData;
		tmpGroupData = (*m_CampaignData.m_GroupList.Iterator(selectedItemIndex));

		m_CampaignData.m_GroupList.Delete(selectedItemIndex);
		if ((int)m_CampaignData.m_GroupList.Count() -1 == selectedItemIndex) {
			m_CampaignData.m_GroupList.Append(tmpGroupData);
		} else {
			m_CampaignData.m_GroupList.Insert(tmpGroupData, selectedItemIndex + 1);
		}
		UpdateData(TRUE);
		setGroupListBoxValues(m_GroupListControl, m_CampaignData.m_GroupList);
		m_GroupListControl.SetCurSel(selectedItemIndex + 1);
		UpdateData(FALSE);
	}
}

static BOOL CSLoadString(int resourceID, CString &targetStr) {
	char szTmp[16384/*max string length*/];
	cLoadString( resourceID, szTmp, 16384/*max string length*/ );
	targetStr = szTmp;
	CString tmpStr;
	tmpStr.Format("mc2res.dll:%d Not defined", resourceID);
	if (0 == strcmp(tmpStr.GetBuffer(0), szTmp)) {
		return (0);
	}
	return (!0);
}

void CCampaignDialog::UpdateNameDisplay() {
	UpdateData(TRUE);
	if (m_NameUseResourceString) {
		m_NameResourceStringIDEdit.Format("%d", m_NameResourceStringID);
		int ret = CSLoadString(m_NameResourceStringID, m_NameEdit);
		if (0 == ret) {
			m_NameEdit = _TEXT("");
		}
	} else {
		m_NameResourceStringIDEdit = _TEXT("");
		m_NameEdit = m_NameUnlocalizedText;
	}
	UpdateData(FALSE);
}

void CCampaignDialog::OnCaNameEditButton() 
{
	CUserTextEdit userTextEditDialog;
	userTextEditDialog.m_UnlocalizedText = m_NameUnlocalizedText;
	userTextEditDialog.m_UseResourceString = m_NameUseResourceString;
	userTextEditDialog.m_ResourceStringID = m_NameResourceStringID;
	int ret = userTextEditDialog.DoModal();
	if (IDOK == ret) {
		m_NameUnlocalizedText = userTextEditDialog.m_UnlocalizedText;
		m_NameUseResourceString = userTextEditDialog.m_UseResourceString;
		m_NameResourceStringID = userTextEditDialog.m_ResourceStringID;
		UpdateNameDisplay();
	}
}

int CCampaignDialog::Save() 
{
	int retVal = IDOK;
	if (m_CampaignData.m_PathName.IsEmpty()) {
		retVal = SaveAs();
	} else {
		m_CampaignData.Save(m_CampaignData.m_PathName);
		m_LastSavedCampaignData = m_CampaignData;
		retVal = IDOK;
	}
	return retVal;
}

void CCampaignDialog::OnCaSaveButton() 
{
	Save();
}

int CCampaignDialog::PromptAndSaveIfNecessary()
{
	setCampaignDataValues(m_CampaignData, *this);
	int res = IDNO;
	bool endFlag = false;
	while (!endFlag) {
		endFlag = true;
		if (!(m_LastSavedCampaignData == m_CampaignData)) {
			res = AfxMessageBox(IDS_DO_YOU_WANT_TO_SAVE_YOUR_CHANGES, MB_YESNOCANCEL);
		}
		if (IDYES == res) {
			int saveRes = Save();
			if (IDCANCEL == saveRes) {
				endFlag = false;
			}
		}
	}
	return res;
}

BOOL CCampaignDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();

	setCampaignDialogValues(*this, m_CampaignData);
	// TODO: Add extra initialization here
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CCampaignDialog::OnClose() 
{
	int res = PromptAndSaveIfNecessary();
	if (IDCANCEL != res) {
		CDialog::OnClose();
	}
}
