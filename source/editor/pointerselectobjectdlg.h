#ifndef POINTERSELECTOBJECTDLG_H
#define POINTERSELECTOBJECTDLG_H
/*************************************************************************************************\
PointerSelectObjectDlg.h		: Interface for the PointerSelectObjectDlg component.
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//
\*************************************************************************************************/

#include "resource.h"
#include "Objective.h"
#include "stdafx.h"


class PointerSelectObjectDlg: public CDialog
{
private:
	bool m_bTimerIsReset;
public:
	PointerSelectObjectDlg();
	PointerSelectObjectDlg(int x, int y);
	virtual ~PointerSelectObjectDlg();

	BOOL OnCommand(WPARAM wParam, LPARAM lParam); // called by child controls to inform of an event
	virtual BOOL OnWndMsg( UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult );
	BOOL OnInitDialog();
	UINT OnNcHitTest( CPoint point );

private:
	typedef CDialog inherited;

	// suppressing these
	//inline PointerSelectObjectDlg();
	PointerSelectObjectDlg& operator=( const PointerSelectObjectDlg& lgUnitPtr );

	CButton *m_pButton;
};


//*************************************************************************************************
#endif  // end of file ( PointerSelectObjectDlg.h )
