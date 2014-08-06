//===========================================================================//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

// MainFrm.cpp : implementation of the MainFrame class
//

#include "stdafx.h"
#include "EditorMFC.h"

#include "MainFrm.h"


/////////////////////////////////////////////////////////////////////////////
// MainFrame

IMPLEMENT_DYNAMIC(MainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(MainFrame, CFrameWnd)
	//{{AFX_MSG_MAP(MainFrame)
	ON_WM_CREATE()
	ON_WM_SETFOCUS()
	ON_WM_CLOSE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // status line indicator
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};

/////////////////////////////////////////////////////////////////////////////
// MainFrame construction/destruction

MainFrame::MainFrame()
{
}

MainFrame::~MainFrame()
{
}

int MainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	// create a view to occupy the client area of the frame
	if (!m_wndView.Create(NULL, NULL, AFX_WS_DEFAULT_VIEW | WS_VSCROLL | WS_HSCROLL,
		CRect(0, 0, 0, 0), this, AFX_IDW_PANE_FIRST, NULL))
	{
		TRACE0("Failed to create view window\n");
		return -1;
	}
	
	/*
	if (!m_wndToolBar.CreateEx(this) ||
		!m_wndToolBar.LoadToolBar(IDR_EDITOR_MENU))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}
	*/
	if (!m_wndDlgBar.Create(this, IDR_EDITOR_MENU, 
		CBRS_ALIGN_TOP, AFX_IDW_DIALOGBAR))
	{
		TRACE0("Failed to create dialogbar\n");
		return -1;		// fail to create
	}

	/* mh: rebars don't seem to be supported under win95 */
#if 0
	if (!m_wndReBar.Create(this) ||
		/*!m_wndReBar.AddBar(&m_wndToolBar) ||*/
		!m_wndReBar.AddBar(&m_wndDlgBar))
	{
		TRACE0("Failed to create rebar\n");
		return -1;      // fail to create
	}
#endif

	if (!m_wndStatusBar.Create(this) ||
		!m_wndStatusBar.SetIndicators(indicators,
		  sizeof(indicators)/sizeof(UINT)))
	{
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
	}

	/*
	m_wndToolBar.SetBarStyle(m_wndToolBar.GetBarStyle() |
		CBRS_TOOLTIPS | CBRS_FLYBY);
		*/


	return 0;
}

BOOL MainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	
	if( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;
//	cs.dwExStyle &= ~WS_EX_CLIENTEDGE;
	cs.lpszClass = AfxRegisterWndClass(0);
	cs.cx = GetSystemMetrics( SM_CXFULLSCREEN );;
	cs.cy = GetSystemMetrics( SM_CYFULLSCREEN );;
	cs.x = 0;
	cs.y = 0;
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// MainFrame diagnostics

#ifdef _DEBUG
void MainFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}

void MainFrame::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// MainFrame message handlers
void MainFrame::OnSetFocus(CWnd* pOldWnd)
{
	// forward focus to the view window
	if (m_wndView.m_hWnd) {
		m_wndView.SetFocus();
	}
}

BOOL MainFrame::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo)
{
	// let the view have first crack at the command
	if (m_wndView.OnCmdMsg(nID, nCode, pExtra, pHandlerInfo))
		return TRUE;

	// otherwise, do default handling
	return CFrameWnd::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}


LRESULT MainFrame::WindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	if ( message == WM_MOVE )
	{
		POINT tmp;
		tmp.x = LOWORD(lParam) + 30; 
		tmp.y = HIWORD(lParam) + 30; 

		EditorInterface::instance()->ScreenToClient( &tmp );

		LPARAM lNewParam = tmp.y << 16 | tmp.x;

		EditorInterface::instance()->SendMessage( WM_MOVE, wParam, lNewParam );

	}
	return CFrameWnd::WindowProc(message, wParam, lParam);
}

void MainFrame::OnClose() 
{
	int res = IDNO;
	if (EditorInterface::instance() && EditorInterface::instance()->ThisIsInitialized()
		&& EditorData::instance) {
		res = EditorInterface::instance()->PromptAndSaveIfNecessary();
	}
	if (IDCANCEL != res) {
		if (EditorInterface::instance()) {
			EditorInterface::instance()->SetBusyMode();
		}
		gos_TerminateApplication();
		PostQuitMessage(0);
		if (EditorInterface::instance()) {
			EditorInterface::instance()->UnsetBusyMode();
		}
	}
}
