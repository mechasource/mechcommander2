//===========================================================================//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#pragma once

#ifndef _EDITORTACMAP_H_
#define _EDITORTACMAP_H_

//#include "tgawnd.h"

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

	void SetData( puint8_t pData, size_t size ){ picture.SetTGAFileData( pData, size ); }
	void UpdateMap( ){ picture.refreshBmp(void); }

	void ReleaseFocus(void);
	virtual void OnCancel() { ReleaseFocus(void); }
	virtual void OnOk() { ReleaseFocus(void); }

	//{{AFX_VIRTUAL(EditorTacMap)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual LRESULT WindowProc(uint32_t message, WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

protected:

	//{{AFX_MSG(EditorTacMap)
	virtual BOOL OnInitDialog(void);
	afx_msg void OnTga(void);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#endif
