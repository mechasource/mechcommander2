//===========================================================================//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#if !defined(AFX_UNITSETTINGSDLG_H__66842972_D1C0_48E8_8DF6_930ED43A3896__INCLUDED_)
#define AFX_UNITSETTINGSDLG_H__66842972_D1C0_48E8_8DF6_930ED43A3896__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// UnitSettingsDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// UnitSettingsDlg dialog

#include "EList.h"
#include "Action.h"
class Unit;

class UnitSettingsDlg : public CDialog
{
public:
	UnitSettingsDlg(EList<Unit*, Unit*>&, ActionUndoMgr &undoMgr);   // standard constructor

	//{{AFX_DATA(UnitSettingsDlg)
	enum { IDD = IDD_UNITSETTINGS };
	CComboBox	m_Variant;
	CComboBox	m_Pilot;
	CComboBox	m_Mech;
	CComboBox	m_Group;
	int		m_Alignment;
	CString	m_SquadEdit;
	int		m_SelfRepairBehavior;
	//}}AFX_DATA


	//{{AFX_VIRTUAL(UnitSettingsDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	void DoColorBox( CWnd* pWnd );

	//{{AFX_MSG(UnitSettingsDlg)
	afx_msg void OnSelchangeGroup();
	afx_msg void OnHighilight2edit();
	afx_msg void OnChangeHighlight1();
	afx_msg void OnHighlight1edit();
	afx_msg void OnChangeHighlight2();
	afx_msg void OnChangeBase();
	afx_msg void OnBaseedit();
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnSelchangeMech();
	virtual void OnCancel();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	afx_msg void OnAlign1(UINT which);

	void applyChanges();
	ModifyBuildingAction *pAction;

	typedef EList< Unit*, Unit* > UNIT_LIST;
	UNIT_LIST& units;

	CBrush		baseBrush;
	CBrush		brush1;
	CBrush		brush2;

	void updatePossibiltyControls();
	Unit *pFirstPossibility;
	int getPossibilityIndex();
	void updateMemberVariables();
	ActionUndoMgr *pUndoMgr;
};

//{{AFX_INSERT_LOCATION}}

#endif // !defined(AFX_UNITSETTINGSDLG_H__66842972_D1C0_48E8_8DF6_930ED43A3896__INCLUDED_)
