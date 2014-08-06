//===========================================================================//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

// TextMessageDlg.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "TextMessageDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// TextMessageDlg dialog


TextMessageDlg::TextMessageDlg(CWnd* pParent /*=NULL*/)
	: CDialog(TextMessageDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(TextMessageDlg)
	m_TextMessage = _T("");
	//}}AFX_DATA_INIT
}


void TextMessageDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(TextMessageDlg)
	DDX_Text(pDX, IDC_TEXT_MESSAGE, m_TextMessage);
	DDV_MaxChars(pDX, m_TextMessage, 2000);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(TextMessageDlg, CDialog)
	//{{AFX_MSG_MAP(TextMessageDlg)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// TextMessageDlg message handlers
