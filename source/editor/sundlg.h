#ifndef SUNDLG_H
#define SUNDLG_H
/*************************************************************************************************\
SunDlg.h			: Interface for the SunDlg component.
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//
\*************************************************************************************************/


#include "stdafx.h"
#include "resource.h"

//*************************************************************************************************

/**************************************************************************************************
CLASS DESCRIPTION
SunDlg:
**************************************************************************************************/
class SunDlg: public CDialog
{
public:
	
	SunDlg();
	virtual void Init();
	void DoDataExchange(CDataExchange* pDX);

	//virtual	void OnCommand(Window *wnd,int nCommand);
	//virtual void OnOwnerDraw( ODSTRUCT* odStruct );

	public:

	float yaw;
	float pitch;

	long sunRGB;
	long ambientRGB;

	CBrush brush;

	//{{AFX_DATA(SunDlg)
	enum { IDD = IDD_LIGHT };
	CEdit	m_LightColor;
	CEdit	m_AmbientEdit;
	float	m_Yaw;
	float	m_InitialSunPitch;
	//}}AFX_DATA

	//{{AFX_VIRTUAL(SunDlg)
	//}}AFX_VIRTUAL

protected:

	//{{AFX_MSG(SunDlg)
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnAmbientButton();
	afx_msg void OnLightButton();
	virtual void OnOK();
	afx_msg void OnRecalculateTerrainShadowsButton();
	afx_msg void OnRefractalizeTerrainButton();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	afx_msg void OnChangeLightcolor(UINT id);

	virtual BOOL OnInitDialog();

	void displayInHex( long number, CEdit& edit );
	long getHexValue( CEdit& edit );
	void DoColorDlg( CEdit& edit);

	void applyChanges();

	CBrush backgroundBrush;



		
};


//*************************************************************************************************
#endif  // end of file ( SunDlg.h )
