//===========================================================================//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

// EditorTacMap.cpp : implementation file
//

#include "stdinc.h"
#include "resource.h"
#include "EditorTacMap.h"
#include "tacmap.h"
#include "EditorInterface.h"

/////////////////////////////////////////////////////////////////////////////
// EditorTacMap dialog

EditorTacMap::EditorTacMap(CWnd* pParent /*=nullptr*/)
	: CDialog(EditorTacMap::IDD, pParent)
{
	//{{AFX_DATA_INIT(EditorTacMap)
	// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

void EditorTacMap::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(EditorTacMap)
	DDX_Control(pDX, IDC_TGA, picture);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(EditorTacMap, CDialog)
	//{{AFX_MSG_MAP(EditorTacMap)
	ON_BN_CLICKED(IDC_TGA, OnTga)
	ON_WM_ERASEBKGND()
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// EditorTacMap message handlers

BOOL EditorTacMap::OnInitDialog()
{
	CDialog::OnInitDialog();
	int32_t borderSize = GetSystemMetrics(SM_CYSMCAPTION);
	borderSize += GetSystemMetrics(SM_CYEDGE) * 2;
	CPoint p(0, 0);
	EditorInterface::instance()->ClientToScreen(&p);
	SetWindowPos(nullptr, p.x, p.y + 32 /*arbitrary*/, (int32_t)TACMAP_SIZE,
		(int32_t)TACMAP_SIZE + borderSize, /*SWP_NOMOVE | */ SWP_NOZORDER);
	picture.SetWindowPos(nullptr, 0, 0, (int32_t)TACMAP_SIZE, (int32_t)TACMAP_SIZE, SWP_NOZORDER);
	return TRUE; // return TRUE unless you set the focus to a control
		// EXCEPTION: OCX Property Pages should return FALSE
}

void EditorTacMap::OnTga()
{
	POINT pt;
	GetCursorPos(&pt);
	picture.ScreenToClient(&pt);
	Stuff::Vector2DOf<int32_t> screen;
	screen.x = pt.x;
	screen.y = pt.y;
	Stuff::Vector3D world;
	TacMap::tacMapToWorld(screen, (int32_t)TACMAP_SIZE, (int32_t)TACMAP_SIZE, world);
	eye->setPosition(world, false);
	/* After calling eye->setPosition(), eye needs to be updated so that
	the function eye->inverseProject() is current. That's why we call
	SafeRunGameOSLogic(), although that might be overkill. */
	EditorInterface::instance()->SafeRunGameOSLogic();
	EditorInterface::instance()->syncScrollBars();
	picture.InvalidateRect(nullptr);
	EditorInterface::instance()->SetFocus();
}

BOOL EditorTacMap::OnEraseBkgnd(CDC* pDC)
{
	return 1;
}

void EditorTacMap::ReleaseFocus()
{
	if (EditorInterface::instance() && EditorInterface::instance()->m_hWnd)
	{
		static int32_t recursionDepth = 0;
		if (1 > recursionDepth)
		{
			recursionDepth += 1;
			EditorInterface::instance()->SetFocus();
			recursionDepth -= 1;
		}
	}
	return;
}

LRESULT
EditorTacMap::WindowProc(uint32_t message, WPARAM wparam, LPARAM lparam)
{
	/* if the left mouse button is not down, then we don't want focus */
	CWnd* pCWnd = GetFocus();
	HWND hwnd = ::GetFocus();
	int16_t val = GetAsyncKeyState(VK_LBUTTON);
	if (/*(GetFocus() == this) &&*/ !(0x8000 && GetAsyncKeyState(VK_LBUTTON)))
	{
		ReleaseFocus();
	}
	switch (message)
	{
	case WM_COMMAND:
	{
		if (IDC_TGA != wparam)
		{
			/* default processing would've closed the dialog */
			return 0;
		}
	}
	break;
	}
	return CDialog::WindowProc(message, wparam, lparam);
}
