/*************************************************************************************************\
PointerSelectObjectDlg.cpp			: Implementation of the PointerSelectObjectDlg component.
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//
\*************************************************************************************************/

#include "resource.h"

#include <stdlib.h>
#include <assert.h>
#include "EString.h"

#include "PointerSelectObjectDlg.h"
#include "Objective.h"

#include "EditorInterface.h"

#include "MFCPlatform.hpp"

static const int WM_DOTICK = WM_USER + 1;

//-------------------------------------------------------------------------------------------------
PointerSelectObjectDlg::PointerSelectObjectDlg():CDialog(IDD_POINTER_SELECT_OBJECT)
{
	m_pButton = 0;
	m_bTimerIsReset = true;
}

PointerSelectObjectDlg::PointerSelectObjectDlg(int x, int y):CDialog(IDD_POINTER_SELECT_OBJECT)
{
	m_pButton = 0;
	m_bTimerIsReset = true;
}

BOOL PointerSelectObjectDlg::OnInitDialog()
{
	//m_pButton = (CButton *)GetDlgItem(IDC_POINTER_SELECT_OBJECT_BUTTON);
	//assert( m_pButton );

	RECT rc;
	EditorInterface::instance()->GetClientRect(&rc);
	EditorInterface::instance()->ClientToScreen(&rc);
	MoveWindow(&rc);
	return 1;
}

UINT PointerSelectObjectDlg::OnNcHitTest( CPoint point )
{
	return HTCLIENT;
}

BOOL PointerSelectObjectDlg::OnCommand(WPARAM wParam, LPARAM lParam) // called by child controls to inform of an event
{
//	HWND hWndCtrl = (HWND)lParam;

//	if (hWndCtrl == m_pButton->m_hWnd)
//	{
//		EndDialog(0);
//	}
	return inherited::OnCommand(wParam, lParam);
}

#include "afxpriv.h" // just for definition of WM_KICKIDLE
BOOL PointerSelectObjectDlg::OnWndMsg( UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult )
{
	switch (message)
	{
	case WM_LBUTTONDOWN:
		{
		}
		break;
	case WM_RBUTTONDOWN:
		{
			EndDialog(0);
			return TRUE;
		}
		break;
	case WM_ACTIVATE:
		{
		}
		break;
	case WM_CAPTURECHANGED:
		{
		}
		break;
	case WM_NCHITTEST:
		{
		}
		break;
	case WM_PAINT:
		{
			PAINTSTRUCT ps;
			BeginPaint(&ps);
			EndPaint(&ps);
			//m_pButton->Invalidate(FALSE);
			return TRUE;
		}
		break;
	case WM_TIMER:
		{
			::KillTimer(GetSafeHwnd(), wParam);
			EditorInterface::instance()->SafeRunGameOSLogic();
			m_bTimerIsReset = true;
			//m_pButton->Invalidate(FALSE);
			return TRUE;
		}
		break;
	case WM_KEYDOWN:
		{
			//int i = 17;
		}
		break;
	}
	BOOL ret = CDialog::OnWndMsg( message, wParam, lParam, pResult );
	if (((WM_MOUSEFIRST <= message) && (WM_MOUSELAST >= message))) {
		POINT pt;
		pt.x = LOWORD(lParam);
		pt.y = HIWORD(lParam);
		ClientToScreen(&pt);
		EditorInterface::instance()->ScreenToClient(&pt);
		lParam = MAKELPARAM(pt.x, pt.y);
		EditorInterface::instance()->SendMessage(message, wParam, lParam);
	} else if ((WM_KEYDOWN == message) || (WM_KEYUP == message)) {
		switch ((int)wParam) {
		case VK_RETURN:
		case VK_SPACE:
		case VK_PRIOR:
		case VK_NEXT:
		case VK_END:
		case VK_HOME:
		case VK_LEFT:
		case VK_UP:
		case VK_RIGHT:
		case VK_DOWN:
		case VK_ADD:
		case VK_SUBTRACT:
			EditorInterface::instance()->SendMessage(message, wParam, lParam);
			break;
		}
	}

	{
		static int depth = 0;
		if (0 == depth) {
			MSG msg;
			/* calling PeekMessage() from within OnWndMsg() allows it to become reentrant */
			depth += 1;
			BOOL result = PeekMessage(&msg, GetSafeHwnd(), 0, 0, PM_NOREMOVE);
			depth -= 1;
			if ((0 == result) && (m_bTimerIsReset)) {
				unsigned int ui = ::SetTimer(GetSafeHwnd(), 1/*arbitrary non-zero ID*/, 2/*milliseconds*/, NULL);
				assert(ui);
				m_bTimerIsReset = false;
			}
		} 	
	}
	return ret;
}
//-------------------------------------------------------------------------------------------------

PointerSelectObjectDlg::~PointerSelectObjectDlg()
{
}


//*************************************************************************************************
// end of file ( PointerSelectObjectDlg.cpp )
