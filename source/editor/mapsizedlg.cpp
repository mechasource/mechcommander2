#define MAPSIZEDLG_CPP
/*************************************************************************************************\
mapsizeDlg.cpp		: Implementation of the mapsizeDlg component.
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//
\*************************************************************************************************/

#include "mclib.h"
#include "mapsizedlg.h"

#include "resource.h"
#include "mclibresource.h"
#include "utilities.h"
#include "terrtxm2.h"

extern DWORD gameResourceHandle;		//Default handle must be used for mc2res.dll due to shared game/editor code

//----------------------------------------------------------------------
void MapSizeDlg::Init()
{
	CListBox* pListBox = (CListBox*)GetDlgItem( IDC_MAPSIZE );

	int index = pListBox->AddString( "60x60" );
	pListBox->SetItemData( index, 0 );

	index = pListBox->AddString( "80x80" );
	pListBox->SetItemData( index, 1 );

	index = pListBox->AddString( "100x100" );
	pListBox->SetItemData( index, 2 );

	index = pListBox->AddString( "120x120" );
	pListBox->SetItemData( index, 3 );

	pListBox->SetCurSel( mapSize );
}

//----------------------------------------------------------------------
void MapSizeDlg::OnOK()
{
	int index = ((CListBox*)GetDlgItem( IDC_MAPSIZE ))->GetCurSel( );
	mapSize = ((CListBox*)GetDlgItem( IDC_MAPSIZE ))->GetItemData( index );

	CDialog::OnOK();
}

//----------------------------------------------------------------------
