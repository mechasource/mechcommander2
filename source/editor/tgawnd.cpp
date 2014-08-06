//===========================================================================//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

// TGAWnd.cpp : implementation file
//

#include "stdafx.h"
#include "TGAWnd.h"
#include "TGAInfo.h"


/////////////////////////////////////////////////////////////////////////////
// TGAWnd

TGAWnd::TGAWnd()
{
	bThisIsInitialized = false;

	m_bTGAChanged = false;
	m_pImage = NULL;
	m_pBmi = (BITMAPINFO*)malloc( sizeof( BITMAPINFOHEADER ) );
	memset( m_pBmi, 0, sizeof( BITMAPINFOHEADER ) );

	m_pBmi->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	m_pBmi->bmiHeader.biPlanes = 1;
	m_pBmi->bmiHeader.biBitCount = 32;
	m_pBmi->bmiHeader.biCompression = 0 ;
	m_pBmi->bmiHeader.biXPelsPerMeter = 0;
	m_pBmi->bmiHeader.biYPelsPerMeter = 0;
	m_pBmi->bmiHeader.biClrUsed = 0;
	m_pBmi->bmiHeader.biClrImportant = 0;

	m_pMemDC = NULL;
	m_hBitmap = NULL;

	m_hSplashBitMap = NULL;

	bThisIsInitialized = true;/*it may be premature to flag it as initialized here*/
}

TGAWnd::~TGAWnd()
{
	bThisIsInitialized = false;

	free( m_pBmi );

	if ( m_hBitmap )
		DeleteObject( m_hBitmap );

	if ( m_pImage )
	{
		free( m_pImage );
		m_pImage = NULL;
	}

	if ( m_pMemDC )
		delete m_pMemDC;

	if ( m_hSplashBitMap )
		DeleteObject( m_hSplashBitMap );
}

void TGAWnd::SetTGAFileName( const CString& str )
{
	m_FileName = str;
	m_bTGAChanged = true;
	if ( m_pImage )
	{
		free( m_pImage );
		m_pImage = NULL;
	}

	CFile File;
	if ( !File.Open( str, CFile::modeRead | CFile::shareDenyNone ) )
	{
		m_pBmi->bmiHeader.biHeight = 0;
		m_pBmi->bmiHeader.biWidth = 0;
		return;
	}

	if ( m_hBitmap )
	{
		DeleteObject( m_hBitmap );
		m_hBitmap = NULL;
	}

	TGAFileHeader header;

	File.Read( &header, sizeof( TGAFileHeader ) );
	if ( header.image_type == UNC_TRUE )
	{	
		m_pImage = (char *)malloc( header.width * header.height * 4 );
		File.Read( m_pImage, header.width * header.height * 4);

		m_pBmi->bmiHeader.biHeight = -header.height;
		m_pBmi->bmiHeader.biWidth = header.width;

		

	}
}

void TGAWnd::SetTGAFileData( BYTE* data, int size )
{
	m_FileName = "";
	m_bTGAChanged = true;

	TGAFileHeader* header;

	header = (TGAFileHeader*)data;


	
	if ( m_pImage && (m_pBmi->bmiHeader.biWidth != - header->width || m_pBmi->bmiHeader.biHeight != header->height) )
	{
		free( m_pImage );
		m_pImage = NULL;
		
	}
	if ( !m_pImage )
		m_pImage = (char *) malloc( header->width * header->height * 4 );		


	if ( header->image_type == UNC_TRUE )
	{	
		memcpy( m_pImage, data + sizeof( TGAFileHeader ), header->width * header->height * 4 );
			
		m_pBmi->bmiHeader.biHeight = -header->height;
		m_pBmi->bmiHeader.biWidth = header->width;

		

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
	if (!bThisIsInitialized) { return; }

	CPaintDC dc(this); // device context for painting
	
	if ( m_bTGAChanged )
	{
		// Fill in the BITMAPINFOHEADER

		if ( !m_pMemDC )
		{
			m_pMemDC = new CDC;
			m_pMemDC->CreateCompatibleDC(&dc);
		}
		
		if ( !m_hBitmap )
			m_hBitmap = CreateDIBSection( dc.m_hDC, m_pBmi, DIB_RGB_COLORS, (void **)&m_pBits, NULL, 0 );

		memcpy( m_pBits, m_pImage, m_pBmi->bmiHeader.biWidth * -m_pBmi->bmiHeader.biHeight * 4 );

		m_bTGAChanged = false;
	}

	if ( m_pMemDC )
	{
		HGDIOBJ hOldObj = m_pMemDC->SelectObject( m_hBitmap );

		CRect rect;
		GetWindowRect( rect );

		dc.SetStretchBltMode( STRETCH_DELETESCANS );
		dc.StretchBlt( 0, 0, rect.Width(), rect.Height(), m_pMemDC, 0, 0, m_pBmi->bmiHeader.biWidth, 
			-m_pBmi->bmiHeader.biHeight, SRCCOPY );

		m_pMemDC->SelectObject( hOldObj );
	}
	else
	{
		if (!m_hSplashBitMap) {
			m_hSplashBitMap = (HBITMAP)LoadImage(NULL, "tacsplash.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
		}
		if (m_hSplashBitMap) {
			BITMAP bm_struct;
			{
				CBitmap *pSplashBitmap = CBitmap::FromHandle(m_hSplashBitMap);
				pSplashBitmap->GetBitmap(&bm_struct);
			}
			CDC l_MemDC;
			l_MemDC.CreateCompatibleDC(&dc);

			HGDIOBJ hOldObj = l_MemDC.SelectObject( m_hSplashBitMap );

			CRect rect;
			GetWindowRect( rect );

			dc.SetStretchBltMode( STRETCH_DELETESCANS );
			dc.StretchBlt( 0, 0, rect.Width(), rect.Height(), &l_MemDC, 0, 0, bm_struct.bmWidth, 
				bm_struct.bmHeight, SRCCOPY );

			l_MemDC.SelectObject( hOldObj );
		}
	}

	// Do not call CStatic::OnPaint() for painting messages
}
