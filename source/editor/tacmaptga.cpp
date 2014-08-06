#define TGAMAPTGA_CPP
/*************************************************************************************************\
TgaMapTGA.cpp			: Implementation of the TgaMapTGA component.
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//
\*************************************************************************************************/
#include "stdafx.h"
#include "TacMapTGA.h"
#include "tacMap.h"
#include "EditorData.h"


BEGIN_MESSAGE_MAP(TacMapTGA, TGAWnd)
	//{{AFX_MSG_MAP(TGAWnd)
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// THIS NEEDS TO BE MOVED TO A DERIVED CLASS!
void TacMapTGA::OnPaint()
{
	TGAWnd::OnPaint();

	CDC* pDC =GetDC();
	CDC& dc = *pDC;
	// device context for painting

	
	if ( eye && land && land->realVerticesMapSide )
	{
 		Stuff::Vector2DOf< long > screen;
		gos_VERTEX tmp;
		Stuff::Vector3D world;
		POINT pts[5];


		// alrighty need to draw that little rectangle
		screen.x = 1;
		screen.y = 1;
		eye->inverseProject( screen, world );
		TacMap::worldToTacMap( world, 0, 0, TACMAP_SIZE, TACMAP_SIZE,  tmp );
		pts[0].x = tmp.x;
		pts[0].y = tmp.y;

		screen.y = Environment.screenHeight - 1;
		eye->inverseProject( screen, world );
		TacMap::worldToTacMap( world, 0, 0, TACMAP_SIZE, TACMAP_SIZE,  tmp );
		pts[1].x = tmp.x;
		pts[1].y = tmp.y;

		screen.x = Environment.screenWidth - 1;
		eye->inverseProject( screen, world );
		TacMap::worldToTacMap( world, 0, 0,TACMAP_SIZE, TACMAP_SIZE, tmp );
		pts[2].x = tmp.x;
		pts[2].y = tmp.y;

		screen.y = 1;
		eye->inverseProject( screen, world );
		TacMap::worldToTacMap( world, 0, 0, TACMAP_SIZE, TACMAP_SIZE, tmp );
		pts[3].x = tmp.x;
		pts[3].y = tmp.y;

		pts[4] = pts[0];

		CPen ourPen( PS_SOLID, 1,  0x00ffffff );
		CPen *pReplacedPen = dc.SelectObject(&ourPen);

		dc.MoveTo( pts[0].x, pts[0].y );
		for ( int i = 1; i < 5; ++i )
		{
			dc.LineTo( pts[i] );
		}

		dc.SelectObject(pReplacedPen);
	}

	ReleaseDC( pDC );
}

void TacMapTGA::refreshBmp()
{
	EditorData::instance->drawTacMap( (BYTE*)m_pBits, 128 * 128 * 4, 128 );
	RedrawWindow( );
}
	
	
//*************************************************************************************************
// end of file ( TgaMapTGA.cpp )
