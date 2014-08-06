#ifndef TARGETAREADLG_H
#define TARGETAREADLG_H
/*************************************************************************************************\
TargetAreaDlg.h		: Interface for the TargetAreaDlg component.
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//
\*************************************************************************************************/

#include "resource.h"
#include <elist.h>
#include "Objective.h"
#include "EditorObjectMgr.h"
#include "stdafx.h"


class TargetAreaDlg: public CDialog
{
public:
	TargetAreaDlg( float &targetCenterX, float &targetCenterY, float &targetRadius );

	BOOL OnCommand(WPARAM wParam, LPARAM lParam); // called by child controls to inform of an event
	void OnCancel();
	void OnOK();

	virtual ~TargetAreaDlg();

	BOOL OnInitDialog();

private:
	typedef CDialog inherited;

	// suppressing these
	inline TargetAreaDlg();
	TargetAreaDlg& operator=( const TargetAreaDlg& lgUnitPtr );

	float *m_pTargetCenterX;
	float *m_pTargetCenterY;
	float *m_pTargetRadius;
	CEdit *m_pTargetCenterXEditBox;
	CEdit *m_pTargetCenterYEditBox;
	CEdit *m_pTargetRadiusEditBox;
	CButton *m_pCancelButton;
	CButton *m_pOKButton;
};


//*************************************************************************************************
#endif  // end of file ( TargetAreaDlg.h )
