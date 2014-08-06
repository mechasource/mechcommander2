//===========================================================================//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

// UserTextEdit.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "UserTextEdit.h"

#include "assert.h"
#include "../MCLib/Utilities.h" /*for cLoadString*/
#include "TextMessageDlg.h"
#include "ResourceStringSelectionDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CUserTextEdit dialog


CUserTextEdit::CUserTextEdit(CWnd* pParent /*=NULL*/)
	: CDialog(CUserTextEdit::IDD, pParent)
{
	m_UseResourceString = false;
	m_ResourceStringID = 0;

	//{{AFX_DATA_INIT(CUserTextEdit)
	m_Edit = _T("");
	m_ResourceStringIDEdit = _T("");
	//}}AFX_DATA_INIT
}


void CUserTextEdit::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CUserTextEdit)
	DDX_Text(pDX, IDC_USER_TEXT_EDIT_EDIT, m_Edit);
	DDX_Text(pDX, IDC_USER_TEXT_EDIT_RESOURCE_STRING_ID_EDIT, m_ResourceStringIDEdit);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CUserTextEdit, CDialog)
	//{{AFX_MSG_MAP(CUserTextEdit)
	ON_BN_CLICKED(IDC_USER_TEXT_EDIT_ENTER_TEXT_BUTTON, OnUserTextEditEnterTextButton)
	ON_BN_CLICKED(IDC_USER_TEXT_EDIT_SELECT_RESOURCE_STRING_BUTTON, OnUserTextEditSelectResourceStringButton)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CUserTextEdit message handlers

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

void CUserTextEdit::UpdateTextDisplay() {
	UpdateData(TRUE);
	if (m_UseResourceString) {
		m_ResourceStringIDEdit.Format("%d", m_ResourceStringID);
		int ret = CSLoadString(m_ResourceStringID, m_Edit);
		if (0 == ret) {
			m_Edit = _TEXT("");
		}
	} else {
		m_ResourceStringIDEdit = _TEXT("");
		m_Edit = m_UnlocalizedText;
	}
	UpdateData(FALSE);
}

void CUserTextEdit::OnUserTextEditEnterTextButton() 
{
	TextMessageDlg textMessageDlg;
	textMessageDlg.m_TextMessage = m_UnlocalizedText;
	int ret = textMessageDlg.DoModal();
	if (IDOK == ret) {
		m_UnlocalizedText = textMessageDlg.m_TextMessage;
		/*remove trailing "new line"s and "carriage return"s because the game doesn't like them*/
		while (("\n" == m_UnlocalizedText.Right(1)) || ("\r" == m_UnlocalizedText.Right(1))) {
			m_UnlocalizedText = m_UnlocalizedText.Left(m_UnlocalizedText.GetLength() - 1);
		}
		m_UseResourceString = false;
		UpdateTextDisplay();
	}
}

void CUserTextEdit::OnUserTextEditSelectResourceStringButton() 
{
	ResourceStringSelectionDlg resourceStringSelectionDlg;
	resourceStringSelectionDlg.m_SelectedResourceStringID = m_ResourceStringID;
	int ret = resourceStringSelectionDlg.DoModal();
	if (IDOK == ret) {
		m_ResourceStringID = resourceStringSelectionDlg.m_SelectedResourceStringID;
		m_UseResourceString = true;
		UpdateTextDisplay();
	}
}

BOOL CUserTextEdit::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	UpdateTextDisplay();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
