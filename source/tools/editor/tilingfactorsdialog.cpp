//===========================================================================//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

// TilingFactorsDialog.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "TilingFactorsDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// TilingFactorsDialog dialog


TilingFactorsDialog::TilingFactorsDialog(CWnd* pParent /*=NULL*/)
	: CDialog(TilingFactorsDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(TilingFactorsDialog)
	m_TerrainDetailTilingFactor = 0.0f;
	m_WaterDetailTilingFactor = 0.0f;
	m_WaterTilingFactor = 0.0f;
	//}}AFX_DATA_INIT
}


void TilingFactorsDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(TilingFactorsDialog)
	DDX_Text(pDX, IDC_TILING_FACTORS_TERRAIN_DETAIL_EDIT, m_TerrainDetailTilingFactor);
	DDX_Text(pDX, IDC_TILING_FACTORS_WATER_DETAIL_EDIT, m_WaterDetailTilingFactor);
	DDX_Text(pDX, IDC_TILING_FACTORS_WATER_EDIT, m_WaterTilingFactor);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(TilingFactorsDialog, CDialog)
	//{{AFX_MSG_MAP(TilingFactorsDialog)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// TilingFactorsDialog message handlers
