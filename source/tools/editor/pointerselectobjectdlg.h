/*************************************************************************************************\
PointerSelectObjectDlg.h		: Interface for the PointerSelectObjectDlg component.
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//
\*************************************************************************************************/

#pragma once

#ifndef POINTERSELECTOBJECTDLG_H
#define POINTERSELECTOBJECTDLG_H

//#include "resource.h"
//#include "objective.h"
//#include "stdafx.h"

class PointerSelectObjectDlg: public CDialog
{
private:
	bool m_bTimerIsReset;
public:
	PointerSelectObjectDlg();
	PointerSelectObjectDlg(int32_t x, int32_t y);
	virtual ~PointerSelectObjectDlg();

	BOOL OnCommand(WPARAM wParam, LPARAM lParam); // called by child controls to inform of an event
	virtual BOOL OnWndMsg( uint32_t message, WPARAM wParam, LPARAM lParam, LRESULT* pResult );
	BOOL OnInitDialog();
	uint32_t OnNcHitTest( CPoint point );

private:
	typedef CDialog inherited;

	// suppressing these
	//inline PointerSelectObjectDlg();
	PointerSelectObjectDlg& operator=( const PointerSelectObjectDlg& lgUnitPtr );

	CButton *m_pButton;
};


//*************************************************************************************************
#endif  // end of file ( PointerSelectObjectDlg.h )
