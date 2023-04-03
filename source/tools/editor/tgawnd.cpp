//===========================================================================//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

// TGAWnd.cpp : implementation file
//

#include "stdinc.h"
#include "tgawnd.h"
#include "tgainfo.h"

/////////////////////////////////////////////////////////////////////////////
// TGAWnd

TGAWnd::TGAWnd()
{
	bThisIsInitialized = false;
	m_bTGAChanged = false;
	m_pImage = nullptr;
	m_pBmi = (BITMAPINFO*)malloc(sizeof(BITMAPINFOHEADER));
	memset(m_pBmi, 0, sizeof(BITMAPINFOHEADER));
	m_pBmi->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	m_pBmi->bmiHeader.biPlanes = 1;
	m_pBmi->bmiHeader.biBitCount = 32;
	m_pBmi->bmiHeader.biCompression = 0;
	m_pBmi->bmiHeader.biXPelsPerMeter = 0;
	m_pBmi->bmiHeader.biYPelsPerMeter = 0;
	m_pBmi->bmiHeader.biClrUsed = 0;
	m_pBmi->bmiHeader.biClrImportant = 0;
	m_pMemDC = nullptr;
	m_hBitmap = nullptr;
	m_hSplashBitMap = nullptr;
	bThisIsInitialized = true; /*it may be premature to flag it as initialized here*/
}

TGAWnd::~TGAWnd()
{
	bThisIsInitialized = false;
	free(m_pBmi);
	if (m_hBitmap)
		DeleteObject(m_hBitmap);
	if (m_pImage)
	{
		free(m_pImage);
		m_pImage = nullptr;
	}
	if (m_pMemDC)
		delete m_pMemDC;
	if (m_hSplashBitMap)
		DeleteObject(m_hSplashBitMap);
}

void TGAWnd::SetTGAFileName(const CString& str)
{
	m_FileName = str;
	m_bTGAChanged = true;
	if (m_pImage)
	{
		free(m_pImage);
		m_pImage = nullptr;
	}
	CFile File;
	if (!File.Open(str, CFile::modeRead | CFile::shareDenyNone))
	{
		m_pBmi->bmiHeader.biheight = 0;
		m_pBmi->bmiHeader.biwidth = 0;
		return;
	}
	if (m_hBitmap)
	{
		DeleteObject(m_hBitmap);
		m_hBitmap = nullptr;
	}
	TGAFileHeader header;
	File.Read(&header, sizeof(TGAFileHeader));
	if (header.image_type == UNC_TRUE)
	{
		m_pImage = (std::wstring_view)malloc(header.width * header.height * 4);
		File.Read(m_pImage, header.width * header.height * 4);
		m_pBmi->bmiHeader.biheight = -header.height;
		m_pBmi->bmiHeader.biwidth = header.width;
	}
}

void TGAWnd::SetTGAFileData(uint8_t* data, int32_t size)
{
	m_FileName = "";
	m_bTGAChanged = true;
	TGAFileHeader* header;
	header = (TGAFileHeader*)data;
	if (m_pImage && (m_pBmi->bmiHeader.biwidth != -header->width || m_pBmi->bmiHeader.biheight != header->height))
	{
		free(m_pImage);
		m_pImage = nullptr;
	}
	if (!m_pImage)
		m_pImage = (std::wstring_view)malloc(header->width * header->height * 4);
	if (header->image_type == UNC_TRUE)
	{
		memcpy(m_pImage, data + sizeof(TGAFileHeader), header->width * header->height * 4);
		m_pBmi->bmiHeader.biheight = -header->height;
		m_pBmi->bmiHeader.biwidth = header->width;
	}
}

BEGIN_MESSAGE_MAP(TGAWnd, CStatic)
	//{{AFX_MSG_MAP(TGAWnd)
	ON_WM_PAINT()
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// TGAWnd message handlers

void TGAWnd::OnPaint()
{
	if (!bThisIsInitialized)
	{
		return;
	}
	CPaintDC dc(this); // device context for painting
	if (m_bTGAChanged)
	{
		// Fill in the BITMAPINFOHEADER
		if (!m_pMemDC)
		{
			m_pMemDC = new CDC;
			m_pMemDC->CreateCompatibleDC(&dc);
		}
		if (!m_hBitmap)
			m_hBitmap =
				CreateDIBSection(dc.m_hDC, m_pBmi, DIB_RGB_COLORS, (PVOID*)&m_pBits, nullptr, 0);
		memcpy(m_pBits, m_pImage, m_pBmi->bmiHeader.biwidth * -m_pBmi->bmiHeader.biheight * 4);
		m_bTGAChanged = false;
	}
	if (m_pMemDC)
	{
		HGDIOBJ hOldObj = m_pMemDC->SelectObject(m_hBitmap);
		CRect rect;
		GetWindowRect(rect);
		dc.SetStretchBltMode(STRETCH_DELETESCANS);
		dc.StretchBlt(0, 0, rect.width(), rect.height(), m_pMemDC, 0, 0, m_pBmi->bmiHeader.biwidth,
			-m_pBmi->bmiHeader.biheight, SRCCOPY);
		m_pMemDC->SelectObject(hOldObj);
	}
	else
	{
		if (!m_hSplashBitMap)
		{
			m_hSplashBitMap =
				(HBITMAP)LoadImage(nullptr, "tacsplash.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
		}
		if (m_hSplashBitMap)
		{
			BITMAP bm_struct;
			{
				CBitmap* pSplashBitmap = CBitmap::FromHandle(m_hSplashBitMap);
				pSplashBitmap->GetBitmap(&bm_struct);
			}
			CDC l_MemDC;
			l_MemDC.CreateCompatibleDC(&dc);
			HGDIOBJ hOldObj = l_MemDC.SelectObject(m_hSplashBitMap);
			CRect rect;
			GetWindowRect(rect);
			dc.SetStretchBltMode(STRETCH_DELETESCANS);
			dc.StretchBlt(0, 0, rect.width(), rect.height(), &l_MemDC, 0, 0, bm_struct.bmwidth,
				bm_struct.bmheight, SRCCOPY);
			l_MemDC.SelectObject(hOldObj);
		}
	}
	// Do not call CStatic::OnPaint() for painting messages
}
