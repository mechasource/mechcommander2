#define HEIGHTDLG_CPP
/*************************************************************************************************\
HeightDlg.cpp			: Implementation of the HeightDlg component.
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//
\*************************************************************************************************/

#include "HeightDlg.h"
#include "resource.h"

HeightDlg::HeightDlg( int minZ, int maxZ)
: CDialog( IDD_HEIGHT_DLG )
{
	min = minZ;
	max = maxZ;
}
void HeightDlg::Init()
{
	char minTxt[256];
	char maxTxt[256];
	itoa( min, minTxt, 10 );
	itoa( max, maxTxt, 10 );
	(GetDlgItem( IDC_MIN ))->SetWindowText( minTxt );
	(GetDlgItem( IDC_MAX ))->SetWindowText( maxTxt );
}

void HeightDlg::OnOK()
{
	char minTxt[256];
	char maxTxt[256];
	(GetDlgItem( IDC_MIN ))->GetWindowText( minTxt, 256 );
	(GetDlgItem( IDC_MAX ))->GetWindowText( maxTxt, 256 );
	min = atoi( minTxt );
	max = atoi( maxTxt );

	CDialog::OnOK();
}

//void HeightDlg::OnCommand(Window *wnd,int nCommand)
//{
/*	if (nCommand == BC_CLICKED)
	{
		wchar_t wcsMinTxt[256];
		wchar_t wcsMaxTxt[256];
		((Entry*)GetDlgItem( IDC_MIN ))->GetEntry( wcsMinTxt, 256 );
		((Entry*)GetDlgItem( IDC_MAX ))->GetEntry( wcsMaxTxt, 256 );
		char minTxt[256];
		char maxTxt[256];
		WcsToStr( minTxt, wcsMinTxt );
		WcsToStr( maxTxt, wcsMaxTxt );
		min = atoi( minTxt );
		max = atoi( maxTxt );

		EndDialog(wnd->GetID());
	}*/

//}


//*************************************************************************************************
// end of file ( HeightDlg.cpp )
