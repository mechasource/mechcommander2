//===========================================================================//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#ifndef TGAWND_H
#define TGAWND_H
//----------------------------------------------------------------------------
//
// TGaWnd.h - window that displays a targa with a given window name
//
// FASA Interactive Technologies
//
// Copyright (c) 1998 FASA Interactive Technologies
//
//---------------------------------------------------------------------------

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "stdafx.h"

class TGAWnd : public CStatic
{
// Construction
public:
	TGAWnd();
	void SetTGAFileName( const CString& str );
	void SetTGAFileData( BYTE* data, int size );

	char* GetImageData( ) { return m_pImage; }

	//{{AFX_VIRTUAL(TGAWnd)
	//}}AFX_VIRTUAL

public:
	virtual ~TGAWnd();

protected:
	//{{AFX_MSG(TGAWnd)
	afx_msg void OnPaint();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

	CString m_FileName;
	bool	m_bTGAChanged;
	char*	m_pImage;
	BITMAPINFO* m_pBmi;
	HBITMAP	m_hBitmap;
	void*	m_pBits;
	CDC*	m_pMemDC;
	CPalette* m_Palette;
	HBITMAP m_hSplashBitMap;
	bool bThisIsInitialized;
};	


//{{AFX_INSERT_LOCATION}}

#endif// TGAWND_H