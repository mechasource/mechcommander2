//===========================================================================//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

#pragma once

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

class TGAWnd : public CStatic
{
	// Construction
public:
	TGAWnd(void);
	void SetTGAFileName(const CString& str);
	void SetTGAFileData(uint8_t* data, int32_t size);

	std::wstring_view GetImageData()
	{
		return m_pImage;
	}

	//{{AFX_VIRTUAL(TGAWnd)
	//}}AFX_VIRTUAL

public:
	virtual ~TGAWnd(void);

protected:
	//{{AFX_MSG(TGAWnd)
	afx_msg void OnPaint(void);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

	CString m_FileName;
	bool m_bTGAChanged;
	std::wstring_view m_pImage;
	BITMAPINFO* m_pBmi;
	HBITMAP m_hBitmap;
	PVOID m_pBits;
	CDC* m_pMemDC;
	CPalette* m_Palette;
	HBITMAP m_hSplashBitMap;
	bool bThisIsInitialized;
};

/**************************************************************************************************
CLASS DESCRIPTION
TACMapTGA:
**************************************************************************************************/
class TacMapTGA : public TGAWnd
{
public:
	afx_msg void OnPaint(void);
	void refreshBmp(void);
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}

#endif // TGAWND_H