//===========================================================================//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

// MissionDialog.cpp : implementation file
//

#include "stdafx.h"
//include "CampaignEditor.h"
#include "resource.h"
#include "MissionDialog.h"
#include "../MCLib/Paths.h"
#include "Inifile.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMissionDialog dialog


CMissionDialog::CMissionDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CMissionDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CMissionDialog)
	m_MissionFileEdit = _T("");
	m_PurchaseFileEdit = _T("");
	m_LogisticsCheck = FALSE;
	m_MandatoryCheck = FALSE;
	m_PilotPromotionCheck = FALSE;
	m_PurchasingCheck = FALSE;
	m_SalvageCheck = FALSE;
	m_SelectionScreenCheck = FALSE;
	//}}AFX_DATA_INIT
}


void CMissionDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMissionDialog)
	DDX_Text(pDX, IDC_MI_MISSION_FILE_EDIT, m_MissionFileEdit);
	DDX_Text(pDX, IDC_MI_PURCHASE_FILE_EDIT, m_PurchaseFileEdit);
	DDX_Check(pDX, IDC_MI_LOGISTICS_CHECK, m_LogisticsCheck);
	DDX_Check(pDX, IDC_MI_MANDATORY_CHECK, m_MandatoryCheck);
	DDX_Check(pDX, IDC_MI_PILOT_PROMOTION_CHECK, m_PilotPromotionCheck);
	DDX_Check(pDX, IDC_MI_PURCHASING_CHECK, m_PurchasingCheck);
	DDX_Check(pDX, IDC_MI_SALVAGE_CHECK, m_SalvageCheck);
	DDX_Check(pDX, IDC_MI_SELECTION_SCREEN, m_SelectionScreenCheck );
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CMissionDialog, CDialog)
	//{{AFX_MSG_MAP(CMissionDialog)
	ON_BN_CLICKED(IDC_MI_MISSION_FILE_BROWSE_BUTTON, OnMiMissionFileBrowseButton)
	ON_BN_CLICKED(IDC_MI_PURCHASE_FILE_BROWSE_BUTTON, OnMiPurchaseFileBrowseButton)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMissionDialog message handlers

void CMissionDialog::OnMiMissionFileBrowseButton() {
	UpdateData(TRUE);
	while (true) {
		CFileDialog selectFileDialog(TRUE,_T("PAK"),_T("*.PAK"),
						 OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT|OFN_NOCHANGEDIR,
						 _T("PAK File (*.PAK)|*.PAK|"));
		selectFileDialog.m_ofn.lpstrInitialDir = missionPath;
		if( selectFileDialog.DoModal()==IDOK ) {
			m_MissionFileEdit = selectFileDialog.GetFileTitle();
			break;
		} else {
			break;
		}
	}
	UpdateData(FALSE);
}

void CMissionDialog::OnMiPurchaseFileBrowseButton() {
	UpdateData(TRUE);
	while (true) {
		CFileDialog selectFileDialog(TRUE,_T("FIT"),_T("*.FIT"),
						 OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT|OFN_NOCHANGEDIR,
						 _T("FIT File (*.FIT)|*.FIT|"));
		selectFileDialog.m_ofn.lpstrInitialDir = missionPath;
		if( selectFileDialog.DoModal()==IDOK ) {
			CString purchasePath = selectFileDialog.GetPathName();
			FitIniFile file;
			long result = 0;
			result = file.open( (char*)(const char*)purchasePath );
			if (NO_ERR != result) {
				AfxMessageBox(IDS_COULDNT_OPEN_PURCHASE_FILE);
			} else {
				long result = 0;
				result = file.seekBlock("Mechs");
				if (NO_ERR != result)
				{
					AfxMessageBox(IDS_NOT_A_VALID_PURCHASE_FILE);
				}
				file.close();
			}
			m_PurchaseFileEdit = selectFileDialog.GetFileTitle();
			break;
		} else {
			break;
		}
	}
	UpdateData(FALSE);
}
