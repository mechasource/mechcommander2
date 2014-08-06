//===========================================================================//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#if !defined(AFX_OBJECTIVEDLG_H__2D6B2B2E_16F8_43B2_9534_A2DC1672352F__INCLUDED_)
#define AFX_OBJECTIVEDLG_H__2D6B2B2E_16F8_43B2_9534_A2DC1672352F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ObjectiveDlg.h : header file
//

#include "resource.h"
#include "stdafx.h"

#include "Objective.h"

/////////////////////////////////////////////////////////////////////////////
// ObjectiveDlg dialog

class ObjectiveDlg : public CDialog
{
// Construction
public:
	ObjectiveDlg(CWnd* pParent = NULL);   // standard constructor
	void LoadDialogValues();
	void SaveDialogValues();

	void UpdateTitleDisplay();
	void UpdateDescriptionDisplay();

	CObjective m_ModifiedObjective;

	CString m_TitleUnlocalizedText;
	bool m_TitleUseResourceString;
	int m_TitleResourceStringID;

	CString m_DescriptionUnlocalizedText;
	bool m_DescriptionUseResourceString;
	int m_DescriptionResourceStringID;

	int nConditionSpeciesSelectionIndex;
	int nActionSpeciesSelectionIndex;
	int nFailureConditionSpeciesSelectionIndex;
	int nFailureActionSpeciesSelectionIndex;

	class CConditionSpeciesList : public EList <condition_species_type, condition_species_type> {};
	class CActionSpeciesList : public EList <action_species_type, action_species_type> {};
	CConditionSpeciesList comboBoxItems;
	CActionSpeciesList actionComboBoxItems;
	CConditionSpeciesList failureConditionComboBoxItems;
	CActionSpeciesList failureActionComboBoxItems;

	/* for persistent storage of miscellaneous working data*/
	CObjectiveCondition *pNewCondition;
	CObjectiveAction *pNewAction;

// Dialog Data
	//{{AFX_DATA(ObjectiveDlg)
	enum { IDD = IDD_OBJECTIVE };
	CButton	m_AddFailureConditionButton;
	CListBox	m_FailureActionList;
	CListBox	m_FailureConditionList;
	CComboBox	m_FailureActionComboBox;
	CComboBox	m_FailureConditionComboBox;
	CComboBox	m_modelGroup;
	CComboBox	m_Mech;
	CButton	m_AddButton;
	CComboBox	m_ActionComboBox;
	CComboBox	m_ComboBox;
	CListBox	m_ActionList;
	CListBox	m_List;
	int		m_PriorityRadioButton;
	int		m_ResourcePointsEdit;
	BOOL	m_PreviousPrimaryObjectiveMustBeCompleteCheckButton;
	BOOL	m_AllPreviousPrimaryObjectivesMustBeCompleteCheckButton;
	BOOL	m_DisplayMarkerCheckButton;
	float	m_XEdit;
	float	m_YEdit;
	BOOL	m_HiddenTriggerCheckButton;
	BOOL	m_ActivateOnFlagCheckButton;
	int		m_ActivateFlagIndexEdit;
	CString	m_TitleEdit;
	CString	m_DescriptionEdit;
	CString	m_TitleResourceStringIDEdit;
	CString	m_DescriptionResourceStringIDEdit;
	CString	m_ActivateFlagIDEdit;
	BOOL	m_ResetStatusOnFlagCheckButton;
	CString	m_ResetStatusFlagIDEdit;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(ObjectiveDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(ObjectiveDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnObjectiveAddConditionButton();
	afx_msg void OnObjectiveRemoveConditionButton();
	afx_msg void OnObjectiveAddActionButton();
	afx_msg void OnObjectiveRemoveActionButton();
	virtual void OnOK();
	afx_msg void OnSelchangeGroup();
	afx_msg void OnBaseedit2();
	afx_msg void OnHighilight2edit2();
	afx_msg void OnHighlight1edit();
	afx_msg void OnChangeHighlight1();
	afx_msg void OnChangeHighlight2();
	afx_msg void OnChangeBase2();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnObjectiveTitleEditButton();
	afx_msg void OnObjectiveDescriptionEditButton();
	afx_msg void OnObjectiveAddFailureConditionButton();
	afx_msg void OnObjectiveRemoveFailureConditionButton();
	afx_msg void OnObjectiveAddFailureActionButton();
	afx_msg void OnObjectiveRemoveFailureActionButton();
	afx_msg void OnObjectiveViewConditionButton();
	afx_msg void OnObjectiveViewActionButton();
	afx_msg void OnObjectiveViewFailureConditionButton();
	afx_msg void OnObjectiveViewFailureActionButton();
	afx_msg void OnObjectiveClearLogisticsModelButton();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	void DoColorBox( CWnd* pWnd );
	void DoEditColorChange( long ID );

	CBrush		baseBrush;
	CBrush		brush1;
	CBrush		brush2;


};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_OBJECTIVEDLG_H__2D6B2B2E_16F8_43B2_9534_A2DC1672352F__INCLUDED_)
