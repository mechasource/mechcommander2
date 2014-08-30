//===========================================================================//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

// EditForestDlg.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"

//#include "forest.h"
//#include "editorobjectmgr.h"
//#include "forestdlg.h"

#include "editforestdlg.h"

/////////////////////////////////////////////////////////////////////////////
// EditForestDlg dialog


EditForestDlg::EditForestDlg(CWnd* pParent /*=nullptr*/)
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
	int32_t index = m_fileList.GetCurSel();
	if ( index != -1 )
	{
		int32_t ID = m_fileList.GetItemData( index );
		EditorObjectMgr::instance()->selectForest( ID );
	}

	CDialog::OnOK();
}

void EditForestDlg::OnEdit() 
{
	
	int32_t index = m_fileList.GetCurSel();
	if ( index != -1 )
	{
		int32_t ID = m_fileList.GetItemData( index );
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
	int32_t index = m_fileList.GetCurSel();
	if ( index != -1 )
	{
		int32_t ID = m_fileList.GetItemData( index );
		EditorObjectMgr::instance()->removeForest( ID );
		m_fileList.DeleteString( index );
	}
	
}

void EditForestDlg::OnLoad() 
{
	CFileDialog dlg( TRUE, terrainPath, nullptr, OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR, szFITFilter );
	if ( IDOK == dlg.DoModal() )
	{
		CString str = dlg.GetFileName();
		Forest tmp( -1 );	
		tmp.setFileName( dlg.m_ofn.lpstrFile );
		tmp.init();

		EditorObjectMgr::instance()->unselectAll();

		int32_t ID = EditorObjectMgr::instance()->createForest( tmp );

		int32_t index = m_fileList.AddString( str );
		m_fileList.SetItemData( index, ID );
		
	}

	
	
}

BOOL EditForestDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	for (int32_t i=0;i < m_fileList.GetCount();i++)
	{
	   m_fileList.DeleteString( i );
	}


	Forest** pForests = nullptr;
	int32_t count = 0;
	EditorObjectMgr::instance()->getForests( pForests,  count );

	if ( count )
	{

		pForests = (Forest**)_alloca( count * sizeof ( Forest* ) );
		
		EditorObjectMgr::instance()->getForests( pForests,  count );

		char name[256];
		cLoadString( IDS_UNNAMDE_FOREST, name, 255 );
		char final[256];


		for ( int32_t i = 0; i < count; i++ )
		{
			PCSTR pName = pForests[i]->getName();
			int32_t index = -1;
			
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
	int32_t index = m_fileList.GetCurSel();
	if ( index != -1 )
	{
		int32_t ID = m_fileList.GetItemData( index );
		Forest* pOldForest = (Forest*)EditorObjectMgr::instance()->getForest( ID );
		if ( pOldForest )
		{

			CFileDialog dlg( 0, "fit", nullptr, OFN_NOCHANGEDIR | OFN_OVERWRITEPROMPT, szFITFilter, this );
			dlg.m_ofn.lpstrInitialDir = terrainPath;
			int32_t retVal = dlg.DoModal();
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
	int32_t index = m_fileList.GetCurSel();
	if ( index != -1 )
	{
		int32_t ID = m_fileList.GetItemData( index );
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
	int32_t index = m_fileList.GetCurSel();
	if ( index != -1 )
	{
		int32_t ID = m_fileList.GetItemData( index );
		Forest* pOldForest = (Forest*)EditorObjectMgr::instance()->getForest( ID );
		m_Name = pOldForest->getName();
		UpdateData( false );
	}

	
}
