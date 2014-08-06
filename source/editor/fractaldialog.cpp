/*************************************************************************************************\
FractalDialog.cpp	: Implementation of the FractalDlg component.
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//
\*************************************************************************************************/

#include "FractalDialog.h"
#include "resource.h"

/////////////////////////////////////////////////////////////////////////////
// FractalDlg dialog

FractalDlg::FractalDlg(CWnd* pParent) : CDialog(FractalDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(FogDlg)
	m_threshold = 1;
	m_noise = 0;
	//}}AFX_DATA_INIT
}

void FractalDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(FogDlg)
	DDX_Text(pDX, IDC_THRESHOLD, m_threshold);
	DDX_Text(pDX, IDC_NOISE, m_noise);
	//}}AFX_DATA_MAP
}


/////////////////////////////////////////////////////////////////////////////

