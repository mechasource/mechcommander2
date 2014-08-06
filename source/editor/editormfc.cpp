//===========================================================================//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//

// EditorMFC.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "EditorMFC.h"

#include "MainFrm.h"
#include "MFCPlatform.hpp"

/////////////////////////////////////////////////////////////////////////////
// EditorMFCApp

BEGIN_MESSAGE_MAP(EditorMFCApp, CWinApp)
	//{{AFX_MSG_MAP(EditorMFCApp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// EditorMFCApp construction

EditorMFCApp::EditorMFCApp()
{
}

/////////////////////////////////////////////////////////////////////////////
// The one and only EditorMFCApp object

EditorMFCApp theApp;

/////////////////////////////////////////////////////////////////////////////
// EditorMFCApp initialization

BOOL EditorMFCApp::InitInstance()
{
	// Standard initialization

#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif

	// Change the registry key under which our settings are stored.
	SetRegistryKey(_T("Local AppWizard-Generated Applications"));

	MainFrame* pFrame = new MainFrame;
	m_pMainWnd = pFrame;


	// create and load the frame with its resources

	pFrame->LoadFrame(IDR_EDITOR_MENU/*, WS_OVERLAPPED |WS_CAPTION | WS_SYSMENU | FWS_ADDTOTITLE*/  );
	HICON editorIco = LoadIcon(IDI_ICON1);
	pFrame->SetIcon(editorIco,true);
	
	pFrame->ShowWindow(SW_SHOW);
	pFrame->UpdateWindow();
	
	InitGameOS(  m_hInstance,	EditorInterface::instance()->m_hWnd, m_lpCmdLine );

	pFrame->SetFocus();

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// EditorMFCApp message handlers





/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_FOG };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
		// No message handlers
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// App command to run the dialog
void EditorMFCApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

/////////////////////////////////////////////////////////////////////////////
// EditorMFCApp message handlers


BOOL EditorMFCApp::OnIdle(LONG lCount) 
{
	const CWnd *pW1 = this->GetMainWnd();
	const CWnd *pW2 = EditorInterface::instance();

	const CWnd *pW3 = CWnd::GetForegroundWindow();
	const CWnd *pW4 = CWnd::GetActiveWindow();
	const CWnd *pW5 = CWnd::GetFocus();

	if (pW1 == pW3)
	{
		EditorInterface::instance()->InvalidateRect(NULL, FALSE);
	}

	Sleep(2/*milliseconds*/); /* limits the framerate to 500fps */
	
	CWinApp::OnIdle(lCount);

	return 1;
}


int EditorMFCApp::ExitInstance() 
{
	{
		Environment.TerminateGameEngine();
		gos_PushCurrentHeap(0); // TerminateGameEngine() forgets to do this
	}
	if (false) {
		ExitGameOS();
		if (!EditorInterface::instance()->m_hWnd)
		{
			/* ExitGameOS() shuts down directX which has the side effect of killing the
			EditorInterface window, so we recreate the window here. The editor window
			may not be referenced after this function is executed, but this is not the correct
			place for the EditorInterface window to be destroyed. */
			EditorInterface::instance()->Create(NULL, NULL, AFX_WS_DEFAULT_VIEW | WS_VSCROLL | WS_HSCROLL,
				CRect(0, 0, 0, 0), m_pMainWnd, AFX_IDW_PANE_FIRST, NULL);
		}
	}

	delete m_pMainWnd;
	m_pMainWnd = 0;

	return CWinApp::ExitInstance();
}

