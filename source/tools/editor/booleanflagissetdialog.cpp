//===========================================================================//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

// BooleanFlagIsSetDialog.cpp : implementation file
//

#include "stdinc.h"
#include "resource.h"
#include "booleanflagissetdialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static wchar_t THIS_FILE[] = __FILE__;
#endif

// enum { IDD = IDD_FLAG_IS_SET_DIALOG };

/////////////////////////////////////////////////////////////////////////////
// CBooleanFlagIsSetDialog dialog

CBooleanFlagIsSetDialog::CBooleanFlagIsSetDialog(CWnd* pParent /*=nullptr*/)
	: CDialog(IDD_FLAG_IS_SET_DIALOG, pParent)
{
	//{{AFX_DATA_INIT(CBooleanFlagIsSetDialog)
	m_Index = -1;
	// m_FlagID = _T("");
	//}}AFX_DATA_INIT
}

void CBooleanFlagIsSetDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CBooleanFlagIsSetDialog)
	DDX_CBIndex(pDX, IDC_FLAG_IS_SET_VALUE_COMBO, m_Index);
	// DDX_Text(pDX, IDC_FLAG_IS_SET_ID_EDIT, m_FlagID);
	//}}AFX_DATA_MAP
	HWND hWndCtrl = pDX->PrepareEditCtrl(IDC_FLAG_IS_SET_ID_EDIT);
	if (pDX->m_bSaveAndValidate)
	{
		size_t nLen = (size_t)::GetWindowTextLengthW(hWndCtrl);
		std::vector<wchar_t> buffer(nLen + 1);
		::GetWindowTextW(hWndCtrl, buffer.data(), (int32_t)nLen + 1);
		m_FlagID.assign(buffer.data(), nLen);
	}
	else
	{
		::SetWindowTextW(hWndCtrl, m_FlagID.c_str());
	}
}

BEGIN_MESSAGE_MAP(CBooleanFlagIsSetDialog, CDialog)
//{{AFX_MSG_MAP(CBooleanFlagIsSetDialog)
// NOTE: the ClassWizard will add message map macros here
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBooleanFlagIsSetDialog message handlers
