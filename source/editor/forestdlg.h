//===========================================================================//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#ifndef FORESTDLG_H
#define FORESTDLG_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#ifndef FOREST_H
#include "Forest.h"
#endif

class ForestDlg : public CDialog
{

public:
	ForestDlg(CWnd* pParent = NULL);   

	//{{AFX_DATA(ForestDlg)
	enum { IDD = IDD_FOREST_DLG };
	float	m_maxDensity;
	float	m_minDensity;
	float	m_maxHeight;
	float	m_minHeight;
	BOOL	m_randomPlacement;
	float	m_xLoc;
	float	m_yLoc;
	float	m_radius;
	CString	m_Name;
	int		m_1;
	int		m_2;
	int		m_3;
	int		m_4;
	int		m_5;
	int		m_6;
	int		m_7;
	int		m_8;
	int		m_9;
	int		m_10;
	int		m_12;
	int		m_13;
	int		m_14;
	int		m_15;
	//}}AFX_DATA

	Forest		forest;

	//{{AFX_VIRTUAL(ForestDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    
	//}}AFX_VIRTUAL

	afx_msg void OnEditChanged( UINT nID );
	afx_msg void OnSliderChanged( UINT id, NMHDR * pNotifyStruct, LRESULT * result );



protected:

	//{{AFX_MSG(ForestDlg)
	afx_msg void OnSave();
	virtual void OnOK();
	virtual void OnCancel();
	virtual BOOL OnInitDialog();
	afx_msg void OnLoad();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


#endif // 
