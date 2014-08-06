//===========================================================================//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#if !defined(AFX_BUILDINGSETTINGSDLG_H__66842972_D1C0_48E8_8DF6_930ED43A3896__INCLUDED_)
#define AFX_BUILDINGSETTINGSDLG_H__66842972_D1C0_48E8_8DF6_930ED43A3896__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// BuildingSettingsDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// BuildingSettingsDlg dialog

#include "EList.h"
#include "Action.h"
class EditorObject;

class BuildingSettingsDlg : public CDialog
{
public:
	BuildingSettingsDlg(EList<EditorObject*, EditorObject*>&, ActionUndoMgr &undoMgr);   // standard constructor

	//{{AFX_DATA(BuildingSettingsDlg)
	enum { IDD = IDD_BUILDINGSETTINGS };
	CComboBox	m_Mech;
	CComboBox	m_Group;
	int		m_Alignment;
	float	m_x;
	float	m_y;
	int		m_partID;
	CString	m_forestName;
	//}}AFX_DATA


	//{{AFX_VIRTUAL(BuildingSettingsDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	void DoColorBox( CWnd* pWnd );

	//{{AFX_MSG(BuildingSettingsDlg)
	afx_msg void OnSelchangeGroup();
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeMech();
	virtual void OnCancel();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	void applyChanges();
	ModifyBuildingAction *pAction;

	typedef EList< EditorObject*, EditorObject* > EDITOROBJECT_LIST;
	EDITOROBJECT_LIST& units;

	void updateMemberVariables();
	ActionUndoMgr *pUndoMgr;
};

//{{AFX_INSERT_LOCATION}}

#endif // !defined(AFX_BUILDINGSETTINGSDLG_H__66842972_D1C0_48E8_8DF6_930ED43A3896__INCLUDED_)
