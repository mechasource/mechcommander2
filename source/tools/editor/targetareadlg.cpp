/*************************************************************************************************\
TargetAreaDlg.cpp			: Implementation of the TargetAreaDlg component.
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//
\*************************************************************************************************/

//#include "resource.h"
//#include <stdlib.h>
//#include <_ASSERT.h>
//#include "estring.h"
//#include "targetareadlg.h"
//#include "objective.h"
//#include "editorinterface.h"

//-------------------------------------------------------------------------------------------------
TargetAreaDlg::TargetAreaDlg(float& targetCenterX, float& targetCenterY, float& targetRadius)
	: CDialog(IDD_TARGET_AREA)
{
	m_pTargetCenterX = &targetCenterX;
	m_pTargetCenterY = &targetCenterY;
	m_pTargetRadius = &targetRadius;
	m_pTargetCenterXEditBox = 0;
	m_pTargetCenterYEditBox = 0;
	m_pTargetRadiusEditBox = 0;
	m_pCancelButton = 0;
	m_pOKButton = 0;
}

BOOL TargetAreaDlg::OnInitDialog()
{
	m_pTargetCenterXEditBox = (CEdit*)GetDlgItem(IDC_TARGET_AREA_CENTER_X_EDIT);
	_ASSERT(m_pTargetCenterXEditBox);
	m_pTargetCenterYEditBox = (CEdit*)GetDlgItem(IDC_TARGET_AREA_CENTER_Y_EDIT);
	_ASSERT(m_pTargetCenterYEditBox);
	m_pTargetRadiusEditBox = (CEdit*)GetDlgItem(IDC_TARGET_AREA_RADIUS_EDIT);
	_ASSERT(m_pTargetRadiusEditBox);
	m_pCancelButton = (CButton*)GetDlgItem(IDCANCEL);
	_ASSERT(m_pCancelButton);
	m_pOKButton = (CButton*)GetDlgItem(IDOK);
	_ASSERT(m_pOKButton);
	std::wstring_view tmpStr;
	tmpStr.Format("%.3f", (*m_pTargetCenterX));
	m_pTargetCenterXEditBox->SetWindowText(tmpStr.Data());
	tmpStr.Format("%.3f", (*m_pTargetCenterY));
	m_pTargetCenterYEditBox->SetWindowText(tmpStr.Data());
	tmpStr.Format("%.3f", (*m_pTargetRadius));
	m_pTargetRadiusEditBox->SetWindowText(tmpStr.Data());
	return 1;
}

BOOL TargetAreaDlg::OnCommand(WPARAM wparam,
	LPARAM lparam) // called by child controls to inform of an event
{
	_ASSERT(m_pCancelButton);
	_ASSERT(m_pOKButton);
	return inherited::OnCommand(wparam, lparam);
}

void TargetAreaDlg::OnCancel()
{
	EndDialog(IDCANCEL);
}

void TargetAreaDlg::OnOK()
{
	CString tmpCStr;
	int32_t result;
	float tmpFloat;
	m_pTargetCenterXEditBox->GetWindowText(tmpCStr);
	result = sscanf_s(tmpCStr.GetBuffer(0), "%f", &tmpFloat);
	if (1 == result)
	{
		(*m_pTargetCenterX) = tmpFloat;
	}
	m_pTargetCenterYEditBox->GetWindowText(tmpCStr);
	result = sscanf_s(tmpCStr.GetBuffer(0), "%f", &tmpFloat);
	if (1 == result)
	{
		(*m_pTargetCenterY) = tmpFloat;
	}
	m_pTargetRadiusEditBox->GetWindowText(tmpCStr);
	result = sscanf_s(tmpCStr.GetBuffer(0), "%f", &tmpFloat);
	if (1 == result)
	{
		(*m_pTargetRadius) = tmpFloat;
	}
	EndDialog(IDOK);
}

//-------------------------------------------------------------------------------------------------

TargetAreaDlg::~TargetAreaDlg() { }

// end of file ( TargetAreaDlg.cpp )
