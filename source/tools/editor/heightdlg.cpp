#define HEIGHTDLG_CPP
/*************************************************************************************************\
heightDlg.cpp			: Implementation of the heightDlg component.
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//
\*************************************************************************************************/

#include "heightDlg.h"
#include "resource.h"

heightDlg::heightDlg(int32_t minZ, int32_t maxZ)
	: CDialog(IDD_HEIGHT_DLG)
{
	min = minZ;
	max = maxZ;
}
void heightDlg::Init()
{
	wchar_t minTxt[256];
	wchar_t maxTxt[256];
	itoa(min, minTxt, 10);
	itoa(max, maxTxt, 10);
	(GetDlgItem(IDC_MIN))->SetWindowText(minTxt);
	(GetDlgItem(IDC_MAX))->SetWindowText(maxTxt);
}

void heightDlg::OnOK()
{
	wchar_t minTxt[256];
	wchar_t maxTxt[256];
	(GetDlgItem(IDC_MIN))->GetWindowText(minTxt, 256);
	(GetDlgItem(IDC_MAX))->GetWindowText(maxTxt, 256);
	min = atoi(minTxt);
	max = atoi(maxTxt);
	CDialog::OnOK();
}

// void heightDlg::OnCommand(Window *wnd,int32_t nCommand)
//{
/*	if (nCommand == BC_CLICKED)
	{
		wchar_t wcsMinTxt[256];
		wchar_t wcsMaxTxt[256];
		((Entry*)GetDlgItem( IDC_MIN ))->GetEntry( wcsMinTxt, 256 );
		((Entry*)GetDlgItem( IDC_MAX ))->GetEntry( wcsMaxTxt, 256 );
		wchar_t minTxt[256];
		wchar_t maxTxt[256];
		WcsToStr( minTxt, wcsMinTxt );
		WcsToStr( maxTxt, wcsMaxTxt );
		min = atoi( minTxt );
		max = atoi( maxTxt );

		EndDialog(wnd->GetID());
	}*/

//}

// end of file ( heightDlg.cpp )
