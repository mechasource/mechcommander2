//===========================================================================//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

// EditForestDlg.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "EditForestDlg.h"
#include "Forest.h"
#include "EditorObjectMgr.h"
#include "ForestDlg.h"
#include "malloc.h"


/////////////////////////////////////////////////////////////////////////////
// EditForestDlg dialog


EditForestDlg::EditForestDlg(CWnd* pParent /*=NULL*/)
	: CDialog(EditForestDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(EditForestDlg)
	m_Name = _T("");
	//}}AFX_DATA_INIT
}


void EditForestDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(EditForestDlg)
	DDX_Control(pDX, IDC_LIST_FILES, m_fileList);
	DDX_Text(pDX, IDC_NAME, m_Name);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(EditForestDlg, CDialog)
	//{{AFX_MSG_MAP(EditForestDlg)
	ON_BN_CLICKED(IDC_EDIT, OnEdit)
	ON_BN_CLICKED(IDC_DELETE, OnDelete)
	ON_BN_CLICKED(IDC_RENAME, OnRename)
	ON_LBN_SELCHANGE(IDC_LIST_FILES, OnSelchangeListFiles)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

static char szFITFilter[] = "FIT Files (*.FIT)|*.fit||";

/////////////////////////////////////////////////////////////////////////////
// EditForestDlg message handlers

void EditForestDlg::OnOK() 
{
	int index = m_fileList.GetCurSel();
	if ( index != -1 )
	{
		int ID = m_fileList.GetItemData( index );
		EditorObjectMgr::instance()->selectForest( ID );
	}

	CDialog::OnOK();
}

void EditForestDlg::OnEdit() 
{
	
	int index = m_fileList.GetCurSel();
	if ( index != -1 )
	{
		long ID = m_fileList.GetItemData( index );
		const Forest* pOldForest = EditorObjectMgr::instance()->getForest( ID );
		if ( pOldForest )
		{
			Forest tmpForest = *pOldForest;

			ForestDlg dlg;
			dlg.forest = tmpForest;

			if ( IDOK == dlg.DoModal() )
			{
				tmpForest = dlg.forest;
				EditorObjectMgr::instance()->editForest( ID, tmpForest );
				m_fileList.DeleteString( index );
				index = m_fileList.InsertString( index, tmpForest.getName() );
				m_fileList.SetItemData( index, ID );
				m_fileList.SetCurSel( index );
			}
		}
	}
}

void EditForestDlg::OnDelete() 
{
	int index = m_fileList.GetCurSel();
	if ( index != -1 )
	{
		int ID = m_fileList.GetItemData( index );
		EditorObjectMgr::instance()->removeForest( ID );
		m_fileList.DeleteString( index );
	}
	
}

void EditForestDlg::OnLoad() 
{
	CFileDialog dlg( TRUE, terrainPath, NULL, OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR, szFITFilter );
	if ( IDOK == dlg.DoModal() )
	{
		CString str = dlg.GetFileName();
		Forest tmp( -1 );	
		tmp.setFileName( dlg.m_ofn.lpstrFile );
		tmp.init();

		EditorObjectMgr::instance()->unselectAll();

		int ID = EditorObjectMgr::instance()->createForest( tmp );

		int index = m_fileList.AddString( str );
		m_fileList.SetItemData( index, ID );
		
	}

	
	
}

BOOL EditForestDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	for (int i=0;i < m_fileList.GetCount();i++)
	{
	   m_fileList.DeleteString( i );
	}


	Forest** pForests = NULL;
	long count = 0;
	EditorObjectMgr::instance()->getForests( pForests,  count );

	if ( count )
	{

		pForests = (Forest**)_alloca( count * sizeof ( Forest* ) );
		
		EditorObjectMgr::instance()->getForests( pForests,  count );

		char name[256];
		cLoadString( IDS_UNNAMDE_FOREST, name, 255 );
		char final[256];


		for ( int i = 0; i < count; i++ )
		{
			const char* pName = pForests[i]->getName();
			int index = -1;
			
			// make up name if necessary
			if ( !pName || !strlen( pName ))
			{
				sprintf( final, name, pForests[i]->getID() );
				pName = "?";
			}
			 index = m_fileList.AddString( pName );

			 

			m_fileList.SetItemData( index, pForests[i]->getID() );
		}
	}
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void EditForestDlg::OnSave() 
{
	int index = m_fileList.GetCurSel();
	if ( index != -1 )
	{
		long ID = m_fileList.GetItemData( index );
		Forest* pOldForest = (Forest*)EditorObjectMgr::instance()->getForest( ID );
		if ( pOldForest )
		{

			CFileDialog dlg( 0, "fit", NULL, OFN_NOCHANGEDIR | OFN_OVERWRITEPROMPT, szFITFilter, this );
			dlg.m_ofn.lpstrInitialDir = terrainPath;
			int retVal = dlg.DoModal();
			if ( IDOK == retVal )
			{
				pOldForest->setFileName( dlg.m_ofn.lpstrFile );
				pOldForest->save();
				m_fileList.DeleteString( index );
				m_fileList.InsertString( index, dlg.GetFileName() );
				m_fileList.SetItemData( index, ID );
				m_fileList.SetCurSel( index );
			}

		}
	}
	
}

void EditForestDlg::OnRename() 
{
	UpdateData();
	int index = m_fileList.GetCurSel();
	if ( index != -1 )
	{
		long ID = m_fileList.GetItemData( index );
		Forest* pOldForest = (Forest*)EditorObjectMgr::instance()->getForest( ID );
		if ( pOldForest )
			pOldForest->setName( m_Name );

		m_fileList.DeleteString( index );
		index = m_fileList.InsertString( index, m_Name );
		m_fileList.SetItemData( index, ID );
		m_fileList.SetCurSel( index );
	}

}

void EditForestDlg::OnSelchangeListFiles() 
{
	int index = m_fileList.GetCurSel();
	if ( index != -1 )
	{
		long ID = m_fileList.GetItemData( index );
		Forest* pOldForest = (Forest*)EditorObjectMgr::instance()->getForest( ID );
		m_Name = pOldForest->getName();
		UpdateData( false );
	}

	
}
