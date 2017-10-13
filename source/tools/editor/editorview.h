//===========================================================================//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//
// EditorView.h : interface of the EditorView class
//
/////////////////////////////////////////////////////////////////////////////

#pragma once

#if !defined(AFX_EDITORVIEW_H__9E5FF9E4_BE22_4D61_AC89_58B0AA90DD56__INCLUDED_)
#define AFX_EDITORVIEW_H__9E5FF9E4_BE22_4D61_AC89_58B0AA90DD56__INCLUDED_

/////////////////////////////////////////////////////////////////////////////
// EditorView window

class EditorView : public CWnd
{
	// Construction
  public:
	EditorView(void);

	// Attributes
  public:
	// Operations
  public:
	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(EditorView)
  protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual LRESULT WindowProc(uint32_t message, WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

	// Implementation
  public:
	virtual ~EditorView(void);

	// Generated message map functions
  protected:
	//{{AFX_MSG(EditorView)
	afx_msg void OnPaint(void);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before
// the previous line.

#endif // !defined(AFX_EDITORVIEW_H__9E5FF9E4_BE22_4D61_AC89_58B0AA90DD56__INCLUDED_)
