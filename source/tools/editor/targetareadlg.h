/*************************************************************************************************\
TargetAreaDlg.h		: Interface for the TargetAreaDlg component.
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//
\*************************************************************************************************/

#pragma once

#ifndef TARGETAREADLG_H
#define TARGETAREADLG_H

//#include <elist.h>
//#include "objective.h"
//#include "editorobjectmgr.h"

class TargetAreaDlg: public CDialog
{
public:
	TargetAreaDlg(float& targetCenterX, float& targetCenterY, float& targetRadius);

	BOOL OnCommand(WPARAM wParam, LPARAM lParam); // called by child controls to inform of an event
	void OnCancel(void);
	void OnOK(void);

	virtual ~TargetAreaDlg(void);

	BOOL OnInitDialog(void);

private:
	typedef CDialog inherited;

	// suppressing these
	inline TargetAreaDlg(void);
	TargetAreaDlg& operator=(const TargetAreaDlg& lgUnitPtr);

	float* m_pTargetCenterX;
	float* m_pTargetCenterY;
	float* m_pTargetRadius;
	CEdit* m_pTargetCenterXEditBox;
	CEdit* m_pTargetCenterYEditBox;
	CEdit* m_pTargetRadiusEditBox;
	CButton* m_pCancelButton;
	CButton* m_pOKButton;
};


//*************************************************************************************************
#endif  // end of file ( TargetAreaDlg.h )
