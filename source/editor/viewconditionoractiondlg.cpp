//===========================================================================//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

// ViewConditionOrActionDlg.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "ViewConditionOrActionDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// ViewConditionOrActionDlg dialog


ViewConditionOrActionDlg::ViewConditionOrActionDlg(CWnd* pParent /*=NULL*/)
	: CDialog(ViewConditionOrActionDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(ViewConditionOrActionDlg)
	m_Edit = _T("");
	//}}AFX_DATA_INIT
}


void ViewConditionOrActionDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(ViewConditionOrActionDlg)
	DDX_Text(pDX, IDC_VIEW_CONDITION_OR_ACTION_EDIT, m_Edit);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(ViewConditionOrActionDlg, CDialog)
	//{{AFX_MSG_MAP(ViewConditionOrActionDlg)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// ViewConditionOrActionDlg message handlers
