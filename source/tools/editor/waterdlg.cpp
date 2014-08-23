//===========================================================================//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

// WaterDlg.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "WaterDlg.h"

/*
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
*/

/////////////////////////////////////////////////////////////////////////////
// WaterDlg dialog


WaterDlg::WaterDlg(CWnd* pParent /*=NULL*/)
	: CDialog(WaterDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(WaterDlg)
	amplitude = 0.0f;
	frequency = 0.0f;
	elevation = 0.0f;
	shallowElevation = 0.0f;
	alphaElevation = 0.0f;
	alphaDeep = 0;
	alphaMiddle = 0;
	alphaShallow = 0;
	//}}AFX_DATA_INIT
}


void WaterDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(WaterDlg)
	DDX_Text(pDX, IDC_AMPLITUDE, amplitude);
	DDX_Text(pDX, IDC_FREQUENCY, frequency);
	DDX_Text(pDX, IDC_ELEVATION, elevation);
	DDX_Text(pDX, IDC_SHALLOW_ELEVATION, shallowElevation);
	DDV_MinMaxFloat(pDX, shallowElevation, 0.f, 20.f);
	DDX_Text(pDX, IDC_ALPHA_ELEVATION, alphaElevation);
	DDX_Text(pDX, IDC_ALPHA_DEEP, alphaDeep);
	DDX_Text(pDX, IDC_ALPHA_MIDDLE, alphaMiddle);
	DDX_Text(pDX, IDC_ALPHA_SHALLOW, alphaShallow);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(WaterDlg, CDialog)
	//{{AFX_MSG_MAP(WaterDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// WaterDlg message handlers

BOOL WaterDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	alphaDeep = (alphaDeep>>24)&0x000000ff;
	alphaMiddle = (alphaMiddle>>24)&0x000000ff;
	alphaShallow = (alphaShallow>>24)&0x000000ff;
	UpdateData(FALSE);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void WaterDlg::OnOK() 
{
	UpdateData(TRUE);
	alphaDeep <<= 24;
	alphaMiddle <<= 24;
	alphaShallow <<= 24;
	UpdateData(FALSE);

	CDialog::OnOK();
}
