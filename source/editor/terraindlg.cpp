#define TERRAINDLG_CPP
/*************************************************************************************************\
TerrainDlg.cpp		: Implementation of the TerrainDlg component.
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//
\*************************************************************************************************/

#include "mclib.h"
#include "TerrainDlg.h"

#include "resource.h"
#include "mclibresource.h"
#include "utilities.h"
#include "terrtxm2.h"

extern DWORD gameResourceHandle;		//Default handle must be used for mc2res.dll due to shared game/editor code

//----------------------------------------------------------------------
void TerrainDlg::Init()
{
	CListBox* pListBox = (CListBox*)GetDlgItem( IDC_TERRAINS );

	int numTerrains = TerrainColorMap::getNumTypes();
	for ( int i = 0; i < numTerrains; i++ )
	{
		
		char buffer[256];
		
		if ( !TerrainColorMap::getTextureNameID(i) ) // if we start to repeat, quit
			break;	
			
		cLoadString( TerrainColorMap::getTextureNameID(i), buffer, 256 );
		int index = pListBox->AddString( buffer );
		pListBox->SetItemData( index, i );
	}

	pListBox->SetCurSel( terrain );
}

//----------------------------------------------------------------------
void TerrainDlg::OnOK()
{
	int index = ((CListBox*)GetDlgItem( IDC_TERRAINS ))->GetCurSel( );
	terrain = ((CListBox*)GetDlgItem( IDC_TERRAINS ))->GetItemData( index );

	CDialog::OnOK();
}

//----------------------------------------------------------------------
