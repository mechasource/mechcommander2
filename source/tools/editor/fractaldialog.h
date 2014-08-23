/*************************************************************************************************\
FogDlg.h			: Interface for the FractalDlg component.  Lets user change refractalize 
						terrain settings
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//
\*************************************************************************************************/

#pragma once

#ifndef FRACTALDLG_H
#define FRACTALDLG_H

//#include "stdafx.h"
//#include "resource.h"

/////////////////////////////////////////////////////////////////////////////
// FractalDlg dialog

class FractalDlg : public CDialog
{
	public:
// Construction
	FractalDlg(CWnd* pParent = NULL);   // standard constructor

	uint8_t GetNoise(){ return m_noise; }
	uint8_t GetThreshold(){ return m_threshold; }

	void SetThreshold( uint8_t newThreshold ){ m_threshold = newThreshold; }
	void SetNoise( uint8_t newNoise ){ m_noise = newNoise; }
		
	enum { IDD = IDD_FRACTAL };
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	
	private:
	
	uint8_t m_threshold;
	uint8_t m_noise;
};

#endif  // end of file ( FractalDlg.h )

