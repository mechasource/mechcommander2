//===========================================================================//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

// SelectSlopeDialog.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "SelectSlopeDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// SelectSlopeDialog dialog


SelectSlopeDialog::SelectSlopeDialog(CWnd* pParent /*=NULL*/)
	: CDialog(SelectSlopeDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(SelectSlopeDialog)
	m_MaxEdit = 0.0f;
	m_MinEdit = 0.0f;
	//}}AFX_DATA_INIT
}


void SelectSlopeDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(SelectSlopeDialog)
	DDX_Text(pDX, IDC_SELECT_SLOPE_MAX_EDIT, m_MaxEdit);
	DDX_Text(pDX, IDC_SELECT_SLOPE_MIN_EDIT, m_MinEdit);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(SelectSlopeDialog, CDialog)
	//{{AFX_MSG_MAP(SelectSlopeDialog)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// SelectSlopeDialog message handlers
