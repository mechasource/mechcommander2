//===========================================================================//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

// ForestDlg.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "ForestDlg.h"
#include "Terrain.h"
#include "mclib.h"
#include "EditorObjectMgr.h"
#include "mclib.h"


/////////////////////////////////////////////////////////////////////////////
// ForestDlg dialog

static char szFITFilter[] = "FIT Files (*.FIT)|*.fit||";


ForestDlg::ForestDlg(CWnd* pParent /*=NULL*/)
: CDialog(ForestDlg::IDD, pParent), forest( -1 )
{
	//{{AFX_DATA_INIT(ForestDlg)
	m_maxDensity = 10.0f;
	m_minDensity = 2.0f;
	m_maxHeight = 1.0f;
	m_minHeight = 1.0f;
	m_randomPlacement = TRUE;
	m_xLoc = 0.0f;
	m_yLoc = 0.0f;
	m_radius = 0.0f;
	m_Name = _T("");
	m_1 = 0;
	m_2 = 0;
	m_3 = 0;
	m_4 = 0;
	m_5 = 0;
	m_6 = 0;
	m_7 = 0;
	m_8 = 0;
	m_9 = 0;
	m_10 = 0;
	m_12 = 0;
	m_13 = 0;
	m_14 = 0;
	m_15 = 0;
	//}}AFX_DATA_INIT
}


void ForestDlg::DoDataExchange(CDataExchange* pDX)
{
	float min = -land->mapTopLeft3d.y;
	float max = land->mapTopLeft3d.y;

	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(ForestDlg)
	DDX_Text(pDX, IDC_DENSITY_MAX, m_maxDensity);
	DDV_MinMaxFloat(pDX, m_maxDensity, 0.f, 10.f);
	DDX_Text(pDX, IDC_DENSITY_MIN, m_minDensity);
	DDV_MinMaxFloat(pDX, m_minDensity, 0.f, 10.f);
	DDX_Text(pDX, IDC_HEIGHT_MAX, m_maxHeight);
	DDV_MinMaxFloat(pDX, m_maxHeight, 0.5f, 1.5f);
	DDX_Text(pDX, IDC_HEIGHT_MIN, m_minHeight);
	DDV_MinMaxFloat(pDX, m_minHeight, 0.5f, 1.5f);
	DDX_Check(pDX, IDC_RANDOM, m_randomPlacement);
	DDX_Text(pDX, IDC_XLOC, m_xLoc);
	DDX_Text(pDX, IDC_YLOC, m_yLoc);
	DDX_Text(pDX, IDC_RADIUS, m_radius);
	DDX_Text(pDX, IDC_NAME_EDIT, m_Name);
	DDX_Text(pDX, IDC_FOREST_EDIT1, m_1);
	DDV_MinMaxInt(pDX, m_1, 0, 100);
	DDX_Text(pDX, IDC_FOREST_EDIT2, m_2);
	DDV_MinMaxInt(pDX, m_2, 0, 100);
	DDX_Text(pDX, IDC_FOREST_EDIT3, m_3);
	DDV_MinMaxInt(pDX, m_3, 0, 100);
	DDX_Text(pDX, IDC_FOREST_EDIT4, m_4);
	DDV_MinMaxInt(pDX, m_4, 0, 100);
	DDX_Text(pDX, IDC_FOREST_EDIT5, m_5);
	DDV_MinMaxInt(pDX, m_5, 0, 100);
	DDX_Text(pDX, IDC_FOREST_EDIT6, m_6);
	DDV_MinMaxInt(pDX, m_6, 0, 100);
	DDX_Text(pDX, IDC_FOREST_EDIT7, m_7);
	DDV_MinMaxInt(pDX, m_7, 0, 100);
	DDX_Text(pDX, IDC_FOREST_EDIT8, m_8);
	DDV_MinMaxInt(pDX, m_8, 0, 100);
	DDX_Text(pDX, IDC_FOREST_EDIT9, m_9);
	DDV_MinMaxInt(pDX, m_9, 0, 100);
	DDX_Text(pDX, IDC_FOREST_EDIT10, m_10);
	DDV_MinMaxInt(pDX, m_10, 0, 100);
	DDX_Text(pDX, IDC_FOREST_EDIT12, m_12);
	DDV_MinMaxInt(pDX, m_12, 0, 100);
	DDX_Text(pDX, IDC_FOREST_EDIT13, m_13);
	DDV_MinMaxInt(pDX, m_13, 0, 100);
	DDX_Text(pDX, IDC_FOREST_EDIT14, m_14);
	DDV_MinMaxInt(pDX, m_14, 0, 100);
	DDX_Text(pDX, IDC_FOREST_EDIT15, m_15);
	DDV_MinMaxInt(pDX, m_15, 0, 100);
	//}}AFX_DATA_MAP
//	DDV_MinMaxFloat(pDX, m_xLoc, land->mapTopLeft3d.x, -land->mapTopLeft3d.x);
	DDV_MinMaxFloat(pDX, m_yLoc, min, max);

}


BEGIN_MESSAGE_MAP(ForestDlg, CDialog)
	//{{AFX_MSG_MAP(ForestDlg)
	ON_BN_CLICKED(IDSAVE, OnSave)
	ON_BN_CLICKED(IDLOAD, OnLoad)
	//}}AFX_MSG_MAP
	ON_CONTROL_RANGE( EN_CHANGE, IDC_FOREST_EDIT1, IDC_FOREST_EDIT15, OnEditChanged )
	ON_NOTIFY_RANGE( NM_RELEASEDCAPTURE, IDC_SLIDER1, IDC_SLIDER15, OnSliderChanged )
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// ForestDlg message handlers

void ForestDlg::OnSave() 
{
	BOOL bRes = UpdateData(TRUE);
	if (!bRes)
	{
		return;
	}
	CFileDialog dlg( 0, "fit", NULL, OFN_OVERWRITEPROMPT | OFN_NOCHANGEDIR, szFITFilter, this );
	dlg.m_ofn.lpstrInitialDir = terrainPath;
	int retVal = dlg.DoModal();
	if ( IDOK == retVal )
	{
			for ( int i = IDC_FOREST_EDIT1; i < IDC_FOREST_EDIT15+1; i+= 2 )
			{
				CWnd* pWnd = GetDlgItem( i );
				CEdit* pEdit = (CEdit*)( pWnd );
				if ( pEdit )
				{
					CString str;
					pEdit->GetWindowText( str );

					forest.percentages[(i-IDC_FOREST_EDIT1)/2] = atof( str );
				}
			}

			forest.minDensity = m_minDensity;
			forest.maxDensity = m_maxDensity;
			forest.minHeight = m_minHeight;
			forest.maxHeight = m_maxHeight;
			forest.bRandom = m_randomPlacement;
			forest.centerX = m_xLoc;
			forest.centerY = m_yLoc;
			forest.radius = m_radius;
	
		forest.setFileName( dlg.m_ofn.lpstrFile );
		forest.save();

		OnInitDialog();
	}
}

void ForestDlg::OnOK() 
{
	BOOL bRes = UpdateData(TRUE);
	if (!bRes)
	{
		return;
	}

	for ( int i = IDC_FOREST_EDIT1; i < IDC_FOREST_EDIT15+1; i+= 2 )
	{
		CWnd* pWnd = GetDlgItem( i );
		CEdit* pEdit = (CEdit*)( pWnd );
		if ( pEdit )
		{
			CString str;
			pEdit->GetWindowText( str );

			forest.percentages[(i-IDC_FOREST_EDIT1)/2] = atof( str );
		}
	}

	forest.minDensity = m_minDensity;
	forest.maxDensity = m_maxDensity;
	forest.minHeight = m_minHeight;
	forest.maxHeight = m_maxHeight;
	forest.bRandom = m_randomPlacement;
	forest.centerX = m_xLoc;
	forest.centerY = m_yLoc;
	forest.radius = m_radius;
	forest.name = m_Name;
	
	CDialog::OnOK();
}

void ForestDlg::OnCancel() 
{
	// TODO: Add extra cleanup here
	
	CDialog::OnCancel();
}

void ForestDlg::OnEditChanged( UINT nID )
{
	CWnd* pWnd = GetDlgItem( nID );
	CEdit* pEdit = (CEdit*)( pWnd );
	if ( pEdit )
	{
		CString text;
		pEdit->GetWindowText( text );
		double val = atof( text );

		CSliderCtrl* pSlider = (CSliderCtrl*)( GetDlgItem( nID - 1 ) );
		if ( pSlider )
		{
			pSlider->SetRange( 0, 100 );
			pSlider->SetPos( val );
		}
	}
}

void ForestDlg::OnSliderChanged( UINT id, NMHDR * pNotifyStruct, LRESULT * result )
{
	CWnd* pWnd = GetDlgItem( id );
	CSliderCtrl* pSlider = (CSliderCtrl*)( pWnd );
	if ( pSlider )
	{
		int Val = pSlider->GetPos();

		
		CEdit* pEdit = (CEdit*)( GetDlgItem( id + 1 ) );
		if ( pEdit )
		{
			CString text;
			text.Format( "%ld", Val );
		
			pEdit->SetWindowText( text );
		}
	}

}


BOOL ForestDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	m_minDensity = forest.minDensity;
	m_maxDensity = forest.maxDensity;
	m_minHeight = forest.minHeight;
	m_maxHeight = forest.maxHeight;
	m_randomPlacement = forest.bRandom;
	m_Name = forest.name;

	if ( !m_xLoc )
	{
		m_xLoc = forest.centerX;
		m_yLoc = forest.centerY;
		m_radius = forest.radius;
	}

	UpdateData( 0 );
	
	
	for ( int i = IDC_SLIDER1; i < IDC_SLIDER15; i+= 2 )
	{
		CWnd* pWnd = GetDlgItem( i );
		CSliderCtrl* pSlider = (CSliderCtrl*)( pWnd );
		if ( pSlider )
		{
			pSlider->SetRange( 0, 100 );
		}

	}

	for ( i = IDC_FOREST_EDIT1; i < IDC_FOREST_EDIT15+1; i+= 2 )
	{
		CWnd* pWnd = GetDlgItem( i );
		CEdit* pEdit = (CEdit*)( pWnd );
		if ( pEdit )
		{
			CString str;
			str.Format( "%ld", (long)forest.percentages[(i-IDC_FOREST_EDIT1)/2] );
			pEdit->SetWindowText( str );
		}

		CSliderCtrl* pSlider = (CSliderCtrl*)( GetDlgItem( i - 1 ) );
		if ( pSlider )
		{
			pSlider->SetRange( 0, 100 );
			pSlider->SetPos( forest.percentages[(i-IDC_FOREST_EDIT1)/2] );
		}


	}

	char tmp[256];
	_splitpath( forest.getFileName(), NULL, NULL, tmp, 0 );

	if (0 != strcmp("", tmp))
	{
		SetWindowText( tmp );
	}
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void ForestDlg::OnLoad() 
{
	CFileDialog dlg( TRUE, "fit", NULL, OFN_NOCHANGEDIR | OFN_FILEMUSTEXIST, szFITFilter, this );
	dlg.m_ofn.lpstrInitialDir = terrainPath;


	if ( IDOK == dlg.DoModal() )
	{
		CString str = dlg.GetFileName();
//		Forest tmp( -1 );	
//		tmp.setFileName( dlg.m_ofn.lpstrFile );
//		tmp.init();

		EditorObjectMgr::instance()->unselectAll();

//		int ID = EditorObjectMgr::instance()->createForest( tmp );	
		forest.setFileName( dlg.m_ofn.lpstrFile );
		forest.init();
		//forest = tmp;
		OnInitDialog();
	}

	
}
