//===========================================================================//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//
// EditorView.h : interface of the EditorView class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_EDITORVIEW_H__9E5FF9E4_BE22_4D61_AC89_58B0AA90DD56__INCLUDED_)
#define AFX_EDITORVIEW_H__9E5FF9E4_BE22_4D61_AC89_58B0AA90DD56__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// EditorView window

class EditorView : public CWnd
{
// Construction
public:
	EditorView();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(EditorView)
	protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~EditorView();

	// Generated message map functions
protected:
	//{{AFX_MSG(EditorView)
	afx_msg void OnPaint();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_EDITORVIEW_H__9E5FF9E4_BE22_4D61_AC89_58B0AA90DD56__INCLUDED_)
