#ifndef WAVEDLG_H
#define WAVEDLG_H
/*************************************************************************************************\
WaveDlg.h			: Interface for the WaveDlg component.  Here you put in
wave/water settings
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//
\*************************************************************************************************/

//#include "stdafx.h"

/**************************************************************************************************
CLASS DESCRIPTION
WaveDlg:
**************************************************************************************************/
class WaveDlg : public CDialog
{
public:
	WaveDlg()
		: CDialog(IDD_WAVES)
	{
	}
	virtual BOOL OnInitDialog()
	{
		Init(void);
		return FALSE;
	}
	virtual void Init(void);
	void UpdateData(void);
	void OnOK(void);

	float amplitude;
	float frequency;
	float elevation;
	float shallowElevation;
	float alphaElevation;
	int32_t alphaShallow;
	int32_t alphaMiddle;
	int32_t alphaDeep;
};

#endif // end of file ( WaveDlg.h )
