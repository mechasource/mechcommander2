#define WAVEDLG_CPP
/*************************************************************************************************\
WaveDlg.cpp			: Implementation of the WaveDlg component.
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//
\*************************************************************************************************/
#include "resource.h"
#include "WaveDlg.h"


void WaveDlg::Init()
{
	char buffer[256];

	itoa( (alphaDeep>>24)&0x000000ff, buffer, 10 );

	((CEdit*)GetDlgItem( IDC_ALPHA_DEEP ))->SetWindowText( buffer );

	_gcvt( (alphaElevation), 6, buffer );

	((CEdit*)GetDlgItem( IDC_ALPHA_ELEVATION ))->SetWindowText( buffer );

	itoa( (alphaMiddle>>24)&0x000000ff, buffer, 10 );

	((CEdit*)GetDlgItem( IDC_ALPHA_MIDDLE ))->SetWindowText( buffer );

	itoa( ((alphaShallow>>24)&0x000000ff), buffer, 10 );

	((CEdit*)GetDlgItem( IDC_ALPHA_SHALLOW ))->SetWindowText( buffer );

	_gcvt( amplitude, 6, buffer );
	
	((CEdit*)GetDlgItem( IDC_AMPLITUDE ))->SetWindowText( buffer );

	_gcvt( elevation, 6, buffer );
	
	((CEdit*)GetDlgItem( IDC_ELEVATION ))->SetWindowText( buffer );

	_gcvt( frequency, 6, buffer );
	
	((CEdit*)GetDlgItem( IDC_FREQUENCY ))->SetWindowText( buffer );

	_gcvt( shallowElevation, 6, buffer );
	
	((CEdit*)GetDlgItem( IDC_SHALLOW_ELEVATION ))->SetWindowText( buffer );










}


void	WaveDlg::OnOK()
{
	UpdateData();
	CDialog::OnOK();
}


void WaveDlg::UpdateData()
{
	char buffer[256];
	
	((CEdit*)GetDlgItem( IDC_ALPHA_DEEP ))->GetWindowText( buffer, 256 );
	alphaDeep = atoi( buffer );
	alphaDeep <<= 24;

	((CEdit*)GetDlgItem( IDC_ALPHA_ELEVATION ))->GetWindowText( buffer, 256 );
	alphaElevation = (float)atof( buffer );

	((CEdit*)GetDlgItem( IDC_ALPHA_MIDDLE ))->GetWindowText( buffer, 256 );
	alphaMiddle = atoi( buffer);
	alphaMiddle <<= 24;

	((CEdit*)GetDlgItem( IDC_ALPHA_SHALLOW ))->GetWindowText( buffer, 256 );
	alphaShallow = atoi( buffer );
	alphaShallow <<= 24;

	((CEdit*)GetDlgItem( IDC_AMPLITUDE ))->GetWindowText( buffer, 256 );
	amplitude = (float)atof( buffer );

	((CEdit*)GetDlgItem( IDC_ELEVATION ))->GetWindowText( buffer, 256 );
	elevation = (float)atof( buffer );

	((CEdit*)GetDlgItem( IDC_FREQUENCY ))->GetWindowText( buffer, 256 );
	frequency = (float)atof( buffer );

	((CEdit*)GetDlgItem( IDC_SHALLOW_ELEVATION ))->GetWindowText( buffer, 256 );
	shallowElevation = (float)atof( buffer );

}

//*************************************************************************************************
// end of file ( WaveDlg.cpp )

//*************************************************************************************************
// end of file ( WaveDlg.cpp )
