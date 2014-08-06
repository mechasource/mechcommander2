//===========================================================================//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#if !defined(AFX_TEXTMESSAGEDLG_H__0B7C2BD0_F277_4D66_9FF8_2169FF65ABC2__INCLUDED_)
#define AFX_TEXTMESSAGEDLG_H__0B7C2BD0_F277_4D66_9FF8_2169FF65ABC2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TextMessageDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// TextMessageDlg dialog

class TextMessageDlg : public CDialog
{
// Construction
public:
	TextMessageDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(TextMessageDlg)
	enum { IDD = IDD_TEXT_MESSAGE };
	CString	m_TextMessage;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(TextMessageDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(TextMessageDlg)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TEXTMESSAGEDLG_H__0B7C2BD0_F277_4D66_9FF8_2169FF65ABC2__INCLUDED_)
