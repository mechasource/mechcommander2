//===========================================================================//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// EditorTacMap.h : header file
//

#include "resource.h"

#ifndef TGAWND_H
#include "TacMapTGA.h"
#endif

/////////////////////////////////////////////////////////////////////////////
// EditorTacMap dialog

class EditorTacMap : public CDialog
{
public:
	EditorTacMap(CWnd* pParent = NULL);   // standard constructor

	//{{AFX_DATA(EditorTacMap)
	enum { IDD = IDD_TACMAP };
	TacMapTGA	picture;
	//}}AFX_DATA

	void SetData( BYTE* pData, long size ){ picture.SetTGAFileData( pData, size ); }
	void UpdateMap( ){ picture.refreshBmp(); }

	void ReleaseFocus();
	virtual void OnCancel() { ReleaseFocus(); }
	virtual void OnOk() { ReleaseFocus(); }

	//{{AFX_VIRTUAL(EditorTacMap)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

protected:

	//{{AFX_MSG(EditorTacMap)
	virtual BOOL OnInitDialog();
	afx_msg void OnTga();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
