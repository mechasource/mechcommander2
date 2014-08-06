#ifndef FRACTALDLG_H
#define FRACTALDLG_H

/*************************************************************************************************\
FogDlg.h			: Interface for the FractalDlg component.  Lets user change refractalize 
						terrain settings
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//
\*************************************************************************************************/

#include "stdafx.h"
#include "resource.h"

/////////////////////////////////////////////////////////////////////////////
// FractalDlg dialog

class FractalDlg : public CDialog
{
	public:
// Construction
	FractalDlg(CWnd* pParent = NULL);   // standard constructor

	unsigned char GetNoise(){ return m_noise; }
	unsigned char GetThreshold(){ return m_threshold; }

	void SetThreshold( unsigned char newThreshold ){ m_threshold = newThreshold; }
	void SetNoise( unsigned char newNoise ){ m_noise = newNoise; }
		
	enum { IDD = IDD_FRACTAL };
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	
	private:
	
	unsigned char m_threshold;
	unsigned char m_noise;
};

#endif  // end of file ( FractalDlg.h )

