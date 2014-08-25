//===========================================================================//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

// MainFrm.h : interface of the MainFrame class
//
/////////////////////////////////////////////////////////////////////////////

#pragma once

#if !defined(AFX_MAINFRM_H__A4EA81D6_9511_4D40_A8C5_25390ECDC6FB__INCLUDED_)
#define AFX_MAINFRM_H__A4EA81D6_9511_4D40_A8C5_25390ECDC6FB__INCLUDED_

//#include "editorinterface.h"

class MainFrame : public CFrameWnd
{
	
public:
	MainFrame(void);
protected: 
	DECLARE_DYNAMIC(MainFrame)

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(MainFrame)
	public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL OnCmdMsg(uint32_t nID, int32_t nCode, PVOID pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);
	protected:
	virtual LRESULT WindowProc(uint32_t message, WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~MainFrame(void);
#ifdef _DEBUG
	virtual void AssertValid(void) const;
	virtual void Dump(CDumpContext& dc) const;
#endif

public:  // control bar embedded members
	CStatusBar  m_wndStatusBar;
	/*CToolBar    m_wndToolBar;*/
	CReBar      m_wndReBar;
	CDialogBar      m_wndDlgBar;
	EditorInterface    m_wndView;

// Generated message map functions
protected:
	//{{AFX_MSG(MainFrame)
	afx_msg int32_t OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSetFocus(CWnd *pOldWnd);
	afx_msg void OnClose(void);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MAINFRM_H__A4EA81D6_9511_4D40_A8C5_25390ECDC6FB__INCLUDED_)
