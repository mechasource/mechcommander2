#ifndef WAVEDLG_H
#define WAVEDLG_H
/*************************************************************************************************\
WaveDlg.h			: Interface for the WaveDlg component.  Here you put in wave/water settings
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//
\*************************************************************************************************/

#include "stdafx.h"
//*************************************************************************************************

/**************************************************************************************************
CLASS DESCRIPTION
WaveDlg:
**************************************************************************************************/
class WaveDlg: public CDialog
{
	public:

		WaveDlg() : CDialog(IDD_WAVES){}
		virtual BOOL OnInitDialog(){ Init(); return FALSE; }
		virtual void Init();
		void	UpdateData();
		void	OnOK();


		float amplitude;
		float frequency;
		float elevation;
		float shallowElevation;
		float alphaElevation;
		long alphaShallow;
		long alphaMiddle;
		long alphaDeep;

};


//*************************************************************************************************
#endif  // end of file ( WaveDlg.h )
