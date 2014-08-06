//===========================================================================//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

// BooleanFlagIsSetDialog.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "BooleanFlagIsSetDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CBooleanFlagIsSetDialog dialog


CBooleanFlagIsSetDialog::CBooleanFlagIsSetDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CBooleanFlagIsSetDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CBooleanFlagIsSetDialog)
	m_Value = -1;
	m_FlagID = _T("");
	//}}AFX_DATA_INIT
}


void CBooleanFlagIsSetDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CBooleanFlagIsSetDialog)
	DDX_CBIndex(pDX, IDC_FLAG_IS_SET_VALUE_COMBO, m_Value);
	DDX_Text(pDX, IDC_FLAG_IS_SET_ID_EDIT, m_FlagID);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CBooleanFlagIsSetDialog, CDialog)
	//{{AFX_MSG_MAP(CBooleanFlagIsSetDialog)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBooleanFlagIsSetDialog message handlers
