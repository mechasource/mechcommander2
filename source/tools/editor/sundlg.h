/*************************************************************************************************\
SunDlg.h			: Interface for the SunDlg component.
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//
\*************************************************************************************************/

#pragma once

#ifndef SUNDLG_H
#define SUNDLG_H

//*************************************************************************************************

/**************************************************************************************************
CLASS DESCRIPTION
SunDlg:
**************************************************************************************************/
class SunDlg : public CDialog
{
  public:
	SunDlg(void);
	virtual void Init(void);
	void DoDataExchange(CDataExchange* pDX);

	// virtual	void OnCommand(Window *wnd,int32_t nCommand);
	// virtual void OnOwnerDraw( ODSTRUCT* odStruct );

  public:
	float yaw;
	float pitch;

	int32_t sunRGB;
	int32_t ambientRGB;

	CBrush brush;

	//{{AFX_DATA(SunDlg)
	enum
	{
		IDD = IDD_LIGHT
	};
	CEdit m_LightColor;
	CEdit m_AmbientEdit;
	float m_Yaw;
	float m_InitialSunPitch;
	//}}AFX_DATA

	//{{AFX_VIRTUAL(SunDlg)
	//}}AFX_VIRTUAL

  protected:
	//{{AFX_MSG(SunDlg)
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, uint32_t nCtlColor);
	afx_msg void OnAmbientButton(void);
	afx_msg void OnLightButton(void);
	virtual void OnOK(void);
	afx_msg void OnRecalculateTerrainShadowsButton(void);
	afx_msg void OnRefractalizeTerrainButton(void);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	afx_msg void OnChangeLightcolor(uint32_t id);

	virtual BOOL OnInitDialog(void);

	void displayInHex(int32_t number, CEdit& edit);
	int32_t getHexValue(CEdit& edit);
	void DoColorDlg(CEdit& edit);

	void applyChanges(void);

	CBrush backgroundBrush;
};

//*************************************************************************************************
#endif // end of file ( SunDlg.h )
