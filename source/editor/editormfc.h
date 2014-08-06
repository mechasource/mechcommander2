//===========================================================================//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

// EditorMFC.h : main header file for the EDITORMFC application
//

#if !defined(AFX_EDITORMFC_H__FFBDF9AD_0923_4563_968D_887E72897ECF__INCLUDED_)
#define AFX_EDITORMFC_H__FFBDF9AD_0923_4563_968D_887E72897ECF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// EditorMFCApp:
// See EditorMFC.cpp for the implementation of this class
//

class EditorMFCApp : public CWinApp
{
public:
	EditorMFCApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(EditorMFCApp)
	public:
	virtual BOOL InitInstance();
	virtual BOOL OnIdle(LONG lCount);
	virtual int ExitInstance();
	//}}AFX_VIRTUAL

// Implementation

public:
	//{{AFX_MSG(EditorMFCApp)
	afx_msg void OnAppAbout();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_EDITORMFC_H__FFBDF9AD_0923_4563_968D_887E72897ECF__INCLUDED_)
