//===========================================================================//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

// ResourceStringSelectionDlg.cpp : implementation file
//

#include "stdinc.h"
#include "resource.h"
#include "ResourceStringSelectionDlg.h"

#include "_ASSERT.h"
#include "utilities.h" /*for cLoadString*/

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static wchar_t THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// ResourceStringSelectionDlg dialog

ResourceStringSelectionDlg::ResourceStringSelectionDlg(CWnd* pParent /*=nullptr*/)
	: CDialog(ResourceStringSelectionDlg::IDD, pParent)
{
	m_BottomOfIDRange = 0;
	m_TopOfIDRange = 65535;
	m_SelectedResourceStringID = -1;
	//{{AFX_DATA_INIT(ResourceStringSelectionDlg)
	// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

void ResourceStringSelectionDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(ResourceStringSelectionDlg)
	DDX_Control(pDX, IDC_RESOURCE_STRING_SELECTION_COMBO, m_Combo);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(ResourceStringSelectionDlg, CDialog)
//{{AFX_MSG_MAP(ResourceStringSelectionDlg)
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// ResourceStringSelectionDlg message handlers

static BOOL
CSLoadString(int32_t resourceID, CString& targetStr)
{
	wchar_t szTmp[16384 /*max string length*/];
	cLoadString(resourceID, szTmp, 16384 /*max string length*/);
	targetStr = szTmp;
	CString tmpStr;
	tmpStr.Format("mc2res.dll:%d Not defined", resourceID);
	if (0 == strcmp(tmpStr.GetBuffer(0), szTmp))
	{
		return (0);
	}
	return (!0);
}

BOOL ResourceStringSelectionDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	m_ResourceStringIDs.Clear();
	m_Combo.ResetContent();
	uint32_t i;
	for (i = m_BottomOfIDRange; i <= (uint32_t)m_TopOfIDRange; i += 1)
	{
		CString tmpStr;
		// BOOL result = tmpStr.LoadString(i);
		BOOL result = CSLoadString(i, tmpStr);
		if (0 != result)
		{
			CString tmpStr2;
			tmpStr2.Format("%d: ", i);
			tmpStr2 += tmpStr;
			m_Combo.AddString(tmpStr2);
			m_ResourceStringIDs.Append(i);
		}
	}
	for (i = 0; i < m_ResourceStringIDs.Count(); i += 1)
	{
		if (m_SelectedResourceStringID == m_ResourceStringIDs[i])
		{
			m_Combo.SetCurSel(i);
			break;
		}
	}
	if (m_ResourceStringIDs.Count() == i)
	{
		m_SelectedResourceStringID = -1;
		m_Combo.SetCurSel(-1);
	}
	return TRUE; // return TRUE unless you set the focus to a control
		// EXCEPTION: OCX Property Pages should return FALSE
}

void ResourceStringSelectionDlg::OnOK()
{
	uint32_t selectionindex = m_Combo.GetCurSel();
	if (CB_ERR != selectionindex)
	{
		_ASSERT(m_ResourceStringIDs.Count() > selectionindex);
		m_SelectedResourceStringID = m_ResourceStringIDs[selectionindex];
	}
	CDialog::OnOK();
}
