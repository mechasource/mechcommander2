/*************************************************************************************************\
DlgFileOpen.cpp			: Implementation of the DlgFileOpen component.
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//
\*************************************************************************************************/

#include "stdinc.h"
#include "resource.h"

//#include "toolos.hpp"
#include "dlgfileopen.h"

BEGIN_MESSAGE_MAP(DlgFileOpen, CDialog)
	//{{AFX_MSG_MAP(DlgFileOpen)
	ON_LBN_SELCHANGE(IDC_FILEOPEN_FILELIST, OnSelchangeFileopenFilelist)
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//-------------------------------------------------------------------------------------------------
DlgFileOpen::DlgFileOpen(std::wstring_view directory, std::wstring_view dlgExtension, bool bsave)
	: CDialog(IDD_FILEOPEN)
{
	strcpy(m_directory, directory);
	strcpy(fileName, directory);
	if (dlgExtension)
	{
		strcpy(extension, dlgExtension);
	}
	else
	{
		strcpy(extension, "");
	}
	m_bSave = bsave;
}

void DlgFileOpen::Init()
{
	CListBox* m_pList = (CListBox*)GetDlgItem(IDC_FILEOPEN_FILELIST);
	// Not Used.
	// CEdit* m_pEntry = (CEdit*)GetDlgItem(IDC_FILEOPEN_EDITBOX);
	wchar_t dirBuffer[1024];
	strcpy(dirBuffer, m_directory);
	strcat(dirBuffer, "*.");
	strcat(dirBuffer, extension);
	std::wstring_view pFileFirst = gos_FindFiles(dirBuffer);
	while (pFileFirst)
	{
		m_pList->AddString(pFileFirst);
		pFileFirst = gos_FindFilesNext();
	}
	strcpy(fileName, m_directory);
}

void DlgFileOpen::OnCancel()
{
	// EndDialog(IDCANCEL);
	EndDialog(IDCANCEL);
}

void DlgFileOpen::OnOK()
{
	CEdit* m_pEntry = (CEdit*)GetDlgItem(IDC_FILEOPEN_EDITBOX);
	wchar_t pszEntryString[1024 /*MAX_STRING_LENGTH*/];
	m_pEntry->GetWindowText(pszEntryString, 1024 /*MAX_STRING_LENGTH*/);
	strcat(fileName, pszEntryString);
	wchar_t tmpExtension[32];
	tmpExtension[0] = '.';
	tmpExtension[1] = 0;
	strcat(tmpExtension, extension);
	if (!fileName || (strlen(fileName) < 4 || 0 != _stricmp(tmpExtension, fileName + strlen(fileName) - 4)))
	{
		strcat(fileName, ".");
		strcat(fileName, extension);
	}
	CDialog::OnOK();
}

//-------------------------------------------------------------------------------------------------

DlgFileOpen::~DlgFileOpen() { }

// end of file ( DlgFileOpen.cpp )

BOOL DlgFileOpen::OnInitDialog()
{
	CDialog ::OnInitDialog();
	Init();
	return TRUE; // return TRUE unless you set the focus to a control
		// EXCEPTION: OCX Property Pages should return FALSE
}

void DlgFileOpen::OnSelchangeFileopenFilelist()
{
	CListBox* m_pList = (CListBox*)GetDlgItem(IDC_FILEOPEN_FILELIST);
	gosASSERT(m_pList);
	int32_t nSelectionIndex = m_pList->GetCurSel();
	int32_t nStringLength = m_pList->GetTextLen(nSelectionIndex);
	if (0 < nStringLength)
	{
		std::wstring_view pszSelectionString = new wchar_t[nStringLength + 1];
		m_pList->GetText(nSelectionIndex, pszSelectionString);
		CEdit* m_pEntry = (CEdit*)GetDlgItem(IDC_FILEOPEN_EDITBOX);
		m_pEntry->SetWindowText(pszSelectionString);
		delete pszSelectionString;
	}
}
