//===========================================================================//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

// EditorView.cpp : implementation of the EditorView class
//

#include "stdinc.h"
//#include "mclib.h"
#include "EditorMFC.h"
#include "EditorView.h"

/////////////////////////////////////////////////////////////////////////////
// EditorView

EditorView::EditorView() {}

EditorView::~EditorView() {}

BEGIN_MESSAGE_MAP(EditorView, CWnd)
//{{AFX_MSG_MAP(EditorView)
ON_WM_PAINT()
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// EditorView message handlers

BOOL
EditorView::PreCreateWindow(CREATESTRUCT& cs)
{
	if (!CWnd::PreCreateWindow(cs))
		return FALSE;
	cs.dwExStyle |= WS_EX_CLIENTEDGE;
	cs.style &= ~WS_BORDER;
	cs.lpszClass = AfxRegisterWndClass(CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS,
		::LoadCursor(nullptr, IDC_ARROW), HBRUSH(COLOR_WINDOW + 1), nullptr);
	return TRUE;
}

void
EditorView::OnPaint()
{
	CPaintDC dc(this); // device context for painting
		// Do not call CWnd::OnPaint() for painting messages
}

LRESULT
EditorView::WindowProc(uint32_t message, WPARAM wparam, LPARAM lparam)
{
	if (eye)
		return GameOSWinProc(m_hWnd, message, wparam, lparam);
	return CWnd ::WindowProc(message, wparam, lparam);
}
