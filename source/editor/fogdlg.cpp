/*************************************************************************************************\
FogDlg.cpp			: Implementation of the FogDlg component.
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//
\*************************************************************************************************/


#include "FogDlg.h"
#include "resource.h"

/////////////////////////////////////////////////////////////////////////////
// FogDlg dialog


FogDlg::FogDlg(CWnd* pParent /*=NULL*/)
	: CDialog(FogDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(FogDlg)
	m_blue = 0;
	m_green = 0;
	m_red = 0;
	m_start = 0.0f;
	m_end = 0.0f;
	//}}AFX_DATA_INIT
}


void FogDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(FogDlg)
	DDX_Text(pDX, IDC_BLUE1, m_blue);
	DDX_Text(pDX, IDC_GREEN1, m_green);
	DDX_Text(pDX, IDC_RED1, m_red);
	DDX_Text(pDX, IDC_START, m_start);
	DDX_Text(pDX, IDC_END, m_end);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(FogDlg, CDialog)
	//{{AFX_MSG_MAP(FogDlg)
	ON_EN_CHANGE(IDC_BLUE1, OnChangeBlue1)
	ON_EN_CHANGE(IDC_GREEN1, OnChangeBlue1)
	ON_EN_CHANGE(IDC_RED1, OnChangeBlue1)
	ON_WM_CTLCOLOR()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// FogDlg message handlers

void FogDlg::OnChangeBlue1() 
{
	RedrawWindow();
}

HBRUSH FogDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	if ( CTLCOLOR_STATIC == nCtlColor && GetDlgItem( IDC_SOURCE_COLOR )->m_hWnd == pWnd->m_hWnd )
	{
		UpdateData( );
		if ( brush.m_hObject )
			brush.DeleteObject();

		brush.CreateSolidBrush(RGB(m_red, m_green, m_blue));
		return (HBRUSH)brush.m_hObject;
	}
	
	HBRUSH hbr = CDialog ::OnCtlColor(pDC, pWnd, nCtlColor);
	
	// TODO: Change any attributes of the DC here
	
	// TODO: Return a different brush if the default is not desired
	return hbr;
}
